#include <tr1/memory>
#include <tr1/functional>

#include "pushserver.h"

using namespace tpush;

int main()
{
    PushServer pushServer;

    pushServer.start();

    return 0;    
}
