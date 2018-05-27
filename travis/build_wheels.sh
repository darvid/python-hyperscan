#!/bin/bash
for py in /opt/python/*/bin; do
  "${py}/pip" install -UI setuptools setuptools_scm
  "${py}/pip" wheel -r /io/test_requirements.txt -w /io/wheelhouse/
  "${py}/python" /io/setup.py bdist_wheel -d /tmp/wheelhouse/
done

for whl in /tmp/wheelhouse/*.whl; do
  auditwheel repair "$whl" -w /io/wheelhouse/
done

mkdir -p /io/dist/
ln -s /io/wheelhouse/hyperscan*.whl /io/dist/
rm -rf /io/.eggs
