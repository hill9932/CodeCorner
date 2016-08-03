/**
 * File:        platform_win32.h
 *
 * Copyright (C) 2014 - 2015  PolyVirtual, Inc.
 * All rights reserved.
 *
 * Abstract: 
 *
 */
#ifndef __HL_PLATFORM_WIN32_INCLUDE_H__
#define __HL_PLATFORM_WIN32_INCLUDE_H__

#include <WinSock2.h>
#include <Windows.h>
#include <tchar.h>
#include <io.h>
#include <direct.h>
#include <ws2ipdef.h>
#include <Ws2tcpip.h>


typedef __int64             int64;
typedef unsigned __int64    u_int64;
typedef HANDLE              handle_t;
typedef int                 SOCKET_LEN;
typedef u_int32             PID_T;
typedef LARGE_INTEGER       TIME_INTERVAL;
typedef HANDLE              process_t;
typedef HANDLE              Aio_t;
typedef int                 ThreadSpecificKey;

#ifndef __GNUC__
#define EPOCHFILETIME (116444736000000000i64)
#else
#define EPOCHFILETIME (116444736000000000LL)
#endif

//
// These macros are not defined in Linux nor Windows
//
#define thread_local    __declspec(thread)


//
// These macros are defined in Linux but not in Windows
//
#define F_GETFL         1
#define F_SETFL         2
#define O_NONBLOCK      1
#define S_IFDIR         _S_IFDIR
#define S_IFREG         _S_IFREG
#define X_OK            04
#define EPOLLIN         0
#define I64D             "%I64d"
#define CS_ATTRIBUTE    int

// 
// redefine these macros, since not all of the system define them 
//
#define net_errno                   WSAGetLastError()
#define INTERLOCKED_INCREMENT(ref)  InterlockedIncrement(ref)   // ϵͳ�ṩ
#define INTERLOCKED_DECREMENT(ref)  InterlockedDecrement(ref)
#define INTERLOCKED_INCREMENT_N(ref, num)  \
{ \
    int count_mm = num; \
    while (count_mm > 0) \
    { \
        INTERLOCKED_INCREMENT(ref); --count_mm; \
    } \
}  

#define INTERLOCKED_DECREMENT_N(ref, num)  \
{ \
    int count_mm = num; \
    while (count_mm > 0) \
    { \
        INTERLOCKED_DECREMENT(ref); --count_mm; \
    } \
}

#define DEFAULT_RIGHT               NULL

#define SleepUS(tick)               TimeUtil::MySleep(tick)
#define SleepMS(tick)               Sleep(tick)
#define SleepSec(tick)              Sleep(tick*1000)      
#define GetNowTime(value)           QueryPerformanceCounter(&value); 
#define AlignedAlloc(size, align)           _aligned_malloc(size, align);
#define AlignedReAlloc(addr, size, align)   _aligned_realloc(addr, size, align);
#define AlignedFree(pointer)                _aligned_free(pointer);

//
// these structures are not defined by windows
//
struct timezone
{
    long tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

struct iovec
{
    int  iov_len;   // byte count to read/write
    char *iov_base; // data to be read/written
};


//
// define ansi/unicode compatible functions
//
#if defined(UNICODE) || defined(_UNICODE)
    #define sprintf_t       wsprintf    // function
    #define strncpy_t       wcsncpy_s

    #define snprintf_t      _snwprintf
    #define vsnprintf_t     _vsnwprintf
    
    #define access_t        _waccess
    #define remove_t        _wremove
    #define stat_t          _wstat
    #define rmdir_t         _wrmdir
    #define rename_t        _wrename
    #define atoi_t          _wtoi
    #define strdup_t        _wcsdup
    #define stat_           _stat       // struct

#else
    #define sprintf_t       sprintf     // function
    #define strncpy_t       strncpy

    #define snprintf_t      _snprintf
    #define vsnprintf_t     _vsnprintf
    #define access_t        _access
    #define strdup_t        _strdup

    #define remove_t        remove
    #define stat_t          stat
    #define rmdir_t         rmdir
    #define rename_t        rename
    #define atoi_t          atoi

    #define  stat_          stat        // struct
#endif

#define mkdir_t             CreateDirectory
#define atoi64              _atoi64


//
// these functions are not defined by windows
//
int  fcntl(int _fd, int _cmd, ... /* arg */ );
int  gettimeofday(struct timeval* _tp);
int  gettimeofday(struct timeval* _tp, struct timezone *_tz);
int  settimeofday(struct timeval* _tp, struct timezone *_tz);
void bzero(void* _src, size_t _size);
void bcopy(const void *_src, void *_dest, size_t _n);

#endif
