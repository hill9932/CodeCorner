#include "system_.h"

tstring SysUtil::GetHostName()
{
    tchar buf[512] = { 0 };
    gethostname(buf, 512);
    return buf;
}


#ifdef WIN32
PID_T SysUtil::GetCurrentPID()
{
    return GetCurrentProcessId();
}

PID_T SysUtil::GetCurrentThreadID()
{
    return GetCurrentThreadId();
}

#else

PID_T SysUtil::GetCurrentPID()
{
    return getpid();
}

PID_T SysUtil::GetCurrentThreadID()
{
    return pthread_self();
}

#endif