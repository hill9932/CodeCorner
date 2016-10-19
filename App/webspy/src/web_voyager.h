#ifndef __HL_WEB_VOYAGER_INCLUDE_H__
#define __HL_WEB_VOYAGER_INCLUDE_H__

#include "basic_manager.h"


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

    void* startHttpRequset(const char* _url, int _flag, const char* _type, const char* _data);

    /**
    * @Function: the thread function of lookup
    */
    void threadFunc();

    static void DownloadCallback(struct evhttp_request* _request,          void* _context);
    static int  ReadHeaderDoneCallback(struct evhttp_request* _request,      void* _context);
    static void ReadChunkCallback(struct evhttp_request* _request,           void* _context);
    static void RemoteRequestErrorCallback(enum evhttp_request_error _error, void* _context);
    static void RemoteConnectionCloseCallback(struct evhttp_connection* _connection, void* _context);

private:
    struct HTTPClient_t
    {
        struct evhttp_uri           *uri;
        struct evhttp_connection    *conn;
        struct evhttp_request       *req;
        struct evbuffer             *buf;
        string query;
        bool   finished;

        HTTPClient_t();
        ~HTTPClient_t();
    };

private:
    std::thread         m_thread;
    struct evdns_base*  m_dnsbase;
    struct event_base*  m_evbase;

};

#endif
