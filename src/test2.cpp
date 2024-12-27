#include <iostream>
#include <functional>
#include <unordered_map>
#include <string>
#include <any>
#include <vector>
#include <tuple>
#include <typeinfo>
#include <cxxabi.h>
#include <array>
#include <list>
#include <memory>
#include <algorithm>
#include <chrono>
#include <optional>
#include <variant>
#include <future>
#include <thread>
#include "rpc_server.hpp"
using namespace rpc;





// 测试用的慢函数
std::string slow_concatenate(std::string str1, std::string str2) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return str1 + str2;
}

// 基本数据类型测试函数
void print_string(std::string str) {
    std::cout << str << std::endl;
}

void print_void() {
    std::cout << "void" << std::endl;
}

void void_void(void){
    std::cout << "void void" << std::endl;
}

int add_integers(int a, int b) {
    return a + b;
}

double calculate_average(std::vector<double> numbers) {
    if (numbers.empty()) return 0.0;
    double sum = 0.0;
    for (double num : numbers) {
        sum += num;
    }
    return sum / numbers.size();
}

std::string concatenate_strings(std::string str1, std::string str2) {
    return str1 + str2;
}

bool compare_numbers(int a, int b) {
    return a > b;
}

// 复杂数据类型测试函数
std::vector<int> sort_numbers(std::vector<int> numbers) {
    std::sort(numbers.begin(), numbers.end());
    return numbers;
}

std::array<int, 3> array_operations(std::array<int, 3> arr) {
    for (auto& num : arr) {
        num *= 2;
    }
    return arr;
}

// Optional 和 Variant 测试函数
std::optional<int> safe_divide(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

std::variant<int, std::string> process_value(std::variant<int, std::string> value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value) * 2;
    } else {
        return std::get<std::string>(value) + std::get<std::string>(value);
    }
}

// 指针和引用测试函数
std::shared_ptr<std::string> create_string_ptr(std::string str) {
    return std::make_shared<std::string>(str);
}

// 组合多种类型的测试函数
std::tuple<int, std::string, double> process_mixed_types(
    int number,
    std::string text,
    std::vector<double> values
) {
    int sum = number;
    std::string processed_text = text + "!";
    double avg = calculate_average(values);
    return std::make_tuple(sum, processed_text, avg);
}

// 多维数组测试函数
void matrix_transpose(const int input[3][3], int output[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            output[j][i] = input[i][j];
        }
    }
}

void calculate_row_sums(const int matrix[3][3], int sums[3]) {
    for (int i = 0; i < 3; i++) {
        sums[i] = 0;
        for (int j = 0; j < 3; j++) {
            sums[i] += matrix[i][j];
        }
    }
}

void rotate_3d_array(const int input[2][2][2], int output[2][2][2]) {
    for (int z = 0; z < 2; z++) {
        for (int y = 0; y < 2; y++) {
            for (int x = 0; x < 2; x++) {
                output[z][x][1-y] = input[z][y][x];
            }
        }
    }
}

// 包装函数，使其适应RPC调用
std::array<std::array<int, 3>, 3> wrapped_matrix_transpose(std::array<std::array<int, 3>, 3> input) {
    int raw_input[3][3];
    int raw_output[3][3];
    std::array<std::array<int, 3>, 3> output;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            raw_input[i][j] = input[i][j];
        }
    }

    matrix_transpose(raw_input, raw_output);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            output[i][j] = raw_output[i][j];
        }
    }

    return output;
}

std::array<int, 3> wrapped_calculate_row_sums(std::array<std::array<int, 3>, 3> input) {
    int raw_input[3][3];
    int raw_output[3];
    std::array<int, 3> output;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            raw_input[i][j] = input[i][j];
        }
    }

    calculate_row_sums(raw_input, raw_output);

    for (int i = 0; i < 3; i++) {
        output[i] = raw_output[i];
    }

    return output;
}

std::array<std::array<std::array<int, 2>, 2>, 2> wrapped_rotate_3d_array(
    std::array<std::array<std::array<int, 2>, 2>, 2> input
) {
    int raw_input[2][2][2];
    int raw_output[2][2][2];
    std::array<std::array<std::array<int, 2>, 2>, 2> output;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                raw_input[i][j][k] = input[i][j][k];
            }
        }
    }

    rotate_3d_array(raw_input, raw_output);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                output[i][j][k] = raw_output[i][j][k];
            }
        }
    }

    return output;
}

int main() {
    RpcServer server;

    // 注册基本类型函数

    server.register_function<void, std::string>(
        "print", std::function<void(std::string)>(print_string)
    );

    server.register_function<void>(
        "print_void", std::function<void()>(print_void)
    );

    server.register_function<void>(
        "void_void", std::function<void()>(void_void)
    );

    server.register_function<int, int, int>(
        "add", std::function<int(int, int)>(add_integers)
    );
    
    server.register_function<double, std::vector<double>>(
        "average", std::function<double(std::vector<double>)>(calculate_average)
    );
    
    server.register_function<std::string, std::string, std::string>(
        "concat", std::function<std::string(std::string, std::string)>(concatenate_strings)
    );

    // 注册带超时的慢函数
    server.register_function<std::string, std::string, std::string>(
        "slow_concat", 
        std::function<std::string(std::string, std::string)>(slow_concatenate),
        std::chrono::milliseconds(3000)  // 3秒超时
    );
    
    server.register_function<bool, int, int>(
        "compare", std::function<bool(int, int)>(compare_numbers)
    );

    // 注册复杂类型函数
    server.register_function<std::vector<int>, std::vector<int>>(
        "sort", std::function<std::vector<int>(std::vector<int>)>(sort_numbers)
    );
    
    server.register_function<std::array<int, 3>, std::array<int, 3>>(
        "array_ops", std::function<std::array<int, 3>(std::array<int, 3>)>(array_operations)
    );

    // 注册 Optional 和 Variant 函数
    server.register_function<std::optional<int>, int, int>(
        "safe_divide", std::function<std::optional<int>(int, int)>(safe_divide)
    );

    server.register_function<std::variant<int, std::string>, std::variant<int, std::string>>(
        "process_value", std::function<std::variant<int, std::string>(std::variant<int, std::string>)>(process_value)
    );
    
    server.register_function<std::shared_ptr<std::string>, std::string>(
        "create_ptr", std::function<std::shared_ptr<std::string>(std::string)>(create_string_ptr)
    );
    
    server.register_function<std::tuple<int, std::string, double>, int, std::string, std::vector<double>>(
        "mixed", std::function<std::tuple<int, std::string, double>(int, std::string, std::vector<double>)>(process_mixed_types)
    );

    server.register_function<std::array<std::array<int, 3>, 3>, std::array<std::array<int, 3>, 3>>(
        "transpose", std::function<std::array<std::array<int, 3>, 3>(std::array<std::array<int, 3>, 3>)>(wrapped_matrix_transpose)
    );

    server.register_function<std::array<int, 3>, std::array<std::array<int, 3>, 3>>(
        "row_sums", std::function<std::array<int, 3>(std::array<std::array<int, 3>, 3>)>(wrapped_calculate_row_sums)
    );

    server.register_function<
        std::array<std::array<std::array<int, 2>, 2>, 2>,
        std::array<std::array<std::array<int, 2>, 2>, 2>
    >(
        "rotate_3d", std::function<
            std::array<std::array<std::array<int, 2>, 2>, 2>(
                std::array<std::array<std::array<int, 2>, 2>, 2>
            )
        >(wrapped_rotate_3d_array)
    );

    try {
        std::cout << "\n=== 测试基本类型 ===" << std::endl;

        server.call_function<void>("print", std::string("Hello, World!"));
        server.call_function<void>("print_void");
        server.call_function<void>("void_void");
        
        int sum = server.call_function<int>("add", 5, 3);
        std::cout << "加法结果: " << sum << std::endl;
        
        double avg = server.call_function<double>("average", std::vector<double>{1.5, 2.5, 3.5});
        std::cout << "平均值: " << avg << std::endl;
        
        std::string concat_result = server.call_function<std::string>("concat", std::string("Hello, "), std::string("World!"));
        std::cout << "字符串拼接: " << concat_result << std::endl;

        // 测试超时函数
        try {
            std::cout << "\n=== 测试超时机制 ===" << std::endl;
            std::string slow_result = server.call_function<std::string>(
                "slow_concat", 
                std::string("This "), 
                std::string("will timeout!")
            );
        } catch (const RpcException& e) {
            if (e.type() == RpcException::ErrorType::TIMEOUT_ERROR) {
                std::cout << "预期的超时错误: " << e.what() << std::endl;
            } else {
                throw;
            }
        }
        
        bool comp_result = server.call_function<bool>("compare", 10, 5);
        std::cout << "比较结果: " << (comp_result ? "true" : "false") << std::endl;

        // 测试复杂类型函数
        std::cout << "\n=== 测试复杂类型 ===" << std::endl;
        
        auto sorted = server.call_function<std::vector<int>>("sort", std::vector<int>{3, 1, 4, 1, 5});
        std::cout << "排序结果: ";
        for (int num : sorted) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
        
        auto arr_result = server.call_function<std::array<int, 3>>("array_ops", std::array<int, 3>{1, 2, 3});
        std::cout << "数组操作结果: ";
        for (int num : arr_result) {
            std::cout << num << " ";
        }
        std::cout << std::endl;

        // 测试 Optional 和 Variant
        std::cout << "\n=== 测试 Optional 和 Variant ===" << std::endl;
        
        auto div_result = server.call_function<std::optional<int>>("safe_divide", 10, 2);
        std::cout << "安全除法 (10/2): " << (div_result ? std::to_string(*div_result) : "无结果") << std::endl;
        
        auto div_by_zero = server.call_function<std::optional<int>>("safe_divide", 10, 0);
        std::cout << "安全除法 (10/0): " << (div_by_zero ? std::to_string(*div_by_zero) : "无结果") << std::endl;

        auto var_int_result = server.call_function<std::variant<int, std::string>>(
            "process_value", 
            std::variant<int, std::string>(42)
        );
        if (std::holds_alternative<int>(var_int_result)) {
            std::cout << "Variant int 结果: " << std::get<int>(var_int_result) << std::endl;
        }

        auto var_str_result = server.call_function<std::variant<int, std::string>>(
            "process_value", 
            std::variant<int, std::string>(std::string("test"))
        );
        if (std::holds_alternative<std::string>(var_str_result)) {
            std::cout << "Variant string 结果: " << std::get<std::string>(var_str_result) << std::endl;
        }
        
        auto str_ptr = server.call_function<std::shared_ptr<std::string>>("create_ptr", std::string("测试字符串"));
        std::cout << "智能指针内容: " << *str_ptr << std::endl;

        // 测试混合类型函数
        std::cout << "\n=== 测试混合类型 ===" << std::endl;
        auto [result_int, result_str, result_double] = server.call_function<std::tuple<int, std::string, double>>(
            "mixed", 42, std::string("测试文本"), std::vector<double>{1.1, 2.2, 3.3}
        );
        std::cout << "混合类型结果: " << result_int << ", " << result_str << ", " << result_double << std::endl;

        std::cout << "\n=== 测试多维数组 ===" << std::endl;
        
        std::array<std::array<int, 3>, 3> matrix = {{
            {{1, 2, 3}},
            {{4, 5, 6}},
            {{7, 8, 9}}
        }};
        
        auto transposed = server.call_function<std::array<std::array<int, 3>, 3>>("transpose", matrix);
        std::cout << "矩阵转置结果:" << std::endl;
        for (const auto& row : transposed) {
            for (int val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        
        auto row_sums = server.call_function<std::array<int, 3>>("row_sums", matrix);
        std::cout << "矩阵行和:" << std::endl;
        for (int sum : row_sums) {
            std::cout << sum << " ";
        }
        std::cout << std::endl;
        
        std::array<std::array<std::array<int, 2>, 2>, 2> cube = {{
            {{ {{1, 2}}, {{3, 4}} }},
            {{ {{5, 6}}, {{7, 8}} }}
        }};
        
        auto rotated = server.call_function<std::array<std::array<std::array<int, 2>, 2>, 2>>("rotate_3d", cube);
        std::cout << "3D数组旋转结果:" << std::endl;
        for (size_t z = 0; z < 2; z++) {
            std::cout << "层 " << z << ":" << std::endl;
            for (const auto& row : rotated[z]) {
                for (int val : row) {
                    std::cout << val << " ";
                }
                std::cout << std::endl;
            }
        }

    } catch (const RpcException& e) {
        std::cerr << "RPC 错误: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "标准错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}