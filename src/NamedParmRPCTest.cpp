#include "rpc_provider.hpp"
#include <iostream>

using json = nlohmann::json;

// 测试函数
int add(int a, int b) {
    return a + b;
}

std::string greet(std::string name, int age) {
    return "Hello, " + name + "! You are " + std::to_string(age) + " years old.";
}

std::vector<int> sum_arrays(std::vector<int> arr1, std::vector<int> arr2) {
    if (arr1.size() != arr2.size()) {
        throw std::invalid_argument("Arrays must have the same size");
    }
    std::vector<int> result;
    result.reserve(arr1.size());
    for (size_t i = 0; i <arr1.size(); ++i) {
        result.push_back(arr1[i] + arr2[i]);
    }
    return result;
}

int main() {
    rpc::RpcProvider server;

    // 注册函数
    REGISTER_FUNCTION(server, "add", add, a, b);
    REGISTER_FUNCTION(server, "greet", greet, name, age);
    REGISTER_FUNCTION(server, "sum_arrays", sum_arrays, arr1, arr2);

    try {
        // 测试基本类型
        json add_args = {
            {"a", 5},
            {"b", 3}
        };
        
        int result = server.call_function_named<int>("add", add_args);
        std::cout << "5 + 3 = " << result << std::endl;

        // 测试字符串和数字混合
        json greet_args = {
            {"name", "Alice"},
            {"age", 25}
        };
        
        std::string greeting = server.call_function_named<std::string>("greet", greet_args);
        std::cout << greeting << std::endl;

        // 测试数组
        json array_args = {
            {"arr1", {1, 2, 3, 4, 5}},
            {"arr2", {10, 20, 30, 40, 50}}
        };
        
        auto array_result = server.call_function_named<std::vector<int>>("sum_arrays", array_args);
        std::cout << "Array sum: ";
        for (int val : array_result) {
            std::cout << val << " ";
        }
        std::cout << std::endl;

        // 测试错误情况：类型不匹配
        json invalid_args = {
            {"a", "not_a_number"},
            {"b", 3}
        };
        
        try {
            server.call_function_named<int>("add", invalid_args);
        } catch (const rpc::RpcException& e) {
            std::cout << "Expected error caught: " << e.what() << std::endl;
        }

        // 测试错误情况：缺少参数
        json missing_args = {
            {"a", 5}
            // missing "b"
        };
        
        try {
            server.call_function_named<int>("add", missing_args);
        } catch (const rpc::RpcException& e) {
            std::cout << "Expected error caught: " << e.what() << std::endl;
        }

        // 测试错误情况：数组大小不匹配
        json invalid_array_args = {
            {"arr1", {1, 2, 3}},
            {"arr2", {10, 20, 30, 40, 50}}
        };
        
        try {
            server.call_function_named<std::vector<int>>("sum_arrays", invalid_array_args);
        } catch (const std::invalid_argument& e) {
            std::cout << "Expected error caught: " << e.what() << std::endl;
        }

    } catch (const rpc::RpcException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}