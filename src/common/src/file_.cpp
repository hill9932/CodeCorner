/**
 * File:        file.cpp
 *
 * Copyright (C) 2014 - 2015  PolyVirtual, Inc.
 * All rights reserved.
 *
 * Abstract: 
 *
 */
#include "file_.h"


namespace LabSpace
{
    namespace Common
    {
        
        CHFile::CHFile(bool _autoClose, bool _secAlign)
        {
            m_secAlign = _secAlign;
            m_autoClose = _autoClose;
            m_flag = m_option = 0;

            init();
        }

        bool CHFile::init()
        {
            m_fileHandle = INVALID_HANDLE_VALUE;
            m_readIndex = m_writeIndex = 0;

            return true;
        }

        CHFile::~CHFile()
        {
            if (m_autoClose)
            {
                close();
            }
        }


        int CHFile::close()
        {
            if (m_fileHandle != INVALID_HANDLE_VALUE)
            {
                CloseHandle(m_fileHandle);
                m_fileHandle = INVALID_HANDLE_VALUE;
            }

            m_fileName.clear();
            return 0;
        }

        u_int32 CHFile::getSize(u_int32* _high32)
        {
            u_int32 high32 = 0;
            u_int32 low32 = 0;

#ifdef WIN32
            low32 = GetFileSize(m_fileHandle, (LPDWORD)&high32);
#else
            struct stat statbuf;
            if (fstat(m_fileHandle, &statbuf) == -1)
            {
                LOG_LAST_ERRORMSG();
                return 0;
            }

            if(S_ISREG(statbuf.st_mode))    return statbuf.st_size;
#endif

            if (_high32)    *_high32 = high32;
            return low32;
        }

        u_int64 CHFile::getSize()
        {
            u_int32 high32 = 0, low32 = 0;
            low32 = getSize(&high32);
            return (u_int64)high32 << 32 | low32;
        }

        int CHFile::setSize(u_int64 _fileSize)
        {
            int z = 0;

#ifdef WIN32
            LONG    hightSize = _fileSize >> 32;
            u_int32 newSize = SetFilePointer(m_fileHandle, _fileSize, &hightSize, FILE_BEGIN);

            if (!SetEndOfFile(m_fileHandle))
                z = Util::GetLastSysError();
#else
            z = truncate(m_fileName.c_str(), _fileSize);
#endif
            ON_ERROR_PRINT_LASTMSG_S_AND_DO(z, != , z, "Set file size " << m_fileName, "");

            return z;
        }

        void CHFile::seek(u_int64 _offset)
        {
#ifdef WIN32
            LONG highOffset = _offset >> 32;
            SetFilePointer(m_fileHandle, _offset, &highOffset, FILE_BEGIN);
#else
            lseek64(m_fileHandle, _offset, SEEK_SET);
#endif
        }

        int CHFile::rename(const tchar *_newFileName)
        {
            if (!_newFileName || !is_open())  return -1;
            if (m_fileName == _newFileName) return 0;

            int z = ::rename_t(m_fileName.c_str(), _newFileName);
            ON_ERROR_PRINT_LASTMSG_S_AND_DO(z, != , 0, "Rename " << _newFileName, return -1);

            m_fileName = _newFileName;
            return z;
        }

        void CHFile::attach(handle_t _fileHandle, bool _autoClose /* = true */)
        {
            m_fileHandle = _fileHandle;
            m_autoClose = _autoClose;
        }

        int CHFile::read(PPER_FILEIO_INFO_t _ioRequest)
        {
            if (m_fileHandle == INVALID_HANDLE_VALUE)  return -1;

            int z = 0;
            int tries = 0;
            _ioRequest->hFile = m_fileHandle;
            _ioRequest->optType = FILE_READ;
            _ioRequest->index = m_readIndex;

#ifdef WIN32
        retry :
            DWORD dataRead = 0;
            if (ReadFile(m_fileHandle,
                _ioRequest->data,
                _ioRequest->dataLen,
                &dataRead,
                (LPOVERLAPPED)_ioRequest) ||
                ERROR_IO_PENDING == (z = Util::GetLastSysError()))
#else
            struct iocb* iocbs[1] = {0};
            iocbs[0] = _ioRequest;
            io_prep_pread(_ioRequest, m_fileHandle, _ioRequest->data, _ioRequest->dataLen, _ioRequest->Offset);

        retry:
            z = io_submit(m_filePool->getAioObject(), 1, iocbs);
            if (z != 1) 
            {
                z = -z;
                Util::SetLastSysError(z);
            }

            if (z == 1)
#endif
            {
                ++m_readIndex;
            }
            else if (z == EAGAIN)
            {
                if (tries++ < 1000)
                {
                    SleepMS(1);
                    goto retry;
                }

                LOG_LASTERROR_AND_DO(return EAGAIN);
            }
            else
            {
                LOG_LASTERROR_AND_RETURN_ERRCODE();
            }

            return 0;
        }


        int CHFile::write(PPER_FILEIO_INFO_t _ioRequest)
        {
            if (m_fileHandle == INVALID_HANDLE_VALUE || !_ioRequest)  return -1;

            int z = 0;
            int tries = 0;
            _ioRequest->hFile = m_fileHandle;
            _ioRequest->index = m_writeIndex;
            _ioRequest->optType = FILE_WRITE;

#ifdef WIN32
        retry :
            if (WriteFile(m_fileHandle,
                _ioRequest->data,
                _ioRequest->dataLen,
                NULL,
                (LPOVERLAPPED)_ioRequest) ||
                ERROR_IO_PENDING == (z = Util::GetLastSysError()))
#else
            struct iocb* iocbs = _ioRequest;
            io_prep_pwrite(_ioRequest, m_fileHandle, _ioRequest->data, _ioRequest->dataLen, _ioRequest->Offset);

        retry:
            z = io_submit(m_filePool->getAioObject(), 1, &iocbs);
            if (z != 1)
            {
                z = -z;
                Util::SetLastSysError(z);
            }

            if (z == 1)
#endif
            {
                ++m_writeIndex;
            }
            else if (z == EAGAIN)
            {
                if (tries++ < 1000)
                {
                    SleepMS(1);
                    goto retry;
                }

                LOG_LASTERROR_AND_DO(return EAGAIN);
            }
            else
            {
                LOG_LASTERROR_AND_RETURN_ERRCODE();
            }

            return 0;
        }

        int CHFile::write(PPER_FILEIO_INFO_t _ioRequests[], int _count)
        {
            if (m_filePool->getAioObject() == 0)    return -1;

            assert(_count < 512);

            int z = 0;
#ifdef LINUX
            /*   struct iocb* iocbs[512] = {0};

               for (int i = 0; i < _count; ++i)
               {
               #ifdef LINUX
               iocbs[i] = _ioRequests[i];
               #endif
               }
               */

            z = io_submit(m_filePool->getAioObject(), _count, (iocb**)_ioRequests);
            ON_ERROR_PRINT_LASTMSG_AND_DO(z, !=, _count, Util::SetLastSysError(-z));

#else
            for (int i = 0; i < _count; ++i)
                z = write(_ioRequests[i]);

#endif

            return z;
        }


#ifdef WIN32

        int CHFile::open(const tstring& _fileName, int _access, int _flag, bool _asyn, bool _directIO)
        {
            if (_fileName.empty())  return -1;

            int access = FileAccessMode::ACCESS_NONE;
            if (_access & FileAccessMode::ACCESS_READ)  access |= GENERIC_READ;
            if (_access & FileAccessMode::ACCESS_WRITE) access |= GENERIC_WRITE;
            if (access == FileAccessMode::ACCESS_NONE)  return -1;

            int flag = FileAccessOption::FILE_CREATE_NONE;
            if (_flag & FileAccessOption::FILE_CREATE_NEW)        flag |= CREATE_NEW;
            if (_flag & FileAccessOption::FILE_CREATE_ALWAYS)     flag |= CREATE_ALWAYS;
            if (_flag & FileAccessOption::FILE_OPEN_ALWAYS)       flag |= OPEN_ALWAYS;
            if (_flag & FileAccessOption::FILE_OPEN_EXISTING)     flag |= OPEN_EXISTING;
            if (_flag & FileAccessOption::FILE_TRUNCATE_EXISTING) flag |= TRUNCATE_EXISTING;

            if (flag == FileAccessOption::FILE_CREATE_NONE)  return -1;
            int opt = _asyn ? FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED : FILE_ATTRIBUTE_NORMAL;
            if (_directIO)  opt |= FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING;

            m_fileHandle = CreateFile(_fileName.c_str(),
                access,          // dwDesiredAccess
                FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE, // dwShareMode
                NULL,            // lpSecurityAttributes
                flag,            // dwCreationDisposition
                opt,             // dwFlagsAndAttributes
                NULL);           // hTemplate
            ON_ERROR_PRINT_LASTMSG_S_AND_DO(m_fileHandle, == , INVALID_HANDLE_VALUE, "Create file " << m_fileName, "");

            m_directIO  = _directIO;
            m_flag      = flag;
            m_option    = opt;
            m_fileName  = _fileName;
            return 0;
        }

        int CHFile::read_w(byte* _data, int _dataLen, u_int64 _offset)
        {
            if (m_fileHandle == INVALID_HANDLE_VALUE)  return -1;

            if (_offset != -1)
            {
                LONG highOffset = _offset >> 32;
                SetFilePointer(m_fileHandle, _offset, &highOffset, FILE_BEGIN);
            }

            DWORD   bytesRead = 0;
            int n = ReadFile(m_fileHandle, _data, _dataLen, &bytesRead, NULL);
            ON_ERROR_PRINT_LASTMSG_AND_DO(bytesRead, != , _dataLen, "");
            return bytesRead;
        }

        int CHFile::write_w(const byte* _data, int _dataLen, u_int64 _offset)
        {
            if (m_fileHandle == INVALID_HANDLE_VALUE || !_data)  return -1;
            if (_offset != -1)
            {
                LONG highOffset = _offset >> 32;
                SetFilePointer(m_fileHandle, _offset, &highOffset, FILE_BEGIN);
            }

            DWORD bytesWritten = 0;
            int n = WriteFile(m_fileHandle, _data, _dataLen, &bytesWritten, NULL);
            ON_ERROR_PRINT_LASTMSG_AND_DO(bytesWritten, != , _dataLen, "");
            return bytesWritten;
        }

        int CHFile::fflush_w()
        {
            return !FlushFileBuffers(m_fileHandle);
        }

#else // for linux

        int CHFile::open(const tstring& _fileName, int _access, int _flag, bool _asyn, bool _directIO)
        {
            if (_fileName.empty())  return -1;

            int access = -1;
            if (_access & FileAccessMode::ACCESS_READ &&
                _access & FileAccessMode::ACCESS_WRITE)
                access = O_RDWR;
            else if (_access & FileAccessMode::ACCESS_WRITE)
                access = O_WRONLY;
            else if (_access & FileAccessMode::ACCESS_READ)
                access = O_RDONLY;

            if (access == -1)  return -1;

            if (_flag & FileAccessOption::FILE_OPEN_ALWAYS)       access |= O_CREAT;
            if (_flag & FileAccessOption::FILE_TRUNCATE_EXISTING) access |= O_TRUNC;
            if (_flag & FileAccessOption::FILE_CREATE_ALWAYS)     access |= O_TRUNC | O_CREAT;
            if (_flag & FileAccessOption::FILE_APPEND)            access |= O_APPEND;

            if (_directIO)  access |= O_DIRECT;
            if (_asyn)      access |= O_NONBLOCK;

            access |= O_LARGEFILE;

        retry:
            m_fileHandle = ::open(_fileName.c_str(), access, 0644);
            int err = Util::GetLastSysError();

            ON_ERROR_PRINT_LASTMSG_S_AND_DO(m_fileHandle, == , INVALID_HANDLE_VALUE, "Create file " << m_fileName, "");
            if (m_fileHandle == INVALID_HANDLE_VALUE)
            {
                if (err == 22)  // not support O_DIRECT
                {
                    access &= ~O_DIRECT;
                    goto retry;
                }
                return err;
            }

            m_directIO  = _directIO;
            m_flag      = access;
            m_option    = 0644;
            m_fileName  = _fileName;
            return 0;
        }

        int CHFile::read_w(byte* _data, int _dataLen, u_int64 _offset)
        {
            if (m_fileHandle == INVALID_HANDLE_VALUE)  return -1;

            if (_offset != (u_int64)-1)  lseek64(m_fileHandle, _offset, SEEK_SET);

            int n = ::read(m_fileHandle, _data, _dataLen);
            ON_ERROR_PRINT_LAST_ERROR(n, == , -1);
            return n;
        }

        int CHFile::write_w(const byte* _data, int _dataLen, u_int64 _offset)
        {
            if (m_fileHandle == INVALID_HANDLE_VALUE || !_data)  return -1;
            if (_offset != (u_int64)-1)  lseek64(m_fileHandle, _offset, SEEK_SET);

            int n = ::write(m_fileHandle, _data, _dataLen);
            ON_ERROR_PRINT_LAST_ERROR(n, != , _dataLen);
            return n;
        }

        int CHFile::fflush_w()
        {
            return fsync(m_fileHandle);
        }

#endif
    }
}