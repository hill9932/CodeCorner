#include "common.h"
#include "stdString.h"

namespace LabSpace
{
    namespace Common
    {
        /**
        * @Function: Get the last error code
        **/
        u_int32 Util::GetLastSysError(bool _herror)
        {
            u_int32 errCode = 0;

#ifdef WIN32
            errCode = _herror ? WSAGetLastError() : GetLastError();
#else
            errCode = _herror ? h_errno : errno;
#endif

            return errCode;
        }

        void Util::SetLastSysError(u_int32 _errCode)
        {
            errno = _errCode;
#ifdef WIN32
            SetLastError(_errCode);
#endif
        }

        /**
        * @Function: Get the error description specified by error code
        *  typically, error code would be errno in Linux and GetLastError() in Windows
        *
        * @Param _errCode: the error code, mostly it is retrieved by GetLastSysError()
        * @Param _herror: indicates whether it is an network error
        **/
        tstring Util::GetLastSysErrorMessage(u_int32 _errCode, bool _herror)
        {
            string errMsg;
            if (0 == _errCode)
            {
#ifdef WIN32
                _errCode = GetLastError();
#else
                _errCode = _herror ? h_errno : errno;
#endif
            }

            if (0 == _errCode)  return "";

#ifdef WIN32
            LPSTR lpMsgBuf = NULL;

            FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                _errCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&lpMsgBuf,
                0,
                NULL);

            if (lpMsgBuf)
            {
                errMsg = lpMsgBuf;
                LocalFree(lpMsgBuf);
            }
#else
            errMsg = strerror(_errCode);
#endif

            return errMsg;
        }

        tstring Util::ChangeUnit(u_int64 _count, u_int32 _unit, const tchar* _suffix, int _maxLevel)
        {
            if (!_suffix)   return " ";

            int level = 0;
            const tchar* unit[] = { " ", "K", "M", "G", "T", "P", "E" };
            int i = 0;
            double dd = _count;
            while (dd >= _unit && i < _maxLevel)
            {
                dd /= _unit;
                ++i;
            }
            if (i >= sizeof(unit) / sizeof(unit[0]))    return " ";

            CStdString v;
            v.Format(TEXT("%.2f %s%s"), dd, unit[i], _suffix);
            return v;
        }


#if defined(LINUX)
        int Util::CreateThreadKey(ThreadSpecificKey* _key, void(*destructor)(void *))
        {
            int err = pthread_key_create(_key, destructor);
            ON_ERROR_PRINT_LAST_ERROR(err, != , 0);
            return err;
        }

        int Util::DeleteThreadKey(ThreadSpecificKey _key)
        {
            int err = pthread_key_delete(_key);
            ON_ERROR_PRINT_LAST_ERROR(err, != , 0);
            return err;
        }

        void Util::SetThreadValueWithKey(ThreadSpecificKey _key, void* _value)
        {
            pthread_setspecific(_key, _value);
        }

        void* Util::GetThreadValueWithKey(ThreadSpecificKey _key)
        {
            return pthread_getspecific(_key);
        }

#else
        int Util::CreateThreadKey(ThreadSpecificKey* _key, void(*destructor)(void *))
        {
            *_key = TlsAlloc();
            return 0;
        }

        int Util::DeleteThreadKey(ThreadSpecificKey _key)
        {
            TlsFree(_key);
            return 0;
        }

        void Util::SetThreadValueWithKey(ThreadSpecificKey _key, void* _value)
        {
            TlsSetValue(_key, _value);
        }

        void* Util::GetThreadValueWithKey(ThreadSpecificKey _key)
        {
            return TlsGetValue(_key);
        }

#endif

    }
}