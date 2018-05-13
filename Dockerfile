FROM ubuntu:bionic as builder

# Install compiler
RUN apt-get update \
&& apt-get install -y build-essential wget \
&& rm -rf /var/lib/apt/lists/*

# Install TBB
ENV TBB_VERSION 2018_20180312
ENV TBB_DOWNLOAD_URL https://github.com/01org/tbb/releases/download/2018_U3/tbb${TBB_VERSION}oss_lin.tgz
ENV TBB_INSTALL_DIR /opt/intel

RUN mkdir -p ${TBB_INSTALL_DIR} \
&& wget ${TBB_DOWNLOAD_URL} \
&& tar -C ${TBB_INSTALL_DIR} -xf tbb${TBB_VERSION}oss_lin.tgz \
&& rm tbb${TBB_VERSION}oss_lin.tgz \
&& mv ${TBB_INSTALL_DIR}/tbb${TBB_VERSION}oss ${TBB_INSTALL_DIR}/tbb \
&& sed -i "s%SUBSTITUTE_INSTALL_DIR_HERE%${TBB_INSTALL_DIR}/tbb%" ${TBB_INSTALL_DIR}/tbb/bin/tbbvars.* \
&& echo "source ${TBB_INSTALL_DIR}/tbb/bin/tbbvars.sh intel64" >> /root/.tbb_setup

# Copy source code
COPY * /root/

# Build
WORKDIR /root/
RUN /bin/bash -c 'source /root/.tbb_setup; make TBB_LIB=${LD_LIBRARY_PATH}'

# Copy into new container
FROM ubuntu:bionic
ENV TBB_INSTALL_DIR /opt/intel
WORKDIR /root/
COPY --from=builder ${TBB_INSTALL_DIR}/tbb ${TBB_INSTALL_DIR}/tbb
COPY --from=builder /root .
