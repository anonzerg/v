"""Run the viewer with specified command-line arguments and/or molecule data and capture the output."""

import ctypes
from ctypes import c_int, c_double, c_char_p
import functools
import vmol

__all__ = ["capture", "run"]

c_double_p = ctypes.POINTER(c_double)
c_int_p = ctypes.POINTER(c_int)
ARGS_T = (c_int, ctypes.POINTER(ctypes.c_char_p))


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
        - 'q': a 1D array-like of integers or strings representing the atomic numbers or element symbols
        - 'r': a 2D array-like of floats with shape (n, 3) representing the atomic coordinates
        - 'name' (optional): a string representing the name of the molecule
    The function converts the 'q' and 'r' arrays to contiguous C arrays of the appropriate types,
    and the 'name' to a C string, and returns an instance of `in_str_t` with the corresponding fields set.

    Args:
        mol (dict): A dictionary representing the molecule, with keys 'q', 'r', and optionally 'name'.

    Returns:
        in_str_t: An instance of `in_str_t` with the fields set according to the input molecule.

    Raises:
        TypeError: If mol is not a dictionary.
        ValueError: If the required keys are missing or their values have wrong shapes.
    """
    import numpy as np

    if not isinstance(mol, dict):
        msg = f"mol must be a dictionary, but got {type(mol)}"
        raise TypeError(msg)
    q = mol.get('q')
    r = mol.get('r')
    name = mol.get('name')
    if q is None or r is None:
        msg = "mol must contain 'q' and 'r' for charge and coordinate arrays, respectively"
        raise ValueError(msg)

    if not isinstance(name, bytes):
        name = str(name).encode('utf-8')

    q_ = np.asarray(q)
    n = len(q_)
    if q_.ndim != 1:
        msg = f"q must be a 1D array, but has shape {q_.shape}"
        raise ValueError(msg)

    r = np.ascontiguousarray(r, dtype=c_double)
    if r.shape != (n, 3):
        msg = f"r must be a 2D array with shape ({n}, 3), but has shape {r.shape}"
        raise ValueError(msg)

    try:
        q = np.ascontiguousarray(q, dtype=c_int)
    except ValueError as e:
        lib = ctypes.cdll.LoadLibrary(vmol.so)
        get_element = declare(lib.get_element, argtypes=[c_char_p], restype=c_int)
        q = q.copy()
        for i, qi in enumerate(q):
            if isinstance(qi, str):
                q[i] = get_element(qi.encode('utf-8'))
            elif isinstance(qi, bytes):
                q[i] = get_element(qi)
        q = np.ascontiguousarray(q, dtype=c_int)

    return in_str_t(n=c_int(n),
                    q=q.ctypes.data_as(c_int_p),
                    r=r.flatten().ctypes.data_as(c_double_p),
                    name=name)


def convert_in(func):
    """Decorate a function to convert Python arguments to the expected C types.

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

    If `func.ret_code_ptr_idx` is defined and equals to N>=2, the decorator also passes a pointer to an integer
    as the Nth argument:
    ```
    void func(int argc, char ** argv, ..., int * ret, ...) -> def wrapped_func(argv: list[str], ..., ...)
              ^0th                         ^Nth
    ```
    The `func.errcheck` function is supposed to take care of the return value.

    Args:
        func (callable): The function to wrap.

    Returns:
        callable: A wrapped function.

    Raises:
        TypeError: If func is not a ctypes function.
        ValueError: If its attributes are wrong.
    """
    if not isinstance(func, ctypes._CFuncPtr):
        msg = "function should be a ctypes function"
        raise TypeError(msg)

    if func.argtypes is None or len(func.argtypes) < len(ARGS_T) or any(x!=y for x, y in zip(func.argtypes, ARGS_T, strict=False)):
        msg = f"function must have a signature that starts with {ARGS_T} for the first arguments to convert the input list of strings"
        raise ValueError(msg)

    if not hasattr(func, 'ret_code_ptr_idx'):
        func.ret_code_ptr_idx = None
    if func.ret_code_ptr_idx is not None:
        if func.ret_code_ptr_idx < len(ARGS_T):
            msg = f"return code argument index must be at least 2, but got {func.ret_code_ptr_idx}"
            raise ValueError(msg)
        if func.argtypes[func.ret_code_ptr_idx] != c_int_p:
            msg = f"return code argument must be a pointer to an integer ({c_int_p}), but got {func.argtypes[func.ret_code_ptr_idx]}"
            raise ValueError(msg)

    @functools.wraps(func)
    def myinner(argv, *args):
        argc = len(argv)
        argv = (c_char_p * argc)(*[arg.encode('utf-8') for arg in argv])
        arguments = [c_int(argc), argv, *args]
        if func.ret_code_ptr_idx is not None:
            ret = c_int(0)
            arguments.insert(func.ret_code_ptr_idx, ctypes.byref(ret))
        return func(*arguments)
    return myinner


def declare(func, argtypes, restype, errcheck=None, ret_code_ptr_idx=None):
    """Declare a function from the shared library with the given argument and return types.

    Args:
        func (callable): The function to declare, typically obtained from the shared library.
        argtypes (list of ctypes types): The argument types of the function.
        restype (ctypes type): The return type of the function.
        errcheck (callable, optional): A function that takes the result, the function, and the arguments,
                                       and returns the processed result.
        ret_code_ptr_idx (int, optional): Position of the `int *` argument to store the return value in.

    Returns:
        callable: The function with the specified arguments and return types.
    """
    func.argtypes = argtypes
    func.restype = restype
    if errcheck:
        func.errcheck = errcheck
    if ret_code_ptr_idx is not None:
        func.ret_code_ptr_idx = ret_code_ptr_idx
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
        arguments = [args, convert_in_mol(mol)]
    else:
        main = lib.main_wrap_out
        argtypes = [*ARGS_T, c_int_p]
        arguments = [args]

    def errcheck(result, func, args):
        ret = args[func.ret_code_ptr_idx]._obj.value
        out = result.decode('utf-8').rstrip('\n') if result else None
        free()
        return ret, out

    main = convert_in(declare(main, argtypes=argtypes, restype=c_char_p, errcheck=errcheck, ret_code_ptr_idx=2))
    ret, out = main(*arguments)
    return (ret, out) if return_code else out
