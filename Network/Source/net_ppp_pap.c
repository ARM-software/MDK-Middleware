/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ppp_pap.c
 * Purpose: PPP Password Authentication
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_common.h"
#include "net_mem.h"
#include "net_ppp.h"
#include "net_ppp_lcp.h"
#include "net_ppp_pap.h"
#include "net_dbg.h"

/* Local variables */
static NET_PAP_CTRL ppp0_pap_control;

/* Code shortening macros */
#define ctrl       (h->Ctrl)
#define pap_ctl    ((NET_PAP_CTRL *)h->PapAuth->Ctrl)

/* Exported functions */
static void pap_init (NET_PPP_CFG *h);
static void pap_uninit (NET_PPP_CFG *h);
static void pap_process (NET_PPP_CFG *h, NET_FRAME *frame);
static void pap_run (NET_PPP_CFG *h);

/* Local functions */
static bool str_compare (const char *user_s, const char *prot_s);
static void pap_auth_rq (NET_PPP_CFG *h);
static void pap_send (NET_PPP_CFG *h, NET_FRAME *frame);
#ifdef DEBUG_STDIO
 static void debug_info (const NET_FRAME *frm);
#endif

/**
  \brief       Initialize PAP protocol for PPP.
  \param[in]   h  PPP interface handle.
  \return      none.
*/
static void pap_init (NET_PPP_CFG *h) {
  DEBUGF (PPP,"PAP_Init\n");
  EvrNetPPP_PapInit ();
  pap_ctl->Delay = 0;
}

/**
  \brief       De-initialize PAP protocol for PPP.
  \param[in]   h  PPP interface handle.
  \return      none.
*/
static void pap_uninit (NET_PPP_CFG *h) {
  DEBUGF (PPP,"PAP_Uninit\n");
  EvrNetPPP_PapUninit ();
  memset (pap_ctl, 0, sizeof(*pap_ctl));
}

/**
  \brief       Compare user string with protocol string.
  \param[in]   user_s  user, null-terminated string.
  \param[in]   prot_s  protocol, length encoded string.
  \return      check result:
               - true  = strings match,
               - false = strings are different.
*/
static bool str_compare (const char *user_s, const char *prot_s) {
  uint32_t len;

  len = strlen (user_s);
  if (len != prot_s[0]) {
    /* Different sizes */
    return (false);
  }
  if (memcmp (user_s, &prot_s[1], len) == 0) {
    return (true);
  }
  return (false);
}

/**
  \brief       Send PAP authenticate request in client mode.
  \param[in]   h  PPP interface handle.
  \return      none.
*/
static void pap_auth_rq (NET_PPP_CFG *h) {
  NET_FRAME *txfrm;
  uint8_t len1,len2,len;

  DEBUGF (PPP,"PAP_Auth_Rq send\n");
  DEBUGF (PPP," Peer-ID : %s\n",ctrl->User);
  DEBUGF (PPP," Password: %s\n",ctrl->Passw);
  len1 = strlen (ctrl->User)  & 0xFF;
  len2 = strlen (ctrl->Passw) & 0xFF;
  EvrNetPPP_PapSendAuthRequest (ctrl->User, len1);
  EvrNetPPP_PapShowPassword (ctrl->Passw, len2);

  len  = PAP_HEADER_LEN + 2 + len1 + len2;
  txfrm = net_mem_alloc (PPP_FRM_OFFS + PPP_HEADER_LEN + len);
  PAP_FRAME(txfrm)->Code = PAP_AUTH_RQ;
  PAP_FRAME(txfrm)->Id   = pap_ctl->RespId;
  PAP_FRAME(txfrm)->Len  = htons(len);
  PPP_FRAME(txfrm)->Data[PAP_HEADER_LEN] = len1;
  memcpy (&PPP_FRAME(txfrm)->Data[PAP_HEADER_LEN + 1], ctrl->User, len1);
  PPP_FRAME(txfrm)->Data[PAP_HEADER_LEN + len1 + 1] = len2;
  memcpy (&PPP_FRAME(txfrm)->Data[PAP_HEADER_LEN + len1 + 2], ctrl->Passw, len2);
  pap_send (h, txfrm);

  /* Now start the time out */
  pap_ctl->Delay = h->RetryTout;
}

/**
  \brief       Send PAP frame.
  \param[in]   h      PPP interface handle.
  \param[in]   frame  network frame.
  \return      none.
*/
static void pap_send (NET_PPP_CFG *h, NET_FRAME *frame) {
  DEBUGF (PPP,"Sending PAP frame\n");
  DEBUG_INFO (frame);
  EvrNetPPP_PapSendFrame (PAP_FRAME(frame));
  net_ppp_send_proto (h, frame, PPP_PROT_PAP);
  net_mem_free (frame);
}

/**
  \brief       Process received PAP frame.
  \param[in]   h      PPP interface handle.
  \param[in]   frame  network frame.
  \return      none.
*/
static void pap_process (NET_PPP_CFG *h, NET_FRAME *frame) {
  NET_FRAME *txfrm;
  uint8_t len;

  DEBUGF (PPP,"*** Process_frame PAP ***\n");
  DEBUG_INFO (frame);
  EvrNetPPP_PapReceiveFrame (PAP_FRAME(frame));

  switch (PAP_FRAME(frame)->Code) {
    case PAP_AUTH_RQ:
      /* Autentication Request received */
      if (ctrl->Flags & PPP_FLAG_CLIENT) {
        /* Silently ignore PAP requests for client mode */
        break;
      }
      len = PPP_FRAME(frame)->Data[PAP_HEADER_LEN];
      if (str_compare (ctrl->User, (char *)&PPP_FRAME(frame)->Data[PAP_HEADER_LEN])    &&
          str_compare (ctrl->Passw, (char *)&PPP_FRAME(frame)->Data[PAP_HEADER_LEN+1 + len])) {
        ctrl->Flags |= PPP_FLAG_OPENED;
        DEBUGF (PPP," Login success\n");
        EvrNetPPP_PapLoginSuccess ();
      }
      else {
        ERRORF (PPP,"PAP_Process, Login failed\n");
        EvrNetPPP_PapLoginFailed ();
      }
      txfrm = net_mem_alloc (frame->length);
      PAP_FRAME(txfrm)->Code = (ctrl->Flags & PPP_FLAG_OPENED) ? PAP_AUTH_ACK : PAP_AUTH_NAK;
      memcpy (&PAP_FRAME(txfrm)->Id, &PAP_FRAME(frame)->Id, ntohs(PAP_FRAME(frame)->Len));
      pap_send (h, txfrm);
      break;

    case PAP_AUTH_ACK:
      /* Our Authentication Request acknowledged */
      if (!(ctrl->Flags & PPP_FLAG_CLIENT)) {
        break;
      }
      if (PAP_FRAME(frame)->Id != pap_ctl->RespId) {
        /* This is a wrong response */
        DEBUGF (PPP," Discarded, Wrong Ack\n");
        EvrNetPPP_PapWrongAckReceived (PAP_FRAME(frame)->Id, pap_ctl->RespId);
        break;
      }
      /* Done, we are Authenticated to remote server */
      DEBUGF (PPP," Login success\n");
      EvrNetPPP_PapLoginSuccess ();
      ctrl->Flags |= PPP_FLAG_OPENED;
      pap_ctl->Delay  = 0;
      break;

    case PAP_AUTH_NAK:
      /* Authentication Failed, terminate PPP */
      if (!(ctrl->Flags & PPP_FLAG_CLIENT)) {
        break;
      }
      if (PAP_FRAME(frame)->Id != pap_ctl->RespId) {
        /* This is a wrong response */
        DEBUGF (PPP," Discarded, Wrong Nak\n");
        EvrNetPPP_PapWrongNakReceived (PAP_FRAME(frame)->Id, pap_ctl->RespId);
        break;
      }
      ERRORF (PPP,"PAP_Process, Login failed\n");
      EvrNetPPP_PapLoginFailed ();
      net_lcp_terminate (h);
      break;
  }
}

/**
  \brief       Run PAP protocol main function.
  \param[in]   h  PPP interface handle.
  \return      none.
  \note        Function is called on every system tick.
*/
static void pap_run (NET_PPP_CFG *h) {
  if (ctrl->Flags & PPP_FLAG_AUTH_REQ) {
    if ((ctrl->Flags & PPP_FLAG_LINKUP) && (ctrl->Auth == PPP_ATYPE_PAP)) {
      /* LCP negotiation finished, start PAP authentication */
      ctrl->Flags      &= ~PPP_FLAG_AUTH_REQ;
      pap_ctl->RespId  = ctrl->Id++;
      pap_ctl->Retries = h->MaxRetry;
      pap_auth_rq (h);
      return;
    }
  }

  if (pap_ctl->Delay == 0) {
    return;
  }
  if (--pap_ctl->Delay == 0) {
    /* Retransmit Authentication request */
    if (pap_ctl->Retries != 0) {
      pap_ctl->Retries--;
      pap_auth_rq (h);
      return;
    }
    /* All retries used up, terminate PPP */
    net_lcp_terminate (h);
  }
}

#ifdef DEBUG_STDIO
/**
  \brief       Debug print PAP header information.
  \param[in]   frm  network frame.
  \return      none.
*/
static void debug_info (const NET_FRAME *frm) {
  static const char p_asc[][9] = {
    "AUTH-REQ",
    "AUTH-ACK",
    "AUTH-NAK"
  };
  static const uint8_t p_bin[] = {
    PAP_AUTH_RQ,
    PAP_AUTH_ACK,
    PAP_AUTH_NAK
  };
  uint32_t i;

  for (i = 0; i < sizeof (p_bin); i++) {
    if (p_bin[i] == PAP_FRAME(frm)->Code) {
      DEBUGF (PPP," Code %s, Id=0x%02X\n",p_asc[i],PAP_FRAME(frm)->Id);
      goto d;
    }
  }
  DEBUGF (PPP," Code (0x%02X), Id=0x%02X\n",PAP_FRAME(frm)->Code,
                                            PAP_FRAME(frm)->Id);
d:DEBUGF (PPP," Length %d bytes\n",ntohs(PAP_FRAME(frm)->Len));
}
#endif

/* Exported ppp0 PAP authentication */
NET_PPP_AUTH net_ppp0_pap_auth = {
  &ppp0_pap_control,
  pap_init,
  pap_uninit,
  pap_run,
  pap_process
};
