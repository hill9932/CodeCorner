/**
 * File:        socketBase.cpp
 *
 * Copyright (C) 2014 - 2015  PolyVirtual, Inc.
 * All rights reserved.
 *
 * Abstract: 
 *
 */
#include "socket_base.h"
#include <fcntl.h>

using namespace LabSpace::Common;

namespace LabSpace
{
    namespace Net
    {
        CSocketBase::CSocketBase(bool _auto_close)
        {
            m_hSocket = INVALID_SOCKET;
            m_auto_close = _auto_close;
        }

        CSocketBase::~CSocketBase()
        {
            if (m_auto_close)
                close();
        }

        SOCKET CSocketBase::createSocket(int _family, int _type, int _protocol)
        {
            return ::socket(_family, _type, _protocol);
        }

        int CSocketBase::open(int _family, int _type, int _protocol, bool _reuseAddr)
        {
            int ret = -1;
            close();

            m_hSocket = createSocket(_family, _type, _protocol);
            if (m_hSocket != INVALID_SOCKET)
            {
                ret = 0;
                if (_family != PF_UNIX && _reuseAddr)
                {
                    int reuse = 1;
                    ret = setOption(SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
                    if (ret != 0)
                    {
                        ret = -1;
                        close();
                    }
                }
            }
            else
            {
                ret = net_errno;
            }

            return ret;
        }

        int CSocketBase::getType()
        {
            return 0;
        }

        int CSocketBase::createAndBind(char* _port)
        {
            struct addrinfo hints = { 0 };
            struct addrinfo *result = NULL, *rp = NULL;
            int s = -1;

            hints.ai_family = AF_UNSPEC;    /* Return IPv4 and IPv6 */
            hints.ai_socktype = getType();    /* socket type */
            hints.ai_flags = AI_PASSIVE;   /* All interfaces */

            s = getaddrinfo(NULL, _port, &hints, &result);
            if (s != 0)
            {
                LOG_LAST_ERRORMSG();
                return -1;
            }

            for (rp = result; rp != NULL; rp = rp->ai_next)
            {
                m_hSocket = createSocket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
                if (m_hSocket == INVALID_SOCKET)  continue;

                s = ::bind(m_hSocket, rp->ai_addr, rp->ai_addrlen);
                if (s == 0)
                {
                    break;
                }

                close();
            }

            if (rp == NULL)
            {
                fprintf(stderr, "Could not bind\n");
                return -1;
            }

            freeaddrinfo(result);
            return 0;
        }

        int CSocketBase::show()
        {
            if (m_hSocket == INVALID_SOCKET)
            {
                cout << "Invalid socket." << endl;
                return 0;
            }
            else
            {
                int recvBufSize = 0, sendBufSize = 0, type = 0;
                int len = sizeof(int);
                getOption(SOL_SOCKET, SO_RCVBUF, &recvBufSize, &len);
                getOption(SOL_SOCKET, SO_SNDBUF, &sendBufSize, &len);
                getOption(SOL_SOCKET, SO_TYPE, &type, &len);

                CInetAddr addr;
                getLocalAddr(addr);

                cout << "Socket options:" << endl;
                cout << "\tHostName = " << addr.hostname() << endl;
                cout << "\tRecvBufSize = " << recvBufSize << ", SndBufSize = " << sendBufSize << endl;
                cout << "\t" << (getBlockMode() ? "Block" : "NonBlock") << endl;
            }

            return 0;
        }

        int CSocketBase::close()
        {
            int ret = 0;
            if (m_hSocket != INVALID_SOCKET)
            {
                shutdown(SD_SEND);  // send FIN
                ret = ::closesocket(m_hSocket);
                m_hSocket = INVALID_SOCKET;
            }

            return ret;
        }

        int CSocketBase::shutdown(int _opt)
        {
            assert(_opt >= 0 && _opt <= 2);
            if (_opt < 0 || _opt > 2)    return -1;

            return ::shutdown(m_hSocket, _opt);
        }

        int CSocketBase::setOption(int _level, int _option, const void *_optVal, int _optLen)
        {
            if (!_optVal)    return -1;

            int ret = ::setsockopt(m_hSocket, _level, _option,
                static_cast<const char*>(_optVal),
                static_cast<socklen_t>(_optLen));
            if (ret == SOCKET_ERROR)
            {
                ret = net_errno;
            }

            return ret;
        }

        int CSocketBase::getOption(int _level, int _option, void *_optVal, int *_optLen)
        {
            if (!_optVal)    return -1;

            int ret = ::getsockopt(m_hSocket, _level, _option,
                static_cast<char*>(_optVal),
                reinterpret_cast<socklen_t*>(_optLen));
            if (ret == SOCKET_ERROR)
            {
                ret = net_errno;
            }

            return ret;
        }


        bool CSocketBase::getBlockMode()
        {
            int block = true;

#ifdef WIN32
            CInetAddr fakeAddress("127.1.2.3:65534", "tcp");
            int len = fakeAddress.size();

            int ret = ::connect(m_hSocket, (sockaddr*)&fakeAddress, len);
            return Util::GetLastSysError() != WSAEWOULDBLOCK;

#else
            int flags = fcntl(m_hSocket, F_GETFL, 0);
            block = flags &= O_NONBLOCK;
#endif

            return block;
        }

        int CSocketBase::setBlockMode(bool block)
        {
#ifdef WIN32
            int val = block ? 0 : 1;
            return control(FIONBIO, &val);

#else
            int val = ::fcntl(m_hSocket, F_GETFL, 0);
            if (val == -1)
                return false;

            if (!block)
                val |= O_NONBLOCK;
            else
                val &= ~O_NONBLOCK;

            if (::fcntl(m_hSocket, F_SETFL, val) == -1)
                return false;
#endif

            return true;
        }

        int CSocketBase::control(int _cmd, void *_arg)
        {
            int ret = ::ioctlsocket(m_hSocket, _cmd, static_cast<unsigned long *>(_arg));
            if (ret == SOCKET_ERROR)
            {
                ret = net_errno;
            }

            return ret;
        }

        int CSocketBase::bind(CInetAddr& _addr)
        {
            int len = _addr.size();
            assert(len);

            int ret = ::bind(m_hSocket, (const sockaddr*)&_addr, len);
            if (ret == SOCKET_ERROR)
                ret = net_errno;
            else
                getLocalAddr(_addr);

            return ret;
        }

        int CSocketBase::connect(const CInetAddr& _addr, int _timeout)
        {
            int len = _addr.size();
            assert(len);

            if (_timeout == 0)     // wait for ever
                setBlockMode(true);
            else
                setBlockMode(false);    // return immediately

            int ret = ::connect(m_hSocket, (const sockaddr*)&_addr, len);
            if (ret == SOCKET_ERROR)
            {
                ret = net_errno;
            }

            if (ret != WSAEWOULDBLOCK)    return ret;

            fd_set  readSet, writeSet, exceptSet;
            FD_ZERO(&readSet);
            FD_ZERO(&writeSet);
            FD_ZERO(&exceptSet);
            FD_SET(m_hSocket, &readSet);
            FD_SET(m_hSocket, &writeSet);
            FD_SET(m_hSocket, &exceptSet);

            struct timeval  timeoutVal;
            timeoutVal.tv_sec = _timeout / 1000;  // 2.5 s
            timeoutVal.tv_usec = (_timeout % 1000) * 1000;

            ret = select(m_hSocket + 1, &readSet, &writeSet, &exceptSet, &timeoutVal);
            ON_ERROR_RETURN_ERRCODE(ret, == , -1);

            ret = isConnect(&readSet, &writeSet, &exceptSet);
            return  ret;
        }

        int CSocketBase::isConnect(fd_set* _rd, fd_set* _wr, fd_set* _ex)
        {
            int err = 0;
            int len = sizeof(err);

            getOption(SOL_SOCKET, SO_ERROR, &err, &len);

            if (!FD_ISSET(m_hSocket, _rd) && !FD_ISSET(m_hSocket, _wr)) // may be timeout
            {
                Util::SetLastSysError(err);
                return err;
            }

#ifdef WIN32
            if (FD_ISSET(m_hSocket, _ex))
                return err;
#else        
            if (getOption(SOL_SOCKET, SO_ERROR, &err, &len) < 0)
                return err;
#endif

            return 0;
        }

        int CSocketBase::getRemoteAddr(CInetAddr& _addr)
        {
            int len = sizeof _addr;
            int ret = ::getpeername(m_hSocket, (sockaddr *)&_addr,
                reinterpret_cast<socklen_t*>(&len));
            if (ret == SOCKET_ERROR)
            {
                ret = net_errno;
            }

            return ret;
        }

        int CSocketBase::getLocalAddr(CInetAddr& _addr)
        {
            int len = sizeof _addr;
            int ret = ::getsockname(m_hSocket, (sockaddr *)&_addr,
                reinterpret_cast<socklen_t*>(&len));
            if (ret == SOCKET_ERROR)
            {
                ret = net_errno;
            }

            return ret;
        }

        int CSocketBase::recv(char *_buffer, u_int16 _len, int _flag)
        {
            int count = _len;
            int ret = ::recv(m_hSocket, _buffer, count, _flag);
            while (count > 0)
            {
                //
                // if interrupted by signal do continue, else if other error return this errno
                // if receives 0 bytes which means EOF return received count of bytes
                //
                if (ret == SOCKET_ERROR)
                {
                    ret = net_errno;
                    if (ret == EINTR)   continue;
                    else                break;
                }
                else if (ret == 0)  // EOF ?
                {
                    ret = _len - count;
                    break;
                }

                _buffer += ret;
                count -= ret;
            }

            return ret;
        }

        int CSocketBase::send(const char *_buffer, u_int16 _len, int _flag)
        {
            int count = _len;
            int ret = -1;
            while (count > 0)
            {
                int ret = ::send(m_hSocket, _buffer, count, _flag);
                if (ret == SOCKET_ERROR)
                {
                    ret = net_errno;
                    break;
                }

                _buffer += ret;
                count -= ret;
            }

            return ret;
        }

        int CSocketBase::recvV(iovec _iov[], u_int16 _count)
        {
            int ret = -1;

#ifdef WIN32
            u_int16 bytesReceived = 0;
            DWORD dwFlags = 0;

            ret = ::WSARecv(m_hSocket,
                (WSABUF *)_iov,
                _count,
                (LPDWORD)&bytesReceived,
                &dwFlags,
                0,
                0);
            if (ret == SOCKET_ERROR)
                ret = net_errno;
            else
                ret = bytesReceived;

#else 
            ret = ::readv(m_hSocket, _iov, _count);
#endif // CM_WIN32

            return ret;
        }

        int CSocketBase::sendV(const iovec _iov[], u_int16 _count)
        {
            int ret = -1;

#ifdef WIN32
            u_int16 bytesSend = 0;
            ret = ::WSASend(m_hSocket,
                (WSABUF *)_iov,
                _count,
                (LPDWORD)&bytesSend,
                0,
                0,
                0);     // OVERLAPPED Completion Routine

            if (ret == SOCKET_ERROR)
                ret = net_errno;
            else
                ret = bytesSend;
#else
            ret = ::writev(m_hSocket, _iov, _count);
#endif

            return ret;
        }
    }
}

