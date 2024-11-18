#include "bytearray.h"
#include "endian.h"


namespace sylar{

ByteArray::Node::Node(size_t s)
    : ptr(new char[s]), size(s), next(nullptr) {
}

ByteArray::Node::Node()
    : ptr(nullptr), size(0), next(nullptr) {
}

ByteArray::Node::~Node(){
    if(ptr){
        delete[] ptr;
    }
}
    
ByteArray::ByteArray(size_t base_size)
    : m_baseSize(base_size), m_capacity(base_size),
      m_position(0), m_size(0), m_endian(SYLAR_BIG_ENDIAN),
      m_root(new Node(base_size)), m_cur(m_root) {
}

ByteArray::~ByteArray(){
    Node *tmp = m_root;
    while(tmp){
        m_cur = tmp;
        tmp = tmp->next;
        delete m_cur;
    }
}

bool ByteArray::isLittleEndian() const{
    return m_endian == SYLAR_LITTLE_ENDIAN;
}

void ByteArray::setIsLittleEndian(bool v){
    if(v){
        m_endian = SYLAR_LITTLE_ENDIAN;
    }else{
        m_endian = SYLAR_BIG_ENDIAN;
    }
}

void ByteArray::writeFint8(int8_t v){
    write(&v, sizeof(v));
}

void ByteArray::writeFuint8(uint8_t v){
    write(&v, sizeof(v));
}

void ByteArray::writeFint16(int16_t v){
    if(m_endian != SYLAR_BYTE_ORDER){
        v = byteswap(v);
    }
    write(&v, sizeof(v));
}

void ByteArray::writeFuint16(uint16_t v){
    if(m_endian != SYLAR_BYTE_ORDER){
        v = byteswap(v);
    }
    write(&v, sizeof(v));
}

void ByteArray::writeFint32(int32_t v){
    if(m_endian != SYLAR_BYTE_ORDER){
        v = byteswap(v);
    }
    write(&v, sizeof(v));
}

void ByteArray::writeFuint32(uint32_t v){
    if(m_endian != SYLAR_BYTE_ORDER){
        v = byteswap(v);
    }
    write(&v, sizeof(v));
}

void ByteArray::writeFint64(int64_t v){
    if(m_endian != SYLAR_BYTE_ORDER){
        v = byteswap(v);
    }
    write(&v, sizeof(v));
}

void ByteArray::writeFuint64(uint64_t v){
    if(m_endian != SYLAR_BYTE_ORDER){
        v = byteswap(v);
    }
    write(&v, sizeof(v));
}

void ByteArray::writeInt32(int32_t v){

}

//Varint 编码,用于编码整数（int32, int64, uint32, uint64 等）,采用可变长度的字节序列，每个字节的最高位（MSB）为继续位。
//例： 数值 300 的二进制表示：00000001 00101100  结果：10101100 00000010（共 2 字节）
void ByteArray::writeUint32(uint32_t v){
    uint8_t tmp[5];
    uint8_t i = 0;
    while(v >= 0x80){
        tmp[i++] = (v & 0x7F) | 0x80;
        v >>= 7;
    }
    tmp[i++] = v;
    write(tmp, i);
}

void ByteArray::writeInt64(int64_t v){}
void ByteArray::writeUint64(uint64_t v){}

void ByteArray::writeFloat(const float &v){}
void ByteArray::writeDouble(const double &v){}

void ByteArray::writeStringF16(const std::string &v){}
void ByteArray::writeStringF32(const std::string &v){}
void ByteArray::writeStringF64(const std::string &v){}
void ByteArray::writeStringVint(const std::string &v){}
void ByteArray::writeStringWithoutLen(const std::string &v){}


int8_t ByteArray::readFint8(const int8_t &v){}
uint8_t ByteArray::readFuint8(const uint8_t &v){}
int16_t ByteArray::readFint16(const int16_t &v){}
uint16_t ByteArray::readFuint16(const uint16_t &v){}
int32_t ByteArray::readFint32(const int32_t &v){}
uint32_t ByteArray::readFuint32(const uint32_t &v){}
int64_t ByteArray::readFint64(const int64_t &v){}
uint64_t ByteArray::readFuint64(const uint64_t &v){}
int32_t ByteArray::readInt32(const int32_t &v){}
uint32_t ByteArray::readUint32(const uint32_t &v){}
int64_t ByteArray::readInt64(const int64_t &v){}
uint64_t ByteArray::readUint64(const uint64_t &v){}

float ByteArray::readFloat(const float &v){}
double ByteArray::readDouble(const double &v){}

std::string ByteArray::readStringF16(const std::string &v){}
std::string ByteArray::readStringF32(const std::string &v){}
std::string ByteArray::readStringF64(const std::string &v){}
std::string ByteArray::readStringVint(const std::string &v){}

void ByteArray::clear(){}
void ByteArray::write(const void *buf, size_t s){}
void ByteArray::read(void *buf, size_t s){}

void ByteArray::setPosition(size_t s){}

bool ByteArray::writeToFile(const std::string &path) const{}
void ByteArray::readFromFile(const std::string &path){}


void ByteArray::addCapacity(size_t s){}

} 
