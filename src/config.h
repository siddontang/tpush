#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>

namespace tpush
{
    class Config
    {
    public:
        static int parseFile(const char* file);    

        static int PushLoopNum;
        static int MaxChannelConnectionNum;
        static int MaxChannelMessageReserveNum;
        static int MaxChannelMessageReserveTime;
        static int MaxChannelIdSize;
        static int MaxChannelMessageSize;
        static int PushLoopCheckRepeat;
    };    
}

#endif
