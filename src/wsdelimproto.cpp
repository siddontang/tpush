#include <vector>
#include <string>
#include <stdlib.h>

#include "wspushrequest.h"
#include "pushenums.h"
#include "config.h"
#include "stringutil.h"

using namespace std;
using namespace tnet;

namespace tpush
{
    int wsDataDelimProto(const string& data, WsPushRequest& request)
    {
        vector<string> tokens = StringUtil::split(data, Config::WsDataDelim, 3);
        if(tokens.size() < 2)
        {
            return -1;    
        }      

        WsPushAction action = (WsPushAction)atoi(tokens[0].c_str());
        if(action < Ws_Subscribe || action > Ws_Publish)
        {
            return -1;    
        }

        request.action = action;
        request.ids = StringUtil::split(tokens[1], Config::ChannelDelim);
        if(request.ids.empty())
        {
            return -1;    
        }

        if(action == Ws_Publish && tokens.size() == 3 && !tokens[2].empty())
        {
            request.message = tokens[2];
        }
        else
        {
            return -1;    
        }

        return 0;
    }    
}
