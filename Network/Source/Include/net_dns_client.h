/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_dns_client.h
 * Purpose: Domain Name Resolver Client Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_DNS_CLIENT_H
#define __NET_DNS_CLIENT_H

#include "rl_net_lib.h"

#define DNS_SERVER_PORT     53          // DNS Server UDP standard port
#define DNS_FLAGS_STQUERY   0x0100      // Name Querry Flags

/* DNS States */
#define DNS_STATE_IDLE      0           // Idle, waiting for user request
#define DNS_STATE_LDONE     1           // Host IP resolved locally
#define DNS_STATE_BUSY      2           // Busy, resolving host IP

/* DNS Record Types */
#define DNS_REC_TYPE_A      1           // Host address (IPv4)
#define DNS_REC_TYPE_NS     2           // Authoritative name server
#define DNS_REC_TYPE_CNAME  5           // Canonical name for an alias
#define DNS_REC_TYPE_SOA    6           // Start of a zone of authority
#define DNS_REC_TYPE_PTR    12          // Domain name pointer
#define DNS_REC_TYPE_MX     15          // Mail exchange
#define DNS_REC_TYPE_AAAA   28          // IPv6 Address

/* DNS Classes */
#define DNS_CLASS_INET      1           // Internet (IN)

/* DNS Flags */
#define DNS_FLAG_SECDNS     0x01        // Secondary DNS Server is active
#define DNS_FLAG_SECDNS6    0x02        // Secondary IPv6 DNS Server is active
#define DNS_FLAG_USE_LM6    0x04        // Use IPv6 DNS server
#define DNS_FLAG_RESP       0x08        // DNS Response received
#define DNS_FLAG_SWITCH     0x10        // DNS Server has been switched

/* DNS Control info */
typedef struct net_dns_ctrl {
  uint8_t  State;                       // DNS Session current state
  uint8_t  Socket;                      // Binded UDP Socket
  uint8_t  Flags;                       // State machine Flags
  uint8_t  Retries;                     // Number of retransmit retries
  uint8_t  HostLen;                     // Length of the Host Name
  netStatus xRetv;                      // GetHostByNameX return value
  uint16_t Timer;                       // Retransmit timeout counter
  uint16_t Tid;                         // Transaction Id
  uint16_t Qtype;                       // Query Type
  NETOS_ID xThread;                     // GetHostByNameX thread identifier
  netDNSc_cb_t cb_func;                 // User callback function
  char     *Host;                       // Hostname that is beeing resolved
  uint32_t HostId;                      // Host Id (CRC32 value of host name)
  __ADDR   Addr;                        // DNS host temporary IP address
  NET_LOCALM  *locm;                    // Network interface for DNS
#ifdef Network_IPv6
  NET_LOCALM6 *locm6;                   // Network IPv6 interface for DNS
#endif
} NET_DNS_CTRL;

/* DNS Header format */
typedef struct net_dns_header {
  uint16_t ID;                          // DNS Transaction Id
  uint16_t Flags;                       // DNS Frame Flags
  uint16_t QDcnt;                       // Question Count
  uint16_t ANcnt;                       // Answer Resource Record Count
  uint16_t NScnt;                       // Authority Resource Record Count
  uint16_t ARcnt;                       // Additional Resource Record Count
  uint8_t  Data[];                      // Message Data Field starts here
} NET_DNS_HEADER;

#define DNS_FRAME(buf)      ((NET_DNS_HEADER *)((uint32_t)buf))

/* Variables */
extern NET_DNS_CFG  net_dns_config;
#define dnsc      (&net_dns_config)

/* Functions */
extern void      net_dns_client_init (void);
extern void      net_dns_client_uninit (void);
extern void      net_dns_client_run (void);
extern netStatus net_dns_client_resolve (const char *name,
                                         int16_t addr_type, __ADDR *addr);

#endif /* __NET_DNS_CLIENT_H */
