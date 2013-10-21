#include "timer.h"

#include "ioloop.h"

namespace tpush
{

    Timer::Timer(IOLoop* loop, const TimerFunc_t& func, int milliseconds)
        : m_loop(loop)
    {
        m_watcher.func = func;
        ev_timer_init(&(m_watcher.timer), Timer::onTimer, 0, (ev_tstamp)(milliseconds / 1000.0));         
    }

    Timer::~Timer()
    {
        
    }

    void Timer::start()
    {
        if(!m_loop->inLoopThread())
        {
            m_loop->runTask(std::tr1::bind(&Timer::startInLoop, this));
        }   
        else
        {
            startInLoop();    
        }
    }
    
    void Timer::stop()
    {
        if(!m_loop->inLoopThread())
        {
            m_loop->runTask(std::tr1::bind(&Timer::stopInLoop, this));    
        }
        else
        {
            stopInLoop();    
        }
    }

    void Timer::reset(int milliseconds)
    {
        if(!m_loop->inLoopThread())
        {
            m_loop->runTask(std::tr1::bind(&Timer::resetInLoop, this, milliseconds));    
        }
        else
        {
            resetInLoop(milliseconds);    
        }
    }

    void Timer::startInLoop()
    {
        ev_timer_start(m_loop->evloop(), &m_watcher.timer);    
    }

    void Timer::stopInLoop()
    {
        ev_timer_stop(m_loop->evloop(), &m_watcher.timer);    
    }

    void Timer::resetInLoop(int milliseconds)
    {
        m_watcher.timer.repeat = milliseconds / 1000.0;
        
        ev_timer_again(m_loop->evloop(), &m_watcher.timer); 
            
    }

    void Timer::onTimer(struct ev_loop* loop, struct ev_timer* w, int revents)
    {
        Watcher* watcher = (Watcher*)w;
        
        (watcher->func)((IOLoop*)(ev_userdata(loop)));    
    }
}
