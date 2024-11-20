#ifndef __SYLLAR_BYTEARRAY_H__
#define __SYLAR_BYTEARRAY_H__

#include <memory>
#include <string>
#include <stdint.h>
#include <sys/socket.h>
#include <vector>

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


    int8_t readFint8();
    uint8_t readFuint8();
    int16_t readFint16();
    uint16_t readFuint16();
    int32_t readFint32();
    uint32_t readFuint32();
    int64_t readFint64();
    uint64_t readFuint64();
    //变长int
    int32_t readInt32();
    uint32_t readUint32();
    int64_t readInt64();
    uint64_t readUint64();

    float readFloat();
    double readDouble();

    std::string readStringF16();
    std::string readStringF32();
    std::string readStringF64();
    std::string readStringVint();

    //内部操作，非const的读写都会修改m_position和m_cur的位置
    void clear();
    void write(const void *buf, size_t s);
    void read(void *buf, size_t s);
    void read(void *buf, size_t s, size_t position) const;

    size_t getPosition() const {return m_position;}
    void setPosition(size_t s);
    size_t getBaseSize() const {return m_baseSize;}
    size_t getReadSize() const {return m_size - m_position;}
    size_t getSize() const {return m_size;}
    //读写文件不修改m_position位置
    bool writeToFile(const std::string &path) const;
    bool readFromFile(const std::string &path);

    bool isLittleEndian() const;
    void setIsLittleEndian(bool v);

    std::string toString() const;
    std::string toHexString() const;

    //仅根据len使得iovec指向可读或写的位置以及设置长度，而非真正写入或读取也不修改m_position
    uint64_t getReadBuffers(std::vector<iovec> &bufs, uint64_t len) const;
    uint64_t getReadBuffers(std::vector<iovec> &bufs, uint64_t len, uint64_t position) const;
    uint64_t getWriteBuffers(std::vector<iovec> &bufs, uint64_t len);       //需要扩容
private:
    void addCapacity(size_t s);
    size_t getCapacity() const {return m_capacity - m_position;}        //获取当前的可写入容量
private:
    size_t m_baseSize;      //每个数据块大小
    size_t m_capacity;      //总容量
    size_t m_position;      //当前操作位置,由于read、write共享该参数，使用时需注意重置防止意外错误
    size_t m_size;          //当前总数据大小
    int8_t m_endian;
    Node *m_root;
    Node *m_cur;
};

}

#endif