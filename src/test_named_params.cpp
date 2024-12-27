#include "rpc_provider.hpp"
#include <iostream>
#include <sstream>

// 测试函数
int add(int a, int b) {
    return a + b;
}

std::string greet(std::string name, int age) {
    return "Hello, " + name + "! You are " + std::to_string(age) + " years old.";
}

int main() {
    rpc::RpcProvider server;

    // 使用宏自动注册函数和参数名
    REGISTER_FUNCTION(server, "add", add, a, b);  // 直接使用参数名
    REGISTER_FUNCTION(server, "greet", greet, name, age);

    try {
        // 使用命名参数调用
        std::map<std::string, std::any> add_args;
        add_args["a"] = std::make_any<int>(5);
        add_args["b"] = std::make_any<int>(3);
        
        int result = server.call_function_named<int>("add", add_args);
        std::cout << "5 + 3 = " << result << std::endl;

        // 参数顺序可以不同
        std::map<std::string, std::any> greet_args;
        greet_args["age"] = std::make_any<int>(25);
        greet_args["name"] = std::make_any<std::string>("Alice");
        
        std::string greeting = server.call_function_named<std::string>("greet", greet_args);
        std::cout << greeting << std::endl;

        // 测试错误情况
        std::map<std::string, std::any> invalid_args;
        invalid_args["a"] = std::make_any<std::string>("not_a_number");  // 错误的类型
        invalid_args["b"] = std::make_any<int>(3);
        
        try {
            server.call_function_named<int>("add", invalid_args);
        } catch (const rpc::RpcException& e) {
            std::cout << "Expected error caught: " << e.what() << std::endl;
        }

    } catch (const rpc::RpcException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
