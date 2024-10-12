#include<iostream>
#include "../my_sylar/log.h"
#include "../my_sylar/config.h"
#include<yaml-cpp/yaml.h>


void print_yaml(const YAML::Node& node,int level){
    if(node.IsScalar()){
        //SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "scalar\n" << node;
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << std::string(4 * level, ' ') << node.Scalar() << " - " << node.Type() << " - " << level;
    }else if(node.IsNull()){
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << std::string(4 * level, ' ') << "NULL - " << node.Type() << " - " << level;
    }else if(node.IsMap()){
        //SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "map\n" << node;
        for(auto it = node.begin(); it != node.end(); it++){
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << std::string(4 * level, ' ') << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    }else if(node.IsSequence()){
        //SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "seq\n" << node;
        for(size_t it = 0; it < node.size(); it++){
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << std::string(4 * level, ' ') << it << " - " << node[it].Type() << " - " << level;
            print_yaml(node[it], level + 1);
        }
    }
}

void test_yaml(){
    YAML::Node root = YAML::LoadFile("/home/jerry/mydir/project_server/bin/conf/log.yml"); 
    print_yaml(root,0);
}

void  test_config(){
    sylar::ConfigVar<int>::ptr g_int_value_config = sylar::Config::Lookup("system.port",(int)8080,"system port");
    sylar::ConfigVar<double>::ptr g_intd_value_config = sylar::Config::Lookup("system.port",(double)80.80f,"system port");
    sylar::ConfigVar<double>::ptr g_double_value_config = sylar::Config::Lookup("system.value",(double)80.80f,"system value"); 
    sylar::ConfigVar<std::vector<int>>::ptr g_vecint_value_config = sylar::Config::Lookup("system.vec",std::vector<int>{1,2},"system vec");
    sylar::ConfigVar<std::list<int>>::ptr g_listint_value_config = sylar::Config::Lookup("system.list",std::list<int>{3,4},"system list");  
    sylar::ConfigVar<std::set<int>>::ptr g_setint_value_config = sylar::Config::Lookup("system.set",std::set<int>{7,7,8,9},"system set");  
    sylar::ConfigVar<std::map<std::string, int>>::ptr g_strintmap_value_config = sylar::Config::Lookup("system.map",std::map<std::string, int>{{"k1",1},{"k2",2}},"system map"); 
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "before " <<g_int_value_config->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "before " <<g_double_value_config->toString();  
#define XX(g_var, name, prefix)\
    {auto v = g_var->getValue();{\
    for(auto& i : v)\
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT) <<  #prefix " " #name " " << i;}\
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT) <<  #prefix " " #name " yaml:  " << g_var->toString();}

#define XX_M(g_var, name, prefix)\
    {auto v = g_var->getValue();{\
    for(auto& i : v)\
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT) <<  #prefix " " #name " " << "{" << i.first << " : " << i.second << " } " ;}\
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT) <<  #prefix " " #name " yaml:  " << g_var->toString();}
    
    XX(g_vecint_value_config, int_vec, before);
    XX(g_listint_value_config, int_list, before);
    XX(g_setint_value_config, int_set, before);
    XX_M(g_strintmap_value_config, int_map, before);
    YAML::Node root = YAML::LoadFile("/home/jerry/mydir/project_server/bin/conf/log.yml"); 
    sylar::Config::LoadFromYaml(root);

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "after " << g_int_value_config->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "after " << g_double_value_config->toString();  
    
    XX(g_vecint_value_config, int_vec, after);
    XX(g_listint_value_config, int_list, after);
    XX(g_setint_value_config, int_list, after);
    XX_M(g_strintmap_value_config, int_map, after);
#undef XX
#undef XX_M
}

class Person{
public:
    std::string m_name = "";
    int m_age = 0;
    bool m_sex = 0;

    std::string toString() const {
        std::stringstream ss;
        ss << "[ person: " << " name= " << m_name 
                           << " age= " << m_age 
                           << " sex= " << m_sex << "]"; 
        return ss.str();                                
    }

    bool operator== (const Person& oth) const{
        return m_name == oth.m_name 
            && m_age == oth.m_age
            && m_sex == oth.m_sex;
    }
};


namespace sylar{
    template<>
    class LexicalCast<std::string, Person>{
    public:
        Person operator() (const std::string& v){
            YAML::Node node = YAML::Load(v);
            Person p;
            p.m_name = node["name"].as<std::string>();
            p.m_age = node["age"].as<int>();
            p.m_sex = node["sex"].as<bool>();
            return p;
        }
    };
    
    template<>
    class LexicalCast<Person, std::string>{
    public:
        std::string operator() (const Person& p){
            YAML::Node node;
            std::stringstream ss;
            node["name"] = p.m_name;
            node["age"] = p.m_age;
            node["sex"] = p.m_sex;
            ss << node;
            return ss.str();
        }
    };

}

void test_class(){
    sylar::ConfigVar<Person>::ptr g_person_config = sylar::Config::Lookup("class.person",Person(),"person");
    sylar::ConfigVar<std::vector<Person>>::ptr g_vperson_config = sylar::Config::Lookup("class.vec_person",std::vector<Person>(),"vec_person");
    g_person_config->addListener([](const Person& oldval, const Person& newval)->void{
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "old = " << oldval.toString() << 
        ", new = " << newval.toString();
    });

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << " before " << g_person_config->getValue().toString();
#define XX_C(g_var,  prefix)\
    {auto v = g_var->getValue();\
    for(auto& i : v)\
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT) <<  prefix  << i.toString();\
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT) <<  prefix  " yaml:  " << g_var->toString();\
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << " size = " << v.size();}
    
    XX_C(g_vperson_config," before ");

    YAML::Node root = YAML::LoadFile("/home/jerry/mydir/project_server/bin/conf/log.yml"); 
    sylar::Config::LoadFromYaml(root);

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << " after " << g_person_config->getValue().toString();
    XX_C(g_vperson_config," after ");
#undef XX_C
    
}

void test_log(){
    sylar::Logger::ptr sys_log = SYLAR_LOG_NAME("system");
    SYLAR_LOG_INFO(sys_log) << "hello system" ;
    std::cout << sylar::LoggerMgr::GetInstance()->toYamlString() << std::endl;
    YAML::Node root = YAML::LoadFile("/home/jerry/mydir/project_server/bin/conf/log.yml"); 
    sylar::Config::LoadFromYaml(root);
    std::cout << "-------------------" <<std::endl;
    std::cout << sylar::LoggerMgr::GetInstance()->toYamlString() <<std::endl;
    std::cout << "-------------------" <<std::endl;
    SYLAR_LOG_INFO(sys_log) << "hello system" ;
    
}


int main(){
    std::cout<<"test config"<<std::endl;     
    test_config();
    test_class();
    test_log();
    
    sylar::Config::Visit([](sylar::ConfigVarBase::ptr var){
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT) << "name: " << var->getName() 
                                << ", description: " << var->getDescription() 
                                << ", typename: " << var->getType() 
                                << ", value: " << var->toString() << std::endl;
    });
    return 0;
}