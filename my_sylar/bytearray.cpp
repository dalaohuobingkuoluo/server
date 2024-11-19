#include "bytearray.h"
#include "endian.h"
#include <string.h>
#include <fstream>
#include <iomanip>
#include "log.h"

namespace sylar{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

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

//ZigZag 编码将负数转换为无符号整数以便用更高效的 Varint 编码来表示。
template<class T>
static typename std::make_unsigned<T>::type EncodeZigzag(const T &v){
    static_assert(std::is_signed<T>::value, "class T must be a signed type");
    using UnsignedT = std::make_unsigned<T>::type;
    if(v < 0){
        return (UnsignedT)(-v) * 2 - 1;
    }else{
        return (UnsignedT)v * 2;
    }
    //return (UnsignedT)((v << 1) ^ (v >> 31))
}

void ByteArray::writeInt32(int32_t v){
    writeUint32(EncodeZigzag(v));
}

//Varint 编码不需要考虑字节序,用于编码整数（int32, int64, uint32, uint64 等）,采用可变长度的字节序列，每个字节的最高位（MSB）为继续位。
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

void ByteArray::writeInt64(int64_t v){
    writeUint64(EncodeZigzag(v));
}

void ByteArray::writeUint64(uint64_t v){
    uint8_t tmp[10];
    uint8_t i = 0;
    while(v >= 0x80){
        tmp[i++] = (v & 0x7F) | 0x80;
        v >>= 7;
    }
    tmp[i++] = v;
    write(tmp, i);
}

//float和double内存视为定长int存储
void ByteArray::writeFloat(const float &v){
    uint32_t u;
    memcpy(&u, &v, sizeof(v));
    writeFint32(u);
}
void ByteArray::writeDouble(const double &v){
    uint64_t u;
    memcpy(&u, &v, sizeof(v));
    writeFint64(u);
}

void ByteArray::writeStringF16(const std::string &v){
    writeFint16(v.size());
    write(v.c_str(), v.size());
}

void ByteArray::writeStringF32(const std::string &v){
    writeFint32(v.size());
    write(v.c_str(), v.size());
}

void ByteArray::writeStringF64(const std::string &v){
    writeFint64(v.size());
    write(v.c_str(), v.size());
}

void ByteArray::writeStringVint(const std::string &v){
    writeUint64(v.size());
    write(v.c_str(), v.size());
}

void ByteArray::writeStringWithoutLen(const std::string &v){
    write(v.c_str(), v.size());
}

int8_t ByteArray::readFint8(){
    int8_t v;
    read(&v, sizeof(v));
    return v;
}

uint8_t ByteArray::readFuint8(){
    uint8_t v;
    read(&v, sizeof(v));
    return v;
}

#define XX(type)\
    type v;\
    read(&v, sizeof(v));\
    if(m_endian == SYLAR_BYTE_ORDER){\
        return v;\
    }else{\
        return byteswap(v);\
    }

int16_t ByteArray::readFint16(){
    XX(int16_t)
}

uint16_t ByteArray::readFuint16(){
    XX(uint16_t)
}

int32_t ByteArray::readFint32(){
    XX(int32_t)
}

uint32_t ByteArray::readFuint32(){
    XX(uint32_t)
}

int64_t ByteArray::readFint64(){
    XX(int64_t)
}

uint64_t ByteArray::readFuint64(){
    XX(uint64_t)
}

#undef XX

template<class T>
static typename std::make_signed<T>::type DecodeZigzag(const T &v){
    static_assert(std::is_unsigned<T>::value, "class T must be a unsigned type");
    using SignedT = std::make_signed<T>::type;
    return (SignedT)((v >> 1) ^ -(v & 1));
}

int32_t ByteArray::readInt32(){
    return DecodeZigzag(readUint32());
}

uint32_t ByteArray::readUint32(){
    uint32_t res = 0;
    for(uint8_t i = 0; i < 32; i += 7){
        uint8_t tmp = readFint8();
        if(tmp < 0x80){
            res |= ((uint32_t)tmp) << i;
            break;
        }else{
            res |= ((uint32_t)(tmp & 0x7F)) << i; 
        }
    }
    return res;
}

int64_t ByteArray::readInt64(){
    return DecodeZigzag(readUint64());
}

uint64_t ByteArray::readUint64(){
    uint64_t res = 0;
    for(uint8_t i = 0; i < 64; i += 7){
        uint8_t tmp = readFint8();
        if(tmp < 0x80){
            res |= ((uint64_t)tmp) << i;
            break;
        }else{
            res |= ((uint64_t)(tmp & 0x7F)) << i; 
        }
    }
    return res;
}

float ByteArray::readFloat(){
    uint32_t v = readFint32();
    float res;
    memcpy(&res, &v, sizeof(v));
    return res;
}

double ByteArray::readDouble(){
    uint64_t v = readFint64();
    double res;
    memcpy(&res, &v, sizeof(v));
    return res;
}

std::string ByteArray::readStringF16(){
    uint16_t len = readFuint16();
    std::string res;
    res.resize(len);
    read(&res[0], len);
    return res;
}

std::string ByteArray::readStringF32(){
    uint32_t len = readFuint32();
    std::string res;
    res.resize(len);
    read(&res[0], len);
    return res;
}

std::string ByteArray::readStringF64(){
    uint64_t len = readFuint64();
    std::string res;
    res.resize(len);
    read(&res[0], len);
    return res;
}

std::string ByteArray::readStringVint(){
    uint64_t len = readUint64();
    std::string res;
    res.resize(len);
    read(&res[0], len);
    return res;
}

void ByteArray::clear(){
    m_position = m_size = 0;
    m_capacity = m_baseSize;
    Node *tmp = m_root->next;
    while(tmp){
        m_cur = tmp->next;
        delete tmp;
        tmp = m_cur;
    }
    m_cur = m_root;
    m_root->next = nullptr; 
}

size_t ByteArray::getReadSize() const{
    size_t rds = m_size - m_position;
    size_t real_rds = 0;
    Node *tmp = m_cur;
    while(tmp->next){
        real_rds += tmp->size;
        tmp = tmp->next;
    }
    if(real_rds == rds){
        return real_rds;
    }else{
        SYLAR_LOG_ERROR(g_logger) << "getReadSize fail, read_size = " << rds 
                                  << ", real_read_size = " << real_rds << ", Node chain error";
        throw std::runtime_error("Node chain error in getReadSize");
    }
}

void ByteArray::write(const void *buf, size_t s){
    if(s == 0){
        return;
    }
    addCapacity(s);
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;
    while(s > 0){
        if(s <= ncap){
            memcpy(m_cur->ptr + npos, (const char*)buf + bpos, s);
            if(npos + s == m_cur->size){
                m_cur = m_cur->next;
            }
            m_position += s;
            s = 0;
        }else{
            memcpy(m_cur->ptr + npos, (const char*)buf + bpos, ncap);
            npos = 0;
            bpos += ncap;
            m_position += ncap;
            s -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
        }
    }
    if(m_position >= m_size){
        m_size = m_position;
    }
}

void ByteArray::read(void *buf, size_t s){
    if(s == 0){
        return;
    }
    if(s > getReadSize()){
        throw std::out_of_range("not enough len to read");
    }
    if(!m_cur){
        SYLAR_LOG_ERROR(g_logger) << "read error, m_cur = nullptr but want to read size = " << s;
        return;
    }
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;
    while(s > 0){
        if(s <= ncap){
            memcpy((char*)buf + bpos, m_cur->ptr + npos, s);
            if(npos + s == m_cur->size){
                m_cur = m_cur->next;
            }
            m_position += s;
            s = 0;
        }else{
            memcpy((char*)buf + bpos, m_cur->ptr + npos, ncap);
            npos = 0;
            bpos += ncap;
            m_position += ncap;
            s -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
        }
    }
}

void ByteArray::read(void *buf, size_t s, size_t position) const{
    if(s == 0){
        return;
    }
    if(s > getReadSize()){
        throw std::out_of_range("not enough len to read");
    }
    if(!m_cur){
        SYLAR_LOG_ERROR(g_logger) << "read error, m_cur = nullptr but want to read size = " << s;
        return;
    }
    Node *cur = m_cur;
    size_t npos = position % m_baseSize;
    size_t ncap = cur->size - npos;
    size_t bpos = 0;
    while(s > 0){
        if(s <= ncap){
            memcpy((char*)buf + bpos, cur->ptr + npos, s);
            if(npos + s == cur->size){
                if (!cur->next) {
                    throw std::runtime_error("Unexpected end of chain while read");
                }
                cur = cur->next;
            }
            position += s;
            s = 0;
        }else{
            memcpy((char*)buf + bpos, cur->ptr + npos, ncap);
            npos = 0;
            bpos += ncap;
            position += ncap;
            s -= ncap;
            cur = cur->next;
            ncap = cur->size;
        }
    }
}

void ByteArray::setPosition(size_t s){
    if(s > m_size){
        throw std::out_of_range("setPosition out of range");
    }
    m_position = s;
    m_cur = m_root;
    while(m_cur && s >= m_cur->size){
        s -= m_cur->size;
        m_cur = m_cur->next;
    }
}

bool ByteArray::writeToFile(const std::string &path) const{
    std::ofstream ofs;
    ofs.open(path, std::ios::trunc | std::ios::binary);
    if(!ofs){
        SYLAR_LOG_ERROR(g_logger) << "writeToFile path = " << path << " fail, errno = "
                                  << errno << ", strerr = " << strerror(errno);
        return false;
    }
    size_t read_size = getReadSize();
    size_t pos = m_position;
    Node *cur = m_cur;
    while(read_size > 0){
        size_t diff = pos % m_baseSize;
        size_t len = (read_size > m_cur->size ? m_cur->size : read_size) - diff;
        ofs.write(cur->ptr + diff, len);
        cur = cur->next;
        read_size -= len;
        pos += len;
    }
    return true;
}

bool ByteArray::readFromFile(const std::string &path){
    std::ifstream ifs;
    ifs.open(path, std::ios::binary);  
    if(!ifs){
        SYLAR_LOG_ERROR(g_logger) << "readFromFile path = " << path << " fail, errno = "
                                  << errno << ", strerr = " << strerror(errno);
        return false;
    }  
    std::shared_ptr<char> buf(new char[m_baseSize], [](char* ptr){delete[] ptr;});
    while(!ifs.eof()){
        ifs.read(buf.get(), m_baseSize);
        write(buf.get(), ifs.gcount());     //gcount获取最后一次输入操作实际读取的字符数
    }
    return true;
}

void ByteArray::addCapacity(size_t s){
    if(s == 0){
        return;
    }
    size_t old_cap = getCapacity();
    if(s <= old_cap){
        return;
    }
    s -= old_cap;
    size_t count = (s + m_baseSize - 1) / m_baseSize;
    Node *first = nullptr, *tmp = m_root;
    while(tmp->next){
        tmp = tmp->next;
    }
    for(size_t i = 0; i < count; ++i){
        tmp->next = new Node(m_baseSize);
        if(!first){
            first = tmp->next;
        }
        tmp = tmp->next;
        m_capacity += m_baseSize;
    }
    if(old_cap == 0){
        m_cur = first;
    }
}

std::string ByteArray::toString() const { 
    std::string str;
    str.resize(getReadSize());
    if(str.empty()){
        return str;
    }
    read(&str[0], str.size(), m_position);
    return str;
}

std::string ByteArray::toHexString() const {
    std::string str = toString();
    std::stringstream ss;
    for(size_t i = 0; i < str.size(); ++i){
        if(i && i % 32 == 0){
            ss << std::endl;
        }
        ss << std::setw(2) << std::setfill('0') << std::hex 
           << (int)(uint8_t)str[i] << " ";
    }
}

} 
