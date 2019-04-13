#!/usr/bin/env bash
set -e -x

mkdir -p /tmp/wheelhouse

for py in /opt/python/*/bin; do
  if [[ "$py" == *"cp27"* ]]; then
    # Poetry uses just the version number, not PEP 425 tags for venv
    # names, and manylinux includes two builds of Python 2.7, so we need
    # to delete the Poetry virtualenv to avoid skipping a build due to
    # Poetry using the wrong virtualenv.
    rm -rf $HOME/.cache/pypoetry/virtualenvs/hyperscan-py2.7
  fi
  "${py}/pip" install -UI poetry setuptools
  "${py}/pip" wheel \
    -r /io/tests/requirements.txt \
    -r /io/docs/requirements.txt \
    -w /io/wheelhouse/
  "${py}/poetry" -vvv build
  if [[ "$py" == *"cp27mu"* ]]; then
    orig_whl="$(ls dist/*cp27*.whl)"
    mv "$orig_whl" "$(echo "$orig_whl" | sed s/cp27m/cp27mu/)"
  fi
  mv /io/dist/*.whl /tmp/wheelhouse/
done

for whl in /tmp/wheelhouse/*.whl; do
  auditwheel repair "$whl" -w /io/wheelhouse/
done

mkdir -p /io/dist
cp /io/wheelhouse/hyperscan*.whl /io/dist/
