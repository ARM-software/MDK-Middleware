/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_dhcp6_client.c
 * Purpose: Dynamic Host Configuration Client for IPv6
 *----------------------------------------------------------------------------*/

#include "net_lib.h"

#ifdef Network_IPv6
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_mem.h"
#include "net_ip6.h"
#include "net_udp.h"
#include "net_dhcp6_client.h"
#include "net_dbg.h"

/* DUID time: Jan 1, 2015 01:01:06 */
#define DHCP6_DUID_TIME   0x1C3758D2U

/* Identity Association Identifier */
#define DHCP6_IAID        0x0E008130U

/* Global variables */
NET_DHCP6_CTRL net_eth0_dhcp6_control;
NET_DHCP6_CTRL net_eth1_dhcp6_control;
NET_DHCP6_CTRL net_wifi0_dhcp6_control;
NET_DHCP6_CTRL net_wifi1_dhcp6_control;

/* Code shortening macros */
#define ctrl         ( h->Ctrl)
#define LocM6        (*h->If->localm6)

/* Local constants */
static uint8_t const net_addr6_all_dhcp_agents[NET_ADDR_IP6_LEN] = {
  0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02 };

/* Local variables */
static uint8_t dhcp6_socket;
static uint8_t num_active;

/* Local functions */
static void dhcp6_client_start (NET_DHCP6_CFG *h, netDHCP6_Mode mode);
static void dhcp6_client_stop (NET_DHCP6_CFG *h);
static void dhcp6_client_run (NET_DHCP6_CFG *h);
static void dhcp6_send_message (NET_DHCP6_CFG *h, uint8_t msg_type);
static uint32_t dhcp6_listener (int32_t socket, const NET_ADDR *addr,
                                                const uint8_t *buf, uint32_t len);
static bool select_server (NET_DHCP6_CFG *h, const uint8_t *buf, uint32_t len);
static bool check_server_id (NET_DHCP6_CFG *h, const uint8_t *buf, uint32_t len);
static bool check_client_id (NET_DHCP6_CFG *h, const uint8_t *buf, uint32_t len);
static void proc_opt_ia_na (NET_DHCP6_CFG *h, const uint8_t *buf, uint32_t len);
static uint16_t get_tout (uint32_t prev_tout, uint32_t max_tout, bool first);
#ifdef Network_Debug_STDIO
 static const char *msg_ascii (uint8_t msg_type);
 static const char *stat_ascii (uint16_t status);
 static void debug_inf2 (const uint8_t *val, uint32_t len);
#endif

/**
  \brief       Initialize DHCPv6 client.
*/
void net_dhcp6_client_init (void) {
  NET_DHCP6_CFG *const *p;
  netDHCP6_Mode mode;
  int32_t sock;

  for (p = &net_dhcp6_list[0]; *p; p++) {
    DEBUGF (DHCP6,"Init_client %s\n",(*p)->If->Name);
    EvrNetDHCP6_InitClient ((*p)->If->Id, (*p)->OptVclass);
#ifdef Network_Debug_STDIO
    if ((*p)->OptVclass) {
      DEBUGF (DHCP6," Opt.16 Vendor-Class\n");
      DEBUGF (DHCP6," + Enterprise-ID=%d\n",(*p)->EnterpId);
      DEBUGF (DHCP6," + Data=[%s]\n",(*p)->VcData);
    }
#endif
    memset ((*p)->Ctrl, 0, sizeof (*(*p)->Ctrl));
  }
  /* One socket for all DHCPv6 clients */
  sock = net_udp_get_socket (dhcp6_listener);
  if (sock < 0) {
    ERRORF (DHCP6,"Get_socket failed\n");
    EvrNetDHCP6_GetSocketFailed ();
    netHandleError (netErrorUdpAlloc);
    return;
  }
  dhcp6_socket = sock & 0xFF;
  num_active   = 0;
  if (!sysc->AutoStart) {
    return;
  }
  for (p = &net_dhcp6_list[0]; *p; p++) {
    mode = ((*p)->Stateful) ? netDHCP6_ModeStateful : netDHCP6_ModeStateless;
    dhcp6_client_start (*p, mode);
  }
}

/**
  \brief       De-initialize DHCPv6 client.
*/
void net_dhcp6_client_uninit (void) {
  NET_DHCP6_CFG *const *p;

  for (p = &net_dhcp6_list[0]; *p; p++) {
    DEBUGF (DHCP6,"Uninit_client %s\n",(*p)->If->Name);
    EvrNetDHCP6_UninitClient ((*p)->If->Id);
    memset ((*p)->Ctrl, 0, sizeof (*(*p)->Ctrl));
  }
}

/**
  \brief       Enable dynamic host configuration.
  \param[in]   if_id   interface identifier.
  \param[in]   mode    DHCPv6 operation mode.
  \return      status code as defined with netStatus.
*/
__WEAK netStatus netDHCP6_Enable (uint32_t if_id, netDHCP6_Mode mode) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_DHCP6_CFG *h;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip6Cfg && net_if->Ip6Cfg->Dhcp6Cfg)) {
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip6Cfg->Dhcp6Cfg;

  if (dhcp6_socket == 0) {
    RETURN (netError);
  }
  if (ctrl->State == DHCP6_STATE_DISABLED) {
    /* Service is disabled */
    dhcp6_client_start (h, mode);
  }
  else if (mode != ctrl->Mode) {
    /* Service active, change mode not allowed */
    RETURN (netError);
  }
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Disable dynamic host configuration.
  \param[in]   if_id   interface identifier.
  \return      status code as defined with netStatus.
*/
__WEAK netStatus netDHCP6_Disable (uint32_t if_id) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_DHCP6_CFG *h;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip6Cfg && net_if->Ip6Cfg->Dhcp6Cfg)) {
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip6Cfg->Dhcp6Cfg;

  if (ctrl->State != DHCP6_STATE_DISABLED) {
    /* Service is running */
    dhcp6_client_stop (h);
  }
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Start DHCPv6 client service.
  \param[in]   h     DHCPv6 instance handle.
  \param[in]   mode  DHCPv6 operation mode.
*/
static void dhcp6_client_start (NET_DHCP6_CFG *h, netDHCP6_Mode mode) {
  DEBUGF (DHCP6,"Start_service %s, State%s\n",h->If->Name,
                (mode == netDHCP6_ModeStateless) ? "less" : "ful");
  EvrNetDHCP6_StartClient (h->If->Id, mode);
  if (num_active++ == 0) {
    net_udp_open (dhcp6_socket, DHCP6_CLIENT_PORT);
  }
  if (mode == netDHCP6_ModeStateful) {
    /* Local IP6 address is unusable, not yet configured */
    net_addr6_copy (LocM6.IpAddr, net_addr_unspec);
  }
  ctrl->xid   = net_rand32 ();
  ctrl->Mode  = mode;
  ctrl->State = DHCP6_STATE_INIT;
}

/**
  \brief       Stop DHCPv6 client service.
  \param[in]   h  DHCPv6 instance handle.
*/
static void dhcp6_client_stop (NET_DHCP6_CFG *h) {
  DEBUGF (DHCP6,"Stop_service %s\n",h->If->Name);
  EvrNetDHCP6_StopClient (h->If->Id);
  if (--num_active == 0) {
    net_udp_close (dhcp6_socket);
  }
  /* Use static configuration */
  net_addr6_aton (h->If->Ip6Cfg->IpAddr, LocM6.IpAddr);
  net_addr6_aton (h->If->Ip6Cfg->DefGW,  LocM6.DefGW);
  net_addr6_aton (h->If->Ip6Cfg->PriDNS, LocM6.PriDNS);
  net_addr6_aton (h->If->Ip6Cfg->SecDNS, LocM6.SecDNS);
  LocM6.PrefixLen = h->If->Ip6Cfg->PrefixLen;
  ctrl->State     = DHCP6_STATE_DISABLED;
  netDHCP6_Notify (h->If->Id,
                   NET_DHCP6_OPTION_IP_ADDRESS, LocM6.IpAddr, NET_ADDR_IP6_LEN);
}

/**
  \brief       Run DHCPv6 client service.
*/
void net_dhcp6_client_run (void) {
  NET_DHCP6_CFG *const *p;

  for (p = &net_dhcp6_list[0]; *p; p++) {
    dhcp6_client_run (*p);
  }
}

/**
  \brief       Run DHCPv6 client service.
  \param[in]   h  DHCPv6 instance handle.
*/
static void dhcp6_client_run (NET_DHCP6_CFG *h) {
  if (ctrl->State == DHCP6_STATE_DISABLED) {
    /* Do nothing, DHCPv6 Client is disabled */
    return;
  }
  /* Executed every 100ms */
  if (!(sys->Flags & SYS_FLAG_TICK)) {
    return;
  }

  switch (ctrl->State) {
    case DHCP6_STATE_INIT:
      /* Initialize, wait for Link-up */
      if (!h->If->State->LinkUp) {
        /* Link is down, do nothing */
        return;
      }
      DEBUGF (DHCP6,"State-INIT %s\n",h->If->Name);
      EvrNetDHCP6_ClientState (h->If->Id, DHCP6_STATE_INIT);
      ctrl->Flags = 0x00;
      ctrl->xid   = net_rand32 ();
      ctrl->tout1 = net_rand_max(SYS_TICK_TSEC) + 1;
      DEBUGF (DHCP6," Link-up, waiting %d00ms\n",ctrl->tout1);
      EvrNetDHCP6_NextStateDelay (h->If->Id, DHCP6_STATE_START, ctrl->tout1);
      ctrl->State = DHCP6_STATE_START;
      return;

    case DHCP6_STATE_START:
      /* Random delay after link-up [0.1 - 1.0 secs] */
      if (--ctrl->tout1 != 0) {
        return;
      }
      EvrNetDHCP6_ClientState (h->If->Id, DHCP6_STATE_START);
      /* Check if Link is still up */
      if (!h->If->State->LinkUp) {
        goto link_down;
      }
      /* No DHCP server selected (IdLen = 0) */
      ctrl->Srv.IdLen = 0;
      ctrl->ElapsTime = 0;
      if (ctrl->Mode == netDHCP6_ModeStateless) {
        /* Switch to Stateless mode */
        ctrl->State = DHCP6_STATE_INFO;
        dhcp6_send_message (h, DHCP6_MSG_INFO_REQUEST);
        ctrl->tout1 = get_tout (1*SYS_TICK_TSEC, 0, true);
        ctrl->tout2 = get_tout (ctrl->tout1, 0, false);
        DEBUGF (DHCP6," Next State-INFO\n");
        EvrNetDHCP6_NextState (h->If->Id, DHCP6_STATE_INFO);
        return;
      }
      /* Stateful mode */
      ctrl->State   = DHCP6_STATE_DISCOVER;
      dhcp6_send_message (h, DHCP6_MSG_SOLICIT);
      /* Retries are for NO ADDR available status */
      ctrl->Retries = 3;
      ctrl->tout1   = get_tout (1*SYS_TICK_TSEC, 1, true);
      ctrl->tout2   = get_tout (ctrl->tout1, 0, false);
      DEBUGF (DHCP6," Next State-DISCOVER\n");
      EvrNetDHCP6_NextState (h->If->Id, DHCP6_STATE_DISCOVER);
      return;
  }

  /* Check link state */
  if (!h->If->State->LinkUp) {
    /* Link down, restart DHCPv6 client */
    if (ctrl->Mode == netDHCP6_ModeStateful) {
      /* Local IP6 address is unusable, not yet configured */
      net_addr6_copy (LocM6.IpAddr, net_addr_unspec);
      netDHCP6_Notify (h->If->Id,
                       NET_DHCP6_OPTION_IP_ADDRESS, LocM6.IpAddr, NET_ADDR_IP6_LEN);
    }
link_down:
    DEBUGF (DHCP6,"Link-down %s, Next State-INIT\n",h->If->Name);
    EvrNetDHCP6_ChangeStateLinkDown (h->If->Id, DHCP6_STATE_INIT);
    ctrl->State = DHCP6_STATE_INIT;
    return;
  }

  if (ctrl->ElapsTime < 0xFFFF) {
    /* Elapsed time since transaction started (in 10ms) */
    ctrl->ElapsTime += 10;
  }

  switch (ctrl->State) {
    case DHCP6_STATE_DISCOVER:
      /* Waiting for ADVERTISE message from DHCPv6 servers */
      if (--ctrl->tout1 != 0) {
        break;
      }
      /* Check if any response received */
      if (ctrl->Srv.IdLen == 0) {
        if (ctrl->Flags & DHCP6_FLAG_NO_ADDR) {
          /* No address available status was received */
          if (--ctrl->Retries == 0) {
            /* Switch to Stateless mode */
            DEBUGF (DHCP6,"State-DISCOVER %s, no address available\n",h->If->Name);
            EvrNetDHCP6_NoAddressAvailable (h->If->Id, DHCP6_STATE_DISCOVER);
            ctrl->ElapsTime = 0;
            /* Get new XID for INFO */
            ctrl->xid   = net_rand32 ();
            ctrl->State = DHCP6_STATE_INFO;
            dhcp6_send_message (h, DHCP6_MSG_INFO_REQUEST);
            ctrl->tout1 = get_tout (1*SYS_TICK_TSEC, 0, true);
            ctrl->tout2 = get_tout (ctrl->tout1, 0, false);
            DEBUGF (DHCP6," Next State-INFO\n");
            EvrNetDHCP6_NextState (h->If->Id, DHCP6_STATE_INFO);
            break;
          }
        }
        /* No response received, retransmit request */
        DEBUGF (DHCP6,"State-DISCOVER %s, retransmit\n",h->If->Name);
        EvrNetDHCP6_StateRetransmit (h->If->Id, DHCP6_STATE_DISCOVER);
        dhcp6_send_message (h, DHCP6_MSG_SOLICIT);
        ctrl->tout1 = ctrl->tout2;
        ctrl->tout2 = get_tout (ctrl->tout2, 120*SYS_TICK_TSEC, false);
        break;
      }
      /* At least one DHCPv6 server has responded  */
      /* We have the offer now, contact the server */
      DEBUGF (DHCP6,"State-DISCOVER %s, timeout\n",h->If->Name);
      EvrNetDHCP6_StateTimeout (h->If->Id, DHCP6_STATE_DISCOVER);
      ctrl->State   = DHCP6_STATE_REQUEST;
      dhcp6_send_message (h, DHCP6_MSG_REQUEST);
      ctrl->Retries = 10;
      ctrl->tout1   = get_tout (1*SYS_TICK_TSEC, 0, true);
      ctrl->tout2   = get_tout (ctrl->tout1, 0, false);
      DEBUGF (DHCP6," Next State-REQUEST\n");
      EvrNetDHCP6_NextState (h->If->Id, DHCP6_STATE_REQUEST);
      break;

    case DHCP6_STATE_REQUEST:
      /* Waiting for REPLY message from server */
      if(--ctrl->tout1 != 0) {
        break;
      }
      /* Timeout occured without receiving REPLY message */
      if (ctrl->Retries != 0) {
        DEBUGF (DHCP6,"State-REQUEST %s, retransmit\n",h->If->Name);
        EvrNetDHCP6_StateRetransmit (h->If->Id, DHCP6_STATE_REQUEST);
        dhcp6_send_message (h, DHCP6_MSG_REQUEST);
        ctrl->Retries--;
        ctrl->tout1 = ctrl->tout2;
        ctrl->tout2 = get_tout (ctrl->tout2, 30*SYS_TICK_TSEC, false);
        break;
      }
      /* Failed, initiate server discovery again (RFC3315 - page 40) */
      DEBUGF (DHCP6,"State-REQUEST %s timeout, Next State-INIT\n",h->If->Name);
      EvrNetDHCP6_ChangeStateOnTimeout (h->If->Id, DHCP6_STATE_REQUEST, DHCP6_STATE_INIT);
      ctrl->State = DHCP6_STATE_INIT;
      break;

    case DHCP6_STATE_BOUND:
      /* IP6 address is valid, wait for T1 to expire */
      if (sys->Flags & SYS_FLAG_SEC) {
        /* Count interval 1 second */
        if(--ctrl->timer == 0) {
          DEBUGF (DHCP6,"State-BOUND %s, T1 expired\n",h->If->Name);
          EvrNetDHCP6_T1Expired (h->If->Id, DHCP6_STATE_BOUND);
          /* T1 expired, start renewing process */
          ctrl->ElapsTime = 0;
          /* Get new XID for RENEW */
          ctrl->xid   = net_rand32 ();
          ctrl->State = DHCP6_STATE_RENEW;
          dhcp6_send_message (h, DHCP6_MSG_RENEW);
          /* T2 > T1 [RFC3315 requirement] */
          ctrl->timer = ctrl->IA.T2 - ctrl->IA.T1;
          ctrl->tout1 = get_tout (10*SYS_TICK_TSEC, 0, true);
          ctrl->tout2 = get_tout (ctrl->tout1, 0, false);
          DEBUGF (DHCP6," Next State-RENEW\n");
          EvrNetDHCP6_NextState (h->If->Id, DHCP6_STATE_RENEW);
          break;
        }
      }
      break;

    case DHCP6_STATE_RENEW:
      /* Lease Timer expired, Renew leased IP address */
      if (sys->Flags & SYS_FLAG_SEC) {
        /* Count interval 1 second */
        if(--ctrl->timer == 0) {
          DEBUGF (DHCP6,"State-RENEW %s, T2 expired\n",h->If->Name);
          EvrNetDHCP6_T2Expired (h->If->Id, DHCP6_STATE_RENEW);
          /* T2 also expired, switch to REBIND state  */
          /* Current DHCP server is dead, clear IdLen */
          ctrl->Srv.IdLen = 0;
          ctrl->ElapsTime = 0;
          /* Get new XID for REBIND */
          ctrl->xid   = net_rand32 ();
          ctrl->State = DHCP6_STATE_REBIND;
          dhcp6_send_message (h, DHCP6_MSG_REBIND);
          /* ValidTime > T2 [RFC3315 requirement] */
          ctrl->timer = ctrl->IA.ValidTime - ctrl->IA.T2;
          ctrl->tout1 = get_tout (1*SYS_TICK_TSEC, 0, true);
          ctrl->tout2 = get_tout (ctrl->tout1, 0, false);
          DEBUGF (DHCP6," Next State-REBIND\n");
          EvrNetDHCP6_NextState (h->If->Id, DHCP6_STATE_REBIND);
          break;
        }
      }
      if(--ctrl->tout1 != 0) {
        break;
      }
      DEBUGF (DHCP6,"State-RENEW %s, retransmit\n",h->If->Name);
      EvrNetDHCP6_StateRetransmit (h->If->Id, DHCP6_STATE_RENEW);
      dhcp6_send_message (h, DHCP6_MSG_RENEW);
      ctrl->tout1 = ctrl->tout2;
      ctrl->tout2 = get_tout (ctrl->tout2, 600*SYS_TICK_TSEC, false);
      break;

    case DHCP6_STATE_REBIND:
      /* Current DHCPv6 server not responding, contact ALL servers */
      if (sys->Flags & SYS_FLAG_SEC) {
        /* Count interval 1 second */
        if(--ctrl->timer == 0) {
          /* Address ValidTime also expired, restart client */
          DEBUGF (DHCP6,"State-REBIND %s timeout, Next State-INIT\n",h->If->Name);
          EvrNetDHCP6_ChangeStateOnTimeout (h->If->Id, DHCP6_STATE_REBIND, DHCP6_STATE_INIT);
          ctrl->State = DHCP6_STATE_INIT;
          break;
        }
      }
      if(--ctrl->tout1 != 0) {
        break;
      }
      DEBUGF (DHCP6,"State-REBIND %s, retransmit\n",h->If->Name);
      EvrNetDHCP6_StateRetransmit (h->If->Id, DHCP6_STATE_REBIND);
      dhcp6_send_message (h, DHCP6_MSG_REBIND);
      ctrl->tout1 = ctrl->tout2;
      ctrl->tout2 = get_tout (ctrl->tout2, 600*SYS_TICK_TSEC, false);
      break;

    case DHCP6_STATE_INFO:
      /* Request additional information, stateless mode */
      if (--ctrl->tout1 != 0) {
        break;
      }
      /* No response received, retransmit request */
      DEBUGF (DHCP6,"State-INFO %s, retransmit\n",h->If->Name);
      EvrNetDHCP6_StateRetransmit (h->If->Id, DHCP6_STATE_INFO);
      dhcp6_send_message (h, DHCP6_MSG_INFO_REQUEST);
      ctrl->tout1 = ctrl->tout2;
      ctrl->tout2 = get_tout (ctrl->tout2, 120*SYS_TICK_TSEC, false);
      break;
  }
}

/**
  \brief       Create a DHCPv6 message and send it to server.
  \param[in]   h         DHCPv6 instance handle.
  \param[in]   msg_type  DHCPv6 message type to create.
*/
static void dhcp6_send_message (NET_DHCP6_CFG *h, uint8_t msg_type) {
  uint8_t *sendbuf;
  uint32_t index,sz;
  NET_ADDR addr;

  DEBUGF (DHCP6,"Send %s, DHCP_%s\n",h->If->Name,msg_ascii(msg_type));
  DEBUG_INF2 (ctrl->Srv.IdVal, ctrl->Srv.IdLen);
  EvrNetDHCP6_SendDhcpMessage (h->If->Id, msg_type, ctrl->xid & 0xFFFFFF);

  sz = 100;
  if (h->OptVclass) {
    /* Add space for Vendor Class option */
    sz += 50;
  }
  sendbuf = net_udp_get_buf (sz);
  sendbuf[0] = msg_type;

  /* XID, transaction ID, a random number */
  sendbuf[1] = (ctrl->xid >> 16) & 0xFF;
  sendbuf[2] = (ctrl->xid >> 8)  & 0xFF;
  sendbuf[3] = (ctrl->xid >> 0)  & 0xFF;

  /* Elapsed time */
  set_u16 (&sendbuf[4], DHCP6_OPT_ELAPSED_TIME);
  set_u16 (&sendbuf[6], 2);
  set_u16 (&sendbuf[8], ctrl->ElapsTime);

  /* Client Identifier (CID) */
  set_u16 (&sendbuf[10], DHCP6_OPT_CLIENT_ID);
  set_u16 (&sendbuf[12], 14);
  /* DUID type: link-layer address + time */
  set_u16 (&sendbuf[14], 1);
  /* Hardware type: Ethernet */
  set_u16 (&sendbuf[16], 1);
  /* DUID time: (used precompiled) */
  set_u32 (&sendbuf[18], DHCP6_DUID_TIME);
  /* Link-layer address */
  net_mac_copy (&sendbuf[22], h->If->MacAddr);

  /* Next depends on message type */
  index = 28;

  /* Server Identifier */
  switch (msg_type) {
    case DHCP6_MSG_REQUEST:
    case DHCP6_MSG_RENEW:
    case DHCP6_MSG_DECLINE:
    case DHCP6_MSG_RELEASE:
      /* Must be added when the server is known */
      set_u16 (&sendbuf[index], DHCP6_OPT_SERVER_ID);
      set_u16 (&sendbuf[index+2], ctrl->Srv.IdLen);
      memcpy (&sendbuf[index+4], ctrl->Srv.IdVal, ctrl->Srv.IdLen);
      /* Max size 20 bytes */
      index += (ctrl->Srv.IdLen + 4);
      break;
  }

  /* Identity Association for Non-temporary Address */
  switch (msg_type) {
    case DHCP6_MSG_SOLICIT:
      set_u16 (&sendbuf[index], DHCP6_OPT_IA_NA);
      set_u16 (&sendbuf[index+2], 12);
      /* IAID: unique for interface */
      set_u32 (&sendbuf[index+4], DHCP6_IAID);
      /* T1 timeout */
      set_u32 (&sendbuf[index+8],  0);
      /* T2 timeout */
      set_u32 (&sendbuf[index+12], 0);
      index += 16;
      break;

    case DHCP6_MSG_REQUEST:
    case DHCP6_MSG_RENEW:
    case DHCP6_MSG_REBIND:
    case DHCP6_MSG_RELEASE:
    case DHCP6_MSG_DECLINE:
      set_u16 (&sendbuf[index], DHCP6_OPT_IA_NA);
      set_u16 (&sendbuf[index+2], 40);
      /* IAID: unique for interface */
      set_u32 (&sendbuf[index+4], DHCP6_IAID);
      /* T1 timeout */
      set_u32 (&sendbuf[index+8],  ctrl->IA.T1);
      /* T2 timeout */
      set_u32 (&sendbuf[index+12], ctrl->IA.T2);
      /* IA Address option */
      set_u16 (&sendbuf[index+16], DHCP6_OPT_IA_ADDR);
      set_u16 (&sendbuf[index+18], 24);
      net_addr6_copy (&sendbuf[index+20], ctrl->IA.Addr);
      set_u32 (&sendbuf[index+36], ctrl->IA.PrefTime);
      set_u32 (&sendbuf[index+40], ctrl->IA.ValidTime);
      index += 44;
      break;

    default:
      break;
  }

  /* Vendor Class option */
  if (h->OptVclass) {
    /* Added if enabled in the interface configuration */
    set_u16 (&sendbuf[index], DHCP6_OPT_VENDOR_CLASS);
    set_u32 (&sendbuf[index+4], h->EnterpId);
    sz = net_strcpy2 ((char *)&sendbuf[index+10], h->VcData);
    set_u16 (&sendbuf[index+8], sz);
    set_u16 (&sendbuf[index+2], sz + 6);
    index += (sz + 10);
  }

  /* Option Request option */
  set_u16 (&sendbuf[index], DHCP6_OPT_ORO);
  set_u16 (&sendbuf[index+2], 2);
  /* Code: DNS recursive name server */
  set_u16 (&sendbuf[index+4], DHCP6_OPT_DNS_SERVERS);
  index += 6;

  /* Send unicast message when server's IP is available */
  addr.addr_type = NET_ADDR_IP6;
  addr.port      = DHCP6_SERVER_PORT;
  net_addr6_copy (addr.addr, net_addr6_all_dhcp_agents);
  net_udp_send_if (h->If, dhcp6_socket, (__ADDR *)&addr, sendbuf, index);
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
static uint32_t dhcp6_listener (int32_t socket, const NET_ADDR *addr,
                                                const uint8_t *buf, uint32_t len) {
  NET_DHCP6_CFG *h;
  uint32_t index,tlen,xid;
  uint16_t stat;

  /* Check if this is our socket and version IPv6 */
  if ((dhcp6_socket != socket) || (addr->addr_type != NET_ADDR_IP6)) {
    return (false);
  }
  /* Get DHCPv6 instance handle */
  h = (__ADDR_X(addr))->net_if->Ip6Cfg->Dhcp6Cfg;
  
  switch (ctrl->State) {
    case DHCP6_STATE_BOUND:
    case DHCP6_STATE_IDLE:
      /* Discard all messages in BOUND or IDLE state */
      return (false);
  }
  DEBUGF (DHCP6,"*** Process_frame %s ***\n",h->If->Name);
  DEBUGF (DHCP6," Length %d bytes\n",len);
  EvrNetDHCP6_ReceiveFrame (h->If->Id, len);

  /* Server might use a random source port number, so  */
  /* do not check if (addr->port != DHCP6_SERVER_PORT) */

  /* Initial correctness checking */
  if (len < 8) {
    ERRORF (DHCP6,"Process %s, Frame too short\n",h->If->Name);
    EvrNetDHCP6_FrameTooShort (h->If->Id, len, 8);
    return (false);
  }
  /* Check XID (only 3 bytes) */
  xid = get_u32 (&buf[0]) & 0xFFFFFF;
  if (xid != (ctrl->xid & 0xFFFFFF)) {
    ERRORF (DHCP6,"Process %s, Wrong Transaction ID\n",h->If->Name);
    EvrNetDHCP6_WrongTransactionId (h->If->Id, xid, ctrl->xid & 0xFFFFFF);
    return (false);
  }
  /* Check Message type */
  if ((buf[0] < DHCP6_MSG_SOLICIT) || (buf[0] > DHCP6_MSG_RELAY_REPLY)) {
    ERRORF (DHCP6,"Process %s, Unknown MsgType (%d)\n",h->If->Name,buf[0]);
    EvrNetDHCP6_MessageTypeUnknown (h->If->Id, buf[0]);
    return (false);
  }
  DEBUGF (DHCP6," MsgType DHCP_%s\n",msg_ascii(buf[0]));
  EvrNetDHCP6_ViewMessage (h->If->Id, buf[0], xid);

  /* Process received message, depends on state we're in */
  switch (ctrl->State) {
    case DHCP6_STATE_DISCOVER:
      /* Waiting for ADVERTISE message from DHCPv6 servers */
      /* We are waiting for responses until first timeout  */
      /* The best Server with highest Preference is then   */
      /* selected.                                         */
      DEBUGF (DHCP6," Client State-DISCOVER\n");
      EvrNetDHCP6_ClientState (h->If->Id, DHCP6_STATE_DISCOVER);
      if (buf[0] != DHCP6_MSG_ADVERTISE) {
        /* Discard other messages */
        DEBUGF (DHCP6," Discarded, Invalid MsgType\n");
        EvrNetDHCP6_InvalidMessageType (h->If->Id, buf[0], DHCP6_MSG_ADVERTISE);
        break;
      }
      /* Check Client Identifier */
      if (!check_client_id (h, &buf[4], len - 4)) {
        DEBUGF (DHCP6," Discarded, Wrong Client ID\n");
        EvrNetDHCP6_WrongClientId (h->If->Id, DHCP6_MSG_ADVERTISE);
        break;
      }
      /* Select best server, save Server Identifier */
      if (!select_server (h, &buf[4], len - 4)) {
        DEBUGF (DHCP6," Discarded, Server not chosen\n");
        EvrNetDHCP6_ServerNotChosen (h->If->Id, DHCP6_MSG_ADVERTISE);
        break;
      }

      /* Process DHCPv6 options */
      ctrl->Flags &= ~(DHCP6_FLAG_OPT_VALID | DHCP6_FLAG_STATUS_FAIL);
      for (index = 4; index < len;  ) {
        switch (get_u16 (&buf[index])) {
          case DHCP6_OPT_IA_NA:
            /* IA for Non-temporary address */
            tlen = get_u16 (&buf[index+2]);
            if ((index + 4 + tlen) <= len) {
              proc_opt_ia_na (h, &buf[index+4], tlen);
            }
            break;
          case DHCP6_OPT_STATUS_CODE:
            /* Status code option */
            stat = get_u16(&buf[index+4]);
            if (stat == DHCP6_STATUS_NoAddrAvail) {
              ctrl->Flags |= DHCP6_FLAG_NO_ADDR;
            }
            if (stat != DHCP6_STATUS_Success) {
              ctrl->Flags |= DHCP6_FLAG_STATUS_FAIL;
            }
            DEBUGF (DHCP6," Status code %s\n",stat_ascii(stat));
            EvrNetDHCP6_StatusCode (h->If->Id, stat);
            break;
        }
        index += (get_u16 (&buf[index+2]) + 4);
      }
      /* Got what we were looking for? */
      if (((ctrl->Flags & DHCP6_FLAG_OPT_VALID)   == 0) ||
          ((ctrl->Flags & DHCP6_FLAG_STATUS_FAIL) != 0)) {
        /* Selected server has invalid IA_NA option, or returned status   */
        /* code is not SUCCESS, find another server or retransmit request.*/
        ctrl->Srv.IdLen = 0;
        break;
      }
      if (ctrl->tout1 > 2*SYS_TICK_TSEC) {
        /* Wait max. 2 secs after valid response received */
        ctrl->tout1 = 2*SYS_TICK_TSEC;
      }
      break;

    case DHCP6_STATE_REQUEST:
      /* Waiting for REPLY message from server */
      DEBUGF (DHCP6," Client State-REQUEST\n");
      EvrNetDHCP6_ClientState (h->If->Id, DHCP6_STATE_REQUEST);
req:  if (buf[0] != DHCP6_MSG_REPLY) {
        /* Discard other messages */
        DEBUGF (DHCP6," Discarded, Invalid MsgType\n");
        EvrNetDHCP6_InvalidMessageType (h->If->Id, buf[0], DHCP6_MSG_REPLY);
        break;
      }
      /* Check Client Identifier */
      if (!check_client_id (h, &buf[4], len - 4)) {
        DEBUGF (DHCP6," Discarded, Wrong Client ID\n");
        EvrNetDHCP6_WrongClientId (h->If->Id, DHCP6_MSG_REPLY);
        break;
      }
      /* Check Server Identifier */
      if (!check_server_id (h, &buf[4], len - 4)) {
        DEBUGF (DHCP6," Discarded, Wrong Server ID\n");
        EvrNetDHCP6_WrongServerId (h->If->Id, DHCP6_MSG_REPLY);
        break;
      }
      /* Process DHCPv6 options */
opt:  ctrl->Flags &= ~(DHCP6_FLAG_OPT_VALID | DHCP6_FLAG_STATUS_FAIL);
      for (index = 4; index < len; ) {
        switch (get_u16 (&buf[index])) {
          case DHCP6_OPT_IA_NA:
            /* IA for Non-temporary address */
            tlen = get_u16 (&buf[index+2]);
            if ((index + 4 + tlen) <= len) {
              /* Update recorded information about IA */
              proc_opt_ia_na (h, &buf[index+4], tlen);
            }
            break;
          case DHCP6_OPT_DNS_SERVERS:
            /* DNS name servers */
            tlen = get_u16 (&buf[index+2]);
            if (tlen < 16) {
              /* Option invalid, too short */
              break;
            }
            /* Primary and Secondary DNS server */
            net_addr6_copy (LocM6.PriDNS, &buf[index+4]);
            net_addr6_copy (LocM6.SecDNS, tlen < 32 ? net_addr_unspec : &buf[index+20]);
            DEBUGF (DHCP6," PriDNS [%s]\n",net_addr6_ntoa(LocM6.PriDNS));
            DEBUGF (DHCP6," SecDNS [%s]\n",net_addr6_ntoa(LocM6.SecDNS));
            EvrNetDHCP6_ViewDnsServers (h->If->Id, LocM6.PriDNS);
            break;
          case DHCP6_OPT_STATUS_CODE:
            /* Status code option */
            stat = get_u16(&buf[index+4]);
            if (stat != DHCP6_STATUS_Success) {
              ctrl->Flags |= DHCP6_FLAG_STATUS_FAIL;
            }
            DEBUGF (DHCP6," Status code %s\n",stat_ascii(stat));
            EvrNetDHCP6_StatusCode (h->If->Id, stat);
            break;
        }
        index += (get_u16 (&buf[index+2]) + 4);
      }
      /* Got what we were looking for? */
      if (((ctrl->Flags & DHCP6_FLAG_OPT_VALID)   == 0) ||
          ((ctrl->Flags & DHCP6_FLAG_STATUS_FAIL) != 0)) {
        break;
      }
      /* Yes, set IP address and renew timer */
      net_addr6_copy (LocM6.IpAddr, ctrl->IA.Addr);
      netDHCP6_Notify (h->If->Id,
                       NET_DHCP6_OPTION_IP_ADDRESS, LocM6.IpAddr, NET_ADDR_IP6_LEN);
      ctrl->State = DHCP6_STATE_BOUND;
      ctrl->timer = ctrl->IA.T1;
      DEBUGF (DHCP6," Next State-BOUND\n");
      EvrNetDHCP6_NextState (h->If->Id, DHCP6_STATE_BOUND);
      /* Multicast addresses changed */
      h->If->State->ConfigMcast = true;
      break;

    case DHCP6_STATE_RENEW:
      /* Lease Timer expired, Renew leased IP address */
      DEBUGF (DHCP6," Client State-RENEW\n");
      EvrNetDHCP6_ClientState (h->If->Id, DHCP6_STATE_RENEW);
      goto req;

    case DHCP6_STATE_REBIND:
      /* Current DHCPv6 server not responding, contact ALL servers */
      DEBUGF (DHCP6," Client State-REBIND\n");
      EvrNetDHCP6_ClientState (h->If->Id, DHCP6_STATE_REBIND);
      if (buf[0] != DHCP6_MSG_REPLY) {
        /* Discard other messages */
        DEBUGF (DHCP6," Discarded, Invalid MsgType\n");
        EvrNetDHCP6_InvalidMessageType (h->If->Id, buf[0], DHCP6_MSG_REPLY);
        break;
      }
      /* Check Client Identifier */
      if (!check_client_id (h, &buf[4], len - 4)) {
        DEBUGF (DHCP6," Discarded, Wrong Client ID\n");
        EvrNetDHCP6_WrongClientId (h->If->Id, DHCP6_MSG_REPLY);
        break;
      }
      /* Select best server, save Server Identifier */
      if (!select_server (h, &buf[4], len - 4)) {
        DEBUGF (DHCP6," Discarded, Server not chosen\n");
        EvrNetDHCP6_ServerNotChosen (h->If->Id, DHCP6_MSG_REPLY);
        break;
      }
      goto opt;

    case DHCP6_STATE_INFO:
      /* Request additional information, stateless mode */
      DEBUGF (DHCP6," Client State-INFO\n");
      EvrNetDHCP6_ClientState (h->If->Id, DHCP6_STATE_INFO);
      if (buf[0] != DHCP6_MSG_REPLY) {
        /* Discard other messages */
        DEBUGF (DHCP6," Discarded, Invalid MsgType\n");
        EvrNetDHCP6_InvalidMessageType (h->If->Id, buf[0], DHCP6_MSG_REPLY);
        break;
      }
      /* Check Client Identifier */
      if (!check_client_id (h, &buf[4], len - 4)) {
        DEBUGF (DHCP6," Discarded, Wrong Client ID\n");
        EvrNetDHCP6_WrongClientId (h->If->Id, DHCP6_MSG_REPLY);
        break;
      }
      /* Choose best server, save Server Identifier */
      if (!select_server (h, &buf[4], len - 4)) {
        DEBUGF (DHCP6," Discarded, Missing Server ID\n");
        EvrNetDHCP6_MissingServerId (h->If->Id, DHCP6_MSG_REPLY);
        break;
      }
      /* Process DHCPv6 options */
      ctrl->Flags &= ~(DHCP6_FLAG_OPT_VALID | DHCP6_FLAG_STATUS_FAIL);
      for (index = 4; index < len; ) {
        switch (get_u16 (&buf[index])) {
          case DHCP6_OPT_DNS_SERVERS:
            /* DNS name servers */
            tlen = get_u16 (&buf[index+2]);
            if (tlen < 16) {
              /* Option invalid, too short */
              break;
            }
            /* Primary and Secondary DNS server */
            net_addr6_copy (LocM6.PriDNS, &buf[index+4]);
            net_addr6_copy (LocM6.SecDNS, tlen < 32 ? net_addr_unspec : &buf[index+20]);
            DEBUGF (DHCP6," PriDNS [%s]\n",net_addr6_ntoa(LocM6.PriDNS));
            DEBUGF (DHCP6," SecDNS [%s]\n",net_addr6_ntoa(LocM6.SecDNS));
            EvrNetDHCP6_ViewDnsServers (h->If->Id, LocM6.PriDNS);
            /* Done, we got a DNS server address */
            ctrl->Flags |= DHCP6_FLAG_OPT_VALID;
            break;
          case DHCP6_OPT_STATUS_CODE:
            /* Status code option */
            stat = get_u16(&buf[index+4]);
            if (stat != DHCP6_STATUS_Success) {
              ctrl->Flags |= DHCP6_FLAG_STATUS_FAIL;
            }
            DEBUGF (DHCP6," Status code %s\n",stat_ascii(stat));
            EvrNetDHCP6_StatusCode (h->If->Id, stat);
            break;
        }
        index += (get_u16 (&buf[index+2]) + 4);
      }
      if (((ctrl->Flags & DHCP6_FLAG_OPT_VALID)   == 0) ||
          ((ctrl->Flags & DHCP6_FLAG_STATUS_FAIL) != 0)) {
        break;
      }
      ctrl->State = DHCP6_STATE_IDLE;
      DEBUGF (DHCP6," Next State-IDLE\n");
      EvrNetDHCP6_NextState (h->If->Id, DHCP6_STATE_IDLE);
      break;

    default:
      /* Dump frames in all other states */
      break;
  }
  return (true);
}

/**
  \brief       Select best DHCP server and save server info.
  \param[in]   h    DHCPv6 instance handle.
  \param[in]   buf  buffer containing DHCPv6 options.
  \param[in]   len  length of options.
  \return      status:
               - true  = Server ID is ok, info recorded,
               - false = Server ID option is missing.
  \note Server selection is based on Server Preference option.
*/
static bool select_server (NET_DHCP6_CFG *h, const uint8_t *buf, uint32_t len) {
  uint32_t index,id_pos,pref,len2;

  id_pos = 0;
  pref   = 0;
  for (index = 0; index < len;  ) {
    switch (get_u16 (&buf[index])) {
      case DHCP6_OPT_SERVER_ID:
        /* Save position of Server ID option */
        id_pos = index + 2;
        break;
      case DHCP6_OPT_PREFERENCE:
        /* Server preference (pref=0 when option missing) */
        pref = buf[index+4];
        break;
    }
    index += (get_u16 (&buf[index+2]) + 4);
  }
  if (id_pos == 0) {
    /* Server ID option not found */
    return (false);
  }
  DEBUG_INF2 (&buf[id_pos+2], get_u16(&buf[id_pos]));
  /* Check if this server has higher preference */
  if ((pref > ctrl->Srv.Pref) || (ctrl->Srv.IdLen == 0)) {
    ctrl->Srv.Pref = pref & 0xFF;
    /* Save Server ID for later use */
    len2 = get_u16 (&buf[id_pos]);
    if (len2 > sizeof (ctrl->Srv.IdVal)) {
      len2 = sizeof (ctrl->Srv.IdVal);
    }
    ctrl->Srv.IdLen = len2 & 0xFF;
    memcpy (ctrl->Srv.IdVal, &buf[id_pos+2], len2);
    /* This server is better than previous */
    return (true);
  }
  /* This server is not chosen */
  return (false);
}

/**
  \brief       Check Server Identifier from DHCPv6 frame.
  \param[in]   h    DHCPv6 instance handle.
  \param[in]   buf  buffer containing DHCPv6 options.
  \param[in]   len  length of options.
  \return      status:
               - true  = Server ID is ok,
               - false = Server ID is wrong or missing.
*/
static bool check_server_id (NET_DHCP6_CFG *h, const uint8_t *buf, uint32_t len) {
  uint32_t index,len2;

  for (index = 0; index < len;  ) {
    if (get_u16 (&buf[index]) == DHCP6_OPT_SERVER_ID) {
      /* Option found, check if Server ID the same */
      len2 = get_u16 (&buf[index+2]);
      if ((len2 == ctrl->Srv.IdLen)                        &&
          (memcmp (&buf[index+4], ctrl->Srv.IdVal, len2) == 0)) {
        /* Server ID is correct */
        return (true);
      }
      return (false);
    }
    index += (get_u16 (&buf[index+2]) + 4);
  }
  /* Server ID option is missing */
  return (false);
}

/**
  \brief       Check if Client Identifier is ours.
  \param[in]   h    DHCPv6 instance handle.
  \param[in]   buf  buffer containing DHCPv6 options.
  \param[in]   len  length of options.
  \return      status:
               - true  = Client ID is ok,
               - false = Client ID is wrong or missing.
*/
static bool check_client_id (NET_DHCP6_CFG *h, const uint8_t *buf, uint32_t len) {
  uint32_t index;

  for (index = 0; index < len;  ) {
    if (get_u16 (&buf[index]) == DHCP6_OPT_CLIENT_ID) {
      /* Option found, check Client ID content */
      if ((get_u16 (&buf[index+2]) == 14)               &&
          (get_u16 (&buf[index+4]) == 1)                &&
          (get_u16 (&buf[index+6]) == 1)                &&
          (get_u32 (&buf[index+8]) == DHCP6_DUID_TIME)  &&
           net_mac_comp (&buf[index+12], h->If->MacAddr)) {
        /* Client ID is ours */
        return (true);
      }
      /* Wrong Client ID, not ours */
      return (false);
    }
    index += (get_u16 (&buf[index+2]) + 4);
  }
  /* Client ID option is missing */
  return (false);
}

/**
  \brief       Process DHCP option IA_NA.
  \param[in]   h    DHCPv6 instance handle.
  \param[in]   buf  buffer containing the option.
  \param[in]   len  length of option.
*/
static void proc_opt_ia_na (NET_DHCP6_CFG *h, const uint8_t *buf, uint32_t len) {
  uint32_t index;

  if (len < 40) {
    /* Option too short */
    return;
  }
  if (get_u32 (&buf[0]) != DHCP6_IAID) {
    /* Wrong IAID, not ours */
    DEBUGF (DHCP6," Discarded, Wrong IAID\n");
    EvrNetDHCP6_WrongIaid (h->If->Id, get_u32(&buf[0]), DHCP6_IAID);
    return;
  }
  for (index = 12; index < len;  ) {
    /* Find IA Address option */
    if (get_u16 (&buf[index]) == DHCP6_OPT_IA_ADDR) {
      if (get_u16 (&buf[index+2]) < 24) {
        /* Option too short, ignore */
        break;
      }
      if ((ctrl->State == DHCP6_STATE_REQUEST)        &&
          !net_addr6_comp (ctrl->IA.Addr, &buf[index+4])) {
        /* We did not request this IP Address */
        break;
      }
      /* Save or update IA address information */
      net_addr6_copy (ctrl->IA.Addr, &buf[index+4]);
      ctrl->IA.PrefTime  = get_u32 (&buf[index+20]);
      ctrl->IA.ValidTime = get_u32 (&buf[index+24]);
      ctrl->IA.T1        = get_u32 (&buf[4]);
      ctrl->IA.T2        = get_u32 (&buf[8]);

      DEBUGF (DHCP6," Offered Addr [%s]\n",net_addr6_ntoa(ctrl->IA.Addr));
      DEBUGF (DHCP6," + Lifetime Valid=%d, Prefrd=%d\n",
                        ctrl->IA.PrefTime,ctrl->IA.ValidTime);
      DEBUGF (DHCP6," + Timeout T1=%d, T2=%d\n",ctrl->IA.T1,ctrl->IA.T2);
      EvrNetDHCP6_ViewIanaOffer (&ctrl->IA);

      /* Check Address timers:                   */
      /* 1. Valid lifetime >= Preferred lifetime */
      /* 2. Valid lifetime >  Rebind timeout     */
      /* 3. Rebind timeout >  Renew timeout      */
      if ((ctrl->IA.ValidTime >= ctrl->IA.PrefTime) &&
          (ctrl->IA.ValidTime >  ctrl->IA.T2)       &&
          (ctrl->IA.T2        >  ctrl->IA.T1)) {
        /* IA_NA option is valid */
        ctrl->Flags |= DHCP6_FLAG_OPT_VALID;
      }
      else {
        /* A client discards any address for which the preferred lifetime */
        /* is greater than the valid lifetime.        [RFC 3315 page 77]  */
        ERRORF (DHCP6,"Offer %s, Timer check failed\n",h->If->Name);
        EvrNetDHCP6_OfferTimerCheckFailed (h->If->Id);
        break;
      }
      if (get_u16 (&buf[index+2]) < 30) {
        /* Too short for Status-code option, assume SUCCESS */
        break;
      }
      if (get_u16 (&buf[index+28]) == DHCP6_OPT_STATUS_CODE) {
        /* Status code option found, check code value */
        uint16_t stat = get_u16(&buf[index+32]);
        if (stat != DHCP6_STATUS_Success) {
          ctrl->Flags |= DHCP6_FLAG_STATUS_FAIL;
        }
        DEBUGF (DHCP6," + Status %s\n",stat_ascii(stat));
        EvrNetDHCP6_StatusCode (h->If->Id, stat);
      }
      break;
    }
    index += (get_u16 (&buf[index+2]) + 4);
  }
}

/**
  \brief       Calculate retransmission timeout.
  \param[in]   prev_tout  previous or initial timeout.
  \param[in]   max_tout   maximum retransmission timeout.
  \bool        first      - true  = first retransmission,
                          - false = subsequent retransmission.
  \return      timeout in number of of 100ms ticks.
*/
static uint16_t get_tout (uint32_t prev_tout, uint32_t max_tout, bool first) {
  uint32_t tout;

  /* Timeout calculation (RFC 3315 - page 26) */
  if (first) {
    /* Use strictly positive RAND if max_tout != 0 */
    uint32_t val = (max_tout) ? 32768 : 29492;
    /* tout = prev_tout + RAND * prev_tout */
    tout = (prev_tout * (val + net_rand_max(6554))) / 32768;
  }
  else {
    /* tout = 2*prev_tout + RAND*prev_tout */
    tout = (prev_tout * (31130 + net_rand_max(3278))) / 16384;
    if (max_tout && tout > max_tout) {
      /* tout = max_tout + RAND*max_tout */
      tout = (max_tout * (29492 + net_rand_max(6554))) / 32768;
    }
  }
  return (tout & 0xFFFF);
}

#ifdef Network_Debug_STDIO
/**
  \brief       Convert DHCPv6 message type to ascii.
  \param[in]   msg_type  DHCPv6 message type.
  \return      pointer to the text representation of a message type.
*/
static const char *msg_ascii (uint8_t msg_type) {
  static const char *const m_asc[13] = {
    "SOLICIT",
    "ADVERTISE",
    "REQUEST",
    "CONFIRM",
    "RENEW",
    "REBIND",
    "REPLY",
    "RELEASE",
    "DECLINE",
    "RECONFIG",
    "INFO-REQUEST",
    "RELAY-FORWARD",
    "RELAY-REPLY"
  };
  return (m_asc[msg_type-1]);
}

/**
  \brief       Convert Status-code to ascii.
  \param[in]   status  Option Status-code.
  \return      pointer to the text representation of a status code.
*/
static const char *stat_ascii (uint16_t status) {
  static const char *const stat_asc[7] = {
    "SUCCESS",
    "UNSPEC-FAIL",
    "NO-ADDRS-AVAIL",
    "NO-BINDING",
    "NOT-ON-LINK",
    "USE-MULTICAST",
    "UNDEFINED"
  };

  if (status > 6) status = 6;
  return (stat_asc[status]);
}

/**
  \brief       Debug print DUID value in hex notation.
  \param[in]   val  DUID array to print;
  \param[in]   len  length of DUID array.
*/
static void debug_inf2 (const uint8_t *val, uint32_t len) {
  static char dbg_buf[32];

  if (len > 15) len = 15;
  net_bin2hex (dbg_buf, val, len);
  DEBUGF (DHCP6," ServerID <%s>\n", len ? dbg_buf : "ALL agents");
}
#endif

/* Library default function(s) */
__WEAK void netDHCP6_Notify (uint32_t if_id, uint8_t option,
                             const uint8_t *val, uint32_t len) {
  (void)if_id; (void)option; (void)val; (void)len; }
#endif /* Network_IPv6 */
