/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ppp_pap.h
 * Purpose: PPP Password Authentication Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_PPP_PAP_H
#define __NET_PPP_PAP_H

#include "net_lib.h"

#define PAP_HEADER_LEN      4           // PAP Header length

/* PAP Codes */
#define PAP_AUTH_RQ         1           // PAP Authenticate request
#define PAP_AUTH_ACK        2           // PAP Authenticate acknowledge
#define PAP_AUTH_NAK        3           // PAP Authenticate Not acknowledge

/* PAP Control info */
typedef struct net_pap_ctrl {
  uint8_t  Retries;                     // Number of retries
  uint8_t  RespId;                      // Response Identification
  uint16_t Delay;                       // PAP delay counter
} NET_PAP_CTRL;

#define PAP_HEADER          LCP_HEADER
#define PAP_FRAME           LCP_FRAME

/* Functions */
extern NET_PPP_AUTH net_ppp_pap_func;

#endif /* __NET_PPP_PAP_H */
