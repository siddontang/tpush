#include "pushserver.h"

#include <tr1/memory>
#include <tr1/functional>

#include "ioloopthreadpool.h"
#include "pushloop.h"
#include "config.h"
#include "misc.h"

using namespace tnet;
using namespace std;
using namespace std::tr1::placeholders;

namespace tpush
{
    PushServer::PushServer()
    {
        m_pool = new IOLoopThreadPool(Config::PushLoopNum, "pushloop");    
    
        vector<IOLoop*> loops = m_pool->getLoops();
        m_loops.resize(loops.size());
        for(size_t i = 0; i < loops.size(); ++i)
        {
            m_loops[i] = new PushLoop(loops[i]);    
        }
    }

    PushServer::~PushServer()
    {
        for_each_all_delete(m_loops);
        delete m_pool;    
    }

    void PushServer::start()
    {
        m_pool->start();
        
        for_each_all(m_loops, std::tr1::bind(&PushLoop::start, _1));    
    }

    void PushServer::stop()
    {
        for_each_all(m_loops, std::tr1::bind(&PushLoop::stop, _1));
        
        m_pool->stop();    
    }
}
