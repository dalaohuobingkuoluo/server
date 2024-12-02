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

int main(int argc, char **argv){
    sylar::IOManager iom(1);
    iom.schedule(run);
    return 0;
}