#include "misc.h"

#include <string.h>

namespace tpush
{
#ifdef LINUX
    __thread char errorBuf[64];
#endif

    const char* errorMsg(int errorNum)
    {
#ifdef LINUX
        return strerror_r(errorNum, errorBuf, sizeof(errorBuf));
#else
        return strerror(errorNum);
#endif
    }
    
}
