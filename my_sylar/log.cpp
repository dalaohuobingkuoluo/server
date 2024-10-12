#include "log.h"
#include "config.h"

namespace sylar {

	const char* LogLevel::ToString(LogLevel::Level level) {
		switch (level) {
#define XX(name)\
		case LogLevel::name:\
			return #name;\
			break;

			XX(DEBUG);
			XX(INFO);
			XX(WARN);
			XX(ERROR);
			XX(FATAL);
		default:
			return "UNKNOW";
#undef XX
		}
		return "UNKNOW";
	}

	LogLevel::Level LogLevel::FromString(const std::string& s) {
#define XX(level,v)\
	if(s == #v)\
		return LogLevel::level;
		
		XX(DEBUG, debug);
		XX(INFO, info);
		XX(WARN, warn);
		XX(ERROR, error);
		XX(FATAL, fatal);
		XX(DEBUG,DEBUG);
		XX(INFO,INFO);
		XX(WARN,WARN);
		XX(ERROR,ERROR);
		XX(FATAL,FATAL);
#undef XX
		return LogLevel::UNKNOW;
	}

	LogEvent::LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,const char* file, int32_t line, 
	uint32_t elapse, uint32_t threadid, uint32_t fiberid, uint64_t time, const std::string& threadname)
		: m_file(file), m_line(line), m_elapse(elapse), m_threadid(threadid), m_fiberid(fiberid), 
		m_time(time),m_threadname(threadname),m_logger(logger),m_level(level)
	{}

    //format函数是一个可变参数函数，第一个参数fmt是一个格式化字符串，后面的参数可以是任意数量的参数，用于填充格式化字符串中的占位符
	void LogEvent::format(const char* fmt,...){
		va_list va;
		va_start(va,fmt);
		format(fmt,va);
		va_end(va);
	}

    void LogEvent::format(const char* fmt,va_list va){
		char *buff = nullptr;
		//vasprintf由系统自动分配足够的内存
		int len = vasprintf(&buff,fmt,va);
		if(len!=-1){
			m_contentss<<std::string(buff);
			free(buff);
		}
	}

	LogEventWrap::LogEventWrap(LogEvent::ptr event) : m_event(event){
	}

	LogEventWrap::~LogEventWrap(){
		m_event->getLogger()->log(m_event->getLevel(),m_event);
		//std::cout<<"deconstruct wrap"<<std::endl;
	}

	std::stringstream& LogEventWrap::getSS(){
		return m_event->getContentss();
	}

	LogFormatter::LogFormatter(const std::string& pattern) : m_pattern(pattern){
		init();
	}

	std::string LogFormatter::format(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) {
		std::stringstream ss;
		for (auto& item : m_items) {
			item->format(ss,logger, level, event);
		}
		// std::cout << "start++++++++++++++++++" <<std::endl;
		// std::cout<<"content : " << ss.str() <<std::endl;
		// std::cout << "end++++++++++++++++++" <<std::endl;
		return ss.str();
	}

	class MessageItem : public LogFormatter::FormatItem {
		public:
			MessageItem(const std::string &str = ""){}
			void format(std::ostream& os,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override {
				os << event->getContent();
			}
	};

	class LevelItem : public LogFormatter::FormatItem {
		public:
			LevelItem(const std::string &str = ""){}
			void format(std::ostream& os,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override {
				os << LogLevel::ToString(level);
			}
	};

	class ElapseItem : public LogFormatter::FormatItem {
		public:
			ElapseItem(const std::string &str = ""){}
			void format(std::ostream& os,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override {
				os << event->getElapse();
			}
	};
	
	class NameItem : public LogFormatter::FormatItem {
		public:
			NameItem(const std::string &str = ""){}
			void format(std::ostream& os,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override {
				os << event->getLogger()->getName();
			}
	};

	class ThreadIdItem : public LogFormatter::FormatItem {
		public:
			ThreadIdItem(const std::string &str = ""){}
			void format(std::ostream& os,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override {
				os << event->getThreadId();
			}
	};

	class NewLineItem : public LogFormatter::FormatItem {
		public:
			NewLineItem(const std::string &str = ""){}
			void format(std::ostream& os,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override {
				os << std::endl;
			}
	};

	class DateTimeItem : public LogFormatter::FormatItem {
		public:
			DateTimeItem(const std::string& str = "%Y-%m-%d %H:%M:%S") : m_format(str){
				if(m_format.empty())
					m_format = "%Y-%m-%d %H:%M:%S";
			}

			void format(std::ostream& os,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override {
				struct tm stm;
				time_t time = event->getTime();
				localtime_r(&time, &stm);
				char buf[64]={};
				strftime(buf,sizeof(buf),m_format.c_str(),&stm);
				os << buf;
			}
		private:
			std::string m_format;
	};

	class FilenameItem : public LogFormatter::FormatItem {
		public:
			FilenameItem(const std::string &str = ""){}
			void format(std::ostream& os,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override {
				os << event->getFile();
			}
	};

	class LineItem : public LogFormatter::FormatItem {
		public:
			LineItem(const std::string &str = ""){}
			void format(std::ostream& os,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override {
				os << event->getLine();
			}
	};	

	class TabItem : public LogFormatter::FormatItem {
		public:
			TabItem(const std::string &str = ""){}
			void format(std::ostream& os,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override {
				os << "\t";
			}
	};

	class FiberIdItem : public LogFormatter::FormatItem {
		public:
			FiberIdItem(const std::string &str = ""){}
			void format(std::ostream& os,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override {
				os <<event->getFiberId();
			}
	};

	class ThreadNameItem : public LogFormatter::FormatItem {
		public:
			ThreadNameItem(const std::string &str = ""){}
			void format(std::ostream& os,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override {
				os <<event->getThreadName();
			}
	};

	class StringItem : public LogFormatter::FormatItem {
		public:
			StringItem(const std::string &str = "") : m_str(str) {}
			void format(std::ostream& os,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event) override {
				os << m_str;
			}
		private:
			std::string m_str;
	};
	
	//1.%x 2.%x{xxx...} 3.%%  4.xx  四种格式 %m  %d{%Y-%m...} %% : (str,fmt,type)---type=0（无格式数据） type=1(格式化数据如 %d、%m....)   type=2(格式解析错误)                                           
 	void LogFormatter::init() {
		std::vector<std::tuple<std::string, std::string, int>> vec;
		//nstr存储无需格式化的数据        fmt存储额外指定的格式（如时间以某种格式输出）
		std::string nstr="";
		for (size_t i = 0; i < m_pattern.size(); i++) {
			//无格式数据  xx
			if (m_pattern[i] != '%') {
				nstr.append(1, m_pattern[i]);
				continue;
			}
			// %%
			if ((i+1) < m_pattern.size() && m_pattern[i+1] == '%') {
				nstr.append(1, '%');
				i++;
				continue;
			}
			//%....格式化数据
			size_t n = i + 1, fmt_s = 0;
			short state = 0;
			//str存储需格式化的数据，如d,m等的具体值从LogEvent获取 
			std::string str="", fmt="";
			while (n < m_pattern.size()) {
				//读取%后第一个,错误处理放在map中
				if (!state) {
					str.append(1, m_pattern[n]);
					if(m_pattern[n+1] != '{')
						break;
					else{
						n++;
						state = 1;
						fmt_s = n;
					}//读取额外的格式{...}
				}
				if (state == 1 && m_pattern[n] == '}') {
					fmt = m_pattern.substr(fmt_s + 1, n - fmt_s - 1);
					state = 0;
					break;
				}
				n++;
				if(n == m_pattern.size()){
					fmt = m_pattern.substr(fmt_s + 1);
				}
			}
			i = n;
			if(state == 0){
				if(!nstr.empty()){
					vec.push_back(std::make_tuple(nstr,std::string(),0));
					nstr.clear();
				}
				if(!str.empty()){
					vec.push_back(std::make_tuple(str,fmt,1));
					str.clear();
				}
			}else if(state == 1){
				std::cout << "pattern parse error : " << str << " " << fmt <<std::endl;
				m_error = true;
				vec.push_back(std::make_tuple("<pattern error>"+str,fmt,2));
			}
    	}

		static std::map<std::string, std::function<FormatItem::ptr(const std::string & str)>> s_format_item = {
#define XX(str,C)\
		{#str, [](const std::string &fmt) {return FormatItem::ptr(new C(fmt));}}
			XX(m,MessageItem),  	//%m 消息
			XX(p,LevelItem),  		//%p 日志级别
			XX(r,ElapseItem),  		//%r 累计毫秒数
			XX(c,NameItem), 		//%c 日志名称
			XX(t,ThreadIdItem),  	//%t 线程id
			XX(n,NewLineItem),  	//%n 换行
			XX(d,DateTimeItem),  	// %d 时间
			XX(f,FilenameItem),  	//%f 文件名
			XX(l,LineItem),  		//%l 行号
			XX(T,TabItem),  		//%T 制表符
			XX(F,FiberIdItem),  	//%F 协程id
			XX(N,ThreadNameItem),  	//%N 线程名称
#undef XX			
		};


		for(const auto &v : vec){
			//std::cout << "(" << std::get<0>(v) << ") - " << "(" << std::get<1>(v) <<
						 //") - " << "(" << std::get<2>(v) << ")" <<std::endl;
			if(std::get<2>(v) == 0){
				m_items.push_back(FormatItem::ptr(new StringItem(std::get<0>(v))));
			}else if(std::get<2>(v) == 1){
				auto it = s_format_item.find(std::get<0>(v));
				if( it == s_format_item.end()){
					m_items.push_back(FormatItem::ptr(new StringItem("<<error format %" + std::get<0>(v) + " >>")));
					m_error = true;
				}else{
					m_items.push_back(it->second(std::get<1>(v)));
				}
			}
		}

	}

	LogLevel::Level LogAppender::getLevel()  {
		// Mutex::Lock lock(m_mutex_level);
		return m_level;
	}

    void LogAppender::setLevel(LogLevel::Level val) {
		// Mutex::Lock lock(m_mutex_level);
		m_level = val;
	}

	bool LogAppender::setFormatter(LogFormatter::ptr val) {
		MutexType::Lock lock(m_mutex);
		if(!val->isError()){
			m_formatter = val;
			if(m_formatter){
				m_hasFormatter = true;
				//std::cout << m_formatter->getPattern() <<std::endl;
			}
			return true;
		} else{
			std::cout << "try to set formatter but pattern error, pattern =  " << val->getPattern() <<std::endl; 
			return false;
		}		
	}

	bool LogAppender::setFormatter(const std::string& pattern) { 
		LogFormatter::ptr fmt(new LogFormatter(pattern));
		return setFormatter(fmt);
	}

	LogFormatter::ptr LogAppender::getFormatter() {
		MutexType::Lock lock(m_mutex);
		return m_formatter;
	}

	Logger::Logger(const std::string& name) : m_name(name),m_level(LogLevel::DEBUG) {
		m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
	}

	std::string Logger::toYamlString(){
		MutexType::Lock lock(m_mutex);
		YAML::Node node;
		std::stringstream ss;
		node["name"] = m_name;
		node["level"] = sylar::LogLevel::ToString(m_level);;
		if(m_formatter){
			node["formatter"] = m_formatter->getPattern();
		}
		for(const auto& a : m_appenders){
			node["appenders"].push_back(YAML::Load(a->toYamlString()));
		}
		ss << node;
		return ss.str();
	}

	void Logger::addAppender(LogAppender::ptr appender) {
		MutexType::Lock lock(m_mutex);
		//防止appender本身为空
		if(!appender->getFormatter()){
			MutexType::Lock slock(appender->m_mutex);
			//appender->setFormatter(m_formatter);  不用setFormatter防止修改hasFormatter的值
			appender->m_formatter = m_formatter;
		}
		m_appenders.push_back(appender);
	}

	void Logger::delAppender(LogAppender::ptr appender) {
		MutexType::Lock lock(m_mutex);
		for (auto it = m_appenders.begin(); it != m_appenders.end(); it++) {
			if (*it == appender) {
				m_appenders.erase(it);
				break;
			}
		}
	}

	void Logger::clearAppender() {
		MutexType::Lock lock(m_mutex);
		m_appenders.clear();
	}
        
	std::list<LogAppender::ptr> Logger::getAppender()  {
		MutexType::Lock lock(m_mutex);
		return m_appenders;
	}

	void Logger::setFormatter(const std::string& val){
		LogFormatter::ptr logformatter(new LogFormatter(val));
		if(logformatter->isError()){
			std::cout << "Logger name: " << m_name << " setFormatter invalid patter : " 
						<< val << std::endl;
			return;
		}
		setFormatter(logformatter);
	}

	void Logger::setFormatter(const LogFormatter::ptr val){  
		MutexType::Lock lock(m_mutex);
		m_formatter = val;
		for(auto& app : m_appenders){
			MutexType::Lock slock(app->m_mutex);
			if(!app->m_hasFormatter){
				app->m_formatter = m_formatter;
			}
		}
	}

	LogFormatter::ptr Logger::getFormatter()  {
		MutexType::Lock lock(m_mutex);
		return m_formatter;
	}


	void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
		if (level >= m_level) {
			auto self = shared_from_this();
			MutexType::Lock lock(m_mutex);
			if(!m_appenders.empty()){
				for (auto& app : m_appenders) {
					//std::cout << m_name << " log, level = " << level << " appender_level = " << app->getLevel() <<std::endl;
					app->log(self,level, event);
				}
			}else if(m_root){
				m_root->log(level, event);
			}
		}
	}

	void Logger::debug(LogEvent::ptr event) {
		log(LogLevel::DEBUG, event);
	}
	void Logger::info(LogEvent::ptr event) {
		log(LogLevel::INFO, event);
	}
	void Logger::warn(LogEvent::ptr event) {
		log(LogLevel::WARN, event);
	}
	void Logger::error(LogEvent::ptr event) {
		log(LogLevel::ERROR, event);
	}
	void Logger::fatal(LogEvent::ptr event) {
		log(LogLevel::FATAL, event);
	}

	LogManager::LogManager(){
		m_root.reset(new Logger);
		m_root->addAppender(LogAppender::ptr(new StdoutAppender));
		m_loggers[m_root->getName()] = m_root; 
		init();
	}

	std::string LogManager::toYamlString(){
		MutexType::Lock lock(m_mutex);
		YAML::Node node;
		std::stringstream ss;
		//MutexType::Lock lock(m_mutex);
		for(const auto& log : m_loggers){
			node.push_back(YAML::Load(log.second->toYamlString()));
		}
		ss << node;
		return ss.str();
	}

	Logger::ptr LogManager::getLogger(const std::string& name) {
		MutexType::Lock lock(m_mutex);
		auto it = m_loggers.find(name);
		if(it != m_loggers.end())
			return it->second;
		Logger::ptr logger(new Logger(name));
		logger->m_root = m_root;
		m_loggers[name] =  logger;
		return logger;
	}

	void LogManager::init(){

	}


	void FileAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) {
		if (level >= m_level) {
			uint64_t now = time(0);
			if(now != m_lastTime){
				reopen();                   //每秒钟重新打开一次文件
				m_lastTime = now;
			}
			MutexType::Lock lock(m_mutex);
			//std::cout << "FileAppender log" << std::endl;
			m_filestream << m_formatter->format(logger,level, event);
		}
	}

	bool FileAppender::reopen() {
		MutexType::Lock lock(m_mutex);
		if (m_filestream) {
			m_filestream.close();
		}
		m_filestream.open(m_filename,std::ofstream::app);
		return m_filestream.goodbit;
	}

	std::string FileAppender::toYamlString() {
		MutexType::Lock lock(m_mutex);
		YAML::Node node;
		std::stringstream ss;
		node["type"] = "FileAppender";
		if(m_level != LogLevel::UNKNOW)
			node["level"] = sylar::LogLevel::ToString(m_level);
		node["file"] = m_filename;
		if(m_hasFormatter && m_formatter)
			node["formatter"] = m_formatter->getPattern();
		ss << node;
		return ss.str();
	}

	void StdoutAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) {
		if (level >= m_level) {
			MutexType::Lock lock(m_mutex);
			//std::cout << "StdoutAppender log" << std::endl;
			std::cout << m_formatter->format(logger,level, event);
		}
	}

	std::string StdoutAppender::toYamlString() {
		MutexType::Lock lock(m_mutex);
		YAML::Node node;
		std::stringstream ss;
		node["type"] = "StdoutAppender";
		if(m_level != LogLevel::UNKNOW)
			node["level"] = sylar::LogLevel::ToString(m_level);
		if(m_hasFormatter && m_formatter)
			node["formatter"] = m_formatter->getPattern();
		ss << node;
		return ss.str();
	}

	sylar::ConfigVar<std::set<LogDefine> >::ptr g_log_defines =
    	sylar::Config::Lookup("logs", std::set<LogDefine>(), "logs config");


	struct LogIniter{
        LogIniter() {
            g_log_defines->addListener([](const std::set<LogDefine>& old_value, const std::set<LogDefine>& new_value)
            {
                SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "on_logger_conf_change";
                for(auto& i : new_value){
                    auto it = old_value.find(i);
		            sylar::Logger::ptr logger;
                    if(it == new_value.end()){//新增
                        logger = SYLAR_LOG_NAME(i.name);
                    }else{//修改
                        if(!(*it == i)){
                            logger = SYLAR_LOG_NAME(i.name);
                        }else{
                            continue;
                        }
                    }
                    logger->setLevel(i.level);
                    if(!i.formatter.empty()){
                        logger->setFormatter(i.formatter);
                    }
                    logger->clearAppender();
                    for(auto& a : i.appenders){
                        sylar::LogAppender::ptr app;
                        if(a.type == 1){
                            app.reset(new sylar::FileAppender(a.file));
                        }else if(a.type == 2){
                            app.reset(new sylar::StdoutAppender());
                        }
                        app->setLevel(a.level);
                        if(!a.formatter.empty()){
                            LogFormatter::ptr fmt(new LogFormatter(a.formatter));
                            if(!fmt->isError()){
                                app->setFormatter(fmt);
                                // std::cout << "setFormatter: log name = " << i.name << ", appender type = " << a.type <<  ", formatter = " 
                                //           << a.formatter <<std::endl; 
                            }else{
                                std::cout << "log: " << i.name << " appender type: " 
                                          << a.type << " formatter : " << a.formatter << " is invalid" << std::endl;
                            }
                        }
                        // std::cout << "log name = " << i.name << ", appender type = " << a.type <<  ", hasformatter = " 
                        //           << app->hasFormatter <<std::endl; 
                        logger->addAppender(app);
                    }
                }
                //删除
                for(auto& i : old_value){
                    auto it = new_value.find(i);
                    if(it == new_value.end()){
                        auto logger = SYLAR_LOG_NAME(i.name);
                        logger->setLevel((sylar::LogLevel::Level)100);
                        logger->clearAppender();
                    }
                }
                    
            });
        }
    };

	static LogIniter __log_init;
}