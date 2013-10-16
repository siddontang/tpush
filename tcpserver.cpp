#include "tcpserver.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/socket.h>

#include "ioloopthreadpool.h"
#include "ioloop.h"
#include "sockutil.h"

#include "acceptor.h"
#include "signaler.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tpush
{
    TcpServer::TcpServer(int acceptLoopNum, int connLoopNum)
    {
        m_mainLoop = new IOLoop();

        m_acceptor = new Acceptor(acceptLoopNum);

        m_connLoops = new IOLoopThreadPool(connLoopNum);
   
        m_signaler = new Signaler(m_mainLoop);


    }
    
    TcpServer::~TcpServer()
    {
        delete m_connLoops;
        delete m_acceptor;

        delete m_mainLoop;
    }

    void TcpServer::onNewConnection(int sockFd, const Address& addr)
    {
    }

    int TcpServer::listen(const Address& addr)
    {
        m_acceptor->listen(addr, std::tr1::bind(&TcpServer::onNewConnection, this, _1, _2));
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


