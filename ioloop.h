#ifndef _IOLOOP_H_
#define _IOLOOP_H_ 

#include <tr1/functional>
#include <vector>

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
   
        typedef std::tr1::function< void ()> TaskCallback;
        
        void addTask(const TaskCallback& callback);


    private:
        void wakeUp();
        void handleWaker();
        static void onWaked(struct ev_loop*, struct ev_async*, int);
       
        void check();
        static void onChecked(struct ev_loop*, struct ev_check*, int); 
        
        void runTasks();

        bool inLoopThread();

    private:
        struct ev_loop* m_loop;
        struct ev_async m_async; 
        struct ev_check m_check;

        bool m_stop;
        bool m_runTasks;
       
        pthread_t m_threadId;
        
        std::vector<TaskCallback> m_tasks;
        
        Mutex m_taskMutex;
    };
    
}

#endif
