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
//协程处于HOLD状态无法析构
int main(){
    test_sleep();
    return 0;
}