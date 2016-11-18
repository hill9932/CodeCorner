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

            /**
             * @Function: Exec a shell command and capture the command output
             **/
            static bool CheckShellStatus(int _status);
            static int  MySystem(const tchar* _cmdString, char* _buf, int _len);
            static int  MySystem(const tchar* _cmd);
        };
    }
}

#endif
