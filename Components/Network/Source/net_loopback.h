/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_loopback.h
 * Purpose: Loopback Interface Definitions
 *----------------------------------------------------------------------------*/

#ifndef NET_LOOPBACK_H_
#define NET_LOOPBACK_H_

#include "net_lib.h"

/* Loopback Control info */
typedef struct net_loop_ctrl {
  NET_FRAME *rx_list;                   // Loopback queue list
} NET_LOOP_CTRL;

/* Loopback Interface Configuration */
typedef struct net_loop_cfg {
  struct net_loop_ctrl    *Ctrl;        // Instance control block
  const struct net_if_cfg *If;          // Common interface configuration
  uint8_t      IfNum;                   // Interface number (0)
} const NET_LOOP_CFG;

/* Loopback Header format */
typedef struct net_loopb {
  NET_FRAME *next;                      // Next frame in chain
  uint8_t  IpVer;                       // IP Protocol version IP4/IP6
  uint8_t  Unused[9];                   // Unused array
  uint8_t  Data[];                      // Frame data 46-1500 bytes
} NET_LOOPB;

#define LOOPB(frame)        ((NET_LOOPB *)(uint32_t)&(frame)->data[0])

/* Variables and constants */
extern NET_IF_CFG net_loop_if_config;

/* Functions */
extern void net_loop_iface_init (void);
extern void net_loop_iface_uninit (void);
extern bool net_loop_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver);
extern void net_loop_iface_run (void);

#endif /* NET_LOOPBACK_H_ */
