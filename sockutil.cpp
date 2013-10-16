#include "sockutil.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <assert.h>

#include "address.h"

namespace tpush
{
    int SockUtil::bindAndListen(const Address& addr)
    {
        int err = 0;
        
        int fd = socket(PF_INET, SOCK_STREAM, 0);
        assert(fd > 0);
        
        do
        {
            struct sockaddr_in sockAddr = addr.sockAddr();
            
            if(bind(fd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
            {
                err = errno;
                perror("bind address error");
                break;    
            }

            if(listen(fd, SOMAXCONN) < 0)
            {
                err = errno;
                perror("listen address error");
                break;     
            }

            SockUtil::setNonBlockingAndCloseOnExec(fd);
            SockUtil::setReuseable(fd, true);

            return fd;

        }while(0);

        close(fd);
        return err;
    }

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

    int SockUtil::setNonBlockingAndCloseOnExec(int sockFd)
    {
        return setFdFlag(sockFd, O_NONBLOCK | O_CLOEXEC, true);    
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
