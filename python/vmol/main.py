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
    if not isinstance(name, bytes):
        name = str(name).encode('utf-8')
    r = np.ascontiguousarray(r, dtype=c_double)
    q = np.ascontiguousarray(q, dtype=c_int)
    n = len(q)
    if q.ndim != 1:
        raise ValueError("q must be a 1D array")
    if r.shape != (n, 3):
        raise ValueError(f"r must be a 2D array with shape ({n}, 3)")
    return in_str_t(n=n,
                    q=q.ctypes.data_as(c_int_p),
                    r=r.flatten().ctypes.data_as(c_double_p),
                    name=name)


def convert_in(func):
    @functools.wraps(func)
    def myinner(argv, /, *args, **kwargs):
        argc = len(argv)
        argv = (c_char_p * argc)(*[arg.encode('utf-8') for arg in argv])
        return func(argc, argv, *args, **kwargs)
    return myinner


def convert_out(func):
    @functools.wraps(func)
    def myinner(*args, **kwargs):
        out = func(*args, **kwargs)
        return out.decode('utf-8').rstrip('\n') if out else None
    return myinner


def declare(fn, argtypes, restype):
    fn.argtypes = argtypes
    fn.restype = restype
    return fn


def run0():
    lib = ctypes.cdll.LoadLibrary(vmol.SO)
    main = convert_in(declare(lib.main, argtypes=ARGS_T, restype=c_int))
    return main(sys.argv)


def run(args):
    lib = ctypes.cdll.LoadLibrary(vmol.SO)
    main = convert_in(declare(lib.main, argtypes=ARGS_T, restype=c_int))
    ret, output = vmol.stdout.capture(main, [vmol.SO, *args])
    return ret, output


def run1(args):
    lib = ctypes.cdll.LoadLibrary(vmol.SO)
    main = convert_out(convert_in(declare(lib.main_wrapper2, argtypes=ARGS_T, restype=c_char_p)))
    free = declare(lib.free_out_str, argtypes=None, restype=None)
    ret = main([vmol.SO, *args])
    free()
    return ret


def run2(q, r, name=None, args=None):
    lib = ctypes.cdll.LoadLibrary(vmol.SO)
    main = convert_out(convert_in(declare(lib.main_wrapper3, argtypes=[*ARGS_T, in_str_t], restype=c_char_p)))
    free = declare(lib.free_out_str, argtypes=None, restype=None)
    ret = main([vmol.SO, *args], convert_in_mol(q, r, name))
    free()
    return ret
