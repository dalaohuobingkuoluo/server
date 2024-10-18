#ifndef __SYLAR_HOOK_H__
#define __SYLAR_HOOK_H__

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace sylar{

bool is_hook_enable();
void set_hook_enable(bool flag);


}

//extern "C"：保证函数采用 C 语言的命名和链接规范，使得这些函数能与 C 编译器链接。
extern "C"{

//sleep
typedef unsigned int(*sleep_fun)(unsigned int seconds);
extern sleep_fun sleep_f;   // 定义了一个名为 sleep_f 的外部变量，它的类型是 sleep_fun 函数指针类型。

typedef int(*usleep_fun)(useconds_t usec);
extern usleep_fun usleep_f;   

typedef int (*nanosleep_fun)(const struct timespec *req, struct timespec *rem);
extern nanosleep_fun nanosleep_f;  

//socket

typedef int (*socket_fun)(int domain, int type, int protocol);
extern socket_fun socket_f;

}

#endif