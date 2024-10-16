#ifndef __SYLAR_TIMER_H__
#define __SYLAR_TIMER_H__

#include <memory>
#include <set>
#include <vector>
#include <functional>
#include "thread.h"


//解决时间漂移问题可以使用相对时间
//clock_gettime()：用于获取指定时钟的纳秒级时间，可以使用 CLOCK_REALTIME（系统时间）、CLOCK_MONOTONIC（自系统启动以来的相对时间） 

namespace sylar{

class TimerManager;

class Timer : public std::enable_shared_from_this<Timer> {
friend class TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;

    bool cancel();                              //取消定时器
    bool reset(uint64_t ms, bool from_now);     //是否从当前时刻开始重置定时器的时间间隔
    bool refresh();                             //刷新定时器的执行时间

private:
    Timer(uint64_t ms, std::function<void()>cb, bool recurring, TimerManager* manager);
    Timer(uint64_t next) : m_next(next) {};

    bool m_recurring = false;   //是否为循环定时器
    uint64_t m_ms = 0;          //执行时间周期（毫秒）
    uint64_t m_next = 0;        //下一次执行的实际时间
    std::function<void()> m_cb;
    TimerManager* m_manager = nullptr;

    struct Comparator{
        bool operator() (const Timer::ptr lhs, const Timer::ptr rhs) const;
    };
};

class TimerManager{
friend class Timer;
public:
    typedef RWMutex RWMutexType;
    
    TimerManager();
    virtual ~TimerManager();

    Timer::ptr addTimer(uint64_t ms, std::function<void()>cb, bool recurring = false);
    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()>cb, 
                                std::weak_ptr<void> weak_cond, bool recurring = false);  
    //条件定时器，根据条件是否存在执行回调

    uint64_t getNextTime();
    void listExpiredCbs(std::vector<std::function<void()>>& cbs);

protected:
    virtual void onTimerInsertAtFront() = 0;        //插入了一个时长最短的定时器，需要唤醒等待的协程重新设置等待时长
    void addTimer(Timer::ptr val, RWMutexType::WriteLock& wrlock);
    bool hasTimer();
private:
    bool detectClockRollover(uint64_t now_ms);      //大致检测服务器时间是否被手动调整(向前调整1小时)
private:
    RWMutexType m_mutex;
    std::set<Timer::ptr, Timer::Comparator> m_timers;
    bool m_tickled = false;                         //避免发生频繁修改时反复调用onTimerInsertAtFront
    uint64_t m_previousTime = 0;                    //程序运行时间戳，用于判断服务器是否被调整时间
};


}

#endif