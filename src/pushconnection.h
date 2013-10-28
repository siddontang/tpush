#ifndef _PUSHCONNECTION_H_
#define _PUSHCONNECTION_H_

#include <tr1/memory>
#include <tr1/functional>
#include <vector>
#include <string>

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
            HttpType = 0,
            WebSocketType,
            TcpType,    
            NoType,
        };

        typedef std::tr1::shared_ptr<tnet::Connection> ConnectionPtr_t;
        typedef std::tr1::weak_ptr<tnet::Connection> WeakConnectionPtr_t;

        PushConnection() {}
        PushConnection(const WeakConnectionPtr_t& conn, int fd, ConnType connType);
        PushConnection(const PushConnection& conn);

        ~PushConnection();
        
        bool operator < (const PushConnection& conn) const
        {
            return m_fd < conn.m_fd;
        }
       
        ConnType getConnType() const { return m_connType; }
        WeakConnectionPtr_t getConn() const { return m_conn; }
      
        int getSockFd() const { return m_fd; }
       
        ConnectionPtr_t lock() const { return m_conn.lock(); }
        
        typedef std::tr1::function<void (const ConnectionPtr_t&, const std::string&)> PushFunc_t;
        static void setPushFunc(const PushFunc_t& func, ConnType connType);

        static void initPushFuncs();

        static void push(const ConnectionPtr_t& conn, const std::string& message, ConnType connType);

    private:
        WeakConnectionPtr_t m_conn;    
        ConnType m_connType;
        int m_fd;

        static std::vector<PushFunc_t> ms_pushFuncs;
    };
    
}

#endif
