#include "structure.h"

vector<HTTPClientPtr> CMemDeleter<HTTPClient_t>::m_memPool;


MemoryHint::MemoryHint()
{
    undone();
}

void MemoryHint::done()
{
#if defined(ENABLE_DEBUG_MEM_POOL_ASSERT)
    L4C_LOG_DEBUG("I'm done at: " << debugId);
#endif
    finished = true;
}

void MemoryHint::undone()
{
#if defined(ENABLE_DEBUG_MEM_POOL_ASSERT)
    L4C_LOG_DEBUG("I'm undone at: " << debugId);
#endif
    finished = false;
}


HTTPClient_t::HTTPClient_t()
{
    uri = NULL;
    conn = NULL;
    request = NULL;

    reset();
}

HTTPClient_t::~HTTPClient_t()
{

}

void HTTPClient_t::reset()
{
    if (respCode == HTTP_OK && request)
    {
        evhttp_request_free(request);
    }
    else
    {
        if (conn)   evhttp_connection_free(conn);
        if (uri)    evhttp_uri_free(uri);
    }


    uri = NULL;
    conn = NULL;
    request = NULL;
    status = HTTPClient_t::STATUS_UNCHECK;
    type = IHttpRequest::Type::NONE;
    respCode = 0;

    undone();
}
