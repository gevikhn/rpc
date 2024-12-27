# RPC Project

这是一个基于 CMake 构建的 RPC 项目。

## 项目结构

```
.
├── CMakeLists.txt
├── include/         # 头文件目录
├── src/            # 源代码目录
│   ├── CMakeLists.txt
│   └── main.cpp
├── test/           # 测试代码目录
│   ├── CMakeLists.txt
│   └── test_main.cpp
└── build/          # 构建目录
```

## 构建说明

1. 创建构建目录：
   ```bash
   mkdir -p build && cd build
   ```

2. 运行 CMake：
   ```bash
   cmake ..
   ```

3. 编译项目：
   ```bash
   make
   ```

4. 运行测试：
   ```bash
   make test
   ```

## 许可证

[待添加]
