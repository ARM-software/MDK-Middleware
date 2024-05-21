/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ip6.h
 * Purpose: Internet Protocol Version 6 Definitions
 *----------------------------------------------------------------------------*/

#ifndef NET_IP6_H_
#define NET_IP6_H_

#include "net_lib.h"

/* IPv6 Definitions */
#define IP6_HEADER_LEN      40          // IPv6 Header Length in bytes
#define IP6_DATA_OFFS       (PHY_HEADER_LEN + IP6_HEADER_LEN)
#define IP6_MTU             (ETH_MAX_FRAME_SIZE - IP6_DATA_OFFS)

#define IP6_DEF_VERCLASS    0x60        // Default IPv6 Version and Class

/* Protocol over IPv6 */
#define IP6_PROT_HOP_BY_HOP 0           // IPv6 Hop-by-hop option
#define IP6_PROT_TCP        6           // TCP over IPv6
#define IP6_PROT_UDP        17          // UDP over IPv6
#define IP6_PROT_FRAG       44          // Fragment header for IPv6
#define IP6_PROT_ICMP       58          // ICMP over IPv6

/* Network IPv6 Address definition */
#define IP6_ADDR(name,w0,w1,w2,w3,w4,w5,w6,w7)                          \
     uint8_t name[NET_ADDR_IP6_LEN] = { (uint8_t)(w0>>8), (uint8_t)w0,  \
                                        (uint8_t)(w1>>8), (uint8_t)w1,  \
                                        (uint8_t)(w2>>8), (uint8_t)w2,  \
                                        (uint8_t)(w3>>8), (uint8_t)w3,  \
                                        (uint8_t)(w4>>8), (uint8_t)w4,  \
                                        (uint8_t)(w5>>8), (uint8_t)w5,  \
                                        (uint8_t)(w6>>8), (uint8_t)w6,  \
                                        (uint8_t)(w7>>8), (uint8_t)w7 }

/* IPv6 Control info */
typedef struct net_ip6_ctrl {
  NET_IF_CFG *DefNetIf;                 // Default interface for internet access
} NET_IP6_CTRL;

/* IPv6 Header format */
typedef struct net_ip6_header {
  uint8_t  VerClass;                    // Version & Traffic Class hi 4 bits
  uint8_t  ClassFlow;                   // Traffic Class lo 4 bits & Flow Label hi 4 bits
  uint16_t Flow;                        // Flow Label lo-bits
  uint16_t Len;                         // Payload Length
  uint8_t  NextHdr;                     // Next header
  uint8_t  HopLim;                      // Hop Limit (TTL)
  uint8_t  SrcAddr[NET_ADDR_IP6_LEN];   // Source IP address
  uint8_t  DstAddr[NET_ADDR_IP6_LEN];   // Destination IP address
} NET_IP6_HEADER;

/* IPv6 Hop-by-hop option header format */
typedef struct net_ip6_opt {
  uint8_t  NextHdr;
  uint8_t  Len;
  uint8_t  Data[];
} NET_IP6_OPT;

#define IP6_FRAME(frame)    ((NET_IP6_HEADER *)(uint32_t)&(frame)->data[PHY_HEADER_LEN])
#define IP6_PROT(frame)     ((IP6_FRAME(frame)->NextHdr != IP6_PROT_HOP_BY_HOP)  ? \
                             IP6_FRAME(frame)->NextHdr : frame->data[IP6_DATA_OFFS])

/* Variables */
extern NET_LOCALM6 *const net_localm6[];
extern uint8_t const net_addr6_all_nodes[];
extern uint8_t const net_addr6_all_routers[];
extern uint8_t const net_addr6_loopback[];

/* Functions */
extern void  net_ip6_init (void);
extern void  net_ip6_uninit (void);
extern bool  net_ip6_chk_frame (NET_IF_CFG *net_if, NET_FRAME *frame);
extern NET_FRAME *net_ip6_reass_frame (NET_FRAME *frame);
extern bool  net_ip6_send_frame (NET_IF_CFG *net_if, NET_FRAME *frame,
                                 const uint8_t *src_addr, const uint8_t *dst_addr,
                                 uint8_t prot, uint8_t t_class, uint8_t hop_limit);
extern NET_IF_CFG *net_ip6_find_route (NET_IF_CFG *net_if, const uint8_t *dst_addr);
extern const uint8_t *net_ip6_get_local (NET_IF_CFG *net_if, const uint8_t *dst_addr);
extern bool  net_ip6_tx_offl_hl (NET_IF_CFG *net_if, uint16_t flag);
extern uint32_t net_ip6_collect_mcast (NET_IF_CFG *net_if, uint8_t *buf);
extern uint16_t net_ip6_chksum (const uint8_t *src_addr, const uint8_t *dst_addr,
                                const void *payload, uint8_t next_hdr, int32_t len);
extern bool  net_ip6_set_netif (NET_IF_CFG *net_if);
extern NET_LOCALM6 *net_ip6_def_localm (void);
#endif /* NET_IP6_H_ */
