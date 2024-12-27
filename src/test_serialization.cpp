#include "serialization.hpp"
#include <iostream>

void print_buffer(const std::vector<uint8_t>& buffer) {
    printf("Buffer size: %zu\n", buffer.size());
    printf("Data in hex:\n");
    for (size_t i = 0; i < buffer.size(); i++) {
        printf("%02x ", buffer[i]);
        if ((i + 1) % 8 == 0) printf("\n");
    }
    if (buffer.size() % 8 != 0) printf("\n");
}

// 定义一个示例结构体
struct Point {
    float x;
    float y;
    std::string name;
};

// 为Point实现序列化
template<>
struct rpc::serialization::Serializer<Point> {
    static constexpr bool is_serializable = true;
    
    static size_t get_size(const Point& p) {
        return sizeof(p.x) + sizeof(p.y) + sizeof(size_t) + p.name.size();
    }
    
    static void serialize(std::vector<uint8_t>& buffer, const Point& p) {
        size_t old_size = buffer.size();
        size_t name_len = p.name.size();
        buffer.resize(old_size + sizeof(p.x) + sizeof(p.y) + sizeof(size_t) + name_len);
        
        // 写入x和y
        memcpy(buffer.data() + old_size, &p.x, sizeof(p.x));
        old_size += sizeof(p.x);
        
        memcpy(buffer.data() + old_size, &p.y, sizeof(p.y));
        old_size += sizeof(p.y);
        
        // 写入name
        memcpy(buffer.data() + old_size, &name_len, sizeof(size_t));
        old_size += sizeof(size_t);
        
        memcpy(buffer.data() + old_size, p.name.data(), name_len);
    }
};

int main() {
    // 测试基础类型
    uint8_t u8 = 255;
    int8_t s8 = -128;

    uint32_t u32 = 0x12;
    
    // 测试数组类型
    uint8_t bytes[4] = {0x12, 0x34, 0x56, 0x78};
    int16_t shorts[3] = {1000, -2000, 3000};
    float floats[2] = {3.14f, -2.718f};

    //生成随机数
    srand(time(nullptr));
    int rand_size = rand() % 100;
    uint8_t *rand_bytes = new uint8_t[rand_size];
    for (int i = 0; i < rand_size; i++) {
        rand_bytes[i] = rand() % 256;
    }
    
    // 测试vector
    std::vector<uint16_t> vec = {100, 200, 300, 400};
    
    // 测试结构体
    Point p{1.5f, 2.5f, "test point"};
    
    // 序列化所有类型
    auto buffer = rpc::serialization::serialize<
        uint8_t, int8_t, 
        uint8_t(&)[4], int16_t(&)[3], float(&)[2],
        std::vector<uint16_t>, Point, uint32_t
    >(u8, s8, bytes, shorts, floats, vec, p, u32);

    print_buffer(buffer);

    using namespace rpc::serialization;
    // 模拟序列化的字节流
    std::vector<uint8_t> buffer1 = {
        static_cast<uint8_t>(DataType::UINT32), 0x2A, 0x00, 0x00, 0x00,  // uint32_t 42
        static_cast<uint8_t>(DataType::STRING), 0x0D, 0x00, 0x00, 0x00, 'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!', // "Hello, world!"
        static_cast<uint8_t>(DataType::VECTOR_FLOAT32), 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x40, 0x40
    };


    Deserializer deserializer(buffer1);

    // 反序列化
    uint32_t num = deserializer.deserialize_value<uint32_t>();
    std::string text = deserializer.deserialize_value<std::string>();
    std::vector<float> vec1 = deserializer.deserialize_vector<float>();

    // 输出反序列化结果
    std::cout << "Number: " << num << std::endl;
    std::cout << "Text: " << text << std::endl;
    std::cout << "Vector: ";
    for (float v : vec1) {
        std::cout << v << " ";
    }
    std::cout << std::endl;

    Deserializer deserializer1(buffer1);

    // 反序列化
    auto result = deserializer1.deserialize_tuple<uint32_t, std::string, std::vector<float>>();

    // 输出反序列化结果
    std::cout << "Number: " << std::get<0>(result) << std::endl;
    std::cout << "Text: " << std::get<1>(result) << std::endl;
    std::cout << "Vector: ";
    for (float v : std::get<2>(result)) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
