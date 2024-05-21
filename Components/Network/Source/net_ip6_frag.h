/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ip6_frag.h
 * Purpose: IPv6 Fragmentation Definitions
 *----------------------------------------------------------------------------*/

#ifndef NET_IP6_FRAG_H_
#define NET_IP6_FRAG_H_

#include "net_lib.h"

/* IPv6 Definitions */
#define IP6_FRAG_HDR_LEN    8           // IPv6 fragment ext. length in bytes
#define IP6_MF_FLAG         0x0001      // More fragments flag
#define IP6_FRAG_MASK       0xFFF8      // Fragment offset mask

/* IPv6 Fragment extension header */
typedef struct net_ip6_frag {
  uint8_t  NextHdr;                     // Next header
  uint8_t  Reserved;                    // Reserved field
  uint16_t FragOffs;                    // IPv6 Flag & Fragment offset
  uint32_t Id;                          // Fragment Identification number
} NET_IP6_FRAG;


#define IP6_QUE(frm)        ((NET_IP_FRAG_LIST *)(uint32_t)&(frm)->data[0])
#define IP6_FRAG(frm)       ((NET_IP6_FRAG *)(uint32_t)&(frm)->data[IP6_DATA_OFFS])

/* Variables */
extern NET_IP_FRAG_CFG  net_ip6_frag_config; 
#define frag6c        (&net_ip6_frag_config)

/* Functions */
extern void net_ip6_frag_init (void);
extern void net_ip6_frag_uninit (void);
extern NET_FRAME *net_ip6_frag_add (NET_FRAME *frame);
extern NET_FRAME *net_ip6_frag_get (NET_FRAME *frame, uint16_t mtu);

#endif /* NET_IP6_FRAG_H_ */
