#include "../http/http_server.h"
#include "../my_sylar/iomanager.h"
#include "../my_sylar/log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT;

void run(){
    sylar::http::HttpServer::ptr server(new sylar::http::HttpServer);
    sylar::Address::ptr addr = sylar::Address::LookupAny("0.0.0.0:8020");
    server->bind(addr);
    server->start();
}
//解决请求大于缓冲区长度导致无法一次性解析发生的段错误错误，同时触发新的解析错误但不影响程序运行
//（当某个头过长导致整个缓冲区丢弃时可能该缓冲区存在另一个头的部分值也被丢弃导致解析到的请求头键不完整）

int main(int argc, char **argv){
    sylar::IOManager iom(1);
    iom.schedule(run);
    return 0;
}