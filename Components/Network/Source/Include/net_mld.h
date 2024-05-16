/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_mld.h
 * Purpose: Multicast Listener Discovery for IPv6 Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_MLD_H
#define __NET_MLD_H

#include "rl_net_lib.h"

/* MLD Definitions */
#define MLD_HEADER_LEN      24          // MLD Header Length in bytes
#define MLD_UNSOL_TOUT      10*10       // Unsolicited Report timeout (100ms ticks)

/* MLD Group States */
#define MLD_STATE_NON       0           // Non-Listener - free Entry
#define MLD_STATE_DELAYING  1           // Delaying Listener wait for Query/Report
#define MLD_STATE_IDLE      2           // Idle Listener

/* MLD Group Flags */
#define MLD_FLAG_LAST       0x01        // Last member flag

/* MLD Control info */
typedef struct net_mld_ctrl {
  uint16_t Tout;                        // Timeout counter
} NET_MLD_CTRL;

/* MLD Header format */
typedef struct net_mld_header {
  uint8_t  Type;                        // Type of the message
  uint8_t  Code;                        // Message code byte
  uint16_t Chksum;                      // Checksum of MLD header
  uint16_t MaxDelay;                    // Maximum response delay
  uint16_t Reserved;                    // Reserved
  uint8_t  McastAddr[NET_ADDR_IP6_LEN]; // Multicast listener IPv6 address
} NET_MLD_HEADER;

/* Variables */
extern NET_MLD_CTRL net_eth0_mld_control;
extern NET_MLD_CTRL net_eth1_mld_control;
extern NET_MLD_CTRL net_wifi0_mld_control;
extern NET_MLD_CTRL net_wifi1_mld_control;

/* Functions */
extern void net_mld_node_init (void);
extern void net_mld_node_uninit (void);
extern bool net_mld_listening (NET_IF_CFG *net_if, const uint8_t *ip6_addr);
extern void net_mld_process (NET_IF_CFG *net_if, NET_FRAME *frame);
extern void net_mld_node_run (void);
extern uint32_t net_mld_collect_mcast (NET_IF_CFG *net_if, uint8_t *buf);

#endif /* __NET_MLD_H */
