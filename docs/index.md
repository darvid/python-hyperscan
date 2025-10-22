# Hyperscan <small>for Python</small>

``python-hyperscan`` is a CPython extension that statically links
[Vectorscan][1] (Linux/macOS) or [Hyperscan][2] (Windows) together with
PCRE and Boost to provide high-performance multi-pattern regex
matching—with Chimera support—out of the box.

## Quickstart

### Install from PyPI

```shell
pip install hyperscan
```

- Binary wheels are published for CPython 3.9–3.14 across Linux
  (manylinux2014/manylinux_2_28 and musllinux on x86_64 & aarch64),
  macOS 11+ universal2, and Windows AMD64.
- Each wheel bundles the scanning engine (Vectorscan 5.4.12 on Unix,
  Intel Hyperscan 5.4.2 on Windows), PCRE 8.45 with UTF-8 + Unicode
  property support, Ragel, and Boost 1.87.0—no system packages
  required.
- Chimera runtimes ship in every build; enable them at runtime with
  ``hyperscan.Database(chimera=True)``.

### Verify the install

```shell
python - <<'PY'
import hyperscan
print(hyperscan.Database().info())
PY
```

Expect output similar to ``Version: 5.4.12 Features: ...`` confirming
the bundled engine.

## When to build from source

The published wheels cover most use cases. Build locally only when you
need to patch the extension, experiment with upstream Vectorscan or
Hyperscan, or produce architecture-native binaries.

### Prerequisites

- Python 3.9–3.14
- [CMake 3.31+][3] (required by ``scikit-build-core``)
- A C/C++ toolchain (GCC/Clang on POSIX, Visual Studio 2022 on
  Windows)
- Ragel 6.9+; if it is not on ``PATH`` the build system can fetch and
  build it automatically.

### Standard workflow

```shell
git clone https://github.com/darvid/python-hyperscan.git
cd python-hyperscan
python -m venv .venv
source .venv/bin/activate  # .\.venv\Scripts\activate on Windows
pip install --upgrade pip build[uv]
pip install .
```

``pip install .`` invokes ``scikit-build-core`` to compile the extension
and vendor the scanning engine automatically. To build a wheel instead
of an in-place install, run ``python -m build`` or ``uvx --from build
pyproject-build --installer=uv --wheel``.

### Customizing the engine

- Set ``CMAKE_ARGS="-DUSE_CPU_NATIVE=ON"`` (and other standard
  Vectorscan options) when invoking ``pip``/``build`` to produce
  processor-specific binaries.
- Provide pre-built libraries by exporting ``HS_BUILD_LIB_ROOT`` (the
  directory containing ``libhs``, ``libhs_runtime``, ``libchimera`` and
  ``libpcre``) and ``HS_SRC_ROOT`` (the matching Vectorscan/Hyperscan
  source tree). When both variables are set, the build reuses the
  supplied artifacts instead of fetching and compiling the engine.

[1]: https://www.vectorcamp.gr/vectorscan/
[2]: https://www.hyperscan.io/
[3]: https://scikit.build/
