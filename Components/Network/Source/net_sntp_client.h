/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_sntp_client.h
 * Purpose: Simple Network Time Protocol Client Definitions
 *----------------------------------------------------------------------------*/

#ifndef NET_SNTP_CLIENT_H_
#define NET_SNTP_CLIENT_H_

#include "net_lib.h"

/* SNTP Definitions */
#define SNTP_SERVER_PORT    123         // SNTP Server standard port
#define SNTP_DEF_VERMODE    0x1b        // SNTP vers.3, mode Client
#define SNTP_TIME1970       2208988800U // Seconds since Jan 1, 1900

/* SNTP States */
#define SNTP_STATE_IDLE     0           // SNTP Client is idle
#define SNTP_STATE_BUSY     1           // Busy, waiting for SNTP response
#define SNTP_STATE_BCAST    2           // Broadcast receive mode

/* SNTP Flags */
#define SNTP_FLAG_BCAST     0x01        // NTP server filtering active
#define SNTP_FLAG_FILTER    0x02        // NTP server filtering active

/* NTP Time Stamp structure */
typedef struct net_ntp_ts {
  uint32_t sec;                         // Seconds since Jan 1, 1900
  uint32_t frac;                        // Fractional part of seconds (2^-32 sec.)
} NET_NTP_TS;

/* SNTP Control info */
typedef struct net_sntp_ctrl {
  uint8_t  State;                       // SNTP Session current state
  uint8_t  Socket;                      // Binded UDP Socket
  uint8_t  Flags;                       // State machine Flags
  uint8_t  Retries;                     // Number of retransmit retries
  uint16_t Timer;                       // SNTP Retransmit timeout counter
  NETOS_ID Thread;                      // Calling thread identifier
  __ADDR   Server;                      // IP address of NTP Server
  netSNTPc_cb_t cb_func;                // User callback function
  NET_NTP_TS ts;                        // Returned time stamp 
} NET_SNTP_CTRL;

/* NTP Header format */
typedef struct net_ntp_header {
  uint8_t  VerMode;                     // Version & Mode of the message
  uint8_t  ClkStrat;                    // Stratum level of the local clock
  uint8_t  PollItv;                     // Poll interval (in power of 2 seconds)
  int8_t   ClkPrec;                     // Clock precision (in power of 2 seconds)
  int32_t  RootDelay;                   // Root Delay (in fixed-point seconds)
  uint32_t RootDisp;                    // Root Dispersion (fixed-point number)
  uint32_t RefClkId;                    // Reference Clock Identifier
  NET_NTP_TS Ref;                       // Reference Timestamp
  NET_NTP_TS Orig;                      // Originate Timestamp
  NET_NTP_TS Rx;                        // Receive Timestamp
  NET_NTP_TS Tx;                        // Transmit Timestamp
} NET_NTP_HEADER;

#define NTP_FRAME(buf)      ((NET_NTP_HEADER *)(uint32_t)(buf))

/* Variables and Constants */
extern NET_SNTP_CFG  net_sntp_config;
extern NET_SNTP_CTRL net_sntp_control;
#define sntpc      (&net_sntp_config)
#define sntp       (&net_sntp_control)

/* Functions */
extern void net_sntp_client_init (void);
extern void net_sntp_client_uninit (void);
extern void net_sntp_client_run (void);
extern netStatus net_sntp_client_start (const __ADDR *addr, netSNTPc_cb_t cb_func);

#endif /* NET_SNTP_CLIENT_H_ */
