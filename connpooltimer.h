#ifndef _CONNPOOLTIMER_H_
#define _CONNPOOLTIMER_H_

#include <tr1/functional>
#include <vector>

#include "nocopyable.h"

namespace tpush
{
    class IOLoopThreadPool;
    class IOLoop;
    class Timer;

    class ConnPoolTimer : public nocopyable
    {
    public:
        typedef std::tr1::function<void (IOLoop*)> TimerFunc_t;
        ConnPoolTimer(IOLoopThreadPool* pool, const TimerFunc_t& func, int milliseconds);
        ~ConnPoolTimer();
        
        void start();
        void stop();

        void reset(int millseconds);

    private:
        IOLoopThreadPool* m_pool;
        
        std::vector<Timer*>  m_timers;    
    };
}

#endif
