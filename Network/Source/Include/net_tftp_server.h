/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_tftp_server.h
 * Purpose: Trivial File Transfer Server Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_TFTP_SERVER_H
#define __NET_TFTP_SERVER_H

#include "rl_net_lib.h"

#define TFTP_SERVER_PORT    69          // TFTP Server standard port

/* TFTP Server States */
#define TFTP_STATE_IDLE     0           // Session is idle
#define TFTP_STATE_OPEN     1           // Open, process a command
#define TFTP_STATE_READ     2           // Read a file from server
#define TFTP_STATE_WRITE    3           // Write a file to server

/* TFTP Server Opcodes */
#define TFTP_OPCOD_RRQ      1           // Read Request packet
#define TFTP_OPCOD_WRQ      2           // Write Request packet
#define TFTP_OPCOD_DATA     3           // Data packet
#define TFTP_OPCOD_ACK      4           // Ack packet
#define TFTP_OPCOD_ERR      5           // Error packet
#define TFTP_OPCOD_OACK     6           // Option Ack packet

/* TFTP Server Error codes */
#define TFTP_ERR_NOTDEF     0           // Not Defined Error
#define TFTP_ERR_NOTFOUND   1           // File Not Found Error
#define TFTP_ERR_NOACCESS   2           // Access Violation Error
#define TFTP_ERR_DISKFULL   3           // Disk Full Error
#define TFTP_ERR_ILLOPCOD   4           // Illegal Opcode (not supported)
#define TFTP_ERR_UNKNTID    5           // Unknown TID Error
#define TFTP_ERR_FILEXIST   6           // File Already Exists Error
#define TFTP_ERR_NOUSER     7           // No such user Error

/* TFTP Server Flags */
#define TFTP_FLAG_WACK      0x01        // Waiting for an ack from client
#define TFTP_FLAG_LASTB     0x02        // Last block sent
#define TFTP_FLAG_MODEOK    0x04        // Transfer mode Binary or ASCII
#define TFTP_FLAG_OPT       0x08        // Option negotiation

/* Variables */
extern NET_TFTP_CFG  net_tftp_config;
#define tftp       (&net_tftp_config)

/* Functions */
extern void net_tftp_server_init (void);
extern void net_tftp_server_uninit (void);
extern void net_tftp_server_run (void);

#endif /* __NET_TFTP_SERVER_H */
