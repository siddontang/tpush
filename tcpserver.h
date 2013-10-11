#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <string>

#include "address.h"

namespace tpush 
{
    
    class TcpServer
    {
    public:
        TcpServer();
        ~TcpServer();

        int listen(const Address& addr);

    private:
        
    };
        
}

#endif

