#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <string>
#include <vector>

#include "nocopyable.h"
#include "address.h"

namespace tpush 
{   
    class IOLoop;
    class IOLoopThreadPool;
     
    class TcpServer : public nocopyable
    {
    public:
        TcpServer(int acceptLoopNum, int connLoopNum);
        ~TcpServer();

        int listen(const Address& addr);
        void stop();

    private:
        //threads for handle accept
        IOLoopThreadPool* m_acceptLoops;

        //threads for handle connections
        IOLoopThreadPool* m_connLoops;
    };
        
}

#endif

