#include "httpconnection.h"

#include <string>
#include <stdlib.h>
#include <string.h>

#include "httpserver.h"
#include "connection.h"
#include "log.h"

using namespace std;

namespace tpush
{

    HttpConnection::HttpConnection(Connection* conn, HttpServer* server)
        : m_conn(conn)
        , m_server(server)
    {
        resetParser();
    }

    HttpConnection::~HttpConnection()
    {
       LOG_INFO("HttpConnection destoryed"); 
    }

    void HttpConnection::resetParser()
    {
        memset(&m_parser, 0, sizeof(m_parser));
        http_parser_init(&m_parser, HTTP_REQUEST); 
    }

    void HttpConnection::initParserSettings(struct http_parser_settings* settings)
    {
        settings->on_message_begin = &HttpConnection::onMessageBegin;
        settings->on_url = &HttpConnection::onUrl;
        settings->on_status_complete = &HttpConnection::onStatusComplete;
        settings->on_header_field = &HttpConnection::onHeaderField;
        settings->on_header_value = &HttpConnection::onHeaderValue;
        settings->on_headers_complete = &HttpConnection::onHeadersComplete;
        settings->on_body = &HttpConnection::onBody;
        settings->on_message_complete = &HttpConnection::onMessageComplete;    
    }    

    int HttpConnection::onMessageBegin(struct http_parser* parser)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        
        return conn->handleMessageBegin();    
    }

    int HttpConnection::onUrl(struct http_parser* parser, const char* at, size_t length)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleUrl(at, length);
    }

    int HttpConnection::onStatusComplete(struct http_parser* parser)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleStatusComplete();
    }

    int HttpConnection::onHeaderField(struct http_parser* parser, const char* at, size_t length)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleHeaderField(at, length);
    }

    int HttpConnection::onHeaderValue(struct http_parser* parser, const char* at, size_t length)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleHeaderValue(at, length);
    }

    int HttpConnection::onHeadersComplete(struct http_parser* parser)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleHeadersComplete();
    }

    int HttpConnection::onBody(struct http_parser* parser, const char* at, size_t length)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleBody(at, length);
    }

    int HttpConnection::onMessageComplete(struct http_parser* parser)
    {
        HttpConnection* conn = (HttpConnection*)parser->data;
        return conn->handleMessageComplete();
    }

     
    int HttpConnection::handleMessageBegin()
    {
        return 0;    
    }
        
    int HttpConnection::handleUrl(const char* at, size_t length)
    {
        return 0;
    }
     
    int HttpConnection::handleStatusComplete()
    {
        return 0;
    }
        
    int HttpConnection::handleHeaderField(const char* at, size_t length)
    {
        return 0;
    }
        
    int HttpConnection::handleHeaderValue(const char* at, size_t length)
    {
        return 0;
    }
        
    int HttpConnection::handleHeadersComplete()
    {
        return 0;
    }
        
    int HttpConnection::handleBody(const char* at, size_t length)
    {
        return 0;
    }
        
    int HttpConnection::handleMessageComplete()
    {
        return 0;
    }
}
