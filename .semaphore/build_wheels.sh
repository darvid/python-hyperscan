#!/usr/bin/env bash
set -e -x

mkdir -p /tmp/wheelhouse

for py in /opt/python/*/bin; do
  "${py}/pip" install -UI poetry setuptools
  "${py}/pip" wheel \
    -r /io/tests/requirements.txt \
    -r /io/docs/requirements.txt \
    -w /io/wheelhouse/
  "${py}/poetry" -vvv build
  mv /io/dist/*.whl /tmp/wheelhouse/
done

for whl in /tmp/wheelhouse/*.whl; do
  auditwheel repair "$whl" -w /io/wheelhouse/
done

mkdir -p /io/dist
cp /io/wheelhouse/hyperscan*.whl /io/dist/
