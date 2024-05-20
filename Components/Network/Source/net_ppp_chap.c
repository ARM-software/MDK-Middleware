/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ppp_chap.c
 * Purpose: PPP Challenge-Handshake Authentication
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_mem.h"
#include "net_md5.h"
#include "net_ppp.h"
#include "net_ppp_lcp.h"
#include "net_ppp_chap.h"
#include "net_dbg.h"

/* Local variables */
static NET_CHAP_CTRL ppp0_chap_control;

/* Code shortening macros */
#define ctrl       (h->Ctrl)
#define chap_ctl   ((NET_CHAP_CTRL *)h->ChapAuth->Ctrl)

/* Exported functions */
static void chap_init (NET_PPP_CFG *h);
static void chap_uninit (NET_PPP_CFG *h);
static void chap_process (NET_PPP_CFG *h, NET_FRAME *frame);
static void chap_run (NET_PPP_CFG *h);

/* Local functions */
static void chap_auth_rq (NET_PPP_CFG *h);
static void chap_send (NET_PPP_CFG *h, NET_FRAME *frame);
#ifdef Network_Debug_STDIO
 static void debug_info (const NET_FRAME *frm);
#endif

/**
  \brief       Initialize CHAP protocol for PPP.
  \param[in]   h  PPP interface handle.
*/
static void chap_init (NET_PPP_CFG *h) {
  DEBUGF (PPP,"CHAP_Init\n");
  EvrNetPPP_ChapInit ();
  chap_ctl->Delay = 0;
}

/**
  \brief       De-initialize CHAP protocol for PPP.
  \param[in]   h  PPP interface handle.
*/
static void chap_uninit (NET_PPP_CFG *h) {
  DEBUGF (PPP,"CHAP_Uninit\n");
  EvrNetPPP_ChapUninit ();
  memset (chap_ctl, 0, sizeof(*chap_ctl));
}

/**
  \brief       Send Authenticate Challenge request in server mode.
  \param[in]   h  PPP interface handle.
*/
static void chap_auth_rq (NET_PPP_CFG *h) {
  NET_FRAME *txfrm;
  NET_MD5_CTX *ctx;
  uint8_t len1, len;

  DEBUGF (PPP,"CHAP_Auth_Rq send\n");
  DEBUGF (PPP," Name: %s\n",sys->HostName);
  len1 = strlen (sys->HostName) & 0xFF;
  EvrNetPPP_ChapSendChallenge (sys->HostName, len1);
  ctx  = __MD5_CTX(net_mem_alloc (sizeof (NET_MD5_CTX)));
  net_md5_init (ctx);
  net_md5_update (ctx, sys->HostName, len1);
  net_md5_update (ctx, (char *)&sys->Ticks, 4);
  net_md5_final (ctx, chap_ctl->ChVal);
  net_mem_free (__FRAME(ctx));

  len  = CHAP_HEADER_LEN + 17 + len1;
  txfrm = net_mem_alloc (PPP_FRM_OFFS + PPP_HEADER_LEN + len);
  CHAP_FRAME(txfrm)->Code = CHAP_CHALLENGE;
  CHAP_FRAME(txfrm)->Id   = chap_ctl->RespId;
  CHAP_FRAME(txfrm)->Len  = htons(len);

  PPP_FRAME(txfrm)->Data[CHAP_HEADER_LEN] = 16;
  memcpy (&PPP_FRAME(txfrm)->Data[CHAP_HEADER_LEN + 1], chap_ctl->ChVal, 16);
  memcpy (&PPP_FRAME(txfrm)->Data[CHAP_HEADER_LEN + 17], sys->HostName, len1);
  chap_send (h, txfrm);

  /* Now start the time out */
  chap_ctl->Delay = h->RetryTout;
}

/**
  \brief       Send CHAP frame.
  \param[in]   h      PPP interface handle.
  \param[in]   frame  network frame.
*/
static void chap_send (NET_PPP_CFG *h, NET_FRAME *frame) {
  DEBUGF (PPP,"Sending CHAP frame\n");
  DEBUG_INFO (frame);
  EvrNetPPP_ChapSendFrame (CHAP_FRAME(frame));
  net_ppp_send_proto (h, frame, PPP_PROT_CHAP);
  net_mem_free (frame);
}

/**
  \brief       Process received CHAP frame.
  \param[in]   h      PPP interface handle.
  \param[in]   frame  network frame.
*/
static void chap_process (NET_PPP_CFG *h, NET_FRAME *frame) {
  NET_FRAME *txfrm;
  NET_MD5_CTX *ctx;
  uint8_t len, len1;

  DEBUGF (PPP,"*** Process_frame CHAP ***\n");
  DEBUG_INFO (frame);
  EvrNetPPP_ChapReceiveFrame (CHAP_FRAME(frame));

  switch (CHAP_FRAME(frame)->Code) {
    case CHAP_CHALLENGE:
      /* Challenge received */
      if (!(ctrl->Flags & PPP_FLAG_CLIENT)) {
        /* Silently ignore challenges for server mode */
        break;
      }
      /* Save the challenge identifier */ 
      chap_ctl->RespId = CHAP_FRAME(frame)->Id;
      /* Calculate the response digest value */
      len1 = strlen (ctrl->Passw) & 0xFF;
      ctx = __MD5_CTX(net_mem_alloc (sizeof (NET_MD5_CTX)));
      net_md5_init (ctx);
      net_md5_update (ctx, (char *)&CHAP_FRAME(frame)->Id, 1);
      net_md5_update (ctx, ctrl->Passw, len1);
      /* Length of the challenge value */
      len = PPP_FRAME(frame)->Data[CHAP_HEADER_LEN];
      net_md5_update (ctx, (char *)&PPP_FRAME(frame)->Data[CHAP_HEADER_LEN + 1], len);
      /* Store the digest value */
      net_md5_final (ctx, chap_ctl->ChVal);
      net_mem_free (__FRAME(ctx));

      len1 = strlen (ctrl->User) & 0xFF;
      len  = CHAP_HEADER_LEN + 17 + len1;
      txfrm = net_mem_alloc (PPP_FRM_OFFS + PPP_HEADER_LEN + len);
      CHAP_FRAME(txfrm)->Code = CHAP_RESPONSE;
      CHAP_FRAME(txfrm)->Id   = chap_ctl->RespId;
      CHAP_FRAME(txfrm)->Len  = htons(len);
      PPP_FRAME(txfrm)->Data[CHAP_HEADER_LEN] = 16;
      memcpy (&PPP_FRAME(txfrm)->Data[CHAP_HEADER_LEN + 1], chap_ctl->ChVal, 16);
      memcpy (&PPP_FRAME(txfrm)->Data[CHAP_HEADER_LEN + 17], ctrl->User, len1);
      chap_send (h, txfrm);
      break;

    case CHAP_RESPONSE:
      /* Response to our challenge received */
      if (ctrl->Flags & PPP_FLAG_CLIENT) {
        break;
      }
      if (CHAP_FRAME(frame)->Id != chap_ctl->RespId) {
        /* This is a wrong response */
        DEBUGF (PPP," Discarded, Wrong RESPONSE\n");
        EvrNetPPP_ChapWrongResponseReceived (CHAP_FRAME(frame)->Id, chap_ctl->RespId);
        break;
      }

      /* Check Username first (plain ascii) */
      len = strlen (ctrl->User) & 0xFF;
      if (ntohs(CHAP_FRAME(frame)->Len) != (CHAP_HEADER_LEN + 17 + len)) {
        goto fail;
      }
      if (!net_strcmp ((char *)&PPP_FRAME(frame)->Data[CHAP_HEADER_LEN + 17], ctrl->User)) {
        goto fail;
      }
      /* Check the response digest value */
      len = strlen (ctrl->Passw) & 0xFF;
      ctx = __MD5_CTX(net_mem_alloc (sizeof (NET_MD5_CTX)));
      net_md5_init (ctx);
      net_md5_update (ctx, (char *)&chap_ctl->RespId, 1);
      net_md5_update (ctx, ctrl->Passw, len);
      net_md5_update (ctx, (char *)chap_ctl->ChVal, 16);
      net_md5_final (ctx, chap_ctl->ChVal);
      net_mem_free (__FRAME(ctx));

      if (memcmp (&PPP_FRAME(frame)->Data[CHAP_HEADER_LEN + 1], chap_ctl->ChVal, 16) == 0) {
        /* Done, remote client is allowed to login */
        ctrl->Flags |= PPP_FLAG_OPENED;
        chap_ctl->Delay = 0;
        DEBUGF (PPP," Login success\n");
        EvrNetPPP_ChapLoginSuccess ();
      }
      else {
fail:   ERRORF (PPP,"CHAP_Process, Login failed\n");
        EvrNetPPP_ChapLoginFailed ();
      }
      txfrm = net_mem_alloc (PPP_FRM_OFFS + PPP_HEADER_LEN + CHAP_HEADER_LEN);
      CHAP_FRAME(txfrm)->Code = (ctrl->Flags & PPP_FLAG_OPENED) ? CHAP_SUCCESS : CHAP_FAILED;
      CHAP_FRAME(txfrm)->Id   = chap_ctl->RespId;
      CHAP_FRAME(txfrm)->Len  = HTONS(4);
      chap_send (h, txfrm);
      break;

    case CHAP_SUCCESS:
      /* Authentication success */
      if (!(ctrl->Flags & PPP_FLAG_CLIENT)) {
        break;
      }
      if (CHAP_FRAME(frame)->Id != chap_ctl->RespId) {
        /* This is a wrong response */
        DEBUGF (PPP," Discarded, Wrong SUCCESS\n");
        EvrNetPPP_ChapWrongSuccessReceived (CHAP_FRAME(frame)->Id, chap_ctl->RespId);
        break;
      }
      DEBUGF (PPP," Login success\n");
      EvrNetPPP_ChapLoginSuccess ();
      ctrl->Flags |= PPP_FLAG_OPENED;
      chap_ctl->Delay = 0;
      break;

    case CHAP_FAILED:
      /* Authentication failed, terminate PPP */
      if (!(ctrl->Flags & PPP_FLAG_CLIENT)) {
        break;
      }
      if (CHAP_FRAME(frame)->Id != chap_ctl->RespId) {
        /* This is a wrong response */
        DEBUGF (PPP," Discarded, Wrong FAILURE\n");
        EvrNetPPP_ChapWrongFailureReceived (CHAP_FRAME(frame)->Id, chap_ctl->RespId);
        break;
      }
      ERRORF (PPP,"CHAP_Process, Login failed\n");
      EvrNetPPP_ChapLoginFailed ();
      net_lcp_terminate (h);
      break;
  }
}

/**
  \brief       Run CHAP protocol main function.
  \param[in]   h  PPP interface handle.
  \note        Function is called on every system tick.
*/
static void chap_run (NET_PPP_CFG *h) {
  if (ctrl->Flags & PPP_FLAG_AUTH_REQ) {
    if ((ctrl->Flags & PPP_FLAG_LINKUP) && (ctrl->Auth == PPP_ATYPE_CHAP)) {
      /* LCP negotiation finished, start CHAP challenge */
      ctrl->Flags  &= ~PPP_FLAG_AUTH_REQ;
      chap_ctl->RespId  = ctrl->Id++;
      chap_ctl->Retries = h->MaxRetry;
      chap_auth_rq (h);
      return;
    }
  }
  if (chap_ctl->Delay == 0) {
    return;
  }
  if (--chap_ctl->Delay == 0) {
    /* Retransmit Authentication request */
    if (chap_ctl->Retries != 0) {
      chap_ctl->Retries--;
      chap_auth_rq (h);
      return;
    }
    /* All retries used up, terminate PPP */
    net_lcp_terminate (h);
  }
}

#ifdef Network_Debug_STDIO
/**
  \brief       Debug print CHAP header information.
  \param[in]   frm  network frame.
*/
static void debug_info (const NET_FRAME *frm) {
  static const char p_asc[][10] = {
    "CHALLENGE",
    "RESPONSE",
    "SUCCESS",
    "FAILURE"
  };
  static const uint8_t p_bin[] = {
    CHAP_CHALLENGE,
    CHAP_RESPONSE,
    CHAP_SUCCESS,
    CHAP_FAILED
  };
  uint32_t i;

  for (i = 0; i < sizeof (p_bin); i++) {
    if (p_bin[i] == CHAP_FRAME(frm)->Code) {
      DEBUGF (PPP," Code %s, Id=0x%02X\n",p_asc[i],CHAP_FRAME(frm)->Id);
      goto d;
    }
  }
  DEBUGF (PPP," Code (0x%02X), Id=0x%02X\n",CHAP_FRAME(frm)->Code,
                                            CHAP_FRAME(frm)->Id);
d:DEBUGF (PPP," Length %d bytes\n",ntohs(CHAP_FRAME(frm)->Len));
}
#endif

/* Exported ppp0 CHAP authentication */
NET_PPP_AUTH net_ppp0_chap_auth = {
  &ppp0_chap_control,
  chap_init,
  chap_uninit,
  chap_run,
  chap_process
};
