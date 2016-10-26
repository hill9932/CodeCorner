#ifndef __HL_SQLITE_INCLUDE_H__
#define __HL_SQLITE_INCLUDE_H__

#include "common.h"
#include "sqlite3.h"

#include <map>

typedef int(*DB_CALLBACK)(void*, int, char**, char**);

namespace LabSpace
{
    namespace Common
    {
        /**
         * @Function: transfer between two database.
         **/
        int SQLite3Backup(sqlite3* src_db, sqlite3* dest_db);

        /**
         * 1. Create the object using sqlite3_prepare_v2() or a related function.
         * 2. Bind values to host parameters using the sqlite3_bind_*() interfaces.
         * 3. Run the SQL by calling sqlite3_step() one or more times.
         * 4. Reset the statement using sqlite3_reset() then go back to step 2. Do this zero or more times.
         * 5. Destroy the object using sqlite3_finalize().
         **/
        class CSqlLiteDB
        {
        public:
            CSqlLiteDB();
            ~CSqlLiteDB();

            int  open(const tchar* _db_path);
            int  close();
            sqlite3_stmt* compile(const tchar* _tableName, const tchar* _sql);

            /**
             * @Function: execute the sql statement.
             * @Param _fmt: the format string, support %d (double), %u (numeric), %s (string)
             * @Param ...: the values
             * @Return:
             *  -1  failed
             *  -2  invalid format
             **/
            int  doStmt(sqlite3_stmt* _stmt, const tchar* _fmt, ...);
            int  exec(const tchar* _sql, void* _context, DB_CALLBACK _callback = NULL);
            int  beginTransact();
            int  commit();
            int  flush();

            /**
             * @Return 1: exist
             *         0: not exist
             *        -1: error
             **/
            int  isTableExist(const tchar* _tableName);
            int  isTableFieldExist(const tchar* _tableName, const tchar* _fieldName);

            operator sqlite3* ()    { return m_db; }
            string getDbPath()      { return m_dbPath; }
            bool isValid()          { return m_db != NULL; }

        private:
            string          m_dbPath;
            sqlite3*        m_db;
            bool            m_hasTransaction;
        };
    }
}

#endif
