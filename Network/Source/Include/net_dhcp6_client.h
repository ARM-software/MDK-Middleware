/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_dhcp6_client.h
 * Purpose: Dynamic Host Configuration Client for IPv6 Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_DHCP6_CLIENT_H
#define __NET_DHCP6_CLIENT_H

#include "rl_net_lib.h"

#define DHCP6_CLIENT_PORT   546         // DHCP6 Client standard port
#define DHCP6_SERVER_PORT   547         // DHCP6 Server standard port

/* DHCP6 States */
#define DHCP6_STATE_DISABLED    0       // DHCP6 Client is disabled
#define DHCP6_STATE_INIT        1       // Initialize Client, request configuration
#define DHCP6_STATE_START       2       // Wait before sending first request
#define DHCP6_STATE_DISCOVER    3       // Discover DHCP6 Server, get offer
#define DHCP6_STATE_REQUEST     4       // Request sent, waiting for reply
#define DHCP6_STATE_BOUND       5       // IP6 address resolved and valid
#define DHCP6_STATE_RENEW       6       // IP6 address will expire, renewing
#define DHCP6_STATE_REBIND      7       // Renew failed, find DHCP6 Server, rebind
#define DHCP6_STATE_INFO        8       // Request Information in stateless mode
#define DHCP6_STATE_IDLE        9       // Idle in stateless mode

/* DHCP6 messages */
#define DHCP6_MSG_SOLICIT       1       // Discover DHCP6 servers
#define DHCP6_MSG_ADVERTISE     2       // DHCP6 server available response
#define DHCP6_MSG_REQUEST       3       // Request an IP address for client
#define DHCP6_MSG_CONFIRM       4       // Confirm IP address assignment
#define DHCP6_MSG_RENEW         5       // Renew IP address lease
#define DHCP6_MSG_REBIND        6       // Find DHCP6 server and rebind
#define DHCP6_MSG_REPLY         7       // Reply to or acknowledge client message
#define DHCP6_MSG_RELEASE       8       // Release leased IP address
#define DHCP6_MSG_DECLINE       9       // Reject offered IP address
#define DHCP6_MSG_RECONFIG      10      // Reconfigure the client
#define DHCP6_MSG_INFO_REQUEST  11      // Request configuration, no IP assignment
#define DHCP6_MSG_RELAY_FORWARD 12      // Relay forward message to server
#define DHCP6_MSG_RELAY_REPLY   13      // Relay reply message to relay agent

/* DHCP6 options */
#define DHCP6_OPT_CLIENT_ID     1       // Client Identifier option
#define DHCP6_OPT_SERVER_ID     2       // Server Identifier option
#define DHCP6_OPT_IA_NA         3       // Identity Association for non-temp address
#define DHCP6_OPT_IA_TA         4       // Identity Association for temporary address
#define DHCP6_OPT_IA_ADDR       5       // IA address option
#define DHCP6_OPT_ORO           6       // Option request option
#define DHCP6_OPT_PREFERENCE    7       // Preference option
#define DHCP6_OPT_ELAPSED_TIME  8       // Elapsed time option
#define DHCP6_OPT_RELAY_MSG     9       // Relay message option
#define DHCP6_OPT_AUTH          11      // Authentication option
#define DHCP6_OPT_UNICAST       12      // Server unicast option
#define DHCP6_OPT_STATUS_CODE   13      // Status code option
#define DHCP6_OPT_RAPID_COMMIT  14      // Rapid commit option
#define DHCP6_OPT_USER_CLASS    15      // User class identifier option
#define DHCP6_OPT_VENDOR_CLASS  16      // Vendor class identifier option
#define DHCP6_OPT_VENDOR_OPTS   17      // Vendor specific information option
#define DHCP6_OPT_INTERFACE_ID  18      // Interface identifier option
#define DHCP6_OPT_RECONF_MSG    19      // Reconfigure message option
#define DHCP6_OPT_RECONF_ACCEPT 20      // Reconfigure accept option
#define DHCP6_OPT_DNS_SERVERS   23      // DNS Recursive Name Server(s) option
#define DHCP6_OPT_DOMAIN_LIST   24      // Domain Name(s) option
#define DHCP6_OPT_SNTP_SERVERS  31      // SNTP Server(s) option
#define DHCP6_OPT_NTP_SERVER    56      // NTP Server option
#define DHCP6_OPT_BOOTFILE_URL  59      // Boot File URL location option
#define DHCP6_OPT_BOOTFILE_PARAM 60     // Boot File parameters option

/* DHCP6 status codes */
#define DHCP6_STATUS_Success        0   // Success
#define DHCP6_STATUS_UnspecFail     1   // Failure, reason unspecified
#define DHCP6_STATUS_NoAddrAvail    2   // Server has no address available to assign
#define DHCP6_STATUS_NoBinding      3   // Client record unavailable
#define DHCP6_STATUS_NotOnLink      4   // Address prefix not appropriate for the link
#define DHCP6_STATUS_UseMulticast   5   // Force using multicast all_dhcp_agents address

/* DHCP6 flags */
#define DHCP6_FLAG_OPT_VALID    0x01    // Processed option is valid
#define DHCP6_FLAG_STATUS_FAIL  0x02    // Status option code not SUCCESS
#define DHCP6_FLAG_NO_ADDR      0x04    // No address available status code

/* DHCP6 Control info */
typedef struct net_dhcp6_ctrl {
  uint8_t  State;                       // DHCP Session current state
  uint8_t  Flags;                       // State flags
  uint8_t  Retries;                     // Number of retries left
  netDHCP6_Mode Mode;                   // DHCP6 mode stateless/stateful
  uint16_t ElapsTime;                   // Elapsed time after transaction (in 10ms)
  uint16_t tout1;                       // Timeout time
  uint16_t tout2;                       // Next timeout time
  uint32_t timer;                       // DHCP Timer (in seconds)
  uint32_t xid;                         // Transaction ID (lower 3 bytes)
  struct {                              // Identity Association for non-temp Address
    uint32_t T1;                        // T1 Renew timeout (in seconds)
    uint32_t T2;                        // T2 Rebind timeout (in seconds)
    uint8_t  Addr[NET_ADDR_IP6_LEN];    // IPv6 address
    uint32_t PrefTime;                  // Address preferred lifetime (in seconds)
    uint32_t ValidTime;                 // Address valid lifetime (in seconds)
  } IA;
  struct {                              // DHCP Server info
    uint8_t Pref;                       // Server Preference
    uint8_t IdLen;                      // Identifier length
    uint8_t IdVal[14];                  // Identifier value (min size 14)
  } Srv;
} NET_DHCP6_CTRL;

/* Variables */
extern NET_DHCP6_CTRL net_eth0_dhcp6_control;
extern NET_DHCP6_CTRL net_eth1_dhcp6_control;
extern NET_DHCP6_CTRL net_wifi0_dhcp6_control;
extern NET_DHCP6_CTRL net_wifi1_dhcp6_control;

/* Functions */
extern void net_dhcp6_client_init (void);
extern void net_dhcp6_client_uninit (void);
extern void net_dhcp6_client_run (void);

#endif /* __NET_DHCP6_CLIENT_H */
