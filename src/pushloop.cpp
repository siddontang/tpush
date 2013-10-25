#include "pushloop.h"

#include "ioloop.h"
#include "timer.h"
#include "config.h"
#include "channel.h"

using namespace tnet;
using namespace std;
using namespace std::tr1::placeholders;

namespace tpush
{
    PushLoop::PushLoop(IOLoop* loop)
        : m_loop(loop)
    {
        m_lastCheckIter = m_channels.end();
    
        m_checkTimer = new Timer(m_loop, std::tr1::bind(&PushLoop::onCheck, this, _1), 
                    Config::PushLoopCheckRepeat, 60 * 1000); 
    }    

    PushLoop::~PushLoop()
    {
        delete m_checkTimer;
    }

    void PushLoop::start()
    {
        m_checkTimer->start();
    }

    void PushLoop::stop()
    {
        m_checkTimer->stop();
    }

    void PushLoop::subscribe(const string& id, const PushConnection& conn)
    {
        vector<string> ids(1, id);
        subscribe(ids, conn);
    }

    void PushLoop::unsubscribe(const string& id, const PushConnection& conn)
    {
        vector<string> ids(1, id);
        unsubscribe(ids, conn);    
    }

    void PushLoop::publish(const string& id, const string& message)
    {
        vector<string> ids(1, id);
        publish(ids, message);    
    }

    void PushLoop::subscribe(const vector<string>& ids, const PushConnection& conn)
    {
        for(size_t i = 0; i < ids.size(); ++i)
        {
            Channel* channel = NULL;
            Channels_t::iterator iter = m_channels.find(ids[i]);
            if(iter != m_channels.end())
            {
                channel = iter->second;
            }
            else
            {
                channel = new Channel(ids[i]);    
                m_channels[ids[i]] = channel;
            }

            channel->subscribe(conn);
        }    
    }

    void PushLoop::unsubscribe(const vector<string>& ids, const PushConnection& conn)
    {
        for(size_t i = 0; i < ids.size(); ++i)
        {
            Channels_t::iterator iter = m_channels.find(ids[i]);
            if(iter == m_channels.end())
            {
                continue;    
            }

            (iter->second)->unsubscribe(conn);
        }    
    }

    void PushLoop::publish(const vector<string>& ids, const string& message)
    {
        for(size_t i = 0; i < ids.size(); ++i)
        {
            Channel* channel = NULL;
            Channels_t::iterator iter = m_channels.find(ids[i]);
            if(iter != m_channels.end())
            {
                channel = iter->second;
            }
            else
            {
                channel = new Channel(ids[i]);    
                m_channels[ids[i]] = channel;
            }

            channel->publish(message);
        }    
    }

    void PushLoop::onCheck(Timer* timer)
    {
        if(m_lastCheckIter == m_channels.end())
        {
            m_lastCheckIter = m_channels.begin();    
        }

        while(m_lastCheckIter != m_channels.end())
        {
            Channel* c = m_lastCheckIter->second;
            if(c->empty())
            {
                delete c;
                m_channels.erase(m_lastCheckIter++);    
            }    
            else
            {
                ++m_lastCheckIter;    
            }
        }
    }
}
