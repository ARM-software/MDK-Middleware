/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_arp.h
 * Purpose: Address Resolution for Ethernet Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_ARP_H
#define __NET_ARP_H

#include "net_lib.h"

/* ARP Definitions */
#define HW_TYPE_ETH         1           // Hardware type: Ethernet
#define ARP_HEADER_LEN      (2*(NET_ADDR_ETH_LEN + NET_ADDR_IP4_LEN) + 8)

/* ARP, RARP Opcode for 'Operation Field' value */
#define ARP_RQ              0x0001      // ARP Request opcode
#define ARP_REP             0x0002      // ARP Reply opcode
#define RARP_RQ             0x0003      // Reverse ARP Request opcode
#define RARP_REP            0x0004      // Reverse ARP Reply opcode
#define InARP_RQ            0x0008      // Inverse ARP Request opcode
#define InARP_RSP           0x0009      // Inverse ARP Response opcode

/* ARP Cache States */
#define ARP_STATE_FREE      0           // Unused entry (initial value)
#define ARP_STATE_RESERVED  1           // Reserved entry (allocated)
#define ARP_STATE_PENDING   2           // Entry is used but incomplete
#define ARP_STATE_RESOLVED  3           // Resolved entry
#define ARP_STATE_REFRESH   4           // Entry is being refreshed

/* ARP Cache Types */
#define ARP_TYPE_FIXED_IP   0           // Always refreshed on timeout
#define ARP_TYPE_TEMP_IP    1           // Removed from cache on timeout
#define ARP_TYPE_INUSE_IP   2           // Refreshed once, then removed
#define ARP_TYPE_STATIC_IP  3           // Cached forever, no refresh

/* ARP Control Flags */
#define ARP_FLAG_PROBE_BUSY 0x01        // Probe busy waiting for response

/* ARP Header format */
typedef struct net_arp_header {
  uint16_t HwType;                      // Hardware address type
  uint16_t ProtType;                    // Protocol address type
  uint8_t  HwLen;                       // Hardware address length
  uint8_t  ProtLen;                     // Protocol address length
  uint16_t OpCode;                      // ARP/RARP Opcode
  uint8_t  SendHwAddr[NET_ADDR_ETH_LEN];// Sender hardware address
  uint8_t  SendIpAddr[NET_ADDR_IP4_LEN];// Sender IP address
  uint8_t  TargHwAddr[NET_ADDR_ETH_LEN];// Target hardware address
  uint8_t  TargIpAddr[NET_ADDR_IP4_LEN];// Target IP address
} NET_ARP_HEADER;

#define ARP_FRAME(frame)    ((NET_ARP_HEADER *)(uint32_t)&ETH_FRAME(frame)->Data[0])

/* ARP Buffer list structure */
typedef struct net_arp_buf_list {
  NET_FRAME *next;                      // Next frame in the queue
  uint16_t   reserved;                  // Reserved (not used)
} NET_ARP_BUF_LIST;

#define ARP_QUE(frm)        ((NET_ARP_BUF_LIST *)(uint32_t)&(frm)->data[0])

/* ARP control info */
typedef struct net_arp_ctrl {
  uint8_t  Flags;                       // Instance state flags
  uint8_t  Retries;                     // Number of retransmit retries
  uint16_t Timer;                       // Retransmit timeout counter
  uint8_t  TargAddr[NET_ADDR_IP4_LEN];  // Probe target IP address
  uint8_t *ProbeIp;                     // Pointer to probe auto IP address
  netARP_cb_t cb_func;                  // Probe user callback function
  NETOS_ID  xThread;                    // ProbeX thread identifier
  netStatus xRetv;                      // ProbeX return value
  uint8_t  entry;                       // Table entry currently processed
  uint8_t  sys_flags;                   // System state flags cached
} NET_ARP_CTRL;

/* Variables */
extern NET_ARP_CTRL net_eth0_arp_control;
extern NET_ARP_CTRL net_eth1_arp_control;
extern NET_ARP_CTRL net_wifi0_arp_control;
extern NET_ARP_CTRL net_wifi1_arp_control;

/* Functions */
extern void net_arp_cache_init (void);
extern void net_arp_cache_uninit (void);
extern void net_arp_process (NET_IF_CFG *net_if, NET_FRAME *frame);
extern void net_arp_cache_add (NET_IF_CFG *net_if,
                               const uint8_t *ip4_addr, const uint8_t *mac_addr);
extern void net_arp_cache_early (NET_IF_CFG *net_if,
                               const uint8_t *ip4_addr, const uint8_t *mac_addr);
extern NET_ARP_INFO *net_arp_cache_find (NET_IF_CFG *net_if, const uint8_t *ip4_addr);
extern bool net_arp_enqueue (NET_IF_CFG *net_if, NET_ARP_INFO *arp_t, NET_FRAME *frame);
extern void net_arp_cache_run (void);
extern void net_arp_probe (NET_IF_CFG *net_if, uint8_t *ip4_addr);
extern void net_arp_notify (NET_IF_CFG *net_if);

#endif /* __NET_ARP_H */
