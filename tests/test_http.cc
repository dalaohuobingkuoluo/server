#include "../my_sylar/sylar.h"
#include "../http/http.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT; 

void test(){
    sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest());
    req->setHeader("host", "www.sylar.top");
    req->setBody("hello sylar");
    req->dump(std::cout) << std::endl;

    sylar::http::HttpResponse::ptr rsp(new sylar::http::HttpResponse());
    rsp->setHeader("x-x", "sylar.top");
    rsp->setStatus((sylar::http::HttpStatus)400);
    rsp->setBody("hello sylar");
    rsp->dump(std::cout) << std::endl;
}

int main(){
    test();
    return 0;
}
