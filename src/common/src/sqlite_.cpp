/**
 * File:        sqlite_.cpp
 *
 * Copyright (C) 2014 - 2015  PolyVirtual, Inc.
 * All rights reserved.
 *
 * Abstract: 
 *
 */
#include "file_log.h"
#include "sqlite_.h"
#include "file_util.h"
#include "string_util.h"
#include "stdString.h"

namespace LabSpace
{
    namespace Common
    {
        int SQLite3Backup(sqlite3* src_db, sqlite3* dest_db)
        {
            int rc = SQLITE_ERROR;
            sqlite3_backup* db_backup = sqlite3_backup_init(dest_db, "main", src_db, "main");
            if (NULL != db_backup)
            {
                rc = sqlite3_backup_step(db_backup, -1);
                rc = sqlite3_backup_finish(db_backup);
            }
            return rc;
        }        

        CSqlLiteDB::CSqlLiteDB()
        {
            m_db = NULL;
            m_hasTransaction = false;
        }

        CSqlLiteDB::~CSqlLiteDB()
        {
            close();
        }

        int CSqlLiteDB::open(const tchar* _dbPath)
        {
            if (!_dbPath)  return -1;

            L4C_LOG_TRACE("Open db: " << _dbPath);

            close();
            int rc = sqlite3_open(_dbPath, &m_db);
            if (rc)
            {
                L4C_LOG_ERROR("Can't open database: " << sqlite3_errmsg(m_db));
                sqlite3_close(m_db);
                m_db = NULL;
            }
            else
                m_dbPath = _dbPath;

            return rc;
        }

        int CSqlLiteDB::close()
        {
            int z = 0;
            if (m_db)
            {
                L4C_LOG_TRACE("Close db: " << m_dbPath);

                commit();

                z = sqlite3_close(m_db);
                m_db = NULL;
            }

            return z;
        }

        sqlite3_stmt* CSqlLiteDB::compile(const tchar* _tableName, const tchar* _sql)
        {
            if (!m_db || !_sql)  return NULL;
            sqlite3_stmt* stmt = NULL;

            L4C_LOG_TRACE("Compile on " << m_dbPath << ": " << _sql);

            int z = sqlite3_prepare_v2(m_db, _sql, -1, &stmt, NULL);
            if (0 != z)
            {
                const char* msg = sqlite3_errmsg(m_db);
                L4C_LOG_ERROR(msg);
            }

            return stmt;
        }

        int CSqlLiteDB::doStmt(sqlite3_stmt* _stmt, const tchar* _fmt, ...)
        {
            if (!m_db || !_fmt)  return -1;

            if (!_stmt)  return -1;

            int z = 0;
            z = sqlite3_reset(_stmt);
            if (0 != z)
                L4C_LOG_ERROR(sqlite3_errmsg(m_db));

            va_list args;
            va_start(args, _fmt);

            int n = 0;
            vector<string> vec;
            StrUtil::StrSplit(_fmt, ",", vec);

            for (unsigned int i = 0; i < vec.size(); ++i)
            {
                ++n;
                CStdString key = vec[i];
                key.Trim();

                if (key == "%d")
                {
                    double v = va_arg(args, double);
                    z = sqlite3_bind_double(_stmt, n, v);
                    ON_ERROR_PRINT_MSG_AND_DO(z, != , 0, sqlite3_errmsg(m_db), return -2);
                }
                else if (key == "%u")
                {
                    int v = va_arg(args, int);
                    z = sqlite3_bind_int(_stmt, n, v);
                    ON_ERROR_PRINT_MSG_AND_DO(z, != , 0, sqlite3_errmsg(m_db), return -2);

                }
                else if (key == "%s")
                {
                    char* v = va_arg(args, char*);
                    z = sqlite3_bind_text(_stmt, n, v, strlen(v), SQLITE_STATIC);
                    ON_ERROR_PRINT_MSG_AND_DO(z, != , 0, sqlite3_errmsg(m_db), return -2);
                }
                else
                {
                    L4C_LOG_ERROR("Invalid type: " << key);
                    return -2;
                }
            }

            z = sqlite3_step(_stmt);
            if (SQLITE_OK != z && SQLITE_DONE != z)
            {
                sqlite3_reset(_stmt);

                if (SQLITE_CONSTRAINT != z)
                    L4C_LOG_ERROR(sqlite3_errmsg(m_db));
            }

            return z;
        }

        int CSqlLiteDB::exec(const tchar* _sql, void* _context, DB_CALLBACK _callback)
        {
            if (!m_db || !_sql)  return -1;

            L4C_LOG_TRACE("Do sql on " << m_dbPath << ": " << _sql);

            char *errMsg = NULL;
            int z = sqlite3_exec(m_db, _sql, _callback, _context, &errMsg);

            if (SQLITE_BUSY != z && 
                SQLITE_OK != z)
            {
                char* msg = errMsg;
                if (!msg)   msg = "unknown";

                ON_ERROR_PRINT_MSG(z, != , SQLITE_OK, m_dbPath << ": " << msg);
                if (errMsg) 
                    sqlite3_free(errMsg);
            }

            return z;
        }

        int CSqlLiteDB::beginTransact()
        {
            if (!m_db)  return -1;

            L4C_LOG_TRACE("Begin transaction: " << m_dbPath);

            char *errMsg = NULL;
            int z = sqlite3_exec(m_db, "begin;", 0, 0, &errMsg);
            if (z != SQLITE_OK)
            {
                L4C_LOG_ERROR(m_dbPath << ": " << errMsg ? errMsg : "");
                if (errMsg) sqlite3_free(errMsg); 
            }
            else
                m_hasTransaction = true;

            return z;
        }

        int CSqlLiteDB::commit()
        {
            if (!m_db)  return -1;
            if (!m_hasTransaction)  return 0;

            L4C_LOG_TRACE("Commit transaction: " << m_dbPath);

            char *errMsg = NULL;
            int z = sqlite3_exec(m_db, "commit;", NULL, NULL, &errMsg);
            if (z != SQLITE_OK)
            {
                L4C_LOG_ERROR(m_dbPath << ": " << errMsg ? errMsg : "");
                if (errMsg) sqlite3_free(errMsg);
            }

            m_hasTransaction = false;
            return z;
        }

        int CSqlLiteDB::flush()
        {
            commit();
            return beginTransact();
        }

        static int TableExist_Callback(void* _exist, int _argc, char** _argv, char** _azColName)
        {
            for (int i = 0; i < _argc; i++)
            {
                if (_exist)
                {
                    *(bool*)_exist = atoi(_argv[0]) == 1;
                    break;
                }
            }

            return 0;
        }

        int CSqlLiteDB::isTableExist(const tchar* _tableName)
        {
            if (!m_db || !_tableName)   return false;
            string sql = "SELECT COUNT(*) FROM SQLITE_MASTER WHERE type='table' AND name='";
            sql += _tableName;
            sql += "'";

            char *errMsg = NULL;
            bool isExist = false;
            int z = sqlite3_exec(m_db, sql.c_str(), TableExist_Callback, &isExist, &errMsg);
            ON_ERROR_PRINT_MSG_AND_DO(z, != , SQLITE_OK, errMsg, sqlite3_free(errMsg));
            return z == SQLITE_OK ? isExist : false;
        }


        static int TableFieldExist_Callback(void* _exist, int _argc, char** _argv, char** _azColName)
        {
            CStdString* result = (CStdString*)_exist;
            *result = _argv[0];

            return 0;
        }

        int CSqlLiteDB::isTableFieldExist(const tchar* _tableName, const tchar* _fieldName)
        {
            if (!m_db || !_tableName || !_fieldName)   return false;
            if (!isTableExist(_tableName))  return false;

            CStdString sql;
            sql.Format("select sql from sqlite_master where type = 'table' and name = '%s'", _tableName);

            char *errMsg = NULL;
            CStdString result;

            int z = sqlite3_exec(m_db, sql, TableFieldExist_Callback, &result, &errMsg);
            ON_ERROR_PRINT_MSG_AND_DO(z, != , SQLITE_OK, errMsg, sqlite3_free(errMsg));
            if (z != SQLITE_OK) return -1;

            int p = result.find(_fieldName);
            return p != string::npos;
        }
    }
}

