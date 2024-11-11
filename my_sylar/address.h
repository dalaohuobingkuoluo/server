#ifndef __SYLAR_ADDRESS_H__
#define __SYLAR_ADDRESS_H__

#include <sys/socket.h>
#include <memory>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <string>
#include <iostream>

namespace sylar{

class Address {
public:
    typedef std::shared_ptr<Address> ptr;

    static Address::ptr Create(const sockaddr* addr, socklen_t addrlen);

    virtual ~Address(){};

    int getFamily() const;

    virtual const sockaddr* getAddr() const = 0;
    virtual const socklen_t getAddrLen() const = 0;

    virtual std::ostream& insert(std::ostream& os) const = 0;
    std::string toString();

    bool operator<(const Address& rhs) const;
    bool operator==(const Address& rhs) const;
    bool operator!=(const Address& rhs) const;
};

class IPAddress : public Address {
public:
    typedef std::shared_ptr<IPAddress> ptr;

    static IPAddress::ptr Create(const char* addr, uint16_t port = 0);

    virtual IPAddress::ptr broadcaseAddress(uint32_t prefix_len) = 0;
    virtual IPAddress::ptr networkAddress(uint32_t prefix_len) = 0;
    virtual IPAddress::ptr subnetMask(uint32_t prefix_len) = 0;

    virtual uint16_t getPort() const = 0;
    virtual void setPort(uint16_t v) = 0; 
};

class IPV4Address : public IPAddress {
public:
    typedef std::shared_ptr<IPV4Address> ptr;

    static IPAddress::ptr Create(const char* addr, uint16_t port = 0);
 
    IPV4Address(const sockaddr_in& addr);
    IPV4Address(uint32_t address = INADDR_ANY, uint16_t port = 0);

    const sockaddr* getAddr() const override;
    const socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

    IPAddress::ptr broadcaseAddress(uint32_t prefix_len) override;
    IPAddress::ptr networkAddress(uint32_t prefix_len) override;
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;

    uint16_t getPort() const override;
    void setPort(uint16_t v) override; 

private:
    sockaddr_in m_addr;
};

//IPv6 地址由 16 个字节组成，每个字节之间都是独立的，因此并不需要对整个地址进行大小端的处理。
//而 IPv4 地址以 32 位整数表示，所以在处理整个 IPv4 地址时，需要注意大小端的差异。
class IPV6Address : public IPAddress {
public:
    typedef std::shared_ptr<IPV6Address> ptr;

    static IPV6Address::ptr Create(const char* address, uint16_t port = 0);

    IPV6Address(const sockaddr_in6& addr);
    IPV6Address();
    IPV6Address(const uint8_t addr[16], uint16_t port = 0);

    const sockaddr* getAddr() const override;
    const socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

    IPAddress::ptr broadcaseAddress(uint32_t prefix_len) override;
    IPAddress::ptr networkAddress(uint32_t prefix_len) override;
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;

    uint16_t getPort() const override;
    void setPort(uint16_t v) override; 

private:
    sockaddr_in6 m_addr;
};

class UnixAddress : public Address {
public:
    typedef std::shared_ptr<UnixAddress> ptr;

    UnixAddress();
    UnixAddress(const std::string& path);

    const sockaddr* getAddr() const override;
    const socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;
private:
    sockaddr_un m_addr;
    socklen_t m_addrlen;
};

class UnknowAddress : public Address {
public:
    typedef std::shared_ptr<UnknowAddress> ptr;

    UnknowAddress(int family);
    UnknowAddress(const sockaddr& addr);

    const sockaddr* getAddr() const override;
    const socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;
private:
    sockaddr m_addr;
};

}

#endif