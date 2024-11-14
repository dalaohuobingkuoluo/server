#include "../my_sylar/address.h"
#include "../my_sylar/sylar.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT;

void test(){
    std::vector<sylar::Address::ptr> res;
    const std::string host = "wwww.baidu.com:80";
    SYLAR_LOG_INFO(g_logger) << "begin";
    bool rt = sylar::Address::Lookup(res, host, AF_UNSPEC, SOCK_STREAM);
    SYLAR_LOG_INFO(g_logger) << "end";
    if(!rt){
        SYLAR_LOG_ERROR(g_logger) << "Address::Lookup fail";
        return;
    }
    for(const auto &v : res){
        SYLAR_LOG_INFO(g_logger) << v->toString();
    }
}
//bug：(1)IPv4地址反了，可能大小端转换出错（www.baidu.com: 220.181.38.148 -> 148.38.181.220）;
//     (2)Lookup函数中无法识别服务或端口号(wwww.baidu.com:http)，单独调用getaddrinfo("www.baidu.com", "http", &hints, &results)时成功
//     (3)以服务名传递时不会出现重复，而以端口号传递时会导致结果重复出现三次
//resolve：(1)IPv4:insert函数中输出前应先转换大小端
//         (2)node截取host中主机地址出错将‘:’一起截取导致解析错误
//         (3)由于Lookup参数socktype等未指定，导致返回包含多个 socktype 和 protocol 的组合从而返回多个相同的 IP 地址，但协议类型不同，限制socktype即可解决

void test_iface(){
    std::multimap<std::string, std::pair<sylar::Address::ptr, uint32_t>> result;
    bool rt = sylar::Address::GetInterfaceAddress(result, AF_UNSPEC);
    if(!rt){
        SYLAR_LOG_ERROR(g_logger) << "Address::GetInterfaceAddress fail";
        return;
    }
    for(const auto &v : result){
        SYLAR_LOG_INFO(g_logger) << v.first << " " << v.second.first->toString() << " " << v.second.second;
    }
}
//bug：IP地址掩码长度有误
//resolve：误将ifa_netmask写成ifa_addr

void test_ipv4(){
    auto addr = sylar::IPAddress::Create("220.181.38.150");
    if(addr){
        SYLAR_LOG_INFO(g_logger) << addr->toString();
        return;
    }
}

int main(){
    // test_iface();
    test_ipv4();
    return 0;
}