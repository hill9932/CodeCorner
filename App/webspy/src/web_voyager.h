#ifndef __HL_WEB_VOYAGER_INCLUDE_H__
#define __HL_WEB_VOYAGER_INCLUDE_H__

#include "basic_manager.h"
#include "task/digest_task.h"


/**
 * @Function: This class download the http pages according to the dns names got from database
 **/
class CWebVoyager : public CBasicManager, public ISingleton<CWebVoyager>
{
public:
    CWebVoyager();
    ~CWebVoyager();

    bool start();
    void join();

    void swap(vector<HTTPClientPtr>& _v)
    {
        _v.swap(m_finishedReqPool);
    }

private:
    #define VOYAGER_RECORD_TABLE_COL_ID         "ID"
    #define VOYAGER_RECORD_TABLE_COL_NAME       "NAME"
    #define VOYAGER_RECORD_TABLE_COL_CREATE_TIME "CREATE_TIME"
    #define VOYAGER_RECORD_TABLE_COL_STATUS     "STATUS"


    /**
    * @Function: do some init work such as open the database and prepare the libevent resource
    **/
    virtual bool    __init__();
    virtual bool    __uninit__();
    virtual string  __getTableCreateSql__();

    /**
    * @Function: use compile statement to add all the new web name to database
    **/
    bool loadPage();


    /**
    * @Function: the thread function of lookup
    */
    void spiderThreadFunc();

    static void HttpRequestCB(struct evhttp_request* _request,  void* _arg);
    static int  ReadHeaderDoneCallback(struct evhttp_request* _request,      void* _context);
    static void ReadChunkCallback(struct evhttp_request* _request,           void* _context);
    static void RemoteRequestErrorCallback(enum evhttp_request_error _error, void* _context);
    static void RemoteConnectionCloseCallback(struct evhttp_connection* _connection, void* _context);

private:
    HTTPClientPtr createHttpRequset(
        const char* _url, 
        IHttpRequest::Type _requestType, 
        int _flag, 
        const char* _contentType, 
        const char* _data);

    bool startRequest(HTTPClientPtr& _hc);
    void finishRequest(HTTPClientPtr& _hc);

private:
    std::thread         m_spiderThread;
    struct evdns_base*  m_dnsbase;
    struct event_base*  m_evbase;

    vector<HTTPClientPtr>   m_finishedReqPool;
    std::mutex              m_poolMutex;

    CFirstTask              m_firstTask;
    CDigestTask             m_digestTask;
};

#endif
