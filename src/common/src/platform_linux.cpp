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

#ifdef LINUX

#include <pwd.h>


int DropPrivileges(const char* _username) 
{
    const char* username = _username ? _username : "anonymous";
    struct passwd *pw = NULL;

    if (getgid() && getuid()) 
    {
        RM_LOG_DEBUG("I'm not a superuser.");
        return 0;
    }

    pw = getpwnam(username);
    ON_ERROR_PRINT_LASTMSG_AND_RETURN(pw, ==, NULL);

    // Drop privileges
    if ((setgid(pw->pw_gid) != 0) || (setuid(pw->pw_uid) != 0)) 
    {
        int err = GetLastSysError();
        LOG_ERRORMSG(err);
        return err;
    } 


    RM_LOG_INFO_S("Success to change user to " << username);

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