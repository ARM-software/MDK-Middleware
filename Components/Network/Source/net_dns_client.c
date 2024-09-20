/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_dns_client.c
 * Purpose: Domain Name Resolver Client
 *----------------------------------------------------------------------------*/

#include "net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_mem.h"
#include "net_ip4.h"
#ifdef Network_IPv6
#include "net_ip6.h"
#endif
#include "net_udp.h"
#include "net_dns_client.h"
#include "net_dbg.h"

/* Local variables */
static NET_DNS_CTRL  dns_control;
#define dns        (&dns_control)

/* Local Functions */
static void     dns_notify (netDNSc_Event event, const NET_ADDR *addr);
static uint32_t dns_listener (int32_t socket, const NET_ADDR *addr,
                                              const uint8_t *buf, uint32_t len);
static netStatus dns_client_start (const char *name,
                                   int16_t addr_type, netDNSc_cb_t cb_func);
static netStatus dns_clear_cache (void);
static netStatus dns_send_message (void);
static void dns_make_qname (const char *name);
static NET_DNS_INFO *dns_cache_find (uint32_t host_id, int16_t addr_type);
static NET_DNS_INFO *dns_cache_alloc (void);
static void dns_client_stop (netDNSc_Event event, const __ADDR *addr);
static bool dns_get_server (void);
static int32_t chk_label (const char *label);
static uint16_t get_tout (uint32_t sec_tout);
#ifdef Network_Debug_STDIO
 static const char *type_ascii (uint16_t rr_type);
 static void debug_info (const NET_DNS_HEADER *dns_hdr);
 static void debug_inf2 (const char *msg, const __ADDR *addr);
#endif

/* ==== Protected DNS Client API functions ==== */

netStatus netDNSc_GetHostByName (const char *name, int16_t addr_type, netDNSc_cb_t cb_func) {
  START_LOCK (netStatus);
  RETURN (dns_client_start (name, addr_type, cb_func));
  END_LOCK;
}
netStatus netDNSc_GetHostByNameX (const char *name, int16_t addr_type, NET_ADDR *addr) {
  START_LOCK (netStatus);
  RETURN (net_dns_client_resolve (name, addr_type, (__ADDR *)addr));
  END_LOCK;
}
netStatus netDNSc_ClearCache (void) {
  START_LOCK (netStatus);
  RETURN (dns_clear_cache ());
  END_LOCK;
}

/* ==== Internal DNS Client functions ==== */

/**
  \brief       Initialize DNS client.
*/
void net_dns_client_init (void) {
  int32_t sock;

  DEBUGF (DNS,"Init Cache, %d entries\n",dnsc->TabSize);
  EvrNetDNS_InitClient (dnsc->TabSize);
  dns->State  = DNS_STATE_IDLE;
  dns->Flags  = 0;
  dns->Retries= 0;
  dns->Timer  = 0;
  dns->Host   = NULL;
  dns->locm   = NULL;
#ifdef Network_IPv6
  dns->locm6  = NULL;
#endif
  sock = net_udp_get_socket (dns_listener);
  if (sock < 0) sock = 0;
  dns->Socket = sock & 0xFF;
  /* Clear the DNS Cache Table */
  memset (dnsc->Table, 0, sizeof (*dnsc->Table) * dnsc->TabSize);
  if (dns->Socket == 0) {
    ERRORF (DNS,"Get socket failed\n");
    EvrNetDNS_GetSocketFailed ();
    net_sys_error (NET_ERROR_UDP_ALLOC);
    return;
  }
}

/**
  \brief       De-initialize DNS client.
*/
void net_dns_client_uninit (void) {
  DEBUGF (DNS,"Uninit Cache\n");
  EvrNetDNS_UninitClient ();

  /* Clear DNS Cache Table and resolver control */
  memset (dnsc->Table, 0, sizeof (*dnsc->Table) * dnsc->TabSize);
  net_sys_resume (&dns->xThread);
  memset (dns, 0, sizeof (*dns));
  dns->xRetv = netTimeout;
}

/**
  \brief       Resove host IP address in blocking mode.
  \param[in]   name       hostname, a null-terminated string.
  \param[in]   addr_type  network address type to resolve (IP4/IP6).
  \param[out]  addr       structure to store resolved IP address to.
  \return      status code as defined with netStatus.
  \note        This function should be called with sys mutex locked.
*/
netStatus net_dns_client_resolve (const char *name,
                                  int16_t addr_type, __ADDR *addr) {
  netStatus res;

  /* Error checking in dns_client_start() */
  if (addr == NULL) name = NULL;
  for (;;) {
    /* Check if blocking DNS resolve is busy */
    if (!dns->xThread) {
      res = dns_client_start (name, addr_type, dns_notify);
      if (res == netOK) {
        break;
      }
      if (res != netBusy) {
        /* res: - netInvalidParameter */
        /*      - netServerError      */
        return (res);
      }
    }
    /* Wait a while and try again */
    net_sys_unlock ();
    netos_delay (100);
    net_sys_lock ();
  }
  /* Suspend this thread, wait for net_sys_resume() */
  net_sys_suspend (&dns->xThread);
  if (dns->xRetv == netOK) {
    /* Host name successfully resolved */
    net_addr_copy (addr, &dns->Addr);
  }
  return (dns->xRetv);
}

/**
  \brief       User event callback notification.
  \param[in]   event  notification event.
  \param[in]   addr   resolved IP address on success or NULL.
*/
static void dns_notify (netDNSc_Event event, const NET_ADDR *addr) {
  (void)addr;
  /* Convert cb_event to return code */
  switch ((int32_t)event) {
    case netDNSc_EventSuccess:
      dns->xRetv = netOK;
      break;
    case netDNSc_EventNotResolved:
      dns->xRetv = netDnsResolverError;
      break;
    case netDNSc_EventTimeout:
      dns->xRetv = netTimeout;
      break;
    default:
      dns->xRetv = netError;
      break;
  }
  /* Resume net_dns_client_resolve thread */
  net_sys_resume (&dns->xThread);
}

/**
  \brief       Run DNS client main function.
*/
void net_dns_client_run (void) {
  NET_DNS_INFO *dns_t;
  int32_t i;

  /* Update DNS cache, decrement TTL's */
  /* Mix-mode timer decrements in two modes: */
  /* - slow mode, if tout value > 3600       */
  /* - fast mode, if tout value <= 3600      */
  if (sys->Flags & SYS_FLAG_SEC) {
    /* Update fast timers, tick is 1 sec */
    for (i = 0, dns_t = &dnsc->Table[0]; i < dnsc->TabSize; dns_t++, i++) {
      if (dns_t->Tout <= 3600 && dns_t->Tout != 0) {
        dns_t->Tout--;
      }
    }
  }
  if (sys->Flags & SYS_FLAG_MIN64) {
    /* Update slow timers, tick is 64 secs */
    for (i = 0, dns_t = &dnsc->Table[0]; i < dnsc->TabSize; dns_t++, i++) {
      if (dns_t->Tout > 3600) {
        dns_t->Tout--;
      }
    }
  }

  if (dns->State == DNS_STATE_IDLE) {
    return;
  }
  if (dns->State == DNS_STATE_LDONE) {
    /* Hostname resolved locally */
    dns_client_stop (netDNSc_EventSuccess, &dns->Addr);
    return;
  }

  if (dns->Timer) {
    if (sys->Flags & SYS_FLAG_TICK) {
      /* Update interval 1 tick */
      dns->Timer--;
    }
    return;
  }
  sys->Busy = true;

  if (dns->Retries) {
    dns->Retries--;
    DEBUGF (DNS,"Retransmit request\n");
    dns_send_message ();
    dns->Timer = 2*SYS_TICK_TSEC;
    return;
  }

  if ((dns->Flags & (DNS_FLAG_RESP | DNS_FLAG_SWITCH)) == 0) {
    /* Current DNS Server not responding, switch DNS servers */
    uint32_t mask = (dns->Flags & DNS_FLAG_USE_LM6) ? DNS_FLAG_SECDNS6 :
                                                      DNS_FLAG_SECDNS;
    dns->Flags ^= mask;
    if (dns_get_server ()) {
      DEBUGF (DNS,"Switch to %s DNS Server\n",
                  (dns->Flags & mask) ? "Secondary" : "Primary");
      EvrNetDNS_ChangeDnsServer ((dns->Flags & mask) ? 1 : 0);
      dns->Flags  |= DNS_FLAG_SWITCH; 
      dns->Retries = 5;
      return;
    }
  }
  /* All retries used up, inform user */
  ERRORF (DNS,"Resolve, Timeout expired\n");
  EvrNetDNS_ClientStopTimeoutExpired ();
  dns_client_stop (netDNSc_EventTimeout, NULL);
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
static uint32_t dns_listener (int32_t socket, const NET_ADDR *addr,
                                              const uint8_t *buf, uint32_t len) {
  NET_DNS_INFO *dns_t;
  uint32_t an_count, ns_count;
  uint32_t idx, ttl;
  uint16_t tid;

  /* Check if this is our socket */
  if (dns->Socket != socket) {
    return (false);
  }
  /* Process DNS responses in busy state only */
  if (dns->State != DNS_STATE_BUSY) {
    return (false);
  }

  DEBUGF (DNS,"*** Process_frame ***\n");
  DEBUG_INF2 ("Server addr",(const __ADDR *)addr);
  EvrNetDNS_ReceiveFrame (addr, len);

  if (addr->port != DNS_SERVER_PORT) {
    ERRORF (DNS,"Process, Wrong Server port\n");
    EvrNetDNS_WrongServerPort (addr->port);
    return (false);
  }
  /* Inital correctness checking */
  if (len < (dns->HostLen + 18)) {
    ERRORF (DNS,"Process, Frame too short\n");
    EvrNetDNS_FrameTooShort (len, 16);
    return (false);
  }
  if (!net_addr_comp ((const __ADDR *)addr, &dns->Addr)) {
    ERRORF (DNS,"Process, Wrong Server addr\n");
    EvrNetDNS_WrongServerAddress ();
    return (false);
  }
  /* Response received from DNS server */
  DEBUG_INFO (DNS_FRAME(buf));
  EvrNetDNS_ShowFrameHeader (buf);
  dns->Flags |= DNS_FLAG_RESP;

  /* Check transaction id */
  tid = htons(DNS_FRAME(buf)->ID);
  if (tid != dns->Tid) {
    ERRORF (DNS,"Process, Wrong TID\n");
    EvrNetDNS_WrongTransactionId (tid, dns->Tid);
    return (false);
  }

  /* Retransmit with short delay, if needed */
  dns->Timer = 1;

  if (!(DNS_FRAME(buf)->Flags & HTONS(0x8000))) {
    ERRORF (DNS,"Process, DNS Request received\n");
    EvrNetDNS_DnsRequestReceived ();
    return (false);
  }

  /* Check OpCode */
  if (DNS_FRAME(buf)->Flags & HTONS(0x7800)) {
    ERRORF (DNS,"Process, OpCode not QUERY\n");
    EvrNetDNS_OpcodeNotQuery ();
    dns_client_stop (netDNSc_EventError, NULL);
    return (false);
  }

  /* Check Truncation */
  if (DNS_FRAME(buf)->Flags & HTONS(0x0200)) {
    /* Accept truncated messages, no retry over TCP available */
    DEBUGF (DNS," Message truncated\n");
    EvrNetDNS_MessageTruncated ();
  }

  /* Check RCODE */
  if ((DNS_FRAME(buf)->Flags & HTONS(0x00ff)) > HTONS(0x0080)) {
    /* RCODE not zero, and recursion is available */
    if ((DNS_FRAME(buf)->Flags & HTONS(0x000f)) == HTONS(0x0003)) {
      DEBUGF (DNS," No such name found\n");
      EvrNetDNS_NoSuchNameFound ();
      dns_client_stop (netDNSc_EventNotResolved, NULL);
      return (true);
    }
    /* There was an error, inform the listener */
    ERRORF (DNS,"Process, RCODE not zero and recursion available\n");
    EvrNetDNS_RcodeAndRecursion ();
    dns_client_stop (netDNSc_EventError, NULL);
    return (false);
  }

  /* Question count == 1 ? */
  if (DNS_FRAME(buf)->QDcnt != HTONS(1)) {
    ERRORF (DNS,"Process, More than one answer received\n");
    EvrNetDNS_MoreAnswersReceived ();
    dns_client_stop (netDNSc_EventError, NULL);
    return (false);
  }

  /* Check if QNAME is the same */
  if (!net_strcmp ((const char *)&buf[12], dns->Host)) {
    ERRORF (DNS,"Process, QNAME not the same\n");
    EvrNetDNS_QnameNotTheSame ();
    return (false);
  }  

  idx = dns->HostLen + 14;
  /* Check if QTYPE is the same */
  if (get_u16 (&buf[idx]) != dns->Qtype) {
    ERRORF (DNS,"Process, QTYPE not the same\n");
    EvrNetDNS_QtypeNotTheSame ();
    dns_client_stop (netDNSc_EventError, NULL);
    return (false);
  }
  /* Check if QCLASS is ok */
  if (get_u16 (&buf[idx+2]) != DNS_CLASS_INET) {
    ERRORF (DNS,"Process, QCLASS not ok\n");
    EvrNetDNS_QclassNotInet ();
    dns_client_stop (netDNSc_EventError, NULL);
    return (false);
  }

  /* Answers */
  an_count = ntohs(DNS_FRAME(buf)->ANcnt);
  ns_count = ntohs(DNS_FRAME(buf)->NScnt);

  /* Process all answer RRs and try to find the answer */
  idx += 4;
  while ((an_count || ns_count) && (idx < len)) {
    /* Domain Name, skip it */
    if ((buf[idx] & 0xC0) == 0xC0) {
      /* Message is compressed    (RFC1035 - page 29) */
      /* Name pointer is 2 bytes, first 2 bits are 11 */
      idx += 2;
    }
    else {
      /* Flat/uncompressed, null-terminated */
      while ((buf[idx] != 0) && (idx < len)) {
        idx++;
      }
      idx++;
    }
    if (len < (idx + 10)) {
      return (false);
    }
    /* Resource Record Type */
    switch (get_u16 (&buf[idx])) {
      case DNS_REC_TYPE_A:
        /* Host address (IPv4) */
        if (get_u16 (&buf[idx+2]) != DNS_CLASS_INET) {
          /* Not INET class, ignore this record */
          break;
        }
        /* TYPE=A and CLASS==INET, read RDLENGTH */
        if (get_u16 (&buf[idx+8]) != NET_ADDR_IP4_LEN) {
          /* Address size not ok */
          break;
        }
        if (len < (idx + 10 + NET_ADDR_IP4_LEN)) {
          /* Frame truncated, possibly due to a cyber attack */
          return (false);
        }
        /* We got some IPv4 address.                */
        /* Is it what we asked for or a NS address? */
        dns->Addr.addr_type = NET_ADDR_IP4;
        net_addr4_copy (dns->Addr.addr, &buf[idx+10]);
        goto cache;

#ifdef Network_IPv6
      case DNS_REC_TYPE_AAAA:
        /* IPv6 address */
        if (get_u16 (&buf[idx+2]) != DNS_CLASS_INET) {
          /* Not INET class, ignore this record */
          break;
        }
        /* TYPE=AAAA and CLASS==INET, read RDLENGTH */
        if (get_u16 (&buf[idx+8]) != NET_ADDR_IP6_LEN) {
          /* Address size is not ok */
          break;
        }
        if (len < (idx + 10 + NET_ADDR_IP6_LEN)) {
          /* Frame truncated, possibly due to a cyber attack */
          return (false);
        }
        /* We got some IPv6 address.                */
        /* Is it what we asked for or a NS address? */
        dns->Addr.addr_type = NET_ADDR_IP6;
        net_addr6_copy (dns->Addr.addr, &buf[idx+10]);
#endif
cache:  if (an_count) {
          /* Great, read IP address and TTL */
          dns_t = dns_cache_alloc ();
#ifdef Network_IPv6
          /* This will copy IPv6 address and IPv4 address too */
          net_addr6_copy (dns_t->IpAddr, dns->Addr.addr);
#else
          /* This is for IPv4 only */
          net_addr4_copy (dns_t->IpAddr, dns->Addr.addr);
#endif
          dns_t->Type   = dns->Addr.addr_type;
          dns_t->HostId = dns->HostId;
          ttl = get_u32 (&buf[idx+4]);
          dns_t->Tout   = get_tout (ttl);
          DEBUG_INF2 ("Got Address",&dns->Addr);
          DEBUGF (DNS," Time to Live %d\n",ttl);
          EvrNetDNS_GotHostAddress (&dns->Addr, ttl);
          dns_client_stop (netDNSc_EventSuccess, &dns->Addr);
          return (true);
        }
        DEBUG_INF2 ("Got Authority Addr",&dns->Addr);
        DEBUGF (DNS," Query the Authority\n");
        EvrNetDNS_GotAuthorityAddress (&dns->Addr);
        /* Invoke another query to the authority */
        dns->Timer = 1;
        return (true);

      default:
        /* Other record types are not handled */
        DEBUGF (DNS," Can't handle %s record\n",
                    type_ascii(get_u16 (&buf[idx])));
        EvrNetDNS_RecordTypeNotSupported (get_u16(&buf[idx]));
        break;
    }
    /* Skip other bytes, goto next record */
    idx += get_u16 (&buf[idx+8]) + 10;
    /* Decrement counters */
    if (an_count) {
      an_count--;
    }
    else {
      ns_count--;
    }
  }
  return (true);
}

/**
  \brief       Start host IP address resolver.
  \param[in]   name       hostname, a null-terminated string.
  \param[in]   addr_type  network address type to resolve (IP4/IP6).
  \param[in]   cb_func    callback function to call, when DNS session ends.
  \return      status code as defined with netStatus:
               - netOK
               - netInvalidParameter
               - netBusy
               - netServerError
  \note        Most recent names are cached.
*/
static netStatus dns_client_start (const char *name,
                                   int16_t addr_type, netDNSc_cb_t cb_func) {
  NET_DNS_INFO *dns_t;
  uint32_t host_id;
  int32_t idx, i;
  int16_t type;

  if ((name == NULL) || (cb_func == NULL)                     ||
#ifdef Network_IPv6
      (addr_type != NET_ADDR_IP4 && addr_type != NET_ADDR_IP6)) {
#else
      (addr_type != NET_ADDR_IP4)                             ) {
#endif
    ERRORF (DNS,"Resolve, Invalid parameter\n");
    EvrNetDNS_ResolveInvalidParameter ();
    return (netInvalidParameter);
  }
  if (dns->State != DNS_STATE_IDLE) {
    ERRORF (DNS,"Resolve, Client busy\n");
    EvrNetDNS_ResolveClientBusy ();
    return (netBusy);
  }
  DEBUGF (DNS,"Resolve \"%s\", IP%d\n",name,(addr_type==NET_ADDR_IP4)?4:6);
  EvrNetDNS_Resolve (name, strlen(name));
  dns->cb_func = cb_func;

  /* Store Query type */
  if (addr_type == NET_ADDR_IP4) {
    dns->Qtype  = DNS_REC_TYPE_A;
  }
#ifdef Network_IPv6
  else {
    dns->Flags |= DNS_FLAG_USE_LM6;
    dns->Qtype  = DNS_REC_TYPE_AAAA;
  }
#endif

  if (strcmp (name, "localhost") == 0) {
    net_addr_get_lhost (addr_type, &dns->Addr);
    DEBUG_INF2 ("Loopback Addr",&dns->Addr);
    goto l_done;
  }

  /* Check if hostname in address text representation */
  if (netIP_aton (name, addr_type, dns->Addr.addr)) {
    /* Done, hostname is IP address in text format */
    dns->Addr.addr_type = addr_type;
    DEBUG_INF2 ("In-name Addr",&dns->Addr);
l_done:
    EvrNetDNS_ResolvedAddress (&dns->Addr);
    dns->State = DNS_STATE_LDONE;
    return (netOK);
  }

  /* Check for text representation type conflict */
  type = (addr_type == NET_ADDR_IP4) ? NET_ADDR_IP6 : NET_ADDR_IP4;
  if (netIP_aton (name, type, dns->Addr.addr)) {
    ERRORF (DNS,"Resolve, Addr_type conflict\n");
    EvrNetDNS_ResolveInvalidParameter ();
    return (netInvalidParameter);
  }

  /* Check if this hostname is already cached */
  host_id = net_strcrc (name);
  dns_t   = dns_cache_find (host_id, addr_type);
  if (dns_t != NULL) {
    /* Found, return IP to the user */
    dns->Addr.addr_type = dns_t->Type;
#ifdef Network_IPv6
    /* Copies both type IPv6 and IPv4 address */
    net_addr6_copy (dns->Addr.addr, dns_t->IpAddr);
#else
    /* Copies only IPv4 address */
    net_addr4_copy (dns->Addr.addr, dns_t->IpAddr);
#endif
    DEBUG_INF2 ("Cached Addr",&dns->Addr);
    dns->State = DNS_STATE_LDONE;
    return (netOK);
  }

  /* Check the DNS hostname format */
  for (idx = 0; ; idx++) {
    i = chk_label (&name[idx]);
    idx += i;
    if (i == 0) {
      /* Invalid character present or label too long */
      ERRORF (DNS,"Resolve, Hostname invalid\n");
      EvrNetDNS_ResolveInvalidParameter ();
      return (netInvalidParameter);
    }
    if (name[idx] == 0) {
      /* End of string found */
      if ((idx == i) || (idx > 255)) {
        /* Domain is missing (no dot found) or hostname too long */
        ERRORF (DNS,"Resolve, Hostname invalid\n");
        EvrNetDNS_ResolveInvalidParameter ();
        return (netInvalidParameter);
      }
      break;
    }
  }

  /* Terminating-0 not included in length */
  dns->HostLen = idx & 0xFF;
  dns->HostId  = host_id;
  dns->locm    = net_ip4_def_localm ();
#ifdef Network_IPv6
  dns->locm6   = net_ip6_def_localm ();
#endif

  /* Set active DNS server address */
  if (!dns_get_server ()) {
    ERRORF (DNS,"Resolve, Server unknown\n");
    EvrNetDNS_ResolveDnsServerUnknown ();
    return (netServerError);
  }

  /* Open DNS Client UDP port for communication */
  net_udp_open (dns->Socket, 0);

  dns_make_qname (name);
  /* Format OK, send a request message */
  DEBUGF (DNS,"Sending request\n");
  dns->Retries = 4;
  dns->Timer   = 1*SYS_TICK_TSEC;
  dns->State   = DNS_STATE_BUSY;
  /* tid, generate a random number */
  dns->Tid     = net_rand16 ();

  if (dns_send_message () != netOK) {
    /* Wait for ARP/NDP resolver and try again */
    dns->Timer = SYS_TICK_T200MS;
  }
  return (netOK);
}

/**
  \brief       Clear the DNS cache.
  \return      status code as defined with netStatus.
*/
static netStatus dns_clear_cache (void) {
  NET_DNS_INFO *dns_t;
  uint32_t i,cnt;

  if (dns->State != DNS_STATE_IDLE) {
    ERRORF (DNS,"ClearCache, Client busy\n");
    EvrNetDNS_ClearCacheClientBusy ();
    return (netBusy);
  }
  cnt = 0;
  for (i = 0, dns_t = &dnsc->Table[0]; i < dnsc->TabSize; dns_t++, i++) {
    if (dns_t->Tout != 0) {
      dns_t->Tout = 0;
      cnt++;
    }
  }
  DEBUGF (DNS,"ClearCache, %d deleted of %d\n",cnt,dnsc->TabSize);
  EvrNetDNS_ClearCache (cnt, dnsc->TabSize);
  (void)cnt;

  return (netOK);
}

/**
  \brief       Construct a DNS message and send it.
  \return      status code as defined with netStatus.
*/
static netStatus dns_send_message (void) {
  uint8_t *sendbuf;
  int32_t idx;

  sendbuf = net_udp_get_buf (dns->HostLen + 18);

  /* First the header */
  DNS_FRAME(sendbuf)->ID    = htons(dns->Tid);
  DNS_FRAME(sendbuf)->Flags = HTONS(DNS_FLAGS_STQUERY);
  DNS_FRAME(sendbuf)->QDcnt = HTONS(1);
  DNS_FRAME(sendbuf)->ANcnt = HTONS(0);
  DNS_FRAME(sendbuf)->NScnt = HTONS(0);
  DNS_FRAME(sendbuf)->ARcnt = HTONS(0);

  DEBUG_INF2 ("Server Addr",&dns->Addr);
  DEBUG_INFO (DNS_FRAME(sendbuf));
  DEBUGF (DNS," Type %s, class IN\n",type_ascii(dns->Qtype));
  EvrNetDNS_SendRequest (&dns->Addr);
  EvrNetDNS_ShowFrameHeader (sendbuf);

  /* Create the question section, first QNAME */
  idx = net_strcpy ((char *)&sendbuf[12], dns->Host) + 13;

  /* Type: Host address or IPv6 Address */
  set_u16 (&sendbuf[idx], dns->Qtype);

  /* Class: Inet */
  set_u16 (&sendbuf[idx+2], DNS_CLASS_INET);

  return (net_udp_send (dns->Socket, &dns->Addr, sendbuf, (uint32_t)idx+4));
}

/**
  \brief       Make a QNAME for DNS request.
  \param[in]   name  pointer to host name.
*/
static void dns_make_qname (const char *name) {
  int32_t i,idx;
  char *buf;

  buf = (char *)net_mem_alloc (dns->HostLen + 2);
  dns->Host = buf;
  for (idx = 0; ; idx += i, name += i) {
    i = net_strchr (name, '.');
    if (i == -1) {
      /* Copies also a terminating 0 */
      i = net_strcpy (&buf[idx+1], name);
      buf[idx] = i & 0xFF;
      break;
    }
    memcpy (&buf[idx+1], name, (uint32_t)i);
    buf[idx] = i & 0xFF; i++;
  }
}

/**
  \brief       Find IP address in DNS cache.
  \param[in]   host_id    host name hash value.
  \param[in]   addr_type  network address type IP4 or IP6.
  \return      pointer to resolved cache entry,
               - NULL= not found.
*/
static NET_DNS_INFO *dns_cache_find (uint32_t host_id, int16_t addr_type) {
  NET_DNS_INFO *dns_t;
  int32_t i;

  for (i = 0, dns_t = &dnsc->Table[0]; i < dnsc->TabSize; dns_t++, i++) {
    if ((dns_t->Tout != 0)         &&
        (dns_t->HostId == host_id) &&
        (dns_t->Type == addr_type)) {
      /* Entry found, Id's and type match */
      return (dns_t);
    }
  }
  return (NULL);
}

/**
  \brief       Allocate a free DNS cache entry.
  \return      pointer to allocated cache entry.
  \note        When cache full, the oldest entry is reused.
*/
static NET_DNS_INFO *dns_cache_alloc (void) {
  NET_DNS_INFO *dns_t, *dns_r;
  uint32_t min_tout;
  int32_t i;

  /* Try to find a free entry */
  for (i = 0, dns_t = &dnsc->Table[0]; i < dnsc->TabSize; dns_t++, i++) {
    if (dns_t->Tout == 0) {
      return (dns_t);
    }
  }
  /* No free entries, find an entry which will expire first */
  dns_r    = &dnsc->Table[0];
  min_tout = dns_r->Tout;
  for (i = 1, dns_t = &dnsc->Table[1]; i < dnsc->TabSize; dns_t++, i++) {
    if (dns_t->Tout < min_tout) {
      min_tout = dns_t->Tout;
      dns_r    = dns_t;
    }
  }
  return (dns_r);
}

/**
  \brief       Stop DNS resolver client.
  \param[in]   event  user notification event.
  \param[in]   addr   resolved IP address.
*/
static void dns_client_stop (netDNSc_Event event, const __ADDR *addr) {
  dns->cb_func (event, (const NET_ADDR *)addr);
  dns->State  = DNS_STATE_IDLE;
  /* Clear all flags, keep only SECDNS flags */
  dns->Flags &= (DNS_FLAG_SECDNS | DNS_FLAG_SECDNS6);
  dns->Timer  = 0;
  dns->Retries= 0;
  if (dns->Host) {
    net_mem_free (__FRAME(dns->Host));
    dns->Host = NULL;
  }
  net_udp_close (dns->Socket);
}

/**
  \brief       Setup a DNS server to use.
  \return      status:
               - true  = DNS server valid,
               - false = unspecified.
*/
static bool dns_get_server (void) {
  dns->Addr.port = DNS_SERVER_PORT;
#ifdef Network_IPv6
  if ((dns->Flags & DNS_FLAG_USE_LM6) && dns->locm6) {
    /* Try IPv6 server for AAAA record query */
    dns->Addr.addr_type = NET_ADDR_IP6;
    if (dns->Flags & DNS_FLAG_SECDNS6) {
      if (!net_addr6_is_unspec (dns->locm6->SecDNS)) {
        net_addr6_copy (dns->Addr.addr, dns->locm6->SecDNS);
        return (true);
      }
      /* Secondary DNS server not known, use primary */
      dns->Flags &= ~DNS_FLAG_SECDNS6;
    }
    if (!net_addr6_is_unspec (dns->locm6->PriDNS)) {
      net_addr6_copy (dns->Addr.addr, dns->locm6->PriDNS);
      return (true);
    }
  }
  else
#endif
  if (dns->locm) {
    /* Use IPv4 DNS server */
    dns->Flags &= ~DNS_FLAG_USE_LM6;
    dns->Addr.addr_type = NET_ADDR_IP4;
    if (dns->Flags & DNS_FLAG_SECDNS) {
      /* Check if secondary DNS server exists */
      if (__ALIGNED_UINT32(dns->locm->SecDNS) != 0) {
        net_addr4_copy (dns->Addr.addr, dns->locm->SecDNS);
        return (true);
      }
      /* Secondary DNS server not known, use primary */
      dns->Flags &= ~DNS_FLAG_SECDNS;
    }
    if (__ALIGNED_UINT32(dns->locm->PriDNS) != 0) {
      net_addr4_copy (dns->Addr.addr, dns->locm->PriDNS);
      return (true);
    }
  }
  return (false);
}

/**
  \brief       Check if hostname label is valid.
  \param[in]   label  pointer to hostname label.
  \return      index of next label or
               0 if error.
*/
static int32_t chk_label (const char *label) {
  int32_t i;

  if (label[0] == '-') {
    /* A label can not start with hyphen '-' */
    return (0);
  }
  for (i = 0; i < 64; i++) {
    if ((label[i] >= 'a' && label[i] <= 'z') ||
        (label[i] >= 'A' && label[i] <= 'Z') ||
        (label[i] >= '0' && label[i] <= '9') ||
        (label[i] == '-' || label[i] == '_')) {
      /* Ascii letters, numbers, hyphen and underscore allowed */
      continue;
    }
    if (label[i] == '.') {
      if ((i == 0) || (label[i-1] == '-')) {
        /* A label can not end with hyphen '-' */
        break;
      }
      return (i);
    }
    if (label[i] == 0) {
      /* End of string found */
      return (i);
    }
    break;
  }
  /* Label invalid */
  return (0);
}

/**
  \brief       Calculate cache timeout.
  \param[in]   sec_tout  requested timeout in seconds.
  \return      value for the mix-mode timer.
*/
static uint16_t get_tout (uint32_t sec_tout) {
  uint32_t tout;

  /* Timeout calculation for mix-mode timer */
  if (sec_tout > 3600) {
    /* Slow mode: max.timeout 45 days and 22 hours */
    tout = ((sec_tout - 3600) / 64) + 3600;
    if (tout > 65535) tout = 65535;
  }
  else {
    /* Fast mode: max.timeout 1 hour */
    tout = sec_tout;
  }
  return (tout & 0xFFFF);
}
 
#ifdef Network_Debug_STDIO
/**
  \brief       Convert Resource Record type to ascii.
  \param[in]   rr_type  Resource Record type.
  \return      pointer to the text representation of rr_type.
*/
static const char *type_ascii (uint16_t rr_type) {
  switch (rr_type) {
    case DNS_REC_TYPE_A:
      return ("A");
    case DNS_REC_TYPE_NS:
      return ("NS");
    case DNS_REC_TYPE_CNAME:
      return ("CNAME");
    case DNS_REC_TYPE_SOA:
      return ("SOA");
    case DNS_REC_TYPE_PTR:
      return ("PTR");
    case DNS_REC_TYPE_MX:
      return ("MX");
    case DNS_REC_TYPE_AAAA:
      return ("AAAA");
  }
  return ("UNDEF");
}

/**
  \brief       Debug print DNS header information.
  \param[in]   dns_hdr  DNS frame header.
*/
static void debug_info (const NET_DNS_HEADER *dns_hdr) {
  DEBUGF (DNS," TID=0x%04X, Flags=0x%04X\n",ntohs(dns_hdr->ID),
                                            ntohs(dns_hdr->Flags));
  DEBUGF (DNS," QDcount=%d, ANcount=%d\n",  ntohs(dns_hdr->QDcnt),
                                            ntohs(dns_hdr->ANcnt));
  DEBUGF (DNS," NScount=%d, ARcount=%d\n",  ntohs(dns_hdr->NScnt),
                                            ntohs(dns_hdr->ARcnt));
}

/**
  \brief       Debug print IP address information.
  \param[in]   msg   explanation message to print.
  \param[in]   addr  IPv4 or IPv6 address.
*/
static void debug_inf2 (const char *msg, const __ADDR *addr) {
  DEBUGF (DNS," %s [%s]\n",msg,net_addr_ntoa(addr));
}
#endif
