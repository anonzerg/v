#!/usr/bin/env python3

import cclib
from vmol import vmol

path = '../mol/CEHZOF_1_SPE.out'
parser = cclib.io.ccopen(path)

data = parser.parse()
mols = [{'q': data.atomnos, 'r': r, 'name': str(parser)} for r in data.atomcoords]
out = vmol.capture(mols=mols)
print('captured output:')
print(out)
