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
    return true;
}
