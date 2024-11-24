#include "../my_sylar/sylar.h"
#include "../http/http_parser.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT;
const char test_request[] = "GET / HTTP/1.1\r\n"
                            "Host: www.sylar.top\r\n"
                            "Content-Length: 100\r\n\r\n"
                            "1234567890";

void test(){
    sylar::http::HttpRequestParser parser;
    std::string tmp = test_request;
    size_t rt = parser.execute(&tmp[0], tmp.size());
    SYLAR_LOG_INFO(g_logger) << "request parser execute rt = " << rt        
                             << " has_error = " << parser.hasError()
                             << " is_finished = " << parser.isFinished()
                             << "totoal len = " << tmp.size()
                             << " content len = " << parser.getContentLen();
    SYLAR_LOG_INFO(g_logger) << parser.getData()->toString();
}

const char test_response_data[] = "HTTP/1.1 200 OK\r\n"
        "Date: Tue, 04 Jun 2019 15:43:56 GMT\r\n"
        "Server: Apache\r\n"
        "Last-Modified: Tue, 12 Jan 2010 13:48:00 GMT\r\n"
        "ETag: \"51-47cf7e6ee8400\"\r\n"
        "Accept-Ranges: bytes\r\n"
        "Content-Length: 81\r\n"
        "Cache-Control: max-age=86400\r\n"
        "Expires: Wed, 05 Jun 2019 15:43:56 GMT\r\n"
        "Connection: Close\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<html>\r\n"
        "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com/\">\r\n"
        "</html>\r\n";

void test_response() {
    sylar::http::HttpResponseParser parser;
    std::string tmp = test_response_data;
    size_t s = parser.execute(&tmp[0], tmp.size());
    SYLAR_LOG_ERROR(g_logger) << "execute rt=" << s
        << " has_error=" << parser.hasError()
        << " is_finished=" << parser.isFinished()
        << " total=" << tmp.size()
        << " content_length=" << parser.getContentLen()
        << " tmp[s]=" << tmp[s];
    SYLAR_LOG_INFO(g_logger) << tmp;
    tmp.resize(tmp.size() - s);

    SYLAR_LOG_INFO(g_logger) << parser.getData()->toString();
    SYLAR_LOG_INFO(g_logger) << tmp;
}


int main(){
    test_response();
    return 0;
}