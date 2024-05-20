/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ppp_chap.h
 * Purpose: PPP Challenge-Handshake Authentication Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_PPP_CHAP_H
#define __NET_PPP_CHAP_H

#include "rl_net_lib.h"

#define CHAP_HEADER_LEN     4           // CHAP Header length

/* CHAP Codes */
#define CHAP_CHALLENGE      1           // Challenge reqest
#define CHAP_RESPONSE       2           // Response to Challenge
#define CHAP_SUCCESS        3           // Authentication Success
#define CHAP_FAILED         4           // Authentication Failure

/* CHAP Control info */
typedef struct net_chap_ctrl {
  uint8_t  Retries;                     // Number of retries
  uint8_t  RespId;                      // Response Identification
  uint16_t Delay;                       // CHAP delay counter
  uint8_t  ChVal[16];                   // Challenge value transmitted
} NET_CHAP_CTRL;

#define CHAP_HEADER         LCP_HEADER
#define CHAP_FRAME          LCP_FRAME

/* Variables */

/* Functions */
extern NET_PPP_AUTH net_ppp_chap_func;

#endif /* __NET_PPP_CHAP_H */
