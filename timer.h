#ifndef _TIMER_H_
#define _TIMER_H_

#include <tr1/functional>

extern "C"
{
#include <ev.h>    
}

#include "nocopyable.h"

namespace tpush
{
    class IOLoop;

    class Timer : public nocopyable
    {
    public:
        typedef std::tr1::function<void (IOLoop*)> TimerFunc_t;

        //milliseconds is Timer interval 
        Timer(IOLoop* loop, const TimerFunc_t& func, int milliseconds);
        ~Timer();
        
        void start();
        void stop();

        void reset(int milliseconds);

    private:
        void startInLoop();
        void stopInLoop();
        void resetInLoop(int);

        static void onTimer(struct ev_loop*, struct ev_timer*, int);

    private:
        IOLoop* m_loop;

        class Watcher
        {
        public:
            struct ev_timer timer;
            TimerFunc_t func;
        };
    
        Watcher m_watcher;
    };
    
}

#endif
