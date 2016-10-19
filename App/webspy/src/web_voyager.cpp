#include "web_voyager.h"
#include "stdString.h"
#include "file_log.h"

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
        LOG_ERROR_STREAM << "Fail to init libevent.";
        return false;
    }

    m_evbase = event_base_new();
    if (!m_evbase)
    {
        LOG_ERROR_STREAM << "Fail to call event_base_new().";
        return false;
    }

    m_dnsbase = evdns_base_new(m_evbase, 1);
    if (!m_dnsbase)
    {
        LOG_ERROR_STREAM << "Fail to call evdns_base_new().";
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

void CWebVoyager::DownloadCallback(struct evhttp_request* _request, void* _context)
{
    HTTPClient_t *hc = (HTTPClient_t *)_context;
    hc->finished = true;

    event_base_loopexit(CWebVoyager::GetInstance()->m_evbase, NULL);
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
    buf     = NULL;
    query   = "/";
    finished = false;
}

CWebVoyager::HTTPClient_t::~HTTPClient_t()
{
    if (uri)    evhttp_uri_free(uri);
    if (req)    evhttp_request_free(req);
    if (conn)   evhttp_connection_free(conn);
    if (buf)    evbuffer_free(buf);
}

void* CWebVoyager::startHttpRequset(const char* _url, int _flag, const char* _type, const char* _data)
{
    if (!_url || !_type || !_data)  return NULL;

//    struct http_request_get *http_req_get = http_request_new(base, url, req_get_flag, content_type, data);
 //   start_url_request(http_req_get, req_get_flag);

 //   return http_req_get;
    return NULL;
}

bool CWebVoyager::loadPage()
{
    shared_ptr<HTTPClient_t> hc(new HTTPClient_t);

    struct timeval tv = { 3, 5000 };
    const char *host = "www.sina.com.cn";
    unsigned int port = 80;

    hc->uri  = evhttp_uri_parse(host);
    hc->conn = evhttp_connection_base_new(m_evbase, m_dnsbase, host, port);
    hc->req  = evhttp_request_new(DownloadCallback, hc.get());  
    hc->buf  = evbuffer_new();

    ON_ERROR_PRINT_MSG_AND_DO(hc->uri , == , NULL, "Fail to parse " << host, return false);
    ON_ERROR_PRINT_MSG_AND_DO(hc->conn, == , NULL, "Fail to call evhttp_connection_base_new().", return false);
    ON_ERROR_PRINT_MSG_AND_DO(hc->req,  == , NULL, "Fail to call evhttp_request_new().", return false);
    ON_ERROR_PRINT_MSG_AND_DO(hc->buf,  == , NULL, "Fail to call evbuffer_new().", return false);

    evhttp_connection_set_timeout(hc->conn, 5);
    evhttp_connection_set_closecb(hc->conn, RemoteConnectionCloseCallback, hc.get());

    evhttp_request_set_header_cb(hc->req,  ReadHeaderDoneCallback);
    evhttp_request_set_chunked_cb(hc->req, ReadChunkCallback);
    evhttp_request_set_error_cb(hc->req, RemoteRequestErrorCallback);

    evhttp_add_header(hc->req->output_headers, "Host", host);
    evhttp_add_header(hc->req->output_headers, "Content-Length", "0");
    evhttp_make_request(hc->conn, hc->req, EVHTTP_REQ_GET, hc->query.c_str());


    event_base_dispatch(m_evbase);
    return true;
}

#define PROCESS_RECORDS_BATCH_COUNT     20

void CWebVoyager::threadFunc()
{
    while (!isStop())
    {
        CStdString sql;
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