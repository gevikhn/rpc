#pragma once

namespace rpc {
namespace math {

class MathOps {
public:
    virtual ~MathOps() = default;
    
    // 基本数学运算
    virtual int add(int a, int b) const;
    virtual int subtract(int a, int b) const;
    virtual int multiply(int a, int b) const;
    virtual double divide(double a, double b) const;
};

} // namespace math
} // namespace rpc
