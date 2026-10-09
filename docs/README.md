# pans

## 代码规范
常量：全部大写
类名：首字母大写，驼峰
非静态成员函数：首字母小写，驼峰
静态成员函数：首字母大写，驼峰
非静态成员变量：以m_开头，首字母小写，驼峰
静态成员变量：以s_开头，首字母小写，驼峰
全局函数：首字母大写，驼峰
函数参数：小写，下划线隔开
局部变量：小写，下划线隔开

# 子模块管理

## 添加子模块
git submodule add -b master https://github.com/jbeder/yaml-cpp.git third_party/yaml-cpp
git submodule add -b master https://github.com/abseil/abseil-cpp.git third_party/abseil-cpp
git submodule add -b main https://github.com/protocolbuffers/protobuf.git third_party/protobuf
git submodule add -b master https://github.com/Tencent/rapidjson.git third_party/rapidjson
git submodule add -b master https://github.com/leethomason/tinyxml2.git third_party/tinyxml2
git submodule add -b master https://github.com/skywind3000/kcp.git third_party/kcp
git submodule add -b master https://github.com/madler/zlib.git third_party/zlib

## 同步代码

### 首次同步pans

git clone --recurse-submodules https://gitee.com/huajianke2026/pans.git

### 更新pans

如果已经同步过pans, 只需要更新其子模块，执行：

git submodule update --init --recursive

# 接入Boost.Context

## boost地址
https://www.boost.org/
https://github.com/boostorg/boost.git

## 接入步骤

### 下载
wget https://archives.boost.io/release/1.92.0/source/boost_1_92_0.tar.gz
(下载之后位置在/root/workstation下面)

### 解压
tar -zvxf boost_1_92_0.tar.gz

### 配置
(当前目录在/root/workstation下面, 下同)
cd boost_1_92_0
./bootstrap.sh

### 编译bcp
./b2 tools/bcp

### 创建boost目录
(pans存放在/root/workstation下面)
mkdir /root/workstation/pans/third_party/boost_1_92_0

### 拷贝项目需要的内容

dist/bin/bcp \
  --boost=/root/workstation/boost_1_92_0 \
  boost/context/fiber.hpp \
  boost/context/detail/exception.hpp \
  boost/context/fixedsize_stack.hpp \
  boost/context/protected_fixedsize_stack.hpp \
  boost/context/stack_traits.hpp \
  /root/workstation/pans/third_party/boost_1_92_0

cp LICENSE_1_0.txt /root/workstation/pans/third_party/boost_1_92_0

