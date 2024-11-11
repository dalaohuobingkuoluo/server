#include "iomanager.h"
#include "macro.h"
#include "log.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>


namespace sylar{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

IOManager::FdContext::EventContext& IOManager::FdContext::getContext(Event event){
    switch(event){
        case READ:
            return read;
        case WRITE:
            return write;
        default:
            SYLAR_ASSERT2(false, "getContext");
    }
}

void IOManager::FdContext::resetContext(EventContext& ctx){
    ctx.cb = nullptr;
    ctx.scheduler = nullptr;
    ctx.fiber.reset();
}

void IOManager::FdContext::triggerEvent(Event e){
    SYLAR_ASSERT(event & e);
    EventContext& ctx = getContext(e);
    event = (Event)(event & ~e);

    if(ctx.cb){
        ctx.scheduler->schedule(&ctx.cb);
    }else{
        ctx.scheduler->schedule(&ctx.fiber);
    }   
    ctx.scheduler = nullptr;
}

IOManager::IOManager(size_t threads, const std::string& name, bool use_caller)
    : Scheduler(threads, name, use_caller) {
    m_epfd = epoll_create(5000);
    SYLAR_ASSERT(m_epfd > 0);

    int rt = pipe(m_ticklefd);  //[0]read [1]write
    SYLAR_ASSERT(!rt);

    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;       
    //LT 模式下，只要文件描述符处于可读或可写状态，每次调用 epoll_wait() 都会通知；ET 模式则只在状态发生变化时通知一次。
    event.data.fd = m_ticklefd[0];
    rt = fcntl(m_ticklefd[0], F_SETFL, O_NONBLOCK);
    SYLAR_ASSERT(!rt);

    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_ticklefd[0], &event);
    SYLAR_ASSERT(!rt);

    contextResize(32);

    start();
}

IOManager::~IOManager(){
    stop();
    close(m_epfd);
    close(m_ticklefd[0]);
    close(m_ticklefd[1]);
    
    for(size_t i = 0; i < m_fdContexts.size(); ++i){
        if(m_fdContexts[i]){
            delete m_fdContexts[i];
        }
    }
}

int IOManager::addEvent(int fd, Event event, std::function<void()> cb){
    FdContext* fd_ctx = nullptr;
    RWMutexType::ReadLock rdlock(m_mutex);
    if(m_fdContexts.size() > fd){
        fd_ctx = m_fdContexts[fd];
        rdlock.unlock();
    }else{
        rdlock.unlock();
        RWMutexType::WriteLock wrlock(m_mutex);
        contextResize(fd * 1.5);
        fd_ctx = m_fdContexts[fd];
    }
    {
        FdContext::MutexType::Lock lock(fd_ctx->mutex);
        if(fd_ctx->event & event){               //不同线程/协程操作同一事件类型
            SYLAR_LOG_ERROR(g_logger) << "addEvent assert fd = " << fd      
                                    << ", event = " << event
                                    << ", fd_ctx->event = " << fd_ctx->event;
            SYLAR_ASSERT(!(fd_ctx->event & event));
        }

        int op = fd_ctx->event ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
        epoll_event epev;
        memset(&epev, 0, sizeof(epoll_event));
        epev.events = EPOLLET | fd_ctx->event | event;
        epev.data.ptr = fd_ctx;

        int rt = epoll_ctl(m_epfd, op, fd, &epev);
        if(rt){
            SYLAR_LOG_ERROR(g_logger) << "epoll_ctl fail(" << m_epfd
                                    << "," << op << "," << fd << "," << epev.events << ") fail,"
                                    << rt << "(" << errno << "," << strerror(errno) << ")";
            return -1;
        }

        ++m_pendingEventCount;
        fd_ctx->event = (Event)(fd_ctx->event | event);
        FdContext::EventContext& event_ctx = fd_ctx->getContext(event);

        SYLAR_ASSERT(!event_ctx.cb && !event_ctx.fiber && !event_ctx.scheduler); 

        event_ctx.scheduler = Scheduler::GetThis();
        if(cb){
            event_ctx.cb.swap(cb);
        }else{
            event_ctx.fiber = Fiber::GetThis();
            SYLAR_ASSERT(event_ctx.fiber->getState() == Fiber::EXEC);
        }
    }
    return 0;
}

bool IOManager::delEvent(int fd, Event event){
    RWMutexType::ReadLock rdlock(m_mutex);
    if(m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    rdlock.unlock();

    FdContext::MutexType::Lock lock(fd_ctx->mutex);
    if(!(fd_ctx->event & event)){
        return false;
    }

    Event new_event = (Event)(fd_ctx->event & ~event);
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epev;
    memset(&epev, 0, sizeof(epoll_event));
    epev.events = EPOLLET | new_event;
    epev.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epev);
    if(rt){
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl fail(" << m_epfd
                                  << "," << op << "," << fd << "," << epev.events << ") fail,"
                                  << rt << "(" << errno << "," << strerror(errno) << ")";
        return false;
    }

    --m_pendingEventCount;
    fd_ctx->event = new_event;
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);
    return true;
}

bool IOManager::cancelEvent(int fd,Event event){
    RWMutexType::ReadLock rdlock(m_mutex);
    if(m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    rdlock.unlock();

    {
        FdContext::MutexType::Lock lock(fd_ctx->mutex);
        if(!(fd_ctx->event & event)){
            return false;
        }

        Event new_event = (Event)(fd_ctx->event & ~event);
        int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
        epoll_event epev;
        memset(&epev, 0, sizeof(epoll_event));
        epev.events = EPOLLET | new_event;
        epev.data.ptr = fd_ctx;

        int rt = epoll_ctl(m_epfd, op, fd, &epev);
        if(rt){
            SYLAR_LOG_ERROR(g_logger) << "epoll_ctl fail(" << m_epfd
                                    << "," << op << "," << fd << "," << epev.events << ") fail,"
                                    << rt << "(" << errno << "," << strerror(errno) << ")";
            return false;
        }

        fd_ctx->triggerEvent(event);
        --m_pendingEventCount;
    }
    return true;
}

bool IOManager::cancelAll(int fd){
    RWMutexType::ReadLock rdlock(m_mutex);
    if(m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    rdlock.unlock();

    FdContext::MutexType::Lock lock(fd_ctx->mutex);
    if(!fd_ctx->event){
        return false;
    }

    int op = EPOLL_CTL_DEL;
    epoll_event epev;
    memset(&epev, 0, sizeof(epoll_event));
    epev.events = 0;
    epev.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epev);
    if(rt){
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl fail(" << m_epfd
                                  << "," << op << "," << fd << "," << epev.events << ") fail,"
                                  << rt << "(" << errno << "," << strerror(errno) << ")";
        return false;
    }

    if(fd_ctx->event & READ){
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;
    }

    if(fd_ctx->event & WRITE){
        fd_ctx->triggerEvent(WRITE);
        --m_pendingEventCount;
    }
    SYLAR_ASSERT(fd_ctx->event == 0);
    return true;
}

IOManager* IOManager::GetThis(){
    return dynamic_cast<IOManager*>(Scheduler::GetThis());
}

void IOManager::tickle() {
    if(!hasIdleThreads()){
        return;
    }
    int rt = write(m_ticklefd[1], "T", 1);
    SYLAR_ASSERT(rt == 1);
    // SYLAR_LOG_DEBUG(g_logger) << Fiber::GetThis() << " send tickle";
}

bool IOManager::stopping(uint64_t& timeout){
    timeout = getNextTime();
    // SYLAR_LOG_DEBUG(g_logger) << "timeout == ~0ull : " << (timeout == ~0ull)
    //                           << ", m_pendingEventCount == 0 : " << (m_pendingEventCount == 0)
    //                           << ", Scheduler::stopping() : " << Scheduler::stopping();
    return timeout == ~0ull && m_pendingEventCount == 0  && Scheduler::stopping();
} 

//Scheduler类也会调用IOManager的stopping方法判断是否结束
bool IOManager::stopping() {
    // SYLAR_LOG_DEBUG(g_logger) << "IOManager stopping()";
    uint64_t timeout = 0;
    return stopping(timeout);
}

void IOManager::idle() {
    SYLAR_LOG_DEBUG(g_logger) << "iomanager idle begin";
    const uint64_t MAX_EVENTS = 64;
    epoll_event* events = new epoll_event[MAX_EVENTS]();
    std::shared_ptr<epoll_event> shared_events(events, [](epoll_event* ptr){delete[] ptr;});

    while(true){
        uint64_t next_timeout = 0;
        if(stopping(next_timeout)){
            SYLAR_LOG_DEBUG(g_logger) << "name = " << getName() << "idle stopping exit";
            break;
        }
        int rt = 0;
        do {
            static const int MAX_TIMEOUT = 2000;
            if(next_timeout != ~0ull){
                next_timeout = (int)next_timeout > MAX_TIMEOUT ? MAX_TIMEOUT : (int)next_timeout;
            }else{
                next_timeout = MAX_TIMEOUT;
            }
            rt = epoll_wait(m_epfd, events, MAX_EVENTS, (int)next_timeout);
            if(rt < 0 && errno == EINTR){       //防止epoll_wait在等待过程由于收到信号而被中断
            }else{
                break;
            }
        }while(true); 
        
        //执行计时器任务
        std::vector<std::function<void()>> cbs;
        listExpiredCbs(cbs);
        if(!cbs.empty()){
            schedule(cbs.begin(),cbs.end());
            // SYLAR_LOG_DEBUG(g_logger) << "schedule timers";
            cbs.clear();
            
            //debug：新增定时器任务则需要额外swapout去执行否则会在idle死循环
            //（因为后续逻辑仅处理IO回调，当没有IO事件时重新循环判断stopping条件，
            //而此时Scheduler的stopping条件一直无法满足也无法跳出idle）
            Fiber::ptr cur = Fiber::GetThis();
            auto raw_ptr = cur.get();
            cur.reset();
            raw_ptr->swapOutCaller();
        }

        for(int i = 0; i < rt ; ++i){
            epoll_event& event = events[i];
            if(event.data.fd == m_ticklefd[0]){
                uint8_t dump[256];
                while(read(m_ticklefd[0], &dump, sizeof(dump)) > 0){
                    // SYLAR_LOG_DEBUG(g_logger) << Fiber::GetThis() << " receive tickle";
                };
                continue;
            }

            {
                FdContext* fd_ctx = (FdContext*)event.data.ptr;
                FdContext::MutexType::Lock lock(fd_ctx->mutex);
                
                //当另一端发生异常时，继续尝试监听原本的事件，保证正确处理这些数据，之后再关闭文件描述符
                if(event.events & (EPOLLHUP | EPOLLERR)){      
                    event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->event;
                }
                int real_event = NONE;
                if(event.events & EPOLLIN){
                    real_event |= READ;
                }
                if(event.events & EPOLLOUT){
                    real_event |= WRITE;
                }
                if((fd_ctx->event & real_event) == NONE){
                    continue;
                }
                int left_event = fd_ctx->event & ~real_event;
                int op = left_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
                event.events = EPOLLET | left_event;

                int rt2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
                if(rt2){
                    SYLAR_LOG_ERROR(g_logger) << "epoll_ctl fail(" << m_epfd
                                    << "," << op << "," << fd_ctx->fd << "," << event.events << ") fail,"
                                    << rt << "(" << errno << "," << strerror(errno) << ")";
                    continue;
                }

                if(real_event & READ){
                    fd_ctx->triggerEvent(READ);
                    --m_pendingEventCount;
                }
                if(real_event & WRITE){
                    fd_ctx->triggerEvent(WRITE);
                    --m_pendingEventCount;
                }
            }
            Fiber::ptr cur = Fiber::GetThis();
            auto raw_ptr = cur.get();
            cur.reset();
            raw_ptr->swapOutCaller();
        }
    }
}

void IOManager::contextResize(size_t size){
    m_fdContexts.resize(64);
    for(size_t i = 0; i< m_fdContexts.size(); ++i){
        if(!m_fdContexts[i]){
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

void IOManager::onTimerInsertAtFront(){
    tickle();
}

}