#ifndef _HTTPPUSHSERVER_H_
#define _HTTPPUSHSERVER_H_

#include <tr1/memory>
#include <tr1/functional>

#include <string>
#include <vector>

#include "httpdefs.h"
#include "nocopyable.h"

namespace tnet
{
    class HttpRequest;
    class Connection;    
    class HttpServer;
    class HttpResponse;
}

namespace tpush
{
    class PushServer;
    class PushConnection;

    class HttpPushServer : public tnet::nocopyable
    {
    public:
        HttpPushServer(PushServer* server, tnet::HttpServer* httpd);
        ~HttpPushServer();
    
        void start();
        void stop();

    private:
        void onSubscribe(const tnet::HttpConnectionPtr_t& conn, const tnet::HttpRequest& request);
        void onUnsubscribe(const tnet::HttpConnectionPtr_t& conn, const tnet::HttpRequest& request);
        void onPublish(const tnet::HttpConnectionPtr_t& conn, const tnet::HttpRequest& request);

        void onRequest(const tnet::HttpConnectionPtr_t& conn, const tnet::HttpRequest& request);

        void push(const tnet::ContextPtr_t& context, const std::string& message);

        void sendResponse(const tnet::HttpConnectionPtr_t& conn, int statusCode, const std::string& message = std::string(""));
        int checkChannel(const tnet::HttpConnectionPtr_t& conn, const tnet::HttpRequest&, std::vector<std::string>& ids); 

    private:
        PushServer* m_server;    
   
        tnet::HttpServer* m_httpd;
    };
}

#endif
