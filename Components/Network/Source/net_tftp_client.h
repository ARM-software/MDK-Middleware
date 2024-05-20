/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_tftp_client.h
 * Purpose: Trivial File Transfer Client Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_TFTP_CLIENT_H
#define __NET_TFTP_CLIENT_H

#include "rl_net_lib.h"

#define TFTPC_SERVER_PORT   69          // TFTP Server standard port

/* TFTP Client States */
#define TFTPC_STATE_IDLE    0           // Client is idle
#define TFTPC_STATE_PUT     1           // Put a file to server
#define TFTPC_STATE_GET     2           // Get file from server
#define TFTPC_STATE_STOP    3           // Close/Stop the client

/* TFTP Client Opcodes */
#define TFTPC_OPCOD_RRQ     1           // Read Request packet
#define TFTPC_OPCOD_WRQ     2           // Write Request packet
#define TFTPC_OPCOD_DATA    3           // Data packet
#define TFTPC_OPCOD_ACK     4           // Ack packet
#define TFTPC_OPCOD_ERR     5           // Error packet
#define TFTPC_OPCOD_OACK    6           // Option Ack packet

/* TFTP Client Error codes */
#define TFTPC_ERR_NOTDEF    0           // Not Defined Error
#define TFTPC_ERR_NOTFOUND  1           // File Not Found Error
#define TFTPC_ERR_NOACCESS  2           // Access Violation Error
#define TFTPC_ERR_DISKFULL  3           // Disk Full Error
#define TFTPC_ERR_ILLOPCOD  4           // Illegal Opcode (not supported)
#define TFTPC_ERR_UNKNTID   5           // Unknown TID Error
#define TFTPC_ERR_FILEXIST  6           // File Already Exists Error
#define TFTPC_ERR_NOUSER    7           // No such user Error

/* TFTP Client Flags */
#define TFTPC_FLAG_WACK     0x01        // Waiting for an ack from Server
#define TFTPC_FLAG_LASTB    0x02        // Last block sent
#define TFTPC_FLAG_TID      0x04        // TID assigned on Server
#define TFTPC_FLAG_OPT      0x08        // Option negotiation

/* TFTP Client Session info */
typedef struct net_tftpc_info {
  uint8_t  State;                       // Session current state
  uint8_t  Socket;                      // Binded UDP Socket
  uint8_t  Flags;                       // State machine Flags
  uint8_t  Retries;                     // Retry Counter
  uint16_t RetryTimer;                  // Retry Timeout Timer
  uint16_t BufLen;                      // Length of retransmit buffer
  uint8_t  *Buf;                        // Transmit/Retransmit buffer
  __ADDR   Server;                      // Server IP address and port
  uint16_t BlockSz;                     // Transfer block size
  uint16_t BlockNr;                     // Block Number
  netTFTPc_Event cb_event;              // Return value for callback
  void     *File;                       // File Handle pointer
} NET_TFTPC_INFO;

/* Variables */
extern NET_TFTPC_CFG  net_tftpc_config;
#define tftpc       (&net_tftpc_config)

/* Functions */
extern void net_tftp_client_init (void);
extern void net_tftp_client_uninit (void);
extern void net_tftp_client_run (void);

#endif /* __NET_TFTP_CLIENT_H */
