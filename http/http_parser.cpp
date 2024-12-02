#include "http_parser.h"
#include "../my_sylar/log.h"
#include "../my_sylar/config.h"


namespace sylar{
namespace http{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static sylar::ConfigVar<uint64_t>::ptr g_http_request_buffer_size = 
    sylar::Config::Lookup<uint64_t>("http.request.buffer_size", 4 * 1024ull, "http request buffer_size");
static sylar::ConfigVar<uint64_t>::ptr g_http_request_body_size = 
    sylar::Config::Lookup<uint64_t>("http.request.body_size", 4 * 1024 * 1024ull, "http request body_size");

static uint64_t s_http_request_buffer_size;
static uint64_t s_http_request_body_size;

uint64_t HttpRequestParser::getHttpRequestBufferSize(){
    return s_http_request_buffer_size;
}

uint64_t HttpRequestParser::getHttpRequestBodySize(){
    return s_http_request_body_size;
}

namespace{
struct _RequestSizeInit{
    _RequestSizeInit(){
        s_http_request_buffer_size = g_http_request_buffer_size->getValue();
        s_http_request_body_size = g_http_request_body_size->getValue();
        g_http_request_buffer_size->addListener([](const uint64_t &ov, const uint64_t &nv){
            s_http_request_buffer_size = nv;
        });
        g_http_request_body_size->addListener([](const uint64_t &ov, const uint64_t &nv){
            s_http_request_body_size = nv;
        });
    };
};
static _RequestSizeInit _init;    
}


void on_request_http_filed(void *data, const char *field, size_t flen, const char *value, size_t vlen){
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    if(flen == 0){
        SYLAR_LOG_WARN(g_logger) << "invalid http request filed len = 0";
        return;
    }
    parser->getData()->setHeader(std::string(field, flen), std::string(value, vlen));
}

void on_request_method(void *data, const char *at, size_t length){
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    HttpMethod method = CharsToHttpMethod(at);
    if(method == HttpMethod::INVALID_METHOD){
        SYLAR_LOG_WARN(g_logger) << "invalid http request method : " 
                                 << std::string(at, length);
        parser->setError(1000);
        return;
    }
    parser->getData()->setMethod(method);
}

void on_request_uri(void *data, const char *at, size_t length){

}

void on_request_fragment(void *data, const char *at, size_t length){
    HttpRequestParser* paser = static_cast<HttpRequestParser*>(data);
    paser->getData()->setFragment(std::string(at, length));
}

void on_request_path(void *data, const char *at, size_t length){
    HttpRequestParser* paser = static_cast<HttpRequestParser*>(data);
    paser->getData()->setPath(std::string(at, length));
}

void on_request_query(void *data, const char *at, size_t length){
    HttpRequestParser* paser = static_cast<HttpRequestParser*>(data);
    paser->getData()->setQuery(std::string(at, length));
}

void on_request_version(void *data, const char *at, size_t length){
    uint8_t v = 0;
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    if(strncmp(at, "HTTP/1.1", length) == 0){
        v = 0x11;
    }else if(strncmp(at, "HTTP/1.0", length) == 0){
        v = 0x10;
    }else {
        SYLAR_LOG_WARN(g_logger) << "invalid http request version : " 
                                 << std::string(at, length);
        parser->setError(1001);
        return;
    }
    parser->getData()->setVersion(v);
}

void on_request_header_done(void *data, const char *at, size_t length){

}

HttpRequestParser::HttpRequestParser(){
    m_data.reset(new HttpRequest());
    http_parser_init(&m_parser);
    m_parser.http_field = on_request_http_filed;
    m_parser.request_method = on_request_method;
    m_parser.request_uri = on_request_uri;
    m_parser.fragment = on_request_fragment;
    m_parser.request_path = on_request_path;
    m_parser.query_string = on_request_query;
    m_parser.http_version = on_request_version;
    m_parser.header_done = on_request_header_done;
    m_parser.data = this;
}

int HttpRequestParser::isFinished(){
    return http_parser_finish(&m_parser);
}

int HttpRequestParser::hasError(){
    return m_error || http_parser_has_error(&m_parser);
}

size_t HttpRequestParser::execute(char* data, size_t len, size_t off){
    size_t rt = http_parser_execute(&m_parser, data, len, off);
    memmove(data, data + off + rt, len - off - rt);
    return rt;
}

uint64_t HttpRequestParser::getContentLen() const{
    return m_data->getHeaderAs<uint64_t>("Content-Length", 0);
}

void on_response_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen){
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    if(flen == 0){
        SYLAR_LOG_WARN(g_logger) << "invalid http response filed len = 0";
        return;
    }
    parser->getData()->setHeader(std::string(field, flen), std::string(value, vlen));
}

void on_response_reason(void *data, const char *at, size_t length){
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    parser->getData()->setReason(std::string(at,length));
}

void on_response_status(void *data, const char *at, size_t length){
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    parser->getData()->setStatus(HttpStatus(atoi(at)));
}

void on_response_chunk(void *data, const char *at, size_t length){

}

void on_response_version(void *data, const char *at, size_t length){
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    uint8_t v = 0;
    if(strncmp(at, "http/1.1", length) == 0){
        v = 0x11;
    } else if(strncmp(at, "http/1.0", length) == 0) {
        v = 0x10;
    } else {
        SYLAR_LOG_WARN(g_logger) << "invalid http response version : " 
                                 << std::string(at, length);
        parser->setError(1001);
        return;
    }
    parser->getData()->setVetsion(v);
}

void on_response_header_done(void *data, const char *at, size_t length){

}

void on_response_last_chunk(void *data, const char *at, size_t length){

}

HttpResponseParser::HttpResponseParser(){
    m_data.reset(new HttpResponse());
    httpclient_parser_init(&m_parser);
    m_parser.http_field = on_response_http_field;
    m_parser.reason_phrase = on_response_reason;
    m_parser.status_code = on_response_status;
    m_parser.chunk_size = on_response_chunk;
    m_parser.http_version = on_response_version;
    m_parser.header_done = on_response_header_done;
    m_parser.last_chunk = on_response_last_chunk;
    m_parser.data = this;
}

int HttpResponseParser::isFinished(){
    return httpclient_parser_finish(&m_parser);
}

int HttpResponseParser::hasError(){
    return m_error || httpclient_parser_has_error(&m_parser);;
}

size_t HttpResponseParser::execute(char* data, size_t len, size_t off){
    size_t rt = httpclient_parser_execute(&m_parser, data, len, off);
    memmove(data, data + off + rt, len - rt - off);
    return rt;
}   

uint64_t HttpResponseParser::getContentLen() const{
    return m_data->getHeaderAs<uint64_t>("Content-Length", 0);
}

}
}