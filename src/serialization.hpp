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
#include <cstring>
#include <cstdio>
#include <cstdint>
#include <type_traits>

namespace rpc {
namespace serialization {
        // 定义类型标识
        enum class DataType : uint8_t {
            // 基础类型
            UINT8 = 1,
            SINT8 = 2,
            UINT16 = 3,
            SINT16 = 4,
            UINT32 = 5,
            SINT32 = 6,
            UINT64 = 7,
            SINT64 = 8,
            FLOAT32 = 9,
            FLOAT64 = 10,
            STRING = 11,
            
            // 数组类型
            ARRAY_UINT8 = 21,
            ARRAY_SINT8 = 22,
            ARRAY_UINT16 = 23,
            ARRAY_SINT16 = 24,
            ARRAY_UINT32 = 25,
            ARRAY_SINT32 = 26,
            ARRAY_UINT64 = 27,
            ARRAY_SINT64 = 28,
            ARRAY_FLOAT32 = 29,
            ARRAY_FLOAT64 = 30,

            // 动态数组类型
            VECTOR_UINT8 = 41,
            VECTOR_SINT8 = 42,
            VECTOR_UINT16 = 43,
            VECTOR_SINT16 = 44,
            VECTOR_UINT32 = 45,
            VECTOR_SINT32 = 46,
            VECTOR_UINT64 = 47,
            VECTOR_SINT64 = 48,
            VECTOR_FLOAT32 = 49,
            VECTOR_FLOAT64 = 50,

            // 自定义类型
            STRUCT = 100
        };

        // 序列化接口
        template<typename T>
        struct Serializer {
            static constexpr bool is_serializable = false;
        };

        // 标记一个结构体为可序列化
        template<typename T>
        constexpr bool is_serializable_v = Serializer<T>::is_serializable;

        // 检查是否为std::array类型
        template<typename T>
        struct is_std_array : std::false_type {};

        template<typename T, std::size_t N>
        struct is_std_array<std::array<T, N>> : std::true_type {};

        template<typename T>
        inline constexpr bool is_std_array_v = is_std_array<T>::value;

        // 检查是否为std::vector类型
        template<typename T>
        struct is_std_vector : std::false_type {};

        template<typename T>
        struct is_std_vector<std::vector<T>> : std::true_type {};

        template<typename T>
        inline constexpr bool is_std_vector_v = is_std_vector<T>::value;

        // 获取数组元素类型
        template<typename T>
        struct array_traits {
            using element_type = typename std::remove_extent_t<T>;
            static constexpr size_t size(const T& arr) { return std::extent_v<T>; }
            static const element_type* data(const T& arr) { return arr; }
        };

        template<typename T, size_t N>
        struct array_traits<std::array<T, N>> {
            using element_type = T;
            static constexpr size_t size(const std::array<T, N>& arr) { return N; }
            static const T* data(const std::array<T, N>& arr) { return arr.data(); }
        };

        template<typename T>
        struct array_traits<std::vector<T>> {
            using element_type = T;
            static size_t size(const std::vector<T>& vec) { return vec.size(); }
            static const T* data(const std::vector<T>& vec) { return vec.data(); }
        };

        // 获取数组元素类型对应的DataType
        template<typename T>
        constexpr DataType get_array_data_type() {
            if constexpr (std::is_same_v<T, uint8_t>) return DataType::ARRAY_UINT8;
            else if constexpr (std::is_same_v<T, int8_t>) return DataType::ARRAY_SINT8;
            else if constexpr (std::is_same_v<T, uint16_t>) return DataType::ARRAY_UINT16;
            else if constexpr (std::is_same_v<T, int16_t>) return DataType::ARRAY_SINT16;
            else if constexpr (std::is_same_v<T, uint32_t>) return DataType::ARRAY_UINT32;
            else if constexpr (std::is_same_v<T, int32_t>) return DataType::ARRAY_SINT32;
            else if constexpr (std::is_same_v<T, uint64_t>) return DataType::ARRAY_UINT64;
            else if constexpr (std::is_same_v<T, int64_t>) return DataType::ARRAY_SINT64;
            else if constexpr (std::is_same_v<T, float>) return DataType::ARRAY_FLOAT32;
            else if constexpr (std::is_same_v<T, double>) return DataType::ARRAY_FLOAT64;
            else static_assert(!std::is_same_v<T,T>, "Unsupported array element type");
        }

        // 获取vector元素类型对应的DataType
        template<typename T>
        constexpr DataType get_vector_data_type() {
            if constexpr (std::is_same_v<T, uint8_t>) return DataType::VECTOR_UINT8;
            else if constexpr (std::is_same_v<T, int8_t>) return DataType::VECTOR_SINT8;
            else if constexpr (std::is_same_v<T, uint16_t>) return DataType::VECTOR_UINT16;
            else if constexpr (std::is_same_v<T, int16_t>) return DataType::VECTOR_SINT16;
            else if constexpr (std::is_same_v<T, uint32_t>) return DataType::VECTOR_UINT32;
            else if constexpr (std::is_same_v<T, int32_t>) return DataType::VECTOR_SINT32;
            else if constexpr (std::is_same_v<T, uint64_t>) return DataType::VECTOR_UINT64;
            else if constexpr (std::is_same_v<T, int64_t>) return DataType::VECTOR_SINT64;
            else if constexpr (std::is_same_v<T, float>) return DataType::VECTOR_FLOAT32;
            else if constexpr (std::is_same_v<T, double>) return DataType::VECTOR_FLOAT64;
            else static_assert(!std::is_same_v<T,T>, "Unsupported vector element type");
        }

        // 获取类型对应的DataType
        template<typename T>
        constexpr DataType get_data_type() {
            if constexpr (std::is_same_v<T, uint8_t>) return DataType::UINT8;
            else if constexpr (std::is_same_v<T, int8_t>) return DataType::SINT8;
            else if constexpr (std::is_same_v<T, uint16_t>) return DataType::UINT16;
            else if constexpr (std::is_same_v<T, int16_t>) return DataType::SINT16;
            else if constexpr (std::is_same_v<T, uint32_t>) return DataType::UINT32;
            else if constexpr (std::is_same_v<T, int32_t>) return DataType::SINT32;
            else if constexpr (std::is_same_v<T, uint64_t>) return DataType::UINT64;
            else if constexpr (std::is_same_v<T, int64_t>) return DataType::SINT64;
            else if constexpr (std::is_same_v<T, float>) return DataType::FLOAT32;
            else if constexpr (std::is_same_v<T, double>) return DataType::FLOAT64;
            else if constexpr (std::is_same_v<T, std::string>) return DataType::STRING;
            else if constexpr (is_serializable_v<T>) return DataType::STRUCT;
            else static_assert(!std::is_same_v<T,T>, "Unsupported type");
        }

        // 基础类型的序列化大小
        template<typename T>
        size_t get_serialized_size(const T& value) {
            if constexpr (std::is_arithmetic_v<T>) {
                return sizeof(DataType) + sizeof(T);  // type + data
            } else if constexpr (std::is_same_v<T, std::string>) {
                return sizeof(DataType) + sizeof(size_t) + value.size();  // type + length + data
            } else if constexpr (std::is_array_v<T> || is_std_array_v<T>) {
                using Traits = array_traits<T>;
                return sizeof(DataType) + sizeof(uint32_t) + 
                       Traits::size(value) * sizeof(typename Traits::element_type);  // type + length + data
            } else if constexpr (is_std_vector_v<T>) {
                using Traits = array_traits<T>;
                return sizeof(DataType) + sizeof(uint32_t) + 
                       Traits::size(value) * sizeof(typename Traits::element_type);  // type + length + data
            } else if constexpr (is_serializable_v<T>) {
                return sizeof(DataType) + Serializer<T>::get_size(value);  // type + data
            } else {
                static_assert(!std::is_same_v<T,T>, "Unsupported type");
                return 0;
            }
        }

        // 获取参数包的总序列化大小
        template<typename... Args>
        size_t get_total_size(const Args&... args) {
            return (get_serialized_size(args) + ...);
        }

        // 序列化单个值到vector
        template<typename T>
        void serialize_value(std::vector<uint8_t>& buffer, const T& value) {
            if constexpr (std::is_arithmetic_v<T>) {
                DataType type = get_data_type<T>();
                size_t old_size = buffer.size();
                buffer.resize(old_size + sizeof(DataType) + sizeof(T));
                memcpy(buffer.data() + old_size, &type, sizeof(DataType));
                memcpy(buffer.data() + old_size + sizeof(DataType), &value, sizeof(T));
            } else if constexpr (std::is_same_v<T, std::string>) {
                DataType type = DataType::STRING;
                size_t len = value.size();
                size_t old_size = buffer.size();
                buffer.resize(old_size + sizeof(DataType) + sizeof(size_t) + len);
                memcpy(buffer.data() + old_size, &type, sizeof(DataType));
                memcpy(buffer.data() + old_size + sizeof(DataType), &len, sizeof(size_t));
                memcpy(buffer.data() + old_size + sizeof(DataType) + sizeof(size_t), value.data(), len);
            } else if constexpr (std::is_array_v<T> || is_std_array_v<T>) {
                using Traits = array_traits<T>;
                using ElementType = typename Traits::element_type;
                
                DataType type = get_array_data_type<ElementType>();
                uint32_t len = Traits::size(value);
                
                size_t old_size = buffer.size();
                buffer.resize(old_size + sizeof(DataType) + sizeof(uint32_t) + len * sizeof(ElementType));
                
                // 写入类型
                memcpy(buffer.data() + old_size, &type, sizeof(DataType));
                old_size += sizeof(DataType);
                
                // 写入长度
                memcpy(buffer.data() + old_size, &len, sizeof(uint32_t));
                old_size += sizeof(uint32_t);
                
                // 写入数据
                memcpy(buffer.data() + old_size, Traits::data(value), len * sizeof(ElementType));
            } else if constexpr (is_std_vector_v<T>) {
                using Traits = array_traits<T>;
                using ElementType = typename Traits::element_type;
                
                DataType type = get_vector_data_type<ElementType>();
                uint32_t len = Traits::size(value);
                
                size_t old_size = buffer.size();
                buffer.resize(old_size + sizeof(DataType) + sizeof(uint32_t) + len * sizeof(ElementType));
                
                // 写入类型
                memcpy(buffer.data() + old_size, &type, sizeof(DataType));
                old_size += sizeof(DataType);
                
                // 写入长度
                memcpy(buffer.data() + old_size, &len, sizeof(uint32_t));
                old_size += sizeof(uint32_t);
                
                // 写入数据
                memcpy(buffer.data() + old_size, Traits::data(value), len * sizeof(ElementType));
            } else if constexpr (is_serializable_v<T>) {
                DataType type = DataType::STRUCT;
                size_t old_size = buffer.size();
                buffer.resize(old_size + sizeof(DataType));
                memcpy(buffer.data() + old_size, &type, sizeof(DataType));
                Serializer<T>::serialize(buffer, value);
            } else {
                static_assert(!std::is_same_v<T,T>, "Unsupported type");
            }
        }

        template<typename ...Args>
        std::vector<uint8_t> serialize(Args... args) {
            // 计算总大小
            size_t total_size = get_total_size(args...);
            printf("Total size: %zu\n", total_size);
            
            // 创建vector并预分配空间
            std::vector<uint8_t> buffer;
            buffer.reserve(total_size);
            
            // 序列化每个参数
            (serialize_value(buffer, args), ...);
            
            return buffer;
        }
    }
}