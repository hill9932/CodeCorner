/**
 * File:        socketTcp.cpp
 *
 * Copyright (C) 2014 - 2015  PolyVirtual, Inc.
 * All rights reserved.
 *
 * Abstract: 
 *
 */
#include "socket_base.h"

using namespace LabSpace::Common;

namespace LabSpace
{
    namespace Net
    {
        /**********************************/
        //     CSocketTcp
        /**********************************/

        int CSocketTcp::listen(int backlog)
        {
            if (backlog < 0)    return -1;

            int ret = ::listen(m_hSocket, backlog);
            if (ret == SOCKET_ERROR)
                ret = net_errno;

            return ret;
        }

        int CSocketTcp::accept(CSocketTcp& _new_socket)
        {
            CInetAddr clientAddr;
            int len = sizeof(clientAddr);
            int ret = ::accept(m_hSocket, (struct sockaddr*)&clientAddr, (socklen_t*)&len);
            if (ret == SOCKET_ERROR)
                ret = net_errno;
            else
            {
                _new_socket.setHandle(ret);
            }

            return ret;
        }


        /**********************************/
        //     CSocketTcpEx
        /**********************************/
        LPFN_ACCEPTEX CSocketTcpEx::m_lpfnAcceptEx = NULL;         // AcceptEx
        LPFN_GETACCEPTEXSOCKADDRS CSocketTcpEx::m_lpfnGetAcceptExSockAddrs = NULL;

        CSocketTcpEx::CSocketTcpEx(bool _auto_close) : CSocketTcp(_auto_close)
        {
            memset(m_buffer, 0, sizeof(m_buffer));
        }

        CSocketTcpEx::~CSocketTcpEx()
        {}


#ifdef WIN32
        SOCKET CSocketTcpEx::createSocket(int _family, int _type, int _protocol)
        {
            SOCKET s = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

            if (!m_lpfnAcceptEx && !m_lpfnGetAcceptExSockAddrs)
            {
                DWORD dwBytes = 0;
                GUID GuidAcceptEx = WSAID_ACCEPTEX;        // GUID of AcceptEx  
                WSAIoctl(
                    s,
                    SIO_GET_EXTENSION_FUNCTION_POINTER,
                    &GuidAcceptEx,
                    sizeof(GuidAcceptEx),
                    &m_lpfnAcceptEx,
                    sizeof(m_lpfnAcceptEx),
                    &dwBytes,
                    NULL,
                    NULL);

                GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
                WSAIoctl(
                    s,
                    SIO_GET_EXTENSION_FUNCTION_POINTER,
                    &GuidGetAcceptExSockAddrs,
                    sizeof(GuidGetAcceptExSockAddrs),
                    &m_lpfnGetAcceptExSockAddrs,
                    sizeof(m_lpfnGetAcceptExSockAddrs),
                    &dwBytes,
                    NULL,
                    NULL);
            }

            return s;
        }

        int CSocketTcpEx::acceptEx(CSocketTcpEx& _new_socket, PPER_IO_CONTEXT_t _io_context)
        {
            if (!m_lpfnAcceptEx)    return -1;
            if (!_new_socket)
            {
                int ret = _new_socket.open();
                ON_ERROR_RETURN_ERRCODE(ret, != , 0);
            }

            DWORD nBytes = 0;
            memset(&_io_context->m_overlapped, 0, sizeof(OVERLAPPED));
            _io_context->m_opt_type = SOCKET_ACCEPT;
            _io_context->m_socket = _new_socket;

            // call AcceptEx()
            int ret = m_lpfnAcceptEx(
                m_hSocket, (SOCKET)_new_socket,
                m_buffer, 0,
                64, 64,
                &nBytes, (LPOVERLAPPED)_io_context);
            if (ret != 0 && Util::GetLastSysError(true) != ERROR_IO_PENDING)
            {
                ret = -1;
            }

            return ret;
        }

        int CSocketTcpEx::connectEx(CInetAddr& _server_address, PPER_IO_CONTEXT_t _io_context)
        {
            DWORD recv_bytes = 0, flags = 0;
            if (_io_context)
            {
                memset(_io_context, 0, sizeof(PER_IO_CONTEXT_t));
                _io_context->m_opt_type = SOCKET_CONN;
                _io_context->m_socket = m_hSocket;
                _io_context->m_WSABuf.buf = _io_context->m_buffer;
                _io_context->m_WSABuf.len = sizeof(_io_context->m_buffer);
            }

            int ret = ::WSAConnect(m_hSocket, (sockaddr*)_server_address, _server_address.size(),
                NULL, NULL, NULL, NULL);
            return ret;
        }

        int CSocketTcpEx::recvEx(PPER_IO_CONTEXT_t _io_context)
        {
            DWORD recv_bytes = 0, flags = 0;
            memset(_io_context, 0, sizeof(PER_IO_CONTEXT_t));

            _io_context->m_opt_type = SOCKET_RECV;
            _io_context->m_socket = m_hSocket;
            _io_context->m_WSABuf.buf = _io_context->m_buffer;
            _io_context->m_WSABuf.len = sizeof(_io_context->m_buffer);

            int ret = ::WSARecv(m_hSocket, &_io_context->m_WSABuf, 1, NULL, &flags, (LPOVERLAPPED)_io_context, NULL);
            return ret;
        }

        int CSocketTcpEx::sendEx(PPER_IO_CONTEXT_t _io_context, DWORD _bytes)
        {
            DWORD flags = 0;
            memset(&_io_context->m_overlapped, 0, sizeof(OVERLAPPED));

            _io_context->m_opt_type = SOCKET_SEND;
            _io_context->m_socket = m_hSocket;
            _io_context->m_WSABuf.buf = _io_context->m_buffer;
            _io_context->m_WSABuf.len = _bytes;

            return ::WSASend(m_hSocket, &_io_context->m_WSABuf, 1, NULL, flags, (LPOVERLAPPED)_io_context, NULL);
        }

#else

        SOCKET CSocketTcpEx::createSocket(int _family, int _type, int _protocol)
        {
            return CSocketTcp::createSocket(_family, _type, _protocol);
        }

        int CSocketTcpEx::acceptEx(CSocketTcpEx& _new_socket, PPER_IO_CONTEXT_t _io_context)
        {
            int ret = -1;
            if (!_new_socket)
            {
                ret = _new_socket.open();
                ON_ERROR_RETURN_ERRCODE(ret, !=, 0);
            }

            memset(&_io_context->m_overlapped, 0, sizeof(OVERLAPPED));
            _io_context->m_opt_type  = SOCKET_ACCEPT;
            _io_context->m_socket    = _new_socket;

            // call AcceptEx()
            return ret; 
        }

        int CSocketTcpEx::connectEx(CInetAddr& _server_address, PPER_IO_CONTEXT_t _io_context)
        {
            return 0;
        }

        int CSocketTcpEx::recvEx(PPER_IO_CONTEXT_t _io_context)
        {
            DWORD recv_bytes = 0, flags = 0;
            memset(_io_context, 0, sizeof(PER_IO_CONTEXT_t));

            _io_context->m_opt_type   = SOCKET_RECV;
            _io_context->m_socket     = m_hSocket;
            _io_context->m_WSABuf.iov_base = _io_context->m_buffer;
            _io_context->m_WSABuf.iov_len  = sizeof(_io_context->m_buffer);

            int ret = 0; //::WSARecv(m_hSocket, &_io_context->m_WSABuf, 1, NULL, &flags, (LPOVERLAPPED)_io_context, NULL);
            return ret;
        }

        int CSocketTcpEx::sendEx(PPER_IO_CONTEXT_t _io_context, DWORD _bytes)
        {
            DWORD flags = 0;
            memset(&_io_context->m_overlapped, 0, sizeof(OVERLAPPED));

            _io_context->m_opt_type  = SOCKET_SEND;
            _io_context->m_socket    = m_hSocket;
            _io_context->m_WSABuf.iov_base = _io_context->m_buffer;
            _io_context->m_WSABuf.iov_len  = _bytes;

            return 0;//::WSASend(m_hSocket, &_io_context->m_WSABuf, 1, NULL, flags, (LPOVERLAPPED)_io_context, NULL);
        }

#endif
    }
}