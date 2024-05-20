/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ppp.h
 * Purpose: Point to Point Interface Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_PPP_H
#define __NET_PPP_H

#include "net_lib.h"

#define PPP_HEADER_LEN      4           // PPP Header length
#define PPP_MTU             1500        // PPP Maximum Transmission Unit
#define PPP_FCS_LEN         2           // PPP Frame Check Sequence length
#define PPP_FCS_OFFS        4           // FCS offset in PPP_HEADER structure
#define PPP_FRM_SZ          (PPP_HEADER_LEN + PPP_MTU + PPP_FCS_LEN)
#define PPP_FRM_OFFS        (PHY_HEADER_LEN - PPP_HEADER_LEN)
#define PPP_NET_MASK        0xFFFFFF00  // 255.255.255.0
#define PPP_NET_MASK_CLIENT 0xFFFFFFFF  // 255.255.255.255

/* PPP Special Characters */
#define PPP_CH_ESC          0x7D        // PPP ESC character
#define PPP_CH_FLAG         0x7E        // PPP Start/Stop framing flag

/* PPP Protocols */
#define PPP_PROT_LCP        0xC021      // PPP Protocol type LCP
#define PPP_PROT_PAP        0xC023      // PPP Protocol type PAP
#define PPP_PROT_CHAP       0xC223      // PPP Protocol type CHAP
#define PPP_PROT_IPCP       0x8021      // PPP Protocol type IPCP
#define PPP_PROT_IP         0x0021      // PPP Protocol type IP

/* PPP State Flags */
#define PPP_FLAG_CLIENT     0x0001      // PPP Client mode active
#define PPP_FLAG_LCP_CONF   0x0002      // Start LCP configuration  - client mode
#define PPP_FLAG_AUTH_REQ   0x0004      // Authentication requested - client mode
#define PPP_FLAG_IPCP_CONF  0x0008      // Start IPCP configuration - client mode
#define PPP_FLAG_ACFC       0x0010      // Address/Control Field Compress. enabled
#define PPP_FLAG_PFC        0x0020      // Protocol Field Compression enabled

/* State flags used in EVR System Viewer */
#define PPP_FLAG_LINKUP     0x0040      // Data link established (LCP is up)
#define PPP_FLAG_OPENED     0x0080      // Data link opened (authenticated)
#define PPP_FLAG_NETWORK    0x0100      // Network layer established (IPCP is up)
  
/* PPP Thread Flags */
#define PPP_TFLAG_SYNC      0x01        // Synchronized, HDLC open flag received
#define PPP_TFLAG_ESC       0x02        // Previous char received was ESC char
#define PPP_TFLAG_TX_ESC    0x04        // Transmit ESC character
#define PPP_TFLAG_ONLINE    0x08        // Modem device is online

/* PPP Authentication Type */
#define PPP_ATYPE_NONE      0           // PPP Authentication not required
#define PPP_ATYPE_PAP       1           // Basic PAP authentication
#define PPP_ATYPE_CHAP      2           // Advanced CHAP authentication

/* PPP-CP common header for PPP Control Protocols */
#define PPP_CP_FRAME        LCP_FRAME
#define PPP_CP_CODE_REJ     LCP_CODE_REJ
#define PPP_CP_PROT_REJ     LCP_PROT_REJ
#define PPP_CP_HEADER_LEN   LCP_HEADER_LEN

/* PPP Control info */
typedef struct net_ppp_ctrl {
  uint16_t Flags;                       // State flags
  uint16_t EchoTimer;                   // Echo request timer
  uint32_t CharMap;                     // Character map
  uint8_t  Id;                          // Packet Id number
  uint8_t  Auth;                        // Authentication request type
  uint32_t RxCount;                     // Number of bytes received
  uint32_t TxCount;                     // Number of bytes transmitted
  const char *User;                     // Identification username
  const char *Passw;                    // Identification password
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
  NET_FRAME *rx_list;                   // Receive frame list
  NET_FRAME *tx_list;                   // Transmit frame list
} NET_PPP_CTRL;

/* PPP Header format */
typedef struct net_ppp_header {
  NET_FRAME *next;                      // Next frame in Tx/Rx queue
  uint16_t Fcs;                         // PPP frame control checksum - appended
  uint8_t  Unused[3];                   // Unused array (a place holder)
  uint8_t  Ch;                          // Character to transmit
  uint8_t  Adr;                         // PPP Address Field - usually 0xFF
  uint8_t  Ctrl;                        // PPP Control Field - usually 0x03
  uint16_t Prot;                        // Protocol field of the PPP header
  uint8_t  Data[];                      // Frame data 46-1500 bytes
} NET_PPP_HEADER;

#define PPP_FRAME(frame)    ((NET_PPP_HEADER *)(uint32_t)&(frame)->data[0])

/* Variables */
extern NET_PPP_CTRL net_ppp0_if_control;

/* Functions */
extern void net_ppp_iface_init (void);
extern void net_ppp_iface_uninit (void);
extern bool net_ppp_send_frame (uint32_t if_num, NET_FRAME *frame, uint8_t ip_ver);
extern bool net_ppp_send_proto (NET_PPP_CFG *ppp_if, NET_FRAME *frame, uint16_t prot);
extern void net_ppp_cp_code_reject (NET_PPP_CFG *ppp_if, NET_FRAME *frame, uint16_t prot);
extern void net_ppp_proto_init (NET_PPP_CFG *ppp_if, bool shutdown);
extern void net_ppp_iface_run (void);
extern netStatus net_ppp_get_option (netIF_Option option, uint8_t *buf, uint32_t buf_len);
extern netStatus net_ppp_set_option (netIF_Option option, const uint8_t *buf, uint32_t buf_len);

#endif /* __NET_PPP_H */
