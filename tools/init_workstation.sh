#!/bin/bash

set -e

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Record start time
START_TIME=$(date +%s)
START_DATE=$(date '+%Y-%m-%d %H:%M:%S')

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}阿里云工作站一键初始化脚本${NC}"
echo -e "${GREEN}========================================${NC}"
echo -e "${BLUE}开始时间: ${START_DATE}${NC}"

# Step 1: Update apt and install git
echo -e "\n${YELLOW}[Step 1/7]${NC} 更新包管理器并安装 git..."
sudo apt update
sudo apt install -y --no-install-recommends \
    git \
    cmake \
    vim \
    curl \
    openssh-client \
    openssh-server \
    wget \
    ca-certificates \
    build-essential \
    libcrypt-dev \
    libmysqlclient-dev \
    libsqlite3-dev \
    pkg-config \
    gdb \
    tree \
    iputils-ping \
    iproute2 \
    net-tools \
    libdw-dev

# Step 2: Create workstation directory and configure git
echo -e "\n${YELLOW}[Step 2/7]${NC} 创建 workstation 目录并配置 git..."
mkdir -p ~/workstation
cd ~/workstation

git config --global credential.helper store
git config --global user.email "huajianke2026@163.com"
git config --global user.name "huajianke2026"

bash -c 'echo "coredump.%e.%p.%t" > /proc/sys/kernel/core_pattern'

# Step 3: Clone repositories
echo -e "\n${YELLOW}[Step 3/7]${NC} 克隆代码仓库..."
if [ ! -d "pans" ]; then
    git clone --recurse-submodules https://gitee.com/huajianke2026/pans.git
fi

# Calculate elapsed time
END_TIME=$(date +%s)
END_DATE=$(date '+%Y-%m-%d %H:%M:%S')
ELAPSED=$((END_TIME - START_TIME))
MINUTES=$((ELAPSED / 60))
SECONDS=$((ELAPSED % 60))

echo -e "${BLUE}结束时间: ${END_DATE}${NC}"
echo -e "${BLUE}总耗时: ${MINUTES}分${SECONDS}秒${NC}"
