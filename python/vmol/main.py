"""Run the viewer with specified command-line arguments and/or molecule data and capture the output."""

import ctypes
from ctypes import c_int, c_double, c_char_p
import functools
import vmol


c_double_p = ctypes.POINTER(c_double)
c_int_p = ctypes.POINTER(c_int)
ARGS_T = [c_int, ctypes.POINTER(ctypes.c_char_p)]


class in_str_t(ctypes.Structure):
    """C structure for the input molecule data, containing the number of atoms, charge array, coordinate array, and name."""
    _fields_ = [
            ("n", c_int),
            ("q", c_int_p),
            ("r", c_double_p),
            ("name", c_char_p),
            ]


def convert_in_mol(mol):
    """Convert a molecule represented as a dictionary to the expected C structure for input.

    The input molecule should be a dictionary with the following keys
        - 'q': a 1D array-like of integers representing the charges of the atoms
        - 'r': a 2D array-like of floats with shape (n, 3) representing the coordinates of the atoms
        - 'name' (optional): a string representing the name of the molecule
    The function converts the 'q' and 'r' arrays to contiguous C arrays of the appropriate types,
    and the 'name' to a C string, and returns an instance of `in_str_t` with the corresponding fields set.

    Args:
        mol (dict): A dictionary representing the molecule, with keys 'q', 'r', and optionally 'name'.

    Returns:
        in_str_t: An instance of `in_str_t` with the fields set according to the input molecule.
    """
    import numpy as np

    if not isinstance(mol, dict):
        msg = f"mol must be a dictionary, but got {type(mol)}"
        raise ValueError(msg)
    q = mol.get('q')
    r = mol.get('r')
    name = mol.get('name')
    if q is None or r is None:
        msg = "mol must contain 'q' and 'r' for charge and coordinate arrays, respectively"
        raise ValueError(msg)

    if not isinstance(name, bytes):
        name = str(name).encode('utf-8')
    r = np.ascontiguousarray(r, dtype=c_double)
    q = np.ascontiguousarray(q, dtype=c_int)
    n = len(q)
    if q.ndim != 1:
        msg = f"q must be a 1D array, but has shape {q.shape}"
        raise ValueError(msg)
    if r.shape != (n, 3):
        msg = f"r must be a 2D array with shape ({n}, 3), but has shape {r.shape}"
        raise ValueError(msg)
    return in_str_t(n=c_int(n),
                    q=q.ctypes.data_as(c_int_p),
                    r=r.flatten().ctypes.data_as(c_double_p),
                    name=name)


def _check_attrs(name, x, attrs):
    for attr in attrs:
        if not hasattr(x, attr):
            msg = f"{name} must have {attr} defined"
            raise ValueError(msg)


def convert_in(func):
    """Decorate a function to convert a list of Python strings to the expected C types.

    The function is expected to take an integer and a pointer to an array of C strings as its first two arguments,
    which represent the argument count and argument values, respectively, i.e.,
    ```
    void func(int argc, char ** argv, ...)
    ```
    The decorator will convert a list of Python strings passed as the first argument
    to the wrapped function into the appropriate C types before calling the original function, i.e.,
    ```
    def wrapped_func(argv: list[str], ...)
    ```
    The function has to have the `argtypes` and `restype` attributes defined and match the above signature.

    Args:
        func (callable): The function to wrap, which should take an integer and a pointer to an array of C strings
                         as its first two arguments.

    Returns:
        callable: A wrapped function that takes a list of Python strings as its first argument and converts
                  it to the expected C types before calling the original function.
    """
    _check_attrs("function", func, ['restype', 'argtypes'])

    if func.argtypes is None or len(func.argtypes) < 2 or func.argtypes[:2] != ARGS_T:
        msg = f"function must have a signature that starts with {ARGS_T} for the first arguments to convert the input list of strings"
        raise ValueError(msg)

    @functools.wraps(func)
    def myinner(argv, *args):
        argc = len(argv)
        argv = (c_char_p * argc)(*[arg.encode('utf-8') for arg in argv])
        return func(c_int(argc), argv, *args)
    myinner = declare(myinner, argtypes=[list, *func.argtypes[2:]], restype=func.restype)
    return myinner


def convert_out(func):
    """Decorate a function to convert its output to a Python string and return it along with the return code.

    The function is expected to return a pointer to a null-terminated C string, which will be decoded as UTF-8
    and stripped of trailing newlines.
    It is also expected to take a pointer to an integer as the third or second argument, where it will write the return code.

    We assume that in the former case the function is a _FuncPtr loaded from the shared library
    and has a signature
    ```
    char * func(int argc, char ** argv, int * ret_code, ...)
    ```
    and in the latter case it is already decorated with `convert_in()` and has a 'signature'
    ```
    char * func(argv: list[str], int * ret_code, ...)
    ```
    The function has to have the `argtypes` and `restype` attributes defined and match one of the above signatures.

    In the future, we could also support the case where the return code argument index is passed
    as an argument to the decorator.

    Args:
        func (callable): The function to wrap, which has the `argtypes` attribute defined
                         and should return a pointer to a null-terminated string
                         and take a pointer to an integer for the return code.

    Returns:
        callable: A wrapped function that returns a tuple of (return code (int), output (str))
                  and takes the same arguments as the original function,
                  except that the argument for the return code is handled internally.
    """
    _check_attrs("function", func, ['restype', 'argtypes'])

    argtypes2 = [list, c_int_p]
    argtypes3 = [*ARGS_T, c_int_p]
    for argtypes in (argtypes3, argtypes2):
        if func.argtypes is None or len(func.argtypes) < len(argtypes):
            continue
        if func.argtypes[:len(argtypes)] == argtypes:
            n = len(argtypes)-1
            break
    else:
        msg = f"function has to have a signature that matches\n{12*' '}either {argtypes3}\n{12*' '}or {argtypes2}\n{12*' '}for the first arguments to retrieve the return code via pointer"
        raise ValueError(msg)

    if func.restype != c_char_p:
        msg = f"function must return a pointer to a C string (restype={c_char_p})"
        raise ValueError(msg)

    @functools.wraps(func)
    def myinner(*args):
        ret = c_int(0)
        out = func(*(args[:n]), ctypes.byref(ret), *(args[n:]))
        out = out.decode('utf-8').rstrip('\n') if out else None
        return ret.value, out

    myinner = declare(myinner, argtypes=func.argtypes[:n]+func.argtypes[n+1:], restype=[int, str])
    return myinner


def declare(func, argtypes, restype):
    """Declare a function from the shared library with the given argument and return types.

    Args:
        func (callable): The function to declare, typically obtained from the shared library.
        argtypes (list of ctypes types): The argument types of the function.
        restype (ctypes type): The return type of the function.

    Returns:
        callable: The function with the specified argument and return types.
    """
    func.argtypes = argtypes
    func.restype = restype
    return func


def run(argv):
    """Run the viewer with the given command-line arguments.

    Args:
        argv (list of str): The command-line arguments to pass to the main function.
                            Unlike `capture()`, the first argument is a string that represents the program name,
                            such as `sys.argv[0]` or the name of the shared library (`vmol.so`),

    Returns:
        int: The return code from the main function.
    """
    lib = ctypes.cdll.LoadLibrary(vmol.so)
    main = convert_in(declare(lib.main, argtypes=ARGS_T, restype=c_int))
    return main(argv)


def capture(*, mol=None, args=None, return_code=False):
    """Run the viewer with the given structure and/or command-line arguments and capture the output.

    If `args` is provided, it will be passed as command-line arguments to the main function.
    It can contain any arguments that the main function accepts, except for the program name,
    includiing the paths to the molecule files to read.

    If `mol` is provided, it will be converted to the appropriate input structure and passed to the main function,
    and the paths to the molecule files in `args` will be ignored.
    The `mol` dictionary must contain 'q' and 'r' keys for the charge and coordinate arrays, respectively,
    and can optionally contain a 'name' key for the molecule name.

    Args:
        mol (dict, optional): A dictionary representing the molecule.
            Must contain 'q' (int 1D array-like) and 'r' (float 2D array-like of shape (n, 3)
            for charge and coordinate arrays, respectively.
            Optionally can contain a 'name' (str-like) for the molecule name.
        args (list of str, optional): Command-line arguments to pass to the main function (without the program name).
        return_code (bool, optional): Whether to return the return code along with the output. Defaults to False.

    Returns:
        If `return_code` is False:
            str: The output from the main function, decoded as UTF-8 and stripped of trailing newlines.
        If `return_code` is True:
            tuple of (int, str): the function return code and the output string.
    """
    args = [vmol.so, *args] if args else [vmol.so]
    lib = ctypes.cdll.LoadLibrary(vmol.so)
    free = declare(lib.free_out_str, argtypes=None, restype=None)

    if mol:
        main = lib.main_wrap_in_out
        argtypes = [*ARGS_T, c_int_p, in_str_t]
        arguments = (args, convert_in_mol(mol))
    else:
        main = lib.main_wrap_out
        argtypes = [*ARGS_T, c_int_p]
        arguments = (args,)

    main = convert_out(convert_in(declare(main, argtypes=argtypes, restype=c_char_p)))
    ret, out = main(*arguments)
    free()
    return (ret, out) if return_code else out
