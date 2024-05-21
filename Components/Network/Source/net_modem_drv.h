/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_modem_drv.h
 * Purpose: Modem Driver Interface Definitions
 *----------------------------------------------------------------------------*/

#ifndef NET_MODEM_DRV_H_
#define NET_MODEM_DRV_H_

#include "net_lib.h"

/* Modem Control info */
typedef struct net_modem_ctrl {
  NET_COM_CFG *com;                 // Serial device handle
  const char  *Cmd;                 // Command to retransmit
  const char  *Resp;                // Expected response
  uint16_t RespTout;                // Response timeout in ticks
  uint16_t Delay;                   // Delay counter
  uint8_t  Retries;                 // Number of retries left
  uint8_t  WrPos;                   // Buffer write position
  char     Buf[128];                // Modem response buffer
} NET_MODEM_CTRL;

/* Variables */
extern NET_MODEM_CTRL net_ppp0_modem_control;
extern NET_MODEM_CTRL net_slip0_modem_control;

/* Functions */
extern void net_modem_init (NET_COM_CFG *com);
extern void net_modem_uninit (NET_COM_CFG *com);
extern bool net_modem_dial (NET_COM_CFG *com, const char *dial_num);
extern bool net_modem_listen (NET_COM_CFG *com);
extern bool net_modem_hangup (NET_COM_CFG *com);
extern bool net_modem_online (NET_COM_CFG *com);
extern bool net_modem_process (NET_COM_CFG *com, int32_t ch);
extern void net_modem_run (NET_COM_CFG *com);

#endif /* NET_MODEM_DRV_H_ */
