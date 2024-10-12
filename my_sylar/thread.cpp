#include "thread.h"
#include "log.h"
#include "util.h"

namespace sylar{

static thread_local Thread* t_thread = nullptr;       //指向当前执行的线程
static thread_local std::string t_thread_name = "UNKNOW";

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

Semaphore::Semaphore(uint32_t count){
    if(sem_init(&m_semaphore, 0, count)){
        SYLAR_LOG_ERROR(g_logger) << "thread name = " << t_thread_name << ", sem_init fail";
        throw std::logic_error("sem_init error");
    }
}

Semaphore::~Semaphore(){
    sem_destroy(&m_semaphore);
}

void Semaphore::wait(){
    if(sem_wait(&m_semaphore)){
        SYLAR_LOG_ERROR(g_logger) << "thread name = " << t_thread_name << ", sem_wait fail";
        throw std::logic_error("sem_wait error");
    }
}

void Semaphore::post(){
    if(sem_post(&m_semaphore)){
        SYLAR_LOG_ERROR(g_logger) << "thread name = " << t_thread_name << ", sem_post fail";
        throw std::logic_error("sem_post error");
    }
}

Thread* Thread::GetThis(){
    return t_thread;
}

const std::string Thread::GetName(){
    return t_thread_name;
}

void Thread::SetName(const std::string& name){
    if(name.empty())
        return;
    if(t_thread){
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

void* Thread::run(void* arg){
    Thread* thread = (Thread*) arg;
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = sylar::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0,15).c_str());
    std::function<void()> cb;
    cb.swap(thread->m_cb);
    thread->m_sem.post();
    cb();
    return 0;
}
    
Thread::Thread(std::function<void()> cb, const std::string name): m_cb(cb), m_name(name) {
    if(name.empty()){
        m_name = "UNKNOW";
    }
    int rt = pthread_create(&m_trhread, nullptr, Thread::run, this);
    if(rt){
        SYLAR_LOG_ERROR(g_logger) << "thread name = " << m_name << " pthread_create fail, rt = " << rt;
        throw std::logic_error("pthread_create error");
    }
    m_sem.wait();
}

Thread::~Thread(){
    if(m_trhread){
        pthread_detach(m_trhread);
    }
}

void Thread::join(){
    if(m_trhread){
        int rt = pthread_join(m_trhread, nullptr);
        if(rt){
            SYLAR_LOG_ERROR(g_logger) << "thread name = " << m_name << " pthread_join fail, rt = " << rt;
            throw std::logic_error("pthread_join error");
        }
        m_trhread = 0; 
    }
}


    
}