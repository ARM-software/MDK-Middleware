/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ndp.h
 * Purpose: Neighbor Discovery for IPv6 Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_NDP_H
#define __NET_NDP_H

#include "rl_net_lib.h"

/* Message option offsets */
#define NDP_OPT_OFFS_RS     4           // Router Solicitation options
#define NDP_OPT_OFFS_RA     12          // Router Advertisement options
#define NDP_OPT_OFFS_NS     20          // Neighbor Solicitation options
#define NDP_OPT_OFFS_NA     20          // Neighbor Advertisement options

/* Neighbor Cache states */
#define NDP_STATE_FREE      0           // Unused entry, initial value
#define NDP_STATE_RESERVED  1           // Entry allocated
#define NDP_STATE_PENDING   2           // Address resolution in progress
#define NDP_STATE_RESOLVED  3           // Resolved entry
#define NDP_STATE_REFRESH   4           // Entry is being refreshed

/* Neighbor Cache flags */
#define NDP_FLAG_HOST       0x01        // Entry is a host
#define NDP_FLAG_ROUTER     0x02        // Entry is a router
#define NDP_FLAG_HOST_INUSE 0x04        // Inuse host is refreshed once, then removed
#define NDP_FLAG_GLOBAL     0x08        // Entry is routed globally (via gateway)

/* Discovery message options */
#define NDP_OPT_SOURCE_ADDR 1           // Source Link-Layer Address
#define NDP_OPT_TARGET_ADDR 2           // Target Link-Layer Address
#define NDP_OPT_PREFIX_INFO 3           // Prefix Information
#define NDP_OPT_REDIR_HDR   4           // Redirected Header
#define NDP_OPT_MTU         5           // Maximum Transmission Unit

/* NDP control flags */
#define NDP_IFLAG_PROBE_BUSY 0x01       // Probe busy waiting for response

/* SLAAC definitions */
#define SLAAC_NUM_PREF      2           // Number of cached prefixes
#define SLAAC_MIN_VALID     7200        // Min address valid lifetime in seconds

/* SLAAC states */
#define SLAAC_STATE_INIT      0         // Init SLAAC Agent, wait for link-up
#define SLAAC_STATE_START     1         // Start delay before state discover
#define SLAAC_STATE_DISCOVER  2         // Discover routers, send Router Solicitation
#define SLAAC_STATE_DAD       3         // Duplicate address detection
#define SLAAC_STATE_ACTIVE    4         // Static address configuration used

/* NDP prefix info */
typedef struct net_ndp_prefix {
  uint16_t Timer;                       // Prefix valid timer
  uint8_t  Addr[8];                     // SLAAC works with 64 bit prefix
} NET_NDP_PREFIX;

/* NDP Buffer list structure */
typedef struct net_ndp_buf_list {
  NET_FRAME *next;                      // Next frame in the queue
  uint16_t   reserved;                  // Reserved (not used)
} NET_NDP_BUF_LIST;

#define NDP_QUE(frm)        ((NET_NDP_BUF_LIST *)(uint32_t)&(frm)->data[0])

/* SLAAC control info */
typedef struct net_slaac_ctrl {
  uint8_t  State;                       // SLAAC current state
  uint8_t  Retries;                     // Number of retries left
  uint8_t  Timer;                       // Timeout timer
  uint16_t AutoTout;                    // Auto-address timeout timer
  NET_NDP_PREFIX Prefix[2];             // Prefix list cached
} NET_SLAAC_CTRL;

/* NDP control info */
typedef struct net_ndp_ctrl {
  uint8_t  Flags;                       // State flags
  uint8_t  Retries;                     // Number of retries left
  uint16_t Timer;                       // Timeout timer
  NET_NDP_INFO  *DefRouter;             // Default Router cached
  NET_NDP_INFO  *AltRouter;             // Alternate Router cached
  NET_SLAAC_CTRL Slaac;                 // SLAAC control info
  netNDP_cb_t    cb_func;               // Probe user callback function
  uint8_t  TargAddr[NET_ADDR_IP6_LEN];  // Probe target IP6 address
  NETOS_ID  xThread;                    // ProbeX thread identifier
  netStatus xRetv;                      // ProbeX return value
  uint8_t  entry;                       // Table entry currently processed
  uint8_t  sys_flags;                   // System state flags cached
} NET_NDP_CTRL;

/* Variables */
extern NET_NDP_CTRL net_eth0_ndp_control;
extern NET_NDP_CTRL net_eth1_ndp_control;
extern NET_NDP_CTRL net_wifi0_ndp_control;
extern NET_NDP_CTRL net_wifi1_ndp_control;

/* Functions */
extern void net_ndp_cache_init (void);
extern void net_ndp_cache_uninit (void);
extern void net_ndp_process (NET_IF_CFG *net_if, NET_FRAME *frame);
extern void net_ndp_cache_refresh (NET_IF_CFG *net_if,
                                   const uint8_t *ip6_addr, const uint8_t *mac_addr);
extern NET_NDP_INFO  *net_ndp_cache_find (NET_IF_CFG *net_if, const uint8_t *ip6_addr);
extern netStatus      net_ndp_cache_mac (uint32_t if_id, const uint8_t *mac_addr);
extern const uint8_t *net_ndp_find_mac (NET_IF_CFG *net_if, const uint8_t *ip6_addr);
extern bool net_ndp_enqueue (NET_IF_CFG *net_if, NET_NDP_INFO *ndp_t, NET_FRAME *frame);
extern void net_ndp_cache_run (void);

#endif /* __NET_NDP_H */
