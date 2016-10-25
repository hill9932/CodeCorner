#include "dns_manager.h"
#include "file_log.h"
#include "file_util.h"
#include "stdString.h"

int CDNSManager::s_pendingLookupCount = 0;

#define DNS_RECORD_DB_NAME          "dns_records.db"
#define MAIN_RECORD_TABLE_NAME      "DNS_RECORDS"

CDNSManager::CDNSManager()
{
    m_evbase    = NULL;
    m_dnsbase   = NULL;
    m_tableName = MAIN_RECORD_TABLE_NAME;
    m_dbName    = DNS_RECORD_DB_NAME;
    m_getRecordsCB = GetMainRecordCallback;
}

CDNSManager::~CDNSManager()
{
    if (m_dnsbase)  evdns_base_free(m_dnsbase, 0);
    if (m_evbase)   event_base_free(m_evbase);
}

bool CDNSManager::__init__()
{
    if (m_evbase && m_dnsbase)    return true;
    
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

string  CDNSManager::__getTableCreateSql__()
{
    return "CREATE TABLE "                                       \
        MAIN_RECORD_TABLE_NAME "("                                           \
        MAIN_RECORD_TABLE_COL_ID             " INTEGER PRIMARY KEY,"         \
        MAIN_RECORD_TABLE_COL_NAME           " TEXT NOT NULL UNIQUE,"        \
        DNS_RECORD_TABLE_COL_CNAME          " TEXT,"                        \
        DNS_RECORD_TABLE_COL_ADDRESS        " TEXT NOT NULL,"               \
        MAIN_RECORD_TABLE_COL_CREATE_TIME    " INT  NOT NULL,"               \
        MAIN_RECORD_TABLE_COL_VISIT_TIME     " INT  NOT NULL,"               \
        MAIN_RECORD_TABLE_COL_STATUS         " INT  NOT NULL);";
}

int CDNSManager::GetMainRecordCallback(void* _context, int _argc, char** _argv, char** _szColName)
{
    CDNSManager* manager = (CDNSManager*)_context;
    DNS_RECORD_t* record = manager->m_getRecordsCBLocal;

    for (int i = 0; i < _argc; ++i)
    {
        if (!_argv[i]) continue;

        else if (0 == stricmp(_szColName[i], DNS_RECORD_TABLE_COL_CNAME))
            record->cname = _argv[i];
        else if (0 == stricmp(_szColName[i], DNS_RECORD_TABLE_COL_ADDRESS))
            record->address  = _argv[i];
    }

    return 0;
}

void CDNSManager::LookupDNSCallback(int _errcode, struct evutil_addrinfo *_addr, void* _context)
{
    DNS_RECORD_t* record = (DNS_RECORD_t*)_context;
 
    if (_errcode)
    {
        LOG_ERROR_STREAM << "Fail to parse " << record->name << ": " << evutil_gai_strerror(_errcode);
    }
    else
    {
        record->address.clear();
        record->visitTime = time(NULL);
        record->status = MAIN_TABLE_RECORD_t::STATUS_e::STATUS_READY;
        if (_addr->ai_canonname)    record->cname = _addr->ai_canonname;

        //
        // get ip addresses
        //
        struct evutil_addrinfo  *ai;
        for (ai = _addr; ai; ai = ai->ai_next) 
        {
            char buf[128] = { 0 };
            const char* ipAddr = NULL;
            if (ai->ai_family == AF_INET) 
            {
                struct sockaddr_in* sin = (struct sockaddr_in *)ai->ai_addr;
                ipAddr = evutil_inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf));
            }
            else if (ai->ai_family == AF_INET6) 
            {
                struct sockaddr_in6* sin6 = (struct sockaddr_in6 *)ai->ai_addr;
                ipAddr = evutil_inet_ntop(AF_INET6, &sin6->sin6_addr, buf, sizeof(buf));
            }

            if (ipAddr)
            {
                record->address += ipAddr;
                record->address += ", ";
            }
        }

        evutil_freeaddrinfo(_addr);
    }

    if (--s_pendingLookupCount == 0)
        event_base_loopexit(CDNSManager::GetInstance()->m_evbase, NULL);        
}

void CDNSManager::lookupDNS(const DNS_RECORD_t& _record)
{
    struct evutil_addrinfo  hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = EVUTIL_AI_CANONNAME;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    struct evdns_getaddrinfo_request* req = evdns_getaddrinfo(
        m_dnsbase, _record.name.c_str(), NULL,
        &hints, LookupDNSCallback, (void*)&_record);
    ON_ERROR_PRINT_MSG(req, == , NULL, "Fail to call evdns_getaddrinfo.");
}

bool CDNSManager::addRecords()
{
    if (!m_newRecords.size())   return false;

    CStdString sql = " INSERT INTO " MAIN_RECORD_TABLE_NAME
        " VALUES (NULL, ?, ?, ?, ?, ?, ?);";

    int z = m_basicDB.compile(MAIN_RECORD_TABLE_NAME, sql);
    sqlite3_stmt* stmt = m_basicDB.getStatment(MAIN_RECORD_TABLE_NAME);
    if (!stmt)   return false;

    m_basicDB.beginTransact();
    for (int i = 0; i < m_newRecords.size(); ++i)
    {
        z = sqlite3_reset(stmt);
        if (z == 0 &&
            0 == sqlite3_bind_text(stmt, 1, m_newRecords[i].name.c_str(), m_newRecords[i].name.size(), SQLITE_STATIC) &&
            0 == sqlite3_bind_text(stmt, 2, m_newRecords[i].cname.c_str(), m_newRecords[i].cname.size(), SQLITE_STATIC) &&
            0 == sqlite3_bind_text(stmt, 3, m_newRecords[i].address.c_str(), m_newRecords[i].address.size(), SQLITE_STATIC) &&
            0 == sqlite3_bind_int(stmt, 4, m_newRecords[i].createTime) &&
            0 == sqlite3_bind_int(stmt, 5, m_newRecords[i].visitTime) &&
            0 == sqlite3_bind_int(stmt, 6, m_newRecords[i].status))
        {
            z = sqlite3_step(stmt);
            if (SQLITE_OK != z && SQLITE_DONE != z)
            {
                const char* msg = sqlite3_errmsg(m_basicDB);
                LOG_ERROR_STREAM << msg;
            }
        }
    }
    m_basicDB.commit();
    m_newRecords.clear();

    return true;
}

bool CDNSManager::updateRecords()
{
    if (!m_pendingRecords.size())   return false;

    CStdString sql = " UPDATE " MAIN_RECORD_TABLE_NAME " SET " \
        MAIN_RECORD_TABLE_COL_NAME "=?, "          \
        DNS_RECORD_TABLE_COL_CNAME "=?, "         \
        DNS_RECORD_TABLE_COL_ADDRESS "=?, "       \
        MAIN_RECORD_TABLE_COL_CREATE_TIME "=?, "   \
        MAIN_RECORD_TABLE_COL_VISIT_TIME "=?, "    \
        MAIN_RECORD_TABLE_COL_STATUS "=?"          \
        " WHERE ID = ?;";

    int z = m_basicDB.compile(MAIN_RECORD_TABLE_NAME, sql);
    sqlite3_stmt* stmt = m_basicDB.getStatment(MAIN_RECORD_TABLE_NAME);
    if (!stmt)   return false;

    m_basicDB.beginTransact();
    vector<DNS_RECORD_t*>::iterator it = m_pendingRecords.begin();
    for (; it != m_pendingRecords.end(); ++it)
    {
        z = sqlite3_reset(stmt);
        if (z == 0 &&
            0 == sqlite3_bind_text(stmt, 1, (*it)->name.c_str(), (*it)->name.size(), SQLITE_STATIC) &&
            0 == sqlite3_bind_text(stmt, 2, (*it)->cname.c_str(), (*it)->cname.size(), SQLITE_STATIC) &&
            0 == sqlite3_bind_text(stmt, 3, (*it)->address.c_str(), (*it)->address.size(), SQLITE_STATIC) &&
            0 == sqlite3_bind_int(stmt, 4, (*it)->createTime) &&
            0 == sqlite3_bind_int(stmt, 5, (*it)->visitTime) &&
            0 == sqlite3_bind_int(stmt, 6, (*it)->status) &&
            0 == sqlite3_bind_int(stmt, 7, (*it)->id))
        {
            z = sqlite3_step(stmt);
            if (SQLITE_OK != z && SQLITE_DONE != z)
            {
                const char* msg = sqlite3_errmsg(m_basicDB);
                LOG_ERROR_STREAM << msg;
                break;
            }
        }
    }
    m_basicDB.commit();
    m_pendingRecords.clear();

    return true;
}


void CDNSManager::threadFunc()
{
    while (!isStop())
    {
        if (m_pendingRecords.size() == 0)
        {
            getNextRecords(GetMainRecordCallback);
        }

        //
        // use libevent to do the dns lookup
        //
        s_pendingLookupCount = m_pendingRecords.size();
        vector<DNS_RECORD_t*>::iterator it = m_pendingRecords.begin();
        for (; it != m_pendingRecords.end(); ++it)
        {
            lookupDNS(**it);
        }

        if (s_pendingLookupCount == 0)
        {
            if (!addRecords())  // add the new records to database
                SleepMS(10);
        }
        else
        {
            event_base_dispatch(m_evbase);  // wait until all the lookup are finished
            updateRecords();
        }
    }
}

bool CDNSManager::start()
{
    if (!init())    return false;
    if (m_thread.get_id() != std::thread::id())   return true;

    m_thread = std::thread(&CDNSManager::threadFunc, this);
    return m_thread.joinable();
}

void CDNSManager::join()
{
    m_thread.join();
}

bool CDNSManager::addWebName(const string& _name)
{
    if (_name.empty())  return false;

    DNS_RECORD_t record;
    record.createTime   = time(NULL);
    record.name         = _name;
    record.status       = MAIN_TABLE_RECORD_t::STATUS_e::STATUS_UNCHECK;
    record.visitTime    = 0;

    m_newRecords.push_back(record);

    return true;
}

