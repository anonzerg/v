#!/usr/bin/env python3

import vmol
import numpy as np

# mol/mol0001.xyz
name = 'acetylene'
q = [6, 6, 1, 1]
r = [
        [0.99495,  0.00000, 0.00000],
        [2.19521,  0.00000, 0.00000],
        [-0.07071, 0.00000, 0.00000],
        [3.26087,  0.00000, 0.00000]
        ]

point_group = vmol.main.run2(q, r, name=name, args=['gui:0', 'com:.'])
print(point_group)

# rotation matrix
rot = 'rot:0.587785,0.000000,0.809017,0.670705,0.559193,-0.487296,-0.452397,0.829038,0.328685'
xyz = vmol.main.run2(q, r, name=name, args=[rot])
print('captured output:')
print(xyz)
