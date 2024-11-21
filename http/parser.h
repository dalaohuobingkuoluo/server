#include "httpclient_parser.h"
#include "http.h"

namespace sylar{
namespace http{

class HttpRequestParser{
public:
  typedef std::shared_ptr<HttpRequestParser> ptr;
  HttpRequestParser();

  int isFinished() const;
  int hasError() const;
  size_t execute(const char* data, size_t len, size_t off);
private:
  http_parser m_parser;
  int m_error;
  HttpRequest::ptr m_request;
};

class HttpResponseParser{
public:
  typedef std::shared_ptr<HttpResponseParser> ptr;
  HttpResponseParser();

  int isFinished() const;
  int hasError() const;
  size_t execute(const char* data, size_t len, size_t off);
private:
  httpclient_parser m_parser;
  int m_error;
  HttpResponse::ptr m_response;
};

}
}