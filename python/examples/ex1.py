#!/usr/bin/env python3

import vmol

# vmol.SO = 'v.cpython-313-x86_64-linux-gnu.so'  # set if want to specify the .so path
args = ['../mol/MOL_3525.xyz']
r, o = vmol.main.run(args)

print("Return value:", r)
print("Captured output:")
print(o)
