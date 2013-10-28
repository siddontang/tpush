#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <stdint.h>

namespace tpush
{
    class Config
    {
    public:
        static int parseFile(const char* file);    

        static int TcpAcceptLoopNum;
        static int TcpConnLoopNum;
        static int TcpMaxConnections;

        static int TcpConnCheckRepeat;
        static int TcpConnCheckStep;
        static int TcpConnTimeout;

        static int PushLoopNum;
        static int MaxChannelConnectionNum;
        static int MaxChannelMessageReserveNum;
        static int MaxChannelMessageReserveTime;
        static int MaxChannelIdSize;
        static int MaxChannelMessageSize;
        static int PushLoopCheckRepeat;
        static int PushLoopCheckStep;
    
        static std::string HttpListenIp;
        static uint16_t HttpListenPort;
        static std::string HttpChannelKey;
        static std::string HttpSubscribeUrl;
        static std::string HttpUnsubscribeUrl;
        static std::string HttpPublishUrl;
    
    };    
}

#endif
