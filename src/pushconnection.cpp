#include "pushconnection.h"

using namespace std;

using namespace std::tr1::placeholders;

namespace tpush
{
    void dummyPushFunc(const std::tr1::shared_ptr<void>& conn, const string& message)
    {
        
    }

    vector<PushConnection::PushFunc_t> PushConnection::ms_pushFuncs;

    class InitPushFuncs
    {
    public:
        InitPushFuncs()
        {
            PushConnection::initPushFuncs();    
        }    
    };

    InitPushFuncs initObj;

    PushConnection::PushConnection(const WeakConnectionPtr_t& conn, int fd, ConnType connType)
        : m_conn(conn)
        , m_connType(connType)
        , m_fd(fd)
    {
        
    }

    PushConnection::PushConnection(const PushConnection& conn)
    {
        m_conn = conn.m_conn;
        m_connType = conn.m_connType;
        m_fd = conn.m_fd;    
    }

    PushConnection::~PushConnection()
    {
        
    }

    void PushConnection::initPushFuncs()
    {
        ms_pushFuncs.resize(int(PushConnection::NoType));
        
        for(size_t i = 0; i < ms_pushFuncs.size(); ++i)
        {
            ms_pushFuncs[i] = std::tr1::bind(&dummyPushFunc, _1, _2);    
        }     
    }

    void PushConnection::setPushFunc(const PushFunc_t& func, ConnType connType)
    {
        if(int(connType) >= 0 && int(connType) < int(NoType))
        {
            ms_pushFuncs[int(connType)] = func;     
        }
    }

    void PushConnection::push(const ConnectionPtr_t& conn, const string& message, ConnType connType)
    {    
        if(int(connType) >= 0 && int(connType) < int(NoType))
        {
            ms_pushFuncs[int(connType)](conn, message);  
        }
    }
}
