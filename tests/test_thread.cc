#include "../my_sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT;
sylar::RWMutex s_mutex;
int count = 0;

void func1(){
    SYLAR_LOG_INFO(g_logger) << "name = " << sylar::Thread::GetName()
                             << ", this name = " << sylar::Thread::GetThis()->getName()
                             << ", id = " << sylar::GetThreadId()
                             << ", this id = " << sylar::Thread::GetThis()->getId();
    for(int i = 0; i < 100; i++){
        sylar::RWMutex::WriteLock lock(s_mutex);
        //std::cout << sylar::Thread::GetName() << "write" << std::endl;
        ++count;
    }
}

void func2(){
    for(int i = 0; i < 1000000; ++i)
        SYLAR_LOG_INFO(g_logger) << "f2xxxxxxxxxxxxxxxxxxxxxx" << i;
}

void func3(){
    for(int i = 0; i < 1000000; ++i)
        SYLAR_LOG_INFO(g_logger) << "f3======================" << i;
}

int main(){ 
    clock_t start = clock();
    SYLAR_LOG_INFO(g_logger) << "test_thread start" << std::endl;
    YAML::Node root = YAML::LoadFile("/home/jerry/mydir/project_server/bin/conf/thread.yml"); 
    sylar::Config::LoadFromYaml(root);
    std::vector<sylar::Thread::ptr> thrs;
    for(int i = 0; i < 2; i++){
        sylar::Thread::ptr thr(new sylar::Thread(&func2, "name_" + std::to_string(i * 2)));
        sylar::Thread::ptr thr2(new sylar::Thread(&func3, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr);
        thrs.push_back(thr2);
    }
    for(auto& i : thrs){
        i->join();
    }
    SYLAR_LOG_INFO(g_logger) << "test_thread end" << std::endl;
    SYLAR_LOG_INFO(g_logger) << "count = " << count << std::endl;
    clock_t end = clock(); 
    double cpu_time_used = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "CPU time used: " << cpu_time_used << " s\n"; 
    return 0;
}