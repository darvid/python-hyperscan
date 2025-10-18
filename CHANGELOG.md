# Changelog

## v0.7.23 — 2025-08-07
- Upgraded the bundled Vectorscan engine to 5.4.12 (with SIMDe) for broader CPU support.
- Replaced the hard-coded `-march=core2` flag with host architecture detection, fixing arm/Apple Silicon builds.
- Silenced Clang `unqualified-std-cast-call` warnings in the extension sources.

## v0.7.19 — 2025-07-11
- Resolved the Unicode pattern compilation regression reported in GH-207.

## v0.7.18 — 2025-07-05
- Ensured Chimera match handlers decref matches properly to avoid leaks.
- Verified the GIL is held for `Py_DECREF`/`Py_XDECREF`, preventing crashes under load.

## v0.7.16 — 2025-05-28
- Guarded `Database_dealloc` against missing scratch space to avoid null dereferences.

## v0.7.13 — 2025-05-18
- Repacked the `__init__.pyi` stub so type checkers can locate the shipped typings.

## v0.7.10 — 2025-05-13
- Restored the original distribution name to unblock PyPI publishing.

## v0.7.9 — 2025-02-14
- Migrated the build to `scikit-build-core` and modernized tooling with `uv`.
- Landed the initial Windows build pipeline (new `build_tools/windows` scripts and `setup.py` flow).
- Published a `py.typed` marker to expose typing metadata to downstream projects.

## v0.7.8 — 2024-10-12
- Added official support for Python 3.13.
- Streamlined the macOS build workflow for Apple Silicon runners.

## v0.7.1 — 2024-03-10
- Added native macOS arm64 wheels and expanded aarch64 coverage in CI.
- Tuned cibuildwheel caching and release automation for the new architectures.

## v0.7.0 — 2024-01-26
- Switched the packaged engine to Vectorscan 5.4.11 (replacing upstream Hyperscan).
- Overhauled wheel automation to accommodate the new backend and Python 3.12.

## v0.6.0 — 2023-08-09
- Added manylinux_2_28 wheels and related build improvements.
- Stabilized version detection when building from source archives.

## v0.5.0 — 2023-08-08
- Updated to Hyperscan 5.4.2 alongside a packaging/tooling overhaul.
- Fixed the `Database.dumpb` leak and added a `mode` parameter to `Database.loadb`.

## v0.4.0 — 2023-01-11
- Introduced typing stubs for the public API.
- Patched the `Database.loadb` leak and refreshed the release workflow.

## v0.3.3 — 2023-01-06
- Fixed the `loadb`/error handling regression, dropped Python 3.6, and refreshed CI.

## v0.3.2 — 2022-06-13
- Hotfix for the static linking regression introduced in v0.3.1.

## v0.3.1 — 2022-06-13
- Prevented PCRE from leaking into extension linkage and cleaned up build flags.

## v0.3.0 — 2022-04-11
- Added initial Chimera support and upgraded to Hyperscan 5.4.0.
- Documented the new requirements and expanded build tooling.

## v0.2.0 — 2020-12-12
- Added `HS_FLAG_COMBINATION`, `HS_FLAG_QUIET`, and `hs_scan_vector`.
- Improved pattern compilation stability and moved to a `src/` package layout.

## v0.1.5 — 2020-02-22
- Patched packaging regressions caused by the `zipp` dependency and tidied CI.

## v0.1.4 — 2019-11-07
- Added Hyperscan 5.2.0 support and the literal API.
- Dropped Python 2.7 support and hardened the build scripts.

## v0.1.3 — 2019-08-04
- Ensured scanner callbacks propagate exceptions correctly.

## v0.1.2 — 2019-06-11
- Released the GIL during pattern compilation and trimmed aging Python versions from CI.

## v0.1.1 — 2019-05-02
- Fixed segfaults in `Stream.scan` and `Database.scan` by tightening GIL handling.

## v0.1.0 — 2019-04-14
- Initial release with the ability to halt scans mid-stream and the first automated release pipeline.

## v0.0.2 — 2018-05-26
- Added audited manylinux wheels, improved `setup.py` discovery, and hardened the original Travis CI deploy scripts.

### Maintenance-only tags
- 0.7.25 (2025-08-08), 0.7.24 (2025-08-07), 0.7.22 (2025-07-13), 0.7.21 (2025-07-12), 0.7.20 (2025-07-12), 0.7.14 (2025-05-19), 0.7.12 (2025-05-14), 0.7.11 (2025-05-14), 0.7.7 (2024-03-11), 0.7.6 (2024-03-11), 0.7.5 (2024-03-11), 0.7.4 (2024-03-10), 0.7.3 (2024-03-10), and 0.7.2 (2024-03-10) only adjusted release automation and contain no code or packaging changes.
