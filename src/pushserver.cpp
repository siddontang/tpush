#include "pushserver.h"

#include <tr1/memory>
#include <tr1/functional>
#include <map>

#include "ioloopthreadpool.h"
#include "pushloop.h"
#include "config.h"
#include "misc.h"
#include "pushconnection.h"
#include "stringutil.h"
#include "tcpserver.h"
#include "httppushserver.h"
#include "httpserver.h"
#include "wspushserver.h"

using namespace tnet;
using namespace std;
using namespace std::tr1::placeholders;

namespace tpush
{
    PushServer::PushServer()
    {
        m_server = new TcpServer(Config::TcpAcceptLoopNum, Config::TcpConnLoopNum, Config::TcpMaxConnections);

        m_server->setConnCheckRepeat(Config::TcpConnCheckRepeat);
        m_server->setConnCheckStep(Config::TcpConnCheckStep);
        m_server->setConnTimeout(Config::TcpConnTimeout);

        m_httpd = new HttpServer(m_server);
       
        m_httpPushServer = new HttpPushServer(this, m_httpd);
        m_wsPushServer = new WsPushServer(this, m_httpd);

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
        
        delete m_wsPushServer;
        delete m_httpPushServer;

        delete m_httpd;

        delete m_server;
    }

    void PushServer::start()
    {
        m_pool->start();
        
        for_each_all(m_loops, std::tr1::bind(&PushLoop::start, _1));    
    
        m_httpPushServer->start();
        m_wsPushServer->start();

        m_httpd->listen(Address(Config::HttpListenIp, Config::HttpListenPort));       

        m_server->start();
    }

    void PushServer::stop()
    {
        for_each_all(m_loops, std::tr1::bind(&PushLoop::stop, _1));
        
        m_pool->stop(); 
        
        m_httpPushServer->stop();
        
        m_server->stop();   
    }

    PushLoop* PushServer::getHashLoop(const string& channel)
    {
        return m_loops[StringUtil::hash(channel) % m_loops.size()]; 
    }

    void PushServer::dispatchChannels(const vector<string>& ids, const DispatchFunc_t& func)
    {
        vector<vector<string> > loopChannels;
        loopChannels.resize(m_loops.size());
        for(size_t i = 0; i < ids.size(); ++i)
        {
            uint32_t index = StringUtil::hash(ids[i]) % m_loops.size();
            loopChannels[index].push_back(ids[i]);
        }

        for(size_t i = 0; i < loopChannels.size(); ++i)
        {
            PushLoop* loop = m_loops[i];
            
            func(loop, loopChannels[i]);    
        }
    }
}
