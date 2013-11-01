#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <string>
#include <set>
#include <vector>
#include <map>

#include <stdint.h>
#include <tr1/functional>
#include <tr1/memory>

#include "nocopyable.h"
#include "pushconnection.h"
#include "httpdefs.h"

namespace tpush
{
    class Channel : public tnet::nocopyable
    {
    public:
        Channel(const std::string& id);
        ~Channel();
       
        bool empty();

        void check();
        
        void subscribe(const PushConnection& conn);
        void unsubscribe(const PushConnection& conn);

        void publish(const std::string& message);

    private:
        void checkMessages();
        void checkConnections();
    
        void pushMessage(const tnet::ContextPtr_t& conn, const std::string& message, PushConnection::ConnType connType);

    private:
        std::string m_id;    

        typedef std::map<int, PushConnection> PushConnections_t;
        PushConnections_t m_connections;

        class PushMessage
        {
        public:
            PushMessage(const std::string& m, int64_t t) 
                : message(m)
                , pubTime(t)
            {}
            std::string message;
            int64_t pubTime;    
        };

        typedef std::vector<PushMessage> PushMessages_t;
        PushMessages_t m_messages;
    };
}

#endif
