#include "httppushserver.h"

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
    HttpPushServer::HttpPushServer(PushServer* server)
        : m_server(server)
    {
        m_httpd = new HttpServer(server->getServer());
    }

    HttpPushServer::~HttpPushServer()
    {
        delete m_httpd;    
    }

    void HttpPushServer::start()
    {
        PushConnection::setPushFunc(std::tr1::bind(&HttpPushServer::onHttpPush, this, _1, _2), PushConnection::HttpType);
        PushConnection::setPushFunc(std::tr1::bind(&HttpPushServer::onWsPush, this, _1, _2), PushConnection::WsType);

        m_httpd->setHttpCallback(Config::HttpSubscribeUrl, std::tr1::bind(&HttpPushServer::onSubscribe, this, _1, _2));
        m_httpd->setHttpCallback(Config::HttpUnsubscribeUrl, std::tr1::bind(&HttpPushServer::onUnsubscribe, this, _1, _2));
        m_httpd->setHttpCallback(Config::HttpPublishUrl, std::tr1::bind(&HttpPushServer::onPublish, this, _1, _2));

        m_httpd->setHttpCallback(Config::WsPushUrl, std::tr1::bind(&HttpPushServer::onWsEvent, this, _1, _2, _3));

        m_httpd->listen(Address(Config::HttpListenIp, Config::HttpListenPort));       
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

    void HttpPushServer::onHttpPush(const ContextPtr_t& context, const string& message)
    {
        HttpConnectionPtr_t conn = std::tr1::static_pointer_cast<HttpConnection>(context);
        sendResponse(conn, 200, message);    
    }
    
    void HttpPushServer::onWsPush(const ContextPtr_t& context, const string& message)
    {
        WsConnectionPtr_t conn = std::tr1::static_pointer_cast<WsConnection>(context);
        conn->send(message);    
    } 

    int HttpPushServer::checkMethod(const HttpConnectionPtr_t& conn, const HttpRequest& request, int method)
    {
        if(request.method != (http_method)method)
        {
            sendResponse(conn, 405);
            return -1;    
        }
        return 0; 
    }

    int HttpPushServer::checkChannel(const HttpConnectionPtr_t& conn, const HttpRequest& request, vector<string>& ids)
    {
        map<string, string>::const_iterator iter = request.params.find(Config::HttpChannelKey);
        if(iter == request.params.end())
        {
            sendResponse(conn, 403);
            return -1;        
        }
        
        ids = StringUtil::split(iter->second, ",");

        if(ids.empty())
        {
            sendResponse(conn, 403);
            return -1;    
        }

        return 0;
    }

    void HttpPushServer::onSubscribe(const HttpConnectionPtr_t& conn, const HttpRequest& request)
    {
        if(checkMethod(conn, request, (int)HTTP_GET) != 0)
        {
            return;    
        }

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
        if(checkMethod(conn, request, (int)HTTP_GET) != 0)
        {
            return;    
        }
        
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
        if(checkMethod(conn, request, (int)HTTP_POST) != 0)
        {
            return;    
        }

        vector<string> ids;
        if(checkChannel(conn, request, ids) != 0)
        {
            return;    
        }   

        if(!request.body.empty())
        {
            m_server->dispatchChannels(ids, std::tr1::bind(&PushLoop::publishs, _1, _2, request.body));
        }

        sendResponse(conn, 200);
    }

    void HttpPushServer::onWsEvent(const WsConnectionPtr_t& conn, WsEvent event, const string& message)
    {
        switch(event)
        {
            case Ws_MessageEvent:
                break;
            default:
                break;    
        }
    } 
}
