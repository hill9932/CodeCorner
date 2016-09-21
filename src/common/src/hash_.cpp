#include "hash_.h"

namespace LabSpace
{
    namespace Common
    {
        u_int16 HashValue(const byte* _value, int _size)
        {
            u_int16 result = 0;
            const u_int16* val = (const u_int16*)_value;
            for (int i = 0; i < _size / 2; ++i)
            {
                result ^= val[i];
            }

            return result;
        }

        u_int16 HashIp(u_int32 ip)
        {
            return HashValue((const byte*)&ip, sizeof(ip));
        }

        u_int16 HashIpPort(u_int32 ip, u_int16 port)
        {
            ip ^= (u_int32)port << 10;
            return HashValue((const byte*)&ip, sizeof(ip));
        }

        u_int16 HashFlow(u_int32 ip1, u_int16 port1, u_int32 ip2, u_int16 port2)
        {
            bool c2s = port1 > port2 || (port1 == port2 && ip1 > ip2);
            u_int64 flow = HashFlow64(ip1, port1, ip2, port2, c2s);
            return HashValue((const byte*)&flow, sizeof(flow));
        }

        u_int64 HashFlow64(u_int32 ip1, u_int16 port1, u_int32 ip2, u_int16 port2, bool c2s)
        {
            register u_int32 val1;
            register u_int32 val2;
            if (c2s)
            {
                val1 = ((ip2 << 1) ^ ip1) ^ ((port2 << 1) ^ port1);
                val2 = (ip2 << 3) + ip1 + (u_int32(port2) << 19) + (u_int32(port1) << 7);
            }
            else
            {
                val1 = ((ip1 << 1) ^ ip2) ^ ((port1 << 1) ^ port2);
                val2 = (ip1 << 3) + ip2 + (u_int32(port1) << 19) + (u_int32(port2) << 7);
            }
            return (u_int64)val2 << 32 | val1;
        }
    }
}