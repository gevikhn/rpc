#include <iostream>
#include <functional>
#include <unordered_map>
#include <string>
#include <any>
#include <vector>
#include <tuple>

class RpcProvider {
public:
    // 注册函数
    template <typename Ret, typename... Args>
    void register_function(const std::string& name, std::function<Ret(Args...)> func) {
        functions[name] = [func](const std::vector<std::any>& args) -> std::any {
            if (args.size() != sizeof...(Args)) {
                throw std::runtime_error("Arguments count mismatch");
            }
            return call_impl<Ret, Args...>(func, args, std::make_index_sequence<sizeof...(Args)>{});
        };
    }

    // 调用函数
    template <typename Ret, typename... Args>
    Ret call_function(const std::string& name, Args... args) {
        auto it = functions.find(name);
        if (it == functions.end()) {
            throw std::runtime_error("Function not found");
        }

        std::vector<std::any> any_args{std::any(args)...};
        std::any result = it->second(any_args);
        return std::any_cast<Ret>(result);
    }

private:
    std::unordered_map<std::string, std::function<std::any(const std::vector<std::any>&)>> functions;

    // 辅助函数：将参数转换并调用目标函数
    template<typename Ret, typename... Args, typename Func, std::size_t... I>
    static std::any call_impl(Func&& func, const std::vector<std::any>& args, std::index_sequence<I...>) {
        return func(std::any_cast<typename std::tuple_element<I, std::tuple<Args...>>::type>(args[I])...);
    }
};

int main() {
    RpcProvider server;

    // 注册一个函数
    server.register_function<int, int, int>("add", std::function<int(int, int)>([](int a, int b) { return a + b; }));

    // 调用已注册的函数
    int result = server.call_function<int>("add", 3, 5);
    std::cout << "Result: " << result << std::endl;

    return 0;
}
