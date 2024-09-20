/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ppp_ipcp.c
 * Purpose: PPP Internet Protocol Control
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_lib.h"
#include "net_common.h"
#include "net_mem.h"
#include "net_addr.h"
#include "net_sys.h"
#include "net_ip4.h"
#include "net_ppp.h"
#include "net_ppp_lcp.h"
#include "net_ppp_ipcp.h"
#include "net_dbg.h"

/* Global variables */
NET_IPCP_CTRL net_ppp0_ipcp_control;

/* Code shortening macros */
#define ctrl       ( h->Ctrl)
#define ipcp_ctl   ( h->IpcpCtrl)
#define LocM       (*h->If->localm)
#define LocM6      (*h->If->localm6)

/* Local Functions */
static void ipcp_set_rem_ip (NET_PPP_CFG *h);
static void ipcp_conf_rq (NET_PPP_CFG *h);
static void ipcp_send (NET_PPP_CFG *h, NET_FRAME *frame);
static void ipcp_configure (NET_PPP_CFG *h);
#ifdef Network_Debug_STDIO
 static void debug_info (const NET_FRAME *frm);
 static void debug_inf2 (const char *msg, const uint8_t *ip4_addr);
#endif

/**
  \brief       Initialize IPCP protocol for PPP.
  \param[in]   h  PPP interface handle.
*/
void net_ipcp_init (NET_PPP_CFG *h) {
  DEBUGF (PPP,"IPCP_Init\n");
  EvrNetPPP_IpcpInit ();
  ctrl->Flags &= ~PPP_FLAG_NETWORK;
  memset (ipcp_ctl, 0, sizeof (*ipcp_ctl));

  /* Init local machine info */
  if (!net_addr4_aton (h->If->Ip4Cfg->IpAddr, LocM.IpAddr) ||
      !net_addr4_aton (h->If->Ip4Cfg->PriDNS, LocM.PriDNS) ||
      !net_addr4_aton (h->If->Ip4Cfg->SecDNS, LocM.SecDNS)) {
    ERRORF (PPP,"Init, IPv4 config error\n");
    EvrNetPPP_IpcpIp4ConfigError ();
    net_sys_error (NET_ERROR_CONFIG);
  }
  net_addr4_copy (ipcp_ctl->MyIP, LocM.IpAddr);
  __ALIGNED_UINT32(LocM.NetMask) = HTONL(PPP_NET_MASK);
}

/**
  \brief       De-initialize IPCP protocol for PPP.
  \param[in]   h  PPP interface handle.
*/
void net_ipcp_uninit (NET_PPP_CFG *h) {
  DEBUGF (PPP,"IPCP_Uninit\n");
  EvrNetPPP_IpcpUninit ();

  /* Clear IPv4 local machine info */
  memset (&LocM, 0, sizeof (LocM));
  memset (ipcp_ctl, 0, sizeof (*ipcp_ctl));
}

/**
  \brief       Determine IPv4 address for remote peer.
  \param[in]   h  PPP interface handle.
  \note        Dynamic host configuration for PPP!
*/
static void ipcp_set_rem_ip (NET_PPP_CFG *h) {
  uint32_t ip, mask, net;

  ip   = ntohl(__ALIGNED_UINT32(LocM.IpAddr));
  mask = ntohl(__ALIGNED_UINT32(LocM.NetMask));

  net = ip & mask;
  ip = (ip + 1) & ~mask;
  if (ip == 0) {
    /* Group IP address 0 reserved */
    ip++;
  }
  if (ip == ~mask) {
    /* Skip the local broadcast IP address */
    ip = 1;
  }
  set_u32 (ipcp_ctl->RemIP, net | ip);
}

/**
  \brief       Send IPCP configuration request.
  \param[in]   h  PPP interface handle.
*/
static void ipcp_conf_rq (NET_PPP_CFG *h) {
  NET_FRAME *txfrm;
  NET_IPCP_OPT *opt;
  uint32_t len;

  DEBUGF (PPP,"IPCP_Conf_Rq send\n");
  EvrNetPPP_IpcpSendConfigRequest ();
  len = 10;
  if (ipcp_ctl->Flags & IPCP_FLAG_PRIDNS) len += 6;
  if (ipcp_ctl->Flags & IPCP_FLAG_SECDNS) len += 6;

  txfrm = net_mem_alloc (PPP_FRM_OFFS + PPP_HEADER_LEN + len);
  IPCP_FRAME(txfrm)->Code = IPCP_CONF_RQ;
  IPCP_FRAME(txfrm)->Id   = ipcp_ctl->RespId;
  IPCP_FRAME(txfrm)->Len  = htons(len);

  /* Our IP Address */
  opt = (NET_IPCP_OPT *)&PPP_FRAME(txfrm)->Data[IPCP_HEADER_LEN];
  opt->Code = IPCP_OPT_IPADDR;
  opt->Len  = 6;
  net_addr4_copy (opt->Data, ipcp_ctl->MyIP);
  DEBUG_INF2 ("Address", ipcp_ctl->MyIP);
  EvrNetPPP_IpcpOptionIpAddress (ipcp_ctl->MyIP);
  opt = (NET_IPCP_OPT *)&opt->Data[4];

  if (ipcp_ctl->Flags & IPCP_FLAG_PRIDNS) {
    /* Primary DNS server */
    opt->Code = IPCP_OPT_PRIDNS;
    opt->Len  = 6;
    net_addr4_copy (opt->Data, ipcp_ctl->PriDNS);
    DEBUG_INF2 ("PriDNS", ipcp_ctl->PriDNS);
    EvrNetPPP_IpcpOptionPrimaryDns (ipcp_ctl->PriDNS);
    opt = (NET_IPCP_OPT *)&opt->Data[4];
  }

  if (ipcp_ctl->Flags & IPCP_FLAG_SECDNS) {
    /* Secondary DNS server */
    opt->Code = IPCP_OPT_SECDNS;
    opt->Len  = 6;
    net_addr4_copy (opt->Data, ipcp_ctl->SecDNS);
    DEBUG_INF2 ("SecDNS", ipcp_ctl->SecDNS);
    EvrNetPPP_IpcpOptionSecondaryDns (ipcp_ctl->SecDNS);
  }
  ipcp_send (h, txfrm);

  /* Now start the time out */
  ipcp_ctl->Delay = h->RetryTout;
}

/**
  \brief       Start IPCP configuration for PPP client mode.
  \param[in]   h  PPP interface handle.
*/
static void ipcp_configure (NET_PPP_CFG *h) {
  if (ctrl->Flags & PPP_FLAG_NETWORK) {
    /* Done, our IP is negotiated */
    ipcp_ctl->Delay = 0;
    return;
  }

  /* Change Code-Id for new request */
  ipcp_ctl->RespId  = ctrl->Id++;
  ipcp_ctl->Retries = h->MaxRetry;
  ipcp_conf_rq (h);
}

/**
  \brief       Send IPCP frame.
  \param[in]   h      PPP interface handle.
  \param[in]   frame  network frame.
*/
static void ipcp_send (NET_PPP_CFG *h, NET_FRAME *frame) {
  DEBUGF (PPP,"Sending IPCP frame\n");
  DEBUG_INFO (frame);
  EvrNetPPP_IpcpSendFrame (IPCP_FRAME(frame));
  net_ppp_send_proto (h, frame, PPP_PROT_IPCP);
  net_mem_free (frame);
}

/**
  \brief       Process received IPCP frame.
  \param[in]   h      PPP interface handle.
  \param[in]   frame  network frame.
*/
void net_ipcp_process (NET_PPP_CFG *h, NET_FRAME *frame) {
  NET_FRAME *txfrm;
  uint32_t ri, ti;
  bool bReject;
  uint8_t opst;

  DEBUGF (PPP,"*** Process_frame IPCP ***\n");
  DEBUG_INFO (frame);
  EvrNetPPP_IpcpReceiveFrame (IPCP_FRAME(frame));

  switch (IPCP_FRAME(frame)->Code) {
    case IPCP_CONF_RQ:
      /* Configuration Request received */
      txfrm = net_mem_alloc (frame->length);
      ri = IPCP_HEADER_LEN;
      ti = IPCP_HEADER_LEN;
      bReject = false;
      opst    = 0;
      while (ri < ntohs(IPCP_FRAME(frame)->Len)) {
        /* Loop through all the options */
        opst++;
        switch (PPP_FRAME(frame)->Data[ri]) {
          case IPCP_OPT_IPADDR:
            /* IP address requested by remote peer */
            DEBUG_INF2 ("Address", &PPP_FRAME(frame)->Data[ri+2]);
            EvrNetPPP_IpcpOptionIpAddress (&PPP_FRAME(frame)->Data[ri+2]);
            if (bReject == false) {
              /* The frame is not rejected, copy the option into accept packet */
              if (get_u32 (&PPP_FRAME(frame)->Data[ri+2]) == 0) {
                /* Remote Peer wants to retrieve his IP address */
                ipcp_set_rem_ip (h);
                opst |= 0x80;
              }
              else if ((ctrl->Flags & PPP_FLAG_CLIENT)   ||
                       net_ip4_is_onlink (h->If, &PPP_FRAME(frame)->Data[ri+2])) {
                /* OK, Remote Static IP belongs to local network */
                net_addr4_copy (ipcp_ctl->RemIP, &PPP_FRAME(frame)->Data[ri+2]);
              }
              else {
                /* Send NAK */
                ipcp_set_rem_ip (h);
                opst |= 0x80;
              }
              memcpy (&PPP_FRAME(txfrm)->Data[ti], &PPP_FRAME(frame)->Data[ri], 6);
              ti += 6;
            }
            ri += 6;
            break;

          default:
            /* An unkown or unsupported option, reject the packet */
            DEBUGF (PPP," Unknown Opt (0x%02X)\n",PPP_FRAME(frame)->Data[ri]);
            EvrNetPPP_IpcpOptionUnknown (PPP_FRAME(frame)->Data[ri]);
            if (bReject == false) {
              ti = IPCP_HEADER_LEN;
              bReject = true;
            }
            /* Copy the option back as per usual */
            memcpy (&PPP_FRAME(txfrm)->Data[ti], &PPP_FRAME(frame)->Data[ri],
                                                 PPP_FRAME(frame)->Data[ri+1]);
            ti += PPP_FRAME(frame)->Data[ri+1];
            ri += PPP_FRAME(frame)->Data[ri+1];
            break;
        }
      }
      if (bReject == true) {
        DEBUGF (PPP,"IPCP Send Reject\n");
        EvrNetPPP_IpcpSendReject ();
        IPCP_FRAME(txfrm)->Code = IPCP_CONF_REJ;
      }
      else if (opst == 0x81) {
        DEBUGF (PPP,"IPCP Send Nak\n");
        EvrNetPPP_IpcpSendNak ();
        IPCP_FRAME(txfrm)->Code = IPCP_CONF_NAK;
        DEBUG_INF2 ("Address", ipcp_ctl->RemIP);
        EvrNetPPP_IpcpOptionIpAddress (ipcp_ctl->RemIP);
        net_addr4_copy (&PPP_FRAME(txfrm)->Data[IPCP_HEADER_LEN+2], ipcp_ctl->RemIP);
      }
      else {
        DEBUGF (PPP,"IPCP Send Ack\n");
        EvrNetPPP_IpcpSendAck ();
        IPCP_FRAME(txfrm)->Code = IPCP_CONF_ACK;
      }
      IPCP_FRAME(txfrm)->Id  = IPCP_FRAME(frame)->Id;
      IPCP_FRAME(txfrm)->Len = htons(ti);
      net_mem_shrink (txfrm, PPP_FRM_OFFS + PPP_HEADER_LEN + ti);
      ipcp_send (h, txfrm);

      if ((bReject == false) && (ipcp_ctl->Delay == 0)) {
        ipcp_configure (h);
      }
      break;

    case IPCP_CONF_ACK:
      /* Client acknowledge our request */
      if ((IPCP_FRAME(frame)->Id) != ipcp_ctl->RespId) {
        /* This is a wrong ack */
        DEBUGF (PPP," Discarded, Wrong Ack\n");
        EvrNetPPP_IpcpWrongAckReceived (IPCP_FRAME(frame)->Id, ipcp_ctl->RespId);
        break;
      }
      ri = IPCP_HEADER_LEN;
      while (ri < ntohs(IPCP_FRAME(frame)->Len)) {
        /* Loop through all the options */
        switch (PPP_FRAME(frame)->Data[ri]) {
          case IPCP_OPT_IPADDR:
            /* IP address acknowledged by remote peer */
            DEBUG_INF2 ("Address", ipcp_ctl->MyIP);
            EvrNetPPP_IpcpOptionIpAddress (ipcp_ctl->MyIP); 
            net_addr4_copy (LocM.IpAddr, ipcp_ctl->MyIP);
            if (ctrl->Flags & PPP_FLAG_CLIENT) {
              /* Set NetMask to 255.255.255.255 for PPP client */
              __ALIGNED_UINT32(LocM.NetMask) = HTONL(PPP_NET_MASK_CLIENT);
            }
            DEBUGF (PPP,"Network-Layer Up\n");
            EvrNetPPP_IpcpNetworkLayerUp ();
            ctrl->Flags |= PPP_FLAG_NETWORK;
            ri += 6;
            break;

          case IPCP_OPT_PRIDNS:
            /* Primary DNS server acknowledged */
            DEBUG_INF2 ("PriDNS", ipcp_ctl->PriDNS);
            EvrNetPPP_IpcpOptionPrimaryDns (ipcp_ctl->PriDNS);
            net_addr4_copy (LocM.PriDNS, ipcp_ctl->PriDNS);
            ri += 6;
            break;

          case IPCP_OPT_SECDNS:
            /* Secondary DNS server acknowledged */
            DEBUG_INF2 ("SecDNS", ipcp_ctl->SecDNS);
            EvrNetPPP_IpcpOptionSecondaryDns (ipcp_ctl->SecDNS);
            net_addr4_copy (LocM.SecDNS, ipcp_ctl->SecDNS);
            ri += 6;
            break;

          default:
            /* This is not what we have requested, terminate PPP */
            goto err;
        }
      }
      ipcp_configure (h);
      break;

    case IPCP_CONF_NAK:
      /* Client did Not Acknowledge our Request */
      if ((IPCP_FRAME(frame)->Id) != ipcp_ctl->RespId) {
        /* This is a wrong ack */
        DEBUGF (PPP," Discarded, Wrong Nak\n");
        EvrNetPPP_IpcpWrongNakReceived (IPCP_FRAME(frame)->Id, ipcp_ctl->RespId);
        break;
      }
      ri = IPCP_HEADER_LEN;
      while (ri < ntohs(IPCP_FRAME(frame)->Len)) {
        /* Loop through all the options */
        switch (PPP_FRAME(frame)->Data[ri]) {
          case IPCP_OPT_PRIDNS:
            /* Primary DNS server not acknowledged */
            net_addr4_copy (ipcp_ctl->PriDNS, &PPP_FRAME(frame)->Data[ri+2]);
            DEBUG_INF2 ("PriDNS", ipcp_ctl->PriDNS);
            EvrNetPPP_IpcpOptionPrimaryDns (ipcp_ctl->PriDNS);
            ri += 6;
            break;

          case IPCP_OPT_SECDNS:
            /* Secondary DNS server not acknowledged */
            net_addr4_copy (ipcp_ctl->SecDNS, &PPP_FRAME(frame)->Data[ri+2]);
            DEBUG_INF2 ("SecDNS", ipcp_ctl->SecDNS);
            EvrNetPPP_IpcpOptionSecondaryDns (ipcp_ctl->SecDNS);
            ri += 6;
            break;

          case IPCP_OPT_IPADDR:
            /* IP address not acknowledged by remote peer */
            if ((ctrl->Flags & PPP_FLAG_CLIENT)   ||
                net_ip4_is_onlink (h->If, &PPP_FRAME(frame)->Data[ri+2])) {
              /* Suggested IP belongs to our subnet */
              net_addr4_copy (ipcp_ctl->MyIP, &PPP_FRAME(frame)->Data[ri+2]);
              DEBUG_INF2 ("Address", ipcp_ctl->MyIP);
              EvrNetPPP_IpcpOptionIpAddress (ipcp_ctl->MyIP);
              ri += 6;
              break;
            }
            /* IP does not belong to our subnet, terminate PPP */
            DEBUGF (PPP," Aborted, Wrong subnet\n");
            EvrNetPPP_IpcpWrongSubnet (&PPP_FRAME(frame)->Data[ri+2]);
            net_lcp_terminate (h);
            return;

          default:
            /* Ignore advertised, but not requested options */
            ri += PPP_FRAME(frame)->Data[ri + 1];
            break;
        }
      }
      ipcp_configure (h);
      break;

    case IPCP_CONF_REJ:
      /* Client Rejected our Request */
      if ((IPCP_FRAME(frame)->Id) != ipcp_ctl->RespId) {
        /* This is a wrong ack */
        DEBUGF (PPP," Discarded, Wrong Reject\n");
        EvrNetPPP_IpcpWrongRejectReceived (IPCP_FRAME(frame)->Id, ipcp_ctl->RespId);
        break;
      }
      ri = IPCP_HEADER_LEN;
      while (ri < ntohs(IPCP_FRAME(frame)->Len)) {
        /* Loop through all the options */
        switch (PPP_FRAME(frame)->Data[ri]) {
          case IPCP_OPT_PRIDNS:
            /* Primary DNS server was rejected */
            DEBUGF (PPP," PriDNS rejected\n");
            EvrNetPPP_IpcpPrimaryDnsRejected ();
            ipcp_ctl->Flags &= ~IPCP_FLAG_PRIDNS;
            ri += 6;
            break;

          case IPCP_OPT_SECDNS:
            /* Secondary DNS server was rejected */
            DEBUGF (PPP," SecDNS rejected\n");
            EvrNetPPP_IpcpSecondaryDnsRejected ();
            ipcp_ctl->Flags &= ~IPCP_FLAG_SECDNS;
            ri += 6;
            break;

          case IPCP_OPT_IPADDR:
            /* IP Addresses was rejected, terminate PPP */
            ERRORF (PPP,"IPCP_Process, IP-Addr rejected\n");
            EvrNetPPP_IpcpIpAddressRejected ();
            net_lcp_terminate (h);
            return;

          default:
            /* We did not request this option */
err:        ERRORF (PPP,"IPCP_Process, Opt (0x%02X) not requested\n",
                         PPP_FRAME(frame)->Data[ri]);
            EvrNetPPP_IpcpNotRequestedOption (PPP_FRAME(frame)->Data[ri]);
            net_lcp_terminate (h);
            return;
        }
      }
      ipcp_configure (h);
      break;

    case IPCP_TERM_RQ:
      /* Server Terminate-Request received */
      txfrm = net_mem_alloc (frame->length);
      memcpy (PPP_FRAME(txfrm)->Data, PPP_FRAME(frame)->Data,
                                      ntohs(IPCP_FRAME(frame)->Len));
      IPCP_FRAME(txfrm)->Code = IPCP_TERM_ACK;
      ipcp_send (h, txfrm);
      __FALLTHROUGH;

    case IPCP_TERM_ACK:
      /* Terminate ACK */
      net_lcp_terminate (h);
      break;

    default:
      /* Reject unsupported IPCP Code */
      net_ppp_cp_code_reject (h, frame, PPP_PROT_IPCP);
      break;
  }
}

/**
  \brief       Run IPCP protocol main function.
  \param[in]   h  PPP interface handle.
  \note        Function is called on every system tick.
*/
void net_ipcp_run (NET_PPP_CFG *h) {
  if (ctrl->Flags & PPP_FLAG_IPCP_CONF) {
    if (ctrl->Flags & PPP_FLAG_OPENED) {
      /* Start IPCP configuration for client mode */
      ctrl->Flags  &= ~PPP_FLAG_IPCP_CONF;
      ipcp_ctl->Flags |= (IPCP_FLAG_PRIDNS | IPCP_FLAG_SECDNS);
      if (h->ObtainIp) {
        /* Obtain an IP address automatically for PPP client */
        set_u32 (ipcp_ctl->MyIP, 0);
      }
      ipcp_configure (h);
      return;
    }
  }

  if (ipcp_ctl->Delay == 0) {
    return;
  }
  if (--ipcp_ctl->Delay == 0) {
    if (ipcp_ctl->Retries != 0) {
      ipcp_ctl->Retries--;
      /* Retransmit Configuration request, keep Code-Id */
      ipcp_conf_rq (h);
      return;
    }
    /* All retries used up, terminate PPP */
    net_lcp_terminate (h);
  }
}

#ifdef Network_Debug_STDIO
/**
  \brief       Debug print IPCP header information.
  \param[in]   frm  network frame.
*/
static void debug_info (const NET_FRAME *frm) {
  static const char p_asc[][9] = {
    "CONF-REQ",
    "CONF-ACK",
    "CONF-NAK",
    "CONF-REJ",
    "TERM-REQ",
    "TERM-ACK",
    "CODE-REJ"
  };
  static const uint8_t p_bin[] = {
    IPCP_CONF_RQ,
    IPCP_CONF_ACK,
    IPCP_CONF_NAK,
    IPCP_CONF_REJ,
    IPCP_TERM_RQ,
    IPCP_TERM_ACK,
    IPCP_CODE_REJ
  };
  uint32_t i;

  for (i = 0; i < sizeof (p_bin); i++) {
    if (p_bin[i] == IPCP_FRAME(frm)->Code) {
      DEBUGF (PPP," Code %s, Id=0x%02X\n",p_asc[i],IPCP_FRAME(frm)->Id);
      goto d;
    }
  }
  DEBUGF (PPP," Code (0x%02X), Id=0x%02X\n",IPCP_FRAME(frm)->Code,
                                            IPCP_FRAME(frm)->Id);
d:DEBUGF (PPP," Length %d bytes\n",ntohs(IPCP_FRAME(frm)->Len));
}

/**
  \brief       Debug print IPCP address information.
  \param[in]   msg       explanation message to print.
  \param[in]   ip4_addr  IPv4 address.
*/
static void debug_inf2 (const char *msg, const uint8_t *ip4_addr) {
  DEBUGF (PPP," %-7s %s\n",msg,net_addr4_ntoa(ip4_addr));
}
#endif
