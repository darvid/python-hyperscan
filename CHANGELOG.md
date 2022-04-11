# Changelog

<!--next-version-placeholder-->

## v0.3.0 (2022-04-11)
### :sparkles:
* :sparkles:  initial Chimera support (and upgrade to Hyperscan v5.4.0) ([`0527aac`](https://github.com/darvid/python-hyperscan/commit/0527aac060f29413a727e7fbb8db2c456bd25aab))

### Other
* 🚧 build: change commit_parser for semantic_release to emoji_parser  ([`518782e`](https://github.com/darvid/python-hyperscan/commit/518782eb7f3ab24eaffeb76631451904309e161c))
* 🛸 ci: add python 3.10 to CI  ([`06e25bc`](https://github.com/darvid/python-hyperscan/commit/06e25bcc96151428c45fe66f4a0b03d8662c9bf1))
* 📜 docs: update docs to include Chimera requirements  ([`5ad70dc`](https://github.com/darvid/python-hyperscan/commit/5ad70dc4f69ba75a289f83ec007b334eac8f4ec3))
* 🚧 build: dependency upgrades  ([`db2d264`](https://github.com/darvid/python-hyperscan/commit/db2d2645ab38a5d74a0cb9edb4466d79ac3a0462))
* 🚧 build: changes to build.py to support static linking hyperscan  ([`e08c883`](https://github.com/darvid/python-hyperscan/commit/e08c883b244b69c6c0c5206cc2bae70df08eb3c8))
* 🚧 build: update package classifiers to include Python 3.10  ([`63aacd0`](https://github.com/darvid/python-hyperscan/commit/63aacd03e5c87f963b899020cd0f37ef8e5b6ee8))
* 🐛 fix: drop single-sourcing package version (#38)  ([`357cce1`](https://github.com/darvid/python-hyperscan/commit/357cce1d1a7476043d925834d8e6d1ac86c10a39))
* Style: clang-format  ([`7d50f8a`](https://github.com/darvid/python-hyperscan/commit/7d50f8a1368b286ea025d877ab23119cd4c65830))
* 💚 correct clone and build issues ([#37](https://github.com/darvid/python-hyperscan/issues/37)) ([`a8d926f`](https://github.com/darvid/python-hyperscan/commit/a8d926f58b3bb82dac8e49a5f08580c856c0a5cb))
* :green_heart:  fixed PCRE_PATH  ([`a175f28`](https://github.com/darvid/python-hyperscan/commit/a175f289393aaf858751a4f02842d8b9f9dd9ef4))
* :green_heart:  bump manylinux container image version  ([`8962139`](https://github.com/darvid/python-hyperscan/commit/89621390a2fd852cdf31dfc32d5bda205164e43b))
* :green_heart:  reverted upgraded Python requirement in pyproject.toml  ([`ae1e89d`](https://github.com/darvid/python-hyperscan/commit/ae1e89dbd594792e7822b343cd7190cac971513c))
* :green_heart:  removed cp310, thought CI was using manylinux, d'oh  ([`7680840`](https://github.com/darvid/python-hyperscan/commit/768084059a897f8faf08360c94f04672b57a8739))
* Fix project homepage ([#34](https://github.com/darvid/python-hyperscan/issues/34)) ([`1d95aea`](https://github.com/darvid/python-hyperscan/commit/1d95aeab4d467e99abf7172de83df710e9b8a868))
* Docs: update README with correct libhs version requirement  ([`a6ccc92`](https://github.com/darvid/python-hyperscan/commit/a6ccc921b17096103314bd4ff4561f4e6c8fa549))

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
