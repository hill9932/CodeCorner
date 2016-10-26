#ifndef __HL_STRUCTURE_INCLUDE_H__
#define __HL_STRUCTURE_INCLUDE_H__

#include "http/request.h"
#include "memory_creator.hpp"

struct MAIN_TABLE_RECORD_t
{
    enum STATUS_e
    {
        STATUS_UNCHECK = 0,
        STATUS_PROCESSING,         // doing dns lookup
        STATUS_READY,              // dns lookup finished and has valid ips if possible
        STATUS_FAILED              // web page grabbed
    };

    u_int32     id;
    u_int32     createTime;
    u_int32     visitTime;
    MAIN_TABLE_RECORD_t::STATUS_e   status;
    string      name;
};


struct URL_RECORD_t : public MAIN_TABLE_RECORD_t
{

};


struct DNS_RECORD_t : public MAIN_TABLE_RECORD_t
{
    string      cname;
    string      address;
};


struct MemoryHint
{
    MemoryHint();

    /**
     * @Function: notify CMemCreator that it can be reused
     **/
    void done();
    void undone();

    virtual void reset() = 0;

#if defined(ENABLE_DEBUG_MEM_POOL_ASSERT)
    void setDebugId(int _i)  { debugId = _i; }
    int  debugId;
#endif

private:
    bool finished;
    template<typename> friend class CMemCreator;
};

struct HTTPClient_t : public MemoryHint
{
    enum RequestStatus_e
    {
        #undef STATUS_TIMEOUT

        STATUS_UNCHECK      = 0,
        STATUS_INITIALIZED  = 1,
        STATUS_DOWNLOADING  = 2,
        STATUS_DOWNLOADED   = 3,
        STATUS_PROCESSING   = 4,
        STATUS_FAILED       = 5,
        STATUS_TIMEOUT      = 6,
        STATUS_UNKNOWN      = 7,
        STATUS_FINISHED     = 10,
        STATUS_ALL
    };

    static const char * RequestStatusDesc[];

    u_int64                     id;
    tstring                     url;
    IHttpRequest::Type          type;
    struct evhttp_uri           *uri;
    struct evhttp_connection    *conn;
    struct evhttp_request       *request;
    RequestStatus_e             status;
    int                         respCode;

    void reset();

private:
    HTTPClient_t();
    ~HTTPClient_t();
    
    friend class CMemCreator<HTTPClient_t>;
    friend class CMemDeleter<HTTPClient_t>;
};
typedef shared_ptr<HTTPClient_t>    HTTPClientPtr;



#endif
