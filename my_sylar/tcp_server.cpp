#include "tcp_server.h"
#include "log.h"
#include "config.h"

namespace sylar{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
static sylar::ConfigVar<uint64_t>::ptr g_tcp_server_recv_timeout = 
            sylar::Config::Lookup<uint64_t>("tcp_server.recv_tieout", (uint64_t)(60 * 1000 * 2), "tcp server recv timeout");

TcpServer::TcpServer(sylar::IOManager* worker, sylar::IOManager* accept_worker, sylar::IOManager* io_worker)
        : m_worker(worker), m_acceptWorker(accept_worker), m_ioWorker(io_worker),
          m_recvTimeout(g_tcp_server_recv_timeout->getValue()),
          m_name("sylar/1.0.0"), m_isStop(true){
}

TcpServer::~TcpServer(){
	if(!m_isStop){
		stop();
	}
}

bool TcpServer::bind(sylar::Address::ptr addr){
	std::vector<sylar::Address::ptr> addrs;
	std::vector<sylar::Address::ptr> fails;
	addrs.push_back(addr);
	return bind(addrs, fails);
}

bool TcpServer::bind(const std::vector<sylar::Address::ptr> &addrs,
                 std::vector<sylar::Address::ptr> &fails){
	for(auto &addr : addrs){
		sylar::Socket::ptr sock = sylar::Socket::CreateTCP(addr);
		if(!sock->bind(addr)){
			SYLAR_LOG_ERROR(g_logger) << m_name << " bind error, addr=[" << addr->toString() << "]";
			fails.push_back(addr);
			continue;
		}
		SYLAR_LOG_DEBUG(g_logger) << m_name << " bind sock = " << sock->toString();
		if(!sock->listen()){
			SYLAR_LOG_ERROR(g_logger) << m_name << " listen error, addr=[" << addr->toString() << "]";
			fails.push_back(addr);
			continue;
		}
		m_socks.push_back(sock);
	}
	if(!fails.empty()){
		m_socks.clear();
		return false;
	}
	for(const auto &sock : m_socks){
		SYLAR_LOG_INFO(g_logger) << "name = " << m_name << " server bind " 
								 << sock->toString() << " successful";  
	}
	return true;
}
                 
bool TcpServer::start(){
	if(!m_isStop){
		return true;
	}
	m_isStop = false;
	for(auto & sock : m_socks){
		m_acceptWorker->schedule(std::bind(&TcpServer::startAccept, shared_from_this(), sock));
	}
	return true;
}

void TcpServer::stop(){
	m_isStop = true;
	auto self = shared_from_this();
	m_acceptWorker->schedule([self, this](){
		for(auto &sock : m_socks){
			sock->cancelAll();
			sock->close();
		}
		m_socks.clear();
	});
}

void TcpServer::handleClient(sylar::Socket::ptr client){
	SYLAR_LOG_INFO(g_logger) << client->toString() <<"handleClient";
}

void TcpServer::startAccept(sylar::Socket::ptr sock){
	while(!m_isStop){
		Socket::ptr client = sock->accept();
		if(!client){
			SYLAR_LOG_ERROR(g_logger) << m_name << " listen_sock = " << sock->toString() << "accept error";
		}else{
			client->setRecvTimeout(m_recvTimeout);
			m_ioWorker->schedule(std::bind(&TcpServer::handleClient, shared_from_this(), client));	//防止该TcpServer提前被析构导致发生错误
		}
	}
}

}