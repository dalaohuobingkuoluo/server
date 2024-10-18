#include "hook.h"
#include <dlfcn.h>
#include <functional>
#include "log.h"
#include "fiber.h"
#include "iomanager.h"

namespace sylar{

static thread_local bool t_hook_enable = false;     //线程级hook
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

#define HOOK_FUN(XX)\
    XX(sleep)\
    XX(usleep)\
    XX(nanosleep)

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