# 使用Ubuntu 22.04作为基础镜像
FROM ubuntu:24.04

# 设置环境变量
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Shanghai

# 更新包列表并安装必要的依赖
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    tcpdump \
    pkg-config \
    libssl-dev \
    libasio-dev \
    libtinyxml2-dev \
    libfoonathan-memory-dev \
    libeigen3-dev \
    libxml2-dev \
    libxslt1-dev \
    python3 \
    python3-pip \
    python3-dev \
    python3-setuptools \
    python3-wheel \
    doxygen \
    graphviz \
    openjdk-11-jdk \
    && rm -rf /var/lib/apt/lists/*

# 手动编译安装 FastCDR (兼容 Fast-DDS 3.3.0 的版本)
WORKDIR /tmp
RUN wget https://github.com/eProsima/Fast-CDR/archive/refs/tags/v2.3.0.tar.gz && \
    tar -xzf v2.3.0.tar.gz && \
    cd Fast-CDR-2.3.0 && \
    mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig && \
    cd /tmp && rm -rf Fast-CDR-2.3.0 v2.3.0.tar.gz

# 验证 FastCDR 安装
RUN ls -la /usr/local/include/fastcdr/cdr/ && \
    ls -la /usr/local/lib/cmake/fastcdr/

# 创建工作目录
WORKDIR /opt/fast-dds

# 下载Fast-DDS v3.3.0源码
RUN wget https://github.com/eProsima/Fast-DDS/archive/refs/tags/v3.3.0.tar.gz && \
    tar -xzf v3.3.0.tar.gz && \
    mv Fast-DDS-3.3.0/* . && \
    rm -rf Fast-DDS-3.3.0 v3.3.0.tar.gz

# 创建构建目录
RUN mkdir -p build

# 设置工作目录为构建目录
WORKDIR /opt/fast-dds/build

# 配置CMake
RUN cmake -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DBUILD_SHARED_LIBS=ON \
    -DCOMPILE_EXAMPLES=OFF \
    -DCOMPILE_TESTS=OFF \
    -DCMAKE_CXX_STANDARD=17 \
    ..

# 编译Fast-DDS
RUN make -j$(nproc)

# 安装Fast-DDS
RUN make install

# 安装fastddsgen工具（使用预编译版本）
WORKDIR /tmp
RUN mkdir -p ./Fast-DDS/src && \
   cd ./Fast-DDS/src && \
   git clone --recursive https://github.com/eProsima/Fast-DDS-Gen.git fastddsgen && \
   cd fastddsgen && \
   ./gradlew assemble && \
   ./gradlew install && \
   cd /tmp && rm -rf Fast-DDS

# 验证fastddsgen安装
RUN which fastddsgen

# 更新动态链接库缓存
RUN ldconfig

# 设置环境变量
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
ENV PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH

# 创建示例目录
WORKDIR /opt/fast-dds/examples

# 注意：示例文件将通过Docker Compose的卷挂载在运行时提供
# 这样可以实时修改代码而无需重新构建镜像

# 设置默认命令
CMD ["/bin/bash"]