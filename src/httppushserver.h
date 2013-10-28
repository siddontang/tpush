#ifndef _HTTPPUSHSERVER_H_
#define _HTTPPUSHSERVER_H_

#include <tr1/memory>
#include <tr1/functional>

#include <string>
#include <vector>

namespace tnet
{
    class HttpRequest;
    class Connection;    
    class HttpServer;
}

namespace tpush
{
    class PushServer;
    class PushConnection;

    class HttpPushServer
    {
    public:
        typedef std::tr1::shared_ptr<tnet::Connection> ConnectionPtr_t;

        HttpPushServer(PushServer* server);
        ~HttpPushServer();
    
        void start();
        void stop();

    private:
        void onRequest(const tnet::HttpRequest&, const ConnectionPtr_t& conn);

        void onSubscribe(const tnet::HttpRequest&, const ConnectionPtr_t& conn);
        void onUnsubscribe(const tnet::HttpRequest&, const ConnectionPtr_t& conn);
        void onPublish(const tnet::HttpRequest&, const ConnectionPtr_t& conn);
        void onPush(const ConnectionPtr_t& conn, const std::string& message);

        void sendResponse(const ConnectionPtr_t& conn, int statusCode, const std::string& message = std::string(""));
        int checkMethod(const tnet::HttpRequest&, int method, const ConnectionPtr_t& conn);
        int checkChannel(const tnet::HttpRequest&, const ConnectionPtr_t& conn, std::vector<std::string>& ids); 

    private:
        PushServer* m_server;    
   
        tnet::HttpServer* m_httpd;
    };
}

#endif
