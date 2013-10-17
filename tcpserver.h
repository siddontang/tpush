#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <string>
#include <vector>
#include <tr1/functional>

#include "nocopyable.h"
#include "address.h"
#include "threadutil.h"

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
        TcpServer(int acceptLoopNum, int connLoopNum, int maxConnections = 0);
        ~TcpServer();

        int listen(const Address& addr);
       
        void setConnLoopIOInterval(int milliseconds);
        
        typedef std::tr1::function<void (int)> SignalFunc_t;
        void addSignal(int signum, const SignalFunc_t& func);

        void start();
        void stop();

        void onConnRead(Connection* conn, const char* buffer, int bufferLen);
        void onConnWriteOver(Connection* conn);
        void onConnError(Connection* conn);
        void onConnClose(Connection* conn);

        typedef std::tr1::function<void (Connection*, const char*, int)> ConnReadCallback_t;
        void setConnReadCallback(const ConnReadCallback_t& func) { m_connReadFunc = func; }

        typedef std::tr1::function<void (Connection*)> ConnCallback_t;
        void setConnWriteOverCallback(const ConnCallback_t& func) { m_connWriteOverFunc = func; }
        void setConnErrorCallback(const ConnCallback_t& func) { m_connErrorFunc = func; }
        void setConnCloseCallback(const ConnCallback_t& func) { m_connCloseFunc = func; }

    private:
        void onNewConnection(int sockFd);

        void newConnectionInLoop(IOLoop* loop, int sockFd);
        void deleteConnection(Connection* conn);
        void deleteConnectionInLoop(Connection* conn);

    private:
        Acceptor* m_acceptor;

        //threads for handle connections
        IOLoopThreadPool* m_connLoops;
    
        IOLoop* m_mainLoop;

        Signaler* m_signaler;

        std::vector<Connection*> m_connections;
    
        int m_maxConnections;
        int m_curConnections;
    
        SpinLock m_lock;

        ConnReadCallback_t m_connReadFunc;
        ConnCallback_t m_connWriteOverFunc;
        ConnCallback_t m_connErrorFunc;
        ConnCallback_t m_connCloseFunc;
    };
        
}

#endif

