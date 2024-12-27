# 添加测试可执行文件
add_executable(rpc_test test_main.cpp)
target_link_libraries(rpc_test 
    PRIVATE 
    rpc_lib
    GTest::gtest_main
    GTest::gmock_main
)

# 包含 Google Test 模块
include(GoogleTest)
gtest_discover_tests(rpc_test
    PROPERTIES
        LABELS "unit;math"
)

# 添加自定义测试
add_test(NAME math_demo COMMAND $<TARGET_FILE:rpc_demo>)
set_tests_properties(math_demo
    PROPERTIES
    PASS_REGULAR_EXPRESSION "2 \\+ 3 = 5.*5 - 3 = 2.*4 \\* 2 = 8.*6 / 2 = 3"
    LABELS "integration;math"
)

# 添加性能测试
add_executable(performance_test performance_test.cpp)
target_link_libraries(performance_test
    PRIVATE
    rpc_lib
    GTest::gtest_main
)
add_test(NAME performance_test COMMAND performance_test)
set_tests_properties(performance_test
    PROPERTIES
    LABELS "performance;math"
    TIMEOUT 5  # 设置超时时间为5秒
)
