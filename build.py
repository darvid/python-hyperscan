import os
import subprocess
import sys

from distutils.core import Extension
from distutils.errors import (
    CCompilerError,
    DistutilsExecError,
    DistutilsPlatformError,
)
from distutils.command.build_ext import build_ext


# http://code.activestate.com/recipes/502261-python-distutils-pkg-config/
def pkgconfig(libs, optional=''):
    flag_map = {
        'include_dirs': (['--cflags-only-I'], 2),
        'library_dirs': (['--libs-only-L'], 2),
        'libraries': (['--libs-only-l'], 2),
        'extra_compile_args': (['--cflags-only-other'], 0),
        'extra_link_args': (['--libs-only-other'], 0),
    }
    ext_kwargs = {
        'extra_compile_args': ['-std=c99'],
    }
    for lib in libs:
        for distutils_kwarg, (pkg_options, trim_offset) in flag_map.items():
            try:
                options = (
                    subprocess.check_output(
                        ['pkg-config', optional, *pkg_options, lib]
                    )
                    .decode()
                    .split()
                )
            except subprocess.CalledProcessError:
                continue
            ext_kwargs.setdefault(distutils_kwarg, []).extend(
                [opt[trim_offset:] for opt in options]
            )
    return ext_kwargs


def build(setup_kwargs):
    setup_kwargs.update(
        {
            'ext_modules': [
                Extension(
                    'hyperscan._hyperscan',
                    ['hyperscan/hyperscanmodule.c'],
                    **pkgconfig(['libhs'])
                )
            ],
            'cmdclass': {'build_ext': build_ext},
        }
    )
