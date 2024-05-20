/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_udp.h
 * Purpose: User Datagram Protocol Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_UDP_H
#define __NET_UDP_H

#include "net_lib.h"

#ifndef Network_IPv6
#define IP6_DATA_OFFS       (IP4_DATA_OFFS + 20)
#endif

#define UDP_HEADER_LEN      8           // UDP Header Length in bytes
#define UDP_DATA_OFFS       (IP6_DATA_OFFS + UDP_HEADER_LEN)
#define UDP_MTU             2000        // UDP default max. transmission unit

#define UDP_TOS_NORMAL      0           // UDP Type of Service for IPv4
#define UDP_TCLASS_NORMAL   0           // UDP Traffic class for IPv6

/* UDP Flags */
#define UDP_FLAG_CKS_MASK   0x03U       // Mask for checking checksum options
#define UDP_FLAG_KEEP       0x04        // Keep the packet (no auto-free)
#define UDP_FLAG_RECV_LLA   0x08        // Receive link-layer addressed packets

/* UDP States */
#define UDP_STATE_FREE      0           // Entry is free and unused
#define UDP_STATE_CLOSED    1           // Entry allocated, socket still closed
#define UDP_STATE_OPENED    2           // Socket opened for communication

/* UDP Header format */
typedef struct net_udp_header {
  uint16_t SrcPort;                     // Source port
  uint16_t DstPort;                     // Destination port
  uint16_t Len;                         // Length of UDP frame
  uint16_t Chksum;                      // UDP frame checksum
  uint8_t  Data[];                      // UDP frame start of Data field
} NET_UDP_HEADER;

/* UDP extended address structure */
typedef struct net_addr_x {
  int16_t  addr_type;                   // IP address type
  uint16_t port;                        // Internet socket port number
  uint8_t  addr[__ADDR_IP_LEN];         // IPv4 or IPv6 address
  uint8_t    *dst_addr;                 // Pointer to destination IPv4 or IPv6 address
  NET_IF_CFG *net_if;                   // Pointer to network interface descriptor
} NET_ADDR_X;

#define __ADDR_X(addr)      (const NET_ADDR_X *)((const void *)(addr))

/* Variables */
extern NET_UDP_CFG net_udp_config;

/* Functions */
extern void      net_udp_socket_init (void);
extern void      net_udp_socket_uninit (void);
extern int32_t   net_udp_get_socket (netUDP_cb_t cb_func);
extern netStatus net_udp_release_socket (int32_t socket);
extern netStatus net_udp_open (int32_t socket, uint16_t port);
extern netStatus net_udp_close (int32_t socket);
extern uint8_t  *net_udp_get_buf (uint32_t size);
extern netStatus net_udp_send (int32_t socket, const __ADDR *addr,
                                               uint8_t *buf, uint32_t len);
extern netStatus net_udp_send_if (NET_IF_CFG *net_if, int32_t socket,
                                  const __ADDR *addr, uint8_t *buf, uint32_t len);
extern netStatus net_udp_set_option (int32_t socket, netUDP_Option option, uint32_t val);
extern uint32_t  net_udp_get_option (int32_t socket, netUDP_Option option);
extern uint16_t  net_udp_get_local_port (int32_t socket);
extern bool      net_udp_keep_buf (int32_t socket, uint8_t *buf);
extern void      net_udp_enable_lla (int32_t socket);
extern void      net_udp_free_buf (uint8_t *buf);
extern void      net_udp_process (NET_IF_CFG *net_if, NET_FRAME *frame, uint8_t ip_ver);

#endif /* __NET_UDP_H */
