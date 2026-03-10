from setuptools import setup, Extension
import subprocess
import os
import tempfile
import shutil
from pathlib import Path


VERSION="3.0rc5"

def run_git(args):
    try:
        r = subprocess.run( ["git", *args], check=True, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)
        return r.stdout.strip() or None
    except Exception:
        return None


def get_git_version_hash():
    """Get tag/hash of the latest commit.

    Thanks to https://gist.github.com/nloadholtes/07a1716a89b53119021592a1d2b56db8
    """
    version = os.getenv("VMOL_GIT_DESCRIBE") or run_git(["describe", "--tags", "--dirty"]) or f"{VERSION}+unknown"
    return version.replace('+', '-').replace('-', '+', 1)


def rel_posix(path):
    return os.path.relpath(path, start=setup_dir).replace(os.sep, "/")


setup_dir = Path(__file__).parent
src_dir = setup_dir.parent / "src"

c_files = [rel_posix(p) for p in src_dir.rglob("*.c")]
include_dirs = sorted({ rel_posix(h.parent) for h in src_dir.rglob("*.h") })
if not c_files:
    raise RuntimeError(f"No C sources found under {src_dir}")

GIT_HASH   = os.getenv("VMOL_GIT_HASH")   or run_git(["rev-parse", "HEAD"])                 or "unknown"
GIT_BRANCH = os.getenv("VMOL_GIT_BRANCH") or run_git(["rev-parse", "--abbrev-ref", "HEAD"]) or "unknown"

VERSION_FLAGS = [f'-DGIT_HASH="{GIT_HASH}"',
                 f'-DGIT_BRANCH="{GIT_BRANCH}"',
                 f'-DBUILD_USER="{os.getenv("USER")}@{os.getenv("HOSTNAME")}"',
                 f'-DBUILD_DIRECTORY="{os.getcwd()}"']

setup(
    version=get_git_version_hash(),
    include_package_data=True,
    ext_modules=[Extension('vmol.v',
                           sources=c_files,
                           include_dirs=include_dirs,

                           libraries = ['X11', 'Xpm'],
                           extra_compile_args=['-std=gnu11', '-O2', ] + VERSION_FLAGS,
                           extra_link_args=[]),
                ],
)
