#include "../my_sylar/sylar.h"
#include "../my_sylar/tcp_server.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT;

void run(){
    auto addr = sylar::Address::LookupAny("0.0.0.0:8033");
    // sylar::UnixAddress::ptr addr2(new sylar::UnixAddress("/tmp/unix"));
    // SYLAR_LOG_INFO(g_logger) << *addr << " - " << *addr2;
    std::vector<sylar::Address::ptr> addrs, fails;
    addrs.push_back(addr);
    // addrs.push_back(addr2);
    sylar::TcpServer::ptr tcp_server(new sylar::TcpServer());
    while(!tcp_server->bind(addrs, fails)){
        sleep(2);
    }
    tcp_server->start();
}
//bug：unix类型地址在bind时getLocalAddress函数内设置地址长度setAddrLen发生段错误
//solution：在将addr动态转换为UnixAddress类型指针时使用相同的变量名addr，仅变换变量名解决，具体原因还未确定
          //(猜测变量声明与初始化同时发生，在语句中左边的变量声明部分会先执行，导致外部变量addr无法访问从而在转换时发生段错误,并且引用计数值很大)

int main(){
    sylar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}