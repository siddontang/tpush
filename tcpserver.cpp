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
        m_mainLoop = new IOLoop();

        m_acceptLoops = new IOLoopThreadPool(acceptLoopNum);
        m_connLoops = new IOLoopThreadPool(connLoopNum);
    
        m_acceptLoops->start();
        m_connLoops->start();
    }
    
    TcpServer::~TcpServer()
    {
        delete m_acceptLoops;
        delete m_connLoops;

        delete m_mainLoop;
    }
    

    int TcpServer::listen(const Address& addr)
    {
        int sockFd = SockUtil::bindAndListen(addr);
        
        
        
        return 0;    
    } 
    
    void TcpServer::addSignal(int signum, const SignalFunc_t& func)
    {
        m_mainLoop->addSignal(signum, func);   
    } 
    
    void TcpServer::start()
    {
        m_mainLoop->start();
    }  

    void TcpServer::stop()
    {
        m_acceptLoops->stop();
        m_connLoops->stop();
    
        m_mainLoop->stop();
    }
}


