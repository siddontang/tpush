#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <tr1/functional>
#include <tr1/memory>
#include <string>

extern "C"
{
#include <ev.h>    
}

namespace tpush
{
    class IOLoop;

    class Connection
    {
    public:
        enum Event
        {
            ReadEvent,
            WriteOverEvent,
            ErrorEvent,
            CloseEvent,
        };
        
        enum Status
        {
            Connecting,
            Connected,
            Disconnecting,
            Disconnected,
        };

        Connection(IOLoop* loop, int sockFd);
        ~Connection();    

        void onEstablished();
        void shutDown();

        typedef std::tr1::function<void (Connection*, Connection::Event, char*, int)> ConnectionFunc_t;
        void setCallback(const ConnectionFunc_t& func) { m_func = func; }

        void send(const char* data, int dataLen);
        void send(const std::string& data);

        Status getStatus() { return m_status; }

        int getSockFd() { return m_io.fd; }

        IOLoop* getLoop() { return m_loop; }

    private:
        static void onData(struct ev_loop*, struct ev_io*, int);

        void handleRead();
        void handleError();
        void handleWrite();
        void handleClose();

        void sendInLoop(const std::string& data);

        void resetIOEvent(int events);

    private:
        IOLoop* m_loop;

        struct ev_io m_io;

        ConnectionFunc_t m_func;

        Status m_status;

        std::string m_sendBuffer;
    };    
}

#endif
