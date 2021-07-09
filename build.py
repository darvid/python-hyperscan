import glob
import os
import subprocess
from distutils.command.build_ext import build_ext
from distutils.core import Extension

pcre_path = os.getenv("PCRE_PATH", "/opt/pcre/.libs")


# http://code.activestate.com/recipes/502261-python-distutils-pkg-config/
def pkgconfig(libs, optional=""):
    flag_map = {
        "include_dirs": (["--cflags-only-I"], 2, []),
        "library_dirs": (["--libs-only-L"], 2, []),
        "libraries": (["--libs-only-l"], 2, []),
        "extra_compile_args": (["--cflags-only-other"], 0, ["-O0"]),
        "extra_link_args": (
            ["--libs-only-other"],
            0,
            [
                # f"-Wl,-rpath={pcre_path}",
                # "-l:libpcre.so",
            ],
        ),
    }
    ext_kwargs = {"extra_compile_args": ["-fPIC"]}
    for lib in libs:
        for distutils_kwarg, (
            pkg_options,
            trim_offset,
            default_value,
        ) in flag_map.items():
            try:
                options = (
                    subprocess.check_output(
                        ["pkg-config", optional, *pkg_options, lib]
                    )
                    .decode()
                    .split()
                )
            except subprocess.CalledProcessError:
                continue
            ext_kwargs.setdefault(distutils_kwarg, default_value).extend(
                [opt[trim_offset:] for opt in options]
            )
    return ext_kwargs


def build(setup_kwargs):
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
                    **pkgconfig(["libhs", "libch"]),
                )
            ],
            "cmdclass": {"build_ext": build_ext},
        }
    )
