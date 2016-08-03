#ifndef __HL_FILE_INCLUDE_H__
#define __HL_FILE_INCLUDE_H__

#include "log_.h"
#include "file_pool.h"

namespace LabSpace
{
    namespace Common
    {
        class CHFile
        {
        public:
            struct FileAccessMode
            {
                enum FILE_ACCESS
                {
                    ACCESS_NONE = 0,
                    ACCESS_READ = 1,
                    ACCESS_WRITE = 2
                };
            };

            struct FileAccessOption
            {
                enum FILE_CREATE
                {
                    FILE_CREATE_NONE = 0,
                    FILE_CREATE_NEW = 1,
                    FILE_CREATE_ALWAYS = 2,
                    FILE_OPEN_EXISTING = 4,
                    FILE_OPEN_ALWAYS = 8,
                    FILE_TRUNCATE_EXISTING = 0x10,
                    FILE_APPEND = 0x20,
                };
            };

        public:
            CHFile(bool _autoClose = true, bool _secAlign = true);
            virtual ~CHFile();

            int  open(const tstring& _fileName, int _access, int _flag, bool _asyn = true, bool _directIO = false);

            /**
             * @Function: use aio to read/write data
             **/
            int  read (PPER_FILEIO_INFO_t _ioRequest);
            int  write(PPER_FILEIO_INFO_t _ioRequest);
            int  write(PPER_FILEIO_INFO_t _ioRequests[], int _count);

            /**
             * @Function: use sync io, _w means wait
             **/
            int  read_w  (byte* _data, int _dataLen, u_int64 _offset = -1);
            int  write_w (const byte* _data, int _dataLen, u_int64 _offset = -1);
            int  fflush_w();

            virtual int  close();
            void attach(handle_t _fileHandle, bool _autoClose = true);

            u_int32 getSize(u_int32* _high32);
            u_int64 getSize();

            int setSize(u_int64 _fileSize);
            int rename(const tchar* _newFileName);
            void seek(u_int64 _offset);

            tstring getFileName()                 { return m_fileName; }

            operator handle_t() { return m_fileHandle; }
            bool is_open() { return m_fileHandle != INVALID_HANDLE_VALUE; }

        private:
            bool init();

        protected:
            CFilePool*  m_filePool;     // for anyn io

            tstring     m_fileName;
            handle_t    m_fileHandle;
            bool        m_autoClose;

            u_int32     m_readIndex;
            u_int32     m_writeIndex;
            u_int32     m_flag;
            u_int32     m_option;

            bool        m_directIO;
            bool        m_secAlign;

        };
    }
}

#endif
