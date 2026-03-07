#!/usr/bin/env python3

from vmol import vmol

# mol/mol0001.xyz
name = 'acetylene'
q = [6, 6, 1, 1]
r = [
        [0.99495,  0.00000, 0.00000],
        [2.19521,  0.00000, 0.00000],
        [-0.07071, 0.00000, 0.00000],
        [3.26087,  0.00000, 0.00000],
        ]

input('press enter to view a molecule...')
# rotation matrix
rot = 'rot:0.587785,0.000000,0.809017,0.670705,0.559193,-0.487296,-0.452397,0.829038,0.328685'
ret, xyz = vmol.capture(mols={'q': q, 'r': r, 'name': name}, args=[rot, 'exitcom:z'], return_code=True)
print('return code:', ret)
print('captured output:')
print(xyz)
print()

input('press enter to get its point group in headless mode...')
point_group = vmol.capture(mols={'q': q, 'r': r}, args=['gui:0', 'com:.'])
print(f'{point_group=}')
print()

input('press enter to view two molecules...')
mol1 = {'q': q, 'r': r, 'name': name}

water_r = [[ 0.7493682,    0.0000000,    0.4424329],
           [ 0.0000000,    0.0000000,   -0.1653507],
           [-0.7493682,    0.0000000,    0.4424329]]
mol2 = {'q': ['H', 8, '1'], 'r': water_r, 'name': 'water'}

out = vmol.capture(mols=[mol2, mol1], args=[rot])
print('captured output:')
print(out)
