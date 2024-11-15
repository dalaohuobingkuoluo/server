#ifndef __SYLAR_SOCKET_H__
#define __SYLAR_SOCKET_H__

#include <memory>
#include "address.h"
#include "fd_manager.h"
#include "noncopyable.h"

namespace sylar{

class Socket : public std::enable_shared_from_this<Socket>, Noncopyable {
public:
    typedef std::shared_ptr<Socket> ptr;
    typedef std::weak_ptr<Socket> wptr;

    Socket(int family, int type, int protocol = 0);
    ~Socket();

    uint64_t getRecvTimeout();
    bool setRecvTimeout(uint64_t v);
    uint64_t getSendTimeout();
    bool setSendTimeout(uint64_t v);

    bool getOption(int level, int optname, void *optval, size_t *len);
    template<class T>
    bool getOption(int level, int optname, T &optval){
        size_t len = sizeof(T);
        return getOption(level, optname, &optval, &len);
    }

    bool setOption(int level, int optname, const void *optval, size_t len);
    template<class T>
    bool setOption(int level, int optname, const T &optval){
        return setOption(level, optname, &optval, sizeof(T));
    }

    bool bind(const Address::ptr addr);
    bool listen(int backlog = SOMAXCONN);
    bool connect(const Address::ptr addr, uint64_t timeout_ms = -1);
    Socket::ptr accept();
    bool close();

    int send(const void *buf, size_t len, int flags = 0);
    int send(const iovec *iov, size_t len, int flags = 0);
    int sendto(const void *buf, size_t len, const Address::ptr addr, int flags = 0);
    int sendto(const iovec *iov, size_t len, const Address::ptr addr, int flags = 0);
    int recv(void *buf, size_t len, int flags = 0);
    int recv(iovec *iov, size_t len, int flags = 0);
    int recvfrom(void *buf, size_t len, Address::ptr addr, int flags = 0);
    int recvfrom(iovec *iov, size_t len, Address::ptr addr, int flags = 0);

    Address::ptr getLocalAddress();
    Address::ptr getRemoteAddress();

    int getFamily() const {return m_family;}
    int getType() const {return m_type;}
    int getProtocol() const {return m_protocol;}
    int getSocket() const {return m_socket;}

    bool isConnected() const {return m_isConnected;}
    bool isValid() const;
    int getError();
    std::ostream dump(std::ostream &os) const;

    bool cancelRead();
    bool cancelWrite();
    bool cancelAccept();
    bool cancelAll();
private:
    //初始化socket句柄，创建并设置sockopt参数
    void initSock();
    //创建新的socket句柄
    void newSock();
    //从socket句柄初始化socket类
    bool init(int sockfd, bool create = false);
private:
    int m_socket;
    int m_family;
    int m_type;
    int m_protocol;
    bool m_isConnected;
    
    Address::ptr m_localAddress;
    Address::ptr m_remoteAddress;
};

}

#endif