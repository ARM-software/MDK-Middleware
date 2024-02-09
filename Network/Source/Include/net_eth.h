/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_eth.h
 * Purpose: Ethernet Interface Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_ETH_H
#define __NET_ETH_H

#include "rl_net_lib.h"

/* ETH Definitions */
#define ETH_MTU             1500        // Ethernet maximum transmission unit
#define ETH_QSIZE           32          // Receive queue size (must be 2^n)

/* ETH Protocol type */
#define ETH_PROT_ARP        0x0806      // Protocol type ARP, RARP
#define ETH_PROT_IP4        0x0800      // Protocol type IPv4
#define ETH_PROT_IP6        0x86dd      // Protocol type IPv6
#define ETH_PROT_VLAN       0x8100      // Protocol type VLAN

/* ETH State flags */
#define ETH_FLAG_PHY_OK     0x01        // Phy initialized successfully
#define ETH_FLAG_POLLING    0x02        // MAC driver in polling mode

/* ETH Control info */
typedef struct net_eth_ctrl {
  uint8_t  Flags;                       // State flags
  uint8_t  n_mcast;                     // Max. number of multicast addresses
  uint16_t VlanId;                      // Vlan tag identifier
  uint16_t Mtu;                         // Maximum transmission unit
  uint8_t  q_head;                      // Rx queue head index
  uint8_t  q_tail;                      // Rx queue tail index
  uint32_t RxCount;                     // Number of bytes received
  uint32_t TxCount;                     // Number of bytes transmitted
  struct {                              // Thread control
    bool    SecTick;                    // Core tick event (1s)
    bool    ChangeSt;                   // Link state change notification
    uint8_t LinkState;                  // Current link state
    uint8_t LinkInfo;                   // Link duplex/speed info
    uint8_t osDelay;                    // Thread polling delay in ms
  } th;
  NETOS_ID   thread;                    // Interface thread
  NETOS_ID   semaphore;                 // Semaphore lock
  NET_FRAME *rx_q[ETH_QSIZE];           // Receive frame queue
} NET_ETH_CTRL;

/* ETH Header format */
typedef struct net_eth_header {
  uint8_t  DstAddr[NET_ADDR_ETH_LEN];   // Ethernet destination MAC address
  uint8_t  SrcAddr[NET_ADDR_ETH_LEN];   // Ethernet source MAC address
  uint16_t volatile Prot;               // Protocol field of ethernet header
  uint8_t  Data[];                      // Frame data 46-1500 bytes
} NET_ETH_HEADER;

/* VLAN Tag format */
typedef struct net_vlan_header {
  uint16_t Prot;                        // TPID - Tag protocol identifier
  uint16_t Ctrl;                        // TCI - Tag control information
} NET_VLAN_HEADER;

#define ETH_FRAME(frame)    ((NET_ETH_HEADER *)(uint32_t)&(frame)->data[0])
#define VLAN_TAG(frame)     ((NET_VLAN_HEADER *)(uint32_t)&(frame)->data[12])

/* Variables and Constants */
extern NET_ETH_CTRL net_eth0_if_control;
extern NET_ETH_CTRL net_eth1_if_control;

/* Functions */
extern void  net_eth_iface_init (void);
extern void  net_eth_iface_uninit (void);
extern void  net_eth_callback (uint32_t event, NET_ETH_CTRL *ctx);
extern const uint8_t *net_eth_get_addr (const __ADDR *addr);
extern bool  net_eth_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver);
extern bool  net_eth_output (uint32_t if_num, NET_FRAME *frame);
extern void  net_eth_iface_run (void);
extern netStatus net_eth_get_option (uint32_t if_num, netIF_Option option,
                                     uint8_t *buf, uint32_t buf_len);
extern netStatus net_eth_set_option (uint32_t if_num, netIF_Option option,
                                     const uint8_t *buf, uint32_t buf_len);

#endif /* __NET_ETH_H */
