#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <string>

#include <stdint.h>

namespace tpush 
{
    
    class TcpServer
    {
    public:
        TcpServer();
        ~TcpServer();

        int listen(const std::string& ip, uint16_t port);

    private:
        
    };
        
}

#endif

