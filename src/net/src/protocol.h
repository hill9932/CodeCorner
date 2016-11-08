/**
 * File:        protocol.h
 *
 * Copyright (C) 2014 - 2015  PolyVirtual, Inc.
 * All rights reserved.
 *
 * Abstract: 
 *
 */
/**
 * @Function
 *  This file defines known protocol's header structure
 * @Memo:
 *  Created by hill, 4/13/2014
 **/
#ifndef __VP_PROTOCOL_INCLUDE_H__
#define __VP_PROTOCOL_INCLUDE_H__

#include "common.h"
#include "protocol_port.h"


struct DecodeFrame
{
    enum FrameValues_e
    {
        JUMBO_FRAME_MAX_SIZE    = 0x2600  // allow max jumbo frame 9.5 KB 
    };
};


struct DecodeLay2
{
    //
    // Host Order of ETH type value
    //
    enum Lay2Types_e
    {
        NS_ETH_IP4          = 0x8000,
        NS_ETH_IP6          = 0x86DD,
        NS_ETH_VLAN         = 0x8100,
        NS_ETH_MPLS         = 0x8847,   
        NS_ETH_PPPoE        = 0x8864, 

        HO_ETH_IP4          = 0x0008,           // htons(ETHERTYPE_IPV4);
        HO_ETH_IP6          = 0xDD86,           // htons(ETHERTYPE_IPV6);
        HO_ETH_VLAN         = 0x0081,           // htons(0x8100);
        HO_ETH_MPLS         = 0x4788,           // htons(0x8847);
        HO_ETH_PPPoE        = 0x6488,           // htons(0x8864);
    };

    enum EtherTypeValues_e
    {
        ETHER_ADDR_LEN = 6,                     // Length of Ethernet address.
        ETHER_TYPE_LEN = 2,                     // Length of Ethernet type field.
        ETHER_CRC_LEN  = 4,                     // Length of Ethernet CRC. 
        ETHER_HDR_LEN  = (ETHER_ADDR_LEN * 2 + ETHER_TYPE_LEN),         // Length of Ethernet header.
        ETHER_MIN_LEN  = 64,                    // Minimum frame len, including CRC. 
        ETHER_MAX_LEN  = 1518,                  // Maximum frame len, including CRC. 
        ETHER_MTU  = (ETHER_MAX_LEN - ETHER_HDR_LEN - ETHER_CRC_LEN)    // Ethernet MTU. 
    };
};


struct DecodeLay3
{
    //
    // NextHeader field of IPv6 header
    //
    enum Ipv6HeaderTypes_e
    {
        NEXTHDR_HOP         = 0,	/* Hop-by-hop option header. */
        NEXTHDR_TCP		    = 6,	/* TCP segment. */
        NEXTHDR_UDP		    = 17,	/* UDP message. */
        NEXTHDR_IPV6		= 41,	/* IPv6 in IPv6 */
        NEXTHDR_ROUTING		= 43,	/* Routing header. */
        NEXTHDR_FRAGMENT	= 44,	/* Fragmentation/reassembly header. */
        NEXTHDR_ESP		    = 50,	/* Encapsulating security payload. */
        NEXTHDR_AUTH		= 51,	/* Authentication header. */
        NEXTHDR_ICMP		= 58,	/* ICMP for IPv6. */
        NEXTHDR_NONE		= 59,	/* No next header */
        NEXTHDR_DEST		= 60,	/* Destination options header. */
        NEXTHDR_MOBILITY	= 135,	/* Mobility header. */
        NEXTHDR_MAX		    = 255
    };

    enum Ipv4TypeValues_e
    {
        IPV4_MTU_DEFAULT    = DecodeLay2::ETHER_MTU
    };
};


struct IParser
{
    /**
     * @Function: Every protocol will decode the data and remove the header
     * @Param [in, out]: the len of payload
     * @Return: payload after removing protocol header
     **/
    virtual byte* decode(byte* _payload, int& _len, bool _detail) = 0;
};


#pragma pack(push, 1)

struct ether_header_t
{
    u_int8  m_ether_dhost[6];       // destination mac address
    u_int8  m_ether_shost[6];	    // source mac address
    u_int16 m_ether_type;		    // ethernet type

    u_int16 etherType() { return ntohs(m_ether_type); }
};


struct ether_80211Q_t 
{
    u_int16 vlanID;
    u_int16 protoType;
};


struct ipv4_header_t
{
    u_int8 	    ihl : 4;
    u_int8 	    ver : 4;
    u_int8   	tos;		    // service type
    u_int16  	len;	        // total len
    u_int16  	id;		        //
    u_int16  	frag_off;	    // offset
    u_int8   	ttl;		    // live time
    u_int8   	protocol;	    //
    u_int16  	chk_sum;	    // check sum
    struct in_addr    srcaddr;	// source ip
    struct in_addr    dstaddr;	// destination ip
    u_int8      options[];

    u_int8  version()       { return ver; }
    u_int8  header_len()    { return ihl << 2; }
    bool    is_fragment()   { return (frag_off & 0xFF3F) != 0; }
};


struct net_in6_addr 
{
    union 
    {
        u_int8      addr8[16];
        u_int16     addr16[8];
        u_int32     addr32[4];
        u_int64     addr64[2];
    };

    bool operator > (const net_in6_addr& _v)
    {
        return memcmp(this, &_v, sizeof(net_in6_addr)) > 0;
    }
    bool operator < (const net_in6_addr& _v)
    {
        return memcmp(this, &_v, sizeof(net_in6_addr)) < 0;
    }
    bool operator >= (const net_in6_addr& _v)
    {
        return memcmp(this, &_v, sizeof(net_in6_addr)) >= 0;
    }
    bool operator <= (const net_in6_addr& _v)
    {
        return memcmp(this, &_v, sizeof(net_in6_addr)) <= 0;
    }
    bool operator == (const net_in6_addr& _v)
    {
        return memcmp(this, &_v, sizeof(net_in6_addr)) == 0;
    }
};


struct ipv6_header_t 
{
    union 
    {
        struct ipv6_header_ctl 
        {
            u_int32     flow;
            u_int16     len;
            u_int8      next;
            u_int8      hlim;
        } ip6_un1;

        struct ipv6_header_ctl2
        {
            u_int8      tc1 : 4;
            u_int8      ver : 4;
            u_int8      fw1 : 4;
            u_int8      tc2 : 4;
            u_int16     fw2;
        } ip6_un2;

    } ip6_ctlun;

    net_in6_addr srcaddr;
    net_in6_addr dstaddr;
};


struct ipv6_ext_header_t
{
    u_int8  nexthdr;
    u_int8  hdrlen;
};


/**
 *	fragmentation header
 */
struct ipv6_ext_frag_hdr
{
    u_int8	nexthdr;
    u_int8	reserved;
    u_int16	fragOff;
    u_int32	identification;
};


//total length : 20Bytes
struct tcp_header_t
{
    u_int16     src_port;		// source port
    u_int16     dst_port;		// destination port
    u_int32     seq_no;		//
    u_int32     ack_no;		//

    union
    {
        struct field0_t
        {
            u_int8      nonce:1;		
            u_int8      reserved_1:3;
            u_int8      th1:4;		    // tcp header length
            u_int8      fin : 1;
            u_int8      syn : 1;
            u_int8      rst : 1;
            u_int8      psh : 1;
            u_int8      ack : 1;
            u_int8      urgent : 1;
            u_int8      echo : 1;
            u_int8      cwr : 1;
        } flags;
        u_int16 m_field0;
    } field0;

    u_int16     wnd_size;		// 16 bit windows 
    u_int16     chk_sum;		// 16 bits check sum
    u_int16     urgt_p;		    // 16 urgent p
    u_int8      options[];

    u_int8 header_len() { return field0.flags.th1 << 2; }
    u_int32 seq_number(){ return ntohl(seq_no); }
    u_int32 ack_number(){ return ntohl(ack_no); }
    u_int16 window_size()  { return ntohs(wnd_size); }
};


struct udp_header_t
{
    u_int16 src_port;
    u_int16 dst_port;
    u_int16 pkt_len;
    u_int16 chk_sum;
};


typedef struct pkt_type
{
    enum type
    {
        PROTOCOL_UNKNOWN,
        PROTOCOL_RTP,
        PROTOCOL_RTCP,
        PROTOCOL_MULTICAST
    };
} PACKET_TYPE_E;


struct rtp_header_t
{
    union 
    {
        struct field0_t
        {
            u_int8  version       : 2;
            u_int8  padding       : 1;
            u_int8  extension     : 1;
            u_int8  ccount        : 4;
        };
        u_int8  m_field0;
    } field0;

    union 
    {
        struct field1_t
        {
            u_int8  marker        : 1;
            u_int8  payload_type  : 7;
        };
        u_int8  m_field1;
    } field1;

    int16   m_seq_number;       
    u_int32 m_timestamp;       
    u_int32 m_ssrc;            
    u_int32 m_csrc[];   

    u_int8 version()    const { return field0.m_field0 >> 6; }
    u_int8 padding()    const { return (field0.m_field0 & 0x20) >> 5; }
    u_int8 extension()  const { return (field0.m_field0 & 0x10) >> 4; }
    u_int8 ccount()     const { return (field0.m_field0 & 0x0F); }
    u_int8 marker()     const { return field1.m_field1 >> 7; }
    u_int8 payload_type() const { return field1.m_field1 & 0x7F; }
    int16  seq_number() const { return ntohs(m_seq_number); }
    u_int32 timestamp() const { return ntohl(m_timestamp); }
    u_int32 ssrc()      const { return ntohl(m_ssrc); }
    u_int32 csrc(int i) const
    { 
        if (i >= ccount())  return -1;
        return ntohl(m_csrc[i]);
    }
};


struct rtp_header_ext_t
{
    u_int16 m_type;
    u_int16 m_len;    // the length of data with unit of 4B, 
    char    m_data[];

    u_int16 type()  { return ntohs(m_type); }
    u_int16 len()   { return ntohs(m_len); }
    char data(int i)
    {
        if (i >= m_len * 4)   return -1;
        return m_data[i];
    }
};


struct rtcp_header_t
{
    union
    {
        struct field0_t
        {
            u_int8  version     : 2;
            u_int8  padding     : 1;
            u_int8  ccount      : 5;
        };
        u_int8  m_field0;
    } field0;

    u_int8  m_payload_type;            
    int16   m_length;     // exclude the header, and 4B alignment

    u_int8 version()    const   { return field0.m_field0 >> 6; }
    u_int8 padding()    const   { return (field0.m_field0 & 0x20) >> 5; }
    u_int8 ccount()      const  { return field0.m_field0 & 0x1F; }
    u_int8 payload_type() const { return m_payload_type; }
    int16 length()      const   { return ntohs(m_length); }
};


typedef struct rtcp_type_e
{
    enum type
    {
        SR     = 200,
        RR     = 201,
        SDES   = 202,
        BYE    = 203,
        APP    = 204
    };
} RTCP_TYPE_E;


struct raw_data_pcap_t
{
    u_int8   hostname[20];
    u_int8   *timestamp;
    u_int8   src_mac[6];
    u_int8   dst_mac[6];
    u_int16  NetL_type;

    u_int8   src_IP[4];
    u_int8   dst_IP[4];
    u_int8   TransL_type;

    u_int16  src_port;
    u_int16  dst_port;

    u_int8   *data;
};

#pragma pack(pop)

#endif
