#!/usr/bin/env bash
set -e -x

for whl in wheelhouse/hyperscan-*.whl; do
  python_version=$(sed -re 's/.*\-cp([0-9]+)\-(cp|m).*/\1/' <<< "$whl")
  if [[ $python_version -eq 35 ]]; then
    image='python:3.5'
  elif [[ $python_version -eq 36 ]]; then
    image='python:3.6'
  elif [[ $python_version -eq 37 ]]; then
    image='python:3.7'
  elif [[ $python_version -eq 38 ]]; then
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
