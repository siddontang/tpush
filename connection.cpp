#include "connection.h"

#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "ioloop.h"
#include "misc.h"
#include "log.h"

using namespace std;
using namespace std::tr1::placeholders;

namespace tpush
{

    static int MaxReadBuffer = 4096;

    static void dummyConnFunc(Connection*, Connection::Event, const char*, int)
    {
    }

    Connection::Connection(IOLoop* loop, int sockFd)
        : m_loop(loop)
    {
        m_status = Connecting;

        m_func = std::tr1::bind(&dummyConnFunc, _1, _2, _3, _4);
        
        ev_io_init(&m_io, Connection::onData, sockFd, EV_READ);
    }

    Connection::~Connection()
    {
            
    }   
   
    void Connection::onEstablished()
    {
        if(!m_loop->inLoopThread())
        {
            return m_loop->runTask(std::tr1::bind(&Connection::onEstablished, this));    
        }

        m_status = Connected;

        m_io.data = this;
    
        ev_io_start(m_loop->evloop(), &m_io);
    
        m_func(this, EstablishedEvent, NULL, 0);
    }
   
    void Connection::shutDown()
    {
        assert(m_status == Connected);

        if(m_status == Disconnecting || m_status == Disconnected)
        {
            return;    
        }

        m_status = Disconnecting;

        if(!m_loop->inLoopThread())
        {
            return m_loop->runTask(std::tr1::bind(&Connection::handleClose, this));    
        }
        else
        {
            handleClose();
        }
    }
    
    void Connection::onData(struct ev_loop* loop, struct ev_io* w, int revents)
    {
        Connection* conn = (Connection*)w->data;
        
        if(revents & EV_ERROR)
        {
            conn->handleError();
        }
        
        if(revents & EV_READ)
        {
            conn->handleRead();    
        }
        
        if(revents & EV_WRITE)
        {
            conn->handleWrite();        
        }
    } 

    void Connection::handleRead()
    {
        if(m_status != Connected)
        {
            return;    
        }

        int sockFd = m_io.fd;
        
        char buf[MaxReadBuffer];

        int n = read(sockFd, buf, sizeof(buf));
        if(n > 0)
        {
            m_func(this, ReadEvent, buf, n); 
            return;
        }
        else if(n == 0)
        {
            handleClose();
            return;
        }
        else
        {
            int err = errno;    
            if(err == EAGAIN || err == EWOULDBLOCK)
            {
                return;    
            }
                 
            handleError();
            LOG_ERROR("socket %d readData Error %s", sockFd, errorMsg(err));
        }
    }

    inline void clearBuffer(string& buffer)
    {
        string tmp;
        buffer.swap(tmp);    
    }
    
    void Connection::handleWrite()
    {
        if(m_status != Connected)
        {
            return;    
        }

        if(m_sendBuffer.empty())
        {
            resetIOEvent(EV_READ);
            return;    
        }

        int sockFd = m_io.fd;
        
        int n = write(sockFd, m_sendBuffer.data(), m_sendBuffer.size());
        
        if(n == int(m_sendBuffer.size()))
        {
            clearBuffer(m_sendBuffer);

            m_func(this, WriteCompleteEvent, NULL, 0);

            resetIOEvent(EV_READ);

            return;
        }
        else if(n < 0)
        {
            int err = errno;
            if(err == EAGAIN || err == EWOULDBLOCK)
            {
                //try write later
                n = 0;   
            }
            else
            {
                clearBuffer(m_sendBuffer);

                handleError();
                
                LOG_ERROR("socket %d writeData Error %s", sockFd, errorMsg(err));
            }
        }
        
        //some send data may be left, we may send it next time
        resetIOEvent(EV_READ | EV_WRITE);

        m_sendBuffer = m_sendBuffer.substr(n);
    }

    void Connection::handleError()
    {
        m_func(this, ErrorEvent, NULL, 0);

        handleClose();
    }

    void Connection::handleClose()
    {
        if(m_status == Disconnected)
        {
            return;    
        }

        int sockFd = m_io.fd;

        ev_io_stop(m_loop->evloop(), &m_io);
    
        close(sockFd);    

        m_status = Disconnected;
    
        m_func(this, CloseEvent, NULL, 0);
    }

    void Connection::send(const char* data, int dataLen)
    {
        send(string(data, dataLen));
    }

    void Connection::send(const string& data)
    {
        if(m_status != Connected)
        {
            return;    
        }

        if(!m_loop->inLoopThread())
        {
            m_loop->addTask(std::tr1::bind(&Connection::sendInLoop, this, data));
        }
        else
        {
            sendInLoop(data);    
        }
    }

    void Connection::sendInLoop(const string& data)
    {
        assert(m_loop->inLoopThread());
        
        if(m_status != Connected)
        {
            return;    
        }
        
        if(m_sendBuffer.empty())
        {
            m_sendBuffer = data;    
        }
        else
        {
            m_sendBuffer.append(data);    
        }
        
        handleWrite();
    }

    void Connection::resetIOEvent(int events)
    {
        assert(m_loop->inLoopThread());

        if(m_io.events != events)
        {
            int sockFd = m_io.fd;
            ev_io_stop(m_loop->evloop(), &m_io);
            ev_io_set(&m_io, sockFd, events);
            ev_io_start(m_loop->evloop(), &m_io);
        }
    }

}
