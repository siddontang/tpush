#include "sockutil.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

namespace tpush
{
    int SockUtil::setNoDelay(int sockFd, bool on)
    {
        int opt = on ? 1 : 0;
        
        return setsockopt(sockFd, IPPROTO_TCP, 
                        TCP_NODELAY, &opt, 
                        static_cast<socklen_t>(sizeof(opt)));    
    }
   
    static int setFdFlag(int fd, int flag, bool on)
    {
        int flags = fcntl(fd, F_GETFL, 0);
        if(flags < 0)
            return flags;

        flags = on ? (flags | flag) : (flags & ~flag);
        return fcntl(fd, F_SETFL, flags);
    }
    
    int SockUtil::setNonBlocking(int sockFd, bool on)
    {
        return setFdFlag(sockFd, O_NONBLOCK, on);
    }

    int SockUtil::setCloseOnExec(int sockFd, bool on)
    {
        return setFdFlag(sockFd, O_CLOEXEC, on);    
    }

    int SockUtil::setReuseable(int sockFd, bool on)
    {
        int opt = on ? 1 : 0;
        return setsockopt(sockFd, SOL_SOCKET, 
                        SO_REUSEADDR, &opt,
                        static_cast<socklen_t>(sizeof(opt)));    
    }

    int SockUtil::setKeepAlive(int sockFd, bool on)
    {
        int opt = on ? 1 : 0;
        return setsockopt(sockFd, SOL_SOCKET, 
                        SO_KEEPALIVE, &opt,
                        static_cast<socklen_t>(sizeof(opt)));    
    }
}
