#include "socket.h"
#include "fd_manager.h"
#include "log.h"
#include "hook.h"
#include "iomanager.h"
#include "macro.h"
#include <netinet/tcp.h>

namespace sylar{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

Socket::ptr Socket::CreateTCP(Address::ptr addr){
    Socket::ptr sock(new Socket(addr->getFamily(), TCP, 0));
    return sock;
}

Socket::ptr Socket::CreateUDP(Address::ptr addr){
    Socket::ptr sock(new Socket(addr->getFamily(), UDP, 0));
    return sock;
}

Socket::ptr Socket::CreateTCPSocket(){
    Socket::ptr sock(new Socket(IPV4, TCP, 0));
    return sock;
}

Socket::ptr Socket::CreateUDPSocket(){
    Socket::ptr sock(new Socket(IPV4, UDP, 0));
    return sock;
}

Socket::ptr Socket::CreateTCPSocket6(){
    Socket::ptr sock(new Socket(IPV6, TCP, 0));
    return sock;
}

Socket::ptr Socket::CreateUDPSocket6(){
    Socket::ptr sock(new Socket(IPV6, UDP, 0));
    return sock;
}

Socket::ptr Socket::CreateUnixTCPSocket(){
    Socket::ptr sock(new Socket(UNIX, TCP, 0));
    return sock;
}

Socket::ptr Socket::CreateUnixUDPSocket(){
    Socket::ptr sock(new Socket(UNIX, UDP, 0));
    return sock;
}

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

bool Socket::listen(int backlog){
    if(!isValid()){
        SYLAR_LOG_ERROR(g_logger) << "listen error, socket = -1 is not valid";
        return false;
    }
    if(::listen(m_socket, backlog)){
        SYLAR_LOG_ERROR(g_logger) << "listen error, errno = " << errno 
                                  << " , strerror = " << strerror(errno);
        return false;
    }
    return true;
}

bool Socket::connect(const Address::ptr addr, uint64_t timeout_ms){
    if(SYLAR_UNLIKELY(addr->getFamily() != m_family)){
        SYLAR_LOG_ERROR(g_logger) << "socket = " << m_socket << ", connect sock.family(" << m_family << ") != addr.family("
                                  << addr->getFamily() << ") , addr = " << addr->toString();
        return false;
    }
    if(SYLAR_UNLIKELY(!isValid())){
        newSock();
        if(SYLAR_UNLIKELY(!isValid())){
            return false;
        }
    }
    m_remoteAddress = addr;
    if(timeout_ms == (uint64_t)-1){
        if(::connect(m_socket, addr->getAddr(), addr->getAddrLen())){
            SYLAR_LOG_ERROR(g_logger) << "socket = " << m_socket << ", connect (" << addr->toString() 
                                      <<") fail, errno = " << errno << " , strerror = " << strerror(errno);
            close();
            return false;
        }
    }else{
        if(connect_with_timeout(m_socket, addr->getAddr(), addr->getAddrLen(), timeout_ms)){
            SYLAR_LOG_ERROR(g_logger) << "socket = " << m_socket << ", connect (" << addr->toString() 
                                      <<") timeout = " << timeout_ms << "fail, errno = " << errno 
                                      << " , strerror = " << strerror(errno);
            close();
            return false;
        }
    }
    m_isConnected = true;
    getRemoteAddress();
    getLocalAddress();
    return true;
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
    if(!m_isConnected && m_socket == -1){
        return true;
    }
    m_isConnected = false;
    if(m_socket != -1){
        ::close(m_socket);
        m_socket = -1;
    }
    return true;
}

int Socket::send(const void *buf, size_t len, int flags){
    if(m_isConnected){
        int rt = ::send(m_socket, buf, len, flags);
        if(rt <= 0){
            SYLAR_LOG_ERROR(g_logger) << "send error, errno = " << errno
                                      << ", strerror = " << strerror(errno);
            return -1;
        }
        return rt;
    }
    return -1;
}

int Socket::send(const iovec *iov, size_t len, int flags){
    if(m_isConnected){
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)iov;
        msg.msg_iovlen = len;
        return sendmsg(m_socket, &msg, flags);
    }
    return -1;
}

int Socket::sendto(const void *buf, size_t len, const Address::ptr to, int flags){
    if(m_isConnected){
        return ::sendto(m_socket, buf, len, flags, to->getAddr(), to->getAddrLen());
    }
    return -1;
}

int Socket::sendto(const iovec *iov, size_t len, const Address::ptr to, int flags){
    if(m_isConnected){
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)iov;
        msg.msg_iovlen = len;
        msg.msg_name = to->getAddr();
        msg.msg_namelen = to->getAddrLen();
        return sendmsg(m_socket, &msg, flags);
    }
    return -1;
}

int Socket::recv(void *buf, size_t len, int flags){
    if(m_isConnected){
        return ::recv(m_socket, buf, len, flags);
    }
    return -1;
}

int Socket::recv(iovec *iov, size_t len, int flags){
    if(m_isConnected){
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)iov;
        msg.msg_iovlen = len;
        return recvmsg(m_socket, &msg, flags);
    }
    return -1;
}

int Socket::recvfrom(void *buf, size_t len, Address::ptr from, int flags){
    if(m_isConnected){
        socklen_t len = from->getAddrLen();
        return ::recvfrom(m_socket, buf, len, flags, from->getAddr(), &len);
    }
    return -1;
}

int Socket::recvfrom(iovec *iov, size_t len, Address::ptr from, int flags){
    if(m_isConnected){
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)iov;
        msg.msg_iovlen = len;
        msg.msg_name = from->getAddr();
        msg.msg_namelen = from->getAddrLen();
        return recvmsg(m_socket, &msg, flags);
    }
    return -1;
}

Address::ptr Socket::getRemoteAddress(){
    if(m_remoteAddress){
        return m_remoteAddress;
    }
    Address::ptr addr;
    switch(m_family){
        case AF_INET:
            addr.reset(new IPV4Address());
            break;
        case AF_INET6:
            addr.reset(new IPV6Address());
            break;
        case AF_UNIX:
            addr.reset(new UnixAddress());
            break;
        default:
            addr.reset(new UnknowAddress(m_family));
            break;
    }
    socklen_t len = addr->getAddrLen();
    if(getpeername(m_socket, addr->getAddr(), &len)){
        SYLAR_LOG_ERROR(g_logger) << "socket = " << m_socket << ", getpeername(" << addr->toString() 
                                      <<") fail, errno = " << errno << " , strerror = " << strerror(errno);
        return Address::ptr(new UnknowAddress(m_family));
    }
    if(m_family == AF_UNIX){
        UnixAddress::ptr uaddr = std::dynamic_pointer_cast<UnixAddress>(addr);
        SYLAR_ASSERT2(uaddr != nullptr, "getRemoteAddress convert to unix_addr fail");
        uaddr->setAddrLen(len);
    }
    m_remoteAddress = addr;
    return m_remoteAddress;
}

Address::ptr Socket::getLocalAddress(){
     if(m_localAddress){
        return m_localAddress;
    }
    Address::ptr addr;
    switch(m_family){
        case AF_INET:
            addr.reset(new IPV4Address());
            break;
        case AF_INET6:
            addr.reset(new IPV6Address());
            break;
        case AF_UNIX:
            addr.reset(new UnixAddress());
            break;
        default:
            addr.reset(new UnknowAddress(m_family));
            break;
    }
    socklen_t len = addr->getAddrLen();
    if(getsockname(m_socket, addr->getAddr(), &len)){
        SYLAR_LOG_ERROR(g_logger) << "socket = " << m_socket << ", getsockname(" << addr->toString() 
                                      <<") fail, errno = " << errno << " , strerror = " << strerror(errno);
        return Address::ptr(new UnknowAddress(m_family));
    }
    if(m_family == AF_UNIX){
        UnixAddress::ptr uaddr = std::dynamic_pointer_cast<UnixAddress>(addr);
        SYLAR_ASSERT2(uaddr != nullptr, "getLocalAddress convert to unix_addr fail");
        uaddr->setAddrLen(len);
        // UnixAddress::ptr addr = std::dynamic_pointer_cast<UnixAddress>(addr);
    }
    m_localAddress = addr;
    return m_localAddress;
}

bool Socket::isValid() const{
    return m_socket != -1;
}

int Socket::getError(){
    int error = 0;
    size_t len = sizeof(error);
    getOption(SOL_SOCKET, SO_ERROR, (void*)&error, &len);
    return error;
}
std::ostream& Socket::dump(std::ostream &os) const {
    os << "[sockfd = " << m_socket << ", family = " << m_family << ", type = " << m_type
       << ", protocol = " << m_protocol << ", isConnected = " << m_isConnected;
    if(m_localAddress){
        os << ", localAddress = " << m_localAddress->toString();
    }
    if(m_remoteAddress){
        os << ", remoteAddress = " << m_remoteAddress->toString();
    }   
    os << "]"; 
    return os;
}

std::string Socket::toString() const {
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

bool Socket::cancelRead(){
    return sylar::IOManager::GetThis()->cancelEvent(m_socket, IOManager::READ);
}

bool Socket::cancelWrite(){
    return sylar::IOManager::GetThis()->cancelEvent(m_socket, IOManager::WRITE);
}

bool Socket::cancelAccept(){
    return sylar::IOManager::GetThis()->cancelEvent(m_socket, IOManager::READ);
}

bool Socket::cancelAll(){
    return sylar::IOManager::GetThis()->cancelAll(m_socket);
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