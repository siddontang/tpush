#include "ioloop.h"

#include <stdlib.h>

using namespace std;

namespace tpush
{
    IOLoop::IOLoop()
    {
        m_stop = false;
        m_runTasks = false;

        m_loop = (struct ev_loop*)ev_loop_new(EVFLAG_AUTO);        
        ev_set_userdata(m_loop, this);
            
        ev_async_init(&m_async, &IOLoop::onWaked);
        ev_check_init(&m_check, &IOLoop::onChecked);
    }

    IOLoop::~IOLoop()
    {
        ev_loop_destroy(m_loop);
    }

    void IOLoop::start()
    {
        m_threadId = pthread_self();
        m_stop = false;

        ev_check_start(m_loop, &m_check);

        ev_run(m_loop, 0);
    }

    void IOLoop::stop()
    {
        m_stop = true;
        ev_break(m_loop, EVBREAK_ALL);    
        wakeUp();
    }

    bool IOLoop::inLoopThread()
    {
        return m_threadId == pthread_self();    
    }

    void IOLoop::wakeUp()
    {
        ev_async_send(m_loop, &m_async);    
    }

    void IOLoop::onWaked(struct ev_loop* loop, struct ev_async* w, int revent)
    {
        IOLoop* ioloop = (IOLoop*)ev_userdata(loop);
        ioloop->handleWaker();
    }

    void IOLoop::handleWaker()
    {
        if(m_stop)
        {
            return;    
        }
    }

    void IOLoop::addTask(const TaskFunc_t& func)
    {
        if(m_stop)
        {
            return;    
        }

        {
            SpinLockGuard gu(m_taskLock);
            m_tasks.push_back(func);    
        }

        if(!inLoopThread() || m_runTasks)
        {
            wakeUp();   
        }
    }

    void IOLoop::runTasks()
    {
        m_runTasks = true;

        vector<TaskFunc_t> tasks;

        {
            SpinLockGuard gu(m_taskLock);
            m_tasks.swap(tasks);    
        }

        for(size_t i = 0; i < tasks.size() && !m_stop; ++i)
        {
            tasks[i]();
        }

        m_runTasks = false;    
    }

    void IOLoop::onChecked(struct ev_loop* loop, struct ev_check* w, int revents)
    {
        IOLoop* ioloop = (IOLoop*)ev_userdata(loop);
        ioloop->check();    
    }

    void IOLoop::check()
    {
        if(m_stop)
        {
            ev_check_stop(m_loop, &m_check);
        }

        runTasks();
    }
}
