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

int SysUtil::MySystem(const tchar* _cmd)
{
    if (!_cmd)  return -2;
    return system(_cmd);
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

bool CheckShellStatus(int _status)
{
    if (-1 == _status && errno == ECHILD)   return true;

    if (-1 == _status)              return false;   // fail to execute shell process
    if (!WIFEXITED(_status))        return false;   // fail to run the shell file
    if (0 != WEXITSTATUS(_status))  return false;

    return true;
}

int SysUtil::MySystem(const tchar* _cmd)
{
    if (!_cmd)  return -2;
    FILE* f = popen(_cmd, "r");
    if (!f)
        return -1;

    int z = pclose(f);
    if (CheckShellStatus(z))
        z = 0;

    return z;
}

int SysUtil::MySystem(const tchar* _cmdString, char* _buf, int _len)
{
    if (!_cmdString)    return -1;

    int fd[2];
    int n, count;
    if (_buf)   memset(_buf, 0, _len);
    if (pipe(fd) < 0)       return -1;

    pid_t pid;
    if ((pid = fork()) < 0)
        return -1;
    else if (pid > 0)     // parent process
    {
        close(fd[1]);     // close write end 
        count = 0;
        while (_buf && (n = read(fd[0], _buf + count, _len)) > 0 && count > _len)
            count += n;
        close(fd[0]);
        if (waitpid(pid, NULL, 0) > 0)
            return -1;
    }
    else    /* child process */
    {
        close(fd[0]);     /* close read end */
        if (fd[1] != STDOUT_FILENO)
        {
            if (dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO)
            {
                return -1;
            }
            close(fd[1]);
        }
        if (execl("/bin/sh", "sh", "-c", _cmdString, (char*)0) == -1)
            return -1;
    }

    return 0;
}

#endif