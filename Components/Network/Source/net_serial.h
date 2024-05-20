/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_serial.h
 * Purpose: Serial Driver Interface Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_SERIAL_H
#define __NET_SERIAL_H

#include "net_lib.h"

/* Serial Control info */
typedef struct net_com_ctrl {
  NET_COM_CFG *com;                     // Serial device handle
  uint16_t Tidx;                        // Transmit write index
  uint16_t Ridx;                        // Receive read index
  uint16_t Rlen;                        // Receive buffer length
  uint8_t  rbuf[256];                   // Receive input buffer
  uint8_t  tbuf[64];                    // Transmit output buffer
} NET_COM_CTRL;

/* Variables */
extern NET_COM_CTRL net_ppp0_com_control;
extern NET_COM_CTRL net_slip0_com_control;

/* Functions */
extern void net_com_init (NET_COM_CFG *com);
extern void net_com_uninit (NET_COM_CFG *com);
extern void net_com_callback (uint32_t event, NET_COM_CTRL *ctx);
extern bool net_com_send_char (NET_COM_CFG *com, int32_t ch);
extern int32_t net_com_recv_char (NET_COM_CFG *com);
extern void net_com_flush_buf (NET_COM_CFG *com);
extern bool net_com_tx_active (NET_COM_CFG *com);

#endif /* __NET_SERIAL_H */
