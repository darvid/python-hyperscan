import glob
import os
import subprocess
from distutils.sysconfig import get_python_inc

from setuptools import Extension, setup

pcre_path = os.getenv("PCRE_PATH", "/opt/pcre/.libs")
hs_cflags = subprocess.getoutput("pkg-config --cflags libhs").strip()

hyperscan_ext = Extension(
    "hyperscan._ext",
    sources=["src/hyperscan/hyperscanmodule.c"],
    include_dirs=[hs_cflags[2:], get_python_inc(plat_specific=1)],
    libraries=["hs", ":libchimera.a", "m", "stdc++"],
    library_dirs=["/opt/hyperscan/lib64", "/usr/lib/x86_64-linux-gnu"],
    extra_compile_args=["-O0", "-DPCRE_STATIC"],
    extra_link_args=["-l:libhs.a", "-l:libchimera.a"],
    extra_objects=[
        os.path.join(pcre_path, "libpcre.a"),
        *glob.glob(os.path.join(pcre_path, '*.o')),
    ],
)


setup(ext_modules=[hyperscan_ext])
