#include "httpserver.h"

#include "tcpserver.h"
#include "httpconnection.h"
#include "log.h"

using namespace std::tr1::placeholders;

namespace tpush
{
    const int DefaultMaxHeaderSize = 1024 * 1024;
    const int DefaultMaxBodySize = 1024 * 1024 * 1024;

    typedef std::tr1::shared_ptr<HttpConnection>  HttpConnectionPtr_t;

    void dummyRequestCallback(HttpConnection* conn)
    {
        
    }

    HttpServer::HttpServer(TcpServer* server)
        : m_server(server)
        , m_maxHeaderSize(DefaultMaxHeaderSize)
        , m_maxBodySize(DefaultMaxBodySize)
    {
        m_func = std::tr1::bind(&dummyRequestCallback, _1);
        HttpConnection::initParserSettings(&m_parserSettings);
    }
   
    HttpServer::~HttpServer()
    {
        
    }
     
    int HttpServer::listen(const Address& addr)
    {
        return m_server->listen(addr, std::tr1::bind(&HttpServer::onConnEvent, this, _1, _2, _3, _4));     
    }

    void HttpServer::onConnEvent(Connection* conn, Connection::Event event, const char* buffer, int count)
    {
        switch(event)
        {
            case Connection::ReadEvent:
                return onConnRead(conn, buffer, count);
            case Connection::WriteCompleteEvent:
                return onConnWriteComplete(conn);
            case Connection::CloseEvent:
                return onConnClose(conn);
            case Connection::ErrorEvent:
                return onConnError(conn);    
            default:
                break;
        }
    }

    void HttpServer::onConnRead(Connection* conn, const char* buffer, int count)
    {
        std::tr1::shared_ptr<HttpConnection> c = std::tr1::static_pointer_cast<HttpConnection>(conn->getUserData());
        
        if(!bool(c))
        {
            LOG_INFO("create http connection");
            c = std::tr1::shared_ptr<HttpConnection>(new HttpConnection(conn, this));
            conn->setUserData(c);
        }                  

        struct http_parser* parser = c->getParser();
        parser->data = c.get(); 

        int n = http_parser_execute(parser, &m_parserSettings, buffer, count);
        if(n != count)
        {
            LOG_ERROR("parser error %s:%s", 
                http_errno_name((http_errno)parser->http_errno), \
                http_errno_description((http_errno)parser->http_errno));
            
            conn->shutDown();
            return;    
        } else if(parser->upgrade)
        {
            //we may not support websocket
            conn->shutDown();
            return;
        }
    }

    void HttpServer::onConnWriteComplete(Connection* conn)
    {
        
    }

    void HttpServer::onConnClose(Connection* conn)
    {
        
    }

    void HttpServer::onConnError(Connection* conn)
    {
        
    }
}
