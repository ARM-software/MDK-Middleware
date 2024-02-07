/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_debug.h
 * Purpose: Network Library Debug Configuration
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include "rl_net_lib.h"

#ifndef DBG_SYSTEM_EN
  #error "::Network:CORE:Debug configuration update required"
#endif

#ifndef DBG_WIFI
  #define DBG_WIFI      0
#endif

#ifdef RTE_Network_Debug_STDIO
/* Printf style debug */

#if defined(__clang__)
  #pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif

#if (DBG_TIME)
  #define __DBG_TIME    net_dbg_time()
#else
  #define __DBG_TIME    ""
#endif

/* Print debug information message */
void net_dbg_info (int32_t proc, const char *fmt, ...) {
  __va_list args;

  switch (proc) {

#if (DBG_SYSTEM_EN)
    case NET_SYSTEM_CORE:   break;
  #if (DBG_MEM > 1)
    case NET_DYNAMIC_MEMORY:break;
  #endif
  #if (DBG_ETH > 1)
    case NET_ETH_INTERFACE: break;
  #endif
  #if (DBG_WIFI > 1)
    case NET_WIFI_INTERFACE:break;
  #endif
  #if (DBG_PPP > 1)
    case NET_PPP_INTERFACE: break;
  #endif
  #if (DBG_SLIP > 1)
    case NET_SLIP_INTERFACE:break;
  #endif
  #if (DBG_LOOP > 1)
    case NET_LOCAL_LOOPBACK:break;
  #endif
#endif

#if (DBG_CORE4_EN)
  #if (DBG_IP4 > 1)
    case NET_IP4_CORE:      break;
  #endif
  #if (DBG_ICMP > 1)
    case NET_ICMP_CONTROL:  break;
  #endif
  #if (DBG_IGMP > 1)
    case NET_IGMP_HOST:     break;
  #endif
  #if (DBG_NBNS > 1)
    case NET_NBNS_CLIENT:   break;
  #endif
  #if (DBG_DHCP > 1)
    case NET_DHCP_CLIENT:   break;
  #endif
  #if (DBG_ARP > 1)
    case NET_ARP_CACHE:     break;
  #endif
#endif

#if (DBG_CORE6_EN)
  #if (DBG_IP6 > 1)
    case NET_IP6_CORE:      break;
  #endif
  #if (DBG_ICMP6 > 1)
    case NET_ICMP6_CONTROL: break;
  #endif
  #if (DBG_DHCP6 > 1)
    case NET_DHCP6_CLIENT:  break;
  #endif
  #if (DBG_NDP6 > 1)
    case NET_NDP_CACHE:     break;
  #endif
#endif

#if (DBG_SOCKET_EN)
  #if (DBG_UDP_SOCKET > 1)
    case NET_UDP_SOCKET:    break;
  #endif
  #if (DBG_TCP_SOCKET > 1)
    case NET_TCP_SOCKET:    break;
  #endif
  #if (DBG_BSD_SOCKET > 1)
    case NET_BSD_SOCKET:    break;
  #endif
#endif

#if (DBG_SERVICE_EN)
  #if (DBG_HTTP_SERVER > 1)
    case NET_HTTP_SERVER:   break;
  #endif
  #if (DBG_FTP_SERVER > 1)
    case NET_FTP_SERVER:    break;
  #endif
  #if (DBG_FTP_CLIENT > 1)
    case NET_FTP_CLIENT:    break;
  #endif
  #if (DBG_TELNET_SERVER > 1)
    case NET_TELNET_SERVER: break;
  #endif
  #if (DBG_TFTP_SERVER > 1)
    case NET_TFTP_SERVER:   break;
  #endif
  #if (DBG_TFTP_CLIENT > 1)
    case NET_TFTP_CLIENT:   break;
  #endif
  #if (DBG_SMTP_CLIENT > 1)
    case NET_SMTP_CLIENT:   break;
  #endif
  #if (DBG_DNS_CLIENT > 1)
    case NET_DNS_CLIENT:    break;
  #endif
  #if (DBG_SNMP_AGENT > 1)
    case NET_SNMP_AGENT:    break;
  #endif
  #if (DBG_SNTP_CLIENT > 1)
    case NET_SNTP_CLIENT:   break;
  #endif
#endif

    default: return;
  }
  printf ("%s%s:",__DBG_TIME,net_dbg_proc(proc));

  va_start (args,fmt);
  vprintf (fmt,args);
  va_end (args);
}

/* Pring debug error message */
void net_dbg_error (int32_t proc, const char *fmt, ...) {
  __va_list args;

  switch (proc) {

#if (DBG_SYSTEM_EN)
    case NET_SYSTEM_CORE:   break;
  #if (DBG_MEM > 0)
    case NET_DYNAMIC_MEMORY:break;
  #endif
  #if (DBG_ETH > 0)
    case NET_ETH_INTERFACE: break;
  #endif
  #if (DBG_WIFI > 0)
    case NET_WIFI_INTERFACE:break;
  #endif
  #if (DBG_PPP > 0)
    case NET_PPP_INTERFACE: break;
  #endif
  #if (DBG_SLIP > 0)
    case NET_SLIP_INTERFACE:break;
  #endif
  #if (DBG_LOOP > 0)
    case NET_LOCAL_LOOPBACK:break;
  #endif
#endif

#if (DBG_CORE4_EN)
  #if (DBG_IP4 > 0)
    case NET_IP4_CORE:      break;
  #endif
  #if (DBG_ICMP > 0)
    case NET_ICMP_CONTROL:  break;
  #endif
  #if (DBG_IGMP > 0)
    case NET_IGMP_HOST:     break;
  #endif
  #if (DBG_NBNS > 0)
    case NET_NBNS_CLIENT:   break;
  #endif
  #if (DBG_DHCP > 0)
    case NET_DHCP_CLIENT:   break;
  #endif
  #if (DBG_ARP > 0)
    case NET_ARP_CACHE:     break;
  #endif
#endif

#if (DBG_CORE6_EN)
  #if (DBG_IP6 > 0)
    case NET_IP6_CORE:      break;
  #endif
  #if (DBG_ICMP6 > 0)
    case NET_ICMP6_CONTROL: break;
  #endif
  #if (DBG_DHCP6 > 0)
    case NET_DHCP6_CLIENT:  break;
  #endif
  #if (DBG_NDP6 > 0)
    case NET_NDP_CACHE:     break;
  #endif
#endif

#if (DBG_SOCKET_EN)
  #if (DBG_UDP_SOCKET > 0)
    case NET_UDP_SOCKET:    break;
  #endif
  #if (DBG_TCP_SOCKET > 0)
    case NET_TCP_SOCKET:    break;
  #endif
  #if (DBG_BSD_SOCKET > 0)
    case NET_BSD_SOCKET:    break;
  #endif
#endif

#if (DBG_SERVICE_EN)
  #if (DBG_HTTP_SERVER > 0)
    case NET_HTTP_SERVER:   break;
  #endif
  #if (DBG_FTP_SERVER > 0)
    case NET_FTP_SERVER:    break;
  #endif
  #if (DBG_FTP_CLIENT > 0)
    case NET_FTP_CLIENT:    break;
  #endif
  #if (DBG_TELNET_SERVER > 0)
    case NET_TELNET_SERVER: break;
  #endif
  #if (DBG_TFTP_SERVER > 0)
    case NET_TFTP_SERVER:   break;
  #endif
  #if (DBG_TFTP_CLIENT > 0)
    case NET_TFTP_CLIENT:   break;
  #endif
  #if (DBG_SMTP_CLIENT > 0)
    case NET_SMTP_CLIENT:   break;
  #endif
  #if (DBG_DNS_CLIENT > 0)
    case NET_DNS_CLIENT:    break;
  #endif
  #if (DBG_SNMP_AGENT > 0)
    case NET_SNMP_AGENT:    break;
  #endif
  #if (DBG_SNTP_CLIENT > 0)
    case NET_SNTP_CLIENT:   break;
  #endif
#endif

    default: return;
  }
  printf ("%s%s-ERR:",__DBG_TIME,net_dbg_proc(proc));

  va_start (args,fmt);
  vprintf (fmt,args);
  va_end (args);
}
#endif /* RTE_Network_Debug_STDIO */

#ifdef RTE_Network_Debug
/* Event Recorder debug */
#define DEBUG_EVR
#include "net_evr.h"

#define __FILTER(dbg)   (dbg == 1) ? EventRecordError : EventRecordAll

#define EVR_ENABLE_FILTER(dbg,s,e) if (dbg) EventRecorderEnable(__FILTER(dbg),s,e)

void net_evr_init (void) {
  EventRecorderDisable(EventRecordAll, EvtNetStart, EvtNetEnd);
#if (DBG_SYSTEM_EN)
  EVR_ENABLE_FILTER(2,        EvtNetSYS,  EvtNetSYS);
  EVR_ENABLE_FILTER(DBG_MEM,  EvtNetMEM,  EvtNetMEM);
  EVR_ENABLE_FILTER(DBG_ETH,  EvtNetETH,  EvtNetETH);
  EVR_ENABLE_FILTER(DBG_WIFI, EvtNetWiFi, EvtNetWiFi);
  EVR_ENABLE_FILTER(DBG_PPP,  EvtNetPPP,  EvtNetPPP);
  EVR_ENABLE_FILTER(DBG_SLIP, EvtNetSLIP, EvtNetSLIP);
  EVR_ENABLE_FILTER(DBG_LOOP, EvtNetLOOP, EvtNetLOOP);
#endif
#if (DBG_CORE4_EN)
  EVR_ENABLE_FILTER(DBG_IP4,  EvtNetIP4,  EvtNetIP4);
  EVR_ENABLE_FILTER(DBG_ICMP, EvtNetICMP, EvtNetICMP);
  EVR_ENABLE_FILTER(DBG_IGMP, EvtNetIGMP, EvtNetIGMP);
  EVR_ENABLE_FILTER(DBG_NBNS, EvtNetNBNS, EvtNetNBNS);
  EVR_ENABLE_FILTER(DBG_DHCP, EvtNetDHCP, EvtNetDHCP);
  EVR_ENABLE_FILTER(DBG_ARP,  EvtNetARP,  EvtNetARP);
#endif
#if (DBG_CORE6_EN)
  EVR_ENABLE_FILTER(DBG_IP6,  EvtNetIP6,  EvtNetIP6);
  EVR_ENABLE_FILTER(DBG_ICMP6,EvtNetICMP6,EvtNetICMP6);
  EVR_ENABLE_FILTER(DBG_DHCP6,EvtNetDHCP6,EvtNetDHCP6);
  EVR_ENABLE_FILTER(DBG_NDP6, EvtNetNDP,  EvtNetNDP);
#endif
#if (DBG_SOCKET_EN)
  EVR_ENABLE_FILTER(DBG_UDP_SOCKET,   EvtNetUDP, EvtNetUDP);
  EVR_ENABLE_FILTER(DBG_TCP_SOCKET,   EvtNetTCP, EvtNetTCP);
  EVR_ENABLE_FILTER(DBG_BSD_SOCKET,   EvtNetBSD, EvtNetBSD);
#endif
#if (DBG_SERVICE_EN)
  EVR_ENABLE_FILTER(DBG_HTTP_SERVER,  EvtNetHTTPs,EvtNetHTTPs);
  EVR_ENABLE_FILTER(DBG_FTP_SERVER,   EvtNetFTPs, EvtNetFTPs);
  EVR_ENABLE_FILTER(DBG_FTP_CLIENT,   EvtNetFTPc, EvtNetFTPc);
  EVR_ENABLE_FILTER(DBG_TELNET_SERVER,EvtNetTeln, EvtNetTeln);
  EVR_ENABLE_FILTER(DBG_TFTP_SERVER,  EvtNetTFTPs,EvtNetTFTPs);
  EVR_ENABLE_FILTER(DBG_TFTP_CLIENT,  EvtNetTFTPc,EvtNetTFTPc);
  EVR_ENABLE_FILTER(DBG_SMTP_CLIENT,  EvtNetSMTP, EvtNetSMTP);
  EVR_ENABLE_FILTER(DBG_DNS_CLIENT,   EvtNetDNS,  EvtNetDNS);
  EVR_ENABLE_FILTER(DBG_SNMP_AGENT,   EvtNetSNMP, EvtNetSNMP);
  EVR_ENABLE_FILTER(DBG_SNTP_CLIENT,  EvtNetSNTP, EvtNetSNTP);
#endif
}
#endif
