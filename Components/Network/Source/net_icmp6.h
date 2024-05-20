/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_icmp6.h
 * Purpose: Internet Control Message Protocol Version 6 Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_ICMP6_H
#define __NET_ICMP6_H

#include "net_lib.h"

/* ICMP6 message types */
#define ICMP6_ECHO_REQ      128         // ICMPv6 Echo Request
#define ICMP6_ECHO_REPLY    129         // ICMPv6 Echo Reply
#define ICMP6_MCAST_QUERY   130         // Multicast Listener Query
#define ICMP6_MCAST_REPORT  131         // Multicast Listener Report
#define ICMP6_MCAST_DONE    132         // Multicast Listener Done
#define ICMP6_ROUTER_SOL    133         // Router Solicitation
#define ICMP6_ROUTER_ADVER  134         // Router Advertisement
#define ICMP6_NEIGHB_SOL    135         // Neighbor Solicitation
#define ICMP6_NEIGHB_ADVER  136         // Neighbor Advertisement
#define ICMP6_INV_NBR_SOL   141         // Inverse Neighbor Solicitation
#define ICMP6_INV_NBR_ADVER 142         // Inverse Neighbor Advertisement
#define ICMP6_NBR_SOL_DAD   255         // Parameter for ndp_send_request()
                                        // (RFC4443: specified as reserved)

#define ICMP6_FRAME(frm)    ((NET_ICMP_HEADER *)(uint32_t)&(frm)->data[IP6_DATA_OFFS])
#define ECHO6_FRAME(frm)    ((NET_ECHO_HEADER *)(uint32_t)(ICMP6_FRAME(frm))->Data)
#define ECHO6_HDR(icmp_hdr) ((NET_ECHO_HEADER *)(uint32_t)(icmp_hdr)->Data)

/* ICMP6 control info */
typedef struct net_icmp6_ctrl {
  bool  NoEcho;                         // Disable ping Echo response
} NET_ICMP6_CTRL;

/* Variables */
extern NET_ICMP6_CTRL net_eth0_icmp6_control;
extern NET_ICMP6_CTRL net_eth1_icmp6_control;
extern NET_ICMP6_CTRL net_wifi0_icmp6_control;
extern NET_ICMP6_CTRL net_wifi1_icmp6_control;

/* Functions */
extern void net_icmp6_init (void);
extern bool net_icmp6_process (NET_IF_CFG *net_if, NET_FRAME *frame);
extern bool net_icmp6_send (NET_IF_CFG *net_if, NET_FRAME *frame,
              const uint8_t *src_addr, const uint8_t *dst_addr, uint8_t type);
extern bool net_icmp6_send_echo (NET_IF_CFG *net_if);

#endif /* __NET_ICMP6_H */
