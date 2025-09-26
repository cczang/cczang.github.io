#!/bin/bash

# HelloWorld DDS示例构建脚本

echo "开始构建HelloWorld DDS示例..."

# 获取脚本所在目录的父目录（examples目录）
EXAMPLES_DIR=$(dirname $(dirname $(realpath $0)))
BUILD_DIR=${EXAMPLES_DIR}/build
BIN_DIR=${EXAMPLES_DIR}/bin

# 创建构建和输出目录
mkdir -p ${BUILD_DIR}
mkdir -p ${BIN_DIR}

# 进入构建目录
cd ${BUILD_DIR}

# 配置CMake
echo "配置CMake..."
cmake ../helloworld

# 编译
echo "编译项目..."
make

echo "构建完成！"

echo "运行示例："
echo "  cd ${BIN_DIR}"
echo "  ./HelloWorldSubscriber 10  # 在终端1中运行"
echo "  ./HelloWorldPublisher 10   # 在终端2中运行"
