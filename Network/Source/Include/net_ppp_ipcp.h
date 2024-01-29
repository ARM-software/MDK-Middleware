/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ppp_ipcp.h
 * Purpose: PPP Internet Protocol Control Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_PPP_IPCP_H
#define __NET_PPP_IPCP_H

#include "rl_net_lib.h"

#define IPCP_HEADER_LEN     4           // IPCP Header length

/* IPCP Commands */
#define IPCP_CONF_RQ        1           // IPCP Configure-Request
#define IPCP_CONF_ACK       2           // IPCP Configure-Ack
#define IPCP_CONF_NAK       3           // IPCP Configure-Nak
#define IPCP_CONF_REJ       4           // IPCP Configure-Reject
#define IPCP_TERM_RQ        5           // IPCP Terminate-Request
#define IPCP_TERM_ACK       6           // IPCP Terminate-Ack
#define IPCP_CODE_REJ       7           // IPCP Code-Reject

/* IPCP Options */
#define IPCP_OPT_IPADRSS    1           // IPCP Opt. IP Addresses
#define IPCP_OPT_IPCOMP     2           // IPCP Opt. IP Compression Protocol
#define IPCP_OPT_IPADDR     3           // IPCP Opt. IP Address
#define IPCP_OPT_PRIDNS     129         // IPCP Ext. Primary DNS
#define IPCP_OPT_PRINBNS    130         // IPCP Ext. Primary NBNS
#define IPCP_OPT_SECDNS     131         // IPCP Ext. Secondary DNS
#define IPCP_OPT_SECNBNS    132         // IPCP Ext. Secondary NBNS

/* IPCP Flags */
#define IPCP_FLAG_PRIDNS    0x01        // Negotiate Primary DNS
#define IPCP_FLAG_SECDNS    0x02        // Negotiate Secondary DNS

/* IPCP Control info */
typedef struct net_ipcp_ctrl {
  uint8_t  Flags;                       // State machine Flags
  uint8_t  Retries;                     // Number of retries
  uint8_t  RespId;                      // Response Identification
  uint16_t Delay;                       // IPCP delay counter
  uint8_t  MyIP[NET_ADDR_IP4_LEN];      // Local host IP address
  uint8_t  RemIP[NET_ADDR_IP4_LEN];     // Remote host IP address
  uint8_t  PriDNS[NET_ADDR_IP4_LEN];    // Primary DNS Server IP address
  uint8_t  SecDNS[NET_ADDR_IP4_LEN];    // Secondary DNS Server IP address
} NET_IPCP_CTRL;

/* IPCP Option info */
typedef struct net_ipcp_opt {
  uint8_t  Code;                        // Option Code
  uint8_t  Len;                         // Option Length
  uint8_t  Data[];                      // Option Data ('Len' size)
} NET_IPCP_OPT;

#define IPCP_HEADER         LCP_HEADER
#define IPCP_FRAME          LCP_FRAME

/* Variables */
extern NET_IPCP_CTRL net_ppp0_ipcp_control;

/* Functions */
extern void net_ipcp_init (NET_PPP_CFG *ppp_if);
extern void net_ipcp_uninit (NET_PPP_CFG *ppp_if);
extern void net_ipcp_process (NET_PPP_CFG *ppp_if, NET_FRAME *frame);
extern void net_ipcp_run (NET_PPP_CFG *ppp_if);

#endif /* __NET_PPP_IPCP_H */
