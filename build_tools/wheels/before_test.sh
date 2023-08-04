#!/bin/bash
set -euxo pipefail

curl -sSL https://pdm.fming.dev/install-pdm.py | python -
export PATH=/root/.local/bin:$PATH
pdm install -g --no-self --no-lock -p /project -G test
