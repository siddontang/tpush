#include "tcpserver.h"

#include <errno.h>
#include <assert.h>
#include <sys/socket.h>

#include "ioloopthreadpool.h"
#include "ioloop.h"
#include "sockutil.h"

using namespace std;

namespace tpush
{
    TcpServer::TcpServer(int acceptLoopNum, int connLoopNum)
    {
        m_acceptLoops = new IOLoopThreadPool(acceptLoopNum);
        m_connLoops = new IOLoopThreadPool(connLoopNum);
    
        m_acceptLoops->start();
        m_connLoops->start();
    }
    
    TcpServer::~TcpServer()
    {
        delete m_acceptLoops;
        delete m_connLoops;
    }
    

    int TcpServer::listen(const Address& addr)
    {
        int sockFd = socket(PF_INET, SOCK_STREAM, 0);
        assert(sockFd > 0);
        
        

        struct sockaddr_in sockAddr = addr.sockAddr();       
        if(bind(sockFd, (struct sockaddr*)&sockAddr, sizeof(sockAddr) < 0))
        {
            int err = errno;
            close(sockFd);
            return err;    
        }

        if(::listen(sockFd, SOMAXCONN))
        {
            int err = errno;
            close(sockFd);
            return err;    
        }

        return 0;    
    }    

    void TcpServer::stop()
    {
        m_acceptLoops->stop();
        m_connLoops->stop();
    }
}


