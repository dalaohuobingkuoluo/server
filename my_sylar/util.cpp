#include "util.h"
#include <sys/time.h>

namespace sylar{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

pthread_t GetThreadId(){
    return syscall(SYS_gettid);  //sysgettid 获取的是内核中线程ID，而pthread_self 是posix描述的线程ID，在某个进程中是唯一的，在不同进程中创建的线程可能 ID 相同。
}


uint64_t GetFiberId(){
    return sylar::Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string>& bt, int size, int skip){
    void** array = (void**)malloc(sizeof(void*) * size);    
    size_t s = backtrace(array, size);        //array返回程序函数调用的栈帧地址, s为实际的栈帧数
    char** strings = backtrace_symbols(array, size);    //将栈帧地址转换为对地址的描述信息

    if(!strings){
        SYLAR_LOG_ERROR(g_logger) << "backtrace_symbols error";
        return;
    }

    for(size_t i = skip; i < s; ++i){
        bt.push_back(strings[i]);
    }
    free(array);
    free(strings);
}

std::string BacktraceToString(int size, const std::string& prefix, int skip){
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for(const auto& i : bt){
        ss << prefix << i ;
    }
    return ss.str();
}

uint64_t GetCurMS(){
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

uint64_t GetCurUM(){
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 * 1000ul + tv.tv_usec;
}

};




