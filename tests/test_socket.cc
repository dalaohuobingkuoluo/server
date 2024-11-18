#include "../my_sylar/socket.h"
#include "../my_sylar/sylar.h"


static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT;

void test_socket(){
    sylar::IPAddress::ptr addr =  sylar::Address::LookupAnyIPAddress("www.baidu.com", AF_INET, sylar::Socket::TCP);
    if(!addr){
        SYLAR_LOG_ERROR(g_logger) << "get address fail";
        return;
    }else{
        SYLAR_LOG_INFO(g_logger) << "get address = " << addr->toString();
    }
    addr->setPort(80);
    sylar::Socket::ptr sock = sylar::Socket::CreateTCP(addr);
    if(!sock->connect(addr)){
        SYLAR_LOG_ERROR(g_logger) << "CreateTCP fail";
        return;
    }else{
        SYLAR_LOG_INFO(g_logger) << "connected to " << addr->toString();
    }

    const char sendbuf[] = "GET / HTTP/1.0\r\n\r\n";
    int rt = sock->send(sendbuf, sizeof(sendbuf));
    if(rt <= 0){
        SYLAR_LOG_ERROR(g_logger) << "send error, rt = " << rt;
        return;
    }

    std::string recvbuf;
    recvbuf.resize(4096);
    rt = sock->recv(&recvbuf[0], recvbuf.size());
    if(rt <= 0){
        SYLAR_LOG_ERROR(g_logger) << "recv error, rt = " << rt;
        return;
    }
    recvbuf.resize(rt);
    SYLAR_LOG_INFO(g_logger) << "recv: " << recvbuf;
}

int main(){
    sylar::IOManager iom;
    iom.schedule(&test_socket);
    return 0;
}