/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_debug.c
 * Purpose: Network Debug Configuration
 *----------------------------------------------------------------------------*/

#include "net_lib.h"

/* Event Recorder debug */
#ifdef Network_Debug_EVR

#include "net_evr.h"

#define __FILTER(dbg)   (dbg == 1) ? EventRecordError : EventRecordAll

#define EVR_ENABLE_FILTER(dbg,s,e) if (dbg) EventRecorderEnable(__FILTER(dbg),s,e)

void net_evr_init (void) {
  EventRecorderDisable(EventRecordAll, EvtNetStart, EvtNetEnd);
#if (NET_DEBUG_SYSTEM)
  EVR_ENABLE_FILTER(2,                 EvtNetSYS,  EvtNetSYS);
  EVR_ENABLE_FILTER(NET_DEBUG_MEM,     EvtNetMEM,  EvtNetMEM);
  EVR_ENABLE_FILTER(NET_DEBUG_ETH,     EvtNetETH,  EvtNetETH);
  EVR_ENABLE_FILTER(NET_DEBUG_WIFI,    EvtNetWiFi, EvtNetWiFi);
  EVR_ENABLE_FILTER(NET_DEBUG_PPP,     EvtNetPPP,  EvtNetPPP);
  EVR_ENABLE_FILTER(NET_DEBUG_SLIP,    EvtNetSLIP, EvtNetSLIP);
  EVR_ENABLE_FILTER(NET_DEBUG_LOOP,    EvtNetLOOP, EvtNetLOOP);
#endif
#if (NET_DEBUG_IP4)
  EVR_ENABLE_FILTER(NET_DEBUG_CORE_IP4,EvtNetIP4,  EvtNetIP4);
  EVR_ENABLE_FILTER(NET_DEBUG_ICMP,    EvtNetICMP, EvtNetICMP);
  EVR_ENABLE_FILTER(NET_DEBUG_IGMP,    EvtNetIGMP, EvtNetIGMP);
  EVR_ENABLE_FILTER(NET_DEBUG_NBNS,    EvtNetNBNS, EvtNetNBNS);
  EVR_ENABLE_FILTER(NET_DEBUG_DHCP,    EvtNetDHCP, EvtNetDHCP);
  EVR_ENABLE_FILTER(NET_DEBUG_ARP,     EvtNetARP,  EvtNetARP);
#endif
#if (NET_DEBUG_IP6)
  EVR_ENABLE_FILTER(NET_DEBUG_CORE_IP6,EvtNetIP6,  EvtNetIP6);
  EVR_ENABLE_FILTER(NET_DEBUG_ICMP6,   EvtNetICMP6,EvtNetICMP6);
  EVR_ENABLE_FILTER(NET_DEBUG_MLD6,    EvtNetMLD,  EvtNetMLD);
  EVR_ENABLE_FILTER(NET_DEBUG_DHCP6,   EvtNetDHCP6,EvtNetDHCP6);
  EVR_ENABLE_FILTER(NET_DEBUG_NDP6,    EvtNetNDP,  EvtNetNDP);
#endif
#if (NET_DEBUG_SOCKET)
  EVR_ENABLE_FILTER(NET_DEBUG_UDP,     EvtNetUDP,  EvtNetUDP);
  EVR_ENABLE_FILTER(NET_DEBUG_TCP,     EvtNetTCP,  EvtNetTCP);
  EVR_ENABLE_FILTER(NET_DEBUG_BSD,     EvtNetBSD,  EvtNetBSD);
#endif
#if (NET_DEBUG_SERVICE)
  EVR_ENABLE_FILTER(NET_DEBUG_HTTP_SERVER,  EvtNetHTTPs,EvtNetHTTPs);
  EVR_ENABLE_FILTER(NET_DEBUG_FTP_SERVER,   EvtNetFTPs, EvtNetFTPs);
  EVR_ENABLE_FILTER(NET_DEBUG_FTP_CLIENT,   EvtNetFTPc, EvtNetFTPc);
  EVR_ENABLE_FILTER(NET_DEBUG_TELNET_SERVER,EvtNetTeln, EvtNetTeln);
  EVR_ENABLE_FILTER(NET_DEBUG_TFTP_SERVER,  EvtNetTFTPs,EvtNetTFTPs);
  EVR_ENABLE_FILTER(NET_DEBUG_TFTP_CLIENT,  EvtNetTFTPc,EvtNetTFTPc);
  EVR_ENABLE_FILTER(NET_DEBUG_SMTP_CLIENT,  EvtNetSMTP, EvtNetSMTP);
  EVR_ENABLE_FILTER(NET_DEBUG_DNS_CLIENT,   EvtNetDNS,  EvtNetDNS);
  EVR_ENABLE_FILTER(NET_DEBUG_SNMP_AGENT,   EvtNetSNMP, EvtNetSNMP);
  EVR_ENABLE_FILTER(NET_DEBUG_SNTP_CLIENT,  EvtNetSNTP, EvtNetSNTP);
#endif
}
#endif /* Network_Debug_EVR */

#if (NET_DEBUG_ENABLE)
 #if ((NET_DEBUG_CHANNEL == 0)) && !defined(RTE_CMSIS_View_EventRecorder)
  #error "Event Recorder component is required!"
 #endif
#endif


/* STDIO printf style debug */
#ifdef Network_Debug_STDIO

#include <stdio.h>
#include <stdarg.h>

#if defined(__clang__)
  #pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif

#if (NET_DEBUG_CHANNEL == 2)
  #define __DEBUG_TIME  net_debug_time()
#else
  #define __DEBUG_TIME  ""
#endif

/* Print debug diagnostic message */
void net_debug_info (int32_t proc, const char *fmt, ...) {
  __va_list args;

  switch (proc) {

#if (NET_DEBUG_SYSTEM)
    case NET_SYSTEM_CORE:   break;
  #if (NET_DEBUG_MEM > 1)
    case NET_DYNAMIC_MEMORY:break;
  #endif
  #if (NET_DEBUG_ETH > 1)
    case NET_ETH_INTERFACE: break;
  #endif
  #if (NET_DEBUG_WIFI > 1)
    case NET_WIFI_INTERFACE:break;
  #endif
  #if (NET_DEBUG_PPP > 1)
    case NET_PPP_INTERFACE: break;
  #endif
  #if (NET_DEBUG_SLIP > 1)
    case NET_SLIP_INTERFACE:break;
  #endif
  #if (NET_DEBUG_LOOP > 1)
    case NET_LOCAL_LOOPBACK:break;
  #endif
#endif

#if (NET_DEBUG_IP4)
  #if (NET_DEBUG_CORE_IP4 > 1)
    case NET_IP4_CORE:      break;
  #endif
  #if (NET_DEBUG_ICMP > 1)
    case NET_ICMP_CONTROL:  break;
  #endif
  #if (NET_DEBUG_ARP > 1)
    case NET_ARP_CACHE:     break;
  #endif
  #if (NET_DEBUG_IGMP > 1)
    case NET_IGMP_HOST:     break;
  #endif
  #if (NET_DEBUG_NBNS > 1)
    case NET_NBNS_CLIENT:   break;
  #endif
  #if (NET_DEBUG_DHCP > 1)
    case NET_DHCP_CLIENT:   break;
  #endif
#endif

#if (NET_DEBUG_IP6)
  #if (NET_DEBUG_CORE_IP6 > 1)
    case NET_IP6_CORE:      break;
  #endif
  #if (NET_DEBUG_ICMP6 > 1)
    case NET_ICMP6_CONTROL: break;
  #endif
  #if (NET_DEBUG_NDP6 > 1)
    case NET_NDP_CACHE:     break;
  #endif
  #if (NET_DEBUG_MLD6 > 1)
    case NET_MLD_NODE:      break;
  #endif
  #if (NET_DEBUG_DHCP6 > 1)
    case NET_DHCP6_CLIENT:  break;
  #endif
#endif

#if (NET_DEBUG_SOCKET)
  #if (NET_DEBUG_UDP > 1)
    case NET_UDP_SOCKET:    break;
  #endif
  #if (NET_DEBUG_TCP > 1)
    case NET_TCP_SOCKET:    break;
  #endif
  #if (NET_DEBUG_BSD > 1)
    case NET_BSD_SOCKET:    break;
  #endif
#endif

#if (NET_DEBUG_SERVICE)
  #if (NET_DEBUG_HTTP_SERVER > 1)
    case NET_HTTP_SERVER:   break;
  #endif
  #if (NET_DEBUG_FTP_SERVER > 1)
    case NET_FTP_SERVER:    break;
  #endif
  #if (NET_DEBUG_FTP_CLIENT > 1)
    case NET_FTP_CLIENT:    break;
  #endif
  #if (NET_DEBUG_TELNET_SERVER > 1)
    case NET_TELNET_SERVER: break;
  #endif
  #if (NET_DEBUG_TFTP_SERVER > 1)
    case NET_TFTP_SERVER:   break;
  #endif
  #if (NET_DEBUG_TFTP_CLIENT > 1)
    case NET_TFTP_CLIENT:   break;
  #endif
  #if (NET_DEBUG_SMTP_CLIENT > 1)
    case NET_SMTP_CLIENT:   break;
  #endif
  #if (NET_DEBUG_DNS_CLIENT > 1)
    case NET_DNS_CLIENT:    break;
  #endif
  #if (NET_DEBUG_SNMP_AGENT > 1)
    case NET_SNMP_AGENT:    break;
  #endif
  #if (NET_DEBUG_SNTP_CLIENT > 1)
    case NET_SNTP_CLIENT:   break;
  #endif
#endif

    default: return;
  }
  printf ("%s%s:",__DEBUG_TIME,net_debug_proc(proc));

  va_start (args,fmt);
  vprintf (fmt,args);
  va_end (args);
}

/* Print debug error message */
void net_debug_error (int32_t proc, const char *fmt, ...) {
  __va_list args;

  switch (proc) {

#if (NET_DEBUG_SYSTEM)
    case NET_SYSTEM_CORE:   break;
  #if (NET_DEBUG_MEM > 0)
    case NET_DYNAMIC_MEMORY:break;
  #endif
  #if (NET_DEBUG_ETH > 0)
    case NET_ETH_INTERFACE: break;
  #endif
  #if (NET_DEBUG_WIFI > 0)
    case NET_WIFI_INTERFACE:break;
  #endif
  #if (NET_DEBUG_PPP > 0)
    case NET_PPP_INTERFACE: break;
  #endif
  #if (NET_DEBUG_SLIP > 0)
    case NET_SLIP_INTERFACE:break;
  #endif
  #if (NET_DEBUG_LOOP > 0)
    case NET_LOCAL_LOOPBACK:break;
  #endif
#endif

#if (NET_DEBUG_IP4)
  #if (NET_DEBUG_CORE_IP4 > 0)
    case NET_IP4_CORE:      break;
  #endif
  #if (NET_DEBUG_ICMP > 0)
    case NET_ICMP_CONTROL:  break;
  #endif
  #if (NET_DEBUG_ARP > 0)
    case NET_ARP_CACHE:     break;
  #endif
  #if (NET_DEBUG_IGMP > 0)
    case NET_IGMP_HOST:     break;
  #endif
  #if (NET_DEBUG_NBNS > 0)
    case NET_NBNS_CLIENT:   break;
  #endif
  #if (NET_DEBUG_DHCP > 0)
    case NET_DHCP_CLIENT:   break;
  #endif
#endif

#if (NET_DEBUG_IP6)
  #if (NET_DEBUG_CORE_IP6 > 0)
    case NET_IP6_CORE:      break;
  #endif
  #if (NET_DEBUG_ICMP6 > 0)
    case NET_ICMP6_CONTROL: break;
  #endif
  #if (NET_DEBUG_NDP6 > 0)
    case NET_NDP_CACHE:     break;
  #endif
  #if (NET_DEBUG_MLD6 > 0)
    case NET_MLD_NODE:      break;
  #endif
  #if (NET_DEBUG_DHCP6 > 0)
    case NET_DHCP6_CLIENT:  break;
  #endif
#endif

#if (NET_DEBUG_SOCKET)
  #if (NET_DEBUG_UDP > 0)
    case NET_UDP_SOCKET:    break;
  #endif
  #if (NET_DEBUG_TCP > 0)
    case NET_TCP_SOCKET:    break;
  #endif
  #if (NET_DEBUG_BSD > 0)
    case NET_BSD_SOCKET:    break;
  #endif
#endif

#if (NET_DEBUG_SERVICE)
  #if (NET_DEBUG_HTTP_SERVER > 0)
    case NET_HTTP_SERVER:   break;
  #endif
  #if (NET_DEBUG_FTP_SERVER > 0)
    case NET_FTP_SERVER:    break;
  #endif
  #if (NET_DEBUG_FTP_CLIENT > 0)
    case NET_FTP_CLIENT:    break;
  #endif
  #if (NET_DEBUG_TELNET_SERVER > 0)
    case NET_TELNET_SERVER: break;
  #endif
  #if (NET_DEBUG_TFTP_SERVER > 0)
    case NET_TFTP_SERVER:   break;
  #endif
  #if (NET_DEBUG_TFTP_CLIENT > 0)
    case NET_TFTP_CLIENT:   break;
  #endif
  #if (NET_DEBUG_SMTP_CLIENT > 0)
    case NET_SMTP_CLIENT:   break;
  #endif
  #if (NET_DEBUG_DNS_CLIENT > 0)
    case NET_DNS_CLIENT:    break;
  #endif
  #if (NET_DEBUG_SNMP_AGENT > 0)
    case NET_SNMP_AGENT:    break;
  #endif
  #if (NET_DEBUG_SNTP_CLIENT > 0)
    case NET_SNTP_CLIENT:   break;
  #endif
#endif

    default: return;
  }
  printf ("%s%s-ERR:",__DEBUG_TIME,net_debug_proc(proc));

  va_start (args,fmt);
  vprintf (fmt,args);
  va_end (args);
}
#endif /* Network_Debug_STDIO */
