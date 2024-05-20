/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_bsd_host.c
 * Purpose: Berkeley Hostname Resolver
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_bsd.h"
#include "net_dns_client.h"
#include "net_dbg.h"

/* Local variables */
static NET_BSD_HOST  host_res;
#define hrs        (&host_res)

/* Local Functions */

/**
  \brief       Initialize BSD host resolver.
*/
void net_bsd_host_init (void) {
  DEBUGF (BSD,"Init Host-Resolver\n");
  EvrNetBSD_GetHostInit ();
  memset (hrs, 0, sizeof (*hrs));
}

/**
  \brief       De-initialize BSD host resolver.
*/
void net_bsd_host_uninit (void) {
  DEBUGF (BSD,"Uninit Host-Resolver\n");
  EvrNetBSD_GetHostUninit ();
  memset (hrs, 0, sizeof (*hrs));
}

/**
  \brief       Retrieve host IP address from host name.
  \param[in]   name  host name.
  \param[out]  err   return error code.
  \return      HOSTENT result structure or NULL in case of error.
*/
HOSTENT *gethostbyname (const char *name, int32_t *err) {
  netStatus res;
 
  START_LOCK(HOSTENT *);

  if (name == NULL) {
    /* Parameter not specified */
    goto param;
  }
  if (hrs->DnsBusy) {
    /* Resolver busy, owner thread suspended */
    ERRORF (BSD,"Gethostbyname, Resolver busy\n");
    EvrNetBSD_GetHostResolverBusy ();
    if (err) *err = BSD_ELOCKED;
    RETURN (NULL);
  }

  DEBUGF (BSD,"Gethostbyname \"%s\"\n",name);
  EvrNetBSD_GetHostByName (name, strlen(name));
  hrs->DnsBusy = true;
  res = net_dns_client_resolve (name, NET_ADDR_IP4, (__ADDR *)&hrs->Addr);
  hrs->DnsBusy = false;
  if (res == netInvalidParameter) {
param: /* Parameter error */
    ERRORF (BSD,"Gethostbyname, Invalid parameter\n");
    EvrNetBSD_GetHostInvalidParameter ();
    if (err) *err = BSD_EINVAL;
    RETURN (NULL);
  }
  if (res == netDnsResolverError) {
    DEBUGF (BSD," Hostname not existing\n");
    EvrNetBSD_GetHostNameNotExisting ();
    if (err) *err = BSD_EHOSTNOTFOUND;
    RETURN (NULL);
  }
  if (res == netTimeout) {
    DEBUGF (BSD," Resolver timeout\n");
    EvrNetBSD_GetHostResolverTimeout ();
    if (err) *err = BSD_ETIMEDOUT;
    RETURN (NULL);
  }
  if (res != netOK) {
    ERRORF (BSD,"Gethostbyname, Protocol error\n");
    EvrNetBSD_GetHostResolverError ();
    if (err) *err = BSD_ERROR;
    RETURN (NULL);
  }

  /* Host IPv4 address successfully resolved */
  DEBUGF (BSD," Hostname resolved\n");
  EvrNetBSD_GetHostNameResolved ();
  hrs->user.h_name      = NULL;
  hrs->user.h_aliases   = NULL;
  hrs->user.h_addrtype  = AF_INET;
  hrs->user.h_length    = NET_ADDR_IP4_LEN;
  hrs->user.h_addr_list = (char **)&hrs->h_addr[0];
  hrs->h_addr[0]        = &hrs->host_ip[0];
  hrs->h_addr[1]        = NULL;
  net_addr4_copy ((uint8_t *)&hrs->host_ip, hrs->Addr.addr);
  if (err) *err = 0;
  RETURN (&hrs->user);

  END_LOCK;
}
