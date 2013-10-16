#include "acceptloop.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "ioloop.h"
#include "address.h"
#include "misc.h"
#include "sockutil.h"

using namespace std;

namespace tpush
{

    AcceptLoop::AcceptLoop(IOLoop* loop)
        : m_loop(loop)
    {
        
    }
    
    AcceptLoop::~AcceptLoop()
    {
        clearContainer(m_watchers);
    }

    void AcceptLoop::start()
    {
     
    }

    void AcceptLoop::stop()
    {
        IOLoop::Callback_t func = std::tr1::bind(&AcceptLoop::stopFromLoop, this);
        m_loop->runTask(func);
    }

    void AcceptLoop::stopFromLoop()
    {
        for(size_t i = 0; i < m_watchers.size(); i++)
        {
            ev_io_stop(m_loop->evloop(), &(m_watchers[i]->io));    
        }
    }

    void AcceptLoop::listen(int sockFd, const NewConnectionFunc_t& func)
    {
        IOLoop::Callback_t taskFunc = std::tr1::bind(&AcceptLoop::listenFromLoop, this, sockFd, func);
        m_loop->runTask(taskFunc);    
    } 

    void AcceptLoop::listenFromLoop(int sockFd, const NewConnectionFunc_t& func)
    {
        Watcher* watcher = NULL;
        for(size_t i = 0; i < m_watchers.size(); i++)
        {
            if(m_watchers[i] && m_watchers[i]->io.fd == sockFd)
            {
                watcher = m_watchers[i];
                break;    
            }    
        }

        if(!watcher)
        {
            watcher = new Watcher;
            ev_io_init(&(watcher->io), &AcceptLoop::onAccept, sockFd, EV_READ);     
            
            m_watchers.push_back(watcher);
        }

        watcher->func = func;
        ev_io_start(m_loop->evloop(), &(watcher->io));
    }

    void AcceptLoop::onAccept(struct ev_loop* loop, struct ev_io* w, int revents)
    {
        if(!(revents & EV_READ))
        {
            //some error may occur, maybe call ev_io_stop ?
            return;    
        }

        Watcher* watcher = (Watcher*)w;

        struct sockaddr_in addr;
        socklen_t addrLen = sizeof(addr);

        int sockFd = accept(w->fd, (struct sockaddr*)&addr, &addrLen);
        if(sockFd < 0)
        {
            int err = errno;
            if(err == EMFILE || err == ENFILE)
            {
                //we may do later    
            }
            return;
        }    

        SockUtil::setNonBlockingAndCloseOnExec(sockFd);
        (watcher->func)(sockFd, Address(addr));
    }
}
