# CHANGELOG


## v0.7.8 (2024-10-12)

### :bug:

* fix: :bug: :ferris_wheel: go back to python-semantic-release (sigh) ([`8c15651`](https://github.com/darvid/python-hyperscan/commit/8c15651fe31fd25589e4040173baca7be5da66be))

* fix: :bug: update docs, force release for macOS arm wheels ([`725a373`](https://github.com/darvid/python-hyperscan/commit/725a373247109a264e49a6656d55d5612cd15082))

### :sparkles:

* feat: :sparkles: support macOS arm64 ([`ba361e4`](https://github.com/darvid/python-hyperscan/commit/ba361e44d157d95050fba3f0d44656510f512371))

* feat: :sparkles: switch to Vectorscan (v5.4.11) ([`9c94b2d`](https://github.com/darvid/python-hyperscan/commit/9c94b2da85a0d085488a2498aa9545023f5b3b0e))

### Other

* ci: :ferris_wheel: update macos build process (#160) (#161) ([`fde63df`](https://github.com/darvid/python-hyperscan/commit/fde63df6cfcbca5640a350dda81961a32538bc7e))

* feat: add support for Python 3.13 (#158) ([`684b308`](https://github.com/darvid/python-hyperscan/commit/684b308acd9008a42466a549495cd45ed40fa509))

* Update lint actions ([`4ba746c`](https://github.com/darvid/python-hyperscan/commit/4ba746cf9d5f8b103b652671e4c0cef5a24e0d42))

* ci: :ferris_wheel: revert artifact moving ([`1588481`](https://github.com/darvid/python-hyperscan/commit/1588481eb4e6ecc2a179d4808b76f5db5e3b73e6))

* ci: :ferris_wheel: restructure downloaded artifacts in publish workflow ([`c0b1e39`](https://github.com/darvid/python-hyperscan/commit/c0b1e394f99921e379884dac8b6f56f0ca7f3f59))

* ci: :ferris_wheel: fix wheels cache key ([`6c285dd`](https://github.com/darvid/python-hyperscan/commit/6c285dd42630f35a3218107e7c461f0fd087c0bf))

* ci: :ferris_wheel: more CI fix spam, fix hashFiles glob and more ([`db862be`](https://github.com/darvid/python-hyperscan/commit/db862be182adb00700cbb6858dc76aff7d867d83))

* ci: :ferris_wheel: add release environment ([`3141a8a`](https://github.com/darvid/python-hyperscan/commit/3141a8a01bb1e84333b07f0a8eed6d186a6fc6af))

* ci: :ferris_wheel: only incr version for wheel building on push ([`9f38e09`](https://github.com/darvid/python-hyperscan/commit/9f38e09975d78546b33e7ec3213a04a491e6db7d))

* ci: :ferris_wheel: more CI release fixes ([`5c3877a`](https://github.com/darvid/python-hyperscan/commit/5c3877a4dee425c8d0f60faa6714f43e95b0e845))

* ci: :ferris_wheel: use gh cli for creating release PRs ([`f08c88e`](https://github.com/darvid/python-hyperscan/commit/f08c88e234d393e80680b09dca5432aed37c3948))

* ci: :ferris_wheel: add checkout job and concurrency to publish workflow ([`7fa6034`](https://github.com/darvid/python-hyperscan/commit/7fa60345269c7e6ed638fd07171406c160788ce1))

* ci: :ferris_wheel: set github pat in env as workaround ([`0d89e3d`](https://github.com/darvid/python-hyperscan/commit/0d89e3dd55c096f3b588883b09410033298d0c7d))

* ci: :ferris_wheel: add github pat for create PR job ([`fb22f32`](https://github.com/darvid/python-hyperscan/commit/fb22f32ded9f9e6c2989a34c6d57048500d91d8e))

* ci: :ferris_wheel: add publish workflow ([`a21f4c9`](https://github.com/darvid/python-hyperscan/commit/a21f4c9263649ec6ede6c36508ff0429105b350f))

* ci: :ferris_wheel: update release workflow to depend on build ([`70ac166`](https://github.com/darvid/python-hyperscan/commit/70ac166a634e56d437cd1f8014fb90ec5ea32c50))

* build: :package: update pdm.lock ([`ff0e52b`](https://github.com/darvid/python-hyperscan/commit/ff0e52bf6a9634be2cfa0076217cd600405fc34c))

* ci: :ferris_wheel: refactor wheel build versioning ([`a7f19b8`](https://github.com/darvid/python-hyperscan/commit/a7f19b84e50744b0b3fd25925eeca2920960236c))

* ci: :ferris_wheel: debug semantic-release ([`451d373`](https://github.com/darvid/python-hyperscan/commit/451d373a79b2e5a9d3328811b540fe3258ec1d6e))

* ci: :ferris_wheel: add --patch to version bump in wheel build process ([`f1bec94`](https://github.com/darvid/python-hyperscan/commit/f1bec9440db075c6476f1a7debf054f4b8c56d7a))

* ci: :ferris_wheel: force upload wheel and source artifacts in CI ([`e8732a2`](https://github.com/darvid/python-hyperscan/commit/e8732a238b68b32a14d5093bc08c50333dcd7bfd))

* ci: :ferris_wheel: update python-semantic-release configuration ([`7ab3e8b`](https://github.com/darvid/python-hyperscan/commit/7ab3e8b025bc63bca07a91fa966728a698c11b49))

* ci: :ferris_wheel: update node version for semantic-release ([`f9c8ae0`](https://github.com/darvid/python-hyperscan/commit/f9c8ae02e8b8243410344cee1cd6fc4c02dd419c))

* ci: :ferris_wheel: refactor CI runner build deps caching ([`13e7430`](https://github.com/darvid/python-hyperscan/commit/13e7430107d037dd94a9edf2cab7e1b527ea5d56))

* ci: :ferris_wheel: use semantic-release (node) [release:patch] ([`1a3b3a7`](https://github.com/darvid/python-hyperscan/commit/1a3b3a7b5be749c00d74beb20dd5605dd7290064))

* ci: :ferris_wheel: fix macOS actions logic ([`e178849`](https://github.com/darvid/python-hyperscan/commit/e178849b918950650d5c283ff08771f9660506c4))

* ci: :ferris_wheel: fix musllinux arm docker image in cibw ([`38f0f22`](https://github.com/darvid/python-hyperscan/commit/38f0f22ee8313df7871d25f8a534854ceaf52f5f))

* ci: :ferris_wheel: fix macOS CI (restore build deps cache) ([`371fe0c`](https://github.com/darvid/python-hyperscan/commit/371fe0c3f983c0d3caa0bdec25f79d0b74a95945))

* build: :package: add additional aarch64 builds and use ubicloud ([`68ea9ff`](https://github.com/darvid/python-hyperscan/commit/68ea9fffc6e28b5bf88ca1943053731a687a7a17))

* ci: :ferris_wheel: more CI improvements ([`90f2c5f`](https://github.com/darvid/python-hyperscan/commit/90f2c5f02f54ec7f52a88ca2d39633427b3739e2))

* ci: :ferris_wheel: create necessary hyperscan/pcre directories on macOS runners ([`68db2ad`](https://github.com/darvid/python-hyperscan/commit/68db2ad7bbd552aec05de1191691ce59de2b92f8))

* build: :package: update dependencies (mostly for dev), fix macOS build caching ([`d375a04`](https://github.com/darvid/python-hyperscan/commit/d375a04eb5879ca35039a81eb705c16fcd749490))

* build: :package: remove musllinux pypy ([`3a0aea6`](https://github.com/darvid/python-hyperscan/commit/3a0aea6c92a65b6299a106f117c83c76028a861d))

* build: :package: fix dupe pypy builds, change to pypy musllinux ([`b8ec574`](https://github.com/darvid/python-hyperscan/commit/b8ec574e6403a769243be7e938f0d624bf8bd6c0))

* build: :package: refactor macos cibuildwheel configuration ([`db7d729`](https://github.com/darvid/python-hyperscan/commit/db7d7299da4181be8ea8011e72fd6d182aad6e20))

* build: :package: disable arm64 for macosx python 3.9 ([`1b2b662`](https://github.com/darvid/python-hyperscan/commit/1b2b662b92cb3a7cc6d3f94f1b4cf16bd11bbc66))

* build: :package: update macos runner to Apple silicon, add aarch64 to CIBW archs ([`629074f`](https://github.com/darvid/python-hyperscan/commit/629074f235608bc9096892fe224aa22eddda09cf))

* build: :package: add aarch64 to build_wheels matrix ([`693f803`](https://github.com/darvid/python-hyperscan/commit/693f803914d6eb8ae4d68b0cf53f5e60f7a016cf))

* ci: :ferris_wheel: update checkout and setup-pdm actions to v4 ([`9189351`](https://github.com/darvid/python-hyperscan/commit/9189351249232c1d7bdda838f4ec28a9a1a4c846))

* build: :package: reduce setuptools pinned version to 68 ([`dbf3062`](https://github.com/darvid/python-hyperscan/commit/dbf306290d63f7d6abed21f4199df1843ae1fda7))

* build: :package: remove python 3.8 from build matrix ([`79105d3`](https://github.com/darvid/python-hyperscan/commit/79105d3154c9403870447b513f38155a23cc3139))

* build: :package: update cibuildwheel configuration to support aarch64 ([`7e06101`](https://github.com/darvid/python-hyperscan/commit/7e0610123fb7d98423a8af8cbc17e268002df702))

* create FUNDING.yml

Signed-off-by: David Gidwani <david.gidwani@atomweight.io> ([`0228bb6`](https://github.com/darvid/python-hyperscan/commit/0228bb68d8849eb2d9f49f9da1840087ddf0f226))

* build: :package: (hopefully) fix versioning with semantic-release and pdm/setuptools ([`a322379`](https://github.com/darvid/python-hyperscan/commit/a322379f226cd6ceb011a8024bb68513d991e1f1))

* ci: :ferris_wheel: set upstream when creating local PR branch ([`615420b`](https://github.com/darvid/python-hyperscan/commit/615420b26d4dfbe5b55f69864df36a38c631cd50))

* ci: :ferris_wheel: add create-pull-request branch to release group ([`12867df`](https://github.com/darvid/python-hyperscan/commit/12867df2eac284e562262ed1de314003a6dffd50))

* ci: :ferris_wheel: set base branch and assignees for create-pull-request ([`ea2c582`](https://github.com/darvid/python-hyperscan/commit/ea2c58243f1e4f256361d10d9ac451e239d29bd6))

* ci: :ferris_wheel: don't set upstream after creating PR branch ([`a6f0d3c`](https://github.com/darvid/python-hyperscan/commit/a6f0d3cd28e5cb687f7deb3554baff86a059cc7a))

* ci: :ferris_wheel: fix release CI (again) ([`66626d4`](https://github.com/darvid/python-hyperscan/commit/66626d4f1ca908645fdac5f0d5581a21add58300))

* build: :package: fix release CI ([`5a50cf3`](https://github.com/darvid/python-hyperscan/commit/5a50cf3fa1e5eb1bc11336174977587887a96db6))

* build: :package: allow release on workflow_dispatch ([`a6b9152`](https://github.com/darvid/python-hyperscan/commit/a6b91522b420b1147aed5a7947ab5eb0c0c8f3ba))

* build: :package: fix python-semantic-release release group ([`74801ac`](https://github.com/darvid/python-hyperscan/commit/74801ac86af462484133ff57e4587e53b92f55ae))

* build: :package: improve release CI/CD ([`95a3c8e`](https://github.com/darvid/python-hyperscan/commit/95a3c8e3ab585c6be9ffb106d3d323b1c226874c))

* ci: :ferris_wheel: remove PAT from checkout action ([`be00d52`](https://github.com/darvid/python-hyperscan/commit/be00d52096906e1e9c9151e149e5bed61a9123cb))

* ci: :ferris_wheel: update lint CI ([`32920b1`](https://github.com/darvid/python-hyperscan/commit/32920b157e2f7620d7618ad67244d6228ec58552))

* docs: :memo: update README ([`d58f4c6`](https://github.com/darvid/python-hyperscan/commit/d58f4c6746a7439c9816bd0edff24b69f6fcfd9c))

* build: :package: update library paths to vectorscan in cibuildwheel config ([`24c7713`](https://github.com/darvid/python-hyperscan/commit/24c77135c58ccaa0c6d1c208489b96a6244cf34f))

* build: :package: fix macos build ([`e72a700`](https://github.com/darvid/python-hyperscan/commit/e72a7003c21d46f1648ddf748d2f10edb608727f))

* build: :package: update cibuildwheel configuration and corrected build_wheels.sh ([`7b5b507`](https://github.com/darvid/python-hyperscan/commit/7b5b507ffc34dabc7507408c0a988300f6b08d78))

* build: :package: only release if github ref is main ([`7af840f`](https://github.com/darvid/python-hyperscan/commit/7af840f37a120990537f1aeeaf4fa63943ae3518))

* build: :package: update build to switch to Vectorscan 5.4.11 ([`93e6fb7`](https://github.com/darvid/python-hyperscan/commit/93e6fb7e4f25f373fff587fdce47c43141b64805))

* ci: 🎡 rename wheels action ([`c7493ce`](https://github.com/darvid/python-hyperscan/commit/c7493cef503ff3023a2e5bad2b2d63ba7980bb8e))

* ci: 🎡 build sdist with setup.py instead of pdm ([`a3c3bdd`](https://github.com/darvid/python-hyperscan/commit/a3c3bddf9d18bf2ad1fa0f0da9e038529006e0c1))

* ci: 🎡 pass required env vars for building macOS wheels ([`f9e503b`](https://github.com/darvid/python-hyperscan/commit/f9e503be9353077a805ab3694f624d440f050360))

* ci: :ferris_wheel: use GNU tar on macOS (in build_hyperscan.sh) ([`20ad70d`](https://github.com/darvid/python-hyperscan/commit/20ad70d4ae922837c02bf658924960ce3e43bb2a))

* ci: 🎡 update cibuildwheel version and pdm lockfile ([`180696b`](https://github.com/darvid/python-hyperscan/commit/180696b236b0fa47be6a7cedae6d54a2e0501c11))

* ci: 🎡 update manylinux-hyperscan tag again ([`c516700`](https://github.com/darvid/python-hyperscan/commit/c516700abd9affaad05686295cd19c07059bf5d4))

* ci: 🎡 update manylinux-hyperscan container tag to latest ([`8dad7a9`](https://github.com/darvid/python-hyperscan/commit/8dad7a9c64356f07acd2079a62b1dee43ff75d37))

* ci: 🎡 add support for building 3.12 wheels ([`9d4c114`](https://github.com/darvid/python-hyperscan/commit/9d4c114c780a49780ec48bd19715da21c3abf8d8))

* ci: :ferris_wheel: enable skip-existing for pypi publishing ([`47b68a7`](https://github.com/darvid/python-hyperscan/commit/47b68a74d18d660c6f12e036a0778b89a107606c))

* ci: :ferris_wheel: add needs to release ([`d2ed026`](https://github.com/darvid/python-hyperscan/commit/d2ed026901291f8a610f735c437d7ca651188bb0))

* ci: :ferris_wheel: only upload dist cache if cache miss ([`2d05b53`](https://github.com/darvid/python-hyperscan/commit/2d05b539f276aa3cf01828ea2e63028ab90527a7))

* ci: :ferris_wheel: add contents permission and concurrency to release ([`89cfc80`](https://github.com/darvid/python-hyperscan/commit/89cfc80ebf586f0642d43695454e1f45bb195711))

* ci: :ferris_wheel: add id-token permission to release job ([`584b635`](https://github.com/darvid/python-hyperscan/commit/584b63586fae2a0167be82dbb6e9a0f4e1416024))

* ci: :ferris_wheel: create dist directory again ([`f2e6bca`](https://github.com/darvid/python-hyperscan/commit/f2e6bcad31a238bdc4c3da481032bb7aa6899dcb))

* ci: :ferris_wheel: remove unnecessary tokens, force release for now ([`57ace96`](https://github.com/darvid/python-hyperscan/commit/57ace964a6b0ef38f728562166736dad74441706))


## v0.6.0 (2023-08-09)

### :sparkles:

* feat: :sparkles: manylinux_2_28 support and build improvements ([`c31c8cf`](https://github.com/darvid/python-hyperscan/commit/c31c8cfebeb2436013853ef773783fa4914841c3))

### Other

* ci: :ferris_wheel: screams in CI/CD (move release to wheels workflow) ([`755b41c`](https://github.com/darvid/python-hyperscan/commit/755b41cd1f5f99f02f63ab071e6de9b1a543435a))

* build: :package: fix versioning process for PDM build backend ([`564fd78`](https://github.com/darvid/python-hyperscan/commit/564fd78d3d740084833509c154a7186108311f4a))

* ci: :ferris_wheel: fix pyproject.toml, remove readme from dynamic ([`951b119`](https://github.com/darvid/python-hyperscan/commit/951b119b9560452901ceb7ab1e72c5339278e264))

* ci: :ferris_wheel: include semantic version in cache key ([`7ee92dc`](https://github.com/darvid/python-hyperscan/commit/7ee92dcaddb146599e6856d7564ac7145b3cdaba))

* ci: :ferris_wheel: always run setup-pdm regardless of cache ([`a08f718`](https://github.com/darvid/python-hyperscan/commit/a08f718e97ceb2fd91197a4518125f41e03b02d8))

* ci: :ferris_wheel: remove release trigger from wheels ([`c226d5f`](https://github.com/darvid/python-hyperscan/commit/c226d5faee5ee8253cf5b4216a8e41fb52a12474))

* ci: :ferris_wheel: remove conditional pypi+github publishing ([`ead1d86`](https://github.com/darvid/python-hyperscan/commit/ead1d86ba766d77e20a7d784aad33c709807f5ae))

* ci: :ferris_wheel: change release CI to run on pull_request_target ([`7e23067`](https://github.com/darvid/python-hyperscan/commit/7e23067e4d1378e5aa3856c194a11216ed379de3))

* build: :package: revert to using semantic-release instead of setuptools-scm ([`93d6a9e`](https://github.com/darvid/python-hyperscan/commit/93d6a9eeb4ba1df4f84493a3dc8d00561c356de6))

* ci: :ferris_wheel: remove branch param from download-artifact step in release ([`8225878`](https://github.com/darvid/python-hyperscan/commit/82258783eb790006b2e045a1018ff13b07738ccb))

* ci: :ferris_wheel: remove unnecessary AUDITWHEEL_PLAT, fix CI ([`22b5676`](https://github.com/darvid/python-hyperscan/commit/22b5676bc887706b3666c62bf5f077b45656e7ae))

* build: :package: run semantic-release with no-commit ([`4614285`](https://github.com/darvid/python-hyperscan/commit/4614285f6682da448bcab4b05e24a730c7682a7f))

* ci: :ferris_wheel: add step to organize dist for pypi release ([`9921c97`](https://github.com/darvid/python-hyperscan/commit/9921c97232d50f7c4b49ecb3fa1b41142cfbcf1a))

* ci: :ferris_wheel: pass strip-dev to setuptools-scm when determining version ([`21dcfb5`](https://github.com/darvid/python-hyperscan/commit/21dcfb51a29282a22223469cd2486d1534033aa3))

* ci: :ferris_wheel: fix release pipeline *again* ([`b166870`](https://github.com/darvid/python-hyperscan/commit/b166870bfcb7a1049f53f10f81a4f25acca187c4))

* build: :package: fix version detection with setuptools-scm ([`84a80e9`](https://github.com/darvid/python-hyperscan/commit/84a80e9f6976d5b49be91eb6813bf873e70c67ef))

* ci: :ferris_wheel: fix release version detection again ([`24cfaa2`](https://github.com/darvid/python-hyperscan/commit/24cfaa2debebd1c5fe55081cd8c0b7fa39370a95))

* ci: :ferris_wheel: fix release trigger in CI ([`fdcfb23`](https://github.com/darvid/python-hyperscan/commit/fdcfb23128c3c9c829ac2afcb84a791a0e2c5cda))

* ci: :ferris_wheel: more CI fixes ([`5c36436`](https://github.com/darvid/python-hyperscan/commit/5c364365cb8347bde36b7d61a58b0e38b606fd00))


## v0.5.0 (2023-08-08)

### :bug:

* fix(serialization): :bug: fix memory leak in Database.dumpb ([`80b5834`](https://github.com/darvid/python-hyperscan/commit/80b58346837e4477b15e394e664e9dc789c3e7b9))

* fix(serialization): :bug: add mode parameter to Database.loadb ([`8e4104e`](https://github.com/darvid/python-hyperscan/commit/8e4104e5fc648eb0ad9ab29166e83f14ed0f87b6))

### :lipstick:

* style: :lipstick: lint ([`ed66d82`](https://github.com/darvid/python-hyperscan/commit/ed66d828b8080045094fa30ec9175dada24f3257))

### Other

* ci: :ferris_wheel: add credentials so semantic release can push to main ([`7041fd2`](https://github.com/darvid/python-hyperscan/commit/7041fd2d0515039716c9f51d8d01efd95eb96eb1))

* build: :package: remove semantic release build_command ([`e233699`](https://github.com/darvid/python-hyperscan/commit/e23369950cf75561d1914735dd61ce85d362a653))

* ci: :ferris_wheel: fix fetch-depth in wheels CI ([`ba2dc6a`](https://github.com/darvid/python-hyperscan/commit/ba2dc6a9b3327b64b5a7b3d3404a15a301e72ec1))

* build: :package: fix commit_parser config for semantic release ([`3f898b2`](https://github.com/darvid/python-hyperscan/commit/3f898b2c5d7416f97906b7c814712105ba9f1665))

* build: :package: prevent semantic release from writing version variable(s) ([`1fdc446`](https://github.com/darvid/python-hyperscan/commit/1fdc446fccb438184039bae523ce2e245fecd5de))

* build: :package: fix python-semantic-release configuration ([`6b2c55c`](https://github.com/darvid/python-hyperscan/commit/6b2c55ca0c4f691d6efae517c05895998485eace))

* ci: :ferris_wheel: update release CI/CD ([`a100b29`](https://github.com/darvid/python-hyperscan/commit/a100b29c46c0f3ac05a0047c59a0543500ec80c6))

* docs: :memo: update README ([`87560ae`](https://github.com/darvid/python-hyperscan/commit/87560aebeadf95a785e57f1ab6ca3d9bbc1df846))

* build: :package: update python-semantic-release configuration ([`68695ce`](https://github.com/darvid/python-hyperscan/commit/68695ce7111fd6e84f3d25f6a54ff7efa4d7d703))

* build: :package: fix macos build, update manylinux images ([`4ee4e0d`](https://github.com/darvid/python-hyperscan/commit/4ee4e0d59938dcd025d18fbd631155545ce609e7))

* build: :package: fix repo/build paths in macos build ([`d8016c9`](https://github.com/darvid/python-hyperscan/commit/d8016c9832ba9a1644bf09d1eef85ef178444980))

* ci: :ferris_wheel: fix typo for musllinux builds ([`857ffa6`](https://github.com/darvid/python-hyperscan/commit/857ffa6e074a408be9aa3e717d29aac98643784f))

* ci: :ferris_wheel: fix build ([`bb63592`](https://github.com/darvid/python-hyperscan/commit/bb63592cc3ab785298c56d3d778bf17585f6dd3b))

* ci: :ferris_wheel: fix musllinux platform ids and invoke semantic-release manually ([`472bc99`](https://github.com/darvid/python-hyperscan/commit/472bc99307e54028cda10be7d560db7c75e30e65))

* build: :package: fix build ([`d73d9c1`](https://github.com/darvid/python-hyperscan/commit/d73d9c167e5095ee091d40bfb13694fd0aa589bf))

* build: :package: update manylinux-hyperscan image ([`7a856f6`](https://github.com/darvid/python-hyperscan/commit/7a856f686ae9cc3c04168058d912aab5c2081c95))

* build: :package: fix before_test pdm installation ([`eaf157c`](https://github.com/darvid/python-hyperscan/commit/eaf157ccf0c8beb1bc9a6ad1100bf601f154f012))

* build: :package: update manylinux-hyperscan image ([`df2a796`](https://github.com/darvid/python-hyperscan/commit/df2a79614d7c0eb722e91bf5788fb7a372d74b03))

* build: :package: remove pdm post build script ([`5e4815c`](https://github.com/darvid/python-hyperscan/commit/5e4815c9eee9dbbc3902e394cce812517f2ea056))

* ci: :ferris_wheel: include develop branch in wheels workflow ([`4417e5f`](https://github.com/darvid/python-hyperscan/commit/4417e5f8c06c43a4832d9b1299672d0c78730e1c))

* refactor: :recycle: tooling, packaging, and CI/CD overhaul, and update to Hyperscan 5.4.2 ([`1b823c9`](https://github.com/darvid/python-hyperscan/commit/1b823c9c99af672e5af23825849e8da8cc102101))

* 🚧 build: rename build.py to avoid conflicts with python package 'build' ([`056538c`](https://github.com/darvid/python-hyperscan/commit/056538c120e547afaa914404262e73c968bbe51e))

* Fix missing error constants

hyperscan.HS_INVALID and other error constants are no longer available.
Calling ADD_INT_CONSTANT() inside ADD_HYPERSCAN_ERROR() expands `hs_err`
too early, causing ADD_INT_CONSTANT() to declare the *value* of the
constant instead of its name.

Duplicate the content of ADD_INT_CONSTANT() to fix this. ([`5eddcc9`](https://github.com/darvid/python-hyperscan/commit/5eddcc9fa4231d48dba3ef01d849a689ca499f41))

* 🚧 build: updates to support building with Poetry 1.4+, support both static and dynamic linking ([`90fd238`](https://github.com/darvid/python-hyperscan/commit/90fd23851a38f2059a5e78c772ef0ce1c8ed1497))


## v0.4.0 (2023-01-11)

### Other

* 🛸 ci: update release pipeline ([`4f70032`](https://github.com/darvid/python-hyperscan/commit/4f7003273267f7dff0877ad28157a63c34e17960))

* 🎉 feat: add typing stubs and update tests ([`60a7324`](https://github.com/darvid/python-hyperscan/commit/60a7324b489a7dac27095311bfb34a9b07e480c3))

* 🐛 fix: fix memory leak in loadb (#46) and minor doc tweaks ([`71b6462`](https://github.com/darvid/python-hyperscan/commit/71b64627e940c1e34ad8dde18f7b5800c113c25a))


## v0.3.3 (2023-01-06)

### :bug:

* :bug: fix memory leak (#46), improve error handling (#41), drop support for Py3.6 ([`297519e`](https://github.com/darvid/python-hyperscan/commit/297519ed5c632bab9477d460d78dfc0ff5291ac1))

### Other

* 🛸 ci: remove release asset pushing to GH for now ([`5943a6e`](https://github.com/darvid/python-hyperscan/commit/5943a6e0b7708af54ec9febce0e57dcc7ae5fa68))

* 🛸 ci: add version tags to event trigger ([`eb7db91`](https://github.com/darvid/python-hyperscan/commit/eb7db91ade9624b3795dc7917318ff848f4e9c72))

* 🛸 ci: add ruff, remove isort, update GH actions workflow ([`87ed0b6`](https://github.com/darvid/python-hyperscan/commit/87ed0b69110e7d8ae9b743ae40962fdc646a1a9b))

* :pencil: CHANGELOG cleanup ([`e698037`](https://github.com/darvid/python-hyperscan/commit/e69803712adb27ce35aa979c35f51ee333c2a89f))

* :construction_worker: use new secret for Github releases ([`6ddb575`](https://github.com/darvid/python-hyperscan/commit/6ddb57592d481183274ee098cf6e9b8bb9e368b7))

* :construction_worker: add back poetry build ([`9890686`](https://github.com/darvid/python-hyperscan/commit/9890686ffcf964846f6e92a5b3636b44df09299e))

* :construction_worker: set git safe.directory to * ([`a10483f`](https://github.com/darvid/python-hyperscan/commit/a10483f0f87ed347c20b0a310974e7b9ffcf4c25))

* :construction_worker: set version_pattern for semantic-release ([`c7165b2`](https://github.com/darvid/python-hyperscan/commit/c7165b25986d71695bd857e98ad27bb038d18774))

* :construction_worker: set version_source to tag for semantic-release ([`e0df50c`](https://github.com/darvid/python-hyperscan/commit/e0df50cf1f7d305f6d5b6c70106c04736c7ed426))

* :construction_worker: update actions/checkout across all jobs ([`cc8f7e5`](https://github.com/darvid/python-hyperscan/commit/cc8f7e50e028f59ba23ce048ed0a34f1d588f16e))

* :construction_worker: update actions/checkout and set fetch-depth to 0 ([`806bf55`](https://github.com/darvid/python-hyperscan/commit/806bf55d491d2a6a5f817f78f56bed12233a76bf))

* :package: update semantic-version configuration ([`8da7b6b`](https://github.com/darvid/python-hyperscan/commit/8da7b6b1afc2116372ef90ee25bd12854a62ddb9))

* :construction_worker: update manylinux-hyperscan Docker path ([`12cbe1c`](https://github.com/darvid/python-hyperscan/commit/12cbe1cfeae79a7c1555e66a7f8f7e875f36ced6))


## v0.3.2 (2022-06-13)

### Other

* 🐛 fix: hotfix for static linking ([`a2a18a1`](https://github.com/darvid/python-hyperscan/commit/a2a18a1ed2efbdef0613ad7167dcaa96f997f627))


## v0.3.1 (2022-06-13)

### Other

* 🛸 ci: add cz emoji to semantic-release configuration ([`6ea4e7d`](https://github.com/darvid/python-hyperscan/commit/6ea4e7dbce8379a932d95555a15e08f2aa78ecda))

* 🐛 fix(build): prevent pcre from being passed to distutils.Extension libraries ([`de2ad84`](https://github.com/darvid/python-hyperscan/commit/de2ad84a999b5c80b162f05d28f0cb5c28442dc1))

* 🚧 build: remove -fPIC, dedupe libraries ([`eaab1da`](https://github.com/darvid/python-hyperscan/commit/eaab1da538600264a6cb0113e61eef607475832f))

* 📜 docs: updated README for v0.3.0 ([`a7dff1d`](https://github.com/darvid/python-hyperscan/commit/a7dff1d9b53822d8cbe473839b5a86761e48f0ad))


## v0.3.0 (2022-04-11)

### :sparkles:

* :sparkles:  initial Chimera support (and upgrade to Hyperscan v5.4.0)

Closes #32 ([`0527aac`](https://github.com/darvid/python-hyperscan/commit/0527aac060f29413a727e7fbb8db2c456bd25aab))

### Other

* 🛸 ci: update release conditionals to include tagged refs ([`9e48488`](https://github.com/darvid/python-hyperscan/commit/9e484887f49fa272616c37db5aaa35caf84441a8))

* 🛸 ci: switch action provider for GitHub releases ([`08cf91d`](https://github.com/darvid/python-hyperscan/commit/08cf91df0a807fd3a95d7915c6c5de1fe8e32c37))

* 🚧 build: change commit_parser for semantic_release to emoji_parser ([`518782e`](https://github.com/darvid/python-hyperscan/commit/518782eb7f3ab24eaffeb76631451904309e161c))

* 🛸 ci: add python 3.10 to CI ([`06e25bc`](https://github.com/darvid/python-hyperscan/commit/06e25bcc96151428c45fe66f4a0b03d8662c9bf1))

* 📜 docs: update docs to include Chimera requirements ([`5ad70dc`](https://github.com/darvid/python-hyperscan/commit/5ad70dc4f69ba75a289f83ec007b334eac8f4ec3))

* 🚧 build: dependency upgrades ([`db2d264`](https://github.com/darvid/python-hyperscan/commit/db2d2645ab38a5d74a0cb9edb4466d79ac3a0462))

* 🚧 build: changes to build.py to support static linking hyperscan ([`e08c883`](https://github.com/darvid/python-hyperscan/commit/e08c883b244b69c6c0c5206cc2bae70df08eb3c8))

* 🚧 build: update package classifiers to include Python 3.10 ([`63aacd0`](https://github.com/darvid/python-hyperscan/commit/63aacd03e5c87f963b899020cd0f37ef8e5b6ee8))

* 🐛 fix: drop single-sourcing package version (#38) ([`357cce1`](https://github.com/darvid/python-hyperscan/commit/357cce1d1a7476043d925834d8e6d1ac86c10a39))

* style: clang-format ([`7d50f8a`](https://github.com/darvid/python-hyperscan/commit/7d50f8a1368b286ea025d877ab23119cd4c65830))

* 💚 correct clone and build issues (#37)

* - Fix scratch cloning
- Ensure static linking for stdc++

* Update compile error message to include expression id

Co-authored-by: Ken Shelton <kshelton@randori.com> ([`a8d926f`](https://github.com/darvid/python-hyperscan/commit/a8d926f58b3bb82dac8e49a5f08580c856c0a5cb))

* :green_heart:  fixed PCRE_PATH ([`a175f28`](https://github.com/darvid/python-hyperscan/commit/a175f289393aaf858751a4f02842d8b9f9dd9ef4))

* :green_heart:  bump manylinux container image version ([`8962139`](https://github.com/darvid/python-hyperscan/commit/89621390a2fd852cdf31dfc32d5bda205164e43b))

* :green_heart:  reverted upgraded Python requirement in pyproject.toml ([`ae1e89d`](https://github.com/darvid/python-hyperscan/commit/ae1e89dbd594792e7822b343cd7190cac971513c))

* :green_heart:  removed cp310, thought CI was using manylinux, d'oh ([`7680840`](https://github.com/darvid/python-hyperscan/commit/768084059a897f8faf08360c94f04672b57a8739))

* fix project homepage (#34) ([`1d95aea`](https://github.com/darvid/python-hyperscan/commit/1d95aeab4d467e99abf7172de83df710e9b8a868))

* docs: update README with correct libhs version requirement ([`a6ccc92`](https://github.com/darvid/python-hyperscan/commit/a6ccc921b17096103314bd4ff4561f4e6c8fa549))

* docs: add usage examples for 0.2.0 features ([`8d9ecfd`](https://github.com/darvid/python-hyperscan/commit/8d9ecfdd5fc1b9d07f81d32c8f9f581df8f68dba))

* docs: update CHANGELOG with 0.2.0 ([`57a4879`](https://github.com/darvid/python-hyperscan/commit/57a487954dc167fac94f07aa7a83a863dc20e172))

* ci(github): add semantic-release publish action back ([`1bbb12d`](https://github.com/darvid/python-hyperscan/commit/1bbb12d757a0718bc8d269846f31f9f4e1043d45))

* docs: add GitHub actions workflow name and badge ([`eb57444`](https://github.com/darvid/python-hyperscan/commit/eb574445ba8fdd3df91d28227fb3fdb9942e7ba6))


## v0.2.0 (2020-12-12)

### Other

* ci(github): update release workflow to work around semantic-release limitations ([`6975583`](https://github.com/darvid/python-hyperscan/commit/6975583d3fc121af305378a5208dfa8fab2d1e79))

* fixup! ci(github): fix semantic_release.build_command and make build job depend on lint ([`88b63e3`](https://github.com/darvid/python-hyperscan/commit/88b63e3db948282f3e9716e6fdc7434c4ab34d03))

* fixup! ci(github): fix semantic_release.build_command and make build job depend on lint ([`1827bed`](https://github.com/darvid/python-hyperscan/commit/1827bed1f2b38c3eb9940c44c8cd76cebd576259))

* build: add setuptools to build requirements (#27) ([`6cb282e`](https://github.com/darvid/python-hyperscan/commit/6cb282edbfde85b05b100b3923a8fd1d43077553))

* ci(github): fix semantic_release.build_command and make build job depend on lint ([`2d66762`](https://github.com/darvid/python-hyperscan/commit/2d667626d0a04b7218e70bce0e0c4a8784d9c82b))

* build: update semantic_release.build_command to pip install rather than rebuild wheel(s) ([`2a5e6d5`](https://github.com/darvid/python-hyperscan/commit/2a5e6d5a0ceba47151961e4019e7639769d9bca1))

* ci(github): add GitHub Actions workflow ([`46b2a52`](https://github.com/darvid/python-hyperscan/commit/46b2a52ec7b461e2541614c9e1c9ce5608add0fe))

* feat: add HS_FLAG_COMBINATION and HS_FLAG_QUIET (#21) ([`9804a55`](https://github.com/darvid/python-hyperscan/commit/9804a5563ee8cfa3e766ceb50481031f73a59009))

* feat: support extended behavior for patterns (#11) ([`5bfc947`](https://github.com/darvid/python-hyperscan/commit/5bfc94765752b3a6fcc424fd58bedb2ca36ee0cc))

* feat: support hs_scan_vector (#24) ([`6869b06`](https://github.com/darvid/python-hyperscan/commit/6869b0687a4efb5a8f4028c68b64a876f7d950fb))

* fix: dynamically allocate memory for arrays used by Database.compile (fixes #25) ([`3fe93fc`](https://github.com/darvid/python-hyperscan/commit/3fe93fc81578dd627991473e3f89dc822523578c))

* style: update clang-format configuration and reformat hyperscanmodule.c ([`d34141f`](https://github.com/darvid/python-hyperscan/commit/d34141ff48dd7a77cde4d2b73b162264b94fb320))

* build: removed requirements.txt from tests ([`5d7ff97`](https://github.com/darvid/python-hyperscan/commit/5d7ff9768baa1d5106388a1b9f2300200fd1e23d))

* style(hsbench.py): lint with black ([`1c4c18b`](https://github.com/darvid/python-hyperscan/commit/1c4c18be6bb35c0695540d9cf85621326bc03ba2))

* style(docs): added missing newline to usage.md ([`810f7b4`](https://github.com/darvid/python-hyperscan/commit/810f7b4c0b0b6ff8b938f28c757a158266389ede))

* build: fix semantic_release version configuration, add test deps to poetry ([`5a570ec`](https://github.com/darvid/python-hyperscan/commit/5a570ec8c67e3c06be8afc452e0405dedcf58c60))

* build: add build_command to semantic_release config ([`8e2d59a`](https://github.com/darvid/python-hyperscan/commit/8e2d59ac9f0de7133a310815fcab97842189c57a))

* build: remove python 3.5 support, update dev dependencies ([`fabd9c9`](https://github.com/darvid/python-hyperscan/commit/fabd9c99628c526014d8c3147d15b180a4524f4f))

* refactor: change directory structure to use src dir ([`af2818a`](https://github.com/darvid/python-hyperscan/commit/af2818ad6c3e10c9984907aca7ad8345eae461e1))

* style: add .markdownlint.json ([`42c059f`](https://github.com/darvid/python-hyperscan/commit/42c059fb9f83995984277d8f5592824bc74ad2b2))

* style: add pre-commit and python-semantic-release ([`614c178`](https://github.com/darvid/python-hyperscan/commit/614c1784faa374cfe585286b1b2197a9d0053651))

* style: add commitizen ([`42f9d97`](https://github.com/darvid/python-hyperscan/commit/42f9d97b0d5b6c24bc7413a678801b36bc16faa3))

* style: add clang-format configuration file ([`6d85da4`](https://github.com/darvid/python-hyperscan/commit/6d85da457013a16a863cc152179679e6af11b42f))


## v0.1.5 (2020-02-22)

### Other

* remove wheelhouse caching from CI tests ([`c26b0a8`](https://github.com/darvid/python-hyperscan/commit/c26b0a8e1b4d445acf498ee61ea9d16dcc6699ab))

* zipp y u do dis ([`ce8e139`](https://github.com/darvid/python-hyperscan/commit/ce8e1399798f9fe79b9a3b26f4c19e16d1903bb6))

* handle zipp take two ([`d0d4b49`](https://github.com/darvid/python-hyperscan/commit/d0d4b49360908ce93b80d00a06a6a63bfbcf0f8f))

* pinn zipp<2 in test requirements to avoid pip install failures ([`981e962`](https://github.com/darvid/python-hyperscan/commit/981e962d11d4a6272117e477268c27039f95c95b))

* fixed poetry build command in semaphore build script ([`97b09ee`](https://github.com/darvid/python-hyperscan/commit/97b09ee7dc44a2cebefc2876de9e0dfcd1deb805))

* fixed merge fail artifact ([`57c898f`](https://github.com/darvid/python-hyperscan/commit/57c898f326175efe6d18ab19c84cca5c8c4bf02c))

* Merge branch 'master' of github.com:darvid/python-hyperscan into develop ([`62b32f6`](https://github.com/darvid/python-hyperscan/commit/62b32f6aa29c1906c9ef142392a58a6b233b1c4c))

* Add missing tune families (#19)

Hyperscan 5.2.1 comes with additional tune families. 

https://intel.github.io/hyperscan/dev-reference/api_constants.html#cpu-tuning-flags ([`6c3f49d`](https://github.com/darvid/python-hyperscan/commit/6c3f49d47f8f5f766a74d08e0ec2fce9915c49e1))

* Release v0.1.4 (#17)

* added Hyperscan 5.2.0 support and literal API (#16) ([`86a1b4e`](https://github.com/darvid/python-hyperscan/commit/86a1b4e83e80f14d87c97e66fdb1f3e8b4f57411))

* updated CHANGELOG, bumped version to 0.1.5, drop Python 3.4 from classifiers and require 3.5 with poetry ([`75dd919`](https://github.com/darvid/python-hyperscan/commit/75dd919019ad6fd370c35a3b62b60dd51b2a3ef3))

* added .markdownlint.json config file ([`73b3b72`](https://github.com/darvid/python-hyperscan/commit/73b3b7294564463db822dd876f15bbe5fe386b8d))

* updated tox.ini to use poetry instead of pip ([`2a18328`](https://github.com/darvid/python-hyperscan/commit/2a18328b1abd77e47955b1e9c930328bba97eddc))

* fixed reference leak (#18), and renamed loads/dumps to loadb/dumpb ([`8695500`](https://github.com/darvid/python-hyperscan/commit/86955004428889731a6eb24e27574ffa2857b8d4))


## v0.1.4 (2019-11-07)

### Other

* updated changelog ([`9b80c67`](https://github.com/darvid/python-hyperscan/commit/9b80c675207d8c9f3c02cbb6c97792644387bc40))

* fixed run_tests script ([`3df178c`](https://github.com/darvid/python-hyperscan/commit/3df178c61bbdd24a1ad1a80985e6ef4163c781f5))

* and again ([`924026d`](https://github.com/darvid/python-hyperscan/commit/924026d8edcb744611350d5f4872a993e9691497))

* and again ([`ea3d0c4`](https://github.com/darvid/python-hyperscan/commit/ea3d0c4ecca9ce8b17096f346584c2fbd21ea7d0))

* fixed build.py again ([`acceabe`](https://github.com/darvid/python-hyperscan/commit/acceabe567616b48fc359be7a80cb4df35e4e15c))

* use std c99 and fixed bug in build.py ([`498d840`](https://github.com/darvid/python-hyperscan/commit/498d840ffd2d0282f2c2deb0f024ffaf9704c839))

* removed -O0 from extra_compile_args ([`c7297bf`](https://github.com/darvid/python-hyperscan/commit/c7297bfb7927adf4a55fca7b9ed61b81c1d11c31))

* dropping python 2.7 support ([`cf5d570`](https://github.com/darvid/python-hyperscan/commit/cf5d5708fc3186a5bce9105a490a0580b69130fa))

* second attempt at fixing encoding error in build ([`2fc95f3`](https://github.com/darvid/python-hyperscan/commit/2fc95f3c74110723239deff088cfbb00054facf8))

* add LC_ALL env var to build ([`e865531`](https://github.com/darvid/python-hyperscan/commit/e8655312e0e725004a362b277ce4933e8b0d3052))

* fixed syntax error in build.py introduced by black ([`66f38a7`](https://github.com/darvid/python-hyperscan/commit/66f38a756e4f3b20c22ca869dec16e560f0dc53c))

* use docker hub image for manylinux-hyperscan again, pinned to latest ([`4a67c24`](https://github.com/darvid/python-hyperscan/commit/4a67c244a2653d16c0e43473c70537fd09d61c35))

* added Hyperscan 5.2.0 support and literal API (#16) ([`4e02072`](https://github.com/darvid/python-hyperscan/commit/4e02072a33e7a3124e65cca650802541f13f3ff1))


## v0.1.3 (2019-08-04)

### Other

* release 0.1.3 ([`f85436d`](https://github.com/darvid/python-hyperscan/commit/f85436d65c2ed25cae56abdf2de8361b108f84c3))

* Handle exceptions in callback. (#15) ([`f1b92be`](https://github.com/darvid/python-hyperscan/commit/f1b92bedb751c4492d29a9e793a27c7941ff6f57))


## v0.1.2 (2019-06-11)

### Other

* delete semaphore caches in CI builds ([`1ebabf6`](https://github.com/darvid/python-hyperscan/commit/1ebabf6167e3b5881f469eb6c57001b63c40ef42))

* drop Python 3.4 support from CI ([`404c8bc`](https://github.com/darvid/python-hyperscan/commit/404c8bc7644b1abc2e2079e4ca50c2620b1c5693))

* release 0.1.2 ([`0d6bf1d`](https://github.com/darvid/python-hyperscan/commit/0d6bf1d6e5289213ad374d276efc6ba996089b7c))

* Release the GIL when compiling patterns (#13)

This will allow other Python threads to run while Hyperscan is compiling
patterns.

Fixes #12. ([`e2e5b50`](https://github.com/darvid/python-hyperscan/commit/e2e5b506e6ea8c3736c4524868a829622369971b))


## v0.1.1 (2019-05-02)

### Other

* remove cache clear from test epilogue ([`efbc82e`](https://github.com/darvid/python-hyperscan/commit/efbc82ef213bbf0c19d522b691e74e04d90635cf))

* fix caching issue leaving old wheels in semaphore CI build environments ([`ad8a7e8`](https://github.com/darvid/python-hyperscan/commit/ad8a7e84fb98631951b2b7387549c53fd0455787))

* fixed segfault in Stream.scan (#10) ([`e6a5b1a`](https://github.com/darvid/python-hyperscan/commit/e6a5b1ae5013c804b1f75aa4e0fdffbe55e6996d))

* bumped package version ([`d08d00b`](https://github.com/darvid/python-hyperscan/commit/d08d00b59d6b8a0fc06ead303a7f9b10db723a7e))

* updated CHANGELOG for 0.1.1 release, with fix for #10 ([`ce01460`](https://github.com/darvid/python-hyperscan/commit/ce01460b8a3bb09847cca017bc6ee9dfbdf741b0))

* fixed segfault in Database.scan due to misplaced Py_END_ALLOW_THREADS ([`cc82ec6`](https://github.com/darvid/python-hyperscan/commit/cc82ec690d801bab8b4ca7a60b78d28b61140671))

* ensure GIL state in Hyperscan error handler ([`440fee2`](https://github.com/darvid/python-hyperscan/commit/440fee2a6d8d127fa263eefe8a432e54bf6245b5))

* Update README.md, fixed docs URL (#9)

Change documentation's URL to use readthedocs.io domain ([`d7e7572`](https://github.com/darvid/python-hyperscan/commit/d7e757258d3f9fe61d5b829f98544bd3db3efe5c))

* fixed docs link in README ([`c5ceea2`](https://github.com/darvid/python-hyperscan/commit/c5ceea28835f4aa2ce25da773dabc751f27e3ead))

* pin mkdocs ([`5b6a77a`](https://github.com/darvid/python-hyperscan/commit/5b6a77a1a85457ac859e477221f953706f8fc5b4))

* fixed rtd configuration ([`0ede04c`](https://github.com/darvid/python-hyperscan/commit/0ede04cd946f9514f22faaa15c4f6da46b1b8482))

* added docs requirements.txt to rtd configuration ([`809a603`](https://github.com/darvid/python-hyperscan/commit/809a6036c8649984af44939c7e83871fc3373de3))

* fixed mkdocs.yml ([`8765e71`](https://github.com/darvid/python-hyperscan/commit/8765e71ae13d43d0bf9cee6ea8c286849e8a8bdc))

* Release v0.1.0 (#7)

* initial release 0.1.0 changes

* make .semaphore/*.sh executable

* fixed build_wheels script

* added checkout to prologue in semaphore configuration

* fixed build_wheels script to avoid issue with cached Poetry venv for py27

* attempting to fix wheel name for cp27mu

* fixed syntax error in build_wheels script

* fixed a derp in build_wheels again

* fixed run_tests to work with cp27mu

* moved README.rst to README.md, added additional package meta

* added CHANGELOG

* added GitHub releases deployment pipeline

* fixed syntax error in ghr-deploy semaphore config

* added missing checkout to ghr-deploy Semaphore config

* fixed auto-promotion and attempt to fix ghr-deploy again

* added secrets configuration

* fixed PyPI deployment

* fixed path to .pypirc

* fixed path to built wheels for PyPI upload ([`16eb091`](https://github.com/darvid/python-hyperscan/commit/16eb091161e813f086a74c18bf7657614cbe364d))

* pinned requests and urllib3 to test_requirements per security vuln advisories ([`7cec3a8`](https://github.com/darvid/python-hyperscan/commit/7cec3a8c835e0e9ebee503b1e05093154a1b7253))

* allow halting in stream scanning (#4) ([`9823622`](https://github.com/darvid/python-hyperscan/commit/98236220135a67c1ab494e4821638bf79cbe9658))


## v0.0.2 (2018-05-26)

### Other

* attempting to copy audited wheels from wheelhouse instead of symlinking ([`e0f85be`](https://github.com/darvid/python-hyperscan/commit/e0f85beacfa60663f43ef1654e17af236c1f79a6))

* attempt to fix permissions take 2 ([`9cb0c68`](https://github.com/darvid/python-hyperscan/commit/9cb0c687513b77a719b5dfc5b8897af89ea60e92))

* attempt to fix travis permission issues when deploying to PyPI ([`54b5a5e`](https://github.com/darvid/python-hyperscan/commit/54b5a5e6915de4b0cf61a48d44b108bbb1e7880d))

* added skip_cleanup to deploy configuration, and attempt to remove .eggs after manylinux build ([`3397967`](https://github.com/darvid/python-hyperscan/commit/339796728e6ea26f182660e1e7e730194d1ec94d))

* added more robust pkg-config handling in setup.py ([`dc580a9`](https://github.com/darvid/python-hyperscan/commit/dc580a9cec0eb6bed28b62865599cc5aabb3aaa8))

* fixed docs requirements ([`35398d2`](https://github.com/darvid/python-hyperscan/commit/35398d24e1af3aa4487b5c47d76e245d576f5d22))

* pin sphinx and sphinx-md-theme versions ([`b232794`](https://github.com/darvid/python-hyperscan/commit/b232794990164bcf6b7082b013cddab506975b03))

* added sphinx to test requirements ([`64a33dd`](https://github.com/darvid/python-hyperscan/commit/64a33dd881acda168971ba9d5e9e9cf55d91bd91))

* added skipsdist to tox configuration ([`3e54d1a`](https://github.com/darvid/python-hyperscan/commit/3e54d1a0d3da872eadf0f6fadac22a757a741b45))

* added skip_install to tox.ini ([`1bae8b5`](https://github.com/darvid/python-hyperscan/commit/1bae8b5c6ec86f6ddc0a66fd0d131327b8d53100))

* re-add language to travis configuration ([`f88446b`](https://github.com/darvid/python-hyperscan/commit/f88446be0a0f22818f5b0113863c565d24a09908))

* sudo required for test stage ([`ab2c0b0`](https://github.com/darvid/python-hyperscan/commit/ab2c0b02e71f54eaf5df211c2366f41e2a756f2e))

* create dist directory in build_wheels script ([`a663b5c`](https://github.com/darvid/python-hyperscan/commit/a663b5ce3a2c4f1116d05d11a16f3fa55ee70474))

* change working directory for manylinux docker run in CI ([`0167738`](https://github.com/darvid/python-hyperscan/commit/0167738a18e0de9d1196fed374df26c780e51cfa))

* make build_wheels.sh executable ([`6eb2154`](https://github.com/darvid/python-hyperscan/commit/6eb2154d77d791e4e6826124e9525def7dcb8420))

* added manylinux builds ([`449eba6`](https://github.com/darvid/python-hyperscan/commit/449eba6bbf2735feb2985664c63ed657be1b031a))

* fixed pypi creds? ([`a321e7a`](https://github.com/darvid/python-hyperscan/commit/a321e7a52cd65dcb17a62615c1820a16fce9ab85))

* witchcraft ([`aaf0e0b`](https://github.com/darvid/python-hyperscan/commit/aaf0e0bf929ffddc202231289c35636c5e2283ff))

* remove branch requirement from deploy stage ([`2ba7623`](https://github.com/darvid/python-hyperscan/commit/2ba7623b6c17fa3445066d511940ce5a33e82cc1))

* travis pls ([`0cf50e6`](https://github.com/darvid/python-hyperscan/commit/0cf50e6d58a30d78a5da666db14cffd8f10365d5))

* attempt to fix deployment trigger ([`2b05a74`](https://github.com/darvid/python-hyperscan/commit/2b05a74ff83b3837039830bb7bc3fdd7ba5a7d73))

* disable boost python, take 2 ([`3287d56`](https://github.com/darvid/python-hyperscan/commit/3287d569741eee6258959c3fdcb61fe8a01dd512))

* disable boost python ([`6675d16`](https://github.com/darvid/python-hyperscan/commit/6675d16d3d23ed53ca54a2a1f30ed240349bf344))

* Merge tag 'v0.0.2' into develop

Initial release. ([`25cb552`](https://github.com/darvid/python-hyperscan/commit/25cb55289d92a982b6e3bec26d888424596c9fff))

* Merge branch 'release/0.0.2' ([`3e9ba89`](https://github.com/darvid/python-hyperscan/commit/3e9ba8944eac5d9543c72acba63cfca3ea46c367))

* use python-simplified-semver version scheme for setuptools_scm ([`5aaebf3`](https://github.com/darvid/python-hyperscan/commit/5aaebf3b110985039a2d06ef9e9a98a612afbb04))

* trigger deploy stage on tagged commits ([`d66ff33`](https://github.com/darvid/python-hyperscan/commit/d66ff3373ab0870984ecab111fa09972f375151f))

* trigger deploy stage on master, not release branches ([`37ff01f`](https://github.com/darvid/python-hyperscan/commit/37ff01f4d9f8bf68ff86cf38974091f89191a799))

* removed pypy from CI matrix and added whl to gitignore ([`fcab64a`](https://github.com/darvid/python-hyperscan/commit/fcab64a3090811793d8c82452156801ec36202e4))

* unfortunately Travis doesn't support per-stage matrix expansion yet ([`86801da`](https://github.com/darvid/python-hyperscan/commit/86801daea6a8d2d36d844002251504ddfc4d5302))

* added PyPI deployment to Travis configuration ([`78224ce`](https://github.com/darvid/python-hyperscan/commit/78224cec1b2a8588b29d3e587920585f255d45ae))

* attempt to sudo install boost ([`2837ca6`](https://github.com/darvid/python-hyperscan/commit/2837ca6327c0ac180af559555ef767ec5a4efb8b))

* set cores to 4 for both boost and hyperscan builds ([`cec0c1a`](https://github.com/darvid/python-hyperscan/commit/cec0c1a7e36ba56f7471fae3aca2e4c5b97e2924))

* attempt to move python matrix into each build stage ([`2aff1ba`](https://github.com/darvid/python-hyperscan/commit/2aff1ba9a7a3380a2bf1bab49b920bc0903b056f))

* removed env from stages, made boost unarchival quiet, and fixed bootstrap path ([`2a81d19`](https://github.com/darvid/python-hyperscan/commit/2a81d199346b40c26c6e214828b0279b6e03d5be))

* override script in build wheels stage ([`909813a`](https://github.com/darvid/python-hyperscan/commit/909813a3b649500f7616a1d3930c494375e2dbd3))

* reordered build stages, removed manylinux builder for now ([`b55e92c`](https://github.com/darvid/python-hyperscan/commit/b55e92ca2ff5711c1e7d17ec18f78a0b331ff573))

* attempt to build boost 1.57 in Travis ([`be5d059`](https://github.com/darvid/python-hyperscan/commit/be5d0590eb243969a82f0897db7304b2ecd06c23))

* fixed test, removed requirements dir, updated tox and travis configs ([`27bdf7e`](https://github.com/darvid/python-hyperscan/commit/27bdf7e30e35ab8333c5240ccaf5e4e673bb535d))

* fixed Database.scan broken order of parameter parsing ([`3f6ba0a`](https://github.com/darvid/python-hyperscan/commit/3f6ba0af8203553feb7bd5704d164c8d5089b614))

* added optional scratch parameter to Database.scan ([`61de465`](https://github.com/darvid/python-hyperscan/commit/61de465c40b766a2733b3860ade6775d43fdfc64))

* fixed Hyperscan compilation instructions in docs ([`5073d2b`](https://github.com/darvid/python-hyperscan/commit/5073d2b7966ef2c53e22cfc428e02e76e386a24e))

* initial commit ([`9ff31e4`](https://github.com/darvid/python-hyperscan/commit/9ff31e404bc345376be4d8f90fa5688eb1e80ff1))
