#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>

namespace tpush
{
    class Config
    {
    public:
        static int parseFile(const char* file);    
    };    
}

#endif
