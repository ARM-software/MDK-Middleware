/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    DNS_Client.c
 * Purpose: DNS Client Code Template
 * Rev.:    V7.0.1
 *----------------------------------------------------------------------------*/
//! [code_DNS_Client]
#include <rl_net.h>
 
// Notify the user application about DNS client events.
static void dns_cbfunc (netDNSc_Event event, const NET_ADDR *addr) {
 
  switch (event) {
    case netDNSc_EventSuccess:
      // Host Address successfully resolved
      /* Example
      if (addr->addr_type == NET_ADDR_IP4) {
        char ip_ascii[16];
        netIP_ntoa (NET_ADDR_IP4, adr->addr, ip_ascii, sizeof(ip_ascii));
        printf("IPv4 Address: %s\n", ip_ascii);
      }
      */
      break;
 
    case netDNSc_EventNotResolved:
      // Error, host name does not exist in DNS record database
      break;
 
    case netDNSc_EventTimeout:
      // Error, DNS resolver timeout expired
      break;
 
    case netDNSc_EventError:
      // Error, DNS protocol error occurred
      break;
  }
}
 
// Start DNS resolving process.
void resolve_host (void) {
  netStatus res;
 
  res = netDNSc_GetHostByName ("www.arm.com", NET_ADDR_IP4, dns_cbfunc);
  switch (res) {
    case netOK:
      // Started, will complete on callback notification
      break;
 
    case netBusy:
      // Busy, resolving in progress
      break;
 
    case netInvalidParameter:
      // Error, undefined parameters or invalid host name 
      break;
 
    case netServerError:
      // Error, DNS Server IP address unknown
      break;
 
    default:
      break;
  }
}
//! [code_DNS_Client]
