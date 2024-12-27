#include "math_ops.h"
#include <stdexcept>

namespace rpc {
namespace math {

int MathOps::add(int a, int b) const {
    return a + b;
}

int MathOps::subtract(int a, int b) const {
    return a - b;
}

int MathOps::multiply(int a, int b) const {
    return a * b;
}

double MathOps::divide(double a, double b) const {
    if (b == 0) {
        throw std::invalid_argument("Division by zero");
    }
    return a / b;
}

} // namespace math
} // namespace rpc
