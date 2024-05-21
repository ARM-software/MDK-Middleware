/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ping.h
 * Purpose: Ping Client Definitions
 *----------------------------------------------------------------------------*/

#ifndef NET_PING_H_
#define NET_PING_H_

#include "net_lib.h"

/**
  Ping debug messages are routed to ICMP debug channel.
  
  Function parameter 'addr->addr_type' defines to which ICMP debug module
  the ping debug messages are redirected:

    - IPv4 target: prints debug messages to ICMP debug,
    - IPv6 target: prints debug messages to ICMP6 debug.
*/

#ifdef Network_IPv6
  #define __IS_ADDR_IP4(addr)   ((addr)->addr_type == NET_ADDR_IP4)
#else
  #define __IS_ADDR_IP4(addr)   (true)
#endif

/* Ping State Flags */
#define PING_FLAG_BUSY      0x01        // Busy waiting for ping reply

/* Ping Control info */
typedef struct net_ping_ctrl {
  uint8_t  Flags;                       // Session state flags
  uint8_t  Retries;                     // Number of retransmit retries
  uint8_t  Timer;                       // Retransmit timeout counter
  netStatus xRetv;                      // PingX return value
  uint16_t Id;                          // Transmitted Echo Identifier
  uint16_t Seq;                         // Transmitted Echo Sequence
  __ADDR   Host;                        // Ping host IP4/IP6 address
  NETOS_ID xThread;                     // PingX thread identifier
  NET_IF_CFG    *net_if;                // Network interface to use
  const uint8_t *LocAddr;               // Local IPv6 address
  netPing_cb_t  cb_func;                // Ping user callback function
} NET_PING_CTRL;

/* Variables and Constants */
extern const char net_ping_payload[];
extern NET_PING_CTRL  net_ping_control;
#define ping        (&net_ping_control)

/* Functions */
extern void net_ping_client_init (void);
extern void net_ping_client_uninit (void);
extern void net_ping_client_run (void);
extern netStatus net_ping_client_start (const __ADDR *addr, netPing_cb_t cb_func);

#endif /* NET_PING_H_ */
