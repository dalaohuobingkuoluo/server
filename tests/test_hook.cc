#include "../my_sylar/sylar.h"
#include "../my_sylar/hook.h"
#include <sys/types.h>         
#include <sys/socket.h>
#include <arpa/inet.h>

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

void test_sock(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_port = htons(80);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "220.181.38.150", &addr.sin_addr.s_addr);

    SYLAR_LOG_INFO(g_logger) << "begin connect";
    int rt = connect(sock, (sockaddr*)&addr, sizeof(addr));
    SYLAR_LOG_INFO(g_logger) << "connect rt = " << rt << " errno = " << errno;
    if(rt){
        return;
    }
    // SYLAR_LOG_DEBUG(g_logger) << "test_sock, " << "scok = " << sock 
    //                                      << " trylock " << sylar::IOManager::GetThis()->fdIsLock(sock);

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    SYLAR_LOG_INFO(g_logger) << "send rt = " << rt << " errno = " << errno;
    if(rt <= 0){
        return;
    }

    std::string buff;
    buff.resize(4096);
    rt = recv(sock, &buff[0], buff.size(), 0);
    SYLAR_LOG_INFO(g_logger) << "recv rt = " << rt << " errno = " << errno;
    if(rt <= 0){
        return;
    }
    buff.resize(rt);
    SYLAR_LOG_INFO(g_logger) << buff;
}
//bug：connect_with_timeout在yield前后发生FdContext上锁导致recv执行do_io addEvent时发生死锁，调试模式下可能由于调试器介入
//     改变调度时间未触发竟态，从而无法定位错误位置
//solve：借助回调机制以及观察者模式，在实现锁的类中添加与ConfigVar中on_change_cb相同机制的锁状态变化回调监听函数，
//       在合适的位置（hook.cpp:268）注册监听回调函数，当锁状态发生变化时通知所有监听函数查看函数调用栈定位错误。
//       错误出现在iomanager.cpp：idle函数332行为fd_ctx加锁后没有解锁直接协程切换导致死锁


int main(){
    // test_sleep();
    sylar::IOManager iom;
    iom.schedule(test_sock);
    return 0;
}