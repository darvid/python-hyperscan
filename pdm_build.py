import glob
import os
import os.path
import subprocess
import sys
from distutils.sysconfig import get_python_inc

from setuptools import Extension


def pdm_build_hook_enabled(context):
    return context.target == "wheel"


def pdm_build_update_setup_kwargs(context, setup_kwargs):
    setup_kwargs.update(ext_modules=_get_extension())


def _get_extension(name="hyperscan._ext", **kwargs):
    return Extension(
        name,
        sources=["src/hyperscan/hyperscanmodule.c"],
        library_dirs=os.getenv("LIBRARY_PATH", "").split(":"),
        extra_compile_args=["-fPIC", "-O0", "-DPCRE_STATIC"],
        **get_platform_specific_options(),
        **kwargs,
    )


def _pkgconfig(args):
    return subprocess.getoutput(f"pkg-config {args}").strip()


def _pkgconfig_get_cflags(lib):
    return _pkgconfig(f"--cflags {lib}")


def _pkgconfig_get_libdir(lib):
    return _pkgconfig(f"--variable=libdir {lib}")


def get_platform_specific_options():
    if sys.platform == "win32":
        raise RuntimeError("win32 not currently supported")

    hs_cflags = _pkgconfig_get_cflags("libhs")
    pcre_libdir = _pkgconfig_get_libdir("libpcre")
    hs_libdir = _pkgconfig_get_libdir("libhs")
    pcre_static_libs = [
        os.path.join(pcre_libdir, "libpcre.a"),
        *glob.glob(os.path.join(pcre_libdir, '*.o')),
    ]
    ext_kwargs = {
        "extra_objects": pcre_static_libs,
        "include_dirs": [hs_cflags[2:], get_python_inc(plat_specific=1)],
        "libraries": ["m", "stdc++"],
    }

    hs_static_libs = [
        os.path.join(hs_libdir, lib) for lib in ["libhs.a", "libchimera.a"]
    ]

    ext_kwargs["extra_objects"] = [
        *hs_static_libs,
        *ext_kwargs["extra_objects"],
    ]
    return ext_kwargs
