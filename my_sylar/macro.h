#ifndef __SYLAR_MACRO_H__
#define __SYLAR_MACRO_H__

#include<string.h>
#include "util.h"
#include<assert.h>

#define SYLAR_ASSERT(x) \
    if(!(x)) { \
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT) << "ASSERTION : " #x \
            << "\nbacktrace" \
            << sylar::BacktraceToString(100,"   \n") << "end"; \
        assert(x); \
    }

#define SYLAR_ASSERT2(x,v) \
    if(!(x)) { \
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT) << "ASSERTION : " #x \
            << "\n" << v\
            << "\nbacktrace" \
            << sylar::BacktraceToString(100,"   \n"); \
        assert(x); \
    }

#endif