#include "servlet.h"

namespace sylar{
namespace http{

FunctionServlet::FunctionServlet(callback cb)
        : Servlet("FunctionServlet"), m_cb(cb) {
}

int32_t FunctionServlet::handle(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session) {
    
}

ServletDispatch::ServletDispatch()
        :Servlet("ServletDispatch"){

}
int32_t ServletDispatch::handle(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session) {
    
}

void ServletDispatch::addServlet(const std::string uri, Servlet::ptr svt){

}
void ServletDispatch::addServlet(const std::string uri, FunctionServlet::callback cb){

}
void ServletDispatch::addGlobServlet(const std::string uri, Servlet::ptr svt){

}
void ServletDispatch::addGlobServlet(const std::string uri, FunctionServlet::callback cb){

}

void ServletDispatch::delServlet(const std::string uri){

}
void ServletDispatch::delGlobServlet(const std::string uri){

}

Servlet::ptr ServletDispatch::getServlet(const std::string uri){

}
Servlet::ptr ServletDispatch::getGlobServlet(const std::string uri){

}
Servlet::ptr ServletDispatch::getMatchServlet(const std::string uri){
    
}



}
}