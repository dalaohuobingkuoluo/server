#include<iostream>
#include "../my_sylar/log.h"
#include "../my_sylar/util.h"


int main(){
    sylar::Logger::ptr logger(new sylar::Logger);
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutAppender()));
    sylar::FileAppender::ptr fileapp(sylar::FileAppender::ptr(new sylar::FileAppender("../logfile.txt")));
    logger->addAppender(fileapp);
    sylar::LogFormatter::ptr fmt(sylar::LogFormatter::ptr(new sylar::LogFormatter("%d%T%m%T%p%n")));
    fileapp->setFormatter(fmt);
    fileapp->setLevel(sylar::LogLevel::ERROR);
    //sylar::LogEvent::ptr event(new sylar::LogEvent(__FILE__, __LINE__, 0, sylar::GetThreadId(),sylar::GetFiberId(), time(0)));
    //event->getContentss() << "hello sylar log";
    //logger->log(sylar::LogLevel::DEBUG, event);
    //sleep(20);
    //SYLAR_LOG_INFO(logger)<<"test micro";
    //SYLAR_LOG_ERROR(logger)<<"test micro error";
    //SYLAR_LOG_DEBUG(logger)<<"test micro debug";
    //SYLAR_LOG_WARN(logger)<<"test micro warn";
    //SYLAR_LOG_FATAL(logger)<<"test micro fatal";
 
    SYLAR_LOG_FMT_DEBUG(logger,"test micro %s","debug");
    SYLAR_LOG_FMT_ERROR(logger,"test micro %s","ERROR");
    auto log = sylar::LoggerMgr::GetInstance()->getLogger("xx");
    SYLAR_LOG_INFO(log)<<"from mgr";
    std::cout << "hello sylar log" << std::endl;
    return 0;
}