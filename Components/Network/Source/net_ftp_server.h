/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ftp_server.h
 * Purpose: File Transfer Server Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_FTP_SERVER_H
#define __NET_FTP_SERVER_H

#include "net_lib.h"

#define FTP_SERVER_PORT     21          // FTP Server standard port
#define FTP_DEF_DPORT       20          // Default FTP data port
#define FTP_MAX_PATH        120         // Max. Path size

/* FTP States */
#define FTP_STATE_IDLE      0           // FTP Server is idle
#define FTP_STATE_ERROR     1           // Error, not initialized
#define FTP_STATE_RESERVED  2           // Reserved, session not activated
#define FTP_STATE_USER      3           // Authentication Username
#define FTP_STATE_PASSW     4           // Authentication Password
#define FTP_STATE_COMMAND   5           // Active, waiting for commands
#define FTP_STATE_LIST      6           // List directory on LIST command
#define FTP_STATE_RDFILE    7           // Read a file on RETR command
#define FTP_STATE_WRFILE    8           // Write a file on STOR command
#define FTP_STATE_DCLOSE    9           // Close a data connection
#define FTP_STATE_MKDIR     10          // Make a Directory (might take longer)
#define FTP_STATE_FSIZE     11          // Get file size info
#define FTP_STATE_FTIME     12          // Get last-modified time of a file
#define FTP_STATE_QUIT      13          // Close FTP session on QUIT command
#define FTP_STATE_KILL      14          // Abort data socket on error
#define FTP_STATE_TWAIT     15          // Wait for sockets to be closed

/* FTP Flags */
#define FTP_FLAG_WACK       0x0001      // Waiting for Control Acknowledge
#define FTP_FLAG_USEROK     0x0002      // Username OK
#define FTP_FLAG_MODEASCII  0x0004      // ASCII transferring mode
#define FTP_FLAG_DACK       0x0008      // Data Socket Acknowledge
#define FTP_FLAG_DOPEN      0x0010      // Data connection opened
#define FTP_FLAG_PASSIVE    0x0020      // Passive Data connection mode
#define FTP_FLAG_LSTLOCK    0x0040      // LIST command locked
#define FTP_FLAG_NAMELST    0x0080      // NLST - Name List command issued
#define FTP_FLAG_ERROR      0x0100      // An error occured in file io operation
#define FTP_FLAG_LOGIN      0x0200      // User login successful
#define FTP_FLAG_FIRST      0x0400      // First call to LIST command

/* FTP Commands */
#define FTP_CMD_USER        0           // Username for authentication
#define FTP_CMD_PASS        1           // Password for authentication
#define FTP_CMD_QUIT        2           // Quit/logout the session
#define FTP_CMD_SYST        3           // Print Operating System type
#define FTP_CMD_NOOP        4           // No operation
#define FTP_CMD_HELP        5           // Pring supported commands
#define FTP_CMD_PWD         6           // Print the working directory
#define FTP_CMD_CWD         7           // Change the working directory
#define FTP_CMD_CDUP        8           // Change working directory to parent dir.
#define FTP_CMD_MKD         9           // Make a new directory
#define FTP_CMD_RMD         10          // Remove an empty directory
#define FTP_CMD_TYPE        11          // Set transfer data type (Ascii/binary)
#define FTP_CMD_PORT        12          // Set/change TCP data port
#define FTP_CMD_PASV        13          // Change to passive mode
#define FTP_CMD_SIZE        14          // Get the size of a file
#define FTP_CMD_MDTM        15          // Get Last-modified time of a file
#define FTP_CMD_NLST        16          // List directory in basic format
#define FTP_CMD_LIST        17          // List directory in extended format
#define FTP_CMD_RETR        18          // Retrieve a file from server
#define FTP_CMD_STOR        19          // Store a file on server
#define FTP_CMD_APPE        20          // Append a file on server
#define FTP_CMD_DELE        21          // Delete a file from server
#define FTP_CMD_RNFR        22          // Rename from name (current name)
#define FTP_CMD_RNTO        23          // Rename to name (new name)
#define FTP_CMD_EPRT        24          // Set Extended TCP data port
#define FTP_CMD_EPSV        25          // Change to Extended passive mode
#define FTP_CMD_FEAT        26          // Print supported extended features
#define FTP_CMD_UNKNOWN     27          // Command not recognized

/* FTP Response codes */
#define FTP_RESP_DONE       0           // Response message sent
#define FTP_RESP_ERROR      1           // Error, Command unrecognized
#define FTP_RESP_READY      2           // Server is ready
#define FTP_RESP_PASSWREQ   3           // Password required for the user
#define FTP_RESP_LOGINOK    4           // User & Password ok, user logged in
#define FTP_RESP_LOGINFAIL  5           // User authentication failed
#define FTP_RESP_QUIT       6           // Quit FTP session
#define FTP_RESP_SYSTEM     7           // Send an OS Type response
#define FTP_RESP_NOTLOGGED  8           // User not logged in, command ignored
#define FTP_RESP_CMDOK      9           // Generic Command OK
#define FTP_RESP_CURDIR     10          // Print Current Working directory
#define FTP_RESP_MKDIROK    11          // Directory created
#define FTP_RESP_USERFIRST  12          // Username must be entered first
#define FTP_RESP_DOPEN      13          // Opening data connection
#define FTP_RESP_DACCEPT    14          // Accepting data connection
#define FTP_RESP_DCLOSE     15          // Closing data connection
#define FTP_RESP_CANTOPEN   16          // Can't open data connection
#define FTP_RESP_PASVOK     17          // Entering Passive mode
#define FTP_RESP_NOTFOUND   18          // File not found error
#define FTP_RESP_CREATERR   19          // Create file error
#define FTP_RESP_DISKFULL   20          // Exceeded storage allocation
#define FTP_RESP_FILECMDOK  21          // File command completed successfully
#define FTP_RESP_DENIED     22          // File/Folder access is denied
#define FTP_RESP_PENDTO     23          // Pending destination name for a rename
#define FTP_RESP_HELP       24          // Print supported commands
#define FTP_RESP_ABORTED    25          // Connection closed, transfer aborted
#define FTP_RESP_EPSVOK     26          // Entering Extended Passive mode
#define FTP_RESP_FEAT       27          // Print supported extended features
#define FTP_RESP_CWDERR     28          // Change working directory failed

/* FTP Command type */
typedef struct net_ftp_ctype {
  const char name[5];                   // Command name (ascii)
  uint8_t id;                           // Command id value
} NET_FTP_CTYPE;

/* FTP Command info */
typedef struct net_ftp_cmd {
  uint8_t session;                      // Current Session index
  uint8_t sel;                          // Selected User command
} NET_FTP_CMD;

/* FTP core control info */
typedef struct net_ftp_ctrl {
  uint16_t port_num;                    // Service port number
  bool     en_auth;                     // Enable authentication 
  bool     run_service;                 // Run service
  const char *root;                     // Root folder path
  const char *user;                     // Username for admin account
  const char *passw;                    // Password for admin account
  uint8_t  user_len;                    // Length of username
  uint8_t  passw_len;                   // Length of password
} NET_FTP_CTRL;

/* Variables */
extern NET_FTP_CFG  net_ftp_config;
#define ftpc      (&net_ftp_config)

/* Functions */
extern void net_ftp_server_init (void);
extern void net_ftp_server_uninit (void);
extern void net_ftp_server_run (void);

#endif /* __NET_FTP_SERVER_H */
