/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_icmp.h
 * Purpose: Internet Control Message Protocol Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_ICMP_H
#define __NET_ICMP_H

#include "rl_net_lib.h"

#define ICMP_HEADER_LEN     4           // ICMP Header Length in bytes

/* ICMP message types */
#define ICMP_ECHO_REPLY     0           // Echo Reply
#define ICMP_DST_UNREACH    3           // Destination Unreachable
#define ICMP_ECHO_REQ       8           // Echo Request
#define ICMP_TIME_EXC       11          // Time Exceeded
#define ICMP_TRACERT        30          // Traceroute

/* ICMP Header format */
typedef struct net_icmp_header {
  uint8_t  Type;                        // Type of the message
  uint8_t  Code;                        // Message Code byte
  uint16_t Chksum;                      // Checksum of ICMP header
  uint8_t  Data[];                      // ICMP frame start of Data field
} NET_ICMP_HEADER;

/* ICMP Echo Header format */
typedef struct net_echo_header {
  uint16_t Id;                          // Echo Identifier
  uint16_t Seq;                         // Echo Sequence
  uint8_t  Data[];                      // Optional Data
} NET_ECHO_HEADER;

#define ICMP_FRAME(frm)     ((NET_ICMP_HEADER *)(uint32_t)&(frm)->data[IP4_DATA_OFFS])
#define ECHO_FRAME(frm)     ((NET_ECHO_HEADER *)(uint32_t)&(frm)->data[IP4_DATA_OFFS+4])

/* ICMP control info */
typedef struct net_icmp_ctrl {
  bool  NoEcho;                         // Disable ping Echo response
} NET_ICMP_CTRL;

/* Variables */
extern NET_ICMP_CTRL net_eth0_icmp_control;
extern NET_ICMP_CTRL net_eth1_icmp_control;
extern NET_ICMP_CTRL net_wifi0_icmp_control;
extern NET_ICMP_CTRL net_wifi1_icmp_control;
extern NET_ICMP_CTRL net_ppp0_icmp_control;
extern NET_ICMP_CTRL net_slip0_icmp_control;

/* Functions */
extern void net_icmp_init (void);
extern bool net_icmp_process (NET_IF_CFG *net_if, NET_FRAME *frame);
extern bool net_icmp_send_echo (NET_IF_CFG *net_if);

#endif /* __NET_ICMP_H */
