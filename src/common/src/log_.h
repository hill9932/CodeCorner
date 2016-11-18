#ifndef __HL_LOG_INCLUDE_H__
#define __HL_LOG_INCLUDE_H__

#include "common.h"
#include "string_util.h"
#include "time_.h"
#include "buffer_.h"
#include "locker_.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/ndc.h>

#include <list>
#include <memory>
#include <atomic>

using namespace LabSpace::Common;

extern log4cplus::Logger* g_logger;


#define PRINT_MSG(msg)  std::cerr   << TimeUtil::GetNowTimeStr() << "\t" \
                                    << StrUtil::GetFileName(__FILE__).c_str() << ":" << __LINE__ << " - " \
                                    << msg << endl;

//#define LOG_FILE_LINE_INFORMATION
#ifdef LOG_FILE_LINE_INFORMATION

#define L4C_LOG_TRACE(str)       {   \
    if (g_logger) { LOG4CPLUS_TRACE(*g_logger, " " << \
                    StrUtil::GetFileName(__FILE__) << ":" << __FUNCTION__ << "():" << __LINE__ << " " << str); }\
}

#define L4C_LOG_DEBUG(str)       {   \
    if (g_logger) { LOG4CPLUS_DEBUG(*g_logger, " " << \
                    StrUtil::GetFileName(__FILE__) << ":" << __FUNCTION__ << "():" << __LINE__ << " " << str); }\
}

#define L4C_LOG_INFO(str)        {   \
    if (g_logger) { LOG4CPLUS_INFO(*g_logger,  " " << \
                    StrUtil::GetFileName(__FILE__) << ":" << __FUNCTION__ << "():" << __LINE__ << " " << str);  }\
}

#define L4C_LOG_WARNING(str)     {   \
    if (g_logger) { LOG4CPLUS_WARN(*g_logger,  " " << \
                    StrUtil::GetFileName(__FILE__) << ":" << __FUNCTION__ << "():" << __LINE__ << " " << str); } \
}

#define L4C_LOG_ERROR(str)     {   \
    if (g_logger) { LOG4CPLUS_ERROR(*g_logger,  " " << \
                    StrUtil::GetFileName(__FILE__) << ":" << __FUNCTION__ << "():" << __LINE__ << " " << str); } \
}

#else

#define L4C_LOG_TRACE(str)       {   \
    if (g_logger) { LOG4CPLUS_TRACE(*g_logger, " " << str); }   \
}

#define L4C_LOG_DEBUG(str)       {   \
    if (g_logger) { LOG4CPLUS_DEBUG(*g_logger, " " << str); }   \
}

#define L4C_LOG_INFO(str)        {   \
    if (g_logger) { LOG4CPLUS_INFO(*g_logger,  " " << str);  }  \
}

#define L4C_LOG_WARNING(str)     {   \
    if (g_logger) { LOG4CPLUS_WARN(*g_logger,  " " << str); }   \
}

#define L4C_LOG_ERROR(str)     {   \
    if (g_logger) { LOG4CPLUS_ERROR(*g_logger,  " " << str); }   \
}

#endif


//
// Check the value and log the error messages
//
#define LOG_LAST_ERRORMSG()                     \
{   u_int32 err = Util::GetLastSysError();      \
    L4C_LOG_ERROR(Util::GetLastSysErrorMessage(err).c_str());  \
}

#define LOG_LAST_ERRORMSG_S(msg)                \
{   u_int32 err = Util::GetLastSysError();      \
    L4C_LOG_ERROR(msg << ": " << Util::GetLastSysErrorMessage(err).c_str());\
}

#define LOG_ERRORMSG(errCode)           L4C_LOG_ERROR(Util::GetLastSysErrorMessage(errCode).c_str());
#define LOG_ERRORMSG_S(errCode, msg)    L4C_LOG_ERROR(msg << ": " << Util::GetLastSysErrorMessage(errCode).c_str());

#define LOG_LASTERROR_AND_RETURN_ERRCODE()      \
{                                               \
    int err = Util::GetLastSysError();          \
    if (0 != err)   LOG_ERRORMSG(err);          \
    return err;                                 \
}

#define LOG_LASTERROR_AND_DO(action)            \
{                                               \
    int err = Util::GetLastSysError();          \
    if (0 != err)  { LOG_ERRORMSG(err);         \
        action;                                 \
    }                                           \
}

#define LOG_LASTERROR_AND_RETURN()              \
{                                               \
    int err = Util::GetLastSysError();          \
    if (0 != err)   LOG_ERRORMSG(err);          \
    return;                                     \
}

#define ON_ERROR_RETURN_ERRCODE(expr, comp, error)  if (expr comp error)    return expr;
#define ON_ERROR_PRINT_LAST_ERROR(expr, comp, error)    \
    if (expr comp error)	{                   \
        u_int32 err = Util::GetLastSysError();  \
        LOG_ERRORMSG(err);                      \
    }

#define ON_ERROR_PRINT_LASTMSG_AND_DO(expr, comp, error, action)    \
    if (expr comp error)	{                   \
        u_int32 err = Util::GetLastSysError();  \
        LOG_ERRORMSG(err);                      \
        action;	                                \
    }

#define ON_ERROR_PRINT_MSG(expr, comp, error, msg)    \
    if (expr comp error)	{                   \
        L4C_LOG_ERROR(msg)                       \
    }

#define ON_ERROR_PRINT_MSG_AND_DO(expr, comp, error, msg, action)    \
    if (expr comp error)	{                   \
        L4C_LOG_ERROR(msg)                       \
        action;	                                \
    }

#define ON_ERROR_PRINT_LASTMSG_S_AND_DO(expr, comp, error, msg, action)    \
    if (expr comp error)	{                   \
        u_int32 err = Util::GetLastSysError();  \
        LOG_ERRORMSG_S(err, msg)                \
        action;	                                \
    }


/**
 * @Function: My own logger function
 **/
namespace LabSpace
{
    namespace Common
    {
        bool InitLog(const tstring& _configure, const tstring& _category);

        #define LOG_BUFFER_SIZE         1024
        #define INIT_LOG_BUFFER_COUNT   100
        #define MAX_LOG_BUFFER_COUNT    1000
        #define MIN_AVAILABLE_SIZE      32

        typedef FixedBuffer<LOG_BUFFER_SIZE>  Buffer;
        typedef std::shared_ptr<Buffer>         BufferPtr;
        
        /**
         * @Function: Get the file name from full file path usually from __FILE__ macro
         * @Memo: The file path length can be deduced at compile time by template
         **/
        class SourceFile
        {
        public:
            template<int N>
            inline SourceFile(const tchar(&arr)[N])
                : m_data(arr), m_size(N - 1)
            {
                string path = m_data;
                int pos = path.find_last_of("/\\");
                if (pos != string::npos)
                {
                    m_data += pos + 1;
                    m_size -= static_cast<int>(m_data - arr);
                }
            }

            explicit SourceFile(const tchar* filename)
                : m_data(filename)
            {
                string path = m_data;
                int pos = path.find_last_of("/\\");
                if (pos != string::npos)
                {
                    m_data += pos + 1;
                }
                m_size = static_cast<int>(strlen(m_data));
            }

            const tchar* m_data;
            int m_size;
        };


        /**
         * @Function: accept different input and keep the values as string
         **/
        class LogStream : public boost::noncopyable
        {
        public:
            typedef LogStream self;
            LogStream();

            self& operator << (const void* _p);
            self& operator << (const tstring& _message);
            self& operator << (const tchar* _message);
            self& operator << (int _message);
            self& operator << (u_int32 _message);
            self& operator << (u_int64 _message);
            self& operator << (double _message);
            self& operator << (std::ostream& (*pf)(std::ostream&));
            self& operator << (const SourceFile&);

            void flush();

            static void initStatic();
            static void getBuffers(list<BufferPtr>& _swap);
            static void setBuffer(const BufferPtr& _buf);
            static void setBuffers(list<BufferPtr>& _bufs);
            static u_int32 getBuffersCount() { return s_buferCount; }

        private:
            void nextBuffer();
            void notify();
            void append(const tchar* _message, int _size);
            inline void checkBuffer(int _size = MIN_AVAILABLE_SIZE);

        private:
            static  std::mutex              s_listMutex;
            static  std::condition_variable s_listCV;
            static  std::list<BufferPtr>    s_bufferList;   // buffer pool for all the streams
            static  std::list<BufferPtr>    s_savingList;   // finished buffers by all the streams
            static  std::atomic<int32>      s_buferCount;
            BufferPtr                       m_curBuffer;
        };


        /**
         * @Function: use file to keep the logger
         **/
        class CLogger : public boost::noncopyable
        {
        public:
            enum LogLevel
            {
                NONE    = 0,
                TRACE,
                DEBUG,
                INFO,
                WARN,
                ERR,
                FATAL
            }; 

        public:
            CLogger();
            CLogger(const SourceFile& _file, int _line, const char* _func, LogLevel _level);
            ~CLogger();

            LogStream& stream() { return m_impl.m_logStream; }

            static LogLevel getLogLevel()  { return s_logLevel; }
            static void setLogLevel(LogLevel _level)       { s_logLevel = _level; }
            static void getBuffers(list<BufferPtr>& _swap);        
            static u_int32 getBuffersCount() { return LogStream::getBuffersCount(); }

            static void initStatic();

        protected:
            static LogLevel         s_logLevel;
            static tstring          s_processName;

            class Impl
            {
            public:
                typedef CLogger::LogLevel LogLevel;
                friend class CLogger;

                Impl();
                Impl(LogLevel level, int old_errno, const SourceFile& file, int line);

                void finish();

            private:                
                LogStream   m_logStream;
            } m_impl;

        };
    }
}


#endif
