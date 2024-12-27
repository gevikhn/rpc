#ifndef __RPC_SERVER_H__
#define __RPC_SERVER_H__

#include <map>
#include <any>
#include <stdexcept>
#include <cstring>
#include <string>
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

namespace rpc{

    // 自定义异常类
    class RpcException : public std::runtime_error {
    public:
        enum class ErrorType {
            FUNCTION_NOT_FOUND,
            TYPE_MISMATCH,
            ARGUMENT_ERROR,
            TIMEOUT_ERROR
        };

        RpcException(ErrorType type, const std::string& message)
            : std::runtime_error(message), type_(type) {}

        ErrorType type() const { return type_; }

    private:
        ErrorType type_;
    };
    class RpcServer {
    public:
        // 存储函数签名信息的结构
        struct FunctionInfo {
            const std::type_info* returnType;
            std::vector<const std::type_info*> paramTypes;
            std::function<std::any(const std::vector<std::any>&)> func;
            std::chrono::milliseconds timeout{5000}; // 默认超时时间：5秒
        };

        // 获取可读的类型名称
        static std::string demangle(const char* name) {
            int status;
            char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
            if (status == 0) {
                std::string result(demangled);
                free(demangled);
                return result;
            }
            return name;
        }

        template <typename Ret, typename... Args>
        void register_function(const std::string& name, 
                            std::function<Ret(Args...)> func,
                            std::chrono::milliseconds timeout = std::chrono::milliseconds(5000)) {
            // 编译时类型检查
            static_assert((std::is_copy_constructible_v<Args> && ...), 
                        "All argument types must be copy constructible");
            static_assert(std::is_copy_constructible_v<Ret> || std::is_void_v<Ret>, 
                        "Return type must be copy constructible");

            FunctionInfo info;
            info.returnType = &typeid(Ret);
            info.paramTypes = {&typeid(Args)...};
            info.timeout = timeout;
            info.func = [func](const std::vector<std::any>& args) -> std::any {
                if (args.size() != sizeof...(Args)) {
                    throw RpcException(
                        RpcException::ErrorType::ARGUMENT_ERROR,
                        "Arguments count mismatch"
                    );
                }
                return call_impl<Ret, Args...>(func, args, std::make_index_sequence<sizeof...(Args)>{});
            };
            functions[name] = std::move(info);
        }

        template <typename Ret, typename... Args>
        Ret call_function(const std::string& name, Args&&... args){
            auto it = functions.find(name);
            if (it == functions.end()) {
                throw RpcException(
                    RpcException::ErrorType::FUNCTION_NOT_FOUND,
                    "Function not found: " + name
                );
            }

            // 类型检查
            if (*it->second.returnType != typeid(Ret)) {
                throw RpcException(
                    RpcException::ErrorType::TYPE_MISMATCH,
                    "Return type mismatch. Expected " + 
                    demangle(it->second.returnType->name()) + 
                    " but got " + demangle(typeid(Ret).name())
                );
            }

            try {
                // 创建异步任务
                auto future = std::async(std::launch::async, [&]() {
                    std::vector<std::any> any_args;
                    any_args.reserve(sizeof...(Args));
                    (any_args.push_back(std::make_any<std::decay_t<Args>>(std::forward<Args>(args))), ...);
                    return it->second.func(any_args);
                });

                // 等待结果，带超时
                if (future.wait_for(it->second.timeout) == std::future_status::timeout) {
                    throw RpcException(
                        RpcException::ErrorType::TIMEOUT_ERROR,
                        "Function call timed out: " + name
                    );
                }

                if constexpr (std::is_void_v<Ret>) {
                    future.get();  // 对于void类型，我们只需要等待执行完成
                    return;
                } else {
                    return std::any_cast<Ret>(future.get());
                }
            } catch (const std::bad_any_cast& e) {
                throw RpcException(
                    RpcException::ErrorType::TYPE_MISMATCH,
                    "Type conversion error: " + std::string(e.what())
                );
            }
        }

        // 设置函数超时时间
        void set_timeout(const std::string& name, std::chrono::milliseconds timeout) {
            auto it = functions.find(name);
            if (it != functions.end()) {
                it->second.timeout = timeout;
            } else {
                throw RpcException(
                    RpcException::ErrorType::FUNCTION_NOT_FOUND,
                    "Function not found: " + name
                );
            }
        }

    private:
        std::unordered_map<std::string, FunctionInfo> functions;
        
        template<typename Ret, typename... Args, typename Func, std::size_t... I>
        static std::any call_impl(Func&& func, const std::vector<std::any>& args, std::index_sequence<I...>) {
            try {
                if constexpr (std::is_void_v<Ret>) {
                    func(std::any_cast<std::decay_t<Args>>(args[I])...);
                    return std::any();
                } else {
                    return func(std::any_cast<std::decay_t<Args>>(args[I])...);
                }
            } catch (const std::bad_any_cast& e) {
                throw RpcException(
                    RpcException::ErrorType::TYPE_MISMATCH,
                    "Parameter type mismatch: " + std::string(e.what())
                );
            }
        }
    };
}

#endif