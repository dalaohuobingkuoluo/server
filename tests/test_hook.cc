#include "../my_sylar/sylar.h"
#include "../my_sylar/hook.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT;

void test_sleep(){
    sylar::IOManager iom(1);
    iom.schedule([](){
        sleep(2);
        SYLAR_LOG_INFO(g_logger) << "-----------sleep 2-----------";
    });
    iom.schedule([](){
        sleep(3);
        SYLAR_LOG_INFO(g_logger) << "-----------sleep 3-----------";
    });
    SYLAR_LOG_INFO(g_logger) << "-----------test sleep-----------";
}

//bug：触发断言协程处于HOLD状态无法析构
//scheduler.cpp:176 条件判断写错导致重新将TERM协程状态置为HOLD

int main(){
    test_sleep();
    return 0;
}