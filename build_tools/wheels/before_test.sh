#!/bin/bash
set -euxo pipefail

export PDM_HOME=${PDM_HOME:-/tmp/pdm}
export PDM_SKIP_ADD_TO_PATH=${PDM_SKIP_ADD_TO_PATH:-true}
export PATH="${PDM_HOME}/bin:${PATH}"
curl -sSL https://pdm.fming.dev/install-pdm.py | python -
pdm export -G test -f requirements -o test_requirements.txt
pip install -r test_requirements.txt
