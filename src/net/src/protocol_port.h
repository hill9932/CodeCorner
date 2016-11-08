/**
 * File:        protocol_port.h
 *
 * Copyright (C) 2014 - 2015  PolyVirtual, Inc.
 * All rights reserved.
 *
 * Abstract: 
 *
 */
#ifndef __VP_PROTOCOL_PORT_INCLUDE_H__
#define __VP_PROTOCOL_PORT_INCLUDE_H__

#include "common.h"


struct DecodeProtocolPorts
{
    enum PortValues_e
    {
        NS_GTP_U_V1_PORT = 0x0868,

        HO_GTP_U_V1_PORT = 0x6808,           // htons(GTP_U_V1_PORT);
    };
};

#endif