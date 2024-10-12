#include "./config.h"

namespace sylar{

ConfigVarBase::ptr Config::LookupBase(const std::string& name){
    RWMutexType::ReadLock lock(getMutex());
    auto it = getDatas().find(name);
    return it == getDatas().end() ? nullptr : it->second;
}

static void ListAllMenber(const std::string& prefix, const YAML::Node& node,
                          std::list<std::pair<std::string,const YAML::Node>>& output){
    if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz1234567890_.") != std::string::npos){
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT) << "Config invalid name: " << prefix << " : " << node;
        return;
    }
    output.push_back(std::make_pair(prefix, node));
    //std::cout << prefix << " ***** " <<  node <<"\n"<<std::endl;
    if(node.IsMap()){
        for(auto it = node.begin(); it != node.end(); it++)
        ListAllMenber(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
    }
}

void Config::Visit(std::function<void(ConfigVarBase::ptr)> cb){
    RWMutexType::ReadLock lock(getMutex());
    auto& m = getDatas();
    for(auto it = m.begin(); it != m.end(); ++it){
        cb(it->second); 
    }
}

void Config::LoadFromYaml(const YAML::Node& root){
    std::list<std::pair<std::string,const YAML::Node>> all_nodes;
    ListAllMenber("", root, all_nodes);
    
    for(auto& i : all_nodes){
        //std::cout << "(" << i.first << " --- " <<  i.second <<")\n"<<std::endl;
        std::string key = i.first;
        if(key.empty())
            continue;
        std::transform(key.begin(),key.end(),key.begin(),::tolower);
        ConfigVarBase::ptr var = LookupBase(key);
        if(var){
            if(i.second.IsScalar()){
                var->fromString(i.second.Scalar());
            }else{
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
    // std::cout << "++++++++++++++++++" <<std::endl;
    // auto d = getDatas();
    // for(auto& i : d)
    //     std::cout << "(" << i.first << " --- " <<  i.second->toString() << ")"<<std::endl;
    // std::cout << "++++++++++++++++++" <<std::endl;
}


};