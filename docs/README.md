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

