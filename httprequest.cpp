#include "httprequest.h"

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
    }
}
