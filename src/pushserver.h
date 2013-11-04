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
    class HttpServer;
}

namespace tpush
{
    class PushLoop;
    class HttpPushServer;
    class WsPushServer;

    class PushServer : public tnet::nocopyable
    {
    public:
        friend class HttpPushServer;
        friend class WsPushServer;

        PushServer();
        ~PushServer();
        
        void start();
        void stop();

    private:
        PushLoop* getHashLoop(const std::string& channel);
       
        typedef std::tr1::function<void (PushLoop*, const std::vector<std::string>&)> DispatchFunc_t;
        void dispatchChannels(const std::vector<std::string>& ids, const DispatchFunc_t& func);

    private:
        tnet::TcpServer* m_server;
        tnet::IOLoopThreadPool* m_pool;
        std::vector<PushLoop*> m_loops;
        
        tnet::HttpServer* m_httpd;
        HttpPushServer* m_httpPushServer;  
        WsPushServer* m_wsPushServer;  
    };
        
}

#endif
