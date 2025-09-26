#!/bin/bash
set -e

# 清理之前的容器
echo "1. 清理之前的容器..."
docker compose down 2>/dev/null || true
docker container prune -f >/dev/null 2>&1 || true

# 构建镜像
echo ""
echo "2. 构建Docker镜像..."
docker compose build

# 启动编译服务
echo ""
echo "3. 启动编译服务 (fast-dds-builder)..."
docker compose run --rm fast-dds-builder

# 启动运行时服务
echo ""
echo "4. 启动运行时服务 (publisher & subscriber)..."
docker compose run -d --rm publisher
docker compose run --rm subscriber


echo ""
echo "=== 测试完成！ ==="
echo ""
