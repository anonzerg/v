import ctypes
import vmol

def run(args):
    lib = ctypes.cdll.LoadLibrary(vmol.SO)
    main = lib.main

    main.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
    main.restype = ctypes.c_int

    argv = [vmol.SO] + args
    argc = len(argv)
    argv = (ctypes.c_char_p * argc)(*[arg.encode('utf-8') for arg in argv])

    ret, output = vmol.stdout.capture(main, argc, argv)
    return ret, output


def run1(args):

    lib = ctypes.cdll.LoadLibrary(vmol.SO)
    main = lib.main_wrapper2

    main.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
    main.restype = ctypes.c_char_p

    argv = [vmol.SO] + args
    argc = len(argv)
    argv = (ctypes.c_char_p * argc)(*[arg.encode('utf-8') for arg in argv])

    ret = main(argc, argv)
    ret = ret.decode('utf-8') if ret else ''

    free = lib.free_out_str
    free.argtypes = None
    free.restype = None
    free()

    return ret
