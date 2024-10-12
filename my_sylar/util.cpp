#include "util.h"


namespace sylar{

sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

pthread_t GetThreadId(){
    return syscall(SYS_gettid);  //sysgettid 获取的是内核中线程ID，而pthread_self 是posix描述的线程ID。
}


uint64_t GetFiberId(){
    return sylar::Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string>& bt, int size, int skip){
    void** array = (void**)malloc(sizeof(void*) * size);
    size_t s = backtrace(array, size);
    char** strings = backtrace_symbols(array, size);

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

};




