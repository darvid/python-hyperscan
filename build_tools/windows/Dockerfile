ARG PYTHON_VERSION
FROM winamd64/python:$PYTHON_VERSION-windowsservercore

ARG WHEEL_NAME
ARG CONFTEST_NAME
ARG CIBW_TEST_REQUIRES

COPY $WHEEL_NAME $WHEEL_NAME
RUN pip install $env:WHEEL_NAME
RUN pip install $env:CIBW_TEST_REQUIRES.split(" ")
