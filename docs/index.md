# Hyperscan <small>for Python</small>

``python-hyperscan`` is an unofficial CPython extension for
Intel's [Hyperscan][1], the open source, high-performance multiple regex
matching library.

## Quickstart

### Building Hyperscan

See the [official documentation][3] for detailed installation
instructions. The following should work for most use cases:

```shell
$ git clone https://github.com/intel/hyperscan.git
$ mkdir -p hyperscan/build
$ cd hyperscan/build
$ git checkout v5.1.1
$ cmake \
    -G "Unix Makefiles" \
    -DCMAKE_INSTALL_PREFIX:PATH=/usr \
    -DBUILD_SHARED_LIBS=ON ../
$ # Compile with all available cores:
$ # make -j $(( $(nproc) + 1 ))
$ make
$ sudo make install
```

### Installing python-hyperscan

Installing via [pip][4] is recommended:

```shell
$ pip install hyperscan
```

To build and install from source, using [Poetry][5]:

```shell
$ pip install poetry
$ git clone https://github.com/darvid/python-hyperscan.git
$ cd python-hyperscan
$ poetry install
```


[1]: https://01.org/hyperscan
[2]: https://github.com/pypa/manylinux
[3]: http://intel.github.io/hyperscan/dev-reference/getting_started.html#very-quick-start
[4]: https://pypi.org/project/pip/
[5]: https://poetry.eustace.io/
