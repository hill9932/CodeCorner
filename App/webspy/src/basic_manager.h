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


class CBasicManager
{
protected:
    CBasicManager();

    void stop()   { m_stop = true; }
    bool isStop() { return m_stop; }
    bool init();
    int  doSql(const tchar* _sql, DB_CALLBACK _callback);

private:
    virtual bool    __init__()              { return true;   }
    virtual bool    __uninit__()            { return true;   }
    virtual string  __getTableName__()      { return m_tableName; }
    virtual string  __getTableCreateSql__() { return ""; }

private:
    bool openDB(const string& _dbPath);

protected:
    string              m_dbName;
    string              m_tableName;
    CSqlLiteDB          m_basicDB;

private:
    bool                m_stop;
};

#endif
