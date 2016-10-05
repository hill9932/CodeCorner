#ifndef HL_DNS_MANAGER_INCLUDE_H__
#define HL_DNS_MANAGER_INCLUDE_H__

#include "common.h"
#include "singleton.h"
#include "sqlite_.h"

#include <event2/dns.h>
#include <event2/util.h>
#include <event2/event.h>
#include <thread>
#include <list>
#include <vector>

using namespace LabSpace::Common;


/**
 * @Function: Keep the dns information which includes name, ip addresses, last visited time, status
 **/
class CDNSManager : public ISingleton<CDNSManager>
{
public:
    CDNSManager();
    ~CDNSManager();

    bool start();
    void join();

    bool addWebName(const string& _name);

private:
    #define DNS_RECORD_TABLE_NAME           "DNS_RECORDS"
    #define DNS_RECORD_TABLE_COL_ID         "ID"
    #define DNS_RECORD_TABLE_COL_NAME       "NAME"
    #define DNS_RECORD_TABLE_COL_CNAME      "CNAME"
    #define DNS_RECORD_TABLE_COL_ADDRESS    "ADDRESS"
    #define DNS_RECORD_TABLE_COL_CREATE_TIME "CREATE_TIME"
    #define DNS_RECORD_TABLE_COL_VISIT_TIME "LAST_VISIT_TIME"
    #define DNS_RECORD_TABLE_COL_STATUS     "STATUS"

    bool isStop() { return m_stop; }  

    /**
     * @Function: do some init work such as open the database and prepare the libevent resource
     **/
    bool init();
    bool openDB(const string& _dbPath);   

    /**
     * @Function: use compile statement to add all the new web name to database
     **/
    bool addRecords();
    int  doSql(const tchar* _sql, DB_CALLBACK _callback);

    string getPendingRecords();

    /**
     * @Function: the thread function of lookup
     *  1. get the web names from database
     *  2. use libevent to lookup the dns
     *  3. update database of the finished lookup
     *  4. add the new web names to database
     */
    void threadFunc();

    static int  GetDNSRecordCallback(void* _context, int _argc, char** _argv, char** _szColName);
    static void LookupDNSCallback(int _errcode, struct evutil_addrinfo *_addr, void* _context);

private:
    struct DNS_RECORD_t
    {
        enum STATUS_e
        {
            UNCHECK     = 0,
            PROCESSING,
            READY
        };

        u_int32     id;
        u_int32     createTime;
        u_int32     visitTime;
        STATUS_e    status;
        string      name;
        string      cname;
        string      address;
    };

    void lookupDNS(const DNS_RECORD_t& _record);
    bool updateRecords();

private:
    static int          s_pendingLookupCount;
    string              m_confDir;
    CSqlLiteDB          m_dnsDB;
    std::thread         m_thread;
    bool                m_stop;

    struct evdns_base*  m_dnsbase;
    struct event_base*  m_evbase;

    list<DNS_RECORD_t>  m_pendingRecords;   // get from database
    vector<DNS_RECORD_t>m_newRecords;       // need to add to database
};


#endif
