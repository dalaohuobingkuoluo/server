#ifndef __SYLAR_HEEP_SERVER_H__
#define __SYLAR_HTTP_SERVER_H__

#include "http_session.h"
#include "../my_sylar/tcp_server.h"

namespace sylar{
namespace http{

class HttpServer : public TcpServer {
public:
    typedef std::shared_ptr<HttpServer> ptr;
    HttpServer(bool keepalive = false,
               sylar::IOManager* worker = sylar::IOManager::GetThis(),
               sylar::IOManager* accept_worker = sylar::IOManager::GetThis(),
               sylar::IOManager* io_worker = sylar::IOManager::GetThis());
protected:
    virtual void handleClient(sylar::Socket::ptr client) override;
private:
    bool m_isKeepAlive;
};

}
}

#endif