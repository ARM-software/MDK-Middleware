/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_slip.h
 * Purpose: IP over Serial Line Interface Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_SLIP_H
#define __NET_SLIP_H

#include "rl_net_lib.h"

#define SLIP_MTU            1500        // SLIP Maximum Transmission Unit
#define SLIP_FRM_OFFS       PHY_HEADER_LEN // SLIP frame offset
#define SLIP_NET_MASK       0xFFFFFF00  // 255.255.255.0

/* SLIP Special Characters */
#define SLIP_CH_END         0xC0        // SLIP end of packet
#define SLIP_CH_ESC         0xDB        // SLIP escape characters (byte stuffing)
#define SLIP_CH_ESC_END     0xDC        // SLIP end data byte
#define SLIP_CH_ESC_ESC     0xDD        // SLIP esc data byte

/* SLIP Thread Flags */
#define SLIP_TFLAG_SYNC     0x01        // Synchronized, Start of Packet received
#define SLIP_TFLAG_ESC      0x02        // Previous char received was ESC char
#define SLIP_TFLAG_TX_ESC   0x04        // Transmit ESC character
#define SLIP_TFLAG_ONLINE   0x08        // Modem device is online

/* SLIP Control info */
typedef struct net_slip_ctrl {
  uint32_t RxCount;                     // Number of bytes received
  uint32_t TxCount;                     // Number of bytes transmitted
  struct {                              // Thread control
    uint8_t   Flags;                    // Thread flags
    bool      SysTick;                  // Core sys tick event (100ms)
    bool      OfflineSt;                // Modem offline status notification
    bool      DisconnRq;                // Modem disconnect request
    uint8_t   osDelay;                  // Thread polling delay in ms
    uint8_t   RecTimer;                 // Receive frame timer
    NET_FRAME *Frame;                   // Receive frame in progress
  } th;
  NETOS_ID   thread;                    // Thread identifier
  NETOS_ID   semaphore;                 // Semaphore lock
  NET_FRAME *rx_list;                   // Receive queue list
  NET_FRAME *tx_list;                   // Transmit queue list
} NET_SLIP_CTRL;

/* SLIP Header format */
typedef struct net_slip_header {
  NET_FRAME *next;                      // Next frame in Tx/Rx queue
  uint8_t   Unused[9];                  // Unused array (a place holder)
  uint8_t   Ch;                         // Character to transmit
  uint8_t   Data[];                     // Frame data 46-1500 bytes
} NET_SLIP_HEADER;

#define SLIP_FRAME(frame)   ((NET_SLIP_HEADER *)(uint32_t)&(frame)->data[0])

/* Variables */
extern NET_SLIP_CTRL net_slip0_if_control;

/* Functions */
extern void net_slip_iface_init (void);
extern void net_slip_iface_uninit (void);
extern bool net_slip_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver);
extern void net_slip_iface_run (void);
extern netStatus net_slip_get_option (netIF_Option option, uint8_t *buf, uint32_t buf_len);
extern netStatus net_slip_set_option (netIF_Option option, const uint8_t *buf, uint32_t buf_len);

#endif /* __NET_SLIP_H */
