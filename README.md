# Hyperscan for Python

![python-hyperscan workflow](https://github.com/darvid/python-hyperscan/workflows/python-hyperscan%20workflow/badge.svg)
![PyPI - Python Version](https://img.shields.io/pypi/pyversions/hyperscan.svg)
![PyPI - Wheel](https://img.shields.io/pypi/wheel/hyperscan.svg)
![PyPI - Status](https://img.shields.io/pypi/status/hyperscan.svg)
![PyPI - License](https://img.shields.io/pypi/l/hyperscan.svg)
[![Read the Docs](https://img.shields.io/readthedocs/python-hyperscan.svg)](https://python-hyperscan.readthedocs.io/en/latest/)

A CPython extension for [Hyperscan](https://www.hyperscan.io/), Intel's
open source, high-performance multiple regex matching library. Currently
only supports manylinux-compatible Linux distributions.

## Installation

```shell
pip install hyperscan
```

## API Support

``python-hyperscan`` currently exposes *most* of the C API, with the
following caveats or exceptions:

* No [stream compression][2] support.
* No [custom allocator][3] support.
* ``hs_expression_info``, ``hs_expression_ext_info``,
  ``hs_populate_platform``, and ``hs_serialized_database_info`` not
  exposed yet.

✨ As of v0.3.0, ``python-hyperscan`` statically links against Hyperscan,
so having the library installed on your system is not required. Prior
versions of ``python-hyperscan`` require Hyperscan v5.2 or newer. ✨

Building from source requires Hyperscan compiled and installed with the
following CMake flags set:

* ``FAT_RUNTIME=OFF``
* ``BUILD_STATIC_AND_SHARED=ON`` *only if* ``BUILD_SHARED_LIBS`` is also
  on, in the event there are other applications colocated with
  ``python-hyperscan`` that need the shared libraries. Otherwise ignore
  this flag, as by default Hyperscan will build static libraries.
* ``CMAKE_C_FLAGS`` and ``CMAKE_CXX_FLAGS`` set to ``-fPIC``

See the [documentation][6] for more detailed build instructions.

## Resources

* [PyPI Project](https://pypi.org/project/hyperscan/)
* [Documentation][6]
* [Hyperscan C API Documentation](http://intel.github.io/hyperscan/dev-reference/)

[1]: http://intel.github.io/hyperscan/dev-reference/chimera.html
[2]: http://intel.github.io/hyperscan/dev-reference/runtime.html#stream-compression
[3]: http://intel.github.io/hyperscan/dev-reference/runtime.html#custom-allocators
[4]: http://intel.github.io/hyperscan/dev-reference/compilation.html
[5]: https://github.com/darvid/python-hyperscan/issues
[6]: https://python-hyperscan.readthedocs.io
