cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(RpcProject VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含 FetchContent 模块
include(FetchContent)

set(ENV{http_proxy} "http://127.0.0.1:10808")
set(ENV{https_proxy} "http://127.0.0.1:10808")

# 下载并配置 Google Test
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# 添加子目录
add_subdirectory(src)

# 设置包含目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 启用测试
enable_testing()

# 配置 CTest
set(CTEST_OUTPUT_ON_FAILURE TRUE)
set(CTEST_PARALLEL_LEVEL 4)  # 并行运行测试

# 添加测试目录
add_subdirectory(test)

# 添加自定义测试目标
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose
    DEPENDS rpc_test performance_test rpc_demo
)

# 添加只运行单元测试的目标
add_custom_target(unit_test
    COMMAND ${CMAKE_CTEST_COMMAND} -L unit --output-on-failure
    DEPENDS rpc_test
)

# 添加只运行性能测试的目标
add_custom_target(perf_test
    COMMAND ${CMAKE_CTEST_COMMAND} -L performance --output-on-failure
    DEPENDS performance_test
)
