#include "web_voyager.h"

#include <evhttp.h>
#include <event2/http.h>
#include <event2/http_struct.h>


#define VOYAGER_RECORD_DB_NAME          "voyager_records.db"
#define VOYAGER_RECORD_TABLE_NAME       "VOYAGER_RECORDS"


CWebVoyager::CWebVoyager()
{
    m_dbName    = VOYAGER_RECORD_DB_NAME;
    m_tableName = VOYAGER_RECORD_TABLE_NAME;

    m_evbase    = NULL;
    m_dnsbase   = NULL;
}

CWebVoyager::~CWebVoyager()
{

}

bool CWebVoyager::__init__()
{
    if (m_evbase && m_dnsbase)    return true;

    if (!event_init())
    {
        L4C_LOG_ERROR("Fail to init libevent.");
        return false;
    }

    m_evbase = event_base_new();
    if (!m_evbase)
    {
        L4C_LOG_ERROR("Fail to call event_base_new().");
        return false;
    }

    m_dnsbase = evdns_base_new(m_evbase, 1);
    if (!m_dnsbase)
    {
        L4C_LOG_ERROR("Fail to call evdns_base_new().");
        return false;
    }

    return true;
}

bool CWebVoyager::__uninit__()
{
    if (m_evbase)
    {
        event_base_free(m_evbase);
        m_evbase = NULL;
    }

    if (m_dnsbase)
    {
        evdns_base_free(m_dnsbase, 0);
        m_dnsbase = NULL;
    }

    return true;
}

string CWebVoyager::__getTableCreateSql__()
{
    return "CREATE TABLE "                                       \
        VOYAGER_RECORD_TABLE_NAME "("                                           \
        VOYAGER_RECORD_TABLE_COL_ID             " INTEGER PRIMARY KEY,"         \
        VOYAGER_RECORD_TABLE_COL_NAME           " TEXT NOT NULL UNIQUE,"        \
        VOYAGER_RECORD_TABLE_COL_CREATE_TIME    " INT  NOT NULL,"               \
        VOYAGER_RECORD_TABLE_COL_STATUS         " INT  NOT NULL);";
}

int CWebVoyager::ReadHeaderDoneCallback(struct evhttp_request* _request, void* _context)
{
    fprintf(stderr, "< HTTP/1.1 %d %s\n", evhttp_request_get_response_code(_request), evhttp_request_get_response_code_line(_request));
    struct evkeyvalq* headers = evhttp_request_get_input_headers(_request);
    struct evkeyval* header = headers->tqh_first;
    while (header) {
        printf("%s: %s\n", header->key, header->value);
        header = header->next.tqe_next;
    }
    return 0;
}

void CWebVoyager::ReadChunkCallback(struct evhttp_request* _request, void* _context)
{
    char buf[4096];
    struct evbuffer* evbuf = evhttp_request_get_input_buffer(_request);
    int n = 0;
    while ((n = evbuffer_remove(evbuf, buf, 4096)) > 0)
    {
        fwrite(buf, n, 1, stdout);
    }
}

void CWebVoyager::RemoteRequestErrorCallback(enum evhttp_request_error _error, void* _context)
{
    event_base_loopexit(CWebVoyager::GetInstance()->m_evbase, NULL);
}

void CWebVoyager::RemoteConnectionCloseCallback(struct evhttp_connection* _connection, void* _context)
{
    event_base_loopexit(CWebVoyager::GetInstance()->m_evbase, NULL);
}

CWebVoyager::HTTPClient_t::HTTPClient_t()
{
    uri     = NULL;
    conn    = NULL;
    req     = NULL;
    status  = UNKNOWN;
}

CWebVoyager::HTTPClient_t::~HTTPClient_t()
{
    if (uri)    evhttp_uri_free(uri);
    if (req)    evhttp_request_free(req);
    if (conn)   evhttp_connection_free(conn);
}

void CWebVoyager::HttpRequestCB(struct evhttp_request* _request, void* _arg)
{
    HTTPClient_t* hc = (HTTPClient_t*)_arg;
    if (!_request || !_arg)
    {
        L4C_LOG_ERROR("Invalid parameters.");
        return;
    }

    L4C_LOG_INFO(hc->url << ": " << _request->response_code);

    switch (_request->response_code)
    {
        case HTTP_OK:
        {
            CHttpUtils::ShowRequestInfo(_request);
            break;
        }

        case HTTP_MOVETEMP:
        {
            const char* newLocation = evhttp_find_header(_request->input_headers, Response::Header::Location::Value);
            HTTPClient_t* newHC = CWebVoyager::GetInstance()->createHttpRequset(newLocation, 0, NULL, NULL);
            startRequest(newHC);

            break;
        }
    }
}

bool CWebVoyager::startRequest(HTTPClient_t* _hc)
{
    if (!_hc)   return false;

    L4C_LOG_INFO("Request " << _hc->url);

    const char* path = evhttp_uri_get_path(_hc->uri);
    if (_hc->type == IHttpRequest::Type::GET)
    {
        const char* query = evhttp_uri_get_query(_hc->uri);
        tstring pathQuery;

        if (!query && !path)
        {
            pathQuery  = path;
            pathQuery += "?";
            pathQuery += query;
        }

        evhttp_make_request(_hc->conn, _hc->req, EVHTTP_REQ_GET, pathQuery.empty() ? "/" : pathQuery.c_str());
    }
    else if (_hc->type == IHttpRequest::Type::POST)
    {
        evhttp_make_request(_hc->conn, _hc->req, EVHTTP_REQ_POST, path ? "/" : path);
    }

    _hc->status = PROCESSING;
    return true;
}

CWebVoyager::HTTPClient_t* CWebVoyager::createHttpRequset(const char* _url, int _flag, const char* _contentType, const char* _data)
{
    if (!_url)  return NULL;

    HTTPClient_t* hc(new HTTPClient_t);
    hc->url  = _url;
    hc->uri  = evhttp_uri_parse(hc->url.c_str());

    const char* host = evhttp_uri_get_host(hc->uri);
    int port = evhttp_uri_get_port(hc->uri);
    if (port == -1) port = 80;

    hc->type        = IHttpRequest::Type::GET;
    hc->conn        = evhttp_connection_base_new(m_evbase, NULL, host, port);
    hc->req         = evhttp_request_new(HttpRequestCB, (void*)hc);

    ON_ERROR_PRINT_MSG_AND_DO(hc->uri,  == , NULL, "Fail to parse " << host, return false);
    ON_ERROR_PRINT_MSG_AND_DO(hc->conn, == , NULL, "Fail to call evhttp_connection_base_new().", return false);
    ON_ERROR_PRINT_MSG_AND_DO(hc->req,  == , NULL, "Fail to call evhttp_request_new().", return false);

    if (hc->type == IHttpRequest::Type::POST)
    {
        if (!_contentType || !_data)
        {
            delete hc;
            return NULL;
        }

        evbuffer_add(hc->req->output_buffer, _data, strlen(_data));
        evhttp_add_header(hc->req->output_headers, Request::Content::ContentType::Value, _contentType);
    }

    //
    // set the header properties
    //
    evhttp_add_header(hc->req->output_headers, Request::Header::Host::Value, host);

    /*
    evhttp_connection_set_timeout(hc->conn, 5);
    evhttp_connection_set_closecb(hc->conn, RemoteConnectionCloseCallback, hc);
    evhttp_request_set_header_cb(hc->req, ReadHeaderDoneCallback);
    evhttp_request_set_chunked_cb(hc->req, ReadChunkCallback);
    evhttp_request_set_error_cb(hc->req, RemoteRequestErrorCallback);
    */

    hc->status = INITIALIZED;
    return hc;
}

bool CWebVoyager::loadPage()
{
    HTTPClient_t* hc = createHttpRequset("http://blog.csdn.net/pcliuguangtao/article/details/9360331", 0, NULL, NULL);
    CHttpUtils::ShowUrlInfo(hc->uri);

    startRequest(hc);

    event_base_dispatch(m_evbase);
    return true;
}

#define PROCESS_RECORDS_BATCH_COUNT     20

void CWebVoyager::threadFunc()
{
    while (!isStop())
    {
        loadPage();
    }
}

bool CWebVoyager::start()
{
    if (!init())    return false;
    if (m_thread.get_id() != std::thread::id())   return true;

    m_thread = std::thread(&CWebVoyager::threadFunc, this);
    return m_thread.joinable();
}

void CWebVoyager::join()
{
    m_thread.join();
}