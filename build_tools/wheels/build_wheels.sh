#!/bin/bash
set -euxo pipefail

# Only applicable to macOS builds
HYPERSCAN_VERSION=${HYPERSCAN_VERSION:-v5.4.2}
HYPERSCAN_BUILD_TYPE=${HYPERSCAN_BUILD_TYPE:-Release}
PCRE_VERSION="${PCRE_VERSION:-8.45}"

if [[ $(uname) == "Darwin" ]]; then
  export MACOSX_DEPLOYMENT_TARGET="10.14"
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-/usr/local/lib}"
  export PCRE_PATH="${PCRE_PATH:-/tmp/hyperscan/pcre-${PCRE_VERSION}/.libs}"
fi

cd "${GITHUB_WORKSPACE}"
pdm run cibuildwheel --output-dir wheelhouse
