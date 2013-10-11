#ifndef _THREADUTIL_H_
#define _THREADUTIL_H_

#include <pthread.h>

#include "nocopyable.h"

namespace tpush
{

    class Mutex : public nocopyable
    {
    public:
        Mutex() 
        {
            pthread_mutex_init(&m_mutex, NULL);   
        }
        ~Mutex()
        {
            pthread_mutex_destroy(&m_mutex);    
        }
        
        pthread_mutex_t* get() {
            return &m_mutex;
        }

        void lock()
        {
            pthread_mutex_lock(&m_mutex);
        }

        void unlock()
        {
            pthread_mutex_unlock(&m_mutex);
        }
        
    private:
        pthread_mutex_t m_mutex;    
    };

    class MutexGuard : public nocopyable
    {
    public:
        MutexGuard(Mutex& mutex)
            : m_mutex(mutex)
        {
            m_mutex.lock();    
        }

        ~MutexGuard()
        {
            m_mutex.unlock();    
        }

    private:
        Mutex& m_mutex;
    };
   
   
    class Condition : public nocopyable
    {
    public:
        Condition(Mutex& mutex)
            : m_mutex(mutex)
        {
            pthread_cond_init(&m_cond, NULL);    
        }

        ~Condition()
        {
            pthread_cond_destroy(&m_cond);
        }

        void wait()
        {
            MutexGuard mg(m_mutex);
            pthread_cond_wait(&m_cond, m_mutex.get());    
        }

        void wait(int timeout);

        void notify()
        {
            pthread_cond_signal(&m_cond);    
        }

        void notifyAll()
        {
            pthread_cond_broadcast(&m_cond);    
        }

    private:
        Mutex& m_mutex;
        pthread_cond_t m_cond;
    }; 
}

#endif
