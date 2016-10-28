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
    int  getHCRecords(vector<HTTPClientRawPtr>& _records);

    void freeHC(HTTPClientRawPtr hc);

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
    bool updateRecord(HTTPClientRawPtr _hc);

    u_int32 getPendingRequest() { return m_issueCount - m_finishCount; }

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
    HTTPClientRawPtr createHttpRequset(
        u_int64 _id,
        const char* _url, 
        IHttpRequest::Type _requestType, 
        int _flag, 
        const char* _contentType, 
        const char* _data);

    bool startRequest(HTTPClientRawPtr _hc);
    void finishHttpRequest(HTTPClientRawPtr _hc);

private:
    std::thread             m_spiderThread;
    struct evdns_base*      m_dnsbase;
    struct event_base*      m_evbase;

    std::atomic_uint64_t    m_issueCount;
    std::atomic_uint64_t    m_finishCount;

    CMemCreator<HTTPClient_t>*  m_memAllocator;
    vector<HTTPClientRawPtr>   m_HCRecords;
    std::mutex              m_HCRecordsMutex;

    CGetWebPageTask m_getWebPageTask;
    CDigestTask             m_digestTask;

    CCollectURLTask         m_collectUrlTask;
    CDownloadTask           m_downloadTask;

    std::mutex              m_dbMutex;      // for performance sick, remove it in the future.
    sqlite3_stmt*           m_addRecordsStmt;
    sqlite3_stmt*           m_updateRecordsStmt;
};

#endif
