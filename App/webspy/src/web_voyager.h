#ifndef __HL_WEB_VOYAGER_INCLUDE_H__
#define __HL_WEB_VOYAGER_INCLUDE_H__

#include "basic_manager.h"
#include "task/digest_task.h"
#include "task/collect_task.h"


/**
 * @Function: This class download the http pages according to the dns names got from database
 **/
class CWebVoyager : public CBasicManager<URL_RECORD_t>, public ISingleton<CWebVoyager>
{
public:
    CWebVoyager();
    ~CWebVoyager();

    bool start();
    void stop();
    void join();

    
    int  getUrlRecords(vector<URL_RECORD_t*>& _records);
    int  getHCRecords(vector<HTTPClientPtr>& _records);

    void freeHC(HTTPClient_t* hc);

    /**
     * @Function: use compile statement to add all the new web name to database
     **/
    bool loadPage(URL_RECORD_t* _record);

    /**
     * @Function: add the urls to database
     * @Memo: these records are parsed from web page
     **/
    bool addRecords(const vector<tstring>& _records);

    /**
     * @Function: update the url records info after http request finished
     **/
    bool updateRecords();

private:
    /**
    * @Function: do some init work such as open the database and prepare the libevent resource
    **/
    virtual bool    __init__();
    virtual bool    __uninit__();
    virtual string  __getTableCreateSql__();

    /**
    * @Function: the thread function of lookup
    */
    void spiderThreadFunc();

    static void HttpRequestCB(struct evhttp_request* _request,  void* _arg);
    static int  GetMainRecordCallback(void* _context, int _argc, char** _argv, char** _szColName);

private:
    HTTPClientPtr createHttpRequset(
        const char* _url, 
        IHttpRequest::Type _requestType, 
        int _flag, 
        const char* _contentType, 
        const char* _data);

    bool startRequest(HTTPClientPtr& _hc);
    void finishHttpRequest(HTTPClientPtr& _hc);

private:
    std::thread         m_spiderThread;
    struct evdns_base*  m_dnsbase;
    struct event_base*  m_evbase;

    vector<HTTPClientPtr>   m_HCRecords;
    std::mutex              m_HCRecordsMutex;

    CFirstProcessTask       m_firstProcessTask;
    CDigestTask             m_digestTask;

    CCollectURLTask         m_collectUrlTask;
    CDownloadTask           m_downloadTask;
};

#endif
