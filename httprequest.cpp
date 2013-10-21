#include "httprequest.h"

extern "C"
{
#include "http_parser.h"    
}

#include "log.h"
#include "uricodec.h"

using namespace std;

namespace tpush
{

    HttpRequest::HttpRequest()
    {
    }
   
    HttpRequest::~HttpRequest()
    {
    } 

    void HttpRequest::clear()
    {
        url.clear();
        body.clear();
        headers.clear();
        
        majorVersion = 0;
        minorVersion = 0;
        method = 0;        

        schema.clear();
        host.clear();
        port = 0;
        path.clear();
        params.clear(); 
    }

    void HttpRequest::parseUrl()
    {
        struct http_parser_url u;
        if(http_parser_parse_url(url.c_str(), url.size(), 0, &u) != 0)
        {
            LOG_ERROR("parseurl error %s", url.c_str());
            return;    
        }

        if(u.field_set & (1 << UF_SCHEMA))
        {
            schema = url.substr(u.field_data[UF_SCHEMA].off, u.field_data[UF_SCHEMA].len);    
        }

        if(u.field_set & (1 << UF_HOST))
        {
            host = url.substr(u.field_data[UF_HOST].off, u.field_data[UF_HOST].len);    
        }

        if(u.field_set & (1 << UF_PORT))
        {
            port = u.port;    
        }
        else
        {
            port = 80;    
        }

        if(u.field_set & (1 << UF_PATH))
        {
            path = url.substr(u.field_data[UF_PATH].off, u.field_data[UF_PATH].len);    
        }

        if(u.field_set & (1 << UF_QUERY))
        {
            string query = url.substr(u.field_data[UF_QUERY].off, u.field_data[UF_QUERY].len);    
        
            parseQuery(query);            
        }
    }

    void HttpRequest::parseQuery(const string& query)
    {
        
    }
}
