# 添加库
add_library(rpc_lib
    # 暂时添加一个空的库
    math_ops.cpp
)

# 设置包含目录
target_include_directories(rpc_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# 添加可执行文件
add_executable(rpc_demo main.cpp)
target_link_libraries(rpc_demo PRIVATE rpc_lib)
