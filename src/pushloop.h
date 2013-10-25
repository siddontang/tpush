#ifndef _PUSHLOOP_H_
#define _PUSHLOOP_H_

#include <string>
#include <map>
#include <vector>

#include "nocopyable.h"

namespace tnet
{
    class IOLoop;
    class Timer;
    class nocopyable;    
}

namespace tpush
{
    class Channel;

    class PushConnection;

    class PushLoop : public tnet::nocopyable
    {
    public:
        PushLoop(tnet::IOLoop* loop);
        ~PushLoop();

        void start();
        void stop();

        void subscribe(const std::string& id, const PushConnection& conn);
        void subscribe(const std::vector<std::string>& ids, const PushConnection& conn);

        void unsubscribe(const std::string& id, const PushConnection& conn);
        void unsubscribe(const std::vector<std::string>& ids, const PushConnection& conn);
        
        void publish(const std::string& id, const std::string& message); 
        void publish(const std::vector<std::string>& ids, const std::string& message); 

    private:
        void onCheck(tnet::Timer* timer);

    private:
        tnet::IOLoop* m_loop;    
        tnet::Timer* m_checkTimer;
    
        typedef std::map<std::string, Channel*> Channels_t;
        
        Channels_t m_channels;
        
        Channels_t::iterator m_lastCheckIter;        
    };
}

#endif
