#!/usr/bin/env python3

import ase.io
from vmol import vmol


mols = ase.io.read('../mol/mol0002.xyz', index=':')

rot = 'rot:0.83,0.40,-0.39,0.55,-0.67,0.49,-0.06,-0.62,-0.78'

out = vmol.capture(mols=mols, args=[rot])
print('captured output:')
print(out)
