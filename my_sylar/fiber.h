#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include<ucontext.h>
#include<memory>
#include<functional> 
#include "thread.h"

namespace sylar{

class Scheduler;

class Fiber : public std::enable_shared_from_this<Fiber> {
friend class Scheduler;
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };

    Fiber(std::function<void()> cb, bool use_caller = false, size_t stacksize = 0);   
    //use_caller=true调度器模式,仅任务协程需要设置为true
    ~Fiber();

    State getState() const {return m_state;}
    uint64_t getId() const {return m_id;}
    void reset(std::function<void()> cb);      //将INIT、TERM或EXCEPT态的协程重置并更改回调函数
    void swapIn();                             //从Main协程切换到当前协程执行   == sylar:call
    void swapOut();                            //切换到Main协程                == sylar:back
    void swapInCaller();                       //                             == sylar:swapIn
    void swapOutCaller();                      //                             == sylar:swapOut

    static void SetThis(Fiber* f);             //设置当前协程
    static Fiber::ptr GetThis();               //返回当前协程，若线程没有启动协程则创建主协程并返回
    static void YieldToReady();                //切换到后台并设置为READY或HOLD
    static void YiledToHold();
    static uint64_t TotalFibers();             //返回总协程数
    static void MainFunc();                    //执行回调函数
    static void MainFuncCaller(); 
    static uint64_t GetFiberId();

private:
    Fiber();                                   //用于创建主协程

    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = INIT;
    ucontext_t m_cxt;
    void* m_stack = nullptr;
    std::function<void()> m_cb;
    bool m_use_caller = false;

};


}

#endif