#!/usr/bin/env python3

from os.path import dirname, normpath
import vmol

mol_dir = normpath(f'{dirname(__file__)}/../../mol/')
args = [
        f'{mol_dir}/MOL_3525.xyz',
        'cell:8.929542,0.0,0.0,4.197206,8.892922,0.0,0.480945,2.324788,10.016044',
        ]

# vmol.so = 'v.cpython-313-x86_64-linux-gnu.so'  # set if want to specify the .so path

r, o = vmol.main.capture(args=args, return_code=True)

print("Return value:", r)
print("Captured output:")
print(o)
