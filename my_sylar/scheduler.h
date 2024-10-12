#ifndef __SYLAR_SCHEDULER_H__
#define __SYLAR_SCHEDULER_H__

#include "thread.h"
#include "fiber.h"
#include<memory>
#include<vector>
#include<list>
#include<string>
#include<atomic>
#include<iostream>

namespace sylar{

class Scheduler{
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    Scheduler(size_t threads = 1, const std::string& name = "", bool use_caller = true);
    virtual ~Scheduler();               //将线程局部变量t_schedule置为空指针

    const std::string& getName() const {return m_name;}

    static Scheduler* GetThis();        //返回线程局部变量当前调度器t_schedule
    static Fiber* GetMainFiber();       //返回线程局部变量t_scheduler_fiber执行协程调度的主协程

    //start涉及三类协程t_threadFiber(创建调度器的主协程)、m_rootFiber(执行调度任务的协程)、FiberAndThread（具体的协程任务m_use_caller = true）
    //协程切换逻辑：1.(Fiber::m_use_caller = true)  :  t_threadFiber <--> m_rootFiber <--> FiberAndThread
    //             2.(Fiber::m_use_caller = false) : {t_threadFiber == m_rootFiber} <--> FiberAndThread
    void start();                       //创建线程池
    void stop();

    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1){
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }
        if(need_tickle){
            tickle();
        }
    }

    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end){
        bool need_tickle = false;
         {
            MutexType::Lock lock(m_mutex);
            while(begin != end){
                need_tickle |= scheduleNoLock(&*begin, -1);
                ++begin;
            }
            
        }
        if(need_tickle){
            tickle();
        }
    }
protected:
    void run();
    virtual void tickle();
    virtual bool stopping();    //判断基类协程调度任务是否结束（m_autoStop && m_stopping && m_fibers.empty() && m_activeThreadCount == 0;）
    virtual void idle();        //没有需要协程调度器继续调度的任务则执行idle方法
    void setThis();
    bool hasIdleThreads() {return m_idleThreadCount > 0;}
private:
    template<class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread){
        bool need_tickle = m_fibers.empty();
        FiberAndThread ft(fc, thread);
        if(ft.cb || ft.fiber){
            m_fibers.push_back(ft);
            //SYLAR_LOG_DEBUG(g_logger) << "add new fiber";
            std::cout << "add new fiber" << std::endl;
        }
        return need_tickle;
    }

    struct FiberAndThread {
        Fiber::ptr fiber;
        std::function<void()> cb;
        int thread;                //用于指定在特定的线程执行

        FiberAndThread(Fiber::ptr f, int thr) 
            : fiber(f), thread(thr){
        }

        FiberAndThread(Fiber::ptr* f, int thr) 
            : thread(thr){
            fiber.swap(*f);
        }

        FiberAndThread(std::function<void()> f, int thr) 
            : cb(f), thread(thr){
        }

        FiberAndThread(std::function<void()>* f, int thr) 
            : thread(thr){
            cb.swap(*f);
        }

        FiberAndThread() : thread(-1){
        }

        void reset(){
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };

private:
    std::vector<Thread::ptr> m_threads;
    MutexType m_mutex;
    std::list<FiberAndThread> m_fibers;      //待执行的协程列表
    Fiber::ptr m_rootFiber;                  //用于真正执行协程调度而创建的新协程
    std::string m_name;

protected:
    std::vector<int> m_threadIds;
    size_t m_threadCount = 0;
    std::atomic<size_t> m_activeThreadCount{0};         
    std::atomic<size_t> m_idleThreadCount = {0};        
    bool m_stopping = true;                  //是否正在运行
    bool m_autoStop = false;                 //是否被stop方法结束
    int m_rootThreadId = 0;                   
};

}

#endif