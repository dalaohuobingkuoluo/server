#ifndef __SYLLAR_BYTEARRAY_H__
#define __SYLAR_BYTEARRAY_H__

#include <memory>
#include <string>
#include <stdint.h>

namespace sylar{

class ByteArray{
public:
    typedef std::shared_ptr<ByteArray> ptr;

    struct Node {
        Node(size_t s);
        Node();
        ~Node();

        char *ptr;
        Node *next;
        size_t size; 
    };

    ByteArray(size_t base_size = 4096);
    ~ByteArray();
    //定长int
    void writeFint8(int8_t v);
    void writeFuint8(uint8_t v);
    void writeFint16(int16_t v);
    void writeFuint16(uint16_t v);
    void writeFint32(int32_t v);
    void writeFuint32(uint32_t v);
    void writeFint64(int64_t v);
    void writeFuint64(uint64_t v);
    //变长int
    void writeInt32(int32_t v);
    void writeUint32(uint32_t v);
    void writeInt64(int64_t v);
    void writeUint64(uint64_t v);

    void writeFloat(const float &v);
    void writeDouble(const double &v);

    void writeStringF16(const std::string &v);
    void writeStringF32(const std::string &v);
    void writeStringF64(const std::string &v);
    void writeStringVint(const std::string &v);
    void writeStringWithoutLen(const std::string &v);


    int8_t readFint8(const int8_t &v);
    uint8_t readFuint8(const uint8_t &v);
    int16_t readFint16(const int16_t &v);
    uint16_t readFuint16(const uint16_t &v);
    int32_t readFint32(const int32_t &v);
    uint32_t readFuint32(const uint32_t &v);
    int64_t readFint64(const int64_t &v);
    uint64_t readFuint64(const uint64_t &v);
    //变长int
    int32_t readInt32(const int32_t &v);
    uint32_t readUint32(const uint32_t &v);
    int64_t readInt64(const int64_t &v);
    uint64_t readUint64(const uint64_t &v);

    float readFloat(const float &v);
    double readDouble(const double &v);

    std::string readStringF16(const std::string &v);
    std::string readStringF32(const std::string &v);
    std::string readStringF64(const std::string &v);
    std::string readStringVint(const std::string &v);

    //内部操作
    void clear();
    void write(const void *buf, size_t s);
    void read(void *buf, size_t s);

    size_t getPosition() const {return m_position;}
    void setPosition(size_t s);
    size_t getBaseSize() const {return m_baseSize;}
    size_t getReadSize() const {return m_size - m_position;}

    bool writeToFile(const std::string &path) const;
    void readFromFile(const std::string &path);

    bool isLittleEndian() const;
    void setIsLittleEndian(bool v);
private:
    void addCapacity(size_t s);
    size_t getCapacity() const {return m_capacity - m_position;}
private:
    size_t m_baseSize;
    size_t m_capacity;
    size_t m_position;
    size_t m_size;
    int8_t m_endian;
    Node *m_root;
    Node *m_cur;
};

}

#endif