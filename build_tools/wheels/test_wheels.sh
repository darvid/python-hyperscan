#!/bin/bash
set -euxo pipefail

# Tests in cibuildwheel are NOT run in the project directory by default
PROJECT_DIR=${1:-/project}

XDIST_WORKERS=$(python -c "import joblib; print(joblib.cpu_count(only_physical_cores=True))")
echo "👷 Number of workers: $XDIST_WORKERS"
pip show hyperscan
pytest --pyargs "${PROJECT_DIR}/tests" -n $XDIST_WORKERS -vvv
