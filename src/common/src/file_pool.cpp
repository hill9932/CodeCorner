/**
 * File:        file_pool.cpp
 *
 * Copyright (C) 2014 - 2015  PolyVirtual, Inc.
 * All rights reserved.
 *
 * Abstract: 
 *
 */
#include "file_pool.h"


namespace LabSpace
{
    namespace Common
    {
        CFilePool::CFilePool()
        {
            m_readCallback = boost::bind(&CFilePool::onDataRead, this, _1, _2, _3);
            m_writeCallback = boost::bind(&CFilePool::onDataWritten, this, _1, _2, _3);
            m_errorCallback = boost::bind(&CFilePool::onError, this, _1, _2, _3);
            m_context = NULL;
            m_isStop = false;
            m_aioCallback = NULL;
        }

        CFilePool::~CFilePool()
        {
            stop();
        }

        int CFilePool::init(int _blockNum, int _threadNum, void* _context)
        {
            m_context = _context;
            return 0;
        }

        int CFilePool::attach(handle_t _fileHandle, void* _context, u_int32 _flag)
        {
            Aio_t h = m_IOService.assoicateObject(_fileHandle, _context, _flag);
            ON_ERROR_PRINT_LASTMSG_AND_DO(h, == , NULL, return -1);

            return 0;
        }

        int CFilePool::onDataRead(void* _handler, PPER_FILEIO_INFO_t _io_context, u_int32 _bytes_count)
        {
            return 0;
        }

        int CFilePool::onDataWritten(void* _handler, PPER_FILEIO_INFO_t _io_context, u_int32 _bytes_count)
        {
            return 0;
        }

        int CFilePool::onError(void* _handler, PPER_FILEIO_INFO_t _io_context, int _error_code)
        {
            return 0;
        }


#ifdef WIN32
#include "file_pool_win32.hxx"
#else
#include "file_pool_linux.hxx"
#endif
    }
}