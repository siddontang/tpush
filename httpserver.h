#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include <tr1/functional>

#include "nocopyable.h"
#include "connection.h"

extern "C"
{
#include "http_parser.h"    
}

namespace tpush
{
    class TcpServer;
    class Address;
    class HttpConnection;

    class HttpServer : public nocopyable
    {
    public:
        HttpServer(TcpServer* server);
        ~HttpServer();
        
        void setMaxHeaderSize(int headerSize) { m_maxHeaderSize = headerSize; }
        void setMaxBodySize(int bodySize) { m_maxBodySize = bodySize; }

        int getMaxHeaderSize() { return m_maxHeaderSize; }
        int getMaxBodySize() { return m_maxBodySize; }

        int listen(const Address& addr);
    
        typedef std::tr1::function<void (HttpConnection*)> HttpRequestCallback_t;
        void setRequestCallback(const HttpRequestCallback_t& func) { m_func = func; }
           
    private:
        void onConnEvent(Connection*, Connection::Event, const char*, int);

        void onConnRead(Connection*, const char*, int);
        void onConnWriteComplete(Connection*);
        void onConnClose(Connection*);
        void onConnError(Connection*);

    private:
        TcpServer* m_server;
    
        int m_maxHeaderSize;
        int m_maxBodySize;
    
        struct http_parser_settings m_parserSettings;
    
        HttpRequestCallback_t  m_func;
    };
    
}

#endif
