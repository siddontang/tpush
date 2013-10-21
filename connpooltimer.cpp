#include "connpooltimer.h"

#include "timer.h"
#include "ioloop.h"
#include "misc.h"
#include "ioloopthreadpool.h"

using namespace std;

namespace tpush
{
    ConnPoolTimer::ConnPoolTimer(IOLoopThreadPool* pool, const TimerFunc_t& func, int milliseconds)
        : m_pool(pool)
    {
        vector<IOLoop*>& loops = pool->getLoops();
        for(size_t i = 0; i < loops.size(); ++i)
        {
            m_timers.push_back(new Timer(loops[i], func, milliseconds));    
        }
    }

    ConnPoolTimer::~ConnPoolTimer()
    {
        clearContainer(m_timers);    
    }

    void ConnPoolTimer::start()
    {
        for(size_t i = 0; i < m_timers.size(); ++i)
        {
            m_timers[i]->start();
        }
    }

    void ConnPoolTimer::stop()
    {
        for(size_t i = 0; i < m_timers.size(); ++i)
        {
            m_timers[i]->stop();    
        }    
    }

    void ConnPoolTimer::reset(int milliseconds)
    {
        for(size_t i = 0; i < m_timers.size(); ++i)
        {
            m_timers[i]->reset(milliseconds);    
        }    
    }
}
