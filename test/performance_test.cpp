#include <gtest/gtest.h>
#include <chrono>
#include "math_ops.h"

using namespace rpc::math;
using namespace std::chrono;

class PerformanceTest : public ::testing::Test {
protected:
    MathOps math_ops;
    const int iterations = 1000000;  // 100万次迭代
    
    void SetUp() override {
        // 预热
        for (int i = 0; i < 1000; ++i) {
            math_ops.add(i, i);
        }
    }
};

// 测试加法性能
TEST_F(PerformanceTest, AdditionPerformance) {
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        math_ops.add(i, i + 1);
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    // 检查每次操作的平均时间是否小于1微秒
    double avg_time = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time, 1.0) << "Average addition time: " << avg_time << " microseconds";
}

// 测试除法性能
TEST_F(PerformanceTest, DivisionPerformance) {
    auto start = high_resolution_clock::now();
    
    for (int i = 1; i < iterations; ++i) {
        math_ops.divide(static_cast<double>(i), static_cast<double>(i));
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    // 检查每次操作的平均时间是否小于2微秒
    double avg_time = static_cast<double>(duration.count()) / iterations;
    EXPECT_LT(avg_time, 2.0) << "Average division time: " << avg_time << " microseconds";
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
