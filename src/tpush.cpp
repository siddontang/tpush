#include "tcpserver.h"
#include "httpserver.h"

using namespace tnet;

int main()
{
    TcpServer s(2, 4, 1000000);

    HttpServer httpd(&s); 

    return 0;    
}
