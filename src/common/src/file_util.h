#ifndef __HL_FILE_UTIL_INCLUDE_H__
#define __HL_FILE_UTIL_INCLUDE_H__

#include "log_.h"
#include <vector>


namespace LabSpace
{
    namespace Common
    {
        struct FileUtil
        {
            /**
            * @Function: Get the path of execute file in the disk.
            *  There will be a '/' in the end.
            **/
            static tstring GetAppDir();
            static tstring GetAppName();

            /**
             * @Function: Judge whether file exist
             **/
            static bool IsFileExist(const tstring& _path);
            static bool IsDirExist(const tstring& _path);

            /**
             * @Function: Judge whether can access the file
             **/
            static bool CanFileAccess(const tstring& _path);
            static bool CanDirAccess(const tstring& _path);

            /**
             * @Function: Create all the directories on the path.
             **/
            static bool CreateAllDir(const tstring& _path);

            /**
             * @Function: Delete the file or directory
             **/
            static bool DeletePath(const tstring& _path);
            static bool ClearPath(const tstring& _path, int depth = 2);

            //
            // Only support file now, need to support copy fold
            //
            static bool CopyPath(const tstring& _src, const tstring& _dst);

            static  bool AllocateFile(const tstring& _path, u_int64 _fileSize);

            static u_int64 HashFile(const tstring& _path);
            static u_int64 TouchFile(const tstring& _path, byte* _data, u_int64 _size);

            static u_int64 GetFreeDiskSpace(const char *path);
            static u_int64 GetTotalDiskSpace(const char *path);


            struct SysFileInfo_t
            {
                tstring     filePath;
                u_int64     fileSize;
            };
            static SysFileInfo_t  GetFirstModifiedFile(const tstring& _path);
            static SysFileInfo_t  GetLastModifiedFile(const tstring& _path);

            typedef int(*VisitDirFunc)(const tstring& _filePath);
            static void EnumDir(const tstring& _dir, int _depth, VisitDirFunc _func);
            static int  ListDir(const tstring& _dirName, std::vector<tstring>& _names);
        };
    }
}

#endif
