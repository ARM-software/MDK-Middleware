/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ping.c
 * Purpose: Ping Client
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_dns_client.h"
#include "net_ping.h"
#include "net_dbg.h"

/* Global variables */

/* Local Functions */
static void ping_notify (netPing_Event event);

/**
  \brief       Send ping (echo) request in blocking mode.
  \param[in]   target  remote hostname or absolute IP address.
  \param[in]   flags   ping process control flags.
  \return      status code as defined with netStatus.
*/
netStatus netPing_EchoX (const char *target, uint32_t flags) {
  __ADDR addr;
  netStatus res;

  START_LOCK (netStatus);

  /* Check if target is absolute IP address */
  addr.addr_type = NET_ADDR_IP4;
  if ((flags == 0) || (flags & NET_PING_IP4_ONLY)) {
    if (target && net_addr4_aton (target, addr.addr)) {
      /* IPv4 address found */
      goto start;
    }
  }
#ifdef Network_IPv6
  addr.addr_type = NET_ADDR_IP6;
  if ((flags == 0) || (flags & NET_PING_IP6_ONLY)) {
    if (target && net_addr6_aton (target, addr.addr)) {
      /* IPv6 address found */
      goto start;
    }
  }
  if (!(flags & NET_PING_IP6_ONLY)) {
    addr.addr_type = NET_ADDR_IP4;
  }
#endif
  /* IP address not found, resolve external host */
  res = net_dns_client_resolve (target, addr.addr_type, &addr);
  if (res == netInvalidParameter) {
    if (__IS_ADDR_IP4(&addr)) {
      ERRORF (ICMP,"Ping, Target not valid\n");
      EvrNetICMP_PingTargetNotValid ();
    }
#ifdef Network_IPv6
    else {
      ERRORF (ICMP6,"Ping, Target not valid\n");
      EvrNetICMP6_PingTargetNotValid ();
    }
#endif
    RETURN (netInvalidParameter);
  }
  if (res != netOK) {
    if (__IS_ADDR_IP4(&addr)) {
      ERRORF (ICMP,"Ping, DNS error\n");
      EvrNetICMP_PingDnsError ();
    }
#ifdef Network_IPv6
    else {
      ERRORF (ICMP6,"Ping, DNS error\n");
      EvrNetICMP6_PingDnsError ();
    }
#endif
    RETURN (netDnsResolverError);
  }
start:
  for (;;) {
    /* Check if blocking ping is busy */
    if (!ping->xThread) {
      res = net_ping_client_start (&addr, ping_notify);
      if (res == netOK) {
        break;
      }
      if (res != netBusy) {
        /* res: - netInvalidParameter */
        RETURN (res);
      }
    }
    /* Wait a while and try again */
    net_sys_unlock ();
    netos_delay (100);
    net_sys_lock ();
  }
  /* Suspend this thread, wait for net_sys_resume() */
  net_sys_suspend (&ping->xThread);
  RETURN (ping->xRetv);

  END_LOCK;
}

/**
  \brief       User event callback notification.
  \param[in]   event  ping event.
*/
static void ping_notify (netPing_Event event) {
  ping->xRetv = (event == netPing_EventSuccess) ? netOK : netTimeout;
  net_sys_resume (&ping->xThread);
}
