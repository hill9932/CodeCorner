#include "basic_manager.h"
#include "stdString.h"

template<class T>
CBasicManager<T>::CBasicManager()
{
    m_stop = false;
    m_getRecordsCB = NULL;
}

template<class T>
bool CBasicManager<T>::openDB(const string& _dbPath)
{
    string dbName = __getTableName__();
    if (dbName.empty()) return false;

    if (0 != m_basicDB.open(_dbPath.c_str()))
    {
        L4C_LOG_ERROR("Fail to open database: " << _dbPath);
        return false;
    }

    if (0 == m_basicDB.exec("PRAGMA synchronous = NORMAL", this, NULL) &&
        0 == m_basicDB.exec("PRAGMA journal_mode = WAL", this, NULL))
    {
    }

    if (!m_basicDB.isTableExist(dbName.c_str()))
    {
        string sql = __getTableCreateSql__();
        if (sql.empty())    return false;

        int z = m_basicDB.exec(sql.c_str(), this, NULL);
        if (0 == z)
        {
            /*
            sql = "CREATE INDEX index_FPT ON " FILE_STATUS_TABLE " (FIRST_PACKET_TIME)";
            m_dbs[_index].exec(sql, this, NULL);
            */
        }
        else
        {
            L4C_LOG_ERROR("Fail to execute: " << sql);
            return false;
        }
    }

    return true;
}

template<class T>
bool CBasicManager<T>::init()
{
    string dbPath = g_AppConfig.dbPath + "/";
    dbPath += m_dbName; 
    if (!openDB(dbPath))
        return false;

    return __init__();
}

template<class T>
int CBasicManager<T>::doSql(const tchar* _sql, DB_CALLBACK _callback)
{
    int z = 0;
    do
    {
        z = m_basicDB.exec(_sql, this, _callback);
        if (z != 0 && z != SQLITE_BUSY)
            ON_ERROR_PRINT_MSG_AND_DO(z, != , 0, "Fail to do " << _sql, SleepUS(1));

        if (z == SQLITE_BUSY) SleepMS(1);

    } while (z == SQLITE_BUSY && !isStop());

    return z;
}

template<class T>
string CBasicManager<T>::getPendingRecordsID(vector<T*>& _records)
{
    string records;
    if (_records.size())
    {
        records = "(";

        vector<T*>::const_iterator it = _records.begin();
        int i = 0;
        for (; it != _records.end(); ++it)
        {
            CStdString tmp;
            tmp.Format("%d", (*it)->id);
            records += tmp;

            if (++i != _records.size())
                records += ",";
        }
        records += ")";
    }

    return records;
}


template<class T>
thread_local T* CBasicManager<T>::m_getRecordsCBLocal;

template<class T>
int CBasicManager<T>::GetRecordCallback(void* _context, int _argc, char** _argv, char** _szColName)
{
    CBasicManager* pThis = (CBasicManager*)_context;
    T* record = new T;

    for (int i = 0; i < _argc; ++i)
    {
        if (!_argv[i]) continue;

        if (0 == stricmp(_szColName[i], MAIN_RECORD_TABLE_COL_NAME))
            record->name = _argv[i];
        else if (0 == stricmp(_szColName[i], MAIN_RECORD_TABLE_COL_VISIT_TIME))
            record->visitTime = atoi64(_argv[i]);
        else if (0 == stricmp(_szColName[i], MAIN_RECORD_TABLE_COL_CREATE_TIME))
            record->createTime = atoi64(_argv[i]);
        else if (0 == stricmp(_szColName[i], MAIN_RECORD_TABLE_COL_STATUS))
            record->status = (MAIN_TABLE_RECORD_t::STATUS_e)atoi(_argv[i]);
        else if (0 == stricmp(_szColName[i], MAIN_RECORD_TABLE_COL_ID))
            record->id = atoi64(_argv[i]);
    }

    if (pThis->m_getRecordsCB)
    {
        m_getRecordsCBLocal = record;
        pThis->m_getRecordsCB(_context, _argc, _argv, _szColName);
        m_getRecordsCBLocal = NULL;
    }

    pThis->m_pendingRecords.push_back(record);
    return 0;
}

template<class T>
int CBasicManager<T>::getNextRecords(GetRecordCallbackFunc _callback)
{
    CStdString sql;
    sql.Format("SELECT * FROM %s WHERE STATUS = %d ORDER BY " MAIN_RECORD_TABLE_COL_CREATE_TIME " ASC LIMIT %d",
        __getTableName__(), MAIN_TABLE_RECORD_t::STATUS_UNCHECK, PROCESS_RECORDS_BATCH_COUNT);
    doSql(sql, GetRecordCallback);

    //
    // update the build state to building
    //
    if (m_pendingRecords.size())
    {
        sql.Format("UPDATE %s SET " MAIN_RECORD_TABLE_COL_STATUS " = %d WHERE ID IN %s",
            __getTableName__(),
            MAIN_TABLE_RECORD_t::STATUS_PROCESSING, getPendingRecordsID(m_pendingRecords).c_str());
        doSql(sql, NULL);
    }

    return m_pendingRecords.size();
}

template<class T>
int CBasicManager<T>::recover()
{
    CStdString sql;
    sql.Format("UPDATE %s SET STATUS = %d WHERE STATUS = %d ",
        __getTableName__(),
        MAIN_TABLE_RECORD_t::STATUS_UNCHECK, MAIN_TABLE_RECORD_t::STATUS_PROCESSING);
    int z = doSql(sql, NULL);

    ON_ERROR_PRINT_MSG(z, != , 0, "Fail to recover the database.");
    return z;
}