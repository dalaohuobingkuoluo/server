#include "address.h"
#include "endian.h"
#include "log.h"
#include <sstream>
#include <algorithm>
#include <netdb.h>
#include <ifaddrs.h>

namespace sylar{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

template<class T>
static T CreateMask(uint32_t bits){
    if(bits >= sizeof(T) * 8){
        return 0;
    }
    return (1 << (sizeof(T) * 8 - bits)) - 1;
}

Address::ptr Address::Create(const sockaddr* addr, socklen_t addrlen){
    if(!addr){
        return nullptr;
    }
    Address::ptr rtaddr;
    switch(addr->sa_family){
        case AF_INET:
            rtaddr.reset(new IPV4Address(*(sockaddr_in*)addr));
            break;
        case AF_INET6:
            rtaddr.reset(new IPV6Address(*(sockaddr_in6*)addr));
            break;
        default:
            rtaddr.reset(new UnknowAddress(*addr));
            break;
    }
    return rtaddr;
}

bool Address::Lookup(std::vector<Address::ptr> &result, const std::string &host,
                       int family, int type, int protocol){
    addrinfo hints, *results, *next;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;

    std::string node;
    const char *service = NULL, *bound = host.c_str() + host.size();
    //[ipv6地址]:service
    if(!host.empty() && host[0] == '['){
        const char* endipv6 = (const char*)memchr(host.c_str(), ']', host.size() - 1);
        if(endipv6){
            if(endipv6 + 1 < bound && *(endipv6 + 1) == ':'){
                if(endipv6 + 2 < bound){
                    service = endipv6 + 2;
                }else{
                    SYLAR_LOG_ERROR(g_logger) << "Address::Lookup(" << host << "," << family 
                                              << "," << type << "," << protocol 
                                              << ") fail, host = [ipv6]:service out of range";
                    return false;
                }
            }
            node = host.substr(1, endipv6 - host.c_str() -1);
        }
        // SYLAR_LOG_DEBUG(g_logger) << "host = " << node << ", service = " << service;
    }
    //node:service
    if(!host.empty()){
        service = (const char*)memchr(host.c_str(), ':', host.size());
        if(service){
            if(service + 1 < bound){
                service++;
            }else{
                SYLAR_LOG_ERROR(g_logger) << "Address::Lookup(" << host << "," << family 
                                          << "," << type << "," << protocol 
                                          << ") fail, host = node:service out of range";
                return false;
            }
            node = host.substr(0, service - host.c_str() - 1);
        }
        // SYLAR_LOG_DEBUG(g_logger) << "host = " << node << ", service = " << service;
    }
    //无service字段
    if(node.empty()){
        node = host;
    }

    int err = getaddrinfo(node.c_str(), service, &hints, &results);
    if(err){
        SYLAR_LOG_ERROR(g_logger) << "Address::Lookup(" << host << "," << family 
                                  << "," << type << "," << protocol 
                                  << ") getaddrinfo fail, err = " << err << ", errstr = "
                                  << gai_strerror(err);
        return false;
    }
    next = results;
    while(next){
        result.push_back(Create(next->ai_addr, next->ai_addrlen));
        next = next->ai_next;
    }
    return true;
}

Address::ptr Address::LookupAny(const std::string& host, int family, int type, int protocol){
    std::vector<Address::ptr> res;
    if(Lookup(res, host, family, type, protocol)){
        return res[0];
    }
    return nullptr;
}    

std::shared_ptr<IPAddress> Address::LookupAnyIPAddress(const std::string& host, int family, int type, int protocol){
    std::vector<Address::ptr> res;
    if(Lookup(res, host, family, type, protocol)){
        SYLAR_LOG_DEBUG(g_logger) << "LookupAnyIPAddress";
        for(auto i : res){
            SYLAR_LOG_DEBUG(g_logger) << i->toString();
        }
        for(auto i : res){
            auto v = std::dynamic_pointer_cast<IPAddress>(i);
            if(v){
                return v;
            }
        }
    }
    return nullptr;
}

template<class T>
static uint32_t CountBytes(T addr){
    uint32_t rt = 0;
    for(; addr; ++rt){
        addr &= addr - 1;
    }
    return rt;
}

//getifaddrs是一个用于获取系统网络接口信息的函数，用来引用系统中的所有网络接口及其地址信息，例如 IP 地址、子网掩码长度等。
bool Address::GetInterfaceAddress(std::multimap<std::string, std::pair<Address::ptr, uint32_t>> &result, 
                                  int family){
    ifaddrs *next, *results;
    if(getifaddrs(&results) != 0){
        SYLAR_LOG_ERROR(g_logger) << "Address::GetInterfaceAddress getifaddrs error" 
                                  << ", errstr = " << strerror(errno);
        return false;
    }
    try{
        for(next = results; next; next = next->ifa_next){
            Address::ptr addr;
            uint32_t prefix_len = ~0u;
            if(family != AF_UNSPEC && next->ifa_addr->sa_family != family){
                continue;
            }
            switch(next->ifa_addr->sa_family){
                case AF_INET:
                    {
                        addr = Create(next->ifa_addr, sizeof(sockaddr_in));
                        uint32_t netmask = ((sockaddr_in*)next->ifa_netmask)->sin_addr.s_addr;
                        prefix_len = CountBytes(netmask);
                    }
                    break;
                case AF_INET6:
                    {
                        addr = Create(next->ifa_addr, sizeof(sockaddr_in6));
                        in6_addr &netmask = ((sockaddr_in6*)next->ifa_netmask)->sin6_addr;
                        prefix_len = 0;
                        for(size_t i = 0; i < 16; ++i){
                            prefix_len += CountBytes(netmask.s6_addr[i]);
                        }
                    }
                    break;
                default:
                    break;
            }
            if(addr){
                result.insert(std::make_pair(next->ifa_name, std::make_pair(addr, prefix_len)));
            }
        }
    }catch(...){
        SYLAR_LOG_ERROR(g_logger) << "Address::GetInterfaceAddress exception";
        freeifaddrs(results);
        return false;
    }
    freeifaddrs(results);
    return true;
}     

bool Address::GetInterfaceAddress(std::vector<std::pair<Address::ptr, uint32_t>> &result, const std::string &iface, 
                         int family){
    if(!iface.empty() || iface == "*"){
        if(family == AF_INET || family == AF_UNSPEC){
            result.push_back(std::make_pair(Address::ptr(new IPV4Address), 0u));
        }
        if(family == AF_INET6 || family == AF_UNSPEC){
            result.push_back(std::make_pair(Address::ptr(new IPV6Address), 0u));
        }
        return true;
    }
    std::multimap<std::string, std::pair<Address::ptr, uint32_t>> results;
    if(!GetInterfaceAddress(results, family)){
        return false;
    }
    auto it = results.equal_range(iface);
    for(auto i = it.first; i != it.second; ++i){
        result.push_back(i->second);
    }
    return true;
}  

int Address::getFamily() const {
    return getAddr()->sa_family;
}

std::string Address::toString() {
    std::stringstream ss;
    insert(ss);
    return ss.str();
}

bool Address::operator<(const Address& rhs) const {
    socklen_t minlen = std::min(getAddrLen(), rhs.getAddrLen());
    int res = memcmp(getAddr(), rhs.getAddr(), minlen);
    if(res < 0){
        return true;
    }else if(res > 0){
        return false;
    }else if(getAddrLen() < rhs.getAddrLen()){
        return true;
    }
    return false;
}
//memcmp函数用于比较两个内存区域的内容, 比较两个内存区域的字节
//strcmp函数用于比较两个字符串，逐个比较两个字符串中对应位置上的字符

bool Address::operator==(const Address& rhs) const {
    return getAddrLen() == rhs.getAddrLen() 
        && memcmp(getAddr(), rhs.getAddr(), getAddrLen()) == 0;
}

bool Address::operator!=(const Address& rhs) const {
    return !(*this == rhs);
}

//getaddrinfo 用于域名解析和套接字地址信息获取,将主机名（如域名）或服务名（如端口号）转换为一个或多个 sockaddr 结构体可直接用于创建和连接套接字。
//node(域名或IP地址)  service(服务名或端口号) hints（地址信息提示） res（返回结果）
IPAddress::ptr IPAddress::Create(const char* addr, uint16_t port){
    addrinfo hints, *res;
    memset(&hints, 0, sizeof(addrinfo));

    // hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;
    int rt = getaddrinfo(addr, NULL, &hints, &res);
    if(rt){
        SYLAR_LOG_ERROR(g_logger) << "IPAddress::Create(" << addr << ","
                                  << port << ") fail, rt = " << rt << ", errno = " << errno
                                  << ", strerror = " << strerror(errno);
        return nullptr;
    }

    try{
        IPAddress::ptr rtaddr = std::dynamic_pointer_cast<IPAddress>(
                                Address::Create(res->ai_addr, res->ai_addrlen));
        if(rtaddr){
            rtaddr->setPort(port);
        }
        freeaddrinfo(res);
        return rtaddr;
    }catch(...){
        freeaddrinfo(res);
        return nullptr;
    }
}


IPAddress::ptr IPV4Address::Create(const char* addr, uint16_t port){
    IPV4Address::ptr rtaddr(new IPV4Address);
    rtaddr->setPort(port);
    int rt = inet_pton(AF_INET, addr, &rtaddr->m_addr.sin_addr);
    if(rt <= 0){
        SYLAR_LOG_ERROR(g_logger) << "IPV4Address::Create(" << addr << ","
                                  << port << ") fail, rt = " << rt << ", errno = " << errno
                                  << ", strerror = " << strerror(errno);
        return nullptr;
    }
    return rtaddr;
}


IPV4Address::IPV4Address(const sockaddr_in& addr){
    m_addr = addr;
}

IPV4Address::IPV4Address(uint32_t address, uint16_t port){
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = byteswapOnLittleEndian(port);
    m_addr.sin_addr.s_addr = byteswapOnLittleEndian(address);
}

const sockaddr* IPV4Address::getAddr() const {
    return (sockaddr*)&m_addr;
}

sockaddr* IPV4Address::getAddr() {
    return (sockaddr*)&m_addr;
}

const socklen_t IPV4Address::getAddrLen() const {
    return sizeof(m_addr);
}

std::ostream& IPV4Address::insert(std::ostream& os) const {
    uint32_t addr = byteswapOnLittleEndian(m_addr.sin_addr.s_addr);
    os << ((addr >> 24) & 0xff) << "."
       << ((addr >> 16) & 0xff) << "."
       << ((addr >> 8) & 0xff) << "."
       << (addr & 0xff)
       << ":" << byteswapOnLittleEndian(m_addr.sin_port);
    return os;
}

IPAddress::ptr IPV4Address::broadcaseAddress(uint32_t prefix_len) {
    if(prefix_len > 32){
        return nullptr;
    }
    sockaddr_in baddr = m_addr;
    baddr.sin_addr.s_addr |= byteswapOnLittleEndian(CreateMask<uint32_t>(prefix_len));
    return IPV4Address::ptr(new IPV4Address(baddr));
}

IPAddress::ptr IPV4Address::networkAddress(uint32_t prefix_len) {
    if(prefix_len > 32){
        return nullptr;
    }
    sockaddr_in naddr = m_addr;
    naddr.sin_addr.s_addr &= ~byteswapOnLittleEndian(CreateMask<uint32_t>(prefix_len));
    return IPV4Address::ptr(new IPV4Address(naddr));
}
IPAddress::ptr IPV4Address::subnetMask(uint32_t prefix_len) {
    sockaddr_in subnet;
    memset(&subnet, 0, sizeof(subnet));
    subnet.sin_family = AF_INET;
    subnet.sin_addr.s_addr = ~byteswapOnLittleEndian(CreateMask<uint32_t>(prefix_len));
    return IPV4Address::ptr(new IPV4Address(subnet));
}

uint16_t IPV4Address::getPort() const {
    return byteswapOnLittleEndian(m_addr.sin_port);;
}

void IPV4Address::setPort(uint16_t v) {
    m_addr.sin_port = byteswapOnLittleEndian(v);
}

IPV6Address::IPV6Address(const sockaddr_in6& addr){
    m_addr = addr;
}

IPV6Address::IPV6Address(){
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
}

IPV6Address::ptr IPV6Address::Create(const char* address, uint16_t port){
    IPV6Address::ptr rtaddr(new IPV6Address);
    rtaddr->setPort(port);
    int rt = inet_pton(AF_INET6, address, &rtaddr->m_addr.sin6_addr);
    if(rt <= 0){
        SYLAR_LOG_ERROR(g_logger) << "IPV6Address::Create(" << address << ","
                                  << port << ") fail, rt = " << rt << ", errno = " << errno
                                  << ", strerror = " << strerror(errno);
        return nullptr;
    }
    return rtaddr;
}

IPV6Address::IPV6Address(const uint8_t address[16], uint16_t port){
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
    m_addr.sin6_port = byteswapOnLittleEndian(port);
    memcpy(&m_addr.sin6_addr.s6_addr, address, 16);
}

const sockaddr* IPV6Address::getAddr() const {
    return (sockaddr*)&m_addr.sin6_addr;
}

sockaddr* IPV6Address::getAddr() {
    return (sockaddr*)&m_addr.sin6_addr;
}

const socklen_t IPV6Address::getAddrLen() const {
    return sizeof(m_addr);
}

std::ostream& IPV6Address::insert(std::ostream& os) const {
    os << "[";
    uint16_t* addr = (uint16_t*)m_addr.sin6_addr.s6_addr;
    bool used_zero = false;
    for(size_t i = 0; i < 8; i++){
        if(addr[i] == 0 && !used_zero){
            continue;
        }
        if(i && addr[i-1] == 0 && !used_zero){
            os << ":";
            used_zero = true;
        }
        if(i){
            os << ":";
        }
        os << std::hex << (int)byteswapOnLittleEndian(addr[i]);
    }
    if(!used_zero && addr[7] == 0){         //结尾全0
        os << "::";
    }
    os << "]:" << std::dec << byteswapOnLittleEndian(m_addr.sin6_port);
    return os;
}

IPAddress::ptr IPV6Address::broadcaseAddress(uint32_t prefix_len) {
    sockaddr_in6 baddr = m_addr;
    baddr.sin6_addr.s6_addr[prefix_len / 8] |= 
                CreateMask<uint8_t>(prefix_len % 8);
    for(int i = prefix_len / 8 + 1; i < 16; i++){
        baddr.sin6_addr.s6_addr[i] = 0xff;
    }
    return IPV6Address::ptr(new IPV6Address(baddr));
}

IPAddress::ptr IPV6Address::networkAddress(uint32_t prefix_len) {
    sockaddr_in6 baddr = m_addr;
    baddr.sin6_addr.s6_addr[prefix_len / 8] &= 
                ~CreateMask<uint8_t>(prefix_len % 8);
    for(size_t i = prefix_len / 8 + 1; i < 16; i++){
        baddr.sin6_addr.s6_addr[i] = 0x00;
    }
    return IPV6Address::ptr(new IPV6Address(baddr));
}
IPAddress::ptr IPV6Address::subnetMask(uint32_t prefix_len) {
    sockaddr_in6 subnet;
    memset(&subnet, 0, sizeof(subnet));
    subnet.sin6_family = AF_INET6;
    subnet.sin6_addr.s6_addr[prefix_len / 8] = 
                ~CreateMask<uint8_t>(prefix_len % 8);
    for(size_t i = 0; i < prefix_len / 8 ; i++){
        subnet.sin6_addr.s6_addr[i] = 0xff;
    }
    return IPV6Address::ptr(new IPV6Address(subnet));
}

uint16_t IPV6Address::getPort() const {
    return byteswapOnLittleEndian(m_addr.sin6_port);
}

void IPV6Address::setPort(uint16_t v) {
    m_addr.sin6_port = byteswapOnLittleEndian(v);
}

static const size_t MAX_PATH_LEN = sizeof(((sockaddr_un*)0)->sun_path) - 1;  //0表示空指针
    
UnixAddress::UnixAddress(){
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    m_addrlen = offsetof(sockaddr_un, sun_path) + MAX_PATH_LEN;     //offsetof用于获取结构体起始地址到成员的偏移
}   

UnixAddress::UnixAddress(const std::string& path){
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    m_addrlen = path.size() + 1;

    if(!path.empty() && path[0] == '\0'){       //当第一个字符为\0时，Linux 内核将其识别为抽象命名空间路径，而不是常规文件系统路径
        --m_addrlen;
    }
    if(m_addrlen > sizeof(m_addr.sun_path)){
        throw std::logic_error("unix path too long");
    }
    
    memcpy(m_addr.sun_path, path.c_str(), m_addrlen);
    m_addrlen += offsetof(sockaddr_un, sun_path);
}

const sockaddr* UnixAddress::getAddr() const {
    return (sockaddr*)&m_addr;
}

sockaddr* UnixAddress::getAddr() {
    return (sockaddr*)&m_addr;
}

const socklen_t UnixAddress::getAddrLen() const {
    return m_addrlen;
}

std::ostream& UnixAddress::insert(std::ostream& os) const {
    if(m_addrlen > offsetof(sockaddr_un, sun_path) 
        && m_addr.sun_path[0] == '\0'){
        return os << "\\0" << std::string(m_addr.sun_path[1], 
                        m_addrlen - offsetof(sockaddr_un, sun_path) - 1);
    }
    return os << m_addr.sun_path;
}

UnknowAddress::UnknowAddress(int family){
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sa_family = family;
}

UnknowAddress::UnknowAddress(const sockaddr& addr){
    m_addr = addr;
}

const sockaddr* UnknowAddress::getAddr() const {
    return (sockaddr*)&m_addr;
}

sockaddr* UnknowAddress::getAddr() {
    return (sockaddr*)&m_addr;
}

const socklen_t UnknowAddress::getAddrLen() const {
    return sizeof(m_addr);
}

std::ostream& UnknowAddress::insert(std::ostream& os) const {
    return os << "[UnknowAddress family = " << m_addr.sa_family << "]";
}

}