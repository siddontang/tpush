#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <string>
#include <vector>
#include <tr1/functional>

#include "nocopyable.h"
#include "address.h"
#include "threadutil.h"
#include "connection.h"

namespace tpush 
{   
    class IOLoop;
    class IOLoopThreadPool;
    
    class Signaler;
    class Acceptor;
    class Connection;

    class TcpServer : public nocopyable
    {
    public:
        TcpServer(int acceptLoopNum, int connLoopNum, int maxConnections);
        ~TcpServer();
      
        typedef std::tr1::function<void (Connection*, Connection::Event, const char*, int)> ConnEventCallback_t;
        int listen(const Address& addr, const ConnEventCallback_t& func);
       
        void setConnLoopIOInterval(int milliseconds);
        
        typedef std::tr1::function<void (int)> SignalFunc_t;
        void addSignal(int signum, const SignalFunc_t& func);

        void start();
        void stop();

    private:
        void onNewConnection(int sockFd, const ConnEventCallback_t& func);

        void newConnectionInLoop(IOLoop* loop, int sockFd, const ConnEventCallback_t& func);

        void deleteConnection(Connection* conn);
        void deleteConnectionInLoop(Connection* conn);

        void onConnEvent(const ConnEventCallback_t& FUNC, Connection* conn, Connection::Event event, const char* buffer, int count);

    private:
        Acceptor* m_acceptor;

        //threads for handle connections
        IOLoopThreadPool* m_connLoops;
    
        IOLoop* m_mainLoop;

        Signaler* m_signaler;

        std::vector<Connection*> m_connections;
    
        int m_maxConnections;
        volatile int m_curConnections;
    
        SpinLock m_lock;
    };
        
}

#endif

