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
    bool close();

    void setUserNonblock(bool v) {m_userNonblock = v;}
    bool getUserNonblock() const {return m_userNonblock;}
    void setSysNonblock(bool v) {m_sysNonblock = v;}
    bool getSysNonblock() const {return m_sysNonblock;}

    void setTimeout(int type, uint64_t v);
    uint64_t getTimeout(int type);
private:
    // : 1 表示使用位域来限定布尔变量只占用 1bit 位的内存空间
    bool m_isInit: 1 = false; 
    bool m_isSocket: 1 = false;
    bool m_sysNonblock: 1 = false;
    bool m_userNonblock: 1 = false;
    bool m_isClose: 1 = false;
    int m_fd;
    uint64_t m_recvTimeout = -1;
    uint64_t m_sendTimeout = -1;
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