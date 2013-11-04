#ifndef _PUSHENUMS_H_
#define _PUSHENUMS_H_

namespace tpush
{

    enum WsPushAction
    {
        Ws_Subscribe = 1,
        Ws_Unsubscribe,
        Ws_Publish,    
    };
    
    enum WsDataProtoType
    {
        Ws_DelimProto = 0,    
        Ws_NoneProto,
    };
}

#endif
