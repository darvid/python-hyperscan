import glob
import os
import os.path
import subprocess
import sys
from distutils.sysconfig import get_python_inc

from setuptools import Extension, setup

try:
    from semantic_release import setup_hook  # pyright: ignore

    if __name__ == "__main__":
        setup_hook(sys.argv)
except ImportError:
    pass


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
        *glob.glob(os.path.join(pcre_libdir, "*.o")),
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


if __name__ == "__main__":
    setup(
        ext_modules=[
            Extension(
                "hyperscan._ext",
                sources=["src/hyperscan/hyperscanmodule.c"],
                library_dirs=os.getenv("LIBRARY_PATH", "").split(":"),
                extra_compile_args=["-O0", "-DPCRE_STATIC"],
                # extra_link_args=["-Wl,--exclude-libs,ALL"],
                **get_platform_specific_options(),
            )
        ],
    )
