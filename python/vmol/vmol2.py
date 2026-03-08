#!/usr/bin/env python
"""Run the vmol viewer with molecular data parsed by cclib."""

import sys
import os
import warnings
from vmol import vmol, _paths

try:
    import cclib
except ModuleNotFoundError:
    msg = "cclib not found. Install cclib to use this script."
    raise ModuleNotFoundError(msg) from None


def read_mols_from_cclib(path):
    """Read molecular data from a file using cclib.

    Arguments:
        path (str): Path to the file to read.

    Returns:
        list[dict]: Dictionaries containing atomic numbers ('q'), coordinates ('r'),
                    and a name for each molecule, i.e. suitable argument for vmol.run().

    Raises:
        FileNotFoundError: If the specified file is not found.
        RuntimeError: If cclib cannot parse the file or if no coordinates are found in the parsed data.
    """
    if not os.path.isfile(path):
        msg = f"File {path} not found."
        raise FileNotFoundError(msg)

    parser = cclib.io.ccopen(path)
    if parser is None:
        msg = f"Could not open {path} with cclib."
        raise RuntimeError(msg)

    try:
        data = parser.parse()
    except Exception:  # noqa: BLE001
        msg = f"Error parsing {path} with cclib. Using partial data."
        warnings.warn(msg, RuntimeWarning, stacklevel=2)
        data = parser

    if not (hasattr(data, "atomcoords") and hasattr(data, "atomnos") and len(data.atomcoords)):
        msg = f"No coordinates found in {path}."
        raise RuntimeError(msg)

    return [{'q': data.atomnos, 'r': r, 'name': str(parser)} for r in data.atomcoords]


def main():
    """Run the viewer with files parsed by cclib.

    Usage:
        vmol2.py file [options]

    Returns:
        int: Exit code (0 for success).

    Raises:
        ImportError: If vmol shared library is not found.
        RuntimeError: If no valid molecular data is found in any provided files.
    """
    if vmol is None:
        paths = '\n'.join(_paths)
        msg = f"vmol shared library not found in paths. Check your installation.\nSearched paths:\n{paths}"
        raise ImportError(msg)

    if len(sys.argv)==1:
        vmol.run(args=sys.argv, with_arg0=True)
        return 0

    to_pop = []
    mols = []
    for i, arg in enumerate(sys.argv[1:], start=1):
        if ':' in arg:
            continue
        to_pop.append(i)
        try:
            mols.extend(read_mols_from_cclib(arg))
        except (FileNotFoundError, RuntimeError) as e:
            warnings.warn(str(e), RuntimeWarning, stacklevel=2)
    if not mols:
        msg = "No valid molecular data found in any provided files."
        raise RuntimeError(msg)

    for i in reversed(to_pop):
        sys.argv.pop(i)
    return vmol.run(mols=mols, args=sys.argv, with_arg0=True)


if __name__ == "__main__":
    main()
