/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ip4_frag.h
 * Purpose: IPv4 Fragmentation Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_IP4_FRAG_H
#define __NET_IP4_FRAG_H

#include "net_lib.h"

/* IPv4 Definitions */
#define IP4_QUE(frm)        ((NET_IP_FRAG_LIST *)(uint32_t)&(frm)->data[0])

/* Variables */
extern NET_IP_FRAG_CFG  net_ip4_frag_config;
#define fragc         (&net_ip4_frag_config)

/* Functions */
extern void net_ip4_frag_init (void);
extern void net_ip4_frag_uninit (void);
extern NET_FRAME *net_ip4_frag_add (NET_FRAME *frame);
extern NET_FRAME *net_ip4_frag_get (NET_FRAME *frame, uint16_t mtu);

#endif /* __NET_IP4_FRAG_H */
