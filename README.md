# Hyperscan/Vectorscan for Python

[![Build wheels](https://github.com/darvid/python-hyperscan/actions/workflows/wheels.yml/badge.svg)](https://github.com/darvid/python-hyperscan/actions/workflows/wheels.yml)
![PyPI - Python Version](https://img.shields.io/pypi/pyversions/hyperscan.svg)
![PyPI - Wheel](https://img.shields.io/pypi/wheel/hyperscan.svg)
![PyPI - Status](https://img.shields.io/pypi/status/hyperscan.svg)
![PyPI - License](https://img.shields.io/pypi/l/hyperscan.svg)
[![Read the Docs](https://img.shields.io/readthedocs/python-hyperscan.svg)](https://python-hyperscan.readthedocs.io/en/latest/)

A CPython extension for [Vectorscan][7], an open source fork of
[Hyperscan][8], Intel's open source ([prior to version 5.4][9]),
high-performance multiple regex matching library. Currently only
supports manylinux-compatible Linux distributions.

* ✅ Binary wheels with support for Linux and macOS x86_64 (ARM64 and
  win32 support is planned)
* ✅ Statically linked (no need to build Hyperscan)
* ✅ [Chimera][1] support

## Installation

```shell
# 🪄 Installing libhs is NOT required, because python-hyperscan is statically linked
pip install hyperscan
```

## Build Optimization

If you'd like to use Intel's Hyperscan rather than Vectorscan, or if
you'd like to enable native CPU detection to build optimized non-FAT
libraries ([default off in Vectorscan][11]), extending the
[manylinux-hyperscan][10] Docker image used to build the binary wheels
for this library should be fairly straightforward.

## API Support

``python-hyperscan`` currently exposes *most* of the C API, with the
following caveats or exceptions:

* No [stream compression][2] support.
* No [custom allocator][3] support.
* ``hs_expression_info``, ``hs_expression_ext_info``,
  ``hs_populate_platform``, and ``hs_serialized_database_info`` not
  exposed yet.

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
[7]: https://www.vectorcamp.gr/vectorscan/
[8]: https://www.hyperscan.io/
[9]: https://github.com/VectorCamp/vectorscan?tab=readme-ov-file#hyperscan-license-change-after-54
[10]: https://github.com/darvid/manylinux-hyperscan/
[11]: https://github.com/VectorCamp/vectorscan?tab=readme-ov-file#configure--build
