#ifndef _PUSHCONNECTION_H_
#define _PUSHCONNECTION_H_

#include <tr1/memory>
#include <tr1/functional>
#include <vector>
#include <string>

#include "httpdefs.h"

namespace tpush
{
    class PushConnection
    {
    public:
        enum ConnType
        {
            HttpType = 0,
            WsType,
            NoType,
        };

        PushConnection() {}
        PushConnection(const tnet::WeakContextPtr_t& conn, int fd, ConnType connType);
        PushConnection(const PushConnection& conn);

        ~PushConnection();
        
        bool operator < (const PushConnection& conn) const
        {
            return m_fd < conn.m_fd;
        }
       
        ConnType getConnType() const { return m_connType; }
        tnet::WeakContextPtr_t getConn() const { return m_conn; }
      
        int getSockFd() const { return m_fd; }
       
        tnet::ContextPtr_t lock() const { return m_conn.lock(); }
        
        typedef std::tr1::function<void (const tnet::ContextPtr_t&, const std::string&)> PushFunc_t;
        static void setPushFunc(const PushFunc_t& func, ConnType connType);

        static void initPushFuncs();

        static void push(const tnet::ContextPtr_t& conn, const std::string& message, ConnType connType);

    private:
        tnet::WeakContextPtr_t m_conn;    
        ConnType m_connType;
        int m_fd;

        static std::vector<PushFunc_t> ms_pushFuncs;
    };
    
}

#endif
