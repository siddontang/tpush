#include "acceptor.h"

#include "ioloop.h"

namespace tpush
{

    Acceptor::Acceptor(IOLoop* loop, int sockFd)
    {
        m_loop = loop;
        m_sockFd = sockFd;

    }

    Acceptor::~Acceptor()
    {
            
    }
    
}
