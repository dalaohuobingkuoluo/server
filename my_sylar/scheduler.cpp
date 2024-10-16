#include "scheduler.h"
#include "log.h"
#include "macro.h"

namespace sylar{
    static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
    static thread_local Scheduler* t_scheduler = nullptr;                 //当前调度器
    static thread_local Fiber* t_scheduler_fiber = nullptr;               //执行run调度算法的调度协程


    Scheduler::Scheduler(size_t threads, const std::string& name, bool use_caller)
        : m_name(name) {
        SYLAR_ASSERT2(threads > 0, "invalid thread number");

        if(use_caller){
            sylar::Fiber::GetThis();
            --threads;
            SYLAR_ASSERT2(GetThis() == nullptr, "scheduler exists");
            t_scheduler = this;

            m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this)));     //创建新的用于执行具体调度任务的协程设置为主协程
            t_scheduler_fiber = m_rootFiber.get();
            
            Thread::SetName(m_name);
            m_rootThreadId = sylar::GetThreadId();
            m_threadIds.push_back(m_rootThreadId);
        }else{
            m_rootThreadId = -1;
        }
        m_threadCount = threads;
        // SYLAR_LOG_DEBUG(g_logger) << "fiber domain this = " << Fiber::GetThis()
        //                           << " rootFiber = " << m_rootFiber;
    }

    Scheduler::~Scheduler(){
        SYLAR_ASSERT(m_stopping);
        if(t_scheduler == this){
            t_scheduler = nullptr;
        }
    }

    Scheduler* Scheduler::GetThis(){
        return t_scheduler;
    }

    Fiber* Scheduler::GetMainFiber(){
        return t_scheduler_fiber;
    }

    void Scheduler::start(){
        if(!m_stopping){
            return;
        }
        m_stopping = false;
        {
            MutexType::Lock lock(m_mutex);
            SYLAR_ASSERT(m_threads.empty());
            m_threads.resize(m_threadCount);
            for(size_t i = 0; i < m_threadCount; ++i){
                m_threads[i].reset(new Thread(std::bind(&Scheduler::run,this), m_name + "_" + std::to_string(i)));
                m_threadIds.push_back(m_threads[i]->getId());
            }
        }
        // if(m_rootFiber){
        //     m_rootFiber->swapIn();
        // }
    }

    void Scheduler::setThis(){
        t_scheduler = this;
    }

    void Scheduler::stop(){
        m_autoStop = true;
        //复用主线程并且没有创建新的线程
        if(m_rootFiber && m_threadCount == 0 
                       && (m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::INIT)){
            SYLAR_LOG_DEBUG(g_logger) << "scheduler : " << this << " stop";
            m_stopping = true;

            if(stopping()){
                return;
            }
        }

        if(m_rootThreadId != -1){  //复用主线程时需在
            SYLAR_ASSERT(GetThis() == this);

        }else{
            SYLAR_ASSERT(GetThis() != this);
        }
        m_stopping = true;
        for(size_t i = 0; i < m_threadCount; ++i){
            tickle();
        }

        if(m_rootFiber){
            tickle();
        }

        if(m_rootFiber){
            // while(!stopping()){
            //     if(m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::EXCEPT){
            //         m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this)));
            //         SYLAR_LOG_DEBUG(g_logger) << "rootFiber is term, reset";
            //     } 
            //     m_rootFiber->swapIn();
            // }
            if(!stopping()){
                m_rootFiber->swapIn();
            }
        }

        std::vector<Thread::ptr> thrs;
        {
            MutexType::Lock lock(m_mutex);
            thrs.swap(m_threads);
        }
        for(auto& thr : thrs){
            thr->join();
        }
        
    }

    void Scheduler::run(){
        SYLAR_LOG_DEBUG(g_logger) << "run";
        setThis();
        if(sylar::GetThreadId() != m_rootThreadId){
            t_scheduler_fiber = Fiber::GetThis().get();
        }
        Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle,this), true));
        Fiber::ptr cb_fiber;
        FiberAndThread ft;

        while(!stopping()){
            ft.reset();
            bool tickle_me = false;
            bool is_active = false;    //防止协程队列取出空的回调函数
            {
                MutexType::Lock lock(m_mutex);
                auto it = m_fibers.begin();
                while(it != m_fibers.end()){
                    SYLAR_LOG_DEBUG(g_logger) << "find fiber to do";
                    //协程指定线程号不等于当前线程，之后通知其他线程
                    if(it->thread != -1 && it->thread != sylar::GetThreadId()){
                        ++it;
                        tickle_me = true;
                        continue;
                    }
                    SYLAR_ASSERT(it->fiber || it->cb);
                    //协程正在执行
                    if(it->fiber && it->fiber->getState() == Fiber::EXEC){
                        ++it;
                        continue;
                    }
                    ft = *it;
                    m_fibers.erase(it);
                    ++m_activeThreadCount;
                    is_active = true;
                    break;
                }
            }

            if(tickle_me){
                tickle();
            }

            if(ft.fiber && (ft.fiber->getState() != Fiber::TERM && ft.fiber->getState() != Fiber::EXCEPT)){
                ft.fiber->swapInCaller();
                if(ft.fiber->getState() == Fiber::READY){
                    schedule(ft.fiber);
                }else if(ft.fiber->getState() != Fiber::TERM || ft.fiber->getState() != Fiber::EXCEPT){
                    --m_activeThreadCount;
                    ft.fiber->m_state = Fiber::HOLD;
                }
                ft.reset();
            }else if(ft.cb){
                if(cb_fiber){
                    cb_fiber->reset(ft.cb);
                }else{
                    cb_fiber.reset(new Fiber(ft.cb, true));
                }
                ft.reset();
                cb_fiber->swapInCaller();                
                if(cb_fiber->getState() == Fiber::READY){
                    schedule(cb_fiber);
                    cb_fiber.reset();
                }else if(cb_fiber->getState() == Fiber::TERM || cb_fiber->getState() == Fiber::EXCEPT){
                    --m_activeThreadCount;
                    cb_fiber->reset(nullptr);
                }else{
                    cb_fiber->m_state = Fiber::HOLD;
                    cb_fiber.reset();
                }
            }else{
                if(is_active){     //取出空的回调函数,不执行也不陷入idle_fiber
                    --m_activeThreadCount;
                    continue;
                }
                if(idle_fiber->getState() == Fiber::TERM){
                    --m_idleThreadCount;
                    SYLAR_LOG_DEBUG(g_logger) << "idle fiber term";
                    break;
                }
                ++m_idleThreadCount;
                SYLAR_LOG_DEBUG(g_logger) << "idle fiber : " << idle_fiber->shared_from_this().get();
                idle_fiber->swapInCaller();
                --m_idleThreadCount;
                SYLAR_LOG_DEBUG(g_logger) << "idle fiber return";
                if(idle_fiber->getState() != Fiber::TERM && idle_fiber->getState() != Fiber::EXCEPT){
                    SYLAR_LOG_DEBUG(g_logger) << "idle fiber hold";
                    idle_fiber->m_state = Fiber::HOLD;
                }
            }
            
        }
        
    }

    void Scheduler::tickle(){
        SYLAR_LOG_INFO(g_logger) << "tickle";
    }

    bool Scheduler::stopping(){
        // SYLAR_LOG_DEBUG(g_logger) << "Scheduler stopping()";
        MutexType::Lock lock(m_mutex);
        return m_autoStop && m_stopping 
            && m_fibers.empty() && m_activeThreadCount == 0;
    }

    void Scheduler::idle(){
        SYLAR_LOG_INFO(g_logger) << "idle";
        while(!stopping()){     //当还有调度任务调度器不结束，一直循环避免该线程退出但让出执行权去调度
            sylar::Fiber::YiledToHold();
        }
    }

}