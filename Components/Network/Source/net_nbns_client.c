/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_nbns_client.c
 * Purpose: NetBIOS Name Service Client
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_lib.h"
#include "net_mem.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_sys.h"
#include "net_ip4.h"
#include "net_udp.h"
#include "net_nbns_client.h"
#include "net_dbg.h"

/* Local variables */
static NET_NBNS_CTRL nbns_control;
#define nbns       (&nbns_control)

/* Local Functions */
static uint32_t nbns_listener (int32_t socket, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len);
static bool nbns_process_response (const uint8_t *buf, uint32_t len);
static void nbns_client_run (void);
static netStatus nbns_send_message (void);
static NET_NBNS_INFO *nbns_cache_find (uint32_t if_id, uint32_t host_id);
static NET_NBNS_INFO *nbns_cache_alloc (void);
static void     base16_encode (char *b16, const char *sp);
static uint32_t base16_decode (char *dp, const char *b16);

/**
  \brief       Initialize NBNS clients.
*/
void net_nbns_client_init (void) {
  int32_t sock;

#ifdef __DEBUG_ENABLED
  NET_IF_CFG *const *p;

  for (p = &net_if_list_lan[0]; *p; p++) {
    if ((*p)->Ip4Cfg && (*p)->Ip4Cfg->EnNbns) {
      DEBUGF (NBNS,"Init_service %s\n",(*p)->Name);
      EvrNetNBNS_InitService ((*p)->Id);
    }
  }
#endif

  /* Clear NBNS cache and resolver control */
  memset (nbns, 0, sizeof (*nbns));
  memset (nbnsc->Table, 0, sizeof (*nbnsc->Table) * nbnsc->TabSize);

  /* One socket for all NBNS clients */
  sock = net_udp_get_socket (nbns_listener);
  if (sock < 0) {
    ERRORF (NBNS,"Get_socket failed\n");
    EvrNetNBNS_GetSocketFailed ();
    net_sys_error (NET_ERROR_UDP_ALLOC);
    return;
  }
  nbns->Socket = sock & 0xFF;

  /* Open UDP port for communication */
  net_udp_open (nbns->Socket, NBNS_PORT);
}

/**
  \brief       De-initialize NBNS client.
*/
void net_nbns_client_uninit (void) {
#ifdef __DEBUG_ENABLED
  NET_IF_CFG *const *p;

  for (p = &net_if_list_lan[0]; *p; p++) {
    if ((*p)->Ip4Cfg && (*p)->Ip4Cfg->EnNbns) {
      DEBUGF (NBNS,"Uninit_service %s\n",(*p)->Name);
      EvrNetNBNS_UninitService ((*p)->Id);
    }
  }
#endif

  /* Clear NBNS cache table and resolver control */
  memset (nbnsc->Table, 0, sizeof (*nbnsc->Table) * nbnsc->TabSize);
  net_sys_resume (&nbns->Thread);
  memset (nbns, 0, sizeof (*nbns));
}

/**
  \brief       Socket event callback notification.
  \param[in]   socket  UDP socket handle.
  \param[in]   addr    structure containing remote IP address and port.
  \param[in]   buf     pointer to a buffer containing the data.
  \param[in]   len     length of the data.
  \return      status:
               - true  = success,
               - false = error.
*/
static uint32_t nbns_listener (int32_t socket, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len) {
  NET_IF_CFG *net_if;
  uint8_t *sendbuf;
  uint32_t n,min_len;
  char *dbuf;

  /* Check if this is our socket and version IPv4 */
  if ((nbns->Socket != socket) || (addr->addr_type != NET_ADDR_IP4)) {
    return (false);
  }
  /* Get network interface descriptor */
  net_if = (__ADDR_X(addr))->net_if;

  DEBUGF (NBNS,"*** Process_frame %s ***\n",net_if->Name);
  EvrNetNBNS_ReceiveFrame (net_if->Id, len);
  if (addr->port != NBNS_PORT) {
    ERRORF (NBNS,"Process %s, Wrong remote port\n",net_if->Name);
    EvrNetNBNS_WrongRemotePort (net_if->Id, addr->port, NBNS_PORT);
    return (false);
  }
  /* Check if NBNS service enabled */
  if (!net_if->Ip4Cfg->EnNbns) {
    DEBUGF (NBNS," Discarded, %s:NetBIOS disabled\n",net_if->Name);
    EvrNetNBNS_NetBiosDisabled (net_if->Id);
    return (false);
  }
  min_len = (NBNS_FRAME(buf)->Flags & NBNS_FLAG_RESP) ? 62 : 50;
  if (len < min_len) {
    ERRORF (NBNS,"Process %s, Frame too short\n",net_if->Name);
    EvrNetNBNS_FrameTooShort (net_if->Id, len, min_len);
    return (false);
  }

  if (NBNS_FRAME(buf)->Flags & HTONS(NBNS_FLAG_RESP)) {
    /* Client, process name query response */
    if ((nbns->process) && (net_if == nbns->net_if)) {
      return (nbns->process (buf, len));
    }
    return (false);
  }

  /* Responder, process name query request */
  if ((NBNS_FRAME(buf)->Flags != HTONS(NBNS_FLAGS_NQUERY)) ||
      (NBNS_FRAME(buf)->QDcnt != HTONS(1))                 ||
      (NBNS_FRAME(buf)->ANcnt != HTONS(0))                 ||
      (NBNS_FRAME(buf)->NScnt != HTONS(0))                 ||
      (NBNS_FRAME(buf)->ARcnt != HTONS(0))) {
    return (false);
  }
  if ((get_u16 (&buf[46]) != NBNS_TYPE_NB) ||
      (get_u16 (&buf[48]) != NBNS_CLASS_INET)) {
    return (false);
  }
  dbuf = (char *)net_mem_alloc (20);
  n    = base16_decode (dbuf, (const char *)&buf[13]);
  DEBUGF (NBNS," IP query for '%s'\n",&dbuf[0]);
  DEBUGF (NBNS," From host [%s]\n",net_addr4_ntoa(addr->addr));
  EvrNetNBNS_NameQueryRequest (dbuf, n+1);
  EvrNetNBNS_QueryFromAddress (net_if->Id, addr->addr);
  /* Check hostname, resource record type and class */
  if ((net_strcasecmp (dbuf, sys->HostName) && (n == strlen(sys->HostName))) &&
      (get_u16 (&buf[46]) == NBNS_TYPE_NB)                                   &&
      (get_u16 (&buf[48]) == NBNS_CLASS_INET)) {
    DEBUGF (NBNS,"Send my IP [%s]\n",net_addr4_ntoa(net_if->localm->IpAddr));
    EvrNetNBNS_NameQueryResponse (net_if->Id, net_if->localm->IpAddr);
    sendbuf = net_udp_get_buf (62);
    NBNS_FRAME(sendbuf)->Id    = NBNS_FRAME(buf)->Id;
    NBNS_FRAME(sendbuf)->Flags = HTONS(NBNS_FLAGS_NQRESP);
    NBNS_FRAME(sendbuf)->QDcnt = HTONS(0);
    NBNS_FRAME(sendbuf)->ANcnt = HTONS(1);
    NBNS_FRAME(sendbuf)->NScnt = HTONS(0);
    NBNS_FRAME(sendbuf)->ARcnt = HTONS(0);

    /* Add host name, type and class */
    memcpy (&sendbuf[12], &buf[12], 38);

    /* Add TTL, length and name flags */
    set_u32 (&sendbuf[50], 0x493e0);
    set_u16 (&sendbuf[54], 6);
    set_u16 (&sendbuf[56], 0);

    /* Add IP address */
    net_addr4_copy (&sendbuf[58], net_if->localm->IpAddr);
    net_udp_send_if (net_if, nbns->Socket, (const __ADDR *)addr, sendbuf, 62);
  }
  net_mem_free (__FRAME(dbuf));
  return (true);
}

/**
  \brief       Process name query response callback.
  \param[in]   buf  pointer to a buffer containing the data.
  \param[in]   len  length of the data.
  \return      status:
               - true  = success,
               - false = error.
*/
static bool nbns_process_response (const uint8_t *buf, uint32_t len) {
  NET_NBNS_INFO *nbns_t;
  uint32_t n,idx;
  uint16_t tid;
  char *dbuf;

  /* Check transaction id */
  tid = htons(NBNS_FRAME(buf)->Id);
  if (tid != nbns->Tid) {
    ERRORF (NBNS,"Process %s, Wrong TID\n",nbns->net_if->Name);
    EvrNetNBNS_WrongTransactionId (nbns->net_if->Id, tid, nbns->Tid);
    return (false);
  }
  if (((NBNS_FRAME(buf)->Flags & HTONS(NBNS_FLAG_OPCODE)) != HTONS(NBNS_OPCODE_QUERY))   ||
      ((NBNS_FRAME(buf)->Flags & HTONS(NBNS_FLAG_RCODE))  != HTONS(NBNS_RCODE_NO_ERROR)) ||
       (NBNS_FRAME(buf)->QDcnt != HTONS(0))                                              ||
       (NBNS_FRAME(buf)->ANcnt != HTONS(1))) {
    return (false);
  }
  dbuf = (char *)net_mem_alloc (20);
  n    = base16_decode (dbuf, (const char *)&buf[13]);
  /* Check hostname, resource record: type, class and length */
  if ((net_strcasecmp (dbuf, nbns->Name) && (n == strlen(nbns->Name))) &&
      (get_u16 (&buf[46])       == NBNS_TYPE_NB)                       &&
      (get_u16 (&buf[48])       == NBNS_CLASS_INET)                    &&
      (get_u16 (&buf[54]) + 56) <= len) {
    /* Find the IP address that is onlink */
    for (idx = 56; idx < len; idx += 6) {
      if (net_ip4_is_onlink (nbns->net_if, &buf[idx+2])) {
        net_addr4_copy (nbns->IpAddr, &buf[idx+2]);
        nbns->Flags |= NBNS_FLAG_VALID;
        break;
      }
    }
    if (nbns->Flags & NBNS_FLAG_VALID) {
      uint32_t ttl = get_u32 (&buf[50]);
      if (ttl > nbnsc->CacheTout) ttl = nbnsc->CacheTout;
      /* Do not cache the IP address when TTL=0 */
      if (ttl != 0) {
        nbns_t = nbns_cache_alloc ();
        nbns_t->HostId  = net_strcrc (nbns->Name);
        nbns_t->IfaceId = nbns->net_if->Id;
        nbns_t->Tout    = ttl & 0xFFFF;
        net_addr4_copy (nbns_t->IpAddr, nbns->IpAddr);
      }
      /* Success, resume the caller thread */
      net_sys_resume (&nbns->Thread);
    }
  }
  net_mem_free (__FRAME(dbuf));
  return (true);
}

/**
  \brief       Resove NBNS host IP address in blocking mode.
  \param[in]   if_id     interface identifier.
  \param[in]   name      hostname, a null-terminated string.
  \param[out]  ip4_addr  resolved IP address.
  \return      status code as defined with netStatus.
*/
__WEAK netStatus netNBNS_Resolve (uint32_t if_id, const char *name, uint8_t *ip4_addr) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_NBNS_INFO *nbns_t;
  uint32_t host_id;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip4Cfg) || (name == NULL) || (ip4_addr == NULL)) {
    ERRORF (NBNS,"Resolve, Invalid parameter\n");
    EvrNetNBNS_ResolveInvalidParameter (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  /* Check if NBNS client enabled */
  if (!net_if->Ip4Cfg->EnNbns) {
    ERRORF (NBNS,"Resolve %s, NetBIOS disabled\n",net_if->Name);
    EvrNetNBNS_ResolveNetBiosDisabled (net_if->Id);
    RETURN (netError);
  }
  if (nbns->Thread) {
    /* Resolver busy, owner thread suspended */
    ERRORF (NBNS,"Resolve %s, Client busy\n",net_if->Name);
    EvrNetNBNS_ResolveClientBusy (net_if->Id);
    RETURN (netBusy);
  }
  DEBUGF (NBNS,"Resolve %s, \"%s\"\n",net_if->Name,name);
  EvrNetNBNS_Resolve (name, strlen(name));

  if ((net_strcasecmp (name, sys->HostName) && (name[strlen(sys->HostName)] == 0)) ||
      (strcmp (name, "localhost") == 0)) {
    net_addr4_copy (ip4_addr, net_addr4_loopback);
    DEBUGF (NBNS," Loopback Addr %s\n",net_addr4_ntoa(net_addr4_loopback));
    EvrNetNBNS_ResolvedAddress (net_if->Id, net_addr4_loopback);
    RETURN (netOK);
  }

  /* Check if this hostname is already cached */
  host_id = net_strcrc (name);
  nbns_t  = nbns_cache_find (net_if->Id, host_id);
  if (nbns_t != NULL) {
    /* Found, return IPv4 address to the user */
    net_addr4_copy (ip4_addr, nbns_t->IpAddr);
    DEBUGF (NBNS," Cached Addr %s\n",net_addr4_ntoa(nbns_t->IpAddr));
    EvrNetNBNS_ResolvedFromCache (net_if->Id, nbns_t->IpAddr);
    RETURN (netOK);
  }

  nbns->Retries = 2;
  nbns->Timer   = 3*SYS_TICK_T200MS;
  /* tid, generate a random number */
  nbns->Tid     = net_rand16 ();
  nbns->net_if  = net_if;
  nbns->Name    = name;
  nbns_send_message ();

  /* Suspend this thread, wait for net_sys_resume() */
  nbns->Flags   = 0x00;
  nbns->run     = nbns_client_run;
  nbns->process = nbns_process_response;
  net_sys_suspend (&nbns->Thread);
  nbns->process = NULL;

  if (nbns->Flags & NBNS_FLAG_TOUT) {
    ERRORF (NBNS,"Resolve %s, Timeout expired\n",net_if->Name);
    EvrNetNBNS_ResolveTimeoutExpired (net_if->Id);
    RETURN (netTimeout);
  }
  if (nbns->Flags & NBNS_FLAG_VALID) {
    /* Host name successfully resolved */
    net_addr4_copy (ip4_addr, nbns->IpAddr);
    DEBUGF (NBNS,"Resolved Addr %s\n",net_addr4_ntoa(nbns->IpAddr));
    EvrNetNBNS_ResolvedAddress (net_if->Id, nbns->IpAddr);
    RETURN (netOK);
  }
  RETURN (netError);

  END_LOCK;
}

/**
  \brief       Clear the NBNS name cache.
  \param[in]   if_id  interface identifier.
  \return      status code as defined with netStatus.
*/
__WEAK netStatus netNBNS_ClearCache (uint32_t if_id) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_NBNS_INFO *nbns_t;
  int32_t i,cnt;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip4Cfg)) {
    ERRORF (NBNS,"ClearCache, Invalid parameter\n");
    EvrNetNBNS_ClearCacheInvalidParam (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  /* Check if NBNS client enabled */
  if (!net_if->Ip4Cfg->EnNbns) {
    ERRORF (NBNS,"ClearCache %s, NetBIOS disabled\n",net_if->Name);
    EvrNetNBNS_ClrCacheNetBiosDisabled (net_if->Id);
    RETURN (netError);
  }
  if (nbns->Thread) {
    ERRORF (NBNS,"ClearCache %s, Client busy\n",net_if->Name);
    EvrNetNBNS_ClearCacheClientBusy (net_if->Id);
    RETURN (netBusy);
  }
  /* Clear cached entries for the interface */
  cnt = 0;
  for (nbns_t = &nbnsc->Table[0], i = nbnsc->TabSize; i > 0; nbns_t++, i--) {
    if ((nbns_t->Tout != 0) && (nbns_t->IfaceId == net_if->Id)) {
      nbns_t->Tout = 0;
      cnt++;
    }
  }
  /* Check if any entry still active */
  for (nbns_t = &nbnsc->Table[0], i = nbnsc->TabSize; i > 0; nbns_t++, i--) {
    if (nbns_t->Tout != 0) {
      break;
    }
  }
  /* Stop cache manager if cache empty */
  if (i == 0) {
    nbns->run = NULL;
  }

  DEBUGF (NBNS,"ClearCache %s, %d deleted of %d\n",net_if->Name,cnt,nbnsc->TabSize);
  EvrNetNBNS_ClearCache (net_if->Id, (uint32_t)cnt, nbnsc->TabSize);
  (void)cnt;

  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Run NBNS client main function.
*/
void net_nbns_client_run (void) {
  if (nbns->run) nbns->run ();
}

/**
  \brief       Run NBNS cache and retransmissions.
*/
static void nbns_client_run (void) {
  NET_NBNS_INFO *nbns_t;
  int32_t i;

  /* Update NBNS cache, decrement timeouts */
  if (sys->Flags & SYS_FLAG_SEC) {
    bool active = false;
    for (nbns_t = &nbnsc->Table[0], i = nbnsc->TabSize; i > 0; nbns_t++, i--) {
      if (nbns_t->Tout != 0) {
        nbns_t->Tout--;
        active = true;
      }
    }
    /* Stop cache manager if cache empty and client idle */
    if (!active && !nbns->Thread) {
      nbns->run = NULL;
    }
  }

  /* Exit if client is idle */
  if (!nbns->Thread) {
    return;
  }

  /* Update retransmission timer */
  if (nbns->Timer) {
    if (sys->Flags & SYS_FLAG_TICK) {
      nbns->Timer--;
    }
    return;
  }
  sys->Busy = true;
  if (nbns->Retries) {
    nbns->Retries--;
    DEBUGF (NBNS,"Resolve %s, Retransmit\n",nbns->net_if->Name);
    EvrNetNBNS_ResolveRetransmit (nbns->net_if->Id);
    nbns_send_message ();
    nbns->Timer = 1*SYS_TICK_TSEC;
    return;
  }

  /* Timeout, resume the caller thread */
  nbns->Flags |= NBNS_FLAG_TOUT;
  net_sys_resume (&nbns->Thread);
}

/**
  \brief       Construct a NBNS message and send it.
  \return      status code as defined with netStatus.
*/
static netStatus nbns_send_message (void) {
  uint8_t *sendbuf;
  NET_ADDR4 addr;

  sendbuf = net_udp_get_buf (50);

  /* First the header */
  NBNS_FRAME(sendbuf)->Id    = htons(nbns->Tid);
  NBNS_FRAME(sendbuf)->Flags = HTONS(NBNS_FLAGS_NQUERY);
  NBNS_FRAME(sendbuf)->QDcnt = HTONS(1);
  NBNS_FRAME(sendbuf)->ANcnt = HTONS(0);
  NBNS_FRAME(sendbuf)->NScnt = HTONS(0);
  NBNS_FRAME(sendbuf)->ARcnt = HTONS(0);

  /* Create the QNAME section */
  sendbuf[12] = 32;
  base16_encode ((char *)&sendbuf[13], nbns->Name);
  sendbuf[45] = 0;

  /* Type: NetBIOS Name Service */
  set_u16 (&sendbuf[46], NBNS_TYPE_NB);

  /* Class: Inet */
  set_u16 (&sendbuf[48], NBNS_CLASS_INET);

  addr.addr_type = NET_ADDR_IP4;
  addr.port      = NBNS_PORT;
  net_addr4_copy (addr.addr, net_addr4_get_subcast (nbns->net_if->localm));
  return (net_udp_send_if (nbns->net_if, nbns->Socket, (__ADDR *)&addr, sendbuf, 50));
}

/**
  \brief       Find IP address in DNS cache.
  \param[in]   if_id    interface identifier.
  \param[in]   host_id  host name hash value.
  \return      pointer to resolved cache entry,
               - NULL= not found.
*/
static NET_NBNS_INFO *nbns_cache_find (uint32_t if_id, uint32_t host_id) {
  NET_NBNS_INFO *nbns_t;
  int32_t i;

  for (nbns_t = &nbnsc->Table[0], i = nbnsc->TabSize; i > 0; nbns_t++, i--) {
    if ((nbns_t->Tout    != 0)       &&
        (nbns_t->HostId  == host_id) &&
        (nbns_t->IfaceId == if_id))  {
      /* Entry found, Id's match */
      return (nbns_t);
    }
  }
  return (NULL);
}

/**
  \brief       Allocate a free NBNS cache entry.
  \return      pointer to allocated cache entry.
*/
static NET_NBNS_INFO *nbns_cache_alloc (void) {
  NET_NBNS_INFO *nbns_t, *nbns_r;
  int32_t i;

  /* Find a free entry or reuse the oldest one */
  nbns_r = &nbnsc->Table[0];
  for (nbns_t = nbns_r+1, i = nbnsc->TabSize; i > 1; nbns_t++, i--) {
    if (nbns_r->Tout == 0) break;
    if (nbns_r->Tout > nbns_t->Tout) {
      nbns_r = nbns_t;
    }
  }
  return (nbns_r);
}

/**
  \brief       Base16 encode the null-terminated string.
  \param[out]  b16  destination base16 encoded string.
  \param[in]   sp   source ascii string.
*/
static void base16_encode (char *b16, const char *sp) {
  uint32_t i;
  int32_t ch;

  /* NetBIOS names are 16 bytes long */
  for (i = 0; i < 15; b16 += 2, i++) {
    ch = sp[i];
    if (ch == 0) break;
    b16[0] = ((ch & 0xF0) >> 4) + 'A';
    b16[1] =  (ch & 0x0F)       + 'A';
  }
  /* Add trailing spaces */
  ch = ' ';
  for ( ; i < 15; b16 += 2, i++) {
    b16[0] = ((ch & 0xF0) >> 4) + 'A';
    b16[1] =  (ch & 0x0F)       + 'A';
  }
  /* Add null-termination */
  ch = 0;
  b16[0] = ((ch & 0xF0) >> 4) + 'A';
  b16[1] =  (ch & 0x0F)       + 'A';
}

/**
  \brief       Decode base16 encoded to null-terminated string.
  \param[out]  dp   destination ascii string.
  \param[in]   b16  source base16 encoded string.
  \return      number of characters copied.
               (null-termination not counted)
*/
static uint32_t base16_decode (char *dp, const char *b16) {
  uint32_t i;
  int32_t ch;

  /* NetBIOS names are 16 bytes long */
  for (i = 0; i < 16; b16 += 2, i++) {
    ch  = (b16[0] - 'A') << 4;
    ch |= (b16[1] - 'A');
    dp[i] = ch & 0xFF;
  }
  /* Remove the trailing spaces */
  for ( ; i; i--) {
    ch = dp[i-1];
    if (ch != 0 && ch != ' ') {
      break;
    }
  }
  /* Add null-termination */
  dp[i] = 0;
  return (i);
}
