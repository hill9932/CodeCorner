/**
 * File:        socketUdp.cpp
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
        //     CSocketUdp
        /**********************************/
        int CSocketUdp::sendTo(const CInetAddr& _target, char* _data, int _dataLen)
        {
            if (!_data) return -1;

            int ret = ::sendto(m_hSocket, _data, _dataLen, 0, (const sockaddr*)&_target, sizeof(_target));
            if (ret == SOCKET_ERROR)
                ret = net_errno;

            return ret;
        }

        int CSocketUdp::recvFrom(CInetAddr& _remote, char* _data, int _dataLen)
        {
            if (!_data) return -1;

            int len = sizeof _remote;
            int ret = ::recvfrom(m_hSocket, _data, _dataLen, 0, (sockaddr*)&_remote, (socklen_t*)&len);
            if (ret == SOCKET_ERROR)
                ret = net_errno;

            return ret;
        }


        /**********************************/
        //     CSocketGroup
        /**********************************/
        int CSocketGroup::open(CInetAddr& _local_if)
        {
            int ret = -1;
            if (0 == CSocketUdp::open(true))
            {
                const u_int8 loop = 1;
                int ret = setOption(IPPROTO_IP, IP_MULTICAST_LOOP, (const char*)&loop, sizeof loop);
                ON_ERROR_RETURN_ERRCODE(ret, < , 0);

                in_addr addr = ((sockaddr_in*)&_local_if)->sin_addr;

                ret = setOption(IPPROTO_IP, IP_MULTICAST_IF, (const char*)&addr, sizeof addr);
                ON_ERROR_RETURN_ERRCODE(ret, < , 0);
            }

            return 0;
        }

        int CSocketGroup::setTTL(int _ttl)
        {
            int ret = setOption(IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&_ttl, sizeof _ttl);
            ON_ERROR_RETURN_ERRCODE(ret, < , 0);
            return 0;
        }

        int CSocketGroup::setLoopback(int _enable)
        {
            int ret = setOption(IPPROTO_IP, IP_MULTICAST_LOOP, (const char*)&_enable, sizeof _enable);
            ON_ERROR_RETURN_ERRCODE(ret, < , 0);
            return 0;
        }

        int CSocketGroup::joinGroup(CInetAddr &_group_address)
        {
            int ret = -1;
            if (!_group_address.isGroupAddress())   return ret;

            struct ip_mreq imr = { 0 };
            imr.imr_multiaddr.s_addr = _group_address;
            imr.imr_interface.s_addr = INADDR_ANY;
            ret = setOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&imr, sizeof(struct ip_mreq));
            ON_ERROR_RETURN_ERRCODE(ret, < , 0);
            return 0;
        }

        int CSocketGroup::leaveGroup(CInetAddr& _group_address)
        {
            int ret = -1;
            if (!_group_address.isGroupAddress())   return ret;

            struct ip_mreq imr;
            imr.imr_multiaddr.s_addr = _group_address;
            imr.imr_interface.s_addr = INADDR_ANY;
            ret = setOption(IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char*)&imr, sizeof(struct ip_mreq));
            ON_ERROR_RETURN_ERRCODE(ret, < , 0);

            return 0;
        }

        int CSocketGroup::joinSSM(CInetAddr &_group_address, CInetAddr &_source_address)
        {
            int ret = -1;
            if (!_group_address.isGroupAddress())   return ret;

            struct ip_mreq_source imr = { 0 };
            imr.imr_multiaddr.s_addr = _group_address;
            imr.imr_sourceaddr.s_addr = _source_address;
            imr.imr_interface.s_addr = INADDR_ANY;

            ret = setOption(IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, (const char*)&imr, sizeof(struct ip_mreq_source));
            ON_ERROR_RETURN_ERRCODE(ret, < , 0);

            return 0;
        }

        int CSocketGroup::leaveSSM(CInetAddr& _group_address, CInetAddr& _source_address)
        {
            int ret = -1;
            if (!_group_address.isGroupAddress())   return ret;

            struct ip_mreq_source imr = { 0 };
            imr.imr_multiaddr.s_addr = _group_address;
            imr.imr_sourceaddr.s_addr = _source_address;
            imr.imr_interface.s_addr = INADDR_ANY;

            ret = setOption(IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP, (const char*)&imr, sizeof(struct ip_mreq_source));
            ON_ERROR_RETURN_ERRCODE(ret, < , 0);

            return 0;
        }

        u_int32 CSocketGroup::getGroupAddress()
        {
            // Choose a random address in the range [232.0.1.0, 232.255.255.255)
            const u_int32 first = 0xE8000100, last = 0xE8FFFFFF;
            const u_int32 range = last - first;

            srand(time(NULL));
            return ntohl(first + (rand()) % range);
        }


        /**********************************/
        //     CSocketUdpEx
        /**********************************/

        CSocketUdpEx::CSocketUdpEx(bool _auto_close) : CSocketUdp(_auto_close)
        {}

        CSocketUdpEx::~CSocketUdpEx()
        {}

#ifdef WIN32

        SOCKET CSocketUdpEx::createSocket(int _family, int _type, int _protocol)
        {
            SOCKET s = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
            return s;
        }

        int CSocketUdpEx::recvfromEx(PPER_IO_CONTEXT_t _io_context)
        {
            int from_len = sizeof(sockaddr_in);
            DWORD flags = 0;
            memset(_io_context, 0, sizeof(PER_IO_CONTEXT_t));
            _io_context->m_opt_type = SOCKET_RECV;
            _io_context->m_socket = m_hSocket;
            _io_context->m_WSABuf.buf = _io_context->m_buffer + from_len;
            _io_context->m_WSABuf.len = sizeof(_io_context->m_buffer) - from_len;

            int ret = ::WSARecvFrom(m_hSocket, &_io_context->m_WSABuf, 1, NULL, &flags,
                (struct sockaddr*)(_io_context->m_buffer), &from_len,
                (LPOVERLAPPED)_io_context, NULL);
            if (ret != 0 && Util::GetLastSysError(true) != ERROR_IO_PENDING)
            {
                ret = SOCKET_EXCEPTION;
            }

            return ret;
        }

        int CSocketUdpEx::sendtoEx(PPER_IO_CONTEXT_t _io_context, DWORD _bytes, CInetAddr& _to_addr)
        {
            memset(&_io_context->m_overlapped, 0, sizeof(OVERLAPPED));

            int flags = 0;
            int len = _to_addr.size();
            _io_context->m_opt_type = SOCKET_SEND;
            _io_context->m_socket = m_hSocket;
            _io_context->m_WSABuf.buf = _io_context->m_buffer;
            _io_context->m_WSABuf.len = _bytes;
            int ret = ::WSASendTo(m_hSocket, &_io_context->m_WSABuf, 1, NULL, flags,
                (sockaddr*)&_to_addr, len,
                (LPOVERLAPPED)_io_context, NULL);
            return ret;
        }

#else

        SOCKET CSocketUdpEx::createSocket(int _family, int _type, int _protocol)
        {
            return CSocketUdp::createSocket(_family, _type, _protocol);
        }

        int CSocketUdpEx::recvfromEx(PPER_IO_CONTEXT_t _io_context)
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

        int CSocketUdpEx::sendtoEx(PPER_IO_CONTEXT_t _io_context, DWORD _bytes, CInetAddr& _to_addr)
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