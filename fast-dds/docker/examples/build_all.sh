#!/bin/bash

# =============================================================================
# Fast-DDS 通用编译脚本
# =============================================================================
# 功能: 自动扫描并编译examples目录下的所有测试用例
# 用法: ./build_all.sh
# =============================================================================

set -e

echo "=== Fast-DDS 通用编译脚本 ==="
echo ""

# 设置工作目录
EXAMPLES_DIR="/opt/fast-dds/examples"
BIN_DIR="/opt/fast-dds/examples/bin"

# 创建bin目录
mkdir -p "$BIN_DIR"

echo "扫描 $EXAMPLES_DIR 目录下的所有测试用例..."
echo ""

# 计数器
total_examples=0
successful_examples=0
failed_examples=0

# 扫描所有子目录
for example_dir in "$EXAMPLES_DIR"/*/; do
    if [ -d "$example_dir" ]; then
        example_name=$(basename "$example_dir")
        
        # 检查是否包含build.sh文件
        if [ -f "$example_dir/build.sh" ]; then
            total_examples=$((total_examples + 1))
            echo "发现测试用例: $example_name"
            
            # 进入测试用例目录
            cd "$example_dir"
            
            # 确保build.sh可执行
            chmod +x build.sh
            
            echo "开始编译 $example_name..."
            
            # 执行编译
            if ./build.sh; then
                echo "✅ $example_name 编译成功！"
                successful_examples=$((successful_examples + 1))
            else
                echo "❌ $example_name 编译失败！"
                failed_examples=$((failed_examples + 1))
            fi
            
            echo ""
            cd "$EXAMPLES_DIR"
        else
            echo "跳过目录 $example_name (未找到build.sh)"
        fi
    fi
done

echo "=== 编译完成 ==="
echo "总测试用例数: $total_examples"
echo "编译成功: $successful_examples"
echo "编译失败: $failed_examples"
echo ""

if [ $total_examples -eq 0 ]; then
    echo "⚠️  未发现任何测试用例"
    echo "请确保examples目录下存在包含build.sh的子目录"
    exit 1
fi

echo "可执行文件位置: $BIN_DIR"
echo "生成的文件:"
ls -la "$BIN_DIR" || echo "bin目录为空"

echo ""
if [ $failed_examples -eq 0 ]; then
    echo "🎉 所有测试用例编译成功！"
    exit 0
else
    echo "⚠️  部分测试用例编译失败，请检查日志"
    exit 1
fi
