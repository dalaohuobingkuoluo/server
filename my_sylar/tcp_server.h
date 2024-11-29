#ifndef __SYLAR_TCP_SERVER_H__
#define __SYLAR_TCP_SERVER_H__

#include <memory>
#include <functional>
#include "socket.h"
#include "address.h"
#include "noncopyable.h"
#include "iomanager.h"

namespace sylar {

class TcpServer : public std::enable_shared_from_this<TcpServer>, Noncopyable {
public:
    typedef std::shared_ptr<TcpServer> ptr;

    TcpServer(sylar::IOManager* worker = sylar::IOManager::GetThis(),
              sylar::IOManager* accept_worker = sylar::IOManager::GetThis(),
              sylar::IOManager* io_worker = sylar::IOManager::GetThis());
    virtual ~TcpServer();

    virtual bool bind(sylar::Address::ptr addr);
    virtual bool bind(const std::vector<sylar::Address::ptr> &addrs,
                 std::vector<sylar::Address::ptr> &fails);

    virtual bool start();
    virtual void stop();

    uint64_t getRecvTimeout() const {return m_recvTimeout;}
    std::string getName() const {return m_name;}
    bool isStop() const {return m_isStop;}

    void setRecvTimeout(uint64_t v) {m_recvTimeout = v;}
    void setName(const std::string &v) {m_name = v;} 
private:
    virtual void handleClient(sylar::Socket::ptr client);
    virtual void startAccept(sylar::Socket::ptr sock);
private:
    std::vector<sylar::Socket::ptr> m_socks;
    sylar::IOManager* m_worker;
    sylar::IOManager* m_acceptWorker;
    sylar::IOManager* m_ioWorker;
    uint64_t m_recvTimeout;
    std::string m_name;
    bool m_isStop;

};


}

#endif