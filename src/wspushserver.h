#ifndef _WSPUSHSERVER_H_
#define _WSPUSHSERVER_H_

#include <string>
#include <vector>

#include "nocopyable.h"
#include "httpdefs.h"

#include "pushenums.h"

namespace tnet
{
    class HttpServer;   
}

namespace tpush
{
    class PushServer;
    class WsPushRequest; 

    class WsPushServer : public tnet::nocopyable
    {
    public:
        WsPushServer(PushServer* server, tnet::HttpServer* httpd);
        ~WsPushServer();
        
        void start();
        void stop();
    
    private:
        void push(const tnet::ContextPtr_t& context, const std::string& message);
        
        void onEvent(const tnet::WsConnectionPtr_t& conn, tnet::WsEvent event, const std::string& message);

        void onData(const tnet::WsConnectionPtr_t& conn, const std::string& data);

    private:
        PushServer* m_server;
        tnet::HttpServer* m_httpd;     
    }; 
}

#endif
