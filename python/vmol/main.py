import sys
import ctypes
from ctypes import c_int, c_double, c_char_p
import functools
import vmol


c_double_p = ctypes.POINTER(c_double)
c_int_p = ctypes.POINTER(c_int)
ARGS_T = [c_int, ctypes.POINTER(ctypes.c_char_p)]


class in_str_t(ctypes.Structure):
    _fields_ = [
            ("n", c_int),
            ("q", c_int_p),
            ("r", c_double_p),
            ("name", c_char_p),
            ]


def convert_in_mol(q, r, name):

    import numpy as np

    if q is None or r is None:
        raise ValueError("q and r must both be provided or both be None")
    if not isinstance(name, bytes):
        name = str(name).encode('utf-8')
    r = np.ascontiguousarray(r, dtype=c_double)
    q = np.ascontiguousarray(q, dtype=c_int)
    n = len(q)
    if q.ndim != 1:
        raise ValueError("q must be a 1D array")
    if r.shape != (n, 3):
        raise ValueError(f"r must be a 2D array with shape ({n}, 3)")
    return in_str_t(n=c_int(n),
                    q=q.ctypes.data_as(c_int_p),
                    r=r.flatten().ctypes.data_as(c_double_p),
                    name=name)


def convert_in(func):
    @functools.wraps(func)
    def myinner(argv, *args, **kwargs):
        argc = len(argv)
        argv = (c_char_p * argc)(*[arg.encode('utf-8') for arg in argv])
        return func(c_int(argc), argv, *args, **kwargs)
    return myinner


def convert_out(func):
    @functools.wraps(func)
    def myinner(argv, *args, **kwargs):
        ret = c_int(0)
        out = func(argv, ctypes.byref(ret), *args, **kwargs)
        out = out.decode('utf-8').rstrip('\n') if out else None
        return ret.value, out
    return myinner


def declare(fn, argtypes, restype):
    fn.argtypes = argtypes
    fn.restype = restype
    return fn


def run(argv):
    lib = ctypes.cdll.LoadLibrary(vmol.so)
    main = convert_in(declare(lib.main, argtypes=ARGS_T, restype=c_int))
    return main(argv)


def capture(*, q=None, r=None, name=None, args=None, return_code=False):
    args = [vmol.so, *args] if args else [vmol.so]
    lib = ctypes.cdll.LoadLibrary(vmol.so)
    free = declare(lib.free_out_str, argtypes=None, restype=None)

    if q or r:
        main = lib.main_wrap_in_out
        argtypes = [*ARGS_T, c_int_p, in_str_t]
        arguments = (args, convert_in_mol(q, r, name))
    else:
        main = lib.main_wrap_out
        argtypes = [*ARGS_T, c_int_p]
        arguments = (args,)

    main = convert_out(convert_in(declare(main, argtypes=argtypes, restype=c_char_p)))
    ret, out = main(*arguments)
    free()
    return (ret, out) if return_code else out
