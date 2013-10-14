#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

extern "C"
{
#include <ev.h>    
}

#include "nocopyable.h"

namespace tpush
{
    class IOLoop;
    class Acceptor : public nocopyable
    {
    public:
        Acceptor(IOLoop* loop, int sockFd);    
        ~Acceptor();

    private:
        void onRead();
        void onAccepted();
        void onError();

    private:
        IOLoop* m_loop;
        int m_sockFd;

    };
    
}

#endif

