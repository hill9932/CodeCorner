/**
 * File:        platform.h
 *
 * Copyright (C) 2014 - 2015  PolyVirtual, Inc.
 * All rights reserved.
 *
 * Abstract: 
 *
 */
/**
 * @Function:
 *  This file includes some OS specific headers and declare some types that
 *  can be used to cross OS.
 * @Memo:
 *  Created by hill, 4/11/2014
 **/

#ifndef __HL_PLATFORM_INCLUDE_H__
#define __HL_PLATFORM_INCLUDE_H__

#include <string>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>
#include <stdarg.h>
#include <assert.h>
#include <iomanip>

using namespace std;


typedef char            int8;
typedef short           int16;
typedef int             int32;
typedef unsigned char   u_int8;
typedef unsigned short  u_int16;
typedef unsigned int    u_int32;
typedef unsigned char   byte;
typedef volatile long   atomic_t;


#define IN 
#define OUT
#define SOCKET_EXCEPTION    -2


#if defined(UNICODE) || defined(_UNICODE)
typedef wchar_t		    tchar;
typedef wstring         tstring;

#define strcpy_t        wcscpy
#define strtok_t        wcstok
#define isspace_t       iswspace
#define strlen_t        wcslen      

#else

typedef char            tchar;
typedef string          tstring;

#define strcpy_t        strcpy
#define strtok_t        strtok
#define isspace_t       isspace
#define strlen_t        strlen          

#endif

#ifdef WIN32
#include "platform_win32.h"
#else
#include "platform_linux.h"
#endif

#endif
