#ifndef _SOCKUTIL_H_
#define _SOCKUTIL_H_

namespace tpush
{
    class Address;
    class SockUtil
    {
    public:
        static int bindAndListen(const Address& addr);

        static int setNoDelay(int sockFd, bool on);
        static int setCloseOnExec(int sockFd, bool on);
        static int setNonBlocking(int sockFd, bool on);
        static int setNonBlockingAndCloseOnExec(int sockFd);

        static int setReuseable(int sockFd, bool on);
        static int setKeepAlive(int scokFd, bool on);
    };

}

#endif
