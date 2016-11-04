#ifndef __HL_COMMON_INCLUDE_H__
#define __HL_COMMON_INCLUDE_H__

#include "def.h"
#include "locker_.h"


namespace LabSpace
{
    namespace Common
    {
        struct Util
        {
            /**
            * @Function: Get the error description specified by error code
            *  typically, error code would be errno in Linux and GetLastError() in Windows
            **/
            static tstring  GetLastSysErrorMessage(u_int32 _errCode = 0, bool _herror = false);
            static u_int32  GetLastSysError(bool _herror = false);
            static void     SetLastSysError(u_int32 _errCode);
            static int      Random(int _low, int _high);

            static tstring  ChangeUnit(u_int64 _count, u_int32 _unit, const tchar* _suffix, int _maxLevel);

            static int      CreateThreadKey(ThreadSpecificKey* _key, void(*destructor)(void *));
            static int      DeleteThreadKey(ThreadSpecificKey _key);
            static void     SetThreadValueWithKey(ThreadSpecificKey _key, void* _value);
            static void*    GetThreadValueWithKey(ThreadSpecificKey _key);
        };

        /**
        * @Function: min()/max()
        **/
        template<typename T>
        inline const T& MyMin(const T& _a, const T& _b)
        {
            return _b < _a ? _b : _a;
        }

        template<typename T>
        inline const T& MyMax(const T& _a, const T& _b)
        {
            return _a < _b ? _b : _a;
        }
    }
}

using namespace LabSpace::Common;

#endif
