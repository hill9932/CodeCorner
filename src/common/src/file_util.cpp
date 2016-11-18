#include "file_util.h"
#include "file_.h"
#include "stdString.h"
#include "system_.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <map>

#ifdef LINUX
#include <dirent.h>
#include <sys/types.h>
#include <libgen.h>
#endif


#define MAXPATH     1024

namespace LabSpace
{
    namespace Common
    {
        bool FileUtil::IsFileExist(const tstring& _path)
        {
            if (_path.empty())  return false;

            struct stat_ fileStat = { 0 };
            int z = stat_t(_path.c_str(), &fileStat);
            if (0 != z) return false;

            return fileStat.st_mode & S_IFREG;
        }

        bool FileUtil::IsDirExist(const tstring& _path)
        {
            if (_path.empty())  return false;

            struct stat_ fileStat = { 0 };
            int z = stat_t(_path.c_str(), &fileStat);
            if (0 != z) return false;

            return fileStat.st_mode & S_IFDIR;
        }

        bool FileUtil::CanFileAccess(const tstring& _path)
        {
            if (_path.empty())  return false;

            return access_t(_path.c_str(), 0) == 0;
        }   

        bool FileUtil::CanDirAccess(const tstring& _path)
        {
            if (_path.empty()) return false;

            tstring tmpFile = _path;
            tmpFile += "/access_detect.txt";

#ifndef WIN32
            int h = ::open(tmpFile.c_str(), O_CREAT | O_RDWR, 0644);
            if (h == -1)    return false;
            ::close(h);
            unlink(tmpFile.c_str());
#endif

            return true;
        }

        tstring FileUtil::GetAppDir()
        {
            tchar appPath[MAXPATH] = { 0 };

#ifdef WIN32
            DWORD dwSize = GetModuleFileName(NULL, appPath, MAXPATH - 1);
#else
            //getcwd(appPath, MAXPATH);
            //strcat(appPath, "/");

            int rval = readlink ("/proc/self/exe", appPath, sizeof(appPath));
            if (rval == -1)
                return "";
            else
                appPath[rval] = '\0';
#endif

            return StrUtil::GetFileDir(appPath) + "/";
        }
    
        tstring FileUtil::GetAppName()
        {
            tchar appPath[MAXPATH] = { 0 };

#ifdef WIN32
            DWORD dwSize = GetModuleFileName(NULL, appPath, MAXPATH - 1);
#else
            //getcwd(appPath, MAXPATH);
            //strcat(appPath, "/");

            int rval = readlink("/proc/self/exe", appPath, sizeof(appPath));
            if (rval == -1)
                return "";
            else
                appPath[rval] = '\0';
#endif

            tstring fileName = StrUtil::GetFileName(appPath);
            vector<tstring> v;
            StrUtil::StrSplit(fileName, ".", v);
            return v[0];
        }

        bool FileUtil::CreateAllDir(const tstring& _path)
        {
            if (_path.empty())      return false;
            if (IsDirExist(_path))  return true;

            tchar  dir_name[256] = { 0 };
            const tchar* p = _path.c_str();
            tchar* q = dir_name;

            while (*p)
            {
                if (('\\' == *p) || ('/' == *p))
                {
                    if (':' != *(p - 1))
                    {
                        mkdir_t(dir_name, DEFAULT_RIGHT);
                    }
                }
                *q++ = *p++;
                *q = '\0';
            }

            int z = mkdir_t(dir_name, DEFAULT_RIGHT);
            bool b = false;
#ifdef WIN32
            b = z || (Util::GetLastSysError() == ERROR_ALREADY_EXISTS);
#else
            b = z == 0 || Util::GetLastSysError() == EEXIST;
#endif
            ON_ERROR_PRINT_LASTMSG_S_AND_DO(b, == , false, "Creating " << dir_name, "");

            return b;
        }

        bool FileUtil::DeletePath(const tstring& _path)
        {
            assert(!_path.empty());
            if (_path.empty())  return false;

            //
            // when _path is a file, call remove
            // when it is a directory, call rmdir
            //
            struct stat_ fileStat = { 0 };
            int z = stat_t(_path.c_str(), &fileStat);
            if (0 != z) return false;

            if (fileStat.st_mode & S_IFDIR)
                return 0 == rmdir_t(_path.c_str());
            else
                return 0 == remove_t(_path.c_str());

            return false;
        }

        bool FileUtil::ClearPath(const tstring& _path, int depth/* = 2*/)
        {
            assert(!_path.empty());
            if (_path.empty())  return false;

            depth--;
#ifdef LINUX
            char realPath[PATH_MAX];
            if (!realpath(_path.c_str(), realPath))
                return false;

            struct dirent *dir;
            DIR *d = opendir(realPath);
            if (d)
            {
                struct stat states;
                while ((dir = readdir(d)) != NULL)
                {
                    if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                        continue;

                    CStdString fullFileName;
                    fullFileName.Format("%s/%s", realPath, dir->d_name);
                    if (!stat(fullFileName.c_str(), &states))
                    {
                        if (S_ISDIR(states.st_mode) && depth > 0)
                        {
                            ClearPath(fullFileName, depth);
                            if (0 != rmdir_t(fullFileName.c_str()))
                                return false; // The depth must be reached
                        }
                        else
                        {
                            if (0 != remove_t(fullFileName.c_str()))
                                return false;
                        }
                    }
                    else
                        return false;
                }
                closedir(d);
            }
            else
                return false;
#else
            if (!DeletePath(_path))
                return false;
            if (!CreateAllDir(_path))
                return false;
#endif
            return true;
        }

        bool FileUtil::CopyPath(const tstring& _src, const tstring& _dst)
        {
            if (!IsFileExist(_src) || IsDirExist(_src)) return false;

#ifdef WIN32
            return CopyFile(_src.c_str(), _dst.c_str(), TRUE);
#else
            tstring cmd = "cp ";
            cmd += _src;
            cmd += " ";
            cmd += _dst;
            return SysUtil::MySystem(cmd.c_str()) == 0;
#endif
        }

#ifdef LINUX
#include <xfs/xfs.h>
#endif

        bool FileUtil::AllocateFile(const tstring& _path, u_int64 _fileSize)
        {
            if (IsFileExist(_path)) return true;
            bool isXFS = false;

            int fd = ::open(_path.c_str(), O_RDWR | O_CREAT, 0644);
            ON_ERROR_PRINT_LASTMSG_S_AND_DO(fd, == , -1, _path, return false);

#ifdef LINUX
            if (platform_test_xfs_fd(fd) == 0)
                isXFS = true;
#endif


#ifdef WIN32
            HANDLE hfile = (HANDLE)_get_osfhandle(fd);
            LONG hiPos = _fileSize >> 32;
            SetFilePointer(hfile, _fileSize, &hiPos, FILE_BEGIN);
            SetEndOfFile(hfile);

#elif defined(LINUX)
            lseek64(fd, _fileSize, SEEK_SET);

            ftruncate(fd, _fileSize);
            if (isXFS)
            {
                xfs_flock64_t flag = { 0 };
                flag.l_whence = SEEK_SET;
                flag.l_start = 0;
                flag.l_len = _fileSize;
                xfsctl(_path.c_str(), fd, XFS_IOC_RESVSP64, &flag);
            }
            else
            {
                posix_fallocate(fd, 0, _fileSize);
            }
#endif
            close(fd);

            return true;
        }

        u_int64 FileUtil::HashFile(const tstring& _path)
        {
            if (!IsFileExist(_path)) return -1;

            CHFile file;
            u_int64 hashValue = 0;
            if (0 != file.open(_path, 
                                CHFile::FileAccessMode::ACCESS_READ, 
                                CHFile::FileAccessOption::FILE_OPEN_EXISTING, 
                                false, false))   
                return -1;

            u_int32 fileSize = file.getSize();
            hashValue = fileSize;
            hashValue <<= 32;

            u_int32 data = 0;
            for (int i = 3; i <= 103; ++i)
            {
                u_int64 offset = fileSize / i;

                file.read_w((byte*)&data, sizeof(data), offset);
                hashValue ^= data;
            }

            return hashValue;
        }

        u_int64 FileUtil::TouchFile(const tstring& _path, byte* _data, u_int64 _size)
        {
            if (!IsFileExist(_path)) return -1;
            CHFile file;

            if (0 != file.open(_path,
                               CHFile::FileAccessMode::ACCESS_READ | CHFile::FileAccessMode::ACCESS_WRITE,
                               CHFile::FileAccessOption::FILE_OPEN_EXISTING, 
                               false, false))   
                               return -1;

            u_int64 fileSize = file.getSize();
            u_int64 offset = 0;

            for (; offset + _size <= fileSize;)
            {
                if (file.read_w(_data, _size, offset) == _size)
                    file.write_w(_data, _size, offset);

                offset += _size;
            }

            return 0;
        }

        // The free space size will return in MB
        u_int64 FileUtil::GetFreeDiskSpace(const char *path)
        {
#ifdef WIN32
            ULARGE_INTEGER free, total, totalFree;
            if (GetDiskFreeSpaceEx(path, &free, &total, &totalFree))
            {
                return totalFree.QuadPart / ONE_MB;
            }
#elif defined(LINUX)
            struct statfs buf;

            if (!statfs(path, &buf)) {
                return (buf.f_bfree * buf.f_bsize) / (1024*1024);
            } else {
                return 0;
            }
#endif
            return 0;
        }

        u_int64 FileUtil::GetTotalDiskSpace(const char *path)
        {
#ifdef WIN32
            ULARGE_INTEGER free, total, totalFree;
            if (GetDiskFreeSpaceEx(path, &free, &total, &totalFree))
            {
                return total.QuadPart / ONE_MB;
            }
#elif defined(LINUX)
            struct statfs buf;

            if (!statfs(path, &buf)) {
                return (buf.f_blocks * buf.f_bsize) / (1024*1024);
            } else {
                return 0;
            }
#endif
            return 0;
        }

#ifndef LINUX
        typedef struct _dirdesc
        {
            int     dd_fd;      /** file descriptor associated with directory */
            long    dd_loc;     /** offset in current buffer */
            long    dd_size;    /** amount of data returned by getdirentries */
            char    *dd_buf;    /** data buffer */
            int     dd_len;     /** size of data buffer */
            long    dd_seek;    /** magic cookie returned by getdirentries */
        } DIR;

# define __dirfd(dp)    ((dp)->dd_fd)

        DIR *opendir(const char *);
        struct dirent *readdir(DIR *);
        void rewinddir(DIR *);
        int closedir(DIR *);

#include <sys/types.h>

        struct dirent
        {
            long d_ino;              /* inode number*/
            off_t d_off;             /* offset to this dirent*/
            unsigned short d_reclen; /* length of this d_name*/
            unsigned char d_type;    /* the type of d_name*/
            char d_name[1];          /* file name (null-terminated)*/
        };

        static std::map<void*, HANDLE> g_hFindMap;   // TODO, no thread safe at this moment

        DIR *opendir(const char *name)
        {
            DIR *dir;
            WIN32_FIND_DATA FindData;
            char namebuf[512] = { 0 };

            sprintf(namebuf, "%s\\*.*", name);

            HANDLE hFind = FindFirstFile(namebuf, &FindData);
            if (hFind == INVALID_HANDLE_VALUE)
            {
                printf("FindFirstFile failed (%d)\n", GetLastError());
                return 0;
            }

            dir = (DIR *)malloc(sizeof(DIR));
            if (!dir)
            {
                FindClose(hFind);
                return 0;
            }

            memset(dir, 0, sizeof(DIR));
            dir->dd_fd = 0;   // simulate return  

            g_hFindMap[dir] = hFind;

            return dir;
        }

        struct dirent *readdir(DIR *d)
        {
            static struct dirent dirent;
            WIN32_FIND_DATA FileData;
            if (!d || g_hFindMap.count(d) == 0) return 0;

            HANDLE hFind = g_hFindMap[d];
            BOOL bf = FindNextFile(hFind, &FileData);
            if (!bf)
                return 0;

            int i = 0;
            for (i = 0; i < 256; i++)
            {
                dirent.d_name[i] = FileData.cFileName[i];
                if (FileData.cFileName[i] == '\0') break;
            }
            dirent.d_reclen = i;
            dirent.d_reclen = FileData.nFileSizeLow;

            //check there is file or directory  
            if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                dirent.d_type = 2;
            }
            else
            {
                dirent.d_type = 1;
            }


            return (&dirent);
        }

        int closedir(DIR *d)
        {
            if (!d || g_hFindMap.count(d) == 0) return -1;

            HANDLE hFind = g_hFindMap[d];
            FindClose(hFind);
            g_hFindMap.erase(d);

            free(d);
            return 0;
        }

#endif

        FileUtil::SysFileInfo_t FileUtil::GetFirstModifiedFile(const tstring& _dir)
        {
            SysFileInfo_t fileInfo;
            fileInfo.fileSize = 0;

            DIR *dp;
            struct dirent *entry;
            if ((dp = opendir(_dir.c_str())) == NULL)
            {
                ON_ERROR_PRINT_LASTMSG_S_AND_DO(dp, == , NULL, "opendir: " << _dir, "");
                return fileInfo;
            }

            time_t lastTime = time(NULL);
            while ((entry = readdir(dp)) != NULL)
            {
                if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                    continue;

                struct stat_ fileStat = { 0 };
                tstring filePath = _dir;
                filePath += "/";
                filePath += entry->d_name;

                int z = stat_t(filePath.c_str(), &fileStat);
                if (0 != z) continue;

                if (S_IFDIR & fileStat.st_mode)
                {

                }
                else if (fileStat.st_mtime < lastTime)
                {
                    lastTime = fileStat.st_mtime;

                    fileInfo.filePath = _dir;
                    fileInfo.filePath += "/";
                    fileInfo.filePath += entry->d_name;
                    fileInfo.fileSize = fileStat.st_size;
                }
            }

            closedir(dp);

            return fileInfo;
        }

        FileUtil::SysFileInfo_t FileUtil::GetLastModifiedFile(const tstring& _dir)
        {
            SysFileInfo_t fileInfo;
            fileInfo.fileSize = 0;

            DIR *dp;
            struct dirent *entry;
            if ((dp = opendir(_dir.c_str())) == NULL)
            {
                ON_ERROR_PRINT_LASTMSG_S_AND_DO(dp, == , NULL, "opendir: " << _dir, "");
                return fileInfo;
            }

            time_t lastTime = 0;
            while ((entry = readdir(dp)) != NULL)
            {
                if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                    continue;

                struct stat_ fileStat = { 0 };
                tstring filePath = _dir;
                filePath += "/";
                filePath += entry->d_name;

                int z = stat_t(filePath.c_str(), &fileStat);
                if (0 != z) continue;

                if (S_IFDIR & fileStat.st_mode)
                {

                }
                else if (fileStat.st_mtime > lastTime)
                {
                    lastTime = fileStat.st_mtime;

                    fileInfo.filePath = _dir;
                    fileInfo.filePath += "/";
                    fileInfo.filePath += entry->d_name;
                    fileInfo.fileSize = fileStat.st_size;
                }
            }

            closedir(dp);

            return fileInfo;
        }

        int FileUtil::ListDir(const tstring& _dirName, vector<tstring>& _names)
        {
            if (_dirName.empty())  return -1;

            DIR *p_dir = NULL;
            struct dirent *p_dirent = NULL;
            p_dir = opendir(_dirName.c_str());
            ON_ERROR_PRINT_LASTMSG_AND_DO(p_dir, == , NULL, "");

            while ((p_dirent = readdir(p_dir)))
            {
                if (p_dirent->d_name[0] != '.')
                    _names.push_back(p_dirent->d_name);
            }
            closedir(p_dir);
            return 0;
        }

        void FileUtil::EnumDir(const tstring& _dir, int _depth, VisitDirFunc _func)
        {
            if (_depth < 0 || _dir.empty() || !_func)    return;

            DIR *dp = NULL;
            struct dirent *entry;
            if ((dp = opendir(_dir.c_str())) == NULL)
            {
                ON_ERROR_PRINT_LASTMSG_S_AND_DO(dp, == , NULL, "opendir: " << _dir, "");
                return;
            }

            time_t lastTime = 0;
            while ((entry = readdir(dp)) != NULL)
            {
                if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                    continue;

                struct stat_ fileStat = { 0 };
                tstring filePath = _dir;
                filePath += "/";
                filePath += entry->d_name;

                int z = stat_t(filePath.c_str(), &fileStat);
                ON_ERROR_PRINT_LASTMSG_S_AND_DO(0, != , z, "Get stat of " << filePath, continue);

                if (S_IFDIR & fileStat.st_mode)
                {
                    EnumDir(filePath, _depth > 0 ? (_depth - 1) : 1, _func);
                }
                else
                {
                    _func(filePath);
                }
            }

            closedir(dp);
        }
    }
}