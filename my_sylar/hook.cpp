#include "hook.h"
#include <dlfcn.h>
#include <functional>
#include "log.h"
#include "fiber.h"
#include "iomanager.h"
#include "fd_manager.h"

namespace sylar{

static thread_local bool t_hook_enable = false;     //线程级hook
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

#define HOOK_FUN(XX)\
    XX(sleep)\
    XX(usleep)\
    XX(nanosleep)\
    XX(socket)\
    XX(connect)\
    XX(accept)\
    XX(read)\
    XX(readv)\
    XX(recv)\
    XX(recvfrom)\
    XX(recvmsg)\
    XX(write)\
    XX(writev)\
    XX(send)\
    XX(sendto)\
    XX(sendmsg)\
    XX(close)\
    XX(fcntl)\
    XX(ioctl)\
    XX(getsockopt)\
    XX(setsockopt)


void hook_init(){
    static bool is_inited = false;
    if(is_inited){
        return;
    }
    SYLAR_LOG_DEBUG(g_logger) << "hook init";

#define XX(name)\
    name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name);
    HOOK_FUN(XX);
#undef XX
}

struct _HookIniter{
    _HookIniter(){
        hook_init();
    }
};

static _HookIniter s_hook_initer;

bool is_hook_enable(){
    return t_hook_enable;
}

void set_hook_enable(bool flag){
    t_hook_enable = flag;
}

struct timer_info{
    int cancelled = 0;
};

template<typename OriginFun, typename... Args>
static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name,
                     uint32_t event, int timeout_so, Args&&... args){
    if(!sylar::t_hook_enable){                          //无需hook
        return fun(fd, std::forward<Args>(args)...);
    }

    sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);
    if(!ctx){                                           //不是程序管理的句柄
        return fun(fd, std::forward<Args>(args)...);
    }
    if(ctx->isClose()){
        errno = EBADF;
        return -1;
    }
    if(!ctx->isSocket() || ctx->getUserNonblock()){     //是程序管理的句柄但不是socket或用户设置了非阻塞
        return fun(fd, std::forward<Args>(args)...);
    }

    uint64_t to = ctx->getTimeout(timeout_so);
    std::shared_ptr<timer_info> tinfo(new timer_info);


    ssize_t n = fun(fd, std::forward<Args>(args)...);
    while(n == -1 && errno == EINTR){                   //因为系统中断导致的错误则重试
        n = fun(fd, std::forward<Args>(args)...);
    }
    if(n == -1 && errno == EAGAIN){                     //因为IO操作暂无要处理的数据则设置定时任务并让出执行权
        sylar::IOManager::ptr iom = sylar::IOManager::GetThis();
        sylar::Timer::ptr timer;
        std::weak_ptr<timer_info> winfo(tinfo);

        if(to != (uint64_t)-1){
            timer = iom->addConditionTimer(to, [winfo, fd, iom, event](){
                auto t = winfo.lock();
                if(!t || t->cancelled){             //条件定时器：条件不存在或事件已经触发则直接退出
                    return;
                }
                t->cancelled = ETIMEDOUT;           //定时器超时则取消并强制执行所有事件
                iom->cancelEvent(fd, (sylar::IOManager::Event)event);
            }, winfo);
        }

        int rt = iom->addEvent(fd, (sylar::IOManager::Event)event);         //将当前协程上下文作为回调函数注册事件
        if(rt){
            SYLAR_LOG_ERROR(g_logger) << hook_fun_name << "addEvent(" 
                                    << fd << ", " << event << ") error";
            if(timer){
                timer->cancel();
            }
            return -1;
        }else{
            sylar::Fiber::YiledToHold();
            if(timer){
                timer->cancel();
            }
            if(tinfo->cancelled){
                errno = tinfo->cancelled;
                return -1;
            }
        }
    }

    return n;
}
    
}

extern "C"{
#define XX(name) name ## _fun name ## _f = nullptr;
    HOOK_FUN(XX)
#undef XX



unsigned int sleep(unsigned int seconds){
    if(!sylar::t_hook_enable){
        return sleep_f(seconds);
    }
    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    // iom->addTimer(seconds * 1000, std::bind(&sylar::IOManager::schedule<sylar::Fiber::ptr>, &iom, fiber));
    iom->addTimer(seconds * 1000, [fiber,iom](){
        iom->schedule(fiber);
        // SYLAR_LOG_DEBUG(sylar::g_logger) << sylar::Fiber::GetThis()->getId() << " addTimer";
    });
    // SYLAR_LOG_DEBUG(sylar::g_logger) << fiber->getId() << " add a timer(" << seconds << ") and  YieldToHold";
    sylar::Fiber::YiledToHold();
    // SYLAR_LOG_DEBUG(sylar::g_logger) << "sleep return";
    return 0;
}

int usleep(useconds_t usec){
    if(!sylar::t_hook_enable){
        return usleep_f(usec);
    }
    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    // iom->addTimer(usec / 1000, std::bind(&sylar::IOManager::schedule<sylar::Fiber::ptr>, &iom, fiber));
    iom->addTimer(usec * 1000, [fiber,iom](){
        iom->schedule(fiber);
    });
    // SYLAR_LOG_DEBUG(sylar::g_logger) << fiber->getId() << " add a timer(" << usec << ") and  YieldToHold";
    sylar::Fiber::YiledToHold();
    // SYLAR_LOG_DEBUG(sylar::g_logger) << "usleep return";
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem){
    if(!sylar::t_hook_enable){
        return nanosleep(req, rem);
    }
    int timeout = req->tv_sec * 1000 + req->tv_nsec / 1000 / 1000;
    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    iom->addTimer(timeout, [fiber,iom](){
        iom->schedule(fiber);
    });
    sylar::Fiber::YiledToHold();
    return 0;
}

}