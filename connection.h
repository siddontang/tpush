#ifndef _CONNECTION_H_
#define _CONNECTION_H_

extern "C"
{
#include <ev.h>    
}

namespace tpush
{
    class IOLoop;

    class Connection
    {
    public:
        Connection(IOLoop* loop, int sockFd);
        ~Connection();    

    private:
        struct ev_io m_io;
        int m_sockFd;
    };    
}

#endif
