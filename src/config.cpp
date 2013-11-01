#include "config.h"

extern "C"
{
#include "ini.h"    
}

#define MATCH(s, n) strcmp(secion, s) == 0 && strcmp(name, n) == 0

using namespace std;

namespace tpush
{
    int Config::TcpAcceptLoopNum = 2;
    int Config::TcpConnLoopNum = 4;
    int Config::TcpMaxConnections = 10000;

    int Config::TcpConnCheckRepeat = 10;
    int Config::TcpConnCheckStep = 2000;
    int Config::TcpConnTimeout = 60;

    int Config::PushLoopNum = 4;
    int Config::MaxChannelConnectionNum = 10000;
    int Config::MaxChannelMessageReserveNum = 10;
    int Config::MaxChannelMessageReserveTime = 60;
    int Config::MaxChannelIdSize = 64;
    int Config::MaxChannelMessageSize = 128;
    int Config::PushLoopCheckRepeat = 30;
    int Config::PushLoopCheckStep = 2000;

    string Config::HttpListenIp = "0.0.0.0";
    uint16_t Config::HttpListenPort = 11181;
    string Config::HttpChannelKey = "id";
    string Config::HttpSubscribeUrl = "/push/subscribe";
    string Config::HttpUnsubscribeUrl = "/push/unsubscribe";
    string Config::HttpPublishUrl = "/push/publish";
    string Config::WsPushUrl = "/push/ws";

    static int parseHandler(void* cfg, const char* section, const char* name, const char* value)
    {
        return 1;    
    }

    int Config::parseFile(const char* file)
    {
        if(ini_parse(file, &parseHandler, NULL) < 0)
        {
            return -1;    
        }

        return 0;
    }

}
