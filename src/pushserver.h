#ifndef _PUSHSERVER_H_
#define _PUSHSERVER_H_

#include <vector>
#include <string>
#include <tr1/memory>
#include <tr1/functional>

#include "nocopyable.h"

namespace tnet
{
    class nocopyable;
    class IOLoop;
    class IOLoopThreadPool;    
    class HttpRequest;
    class Connection;
    class TcpServer;
}

namespace tpush
{
    class PushLoop;
    class HttpPushServer;

    class PushServer : public tnet::nocopyable
    {
    public:
        friend class HttpPushServer;

        PushServer();
        ~PushServer();
        
        void start();
        void stop();
   
        tnet::TcpServer* getServer() { return m_server; }

    private:
        PushLoop* getHashLoop(const std::string& channel);
       
        typedef std::tr1::function<void (PushLoop*, const std::vector<std::string>&)> DispatchFunc_t;
        void dispatchChannels(const std::vector<std::string>& ids, const DispatchFunc_t& func);

    private:
        tnet::TcpServer* m_server;
        tnet::IOLoopThreadPool* m_pool;
        std::vector<PushLoop*> m_loops;
        
        HttpPushServer* m_httpPushServer;    
    };
        
}

#endif
