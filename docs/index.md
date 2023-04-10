# Hyperscan <small>for Python</small>

``python-hyperscan`` is an unofficial CPython extension for
Intel's [Hyperscan][1], the open source, high-performance multiple regex
matching library.

## Quickstart

### Building Hyperscan

See the [official documentation][3] for detailed installation
instructions and dependencies.

The following should work for most use cases, once the prerequisite
dependencies have been installed:

```shell
$ git clone https://github.com/intel/hyperscan.git
$ mkdir -p hyperscan/build
$ cd hyperscan
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

Installing via [pip][4] is recommended:

```shell
$ pip install hyperscan
```

To build and install from source, using [Poetry][5]:

```shell
$ pip install poetry==1.4.2
$ git clone https://github.com/darvid/python-hyperscan.git
$ cd python-hyperscan
$ poetry install
```


[1]: https://01.org/hyperscan
[2]: https://github.com/pypa/manylinux
[3]: http://intel.github.io/hyperscan/dev-reference/getting_started.html#very-quick-start
[4]: https://pypi.org/project/pip/
[5]: https://poetry.eustace.io/
[6]: https://github.com/intel/hyperscan/issues/292
[7]: https://github.com/intel/hyperscan/issues/292#issuecomment-762635447
