#!/bin/bash
set -euxo pipefail

HYPERSCAN_GIT_SOURCE=${HYPERSCAN_GIT_SOURCE:-https://github.com/VectorCamp/vectorscan}
# HYPERSCAN_GIT_REF=${HYPERSCAN_GIT_REF:-vectorscan/5.4.11}
HYPERSCAN_BUILD_TYPE=${HYPERSCAN_BUILD_TYPE:-MinSizeRel}
PCRE_VERSION=${PCRE_VERSION:-8.45}

if pkg-config --validate libhs; then
  echo "✅ libhs found, nothing to do"
  exit 0
fi

# if [ -z "$HYPERSCAN_GIT_REF" ]; then
#   echo >&2 "HYPERSCAN_GIT_REF must be set"
#   exit 1
# fi

cd /tmp
# git clone -b "${HYPERSCAN_GIT_REF}" ${HYPERSCAN_GIT_SOURCE}
git clone ${HYPERSCAN_GIT_SOURCE}
brew install boost cmake git pkg-config ragel wget gnu-tar
cd $(basename ${HYPERSCAN_GIT_SOURCE})
# git checkout "24786ae"

git checkout 5145b6d

export CFLAGS="-fPIC -Wno-error"
export CPPFLAGS="-fPIC -I/usr/local/opt/binutils/include"
export PATH="$(brew --prefix gnu-tar)/bin:$PATH"
nproc=$(sysctl -n hw.logicalcpu)

# build and install PCRE (static required for Chimera)
wget -qO- https://sourceforge.net/projects/pcre/files/pcre/${PCRE_VERSION}/pcre-${PCRE_VERSION}.tar.gz/download | tar xvz
mv "pcre-${PCRE_VERSION}" pcre
cd pcre
./configure --prefix=/opt/pcre --enable-unicode-properties --enable-utf
make -j${nproc} && sudo make install
cd ..
sed -i -e 's/-Wall -Werror/-Wno-error/g' cmake/*.cmake

# build and install Hyperscan/Vectorscan
cmake \
  -B build \
  -S . \
  -DEXTRA_C_FLAGS=-Wno-error \
  -DEXTRA_CXX_FLAGS=-Wno-error \
  -DCMAKE_INSTALL_PREFIX=/opt/vectorscan \
  -DBUILD_STATIC_LIBS=ON \
  -DBUILD_SHARED_LIBS=ON \
  -DCMAKE_BUILD_TYPE=${HYPERSCAN_BUILD_TYPE} \
  -DFAT_RUNTIME=OFF \
  -DPCRE_SOURCE="${PWD}/pcre" \
  -DCMAKE_C_FLAGS="$CFLAGS" \
  -DCMAKE_CXX_FLAGS="$CPPFLAGS -D_GLIBCXX_USE_CXX11_ABI=0"
cmake --build build --parallel ${nproc}
sudo cmake --install build
