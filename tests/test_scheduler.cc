#include "../my_sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT;
sylar::Scheduler sc(3, "test", false);

void test_fiber(){
    static int i = 5;
    SYLAR_LOG_INFO(g_logger) << "\n===============\n"
                             << "test in fiber\n"
                             << "i = " << i
                             << "\n===============";
    sleep(20);
    if(--i > 0)
        sc.schedule(&test_fiber, sylar::GetThreadId());
}

void add_fibers(){
    sleep(5);
    sc.schedule(&test_fiber);
    SYLAR_LOG_INFO(g_logger) << sylar::Thread::GetName() << " add a fiber";
}

int main(){
    SYLAR_LOG_INFO(g_logger) << "main run";
    
    // sylar::Thread::ptr thr(new sylar::Thread(add_fibers, "add_fiber_thr"));
    // thr->join();
    sc.start();

    sc.schedule(&test_fiber);
    sc.stop();
    SYLAR_LOG_INFO(g_logger) << "main return";
    return 0;
}