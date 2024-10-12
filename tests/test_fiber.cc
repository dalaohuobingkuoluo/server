#include "../my_sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT;



void run_in_fiber(){
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber begin";
    sylar::Fiber::GetThis()->YiledToHold();
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber end";
    // sylar::Fiber::GetThis()->YiledToHold();
}

void test_fiber(){
    SYLAR_LOG_INFO(g_logger) << "test_fiber begin0";
    {
        sylar::Fiber::GetThis();
        SYLAR_LOG_INFO(g_logger) << "test_fiber begin";
        sylar::Fiber::ptr fiber(new sylar::Fiber(run_in_fiber));
        fiber->swapIn();
        SYLAR_LOG_INFO(g_logger) << "test_fiber after swapin";
        fiber->swapIn();
        SYLAR_LOG_INFO(g_logger) << "test_fiber end";
        // fiber->swapIn();
    }
    SYLAR_LOG_INFO(g_logger) << "test_fiber end1";
}

int main(){
    sylar::Thread::SetName("main");
    SYLAR_LOG_INFO(g_logger) << "main begin";
    std::vector<sylar::Thread::ptr> thrs;
    for(int i = 0; i < 1; i++){
        sylar::Thread::ptr thr(new sylar::Thread(test_fiber, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }
    for(auto& i : thrs){
        i->join();
    }
    SYLAR_LOG_INFO(g_logger) << "main end";
    return 0;
}






