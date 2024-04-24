/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_sys.h
 * Purpose: Network System Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_SYS_H
#define __NET_SYS_H

#include "rl_net_lib.h"

#define SYS_TICK_INTERVAL   100U        // Tick interval in ms
#define SYS_TICK_TSEC       10U         // Number of ticks for 1s
#define SYS_TICK_T200MS     2U          // Number of ticks for 200ms

#define NETOS_WAIT_FOREVER  0xFFFFFFFF  // OS wait forever timeout value

/* Protocol version */
#define IP_VER4             0           // Internet Protocol v4
#define IP_VER6             1           // Internet Protocol v6

/* IP Checksum Offload capabilities */
#define SYS_OFFL_IP4_RX     0x0001      // IP v4 cksum check
#define SYS_OFFL_IP4_TX     0x0002      // IP v4 cksum generation
#define SYS_OFFL_UDP4_RX    0x0004      // UDP v4 cksum check
#define SYS_OFFL_UDP4_TX    0x0008      // UDP v4 cksum generation
#define SYS_OFFL_TCP4_RX    0x0010      // TCP v4 cksum check
#define SYS_OFFL_TCP4_TX    0x0020      // TCP v4 cksum generation
#define SYS_OFFL_ICMP4_RX   0x0040      // ICMP4 cksum check
#define SYS_OFFL_ICMP4_TX   0x0080      // ICMP4 cksum generation
#define SYS_OFFL_UDP6_RX    0x0100      // UDP v6 cksum check
#define SYS_OFFL_UDP6_TX    0x0200      // UDP v6 cksum generation
#define SYS_OFFL_TCP6_RX    0x0400      // TCP v6 cksum check
#define SYS_OFFL_TCP6_TX    0x0800      // TCP v6 cksum generation
#define SYS_OFFL_ICMP6_RX   0x1000      // ICMP6 cksum check
#define SYS_OFFL_ICMP6_TX   0x2000      // ICMP6 cksum generation

#define SYS_OFFL_RX4       (SYS_OFFL_IP4_RX  | SYS_OFFL_UDP4_RX | \
                            SYS_OFFL_TCP4_RX | SYS_OFFL_ICMP4_RX)
#define SYS_OFFL_TX4       (SYS_OFFL_IP4_TX  | SYS_OFFL_UDP4_TX | \
                            SYS_OFFL_TCP4_TX | SYS_OFFL_ICMP4_TX)
#define SYS_OFFL_RX6       (SYS_OFFL_UDP6_RX | SYS_OFFL_TCP6_RX | \
                                               SYS_OFFL_ICMP6_RX)
#define SYS_OFFL_TX6       (SYS_OFFL_UDP6_TX | SYS_OFFL_TCP6_TX | \
                                               SYS_OFFL_ICMP6_TX)
#define SYS_OFFL_RX_ALL    (SYS_OFFL_RX4 | SYS_OFFL_RX6)
#define SYS_OFFL_TX_ALL    (SYS_OFFL_TX4 | SYS_OFFL_TX6)
#define SYS_OFFL_NONE       0x0000

/* Sys flags */
#define SYS_FLAG_TICK       0x01        // System tick (100ms)
#define SYS_FLAG_SEC        0x02        // One second tick
#define SYS_FLAG_SEC2       0x04        // Phase shifted one second tick
#define SYS_FLAG_MIN64      0x08        // One minute tick (64sec)
#define SYS_FLAG_RUNNING    0x10        // Core thread currently running
#define SYS_FLAG_LINKADDR   0x20        // Link-layer addressed ethernet frame
#define SYS_FLAG_LLA_UDP    0x40        // Link-layer addressed UDP frame

/* Random number generator */
#define net_rand16()        (uint16_t)(net_rand32() >> 16)
#define net_rand_max(max)   ((net_rand16() * (max)) >> 16)

/* Mutex lock macros */
#ifdef Network_Debug_STDIO
  #define NET_SYS_LOCK      net_sys_lock()
  #define NET_SYS_UNLOCK    net_sys_unlock()
#else
  #define NET_SYS_LOCK      while(0)
  #define NET_SYS_UNLOCK    while(0)
#endif
/* Always lock */
#define START_LOCK(type)    type rval;    net_sys_lock()
#define RETURN(x)           rval = (x);   goto end_of_func
#define END_LOCK            end_of_func:  net_sys_unlock(); return(rval)
/* Lock only in STDIO debug mode */
#define START_LOCK_DBG(type)type rval_d;  NET_SYS_LOCK
#define RETURN_DBG(x)       rval_d = (x); goto end_of_func_d
#define END_LOCK_DBG        end_of_func_d:NET_SYS_UNLOCK; return(rval_d)

/* System Control info */
typedef struct net_sys_ctrl {
  uint32_t Ticks;                       // Ticks elapsed from start
  uint32_t RndState;                    // Random generator state value
  uint32_t Time;                        // Time in format SSSSSSS.H
  uint8_t  Flags;                       // System tick flags
  bool     Tick;                        // Tick event notification (must be atomic)
  bool     Busy;                        // Network core busy (needs another call)
  uint16_t RxOffload;                   // Receive checksum offload flags
  const uint8_t *RxIpAddr;              // Receive IP address (destination)
  const char    *HostName;              // Local host name
} NET_SYS_CTRL;

/* Variables */
extern NET_SYS_CTRL net_sys_control;
#define sys       (&net_sys_control)
extern NET_SYS_CFG  net_sys_config;
#define sysc      (&net_sys_config)
extern NETIF_SETOPT_FUNC netif_setopt_func;
extern NETIF_GETOPT_FUNC netif_getopt_func;

/* Functions */
extern uint32_t net_rand32 (void);
extern const char *net_path (const char *root, const char *fname);
extern NET_IF_CFG *net_if_map_lan (uint32_t if_id);
extern NET_IF_CFG *net_if_map_all (uint32_t if_id);
extern void net_sys_suspend (NETOS_ID *thread);
extern void net_sys_resume (NETOS_ID *thread);
extern void net_sys_wait (uint32_t ms);
extern void net_sys_lock   (void);
extern void net_sys_unlock (void);
extern void net_sys_wakeup (void);

#endif /* __NET_SYS_H */
