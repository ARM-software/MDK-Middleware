/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_dhcp_client.c
 * Purpose: Dynamic Host Configuration Client
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_mem.h"
#include "net_arp.h"
#include "net_ip4.h"
#include "net_udp.h"
#include "net_dhcp_client.h"
#include "net_dbg.h"

/* Global variables */
NET_DHCP_CTRL net_eth0_dhcp_control;
NET_DHCP_CTRL net_eth1_dhcp_control;
NET_DHCP_CTRL net_wifi0_dhcp_control;
NET_DHCP_CTRL net_wifi1_dhcp_control;

/* Code shortening macros */
#define ctrl        ( h->Ctrl)
#define LocM        (*h->If->localm)

/* Local variables */
static uint8_t dhcp_socket;
static uint8_t num_active;

/* Local functions */
static void dhcp_client_start (NET_DHCP_CFG *h);
static void dhcp_client_stop (NET_DHCP_CFG *h);
static void dhcp_client_run (NET_DHCP_CFG *h);
static void dhcp_send_message (NET_DHCP_CFG *h, uint8_t msg_type);
static uint32_t dhcp_listener (int32_t socket, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len);
static bool is_selected (NET_DHCP_CFG *h, const uint8_t *ip_addr);
static uint32_t proc_opt_sel (NET_DHCP_CFG *h, const uint8_t *buf, uint32_t index);
static uint32_t proc_opt_req (NET_DHCP_CFG *h, const uint8_t *buf, uint32_t index);
static uint32_t proc_opt_renew (NET_DHCP_CFG *h, const uint8_t *buf, uint32_t index);
static void assign_auto_ip (NET_DHCP_CFG *h);
#ifdef Network_Debug_STDIO
 static void debug_info (const uint8_t *opt);
 static void debug_inf2 (const char *msg, const uint8_t *ip4_addr);
#endif

/**
  \brief       Initialize DHCP clients.
*/
void net_dhcp_client_init (void) {
  NET_DHCP_CFG *const *p;
  int32_t sock;

  for (p = &net_dhcp_list[0]; *p; p++) {
    DEBUGF (DHCP,"Init_client %s\n",(*p)->If->Name);
    EvrNetDHCP_InitClient ((*p)->If->Id, (*p)->OptBootfile,
                                         (*p)->OptNtpServ, ((*p)->VcidLen>0));
#ifdef Network_Debug_STDIO
    if ((*p)->VcidLen) {
      DEBUGF (DHCP," Opt.60 Vcid [%s]\n",(*p)->Vcid);
    }
    if ((*p)->OptBootfile) {
      DEBUGF (DHCP," Opt.67 Bootfile name\n");
    }
    if ((*p)->OptNtpServ) {
      DEBUGF (DHCP," Opt.42 NTP servers\n");
    }
#endif
    memset ((*p)->Ctrl, 0, sizeof (*(*p)->Ctrl));
  }
  /* One socket for all DHCP clients */
  sock = net_udp_get_socket (dhcp_listener);
  if (sock < 0) {
    ERRORF (DHCP,"Get_socket failed\n");
    EvrNetDHCP_GetSocketFailed ();
    net_sys_error (NET_ERROR_UDP_ALLOC);
    return;
  }
  dhcp_socket = sock & 0xFF;
  num_active  = 0;
  if (!sysc->AutoStart) {
    return;
  }
  for (p = &net_dhcp_list[0]; *p; p++) {
    dhcp_client_start (*p);
  }
}

/**
  \brief       De-initialize DHCP client.
*/
void net_dhcp_client_uninit (void) {
  NET_DHCP_CFG *const *p;

  for (p = &net_dhcp_list[0]; *p; p++) {
    DEBUGF (DHCP,"Uninit_client %s\n",(*p)->If->Name);
    EvrNetDHCP_UninitClient ((*p)->If->Id);
    memset ((*p)->Ctrl, 0, sizeof (*(*p)->Ctrl));
  }
}

/**
  \brief       Enable dynamic host configuration.
  \param[in]   if_id  interface identifier.
  \return      status code as defined with netStatus.
*/
__WEAK netStatus netDHCP_Enable (uint32_t if_id) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_DHCP_CFG *h;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip4Cfg && net_if->Ip4Cfg->DhcpCfg)) {
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->DhcpCfg;

  if (dhcp_socket == 0) {
    RETURN (netError);
  }
  if (ctrl->State == DHCP_STATE_DISABLED) {
    /* Service is disabled */
    dhcp_client_start (h);
  }
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Disable dynamic host configuration.
  \param[in]   if_id  interface identifier.
  \return      status code as defined with netStatus.
*/
__WEAK netStatus netDHCP_Disable (uint32_t if_id) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_DHCP_CFG *h;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip4Cfg && net_if->Ip4Cfg->DhcpCfg)) {
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->DhcpCfg;

  if (ctrl->State != DHCP_STATE_DISABLED) {
    /* Service is running */
    dhcp_client_stop (h);
  }
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Set DHCP client option value.
  \param[in]   if_id   interface identifier.
  \param[in]   option  DHCP option code.
  \param[in]   val     pointer to option value.
  \param[in]   len     length of option value in bytes.
  \return      status code as defined with netStatus.
*/
__WEAK netStatus netDHCP_SetOption (uint32_t if_id, uint8_t option,
                                             const uint8_t *val, uint32_t len) {
  NET_IF_CFG *net_if = net_if_map_lan (if_id);
  NET_DHCP_CFG *h;
  static NET_DHCP_CLID clid_buf;

  START_LOCK (netStatus);

  if (!(net_if && net_if->Ip4Cfg && net_if->Ip4Cfg->DhcpCfg)  ||
      (option != NET_DHCP_OPTION_CLIENT_ID) || (len > DHCP_CLID_SIZE)) {
    ERRORF (DHCP,"SetOption, Invalid parameter\n");
    EvrNetDHCP_SetOptionInvalidParam (if_id & 0xFFFF);
    RETURN (netInvalidParameter);
  }
  h = net_if->Ip4Cfg->DhcpCfg;

  if (ctrl->State != DHCP_STATE_DISABLED) {
    /* Service is running */
    ERRORF (DHCP,"SetOption %s, Invalid state\n",h->If->Name);
    EvrNetDHCP_SetOptionInvalidState (h->If->Id, ctrl->State);
    RETURN (netWrongState);
  }
  if (val == NULL) {
    /* Disable custom Client-identifier */
    DEBUGF (DHCP,"SetOption %s, CLIENT_ID default\n",h->If->Name);
    EvrNetDHCP_SetOptionClientIdDefault (h->If->Id, 1);
    ctrl->Clid = NULL;
    RETURN (netOK);
  }
  /* Client-identifier option is not added in DHCP message if len < 2 */
  DEBUGF (DHCP,"SetOption %s, CLIENT_ID type=%d, len=%d\n",h->If->Name,(len)?val[0]:0,len);
  EvrNetDHCP_SetOptionClientId (h->If->Id, (len)?val[0]:0, len);
  ctrl->Clid = &clid_buf;
  clid_buf.Len = len & 0xFF;
  memcpy (&clid_buf.Val, val, len);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Start DHCP client service.
  \param[in]   h  DHCP instance handle.
*/
static void dhcp_client_start (NET_DHCP_CFG *h) {
  DEBUGF (DHCP,"Start_client %s\n",h->If->Name);
  EvrNetDHCP_StartClient (h->If->Id);
  if (num_active++ == 0) {
    net_udp_open (dhcp_socket, DHCP_CLIENT_PORT);
  }
  /* Enable reception of link-layer addressed frames */
  net_udp_enable_lla (dhcp_socket);
  /* Local IP address is unusable, not yet configured */
  net_addr4_copy (ctrl->ReqAddr, LocM.IpAddr);
  __ALIGNED_UINT32(LocM.IpAddr) = 0;
  ctrl->State    = DHCP_STATE_INIT;
  ctrl->xid      = net_rand32 ();
  ctrl->BootSecs = 0;
}

/**
  \brief       Stop DHCP client service.
  \param[in]   h  DHCP instance handle.
*/
static void dhcp_client_stop (NET_DHCP_CFG *h) {
  DEBUGF (DHCP,"Stop_client %s\n",h->If->Name);
  EvrNetDHCP_StopClient (h->If->Id);
  if (--num_active == 0) {
    net_udp_close (dhcp_socket);
  }
  /* Use static configuration */
  net_addr4_aton (h->If->Ip4Cfg->IpAddr, LocM.IpAddr);
  net_addr4_aton (h->If->Ip4Cfg->NetMask,LocM.NetMask);
  net_addr4_aton (h->If->Ip4Cfg->DefGW,  LocM.DefGW);
  net_addr4_aton (h->If->Ip4Cfg->PriDNS, LocM.PriDNS);
  net_addr4_aton (h->If->Ip4Cfg->SecDNS, LocM.SecDNS);
  ctrl->State = DHCP_STATE_DISABLED;
  netDHCP_Notify (h->If->Id,
                  NET_DHCP_OPTION_IP_ADDRESS, LocM.IpAddr, NET_ADDR_IP4_LEN);
  net_arp_notify (h->If);
}

/**
  \brief       Run DHCP client service.
*/
void net_dhcp_client_run (void) {
  NET_DHCP_CFG *const *p;

  for (p = &net_dhcp_list[0]; *p; p++) {
    dhcp_client_run (*p);
  }
}

/**
  \brief       Run DHCP client service.
  \param[in]   h  DHCP instance handle.
*/
static void dhcp_client_run (NET_DHCP_CFG *h) {
  switch (ctrl->State) {
    case DHCP_STATE_DISABLED:
      /* Do nothing, DHCP Client is disabled */
      return;

    case DHCP_STATE_INIT_REBOOT:
      /* Reboot DHCP Client and Request existing IP */
      DEBUGF (DHCP,"State-INIT_REBOOT %s\n",h->If->Name);
      EvrNetDHCP_ClientState (h->If->Id, DHCP_STATE_INIT_REBOOT);
      ctrl->State = DHCP_STATE_REBOOTING;
      ctrl->tout1 = 4;          /* next retransmission after 4 secs */
      ctrl->tout2 = 8;          /* after that after 8 secs retrans  */
      if (__ALIGNED_UINT32(LocM.IpAddr) != 0) {
        /* The first Reboot is when IP address is 0 */
        net_addr4_copy (ctrl->ReqAddr, LocM.IpAddr);
      }
      dhcp_send_message (h, DHCP_REQUEST);
      DEBUGF (DHCP," Next State-REBOOTING\n");
      EvrNetDHCP_NextState (h->If->Id, DHCP_STATE_REBOOTING);
      return;

    case DHCP_STATE_INIT:
      /* Switch to selecting, send DHCP_DISCOVER and init timeout timer */
      if (!h->If->State->LinkUp) {
        /* Link is down, do nothing */
        return;
      }
      if (!(sys->Flags & SYS_FLAG_TICK)) {
        /* A small delay after the link is up */
        return;
      }
      DEBUGF (DHCP,"State-INIT %s, Link up\n",h->If->Name);
      EvrNetDHCP_ClientState (h->If->Id, DHCP_STATE_INIT);
      ctrl->State = DHCP_STATE_SELECTING;
      ctrl->BootSecs = 0;
      /* xid, generate a random number */
      ctrl->xid = net_rand32 ();
      dhcp_send_message (h, DHCP_DISCOVER);
      /* T1 will be timeout timer. T2 will hold next timeout value. */
      ctrl->tout1 = 4;          /* next retransmission after 4 secs */
      ctrl->tout2 = 8;          /* next timeout after 4 secs expire */
      DEBUGF (DHCP," Next State-SELECTING\n");
      EvrNetDHCP_NextState (h->If->Id, DHCP_STATE_SELECTING);
      return;
  }

  if (!(sys->Flags & SYS_FLAG_SEC)) {
    return;
  }
  /* Check link state */
  if (!h->If->State->LinkUp) {
    /* Save current IP to be reused in next DHCP request */
    if (__ALIGNED_UINT32(LocM.IpAddr) != 0) {
      net_addr4_copy (ctrl->ReqAddr, LocM.IpAddr);
      __ALIGNED_UINT32(LocM.IpAddr) = 0;
      netDHCP_Notify (h->If->Id,
                      NET_DHCP_OPTION_IP_ADDRESS, LocM.IpAddr, NET_ADDR_IP4_LEN);
    }
    /* Restart DHCP client */
    ctrl->State = DHCP_STATE_INIT;
    return;
  }

  /* Executed with interval of 1 second */
  ctrl->BootSecs++;

  switch (ctrl->State) {
    case DHCP_STATE_REBOOTING:
      /* DHCP Client is rebooting, request has been sent */
      if (--ctrl->tout1 == 0) {
        if (ctrl->tout2 <= 32) {
          DEBUGF (DHCP,"State-REBOOTING %s, retransmit\n",h->If->Name);
          EvrNetDHCP_StateRetransmit (h->If->Id, DHCP_STATE_REBOOTING);
          ctrl->tout1 = ctrl->tout2;
          /* Exponential backoff retransmission */
          ctrl->tout2 <<= 1;
          dhcp_send_message (h, DHCP_REQUEST);
          break;
        }
        DEBUGF (DHCP,"State-REBOOTING %s timeout, next State-INIT\n",h->If->Name);
        EvrNetDHCP_ChangeStateOnTimeout (h->If->Id, DHCP_STATE_REBOOTING, DHCP_STATE_INIT);
        /* Restart the DHCP client, we don't want to wait more */
        /* than 30 secs for retransmissions.                   */
        ctrl->State = DHCP_STATE_INIT;
      }
      break;

    case DHCP_STATE_SELECTING:
      /* Waiting for DHCP_OFFER message from DHCP Server */
      if (--ctrl->tout1 == 0) {
        /* Timeout expired without receiving DHCP_OFFER? */
        /* Yes, retransmit or restart the process. */
        if (ctrl->tout2 <= 32) {
          DEBUGF (DHCP,"State-SELECTING %s, retransmit\n",h->If->Name);
          EvrNetDHCP_StateRetransmit (h->If->Id, DHCP_STATE_SELECTING);
          ctrl->tout1 = ctrl->tout2;
          /* exponential backoff retransmission */
          ctrl->tout2 <<= 1;
          dhcp_send_message (h, DHCP_DISCOVER);
          break;
        }
        /* Timeout, start Auto IP process */
        DEBUGF (DHCP,"State-SELECTING %s timeout, starting Auto-IP\n",h->If->Name);
        EvrNetDHCP_ChangeStateOnTimeout (h->If->Id, DHCP_STATE_SELECTING, DHCP_STATE_AUTO_IP);
        ctrl->AutoIp[0] = 0;
        ctrl->tout1 = 120 - 32;
        ctrl->State = DHCP_STATE_AUTO_IP;
      }
      break;

    case DHCP_STATE_AUTO_IP:
      /* Waiting for DHCP_OFFER message from DHCP Server */
      if (--ctrl->tout1 == 0) {
        /* Timeout expired, no DHCP server response. */
        /* Send DHCP_DISCOVER every 120 seconds.     */
        DEBUGF (DHCP,"State-AUTO_IP %s, retransmit\n",h->If->Name);
        EvrNetDHCP_StateRetransmit (h->If->Id, DHCP_STATE_AUTO_IP);
        ctrl->tout1 = 120;
        ctrl->xid   = net_rand32 ();
        dhcp_send_message (h, DHCP_DISCOVER);
      }
      if (ctrl->AutoIp[0] == 0) {
        /* ARP probe responded, this IP is used, select new IP */
        ctrl->tout2 = 3;
        assign_auto_ip (h);
        DEBUGF (DHCP,"Probe %s, AutoIP address\n",h->If->Name);
        EvrNetDHCP_AutoIpAddressProbe (h->If->Id, ctrl->AutoIp);
        net_arp_probe (h->If, ctrl->AutoIp);
        break;
      }
      if (ctrl->tout2 == 0) {
        break;
      }
      if (--ctrl->tout2) {
        /* Send 3 probes in 1 sec interval */
        DEBUGF (DHCP,"Probe %s, AutoIP address\n",h->If->Name);
        EvrNetDHCP_AutoIpAddressProbe (h->If->Id, ctrl->AutoIp);
        net_arp_probe (h->If, ctrl->AutoIp);
        break;
      }
      /* This IP is unused, no ARP response received */
      DEBUGF (DHCP,"State-AUTO_IP %s, success\n",h->If->Name);
      net_addr4_copy (LocM.IpAddr, ctrl->AutoIp);
      LocM.NetMask[0] = 255;
      LocM.NetMask[1] = 255;
      LocM.NetMask[2] = 0;
      LocM.NetMask[3] = 0;
      DEBUG_INF2 ("IP Address",LocM.IpAddr);
      DEBUG_INF2 ("Net mask",LocM.NetMask);
      EvrNetDHCP_AutoIpSuccess (h->If->Id, LocM.IpAddr);
      netDHCP_Notify (h->If->Id,
                      NET_DHCP_OPTION_IP_ADDRESS, LocM.IpAddr, NET_ADDR_IP4_LEN);
      net_arp_notify (h->If);
      break;

    case DHCP_STATE_REQUESTING:
      /* DHCP_REQUEST sent, waiting for proper response from DHCP Server */
      if(--ctrl->tout1 == 0) {
        /* Timeout occured without receiving DHCP_ACK */
        if (ctrl->tout2 < 32) {
          DEBUGF (DHCP,"State-REQUESTING %s, retransmit\n",h->If->Name);
          EvrNetDHCP_StateRetransmit (h->If->Id, DHCP_STATE_REQUESTING);
          ctrl->tout1 = ctrl->tout2;
          /* exponential backoff retransmission */
          ctrl->tout2 <<= 1;
          dhcp_send_message (h, DHCP_REQUEST);
          break;
        }
        DEBUGF (DHCP,"State-REQUESTING %s timeout, next State-INIT\n",h->If->Name);
        EvrNetDHCP_ChangeStateOnTimeout (h->If->Id, DHCP_STATE_REQUESTING, DHCP_STATE_INIT);
        ctrl->State = DHCP_STATE_INIT;
      }
      break;

    case DHCP_STATE_BOUND:
      /* IP address is valid, Wait for T1 to expire */
      ctrl->t2--;
      if(--ctrl->t1 == 0) {
        DEBUGF (DHCP,"State-BOUND %s, starting renew\n",h->If->Name);
        EvrNetDHCP_ClientState (h->If->Id, DHCP_STATE_BOUND);
        /* T1 expired, start renewing process */
        ctrl->State = DHCP_STATE_RENEWING;
        dhcp_send_message (h, DHCP_REQUEST);
        /* T1 will be used for retransmissions until we return */
        /* to BOUND state or reset to INIT state.              */
        ctrl->tout1 = 10;     /* fixed 10 sec retransmissions  */
        DEBUGF (DHCP," Next State-RENEWING\n");
        EvrNetDHCP_NextState (h->If->Id, DHCP_STATE_RENEWING);
      }
      break;

    case DHCP_STATE_RENEWING:
      /* Lease Timer expired, Renew leased IP address */
      if(--ctrl->t2 == 0) {
        DEBUGF (DHCP,"State-RENEWING %s, T2 expired\n",h->If->Name);
        EvrNetDHCP_T2Expired (h->If->Id, DHCP_STATE_RENEWING);
        /* T2 also expired. Switch to rebinding state. This */
        /* is our last attempt to retain this IP address.   */
        ctrl->State = DHCP_STATE_REBINDING;
        dhcp_send_message (h, DHCP_REQUEST);
        /* 't1' will be used for timeouts */
        ctrl->tout1 = 5;      /* 5 second retransmits */
        /* 't2' will be used for fixed numer of retries. This */
        /* is a bit different than per RFC, but we don't want */
        /* another 32-bit timer value for keeping lease time. */
        ctrl->tout2 = 10;
        DEBUGF (DHCP," Next State-REBINDING\n");
        EvrNetDHCP_NextState (h->If->Id, DHCP_STATE_REBINDING);
        break;
      }
      if (--ctrl->tout1 == 0) {
        DEBUGF (DHCP,"State-RENEWING %s, retransmit\n",h->If->Name);
        EvrNetDHCP_StateRetransmit (h->If->Id, DHCP_STATE_RENEWING);
        dhcp_send_message (h, DHCP_REQUEST);
        ctrl->tout1 = 10;
      }
      break;

    case DHCP_STATE_REBINDING:
      /* Current DHCP Host not responding, send a broadcast message */
      if (--ctrl->tout1 == 0) {
        ctrl->tout1 = 5;      /* 5 second retransmits */
        if(--ctrl->tout2 == 0) {
          /* Retransmit count. */
          ERRORF (DHCP,"State-REBINDING %s, Lease time expired\n",h->If->Name);
          EvrNetDHCP_LeaseExpired (h->If->Id, DHCP_STATE_REBINDING);
          /* All retransmissions used up. Assume that lease time */
          /* is expired by now. Restart the process.             */
          ctrl->State = DHCP_STATE_INIT;
          /* This leased IP address has expired, can't be used anymore */
          __ALIGNED_UINT32(LocM.IpAddr) = 0;
          netDHCP_Notify (h->If->Id,
                          NET_DHCP_OPTION_IP_ADDRESS, LocM.IpAddr, NET_ADDR_IP4_LEN);
          DEBUGF (DHCP," Next State-INIT\n");
          EvrNetDHCP_NextState (h->If->Id, DHCP_STATE_INIT);
          break;
        }
        DEBUGF (DHCP,"State-REBINDING %s, retransmit\n",h->If->Name);
        EvrNetDHCP_StateRetransmit (h->If->Id, DHCP_STATE_REBINDING);
        /* Still have some time */
        dhcp_send_message (h, DHCP_REQUEST);
      }
      break;
  }
}

/**
  \brief       Create a DHCP message and send it to server.
  \param[in]   h         DHCP instance handle.
  \param[in]   msg_type  DHCP message type to create.
*/
static void dhcp_send_message (NET_DHCP_CFG *h, uint8_t msg_type) {
  uint8_t *sendbuf;
  uint32_t index,sz,lm_ip;
  NET_ADDR4 addr;
  bool bcast = true;

  switch (ctrl->State) {
    case DHCP_STATE_BOUND:
    case DHCP_STATE_RENEWING:
      /* Do not send an IP bcast message in BOUND and RENEW states */
      bcast = false;
      break;
  }
#ifdef __DEBUG_ENABLED
  switch (msg_type) {
    case DHCP_DISCOVER:
      DEBUGF (DHCP," Sending %ccast DHCP_DISCOVER\n",bcast ? 'b' : 'u');
      EvrNetDHCP_SendDhcpMessage (h->If->Id, msg_type, bcast);
      break;
    case DHCP_REQUEST:
      DEBUGF (DHCP," Sending %ccast DHCP_REQUEST\n", bcast ? 'b' : 'u');
      EvrNetDHCP_SendDhcpMessage (h->If->Id, msg_type, bcast);
      break;
  }
#endif

  /* Calculate space required for a message */
  sz = 300;
  if (h->VcidLen) sz += h->VcidLen + 2;
  if (ctrl->Clid) sz += 19 - 7;

  sendbuf = net_udp_get_buf (sz);
  memset (sendbuf, 0, sz);

  /* Create DHCP message */
  sendbuf[0] = BOOTP_REQUEST;
  sendbuf[1] = HW_TYPE_ETH;
  sendbuf[2] = NET_ADDR_ETH_LEN;
  sendbuf[3] = 0x00;

  /* xid, transaction ID, a random number */
  set_u32 (&sendbuf[4], ctrl->xid);

  /* Seconds elapsed from boot */
  set_u16 (&sendbuf[8], ctrl->BootSecs);

  /* flags, use broadcast or unicast */
  set_u16 (&sendbuf[10], bcast ? BOOTP_FLAG_BCAST : BOOTP_FLAG_UCAST);

  /* ciaddr, sent only if client in BOUND, RENEW or REBINDING state */
  switch (ctrl->State) {
    case DHCP_STATE_BOUND:
    case DHCP_STATE_RENEWING:
    case DHCP_STATE_REBINDING:
      net_addr4_copy (&sendbuf[12], LocM.IpAddr);
      break;
  }
  /* yiaddr, siaddr, giaddr, skip and leave set to 0 */

  /* chadr, enter ethernet MAC address */
  net_mac_copy (&sendbuf[28], h->If->MacAddr);

  /* sname and file, skip */

  /* options field, first is Magic Cookie */
  set_u32 (&sendbuf[236], BOOTP_MAGIC_COOKIE);

  /* vendor-specific area 64 bytes*/
  index = 240;

  /* Enter DHCP Message type */
  sendbuf[index]   = DHCP_OPT_MSG_TYPE;
  sendbuf[index+1] = 1;
  sendbuf[index+2] = msg_type;
  index += 3;

  /* Enter DHCP Client-identifier */
  if (ctrl->Clid == NULL) {
    /* MAC address Client-identifier */
    sendbuf[index]   = DHCP_OPT_CLIENT_ID;
    sendbuf[index+1] = 7;
    sendbuf[index+2] = HW_TYPE_ETH;
    net_mac_copy (&sendbuf[index+3], h->If->MacAddr);
    index += 9;
  }
  else if (ctrl->Clid->Len >= 2) {
    /* User-provided Client-identifier */
    sendbuf[index]   = DHCP_OPT_CLIENT_ID;
    sendbuf[index+1] = ctrl->Clid->Len;
    /* 1st byte is hardware type */
    memcpy (&sendbuf[index+2], ctrl->Clid->Val, ctrl->Clid->Len);
    index += ctrl->Clid->Len + 2;    /* Max. 21 bytes */
  }

  /* Next depends on what state we're in and message type */
  switch (msg_type) {
    case DHCP_REQUEST:
      /* sent only from REQUESTING state */
    case DHCP_DECLINE:
      /* Requested IP address MUST NOT be sent in RENEWING */
      /* and REBINDING states (RFC2131 page 32)            */
      switch (ctrl->State) {
        case DHCP_STATE_RENEWING:
        case DHCP_STATE_REBINDING:
          break;
        default:
          sendbuf[index]  = DHCP_OPT_REQUESTED_IP;
          sendbuf[index+1]= 4;
          net_addr4_copy (&sendbuf[index+2], ctrl->ReqAddr);
          index += 6;
          break;
      }
      /* Server identifier MUST NOT be sent in INIT_REBOOT, */
      /* RENEWING and REBINDING states (RFC2131 page 31)    */
      switch (ctrl->State) {
        case DHCP_STATE_INIT_REBOOT:
        case DHCP_STATE_REBOOTING:
          /* for retransmissions */
        case DHCP_STATE_RENEWING:
        case DHCP_STATE_REBINDING:
          break;
        default:
          sendbuf[index]  = DHCP_OPT_SERVER_ID;
          sendbuf[index+1]= 4;
          net_addr4_copy (&sendbuf[index+2], ctrl->SrvAddr);
          index += 6;
          break;
      }
      /* Add parameter list of parameters we're interested in, */
      /* but not if we're sending a DECLINE message.           */
      if (msg_type == DHCP_DECLINE) {
        break;
      }
      __FALLTHROUGH;

    case DHCP_DISCOVER:
      /* Add host name */
      sendbuf[index]  = DHCP_OPT_HOST_NAME;
      sendbuf[index+1]= net_strcpy ((char *)&sendbuf[index+2],sys->HostName) & 0xFF;
      index += sendbuf[index+1] + 2;    /* Max 17 bytes */
      /* Add Vendor Class Identifier */
      if (h->VcidLen) {
        /* Added only if a string is defined in configuration */
        sendbuf[index]  = DHCP_OPT_VENDOR_CLASS;
        sendbuf[index+1]= h->VcidLen;
        net_strcpy ((char *)&sendbuf[index+2], h->Vcid);
        index += h->VcidLen + 2;
      }
      /* Parameter request list must also be transmitted */
      /* with a DHCP_REQUEST message.                    */
      sendbuf[index]  = DHCP_OPT_PARAM_REQUEST;
      sendbuf[index+2]= DHCP_OPT_SUBNET_MASK;
      sendbuf[index+3]= DHCP_OPT_ROUTER;
      sendbuf[index+4]= DHCP_OPT_DNS_SERVER;
      sendbuf[index+5]= DHCP_OPT_HOST_NAME;
      sendbuf[index+6]= DHCP_OPT_LEASE_TIME;
      sendbuf[index+7]= DHCP_OPT_T1_VALUE;
      sendbuf[index+8]= DHCP_OPT_T2_VALUE;
      sz = 7;
      if (h->OptBootfile) {
        /* Request also a Bootfile name */
        sendbuf[index+2+sz] = DHCP_OPT_BOOTFILE;
        sz++;
      }
      if (h->OptNtpServ) {
        /* Request also a list of NTP Servers */
        sendbuf[index+2+sz] = DHCP_OPT_NTP_SERVER;
        sz++;
      }
      /* Number of parameters we are requesting */
      sendbuf[index+1] = sz & 0xFF;
      index += sz + 2;
      break;
  }

  /* End Option */
  sendbuf[index] = DHCP_OPT_END;

  sz = index + 1;
  if (sz < 300) sz = 300;
 
  /* Send unicast message when server's IP is available */
  addr.addr_type = NET_ADDR_IP4;
  addr.port      = DHCP_SERVER_PORT;

  /* Save current IP address (AutoIP) */
  lm_ip = __ALIGNED_UINT32(LocM.IpAddr);
  if (bcast) {
    /* DHCP messages broadcast by a client prior to that client */
    /* obtaining its IP address must have the source address    */
    /* field in the IP header set to 0.       (RFC2131 page 22) */

    /* Clear local IP address only for this net_udp_send_if() */
    __ALIGNED_UINT32(LocM.IpAddr) = 0;
    net_addr4_copy (addr.addr, net_addr_bcast);
  }
  else {
    /* Send directly to DHCP server via unicast (RFC2131 page 40) */
    net_addr4_copy (addr.addr, ctrl->SrvAddr);
  }
  net_udp_send_if (h->If, dhcp_socket, (__ADDR *)&addr, sendbuf, sz);
  /* Restore current IP address */
  __ALIGNED_UINT32(LocM.IpAddr) = lm_ip;
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
static uint32_t dhcp_listener (int32_t socket, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len) {
  uint32_t index,overload,xid;
  NET_DHCP_CFG *h;

  /* Check if this is our socket and version IPv4 */
  if ((dhcp_socket != socket) || (addr->addr_type != NET_ADDR_IP4)) {
    return (false);
  }
  /* Get DHCP instance handle */
  h = (__ADDR_X(addr))->net_if->Ip4Cfg->DhcpCfg;

  if (ctrl->State == DHCP_STATE_BOUND) {
    return (false);
  }
  DEBUGF (DHCP,"*** Process_frame %s ***\n",h->If->Name);
  DEBUGF (DHCP," From %s, %d bytes\n",net_addr4_ntoa(addr->addr),len);
  EvrNetDHCP_ReceiveFrame (h->If->Id, addr->addr, len);

  if (addr->port != DHCP_SERVER_PORT) {
    ERRORF (DHCP,"Process %s, Wrong Server port\n",h->If->Name);
    EvrNetDHCP_WrongServerPort (h->If->Id, addr->port, DHCP_SERVER_PORT);
    return (false);
  }
  /* Initial correctness checking */
  if (len < 236) {
    ERRORF (DHCP,"Process %s, Frame too short\n",h->If->Name);
    EvrNetDHCP_FrameTooShort (h->If->Id, len, 236);
    return (false);
  }
  if (buf[0] != BOOTP_REPLY || buf[1] != HW_TYPE_ETH  || buf[2] != NET_ADDR_ETH_LEN) {
    ERRORF (DHCP,"Process %s, Misformed reply\n",h->If->Name);
    EvrNetDHCP_MisformedReply (h->If->Id);
    return (false);
  }
  /* Check xid */
  xid = get_u32 (&buf[4]);
  if (xid != ctrl->xid) {
    ERRORF (DHCP,"Process %s, Wrong XID\n",h->If->Name);
    EvrNetDHCP_WrongTransactionId (h->If->Id, xid, ctrl->xid);
    return (false);
  }
  /* Check chaddr */
  if (!net_mac_comp (&buf[28], h->If->MacAddr)) {
    ERRORF (DHCP,"Process %s, Wrong CHADDR\n",h->If->Name);
    EvrNetDHCP_WrongClientHwAddress (h->If->Id, &buf[28]);
    return (false);
  }

  /* Skip secs, flags, ciaddr, yiaddr, siaddr, giaddr, sname, file */

  /* Check the magic cookie */
  if (get_u32 (&buf[236]) != BOOTP_MAGIC_COOKIE) {
    ERRORF (DHCP,"Process %s, Wrong Magic Cookie\n",h->If->Name);
    EvrNetDHCP_WrongMagicCookie (h->If->Id, get_u32 (&buf[236]), BOOTP_MAGIC_COOKIE);
    return (false);
  }
  DEBUGF (DHCP," Initial check OK\n");

  /* Process further received message, depends on state we're in. */
  overload = 0;
  switch (ctrl->State) {
    case DHCP_STATE_SELECTING:
    case DHCP_STATE_AUTO_IP:
      /* Select first DHCP_OFFER */
      set_u32 (ctrl->SrvAddr, 0);
      set_u32 (ctrl->RelayAddr, 0);
      DEBUGF (DHCP," Client State-SELECTING\n");
      EvrNetDHCP_ClientState (h->If->Id, DHCP_STATE_SELECTING);
      /* Go through all parameters and try to find if this is DHCP_OFFER */
      /* Search for the SERVER_IDENTIFIER option (a server's IP address) */
      for (index = 240; index < len; ) {
        switch (buf[index]) {
          case DHCP_OPT_MSG_TYPE:
            /* Check message type */
            if (buf[index+2] != DHCP_OFFER) {
              DEBUGF (DHCP," Discarded, Not DHCP_OFFER message\n");
              EvrNetDHCP_InvalidMessageType (h->If->Id, buf[index+2], DHCP_OFFER);
              return (false);
            }
            DEBUGF (DHCP," Received DHCP_OFFER message\n");
            EvrNetDHCP_ViewMessage (h->If->Id, DHCP_OFFER);
            break;

          case DHCP_OPT_OVERLOAD:
            /* Overloaded message, read overload value */
            overload = buf[index+2];
            break;
        }
        index = proc_opt_sel (h, buf, index);
      }
      /* For overload message, 'sname' and 'file' fields hold options */
      if (overload & 0x01) {
        /* Process options overloaded to 'file' field */
        DEBUGF (DHCP," Processing Overload 'file' options\n");
        EvrNetDHCP_FileOverloadOptions (h->If->Id, DHCP_OFFER);
        for (index = 108; index < (108+128); ) {
          index = proc_opt_sel (h, buf, index);
        }
      }
      if (overload & 0x02) {
        /* Process options overloaded to 'sname' field */
        DEBUGF (DHCP," Processing Overload 'sname' options\n");
        EvrNetDHCP_SnameOverloadOptions (h->If->Id, DHCP_OFFER);
        for (index = 44; index < (44+64); ) {
          index = proc_opt_sel (h, buf, index);
        }
      }
      /* Processed all, got what we were looking for ? */
      if (get_u32 (ctrl->SrvAddr) == 0) {
        DEBUGF (DHCP," Discarded, DHCP_OFFER with no Server ID\n");
        EvrNetDHCP_MissingServerId (h->If->Id, DHCP_OFFER);
        return (false);
      }

      /* yiaddr, Offered IP address */
      net_addr4_copy (ctrl->ReqAddr, &buf[16]);
      /* Check if offered IP address is valid */
      /* The following addresses are not valid:           */
      /*   a) 224.0.0.0/4     - Multicast range (Class C) */
      /*   b) 240.0.0.0/4     - Reserved range  (Class D) */
      /*   c) 255.255.255.255 - Limited broadcast         */
      /*   d) 127.x.x.x       - Internal host loopback    */
      /*   e) 0.0.0.0         - Unspecified address       */
      if (((ctrl->ReqAddr[0] & 0xF0) >= 0xE0) ||
           (ctrl->ReqAddr[0]         == 0x7F) ||
           (get_u32 (ctrl->ReqAddr)  == 0))   {
        ERRORF (DHCP,"Process %s, Offered IP invalid\n",h->If->Name);
        EvrNetDHCP_OfferedAddressInvalid (h->If->Id, ctrl->ReqAddr);
        return (false);
      }
      DEBUG_INF2 ("Offered IP",ctrl->ReqAddr);
      EvrNetDHCP_ViewOfferedAddress (h->If->Id, ctrl->ReqAddr);

      /* Check if DHCP message is forwarded */
      if (!net_addr4_comp (ctrl->SrvAddr, addr->addr)) {
        DEBUGF (DHCP," Forwarded message\n");
        EvrNetDHCP_ForwardedMessage (h->If->Id, DHCP_OFFER);
        /* giaddr, Relay agent IP address */
        net_addr4_copy (ctrl->RelayAddr, &buf[24]);
        DEBUG_INF2 ("RelayAgent",ctrl->RelayAddr);
        EvrNetDHCP_ViewRelayAgentAddress (h->If->Id, ctrl->RelayAddr);
      }

      /* We have the offer, contact the server! */
      ctrl->State = DHCP_STATE_REQUESTING;
      dhcp_send_message (h, DHCP_REQUEST);
      ctrl->tout1 = 4;  /* 4 secs for first retransmission */
      ctrl->tout2 = 8;  /* next one after 8 secs           */
      DEBUGF (DHCP," Next State-REQUESTING\n");
      EvrNetDHCP_NextState (h->If->Id, DHCP_STATE_REQUESTING);
      break;

    case DHCP_STATE_REQUESTING:
      /* Wait for DHCP Acknowledge */
      DEBUGF (DHCP," Client State-REQUESTING\n");
      EvrNetDHCP_ClientState (h->If->Id, DHCP_STATE_REQUESTING);
      /* Check if this message is from selected server. */
      /* This will also reject subsequent DHCP_OFFERs   */
      /* from slow DHCP servers.                        */
      if (!is_selected (h, addr->addr)) {
        DEBUGF (DHCP," Discarded, Server not the same\n");
        EvrNetDHCP_ServerAddressNotSelected (h->If->Id, addr->addr);
        return (false);
      }
      /* Check if assigned IP == offered IP */
      if (!net_addr4_comp (ctrl->ReqAddr, &buf[16])) {
        DEBUGF (DHCP," Discarded, Requested IP not the same\n");
        EvrNetDHCP_AssignedAddrNotRequested (h->If->Id, &buf[16]);
        return (false);
      }
      /* Go through param list and process them. We expect to get */
      /* all the params here and that this is DHCP_ACK message.   */
      for (index = 240; index < len; ) {
        switch (buf[index]) {
          case DHCP_OPT_MSG_TYPE:
            /* If this is DHCP_ACK we can switch state and assume */
            /* that the parameters are OK.                        */
            if (buf[index+2] != DHCP_ACK) {
              DEBUGF (DHCP," Discarded, Not DHCP_ACK message\n");
              EvrNetDHCP_InvalidMessageType (h->If->Id, buf[index+2], DHCP_ACK);
              return (false);
            }
            DEBUGF (DHCP," Received DHCP_ACK message\n");
            EvrNetDHCP_ViewMessage (h->If->Id, DHCP_ACK);
            ctrl->State = DHCP_STATE_BOUND;
            /* We should actually check the received IP with ARP first, */
            /* but let's wait with that for now.....                    */
            net_addr4_copy (LocM.IpAddr, ctrl->ReqAddr);
            DEBUG_INF2 ("IP Address",LocM.IpAddr);
            EvrNetDHCP_ViewAssignedAddress (h->If->Id, LocM.IpAddr);
            /* 'ReqAddr' holds the assigned IP address, use it now. */
            break;

          case DHCP_OPT_OVERLOAD:
            /* Overloaded message, read overload value */
            overload = buf[index+2];
            break;
        }
        index = proc_opt_req (h, buf, index);
      }
      /* For overload message, 'sname' and 'file' fields hold options */
      if (overload & 0x01) {
        /* Process options overloaded to 'file' field. */
        DEBUGF (DHCP," Processing Overload 'file' options\n");
        EvrNetDHCP_FileOverloadOptions (h->If->Id, DHCP_ACK);
        for (index = 108; index < (108+128); ) {
          index = proc_opt_req (h, buf, index);
        }
      }
      if (overload & 0x02) {
        /* Process options overloaded to 'sname' field */
        DEBUGF (DHCP," Processing Overload 'sname' options\n");
        EvrNetDHCP_SnameOverloadOptions (h->If->Id, DHCP_ACK);
        for (index = 44; index < (44+64); ) {
          index = proc_opt_req (h, buf, index);
        }
      }
      DEBUGF (DHCP," Next State-BOUND\n");
      EvrNetDHCP_NextState (h->If->Id, DHCP_STATE_BOUND);
      netDHCP_Notify (h->If->Id,
                      NET_DHCP_OPTION_IP_ADDRESS, LocM.IpAddr, NET_ADDR_IP4_LEN);
      net_arp_notify (h->If);
      break;

    case DHCP_STATE_RENEWING:
      DEBUGF (DHCP," Client State-RENEWING\n");
      EvrNetDHCP_ClientState (h->If->Id, DHCP_STATE_RENEWING);
      goto ack;
    case DHCP_STATE_REBINDING:
      DEBUGF (DHCP," Client State-REBINDING\n");
      EvrNetDHCP_ClientState (h->If->Id, DHCP_STATE_REBINDING);
      goto ack;
    case DHCP_STATE_REBOOTING:
      /* Wait for DHCP_ACK. If we get DHCP_NAK restart. Here we're only */
      /* interested in ACK and T1, T2 and DNS Server parameters. Other  */
      /* parameters can change as well during time (gateway etc).       */
      DEBUGF (DHCP," Client State-REBOOTING\n");
      EvrNetDHCP_ClientState (h->If->Id, DHCP_STATE_REBOOTING);
ack:  if (!is_selected (h, addr->addr)) {
        DEBUGF (DHCP," Discarded, Server not the same\n");
        EvrNetDHCP_ServerAddressNotSelected (h->If->Id, addr->addr);
        return (false);
      }
      for (index = 240; index < len; ) {
        switch (buf[index]) {
          case DHCP_OPT_MSG_TYPE:
            if (buf[index+2] == DHCP_NAK) {
              DEBUGF (DHCP," Received DHCP_NAK message\n");
              DEBUGF (DHCP," Next State-INIT\n");
              EvrNetDHCP_ViewMessage (h->If->Id, DHCP_NAK);
              EvrNetDHCP_NextState (h->If->Id, DHCP_STATE_INIT);
              ctrl->State = DHCP_STATE_INIT;
              __ALIGNED_UINT32(LocM.IpAddr) = 0;
              return (false);
            }
            if (buf[index+2] != DHCP_ACK) {
              DEBUGF (DHCP," Discarded, Not DHCP_ACK message\n");
              EvrNetDHCP_InvalidMessageType (h->If->Id, buf[index+2], DHCP_ACK);
              return (false);
            }
            DEBUGF (DHCP," Received DHCP_ACK message\n");
            EvrNetDHCP_ViewMessage (h->If->Id, DHCP_ACK);
            ctrl->State = DHCP_STATE_BOUND;
            break;

          case DHCP_OPT_OVERLOAD:
            /* Overloaded message, read overload value */
            overload = buf[index+2];
            break;
        }
        index = proc_opt_renew (h, buf, index);
      }
      /* For overload message, 'sname' and 'file' fields hold options */
      if (overload & 0x01) {
        /* Process options overloaded to 'file' field */
        DEBUGF (DHCP," Processing Overload 'file' options\n");
        EvrNetDHCP_FileOverloadOptions (h->If->Id, DHCP_ACK);
        for (index = 108; index < (108+128); ) {
          index = proc_opt_renew (h, buf, index);
        }
      }
      if (overload & 0x02) {
        /* Process options overloaded to 'sname' field */
        DEBUGF (DHCP," Processing Overload 'sname' options\n");
        EvrNetDHCP_SnameOverloadOptions (h->If->Id, DHCP_ACK);
        for (index = 44; index < (44+64); ) {
          index = proc_opt_renew (h, buf, index);
        }
      }
      DEBUGF (DHCP," Next State-BOUND\n");
      EvrNetDHCP_NextState (h->If->Id, DHCP_STATE_BOUND);
      break;

    default:
      /* Dump frames in all other states */
      break;
  }
  return (true);
}

/**
  \brief       Check if IP address is selected.
  \param[in]   h        DHCP instance handle.
  \param[in]   ip_addr  remote IP address.
  \return      status:
               - true  = address is selected,
               - false = address is not selected.
  \note Accept messages only from selected DHCP server or Relay agent!
*/
static bool is_selected (NET_DHCP_CFG *h, const uint8_t *ip_addr) {
  /* Check DHCP server address */
  if (net_addr4_comp (ctrl->SrvAddr, ip_addr)) {
    return (true);
  }
  /* Check Relay agent address */
  if (get_u32 (ip_addr) == 0) {
    /* Unspecified, probably set by a firewall */
    return (true);
  }
  return (net_addr4_comp (ctrl->RelayAddr, ip_addr));
}

/**
  \brief       Process DHCP option in state SELECTING.
  \param[in]   h      DHCP instance handle.
  \param[in]   buf    buffer containing DHCP options.
  \param[in]   index  position of an option to process.
  \return      index of next option.
*/
static uint32_t proc_opt_sel (NET_DHCP_CFG *h, const uint8_t *buf, uint32_t index) {
  switch (buf[index]) {
    case DHCP_OPT_SERVER_ID:
      /* Read Server Identifier */
      net_addr4_copy (ctrl->SrvAddr, &buf[index+2]);
      DEBUG_INF2 ("Server ID",ctrl->SrvAddr);
      EvrNetDHCP_ViewServerId (h->If->Id, ctrl->SrvAddr);
      break;

    case DHCP_OPT_PAD:
      return (index + 1);

    case DHCP_OPT_END:
      /* End of options */
      return (0xFFFFFFFF);
  }
  /* Calculate index of next option */
  index += buf[index+1] + 2;
  return (index);
}

/**
  \brief       Process DHCP option in state REQUESTING.
  \param[in]   h      DHCP instance handle.
  \param[in]   buf    buffer containing DHCP options.
  \param[in]   index  position of an option to process.
  \return      index of next option.
*/
static uint32_t proc_opt_req (NET_DHCP_CFG *h, const uint8_t *buf, uint32_t index) {
  uint32_t tmp;

  switch (buf[index]) {
    case DHCP_OPT_SUBNET_MASK:
      net_addr4_copy (LocM.NetMask, &buf[index+2]);
      DEBUG_INF2 ("Net mask",LocM.NetMask);
      EvrNetDHCP_ViewNetMask (h->If->Id, LocM.NetMask);
      break;

    case DHCP_OPT_ROUTER:
      /* Read Default Gateway IP address */
      net_addr4_copy (LocM.DefGW, &buf[index+2]);
      DEBUG_INF2 ("Gateway",LocM.DefGW);
      EvrNetDHCP_ViewGatewayAddress (h->If->Id, LocM.DefGW);
      break;

    case DHCP_OPT_DNS_SERVER:
      /* Read primary and secondary DNS Server */
      net_addr4_copy (LocM.PriDNS, &buf[index+2]);
      net_addr4_copy (LocM.SecDNS, (buf[index+1] < 8) ? net_addr_unspec : &buf[index+6]);
      DEBUG_INF2 ("Prim. DNS",LocM.PriDNS);
      DEBUG_INF2 ("Sec. DNS",LocM.SecDNS);
      EvrNetDHCP_ViewDnsServers (h->If->Id, LocM.PriDNS);
      break;

    case DHCP_OPT_HOST_NAME:
      /* Read host name here and store it */
      break;

    case DHCP_OPT_LEASE_TIME:
      tmp = get_u32 (&buf[index+2]);
      /* Time calculation from RFC! These values can */
      /* also be received directly in options.       */
      ctrl->t1 = 5 * tmp / 10;  /* 0.5*tmp   */
      ctrl->t2 = 7 * tmp / 8;   /* 0.875*tmp */
      DEBUGF (DHCP," Lease time: %d sec\n",tmp);
      EvrNetDHCP_ViewLeaseTime (h->If->Id, tmp);
      break;

    case DHCP_OPT_T1_VALUE:
      /* Read T1 - DHCP Renewal time */
      ctrl->t1 = get_u32 (&buf[index+2]);
      DEBUGF (DHCP," T1 timeout: %d sec\n",ctrl->t1);
      EvrNetDHCP_ViewTimeT1 (h->If->Id, ctrl->t1);
      break;

    case DHCP_OPT_T2_VALUE:
      /* Read T2 - DHCP Rebinding time */
      ctrl->t2 = get_u32 (&buf[index+2]);
      DEBUGF (DHCP," T2 timeout: %d sec\n",ctrl->t2);
      EvrNetDHCP_ViewTimeT2 (h->If->Id, ctrl->t2);
      break;

    case DHCP_OPT_BOOTFILE:
      /* Read Bootfile Name and provide it to  */
      /* the user as a null-terminated string. */
      tmp = buf[index + buf[index+1] + 2];
      (__CONST_CAST(uint8_t *)buf)[index + buf[index+1] + 2] = 0;
      DEBUGF (DHCP," Boot fname: %s\n",&buf[index+2]);
      EvrNetDHCP_ViewBootfileName (&buf[index+2], buf[index+1]+1);
      netDHCP_Notify (h->If->Id,
                      NET_DHCP_OPTION_BOOTFILE_NAME, &buf[index+2], buf[index+1]);
      (__CONST_CAST(uint8_t *)buf)[index + buf[index+1] + 2] = tmp & 0xFF;
      break;

    case DHCP_OPT_NTP_SERVER:
      /* Read a list of NTP Servers and provide it to the user */
      DEBUG_INFO (&buf[index]);
      EvrNetDHCP_ViewNtpServerList (h->If->Id, &buf[index+2], buf[index+1]);
      netDHCP_Notify (h->If->Id,
                      NET_DHCP_OPTION_NTP_SERVERS, &buf[index+2], buf[index+1]);
      break;

    case DHCP_OPT_PAD:
      return (index + 1);

    case DHCP_OPT_END:
      /* End of options */
      return (0xFFFFFFFF);
  }
  /* Calculate index of next option */
  index += buf[index+1] + 2;
  return (index);
}

/**
  \brief       Process DHCP option in state RENEWING.
  \param[in]   h      DHCP instance handle.
  \param[in]   buf    buffer containing DHCP options.
  \param[in]   index  position of an option to process.
  \return      index of next option.
*/
static uint32_t proc_opt_renew (NET_DHCP_CFG *h, const uint8_t *buf, uint32_t index) {
  uint32_t tmp;

  switch (buf[index]) {
    case DHCP_OPT_LEASE_TIME:
      tmp = get_u32 (&buf[index+2]);
      /* Time calculation from RFC! These values   */
      /* can also be received directly in options. */
      ctrl->t1 = 5 * tmp / 10;  /* 0.5*tmp   */
      ctrl->t2 = 7 * tmp / 8;   /* 0.875*tmp */
      DEBUGF (DHCP," Lease time: %d sec\n",tmp);
      EvrNetDHCP_ViewLeaseTime (h->If->Id, tmp);
      break;

    case DHCP_OPT_T1_VALUE:
      /* Read T1 - DHCP Renewal time */
      ctrl->t1 = get_u32 (&buf[index+2]);
      DEBUGF (DHCP," T1 timeout: %d sec\n",ctrl->t1);
      EvrNetDHCP_ViewTimeT1 (h->If->Id, ctrl->t1);
      break;

    case DHCP_OPT_T2_VALUE:
      /* Read T2 - DHCP Rebinding time */
      ctrl->t2 = get_u32 (&buf[index+2]);
      DEBUGF (DHCP," T2 timeout: %d sec\n",ctrl->t2);
      EvrNetDHCP_ViewTimeT2 (h->If->Id, ctrl->t2);
      break;

    case DHCP_OPT_ROUTER:
      /* Update Default Gateway IP address */
      net_addr4_copy (LocM.DefGW, &buf[index+2]);
      DEBUG_INF2 ("Gateway",LocM.DefGW);
      EvrNetDHCP_ViewGatewayAddress (h->If->Id, LocM.DefGW);
      break;

    case DHCP_OPT_DNS_SERVER:
      /* Update Primary and Secondary DNS Server */
      net_addr4_copy (LocM.PriDNS, &buf[index+2]);
      net_addr4_copy (LocM.SecDNS, (buf[index+1] < 8) ? net_addr_unspec : &buf[index+6]);
      DEBUG_INF2 ("Prim. DNS",LocM.PriDNS);
      DEBUG_INF2 ("Sec. DNS",LocM.SecDNS);
      EvrNetDHCP_ViewDnsServers (h->If->Id, LocM.PriDNS);
      break;

    case DHCP_OPT_PAD:
      return (index + 1);

    case DHCP_OPT_END:
      /* End of options */
      return (0xFFFFFFFF);
  }

  /* Calculate index of next option */
  index += buf[index+1] + 2;
  return (index);
}

/**
  \brief       Assign Auto-IP address.
  \param[in]   h  DHCP instance handle.
  \note        Address range 169.254.1.0 - 169.254.254.255
*/
static void assign_auto_ip (NET_DHCP_CFG *h) {
  uint32_t tmp;

  tmp = net_rand16 ();
  ctrl->AutoIp[0] = 169;
  ctrl->AutoIp[1] = 254;
  ctrl->AutoIp[2] = (tmp >> 8) & 0xFF;
  ctrl->AutoIp[3] = (tmp)      & 0xFF;

  if ((ctrl->AutoIp[2] == 0 || ctrl->AutoIp[2] == 255)) {
    /* Skip reserved IPs 169.254.0.x and 169.254.255.x */
    ctrl->AutoIp[2] += 2;
  }
}

#ifdef Network_Debug_STDIO
/**
  \brief       Debug print NTP Server list option.
  \param[in]   opt  pointer to option list.
*/
static void debug_info (const uint8_t *opt) {
  int32_t i,max = opt[1] >> 2;

  DEBUGF (DHCP," NTP Count : %d\n",max);
  for (i = 1, opt += 2; i <= max; opt += 4, i++) {
    DEBUGF (DHCP,"  Server_%-2d: %s\n",i,net_addr4_ntoa(opt));
  }
}

/**
  \brief       Debug print DHCP address information.
  \param[in]   msg       explanation message to print.
  \param[in]   ip4_addr  IPv4 address.
*/
static void debug_inf2 (const char *msg, const uint8_t *ip4_addr) {
  DEBUGF (DHCP," %-10s: %s\n",msg,net_addr4_ntoa(ip4_addr));
}
#endif

/* Library default function(s) */
__WEAK void netDHCP_Notify (uint32_t if_id, uint8_t option,
                            const uint8_t *val, uint32_t len) {
  (void)if_id; (void)option; (void)val; (void)len; }
