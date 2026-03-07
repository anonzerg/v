# vmol: v -- python binding

This package allows to
* run `v` from python scripts and receive its output without calling subprocesses and
* facilitate installation (especially if it is a dependency).

Inspired by @aligfellow's [xyzrender](https://github.com/aligfellow/xyzrender).

## Requirements

* X11
* numpy

### For installation:
* `libX11-devel libXpm-devel xproto-devel` (`libx11-dev libxpm-dev x11proto-dev` on Ubuntu) for C compilation
* `setuptools` for Python

## Installation

```
VV="3.0rc3" # v version
PY="313"    # python version for wheels, also available "311" and "312"
```

### Option 1 -- install wheels

```
pip install "https://github.com/briling/v/releases/download/v3.0rc3/vmol-${VV}-cp${PY}-cp${PY}-linux_x86_64.whl"
```
alternatively
```
wget "https://github.com/briling/v/releases/download/v3.0rc3/vmol-${VV}-cp${PY}-cp${PY}-linux_x86_64.whl"
pip install "vmol-${VV}-cp${PY}-cp${PY}-linux_x86_64.whl"
```

### Option 2 -- build and install from github
```
pip install git+https://github.com/briling/v@v${VV}#subdirectory=python
```

### Option 3 -- build and install from github but with more control

#### download
```
git clone git@github.com:briling/v.git && cd v && git checkout v${VV}
# OR
wget https://github.com/briling/v/archive/v${VV}.tar.gz && tar -xvf v${VV}.tar.gz && cd v-${VV}
```
#### build and install
```
cd python
pip install .
# OR
pip install .
# OR build a wheel
make
```

# Option 4 -- use the `.so` (probably don't)

This can be tried in case of some compilation issues.
You'll have to manually add the python files to the path.
```
# download
wget https://github.com/briling/v/releases/download/v${VV}/v.so && chmod +x v.so
# OR build in the repo root (see Option 3/download)
make v.so
```
The package searches for the `v.so` file in its parent directory
and standard paths.  For example,
```
>>> import vmol
>>> vmol.so
'/home/xe/soft/miniconda3/lib/python3.13/site-packages/vmol/v.cpython-313-x86_64-linux-gnu.so'
```
To subsitute the `.so`, put in in the same directory or change manually:
```
>>> vmol.so='./v.so'
```


## Usage

```python
>>> from vmol import vmol
>>> # `vmol` provides the two wrapper functions `capture` and `run`
>>> # along with the path to the shared library `so`,
>>> # loaded library `lib`, and function namespace `f`.
>>> [*filter(lambda x: not str.startswith(x, '_'), dir(vmol))]
['capture', 'f', 'lib', 'run', 'so']
>>> vmol.so
/home/xe/Documents/git/v/python/vmol/v.cpython-313-x86_64-linux-gnu.so
>>> # one can use a custom .so file:
>>> vmol.so = '../v.so'
```

### 1. Simple wrapper

The package provides a script which can be used exactly as `v` (see [reference](../README.md)).
The following command are equivalent:
```bash
vmol ...
python -m vmol ...
vmol/__main__.py ...
```
For example,
```bash
python -m vmol ../mol/MOL_3525.xyz cell:8.93,0.0,0.0,4.2,8.9,0.0,0.48,2.32,10
```

It can also be run from a script, i.e.
```python
from vmol import vmol
vmol.run(['../mol/MOL_3525.xyz', 'cell:8.93,0.0,0.0,4.2,8.9,0.0,0.48,2.32,10'])
```
The arguments are the same as the CLI ones and should be an array of strings.

### 2. Capture the output
See [example 1](examples/ex1.py).

```python
from vmol import vmol
out = vmol.capture(args=['../mol/MOL_3525.xyz', 'cell:8.93,0.0,0.0,4.2,8.9,0.0,0.48,2.32,10'])
# look at the molecule, press `x`/`z`/`p` to produce an output, close with `q`/`esc`
print(out)
```
The arguments `args` are the same as the CLI ones and should be an array of strings.

The return code can be captured as well:
```python
ret, out = vmol.capture(args=['../mol/MOL_3525.xyz', 'cell:8.93,0.0,0.0,4.2,8.9,0.0,0.48,2.32,10'], return_code=True)
```

Headless mode also works:
```python
>>> vmol.capture(args=['../mol/S8.qm.out', 'gui:0', 'com:.'])
'D8h'
>>> vmol.capture(args=['../mol/S8.qm.out', 'gui:0', 'com:.', 'frame:-1'])
'D4d'
```

Tell the viewer to automatically print the coordinates before exit:
```python
>>> out = vmol.capture(args=['../mol/rotation1.in', 'exitcom:z'])
>>> print(out)
```

### 3. Pass a structure

One can pass a structure (or several structures) as an argument.
See [example 2](examples/ex2.py).

```python
from vmol import vmol
name = 'HF molecule'
q = [1, 'F']
r = [[0,0,0],[0.9,0,0]]
out = vmol.capture(mols={'q': q, 'r': r, 'name': name}, args=['shell:0.6,0.7'])
# look at the molecule, press `x`/`z`/`p` to produce an output, close with `q`/`esc`
print(out)
```

ASE Atoms (or anything with `.numbers` and `.positions`) are also [supported](examples/ex_ase.py):
```python
import ase.io
mols = ase.io.read('../mol/mol0002.xyz', index=':')
out = vmol.capture(mols=mols)
```

Without capturing the output:
```python
from vmol import vmol
vmol.run(args=['shell:0.6,0.7'], mols={'q': [1, 'F'], 'r': [[0,0,0],[0.9,0,0]], 'name': 'hydrogen fluoride'})
```

Formats which are not supported natively can be read with `cclib` as passed (see [example](examples/ex_cclib)).
