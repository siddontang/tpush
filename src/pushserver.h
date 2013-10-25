#ifndef _PUSHSERVER_H_
#define _PUSHSERVER_H_

#include <vector>
#include <tr1/memory>
#include <tr1/functional>

#include "nocopyable.h"

namespace tnet
{
    class nocopyable;
    class IOLoop;
    class IOLoopThreadPool;    
    class HttpRequest;
    class Connection;
}

namespace tpush
{
    class PushLoop;
    class PushServer : public tnet::nocopyable
    {
    public:
        typedef std::tr1::shared_ptr<tnet::Connection> ConnectionPtr_t;

        PushServer();
        ~PushServer();
        
        void start();
        void stop();
    
        void onHttpRequest(const tnet::HttpRequest&, const ConnectionPtr_t& conn);

    private:
        tnet::IOLoopThreadPool* m_pool;
        std::vector<PushLoop*> m_loops;    
    };
        
}

#endif
