/**
 * File:        platform_linux.h
 *
 * Copyright (C) 2014 - 2015  PolyVirtual, Inc.
 * All rights reserved.
 *
 * Abstract: 
 *
 */
#ifndef __HL_PLATFORM_LINUX_INCLUDE_H__
#define __HL_PLATFORM_LINUX_INCLUDE_H__

#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <aio.h>
#include <libaio.h>
//#include <linux/aio_abi.h>


typedef long long           int64;
typedef unsigned long long  u_int64;
typedef const char*         LPCSTR;
typedef char*               LPSTR;

typedef int                 handle_t;
typedef socklen_t           SOCKET_LEN;
typedef pid_t               PID_T;
typedef struct timeval      TIME_INTERVAL;
typedef pid_t               process_t;
typedef int                 SOCKET;
typedef int                 HANDLE;
typedef u_int32             DWORD, UINT;
typedef iovec               WSABUF;
typedef DWORD*              LPDWORD;
typedef void*               HMODULE;
typedef io_context_t        Aio_t;    
typedef pthread_key_t       ThreadSpecificKey;

#define thread_local        __thread

// 
// redefine these macros, since not all of the system define them 
//
#define SD_RECEIVE                  SHUT_RD
#define SD_SEND                     SHUT_WR
#define SD_BOTH                     SHUT_RDWR
#define WSAEWOULDBLOCK              EINPROGRESS
#define ERROR_IO_PENDING            EINPROGRESS
#define INTERLOCKED_INCREMENT(ref)  __sync_add_and_fetch(ref, 1)    //__sync_fetch_and_add(ref, 1)    // GCC 4.1xx�ṩ
#define INTERLOCKED_DECREMENT(ref)  __sync_add_and_fetch(ref, -1)   //__sync_fetch_and_add(ref, -1)
#define INTERLOCKED_INCREMENT_N(ref, num)  __sync_add_and_fetch(ref,  num)    //__sync_fetch_and_add(ref, 1)    // GCC 4.1xx�ṩ
#define INTERLOCKED_DECREMENT_N(ref, num)  __sync_add_and_fetch(ref, -num)   //__sync_fetch_and_add(ref, -1)
#define INVALID_HANDLE_VALUE        -1
#define INVALID_SOCKET              -1
#define SOCKET_ERROR                -1
#define INFINITE                    -1
#define net_errno                   errno
#define CRITICAL_SECTION            pthread_mutex_t
#define CS_ATTRIBUTE                pthread_mutexattr_t
#define TRUE                true
#define FALSE               false
#define AF_NETBIOS          17              // NetBios-style addresses
#define DEFAULT_RIGHT       S_IRUSR|S_IWUSR|S_IXUSR | S_IRGRP|S_IWGRP|S_IXGRP | S_IROTH|S_IWOTH|S_IXOTH
#define I64D                 "%lld"

//
// define ansi/unicode compatible functions
//
#if defined(UNICODE) || defined(_UNICODE)
    #define TEXT(text)      L##text

    #define access_t        waccess     // function
    #define sprintf_t       wsprintf
    #define snprintf_t      wsnprintf
    #define remove_t        wremove
    #define atoi_t          wtoi
    #define stat_t          wstat
    #define rmdir_t         wrmdir
    #define rename_t        wrename
    #define strdup_t        wcsdup

#else
    #define TEXT(text)      text

    #define access_t        access      // function
    #define sprintf_t       sprintf
    #define snprintf_t      snprintf
    #define vsnprintf_t     vsnprintf
    #define strncpy_t       strncpy

    #define remove_t        remove
    #define stat_t          stat
    #define rmdir_t         rmdir
    #define atoi_t          atoi
    #define rename_t        rename
    #define strdup_t        strdup

#endif

typedef const tchar*        LPCTSTR;
typedef tchar*              LPTSTR;
typedef struct iocb         OVERLAPPED;     // libaio provide

struct VS_FIXEDFILEINFO {};
#define Offset                      u.c.offset

#define SleepUS(tick)               usleep(tick)
#define SleepMS(tick)               usleep((tick) * 1000)
#define SleepSec(tick)              ::sleep(tick)
#define CloseHandle                 ::close
#define SetConsoleCtrlHandler       signal
#define closesocket                 close
#define ioctlsocket                 ioctl    
#define GetNowTime(value)           gettimeofday(&value, NULL);
#define AlignedAlloc(size, align)           aligned_alloc(align, size);
#define AlignedReAlloc(addr, size, align)   _aligned_realloc(addr, size, align);
#define AlignedFree(pointer)                free(pointer);

#define stat_                       stat
#define stricmp                     strcasecmp
#define strnicmp                    strncasecmp
#define mkdir_t                     mkdir
#define atoi64                      atoll

#endif
