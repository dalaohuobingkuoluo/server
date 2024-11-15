#include "socket.h"
#include "fd_manager.h"
#include "log.h"
#include "macro.h"
#include <netinet/tcp.h>

namespace sylar{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

Socket::Socket(int family, int type, int protocol)
        : m_socket(-1),
          m_family(family),
          m_type(type),
          m_protocol(protocol),
          m_isConnected(false){
}

Socket::~Socket(){
    close();
}

uint64_t Socket::getRecvTimeout(){
    FdCtx::ptr ctx = FdMgr::GetInstance()->get(m_socket);
    if(ctx){
        return ctx->getTimeout(SO_RCVTIMEO);
    }
    return -1;
}

bool Socket::setRecvTimeout(uint64_t v){
    timeval tv{(int)(v / 1000), (int)(v % 1000 * 1000)};
    return setOption(SOL_SOCKET, SO_RCVTIMEO, tv);
}
uint64_t Socket::getSendTimeout(){
    FdCtx::ptr ctx = FdMgr::GetInstance()->get(m_socket);
    if(ctx){
        return ctx->getTimeout(SO_SNDTIMEO);
    }
    return -1;
}
bool Socket::setSendTimeout(uint64_t v){
    timeval tv{(int)(v / 1000), (int)(v % 1000 * 1000)};
    return setOption(SOL_SOCKET, SO_SNDTIMEO, tv);
}

bool Socket::getOption(int level, int optname, void *optval, size_t *len){
    int rt = getsockopt(m_socket, level, optname, optval, (socklen_t*)len);
    if(rt){
        SYLAR_LOG_DEBUG(g_logger) << "getOption: socket = " << m_socket
                                  << ", level = " << level << ", optname = " << optname
                                  << ", errno = " << errno << ", strerr = " << strerror(errno);
        return false;
    }
    return true;
}

bool Socket::setOption(int level, int optname, const void *optval, size_t len){
    int rt = setsockopt(m_socket, level, optname, optval, (socklen_t)len);
    if(rt){
        SYLAR_LOG_DEBUG(g_logger) << "setOption: socket = " << m_socket
                                  << ", level = " << level << ", optname = " << optname
                                  << ", errno = " << errno << ", strerr = " << strerror(errno);
        return false;
    }
    return true;
}

bool Socket::bind(const Address::ptr addr){
    if(SYLAR_UNLIKELY(addr->getFamily() != m_family)){
        SYLAR_LOG_ERROR(g_logger) << "socket = " << m_socket << ", bind sock.family(" << m_family << ") != addr.family("
                                  << addr->getFamily() << ") , addr = " << addr->toString();
        return false;
    }
    if(SYLAR_UNLIKELY(!isValid())){
        newSock();
        if(SYLAR_UNLIKELY(!isValid())){
            return false;
        }
    }
    if(::bind(m_socket, addr->getAddr(), addr->getAddrLen())){
        SYLAR_LOG_ERROR(g_logger) << "socket = " << m_socket << ", bind(" << addr->toString() << ") fail, errno = "
                                  << errno << ", strerr = " << strerror(errno);
        return false;
    }
    getLocalAddress();
    return true;
}

bool Socket::listen(int backlog = SOMAXCONN){

}

bool Socket::connect(const Address::ptr addr, uint64_t timeout_ms){

}

Socket::ptr Socket::accept(){
    Socket::ptr sock(new Socket(m_family, m_type, m_protocol));
    int sockfd = ::accept(m_socket, NULL, NULL);
    if(sockfd == -1){
        SYLAR_LOG_ERROR(g_logger) << "accept(" << m_socket << ") fail, errno = "
                                  << errno << ", strerr = " << strerror(errno);
        return nullptr; 
    }
    if(!sock->init(sockfd, true)){
        SYLAR_LOG_ERROR(g_logger) << "accept: sock.init(" << sockfd << ") fail";
        return nullptr;
    }
    return sock;
}

bool Socket::close(){

}

int Socket::send(const void *buf, size_t len, int flags){

}

int Socket::send(const iovec *iov, size_t len, int flags){

}

int Socket::sendto(const void *buf, size_t len, const Address::ptr addr, int flags){

}

int Socket::sendto(const iovec *iov, size_t len, const Address::ptr addr, int flags){

}

int Socket::recv(void *buf, size_t len, int flags = 0){

}

int Socket::recv(iovec *iov, size_t len, int flags = 0){

}

int Socket::recvfrom(void *buf, size_t len, Address::ptr addr, int flags){

}

int Socket::recvfrom(iovec *iov, size_t len, Address::ptr addr, int flags){

}

Address::ptr Socket::getLocalAddress(){

}
Address::ptr Socket::getRemoteAddress(){

}

bool Socket::isValid() const{
    return m_socket != -1;
}

int Socket::getError(){

}
std::ostream Socket::dump(std::ostream &os) const{

}

bool Socket::cancelRead(){

}
bool Socket::cancelWrite(){

}
bool Socket::cancelAccept(){

}
bool Socket::cancelAll(){

}

void Socket::initSock(){
    int val = 1;
    setOption(SOL_SOCKET, SO_REUSEADDR, val);       //设置socket允许重用本地地址
    if(m_type == SOCK_STREAM){
        setOption(IPPROTO_TCP, TCP_NODELAY, val);   //设置tcp禁用 Nagle 算法，降低延迟
    }
}
void Socket::newSock(){
    m_socket = socket(m_family, m_type, m_protocol);
    if(SYLAR_LIKELY(m_socket != -1)){
        initSock();
    }else{
        SYLAR_LOG_ERROR(g_logger) << "socket(" << m_family << ", " << m_type << ", " << m_protocol 
                                  <<") fail, errno = " << errno << ", strerr = " << strerror(errno);
    }
}

bool Socket::init(int sockfd, bool create){
    FdCtx::ptr ctx = FdMgr::GetInstance()->get(sockfd, create);
    if(ctx && ctx->isSocket() && !ctx->isClose()){
        m_socket = sockfd;
        m_isConnected = true;
        initSock();
        getLocalAddress();
        getRemoteAddress();
        return true;
    }
    return false;
}
    
}