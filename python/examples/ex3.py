#!/usr/bin/env python3

from vmol import vmol

CO2 = {'q': [8, 6, 8], 'r': [(x,0,0) for x in [-1.16, 0, 1.16]]}
sym_stretch  = {'freq': 1480, 'ints': 0,   'mass': 16, 'disp': [(dx,0,0) for dx in [-1, 0, 1]]}
asym_stretch = {'freq': 2565, 'ints': 750, 'mass': 13, 'disp': [(dx,0,0) for dx in [-0.37, 0.85, -0.37]]}
vib = {key: ((sym_stretch[key], asym_stretch[key])) for key in sym_stretch}
CO2['name'] = 'press Insert to see the animation'
vmol.run(mols=CO2, vib=vib)
