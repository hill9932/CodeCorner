#ifndef __HL_BASIC_MANAGER_INCLUDE_H__
#define __HL_BASIC_MANAGER_INCLUDE_H__

#include "common.h"
#include "singleton.h"
#include "sqlite_.h"
#include "global.h"

#include <event2/dns.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/buffer.h>

#include <thread>
#include <list>
#include <vector>
#include <map>

using namespace LabSpace::Common;


#define PROCESS_RECORDS_BATCH_COUNT     20

#define MAIN_RECORD_TABLE_COL_ID            "ID"
#define MAIN_RECORD_TABLE_COL_CREATE_TIME   "CREATE_TIME"
#define MAIN_RECORD_TABLE_COL_VISIT_TIME    "VISIT_TIME"
#define MAIN_RECORD_TABLE_COL_NAME          "NAME"
#define MAIN_RECORD_TABLE_COL_STATUS        "STATUS"

typedef int(*GetRecordCallbackFunc)(void* _context, int _argc, char** _argv, char** _szColName);


template<class T>
class CBasicManager
{
protected:
    CBasicManager();

    void stop()   { m_stop = true; }
    bool isStop() { return m_stop; }
    bool init();

    int  doSql(const tchar* _sql, DB_CALLBACK _callback);
    int  recover();

    string getPendingRecordsID(vector<T*>& _records);
    int    getNextRecords(GetRecordCallbackFunc _callback);

private:
    virtual bool    __init__()              { return true;   }
    virtual bool    __uninit__()            { return true;   }
    virtual string  __getTableName__()      { return m_tableName; }
    virtual string  __getTableCreateSql__() { return ""; }

private:
    bool openDB(const string& _dbPath);

    static int  GetRecordCallback(void* _context, int _argc, char** _argv, char** _szColName);

protected:
    string              m_dbName;
    string              m_tableName;
    CSqlLiteDB          m_basicDB;
    vector<T*>          m_pendingRecords;   // get from database

    GetRecordCallbackFunc   m_getRecordsCB;

    static thread_local T*  m_getRecordsCBLocal;

private:
    bool                m_stop;
};

#include "basic_manager.hpp"

#endif
