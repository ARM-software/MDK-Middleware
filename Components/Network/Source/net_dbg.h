/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_dbg.h
 * Purpose: Debug Variant Definitions
 *----------------------------------------------------------------------------*/

#ifndef NET_DBG_H_
#define NET_DBG_H_

#include <stdint.h>
#include "net_evr.h"

#if (defined(Network_Debug_EVR) || defined(Network_Debug_STDIO))
  #define __DEBUG_ENABLED
#endif

#ifdef Network_Debug_STDIO
  #define DEBUGF(id,fmt,...) net_debug_info(id,fmt,##__VA_ARGS__)
  #define ERRORF(id,fmt,...) net_debug_error(id,fmt,##__VA_ARGS__)
  #define DEBUG_INFO(f)      debug_info(f)
  #define DEBUG_INF2(p,v)    debug_inf2(p,v)
  extern void net_debug_info (int32_t proc, const char *fmt,...);
  extern void net_debug_error (int32_t proc, const char *fmt,...);
#else
  #define DEBUGF(id,fmt,...)
  #define ERRORF(id,fmt,...)
  #define DEBUG_INFO(f)
  #define DEBUG_INF2(p,v)
#endif

#ifdef Network_Debug_STDIO
  /* Debug module definition wrapper */
  #define SYS               NET_SYSTEM_CORE
  #define MEM               NET_DYNAMIC_MEMORY

  #define ETH               NET_ETH_INTERFACE
  #define WIFI              NET_WIFI_INTERFACE
  #define PPP               NET_PPP_INTERFACE
  #define SLIP              NET_SLIP_INTERFACE
  #define LOOP              NET_LOCAL_LOOPBACK

  #define ARP               NET_ARP_CACHE
  #define IP4               NET_IP4_CORE
  #define ICMP              NET_ICMP_CONTROL
  #define IGMP              NET_IGMP_HOST

  #define IP6               NET_IP6_CORE
  #define ICMP6             NET_ICMP6_CONTROL
  #define NDP               NET_NDP_CACHE
  #define MLD               NET_MLD_NODE

  #define UDP               NET_UDP_SOCKET
  #define TCP               NET_TCP_SOCKET
  #define BSD               NET_BSD_SOCKET

  #define NBNS              NET_NBNS_CLIENT
  #define DHCP              NET_DHCP_CLIENT
  #define DHCP6             NET_DHCP6_CLIENT
  #define DNS               NET_DNS_CLIENT
  #define SNMP              NET_SNMP_AGENT

  #define HTTP              NET_HTTP_SERVER
  #define FTP               NET_FTP_SERVER
  #define FTPC              NET_FTP_CLIENT
  #define TELNET            NET_TELNET_SERVER
  #define TFTP              NET_TFTP_SERVER
  #define TFTPC             NET_TFTP_CLIENT
  #define SMTP              NET_SMTP_CLIENT
  #define SNTP              NET_SNTP_CLIENT
#endif

#endif /* NET_DBG_H_ */
