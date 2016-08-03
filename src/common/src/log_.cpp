#include "log_.h"
#include "file_util.h"
#include "thread_.h"
#include "system_.h"
#include "stdString.h"


namespace LabSpace
{
    namespace Common
    {
        std::mutex                          LogStream::s_listMutex;
        std::condition_variable             LogStream::s_listCV;
        std::list<BufferPtr>                LogStream::s_bufferList;
        std::list<BufferPtr>                LogStream::s_savingList;
        std::atomic_int32_t                 LogStream::s_buferCount;

        /************************************************************************/
        /* Class LogStream                                                      */
        /************************************************************************/

        void LogStream::initStatic()
        {
            assert(s_bufferList.size() == 0);
            if (s_bufferList.size())    return;

            std::unique_lock<std::mutex> lock(s_listMutex);
            for (int i = 0; i < INIT_LOG_BUFFER_COUNT; ++i)
                s_bufferList.push_back(std::make_shared<Buffer>());

            s_buferCount = INIT_LOG_BUFFER_COUNT;
        }

        LogStream::LogStream()
        {
            nextBuffer();
        }

        void LogStream::notify()
        {
            s_listCV.notify_one();
        }

        void LogStream::flush()
        {
            BufferPtr tmp;
            tmp.swap(m_curBuffer);

            std::unique_lock<std::mutex> lock(s_listMutex);
            s_savingList.push_back(tmp);
        }

        void LogStream::nextBuffer()
        {
            do
            {
                if (m_curBuffer)
                {
                    flush();

                    std::unique_lock<std::mutex> lock(s_listMutex);
                    if (s_bufferList.size())
                    {
                        m_curBuffer = s_bufferList.back();
                        s_bufferList.pop_back();
                    }
                }

                //
                // get the buffer from the buffer cache first
                // then allocate a buffer if the cache buffer size is <= 1024
                // or wait for io finished and return one buffer
                //
                if (!m_curBuffer)
                {
                    std::unique_lock<std::mutex> lock(s_listMutex);
                    if (s_bufferList.size())
                    {
                        m_curBuffer = s_bufferList.back();
                        s_bufferList.pop_back();
                    }
                    else if (s_buferCount < MAX_LOG_BUFFER_COUNT)
                    {
                        m_curBuffer = std::make_shared<Buffer>();
                        ++s_buferCount;
                    }
                    else
                    {
                        s_listCV.wait(lock);
                    }
                }
            } while (!m_curBuffer);

            m_curBuffer->reset();
        }

        void LogStream::getBuffers(list<BufferPtr>& _swap)
        {
            std::unique_lock<std::mutex> lock(s_listMutex);
            _swap.swap(s_savingList);
        }

        void LogStream::setBuffer(const BufferPtr& _buf)
        {
            {
                std::unique_lock<std::mutex> lock(s_listMutex);
                s_bufferList.push_back(_buf);
            }

            s_listCV.notify_one();
        }

        void LogStream::setBuffers(list<BufferPtr>& _bufs)
        {
            {
                std::unique_lock<std::mutex> lock(s_listMutex);
                s_bufferList.insert(s_bufferList.end(), _bufs.begin(), _bufs.end());
            }

            _bufs.size() > 1 ? s_listCV.notify_all() : s_listCV.notify_one();
        }

        inline void LogStream::checkBuffer(int _size)
        {
            if (!m_curBuffer || m_curBuffer->avail() <= _size)
                nextBuffer();
        }

        void LogStream::append(const tchar* _message, int _size)
        {
            if (!_message || _size <= 0)    return;
            
            //
            // need a new buffer
            //
            checkBuffer(_size + 2);
            m_curBuffer->append(_message, _size);
        }

        LogStream& LogStream::operator << (const tstring& _message)
        {
            append(_message.c_str(), _message.size());
            return *this;
        }

        LogStream& LogStream::operator << (const tchar* _message)
        {
            if (_message)
                append(_message, strlen_t(_message));

            return *this;
        }

        LogStream& LogStream::operator << (const SourceFile& _file)
        {
            append(_file.m_data, _file.m_size);
            return *this;
        }

        LogStream& LogStream::operator << (int _message)
        {
            checkBuffer();

            int n = snprintf_t(m_curBuffer->current(), MIN_AVAILABLE_SIZE, "%d", _message);
            m_curBuffer->add(n);
            return *this;
        }

        LogStream& LogStream::operator << (u_int32 _message)
        {
            checkBuffer();

            int n = snprintf_t(m_curBuffer->current(), MIN_AVAILABLE_SIZE, "%u", _message);
            m_curBuffer->add(n);
            return *this;
        }

        LogStream& LogStream::operator << (u_int64 _message)
        {
            checkBuffer();

            int n = snprintf_t(m_curBuffer->current(), MIN_AVAILABLE_SIZE, I64D, _message);
            m_curBuffer->add(n);
            return *this;
        }

        LogStream& LogStream::operator << (double _message)
        {
            checkBuffer();

            int n = snprintf_t(m_curBuffer->current(), MIN_AVAILABLE_SIZE, "%.16f", _message);
            m_curBuffer->add(n);
            return *this;
        }

        LogStream& LogStream::operator << (const void* _p)
        {
            checkBuffer();

            m_curBuffer->append("0x", 2);
            int n = StrUtil::convertHex(m_curBuffer->current(), _p);
            m_curBuffer->add(n);

            return *this;
        }

        LogStream& LogStream::operator << (std::ostream& (*pf)(std::ostream&))
        {
            checkBuffer();
            const tchar* buf = "\r\n";
            int n = snprintf_t(m_curBuffer->current(), MIN_AVAILABLE_SIZE, "%s", buf);
            m_curBuffer->add(n);
            return *this;
        }

        /************************************************************************/
        /* class CFileLogger                                                    */
        /************************************************************************/
        CLogger::LogLevel       CLogger::s_logLevel;
        tstring                 CLogger::s_processName;

        CLogger::CLogger()
        {
        }

        CLogger::CLogger(const SourceFile& _file, int _line, const char* _func, LogLevel _level)
            : m_impl(_level, 0, _file, _line)
        {
        }  

        void CLogger::initStatic()
        {
            LogStream::initStatic();

            s_logLevel      = DEBUG;
            s_processName   = FileUtil::GetAppName();
        }

        void CLogger::getBuffers(list<BufferPtr>& _swap)
        {
            return LogStream::getBuffers(_swap);
        }

        CLogger::Impl::Impl()
        {
        }

        CLogger::Impl::Impl(LogLevel _level, int _savedErrno, const SourceFile& _file, int _line)
        {
            struct timeval now;
            gettimeofday(&now);

            m_logStream << "Entry {\"" << CCurrentThread::ID() << ", " << TimeUtil::GetTimeString(&now) << ", ";
            
            if (_savedErrno != 0)
            {
                m_logStream << "Error = " << Util::GetLastSysErrorMessage(_savedErrno) << ", ";
            }

            m_logStream << _file << ":" << (u_int64)_line << ", ";
        }

        void CLogger::Impl::finish()
        {
            m_logStream << "\"}" << endl;
            m_logStream.flush();
        }

        CLogger::~CLogger()
        {
            m_impl.finish();
        }
    }
}
