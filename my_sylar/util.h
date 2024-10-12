#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include<iostream>
#include<pthread.h>
#include<sys/syscall.h>  
#include<unistd.h>
#include<execinfo.h>
#include<vector>
#include<string>
#include "log.h"
#include "fiber.h"


namespace sylar{

pthread_t GetThreadId();
uint64_t GetFiberId();
void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);
std::string BacktraceToString(int size = 64, const std::string& prefix = "", int skip = 2);

};


#endif