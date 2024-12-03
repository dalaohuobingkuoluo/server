#ifndef __SYLAR_HTTP_SERVLET_H__
#define __SYLAR_HTTP_SERVLET_H__

#include "http_session.h"
#include "http.h"
#include "../my_sylar/thread.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

namespace sylar{
namespace http{

class Servlet{
public:
    typedef std::shared_ptr<Servlet> ptr;
    Servlet(const std::string &name) : m_name(name){}
    ~Servlet(){}
    virtual int32_t handle(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session) = 0;
    const std::string& getName() const {return m_name;}
protected:
    std::string m_name;
};

class FunctionServlet : public Servlet {
public:
    typedef std::shared_ptr<FunctionServlet> ptr;
    typedef std::function<int32_t(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session)> callback;

    FunctionServlet(callback cb);
    virtual int32_t handle(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session) override;
private:
    callback m_cb;
};

class ServletDispatch : public Servlet {
public:
    typedef std::shared_ptr<ServletDispatch> ptr;
    typedef sylar::RWMutex RWMutexType; 

    ServletDispatch();
    virtual int32_t handle(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session) override;

    void addServlet(const std::string uri, Servlet::ptr svt);
    void addServlet(const std::string uri, FunctionServlet::callback cb);
    void addGlobServlet(const std::string uri, Servlet::ptr svt);
    void addGlobServlet(const std::string uri, FunctionServlet::callback cb);

    void delServlet(const std::string uri);
    void delGlobServlet(const std::string uri);

    Servlet::ptr getServlet(const std::string uri);
    Servlet::ptr getGlobServlet(const std::string uri);
    Servlet::ptr getMatchServlet(const std::string uri);

    Servlet::ptr getDefault() const {return m_default;}
    void getDefault(Servlet::ptr v) {m_default = v;}
private:
    //精准匹配 uri(/sylar/xx)  --> servlet
    std::unordered_map<std::string, Servlet::ptr> m_datas;
    //模糊匹配 uri(/sylar/*) --> servlet
    std::vector<std::pair<std::string, Servlet::ptr>> m_globs;
    //匹配失败默认转发
    Servlet::ptr m_default;
    RWMutexType m_mutex;

};

}
}

#endif