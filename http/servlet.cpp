#include "servlet.h"
#include "../my_sylar/log.h"
#include <fnmatch.h>

namespace sylar{
namespace http{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

FunctionServlet::FunctionServlet(callback cb)
        : Servlet("FunctionServlet"), m_cb(cb) {
}

int32_t FunctionServlet::handle(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session) {
    return m_cb(req, rsp, session);
}

ServletDispatch::ServletDispatch()
        :Servlet("ServletDispatch"){
	m_default.reset(new NotFoundServlet());
}

int32_t ServletDispatch::handle(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session) {
    auto svt = getMatchServlet(req->getPath());
	return svt->handle(req, rsp, session);
}

void ServletDispatch::addServlet(const std::string uri, Servlet::ptr svt){
	RWMutexType::WriteLock lock(m_mutex);
	m_datas[uri] = svt;
}

void ServletDispatch::addServlet(const std::string uri, FunctionServlet::callback cb){
	RWMutexType::WriteLock lock(m_mutex);
	m_datas[uri].reset(new FunctionServlet(cb));
}

void ServletDispatch::addGlobServlet(const std::string uri, Servlet::ptr svt){
	RWMutexType::WriteLock lock(m_mutex);
	for(auto it = m_globs.begin(); it != m_globs.end(); ++it){
		if(!fnmatch(it->first.c_str(), uri.c_str(), 0)){
			m_globs.erase(it);
			break;
		}
	}
	m_globs.push_back(std::make_pair(uri, svt));
}

void ServletDispatch::addGlobServlet(const std::string uri, FunctionServlet::callback cb){
	addGlobServlet(uri, FunctionServlet::ptr(new FunctionServlet(cb)));
}

void ServletDispatch::delServlet(const std::string uri){
	RWMutexType::WriteLock lock(m_mutex);
	auto it = m_datas.find(uri);
	if(it != m_datas.end()){
		m_datas.erase(it);
	}
}

void ServletDispatch::delGlobServlet(const std::string uri){
	RWMutexType::WriteLock lock(m_mutex);
	for(auto it = m_globs.begin(); it != m_globs.end(); ++it){
		if(!fnmatch(it->first.c_str(), uri.c_str(), 0)){
			m_globs.erase(it);
			break;
		}
	}
}

Servlet::ptr ServletDispatch::getServlet(const std::string uri){
	RWMutexType::ReadLock lock(m_mutex);
	auto it = m_datas.find(uri);
	return it == m_datas.end() ? nullptr : it->second;
}

Servlet::ptr ServletDispatch::getGlobServlet(const std::string uri){
	RWMutexType::ReadLock lock(m_mutex);
	for(auto it = m_globs.begin(); it != m_globs.end(); ++it){
		if(!fnmatch(it->first.c_str(), uri.c_str(), 0)){
			return it->second;
		}
	}
	return nullptr;
}

Servlet::ptr ServletDispatch::getMatchServlet(const std::string uri){
    Servlet::ptr res = getServlet(uri);
	if(res){
		return res;
	}
	res = getGlobServlet(uri);
	if(res){
		return res;
	}
	SYLAR_LOG_WARN(g_logger) << "don't find uri = " << uri << ", use default NotFoundServlet";
	return m_default;
}

NotFoundServlet::NotFoundServlet()
		: Servlet("NotFoundServlet"){
}

int32_t NotFoundServlet::handle(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session) {
	
	return 0;
}

}
}