/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_sntp_client.c
 * Purpose: Simple Network Time Protocol Client
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_dns_client.h"
#include "net_sntp_client.h"
#include "net_dbg.h"

/* Local variables */

/* Local Functions */
static void sntp_notify (uint32_t sec, uint32_t frac);

/**
  \brief       Get current NTP time in blocking mode.
  \param[in]   server  server name or absolute IP address.
  \param[out]  sec     pointer to return time in seconds.
  \param[out]  frac    pointer to return fraction of seconds, can be NULL.
  \return      status code as defined with netStatus.
*/
netStatus netSNTPc_GetTimeX (const char *server, uint32_t *sec, uint32_t *frac) {
  netStatus res;
  __ADDR *addr;

  START_LOCK (netStatus);

  /* NULL for 'server' is accepted */
  if (sec == NULL) {
    ERRORF (SNTP,"GetTimeX, Invalid parameter\n");
    EvrNetSNTP_GetTimexInvalidParameter ();
    RETURN (netInvalidParameter);
  }
  if (sntp->State != SNTP_STATE_IDLE) {
    ERRORF (SNTP,"GetTimeX, Client busy\n");
    EvrNetSNTP_GetTimexClientBusy ();
    RETURN (netWrongState);
  }

  /* Implicitly switch to unicast mode */
  if (sntp->Flags & SNTP_FLAG_BCAST) {
    sntp->Flags &= ~SNTP_FLAG_BCAST;
    DEBUGF (SNTP,"Set Unicast mode\n");
    EvrNetSNTP_SetMode (netSNTPc_ModeUnicast);
  }

  if (server != NULL) {
    /* Resolve IP address of NTP server */
    res = net_dns_client_resolve (server, NET_ADDR_IP4, &sntp->Server);
    if (res == netInvalidParameter) {
      ERRORF (SNTP,"GetTimeX, Server not valid\n");
      EvrNetSNTP_GetTimexServerNotValid ();
      RETURN (netInvalidParameter);
    }
    if (res != netOK) {
      ERRORF (SNTP,"GetTimeX, DNS error\n");
      EvrNetSNTP_GetTimexDnsError ();
      RETURN (netDnsResolverError);
    }
    /* Server IP address is now resolved */
    addr = &sntp->Server;
  }
  else {
    /* Use NTP server from the configuration */
    addr = NULL;
  }

  res = net_sntp_client_start (addr, sntp_notify);
  if (res != netOK) {
    /* res: - netInvalidParameter */
    /*      - netWrongState       */
    /*      - netServerError      */
    RETURN (res);
  }

  /* Suspend this thread, wait for net_sys_resume() */
  net_sys_suspend (&sntp->Thread);

  if (sntp->ts.sec == 0) {
    RETURN (netError);
  }

  /* NTP time retrieved successfully */
  *sec = sntp->ts.sec;
  if (frac != NULL) {
    *frac = sntp->ts.frac;
  }
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       User event callback notification.
  \param[in]   sec   seconds since Jan 1, 1970
  \param[in]   frac  fractional part of seconds.
  \return      none.
*/
static void sntp_notify (uint32_t sec, uint32_t frac) {
  sntp->ts.sec  = sec;
  sntp->ts.frac = frac;
  net_sys_resume (&sntp->Thread);
}
