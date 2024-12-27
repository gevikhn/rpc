#include <iostream>
#include "math_ops.h"

int main() {
    rpc::math::MathOps math;
    
    std::cout << "RPC Project Demo" << std::endl;
    std::cout << "2 + 3 = " << math.add(2, 3) << std::endl;
    std::cout << "5 - 3 = " << math.subtract(5, 3) << std::endl;
    std::cout << "4 * 2 = " << math.multiply(4, 2) << std::endl;
    std::cout << "6 / 2 = " << math.divide(6.0, 2.0) << std::endl;
    
    return 0;
}
