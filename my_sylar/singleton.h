#ifndef __SYLAR__SINGLETON__H__
#define __SYALR__SINGLETON__H__

namespace sylar{

template<class T,class X = void, int N=0>
class Singleton{
public:
    static T* GetInstance(){
        static T v;
        return &v;
    }

};

template<class T,class X = void, int N=0>
class Singletonptr{
public:
    static std::shared_ptr<T> GetInstance(){
        static std::shared_ptr<T> v(new T);
        return v;
    }

};



};


#endif