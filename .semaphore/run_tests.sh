#!/usr/bin/env bash
set -e -x

for whl in wheelhouse/hyperscan-*.whl; do
  python_version=$(expr "$whl" : '.*cp\(.*mu\?\)-')
  if [[ $python_version == "35m" ]]; then
    image='python:3.5'
  elif [[ $python_version == "36m" ]]; then
    image='python:3.6'
  elif [[ $python_version == "37m" ]]; then
    image='python:3.7'
  elif [[ $python_version == "38m" ]]; then
    image='python:3.8'
  else
    continue
  fi
  docker run \
    --rm \
    -i \
    -w /io/ \
    -v "$(pwd)"/tests:/io/tests \
    -v "$(pwd)"/tox.ini:/io/tox.ini \
    -v "$(pwd)"/wheelhouse:/io/wheelhouse \
    $image \
    /bin/bash -c "pip install --no-index -f /io/wheelhouse/ tox && tox -e python"
done
