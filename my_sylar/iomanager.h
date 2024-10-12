#ifndef __SYLAR_IOMANAGER_H__
#define __SYLAR_IOMANAGER_H__

#include "scheduler.h"


namespace sylar{

class IOManager : public Scheduler{
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;

    enum Event{
        NONE = 0x0,
        READ = 0x1,
        WRITE = 0x2,
    };            //每一位对应一个操作

    IOManager(size_t threads = 1, const std::string& name = "", bool use_caller = true);
    ~IOManager();

    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    //在当前协程为fd添加事件，事件执行函数或为回调函数cb，或为当前协程的上下文（如当前协程的fd需等待IO，则添加事件后可挂起等待IO结束后回来执行）
    bool delEvent(int fd, Event event);     //删除注册的事件和执行函数
    bool cancelEvent(int fd,Event event);   //立即触发事件执行函数但不删除
    bool cancelAll(int fd);

    static IOManager* GetThis();

private:

    struct FdContext{
        typedef Mutex MutexType;
        struct EventContext{
            Scheduler* scheduler = nullptr;
            Fiber::ptr fiber;
            std::function<void()> cb;
        };

        EventContext& getContext(Event event);
        void resetContext(EventContext& ctx);
        void triggerEvent(Event e);

        EventContext read;
        EventContext write;
        int fd = 0;
        MutexType mutex;
        Event event = NONE;        //记录已经注册的事件
    };


protected:
    void tickle() override;
    bool stopping() override;    
    void idle() override;         
    //基于epoll实现的调度器在run函数执行协程任务调度执行，当没有待调度的任务时陷入idle函数实现IO结束后回调函数的处理（加入到调度器等待调度）

    void contextResize(size_t size);
private:
    int m_epfd = 0;
    int m_ticklefd[2];

    std::atomic<size_t> m_pendingEventCount{0};
    RWMutexType m_mutex;
    std::vector<FdContext*> m_fdContexts;

};


}

#endif 