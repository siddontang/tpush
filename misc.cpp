#include "misc.h"

#include <string.h>

namespace tpush
{
    __thread char errorBuf[64];

    const char* errorMsg(int errorNum)
    {
        return strerror_r(errorNum, errorBuf, sizeof(errorBuf));
    }
    
}
