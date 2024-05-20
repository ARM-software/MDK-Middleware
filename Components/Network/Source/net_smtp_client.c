/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_smtp_client.c
 * Purpose: Mail Transfer Client advanced blocking mode
 *----------------------------------------------------------------------------*/

#include "net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_mem.h"
#include "net_md5.h"
#include "net_base64.h"
#include "net_dns_client.h"
#include "net_smtp_client.h"
#include "net_dbg.h"

/* Local variables */
static NET_SMTP_TAG  p_tag;                 // Parameter parser tag structure
static NET_SMTP_INFO smtp_control_2;
#define smtp2      (&smtp_control_2)

/* Interface functions */
#define attach_io   (smtpc->attach_if)      // Attachment support interface
#define fs_io       (&smtpc->fs_if)         // File system interface
#define tls_io      (smtpc->tls_if)         // mbedTLS interface

/* Exported functions */
static uint32_t smtp_mail_attach (char *buf, uint32_t buf_len);
static uint32_t smtp_mime_header (char *buf, const void *ftag);
static const char *smtp_parse_fname (const char *buf, void *ftag);

/* Local Functions */
static uint32_t smtp_process (netSMTPc_Request request,
                              char *buf, uint32_t buf_len, uint32_t *pvar);
static bool     smtp_accept_authentication (const NET_ADDR *addr);
static void     smtp_notify (netSMTPc_Event event);
static uint32_t mime_add_footer (char *buf);
static uint32_t check_addr (const NET_SMTP_MAIL *mail);
static uint32_t check_attachment (const NET_SMTP_MAIL *mail);
static const char *parse_addr (const char *buf, NET_SMTP_TAG *tag);
static uint32_t mime_encode_word (char *buf, const char *sp, uint32_t len);
static uint32_t mime_encode_line_qp (char *buf);
static uint32_t mime_encode_line_7bit (char *buf);
static bool     check_8bit (const NET_SMTP_MAIL *mail);
static uint32_t add_charset (char *buf, const NET_SMTP_MAIL *mail);
static uint32_t add_encoding (char *buf, bool is_8bit);
static uint32_t add_address (char *buf, const NET_SMTP_TAG *tag);
static uint32_t add_boundary (char *buf);
static void     compute_hash (uint8_t *buf);

/* Compose callback functions */
static uint32_t compose_header (char *buf, uint32_t buf_len);
static uint32_t compose_body   (char *buf, uint32_t buf_len);

/**
  \brief       Send email in blocking mode.
  \param[in]   mail  pointer to email descriptor.
  \param[in]   mta   mail transfer agent address and user credentials.
  \return      status code as defined with netStatus.
*/
netStatus netSMTPc_SendMail (const NET_SMTP_MAIL *mail, const NET_SMTP_MTA *mta) {
  /* New internal user interface functions */
  static const NET_SMTP_UIF uif_new = {
    smtp_process,
    smtp_accept_authentication,
    smtp_notify
  };
  netStatus res;

  START_LOCK (netStatus);

  if ((mail == NULL) || (mail->Subject == NULL) ||
      (mta  == NULL) || (mta->Address  == NULL)) {
    ERRORF (SMTP,"SendMail, Invalid parameter\n");
    EvrNetSMTP_SendMailInvalidParameter ();
    RETURN (netInvalidParameter);
  }
  if ((mta->Flags & NET_SMTP_MTA_USETLS) && (tls_io == NULL)) {
    ERRORF (SMTP,"SendMail, TLS not enabled\n");
    EvrNetSMTP_SendMailTlsNotEnabled ();
    RETURN (netInvalidParameter);
  }
  if (smtp->State != SMTP_STATE_IDLE) {
    ERRORF (SMTP,"SendMail, Client busy\n");
    EvrNetSMTP_SendMailClientBusy ();
    RETURN (netWrongState);
  }
  /* Save function arguments */
  smtp2->mail   = mail;
  smtp2->mta    = mta;

  /* Check number of mail recipients */
  smtp->NumRcpt = check_addr (smtp2->mail) & 0xFF;
  if (smtp->NumRcpt == 0) {
    ERRORF (SMTP,"SendMail, No recipients\n");
    EvrNetSMTP_SendMailNoRecipients ();
    RETURN (netInvalidParameter);
  }
  DEBUGF (SMTP,"SendMail, %d recipients\n",smtp->NumRcpt);
  EvrNetSMTP_SendMail (smtp->NumRcpt);

  /* Resolve IP address of SMTP server or MTA agent */
  res = net_dns_client_resolve (smtp2->mta->Address, NET_ADDR_IP4, &smtp->Server);
  if (res == netInvalidParameter) {
    ERRORF (SMTP,"SendMail, Server not valid\n");
    EvrNetSMTP_SendMailServerNotValid ();
    RETURN (netInvalidParameter);
  }
  if (res != netOK) {
    ERRORF (SMTP,"SendMail, DNS error\n");
    EvrNetSMTP_SendMailDnsError ();
    RETURN (netDnsResolverError);
  }

  /* Server IP address is now resolved, set    */
  /* port and Server name (name used in SMTPS) */
  smtp->Server.port = smtp2->mta->Port;
  smtp->SrvName     = smtp2->mta->Address;

  /* Set compose callback and user interface functions */
  smtp->uif         = &uif_new;
  smtp->f_compose   = compose_header;

  /* Check mail encoding 7bit or 8bit */
  smtp2->Encode8bit = check_8bit (smtp2->mail);

  /* Check mail attachments */
  if (attach_io) {
    smtp2->NumFiles = check_attachment (smtp2->mail) & 0xFF;
    if (smtp2->NumFiles) {
      compute_hash (smtp2->Hash);
    }
  }
  else {
    smtp2->NumFiles = 0;
    if (smtp2->mail->Attachment && smtp2->mail->Attachment[0]) {
      /* Error if support for attachments not enabled */
      ERRORF (SMTP,"SendMail, Attach not enabled\n");
      EvrNetSMTP_SendMailAttachNotEnabled ();
      RETURN (netInvalidParameter);
    }
  }
  smtp2->Attach = false;

  /* Start SMTP client */
  net_smtp_client_start (smtp2->mta->Flags & NET_SMTP_MTA_USETLS);

  /* Suspend this thread, wait for net_sys_resume() */
  smtp->Flags |=  SMTP_FLAG_SUSPEND;
  net_sys_suspend (&smtp2->Thread);
  smtp->Flags &= ~SMTP_FLAG_SUSPEND;

  /* Check if attachment file still open */
  /* Could happen on SMTP client timeout */
  if (smtp2->Attach && smtp2->From) {
    fs_io->fclose (__CONST_CAST(void *)smtp2->From);
  }

  /* Set return code */
  switch ((int32_t)smtp->cb_event) {
    case netSMTPc_EventSuccess:
      res = netOK;
      break;
    case netSMTPc_EventTimeout:
      res = netServerError;
      break;
    case netSMTPc_EventAuthenticationFailed:
      res = netAuthenticationFailed;
      break;
    default:
      res = (smtp->Flags & SMTP_FLAG_NOFILE) ? netFileError : netError;
      break;
  }
  RETURN (res);

  END_LOCK;
}

/**
  \brief       Request parameters callback function.
  \param[in]   request  request code.
  \param[out]  buf      output buffer to write the data to.
  \param[in]   buf_len  length of the output buffer in bytes.
  \param[in,out] pvar   pointer to a session's local buffer of 4 bytes.
  \return      number of bytes written to output buffer.
*/
static uint32_t smtp_process (netSMTPc_Request request,
                              char *buf, uint32_t buf_len, uint32_t *pvar) {
  (void)buf_len;

  switch ((int32_t)request) {
    case netSMTPc_RequestUsername:
      if (!smtp2->mta->Username) break;
      return (net_strcpy2 (buf, smtp2->mta->Username));

    case netSMTPc_RequestPassword:
      if (!smtp2->mta->Password) break;
      return (net_strcpy2 (buf, smtp2->mta->Password));

    case netSMTPc_RequestSender:
      /* Address type: FROM */
      parse_addr (smtp2->From, &p_tag);
      if (p_tag.AddrLen == 0) break;
      return (net_strncpy2 (buf, p_tag.Base+p_tag.AddrOffs, p_tag.AddrLen));

    case netSMTPc_RequestRecipient:
      /* Address type: TO, CC, BCC */
      parse_addr (smtp2->Rcpt[*pvar], &p_tag);
      if (p_tag.AddrLen == 0) break;
      return (net_strncpy2 (buf, p_tag.Base+p_tag.AddrOffs, p_tag.AddrLen));

    case netSMTPc_RequestSubject:
    case netSMTPc_RequestBody:
      /* Never used in blocking mode */
    default:
      break;
  }
  /* Add null-termination */
  buf[0] = 0;
  return (0);
}

/**
  \brief       Accept authentication requested by SMTP server.
  \param[in]   addr   structure containing IP address and port.
  \return      - true  = authentication accepted,
               - false = authentication denied.
*/
static bool smtp_accept_authentication (const NET_ADDR *addr) {
  (void)addr;

  if (smtp2->mta->Username && smtp2->mta->Password) {
    /* Username and password provided, use authentication */
    return (true);
  }
  return (false);
}

/**
  \brief       User event callback notification.
  \param[in]   event  notification event.
*/
static void smtp_notify (netSMTPc_Event event) {
  (void)event;
  /* Resume SendMail thread */
  net_sys_resume (&smtp2->Thread);
}

/**
  \brief       Compose mail header callback.
  \param[out]  buf      output buffer to write the data to.
  \param[in]   buf_len  length of the output buffer in bytes.
  \return      number of bytes written to output buffer.
*/
static uint32_t compose_header (char *buf, uint32_t buf_len) {
  const char *next;
  uint32_t len,v;

  /* Add 'Content-Type' header */
  len = net_strcpy2 (buf, "Content-Type: ");
  if (smtp2->NumFiles != 0) {
    /* Use multipart content for attachments */
    DEBUGF (SMTP," Multipart, attach=%d\n",smtp2->NumFiles);
    EvrNetSMTP_SendMailMultipart (smtp2->NumFiles);
    len += net_strcpy2 (buf+len, "multipart/mixed; boundary=");
    len += add_boundary (buf+len);
  }
  else {
    /* Only mail message, no MIME nesting */
    len += net_strcpy2(buf+len, "text/plain; charset=");
    len += add_charset (buf+len, smtp2->mail);
    /* Add 'Content-Transfer-Encoding' header */
    len += net_strcpy2 (buf+len, "\r\nContent-Transfer-Encoding: ");
    len += add_encoding (buf+len, smtp2->Encode8bit);
    smtp->Flags |= SMTP_FLAG_HEADER;
  }

  /* Add MIME version header */
  len += net_strcpy2 (buf+len, "\r\nMIME-Version: 1.0");

  /* Add 'From' address */
  for (next = smtp2->mail->From; next; ) {
    next = parse_addr (next, &p_tag);
    if (p_tag.AddrLen == 0) {
      /* Skip, address is not valid */
      continue;
    }
    /* Valid 'From' address found */
    len += net_strcpy2 (buf+len, "\r\nFrom: ");
    len += add_address  (buf+len, &p_tag);
    break;
  }

  /* Add 'To' address */
  for (v = 1, next = smtp2->mail->To; next; ) {
    next = parse_addr (next, &p_tag);
    if (p_tag.AddrLen == 0) {
      /* Skip, address is not valid */
      continue;
    }
    if ((len + 240) > buf_len) {
      break;
    }
    len += net_strcpy2 (buf+len, v ? "\r\nTo: " : ",\r\n\t");
    len += add_address  (buf+len, &p_tag);
    v = 0;
  }

  /* Add 'Cc' address */
  for (v = 1, next = smtp2->mail->Cc; next; ) {
    next = parse_addr (next, &p_tag);
    if (p_tag.AddrLen == 0) {
      /* Skip, address is not valid */
      continue;
    }
    if ((len + 240) > buf_len) {
      break;
    }
    len += net_strcpy2 (buf+len, v ? "\r\nCc: " : ",\r\n\t");
    len += add_address (buf+len, &p_tag);
    v = 0;
  }

  /* Add 'Subject' */
  if (smtp2->mail->Subject) {
    len += net_strcpy2 (buf+len, "\r\nSubject: ");
    len += mime_encode_word (buf+len, smtp2->mail->Subject, 120);
  }

  /* Add end of header marker */
  len += net_strcpy2 (buf+len, "\r\n\r\n");

  if (!smtp2->mail->Message && !smtp2->NumFiles) {
    /* Done, no more information available */
    smtp->Flags |= SMTP_FLAG_DATAEND;
    return (len);
  }

  /* Next step: Compose body */
  smtp->f_compose = compose_body;
  smtp->SVar      = 0;
  return (len);
}

/**
  \brief       Compose mail body callback.
  \param[out]  buf      output buffer to write the data to.
  \param[in]   buf_len  length of the output buffer in bytes.
  \return      number of bytes written to output buffer.
*/
static uint32_t compose_body (char *buf, uint32_t buf_len) {
  uint32_t (*encode_line)(char*);
  const char *next;
  uint32_t i,len = 0;

  /* Add MIME header in multipart message */
  if (!(smtp->Flags & SMTP_FLAG_HEADER)) {
    if (attach_io) len += attach_io->mime_header (buf, NULL);
  }

  /* Compose message content */
  encode_line = (smtp2->Encode8bit) ? mime_encode_line_qp :
                                      mime_encode_line_7bit;
  for ( ; (len + 120) < buf_len; ) {
    /* Line is max. 80 characters */
    len += encode_line (buf+len);
    /* Bit-31 of return value is end flag */
    if (len & 0x80000000) {
      break;
    }
  }
  if (!(len & 0x80000000)) {
    /* Compose message in progress */
    return (len);
  }
  len &= 0xFFFF;

  /* Add attachment in multipart message */
  smtp->Flags &= ~SMTP_FLAG_HEADER;
  if (!attach_io || !smtp2->NumFiles) {
    /* Done, not multipart message */
    smtp->Flags |= SMTP_FLAG_DATAEND;
    return (len);
  }

  /* Reuse 'Rcpt' for fname(s) now */
  if (smtp2->NumFiles > SMTP_MAX_RCPT) {
    smtp2->NumFiles = SMTP_MAX_RCPT;
  }
  for (i = 0, next = smtp2->mail->Attachment; next;  ) {
    next = attach_io->parse_fname (next, &p_tag);
    if (p_tag.NameLen && i < SMTP_MAX_RCPT) {
      /* Valid fname found */
      smtp2->Rcpt[i++] = p_tag.Base;
    }
  }

  /* Next step: Compose attachment */
  smtp->f_compose = attach_io->mail_attach;
  smtp->SVar      = 0;
  return (len);
}

/**
  \brief       Compose mail attachment callback.
  \param[out]  buf      output buffer to write the data to.
  \param[in]   buf_len  length of the output buffer in bytes.
  \return      number of bytes written to output buffer.
  \note        Called via function pointer!
*/
static uint32_t smtp_mail_attach (char *buf, uint32_t buf_len) {
  uint32_t rlen,len = 0;

  /* Add MIME header in multipart message */
  if (!(smtp->Flags & SMTP_FLAG_HEADER)) {
    attach_io->parse_fname (smtp2->Rcpt[smtp->SVar], &p_tag);
    len += attach_io->mime_header (buf, &p_tag);
    /* Create null-terminated fname */
    net_strncpy (buf+len+1, p_tag.Base+p_tag.AddrOffs, p_tag.AddrLen);
    DEBUGF (SMTP," Attach \"%s\"\n",buf+len+1);
    EvrNetSMTP_SendMailAttachment (buf+len+1, strlen(buf+len+1));
    /* Reuse 'From' for file pointer now */
    smtp2->From   = fs_io->fopen (buf+len+1);
    smtp2->Attach = true;
  }
  /* Base64 encode attachment content */
  if (!smtp2->From) {
    /* Failed to open a file for attachment */
    ERRORF (SMTP,"Attach failed \"%s\"\n",buf+len+1);
    EvrNetSMTP_SendMailAttachFailed (buf+len+1, strlen(buf+len+1));
    /* Write some text and abort connection */
    len += net_base64_encode (buf+len, "[NoFile]", 8);
    smtp->Flags |= (SMTP_FLAG_ERROR | SMTP_FLAG_NOFILE);
    return (len);
  }
  else {
    for (rlen = 0; (len + 140) < buf_len; ) {
      /* MIME line size limit is 76 characters + CRLF */
      rlen = fs_io->fread (__CONST_CAST(void *)smtp2->From, (uint8_t *)buf+len+80, 57);
      len += net_base64_encode (buf+len, buf+len+80, rlen);
      buf[len]   = '\r';
      buf[len+1] = '\n';
      len += 2;
      /* Check for end of file */
      if (rlen < 57) {
        break;
      }
    }
    if (rlen == 57) {
      /* Buffer is full */
      return (len);
    }
    /* Done with the attachment, close this file */
    smtp2->Attach = false;
    fs_io->fclose (__CONST_CAST(void *)smtp2->From);
  }
  if (++smtp->SVar < smtp2->NumFiles) {
    /* More attachments to add */
    smtp->Flags &= ~SMTP_FLAG_HEADER;
    return (len);
  }
  /* Add MIME footer in multipart message */
  len += mime_add_footer (buf+len);

  /* Compose mail completed */
  smtp->Flags |= SMTP_FLAG_DATAEND;
  return (len);
}

/**
  \brief       Compose multipart MIME header.
  \param[out]  buf   output buffer to write the data to.
  \param[in]   ftag  structure with fname tags or NULL.
  \return      number of bytes written to output buffer.
  \note        Called via function pointer!
*/
static uint32_t smtp_mime_header (char *buf, const void *ftag) {
  const NET_SMTP_TAG *tag = ftag;
  uint32_t len;

  /* Add opening boundary marker */
  len  = net_strcpy2 (buf, "\r\n--");
  len += add_boundary (buf+len);

  /* Add 'Content-Type' header */
  len += net_strcpy2 (buf+len, "\r\nContent-Type: ");
  if (tag) {
    len += net_strcpy2 (buf+len, "application/octet-stream; name=\"");
    len += mime_encode_word (buf+len, tag->Base+tag->NameOffs, tag->NameLen);
    buf[len++] = '"';
  }
  else {
    len += net_strcpy2 (buf+len, "text/plain; charset=");
    len += add_charset (buf+len, smtp2->mail);
  }

  /* Add 'Content-Disposition' header */
  if (tag) {
    len += net_strcpy2 (buf+len, "\r\nContent-Disposition: attachment; filename=\"");
    len += mime_encode_word (buf+len, tag->Base+tag->NameOffs, tag->NameLen);
    buf[len++] = '"';
  }

  /* Add 'Content-Transfer-Encoding' header */
  len += net_strcpy2 (buf+len, "\r\nContent-Transfer-Encoding: ");
  if (tag) {
    len += net_strcpy2 (buf+len, "base64");
  }
  else {
    len += add_encoding (buf+len, smtp2->Encode8bit);
  }

  /* Add end of header marker */
  len += net_strcpy2 (buf+len, "\r\n\r\n");
  smtp->Flags |= SMTP_FLAG_HEADER;
  return (len);
}

/**
  \brief       Construct multipart MIME footer.
  \param[out]  buf    output buffer to write the data to.
  \return      number of bytes written to output buffer.
*/
static uint32_t mime_add_footer (char *buf) {
  uint32_t len;

  /* Add closing boundary marker */
  len  = net_strcpy2 (buf, "\r\n--");
  len += add_boundary (buf+len);
  /* Add end of header marker */
  len += net_strcpy2 (buf+len, "--\r\n");
  return (len);
}

/**
  \brief       Check if email addresses are ok.
  \param[in]   mail  pointer to email descriptor.
  \return      number of recipients found.
*/
static uint32_t check_addr (const NET_SMTP_MAIL *mail) {
  const char *next;
  uint32_t cnt = 0;

  /* Process 'From' address first */
  for (next = mail->From; next; ) {
    next = parse_addr (next, &p_tag);
    if (p_tag.AddrLen) {
      smtp2->From = p_tag.Base;
      break;
    }
  }
  /* Check number of recipients */
  for (next = mail->To; next; ) {
    next = parse_addr (next, &p_tag);
    if (p_tag.AddrLen && cnt < SMTP_MAX_RCPT) {
      smtp2->Rcpt[cnt++] = p_tag.Base;
    }
  }
  for (next = mail->Cc; next; ) {
    next = parse_addr (next, &p_tag);
    if (p_tag.AddrLen && cnt < SMTP_MAX_RCPT) {
      smtp2->Rcpt[cnt++] = p_tag.Base;
    }
  }
  for (next = mail->Bcc; next; ) {
    next = parse_addr (next, &p_tag);
    if (p_tag.AddrLen && cnt < SMTP_MAX_RCPT) {
      smtp2->Rcpt[cnt++] = p_tag.Base;
    }
  }
  return (cnt);
}

/**
  \brief       Parse address string and search for next parameter.
  \param[in]   buf  buffer containing address string.
  \param[out]  tag  structure that will receive address tags.
  \return      pointer to next parameter or NULL.
  \note        Address parameters are separated with ';' characters.
*/
static const char *parse_addr (const char *buf, NET_SMTP_TAG *tag) {
  uint32_t i,j;

  /* Store pointer to base address */
  tag->Base = buf;

  /* Friendly name: skip leading spaces */
  for (i = 0; buf[i] == ' '; i++);
  tag->NameOffs = i & 0xFF;
  for ( ; buf[i] != '<'; i++) {
    if ((buf[i] == ';') || (buf[i] == 0)) {
      /* Only mail address found */
      tag->NameLen  = 0;
      tag->AddrOffs = tag->NameOffs;
      /* Remove trailing spaces */
      for (j = i - 1; buf[j] == ' ' && j; j--);
      tag->AddrLen  = (j - tag->AddrOffs + 1) & 0xFF;
      return ((buf[i]) ? &buf[i+1] : NULL);
    }
  }
  /* Remove trailing spaces */
  for (j = i - 1; buf[j] == ' ' && j; j--);
  tag->NameLen = (j - tag->NameOffs + 1) & 0xFF;

  /* Email address: skip leading spaces */
  for (++i; buf[i] == ' '; i++);
  tag->AddrOffs = i & 0xFF;
  for ( ; buf[i] != '>'; i++) {
    if ((buf[i] == ';') || (buf[i] == 0)) {
      /* Error, invalid mail address */
      tag->AddrLen  = 0;
      tag->NameLen  = 0;
      return ((buf[i]) ? &buf[i+1] : NULL);
    }
  }
  /* Remove trailing spaces */
  for (j = i - 1; buf[j] == ' ' && j; j--);
  tag->AddrLen = (j - tag->AddrOffs + 1) & 0xFF;

  /* Advance to next addr */
  for (++i; (buf[i] != ';' && buf[i]); i++);
  return ((buf[i]) ? &buf[i+1] : NULL);
}

/**
  \brief       Check attachemt fname(s).
  \param[in]   mail  pointer to email descriptor.
  \return      number of attachments found.
*/
static uint32_t check_attachment (const NET_SMTP_MAIL *mail) {
  const char *next;
  uint32_t cnt = 0;

  if (!attach_io) {
    return (0);
  }
  /* Check number of attachments */
  for (next = mail->Attachment; next; ) {
    next = attach_io->parse_fname (next, &p_tag);
    if (p_tag.NameLen) {
      /* Valid fname found */
      cnt++;
    }
  }
  return (cnt);
}

/**
  \brief       Parse fname(s) and search for next parameter.
  \param[in]   buf   buffer containing fname string.
  \param[out]  ftag  structure that will receive fname tags.
  \return      pointer to next parameter or NULL.
  \note        Filename parameters are separated with ';' characters.
  \note        Called via function pointer!
*/
static const char *smtp_parse_fname (const char *buf, void *ftag) {
  NET_SMTP_TAG *tag = ftag;
  uint32_t i,j;

  /* Store pointer to base address */
  tag->Base = buf;

  /* Address: full path, skip leading spaces  */
  /* Example: "M:/logs/0001/system.log"   */
  for (i = 0; buf[i] == ' '; i++);
  tag->AddrOffs = i & 0xFF;
  for ( ; buf[i] != ';' && buf[i]; i++);
  /* Remove trailing spaces */
  for (j = i - 1; buf[j] == ' ' && j; j--);
  if (j < tag->AddrOffs) {
    /* Invalid path found */
    tag->AddrLen = 0;
    tag->NameLen = 0;
    return ((buf[i]) ? &buf[i+1] : NULL);
  }
  tag->AddrLen = (j - tag->AddrOffs + 1) & 0xFF;

  /* Name: fname with no path */
  /* Example: "system.log" */
  tag->NameOffs = tag->AddrOffs;
  for (j = tag->AddrOffs; j < i; j++) {
    if (buf[j] == '/' || buf[j] == '\\' || buf[j] == ':') {
      tag->NameOffs = (j + 1) & 0xFF;
    }
  }
  if (tag->NameOffs == i) {
    /* Invalid path name found */
    tag->AddrLen = 0;
    tag->NameLen = 0;
    return ((buf[i]) ? &buf[i+1] : NULL);
  }
  tag->NameLen = tag->AddrOffs + tag->AddrLen - tag->NameOffs;

  /* Advance to next addr */
  return ((buf[i]) ? &buf[i+1] : NULL);
}

/**
  \brief       MIME encode-word up to len characters.
  \param[out]  buf  pointer to output buffer.
  \param[in]   sp   source null-terminated string.
  \param[in]   len  maximum number of characters to encode.
  \return      length of the added string.
*/
static uint32_t mime_encode_word (char *buf, const char *sp, uint32_t len) {
  static const char hex_digit[16] = { "0123456789ABCDEF" };
  uint32_t i,j,n;
  char ch;

  for (i = j = n = 0; i < len; i++) {
    ch = sp[i];
    if ((ch == '_') || (ch == '=')) {
      /* Count escaped 7-bit characters */
      n++;
    }
    else if (ch > 127) {
      /* Count 8-bit characters */
      j++;
    }
    buf[i] = ch;
    if (ch == 0) {
      /* null-termination */
      break;
    }
  }
  if (j == 0) {
    /* Name is 7-bit ascii */
    return (i);
  }
  if (!(smtp2->mail->Encoding && smtp2->mail->Encoding[0])) {
    /* Name needs encoding, but encoding type not specified */
    return (i);
  }
  /* Add number of escaped 7-bit characters */
  j += n;

  /* Name needs encoding, 8-bit characters found */
  n  = net_strcpy2 (buf, "=?");
  n += net_strcpy2 (buf+n, smtp2->mail->Encoding);
  /* Decide which encoding is more optimal */
  if ((j > 2) && ((i * 4 + 8) < (i + 2 * j) * 3)) {
    /* Use base64 encoding */
    n += net_strcpy2 (buf+n, "?B?");
    n += net_base64_encode (buf+n, sp, i);
  }
  else {
    /* Use quoted-printable encoding */
    n += net_strcpy2 (buf+n, "?Q?");
    for (j = 0; j < i; j++) {
      if (sp[j] == ' ') {
        /* Space encoded as '_' */
        buf[n++] = '_';
        continue;
      }
      if ((sp[j] > 127) || (sp[j] == '_') || (sp[j] == '=')) {
        /* Quoted-printable encoding "=XX" */
        buf[n]   = '=';
        buf[n+1] = hex_digit[sp[j] >> 4];
        buf[n+2] = hex_digit[sp[j] & 0x0F];
        n += 3;
        continue;
      }
      /* 7-bit ascii, store unmodified */
      buf[n++] = sp[j];
    }
  }
  /* null-termination added */
  n += net_strcpy2 (buf+n, "?=");
  return (n);
}

/**
  \brief       Add 7-bit encoded message line.
  \param[out]  buf  pointer to output buffer.
  \return      length of the added string.
  \note        Message line is limited to 1000 characters.
*/
static uint32_t mime_encode_line_7bit (char *buf) {
  const char *sp = &smtp2->mail->Message[smtp->SVar];
  static uint8_t line_cnt;
  uint32_t i,n;

  if (smtp->SVar == 0) line_cnt = 0;
  for (n = i = 0; n < 76; i++) {
    if (sp[i] == 0) {
      /* End of message */
      return (n | 0x80000000);
    }
    /* 7-bit ascii, store unmodified */
    buf[n++] = sp[i];
    if (sp[i] == '\n') {
      /* LF is end of line */
      smtp->SVar += (i + 1);
      line_cnt = 0;
      return (n);
    }
  }
  /* Limit line size to 13*76 = 988 chars */
  if (++line_cnt == 13) {
    line_cnt = 0;
    /* Add line-break */
    buf[n]   = '\r';
    buf[n+1] = '\n';
    n += 2;
  }
  smtp->SVar += i;
  return (n);
}

/**
  \brief       Add quoted-printable encoded message line.
  \param[out]  buf  pointer to output buffer.
  \return      length of the added string.
  \note        Message line is limited to 76 characters.
*/
static uint32_t mime_encode_line_qp (char *buf) {
  static const char hex_digit[16] = { "0123456789ABCDEF" };
  const char *sp = &smtp2->mail->Message[smtp->SVar];
  uint32_t i,n;

  for (n = i = 0; n < 76; i++) {
    /* 8-bit ascii or '=' character, store encoded */
    if ((sp[i] > 127) || (sp[i] == '=')) {
      buf[n]   = '=';
      buf[n+1] = hex_digit[sp[i] >> 4];
      buf[n+2] = hex_digit[sp[i] & 0x0F];
      n += 3;
      continue;
    }
    if (sp[i] == 0) {
      /* End of message */
      return (n | 0x80000000);
    }
    /* 7-bit ascii, store unmodified */
    buf[n++] = sp[i];
    if (sp[i] == '\n') {
      /* LF is end of line */
      smtp->SVar += (i + 1);
      return (n);
    }
  }
  /* Add soft line-break */
  buf[n]   = '=';
  buf[n+1] = '\r';
  buf[n+2] = '\n';
  n += 3;
  smtp->SVar += i;
  return (n);
}

/**
  \brief       Check if a message contains 8-bit characters.
  \param[in]   mail  pointer to email content descriptor.
  \return      Status:
               - true:  8-bit characters,
               - false: 7-bit characters.
*/
static bool check_8bit (const NET_SMTP_MAIL *mail) {
  uint32_t i;

  if (mail->Message && mail->Encoding && mail->Encoding[0]) {
    /* Scan the message */
    for (i = 0; mail->Message[i]; i++) {
      if (mail->Message[i] > 127) {
        return (true);
      }
    }
  }
  return (false);
}

/**
  \brief       Add the value of charset parameter.
  \param[out]  buf   pointer to output buffer.
  \param[in]   mail  pointer to email content descriptor.
  \return      length of the added string.
*/
static uint32_t add_charset (char *buf, const NET_SMTP_MAIL *mail) {
  const char *sp;

  sp = (mail->Encoding && mail->Encoding[0]) ? mail->Encoding : "us-ascii";
  return (net_strcpy2 (buf, sp));
}

/**
  \brief       Add the value of encoding parameter.
  \param[out]  buf      pointer to output buffer.
  \param[in]   is_8bit  encoding is 8bit.
  \return      length of the added string.
*/
static uint32_t add_encoding (char *buf, bool is_8bit) {
  const char *sp;

  sp = (is_8bit) ? "quoted-printable" : "7bit";
  return (net_strcpy2 (buf, sp));
}

/**
  \brief       Add email address parameter.
  \param[out]  buf   pointer to output buffer.
  \param[in]   addr  pointer to start of address entry.
  \param[in]   tag   structure containing address tags.
  \return      length of the added string.
*/
static uint32_t add_address (char *buf, const NET_SMTP_TAG *tag) {
  uint32_t len;

  if (tag->NameLen) {
    /* Add friendly name and email address */
    len  = mime_encode_word  (buf, tag->Base+tag->NameOffs, tag->NameLen);
    len += net_strcpy2  (buf+len, " <");
    len += net_strncpy2 (buf+len, tag->Base+tag->AddrOffs, tag->AddrLen);
    buf[len++] = '>';
    return (len);
  }
  /* Add email address only */
  return (net_strncpy2 (buf, tag->Base+tag->AddrOffs, tag->AddrLen));
}

/**
  \brief       Add MIME boundary string.
  \param[out]  buf  pointer to output buffer.
  \return      length of the added string.
*/
static uint32_t add_boundary (char *buf) {
  uint32_t len,i;
  char ch;

  /* Boundary format: "--------YWWaczOeFHKoLMDpIghDLQbc" */
  len  = net_strcpy2 (buf, "--------");
  len += net_base64_encode (buf+len, smtp2->Hash, 16);
  /* Cleanup, replace '+' and '/' characters */
  for (ch = 'a', i = 8; i < len; i++) {
    if ((buf[i] == '+') || (buf[i] == '/') || (buf[i] == '=')) {
      buf[i] = ch++;
    }
  }
  return (len);
}

/**
  \brief       Compute 16-byte random MD5 hash.
  \param[out]  hash_buf  pointer to output buffer.
*/
static void compute_hash (uint8_t *hash_buf) {
  NET_MD5_CTX *ctx;

  ctx = __MD5_CTX(net_mem_alloc (sizeof (NET_MD5_CTX)));
  net_md5_init (ctx);
  net_md5_update (ctx, sys->HostName, (int32_t)strlen(sys->HostName));
  net_md5_update (ctx, (char *)&sys->Ticks, 4);
  net_md5_final (ctx, hash_buf);
  net_mem_free (__FRAME(ctx));
}

/* Exported SMTP attachment functions */
NET_SMTP_ATTACH net_smtp_attach_func = {
  smtp_mail_attach,
  smtp_mime_header,
  smtp_parse_fname
};
