#include "tcpserver.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#include "ioloopthreadpool.h"
#include "ioloop.h"
#include "sockutil.h"

#include "acceptor.h"
#include "signaler.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tpush
{
    TcpServer::TcpServer(int acceptLoopNum, int connLoopNum, int maxConnections)
    {
        assert(maxConnections >= 0);
        m_mainLoop = new IOLoop();

        m_acceptor = new Acceptor(acceptLoopNum);

        m_connLoops = new IOLoopThreadPool(connLoopNum);
   
        m_signaler = new Signaler(m_mainLoop);

        int dummyFd = SockUtil::createDummyFile(); 
        close(dummyFd);
    
        maxConnections += dummyFd;

        int maxFileNum = getdtablesize();
        maxConnections = (maxConnections > maxFileNum || maxConnections == 0) ? maxFileNum : maxConnections;
    
        m_connections.resize(maxConnections);
    }
    
    TcpServer::~TcpServer()
    {
        delete m_connLoops;
        delete m_acceptor;

        delete m_mainLoop;
    }

    void TcpServer::onNewConnection(int sockFd)
    {
    }

    int TcpServer::listen(const Address& addr)
    {
        m_acceptor->listen(addr, std::tr1::bind(&TcpServer::onNewConnection, this, _1));
        return 0;
    } 
   
    void TcpServer::setConnLoopIOInterval(int milliseconds)
    {
        vector<IOLoop*> loops = m_connLoops->getLoops();
        for(size_t i = 0; loops.size(); i++)
        {
            loops[i]->setIOInterval(milliseconds);    
        }
    }
    
    void TcpServer::addSignal(int signum, const SignalFunc_t& func)
    {
        m_signaler->add(signum, func);   
    } 
    
    void TcpServer::start()
    {
        m_connLoops->start();
        m_acceptor->start();

        m_mainLoop->start();
    }  

    void TcpServer::stop()
    {
        m_signaler->clear();
    
        m_acceptor->stop();
    
        m_connLoops->stop();
    
        m_mainLoop->stop();
    }
}


