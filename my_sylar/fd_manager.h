#ifndef __SYLAR_FD_MANAGER_H__
#define __SYLAR_FD_MANAGER_H__

#include <memory>
#include <vector>
#include "thread.h"
#include "singleton.h"

namespace sylar{

class FdCtx : public std::enable_shared_from_this<FdCtx>{
public:
    typedef std::shared_ptr<FdCtx> ptr;
    FdCtx(int fd);
    ~FdCtx();

    bool init();
    bool isInit() const {return m_isInit;}
    bool isSocket() const {return m_isSocket;}
    bool isClose() const {return m_isClose;}

    void setUserNonblock(bool v) {m_userNonblock = v;}
    bool getUserNonblock() const {return m_userNonblock;}
    void setSysNonblock(bool v) {m_sysNonblock = v;}
    bool getSysNonblock() const {return m_sysNonblock;}

    void setTimeout(int type, uint64_t v);
    uint64_t getTimeout(int type);
private:
    // : 1 表示使用位域来限定布尔变量只占用 1bit 位的内存空间
    bool m_isInit: 1; 
    bool m_isSocket: 1;
    bool m_sysNonblock: 1;              //标志是否已经用fcntl设置O_NONBLOCK状态，在被hook的fcntl系统调用中设置arg参数时用到
    bool m_userNonblock: 1;             //标志是否用户已经设置O_NONBLOCK状态，在被hook的IO函数中用到，若已经设为非阻塞则无需执行额外的异步逻辑
    bool m_isClose: 1;
    int m_fd;
    uint64_t m_recvTimeout;
    uint64_t m_sendTimeout;
};

class FdManager{
public:
    typedef RWMutex RWMutexType;
    FdManager();

    FdCtx::ptr get(int fd, bool auto_create = false);
    void del(int fd);
private:
    RWMutexType m_mutex;
    std::vector<FdCtx::ptr> m_fds;
};

typedef sylar::Singleton<FdManager> FdMgr;

}

#endif