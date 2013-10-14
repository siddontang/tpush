#ifndef _IOLOOP_H_
#define _IOLOOP_H_ 

#include <tr1/functional>
#include <vector>
#include <map>

extern "C" 
{
#include <ev.h>
}

#include "threadutil.h"

namespace tpush
{

    class IOLoop
    {
    public:
        IOLoop();
        ~IOLoop();
           
        void start();
        void stop();
   
        typedef std::tr1::function<void ()> Callback_t;      
        void addTask(const Callback_t& func);

        typedef std::tr1::function<void (int)> SignalFunc_t;
        void addSignal(int signum, const SignalFunc_t& func);

        typedef std::tr1::function<void (int, int)> SocketFunc_t;
        void addSocket(int sockFd, const SocketFunc_t& func, int events);
        void delSocket(int sockFd); 

    private:
        void wakeUp();
        void handleWaker();
        static void onWaked(struct ev_loop*, struct ev_async*, int);
       
        void check();
        static void onChecked(struct ev_loop*, struct ev_check*, int); 
       
        void clearSignals();
        static void onSignal(struct ev_loop*, struct ev_signal*, int);
        
        void asyncAddSignal(int signum, const SignalFunc_t& func);
        
        void runTasks();

        bool inLoopThread();

    private:
        struct ev_loop* m_loop;
        struct ev_async m_asyncWatcher; 
        struct ev_check m_checkWatcher;

        bool m_stop;
        bool m_runTasks;
       
        pthread_t m_threadId;
        
        std::vector<Callback_t> m_tasks;
            
        SpinLock m_taskLock;

        class SignalWatcher
        {
        public:
            ev_signal     signal;    
            SignalFunc_t  func;
            int           signum;
        };

        typedef std::map<int, SignalWatcher*> SignalWatchers_t;
        SignalWatchers_t m_signalWatchers;

        bool m_mainLoop;
    };
    
}

#endif
