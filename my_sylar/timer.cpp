#include "timer.h"
#include "util.h"

namespace sylar{

Timer::Timer(uint64_t ms, std::function<void()>cb, bool recurring, TimerManager* manager)
        : m_recurring(recurring), m_ms(ms), m_cb(cb), m_manager(manager) {
        m_next = sylar::GetCurMS() + m_ms;
}

bool Timer::Comparator::operator() (const Timer::ptr lhs, const Timer::ptr rhs) const {
    if(!lhs && !rhs){
        return false;
    }
    if(!lhs){
        return true;
    }
    if(!rhs){
        return false;
    } 
    if(lhs->m_next < rhs->m_next){
        return true;
    }
    if(rhs->m_next < lhs->m_next){
        return false;
    }
    return lhs.get() < rhs.get(); 
}

bool Timer::cancel(){
    TimerManager::RWMutexType::WriteLock wrlock(m_manager->m_mutex);
    if(m_cb){
        m_cb = nullptr;
        auto it = m_manager->m_timers.find(shared_from_this());
        m_manager->m_timers.erase(it);
        return true;
    }
    return false;
}

bool Timer::reset(uint64_t ms, bool from_now){
    if(ms == m_ms && !from_now){
        return true;
    }
    TimerManager::RWMutexType::WriteLock wrlock(m_manager->m_mutex);
    if(!m_cb){
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if(it == m_manager->m_timers.end()){
        return false;
    }
    m_manager->m_timers.erase(it);                      
    uint64_t start = 0;
    if(from_now){
        start = sylar::GetCurMS();
    }else{
        start = m_next - ms;
    }
    m_ms = ms;
    m_next = start + ms;
    m_manager->addTimer(shared_from_this(), wrlock);
    return true;
}

bool Timer::refresh(){
    TimerManager::RWMutexType::WriteLock wrlock(m_manager->m_mutex);
    if(!m_cb){
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if(it == m_manager->m_timers.end()){
        return false;
    }
    m_manager->m_timers.erase(it);                      //先删除后添加而非直接修改保证set基于m_next的顺序结构
    m_next = sylar::GetCurMS() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return true;
}

TimerManager::TimerManager(){
    m_previousTime = sylar::GetCurMS();
}

TimerManager::~TimerManager(){
}

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()>cb, bool recurring){
    Timer::ptr timer(new Timer(ms, cb, recurring, this));
    RWMutexType::WriteLock wrlock(m_mutex);
    addTimer(timer, wrlock);
    return timer;
}
// std::set::insert() 方法的返回值是一个 pair，包含两个部分：
// 迭代器：指向插入元素的位置。如果元素已经存在，返回指向已存在元素的迭代器。
// 布尔值：表示是否成功插入元素。如果元素已经存在，返回 false；如果元素是新插入的，返回 true。


//判断插入的定时器是否时长最短
void TimerManager::addTimer(Timer::ptr val, RWMutexType::WriteLock& wrlock){
    auto it = m_timers.insert(val).first;
    bool at_front = (it == m_timers.begin()) && !m_tickled;
    if(at_front){
        m_tickled = true;
    }
    wrlock.unlock();
    if(at_front){ 
        onTimerInsertAtFront();
    }
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb){
    std::shared_ptr<void> tmp = weak_cond.lock();   //获取管理所监测资源的shared_ptr对象
    if(tmp){
        cb();
    }
}
// weak_ptr 的主要用法包括
// 解决循环引用：当两个对象互相持有对方的 std::shared_ptr 时，会形成循环引用，导致内存泄漏。通过将其中一个对象对另一个对象的引用改为 std::weak_ptr，可以打破循环引用，使得对象能够被正确释放。
// 观察者模式：在观察者模式中，观察者通常需要跟踪可观察对象但不拥有它。观察者可以通过 std::weak_ptr.expired()->bool 方法来判断观测的资源是否已经被释放 来检查可观察对象是否仍然存在，并在需要时与之交互。
// 缓存实现：当实现缓存时，不希望缓存的存在延长对象的生命周期。std::weak_ptr 可以用来跟踪对象，如果对象仍然存在，就可以通过将 std::weak_ptr.lock() 提升为 std::shared_ptr 来使用它，否则可以认为对象已经被销毁或需要重新创建。


Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()>cb, 
                                std::weak_ptr<void> weak_cond, bool recurring){
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);

    }

uint64_t TimerManager::getNextTime(){
    RWMutexType::ReadLock rdlock(m_mutex);
    m_tickled = false;
    if(m_timers.empty()){
        return ~0ull;
    }
    uint64_t now_ms = sylar::GetCurMS();
    const Timer::ptr it = *m_timers.begin();
    if(it->m_next <= now_ms){
        return 0;
    }else{
        return it->m_next - now_ms;
    }
}

void TimerManager::listExpiredCbs(std::vector<std::function<void()>>& cbs){
    uint64_t now_ms = sylar::GetCurMS();
    {
        RWMutexType::ReadLock rdlock(m_mutex);
        if(m_timers.empty()){
            return;
        }
    }

    RWMutexType::WriteLock wrlock(m_mutex);
    bool rollover = detectClockRollover(now_ms);
    if(!rollover && (*m_timers.begin())->m_next > now_ms){
        return;
    }

    Timer::ptr now_timer(new Timer(now_ms));
    auto it = rollover ? m_timers.end() : m_timers.upper_bound(now_timer);   //检测到时间被修改则超时所有定时器

    std::vector<Timer::ptr> expired;
    expired.insert(expired.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(),it);
    cbs.reserve(expired.size());

    for(auto& t : expired){
        cbs.push_back(t->m_cb);
        if(t->m_recurring){
            t->m_next = now_ms + t->m_ms;
            m_timers.insert(t);
        }else{
            t->m_cb = nullptr;
        }
    }

}
// lower_bound 返回指向第一个值不小于 val 的位置，也就是返回第一个大于等于val值的位置。（通过二分查找） 
// upper_bound 返回第一个大于--val值的位置。

bool TimerManager::detectClockRollover(uint64_t now_ms){
    bool rollover = false;
    if(now_ms < m_previousTime && now_ms < (m_previousTime - 60 * 60 * 1000)){
        rollover = true;
    }
    m_previousTime = now_ms;
    return rollover;
}

}