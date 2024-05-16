/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_bsd.h
 * Purpose: Berkeley Socket API Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_BSD_H
#define __NET_BSD_H

#include "rl_net_lib.h"

#define NET_BUFFER_HLEN     8           // The size of NET_BUFFER header
#define BSD_HEADER_LEN      ((NET_BUFFER_HLEN - NET_HEADER_LEN) | 0x40000000)
#define BSD_SEND_TOUT       0           // Default: no timeout

/* BSD Socket Address Family */
#define AF_UNSPEC           0           // Unspecified
#define AF_INET             1           // Internet Address Family
#define AF_NETBIOS          2           // NetBios-style addresses
#define AF_INET6            3           // Internet Address Family version 6

/* BSD Socket Type */
#define SOCK_STREAM         1           // Stream Socket (Connection oriented)
#define SOCK_DGRAM          2           // Datagram Socket (Connectionless)
#define SOCK_RAW            3           // Raw protocol on the network layer

/* BSD Socket Protocol */
#define IPPROTO_TCP         1           // TCP Protocol
#define IPPROTO_UDP         2           // UDP Protocol

/* BSD Internet Addresses */
#define INADDR_ANY          0x00000000  // All IP addresses accepted
#define INADDR_NONE         0xffffffff  // No IP address accepted

/* BSD States */
#define BSD_STATE_FREE      0           // Entry is free and unused
#define BSD_STATE_CREATED   1           // Socket created, TCP/UDP socket assigned
#define BSD_STATE_BOUND     2           // Socket bound, local IP/Port assigned
#define BSD_STATE_LISTEN    3           // Stream: socket listening
#define BSD_STATE_CONNREQ   4           // Stream: connect request processing
#define BSD_STATE_CONNECTED 5           // Stream: socket connected
#define BSD_STATE_CLOSING   6           // Stream: socket closing in progress
#define BSD_STATE_CLOSED    7           // Stream: socket closed by remote peer
#define BSD_STATE_SERVER    8           // Stream: socket in TCP server mode

/* BSD Flags */
#define BSD_FLAG_NONBLOCK   0x01        // Socket is non-blocking
#define BSD_FLAG_IP6ONLY    0x02        // Restricted IPv6 communication only
#define BSD_FLAG_FILTER     0x04        // Dgram:  frame filtering active
/* Overlapping flags for Stream or Datagram socket */
#define BSD_FLAG_TIMEOUT    0x10        // Stream: connect timeout
#define BSD_FLAG_OPEN       0x10        // Dgram:  associated UDP socket opened
#define BSD_FLAG_DACK       0x20        // Stream: Delay-ack mode active
#define BSD_FLAG_DESTIP4    0x20        // Dgram:  receive IPv4 destination address
#define BSD_FLAG_KEEP       0x40        // Stream: Keep-alive mode active
#define BSD_FLAG_DESTIP6    0x40        // Dgram:  receive IPv6 destination address
#define BSD_FLAG_FLOW       0x80        // Stream: Flow-control mode active

#define BSD_FLAGS_CHILD     (BSD_FLAG_KEEP     | BSD_FLAG_DACK | \
                             BSD_FLAG_NONBLOCK | BSD_FLAG_FLOW)

/* BSD Events */
#define BSD_EVT_CONNECT     0x01        // Socket connected
#define BSD_EVT_CLOSE       0x02        // Socket closed or aborted
#define BSD_EVT_ACK         0x04        // Sending data acked
#define BSD_EVT_DATA        0x08        // Data received
#define BSD_EVT_TIMEOUT     0x10        // Receive or send socket timeout
#define BSD_EVT_KILL        0x20        // Socket was killed locally
#define BSD_EVT_SEND        0x40        // Suspend socket sending thread

/* Host resolver Ctrl block structure */
typedef struct net_bsd_host {
  bool      DnsBusy;                    // DNS resolver busy
  NET_ADDR4 Addr;                       // Resolved IPv4 address
  HOSTENT   user;                       // Host entry returned to the user
  uint32_t *h_addr[2];                  // Pointer to host IP addresses
  uint32_t  host_ip[1];                 // Resolved Host IP address net-byte order
} NET_BSD_HOST;

/* Socket address access macros */
#define  SOCKADDR4(addr)    ((SOCKADDR_IN *)((uint32_t)addr))
#define  SOCKADDR6(addr)    ((SOCKADDR_IN6 *)((uint32_t)addr))

/* Select set access macros */
#define __FD_SET(fd,set)       (set)->fd_bits[(fd-1)>>5] |=  (1U << ((fd-1)&0x1F))
#define __FD_CLR(fd,set)       (set)->fd_bits[(fd-1)>>5] &= ~(1U << ((fd-1)&0x1F))
#define __FD_ISSET(fd,set)     ((set)->fd_bits[(fd-1)>>5] &  (1U << ((fd-1)&0x1F)))

/* Variables */
extern NET_BSD_CFG  net_bsd_config;
#define bsd       (&net_bsd_config)

/* Functions */
extern void net_bsd_socket_init (void);
extern void net_bsd_socket_uninit (void);
extern void net_bsd_host_init (void);
extern void net_bsd_host_uninit (void);
extern void net_bsd_socket_run (void);
extern void net_bsd_notify (int32_t sock, uint8_t event);

#endif /* __NET_BSD_H */
