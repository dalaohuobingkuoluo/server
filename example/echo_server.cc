#include "../my_sylar/iomanager.h"
#include "../my_sylar/tcp_server.h"
#include "../my_sylar/log.h"
#include "../my_sylar/bytearray.h"
#include "../my_sylar/address.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT;

class EchoServer : public sylar::TcpServer {
public:
    EchoServer(int type);
    void handleClient(sylar::Socket::ptr client);
private:
    int m_type = 0;  //0:bit 1：text
};


EchoServer::EchoServer(int type) : m_type(type){
}
    
void EchoServer::handleClient(sylar::Socket::ptr client){
    SYLAR_LOG_INFO(g_logger) << "handleClient";
    sylar::ByteArray::ptr ba(new sylar::ByteArray());
    while(1){
        ba->clear();
        std::vector<iovec> iovs;
        ba->getWriteBuffers(iovs, 1024);

        int rt = client->recv(&iovs[0], iovs.size());
        if(rt == 0){
            SYLAR_LOG_INFO(g_logger) << "client close: " << client->toString();
            break;
        }else if(rt < 0){
            SYLAR_LOG_ERROR(g_logger) << "client error rt = " << rt << ", errno = " << errno
                                      << ", strerror = " << strerror(errno);
            break;
        }
        ba->setPosition(ba->getPosition() + rt);
        ba->setPosition(0);
        // SYLAR_LOG_INFO(g_logger) << "iov = " << std::string((char*)iovs[0].iov_base, rt);
        if(m_type == 1) {
            std::cout << ba->toString();
        }else{
            std::cout << ba->toHexString();
        }
        std::cout << std::endl;
    }
}

int type = 1;

void run(){
    EchoServer::ptr es(new EchoServer(type));
    auto addr = sylar::Address::LookupAny("0.0.0.0:8020");
    while(!es->bind(addr)){
        sleep(2);
    }
    es->start();
}

int main(int argc, char** argv){
    if(argc < 2){
        SYLAR_LOG_INFO(g_logger) << "use as[" << argv[0] << " -t] or [" << argv[0] << " -b]";
        return 0;
    }
    if(strcmp(argv[1], "-t") == 0){
        type = 1;
    }else{
        type = 0;
    }
    sylar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}