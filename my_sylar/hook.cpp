#include "hook.h"
#include <dlfcn.h>
#include <functional>
#include "log.h"
#include "config.h"
#include "fiber.h"
#include "iomanager.h"
#include "fd_manager.h"
#include "util.h"
#include <functional>

namespace sylar{

static sylar::ConfigVar<int>::ptr g_tcp_connect_timeout 
                = sylar::Config::Lookup("tcp.connect.timeout", 5000, "tcp connect timeout");
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
    name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name);     //动态链接时将系统函数name映射到name_f上
    HOOK_FUN(XX);
#undef XX
}

static uint64_t s_connect_timeout = -1;
struct _HookIniter{
    _HookIniter(){
        hook_init();
        s_connect_timeout = g_tcp_connect_timeout->getValue();
        g_tcp_connect_timeout->addListener([](const int& oldv, const int& newv){
            SYLAR_LOG_DEBUG(g_logger) << "tcp connect timeout changed from " << oldv << " to " << newv;
            s_connect_timeout = newv;
        });
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

    SYLAR_LOG_DEBUG(g_logger) << "do_io hook <" << hook_fun_name << ">";

    uint64_t to = ctx->getTimeout(timeout_so);
    std::shared_ptr<timer_info> tinfo(new timer_info);


    ssize_t n;
    do {
        n = fun(fd, std::forward<Args>(args)...);
        while(n == -1 && errno == EINTR){                   //因为系统中断导致的错误则重试
            SYLAR_LOG_DEBUG(g_logger) << hook_fun_name << " EINTR";
            n = fun(fd, std::forward<Args>(args)...);
        }
        if(n == -1 && errno == EAGAIN){                     //因为IO操作暂无要处理的数据则设置定时任务并让出执行权执行异步操作
            SYLAR_LOG_DEBUG(g_logger) << hook_fun_name << " EAGAIN";
            sylar::IOManager* iom = sylar::IOManager::GetThis();
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
                SYLAR_LOG_DEBUG(g_logger) << hook_fun_name << " addEvent(" 
                                        << fd << ", " << event << ") error";
                if(timer){
                    timer->cancel();
                }
                return -1;
            }else{
                SYLAR_LOG_DEBUG(g_logger) << hook_fun_name << " before yiled";
                sylar::Fiber::YiledToHold();             //从条件定时器timer返回或有数据返回115行注册的IO事件返回
                SYLAR_LOG_DEBUG(g_logger) << hook_fun_name << " after yiled";
                if(timer){
                    timer->cancel();
                }
                if(tinfo->cancelled){                   //定时器超时返回
                    errno = tinfo->cancelled;
                    return -1;
                }
            }
        }
    }while(n < 0);       //使用do-while替换goto
    return n;
}
    
}

extern "C"{
#define XX(name) name ## _fun name ## _f = nullptr;
    HOOK_FUN(XX)
#undef XX



unsigned int sleep(unsigned int seconds){   //利用C无法重载的特性，重写sleep系统函数
    if(!sylar::t_hook_enable){
        return sleep_f(seconds);
    }
    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    iom->addTimer(seconds * 1000, std::bind((void(sylar::IOManager::*)
                 (sylar::Fiber::ptr, int))&sylar::IOManager::schedule<sylar::Fiber::ptr>, 
                  iom, fiber, -1));
    // iom->addTimer(seconds * 1000, [fiber,iom](){
    //     iom->schedule(fiber);
    //     // SYLAR_LOG_DEBUG(sylar::g_logger) << sylar::Fiber::GetThis()->getId() << " addTimer";
    // });
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
    iom->addTimer(usec / 1000, std::bind((void(sylar::IOManager::*)
                 (sylar::Fiber::ptr, int))&sylar::IOManager::schedule<sylar::Fiber::ptr>,
                  iom, fiber, -1));
    // iom->addTimer(usec * 1000, [fiber,iom](){
    //     iom->schedule(fiber);
    // });
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

int socket(int domain, int type, int protocol){
    if(!sylar::t_hook_enable){
        return socket_f(domain, type, protocol);
    }
    int fd = socket_f(domain, type, protocol);
    if(fd == -1){
        SYLAR_LOG_ERROR(sylar::g_logger) << "socket create fail";
        return fd;
    }
    sylar::FdMgr::GetInstance()->get(fd, true);
    return fd;
}

int connect_with_timeout(int sockfd, const struct sockaddr *addr, socklen_t addrlen, uint64_t timeout){
    if(!sylar::t_hook_enable){
        return connect_f(sockfd, addr, addrlen);
    }
    sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(sockfd);
    if(!ctx || ctx->isClose()){
        errno = EBADF;
        return -1;
    }
    if(!ctx->isSocket()){
        return connect_f(sockfd, addr, addrlen);
    }
    if(ctx->getUserNonblock()){
        return connect_f(sockfd, addr, addrlen);
    }

    int n = connect_f(sockfd, addr, addrlen);
    if(n == 0){
        return 0;
    }else if(n != -1 || errno != EINPROGRESS){
        return n;
    }
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    sylar::Timer::ptr timer;
    std::shared_ptr<sylar::timer_info> tinfo(new sylar::timer_info);
    std::weak_ptr<sylar::timer_info> winfo(tinfo);

    if(timeout != (uint64_t)-1){
        iom->addConditionTimer(timeout, [sockfd, iom, winfo](){
            SYLAR_LOG_DEBUG(sylar::g_logger) << "-------trigger cancelEvent------";
            auto t = winfo.lock();
            if(!t || t->cancelled){
                return;
            }
            t->cancelled = ETIMEDOUT;
            iom->cancelEvent(sockfd, sylar::IOManager::WRITE);
        }, winfo);
    }

    int rt = iom->addEvent(sockfd, sylar::IOManager::WRITE);
    if(rt == 0){
        // iom->m_fdContexts[sockfd]->mutex.addListener([](bool old_locked, bool new_locked){
        //     if (old_locked != new_locked) {
        //         SYLAR_LOG_DEBUG(sylar::g_logger) << "Lock state changed: from "
        //                                         << (old_locked ? "locked" : "unlocked") 
        //                                         << " to " << (new_locked ? "locked" : "unlocked");
        //         SYLAR_LOG_ERROR(SYLAR_LOG_ROOT) << "backtrace" 
        //                                         << sylar::BacktraceToString(100,"   \n") << "end"; 
        //     }
        // });
        // SYLAR_LOG_DEBUG(sylar::g_logger) << "connect_with_timeout before yield, " << "scokfd = " << sockfd 
        //                                  << " trylock " << iom->fdIsLock(sockfd);
        sylar::Fiber::YiledToHold();
        // SYLAR_LOG_DEBUG(sylar::g_logger) << "connect_with_timeout after yield, " << "scokfd = " << sockfd 
        //                                  << " trylock " << iom->fdIsLock(sockfd);
        if(timer){
            timer->cancel();
        }
        if(tinfo->cancelled){
            errno = tinfo->cancelled;
            return -1;
        }
    }else{
        if(timer){
            timer->cancel();
        }
        SYLAR_LOG_ERROR(sylar::g_logger) << "connect addEvent(" << sockfd << ", WRITE) error";
    }
    // SYLAR_LOG_DEBUG(sylar::g_logger) << "connect_with_timeout rt = " << rt;
    int error = 0;
    socklen_t size = sizeof(int);
    if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &size) == -1){
        return -1;
    }else if(error == 0){
        return 0;
    }else{
        errno = error;
        return -1;
    }
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
    return connect_with_timeout(sockfd, addr, addrlen, sylar::s_connect_timeout);
}


int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
    int fd = sylar::do_io(sockfd, accept_f, "accept", sylar::IOManager::Event::READ, SO_RCVTIMEO, addr, addrlen);
    if(fd >=0){
        sylar::FdMgr::GetInstance()->get(fd, true);
    }
    return fd;
}

ssize_t read(int fd, void *buf, size_t count){
    return sylar::do_io(fd, read_f, "read", sylar::IOManager::Event::READ, SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt){
    return sylar::do_io(fd, readv_f, "readv", sylar::IOManager::Event::READ, SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags){
    return sylar::do_io(sockfd, recv_f, "recv", sylar::IOManager::Event::READ, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen){
    return sylar::do_io(sockfd, recvfrom_f, "recvfrom", sylar::IOManager::Event::READ, 
                        SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags){
    return sylar::do_io(sockfd, recvmsg_f, "recvmsg", sylar::IOManager::Event::READ, SO_RCVTIMEO, msg, flags);
}

ssize_t write(int fd, const void *buf, size_t count){
    return sylar::do_io(fd, write_f, "write", sylar::IOManager::Event::WRITE, SO_SNDTIMEO, buf, count);
 }

ssize_t writev(int fd, const struct iovec *iov, int iovcnt){
    return sylar::do_io(fd, writev_f, "writev", sylar::IOManager::Event::WRITE, SO_SNDTIMEO, iov, iovcnt);
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags){
    return sylar::do_io(sockfd, send_f, "send", sylar::IOManager::Event::WRITE, SO_SNDTIMEO, buf, len, flags);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, 
            const struct sockaddr *dest_addr, socklen_t addrlen){
    return sylar::do_io(sockfd, sendto_f, "sendto", sylar::IOManager::Event::WRITE, 
                        SO_SNDTIMEO, buf, len, flags, dest_addr, addrlen);
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags){
    return sylar::do_io(sockfd, sendmsg_f, "sendmsg", sylar::IOManager::Event::WRITE, SO_SNDTIMEO, msg, flags);
}

int close(int fd){
    if(!sylar::t_hook_enable){
        return close_f(fd);
    }

    sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);
    if(ctx){
        sylar::IOManager* iom = sylar::IOManager::GetThis();
        if(iom){
            iom->cancelAll(fd); 
        }
    }
    sylar::FdMgr::GetInstance()->del(fd);
    return close_f(fd);
}

int fcntl(int fd, int cmd, ... /* arg */ ){
    va_list va;
    va_start(va, cmd);
    switch(cmd){
        case F_SETFL:
            {
                int arg = va_arg(va, int);
                va_end(va);
                sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);
                if(!ctx || ctx->isClose() || !ctx->isSocket()){
                    return fcntl_f(fd, cmd, arg);
                }
                ctx->setUserNonblock(arg & O_NONBLOCK);
                if(ctx->getSysNonblock()){
                    arg |= O_NONBLOCK;
                }else{
                    arg &= ~O_NONBLOCK;
                }
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETFL:
            {
                va_end(va);
                int arg = fcntl_f(fd, cmd);
                sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);
                if(!ctx || ctx->isClose() || !ctx->isSocket()){
                    return arg;
                }
                if(ctx->getUserNonblock()){
                    return arg | O_NONBLOCK;
                }else{
                    return arg & ~O_NONBLOCK;
                }
            }
            break;
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
        case F_SETPIPE_SZ:
        case F_ADD_SEALS:
            {
                int arg = va_arg(va, int);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
        case F_GETPIPE_SZ:
        case F_GET_SEALS:
            {
                va_end(va);
                return fcntl_f(fd, cmd);
            }
            break;
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
        case F_OFD_SETLK:
        case F_OFD_SETLKW:
        case F_OFD_GETLK:
            {
                struct flock * arg = va_arg(va, struct flock *);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETOWN_EX:
        case F_SETOWN_EX:
            {
                struct f_owner_ex * arg = va_arg(va, struct f_owner_ex *);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GET_RW_HINT:
        case F_SET_RW_HINT:
        case F_GET_FILE_RW_HINT:
        case F_SET_FILE_RW_HINT:
            {
                uint64_t * arg = va_arg(va, uint64_t *);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        default:
            {
                va_end(va);
                return fcntl_f(fd, cmd);
            }
            break;
    }
}

int ioctl(int fd, unsigned long request, ...){
    va_list va;
    va_start(va, request);
    void* arg = va_arg(va, void*);
    va_end(va);
    if(request == FIONBIO){
        bool user_nonblk = !!*(int*)arg;
        sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);
        if(!ctx || ctx->isClose() || !ctx->isSocket()){
            return ioctl_f(fd, request, arg);
        }
        ctx->setUserNonblock(user_nonblk);
    }
    return ioctl_f(fd, request, arg);
}

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen){
    return getsockopt_f(sockfd, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen){
    if(!sylar::t_hook_enable){
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }
    if(level == SOL_SOCKET){
        if(optname == SO_RCVTIMEO || optname == SO_SNDTIMEO){
            sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(sockfd);
            const timeval* t = (const timeval*)optval;
            ctx->setTimeout(optname, t->tv_sec * 1000 + t->tv_usec / 1000);
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
}

}