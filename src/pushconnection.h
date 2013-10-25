#ifndef _PUSHCONNECTION_H_
#define _PUSHCONNECTION_H_

#include <tr1/memory>

namespace tnet
{
    class Connection;
}

namespace tpush
{
    class PushConnection
    {
    public:
        enum ConnType
        {
            HttpType,
            WebSocketType,
            TcpType,    
        };

        typedef std::tr1::shared_ptr<tnet::Connection> ConnectionPtr_t;
        typedef std::tr1::weak_ptr<tnet::Connection> WeakConnectionPtr_t;

        PushConnection(const WeakConnectionPtr_t& conn, int fd, ConnType connType);
        ~PushConnection();
        
        bool operator < (const PushConnection& conn) const
        {
            return m_fd < conn.m_fd;
        }
       
        ConnType getConnType() const { return m_connType; }
        WeakConnectionPtr_t getConn() const { return m_conn; }
       
        ConnectionPtr_t lock() const { return m_conn.lock(); }
        
    private:
        WeakConnectionPtr_t m_conn;    
        ConnType m_connType;
        int m_fd;
    };
    
}

#endif
