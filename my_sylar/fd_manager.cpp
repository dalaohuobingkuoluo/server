#include "fd_manager.h"
#include "hook.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace sylar{

FdCtx::FdCtx(int fd)
    : m_fd(fd) {
    init();
}

FdCtx::~FdCtx(){

}

bool FdCtx::init(){
    if(m_isInit){
        return true;
    }
    m_recvTimeout = -1;
    m_sendTimeout = -1;
    struct stat fd_stat;
    if(fstat(m_fd, &fd_stat) == -1){
        m_isInit = false;
        m_isSocket = false;
    }else{
        m_isInit = true;
        m_isSocket = S_ISSOCK(fd_stat.st_mode);
    }
    if(m_isSocket){
        int flag = fcntl_f(m_fd, F_GETFL, 0);
        if(!(flag & O_NONBLOCK)){
            fcntl_f(m_fd, F_SETFL, flag | O_NONBLOCK);
        }
        m_sysNonblock = true;
    }else{
        m_sysNonblock = false;
    }
    m_userNonblock = false;
    m_isClose = false;
    return m_isInit;
}

void FdCtx::setTimeout(int type, uint64_t v){
    if(type == SO_RCVTIMEO){
        m_recvTimeout = v;
    }else{
        m_sendTimeout = v;
    }
}

/// @brief 
/// @param type 
/// @return 
uint64_t FdCtx::getTimeout(int type){
    if(type == SO_RCVTIMEO){
        return m_recvTimeout;
    }else{
        return m_sendTimeout;
    }
}

FdManager::FdManager(){
    m_fds.resize(64);
}

FdCtx::ptr FdManager::get(int fd, bool auto_create){
    RWMutexType::ReadLock rdlock(m_mutex);
    if(m_fds.size() <= fd){
        if(!auto_create){
            return nullptr;
        }
    }else{
        if(m_fds[fd] || !auto_create){
            return m_fds[fd];
        }
    }
    rdlock.unlock();

    RWMutexType::WriteLock wrlock(m_mutex);
    FdCtx::ptr ctx(new FdCtx(fd));
    if(m_fds.size() <= fd){
        m_fds.resize(fd * 1.5); 
    }
    m_fds[fd] = ctx;
    return ctx;
}

void FdManager::del(int fd){
    RWMutexType::WriteLock wrlock(m_mutex);
    if(m_fds.size() <= fd){
        return ;
    }
    m_fds[fd].reset();
}

}