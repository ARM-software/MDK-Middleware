/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_nbns_client.h
 * Purpose: NetBIOS Name Service Client Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_NBNS_CLIENT_H
#define __NET_NBNS_CLIENT_H

#include "rl_net_lib.h"

/* NBNS Client Definitions */
#define NBNS_PORT           137         // NBNS standard UDP port

/* NBNS Message Flags */
#define NBNS_FLAG_RESP      0x8000      // Response flag (1=yes)
#define NBNS_FLAG_OPCODE    0x7800      // Opcode mask
#define NBNS_FLAG_AUTHORITY 0x0400      // Authority for the domain (1=yes)
#define NBNS_FLAG_TRUNC     0x0200      // Message is truncated
#define NBNS_FLAG_RECURDES  0x0100      // Recursion desired
#define NBNS_FLAG_RECURAVA  0x0080      // Recursion available on server (0=no)
#define NBNS_FLAG_BCAST     0x0010      // Broadcast packet (1=broadcast)
#define NBNS_FLAG_RCODE     0x000F      // Reply code (0=no error)

#define NBNS_FLAGS_NQUERY   0x0110      // Name query flags
#define NBNS_FLAGS_NQRESP   0x8500      // Name query response no errors
#define NBNS_OPCODE_QUERY   0x0000      // Opcode name query
#define NBNS_RCODE_NO_ERROR 0x0000      // Rcode no error


/* NBNS RR Classes */
#define NBNS_CLASS_INET     1           // Internet (IN)

/* NBNS RR Types */
#define NBNS_TYPE_NB        32          // NetBIOS Name Service

/* NBNS Flags */
#define NBNS_FLAG_TOUT      0x01        // Timeout, no response received
#define NBNS_FLAG_VALID     0x02        // Resolved IP address is valid

/* NBNS Control info */
typedef struct net_nbns_ctrl {
  uint8_t  Socket;                      // Binded UDP socket
  uint8_t  Flags;                       // State machine Flags
  uint8_t  Retries;                     // Number of retransmit retries
  uint16_t Timer;                       // Retransmit timeout counter
  uint16_t Tid;                         // Transaction id
  uint8_t  IpAddr[NET_ADDR_IP4_LEN];    // Resolved IPv4 address
  NETOS_ID Thread;                      // Resolver thread identifier
  const char *Name;                     // Hostname beeing resolved
  NET_IF_CFG *net_if;                   // Network interface descriptor
  void (*run)    (void);                // Run NBNS name cache manager
  bool (*process)(const uint8_t*,uint32_t);// Process name query response function
} NET_NBNS_CTRL;

/* NBNS Header Format */
typedef struct net_nbns_header {
  uint16_t Id;                          // Transaction id
  uint16_t Flags;                       // Message flags
  uint16_t QDcnt;                       // Question section count
  uint16_t ANcnt;                       // Answer name count
  uint16_t NScnt;                       // Resource record count
  uint16_t ARcnt;                       // Additional record count
  uint8_t  Data[];                      // Message data starts here
} NET_NBNS_HEADER;

#define NBNS_FRAME(buf)     ((NET_NBNS_HEADER *)(uint32_t)(buf))

/* Variables */
extern NET_NBNS_CFG  net_nbns_config;
#define nbnsc      (&net_nbns_config)

/* Functions */
extern void net_nbns_client_init (void);
extern void net_nbns_client_uninit (void);
extern void net_nbns_client_run (void);

#endif /* __NET_NBNS_CLIENT_H */
