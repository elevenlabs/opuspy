# docker build --platform linux/amd64 -f multilinux.env.Dockerfile . -t multilinux-env
FROM quay.io/pypa/manylinux2014_x86_64

RUN yum install autoconf automake libtool curl make cmake gcc-c++ unzip wget -y
RUN yum install python3 -y
RUN pip3 install pybind11[global]
RUN yum install openssl-devel -y
RUN yum install libogg-devel -y
ADD install_opus.sh .
RUN sh install_opus.sh


