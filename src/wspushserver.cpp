#include "wspushserver.h"

#include "pushserver.h"
#include "stringutil.h"
#include "httpserver.h"
#include "config.h"
#include "pushconnection.h"
#include "wsconnection.h"
#include "log.h"
#include "wspushrequest.h"
#include "pushloop.h"

using namespace tnet;
using namespace std;
using namespace std::tr1::placeholders;

namespace tpush
{
    extern int wsDataDelimProto(const string&, WsPushRequest&);

    WsPushServer::WsPushServer(PushServer* server, HttpServer* httpd)
        : m_server(server)
        , m_httpd(httpd)
    {
        
    }

    WsPushServer::~WsPushServer()
    {
        
    }

    void WsPushServer::start()
    {
        regDefaultProtoHandler();

        PushConnection::setPushFunc(std::tr1::bind(&WsPushServer::push, this, _1, _2), PushConnection::WsType);
    
        m_httpd->setWsCallback(Config::WsPushUrl, std::tr1::bind(&WsPushServer::onEvent, this, _1, _2, _3));
    }

    void WsPushServer::stop()
    {
        
    }

    void WsPushServer::regDefaultProtoHandler()
    {
        setDataProtoHandler(Ws_DelimProto, std::tr1::bind(&wsDataDelimProto, _1, _2));
    }

    void WsPushServer::push(const ContextPtr_t& context, const string& message)
    {
        WsConnectionPtr_t conn = std::tr1::static_pointer_cast<WsConnection>(context);
        conn->send(message);    
    }

    void WsPushServer::onEvent(const WsConnectionPtr_t& conn, WsEvent event, const string& message)
    {
        switch(event)
        {
            case Ws_MessageEvent:
                onData(conn, message);
                break;
            default:
                break;    
        }
    }

    void WsPushServer::setDataProtoHandler(WsDataProtoType proto, const ProtoHandler_t& handler)
    {
        if(int(proto) >= 0 && int(proto) < Ws_NoneProto)
        {
            m_handlers[proto] = handler;     
        }
    }

    void WsPushServer::onData(const WsConnectionPtr_t& conn, const string& data)
    {
        WsDataProtoType proto = Config::WsDataProto;
       
        WsPushRequest request;
        if((m_handlers[proto])(data, request) != 0)
        {
            conn->shutDown();    
            return;
        } 

        switch(request.action)
        {
            case(Ws_Subscribe):
                {
                    PushConnection c(conn, conn->getSockFd(), PushConnection::WsType); 
                    m_server->dispatchChannels(request.ids, std::tr1::bind(&PushLoop::subscribes, _1, _2, c));
                }
                break;
            case(Ws_Unsubscribe):
                {
                    PushConnection c(conn, conn->getSockFd(), PushConnection::WsType);
                    m_server->dispatchChannels(request.ids, std::tr1::bind(&PushLoop::unsubscribes, _1, _2, c));
                }
                break;
            case(Ws_Publish):
                {
                    m_server->dispatchChannels(request.ids, std::tr1::bind(&PushLoop::publishs, _1, _2, request.message));
                }
                break;    
        }
    }
}
