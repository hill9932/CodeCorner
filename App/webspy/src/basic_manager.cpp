#include "basic_manager.h"
#include "stdString.h"

CBasicManager::CBasicManager()
{
    m_stop = false;
}

bool CBasicManager::openDB(const string& _dbPath)
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


bool CBasicManager::init()
{
    string dbPath = g_AppConfig.dbPath + "/";
    dbPath += m_dbName; 
    if (!openDB(dbPath))
        return false;

    return __init__();
}

int CBasicManager::doSql(const tchar* _sql, DB_CALLBACK _callback)
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
