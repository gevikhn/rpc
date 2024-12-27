#include "serialization.hpp"

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
    
    // 测试数组类型
    uint8_t bytes[4] = {0x12, 0x34, 0x56, 0x78};
    int16_t shorts[3] = {1000, -2000, 3000};
    float floats[2] = {3.14f, -2.718f};
    
    // 测试vector
    std::vector<uint16_t> vec = {100, 200, 300, 400};
    
    // 测试结构体
    Point p{1.5f, 2.5f, "test point"};
    
    // 序列化所有类型
    auto buffer = rpc::serialization::serialize<
        uint8_t, int8_t, 
        uint8_t(&)[4], int16_t(&)[3], float(&)[2],
        std::vector<uint16_t>, Point
    >(u8, s8, bytes, shorts, floats, vec, p);

    print_buffer(buffer);
    
    return 0;
}
