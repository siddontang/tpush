#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <string>
#include <vector>
#include <tr1/functional>

#include "nocopyable.h"
#include "address.h"

namespace tpush 
{   
    class IOLoop;
    class IOLoopThreadPool;
    
    class Signaler;
    class Acceptor;

    class TcpServer : public nocopyable
    {
    public:
        TcpServer(int acceptLoopNum, int connLoopNum);
        ~TcpServer();

        int listen(const Address& addr);
       
        void setConnLoopIOInterval(int milliseconds);
        
        typedef std::tr1::function<void (int)> SignalFunc_t;
        void addSignal(int signum, const SignalFunc_t& func);

        void start();
        void stop();

    private:
        void onNewConnection(int sockFd, const Address& addr);

    private:
        Acceptor* m_acceptor;

        //threads for handle connections
        IOLoopThreadPool* m_connLoops;
    
        IOLoop* m_mainLoop;

        Signaler* m_signaler;
    };
        
}

#endif

