#include "file_log.h"
#include "stdString.h"
#include "file_util.h"
#include "system_.h"

namespace LabSpace
{
    namespace Common
    {
        CFileLogger::CFileLogger()
        {
            init();
        } 

        CFileLogger::~CFileLogger()
        {
            stop();
            join();
        }

        void CFileLogger::init()
        {
            m_stop = false;
            m_conf.logPath    = ".";
            m_conf.createTime = 0;
            m_conf.fileSize   = ONE_MB;
            m_conf.fileCount  = 3;
            m_conf.fileIndex  = 1;
        }

        bool CFileLogger::setLogPath(const tstring& _path)
        {
            if (!FileUtil::CanDirAccess(_path)) return false;

            m_conf.logPath = _path;
            return true;
        }

        tstring CFileLogger::getFileName()
        {
            CStdString fileIndex;
            fileIndex.Format(" (%d)", m_conf.fileIndex);

            tstring logPath = m_conf.logPath;
            logPath += "/";
            logPath += m_conf.fileName;
            logPath += fileIndex;
            logPath += ".log";
            return logPath;
        }

        bool CFileLogger::newFile(bool _override)
        {
            m_ofs.close();

            //
            // if need multiple files in one second, name the files xxx (1)/(2)...
            //
            time_t now = time(NULL);
            if (now != m_conf.createTime)
            {
                CStdString name;
                name.Format("%s.%s.%s.%d",
                    s_processName.c_str(),
                    TimeUtil::GetNowTimeStr(true).c_str(),
                    SysUtil::GetHostName().c_str(),
                    SysUtil::GetCurrentPID());

                m_conf.fileName   = name;
                m_conf.createTime = now;
            }
            else    // more files in one second
            {
                ++m_conf.fileIndex %= m_conf.fileCount;
                ++m_conf.fileIndex;
            }

            int flag = CHFile::FileAccessOption::FILE_OPEN_ALWAYS;
            if (_override)
                flag = CHFile::FileAccessOption::FILE_CREATE_ALWAYS;

            tstring logPath = getFileName();
            m_ofs.open( logPath, 
                        CHFile::FileAccessMode::ACCESS_READ | CHFile::FileAccessMode::ACCESS_WRITE, 
                        flag, 
                        false, 
                        false);
            if (!m_ofs.is_open())
            {
                cerr << "Fail to create log file " << logPath << endl;
            }

            return m_ofs.is_open();
        }

        int CFileLogger::saveBuffers()
        {
            list<BufferPtr> buffers;
            getBuffers(buffers);

            list<BufferPtr>::const_iterator it = buffers.begin();
            for (; it != buffers.end(); ++it)
            {
                m_ofs.write_w((const byte*)(*it)->data(), (*it)->length());
            }

            if (buffers.size())
            {
                stream().setBuffers(buffers);
            }

            return buffers.size();
        }

        void CFileLogger::threadFunc()
        {
            char counts[ONE_KB * 4] = { 0 };
            char* p  = counts;
            char* sp = counts;
            u_int32 zeros = 0;

            if (!newFile(true)) return;

            while (!m_stop)
            {
                int n = 0;
                if (0 == (n = saveBuffers()))
                {
                    ++zeros;
                    SleepUS(1);
                }

                if (n > 0)
                {
                    int m = snprintf_t(p, 32, "%d, ", n);
                    p += m;

                    //
                    // print end line about every 80 
                    //
                    if (p - sp >= 80)
                    {
                        m = snprintf_t(p, 32, "\r\n");
                        p += m;
                        sp = p;
                    }
                }

                //
                // buffer is full, print and clear
                //
                if (p + 32 - counts >= sizeof(counts))
                {
                    cerr << ".";
                    p = counts;
                    sp = counts;
                }
            }

            int n = saveBuffers();
            m_ofs.close();

            int bc = getBuffersCount();
            snprintf_t(p, 32, "%d, zeros = %d, count = %d\r\n", n, zeros, bc);
            cerr << endl << counts << endl;
        }

        bool CFileLogger::start()
        {
            m_thread = std::thread(&CFileLogger::threadFunc, this);
            return m_thread.joinable();
        }

        void CFileLogger::stop()
        {
            m_stop = true;
        }

        void CFileLogger::join()
        {
            if (m_thread.joinable())
                m_thread.join();
        }
    }
}
