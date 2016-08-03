#include "time_.h"
#include "stdString.h"

namespace LabSpace
{
    namespace Common
    {
        u_int64 TimeUtil::Now()
        {
            struct timeval ts;
            gettimeofday(&ts, NULL);
            return  (u_int64)ts.tv_sec * NS_PER_SECOND + ts.tv_usec * 1000;
        }

        tstring TimeUtil::GetNowTimeStr(bool _fileName)
        {
            return GetTimeString(time(NULL), _fileName);
        }
        
        tstring TimeUtil::GetTimeString(time_t _time, bool _fileName)
        {
            struct tm tm;
            tm = *localtime(&_time);

            CStdString r;
            if (!_fileName)
                r.Format("%4d%02d%02d %02d:%02d:%02d",
                tm.tm_year + 1900,
                tm.tm_mon + 1,
                tm.tm_mday,
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec);
            else
                r.Format("%4d%02d%02d %02d%02d%02d",
                tm.tm_year + 1900,
                tm.tm_mon + 1,
                tm.tm_mday,
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec);

            return r;
        }

        tstring TimeUtil::GetTimeString(struct timeval *_tmvalue)
        {
            CStdString str = GetTimeString((time_t)_tmvalue->tv_sec);
            str.AppendFormat(".%06d", _tmvalue->tv_usec);
            return str;
        }

        void TimeUtil::MySleep(u_int64 _usec)
        {
            struct timeval timeout;
            fd_set  fdSet;
            int     fd = socket(AF_INET, SOCK_STREAM, 0);  //fileno(stdin);
            FD_ZERO(&fdSet);
            FD_SET(fd, &fdSet);

            timeout.tv_sec = _usec / 1000000;
            timeout.tv_usec = _usec % 1000000;

            if (select(fd + 1, &fdSet, NULL, NULL, &timeout) == 0);
            ::closesocket(fd);
        }


        /************************************************************************/
        /* Class CScopeTimer                                                    */
        /************************************************************************/
        TimeUtil::CScopeTimer::CScopeTimer(const char* _msg)
        {
            if (_msg)   m_msg = _msg;
            GetNowTime(m_startTime);
        }

        TimeUtil::CScopeTimer::~CScopeTimer()
        {
            #define MESSAGE_SIZE    128
            tchar elapse[MESSAGE_SIZE] = { 0 };
            TIME_INTERVAL now;
            u_int64 fElap = 0;

#ifdef WIN32
            static TIME_INTERVAL   freq;
            if (freq.LowPart == freq.HighPart && freq.LowPart == 0)
                QueryPerformanceFrequency(&freq);
            QueryPerformanceCounter(&now);
            fElap = (now.QuadPart - m_startTime.QuadPart) * 1e+6 / freq.QuadPart;
#else
            gettimeofday(&now, NULL);
            fElap = 1000000.0 * (now.tv_sec - m_startTime.tv_sec) + now.tv_usec - m_startTime.tv_usec;
#endif
            u_int64 timeElapse = fElap / 1e+6;     // in seconds
            u_int32 days    = timeElapse / (3600 * 24);
            u_int32 hours   = (timeElapse - days * 3600 * 24) / 3600;
            u_int32 minutes = (timeElapse - days * 3600 * 24 - hours * 3600) / 60;
            u_int32 seconds = timeElapse % 60;
            fElap -= timeElapse * 1e+6;

            snprintf_t(elapse, MESSAGE_SIZE, TEXT("%s cost - %dd %d:%d:%d.%d.%d"),
                m_msg.empty() ? "" : m_msg.c_str(),
                days, hours, minutes, seconds, fElap / 1000, fElap % 1000);
            cerr << elapse << endl;
        }
    }
}
