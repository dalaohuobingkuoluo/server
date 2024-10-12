#include "../my_sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT;



void test(){
    SYLAR_LOG_INFO(g_logger) << sylar::BacktraceToString(10, "   \n");
    SYLAR_ASSERT2(0==1,"asda adf");

}

int main(){
    test();
    return 0;
}