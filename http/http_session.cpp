#include "http_session.h"
#include "http_parser.h"
#include "../my_sylar/log.h"

namespace sylar{
namespace http{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

HttpSession::HttpSession(Socket::ptr sock, bool owner)
        : SocketStream(sock, owner){
}

HttpRequest::ptr HttpSession::recvRequest(){
    HttpRequestParser::ptr parser(new HttpRequestParser);
    uint64_t buf_size = HttpRequestParser::getHttpRequestBufferSize();
    std::shared_ptr<char> buf(new char[buf_size], [](char* ptr){delete[] ptr;});
    char *data = buf.get();
    int off = 0;
    // SYLAR_LOG_DEBUG(g_logger) << "recvRequest";
    do{
        int len = read(data + off, buf_size - off);
        if(len <= 0){
            // SYLAR_LOG_DEBUG(g_logger) << "recv len = " << len;
            return nullptr;
        }
        // SYLAR_LOG_DEBUG(g_logger) << "recv data = " << data;
        len += off;
        size_t nparse = parser->execute(data, len);
        if(parser->hasError()){
            return nullptr;
        }
        off = len - nparse;
        if(off == buf_size){
            return nullptr;
        }
        if(parser->isFinished()){
            break;
        }
    }while(1);
    // SYLAR_LOG_DEBUG(g_logger) << "recv data = " << data;
    int64_t body_len = parser->getContentLen();
    if(body_len > 0){
        std::string body;
        body.reserve(body_len);
        if(body_len > off){
            memcpy(&body[0], data, off);
        }else {
            memcpy(&body[0], data, body_len);
        }
        body_len -= off;
        if(body_len > 0){
            if(readFixLen(&body[body.size()], body_len) <= 0){
                return nullptr;
            }
            parser->getData()->setBody(body);
        }
    }
    return parser->getData();
}

int HttpSession::sendResponse(HttpResponse::ptr rsp){
    std::stringstream ss;
    ss << *rsp;
    std::string data = ss.str();
    return writeFixLen(data.c_str(), data.size());
}

}
}