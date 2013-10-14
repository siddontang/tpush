#include "ioloop.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

namespace tpush
{
    IOLoop::IOLoop()
    {
        m_stop = false;
        m_runTasks = false;
        m_mainLoop = false;

        m_threadId = 0;

        m_loop = (struct ev_loop*)ev_loop_new(EVFLAG_AUTO);        
        ev_set_userdata(m_loop, this);
            
        ev_async_init(&m_asyncWatcher, &IOLoop::onWaked);
        ev_check_init(&m_checkWatcher, &IOLoop::onChecked);
    
        ev_async_start(m_loop, &m_asyncWatcher);
        ev_check_start(m_loop, &m_checkWatcher);
    }

    IOLoop::~IOLoop()
    {
        ev_async_stop(m_loop, &m_asyncWatcher);
        ev_check_stop(m_loop, &m_checkWatcher);
        
        clearSignals();
        
        ev_loop_destroy(m_loop);
    }

    void IOLoop::start()
    {
        m_threadId = pthread_self();
        m_stop = false;

        m_mainLoop = isMainThread(); 

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
        ev_async_send(m_loop, &m_asyncWatcher);    
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
            ev_break(m_loop, EVBREAK_ALL);
            return;    
        }
    }

    void IOLoop::addTask(const Callback_t& func)
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

        vector<Callback_t> tasks;

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
            ev_check_stop(m_loop, &m_checkWatcher);
        }

        runTasks();
    }

    void IOLoop::onSignal(struct ev_loop* loop, ev_signal* w, int revents)
    {
        SignalWatcher* sigWatcher = (SignalWatcher*)w;
        (sigWatcher->func)(sigWatcher->signum);
    }


    void IOLoop::addSignal(int signum, const SignalFunc_t& func)
    {
        std::tr1::function<void ()> taskFunc = std::tr1::bind(&IOLoop::asyncAddSignal, this, signum, func);
        addTask(taskFunc);    
    }

    void IOLoop::asyncAddSignal(int signum, const SignalFunc_t& func)
    {
        if(!m_mainLoop)
        {
            //only can add signal in main loop
            return;    
        }

        SignalWatchers_t::iterator iter = m_signalWatchers.find(signum);
        if(iter == m_signalWatchers.end())
        {
            SignalWatcher* watcher = new SignalWatcher;
            watcher->func = func;
            watcher->signum = signum;

            m_signalWatchers[signum] = watcher;

            ev_signal_init(&watcher->signal, IOLoop::onSignal, signum);      
            ev_signal_start(m_loop, &watcher->signal);
        }
        else
        {
            iter->second->func = func;    
        }
    }

    void IOLoop::clearSignals()
    {
        SignalWatchers_t::iterator iter = m_signalWatchers.begin();
        while(iter != m_signalWatchers.end())
        {
            ev_signal_stop(m_loop, &iter->second->signal);
            delete iter->second;
            ++iter;    
        }

        m_signalWatchers.clear();
    }
}
