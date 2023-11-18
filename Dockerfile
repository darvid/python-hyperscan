FROM ghcr.io/darvid/python_hyperscan_manylinux_2_28_x86_64:latest
# FROM musllinux-hyperscan
ENV PATH=/opt/python/cp312-cp312/bin:$PATH
ENV SETUPTOOLS_SCM_PRETEND_VERSION="0.0.0-dev"
RUN pip install setuptools pdm auditwheel
WORKDIR /project
COPY ./src /project/src
COPY ./pyproject.toml setup.py README.md /project/
RUN find . -name "*.so" -delete
RUN pdm build -v --no-sdist
RUN auditwheel repair dist/*.whl
RUN pip install wheelhouse/*.whl
# RUN python -c "import hyperscan"
