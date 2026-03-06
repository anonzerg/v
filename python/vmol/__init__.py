import os
from sysconfig import get_path, get_config_var
from vmol.main import Vmol

_v = 'v'
_suffix = get_config_var('EXT_SUFFIX')
_paths = [
        f'./{_v}.so',
        f'./{_v}{_suffix}',
        f'{__path__[0]}/{_v}.so',
        f'{__path__[0]}/{_v}{_suffix}',
        f"{get_path('purelib')}/{__package__}/{_v}{_suffix}",
        f"{get_path('platlib')}/{__package__}/{_v}{_suffix}",
        f"{get_path('purelib')}/{_v}{_suffix}",
        f"{get_path('platlib')}/{_v}{_suffix}",
        ]

_exists = [os.path.isfile(p) for p in _paths]

_so = _paths[_exists.index(True)] if sum(_exists) else None

vmol = Vmol(so=_so)

del main
del _v, _suffix
del get_config_var, get_path, os
