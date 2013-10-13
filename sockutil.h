#ifndef _SOCKUTIL_H_
#define _SOCKUTIL_H_

namespace tpush
{
    class SockUtil
    {
    public:
        static int setNoDelay(int sockFd, bool on);
        static int setCloseOnExec(int sockFd, bool on);
        static int setNonBlocking(int sockFd, bool on);
        static int setReuseable(int sockFd, bool on);
        static int setKeepAlive(int scokFd, bool on);
    };

}

#endif
