import ctypes
import stdout

def run(path, args):
    lib = ctypes.cdll.LoadLibrary(path)
    main = lib.main

    main.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
    main.restype = ctypes.c_int

    argv = [path] + args
    argc = len(argv)
    argv = (ctypes.c_char_p * argc)(*[arg.encode('utf-8') for arg in argv])

    ret, output = stdout.capture(main, argc, argv)
    return ret, output
