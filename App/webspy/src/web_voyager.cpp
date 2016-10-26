#include "web_voyager.h"
#include "task/task.h"

#include <evhttp.h>
#include <event2/http.h>
#include <event2/http_struct.h>


#define VOYAGER_RECORD_DB_NAME          "voyager_records.db"
#define MAIN_RECORD_TABLE_NAME          "VOYAGER_RECORDS"

#define MAIN_RECORD_TABLE_COL_CODE      "RESPONSE_CODE"
#define MAIN_RECORD_TABLE_COL_HIT_COUNT "HIT_COUNT"

CWebVoyager::CWebVoyager()
{
    m_dbName        = VOYAGER_RECORD_DB_NAME;
    m_tableName     = MAIN_RECORD_TABLE_NAME;
    m_getRecordsCB  = GetMainRecordCallback;

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

    recover();

    CMemCreator<HTTPClient_t>::GetInstance()->init();

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
    return "CREATE TABLE "                                                                  \
        MAIN_RECORD_TABLE_NAME "("                                                          \
        MAIN_RECORD_TABLE_COL_ID             " INTEGER PRIMARY KEY, "                       \
        MAIN_RECORD_TABLE_COL_CREATE_TIME    " INT NOT NULL, "                              \
        MAIN_RECORD_TABLE_COL_VISIT_TIME     " INT DEFAULT 0, "                             \
        MAIN_RECORD_TABLE_COL_NAME           " TEXT NOT NULL UNIQUE, "                      \
        MAIN_RECORD_TABLE_COL_CODE           " INT NOT NULL DEFAULT 0, "                    \
        MAIN_RECORD_TABLE_COL_HIT_COUNT      " INT NOT NULL DEFAULT 1, "                    \
        MAIN_RECORD_TABLE_COL_STATUS         " INT  NOT NULL);";
}

bool CWebVoyager::addRecords(const vector<tstring>& _records)
{
    if (_records.size() == 0)   return true;

    tstring sql = " INSERT INTO "   MAIN_RECORD_TABLE_NAME "(" \
                                    MAIN_RECORD_TABLE_COL_CREATE_TIME ", " \
                                    MAIN_RECORD_TABLE_COL_NAME ", " \
                                    MAIN_RECORD_TABLE_COL_STATUS ") VALUES ( ?, ?, ?);";
    int z = m_basicDB.compile(MAIN_RECORD_TABLE_NAME, sql.c_str());
    sqlite3_stmt* stmt = m_basicDB.getStatment(MAIN_RECORD_TABLE_NAME);
    if (!stmt)   return false;

    time_t now = time(NULL);
    m_basicDB.beginTransact();
    for (int i = 0; i < _records.size(); ++i)
    {
        z = sqlite3_reset(stmt);
        if (z == 0 &&
            0 == sqlite3_bind_int(stmt,  1, now) &&
            0 == sqlite3_bind_text(stmt, 2, _records[i].c_str(), _records[i].size(), SQLITE_STATIC) &&
            0 == sqlite3_bind_int(stmt,  3, HTTPClient_t::STATUS_UNCHECK))
        {
            z = sqlite3_step(stmt);
            if (SQLITE_DONE != z) // url existing
            {
                const char* msg = sqlite3_errmsg(m_basicDB);
                if (SQLITE_CONSTRAINT == z)
                {
                    L4C_LOG_TRACE(msg << ": " << _records[i]);
                }
                else
                {
                    L4C_LOG_ERROR(msg << ": " << _records[i]);
                }

                z = sqlite3_reset(stmt);
            }
        }
        else
        {
            const char* msg = sqlite3_errmsg(m_basicDB);
            L4C_LOG_ERROR(msg << ": " << _records[i]);
        }
    }

    z = m_basicDB.commit();
    ON_ERROR_PRINT_MSG_AND_DO(z, != , 0, "Fail to commit db.", return false);

    return true;
}

bool CWebVoyager::updateRecords()
{
    if (!m_HCRecords.size())   return false;

    tstring sql = " UPDATE " MAIN_RECORD_TABLE_NAME " SET " \
        MAIN_RECORD_TABLE_COL_VISIT_TIME "=?, "             \
        MAIN_RECORD_TABLE_COL_STATUS "=?, "                 \
        MAIN_RECORD_TABLE_COL_CODE "=?"                     \
        " WHERE ID = ?;";

    int z = m_basicDB.compile(MAIN_RECORD_TABLE_NAME, sql.c_str());
    sqlite3_stmt* stmt = m_basicDB.getStatment(MAIN_RECORD_TABLE_NAME);
    if (!stmt)   return false;

    time_t now = time(NULL);
    m_basicDB.beginTransact();
    {
        SCOPED_GUARD(m_HCRecordsMutex);
        vector<HTTPClientPtr>::iterator it = m_HCRecords.begin();
        for (; it != m_HCRecords.end(); ++it)
        {
            z = sqlite3_reset(stmt);
            if (z == 0 &&
                0 == sqlite3_bind_int(stmt, 1, now) &&
                0 == sqlite3_bind_int(stmt, 2, (*it)->status) &&
                0 == sqlite3_bind_int(stmt, 3, (*it)->respCode) &&
                0 == sqlite3_bind_int(stmt, 4, (*it)->id))
            {
                z = sqlite3_step(stmt);
                if (SQLITE_OK != z && SQLITE_DONE != z)
                {
                    const char* msg = sqlite3_errmsg(m_basicDB);
                    L4C_LOG_ERROR(msg);
                    sqlite3_reset(stmt);
                }
            }
        }
    }
    m_basicDB.commit();

    return true;
}

bool CWebVoyager::updateRecorde(HTTPClientPtr& _hc)
{
    time_t now = time(NULL);
    CStdString sql;
    sql.Format(" UPDATE " MAIN_RECORD_TABLE_NAME " SET "     \
                          MAIN_RECORD_TABLE_COL_VISIT_TIME "=%d, "             \
                          MAIN_RECORD_TABLE_COL_STATUS "=%d, "                 \
                          MAIN_RECORD_TABLE_COL_CODE "=%d"                     \
               " WHERE ID = %d;", 
               now, _hc->status, _hc->respCode, _hc->id);

    return 0 == doSql(sql, NULL);
}

void CWebVoyager::freeHC(HTTPClient_t* _hc)
{
    _hc->done();
}

void CWebVoyager::finishHttpRequest(HTTPClientPtr& _hc)
{
    if (_hc->respCode == HTTP_OK)
    {
        CHttpUtils::ShowRequestInfo(_hc->request);

        SCOPED_GUARD(m_HCRecordsMutex);
        m_HCRecords.push_back(_hc);
    }
    else
    {
        updateRecorde(_hc);
        freeHC(_hc.get());
    }
}

void CWebVoyager::HttpRequestCB(struct evhttp_request* _request, void* _arg)
{
    CWebVoyager* voyager = CWebVoyager::GetInstance();
    HTTPClient_t* hc = (HTTPClient_t*)_arg;

    if (!_arg)
    {
        hc->status = HTTPClient_t::STATUS_UNKNOWN;
        L4C_LOG_ERROR("Invalid parameters.");
    }
    else if (!_request)
    {
        hc->status = HTTPClient_t::STATUS_TIMEOUT;
        L4C_LOG_INFO("Timeout to " << hc->url);
    }
    else
    {
        L4C_LOG_INFO(hc->url << ": " << _request->response_code << " - " << _request->response_code_line);

        hc->status = HTTPClient_t::STATUS_EXCEPTION;
        hc->respCode = _request->response_code;

        switch (_request->response_code)
        {
            case HTTP_OK:
            {
                hc->status = HTTPClient_t::STATUS_DOWNLOADED;
                evhttp_request_own(hc->request);    // freed by CDigestTask
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

    voyager->finishHttpRequest(HTTPClientPtr(hc, NoneDeleter<HTTPClient_t>()));
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
    while (!hc && !isStop())
    {
        L4C_LOG_TRACE("Out of memory: Fail to allocate memory for http client.");

        SleepMS(1);
        hc = CMemCreator<HTTPClient_t>::GetInstance()->create();
    }

    if (!hc)
    {
        return hc;
    }

    hc->url  = _url;
    hc->uri  = evhttp_uri_parse(hc->url.c_str());
    if (!hc->uri)
    {
        L4C_LOG_ERROR("Fail to parse uri from: " << _url);
        freeHC(hc.get());
        return HTTPClientPtr();
    }

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
    evhttp_connection_set_timeout(hc->conn, 30);

    /*
    evhttp_connection_set_closecb(hc->conn, RemoteConnectionCloseCallback, hc);
    evhttp_request_set_header_cb(hc->req, ReadHeaderDoneCallback);
    evhttp_request_set_chunked_cb(hc->req, ReadChunkCallback);
    evhttp_request_set_error_cb(hc->req, RemoteRequestErrorCallback);
    */

    hc->status = HTTPClient_t::STATUS_INITIALIZED;
    return hc;
}

bool CWebVoyager::loadPage(URL_RECORD_t* _urlRecord)
{
    if (!_urlRecord || _urlRecord->name.empty()) return false;

    HTTPClientPtr hc = createHttpRequset(_urlRecord->name.c_str(), IHttpRequest::Type::GET, 0, NULL, NULL);
    if (!hc)    return false;

    hc->id = _urlRecord->id;
    startRequest(hc);
    
    return true;
}


int CWebVoyager::GetMainRecordCallback(void* _context, int _argc, char** _argv, char** _szColName)
{
    CWebVoyager* pThis = (CWebVoyager*)_context;
    URL_RECORD_t* record = pThis->m_getRecordsCBLocal;

    return 0;
}

int CWebVoyager::getUrlRecords(vector<URL_RECORD_t*>& _v)
{
    if (m_pendingRecords.size() == 0)
        getNextRecords(GetMainRecordCallback);

    _v.swap(m_pendingRecords);
    return _v.size();
}

int CWebVoyager::getHCRecords(vector<HTTPClientPtr>& _records)
{
    updateRecords();

    SCOPED_GUARD(m_HCRecordsMutex);
    _records.swap(m_HCRecords);
    return _records.size();
}

void CWebVoyager::spiderThreadFunc()
{
    while (!isStop())
    {
        int z = event_base_dispatch(m_evbase);
        ON_ERROR_PRINT_MSG_AND_DO(z, == , -1, "Fail to call event_base_dispatch().", break);
        ON_ERROR_PRINT_MSG_AND_DO(z, == , 1, "No more pending active.", SleepSec(3));

        L4C_LOG_INFO("Exit event loop.");
    }
}

bool CWebVoyager::start()
{
    if (!init())    return false;
    if (m_spiderThread.get_id() != std::thread::id())   return true;

    vector<string> v;
    const char* entrance = "http://www.cplusplus.com/reference/string/string/substr/";
    v.push_back(entrance);
    addRecords(v);

    CCollectPipeline::GetInstance()->addTask(m_collectUrlTask);
    CCollectPipeline::GetInstance()->addTask(m_downloadTask);
    CCollectPipeline::GetInstance()->start();

    CProcessPipeline::GetInstance()->addTask(m_firstProcessTask);
    CProcessPipeline::GetInstance()->addTask(m_digestTask);
    CProcessPipeline::GetInstance()->start();

    m_spiderThread = std::thread(&CWebVoyager::spiderThreadFunc, this);
    return m_spiderThread.joinable();
}

void CWebVoyager::stop()
{
    CProcessPipeline::GetInstance()->stop();
    CCollectPipeline::GetInstance()->stop();
    CBasicManager::stop();
    event_base_loopexit(m_evbase, NULL);
}

void CWebVoyager::join()
{
    CProcessPipeline::GetInstance()->join();
    CCollectPipeline::GetInstance()->join();
    m_spiderThread.join();
}