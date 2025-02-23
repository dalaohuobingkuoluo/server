#ifndef __SYLAR__LOG__H__
#define __SYLAR__LOG__H__

#include<string>
#include<cstdint>
#include<memory>
#include<list>
#include<fstream>
#include<sstream>
#include<iostream>
#include<vector>
#include<tuple>
#include<map>
#include<set>
#include<time.h>
#include<functional>
#include<stdarg.h>
#include<yaml-cpp/yaml.h>
#include<map>
#include"singleton.h"
#include "thread.h"
#include"util.h"


#define SYLAR_LOG_LEVEL(logger,level)\
        if(logger->getLevel() <= level)\
            sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger,level,__FILE__,__LINE__,0,\
            sylar::GetThreadId(),sylar::GetFiberId(),time(0),sylar::Thread::GetName()))).getSS()

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::FATAL)

#define SYLAR_LOG_FMT_LEVEL(logger,level,fmt,...)\
        if(logger->getLevel() <= level)\
            sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger,level,__FILE__,__LINE__,0,\
            sylar::GetThreadId(),sylar::GetFiberId(),time(0),sylar::Thread::GetName()))).getEvent()->format(fmt,__VA_ARGS__ )

#define SYLAR_LOG_FMT_DEBUG(logger,fmt,...) SYLAR_LOG_FMT_LEVEL(logger,sylar::LogLevel::DEBUG,fmt,__VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger,fmt,...) SYLAR_LOG_FMT_LEVEL(logger,sylar::LogLevel::INFO,fmt,__VA_ARGS__ )
#define SYLAR_LOG_FMT_WARN(logger,fmt,...) SYLAR_LOG_FMT_LEVEL(logger,sylar::LogLevel::WARN,fmt,__VA_ARGS__ )
#define SYLAR_LOG_FMT_ERROR(logger,fmt,...) SYLAR_LOG_FMT_LEVEL(logger,sylar::LogLevel::ERROR,fmt,__VA_ARGS__ )
#define SYLAR_LOG_FMT_FATAL(logger,fmt,...) SYLAR_LOG_FMT_LEVEL(logger,sylar::LogLevel::FATAL,fmt,__VA_ARGS__ )

#define SYLAR_LOG_ROOT sylar::LoggerMgr::GetInstance()->getRoot()
#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::GetInstance()->getLogger(name)

namespace sylar {

    class Logger;
    class LogManager;

    //日志级别
    class LogLevel {
    public:
        enum Level {
            UNKNOW = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };

        static const char* ToString(LogLevel::Level level);
        static LogLevel::Level FromString(const std::string& s);
    };

    //日志事件
    class LogEvent {
        public:
            typedef std::shared_ptr<LogEvent> ptr;
            LogEvent()=default;
            LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char* file, int32_t line, 
            uint32_t elapse, uint32_t threadid, uint32_t fiberid, uint64_t time, const std::string& threadname);

            std::string getContent()const{return m_contentss.str();}
            std::stringstream& getContentss(){return m_contentss;}
            LogLevel::Level getLevel()const{return m_level;}
            uint32_t getElapse()const{return m_elapse;}
            uint64_t getThreadId()const{return m_threadid;}
            uint64_t getTime()const{return m_time;}
            const char* getFile()const{return m_file;}
            uint32_t getLine()const{return m_line;}
            uint32_t getFiberId()const{return m_fiberid;}
            const std::string& getThreadName()const{return m_threadname;}
            std::shared_ptr<Logger> getLogger() const {return m_logger;}

            void format(const char* fmt,...);
            void format(const char* fmt,va_list al);

        private:
            const char* m_file = nullptr; //文件名       通过重定义宏__FILE__修改绝对路径为相对路径
            uint32_t m_line = 0;          //行号
            uint32_t m_elapse = 0;        //程序启动到现在的时间
            uint32_t m_threadid = 0;      //线程号
            uint32_t m_fiberid = 0;       //协程号
            uint64_t m_time = 0;          //时间戳
            std::stringstream m_contentss;//日志内容
            
            std::string m_threadname;     //线程名
            std::shared_ptr<Logger> m_logger;//需要写入的日志器
            LogLevel::Level m_level;      //事件级别
    };

    class LogEventWrap{
        public:
            LogEventWrap(LogEvent::ptr event);
            ~LogEventWrap();
            std::stringstream& getSS();
            LogEvent::ptr getEvent() const {return m_event;}
        private:
            LogEvent::ptr m_event;
    };


    //日志格式器，用于格式化一个日志事件。该类构建时可以指定pattern，表示如何进行格式化。提供format方法，用于将日志事件格式化成字符串。
    class LogFormatter {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        LogFormatter(const std::string& pattern);
        std::string format(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event);
        bool isError() const {return m_error;}
        std::string getPattern() const {return m_pattern;}

        class FormatItem {
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            virtual ~FormatItem(){}
            virtual void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) = 0;
        };

    private:
        std::string m_pattern;                  //要求解析的格式，类似printf自定义的输出格式，如"%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
        void init();                            //用于解析pattern的格式
        bool m_error = false;                   //解析状态
        std::vector<FormatItem::ptr> m_items;   //解析后的格式,需要输出event的哪些信息
        /*
        %m 消息
        %p 日志级别
        %r 累计毫秒数
        %c 日志名称
        %t 线程id
        %n 换行
        %d 时间
        %f 文件名
        %l 行号
        %T 制表符
        %F 协程id 
        %N 线程名称
        默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
        */
    };

    
    //日志输出器，用于将一个日志事件输出到对应的输出地。该类内部包含一个LogFormatter成员和一个log方法，
    //日志事件先经过LogFormatter格式化后再输出到对应的输出地。从这个类可以派生出不同的Appender类型，比如StdoutLogAppender和FileLogAppender，分别表示输出到终端和文件。
    class LogAppender {
    friend class Logger;       //允许所属的Logger直接修改formatter避免使用setFormatter导致m_hasFormatter被修改产生歧义       
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        typedef Mutex MutexType;
        LogAppender() = default;
        virtual ~LogAppender() {}

        //输出日志的目的地根据类型需要重载
        virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) = 0;
        virtual std::string toYamlString() = 0;
         
        //设置Formatter格式
        bool setFormatter(LogFormatter::ptr val);
        bool setFormatter(const std::string& pattern);
        LogFormatter::ptr getFormatter();

        LogLevel::Level getLevel();
        void setLevel(LogLevel::Level val);
    protected:
        LogLevel::Level m_level = LogLevel::DEBUG;//appender日志级别，仅允许级别高于自身的日志可以输出到目的地
        LogFormatter::ptr m_formatter;   //自定义输出格式器
        bool m_hasFormatter = false;     //是否为自定义设置的格式，false则继承所属Logger的格式，true则为自定义的格式
        MutexType m_mutex;
        // Mutex m_mutex_level;
    };

    //日志器，负责进行日志输出。一个Logger包含多个LogAppender和一个日志级别，提供log方法，
    //传入日志事件，判断该日志事件的级别高于日志器本身的级别之后调用LogAppender将日志进行输出，否则该日志被抛弃。
    class Logger : public std::enable_shared_from_this<Logger> {
    friend class LogManager;
    public:
        typedef std::shared_ptr<Logger> ptr;
        typedef Mutex MutexType;

        Logger(const std::string& name = "root");

        //输出不同的日志信息（调用日志输出的根方法）
        void log(LogLevel::Level level, LogEvent::ptr event);   //用于日志输出的根方法
        void debug(LogEvent::ptr event); 
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        //修改appender
        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);
        void clearAppender();
        std::list<LogAppender::ptr> getAppender();

        //操作日志级别
        LogLevel::Level getLevel() const { return m_level; }
        void setLevel(LogLevel::Level level) { m_level = level; }

        //设置Formatter格式并且遍历appenders列表,若appenders中存在未设置formatter的成员则继承logger的formatter格式
        void setFormatter(const std::string& val);
        void setFormatter(const LogFormatter::ptr val);
        LogFormatter::ptr getFormatter();

        std::string toYamlString();

        std::string getName() const {return m_name;}
    private:
        std::string m_name;                                     //日志器名称
        LogLevel::Level m_level;                                //日志器级别,仅当日志级别高于日志器才允许写入appender(在appender中再次判断与appender的级别)
        MutexType m_mutex;                                          //对appender列表加锁
        std::list<LogAppender::ptr> m_appenders;                //appender列表
        LogFormatter::ptr m_formatter;                          //默认的输出格式（在addAppender中不一定设置额外的格式，此时用该成员设置）
        Logger::ptr m_root;                                     //当m_appender为空，默认使用LogMgr中的m_root输出
    };


    class LogManager{
        public:
            typedef Mutex MutexType;
            LogManager();
            Logger::ptr getLogger(const std::string& name);         //返回或生成name的Logger
            void init();
            Logger::ptr getRoot() const {return m_root;}
            std::string toYamlString();
        private:
            std::map<std::string,Logger::ptr> m_loggers;
            Logger::ptr m_root;
            MutexType m_mutex;
    };

    typedef sylar::Singleton<LogManager> LoggerMgr;


    //输出到文件的Appender
    class FileAppender : public LogAppender {
    public:
        typedef std::shared_ptr<FileAppender> ptr;
        FileAppender(const std::string name) : m_filename(name) {reopen();}
        void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override;
        std::string toYamlString() override;
        bool reopen();
    private:
        std::string m_filename;
        std::ofstream m_filestream;
        uint64_t m_lastTime = 0;                //每秒检查一次并重新打开文件，防止文件被删除程序无法感知该异常
    };

    //输出到控制台的Appender
    class StdoutAppender : public LogAppender {
    public:
        typedef std::shared_ptr<StdoutAppender> ptr;
        StdoutAppender() = default;
        void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override;
        std::string toYamlString() override;
    private:
    };
};

    
#endif
