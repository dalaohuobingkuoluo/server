#include "fiber.h"
#include "config.h"
#include "macro.h"
#include "scheduler.h"
#include<atomic>

namespace sylar{
    static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
    static std::atomic<uint64_t> s_fiber_id{0};
    static std::atomic<uint64_t> s_fiber_count(0);

    static thread_local Fiber* t_fiber = nullptr;                   //当前协程
    static thread_local Fiber::ptr t_threadFiber = nullptr;         //Main主协程（线程的首个协程）

    static ConfigVar<uint32_t>::ptr g_fiber_stack_size =
            Config::Lookup("fiber.stack_size", (uint32_t)(1024 * 1024), "fiber stack size");

    class MallocStackAllocator{
    public:
        static void* Alloc(size_t size){
            return malloc(size);
        }

        static void Dealloc(void* vp, size_t size){
            free(vp);
        }
    };

    using StackAllocator = MallocStackAllocator;

    Fiber::Fiber(){
        m_state = EXEC;
        SetThis(this);
        if(getcontext(&m_cxt)){
            SYLAR_ASSERT2(false,"getcontext error");
        }
        ++s_fiber_count;
        SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber";
    }

    Fiber::Fiber(std::function<void()> cb, bool use_caller, size_t stacksize) 
        : m_id(++s_fiber_id),  m_cb(cb), m_use_caller(use_caller) {
        m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue(); 
        m_stack = StackAllocator::Alloc(m_stacksize);
        if(getcontext(&m_cxt)){
            SYLAR_ASSERT2(false,"init fiber getcontext error");
        }
        ++s_fiber_count;

        m_cxt.uc_link = nullptr;
        m_cxt.uc_stack.ss_sp = m_stack;
        m_cxt.uc_stack.ss_size = m_stacksize;

        if(!m_use_caller){
            makecontext(&m_cxt,MainFunc,0);
        }else{
            makecontext(&m_cxt,MainFuncCaller,0);
        }
        SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber id = " << m_id;
    }

    Fiber::~Fiber(){
        --s_fiber_count;
        if(m_stack){
            // SYLAR_LOG_DEBUG(g_logger) << m_id << m_state;
            SYLAR_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
            StackAllocator::Dealloc(m_stack, m_stacksize);
        }else {
            SYLAR_ASSERT(!m_cb);
            SYLAR_ASSERT(m_state == EXEC);    //只有主协程没有分配独立的栈空间，并且在生命周期始终为EXEC态
            Fiber* cur = t_fiber;
            if(cur == this)
                SetThis(nullptr);
        }
        SYLAR_LOG_DEBUG(g_logger) << "Fiber::~Fiber id = " << m_id;
    }

    void Fiber::reset(std::function<void()> cb){
        SYLAR_ASSERT(m_stack);
        SYLAR_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
        m_cb = cb;
        if(getcontext(&m_cxt)){
            SYLAR_ASSERT2(false,"reset getcontext error");
        }
        m_cxt.uc_link = nullptr;
        m_cxt.uc_stack.ss_sp = m_stack;
        m_cxt.uc_stack.ss_size = m_stacksize;
        if(!m_use_caller){
            makecontext(&m_cxt,MainFunc,0);
        }else{
            makecontext(&m_cxt,MainFuncCaller,0);
        }
        m_state = INIT;
    }    

    //频繁的协程切换会导致判断语句带来极大开销，拆分为两个函数提升性能
    void Fiber::swapIn(){
        SetThis(this);
        SYLAR_ASSERT(m_state != EXEC);
        m_state = EXEC;
        //SYLAR_LOG_DEBUG(g_logger) << this << " swapOut to t_threadFiber";
        if(swapcontext(&t_threadFiber->m_cxt, &m_cxt)){
            SYLAR_ASSERT2(false, "swapIn swapcontext error");
        }
        
    }   

    void Fiber::swapInCaller(){
        SetThis(this);
        SYLAR_ASSERT(m_state != EXEC);
        m_state = EXEC;
        //SYLAR_LOG_DEBUG(g_logger) << this << " swapOut to m_rootFiber";
        if(swapcontext(&Scheduler::GetMainFiber()->m_cxt, &m_cxt)){
            SYLAR_ASSERT2(false, "swapIn swapcontext error");
        }
        
    }   

    void Fiber::swapOut(){
        SetThis(t_threadFiber.get());
        if(swapcontext(&m_cxt, &t_threadFiber->m_cxt)){
            SYLAR_ASSERT2(false, "swapOut swapcontext error");
        }
    }    

    void Fiber::swapOutCaller(){
        SetThis(Scheduler::GetMainFiber());
        if(swapcontext(&m_cxt, &Scheduler::GetMainFiber()->m_cxt)){
            SYLAR_ASSERT2(false, "swapOut swapcontext error");
        }
    }      

    void Fiber::SetThis(Fiber* f){
        t_fiber = f;
    }                      

    Fiber::ptr Fiber::GetThis(){
        if(t_fiber){
            return t_fiber->shared_from_this();
        } 
        Fiber::ptr main_fiber(new Fiber);
        SYLAR_ASSERT(t_fiber == main_fiber.get());
        t_threadFiber = main_fiber;
        return t_fiber->shared_from_this();
    }               

    void Fiber::YieldToReady(){
        Fiber::ptr cur = GetThis();
        cur->m_state = READY;
        if(GetThis()->m_use_caller){
            cur->swapOutCaller();
        }else{
            cur->swapOut();
        }
    }               

    void Fiber::YiledToHold(){
        Fiber::ptr cur = GetThis();
        // cur->m_state = HOLD;
        if(GetThis()->m_use_caller){
            cur->swapOutCaller();
        }else{
            cur->swapOut();
        }
    }

    uint64_t Fiber::TotalFibers(){
        return s_fiber_count;
    }          

    void Fiber::MainFunc(){
        Fiber::ptr cur = GetThis();
        SYLAR_ASSERT(cur);
        try{
            cur->m_cb();
            cur->m_cb = nullptr;
            //释放回调函数持有的资源，避免内存泄漏
            //防止协程在被错误地重新调度时重复执行相同的回调函数,明确表示协程已经执行完毕，不再持有任何有效的回调函数
            cur->m_state = TERM;
        }catch(std::exception& ex){
            cur->m_state = EXCEPT;
            SYLAR_LOG_ERROR(g_logger) << "Fiber Except : " << ex.what();
        }catch(...){
            cur->m_state = EXCEPT;
            SYLAR_LOG_ERROR(g_logger) << "Fiber Except" ;
        }
        //Fiber::ptr cur = GetThis();使得智能指针加1，此时即使回到主协程cur仍在子协程栈上，
        //因此即使出作用域子协程其智能指针计数仍存在导致无法释放
        auto raw_ptr = cur.get();    
        cur.reset();
        raw_ptr->swapOut();
        
        SYLAR_ASSERT2(false, "never reach");
    }

    void Fiber::MainFuncCaller(){
        Fiber::ptr cur = GetThis();
        SYLAR_ASSERT(cur);
        // try{
            cur->m_cb();
            cur->m_cb = nullptr;
            cur->m_state = TERM;
        // }catch(std::exception& ex){
        //     cur->m_state = EXCEPT;
        //     SYLAR_LOG_ERROR(g_logger) << "Fiber Except : " << ex.what() 
        //                               << ", fiber id = " << cur->getId()
        //                               << std::endl << sylar::BacktraceToString();
        // }catch(...){
        //     cur->m_state = EXCEPT;
        //     SYLAR_LOG_ERROR(g_logger) << "Fiber Except" ;
        // }
        //Fiber::ptr cur = GetThis();使得智能指针加1，此时即使回到主协程cur仍在子协程栈上，
        //因此即使出作用域子协程其智能指针计数仍存在导致无法释放
        auto raw_ptr = cur.get();    
        cur.reset();
        raw_ptr->swapOutCaller();
        
        SYLAR_ASSERT2(false, "never reach");
    }

    uint64_t Fiber::GetFiberId(){
        if(t_fiber){
            return t_fiber->getId();
        }
        return 0;
    }
        
} 
