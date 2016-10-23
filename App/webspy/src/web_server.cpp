#include "web_server.h"

#include <evhttp.h>
#include <event2/http.h>
#include <event2/http_struct.h>


CWebServer::CWebServer()
{

}

CWebServer::~CWebServer()
{

}

bool CWebServer::__init__()
{
    // http://blog.csdn.net/toontong/article/details/6029151
    return true;
}
