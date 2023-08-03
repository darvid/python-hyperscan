#!/bin/bash
set -euxo pipefail

# Only applicable to macOS builds
HYPERSCAN_VERSION=${HYPERSCAN_VERSION:-v5.4.2}
HYPERSCAN_BUILD_TYPE=${HYPERSCAN_BUILD_TYPE:-Release}
PCRE_VERSION="${PCRE_VERSION:-10.42}"

if [[ $(uname) == "Darwin" ]]; then
  export MACOSX_DEPLOYMENT_TARGET="10.14"
  mkdir -p /tmp/hyperscan
  cd /tmp/hyperscan
  brew install boost cmake git pkg-config python@3.11 ragel wget
  wget -qO- https://github.com/PCRE2Project/pcre2/releases/download/pcre2-${PCRE_VERSION}/pcre2-${PCRE_VERSION}.tar.gz | tar xvz
  cd "pcre2-${PCRE_VERSION}"
  ./configure --prefix=/usr/local --enable-unicode-properties --enable-utf
  nproc=$(sysctl -n hw.logicalcpu)
  make -j${nproc} && make install

  cd /tmp/hyperscan
  mkdir -p build
  git clone -b ${HYPERSCAN_VERSION} https://github.com/01org/hyperscan.git
  export CFLAGS="-fPIC"
  export CXXFLAGS="$CFLAGS -D_GLIBCXX_USE_CXX11_ABI=0"
  cd build
  cmake \
    -DCMAKE_INSTALL_PREFIX=/opt/hyperscan \
    -DFAT_RUNTIME=ON \
    -DBUILD_STATIC_AND_SHARED=ON \
    -DCMAKE_BUILD_TYPE=${HYPERSCAN_BUILD_TYPE} \
    -DPCRE_SOURCE=../pcre2-${PCRE_VERSION} \
    -DCMAKE_C_FLAGS="${CFLAGS}" \
    -DCMAKE_CXX_FLAGS="${CXXFLAGS}" \
    ../
  make -j${nproc} && make install
fi

python -m pip install cibuildwheel
python -m cibuildwheel --output-dir wheelhouse
