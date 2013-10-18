#ifndef _HTTPCONNECTION_H_
#define _HTTPCONNECTION_H_

extern "C"
{
#include "http_parser.h"
}

#include "nocopyable.h"

namespace tpush
{
    class Connection;
    class HttpServer;

    class HttpConnection : public nocopyable
    {
    public:
        HttpConnection(Connection* conn, HttpServer* server);
        ~HttpConnection();

        struct http_parser* getParser() { return &m_parser; }
        void resetParser();

        static void initParserSettings(struct http_parser_settings* settings);

    private:
        static int onMessageBegin(struct http_parser*);
        static int onUrl(struct http_parser*, const char*, size_t);
        static int onStatusComplete(struct http_parser*);
        static int onHeaderField(struct http_parser*, const char*, size_t);
        static int onHeaderValue(struct http_parser*, const char*, size_t);
        static int onHeadersComplete(struct http_parser*);
        static int onBody(struct http_parser*, const char*, size_t);
        static int onMessageComplete(struct http_parser*);
    
        int handleMessageBegin();
        int handleUrl(const char*, size_t);
        int handleStatusComplete();
        int handleHeaderField(const char*, size_t);
        int handleHeaderValue(const char*, size_t);
        int handleHeadersComplete();
        int handleBody(const char*, size_t);
        int handleMessageComplete();

    private:
        Connection* m_conn;
        HttpServer* m_server;
        struct http_parser m_parser;  
    };
    
}

#endif
