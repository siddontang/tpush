#include "httppushserver.h"

#include "httprequest.h"
#include "httpserver.h"
#include "httpresponse.h"
#include "connection.h"
#include "pushconnection.h"
#include "pushloop.h"
#include "log.h"
#include "misc.h"
#include "stringutil.h"
#include "httpurl.h"
#include "address.h"
#include "pushserver.h"
#include "httpurl.h"
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
    
        m_httpd->setRequestCallback(std::tr1::bind(&HttpPushServer::onRequest, this, _1, _2));
    }

    HttpPushServer::~HttpPushServer()
    {
        delete m_httpd;    
    }

    void HttpPushServer::start()
    {
        PushConnection::setPushFunc(std::tr1::bind(&HttpPushServer::onPush, this, _1, _2), PushConnection::HttpType);

        m_httpd->listen(Address(Config::HttpListenIp, Config::HttpListenPort));       
    }

    void HttpPushServer::stop()
    {
        
    }

    void HttpPushServer::sendResponse(const ConnectionPtr_t& conn, int statusCode, const string& message)
    {
        HttpResponse resp;
        resp.statusCode = statusCode;
        resp.body = message;
       
        resp.setContentType("text/html");
        resp.enableDate();
        resp.setKeepAlive(true);
        
        conn->send(resp.dump());    
    }

    void HttpPushServer::onPush(const ConnectionPtr_t& conn, const string& message)
    {
        sendResponse(conn, 200, message);    
    } 

    void HttpPushServer::onRequest(const HttpRequest& request, const ConnectionPtr_t& conn)
    {
        const string& path = request.url;         
        if(path.find(Config::HttpSubscribeUrl) != string::npos)
        {
            onSubscribe(request, conn);
        }
        else if(path.find(Config::HttpPublishUrl) != string::npos)
        {
            onPublish(request, conn);
        }
        else if(path.find(Config::HttpUnsubscribeUrl) != string::npos)
        {
            onUnsubscribe(request, conn);
        }
        else
        {
            sendResponse(conn, 404);
        }
    }

    int HttpPushServer::checkMethod(const HttpRequest& request, int method, const ConnectionPtr_t& conn)
    {
        if(request.method != (http_method)method)
        {
            sendResponse(conn, 405);
            return -1;    
        }
        return 0; 
    }

    int HttpPushServer::checkChannel(const HttpRequest& request, const ConnectionPtr_t& conn, vector<string>& ids)
    {
        HttpUrl url = HttpUrl(request.url);
        map<string, string>::iterator iter = url.params.find(Config::HttpChannelKey);
        if(iter == url.params.end())
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

    void HttpPushServer::onSubscribe(const HttpRequest& request, const ConnectionPtr_t& conn)
    {
        if(checkMethod(request, (int)HTTP_GET, conn) != 0)
        {
            return;    
        }

        vector<string> ids;
        if(checkChannel(request, conn, ids) != 0)
        {
            return;    
        }   
        
        PushConnection c(conn, conn->getSockFd(), PushConnection::HttpType);

        m_server->dispatchChannels(ids, std::tr1::bind(&PushLoop::subscribes, _1, _2, c));
    }

    void HttpPushServer::onUnsubscribe(const HttpRequest& request, const ConnectionPtr_t& conn)
    {
        if(checkMethod(request, (int)HTTP_GET, conn) != 0)
        {
            return;    
        }
        
        vector<string> ids;
        if(checkChannel(request, conn, ids) != 0)
        {
            return;    
        }   
        
        PushConnection c(conn, conn->getSockFd(), PushConnection::HttpType);

        m_server->dispatchChannels(ids, std::tr1::bind(&PushLoop::unsubscribes, _1, _2, c)); 
    }

    void HttpPushServer::onPublish(const HttpRequest& request, const ConnectionPtr_t& conn)
    {
        if(checkMethod(request, (int)HTTP_POST, conn) != 0)
        {
            return;    
        }

        vector<string> ids;
        if(checkChannel(request, conn, ids) != 0)
        {
            return;    
        }   

        if(!request.body.empty())
        {
            m_server->dispatchChannels(ids, std::tr1::bind(&PushLoop::publishs, _1, _2, request.body));
        }

        sendResponse(conn, 200);
    }

    
}
