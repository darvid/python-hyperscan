# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

python-hyperscan is a CPython extension providing Python bindings for Vectorscan (fork of Intel's Hyperscan), a high-performance multiple regex matching library. The project enables fast pattern matching with statically linked binaries and cross-platform support.

## Development Commands

### Environment Setup
```bash
# Install tools via mise
mise install

# Install Python dependencies  
uv sync --no-editable --no-install-project

# Install dev dependencies
uv sync --only-dev --no-editable --no-install-project
```

### Building
```bash
# Build source distribution
uvx --from build pyproject-build --installer=uv --sdist --verbose

# Build wheels (via cibuildwheel)
cibuildwheel --platform linux
```

### Testing
```bash
# Run all tests
pytest tests/ -vvv

# Run specific test
pytest tests/test_hyperscan.py::test_name -vvv

# Test with coverage
pytest --pyargs hyperscan/tests -vvv
```

### Code Quality
```bash
# Format C code
clang-format -i src/hyperscan/extension.c

# Format Python code  
black src/

# Lint Python code
ruff check src/ --fix

# Full lint workflow (matches CI)
ruff check --fix src/ && black src/
```

## Architecture

### Core Components
- **Database**: Compiled regex patterns (`hyperscan.Database`)
- **Scratch**: Thread-specific scratch space for matching
- **Stream**: Context manager for streaming text scanning
- **ExpressionExt**: Extended pattern configuration

### File Structure
```
src/hyperscan/
├── __init__.py         # Main Python API
├── __init__.pyi        # Type stubs  
├── extension.c         # C extension implementation
└── _version.py         # Version info

tests/                  # pytest test suite
examples/               # Usage examples & benchmarks
build_tools/            # Cross-platform build scripts
cmake/                  # CMake configuration
```

### Scanning Modes
- `HS_MODE_BLOCK` - Complete text blocks
- `HS_MODE_STREAM` - Streaming data
- `HS_MODE_VECTORED` - Multiple buffers
- Chimera mode - PCRE + Hyperscan features

### Build System
- **scikit-build-core** backend with CMake
- **Static linking** of Boost, PCRE, Ragel, Vectorscan
- **Cross-compilation** for Linux, macOS (including ARM64), Windows
- **manylinux wheels** for easy distribution

## Development Notes

### Dependencies
- Uses **uv** for fast Python package management
- **mise** for tool version management
- **CMake 3.31+** required for builds
- **Vectorscan 5.4.11** statically linked

### Testing Strategy
- pytest with fixtures for database setup
- Tests for block, stream, and vectored scanning
- Chimera mode testing
- Platform-specific CI testing (excludes emulated archs)

### Release Process
- **Semantic release** with emoji commits
- **GitHub Actions** for build/test/publish
- **Binary wheels** for multiple platforms
- Version sync between pyproject.toml and _version.py

## Unicode Support (Issue #207)

### Problem
Starting in v0.7.9, users reported "Expression is not valid UTF-8" errors when compiling valid Unicode patterns (Arabic, Hebrew, etc.). This broke existing code that worked in v0.7.8.

### Root Cause
The build system migration from setup.py to CMake changed how PCRE is linked:
- **v0.7.8 and earlier**: Used system PCRE via pkg-config (had UTF-8 support)
- **v0.7.9+**: Builds PCRE from source without UTF-8 support enabled

### Solution
Force PCRE UTF-8 support in CMakeLists.txt by patching PCRE's build configuration:
```cmake
# Force PCRE UTF-8 settings globally for all platforms
set(PCRE_SUPPORT_UTF8 ON CACHE BOOL "Enable UTF-8 support in PCRE" FORCE)
set(PCRE_SUPPORT_UNICODE_PROPERTIES ON CACHE BOOL "Enable Unicode properties support in PCRE" FORCE)
```

### HS_FLAG_UTF8 Limitations
We **avoid using HS_FLAG_UTF8** due to well-documented Hyperscan/Vectorscan bugs:
- **intel/hyperscan#57**: UTF-8 match failures with `\Q...\E` literal patterns
- **intel/hyperscan#133**: Ragel v7 parser bug incorrectly rejects valid UTF-8 sequences  
- **intel/hyperscan#163**: Severe performance issues with UTF-8 + case-insensitive flags

### Best Practice
Unicode patterns work correctly **without HS_FLAG_UTF8** when PCRE has proper UTF-8 support. The runtime flag triggers stricter validation that often fails on valid patterns, while the underlying PCRE engine handles Unicode correctly when built with UTF-8 support.