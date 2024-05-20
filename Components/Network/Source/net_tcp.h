/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_tcp.h
 * Purpose: Transmission Control Protocol Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_TCP_H
#define __NET_TCP_H

#include "net_lib.h"

#ifndef Network_IPv6
#define IP6_DATA_OFFS       (IP4_DATA_OFFS + 20)
#endif

#define TCP_HEADER_LEN      20          // TCP Header Length in bytes
#define TCP_OPT_LEN         40          // Max TCP Header option field length
#define TCP_MAX_HLEN        (TCP_HEADER_LEN + TCP_OPT_LEN)
#define TCP_DATA_OFFS       (IP6_DATA_OFFS + TCP_HEADER_LEN)
#define TCP_MTU             1440        // TCP default max. transmission unit
#define TCP_DEF_MSS         536         // Standard/default MSS value for TCP
#define TCP_DEF_WINSIZE     4320        // TCP default window size

#define TCP_TOS_NORMAL      0           // TCP Type of Service for IPv4
#define TCP_TCLASS_NORMAL   0           // TCP Traffic class for IPv6
#define TCP_NO_ACK          0xFFFFFFFF  // Callback return value for no ACK

/* TCP Macros */
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define SEQ_GT(a,b)         ((int32_t)((a) - (b)) >  0)
#define SEQ_GE(a,b)         ((int32_t)((a) - (b)) >= 0)
#define SEQ_LT(a,b)         ((int32_t)((a) - (b)) <  0)
#define SEQ_LE(a,b)         ((int32_t)((a) - (b)) <= 0)

/* TCP Packet Flags */
#define TCP_FLAG_FIN        0x01        // Finished, No more data from sender
#define TCP_FLAG_SYN        0x02        // Synchronize sequence numbers
#define TCP_FLAG_RST        0x04        // Reset the connection
#define TCP_FLAG_PSH        0x08        // Push Function
#define TCP_FLAG_ACK        0x10        // Acknowledgment field significant
#define TCP_FLAG_URG        0x20        // Urgent Pointer Field significant

/* TCP Internal Flags */
#define TCP_IFLAG_CLOSING   0x01        // Socket Close pending
#define TCP_IFLAG_KALIVE    0x02        // Keep alive recovery state
#define TCP_IFLAG_RESEND    0x04        // Resend the buffered data
#define TCP_IFLAG_FAST      0x08        // Fast Recovery active
#define TCP_IFLAG_CBACK     0x10        // Callback context active
#define TCP_IFLAG_DACK      0x20        // Callback ACK notification pending
#define TCP_IFLAG_KSEG      0x40        // Send Keep Alive segment
#define TCP_IFLAG_PUSH      0x80        // Push the data (set PSH flag)

/* TCP Socket Types */
#define TCP_TYPE_DELAY_ACK  0x01        // Delayed Acknowledge enabled
#define TCP_TYPE_FLOW_CTRL  0x02        // Flow Control enabled
#define TCP_TYPE_KEEP_ALIVE 0x04        // Keep Alive enabled
#define TCP_TYPE_MASK       0x07        // Bit-mask of type flags
#define TCP_TYPE_SERVER     0x08        // Server mode active flag

/* TCP Header format */
typedef struct net_tcp_header {
  uint16_t SrcPort;                     // Source port
  uint16_t DstPort;                     // Destination port
  uint32_t SeqNr;                       // Frame Sequence Number
  uint32_t AckNr;                       // Frame Acknowledgment Number
  uint8_t  HLen;                        // Header Length of frame
  uint8_t  Flags;                       // TCP Frame Flags
  uint16_t Window;                      // Size of Window
  uint16_t Chksum;                      // TCP frame checksum
  uint16_t UrgentPtr;                   // Urgent pointer
} NET_TCP_HEADER;

/* TCP Write paramer structure */
typedef struct net_tcp_wr_info {
  uint16_t sport;                       // Source port
  uint16_t dport;                       // Destination port
  uint32_t seqnr;                       // Sequence number
  uint32_t acknr;                       // Acknowledgment number
  uint16_t window;                      // Window size
  uint8_t  flags;                       // TCP Flags
  uint8_t  opt;                         // IP option: tos or tclass
  uint8_t  ver;                         // IP version: IPv4 or IPv6
} NET_TCP_WR_INFO;

#define TCP_WI(frm)         ((NET_TCP_WR_INFO *)(uint32_t)&(frm)->data[0])

/* TCP Buffer list structure */
typedef struct net_tcp_buf_list {
  NET_FRAME *next;                      // Next frame in the queue
  uint32_t ticks;                       // Current time stamp in ticks
  uint16_t dlen;                        // Data length of a frame
  uint16_t delta;                       // Retransmit delta
} NET_TCP_BUF_LIST;

#define TCP_QUE(frm)        ((NET_TCP_BUF_LIST *)(uint32_t)&(frm)->data[0])

/* Variables */
extern NET_TCP_CFG  net_tcp_config;
#define tcp       (&net_tcp_config)

/* Functions */
extern void      net_tcp_socket_init (void);
extern void      net_tcp_socket_uninit (void);
extern int32_t   net_tcp_get_socket (netTCP_cb_t cb_func);
extern netStatus net_tcp_release_socket (int32_t socket);
extern netStatus net_tcp_listen (int32_t socket, uint16_t port);
extern netStatus net_tcp_connect (int32_t socket, const __ADDR *addr, uint16_t local_port);
extern netStatus net_tcp_close (int32_t socket);
extern netStatus net_tcp_abort (int32_t socket);
extern uint32_t  net_tcp_get_mss (int32_t socket);
extern uint8_t  *net_tcp_get_buf (uint32_t size);
extern bool      net_tcp_check_send (int32_t socket);
extern netStatus net_tcp_send (int32_t socket, uint8_t *buf, uint32_t len);
extern netTCP_State net_tcp_get_state (int32_t socket);
extern netStatus net_tcp_reset_window (int32_t socket);
extern netStatus net_tcp_set_option (int32_t socket, netTCP_Option option, uint32_t val);
extern uint32_t  net_tcp_get_option (int32_t socket, netTCP_Option option);
extern uint16_t       net_tcp_get_local_port (int32_t socket);
extern const uint8_t *net_tcp_get_local_addr (int32_t socket);
extern netStatus net_tcp_get_peer (int32_t socket, __ADDR *addr, uint32_t addr_len);
extern NET_ADDR *net_tcp_get_peer_ptr (int32_t socket);
extern uint32_t  net_tcp_get_timer (int32_t socket);
extern void      net_tcp_ctrl_opt (int32_t socket, uint8_t type, uint32_t ip_opt);
extern void      net_tcp_set_cb (int32_t socket, netTCP_cb_t cb_func);
extern void      net_tcp_reset_tout (int32_t socket);
extern uint16_t  net_tcp_alloc_port (void);
extern void      net_tcp_socket_run (void);
extern void      net_tcp_process (NET_IF_CFG *net_if, NET_FRAME *frame, uint8_t ip_ver);

#endif /* __NET_TCP_H */
