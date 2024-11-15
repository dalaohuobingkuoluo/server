#ifndef __SYLAR_MACRO_H__
#define __SYLAR_MACRO_H__

#include<string.h>
#include "util.h"
#include<assert.h>

#if defined __GNUC__ || defined __llvm__              
#   define SYLAR_LIKELY(x)       __builtin_expect(!!(x), 1)
#   define SYLAR_UNLIKELY(x)     __builtin_expect(!!(x), 0)
#else
#   define SYLAR_LIKELY(x)       (x)
#   define SYLAR_UNLIKELY(x)     (x)
#endif
//用于对条件分支预测进行编译器优化提示
//__GNUC__宏定义表示当前编译器是 GCC 或与 GCC 兼容的编译器, __llvm__宏定义表示当前编译器是 LLVM（如 Clang）。
//__builtin_expect 是 GCC 和 LLVM 提供的一个内置函数，用于提示分支预测的可能性。

 
#define SYLAR_ASSERT(x) \
    if(SYLAR_UNLIKELY(!(x))) { \
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT) << "ASSERTION : " #x \
            << "\nbacktrace" \
            << sylar::BacktraceToString(100,"   \n") << "end"; \
        assert(x); \
    }

#define SYLAR_ASSERT2(x,v) \
    if(SYLAR_UNLIKELY(!(x))) { \
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT) << "ASSERTION : " #x \
            << "\n" << v\
            << "\nbacktrace" \
            << sylar::BacktraceToString(100,"   \n"); \
        assert(x); \
    }

#endif