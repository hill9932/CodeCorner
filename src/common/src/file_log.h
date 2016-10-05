#ifndef __HL_FILE_LOG_H_INCLUDE__
#define __HL_FILE_LOG_H_INCLUDE__

#include "log_.h"
#include "file_.h"
#include <thread>

namespace LabSpace
{
    namespace Common
    {
        class CFileLogger : public CLogger
        {
        public:
            CFileLogger();
            ~CFileLogger();

            tstring getFileName();
            bool setLogPath(const tstring& _path);
            bool start();
            void stop();
            void join();

        private:
            void init();
            void threadFunc();
            bool newFile(bool _override);
            int  saveBuffers();

        private:
            std::thread     m_thread;
            CHFile          m_ofs;
            bool            m_stop;
            
            struct LoggerConf
            {
                tstring         logPath;
                u_int32         fileSize;
                u_int32         fileCount;

                time_t          createTime;
                tstring         fileName;
                u_int32         fileIndex;
            } m_conf;
        };
    }
}


#define LOG_TRACE_STREAM if (CFileLogger::getLogLevel()    <= CFileLogger::TRACE)      \
  CLogger(__FILE__, __LINE__, __FUNCTION__, CFileLogger::TRACE).stream()

#define LOG_DEBUG_STREAM if (CFileLogger::getLogLevel()    <= CFileLogger::DEBUG)      \
  CLogger(__FILE__, __LINE__, __FUNCTION__, CFileLogger::DEBUG).stream()

#define LOG_INFO_STREAM if (CFileLogger::getLogLevel()     <= CFileLogger::INFO)       \
  CLogger(__FILE__, __LINE__, __FUNCTION__, CFileLogger::INFO).stream()

#define LOG_WARN_STREAM if (CFileLogger::getLogLevel()     <= CFileLogger::WARN)       \
  CLogger(__FILE__, __LINE__, __FUNCTION__, CFileLogger::WARN).stream()

#define LOG_ERROR_STREAM if (CFileLogger::getLogLevel()    <= CFileLogger::ERR)        \
  CLogger(__FILE__, __LINE__, __FUNCTION__, CFileLogger::ERR).stream()

#define LOG_FATAL_STREAM if (CFileLogger::getLogLevel()    <= CFileLogger::FATAL)      \
  CLogger(__FILE__, __LINE__, __FUNCTION__, CFileLogger::FATAL).stream()

#endif
