#include "inetaddr.h"

namespace LabSpace
{
    namespace Net
    {
        #define UNDEFINED_PORT  13

        int CInetAddr::init()
        {
            int err = 0;

#ifdef WIN32
            WORD wVersionRequested;
            WSADATA wsaData;
            wVersionRequested = MAKEWORD(2, 2);

            err = WSAStartup(wVersionRequested, &wsaData);
            ON_ERROR_PRINT_LASTMSG_AND_DO(err, != , 0, ;);
#endif
            return err;
        }

        int CInetAddr::cleanup()
        {
            int err = 0;
#ifdef WIN32
            WSACleanup();
#endif
            return err;
        }

        CInetAddr::CInetAddr()
        {
            memset(&m_storage, 0, sizeof(m_storage));
        }

        CInetAddr::CInetAddr(const char *_addr, const char *_protocol)
        {
            assert(_addr);
            memset(&m_storage, 0, sizeof(m_storage));

            if (0 != makeAddr(_addr, _protocol, true))
                throw "Fail";
        }

        CInetAddr::CInetAddr(u_int32 _addr)
        {
            memset(&m_storage, 0, sizeof(m_storage));
            m_storage.ss_family = AF_INET;
            sockaddr_in* addr = (sockaddr_in*)&m_storage;
            addr->sin_addr.s_addr = htonl(_addr);
            addr->sin_port = UNDEFINED_PORT;
        }

        CInetAddr::CInetAddr(const in_addr& _addr)
        {
            memset(&m_storage, 0, sizeof(m_storage));

            m_storage.ss_family = AF_INET;

            sockaddr_in* addr = (sockaddr_in*)&m_storage;
            addr->sin_addr = _addr;
            addr->sin_port = UNDEFINED_PORT;
        }

        CInetAddr::CInetAddr(const sockaddr_in& _addr)
        {
            memcpy(&m_storage, &_addr, sizeof(_addr));
        }

        CInetAddr::CInetAddr(const sockaddr& _addr)
        {
            memcpy(&m_storage, &_addr, sizeof(_addr));
        }

        CInetAddr::CInetAddr(sockaddr_storage& _addr)
        {
            memcpy(&m_storage, &_addr, sizeof(_addr));
        }

        bool CInetAddr::operator == (const CInetAddr& _addr) const
        {
            return memcmp(this, &_addr, sizeof(_addr)) == 0;
        }

        bool CInetAddr::operator < (const CInetAddr& _addr) const
        {
            if (*this == _addr) return false;

            if (family() > _addr.family())
            {
                return false;
            }
            else if (family() < _addr.family())
            {
                return true;
            }

            if (family() == AF_INET)
            {
                return ((sockaddr_in*)this)->sin_addr.s_addr < ((sockaddr_in*)&_addr)->sin_addr.s_addr
                    || (((sockaddr_in*)this)->sin_addr.s_addr == ((sockaddr_in*)&_addr)->sin_addr.s_addr
                    && port() < _addr.port());
            }
            else
            {
                return strcmp((char*)((sockaddr_in6*)this)->sin6_addr.s6_addr, (char*)((sockaddr_in6*)&_addr)->sin6_addr.s6_addr) < 0
                    || ((strcmp((char*)((sockaddr_in6*)this)->sin6_addr.s6_addr, (char*)((sockaddr_in6*)&_addr)->sin6_addr.s6_addr)) == 0
                    && port() < _addr.port());
            }

            return false;
        }

        int CInetAddr::size() const
        {
            int len = 0;
            if (m_storage.ss_family == AF_INET)
                len = sizeof(struct sockaddr_in);
            else if (m_storage.ss_family == AF_INET6)
                len = sizeof(struct sockaddr_in6);

            return len;
        }

        tstring CInetAddr::toString() const
        {
            stringstream    ss;
            ss << ip() << " : " << port();

            return ss.str();
        }

        tstring CInetAddr::ip() const
        {
            int f = family();
            if (f == AF_INET)
            {
                sockaddr_in* listen_addr_s = (sockaddr_in*)&m_storage;
                return inet_ntoa(listen_addr_s->sin_addr);
            }
            else if (f == AF_INET6)
            {
                sockaddr_in6* listen_addr_s = (sockaddr_in6*)&m_storage;
                char addrv6[128] = { 0 };
                inet_ntop(AF_INET6, &listen_addr_s->sin6_addr, addrv6, sizeof addrv6);
                return addrv6;
            }

            return "Invalid address";
        }

        int CInetAddr::port() const
        {
            int f = family();
            if (f == AF_INET)
            {
                sockaddr_in* listen_addr_s = (sockaddr_in*)&m_storage;
                return ntohs(listen_addr_s->sin_port);
            }
            else if (f == AF_INET6)
            {
                sockaddr_in6* listen_addr_s = (sockaddr_in6*)&m_storage;
                return ntohs(listen_addr_s->sin6_port);
            }

            return -1;
        }

        tstring CInetAddr::hostname() const
        {
            char name[128] = "invalid";
            int f = family();
            struct hostent *hp = NULL;

            if (f == AF_INET)
            {
                sockaddr_in* client_addr_s = (sockaddr_in*)&m_storage;
                hp = gethostbyaddr((char *)&client_addr_s->sin_addr,
                    sizeof client_addr_s->sin_addr,
                    client_addr_s->sin_family);

                snprintf_t(name, sizeof name, "%s (%s:%u)",
                    hp ? hp->h_name : "Anonymous",
                    inet_ntoa(client_addr_s->sin_addr),
                    (unsigned)ntohs(client_addr_s->sin_port));
            }
            else if (f == AF_INET6)
            {
                sockaddr_in6* client_addr_s = (sockaddr_in6*)&m_storage;
                hp = gethostbyaddr((char *)&client_addr_s->sin6_addr,
                    sizeof client_addr_s->sin6_addr,
                    client_addr_s->sin6_family);

                char addrv6[128] = { 0 };
                inet_ntop(AF_INET6, &client_addr_s->sin6_addr, addrv6, sizeof addrv6);
                snprintf_t(name, sizeof name, "%s (%s:%u)",
                    hp ? hp->h_name : "Anonymous",
                    addrv6,
                    (unsigned)ntohs(client_addr_s->sin6_port));
            }
            else
            {

            }
            return name;
        }

        bool CInetAddr::isGroupAddress()
        {
            int addr = htonl(((sockaddr_in*)this)->sin_addr.s_addr);
            return addr > 0xE00000FF && addr <= 0xEFFFFFFF;
        }

        int CInetAddr::getLocalAddrs(local_addr_t** _addrs, int _family, int _type)
        {
            int n = 0;
            char buffer[256] = { 0 };
            int ret = gethostname(buffer, sizeof(buffer));
            ON_ERROR_PRINT_LASTMSG_AND_DO(ret, != , 0, return (-ret));

            struct addrinfo hints, *res;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = _family;
            hints.ai_socktype = _type;

            ret = getaddrinfo(buffer, NULL, &hints, &res);
            ON_ERROR_RETURN_ERRCODE(ret, == , -1);

            if (*_addrs) *_addrs = NULL;
            struct addrinfo *tmp = res;
            while (tmp)
            {
                local_addr_t *addr = new local_addr_t;
                addr->addr_len = tmp->ai_addrlen;
                memcpy(&addr->addr, tmp->ai_addr, addr->addr_len);
                addr->next = NULL;

                if (!(*_addrs))
                {
                    *_addrs = addr;
                }
                else
                {
                    addr->next = *_addrs;
                    *_addrs = addr;
                }

                tmp = tmp->ai_next;
                ++n;
            }

            freeaddrinfo(res);

            return n;
        }

        void CInetAddr::freeLocalAddrs(local_addr_t*& _addr)
        {
            local_addr_t *tmp;

            while (_addr)
            {
                tmp = _addr;
                _addr = _addr->next;
                delete tmp;
            }
        }

        int CInetAddr::isIPAddress(const char* _ipAddr)
        {
            if (!_ipAddr)   return false;

            struct in_addr addr4;
            struct in6_addr addr6;

            if (strchr(_ipAddr, ':') != NULL) // IPv6
            {
                if (inet_pton(AF_INET6, _ipAddr, &addr6) == 1)
                    return 6;
            }
            else
            {
                if (inet_pton(AF_INET, _ipAddr, &addr4) == 1)
                    return 4;
            }

            return -1;
        }

        int CInetAddr::isValidPort(const tstring& _port)
        {
            if (!StrUtil::IsAllNumber(_port))   return -1;

            int port = atoi(_port.c_str());
            if (port > 0 && port <= 0xFFFF)
                return port;

            return -1;
        }

        int CInetAddr::makeAddr(const char* _addr, const char* _protocol, bool _checkHostName)
        {
            if (!_addr)    return -3;

            std::shared_ptr<char>inp_addr(strdup(_addr));
            const char *host_part = strtok(inp_addr.get(), ":");
            const char *port_part = strtok(NULL, "\n");
            struct sockaddr_in *ap = (sockaddr_in *)this;
            struct hostent *hp = NULL;
            struct servent *sp = NULL;
            char *cp = NULL;
            long lv;
            int ipVersion = 0;

            if (!host_part) host_part = "*";
            if (!port_part) port_part = "*";
            if (!_protocol)  _protocol = "tcp";

            ap->sin_family = AF_INET;
            ap->sin_port = 0;
            ap->sin_addr.s_addr = INADDR_ANY;

            //
            // Fill in the host address:
            //
            if (strcmp(host_part, "*") == 0);   // Leave as INADDR_ANY
            else if ((ipVersion = isIPAddress(host_part)) > 0)       // Numeric IP address
            {
                if (inet_pton(ipVersion == 4 ? AF_INET : AF_INET6, host_part, &ap->sin_addr) <= 0)
                    return -1;
                /*      ap->sin_addr.s_addr = inet_addr(host_part);
                      if (ap->sin_addr.s_addr == INADDR_NONE) return -1;
                      */
            }
            else if (_checkHostName)   // Assume a hostname
            {
                hp = gethostbyname(host_part);
                if (!hp)                        return -1;
                if (hp->h_addrtype != AF_INET &&
                    hp->h_addrtype != AF_INET6)  return -1;

                ap->sin_family = hp->h_addrtype;

                if (hp->h_addrtype == AF_INET)
                    ap->sin_addr = *(struct in_addr *) hp->h_addr_list[0];
                else if (hp->h_addrtype == AF_INET6)
                    ((struct sockaddr_in6*)ap)->sin6_addr = *(struct in6_addr *) hp->h_addr_list[0];
            }
            else
            {
                return -1;
            }

            //
            // Process an optional port
            //
            if (!strcmp(port_part, "*"));       // Leave as wild (zero)
            else if (isdigit(*port_part))       // Process numeric port
            {
                lv = strtol(port_part, &cp, 10);
                if (cp != NULL && *cp)          return -2;  // there is invalid character
                if (lv < 0L || lv >= 65535)     return -2;  // the port number is invalid

                ap->sin_port = htons((short)lv);
            }
            else    // Lookup the service
            {
                sp = getservbyname(port_part, _protocol);
                if (!sp)                        return -2;
                ap->sin_port = (short)sp->s_port;
            }

            return 0;
        }
    }
}
