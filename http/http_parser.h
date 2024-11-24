#ifndef __SYLAR_HTTP_PARSER_H__
#define __SYLAR_HTTP_PARSER_H__

#include "http.h"
#include "http11_parser.h"
#include "httpclient_parser.h"

namespace sylar{
namespace http{

class HttpRequestParser{
public:
  typedef std::shared_ptr<HttpRequestParser> ptr;
  HttpRequestParser();

  int isFinished();
  int hasError();
  size_t execute(char* data, size_t len, size_t off = 0);

  HttpRequest::ptr getData() const { return m_data;}
  void setError(int e) {m_error = e;}
  uint64_t getContentLen() const;
private:
  http_parser m_parser;
  //1000: invalid method
  //1001: invalid version
  int m_error = 0;
  HttpRequest::ptr m_data;
};

class HttpResponseParser{
public:
  typedef std::shared_ptr<HttpResponseParser> ptr;
  HttpResponseParser();

  int isFinished();
  int hasError();
  size_t execute(char* data, size_t len, size_t off = 0);

  HttpResponse::ptr getData() const {return m_data;}
  void setError(int e) {m_error = e;}
  uint64_t getContentLen() const;
private:
  httpclient_parser m_parser;
  int m_error = 0;
  HttpResponse::ptr m_data;
};

}
}

#endif