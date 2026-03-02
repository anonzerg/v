# vmol: v -- python binding

This package allows to
* run `v` from python scripts and receive its output without calling subprocesses and
* facilitate installation (especially if it is a dependency).

Inspired by @aligfellow's [xyzrender](https://github.com/aligfellow/xyzrender).

## Installation

### requirements 
* `libX11-devel libXpm-devel xproto-devel` (`libx11-dev libxpm-dev x11proto-dev` on Ubuntu) for C compilation
* `setuptools` for Python

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
>>> vmol.SO
'/home/xe/soft/miniconda3/lib/python3.13/site-packages/vmol/v.cpython-313-x86_64-linux-gnu.so'
```
To subsitute the `.so`, put in in the same directory or change manually:
```
>>> vmol.SO='./v.so'
```


## Usage example

```
>>> # import the library
>>> import vmol

>>> # it provides the main function wrapper `main`,
>>> # path to the shared library `SO`,
>>> # and a stdout parsing module `stdout`
>>> dir(vmol)
['SO', '__builtins__', '__cached__', '__doc__', '__file__', '__loader__', '__name__',
'__package__', '__path__', '__spec__', '_exists', '_paths', 'main', 'stdout']

>>> # the arguments are the same as the CLI ones:
>>> args = ['v/mol/MOL_3525.xyz']
>>> # they should be an array of strings: 
>>> args = ['v/mol/MOL_3525.xyz', 'bonds:0']

>>> # look at the molecule:
>>> r, o = vmol.main.run(args)

>>> # check the output:
>>> print(o)
```
