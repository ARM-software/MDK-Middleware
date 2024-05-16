/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_smtp_client_core.c
 * Purpose: Mail Transfer Client core
 *----------------------------------------------------------------------------*/

#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_md5.h"
#include "net_base64.h"
#include "net_tcp.h"
#include "net_smtp_client.h"
#include "net_dbg.h"

/* Global variables */
NET_SMTP_CTRL net_smtp_control;

/* Interface functions */
#define tls_io      (smtpc->tls_if)         // mbedTLS interface

/* Local Functions */
static uint32_t smtp_listener (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len);
static void smtp_transit (uint8_t state);
static void smtp_stop (netSMTPc_Event event);
static uint8_t get_response (const char *buf);
static bool smtp_chk_multi (const char *buf, uint32_t len);
static const char *get_next_line (const char *buf, const char *end);
static uint8_t get_auth_mode (const char *buf, const char *end);
static void hmac_md5 (const char *buf, uint32_t len, const char *passw, uint8_t *digest);
static void free_cram_buf (void);
static uint32_t get_max_dsize (void);
static uint8_t *smtp_get_buf (uint32_t size);
static void smtp_send_buf (uint8_t *buf, uint32_t len);
#ifdef Network_Debug_STDIO
 static const char *auth_mode_ascii (uint8_t auth_mode);
 static const char *evt_ascii (netSMTPc_Event cb_evt);
 static void debug_info (const __ADDR *addr);
#endif

/**
  \brief       Initialize SMTP client.
*/
void net_smtp_client_init (void) {
  int32_t sock;

  DEBUGF (SMTP,"Init Client\n");
  EvrNetSMTP_InitClient ();
  memset (smtp, 0, sizeof (*smtp));
  sock = net_tcp_get_socket (smtp_listener);
  if (sock < 0) sock = 0;
  smtp->Socket = sock & 0xFF;
  if (smtp->Socket == 0) {
    ERRORF (SMTP,"Get socket failed\n");
    EvrNetSMTP_GetSocketFailed ();
    net_sys_error (NET_ERROR_TCP_ALLOC);
    return;
  }
  if (tls_io) {
    /* Allocate and initialize TLS context for SMTPS */
    smtp->TlsId = tls_io->get_context (smtp->Socket, smtp_listener);
    if (smtp->TlsId == 0) {
      ERRORF (SMTP,"TLS get context failed\n");
      EvrNetSMTP_TlsGetContextFailed ();
    }
  }
}

/**
  \brief       De-initialize SMTP client.
*/
void net_smtp_client_uninit (void) {
  DEBUGF (SMTP,"Uninit Client\n");
  EvrNetSMTP_UninitClient ();

  /* We do not close local file here */
  if (smtp->Flags & SMTP_FLAG_SUSPEND) {
    /* Resume thread from the notify function */
    smtp->uif->notify (netSMTPc_EventError);
  }
  memset (smtp, 0, sizeof (*smtp));
  smtp->cb_event = netSMTPc_EventError;
}

/**
  \brief       Start SMTP client to send an email.
  \param[in]   use_tls  use secure TLS mode.
*/
void net_smtp_client_start (uint16_t use_tls) {

  /* Check if TLS context available for secure session */
  smtp->Flags    = (use_tls && smtp->TlsId) ? SMTP_FLAG_USETLS : 0;
  smtp->AuthMode = SMTP_AUTH_NONE;
  smtp->cb_event = netSMTPc_EventSuccess;
  smtp->Secure   = false;
  smtp->SVar     = 0;

  if (smtp->Server.port == 0) {
    /* Port not specified, use default server port */
    smtp->Server.port = (smtp->Flags & SMTP_FLAG_USETLS) ?
                         SMTPS_SERVER_PORT : SMTP_SERVER_PORT;
  }
  DEBUG_INFO (&smtp->Server);
  EvrNetSMTP_Connect (&smtp->Server);
  smtp_transit (SMTP_STATE_CONNECT);
}

/**
  \brief       Socket event callback notification.
  \param[in]   socket  socket handle.
  \param[in]   event   event type as defined with netTCP_Event.
  \param[in]   addr    structure containing remote IP address and port.
  \param[in]   buf     pointer to a buffer containing the data.
  \param[in]   len     length of the data.
  \return      status:
               - true  = success,
               - false = error.
*/
static uint32_t smtp_listener (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len) {
  uint32_t res;

  (void)addr;
  /* Check if this is our socket */
  if (smtp->Socket != socket) {
    return (false);
  }

  switch (event) {
    case netTCP_EventConnect:
      DEBUGF (SMTP,"Inbound conn.rejected\n");
      EvrNetSMTP_InboundConnRejected (smtp->Socket);
      return (false);

    case netTCP_EventAborted:
      if (smtp->State == SMTP_STATE_IDLE) {
        break;
      }
      ERRORF (SMTP,"Client stop, Socket aborted\n");
      EvrNetSMTP_SocketAborted (smtp->Socket);
      smtp_stop (netSMTPc_EventError);
      break;

    case netTCP_EventEstablished:
      if (!smtp->Secure) {
        /* Socket connected, non secure mode */
        DEBUGF (SMTP,"Socket connected\n");
        EvrNetSMTP_SocketConnected (smtp->Socket);
        if (smtp->Flags & SMTP_FLAG_USETLS) {
          /* Implicit TLS, start with TLS initialize */
          smtp->Flags &= ~SMTP_FLAG_RESP;
          smtp_transit (SMTP_STATE_TLSINIT);
          break;
        }
        smtp_transit (SMTP_STATE_READY);
      }
      else {
        /* TLS connected, secure session established */
        DEBUGF (SMTP,"TLS mode established\n");
        EvrNetSMTP_TlsModeEstablished (smtp->TlsId);
        if (smtp->Flags & SMTP_FLAG_STARTTLS) {
          /* We are here after STARTTLS command */
          smtp->Flags &= ~SMTP_FLAG_STARTTLS;
          smtp_transit (SMTP_STATE_EHLO);
        }
      }
      break;

    case netTCP_EventClosed:
      DEBUGF (SMTP,"Client stop, Socket closed\n");
      EvrNetSMTP_SocketClosed (smtp->Socket);
      smtp_stop (netSMTPc_EventError);
      break;

    case netTCP_EventACK:
      /* Message data frame acked */
      smtp->Flags &= ~SMTP_FLAG_WACK;
      break;

    case netTCP_EventData:
      DEBUGF (SMTP,"*** Process_frame ***\n");
      DEBUGF (SMTP," Length %d bytes\n",len);
      EvrNetSMTP_ReceiveFrame (smtp->Socket, len);
      /* Do we have not acknowledged data? */
      if (smtp->Flags & SMTP_FLAG_WACK) {
        ERRORF (SMTP,"Process, Unacked data\n");
        EvrNetSMTP_UnackedDataError (smtp->Socket);
        return (false);
      }
      /* Get reply from server */
      if(len < 3) {
        /* Data frame too short */
        ERRORF (SMTP,"Process, Frame too short\n");
        EvrNetSMTP_FrameTooShort (len, 3);
        return (false);
      }

      /* Get response code from server */
      res = get_response ((const char *)buf);
      smtp->Flags &= ~SMTP_FLAG_RESP;
      switch (smtp->State) {
        case SMTP_STATE_READY:
          /* Initial welcome message */
          if (res != SMTP_RESP_READY) {
            break;
          }
          DEBUGF (SMTP," Server is ready\n");
          EvrNetSMTP_ServerReady ();
          /* Try with Extended SMTP first */
          smtp_transit (SMTP_STATE_EHLO);
          return (true);

        case SMTP_STATE_EHLO:
          /* Response to 'EHLO' command */
          if (res != SMTP_RESP_OK) {
            /* EHLO failed, try with HELO */
            DEBUGF (SMTP," EHLO failed, ESMTP not supported\n");
            EvrNetSMTP_EsmtpModeNotSupported ();
            smtp_transit (SMTP_STATE_HELO);
            return (true);
          }
          /* OK, Extended SMTP is supported */
          if (smtp_chk_multi ((const char *)buf, len) == false) {
            /* Fragmented Multiline response not completed yet */
            DEBUGF (SMTP," Fragmented multiline EHLO response\n");
            EvrNetSMTP_EhloResponseFragmented ();
            smtp->Flags |= SMTP_FLAG_RESP;
            return (true);
          }
          DEBUGF (SMTP," EHLO acked, ESMTP mode\n");
          EvrNetSMTP_EsmtpModeActive ();

          /* Check for 'STARTTLS' command indication */
          if (smtp->Flags & SMTP_FLAG_STARTTLS) {
            /* Command advertised, server supports TLS secure */
            DEBUGF (SMTP," TLS support indicated\n");
            EvrNetSMTP_TlsSupportIndicated ();
            if (!smtp->Secure && smtp->TlsId) {
              /* We support TLS too, force using it now */
              smtp_transit (SMTP_STATE_STARTTLS);
              return (true);
            }
          }
          switch (smtp->AuthMode) {
            case SMTP_AUTH_LOGIN:
            case SMTP_AUTH_PLAIN:
            case SMTP_AUTH_CRAM_MD5:
              /* Check if the user accepts authentication */
              if (smtp->uif->accept ((NET_ADDR *)&smtp->Server)) {
                DEBUGF (SMTP," Auth-Method %s\n",auth_mode_ascii(smtp->AuthMode)); 
                EvrNetSMTP_StartAuthentication (smtp->AuthMode);
                smtp_transit (SMTP_STATE_AUTH);
                break;
              }
              DEBUGF (SMTP," Authentication not accepted\n"); 
              EvrNetSMTP_AuthenticationDenied ();
              __FALLTHROUGH;
            case SMTP_AUTH_NONE:
              smtp_transit (SMTP_STATE_MAILFROM);
              break;
            default:
              ERRORF (SMTP,"Auth-Method not supported\n");
              EvrNetSMTP_AuthMethodNotSupported (smtp->AuthMode);
              smtp_transit (SMTP_STATE_QUIT);
              break;
          }
          return (true);

        case SMTP_STATE_HELO:
          /* Response to 'HELO' command */
          if (res != SMTP_RESP_OK) {
            break;
          }
          /* OK, Standard SMTP is supported */
          DEBUGF (SMTP," HELO acked, SMTP mode\n");
          EvrNetSMTP_SmtpModeActive ();
          smtp_transit (SMTP_STATE_MAILFROM);
          return (true);

        case SMTP_STATE_STARTTLS:
          /* Response to 'STARTTLS' command */
          if (res != SMTP_RESP_READY) {
            break;
          }
          /* OK, Server ready to start TLS */
          DEBUGF (SMTP," STARTTLS acked, TLS mode\n");
          EvrNetSMTP_StartTlsAccepted ();
          smtp_transit (SMTP_STATE_TLSINIT);
          return (true);

        case SMTP_STATE_AUTH:
          /* Response to 'AUTH <method>' */
          if (res != SMTP_RESP_CONTINUE) {
            break;
          }
          /* Method AUTH LOGIN */
          if (smtp->AuthMode == SMTP_AUTH_LOGIN) {
#ifdef Network_Debug_STDIO
            (__CONST_CAST(char *)buf)[len-1] = 0;
            DEBUGF (SMTP," Prompt: %s\n",buf+4);
#endif
            /* Decode base64 string to the same buffer */
            net_base64_decode (__CONST_CAST(char *)buf+4, (const char *)buf+4, len-4);
            if (net_strcmp ((const char *)buf+4, "Username:")) {
              smtp_transit (SMTP_STATE_USER);
              return (true);
            }
            break;
          }
          /* Method AUTH PLAIN */
          if (smtp->AuthMode == SMTP_AUTH_PLAIN) {
            DEBUGF (SMTP," Credentials requested\n");
            smtp_transit (SMTP_STATE_USER);
            return (true);
          }
          /* Method AUTH CRAM-MD5 */
          smtp->CramBuf = (char *)net_mem_alloc (len-4);
          /* Store decoded challenge */
          smtp->CramLen = net_base64_decode (smtp->CramBuf, (const char *)buf+4, len-4) & 0xFF;
#ifdef Network_Debug_STDIO
          (__CONST_CAST(char *)buf)[len-1] = 0;
          DEBUGF (SMTP," Challenge: %s\n",buf+4);
#endif
          smtp_transit (SMTP_STATE_USER);
          return (true);

        case SMTP_STATE_USER:
          /* Response to 'Username' */
          if (res == SMTP_RESP_AUTHOK) {
            DEBUGF (SMTP," Authentication successful\n");
            EvrNetSMTP_AuthenticationSuccessful ();
            smtp_transit (SMTP_STATE_MAILFROM);
            return (true);
          }
          if (res != SMTP_RESP_CONTINUE) {
            ERRORF (SMTP,"Authentication failed\n");
            EvrNetSMTP_AuthenticationFailed ();
            smtp->cb_event = netSMTPc_EventAuthenticationFailed;
            smtp_transit (SMTP_STATE_QUIT);
            return (true);
          }
#ifdef Network_Debug_STDIO
          (__CONST_CAST(char *)buf)[len-1] = 0;
          DEBUGF (SMTP," Prompt: %s\n",buf+4);
#endif
          /* Decode base64 string to the same buffer */
          net_base64_decode (__CONST_CAST(char *)buf+4, (const char *)buf+4, len-4);
          if (net_strcmp ((const char *)buf+4, "Password:")) {
            smtp->Flags |= SMTP_FLAG_PASSW;
            smtp_transit (SMTP_STATE_USER);
            return (true);
          }
          break;

        case SMTP_STATE_MAILFROM:
          /* Response to 'MAIL FROM' command */
          if (res != SMTP_RESP_OK) {
            break;
          }
          DEBUGF (SMTP," Sender OK\n");
          EvrNetSMTP_ServerAcknowledge ();
          smtp_transit (SMTP_STATE_RCPTTO);
          return (true);

        case SMTP_STATE_RCPTTO:
          /* Response to 'RCPT TO' command */
          if (res != SMTP_RESP_OK) {
            break;
          }
          DEBUGF (SMTP," Recipient OK\n");
          EvrNetSMTP_ServerAcknowledge ();
          smtp_transit ((smtp->Flags & SMTP_FLAG_RCPTEND) ?
                         SMTP_STATE_DATA : SMTP_STATE_RCPTTO);
          return (true);

        case SMTP_STATE_DATA:
          /* Response to 'DATA' command */
          if (res != SMTP_RESP_DATAOK) {
            break;
          }
          DEBUGF (SMTP," Data OK\n");
          EvrNetSMTP_ServerAcknowledge ();
          smtp_transit (SMTP_STATE_MSGBODY);
          return (true);

        case SMTP_STATE_MSGEND:
          /* Response to '<CRLF>.<CRLF>' command */
          if (res != SMTP_RESP_OK) {
            break;
          }
          DEBUGF (SMTP," End-of-message OK\n");
          EvrNetSMTP_ServerAcknowledge ();
          smtp_transit (SMTP_STATE_QUIT);
          return (true);

        case SMTP_STATE_QUIT:
          /* Response to 'QUIT' command */
          if (res != SMTP_RESP_QUITOK) {
            break;
          }
          /* Keep already assigned callback event */
          smtp_stop (smtp->cb_event);
          return (true);
      }
#ifdef __DEBUG_ENABLED
      (__CONST_CAST(char *)buf)[len-1] = 0;
      ERRORF (SMTP,"Wrong response: %s\n",buf);
      EvrNetSMTP_WrongResponse (buf, len);
#endif
      smtp_stop (netSMTPc_EventError);
      break;
  }
  return (false);
}

/**
  \brief       Run SMTP client main function.
*/
void net_smtp_client_run (void) {
  uint8_t *sendbuf;
  uint32_t len,max_dsize;

  if (smtp->State == SMTP_STATE_IDLE) {
    return;
  }

  if ((sys->Flags & SYS_FLAG_SEC) && smtp->Timer) {
    if (--smtp->Timer == 0) {
      /* SMTP Timeout timer has expired */
      ERRORF (SMTP,"Client stop, Timeout expired\n");
      EvrNetSMTP_ClientStopTimeoutExpired ();
      smtp->Flags |= SMTP_FLAG_ERROR;
      smtp_stop (netSMTPc_EventTimeout);
      return;
    }
  }
  if (smtp->Flags & (SMTP_FLAG_WACK | SMTP_FLAG_RESP)) {
    /* Waiting for data ack or command response */
    return;
  }
  sys->Busy = true;

  switch (smtp->State) {
    case SMTP_STATE_CONNECT:
      /* User has requested to open a TCP connection */
      if (net_tcp_connect (smtp->Socket, &smtp->Server, 0) != netOK) {
        smtp_stop (netSMTPc_EventError);
        break;
      }
      smtp->Flags |= SMTP_FLAG_RESP;
      break;

    case SMTP_STATE_EHLO:
      /* Send 'EHLO' command for ESMTP */

    case SMTP_STATE_HELO:
      /* Send 'HELO' command if ESMTP not supported */
      sendbuf = smtp_get_buf (50);
      len     = net_strcpy2 ((char *)sendbuf, "EHLO ");
      if (smtp->State == SMTP_STATE_HELO) {
        /* Send 'HELO' command for standard SMTP */
        sendbuf[0] = 'H';
        sendbuf[1] = 'E';
      }
      len += net_strcpy2 ((char *)sendbuf+len, sys->HostName);
      goto cr_send;

    case SMTP_STATE_STARTTLS:
      /* Send 'STARTTLS' command to start secure mode */
      sendbuf = smtp_get_buf (12);
      len     = net_strcpy2 ((char *)sendbuf, "STARTTLS");
      goto cr_send;

    case SMTP_STATE_AUTH:
      /* User Authentication for ESMTP Server */
      sendbuf = smtp_get_buf (20);
      len = 0;
      switch (smtp->AuthMode) {
        case SMTP_AUTH_LOGIN:
          len = net_strcpy2 ((char *)sendbuf, "AUTH LOGIN");
          break;
        case SMTP_AUTH_PLAIN:
          len = net_strcpy2 ((char *)sendbuf, "AUTH PLAIN");
          break;
        case SMTP_AUTH_CRAM_MD5:
          len = net_strcpy2 ((char *)sendbuf, "AUTH CRAM-MD5");
          break;
      }
      goto cr_send;

    case SMTP_STATE_USER:
      /* Send Username and/or Password */
      sendbuf = smtp_get_buf (120);
      /* Method AUTH LOGIN */
      if (smtp->AuthMode == SMTP_AUTH_LOGIN) {
        len = smtp->uif->process ((smtp->Flags & SMTP_FLAG_PASSW) ?
                                  netSMTPc_RequestPassword : netSMTPc_RequestUsername,
                                  (char *)sendbuf+50, 30, NULL);
        len = net_base64_encode ((char *)sendbuf, sendbuf+50, len);
        goto cr_send;
      }
      /* Method AUTH PLAIN */
      if (smtp->AuthMode == SMTP_AUTH_PLAIN) {
        sendbuf[50] = 0;
        len  = smtp->uif->process (netSMTPc_RequestUsername,
                                  (char *)sendbuf+51,     30, NULL);
        len += smtp->uif->process (netSMTPc_RequestPassword,
                                  (char *)sendbuf+52+len, 30, NULL);
        len = net_base64_encode ((char *)sendbuf, sendbuf+50, len+2);
        goto cr_send;
      }
      /* Method AUTH CRAM-MD5 */
      len = smtp->uif->process (netSMTPc_RequestUsername, (char *)sendbuf+50, 30, NULL);
      smtp->uif->process (netSMTPc_RequestPassword, (char *)sendbuf+51+len, 30, NULL);
      hmac_md5 (smtp->CramBuf, smtp->CramLen, (char *)sendbuf+51+len, sendbuf);
      free_cram_buf ();
      net_bin2hex ((char *)sendbuf+51+len, sendbuf, 16);
      /* Space separated username and text-digest */
      sendbuf[50+len] = ' ';
      len = net_base64_encode ((char *)sendbuf, sendbuf+50, len+33);
      goto cr_send;

    case SMTP_STATE_MAILFROM:
      /* Send 'MAIL FROM' command */
      sendbuf = smtp_get_buf (120);
      len  = net_strcpy2 ((char *)sendbuf, "MAIL FROM: <");
      /* User might enter number of recipients to SVar in the callback function */
      len += smtp->uif->process (netSMTPc_RequestSender,
                                (char *)sendbuf+len, 120-len, &smtp->SVar);
      sendbuf[len++] = '>';
      if (smtp->SVar != 0) {
        /* Save number of recipients if running in legacy mode */
        smtp->NumRcpt = ((smtp->SVar > SMTP_MAX_RCPT) ? SMTP_MAX_RCPT : smtp->SVar) & 0xFF;
        smtp->SVar    = 0;
      }
      goto cr_send;

    case SMTP_STATE_RCPTTO:
      /* Send 'RCPT TO' command */
      sendbuf = smtp_get_buf (120);
      len  = net_strcpy2 ((char *)sendbuf, "RCPT TO: <");
      len += smtp->uif->process (netSMTPc_RequestRecipient,
                                (char *)sendbuf+len, 120-len, &smtp->SVar);
      /* End if all recipients processed */
      if (++smtp->SVar == smtp->NumRcpt) {
        smtp->Flags |= SMTP_FLAG_RCPTEND;
      }
      sendbuf[len++] = '>';
      goto cr_send;

    case SMTP_STATE_DATA:
      /* Send 'DATA' command */
      sendbuf = smtp_get_buf (10);
      len = net_strcpy2 ((char *)sendbuf, "DATA");
      goto cr_send;

    case SMTP_STATE_MSGBODY:
      /* Send message content */
      max_dsize = get_max_dsize ();
      sendbuf   = smtp_get_buf (max_dsize);
      if (!(smtp->Flags & SMTP_FLAG_FIRST)) {
        smtp->Flags |= SMTP_FLAG_FIRST;
        DEBUGF (SMTP,"Sending message body\n");
        EvrNetSMTP_SendMessageBody ();
        smtp->SVar = 0;
      }
      /* Refresh SMTP timeout timer */
      smtp->Timer = smtpc->DefTout;
      /* Compose message in a callback function */
      len = smtp->f_compose ((char *)sendbuf, max_dsize);
      if (smtp->Flags & SMTP_FLAG_ERROR) {
        /* Error non existing attachment */
        smtp_send_buf (sendbuf, len);
        smtp_stop (netSMTPc_EventError);
        break;
      }
      if (smtp->Flags & SMTP_FLAG_DATAEND) {
        /* Send '<CRLF>.<CRLF>' command */
        DEBUGF (SMTP,"Sending: End-of-message\n");
        EvrNetSMTP_SendMessageEnd ();
        len += net_strcpy2 ((char *)sendbuf+len, "\r\n.\r\n");
        smtp_transit (SMTP_STATE_MSGEND);
        smtp->Flags |= SMTP_FLAG_RESP;
      }
      smtp_send_buf (sendbuf, len);
      smtp->Flags |= SMTP_FLAG_WACK;
      break;

    case SMTP_STATE_QUIT:
      /* Send QUIT command */
      sendbuf = smtp_get_buf (10);
      len  = net_strcpy2 ((char *)sendbuf, "QUIT");
cr_send:
      len += net_strcpy2 ((char *)sendbuf+len, "\r\n");
#ifdef __DEBUG_ENABLED
      sendbuf[len] = 0;
      DEBUGF (SMTP,"Sending: %s",sendbuf);
      EvrNetSMTP_SendCommand (sendbuf, len-2);
#endif
      smtp_send_buf (sendbuf, len);
      smtp->Flags |= SMTP_FLAG_RESP;
      break;

    case SMTP_STATE_STOP:
      /* Stop SMTP client */
      if (!(smtp->Flags & SMTP_FLAG_ERROR)) {
        /* No error, normal socket closing */
        DEBUGF (SMTP,"Stop Client, Close socket\n");
        EvrNetSMTP_ClientCloseSocket (smtp->Socket);
        if (smtp->Secure) tls_io->close (smtp->TlsId, 1);
        else              net_tcp_close (smtp->Socket);
      }
      else {
        /* Error, abort socket connection */
        if (smtp->Secure) tls_io->close (smtp->TlsId, 0);
        else              net_tcp_abort (smtp->Socket);
      }
      free_cram_buf ();
      smtp->State = SMTP_STATE_TWAIT;
      break;

    case SMTP_STATE_TWAIT:
      /* Waiting for TCP socket to close */
      if (((sys->Flags & SYS_FLAG_TICK) == 0)                    ||
          (net_tcp_get_state (smtp->Socket) > netTCP_StateCLOSED)) {
        break;
      }
      /* Socket is closed, notify the user and goto idle */
      DEBUGF (SMTP,"Return event %s\n",evt_ascii(smtp->cb_event));
      EvrNetSMTP_ClientDone (smtp->cb_event);
      /* Send notification in legacy mode only */
      smtp->uif->notify (smtp->cb_event);
      DEBUGF (SMTP,"Client idle\n");
      /* Re-route callback function back to the application */
      /* It was set to TLS_mbed callback in tls_io->connect */
      if (smtp->Secure) {
        net_tcp_set_cb (smtp->Socket, smtp_listener);
      }
      smtp->State = SMTP_STATE_IDLE;
      break;

    case SMTP_STATE_TLSINIT:
      /* Initialize and start TLS context */
      if (smtp->TlsId == 0) {
        smtp_stop (netSMTPc_EventError);
        break;
      }
      smtp->Secure = true;
      tls_io->connect (smtp->TlsId, smtp->SrvName);
      DEBUGF (SMTP,"TLS mode started\n");
      EvrNetSMTP_TlsModeStarted ();
      smtp_transit (SMTP_STATE_READY);
      break;
  }
}

/**
  \brief       Change state of SMTP client to a new state.
  \param[in]   state  new client state.
  \note        This function refreshes also Timeout timer.
*/
static void smtp_transit (uint8_t state) {
  smtp->State = state;
  smtp->Timer = smtpc->DefTout;
}

/**
  \brief       Stop SMTP client.
  \param[in]   event  user notification event.
*/
static void smtp_stop (netSMTPc_Event event) {
  switch (smtp->State) {
    case SMTP_STATE_IDLE:
    case SMTP_STATE_STOP:
    case SMTP_STATE_TWAIT:
      break;
    default:
      smtp->cb_event = event;
      smtp->Flags   &= ~(SMTP_FLAG_WACK | SMTP_FLAG_RESP);
      smtp->Timer    = 0;
      smtp->State    = SMTP_STATE_STOP;
  }
}

/**
  \brief       Read response code from the server.
  \param[in]   buf  buffer containing response text.
  \return      binary response code.
*/
static uint8_t get_response (const char *buf) {
  static const uint32_t s_resp[] = {
    RESP(2,2,0, SMTP_RESP_READY   ),
    RESP(2,2,1, SMTP_RESP_QUITOK  ),
    RESP(2,3,5, SMTP_RESP_AUTHOK  ),
    RESP(2,5,0, SMTP_RESP_OK      ),
    RESP(3,3,4, SMTP_RESP_CONTINUE),
    RESP(3,5,4, SMTP_RESP_DATAOK  ) };
  const uint32_t *sp;
  uint32_t cmp;

  /* Construct the comparison string */
  cmp = (uint32_t)(buf[0] << 16 | buf[1] << 8 | buf[2]);
  for (sp = &s_resp[0]; sp < &s_resp[sizeof(s_resp)/sizeof(uint32_t)]; sp++) {
    /* The upper 3 bytes contain the comparison string */
    if ((sp[0] >> 8) == cmp) {
      return (sp[0] & 0xFF);
    }
  }
  return (SMTP_RESP_UNKNOWN);
}

/**
  \brief       Check multi-line response for authentication request.
  \param[in]   buf  buffer containing response text.
  \param[in]   len  length of response text.
  \return      status:
               - true  = response complete,
               - false = fragmented response, more to come.
*/
static bool smtp_chk_multi (const char *buf, uint32_t len) {
  const char *fp, *end;

  /* Scan EHLO OK response, search for Authentication Request */
  for (fp = buf, end = fp + len; fp; fp = get_next_line (fp, end)) {
    if (net_strcmp (fp+4, "AUTH ")) {
      /* Auth command found, check parameters */
      smtp->AuthMode = get_auth_mode (fp+9, end);
    }
    else if (net_strcmp (fp+4, "STARTTLS")) {
      smtp->Flags |= SMTP_FLAG_STARTTLS;
    }
    if (fp[3] != '-') {
      /* Last line of a multiline response */
      return (true);
    }
  }
  /* Multiline response not completed yet */
  return (false);
}

/**
  \brief       Parse multiline response and search for next line.
  \param[in]   buf  buffer containing response text.
  \param[in]   end  buffer end.
  \return      pointer to next option or NULL.
  \note        Lines are terminated with "\r\n" (CRLF).
*/
static const char *get_next_line (const char *buf, const char *end) {
  const char *bp;

  for (bp = buf, end--; bp < end; bp++) {
    if (bp[0] == '\r' && bp[1] == '\n') {
      break;
    }
  }
  if (bp == buf || bp+2 > end) {
    /* An empty line, or fragmented response */
    return (NULL);
  }
  return (bp+2);
}

/**
  \brief       Check supported authentication methods of the server.
  \param[in]   buf  buffer containing response text.
  \param[in]   end  buffer end.
  \return      selected authentication method.
  \note        Methods are provided as a space-separated list.
*/
static uint8_t get_auth_mode (const char *buf, const char *end) {
  uint32_t type;

  for (type = 0; buf < end; buf++) {
    if      (net_strcmp (buf, "LOGIN")) {
      type |= 0x01;
      buf  += 5;
    }
    else if (net_strcmp (buf, "PLAIN")) {
      type |= 0x02;
      buf  += 5;
    }
    else if (net_strcmp (buf, "CRAM-MD5")) {
      type |= 0x04;
      buf  += 8;
    }
    else if (net_strcmp (buf, "DIGEST-MD5")) {
      type |= 0x08;
      buf  += 10;
    }
    else if (net_strcmp (buf, "NTLM")) {
      type |= 0x10;
      buf  += 4;
    }
    else {
      /* Skip unknown method in the list */
      while (*buf != ' ' && *buf != '\r' && buf < end) {
        buf++;
      }
    }
    if (*buf != ' ') break;
  }
  /* Select the best method supported */
  if (type & 0x04) {
    return (SMTP_AUTH_CRAM_MD5);
  }
  if (type & 0x02) {
    return (SMTP_AUTH_PLAIN);
  }
  if (type & 0x01) {
    return (SMTP_AUTH_LOGIN);
  }
  if (type == 0) {
    return (SMTP_AUTH_NONE);
  }
  return (SMTP_AUTH_UNKNOWN);
}

/**
  \brief       Compute HMAC using MD5 hash algorithm.
  \param[in]   buf     pointer to input data buffer.
  \param[in]   len     length of the data.
  \param[in]   passw   authentication password null-terminated.
  \param[out]  digest  output MD5 digest buffer.
  \note        Implemented according to RFC2104, page 3.
*/
static void hmac_md5 (const char *buf, uint32_t len, const char *passw, uint8_t *digest) {
  NET_MD5_CTX *ctx;
  uint8_t *key;
  uint32_t i;

  /* Allocate a single memory block for both MD5 context and key buffer */
  key = (uint8_t *)net_mem_alloc (64 + sizeof (NET_MD5_CTX));

  /* XOR key with ipad value (0x36) */
  for (i = 0; passw[i] && i < 64; i++) {
    key[i] = passw[i] ^ 0x36;
  }
  for ( ; i < 64; i++) {
    key[i] = 0x36;
  }
  ctx  = __MD5_CTX(key + 64);

  /* Perform inner MD5 */
  net_md5_init (ctx);
  net_md5_update (ctx, (char *)key, 64);
  net_md5_update (ctx, buf, (int32_t)len);
  net_md5_final (ctx, digest);

  /* XOR key with opad value (0x5c) */
  for (i = 0; i < 64; i++) {
    key[i] ^= (0x36 ^ 0x5c);
  }
  /* Perform outer MD5 */
  net_md5_init (ctx);
  net_md5_update (ctx, (char *)key, 64);
  net_md5_update (ctx, (char *)digest, 16);
  net_md5_final (ctx, digest);

  net_mem_free (__FRAME(key));
}

/**
  \brief       Release allocated challenge message buffer.
*/
static void free_cram_buf (void) {
  if (smtp->CramBuf) {
    net_mem_free (__FRAME(smtp->CramBuf));
    smtp->CramBuf = NULL;
    smtp->CramLen = 0;
  }
}

/**
  \brief       Get maximum data size.
  \return      maximum data size in bytes.
*/
static uint32_t get_max_dsize (void) {
  uint32_t dsize;

  dsize = net_tcp_get_mss (smtp->Socket);
  if (smtp->Secure) {
    /* Add space for TLS header */
    dsize -= SMTPS_HEADER_LEN;
  }
  return (dsize);
}

/**
  \brief       Allocate memory for data send buffer.
  \param[in]   size  number of bytes to allocate.
  \return      pointer to the allocated memory.
*/
static uint8_t *smtp_get_buf (uint32_t size) {
  if (smtp->Secure) {
    return (tls_io->get_buf (size));
  }
  else {
    return (net_tcp_get_buf (size));
  }
}

/**
  \brief       Send data to remote server.
  \param[in]   buf  buffer containing the data.
  \param[in]   len  length of data in bytes.
*/
static void smtp_send_buf (uint8_t *buf, uint32_t len) {
  if (smtp->Secure) {
    tls_io->write (smtp->TlsId, buf, len);
  }
  else {
    net_tcp_send (smtp->Socket, buf, len);
  }
}

#ifdef Network_Debug_STDIO
/**
  \brief       Convert authentication mode to ascii.
  \param[in]   auth_mode  SMTP authentication mode.
  \return      pointer to text representation of authentication mode.
*/
static const char *auth_mode_ascii (uint8_t auth_mode) {
  switch (auth_mode) {
    case SMTP_AUTH_NONE:
      return ("NONE");
    case SMTP_AUTH_PLAIN:
      return ("PLAIN");
    case SMTP_AUTH_LOGIN:
      return ("LOGIN");
    case SMTP_AUTH_CRAM_MD5:
      return ("CRAM-MD5");
  }
  /* Other types not supported */
  return ("UNKNOWN");
}

/**
  \brief       Convert callback event to ascii.
  \param[in]   cb_evt  callback event id.
  \return      pointer to text representation of an event.
*/
static const char *evt_ascii (netSMTPc_Event cb_evt) {
  switch (cb_evt) {
    case netSMTPc_EventSuccess:
      return ("SUCCESS");
    case netSMTPc_EventTimeout:
      return ("TIMEOUT");
    case netSMTPc_EventAuthenticationFailed:
      return ("AUTH-FAILED");
    case netSMTPc_EventError:
      return ("ERROR");
  }
  return ("");
}

/**
  \brief       Debug print IP address and port.
  \param[in]   addr  structure containing IP address and port.
*/
static void debug_info (const __ADDR *addr) {
  DEBUGF (SMTP," Server [%s], port %d\n",net_addr_ntoa(addr),addr->port);
}
#endif
