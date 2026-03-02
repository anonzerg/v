import ctypes
import numpy as np
import vmol


class in_str_t(ctypes.Structure):
    _fields_ = [
            ("n", ctypes.c_int),
            ("q", ctypes.POINTER(ctypes.c_int)),
            ("r", ctypes.POINTER(ctypes.c_double)),
            ("name", ctypes.c_char_p),
            ]


def run(args):
    lib = ctypes.cdll.LoadLibrary(vmol.SO)
    main = lib.main

    main.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
    main.restype = ctypes.c_int

    argv = [vmol.SO, *args]
    argc = len(argv)
    argv = (ctypes.c_char_p * argc)(*[arg.encode('utf-8') for arg in argv])

    ret, output = vmol.stdout.capture(main, argc, argv)
    return ret, output


def run1(args):

    lib = ctypes.cdll.LoadLibrary(vmol.SO)
    main = lib.main_wrapper2

    main.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
    main.restype = ctypes.c_char_p

    argv = [vmol.SO, *args]
    argc = len(argv)
    argv = (ctypes.c_char_p * argc)(*[arg.encode('utf-8') for arg in argv])

    ret = main(argc, argv)
    ret = ret.decode('utf-8') if ret else ''

    free = lib.free_out_str
    free.argtypes = None
    free.restype = None
    free()

    return ret


def run2(q, r, name=None, args=None):

    lib = ctypes.cdll.LoadLibrary(vmol.SO)
    main = lib.main_wrapper3

    main.argtypes = [in_str_t, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
    main.restype = ctypes.c_char_p

    if args is None:
        args = []

    if not isinstance(name, bytes):
        name = str(name).encode('utf-8')
    r = np.ascontiguousarray(r, dtype=ctypes.c_double)
    q = np.ascontiguousarray(q, dtype=ctypes.c_int)
    n = len(q)

    if q.ndim != 1:
        raise ValueError("q must be a 1D array")
    if r.shape != (n, 3):
        raise ValueError(f"r must be a 2D array with shape ({n}, 3)")

    in_str = in_str_t(
            n=n,
            q=q.ctypes.data_as(ctypes.POINTER(ctypes.c_int)),
            r=r.flatten().ctypes.data_as(ctypes.POINTER(ctypes.c_double)),
            name=name,
            )

    argv = [vmol.SO, *args]
    argc = len(argv)
    argv = (ctypes.c_char_p * argc)(*[arg.encode('utf-8') for arg in argv])

    ret = main(in_str, argc, argv)
    ret = ret.decode('utf-8') if ret else ''

    free = lib.free_out_str
    free.argtypes = None
    free.restype = None
    free()

    return ret
