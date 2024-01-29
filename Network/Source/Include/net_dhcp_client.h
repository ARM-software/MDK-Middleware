/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_dhcp_client.h
 * Purpose: Dynamic Host Configuration Client Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_DHCP_CLIENT_H
#define __NET_DHCP_CLIENT_H

#include "rl_net_lib.h"

#define DHCP_SERVER_PORT    67          // DHCP Server standard port
#define DHCP_CLIENT_PORT    68          // DHCP Client standard port
#define DHCP_CLID_SIZE      19          // DHCP Option Client-identifier length

/* BOOTP definitions */
#define BOOTP_REQUEST       1           // BOOTP Request Opcode
#define BOOTP_REPLY         2           // BOOTP Reply Opcode
#define BOOTP_FLAG_UCAST    0x0000      // Flags - unicast message
#define BOOTP_FLAG_BCAST    0x8000      // Flags - broadcast message
#define BOOTP_MAGIC_COOKIE  0x63825363  // Magic Cookie

/* DHCP states */
#define DHCP_STATE_DISABLED     0       // DHCP Client is disabled
#define DHCP_STATE_INIT_REBOOT  1       // DHCP Reboot, request existing IP
#define DHCP_STATE_REBOOTING    2       // DHCP Client is rebooting
#define DHCP_STATE_INIT         3       // Initialize DHCP Client, request new IP
#define DHCP_STATE_SELECTING    4       // Select DHCP Server, get offer
#define DHCP_STATE_REQUESTING   5       // Request sent, waiting for reply
#define DHCP_STATE_BOUND        6       // IP address resolved and valid
#define DHCP_STATE_RENEWING     7       // IP address will expire, renewing
#define DHCP_STATE_REBINDING    8       // Renew failed, find DHCP Server, rebind
#define DHCP_STATE_AUTO_IP      9       // Selecting failed, select random Auto IP

/* DHCP messages */
#define DHCP_DISCOVER           1       // Choose an IP address for Client
#define DHCP_OFFER              2       // Offer new IP address to Client
#define DHCP_REQUEST            3       // Request an IP address for Client
#define DHCP_DECLINE            4       // Reject offered IP address
#define DHCP_ACK                5       // IP Address lease acknowledge
#define DHCP_NAK                6       // Lease not acked, offer withdrawn
#define DHCP_RELEASE            7       // Release Leased IP Address
#define DHCP_INFORM             8       // Send DHCP Options to Client

/* DHCP options */
#define DHCP_OPT_PAD            0       // Pad option used for aligning
#define DHCP_OPT_SUBNET_MASK    1       // Client's subnet mask
#define DHCP_OPT_TIME_OFFSET    2       // Client's subnet offset from UTC(seconds)
#define DHCP_OPT_ROUTER         3       // Routers IP address in Client's subnet
#define DHCP_OPT_TIME_SERVER    4       // Time servers available to the Client
#define DHCP_OPT_NAME_SERVER    5       // Name servers available to the Client
#define DHCP_OPT_DNS_SERVER     6       // DNS servers available to the Client
#define DHCP_OPT_HOST_NAME      12      // Host Name of the Client
#define DHCP_OPT_NTP_SERVER     42      // Network Time Protocol Servers
#define DHCP_OPT_REQUESTED_IP   50      // Client's requested IP Address
#define DHCP_OPT_LEASE_TIME     51      // Lease Time for the IP Address
#define DHCP_OPT_OVERLOAD       52      // Overload sname,file fields for options
#define DHCP_OPT_MSG_TYPE       53      // DHCP Message Type option
#define DHCP_OPT_SERVER_ID      54      // DHCP Server Identifier (IP Address)
#define DHCP_OPT_PARAM_REQUEST  55      // Parameter Request List of options
#define DHCP_OPT_T1_VALUE       58      // Renewal Time value option (T1)
#define DHCP_OPT_T2_VALUE       59      // Rebinding Time value option (T2)
#define DHCP_OPT_VENDOR_CLASS   60      // Vendor Class Identifier option (VCI)
#define DHCP_OPT_CLIENT_ID      61      // Client Identifier option
#define DHCP_OPT_BOOTFILE       67      // Bootfile Name option
#define DHCP_OPT_POP3_SERVER    70      // POP3 servers available to the Client
#define DHCP_OPT_END            255     // End of valid information in vendor field

/* DHCP Client-id info */
typedef struct net_dhcp_clid {
  uint8_t  Len;                         // Client-id length
  uint8_t  Val[DHCP_CLID_SIZE];         // Client-id value
} NET_DHCP_CLID;

/* DHCP control info */
typedef struct net_dhcp_ctrl {
  uint8_t  State;                       // DHCP Session current state
  uint16_t BootSecs;                    // Time elapsed since started to boot
  uint8_t  SrvAddr[NET_ADDR_IP4_LEN];   // DHCP Server IP address
  uint8_t  RelayAddr[NET_ADDR_IP4_LEN]; // DHCP Relay Agent IP address
  uint8_t  ReqAddr[NET_ADDR_IP4_LEN];   // Requested IP address
  uint8_t  AutoIp[NET_ADDR_IP4_LEN];    // Auto IP address (169.254.x.x)
  uint32_t t1;                          // DHCP Renew Timer (in seconds)
  uint32_t t2;                          // DHCP Rebind Timer (in seconds)
  uint32_t xid;                         // Transaction ID
  uint16_t tout1;                       // Timeout time
  uint16_t tout2;                       // Next Tout time
  NET_DHCP_CLID *Clid;                  // Pointer to Client-identifier
} NET_DHCP_CTRL;

/* Variables */
extern NET_DHCP_CTRL net_eth0_dhcp_control;
extern NET_DHCP_CTRL net_eth1_dhcp_control;
extern NET_DHCP_CTRL net_wifi0_dhcp_control;
extern NET_DHCP_CTRL net_wifi1_dhcp_control;

/* Functions */
extern void net_dhcp_client_init (void);
extern void net_dhcp_client_uninit (void);
extern void net_dhcp_client_run (void);

#endif /* __NET_DHCP_CLIENT_H */
