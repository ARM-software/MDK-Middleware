/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ping.c
 * Purpose: Ping Client core
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_icmp.h"
#ifdef Network_IPv6
#include "net_icmp6.h"
#endif
#include "net_ping.h"
#include "net_dbg.h"

/* Global variables */
const char net_ping_payload[18] = "AbcdEfghIjklMnopQr";
NET_PING_CTRL net_ping_control;

/* Local Functions */
static bool ping_send_req (void);

/* ==== Protected Ping Client API functions ==== */

netStatus netPing_Echo (const NET_ADDR *addr, netPing_cb_t cb_func) {
  START_LOCK (netStatus);
  RETURN (net_ping_client_start ((const __ADDR *)addr, cb_func));
  END_LOCK;
}

/* ==== Internal Ping Client functions ==== */

/**
  \brief       Initialize ping client.
*/
void net_ping_client_init (void) {
  DEBUGF (ICMP,"Init Client\n");
  EvrNetICMP_PingInit ();
  ping->Flags = 0x00;
  ping->Timer = 0;
}

/**
  \brief       De-initialize ping client.
*/
void net_ping_client_uninit (void) {
  DEBUGF (ICMP,"Uninit Client\n");
  EvrNetICMP_PingUninit ();
  net_sys_resume (&ping->xThread);
  memset (ping, 0, sizeof(*ping));
  ping->xRetv = netTimeout;
}

/**
  \brief       Start ping (echo) client.
  \param[in]   addr     structure with destination IP address.
  \param[in]   cb_func  callback notification function.
  \return      status code as defined with netStatus.
*/
netStatus net_ping_client_start (const __ADDR *addr, netPing_cb_t cb_func) {

  if (addr == NULL) {
    goto err_ip4;
  }
  if ((cb_func == NULL) || net_addr_is_unspec (addr)) {
#ifdef Network_IPv6
    if (addr->addr_type == NET_ADDR_IP6) {
      ERRORF (ICMP6,"Ping, Invalid parameter\n");
      EvrNetICMP6_PingInvalidParameter ();
      return (netInvalidParameter);
    }
#endif
err_ip4:
    ERRORF (ICMP,"Ping, Invalid parameter\n");
    EvrNetICMP_PingInvalidParameter ();
    return (netInvalidParameter);
  }
  if (__IS_ADDR_IP4(addr)) {
    DEBUGF (ICMP,"PingEcho %s\n",net_addr4_ntoa (addr->addr));
    EvrNetICMP_PingEcho (addr->addr);
  }
#ifdef Network_IPv6
  else {
    DEBUGF (ICMP6,"PingEcho [%s]\n",net_addr6_ntoa (addr->addr));
    EvrNetICMP6_PingEcho (addr->addr);
  }
#endif
  if (ping->Flags & PING_FLAG_BUSY) {
    if (__IS_ADDR_IP4(addr)) {
      ERRORF (ICMP,"Ping, Client busy\n");
      EvrNetICMP_PingClientBusy ();
    }
#ifdef Network_IPv6
    else {
      ERRORF (ICMP6,"Ping, Client busy\n");
      EvrNetICMP6_PingClientBusy ();
    }
#endif
    return (netBusy);
  }
  /* Send a ping request now */
  net_addr_copy (&ping->Host, addr);
  ping->net_if  = net_addr_find_route (NULL, addr);
  ping->LocAddr = net_addr_get_local (ping->net_if, addr);
  if (__IS_ADDR_IP4(addr)) {
    DEBUGF (ICMP,"Send %s, Ping request\n",ping->net_if->Name);
    EvrNetICMP_PingSendRequest (ping->net_if->Id);
  }
#ifdef Network_IPv6
  else {
    DEBUGF (ICMP6,"Send %s, Ping request\n",ping->net_if->Name);
    EvrNetICMP6_PingSendRequest (ping->net_if->Id);
  }
#endif
  ping->cb_func = cb_func;
  ping->Flags  |= PING_FLAG_BUSY;
  ping->Timer   = 1*SYS_TICK_TSEC;
  ping->Retries = 3;
  ping->Id      = net_rand16 ();
  ping->Seq     = 0;
  if (ping_send_req () == false) {
    ping->Timer   = 1;
    ping->Retries = 4;
  }
  return (netOK);
}

/**
  \brief       Run main process for ping client.
*/
void net_ping_client_run (void) {

  if (!(ping->Flags & PING_FLAG_BUSY)) {
    return;
  }
  /* Update interval is 1 tick */
  if (ping->Timer) {
    if (sys->Flags & SYS_FLAG_TICK) {
      ping->Timer--;
    }
    return;
  }
  if (ping->Retries) {
    ping->Retries--;
    if (__IS_ADDR_IP4(&ping->Host)) {
      DEBUGF (ICMP,"Retransmit %s, Ping request\n",ping->net_if->Name);
      EvrNetICMP_PingRetransmitRequest (ping->net_if->Id);
    }
#ifdef Network_IPv6
    else {
      DEBUGF (ICMP6,"Retransmit %s, Ping request\n",ping->net_if->Name);
      EvrNetICMP6_PingRetransmitRequest (ping->net_if->Id);
    }
#endif
    ping_send_req ();
    ping->Timer = 1*SYS_TICK_TSEC-1;
    return;
  }
  /* Ping timeout expired */
  if (__IS_ADDR_IP4(&ping->Host)) {
    DEBUGF (ICMP,"Ping %s, Timeout\n",ping->net_if->Name);
    EvrNetICMP_PingTimeout (ping->net_if->Id);
  }
#ifdef Network_IPv6
  else {
    DEBUGF (ICMP6,"Ping %s, Timeout\n",ping->net_if->Name);
    EvrNetICMP6_PingTimeout (ping->net_if->Id);
  }
#endif
  ping->cb_func (netPing_EventTimeout);
  ping->Flags = 0x00;
}

/**
  \brief       Send Echo request.
*/
static bool ping_send_req (void) {

  ping->Seq++;
#ifdef Network_IPv6
  if (ping->Host.addr_type == NET_ADDR_IP6) {
    return (net_icmp6_send_echo (ping->net_if));
  }
#endif
  return (net_icmp_send_echo (ping->net_if));
}
