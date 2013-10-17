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
#include "misc.h"
#include "log.h"
#include "acceptor.h"
#include "signaler.h"

#include "connection.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tpush
{
    void dummyConnReadCallback(Connection*, const char*, int)
    {
        
    }

    void dummyConnCallback(Connection*)
    {
        
    }

    TcpServer::TcpServer(int acceptLoopNum, int connLoopNum, int maxConnections)
    {
        assert(maxConnections >= 0);
        m_mainLoop = new IOLoop();

        m_acceptor = new Acceptor(acceptLoopNum);

        m_connLoops = new IOLoopThreadPool(connLoopNum);
   
        m_signaler = new Signaler(m_mainLoop);

        m_maxConnections = maxConnections;
        m_curConnections = 0;
        
        int sysMaxFileNum = getdtablesize();
    
        m_connections.resize(sysMaxFileNum);
    
        m_connReadFunc = std::tr1::bind(&dummyConnReadCallback, _1, _2, _3);
        m_connWriteOverFunc = std::tr1::bind(&dummyConnCallback, _1);
        m_connErrorFunc = std::tr1::bind(&dummyConnCallback, _1);
        m_connCloseFunc = std::tr1::bind(&dummyConnCallback, _1);
    }
    
    TcpServer::~TcpServer()
    {
        delete m_connLoops;
        delete m_acceptor;

        delete m_mainLoop;
    
        clearContainer(m_connections);
    }

    void TcpServer::onNewConnection(int sockFd)
    {
        IOLoop* loop = m_connLoops->getHashLoop(sockFd);

        loop->runTask(std::tr1::bind(&TcpServer::newConnectionInLoop, this, loop, sockFd));
    }

    static void onConnectionEvent(TcpServer* server, Connection* conn, Connection::Event event, const char* buffer, int bufferLen)
    {
        switch(event)
        {
            case Connection::ReadEvent:
                server->onConnRead(conn, buffer, bufferLen);
                break;
            case Connection::WriteOverEvent:
                server->onConnWriteOver(conn);
                break;
            case Connection::CloseEvent:
                server->onConnClose(conn);
                break;
            case Connection::ErrorEvent:
                server->onConnError(conn);
                break;    
        }    
    }

    void TcpServer::onConnRead(Connection* conn, const char* buffer, int bufferLen)
    {
        m_connReadFunc(conn, buffer, bufferLen);    
    }

    void TcpServer::onConnWriteOver(Connection* conn)
    {
        m_connWriteOverFunc(conn);
    }

    void TcpServer::onConnClose(Connection* conn)
    {
        m_connCloseFunc(conn);
        deleteConnection(conn);
    }

    void TcpServer::onConnError(Connection* conn)
    {
        m_connErrorFunc(conn);
        deleteConnection(conn);
    }

    void TcpServer::newConnectionInLoop(IOLoop* loop, int sockFd)
    {
        Connection* conn = m_connections[sockFd];
        if(!conn)
        {
            conn = new Connection(loop, sockFd);
            m_connections[sockFd] = conn;    
        }    

        conn->setCallback(std::tr1::bind(&onConnectionEvent, this, _1, _2, _3, _4));

        conn->onEstablished();
    }

    void TcpServer::deleteConnection(Connection* conn)
    {
        IOLoop* loop = conn->getLoop();

        if(!loop->inLoopThread())
        {
            loop->addTask(std::tr1::bind(&TcpServer::deleteConnectionInLoop, this, conn));    
        }
        else
        {
            deleteConnectionInLoop(conn);   
        }
    }

    void TcpServer::deleteConnectionInLoop(Connection* conn)
    {
        IOLoop* loop = conn->getLoop();

        assert(loop->inThreadLoop());
        (void)loop;

        int sockFd = conn->getSockFd();
        delete conn;
        m_connections[sockFd] = NULL;
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


