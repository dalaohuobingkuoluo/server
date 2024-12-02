#include "http_server.h"
#include "../my_sylar/log.h"

namespace sylar{
namespace http{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

HttpServer::HttpServer(bool keepalive, sylar::IOManager* worker, 
          sylar::IOManager* accept_worker, sylar::IOManager* io_worker)
          : TcpServer(worker, accept_worker, io_worker),
            m_isKeepAlive(keepalive){
}

void HttpServer::handleClient(sylar::Socket::ptr client) {
    HttpSession::ptr session(new HttpSession(client));
	do{
		auto req = session->recvRequest();
		if(!req){
			SYLAR_LOG_WARN(g_logger) << "recv http request fail, errno = " << errno
									 << ", strerror = " << strerror(errno) 
									 << ", client = " << *client << ", keepalive = " << m_isKeepAlive;
			break;
		}
		HttpResponse:: ptr rsp(new HttpResponse(req->getVersion(), req->isClose() || !m_isKeepAlive));
		rsp->setBody("hello sylar");
		session->sendResponse(rsp);
		SYLAR_LOG_INFO(g_logger) << "request = " << *req;
		SYLAR_LOG_INFO(g_logger) << "response = " << *rsp;
	}while(m_isKeepAlive);
	session->close();
}

}
}