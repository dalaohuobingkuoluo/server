#include "../my_sylar/sylar.h"
#include "../my_sylar/iomanager.h"
#include <sys/types.h>         
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT;

void test_fiber(){
    SYLAR_LOG_INFO(g_logger) << "test fiber";
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_port = htons(80);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "220.181.38.150", &addr.sin_addr.s_addr);

    if(!connect(sock, (sockaddr*)&addr, sizeof(addr))){
    }else if(errno == EINPROGRESS){
        //非阻塞模式下的 EINPROGRESS 错误意味着连接还在进行中，需要等待后续的 WRITE 事件来确认连接是否成功。
        //在建立 TCP 连接时，connect() 调用本身不会触发 EPOLLIN。只有在连接成功并有数据到达时，才会触发 EPOLLIN 事件。
        SYLAR_LOG_INFO(g_logger) << errno << strerror(errno);
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::READ, [](){
            SYLAR_LOG_INFO(g_logger) << "read callback";
        });
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::WRITE, [=](){        
            SYLAR_LOG_INFO(g_logger) << "write callback";   
            close(sock);
        });
        //问题：读写事件先被注册，读事件无法触发；
        //      试图在取消读事件之前sleep等待连接建立优先触发写事件，结果是读事件先被触发
        //      原因：任务调度优先于idle的IO回调
        sylar::IOManager::GetThis()->cancelEvent(sock, sylar::IOManager::READ);
    }else{
        SYLAR_LOG_DEBUG(g_logger) << errno << strerror(errno);
    }
}

void test1(){
    sylar::IOManager iom(2);
    iom.schedule(test_fiber);
}

int main(){
    test1();
    return 0;
}
