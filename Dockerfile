FROM darvid/manylinux-hyperscan:5.4.0
ENV PCRE_PATH=/opt/pcre/.libs
RUN python3.9 -m pip install poetry
RUN mkdir /src
WORKDIR /src
COPY . .
RUN python3.9 -m poetry install -vvv
