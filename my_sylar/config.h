#ifndef __SYLAR__CONFIG__H__
#define __SYLAR__CONFIG__H__

#include <memory>
#include<sstream>
#include<boost/lexical_cast.hpp>    //数值与字符串转换
#include<string>
#include<vector>
#include<list>
#include<map>
#include<algorithm>
#include<yaml-cpp/yaml.h>
#include<cctype>
#include<functional>
#include "log.h"
#include "thread.h"
#include "macro.h"

namespace sylar{

    class ConfigVarBase{
    public:
        typedef std::shared_ptr<ConfigVarBase> ptr;
        //using ptr = std::shared_ptr<ConfigVarBase>;
        ConfigVarBase(const std::string& name,const std::string& description = "")
            : m_name(name),m_description(description){
                std::transform(m_name.begin(),m_name.end(),m_name.begin(),::tolower);
            }
        virtual ~ConfigVarBase(){}

        const std::string getName() const {return m_name;}
        const std::string getDescription() const {return m_description;}

        virtual std::string toString() = 0;
        virtual bool fromString(const std::string& str) = 0;
        virtual std::string getType() const = 0;
    protected:
        std::string m_name;
        std::string m_description;
    };

    template<class F, class T>
    class LexicalCast {
    public:
        T operator()(const F& v){
            return boost::lexical_cast<T>(v);
        }
    };

    template<class T>
    class LexicalCast<std::string, class std::vector<T>>{
    public:
        std::vector<T> operator() (const std::string& v){
            YAML::Node node = YAML::Load(v);
            typename std::vector<T> vec;
            std::stringstream ss;
            for(size_t i = 0; i < node.size(); i++){
                ss.str("");
                ss << node[i];
                vec.push_back(LexicalCast<std::string,T>()(ss.str()));
            }
            return vec;
        }
    };
        
    template<class T>
    class LexicalCast<class std::vector<T>, std::string>{
    public:
        std::string operator() (const std::vector<T>& v){
            YAML::Node node;
            std::stringstream ss;
            for(auto& i : v){
                node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
            }
            ss << node;
            return ss.str();
        }
    };

    template<class T>
    class LexicalCast<std::string, class std::list<T>>{
    public:
        std::list<T> operator() (const std::string& v){
            YAML::Node node = YAML::Load(v);
            typename std::list<T> ls;
            std::stringstream ss;
            for(size_t i = 0; i < node.size(); i++){
                ss.str("");
                ss << node[i];
                ls.push_back(LexicalCast<std::string,T>()(ss.str()));
            }
            return ls;
        }
    };
        
    template<class T>
    class LexicalCast<class std::list<T>, std::string>{
    public:
        std::string operator() (const std::list<T>& v){
            YAML::Node node;
            std::stringstream ss;
            for(auto& i : v){
                node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
            }
            ss << node;
            return ss.str();
        }
    };

    template<class T>
    class LexicalCast<std::string, class std::set<T>>{
    public:
        std::set<T> operator() (const std::string& v){
            YAML::Node node = YAML::Load(v);
            typename std::set<T> m_set;
            std::stringstream ss;
            for(size_t i = 0; i < node.size(); i++){
                ss.str("");
                ss << node[i];
                m_set.insert(LexicalCast<std::string,T>()(ss.str()));
            }
            return m_set;
        }
    };
        
    template<class T>
    class LexicalCast<class std::set<T>, std::string>{
    public:
        std::string operator() (const std::set<T>& v){
            YAML::Node node;
            std::stringstream ss;
            for(auto& i : v){
                node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
            }
            ss << node;
            return ss.str();
        }
    };

    template<class T>
    class LexicalCast<std::string, class std::map<std::string, T>>{
    public:
        std::map<std::string, T> operator() (const std::string& v){
            YAML::Node node = YAML::Load(v);
            typename std::map<std::string, T> m_map;
            std::stringstream ss;
            for(auto it = node.begin(); it != node.end(); ++it){
                ss.str("");
                ss << it->second;
                m_map.insert(std::make_pair(it->first.Scalar(),
                        LexicalCast<std::string,T>()(ss.str())));
            }
            return m_map;
        }
    };
        
    template<class T>
    class LexicalCast<class std::map<std::string, T>, std::string>{
    public:
        std::string operator() (const std::map<std::string, T>& v){
            YAML::Node node;
            std::stringstream ss;
            for(auto& i : v){
                node[i.first] = YAML::Load(LexicalCast<T,std::string>()(i.second));
            }
            ss << node;
            return ss.str();
        }
    };

    template<class T, class FromStr = LexicalCast<std::string,T>, class ToStr = LexicalCast<T,std::string>>
    class ConfigVar : public ConfigVarBase {
    public:
        typedef RWMutex RWMutexType;
        typedef std::shared_ptr<ConfigVar> ptr;
        typedef std::function<void(const T& oldval, const T& newval)> on_change_cb;

        ConfigVar(const std::string& name,const T& val,const std::string& description = "")
            : ConfigVarBase(name,description), m_val(val){

        }

        std::string toString() override {
            try{
                //return boost::lexical_cast<std::string>(m_val);
                RWMutexType::ReadLock lock(m_mutex);
                return ToStr()(m_val);
            }catch(std::exception& e){
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT) << "ConfigVar::toString expection " <<
                e.what() << " convert " << typeid(m_val).name() << " to string";
            }
            return "";
        }

        bool fromString(const std::string& str) override {
            try{
                //m_val =  boost::lexical_cast<T>(str);
                setValue(FromStr()(str));
                return true;
            }catch(std::exception& e){
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT) << "ConfigVar::fromString expection" <<
                e.what() << " convert from string to " << typeid(T).name();
            }
            return false;
        }

        // void addListener(uint64_t key, on_change_cb cb){
        //     m_cbs[key] = cb;
        // }

        uint64_t addListener(on_change_cb cb){
            static uint64_t s_func_id = 0;
            RWMutexType::WriteLock lock(m_mutex);
            ++s_func_id;
            m_cbs[s_func_id] = cb;
            return s_func_id;
        }

        void delListener(uint64_t key){
            RWMutexType::WriteLock lock(m_mutex);
            m_cbs.erase(key);
        }

        on_change_cb getListerner(uint64_t key){
            RWMutexType::ReadLock lock(m_mutex);
            return m_cbs[key];
        }

        void clearListener(){
            RWMutexType::WriteLock lock(m_mutex);
            m_cbs.clear();
        }

        const T getValue() {
            RWMutex::ReadLock lock(m_mutex);
            return m_val;
        }
        //通过setValue更新值时才触发监听回调
        void setValue(const T& val) {
            {
                RWMutex::ReadLock lock(m_mutex);
                if(m_val == val)
                    return;
                for(auto& i : m_cbs)
                    i.second(m_val, val);
            }
            RWMutexType::WriteLock lock(m_mutex);
            m_val = val;
        }

        std::string getType() const override {return typeid(T).name();}
    private:
        T m_val;
        std::map<uint64_t,on_change_cb> m_cbs;
        RWMutexType m_mutex;
    };

    class Config{
        public:
            typedef RWMutex RWMutexType;
            typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;

            template<class T>
            static typename ConfigVar<T>::ptr Lookup(const std::string& name,const T& val, const std::string& description=""){
                //auto it = Lookup<T>(name);                    //why
                //if(it){
                  //  SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "Lookup name= " << name  <<" exists ";
                  //  return it;
                //}
                {
                    RWMutexType::ReadLock lock(getMutex());
                    auto it = getDatas().find(name);
                    if(it != getDatas().end()){
                        auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
                        if(tmp){
                            SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "Lookup name = " << name  <<" exists ";
                            return tmp;
                        }else{
                            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT) << "Lookup name= " << name  <<" exists but type errer: " 
                                                            << typeid(T).name()  << " != (real type) "<< it->second->getType()
                                                            << " " <<it->second->toString();
                            return nullptr;
                        }
                    }
                }
                if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz1234567890_.") != std::string::npos){
                    SYLAR_LOG_ERROR(SYLAR_LOG_ROOT) << "name invalid " << name;
                    throw std::invalid_argument(name);
                }
                typename ConfigVar<T>::ptr v(new ConfigVar<T>(name,val,description));
                RWMutexType::WriteLock lock(getMutex());
                getDatas()[name] = v;
                return v;
            }

            template<class T>
            static typename ConfigVar<T>::ptr Lookup(const std::string& name){
                RWMutexType::ReadLock lock(getMutex());
                auto it = getDatas().find(name);
                if(it == getDatas().end())
                    return nullptr;
    
                return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);  
            }

            static void LoadFromYaml(const YAML::Node& root);
            static ConfigVarBase::ptr LookupBase(const std::string& name);
            static void Visit(std::function<void(ConfigVarBase::ptr)> cb);
        private:
            //避免静态成员初始化顺序导致访问到未初始化的s_datas
            //采用静态局部变量在首次调用时才初始化，避免初始化顺序不确定的问题
            static ConfigVarMap& getDatas(){
                static ConfigVarMap s_datas;
                return s_datas;
            } 

            static RWMutexType& getMutex(){
                static RWMutexType s_mutex;
                return s_mutex;
            } 
    };


    struct LogAppenderDefine {
        int type;    //1 file 2 stdout
	    sylar::LogLevel::Level level = sylar::LogLevel::UNKNOW;
        std::string formatter;
        std::string file;

        bool operator==(const LogAppenderDefine& oth) const {
            return type == oth.type
                && level == oth.level
                && formatter == oth.formatter
                && file == oth.file; 
        }
    };

    struct LogDefine {
        std::string name;
	    sylar::LogLevel::Level level;
        std::string formatter;
        std::vector<LogAppenderDefine> appenders;

        bool operator==(const LogDefine& oth) const {
            return name == oth.name
                && level == oth.level
                && formatter == oth.formatter
                && appenders == oth.appenders;
        }

         bool operator<(const LogDefine& oth) const {
            return name < oth.name;
        }
    };

    //自定义类型的偏特化
    template<>
    class LexicalCast<std::string, class std::set<LogDefine>>{
    public:
        std::set<LogDefine> operator() (const std::string& v){
            YAML::Node node = YAML::Load(v);
            std::set<LogDefine> m_set_log;
            for(const auto& n : node){
                if(!n["name"].IsDefined()){
                    std::cout << "log config define error: name undefined " << n  << std::endl; 
                    continue;
                }
                LogDefine ld;
                ld.name = n["name"].as<std::string>();
                ld.level = sylar::LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>():"");
                if(n["formatter"].IsDefined()){
                    ld.formatter =  n["formatter"].as<std::string>();
                }
                if(n["appenders"].IsDefined()){
                    for(size_t i = 0; i < n["appenders"].size(); ++i){
                        const auto& a = n["appenders"][i];
                        if(!a["type"].IsDefined()){
                            std::cout << "log appender define error: type undefined " << a  << std::endl; 
                            continue;
                        }
                        std::string type = a["type"].as<std::string>();
                        LogAppenderDefine lad;
                        if(type == "FileAppender"){
                            lad.type = 1;
                            if(!a["file"].IsDefined()){
                                std::cout << "log file appender require file path: " << a  << std::endl; 
                                continue;
                            }
                            lad.file = a["file"].as<std::string>();
                            if(a["formatter"].IsDefined())
                                lad.formatter =  a["formatter"].as<std::string>();
                        }else if(type == "StdoutAppender"){
                            lad.type = 2;
                            if(a["formatter"].IsDefined())
                                lad.formatter =  a["formatter"].as<std::string>();
                        }else{
                            std::cout << "log appender type error: unknown type " << a  << std::endl; 
                            continue;
                        }
                        ld.appenders.push_back(lad);
                    }
                }
                m_set_log.insert(ld);
            }         
            return m_set_log;
        }
    };

    template<>
    class LexicalCast<class std::set<LogDefine>, std::string>{
    public:
        std::string operator() (const std::set<LogDefine>& v){
            YAML::Node node;
            std::stringstream ss;
            for(auto& i : v){
                YAML::Node n;
                n["name"] = i.name;
                n["level"] = sylar::LogLevel::ToString(i.level);
                if(!i.formatter.empty())
                    n["formatter"] = i.formatter;
                for(auto& a : i.appenders){
                    YAML::Node na;
                    if(a.type == 1){
                        na["type"] =  "FileAppender";
                        na["file"] = a.file;
                    }else if(a.type == 2){
                        na["type"] =  "StdoutAppender";
                    }
                    if(a.level != LogLevel::UNKNOW)
                        na["level"] = sylar::LogLevel::ToString(a.level);
                    if(!a.formatter.empty())
                        na["formatter"] = a.formatter;
                    n["appenders"].push_back(na);
                }
                node.push_back(n);
            }
            ss << node;
            return ss.str();
        }
    };

    // static sylar::ConfigVar<std::set<LogDefine>>::ptr g_log_defines = 
    //         Config::Lookup("logs",std::set<LogDefine>(),"logs config");
    
    //g_log_defines和__log_init静态变量的初始化顺序在同一编译单元内是确定的,跨编译单元的静态变量初始化顺序是不确定的,
    //头文件被包含在多个源文件中，则可能导致初始化顺序不确定。即使它们定义在同一个头文件中，只要它们被链接到不同的编译单元，
    //初始化顺序也是不确定的。
    //解决：1. 使用静态局部变量    2. 将静态变量在头文件中声明，而在 .cpp 文件中定义

    // static sylar::ConfigVar<std::set<LogDefine>>::ptr& getLogDefines() {
    //     static sylar::ConfigVar<std::set<LogDefine>>::ptr g_log_defines = 
    //         Config::Lookup("logs", std::set<LogDefine>(), "logs config");
    //     return g_log_defines;
    // }

    // struct LogIniter{
    //     LogIniter() {
    //         auto& g_log_defines = getLogDefines();
    //         if(g_log_defines == nullptr)
    //             SYLAR_ASSERT2(false, "log_defines not exist");
    //         g_log_defines->addListener([](const std::set<LogDefine>& old_value, const std::set<LogDefine>& new_value)
    //         {
    //             SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "on_logger_conf_change";
    //             for(auto& i : new_value){
    //                 auto it = old_value.find(i);
	// 	            sylar::Logger::ptr logger;
    //                 if(it == new_value.end()){//新增
    //                     logger = SYLAR_LOG_NAME(i.name);
    //                 }else{//修改
    //                     if(!(*it == i)){
    //                         logger = SYLAR_LOG_NAME(i.name);
    //                     }else{
    //                         continue;
    //                     }
    //                 }
    //                 logger->setLevel(i.level);
    //                 if(!i.formatter.empty()){
    //                     logger->setFormatter(i.formatter);
    //                 }
    //                 logger->clearAppender();
    //                 for(auto& a : i.appenders){
    //                     sylar::LogAppender::ptr app;
    //                     if(a.type == 1){
    //                         app.reset(new sylar::FileAppender(a.file));
    //                     }else if(a.type == 2){
    //                         app.reset(new sylar::StdoutAppender());
    //                     }
    //                     app->setLevel(a.level);
    //                     if(!a.formatter.empty()){
    //                         LogFormatter::ptr fmt(new LogFormatter(a.formatter));
    //                         if(!fmt->isError()){
    //                             app->setFormatter(fmt);
    //                             // std::cout << "setFormatter: log name = " << i.name << ", appender type = " << a.type <<  ", formatter = " 
    //                             //           << a.formatter <<std::endl; 
    //                         }else{
    //                             std::cout << "log: " << i.name << " appender type: " 
    //                                       << a.type << " formatter : " << a.formatter << " is invalid" << std::endl;
    //                         }
    //                     }
    //                     // std::cout << "log name = " << i.name << ", appender type = " << a.type <<  ", hasformatter = " 
    //                     //           << app->hasFormatter <<std::endl; 
    //                     logger->addAppender(app);
    //                 }
    //             }
    //             //删除
    //             for(auto& i : old_value){
    //                 auto it = new_value.find(i);
    //                 if(it == new_value.end()){
    //                     auto logger = SYLAR_LOG_NAME(i.name);
    //                     logger->setLevel((sylar::LogLevel::Level)100);
    //                     logger->clearAppender();
    //                 }
    //             }
                    
    //         });
    //     }
    // };

    //初始化加事件，全局变量在main前构造
    // static LogIniter __log_init;

    //Config::ConfigVarMap Config::s_datas;   静态成员变量还要另外定义一次，常量只能在头文件初始化，变量在cpp文件定义
};

#endif
