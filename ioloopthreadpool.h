#ifndef _IOLOOPTHREADPOOL_H_
#define _IOLOOPTHREADPOOL_H_

#include <vector>

namespace tpush
{
    class IOLoop;
    class IOLoopThread;
    class IOLoopThreadPool
    {
    public:
        IOLoopThreadPool(int threadNum);
        ~IOLoopThreadPool();
        
        void start();
        void stop();

        int getPoolNum() { return (int)m_loops.size(); }
        IOLoop* getLoop(int hashKey) { return m_loops[hashKey % getPoolNum()]; }
        std::vector<IOLoop*>& getLoops() { return m_loops; }
    
    private:
        std::vector<IOLoopThread*> m_threads;
        std::vector<IOLoop*> m_loops;
    };
    
}

#endif