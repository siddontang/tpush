#include <stdio.h>

#include <signal.h>

#include "address.h"
#include "tcpserver.h"

using namespace tpush;

void sigAction(TcpServer* pServer, int signum)
{
    printf("signum %d\n", signum);

    pServer->stop();
}

int main()
{
    TcpServer s(2, 10);
    s.listen(Address(11181));
    s.listen(Address(11182));

    s.addSignal(SIGINT, std::tr1::bind(sigAction, &s, std::tr1::placeholders::_1));
    
    s.start();

    printf("end\n");

    return 0;
}   
