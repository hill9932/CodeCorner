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


#endif
