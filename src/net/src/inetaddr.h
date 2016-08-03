#ifndef __HL_INETADDR_INCLUDE_H__
#define __HL_INETADDR_INCLUDE_H__

#include "log_.h"
#include <sstream>

namespace LabSpace
{
    namespace Net
    {
#if !defined(IP_ADD_SOURCE_MEMBERSHIP)
        struct ip_mreq_source
        {
            struct  in_addr imr_multiaddr;  // IP multicast address of group 
            struct  in_addr imr_sourceaddr; // IP address of source 
            struct  in_addr imr_interface;  // local IP address of interface 
        };
#endif

#ifndef IP_ADD_SOURCE_MEMBERSHIP

#ifdef LINUX
#define IP_ADD_SOURCE_MEMBERSHIP    39
#define IP_DROP_SOURCE_MEMBERSHIP   40
#else
#define IP_ADD_SOURCE_MEMBERSHIP    25
#define IP_DROP_SOURCE_MEMBERSHIP   26
#endif

#endif

        typedef struct local_addr
        {
            struct sockaddr_storage addr;
            int     addr_len;
            struct  local_addr * next;
        } local_addr_t;

        //
        // This class represents the INET family address including INET4 and INET6
        //
        class CInetAddr
        {
        public:
            CInetAddr();
            CInetAddr(u_int32 _addr);
            CInetAddr(const char *_addr, const char *_protocol);
            CInetAddr(sockaddr_storage& _addr);
            CInetAddr(const in_addr& _addr);
            CInetAddr(const sockaddr_in& _addr);
            CInetAddr(const sockaddr& _addr);

            /**
               local_addr_t* addr = NULL, *tmp = NULL;
               CInetAddr::getLocalAddrs(&addr, AF_INET6);
               tmp = addr;
               while (addr)
               {
               CInetAddr iaddr(addr->addr);
               cout << iaddr.ip() << endl;
               addr = addr->next;
               }

               CInetAddr::freeLocalAddrs(tmp);
            */
            static int  getLocalAddrs(local_addr_t**, int _family = AF_INET, int _type = SOCK_STREAM);
            static void freeLocalAddrs(local_addr_t*&);
            static int  isValidPort(const tstring& _port);
            static int  isIPAddress(const char* _ipAddr);

            /**
             * @Function: Create an AF_INET Address
             * @Param _addr[out]:Ptr to area where address is to be placed.
             * @Param _addrlen:  Ptr to int that will hold the final address length.
             * @Param _str_addr: The input string format hostname, and port.
             * @Param _protocol: The input string indicating the protocol being used.
             *                   NULL implies "tcp".
             * @Return:
             *  0:  Success.
             *  -1: Bad host part
             *  -2: Bad port part
             *  -3: Invalid parameter
             * @Notes:
             *  "*" for the host portion of the address implies INADDR_ANY.
             *  "*" for the port portion will imply zero for the port (assign a port number).
             * @Examples:
             *  "www.lwn.net:80"
             *  "localhost:telnet"
             *  "*:21"
             *  "*:*"
             *  "ftp.redhat.com:ftp"
             *  "sunsite.unc.edu"
             **/
            int makeAddr(const char *_strAddr, const char *_protocol, bool _checkHostName);
            bool isGroupAddress();

            bool operator == (const CInetAddr& _addr) const;
            bool operator <  (const CInetAddr& _addr) const;

            operator void*()            { return &m_storage; }
            operator char*()            { return (char*)&m_storage; }
            operator sockaddr_in *()    { return (sockaddr_in *)&m_storage; }
            operator sockaddr_in6 *()   { return (sockaddr_in6 *)&m_storage; }
            operator sockaddr* () const { return (sockaddr*)&m_storage; }
            operator u_int32() const    { return ((sockaddr_in*)this)->sin_addr.s_addr; }

            int family() const          { return m_storage.ss_family; }

            /**
             * @Function: return the real size of address.
             *  for INET family it is sizeof(sockaddr_in),
             *  for INET6 family it is sizeof(sockaddr_in6)
             **/
            int size() const;
            int port() const;
            int len()  const { return sizeof(m_storage); }

            /**
             * @Function: return the string format of address like 127.0.0.1:8090
             **/
            tstring toString() const;

            /**
             * @Function: return the string format of ip address like 127.0.0.1
             **/
            tstring ip() const;

            /**
             * @Function: return the host name of the ip address
             **/
            tstring hostname() const;

        private:
            sockaddr_storage    m_storage;
        };


        /**
        * @Function: Create the socket client
        * @Param addr: specify the server address to connect
        */
        int client(CInetAddr& _server_addr);

        /**
         * @Function: Create the socket server
         * @Param addr: specify the local address and port to listen on
         */
        int server(CInetAddr& _listen_addr);
    }
}

#endif
