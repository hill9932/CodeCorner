#ifndef __HL_DNS_MANAGER_INCLUDE_H__
#define __HL_DNS_MANAGER_INCLUDE_H__

#include "basic_manager.h"


/**
 * @Function: Keep the dns information which includes name, ip addresses, last visited time, status
 **/
class CDNSManager : public CBasicManager, public ISingleton<CDNSManager>
{
public:
    CDNSManager();
    ~CDNSManager();

    bool start();
    void join();

    bool addWebName(const string& _name);

private:
    #define DNS_RECORD_TABLE_COL_ID         "ID"
    #define DNS_RECORD_TABLE_COL_NAME       "NAME"
    #define DNS_RECORD_TABLE_COL_CNAME      "CNAME"
    #define DNS_RECORD_TABLE_COL_ADDRESS    "ADDRESS"
    #define DNS_RECORD_TABLE_COL_CREATE_TIME "CREATE_TIME"
    #define DNS_RECORD_TABLE_COL_VISIT_TIME "LAST_VISIT_TIME"
    #define DNS_RECORD_TABLE_COL_STATUS     "STATUS"


    /**
     * @Function: do some init work such as open the database and prepare the libevent resource
     **/
    virtual bool    __init__();
    virtual string  __getTableCreateSql__();

    /**
     * @Function: use compile statement to add all the new web name to database
     **/
    bool addRecords();

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
            PROCESSING,         // doing dns lookup
            READY,              // dns lookup finished and has valid ips if possible
            RESOLVED            // web page grabbed
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
 
    std::thread         m_thread;

    struct evdns_base*  m_dnsbase;
    struct event_base*  m_evbase;

    list<DNS_RECORD_t>  m_pendingRecords;   // get from database
    vector<DNS_RECORD_t>m_newRecords;       // need to add to database
};


#endif
