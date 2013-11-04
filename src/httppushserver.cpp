#include "httppushserver.h"

#include <stdlib.h>

#include "httprequest.h"
#include "httpserver.h"
#include "httpresponse.h"
#include "httpconnection.h"
#include "wsconnection.h"
#include "pushconnection.h"
#include "pushloop.h"
#include "log.h"
#include "misc.h"
#include "stringutil.h"
#include "address.h"
#include "pushserver.h"
#include "config.h"

using namespace tnet;
using namespace std;
using namespace std::tr1::placeholders;

namespace tpush
{
    HttpPushServer::HttpPushServer(PushServer* server, HttpServer* httpd)
        : m_server(server)
        , m_httpd(httpd)
    {
    }

    HttpPushServer::~HttpPushServer()
    {
    }

    void HttpPushServer::start()
    {
        PushConnection::setPushFunc(std::tr1::bind(&HttpPushServer::push, this, _1, _2), PushConnection::HttpType);

        m_httpd->setHttpCallback(Config::HttpPushUrl, std::tr1::bind(&HttpPushServer::onRequest, this, _1, _2));
    }

    void HttpPushServer::stop()
    {
        
    }

    void HttpPushServer::sendResponse(const HttpConnectionPtr_t& conn, int statusCode, const string& message)
    {
        HttpResponse resp;
        resp.statusCode = statusCode;
        resp.body = message;
       
        resp.setContentType("text/html");
        resp.enableDate();
        resp.setKeepAlive(true);
        
        conn->send(resp);    
    }

    void HttpPushServer::push(const ContextPtr_t& context, const string& message)
    {
        HttpConnectionPtr_t conn = std::tr1::static_pointer_cast<HttpConnection>(context);
        sendResponse(conn, 200, message);    
    }

    int HttpPushServer::checkChannel(const HttpConnectionPtr_t& conn, const HttpRequest& request, vector<string>& ids)
    {
        map<string, string>::const_iterator iter = request.params.find(Config::HttpChannelKey);
        if(iter == request.params.end())
        {
            sendResponse(conn, 403);
            return -1;        
        }
       
        ids = StringUtil::split(iter->second, Config::ChannelDelim);

        if(ids.empty())
        {
            sendResponse(conn, 403);
            return -1;    
        }

        return 0;
    }

    void HttpPushServer::onRequest(const HttpConnectionPtr_t& conn, const HttpRequest& request)
    { 
        switch(request.method)
        {
            case HTTP_GET:
                onSubscribe(conn, request);
                break;
            case HTTP_POST:
                onPublish(conn, request);
                break;
            case HTTP_DELETE:
                onUnsubscribe(conn, request);
                break;
            default:
                sendResponse(conn, 405);
                break;    
        }    
    }

    void HttpPushServer::onSubscribe(const HttpConnectionPtr_t& conn, const HttpRequest& request)
    {
        vector<string> ids;
        if(checkChannel(conn, request, ids) != 0)
        {
            return;    
        }
         
        PushConnection c(conn, conn->getSockFd(), PushConnection::HttpType);

        m_server->dispatchChannels(ids, std::tr1::bind(&PushLoop::subscribes, _1, _2, c));
    }

    void HttpPushServer::onUnsubscribe(const HttpConnectionPtr_t& conn, const HttpRequest& request)
    {
        vector<string> ids;
        if(checkChannel(conn, request, ids) != 0)
        {
            return;    
        }
         
        PushConnection c(conn, conn->getSockFd(), PushConnection::HttpType);

        m_server->dispatchChannels(ids, std::tr1::bind(&PushLoop::unsubscribes, _1, _2, c)); 
    }

    void HttpPushServer::onPublish(const HttpConnectionPtr_t& conn, const HttpRequest& request)
    {
        vector<string> ids;
        if(checkChannel(conn, request, ids) != 0)
        {
            return;    
        }
         
        if(!request.body.empty())
        {
            m_server->dispatchChannels(ids, std::tr1::bind(&PushLoop::publishs, _1, _2, request.body));
        }
        else
        {
            sendResponse(conn, 400);
            return;    
        }

        sendResponse(conn, 200);
    }
}
