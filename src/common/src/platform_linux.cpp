/**
 * File:        platform_linux.cpp
 *
 * Copyright (C) 2014 - 2015  PolyVirtual, Inc.
 * All rights reserved.
 *
 * Abstract: 
 *
 */
#include "common.h"
#include "platform.h"
#include "log_.h"

#ifdef LINUX

#include <pwd.h>


int DropPrivileges(const char* _username) 
{
    const char* username = _username ? _username : "anonymous";
    struct passwd *pw = NULL;

    if (getgid() && getuid()) 
    {
        L4C_LOG_ERROR("I'm not a superuser.");
        return 0;
    }

    pw = getpwnam(username);
    ON_ERROR_PRINT_LASTMSG_AND_DO(pw, ==, NULL, return err);

    // Drop privileges
    if ((setgid(pw->pw_gid) != 0) || (setuid(pw->pw_uid) != 0)) 
    {
        int err = Util::GetLastSysError();
        LOG_ERRORMSG(err);
        return err;
    } 


    L4C_LOG_ERROR("Success to change user to " << username);

    umask(0);
    return 0;
}

bool CheckShellStatus(int _status)
{
    if (-1 == _status && errno == ECHILD)   return true;

    if (-1 == _status)              return false;   // fail to execute shell process
    if (!WIFEXITED(_status))        return false;   // fail to run the shell file
    if (0 != WEXITSTATUS(_status))  return false;

    return true;
}
#endif