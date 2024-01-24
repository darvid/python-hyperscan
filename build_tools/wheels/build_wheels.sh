#!/bin/bash
set -euxo pipefail

if [[ $(uname) == "Darwin" ]]; then
  export MACOSX_DEPLOYMENT_TARGET="10.14"
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-/usr/local/lib}"
  export PCRE_PATH="${PCRE_PATH:-/tmp/hyperscan/pcre-${PCRE_VERSION}/.libs}"
fi

cd "${GITHUB_WORKSPACE}"
pdm run cibuildwheel --output-dir wheelhouse
