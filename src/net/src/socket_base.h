#ifndef __HL_SOCKET_BASE_INCLUDE_H__
#define __HL_SOCKET_BASE_INCLUDE_H__

#include "inetaddr.h"


#ifdef WIN32
    #include <MSWSock.h>
#else
    #define LPFN_ACCEPTEX   void*
    #define LPFN_GETACCEPTEXSOCKADDRS   void*
#endif

namespace LabSpace
{
    namespace Net
    {

        /**********************************/
        //     CSocketBase
        /**********************************/
        class CSocketBase
        {
        public:
            CSocketBase(bool _auto_close = true);
            virtual ~CSocketBase();

            /**
             * @Function: create a socket according to the input
             * @Return:
             *  0: success
             *  -1: if reuseAddr is true, failed to setoption
             *  others: see errno or WSAGetLastError
             *
             **/
            virtual int open(int _family, int _type, int _protocol, bool _reuseAddr = false);
            virtual int close();
            virtual int shutdown(int _opt);
            virtual int show();
            virtual int getType();

            int createAndBind(char* _port);

            operator SOCKET()   { return m_hSocket; }
#ifdef WIN32
            operator HANDLE()   { return (HANDLE)m_hSocket; }
#endif
            bool operator !()   { return m_hSocket == INVALID_SOCKET; }

            /**
             * @Function: call setsockopt to set options to the socket
             * @Return:
             *  0: success
             *  -1: parameter wrong
             *  others: see errno or WSAGetLastError
             **/
            int setOption(int _level, int _option, const void* _optVal, int _optLen);
            int getOption(int _level, int _option, void* _optVal, int* _optLen);

            /**
             * @Function: set the socket in block/nonblock mode
             * @param block: if true, set in block mode, otherwise in nonblock mode
             **/
            int setBlockMode(bool block);
            virtual bool getBlockMode();

            /**
            * @Function: call ioctlsocket to set options to the socket
            * @Return:
            *  0: success
            *  -1: parameter wrong
            *  others: see errno or WSAGetLastError
            * @example:
            *   Control(FIONBIO, 1);
            **/
            int control(int _cmd, void* _arg);

            /**
             * @Function: get the address of the remotely connected peer
             **/
            int getRemoteAddr(CInetAddr& _addr);
            int getLocalAddr(CInetAddr& _addr);

            int bind(CInetAddr& _addr);
            void setHandle(int _sock) { if (_sock > 0) m_hSocket = _sock; }

            /**
             * @Function: connect to the remote address
             * @Param _addr: the remote address
             * @Param _timeout: == 0 wait for ever until success or fail
             *                  < 0 return immediately
             *                  > 0 wait specified time until success or fail
             * @Return 0: success
             *         errno
             **/
            int connect(const CInetAddr& _addr, int _timeout = 0);
            int isConnect(fd_set* _rd, fd_set* _wr, fd_set* ex);

            /**
             * @Function: receive/send specified byte buffer from the connected socket
             **/
            int recv(char* _buffer, u_int16 _len, int _flag = 0);
            int send(const char* _buffer, u_int16 _len, int _flag = 0);

            /**
             * @Function: receive/send an <iov> of size <count> from the connected socket
             **/
            int recvV(iovec _iov[], u_int16 _count);
            int sendV(const iovec _iov[], u_int16 _count);

        protected:
            /**
             * @Function: windows may call WSASocket to create
             **/
            virtual SOCKET createSocket(int _family, int _type, int _protocol);

        protected:
            SOCKET  m_hSocket;
            bool    m_auto_close;
        };


        /**********************************/
        //     CSocketTcp
        /**********************************/
        class CSocketTcp : public CSocketBase
        {
        public:
            CSocketTcp(bool _auto_close = true) : CSocketBase(_auto_close)
            {}
            virtual ~CSocketTcp()
            {}

            virtual int getType()
            {
                return SOCK_STREAM;
            }

            int open(bool _reuseAddr = true)
            {
                return CSocketBase::open(AF_INET, SOCK_STREAM, 0, _reuseAddr);
            }

            int listen(int _backlog);

            /**
            * @Function: when receive client connect request, return a new socket
            **/
            int accept(CSocketTcp& _new_socket);
        };


        /**********************************/
        //     CSocketUdp
        /**********************************/
        class CSocketUdp : public CSocketBase
        {
        public:
            CSocketUdp(bool _auto_close = true) : CSocketBase(_auto_close)
            {}
            virtual ~CSocketUdp()
            {}

            virtual int getType()
            {
                return SOCK_DGRAM;
            }

            int open(bool _reuseAddr = false)
            {
                return CSocketBase::open(AF_INET, SOCK_DGRAM, 0, _reuseAddr);
            }

            int sendTo(const CInetAddr& _target, char* _data, int _dataLen);
            int recvFrom(CInetAddr& _remote, char* _data, int _dataLen);
        };


        /**********************************/
        //     CSocketGroup
        /**********************************/
        class CSocketGroup : public CSocketUdp
        {
        public:
            CSocketGroup(bool _auto_close = true) : CSocketUdp(_auto_close)
            {}
            virtual ~CSocketGroup()
            {}

            static u_int32 getGroupAddress();

            int open(CInetAddr& _local_if);

            /**
            setsockopt()            getsockopt()
            IP_MULTICAST_LOOP           yes                     yes
            IP_MULTICAST_TTL            yes                     yes
            IP_MULTICAST_IF             yes                     yes
            IP_ADD_MEMBERSHIP           yes                      no
            IP_DROP_MEMBERSHIP          yes                      no
            **/
            int setTTL(int _ttl);
            int setLoopback(int _enable);
            int joinGroup(CInetAddr& _group_address);
            int leaveGroup(CInetAddr& _group_address);
            int joinSSM(CInetAddr& _group_address, CInetAddr& _source_address);
            int leaveSSM(CInetAddr& _group_address, CInetAddr& _source_address);
        };


        enum SOCKET_OPERATION_TYPE
        {
            SOCKET_ACCEPT,
            SOCKET_CONN,
            SOCKET_SEND,
            SOCKET_RECV
        };

#define MAX_BUFFER_LEN  1024
        typedef struct _PER_IO_CONTEXT
        {
            OVERLAPPED   m_overlapped;
            SOCKET       m_socket;
            SOCKET_OPERATION_TYPE  m_opt_type;
            WSABUF       m_WSABuf;
            char         m_buffer[MAX_BUFFER_LEN];
        } PER_IO_CONTEXT_t, *PPER_IO_CONTEXT_t;


        /**********************************/
        //     CSocketTcpEx
        /**********************************/
        class CSocketTcpEx : public CSocketTcp
        {
        public:
            CSocketTcpEx(bool _auto_close = true);
            ~CSocketTcpEx();

            int connectEx(CInetAddr& _server_address, PPER_IO_CONTEXT_t _io_context);
            int acceptEx(CSocketTcpEx& _new_socket, PPER_IO_CONTEXT_t _io_context);
            int recvEx(PPER_IO_CONTEXT_t _io_context);
            int sendEx(PPER_IO_CONTEXT_t _io_context, DWORD _bytes);

        protected:
            /**
             * @Function: windows may call WSASocket to create
             **/
            virtual SOCKET createSocket(int _family, int _type, int _protocol);
            virtual bool getBlockMode() { return false; }

        public:
            static LPFN_ACCEPTEX    m_lpfnAcceptEx;         // AcceptE
            static LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptExSockAddrs;

            char m_buffer[128];  // keep the local and remote addresses saved by AcceptEx
        };


        class CSocketUdpEx : public CSocketUdp
        {
        public:
            CSocketUdpEx(bool _auto_close = true);
            ~CSocketUdpEx();

            int recvfromEx(PPER_IO_CONTEXT_t _io_context);
            int sendtoEx(PPER_IO_CONTEXT_t _io_context, DWORD _bytes, CInetAddr& _to_addr);

        protected:
            /**
            * @Function: windows may call WSASocket to create
            **/
            virtual SOCKET createSocket(int _family, int _type, int _protocol);
            virtual bool getBlockMode() { return false; }

        };

    }
}

#endif
