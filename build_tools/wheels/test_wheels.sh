#!/bin/bash
set -euxo pipefail

PROJECT_DIR=${1:-/project}
XDIST_WORKERS=$(python -c "import joblib; print(joblib.cpu_count(only_physical_cores=True))")
echo "👷 Number of workers: $XDIST_WORKERS"
pytest --pyargs "${PROJECT_DIR}/tests/" -n $XDIST_WORKERS -vvv
