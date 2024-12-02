#include "stream.h"

namespace sylar{

int Stream::readFixLen(void *buf, size_t len){
    size_t off = 0;
    size_t left = len;
    while(left > 0){
        int rt = read((char*)buf, left);
        if(rt <= 0){
            return rt;
        }
        off += rt;
        left -= rt;
    }
    return len;
}

int Stream::readFixLen(ByteArray::ptr ba, size_t len){
    size_t left = len;
    while(left > 0){
        int rt = read(ba, left);
        if(rt <= 0){
            return rt;
        }
        len -= rt;
    }
    return left;
}

int Stream::writeFixLen(const void *buf, size_t len){
    size_t off = 0;
    size_t left = len;
    while(left > 0){
        int rt = write((const char*)buf, left);
        if(rt <= 0){
            return rt;
        }
        off += rt;
        left -= rt;
    }
    return len;
}

int Stream::writeFixLen(ByteArray::ptr ba, size_t len){
    size_t left = len;
    while(left > 0){
        int rt = write(ba, left);
        if(rt <= 0){
            return rt;
        }
        left -= rt;
    }
    return len;
}

SocketStream::SocketStream(Socket::ptr sock, bool owner)
        : m_socket(sock), m_owner(owner) {
}

SocketStream::~SocketStream(){
    if(m_owner && m_socket){
        m_socket->close();
    }
}

bool SocketStream::isConnected() const{
    return m_socket && m_socket->isConnected();
}

int SocketStream::read(void *buf, size_t len) {
    if(!isConnected()){
        return -1;
    }
    return m_socket->recv(buf, len);
}

int SocketStream::read(ByteArray::ptr ba, size_t len) {
    if(!isConnected()){
        return -1;
    }
    std::vector<iovec> iovs;
    ba->getWriteBuffers(iovs, len);
    int rt = m_socket->recv(&iovs[0], iovs.size());
    if(rt > 0){
        ba->setPosition(ba->getPosition() + rt);
    }
    return rt;
}

int SocketStream::write(const void *buf, size_t len)  {
    if(!isConnected()){
        return -1;
    }
    return m_socket->send(buf, len);
}

int SocketStream::write(ByteArray::ptr ba, size_t len)  {
    if(!isConnected()){
        return -1;
    }
    std::vector<iovec> iovs;
    ba->getReadBuffers(iovs, len);
    int rt = m_socket->send(&iovs[0], iovs.size());
    if(rt > 0){
        ba->setPosition(ba->getPosition() + rt);
    }
    return rt;
}

void SocketStream::close()  {
    if(m_socket){
        m_socket->close();
    }
}




}



