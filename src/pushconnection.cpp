#include "pushconnection.h"

namespace tpush
{
    PushConnection::PushConnection(const WeakConnectionPtr_t& conn, int fd, ConnType connType)
        : m_conn(conn)
        , m_connType(connType)
        , m_fd(fd)
    {
        
    }

    PushConnection::~PushConnection()
    {
        
    }
}
