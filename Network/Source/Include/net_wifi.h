/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_wifi.h
 * Purpose: WIFI Interface Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_WIFI_H
#define __NET_WIFI_H

#include "rl_net_lib.h"

/* WIFI Definitions */
#define WIFI_MTU            1500        // WiFi maximum transmission unit
#define WIFI_QSIZE          16          // Receive queue size (must be 2^n)

#define WIFI_IF_STA         0           // WiFi interface Station
#define WIFI_IF_AP          1           // WiFi interface Access Point

/* WIFI State flags */
#define WIFI_FLAG_POLLING   0x01        // Driver in polling mode
#define WIFI_FLAG_MAC_ADDR  0x02        // Driver provides MAC address

/* WIFI Control info */
typedef struct net_wifi_ctrl {
  uint8_t  Flags;                       // State flags
  uint16_t Mtu;                         // Maximum transmission unit
  uint8_t  q_head;                      // Rx queue head index
  uint8_t  q_tail;                      // Rx queue tail index
  uint8_t  x_head;                      // Tx queue head index
  uint8_t  x_tail;                      // Tx queue tail index
  uint32_t RxCount;                     // Number of bytes received
  uint32_t TxCount;                     // Number of bytes transmitted
  struct {                              // Thread control
    bool    SecTick;                    // Core tick event (1s)
    bool    ChangeSt;                   // Link state change notification
    uint8_t LinkState;                  // Current link state (0= disconnected)
    uint8_t osDelay;                    // Thread polling delay in ms
  } th;
  NETOS_ID   thread;                    // Interface thread
  NETOS_ID   semaphore;                 // Semaphore lock
  NET_FRAME *rx_q[WIFI_QSIZE];          // Receive frame queue
  NET_FRAME *tx_q[WIFI_QSIZE];          // Transmit frame queue
} NET_WIFI_CTRL;

/* Variables and Constants */
extern NET_WIFI_CTRL net_wifi0_if_control;
extern NET_WIFI_CTRL net_wifi1_if_control;

/* Functions */
extern void  net_wifi_iface_init (void);
extern void  net_wifi_iface_uninit (void);
extern void  net_wifi_callback (uint32_t event, NET_WIFI_CTRL *ctx);
extern const uint8_t *net_wifi_get_addr (const __ADDR *addr);
extern bool  net_wifi_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver);
extern bool  net_wifi_output (uint32_t if_num, NET_FRAME *frame);
extern void  net_wifi_iface_run (void);
extern netStatus net_wifi_get_option (uint32_t if_num, netIF_Option option,
                                      uint8_t *buf, uint32_t buf_len);
extern netStatus net_wifi_set_option (uint32_t if_num, netIF_Option option,
                                      const uint8_t *buf, uint32_t buf_len);

#endif /* __NET_WIFI_H */
