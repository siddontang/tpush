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

    static void dummyConnFunc(Connection* conn, Connection::Event event, char* buffer, int bufferLen)
    {
        (void)conn;
        (void)event;
        (void)buffer;
        (void)bufferLen;    
    }

    Connection::Connection(IOLoop* loop, int sockFd)
    {
        m_loop = loop;

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
    }
   
    void Connection::shutDown()
    {
        m_status = Disconnecting;
        
        if(!m_loop->inLoopThread())
        {
            return m_loop->runTask(std::tr1::bind(&Connection::shutDown, this));    
        }

        handleClose();
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
        int sockFd = m_io.fd;
        
        char buf[MaxReadBuffer];

        int n = read(sockFd, buf, sizeof(buf));
        if(n > 0)
        {
            m_func(this, ReadEvent, buf, n); 
        }
        else if(n == 0)
        {
            m_func(this, CloseEvent, NULL, 0);
            handleClose();
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

    void Connection::handleWrite()
    {
        int sockFd = m_io.fd;
        
        int n = write(sockFd, m_sendBuffer.data(), m_sendBuffer.size());
        if(n == int(m_sendBuffer.size()))
        {
            m_sendBuffer.clear();

            m_func(this, WriteOverEvent, NULL, 0);
            
            ev_io_stop(m_loop->evloop(), &m_io);
            ev_io_set(&m_io, sockFd, EV_READ);    
            ev_io_start(m_loop->evloop(), &m_io);
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
                m_sendBuffer.clear();

                handleError();
                
                LOG_ERROR("socket %d writeData Error %s", sockFd, errorMsg(err));
            }
        }
        
        //not write over, write later
        if(!(m_io.events & EV_WRITE))
        {
            //if no write event, add it
            ev_io_stop(m_loop->evloop(), &m_io);
            ev_io_set(&m_io, sockFd, EV_READ | EV_WRITE);    
            ev_io_start(m_loop->evloop(), &m_io);
        }    

        m_sendBuffer = m_sendBuffer.substr(n);
    }

    void Connection::handleError()
    {
        m_status = Disconnected;

        ev_io_stop(m_loop->evloop(), &m_io);
    
        m_func(this, ErrorEvent, NULL, NULL);
    }

    void Connection::handleClose()
    {
        m_status = Disconnected;

        ev_io_stop(m_loop->evloop(), &m_io);
    
        m_func(this, CloseEvent, NULL, NULL);
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
}