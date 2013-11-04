#ifndef _WSPUSHREQUEST_H_
#define _WSPUSHREQUEST_H_

#include <vector>
#include <string>

#include "pushenums.h"

namespace tpush
{
    class WsPushRequest
    {
    public:
        WsPushAction action;
        std::vector<std::string> ids;
        std::string message;
    };    
}

#endif
