#!/usr/bin/env bash
set -e -x

for whl in wheelhouse/hyperscan-*.whl; do
  python_version=$(expr "$whl" : '.*cp\(.*mu\?\)-')
  if [[ $python_version == "27m" ]]; then
    continue
  elif [[ $python_version == "27mu" ]]; then
    image='python:2.7'
  elif [[ $python_version == "34m" ]]; then
    image='python:3.4'
  elif [[ $python_version == "35m" ]]; then
    image='python:3.5'
  elif [[ $python_version == "36m" ]]; then
    image='python:3.6'
  elif [[ $python_version == "37m" ]]; then
    image='python:3.7'
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
