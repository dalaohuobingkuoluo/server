#ifndef __SYLAR_ENDIAN_H__
#define __SYLAR_ENDIAN_H__

#define SYLAR_LITTLE_ENDIAN 1
#define SYLAR_BIG_ENDIAN 2

#include <byteswap.h>
#include <stdint.h>
#include <iostream>

namespace sylar{

//std::enable_if 通过第一个模板参数 condition 来控制第二个模板参数 type 的可用性
//如果 condition 为 true，std::enable_if 会定义一个名为 ::type 的成员类型，其值为第二个模板参数 type；如果 condition 为 false，那么 ::type 不存在
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
byteswap(T v){
    return (T)bswap_64((uint64_t)v);
}

template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
byteswap(T v){
    return (T)bswap_32((uint32_t)v);
}

template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T v){
    return (T)bswap_16((uint16_t)v);
}

#if BYTE_ORDER == BIG_ENDIAN            //系统宏，定义系统的字节序信息
#define SYLAR_BYTE_ORDER SYLAR_BIG_ENDIAN
#else
#define SYLAR_BYTE_ORDER SYLAR_LITTLE_ENDIAN
#endif

#if SYLAR_BYTE_ORDER == SYLAR_BIG_ENDIAN

template<class T>
T byteswapOnLittleEndian(T t){
    return t;
}
//表示在小端机器执行bswap转成大端表示，由于运行环境为大端实际无需操作

template<class T>
T byteswapOnBigEndian(T t){
    return byteswap(t);
}

#else

template<class T>
T byteswapOnLittleEndian(T t){
    return byteswap(t);
}

template<class T>
T byteswapOnBigEndian(T t){
    return t;
}

#endif


}

#endif