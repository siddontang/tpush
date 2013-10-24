#include "config.h"

extern "C"
{
#include "ini.h"    
}

#define MATCH(s, n) strcmp(secion, s) == 0 && strcmp(name, n) == 0

namespace tpush
{
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
