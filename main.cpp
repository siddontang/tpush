#include <stdio.h>
#include <string>

#include <signal.h>

#include "log.h"

#include "address.h"
#include "tcpserver.h"

#include "connection.h"

using namespace std;
using namespace tpush;
using namespace std::tr1::placeholders;

void sigAction(TcpServer* pServer, int signum)
{
    printf("signum %d\n", signum);

    pServer->stop();
}

void onRead(Connection* conn)
{
    string buf = conn->popRecvBuffer();
    LOG_INFO("conn read data %d", int(buf.size()));
    
    conn->send(buf);    
}

void onWriteOver(Connection* conn)
{
    LOG_INFO("conn write over");

//    conn->shutDown();
}

void onError(Connection* conn)
{
    LOG_INFO("conn error");
}

void onClose(Connection* conn)
{
    LOG_INFO("conn close");
}

void onConnEvent(Connection* conn, Connection::Event event)
{
    switch(event)
    {
        case Connection::ReadEvent:
            onRead(conn);
            break;
        case Connection::WriteOverEvent:
            onWriteOver(conn);
            break;
        case Connection::CloseEvent:
            onClose(conn);
            break;
        case Connection::ErrorEvent:
            onError(conn);
            break;
    }    
}

int main()
{
    TcpServer s(2, 10, 100);
    s.listen(Address(11181), std::tr1::bind(&onConnEvent, _1, _2));

    s.addSignal(SIGINT, std::tr1::bind(sigAction, &s, std::tr1::placeholders::_1));

    LOG_INFO("start tcp server");
    

    s.start();

    LOG_INFO("stop server");

    return 0;
}   
