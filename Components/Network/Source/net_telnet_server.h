/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_telnet_server.h
 * Purpose: Telnet Server Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_TELNET_SERVER_H
#define __NET_TELNET_SERVER_H

#include "net_lib.h"

#define TELN_SERVER_PORT    23          // Telnet Server standard port
#define TELN_FIFOMSK        (TELNET_FIFO_SZ-1)

/* Telnet Server States */
#define TELN_STATE_IDLE     0           // Session is free and unused
#define TELN_STATE_ERROR    1           // Session Error, not initialized
#define TELN_STATE_RESERVED 2           // Session reserved, not activated
#define TELN_STATE_NEGOTIATE 3          // Connected, negotiate parameters
#define TELN_STATE_CONNECT  4           // Connected, Send a header reply
#define TELN_STATE_AUTHUSER 5           // User Authorization state
#define TELN_STATE_ACTIVE   6           // Session active, user authorized
#define TELN_STATE_SENDING  7           // Sending data in repeated loop

/* Telnet Server Flags */
#define TELN_FLAG_FIRST     0x01        // First Telnet Header
#define TELN_FLAG_WACK      0x02        // Waiting for Data Acknowledge
#define TELN_FLAG_CLOSE     0x04        // Telnet Session Close request
#define TELN_FLAG_PASSW     0x08        // Password is not echoed back
#define TELN_FLAG_USEROK    0x10        // Username valid
#define TELN_FLAG_DELAY     0x20        // Delay function is active
#define TELN_FLAG_MESSAGE   0x40        // Solicited message available

/* Telnet Server Local Key Codes */
#define TELN_KEY_UP         0x80        // Cursor Up key code
#define TELN_KEY_DOWN       0x81        // Cursor Down key code
#define TELN_KEY_RIGHT      0x82        // Cursor Right key code
#define TELN_KEY_LEFT       0x83        // Cursor Left key code

/* Telnet Server ASCII Key Codes */
#define ASCII_CTRLC         3           // ^C ascii code
#define ASCII_BS            8           // Back Space
#define ASCII_LF            10          // Line Feed, New Line
#define ASCII_CR            13          // Carriage Return, Enter key
#define ASCII_ESC           27          // ESC ascii code
#define ASCII_DEL           127         // Delete Key

/* Telnet Server Commands */
#define TELN_CMD_SE         240         // End of Subnegotiation parameters
#define TELN_CMD_NOP        241         // No Operation
#define TELN_CMD_DMARK      242         // Data Mark
#define TELN_CMD_BREAK      243         // NVT Character Break
#define TELN_CMD_IP         244         // Interrupt Process
#define TELN_CMD_AO         245         // Abort Output
#define TELN_CMD_AYT        246         // Are You There
#define TELN_CMD_EC         247         // Erase Character
#define TELN_CMD_EL         248         // Erase Line
#define TELN_CMD_GA         249         // Go Ahead
#define TELN_CMD_SB         250         // Subnegotiation Begin
#define TELN_CMD_WILL       251         // Telnet Command WILL
#define TELN_CMD_WONT       252         // Telnet Command WON'T
#define TELN_CMD_DO         253         // Telnet Command DO
#define TELN_CMD_DONT       254         // Telnet Command DON'T
#define TELN_CMD_IAC        255         // Telnet Command IAC

/* Telnet Server Options */
#define TELN_OPT_ECHO       1           // Telnet Option Echo
#define TELN_OPT_SUPGOAH    3           // Telnet Option Supress Go Ahead
#define TELN_OPT_STATUS     5           // Telnet Option Status
#define TELN_OPT_TMARK      6           // Telnet Option Timing-Mark
#define TELN_OPT_TERMTYPE   24          // Telnet Option Terminal Type
#define TELN_OPT_WINSIZE    31          // Terminal Option window size

/* Telnet core control info */
typedef struct net_telnet_ctrl {
  uint16_t port_num;                    // Service port number
  bool     en_auth;                     // Enable authentication 
  bool     run_service;                 // Run service
  const char *user;                     // Username for admin account
  const char *passw;                    // Password for admin account
  uint8_t  user_len;                    // Length of username
  uint8_t  passw_len;                   // Length of password
  uint8_t  cmd_session;                 // Active command session
} NET_TELNET_CTRL;

/* Variables */
extern NET_TELNET_CFG  net_telnet_config;
#define telnc        (&net_telnet_config)

/* Functions */
extern void net_telnet_server_init (void);
extern void net_telnet_server_uninit (void);
extern void net_telnet_server_run (void);

#endif /* __NET_TELNET_SERVER_H */
