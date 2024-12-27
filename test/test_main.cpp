#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "math_ops.h"

using namespace rpc::math;
using namespace testing;

// 创建一个 Mock 类
class MockMathOps : public MathOps {
public:
    MOCK_METHOD(int, add, (int a, int b), (const, override));
    MOCK_METHOD(int, subtract, (int a, int b), (const, override));
    MOCK_METHOD(int, multiply, (int a, int b), (const, override));
    MOCK_METHOD(double, divide, (double a, double b), (const, override));
};

// 测试实际的 MathOps 类
class MathOpsTest : public Test {
protected:
    MathOps math_ops;
};

// 基本运算测试
TEST_F(MathOpsTest, Addition) {
    EXPECT_EQ(math_ops.add(2, 3), 5);
    EXPECT_EQ(math_ops.add(-2, 3), 1);
    EXPECT_EQ(math_ops.add(0, 0), 0);
}

TEST_F(MathOpsTest, Subtraction) {
    EXPECT_EQ(math_ops.subtract(5, 3), 2);
    EXPECT_EQ(math_ops.subtract(3, 5), -2);
    EXPECT_EQ(math_ops.subtract(0, 0), 0);
}

TEST_F(MathOpsTest, Multiplication) {
    EXPECT_EQ(math_ops.multiply(2, 3), 6);
    EXPECT_EQ(math_ops.multiply(-2, 3), -6);
    EXPECT_EQ(math_ops.multiply(0, 5), 0);
}

TEST_F(MathOpsTest, Division) {
    EXPECT_DOUBLE_EQ(math_ops.divide(6.0, 2.0), 3.0);
    EXPECT_DOUBLE_EQ(math_ops.divide(-6.0, 2.0), -3.0);
    EXPECT_THROW(math_ops.divide(5.0, 0.0), std::invalid_argument);
}

// 使用 Mock 对象的测试示例
TEST(MathOpsMockTest, MockExample) {
    MockMathOps mock_math;
    
    // 设置期望
    EXPECT_CALL(mock_math, add(2, 3))
        .Times(1)
        .WillOnce(Return(5));
        
    EXPECT_CALL(mock_math, multiply(_, _))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(10));
    
    // 使用 mock 对象
    EXPECT_EQ(mock_math.add(2, 3), 5);
    EXPECT_EQ(mock_math.multiply(2, 3), 10);
    EXPECT_EQ(mock_math.multiply(4, 5), 10);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
