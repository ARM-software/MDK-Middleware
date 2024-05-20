/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ip4.h
 * Purpose: Internet Protocol Version 4 Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_IP4_H
#define __NET_IP4_H

#include "net_lib.h"

/* IPv4 Definitions */
#define IP4_HEADER_LEN      20          // IP4 Header Length in bytes
#define IP4_OPT_LEN         40          // Max IP4 Header option field length
#define IP4_MAX_HLEN        (IP4_HEADER_LEN + IP4_OPT_LEN)
#define IP4_DATA_OFFS       (PHY_HEADER_LEN + IP4_HEADER_LEN)
#define IP4_MTU             (ETH_MAX_FRAME_SIZE - IP4_DATA_OFFS)

#define IP4_DEF_VERHLEN     0x45        // Default IP4 Version and Header Length
#define IP4_RF_FLAG         0x8000u     // Reserved fragment flag
#define IP4_DF_FLAG         0x4000u     // Don't fragment flag
#define IP4_MF_FLAG         0x2000u     // More fragments flag
#define IP4_FRAG_MASK       0x1FFF      // Fragment offset mask

/* Protocol over IPv4 */
#define IP4_PROT_ICMP       1           // ICMP over IPv4
#define IP4_PROT_IGMP       2           // IGMP over IPv4
#define IP4_PROT_UDP        17          // UDP over IPv4
#define IP4_PROT_TCP        6           // TCP over IPv4

/* IPv4 Control info */
typedef struct net_ip4_ctrl {
  NET_IF_CFG *DefNetIf;                 // Default interface for internet access
  uint16_t Id;                          // Id of next packet to be sent
} NET_IP4_CTRL;

/* IPv4 Header format */
typedef struct net_ip4_header {
  uint8_t  VerHLen;                     // Version & Header Length field
  uint8_t  Tos;                         // Type Of Service field
  uint16_t Len;                         // Total Length of IP packet
  uint16_t Id;                          // IP Identification number
  uint16_t FragOffs;                    // IP Flags & Fragment offset
  uint8_t  Ttl;                         // Time to live
  uint8_t  Prot;                        // High level Protocol over IP
  uint16_t Chksum;                      // IP Header Checksum
  uint8_t  SrcAddr[NET_ADDR_IP4_LEN];   // Source IP address
  uint8_t  DstAddr[NET_ADDR_IP4_LEN];   // Destination IP address
} NET_IP4_HEADER;

#define IP4_FRAME(frame)    ((NET_IP4_HEADER *)(uint32_t)&(frame)->data[PHY_HEADER_LEN])
#define NET_WI4(frm)        ((NET_FRAME *)(uint32_t)&(frame)->data[20-NET_HEADER_LEN])

/* Variables and constants */
extern uint8_t const net_addr4_loopback[];

/* Functions */
extern void  net_ip4_init (void);
extern void  net_ip4_uninit (void);
extern bool  net_ip4_chk_frame (NET_IF_CFG *net_if, NET_FRAME *frame);
extern NET_FRAME *net_ip4_reass_frame (NET_FRAME *frame);
extern bool  net_ip4_send_frame (NET_IF_CFG *net_if,
                                 NET_FRAME *frame, const uint8_t *dst_addr,
                                 uint8_t prot, uint8_t tos, uint8_t ttl);
extern NET_IF_CFG *net_ip4_find_route (NET_IF_CFG *net_if, const uint8_t *dst_addr);
extern const uint8_t *net_ip4_get_local (NET_IF_CFG *net_if, const uint8_t *dst_addr);
extern bool  net_ip4_tx_offl_hl (NET_IF_CFG *net_if, uint8_t flag, uint32_t len);
extern bool  net_ip4_is_onlink  (NET_IF_CFG *net_if, const uint8_t *ip4_addr);
extern uint16_t net_ip4_chksum_buf (const void *buf, int32_t len);
extern uint16_t net_ip4_chksum (const uint8_t *src_addr, const uint8_t *dst_addr,
                                const void *payload, uint8_t proto, int32_t len);
extern bool  net_ip4_set_netif (NET_IF_CFG *net_if);
extern NET_LOCALM *net_ip4_def_localm (void);

#endif /* __NET_IP4_H */
