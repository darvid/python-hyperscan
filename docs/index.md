# Hyperscan <small>for Python</small>

``python-hyperscan`` is a CPython extension for [Vectorscan][1], an
open source fork of [Hyperscan][2], Intel's open source
([prior to version 5.4][3]), high-performance multiple regex matching
library.

## Quickstart

### Building Vectorscan

See the [official documentation][4] for detailed installation
instructions and dependencies.

The following should work for most use cases, once the prerequisite
dependencies have been installed:

```shell
$ git clone https://github.com/VectorCamp/vectorscan
$ mkdir -p vectorscan/build
$ cd vectorscan
$ # PCRE1 (8.xx series) is required for Chimera support
$ wget -qO- https://sourceforge.net/projects/pcre/files/pcre/8.45/pcre-8.45.tar.gz/download | tar xvz
$ git checkout v5.4.0
$ cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr \
    -DBUILD_STATIC_AND_SHARED=ON \
    -DFAT_RUNTIME=ON \
    -DPCRE_SOURCE=../pcre-8.45 \
    ../
$ # Compile with all available cores:
$ # make -j $(( $(nproc) + 1 ))
$ make
$ sudo make install
```

**Note:** if you are building v5.4.0 and encounter undefined reference
errors when linking, see [intel/hyperscan#292][6] (and more specifically,
[this comment][7]) for the fix. Per the maintainers, this will be fixed
in the next Hyperscan release.

### Installing python-hyperscan

Installing via [pip][5] is recommended:

```shell
pip install hyperscan
```

[1]: https://www.vectorcamp.gr/vectorscan/
[2]: https://www.hyperscan.io/
[3]: https://github.com/VectorCamp/vectorscan?tab=readme-ov-file#hyperscan-license-change-after-54
[4]: https://github.com/VectorCamp/vectorscan
[5]: https://pypi.org/project/pip/
[6]: https://poetry.eustace.io/
[7]: https://github.com/intel/hyperscan/issues/292
