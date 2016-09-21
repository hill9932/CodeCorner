#ifndef __HL_HASH_INCLUDE_H__
#define __HL_HASH_INCLUDE_H__

#include "common.h"

namespace LabSpace
{
    namespace Common
    {
        u_int16 HashValue(const byte* _value, int _size);
        u_int16 HashIp(u_int32 ip);
        u_int16 HashIpPort(u_int32 ip, u_int16 port);
        u_int16 HashFlow(u_int32 ip1, u_int16 port1, u_int32 ip2, u_int16 port2);
        u_int64 HashFlow64(u_int32 ip1, u_int16 port1, u_int32 ip2, u_int16 port2, bool c2s);
    }
}

#endif