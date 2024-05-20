/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ftp_client.h
 * Purpose: File Transfer Client Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_FTP_CLIENT_H
#define __NET_FTP_CLIENT_H

#include "net_lib.h"

#define FTPC_SERVER_PORT    21          // FTP Server standard port
#define FTPC_MAX_PATH       120         // Max. Path size

/* FTP Client States */
#define FTPC_STATE_IDLE     0           // FTP Client is idle
#define FTPC_STATE_CONNRQ   1           // User has requested connect to Server
#define FTPC_STATE_OPEN     2           // TCP Socket Connected
#define FTPC_STATE_USER     3           // Authentication: Username sent
#define FTPC_STATE_PASSW    4           // Authentication: Password sent
#define FTPC_STATE_CWD      5           // Change working directory
#define FTPC_STATE_TYPE_I   6           // TYPE I cmd given, binary mode
#define FTPC_STATE_XPASV    7           // Enter passive mode, PASV/EPSV command
#define FTPC_STATE_XPORT    8           // Enter active mode, PORT/EPRT command
#define FTPC_STATE_FOPEN    9           // Open local file, GET, PUT or APPEND cmd
#define FTPC_STATE_CMDSEND  10          // Send a command to server
#define FTPC_STATE_STOR     11          // Write a file, STOR or APPE command
#define FTPC_STATE_STOREND  12          // Close a data connection after STOR
#define FTPC_STATE_RETR     13          // Read a file, RETR command
#define FTPC_STATE_LIST     14          // List directory, LIST command
#define FTPC_STATE_DELE     15          // Delete a file, DELE command
#define FTPC_STATE_RNFR     16          // Rename from 'old name', RNFR command
#define FTPC_STATE_RNTO     17          // Rename to 'new name', RNTO command
#define FTPC_STATE_MKD      18          // Makes a new directory, MKD command
#define FTPC_STATE_RMD      19          // Removes an empty directory, RMD command
#define FTPC_STATE_QUIT     20          // Send QUIT command
#define FTPC_STATE_STOP     21          // Stop FTP Client, close local file
#define FTPC_STATE_TWAIT    22          // Wait for sockets to be closed

/* FTP Client Flags */
#define FTPC_FLAG_RESP      0x01        // Wait for FTP Server response
#define FTPC_FLAG_FIRST     0x02        // First Response line processed
#define FTPC_FLAG_PASSIVE   0x04        // The FTP server is in passive mode
#define FTPC_FLAG_DACK      0x08        // Data Socket Acknowledge
#define FTPC_FLAG_DOPEN     0x10        // Data connection opened
#define FTPC_FLAG_DCLOSED   0x20        // Data connection closed
#define FTPC_FLAG_CDONE     0x40        // Control connection ready for QUIT
#define FTPC_FLAG_ABORT     0x80        // Data connection abort (local error)

/* FTP server reply codes */
#define FTPC_RESP_READY     0           // Server is ready
#define FTPC_RESP_QUITOK    1           // Service closing transmission Channel
#define FTPC_RESP_RQPASSW   2           // User OK, required password
#define FTPC_RESP_USEROK    3           // User login success
#define FTPC_RESP_FSTATOK   4           // File status ok, about to open data conn.
#define FTPC_RESP_CMDOK     5           // Command Successful
#define FTPC_RESP_CMDERR    6           // Command Error
#define FTPC_RESP_FCMDOK    7           // File Operation Command Successful
#define FTPC_RESP_NOACCESS  8           // No access, File not found
#define FTPC_RESP_PASVOK    9           // Entering Passive mode
#define FTPC_RESP_DSTART    10          // Data connection open, starting transfer
#define FTPC_RESP_DCLOSE    11          // Closing data connection
#define FTPC_RESP_DABORT    12          // Data connection closed, transfer aborted
#define FTPC_RESP_RQNAME    13          // Filename OK, new name required
#define FTPC_RESP_NEWDIROK  14          // New Directory created successfully
#define FTPC_RESP_EPSVOK    15          // Entering Extended Passive mode
#define FTPC_RESP_UNKNOWN   16          // Unrecognised response

/* FTP Client Session info */
typedef struct net_ftpc_info {
  uint8_t  State;                       // Session current state
  uint8_t  Socket;                      // TCP control socket
  uint8_t  Flags;                       // State machine flags
  uint8_t  Timer;                       // Timeout timer value
  __ADDR   Server;                      // Server IP address and port
  uint16_t Port;                        // Server control port
  uint16_t DPort;                       // Server data port (active or passive)
  uint8_t  DSocket;                     // TCP data socket
  netFTP_Command Command;               // Command given
  uint8_t  Resp;                        // Response received
  netFTPc_Event cb_event;               // Return value for callback
  void     *File;                       // File handle pointer
} NET_FTPC_INFO;

/* Variables */
extern NET_FTPC_CFG  net_ftpc_config;
#define ftpc       (&net_ftpc_config)

/* Functions */
extern void net_ftp_client_init (void);
extern void net_ftp_client_uninit (void);
extern void net_ftp_client_run (void);

#endif /* __NET_FTP_CLIENT_H */
