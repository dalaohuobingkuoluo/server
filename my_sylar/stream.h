#ifndef __SYLAR_STREAM_H__
#define __SYLAR_STREAM_H__

#include <memory>
#include "bytearray.h"
#include "socket.h"

namespace sylar{

class Stream{
public:
    typedef std::shared_ptr<Stream> ptr;
    ~Stream() {}

    virtual int read(void *buf, size_t len) = 0;
    virtual int read(ByteArray::ptr ba, size_t len) = 0;
    virtual int readFixLen(void *buf, size_t len);
    virtual int readFixLen(ByteArray::ptr ba, size_t len);

    virtual int write(const void *buf, size_t len) = 0;
    virtual int write(ByteArray::ptr ba, size_t len) = 0;
    virtual int writeFixLen(const void *buf, size_t len);
    virtual int writeFixLen(ByteArray::ptr ba, size_t len);

    virtual void close() = 0;
};


class SocketStream : public Stream {
public:
    typedef std::shared_ptr<SocketStream> ptr;
    SocketStream(Socket::ptr sock, bool owner = true);
    ~SocketStream();

    virtual int read(void *buf, size_t len) override;
    virtual int read(ByteArray::ptr ba, size_t len) override;
    virtual int write(const void *buf, size_t len) override;
    virtual int write(ByteArray::ptr ba, size_t len) override;
    virtual void close() override;

    Socket::ptr getSocket() const {return m_socket;}
    bool isConnected() const;
protected:
    Socket::ptr m_socket;
    bool m_owner;               //析构时是否需要关闭socket
};

}



#endif