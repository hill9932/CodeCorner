#ifndef __HL_TIME_INCLUDE_H__
#define __HL_TIME_INCLUDE_H__

#include "def.h"
#include <boost/noncopyable.hpp>


namespace LabSpace
{
    namespace Common
    {
        struct TimeUtil
        {
            /**
            *@Function: Get the string format of local time
            *  the format is %4d.%02d.%02d %02d.%02d.%02d
            **/
            static tstring GetNowTimeStr(bool _fileName = false);
            static tstring GetTimeString(time_t _time, bool _fileName = false);
            static tstring GetTimeString(struct timeval *_tmvalue);

            static u_int64 Now();
            static void MySleep(u_int64 _usec);  


            /**
            * @Function: This class will print the current time when it is created and destroyed
            *  Often used when need to timer a command execution.
            **/
            class CScopeTimer : public boost::noncopyable
            {
            public:
                CScopeTimer(const tchar* _msg = NULL);
                ~CScopeTimer();

            private:
                tstring         m_msg;
                TIME_INTERVAL   m_startTime;
            };
        };
    }
}

#endif
