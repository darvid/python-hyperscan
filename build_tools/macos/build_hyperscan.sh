#!/bin/bash
set -euxo pipefail

HYPERSCAN_VERSION=${HYPERSCAN_VERSION:-v5.4.2}
HYPERSCAN_BUILD_TYPE=${HYPERSCAN_BUILD_TYPE:-Release}
PCRE_VERSION=${PCRE_VERSION:-8.45}

if pkg-config --validate libhs; then
  echo "✅ libhs found, nothing to do"
  exit 0
fi

if [ -z "$HYPERSCAN_VERSION" ]; then
  >&2 echo "HYPERSCAN_VERSION must be set"
  exit 1
fi

cd /tmp
git clone -b "${HYPERSCAN_VERSION}" https://github.com/intel/hyperscan.git
brew install boost cmake git pkg-config python@3.11 ragel wget gnu-tar
cd hyperscan

export PATH="$(brew --prefix gnu-tar)/bin:$PATH"
# build and install PCRE (static required for Chimera)
wget -qO- https://sourceforge.net/projects/pcre/files/pcre/${PCRE_VERSION}/pcre-${PCRE_VERSION}.tar.gz/download | tar xvz
cd "pcre-${PCRE_VERSION}"

export CFLAGS=-fPIC
export CXXFLAGS=-fPIC
./configure --prefix=/opt/pcre --enable-unicode-properties --enable-utf
nproc=$(sysctl -n hw.logicalcpu)
make -j${nproc} && sudo make install

# build and install Hyperscan
cd /tmp/hyperscan
cmake \
  -B build \
  -S . \
  -DCMAKE_INSTALL_PREFIX=/opt/hyperscan \
  -DBUILD_STATIC_AND_SHARED=ON \
  -DCMAKE_BUILD_TYPE=${HYPERSCAN_BUILD_TYPE} \
  -DPCRE_SOURCE=/tmp/hyperscan/pcre-${PCRE_VERSION} \
  -DCMAKE_C_FLAGS="$CFLAGS" \
  -DCMAKE_CXX_FLAGS="$CXXFLAGS -D_GLIBCXX_USE_CXX11_ABI=0"
cmake --build build --parallel ${nproc}
sudo cmake --install build
