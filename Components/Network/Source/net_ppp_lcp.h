/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ppp_lcp.h
 * Purpose: PPP Link Control Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_PPP_LCP_H
#define __NET_PPP_LCP_H

#include "rl_net_lib.h"

#define LCP_HEADER_LEN      4           // LCP Header length

/* LCP Codes */
#define LCP_CONF_RQ         1           // LCP Configure-Request
#define LCP_CONF_ACK        2           // LCP Configure-Ack
#define LCP_CONF_NAK        3           // LCP Configure-Nak
#define LCP_CONF_REJ        4           // LCP Configure-Reject
#define LCP_TERM_RQ         5           // LCP Terminate-Request
#define LCP_TERM_ACK        6           // LCP Terminate-Ack
#define LCP_CODE_REJ        7           // LCP Code-Reject
#define LCP_PROT_REJ        8           // LCP Protocol-Reject
#define LCP_ECHO_RQ         9           // LCP Echo-Request
#define LCP_ECHO_REP        10          // LCP Echo-Reply
#define LCP_DISC_RQ         11          // LCP Discard-Request

/* LCP Options */
#define LCP_OPT_MRU         1           // LCP Opt. Maximum Receive Unit
#define LCP_OPT_ACCMAP      2           // LCP Opt. Async Control Character Map
#define LCP_OPT_AUTHP       3           // LCP Opt. Authentication Protocol
#define LCP_OPT_QUALP       4           // LCP Opt. Quality Protocol
#define LCP_OPT_MAGIC       5           // LCP Opt. Magic Number
#define LCP_OPT_PFC         7           // LCP Opt. Protocol Field Compression
#define LCP_OPT_ACFC        8           // LCP Opt. Addr and Ctrl Field Compression

/* LCP Flags */
#define LCP_FLAG_ACCMAP     0x01        // Async Control Character map option
#define LCP_FLAG_PFC        0x02        // Protocol Field Compression option
#define LCP_FLAG_ACFC       0x04        // Address and Control Field Compression
#define LCP_FLAG_MAGIC      0x08        // Magic Number option
#define LCP_FLAG_AUTHP      0x10        // Authentication Protocol option
#define LCP_FLAG_TXACK      0x20        // Config acknowledge sent
#define LCP_FLAG_ECHO       0x40        // Sending Echo Request
#define LCP_FLAG_SHUTDN     0x80        // Shutdown LCP
#define LCP_MASK_OPT        0x1F        // All negotiation options flag mask

/* LCP Control info */
typedef struct net_lcp_ctrl {
  uint8_t  Flags;                       // State machine Flags
  uint8_t  Retries;                     // Number of retries
  uint8_t  ReqCount;                    // Request counter
  uint8_t  AuthType;                    // Current negotiation Auth. type
  uint8_t  RespId;                      // Response Identification
  uint16_t Delay;                       // Delay counter
  uint32_t MyMagic;                     // Local Magic number
  uint32_t PeerMagic;                   // Remote peer Magic number
  uint32_t RecvChMap;                   // Receiving Async Control Character Map
  uint32_t SendChMap;                   // Sending Async Control Character Map
} NET_LCP_CTRL;

/* LCP Header format */
typedef struct net_lcp_header {
  uint8_t  Code;                        // LCP Code field
  uint8_t  Id;                          // Code identification
  uint16_t Len;                         // Code length
} NET_LCP_HEADER;

#define LCP_FRAME(frame)  ((NET_LCP_HEADER *)(uint32_t)&(frame)->data[PPP_FRM_OFFS+PPP_HEADER_LEN])

/* Variables */
extern NET_LCP_CTRL net_ppp0_lcp_control;

/* Functions */
extern void net_lcp_init (NET_PPP_CFG *ppp_if);
extern void net_lcp_uninit (NET_PPP_CFG *ppp_if);
extern void net_lcp_echo (NET_PPP_CFG *ppp_if);
extern void net_lcp_terminate (NET_PPP_CFG *ppp_if);
extern void net_lcp_process (NET_PPP_CFG *ppp_if, NET_FRAME *frame);
extern void net_lcp_run (NET_PPP_CFG *ppp_if);

#endif /* __NET_PPP_LCP_H */
