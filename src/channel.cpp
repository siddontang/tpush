#include "channel.h"

#include "config.h"

using namespace std;
using namespace tnet;

namespace tpush
{
    Channel::Channel(const string& id)
        : m_id(id)
    {
        
    }    

    Channel::~Channel()
    {
        
    }

    bool Channel::empty()
    {
        return m_messages.empty() && m_connections.empty();    
    }

    void Channel::check()
    {
        checkMessages();        
        checkConnections();
    }

    void Channel::checkConnections()
    {
        for(PushConnections_t::iterator iter = m_connections.begin();
            iter != m_connections.end();)
        {
            if(!(iter->lock()))
            {
                m_connections.erase(iter++);    
            }
            else
            {
                ++iter;    
            }
        }    
    }

    void Channel::subscribe(const PushConnection& conn)
    {
        if(m_connections.size() > Config::MaxChannelConnectionNum)
        {
            return;    
        }
    
        ConnectionPtr_t c = conn.lock();
        if(!c)
        {
            return;    
        } 

        checkMessages();

        if(!m_messages.empty())
        {
            if(conn.getConnType() == PushConnection::HttpType)
            {
                //for http connection, push one message once
                pushMessage(c, m_messages[0].message, conn.getConnType());
                m_messages.erase(m_messages.begin());
            }
            else
            {
                for(size_t i = 0; i < m_messages.size(); ++i)
                {
                    pushMessage(c, m_messages[i].message, conn.getConnType());    
                }
                m_messages.clear();
            }
        }
        
        m_connections.insert(conn);     
    }

    void Channel::unsubscribe(const PushConnection& conn)
    {
        m_connections.erase(conn);    
    }

    void Channel::publish(const string& message)
    {
        bool pushed = false;
        if(!m_connections.empty())
        {
            for(PushConnections_t::iterator iter = m_connections.begin(); 
                iter != m_connections.end();)
            {
                ConnectionPtr_t c = iter->lock();
                if(c)
                {
                    pushMessage(c, message, iter->getConnType());    
                    pushed = true;
                    ++iter;
                }
                else
                {
                    m_connections.erase(iter++);    
                }
            }       
        }
        
        if(!pushed)
        {
            //the message has not pushed, reserve it
            if(m_messages.size() > Config::MaxChannelMessageReserveNum)
            {
                m_messages.erase(m_messages.begin());    
            }

            m_messages.push_back(PushMessage(message, time(NULL)));    
        }
    }

    void Channel::pushMessage(const ConnectionPtr_t& conn, const string& message, PushConnection::ConnType connType)
    {
        //    
    }

    void Channel::checkMessages()
    {
        int64_t now = time(NULL);
        for(size_t i = 0; i < m_messages.size(); ++i)
        {
            if(now - m_messages[i].pubTime < Config::MaxChannelMessageReserveTime)
            {
                m_messages.erase(m_messages.begin(), m_messages.begin() + i);
                break;  
            }    
        }
    }
}
