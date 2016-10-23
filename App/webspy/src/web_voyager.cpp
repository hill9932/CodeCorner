#include "web_voyager.h"
#include "task/task.h"

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

    CMemCreator<HTTPClient_t>::GetInstance()->init();
    m_firstTask.init();
    m_digestTask.init();

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
        VOYAGER_RECORD_TABLE_COL_CREATE_TIME    " INT  NOT NULL,"               \
        VOYAGER_RECORD_TABLE_COL_NAME           " TEXT NOT NULL UNIQUE,"        \
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

HTTPClient_t::HTTPClient_t()
{
    uri = NULL;
    conn = NULL;
    request = NULL;
    status = STATUS_UNKNOWN;
    type = IHttpRequest::Type::NONE;
}

HTTPClient_t::~HTTPClient_t()
{
    if (uri)    evhttp_uri_free(uri);
    if (request)evhttp_request_free(request);
    if (conn)   evhttp_connection_free(conn);
}

void CWebVoyager::finishRequest(HTTPClientPtr& _hc)
{
    CHttpUtils::ShowRequestInfo(_hc->request);

    SCOPED_GUARD(m_poolMutex);
    m_finishedReqPool.push_back(_hc);
}

void CWebVoyager::HttpRequestCB(struct evhttp_request* _request, void* _arg)
{
    HTTPClient_t* hc = (HTTPClient_t*)_arg;
    if (!_arg)
    {
        L4C_LOG_ERROR("Invalid parameters.");
        return;
    }
    else if (!_request)
    {
        hc->status = HTTPClient_t::STATUS_TIMEOUT;
        L4C_LOG_ERROR("Timeout.");
        return;
    }
    
    L4C_LOG_INFO(hc->url << ": " << _request->response_code);

    hc->status = HTTPClient_t::STATUS_EXCEPTION;
    CWebVoyager* voyager = CWebVoyager::GetInstance();
    switch (_request->response_code)
    {
        case HTTP_OK:
        {
            hc->status = HTTPClient_t::STATUS_DOWNLOADED;
            evhttp_request_own(hc->request);
            voyager->finishRequest(HTTPClientPtr(hc, NoneDeleter<HTTPClient_t>()));
            break;
        }

        case 0:
            L4C_LOG_ERROR("Connection refused to " << hc->url);
            break;

        case HTTP_MOVETEMP:
        {
            const char* newLocation = evhttp_find_header(_request->input_headers, Response::Header::Location::Value);
            HTTPClientPtr newHC = voyager->createHttpRequset(newLocation, IHttpRequest::Type::GET, 0, NULL, NULL);
            voyager->startRequest(newHC);

            break;
        }
    }
}

bool CWebVoyager::startRequest(HTTPClientPtr& _hc)
{
    if (!_hc)   return false;

    L4C_LOG_INFO("Request " << _hc->url);

    const char* path = evhttp_uri_get_path(_hc->uri);
    if (_hc->type == IHttpRequest::Type::GET)
    {
        const char* query = evhttp_uri_get_query(_hc->uri);
        tstring pathQuery;

        if (path)   pathQuery = path;
        if (query)
        {
            pathQuery += "?";
            pathQuery += query;
        }

        evhttp_make_request(_hc->conn, _hc->request, EVHTTP_REQ_GET, pathQuery.empty() ? "/" : pathQuery.c_str());
    }
    else if (_hc->type == IHttpRequest::Type::POST)
    {
        evhttp_make_request(_hc->conn, _hc->request, EVHTTP_REQ_POST, path ? "/" : path);
    }

    CHttpUtils::ShowUrlInfo(_hc->uri);

    _hc->status = HTTPClient_t::STATUS_DOWNLOADING;
    return true;
}

HTTPClientPtr CWebVoyager::createHttpRequset(
    const char* _url, 
    IHttpRequest::Type _requestType, 
    int _flag, 
    const char* _contentType,
    const char* _data)
{
    if (!_url)  return NULL;

    HTTPClientPtr hc = CMemCreator<HTTPClient_t>::GetInstance()->create();
    if (!hc)
    {
        L4C_LOG_INFO("Out of memory: Fail to allocate memory for http client.");
        return hc;
    }

    hc->url  = _url;
    hc->uri  = evhttp_uri_parse(hc->url.c_str());

    const char* host = evhttp_uri_get_host(hc->uri);
    int port = evhttp_uri_get_port(hc->uri);
    if (port == -1) port = 80;

    hc->type    = _requestType; 
    hc->conn    = evhttp_connection_base_new(m_evbase, NULL, host, port);
    hc->request = evhttp_request_new(HttpRequestCB, (void*)hc.get());

    ON_ERROR_PRINT_MSG_AND_DO(hc->uri,  == , NULL, "Fail to parse " << host, return false);
    ON_ERROR_PRINT_MSG_AND_DO(hc->conn, == , NULL, "Fail to call evhttp_connection_base_new().", return false);
    ON_ERROR_PRINT_MSG_AND_DO(hc->request,  == , NULL, "Fail to call evhttp_request_new().", return false);

    if (hc->type == IHttpRequest::Type::POST)
    {
        if (!_contentType || !_data)
        {
            return NULL;
        }

        evbuffer_add(hc->request->output_buffer, _data, strlen(_data));
        evhttp_add_header(hc->request->output_headers, Request::Content::ContentType::Value, _contentType);
    }

    //
    // set the header properties
    //
    evhttp_add_header(hc->request->output_headers, Request::Header::Host::Value, host);
    evhttp_add_header(hc->request->output_headers, Request::Header::UserAgent::Value, "HL WEBSPY");
    evhttp_connection_set_timeout(hc->conn, 5);

    /*
    evhttp_connection_set_closecb(hc->conn, RemoteConnectionCloseCallback, hc);
    evhttp_request_set_header_cb(hc->req, ReadHeaderDoneCallback);
    evhttp_request_set_chunked_cb(hc->req, ReadChunkCallback);
    evhttp_request_set_error_cb(hc->req, RemoteRequestErrorCallback);
    */

    hc->status = HTTPClient_t::STATUS_INITIALIZED;
    return hc;
}

bool CWebVoyager::loadPage()
{
    const char* entrance = "http://blog.csdn.net/pcliuguangtao/article/details/9360331";
    HTTPClientPtr hc = createHttpRequset(entrance, IHttpRequest::Type::GET, 0, NULL, NULL);
    startRequest(hc);

    event_base_dispatch(m_evbase);
    return true;
}

#define PROCESS_RECORDS_BATCH_COUNT     20

void CWebVoyager::spiderThreadFunc()
{
    while (!isStop())
    {
        loadPage();
    }
}

bool CWebVoyager::start()
{
    if (!init())    return false;
    if (m_spiderThread.get_id() != std::thread::id())   return true;

    CTaskManager::GetInstance()->addTask(m_firstTask);
    CTaskManager::GetInstance()->addTask(m_digestTask);
    CTaskManager::GetInstance()->start();

    m_spiderThread = std::thread(&CWebVoyager::spiderThreadFunc, this);
    return m_spiderThread.joinable();
}

void CWebVoyager::join()
{
    m_spiderThread.join();
}