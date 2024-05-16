/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_smtp_client_legacy.c
 * Purpose: Mail Transfer Client legacy non-blocking mode
 *----------------------------------------------------------------------------*/

#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_addr.h"
#include "net_smtp_client.h"
#include "net_dbg.h"

/* Local variables */

/* Compose callback functions */
static uint32_t compose_header (char *buf, uint32_t buf_len);
static uint32_t compose_body   (char *buf, uint32_t buf_len);

/**
  \brief       Connect to SMTP server and start sending an email.
  \param[in]   addr  structure containing server IP address and port.
  \return      status code as defined with netStatus.
  \note        Non-blocking send email function, legacy mode.
*/
netStatus netSMTPc_Connect (const NET_ADDR *addr) {
  /* User API functions in "SMTP_Client_UIF.c" */
  static const NET_SMTP_UIF uif_legacy = {
    netSMTPc_Process, 
    netSMTPc_AcceptAuthentication,
    netSMTPc_Notify
  };

  START_LOCK (netStatus);

  DEBUGF (SMTP,"Connect Client\n");
  if (net_addr_is_unspec ((const __ADDR *)addr)) {
    ERRORF (SMTP,"Connect, Invalid parameter\n");
    EvrNetSMTP_ConnectInvalidParameter ();
    RETURN (netInvalidParameter);
  }
  if (smtp->State != SMTP_STATE_IDLE) {
    ERRORF (SMTP,"Connect, Client busy\n");
    EvrNetSMTP_ConnectClientBusy ();
    RETURN (netWrongState);
  }

  /* Copy server address and port */
  net_addr_copy (&smtp->Server, (const __ADDR *)addr);

  /* Set callback and user interface functions */
  smtp->uif       = &uif_legacy;
  smtp->f_compose = compose_header;

  /* Start SMTP client */
  smtp->NumRcpt = 1;
  net_smtp_client_start (0);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Compose mail header callback.
  \param[out]  buf      output buffer to write the data to.
  \param[in]   buf_len  length of the output buffer in bytes.
  \return      number of bytes written to output buffer.
*/
static uint32_t compose_header (char *buf, uint32_t buf_len) {
  uint32_t len;

  /* Add 'From' address */
  len  = net_strcpy2 (buf, "From: ");
  len += smtp->uif->process (netSMTPc_RequestSender,
                             buf+len, buf_len-len, &smtp->SVar);
  /* Add 'To" addresses */
  len += net_strcpy2 (buf+len, "\r\nTo: ");
  for (smtp->SVar = 0;;) {
    len += smtp->uif->process (netSMTPc_RequestRecipient,
                               buf+len, buf_len-len, &smtp->SVar);
    /* End if recipients processed, or buffer filled with data */
    if ((++smtp->SVar == smtp->NumRcpt) || ((len + 240) > buf_len)) {
      break;
    }
    len += net_strcpy2 (buf+len, ",\r\n    ");
  }
  len += net_strcpy2 (buf+len, "\r\nSubject: ");
  len += smtp->uif->process (netSMTPc_RequestSubject,
                             buf+len, buf_len-len, NULL);
  len += net_strcpy2 (buf+len, "\r\n\r\n");
  /* Next step: Compose body */
  smtp->f_compose = compose_body;
  smtp->SVar      = 0;
  return (len);
}

/**
  \brief       Compose email body callback.
  \param[out]  buf      output buffer to write the data to.
  \param[in]   buf_len  length of the output buffer in bytes.
  \return      number of bytes written to output buffer.
*/
static uint32_t compose_body (char *buf, uint32_t buf_len) {
  uint32_t len,v;

  /* Repeat to use as much buffer as possible */
  for (len = 0; (len + 240) < buf_len; ) {
    v = smtp->uif->process (netSMTPc_RequestBody,
                            buf+len, buf_len-len, &smtp->SVar);
    len += (v & 0xFFFF);
    /* Bit-31 of return value is a repeat flag */
    if (!(v & 0x80000000) || !(v & 0xFFFF)) {
      /* Repeat flag not set or 0-length returned */
      smtp->Flags |= SMTP_FLAG_DATAEND;
      break;
    }
  }
  return (len);
}

/* Library default function(s) */
__WEAK uint32_t netSMTPc_Process (netSMTPc_Request request, char *buf,
                                  uint32_t buf_len, uint32_t *pvar) {
  (void)request; (void)buf; (void)buf_len; (void)pvar; return (0); }
__WEAK bool netSMTPc_AcceptAuthentication (const NET_ADDR *addr) {
  (void)addr; return (false); }
__WEAK void netSMTPc_Notify (netSMTPc_Event event) {
  (void)event; }

