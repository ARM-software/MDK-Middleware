/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_igmp.h
 * Purpose: Internet Group Management Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_IGMP_H
#define __NET_IGMP_H

#include "rl_net_lib.h"

/* IGMP Definitions */
#define IGMP_HEADER_LEN     8           // IGMP Header Length in bytes
#define IGMP_UNSOL_TOUT     10*10       // Unsolicited Report timeout (100ms ticks)
#define IGMP_V1_TOUT        400*10      // IGMPv1 Router present tout (100ms ticks)

/* IGMP Group States */
#define IGMP_STATE_NON      0           // Non-Member - free Entry
#define IGMP_STATE_DELAYING 1           // Delaying Member wait for Query/Report
#define IGMP_STATE_IDLE     2           // Idle Member of a group

/* IGMP Group Flags */
#define IGMP_FLAG_LAST      0x01        // Last member flag

/* IGMP message types */
#define IGMP_QUERY          0x11        // Host Membership Query all versions
#define IGMP_REPORT_V1      0x12        // Host Membership Report IGMPv1
#define IGMP_REPORT_V2      0x16        // Host Membership Report IGMPv2
#define IGMP_LEAVE_V2       0x17        // Leave Group IGMPv2
#define IGMP_REPORT_V3      0x22        // Host Membership Report IGMPv3

/* IGMP Mode States */
#define IGMP_MODE_V1        0x00        // IGMP v1 mode active
#define IGMP_MODE_V2        0x01        // IGMP v2 mode active

/* IGMP Control info */
typedef struct net_igmp_ctrl {
  uint8_t  Mode;                        // IGMP current mode (IGMPv1,IGMPv2)
  uint8_t  T200ms;                      // Timer prescaler for 200ms ticks
  uint16_t Tout;                        // IGMPv1 present timeout counter
} NET_IGMP_CTRL;

/* IGMP Header format */
typedef struct net_igmp_header {
  uint8_t  VerType;                     // Version & Type of the message
  uint8_t  MaxTime;                     // Max. Response Time (Unused in IGMPv1)
  uint16_t Chksum;                      // Checksum of IGMP message
  uint8_t  GrpIpAddr[NET_ADDR_IP4_LEN]; // Group IP address
} NET_IGMP_HEADER;

/* Variables */
extern NET_IGMP_CTRL net_eth0_igmp_control;
extern NET_IGMP_CTRL net_eth1_igmp_control;
extern NET_IGMP_CTRL net_wifi0_igmp_control;
extern NET_IGMP_CTRL net_wifi1_igmp_control;

/* Functions */
extern void net_igmp_host_init (void);
extern void net_igmp_host_uninit (void);
extern bool net_igmp_is_member (NET_IF_CFG *net_if, const uint8_t *ip4_addr);
extern void net_igmp_process (NET_IF_CFG *net_if, NET_FRAME *frame);
extern void net_igmp_host_run (void);
extern uint32_t net_igmp_collect_mcast (NET_IF_CFG *net_if, uint8_t *buf);

#endif /* __NET_IGMP_H */
