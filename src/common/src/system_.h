#ifndef __HL_SYSTEM_INCLUDE_H__
#define __HL_SYSTEM_INCLUDE_H__

#include "log_.h"


namespace LabSpace
{
    namespace Common
    {
        struct SysUtil
        {
            static PID_T   GetCurrentPID();
            static PID_T   GetCurrentThreadID();
            static tstring GetHostName();
        };
    }
}

#endif
