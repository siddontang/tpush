#include <stdio.h>

#include <signal.h>

#include "log.h"

#include "address.h"
#include "tcpserver.h"

#include "connection.h"

using namespace tpush;
using namespace std::tr1::placeholders;

void sigAction(TcpServer* pServer, int signum)
{
    printf("signum %d\n", signum);

    pServer->stop();
}

void onRead(Connection* conn, const char* buf, int bufLen)
{
    LOG_INFO("conn read data %d", bufLen);
    
    conn->send(buf, bufLen);    
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

int main()
{
    TcpServer s(2, 10, 100);
    s.listen(Address(11181));

    s.addSignal(SIGINT, std::tr1::bind(sigAction, &s, std::tr1::placeholders::_1));

    LOG_INFO("start tcp server");
    
    LOG_INFO("max file num %d", getdtablesize());

    s.setConnReadCallback(std::tr1::bind(onRead, _1, _2, _3));
    s.setConnWriteOverCallback(std::tr1::bind(onWriteOver, _1));
    s.setConnErrorCallback(std::tr1::bind(onError, _1));
    s.setConnCloseCallback(std::tr1::bind(onClose, _1));

    s.start();

    LOG_INFO("stop server");

    return 0;
}   
