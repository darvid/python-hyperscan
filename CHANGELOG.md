# Changelog

<!--next-version-placeholder-->

## [0.2.0] - 2020-12-12

### Feature
* Add HS_FLAG_COMBINATION and HS_FLAG_QUIET (#21) ([`9804a55`](https://github.com/darvid/python-hyperscan/commit/9804a5563ee8cfa3e766ceb50481031f73a59009))
* Support extended behavior for patterns (#11) ([`5bfc947`](https://github.com/darvid/python-hyperscan/commit/5bfc94765752b3a6fcc424fd58bedb2ca36ee0cc))
* Support hs_scan_vector (#24) ([`6869b06`](https://github.com/darvid/python-hyperscan/commit/6869b0687a4efb5a8f4028c68b64a876f7d950fb))

### Fix
* Dynamically allocate memory for arrays used by Database.compile (fixes #25) ([`3fe93fc`](https://github.com/darvid/python-hyperscan/commit/3fe93fc81578dd627991473e3f89dc822523578c))

## [0.1.5] - 2020-02-21

### Fixed

- Fixed reference leak in ``Database.compile`` ([#18])

### Changed

- Renamed ``dumps``/``loads`` to the more appropriate
  ``loadb``/``dumpb``. Both use ``bytes`` rather than ``bytearray`` now,
  as well.
- Added serialization/deserialization examples to the usage guide.

## [0.1.4] - 2019-11-07

### Added

- Hyperscan 5.2.0 and support for literal API ([#16])

### Removed

- Python 2.7 support

## [0.1.3] - 2019-07-04

### Fixed

- Handle exceptions in callback. ([#15])

## [0.1.2] - 2019-06-10

### Added

- Release the GIL when compiling patterns ([#13])

## [0.1.1] - 2019-05-01

### Fixed

- Fixed segfault ([#10])

## [0.1.0] - 2019-04-13

### Changed

- ``match_event_handler`` will now halt scanning if a truthy (and
  not **None**) value is returned.
- The C extension module is now accessible with
  ``import hyperscan._hyperscan``, and ``hyperscan.version.__version__``
  now returns the Hyperscan library version as opposed to the Python
  package version. ``hyperscan.__version__`` will, however, return the
  Python package version.
- Major packaging update, now using [Poetry] rather than ``setup.py``.
- Replaced [Sphinx] documentation with [MkDocs] and added an initial
  usage guide.
- Replaced [Travis CI] configuration with [Semaphore].


## [0.0.2] - 2018-05-26

Initial release

[#10]: https://github.com/darvid/python-hyperscan/issues/10
[#13]: https://github.com/darvid/python-hyperscan/issues/13
[#15]: https://github.com/darvid/python-hyperscan/issues/15
[#16]: https://github.com/darvid/python-hyperscan/issues/16
[#18]: https://github.com/darvid/python-hyperscan/issues/18
[MkDocs]: https://www.mkdocs.org/
[Poetry]: https://poetry.eustace.io/
[Semaphore]: https://semaphoreci.com/
[Sphinx]: http://www.sphinx-doc.org/en/master/
[Travis CI]: https://travis-ci.org/
[0.2.0]: https://github.com/darvid/python-hyperscan/releases/tag/v0.2.0
[0.1.5]: https://github.com/darvid/python-hyperscan/releases/tag/v0.1.5
[0.1.4]: https://github.com/darvid/python-hyperscan/releases/tag/v0.1.4
[0.1.3]: https://github.com/darvid/python-hyperscan/releases/tag/v0.1.3
[0.1.2]: https://github.com/darvid/python-hyperscan/releases/tag/v0.1.2
[0.1.1]: https://github.com/darvid/python-hyperscan/releases/tag/v0.1.1
[0.1.0]: https://github.com/darvid/python-hyperscan/releases/tag/v0.1.0
[0.0.2]: https://github.com/darvid/python-hyperscan/releases/tag/v0.0.2
