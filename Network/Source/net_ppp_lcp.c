/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ppp_lcp.c
 * Purpose: PPP Link Control
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_mem.h"
#include "net_ppp.h"
#include "net_ppp_lcp.h"
#include "net_dbg.h"

/* Global variables */
NET_LCP_CTRL net_ppp0_lcp_control;

/* Code shortening macros */
#define ctrl       (h->Ctrl)
#define lcp_ctl    (h->LcpCtrl)

/* Local Functions */
static void lcp_conf_rq (NET_PPP_CFG *h);
static void lcp_echo_rq (NET_PPP_CFG *h);
static void lcp_term_rq (NET_PPP_CFG *h);
static void lcp_send (NET_PPP_CFG *h, NET_FRAME *frame);
static void lcp_configure (NET_PPP_CFG *h);
#ifdef Network_Debug_STDIO
 static void debug_info (uint32_t ch_map);
 static void debug_inf2 (const NET_FRAME *frm, int32_t dummy);
#endif

/**
  \brief       Initialize LCP protocol for PPP.
  \param[in]   h  PPP interface handle.
*/
void net_lcp_init (NET_PPP_CFG *h) {
  DEBUGF (PPP,"LCP_Init\n");
  EvrNetPPP_LcpInit ();
  ctrl->Flags  &= ~(PPP_FLAG_PFC    | PPP_FLAG_ACFC   |
                    PPP_FLAG_LINKUP | PPP_FLAG_OPENED | PPP_FLAG_AUTH_REQ);
  /* Default all control characters are send escaped */
  ctrl->CharMap = 0xFFFFFFFF;
  ctrl->Auth    = PPP_ATYPE_NONE;

  memset (lcp_ctl, 0, sizeof (*lcp_ctl));
  lcp_ctl->RecvChMap = h->ACCmap;
  lcp_ctl->SendChMap = 0xFFFFFFFF;
}

/**
  \brief       De-initialize LCP protocol for PPP.
  \param[in]   h  PPP interface handle.
*/
void net_lcp_uninit (NET_PPP_CFG *h) {
  DEBUGF (PPP,"LCP_Uninit\n");
  EvrNetPPP_LcpUninit ();
  memset (lcp_ctl, 0, sizeof (*lcp_ctl));
}

/**
  \brief       Send LCP configuration request.
  \param[in]   h  PPP interface handle.
*/
static void lcp_conf_rq (NET_PPP_CFG *h) {
  NET_FRAME *txfrm;
  uint32_t ti;

  /* Negotiation counter to prevent dead loops */
  if (++lcp_ctl->ReqCount == 30) {
    /* Too many requests has been sent, terminate PPP */
    ERRORF (PPP,"LCP_Conf_Rq, Too many retries\n");
    EvrNetPPP_LcpNoRetriesLeft ();
    net_lcp_terminate (h);
    return;
  }

  DEBUGF (PPP,"LCP_Conf_Rq send\n");
  EvrNetPPP_LcpSendConfigRequest ();
  txfrm = net_mem_alloc (PPP_FRM_OFFS + PPP_HEADER_LEN + 25);
  LCP_FRAME(txfrm)->Code = LCP_CONF_RQ;
  LCP_FRAME(txfrm)->Id   = lcp_ctl->RespId;
  ti = LCP_HEADER_LEN;

  /* Async Control Character Map */
  if ((lcp_ctl->Flags & LCP_FLAG_ACCMAP) == 0) {
    PPP_FRAME(txfrm)->Data[ti]   = LCP_OPT_ACCMAP;
    PPP_FRAME(txfrm)->Data[ti+1] = 0x06;
    set_u32 (&PPP_FRAME(txfrm)->Data[ti+2], lcp_ctl->RecvChMap);
    DEBUG_INFO (lcp_ctl->RecvChMap);
    EvrNetPPP_LcpOptionCharMap (lcp_ctl->RecvChMap);
    ti += 6;
  }

  /* Protocol Field Compression */
  if ((lcp_ctl->Flags & LCP_FLAG_PFC) == 0) {
    PPP_FRAME(txfrm)->Data[ti]   = LCP_OPT_PFC;
    PPP_FRAME(txfrm)->Data[ti+1] = 2;
    DEBUGF (PPP," Opt PFC\n"); 
    EvrNetPPP_LcpOptionPfc ();
    ti += 2;
  }

  /* Address and Control Field Compression */
  if ((lcp_ctl->Flags & LCP_FLAG_ACFC) == 0) {
    PPP_FRAME(txfrm)->Data[ti]   = LCP_OPT_ACFC;
    PPP_FRAME(txfrm)->Data[ti+1] = 2;
    DEBUGF (PPP," Opt ACFC\n");
    EvrNetPPP_LcpOptionAcfc ();
    ti += 2;
  }

  /* Magic Number */
  if ((lcp_ctl->Flags & LCP_FLAG_MAGIC) == 0) {
    PPP_FRAME(txfrm)->Data[ti]   = LCP_OPT_MAGIC;
    PPP_FRAME(txfrm)->Data[ti+1] = 0x06;
    if (lcp_ctl->MyMagic == 0x00000000) {
      lcp_ctl->MyMagic = net_rand32 ();
    }
    set_u32 (&PPP_FRAME(txfrm)->Data[ti+2], lcp_ctl->MyMagic);
    DEBUGF (PPP," Opt Magic-Num=0x%08X\n",lcp_ctl->MyMagic);
    EvrNetPPP_LcpOptionMagicNumber (lcp_ctl->MyMagic);
    ti += 6;
  }

  /* Authentication Protocol */
  if ((lcp_ctl->Flags & LCP_FLAG_AUTHP) == 0) {
    /* Negotiate authentication type for server mode */
    switch (lcp_ctl->AuthType) {
      case PPP_ATYPE_NONE:
        /* Initial request, select protocol */
        if (h->ChapAuth) {
          /* CHAP-MD5 enabled */
          lcp_ctl->AuthType = PPP_ATYPE_CHAP;
          goto rchap;
        }
        if (h->PapAuth) {
          /* PAP enabled */
          lcp_ctl->AuthType = PPP_ATYPE_PAP;
          goto rpap;
        }
        /* Authentication not required */
        lcp_ctl->Flags |= LCP_FLAG_AUTHP;
        ctrl->Auth      = PPP_ATYPE_NONE;
        break;
      case PPP_ATYPE_PAP:
        /* Request PAP authentication */
rpap:   PPP_FRAME(txfrm)->Data[ti] = LCP_OPT_AUTHP;
        PPP_FRAME(txfrm)->Data[ti+1] = 0x04;
        PPP_FRAME(txfrm)->Data[ti+2] = HI_BYTE(PPP_PROT_PAP);
        PPP_FRAME(txfrm)->Data[ti+3] = LO_BYTE(PPP_PROT_PAP);
        DEBUGF (PPP," Opt Auth=PAP\n");
        EvrNetPPP_LcpOptionAuthPap ();
        ti += 4;
        break;
      case PPP_ATYPE_CHAP:
        /* Request CHAP-MD5 authentication */ 
rchap:  PPP_FRAME(txfrm)->Data[ti]   = LCP_OPT_AUTHP;
        PPP_FRAME(txfrm)->Data[ti+1] = 0x05;
        PPP_FRAME(txfrm)->Data[ti+2] = HI_BYTE(PPP_PROT_CHAP);
        PPP_FRAME(txfrm)->Data[ti+3] = LO_BYTE(PPP_PROT_CHAP);
        PPP_FRAME(txfrm)->Data[ti+4] = 0x05;
        DEBUGF (PPP," Opt Auth=CHAP-MD5\n");
        EvrNetPPP_LcpOptionAuthChapMd5 ();
        ti += 5;
        break;
    }
  }

  LCP_FRAME(txfrm)->Len = htons(ti);
  net_mem_shrink (txfrm, PPP_FRM_OFFS + PPP_HEADER_LEN + ti);
  lcp_send (h, txfrm);

  /* Now start the time out */
  lcp_ctl->Delay = h->RetryTout;
}

/**
  \brief       Construct and send LCP echo request.
  \param[in]   h  PPP interface handle.
*/
static void lcp_echo_rq (NET_PPP_CFG *h) {
  NET_FRAME *txfrm;

  DEBUGF (PPP,"LCP_Echo_Rq send\n");
  DEBUGF (PPP," Magic-Num=0x%08X\n",lcp_ctl->MyMagic);
  EvrNetPPP_LcpSendEchoRequest (lcp_ctl->MyMagic);
  txfrm = net_mem_alloc (PPP_FRM_OFFS + PPP_HEADER_LEN + LCP_HEADER_LEN + 4);
  LCP_FRAME(txfrm)->Code = LCP_ECHO_RQ;
  LCP_FRAME(txfrm)->Id   = lcp_ctl->RespId;
  LCP_FRAME(txfrm)->Len  = HTONS(8);
  set_u32 (&PPP_FRAME(txfrm)->Data[LCP_HEADER_LEN], lcp_ctl->MyMagic);
  lcp_send (h, txfrm);
  /* Now start the time out */
  lcp_ctl->Delay = h->RetryTout;
}

/**
  \brief       Construct and send LCP terminate request.
  \param[in]   h  PPP interface handle.
*/
static void lcp_term_rq (NET_PPP_CFG *h) {
  NET_FRAME *txfrm;

  DEBUGF (PPP,"LCP_Term_Rq send\n");
  EvrNetPPP_LcpSendTerminateRequest ();
  txfrm = net_mem_alloc (PPP_FRM_OFFS + PPP_HEADER_LEN + LCP_HEADER_LEN);
  LCP_FRAME(txfrm)->Code = LCP_TERM_RQ;
  LCP_FRAME(txfrm)->Id   = lcp_ctl->RespId;
  LCP_FRAME(txfrm)->Len  = HTONS(4);
  lcp_send (h, txfrm);
  /* Now start the time out */
  lcp_ctl->Delay = h->RetryTout;
}

/**
  \brief       Send LCP frame.
  \param[in]   h      PPP interface handle.
  \param[in]   frame  network frame.
*/
static void lcp_send (NET_PPP_CFG *h, NET_FRAME *frame) {
  DEBUGF (PPP,"Sending LCP frame\n");
  DEBUG_INF2 (frame,0);
  EvrNetPPP_LcpSendFrame (LCP_FRAME(frame));
  net_ppp_send_proto (h, frame, PPP_PROT_LCP);
  net_mem_free (frame);
}

/**
  \brief       Start LCP configuration negotiation for PPP client mode.
  \param[in]   h  PPP interface handle.
*/
static void lcp_configure (NET_PPP_CFG *h) {
  if (lcp_ctl->Flags & LCP_FLAG_SHUTDN) {
    return;
  }
  if ((lcp_ctl->Flags & LCP_MASK_OPT) == LCP_MASK_OPT) {
    /* Done, we have negotiated all the options we need */
    lcp_ctl->Delay = 0;
    if (lcp_ctl->Flags & LCP_FLAG_TXACK) {
      /* Start using negotiated sending ACC-Map now */
      ctrl->CharMap = lcp_ctl->SendChMap;
      ctrl->Flags  |= PPP_FLAG_LINKUP;
      switch (ctrl->Auth) {
        case PPP_ATYPE_NONE:
          ctrl->Flags |= PPP_FLAG_OPENED;
          break;
        case PPP_ATYPE_PAP:
          if (ctrl->Flags & PPP_FLAG_CLIENT) {
            /*Start PPP in client mode */
            ctrl->Flags |= PPP_FLAG_AUTH_REQ;
          }
          break;
        case PPP_ATYPE_CHAP:
          if (!(ctrl->Flags & PPP_FLAG_CLIENT)) {
            /* Start CHAP in server mode */
            ctrl->Flags |= PPP_FLAG_AUTH_REQ;
          }
          break;
      }
    }
    return;
  }

  if (ctrl->Flags & PPP_FLAG_CLIENT) {
    /* Do not request Authentication for client mode */
    lcp_ctl->Flags |= LCP_FLAG_AUTHP;
  }
  /* Change Code-Id for new request */
  lcp_ctl->RespId  = ctrl->Id++;
  lcp_ctl->Retries = h->MaxRetry;
  lcp_conf_rq (h);
}

/**
  \brief       Send LCP terminate request, terminate PPP.
  \param[in]   h  PPP interface handle.
*/
void net_lcp_terminate (NET_PPP_CFG *h) {
  if (lcp_ctl->Flags & LCP_FLAG_SHUTDN) {
    return;
  }
  lcp_ctl->Flags  |= LCP_FLAG_SHUTDN;
  lcp_ctl->RespId  = ctrl->Id++;
  lcp_ctl->Retries = h->MaxRetry;
  lcp_term_rq (h);
}

/**
  \brief       Send echo request to check remote peer.
  \param[in]   h  PPP interface handle.
*/
void net_lcp_echo (NET_PPP_CFG *h) {
  if (!(ctrl->Flags & PPP_FLAG_NETWORK)) {
    return;
  }
  lcp_ctl->Flags  |= LCP_FLAG_ECHO;
  lcp_ctl->RespId  = ctrl->Id++;
  lcp_ctl->Retries = h->MaxRetry;
  lcp_echo_rq (h);
}

/**
  \brief       Process received LCP frame.
  \param[in]   h      PPP interface handle.
  \param[in]   frame  network frame.
*/
void net_lcp_process (NET_PPP_CFG *h, NET_FRAME *frame) {
  NET_FRAME *txfrm;
  uint32_t ri, ti, tmp;
  bool bReject;
  uint8_t nak;

  DEBUGF (PPP,"*** Process_frame LCP ***\n");
  DEBUG_INF2 (frame,0);
  EvrNetPPP_LcpReceiveFrame (LCP_FRAME(frame));

  switch (LCP_FRAME(frame)->Code) {
    case LCP_CONF_RQ:
      /* Configuration Request received */
      if (lcp_ctl->Flags & LCP_FLAG_SHUTDN) {
        /* Link is shutting down, ignore request */
        break;
      }
      if ((ctrl->Flags & PPP_FLAG_LINKUP) && !(ctrl->Flags & PPP_FLAG_CLIENT)) {
        /* Link is up, need to restart ppp */
        DEBUGF (PPP," Link up, restart PPP\n");
        EvrNetPPP_LcpDataLinkUp ();
        net_ppp_proto_init (h, false);
      }
      txfrm = net_mem_alloc (frame->length);
      ri = LCP_HEADER_LEN;
      ti = LCP_HEADER_LEN;
      bReject = false;
      nak     = 0x00;
      while (ri < ntohs(LCP_FRAME(frame)->Len)) {
        /* Loop through all the options */
        switch (PPP_FRAME(frame)->Data[ri]) {
          case LCP_OPT_MRU:
            /* Maximum Receive Unit */
            DEBUGF (PPP," Opt MRU=%d\n",get_u16(&PPP_FRAME(frame)->Data[ri+2]));
            EvrNetPPP_LcpOptionMru (get_u16(&PPP_FRAME(frame)->Data[ri+2]));
            /* This is an advice for optimum performance. The interface must */
            /* nevertheless be able to receive frames of up to 1500 bytes.   */
            if (bReject == false) {
              /* The frame is not rejected, copy the option into accept packet */
              memcpy (&PPP_FRAME(txfrm)->Data[ti], &PPP_FRAME(frame)->Data[ri], 4);
              ti += 4;
            }
            ri += 4;
            break;

          case LCP_OPT_ACCMAP:
            /* Async Control Character Map */
            tmp = get_u32 (&PPP_FRAME(frame)->Data[ri+2]);
            DEBUG_INFO (tmp);
            EvrNetPPP_LcpOptionCharMap (tmp);
            if (bReject == false) {
              /* Peer ACC-Map always accepted */
              lcp_ctl->SendChMap = tmp;
#if 0
              /* Check if ACC-Map acceptable */
              lcp_ctl->ChMap |= tmp;
              if (lcp_ctl->ChMap != tmp) {
                nak = (nak >> 4) | 0x20;
                ri += 6;
                break;
              }
#endif
              /* The frame is not rejected, copy the option into accept packet */
              memcpy (&PPP_FRAME(txfrm)->Data[ti], &PPP_FRAME(frame)->Data[ri], 6);
              ti += 6;
            }
            ri += 6;
            break;

          case LCP_OPT_PFC:
            /* Protocol Field Compression */
            DEBUGF (PPP," Opt PFC\n");
            EvrNetPPP_LcpOptionPfc ();
            goto acfc;

          case LCP_OPT_ACFC:
            /* Address and Control Field Compression */
            DEBUGF (PPP," Opt ACFC\n");
            EvrNetPPP_LcpOptionAcfc ();
acfc:       if (bReject == false) {
              /* Copy the data into our accept packet */
              memcpy (&PPP_FRAME(txfrm)->Data[ti], &PPP_FRAME(frame)->Data[ri], 2);
              ti += 2;
            }
            ri += 2;
            break;

          case LCP_OPT_MAGIC:
            /* Magic Number */
            lcp_ctl->PeerMagic = get_u32 (&PPP_FRAME(frame)->Data[ri+2]);
            DEBUGF (PPP," Opt Magic-Num=0x%08X\n",lcp_ctl->PeerMagic);
            EvrNetPPP_LcpOptionMagicNumber (lcp_ctl->PeerMagic);
            if (bReject == false) {
              /* Copy the data into our accept packet */
              memcpy (&PPP_FRAME(txfrm)->Data[ti], &PPP_FRAME(frame)->Data[ri], 6);
              ti += 6;
              /* Now Create our magic number */
              if (lcp_ctl->MyMagic == 0x00000000) {
                lcp_ctl->MyMagic = ~(lcp_ctl->PeerMagic << 1);
              }
            }
            ri += 6;
            break;

          case LCP_OPT_AUTHP:
            /* Authentication Protocol */
            DEBUGF (PPP," Opt Auth=0x%04X\n",get_u16(&PPP_FRAME(frame)->Data[ri+2]));
            EvrNetPPP_LcpOptionAuth (get_u16(&PPP_FRAME(frame)->Data[ri+2]));
            if (bReject == false) {
              if (!(ctrl->Flags & PPP_FLAG_CLIENT) || (h->EnAuth == 0)) {
                /* PPP Server or Authentication disabled, reject option */
                goto opt_rej;
              }
              /* Check Authentication Protocol selected */
              switch (get_u16 (&PPP_FRAME(frame)->Data[ri+2])) {
                case PPP_PROT_PAP:
                  if (!(h->PapAuth)) {
                    /* PAP disabled in PPP configuration */  
                    goto auth_nak;
                  }
                  ctrl->Auth = PPP_ATYPE_PAP;
                  /* Copy the data into our accept packet */
                  memcpy (&PPP_FRAME(txfrm)->Data[ti], &PPP_FRAME(frame)->Data[ri], 4);
                  ti += 4;
                  break; 
                case PPP_PROT_CHAP:
                  if (!(h->ChapAuth)) {
                    /* CHAP disabled in PPP configuration */
                    goto auth_nak;
                  }
                  if (PPP_FRAME(frame)->Data[ri+4] != 5) {
                    /* Algorithm other than MD5 is not supported */
                   DEBUGF (PPP," + CHAP not MD5\n");
                   EvrNetPPP_LcpOptionAuthChapNotMd5 ();
                   goto auth_nak;
                  }
                  ctrl->Auth = PPP_ATYPE_CHAP;
                  /* Copy the data into our accept packet */
                  memcpy (&PPP_FRAME(txfrm)->Data[ti], &PPP_FRAME(frame)->Data[ri], 5);
                  ti += 5;
                  break;
                default:
auth_nak:         nak = (nak >> 4) | 0x10;
                  break;
              }
            }
            ri += PPP_FRAME(frame)->Data[ri+1];
            break;

          default:
            /* An unkown or unsupported option, reject the packet */
            DEBUGF (PPP," Unknown Opt (0x%02X)\n",PPP_FRAME(frame)->Data[ri]);
            EvrNetPPP_LcpOptionUnknown (PPP_FRAME(frame)->Data[ri]);
            if (bReject == false) {
opt_rej:      ti = LCP_HEADER_LEN;
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
        DEBUGF (PPP,"LCP Send Reject\n");
        EvrNetPPP_LcpSendReject ();
        LCP_FRAME(txfrm)->Code = LCP_CONF_REJ;
      }
      else if (nak != 0x00) {
        DEBUGF (PPP,"LCP Send Nak\n");
        EvrNetPPP_LcpSendNak ();
        LCP_FRAME(txfrm)->Code = LCP_CONF_NAK;
        ti = LCP_HEADER_LEN;
        while (nak) {
          /* The sequence of options in a packet should not change */
          if (nak & 0x01) {
            /* NAK the Auth. option */
            if (h->ChapAuth) {
              /* Advice CHAP-MD5 */
              DEBUGF (PPP," Opt Auth=CHAP-MD5\n");
              EvrNetPPP_LcpOptionAuthChapMd5 ();
              PPP_FRAME(txfrm)->Data[ti]   = LCP_OPT_AUTHP;
              PPP_FRAME(txfrm)->Data[ti+1] = 5;
              PPP_FRAME(txfrm)->Data[ti+2] = HI_BYTE(PPP_PROT_CHAP);
              PPP_FRAME(txfrm)->Data[ti+3] = LO_BYTE(PPP_PROT_CHAP);
              PPP_FRAME(txfrm)->Data[ti+4] = 0x05;
              ti += 5;
            }
            else {
              /* Advice PAP */
              DEBUGF (PPP," Opt Auth=PAP\n");
              EvrNetPPP_LcpOptionAuthPap ();
              PPP_FRAME(txfrm)->Data[ti]   = LCP_OPT_AUTHP;
              PPP_FRAME(txfrm)->Data[ti+1] = 4;
              PPP_FRAME(txfrm)->Data[ti+2] = HI_BYTE(PPP_PROT_PAP);
              PPP_FRAME(txfrm)->Data[ti+3] = LO_BYTE(PPP_PROT_PAP);
              ti += 4;
            }
          }
          if (nak & 0x02) {
            /* NAK the ACC-Map */
            DEBUG_INFO (lcp_ctl->SendChMap);
            EvrNetPPP_LcpOptionCharMap (lcp_ctl->SendChMap);
            PPP_FRAME(txfrm)->Data[ti]   = LCP_OPT_ACCMAP;
            PPP_FRAME(txfrm)->Data[ti+1] = 6;
            set_u32 (&PPP_FRAME(txfrm)->Data[ti+2], lcp_ctl->SendChMap);
            ti += 6;
          }
          nak >>= 4;
        }
        bReject = true;
      }
      else {
        DEBUGF (PPP,"LCP Send Ack\n");
        EvrNetPPP_LcpSendAck ();
        LCP_FRAME(txfrm)->Code = LCP_CONF_ACK;
        lcp_ctl->Flags |= LCP_FLAG_TXACK;
      }
      LCP_FRAME(txfrm)->Id  = LCP_FRAME(frame)->Id;
      LCP_FRAME(txfrm)->Len = htons(ti);
      net_mem_shrink (txfrm, PPP_FRM_OFFS + PPP_HEADER_LEN + ti);
      lcp_send (h, txfrm);

      if ((bReject == false) && (lcp_ctl->Delay == 0)) {
        /* If we ack the packet, we can send a REQ packet then */
        lcp_configure (h);
      }
      break;

    case LCP_CONF_ACK:
      /* Client Acknowledge our request */
      if ((LCP_FRAME(frame)->Id) != lcp_ctl->RespId) {
        /* This is a wrong ack */
        DEBUGF (PPP," Discarded, Wrong Ack\n");
        EvrNetPPP_LcpWrongAckReceived (LCP_FRAME(frame)->Id, lcp_ctl->RespId);
        break;
      }
      ri = LCP_HEADER_LEN;
      while (ri < ntohs(LCP_FRAME(frame)->Len)) {
        /* Loop through all the options */
        switch (PPP_FRAME(frame)->Data[ri]) {
          case LCP_OPT_ACCMAP:
            /* Async Control Character Map */
            lcp_ctl->RecvChMap = get_u32 (&PPP_FRAME(frame)->Data[ri+2]);
            DEBUG_INFO (lcp_ctl->RecvChMap);
            EvrNetPPP_LcpOptionCharMap (lcp_ctl->RecvChMap);
            lcp_ctl->Flags |= LCP_FLAG_ACCMAP;
            ri += 6;
            break;

          case LCP_OPT_PFC:
            /* Protocol Field Compression */
            DEBUGF (PPP," Opt PFC\n");
            EvrNetPPP_LcpOptionPfc ();
            ctrl->Flags    |= PPP_FLAG_PFC;
            lcp_ctl->Flags |= LCP_FLAG_PFC;
            ri += 2;
            break;

          case LCP_OPT_ACFC:
            /* Address and Control Field Compression */
            DEBUGF (PPP," Opt ACFC\n");
            EvrNetPPP_LcpOptionAcfc ();
            ctrl->Flags    |= PPP_FLAG_ACFC;
            lcp_ctl->Flags |= LCP_FLAG_ACFC;
            ri += 2;
            break;

          case LCP_OPT_MAGIC:
            /* Magic Number */
            lcp_ctl->MyMagic = get_u32 (&PPP_FRAME(frame)->Data[ri+2]);
            DEBUGF (PPP," Opt Magic-Num=0x%08X\n",lcp_ctl->MyMagic);
            EvrNetPPP_LcpOptionMagicNumber (lcp_ctl->MyMagic);
            lcp_ctl->Flags |= LCP_FLAG_MAGIC;
            ri += 6;
            break;

          case LCP_OPT_AUTHP:
            /* Authentication Protocol */
            switch (get_u16 (&PPP_FRAME(frame)->Data[ri+2])) {
              case PPP_PROT_PAP:
                DEBUGF (PPP," Opt Auth=PAP\n");
                EvrNetPPP_LcpOptionAuthPap (); 
                ctrl->Auth = PPP_ATYPE_PAP;
                ri += 4;
                break;
              case PPP_PROT_CHAP:
                DEBUGF (PPP," Opt Auth=CHAP-MD5\n");
                EvrNetPPP_LcpOptionAuthChapMd5 ();
                ctrl->Auth = PPP_ATYPE_CHAP;
                ri += 5;
                break;
            }
            lcp_ctl->Flags |= LCP_FLAG_AUTHP;
            break;
        }
      }
      lcp_configure (h);
      break;

    case LCP_CONF_NAK:
      /* Client Nak-ed our request */
      if ((LCP_FRAME(frame)->Id) != lcp_ctl->RespId) {
        /* This is a wrong nak */
        DEBUGF (PPP," Discarded, Wrong Nak\n");
        EvrNetPPP_LcpWrongNakReceived (LCP_FRAME(frame)->Id, lcp_ctl->RespId);
        break;
      }
      ri = LCP_HEADER_LEN;
      while (ri < ntohs(LCP_FRAME(frame)->Len)) {
        /* Loop through all the options */
        switch (PPP_FRAME(frame)->Data[ri]) {
          case LCP_OPT_ACCMAP:
            /* Async Control Character Map */
            lcp_ctl->RecvChMap = get_u32 (&PPP_FRAME(frame)->Data[ri+2]);
            DEBUG_INFO (lcp_ctl->RecvChMap);
            EvrNetPPP_LcpOptionCharMap (lcp_ctl->RecvChMap);
            ri += 6;
            break;

          case LCP_OPT_PFC:
            /* Protocol Field Compression */
            DEBUGF (PPP," Opt PFC\n");
            EvrNetPPP_LcpOptionPfc ();
            lcp_ctl->Flags |= LCP_FLAG_PFC;
            ri += 2;
            break;

          case LCP_OPT_ACFC:
            /* Address and Control Field Compression */
            DEBUGF (PPP," Opt ACFC\n");
            EvrNetPPP_LcpOptionAcfc ();
            lcp_ctl->Flags |= LCP_FLAG_ACFC;
            ri += 2;
            break;

          case LCP_OPT_MAGIC:
            /* Magic Number */
            lcp_ctl->MyMagic = get_u32 (&PPP_FRAME(frame)->Data[ri+2]);
            DEBUGF (PPP," Opt Magic-Num=0x%08X\n",lcp_ctl->MyMagic);
            EvrNetPPP_LcpOptionMagicNumber (lcp_ctl->MyMagic);
            ri += 6;
            break;

          case LCP_OPT_AUTHP:
            /* Authentication Protocol */
#ifdef Network_Debug_STDIO
            if (get_u16 (&PPP_FRAME(frame)->Data[ri+2]) == PPP_PROT_PAP) {
              DEBUGF (PPP," Opt Auth=PAP\n");
            }
            else {
              DEBUGF (PPP," Unknown Auth Proto\n");
            }
#endif
            EvrNetPPP_LcpOptionAuth (get_u16(&PPP_FRAME(frame)->Data[ri+2]));
            if ((lcp_ctl->AuthType == PPP_ATYPE_CHAP) && (h->PapAuth)) {
              /* CHAP was Nak-ed, try with PAP now */
              lcp_ctl->AuthType = PPP_ATYPE_PAP;
              ri += PPP_FRAME(frame)->Data[ri+1];
              break;
            }
            /* Authentication nacked, terminate PPP */
            ERRORF (PPP,"LCP_Process, Config Auth failed\n");
            EvrNetPPP_LcpConfigAuthFailed ();
            net_lcp_terminate (h);
            return;
        }
      }
      lcp_configure (h);
      break;

    case LCP_CONF_REJ:
      /* Client Rejected our request */
      if ((LCP_FRAME(frame)->Id) != lcp_ctl->RespId) {
        /* This is a wrong reject */
        DEBUGF (PPP," Discarded, Wrong Reject\n");
        EvrNetPPP_LcpWrongRejectReceived (LCP_FRAME(frame)->Id, lcp_ctl->RespId);
        break;
      }
      ri = LCP_HEADER_LEN;
      while (ri < ntohs(LCP_FRAME(frame)->Len)) {
        /* Loop through all the options */
        switch (PPP_FRAME(frame)->Data[ri]) {
          case LCP_OPT_ACCMAP:
            /* Async Control Character Map */
            DEBUGF (PPP," Opt ACC-Map\n");
            EvrNetPPP_LcpOptionCharMap (PPP_FRAME(frame)->Data[ri+2]);
            lcp_ctl->RecvChMap = 0xFFFFFFFF;
            lcp_ctl->Flags    |= LCP_FLAG_ACCMAP;
            ri += 6;
            break;

          case LCP_OPT_PFC:
            /* Protocol Field Compression */
            DEBUGF (PPP," Opt PFC\n");
            EvrNetPPP_LcpOptionPfc ();
            lcp_ctl->Flags |= LCP_FLAG_PFC;
            ri += 2;
            break;

          case LCP_OPT_ACFC:
            /* Address and Control Field Compression */
            DEBUGF (PPP," Opt ACFC\n");
            EvrNetPPP_LcpOptionAcfc ();
            lcp_ctl->Flags |= LCP_FLAG_ACFC;
            ri += 2;
            break;

          case LCP_OPT_MAGIC:
            /* Magic Number */
            DEBUGF (PPP," Opt Magic-Num\n");
            EvrNetPPP_LcpOptionMagicNumber (PPP_FRAME(frame)->Data[ri+2]);
            lcp_ctl->MyMagic = 0x00000000;
            lcp_ctl->Flags  |= LCP_FLAG_MAGIC;
            ri += 6;
            break;

          case LCP_OPT_AUTHP:
            /* Authentication Protocol Rejected, terminate PPP */
            ERRORF (PPP,"LCP_Process, Config Auth rejected\n");
            EvrNetPPP_LcpConfigAuthFailed ();
            net_lcp_terminate (h);
            return;
        }
      }
      lcp_configure (h);
      break;

    case LCP_ECHO_RQ:
      /* Echo Request received */
      if (!(ctrl->Flags & PPP_FLAG_OPENED)) {
        /* Silently discard if not in LCP opened state */
        break;
      }
      tmp = get_u32 (&PPP_FRAME(frame)->Data[LCP_HEADER_LEN]);
      DEBUGF (PPP," Magic-Num=0x%08X\n",tmp);
      EvrNetPPP_LcpEchoMagicNumber (tmp);
      if (tmp != lcp_ctl->PeerMagic) {
        DEBUGF (PPP," Discarded, Wrong Magic-Num\n");
        EvrNetPPP_LcpWrongPeerMagicNumber (tmp, lcp_ctl->PeerMagic);
        break;
      }
      txfrm = net_mem_alloc (frame->length);
      memcpy (&PPP_FRAME(txfrm)->Data, &PPP_FRAME(frame)->Data,
                                       ntohs(LCP_FRAME(frame)->Len));
      LCP_FRAME(txfrm)->Code = LCP_ECHO_REP;
      /* Reply with my magic number */
      set_u32 (&PPP_FRAME(txfrm)->Data[LCP_HEADER_LEN], lcp_ctl->MyMagic);
      DEBUGF (PPP,"LCP Send Echo Reply\n");
      EvrNetPPP_LcpSendEchoReply ();
      lcp_send (h, txfrm);
      break;

    case LCP_ECHO_REP:
      /* Echo Reply received */
      tmp = get_u32 (&PPP_FRAME(frame)->Data[LCP_HEADER_LEN]);
      DEBUGF (PPP," Magic-Num=0x%08X\n",tmp);
      EvrNetPPP_LcpEchoMagicNumber (tmp);
      if (((LCP_FRAME(frame)->Id) != lcp_ctl->RespId) || (tmp != lcp_ctl->PeerMagic)) {
        DEBUGF (PPP," Discarded, Wrong Reply\n");
        EvrNetPPP_LcpWrongEchoReplyReceived (LCP_FRAME(frame)->Id, lcp_ctl->RespId);
        break;
      }
      lcp_ctl->Flags &= ~LCP_FLAG_ECHO;
      lcp_ctl->Delay  = 0;
      break;

    case LCP_TERM_RQ:
      /* Terminate Request received */
      txfrm = net_mem_alloc (frame->length);
      memcpy (&PPP_FRAME(txfrm)->Data, &PPP_FRAME(frame)->Data,
                                       ntohs(LCP_FRAME(frame)->Len));
      DEBUGF (PPP,"LCP Send Terminate Ack\n");
      EvrNetPPP_LcpSendTerminateAck ();
      LCP_FRAME(txfrm)->Code = LCP_TERM_ACK;
      lcp_send (h, txfrm);
      net_ppp_proto_init (h, true);
      break;

    case LCP_TERM_ACK:
      /* Terminate Ack received */
      net_ppp_proto_init (h, true);
      break;

    case LCP_DISC_RQ:
      /* Discard Request received. Allowed when LCP in LCP_Opened state */
      if ((ctrl->Flags & PPP_FLAG_LINKUP) &&
          get_u32 (&PPP_FRAME(frame)->Data[LCP_HEADER_LEN]) == lcp_ctl->PeerMagic) {
        break;
      }
      __FALLTHROUGH;

    default:
      /* Reject unsupported LCP Code */
      net_ppp_cp_code_reject (h, frame, PPP_PROT_LCP);
      break;
  }
}

/**
  \brief       Run LCP protocol main function.
  \param[in]   h  PPP interface handle.
  \note        Function is called on every system tick.
*/
void net_lcp_run (NET_PPP_CFG *h) {
  if (ctrl->Flags & PPP_FLAG_LCP_CONF) {
    if (ctrl->th.Flags & PPP_TFLAG_ONLINE) {
      /* Start LCP negotiation for PPP client mode */
      ctrl->Flags &= ~PPP_FLAG_LCP_CONF;
      lcp_configure (h);
      return;
    }
  }

  if (lcp_ctl->Delay == 0) {
    return;
  }
  if (--lcp_ctl->Delay == 0) {
    if (lcp_ctl->Retries != 0) {
      lcp_ctl->Retries--;
      if (lcp_ctl->Flags & LCP_FLAG_SHUTDN) {
        /* Retransmit Terminate request */
        lcp_term_rq (h);
        return;
      }
      if (lcp_ctl->Flags & LCP_FLAG_ECHO) {
        /* Retransmit Echo request */
        lcp_echo_rq (h);
        return;
      }
      /* Retransmit Config. request, keep Code-Id */
      lcp_conf_rq (h);
      return;
    }
    if (lcp_ctl->Flags & LCP_FLAG_SHUTDN) {
      /* All retries used up, shutdown PPP */
      net_ppp_proto_init (h, true);
      return;
    }
    /* All retries used up, terminate */
    net_lcp_terminate (h);
  }
}

#ifdef Network_Debug_STDIO
/**
  \brief       Debug print LCP character map information.
  \param[in]   ch_map  character map bit-mask.
*/
static void debug_info (uint32_t ch_map) {
  DEBUGF (PPP," Opt ACC-Map=0x%04X-%04X\n",(uint16_t)(ch_map>>16),(uint16_t)ch_map);
}

/**
  \brief       Debug print LCP header information.
  \param[in]   frm     network frame.
  \param[in]   dummmy  unused parameter.
*/
static void debug_inf2 (const NET_FRAME *frm, int32_t dummy) {
  static const char p_asc[][9] = {
    "CONF-REQ",
    "CONF-ACK",
    "CONF-NAK",
    "CONF-REJ",
    "TERM-REQ",
    "TERM-ACK",
    "CODE-REJ",
    "PROT-REJ",
    "ECHO-REQ",
    "ECHO-REP",
    "DISC-REQ"
  };
  static const uint8_t p_bin[] = {
    LCP_CONF_RQ,
    LCP_CONF_ACK,
    LCP_CONF_NAK,
    LCP_CONF_REJ,
    LCP_TERM_RQ,
    LCP_TERM_ACK,
    LCP_CODE_REJ,
    LCP_PROT_REJ,
    LCP_ECHO_RQ,
    LCP_ECHO_REP,
    LCP_DISC_RQ
  };
  uint32_t i;

  (void)dummy;
  for (i = 0; i < sizeof (p_bin); i++) {
    if (p_bin[i] == LCP_FRAME(frm)->Code) {
      DEBUGF (PPP," Code %s, Id=0x%02X\n",p_asc[i],LCP_FRAME(frm)->Id);
      goto d;
    }
  }
  DEBUGF (PPP," Code (0x%02X), Id=0x%02X\n",LCP_FRAME(frm)->Code,
                                            LCP_FRAME(frm)->Id);
d:DEBUGF (PPP," Length %d bytes\n",ntohs(LCP_FRAME(frm)->Len));
}
#endif
