import glob
import os
import subprocess
from distutils.command.build_ext import build_ext
from distutils.core import Extension

pcre_path = os.getenv("PCRE_PATH", "/opt/pcre/.libs")


# http://code.activestate.com/recipes/502261-python-distutils-pkg-config/
def pkgconfig(libs, optional="", static=False):
    flag_map = {
        "include_dirs": (["--cflags-only-I"], 2, []),
        "library_dirs": (["--libs-only-L"], 2, []),
        "libraries": (["--libs-only-l"], 2, []),
        "extra_compile_args": (
            ["--cflags-only-other"],
            0,
            ["-O0", "-DPCRE_STATIC"],
        ),
        "extra_link_args": (
            ["--libs-only-other"],
            0,
            [
                # f"-Wl,-rpath={pcre_path}",
                # "-l:libpcre.so",
                "-l:libhs.a",
                "-l:libchimera.a",
            ],
        ),
    }
    ext_kwargs = {"extra_compile_args": []}
    library_options = set(
        subprocess.check_output(
            ["pkg-config", optional, "--libs-only-l", *libs]
        )
        .decode()
        .split()
    )
    for lib in libs:
        for distutils_kwarg, (
            pkg_options,
            trim_offset,
            default_value,
        ) in flag_map.items():
            _pkg_options = pkg_options[:]
            if static:
                _pkg_options += [
                    "--static",
                    *[_lib for _lib in libs if _lib != lib],
                ]
            options = set(
                subprocess.check_output(
                    ["pkg-config", optional, *_pkg_options, lib]
                )
                .decode()
                .split()
            )
            ext_kwargs.setdefault(distutils_kwarg, default_value).extend(
                set([opt[trim_offset:] for opt in options])
            )
    ext_kwargs["libraries"] = list(set(ext_kwargs["libraries"]))
    return ext_kwargs


def build(setup_kwargs):
    pkg_config_options = pkgconfig(["libhs", "libch"], static=True)
    for lib in ("pcre", "hs", "chimera"):
        pkg_config_options["libraries"].remove(lib)
    setup_kwargs.update(
        {
            "ext_modules": [
                Extension(
                    "hyperscan._hyperscan",
                    ["src/hyperscan/hyperscanmodule.c"],
                    extra_objects=[
                        os.path.join(pcre_path, "libpcre.a"),
                        *glob.glob(os.path.join(pcre_path, '*.o')),
                    ],
                    **pkg_config_options,
                )
            ],
            "cmdclass": {"build_ext": build_ext},
        }
    )
