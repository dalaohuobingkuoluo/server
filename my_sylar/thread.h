#ifndef __SYLAR_THREAD_H__
#define __SYLAR_THREAD_H__

#include <thread>
#include <memory>
#include <string>
#include <pthread.h>
#include <functional>
#include <semaphore.h>

namespace sylar{

class Semaphore{
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();
    void wait();
    void post();
private:
    Semaphore(const Semaphore&) = delete;
    Semaphore(const Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    sem_t m_semaphore;
};

template<class T>
class ScopedLockImpl{
public:
    ScopedLockImpl(T& mutex): m_mutex(mutex){
        m_mutex.lock();
        m_locked = true;
    }

    ~ScopedLockImpl(){
        unlock();
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }

    void lock(){
        if(!m_locked){
            m_mutex.lock();
            m_locked = true;
        }
    }

private:
    T& m_mutex;
    bool m_locked;  //确保区域锁只被释放一次
};

class NullMutex{
public:
    typedef ScopedLockImpl<NullMutex> Lock;
    NullMutex(){}
    ~NullMutex(){}
    void lock(){}
    void unlock(){}
};

class Mutex{
public:
    typedef ScopedLockImpl<Mutex> Lock;
    Mutex(){
        pthread_mutex_init(&m_mutex,nullptr);
    }

    ~Mutex(){
        pthread_mutex_destroy(&m_mutex);
    }

    void lock(){    
        pthread_mutex_lock(&m_mutex);
        m_locked = true;
    }

    void unlock(){
        pthread_mutex_unlock(&m_mutex);
        m_locked = false;
    }

    bool islock(){
        return m_locked;
    }

private:
    pthread_mutex_t m_mutex;
    bool m_locked;  
};

template<class T>
class ReadScopedLockImpl{
public:
    ReadScopedLockImpl(T& mutex): m_mutex(mutex){
        m_mutex.rdlock();
        m_locked = true;
    }

    ~ReadScopedLockImpl(){
        unlock();
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }

    void lock(){
        if(!m_locked){
            m_mutex.rdlock();
            m_locked = true;
        }
    }

private:
    T& m_mutex;
    bool m_locked;  
};

template<class T>
class WriteScopedLockImpl{
public:
    WriteScopedLockImpl(T& mutex): m_mutex(mutex){
        m_mutex.wrlock();
        m_locked = true;
    }

    ~WriteScopedLockImpl(){
        unlock();
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }

    void lock(){
        if(!m_locked){
            m_mutex.wrlock();
            m_locked = true;
        }
    }

private:
    T& m_mutex;
    bool m_locked; 
};

class NullRWMutex{
public:
    typedef WriteScopedLockImpl<NullRWMutex> WriteLock;
    typedef ReadScopedLockImpl<NullRWMutex> ReadLock;
    NullRWMutex(){}
    ~NullRWMutex(){}
    void rdlock(){}
    void wrlock(){}
    void unlock(){}
};

class RWMutex{
public:
    typedef WriteScopedLockImpl<RWMutex> WriteLock;
    typedef ReadScopedLockImpl<RWMutex> ReadLock;

    RWMutex(){
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex(){
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock(){
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock(){
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock(){
        pthread_rwlock_unlock(&m_lock);
    }

private:
    pthread_rwlock_t m_lock;
};

class Spinlock{
public:
    typedef ScopedLockImpl<Spinlock> Lock;

    Spinlock(){
        pthread_spin_init(&m_mutex,0);
    }

    ~Spinlock(){
        pthread_spin_destroy(&m_mutex);
    }

    void lock(){
        pthread_spin_lock(&m_mutex);
    }

    void unlock(){
        pthread_spin_unlock(&m_mutex);
    }

private:
    pthread_spinlock_t m_mutex;
};

class Thread {
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, const std::string name);
    ~Thread();
    
    pid_t getId() const { return m_id; }
    const std::string getName() const { return m_name; }

    void join();

    static Thread* GetThis();
    static const std::string GetName();
    static void SetName(const std::string& name);

private:
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;

    static void* run(void* arg);   
    //线程具体的初始化以及执行回调函数都在run函数中进行
    //（可能在回调函数中要用到线程的参数信息，因此在构造函数进行参数初始化由于异步导致参数可能未初始化完成）

    pid_t m_id = -1;
    pthread_t m_trhread = 0;
    std::function<void()> m_cb;
    std::string m_name;
    Semaphore m_sem;              //确保构造函数执行完之后，初始化线程参数已经完成
};


}

#endif