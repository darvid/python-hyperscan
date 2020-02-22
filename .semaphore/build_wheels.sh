#!/usr/bin/env bash
set -e -x
mkdir -p /tmp/wheelhouse

for py in /opt/python/*/bin; do
  [[ "$py" == *"cp27"* ]] || [[ "$py" == *"cp34"* ]] && continue
  # https://gitlab.com/python-devs/importlib_metadata/issues/107
  "${py}/pip" install -UI poetry setuptools "zipp<2"
  "${py}/pip" wheel \
    -r /io/tests/requirements.txt \
    -w /io/wheelhouse/
  "${py}/poetry" build -vvv
  mv /io/dist/*.whl /tmp/wheelhouse/
done

for whl in /tmp/wheelhouse/*.whl; do
  auditwheel repair "$whl" -w /io/wheelhouse/
done

mkdir -p /io/dist
cp /io/wheelhouse/hyperscan*.whl /io/dist/
