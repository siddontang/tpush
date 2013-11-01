#ifndef _PUSHLOOP_H_
#define _PUSHLOOP_H_

#include <string>
#include <map>
#include <vector>

#include "nocopyable.h"
#include "coredefs.h"

namespace tnet
{
    class IOLoop;
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
        void subscribes(const std::vector<std::string>& ids, const PushConnection& conn);

        void unsubscribe(const std::string& id, const PushConnection& conn);
        void unsubscribes(const std::vector<std::string>& ids, const PushConnection& conn);
        
        void publish(const std::string& id, const std::string& message); 
        void publishs(const std::vector<std::string>& ids, const std::string& message); 

    private:
        void onCheck(const tnet::TimerPtr_t& timer);

        void subscribesInLoop(const std::vector<std::string>& ids, const PushConnection& conn);
        void unsubscribesInLoop(const std::vector<std::string>& ids, const PushConnection& conn);
        void publishsInLoop(const std::vector<std::string>& ids, const std::string& message);

    private:
        tnet::IOLoop* m_loop;    
        tnet::TimerPtr_t m_checkTimer;
    
        typedef std::map<std::string, Channel*> Channels_t;
        
        Channels_t m_channels;
        
        Channels_t::iterator m_lastCheckIter;        
    };
}

#endif
