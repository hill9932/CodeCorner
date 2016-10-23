#ifndef __HL_STRUCTURE_INCLUDE_H__
#define __HL_STRUCTURE_INCLUDE_H__

#include "http/request.h"
#include "memory_creator.hpp"


struct HTTPClient_t
{
    enum RequestStatus_e
    {
        #undef STATUS_TIMEOUT

        STATUS_UNKNOWN     = -1,
        STATUS_BORN        = 0,
        STATUS_INITIALIZED = 1,
        STATUS_DOWNLOADING = 2,
        STATUS_DOWNLOADED  = 3,
        STATUS_TIMEOUT     = 4,
        STATUS_PROCESSING  = 5,
        STATUS_EXCEPTION   = 9,
        STATUS_FINISHED    = 10
    };

    tstring                     url;
    IHttpRequest::Type          type;
    struct evhttp_uri           *uri;
    struct evhttp_connection    *conn;
    struct evhttp_request       *request;
    RequestStatus_e             status;

private:
    HTTPClient_t();
    ~HTTPClient_t();

    friend class CMemCreator<HTTPClient_t>;
    friend class CMemDeleter<HTTPClient_t>;
};
typedef shared_ptr<HTTPClient_t>    HTTPClientPtr;


#endif