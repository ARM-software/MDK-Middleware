/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_telnet_server.c
 * Purpose: Telnet Server
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_tcp.h"
#include "net_telnet_server.h"
#include "net_dbg.h"

/* Local variables */
static NET_TELNET_CTRL net_telnet_control;
#define teln         (&net_telnet_control)

/* Local functions */
static void telnet_server_start (void);
static void telnet_server_stop (void);
static void telnet_server_run (void);
static uint32_t telnet_listener (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                                 const uint8_t *buf, uint32_t len);
static NET_TELNET_INFO *telnet_map_session (int32_t socket);
static void telnet_kill_session (NET_TELNET_INFO *teln_s);
static void telnet_parse_cmd (NET_TELNET_INFO *teln_s, const uint8_t *buf, int32_t len);
static uint8_t check_reply (const uint8_t *buf, int32_t len);
static uint16_t proc_data (NET_TELNET_INFO *teln_s);
static void store_hist (NET_TELNET_INFO *teln_s);
static uint32_t get_hist (NET_TELNET_INFO *teln_s, uint32_t key);

/**
  \brief       Initialize Telnet server.
*/
void net_telnet_server_init (void) {
  NET_TELNET_INFO *teln_s;
  int32_t  sock,i;

  DEBUGF (TELNET,"Init %d Sessions, Port %d\n",telnc->NumSess,telnc->PortNum);
  EvrNetTeln_InitServer (telnc->NumSess, telnc->PortNum, telnc->IdleTout);
#ifdef Network_Debug_STDIO
  if (telnc->IdleTout) {
    DEBUGF (TELNET," Idle timeout %d secs\n",telnc->IdleTout);
  }
  else {
    DEBUGF (TELNET," Connection permanent\n");
  }
  if (telnc->NoEcho) {
    DEBUGF (TELNET," Echo disabled\n");
  }
#endif
  /* Init telnet sessions */
  for (i = 1, teln_s = &telnc->Scb[0]; i <= telnc->NumSess; teln_s++, i++) {
    teln_s->BCnt   = 0;
    teln_s->Flags  = 0;
    teln_s->Tout   = 0;
    teln_s->Widx   = 0;
    teln_s->Ridx   = 0;
    teln_s->hCurr  = 0;
    teln_s->hNext  = 0;
    teln_s->Hist[0]= 0;
    sock = net_tcp_get_socket (telnet_listener);
    if (sock < 0) sock = 0;
    teln_s->Socket = sock & 0xFF;
    teln_s->State  = TELN_STATE_IDLE;
    if (teln_s->Socket == 0) {
      teln_s->State = TELN_STATE_ERROR;
      ERRORF (TELNET,"Session %d, Get socket failed\n",i);
      EvrNetTeln_GetSocketFailed (i & 0xFF);
      net_sys_error (NET_ERROR_TCP_ALLOC);
      return;
    }
    teln_s->Id = i & 0xFF;
  }

  /* Init service control */
  memset (teln, 0, sizeof(*teln));
  teln->port_num = telnc->PortNum;
  if (telnc->EnAuth) {
    teln->user      = telnc->User;
    teln->passw     = telnc->Passw;
    teln->user_len  = telnc->UserLen;
    teln->passw_len = telnc->PasswLen;
    teln->en_auth   = true;
  }
  if (sysc->AutoStart) {
    telnet_server_start ();
  }
}

/**
  \brief       De-initialize Telnet server.
*/
void net_telnet_server_uninit (void) {
  DEBUGF (TELNET,"Uninit Server\n");
  EvrNetTeln_UninitServer ();

  /* Clear session control blocks */
  memset (telnc->Scb, 0, sizeof(*telnc->Scb) * telnc->NumSess);

  /* Uninit service control */
  memset (teln, 0, sizeof(*teln));
}

/**
  \brief       Start Telnet server service.
  \return      status code as defined with netStatus.
*/
netStatus netTELNETs_Start (void) {

  START_LOCK (netStatus);

  if (!teln->run_service) {
    /* Service is down */
    telnet_server_start ();
  }

  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Stop Telnet server service.
  \return      status code as defined with netStatus.
*/
netStatus netTELNETs_Stop (void) {

  START_LOCK (netStatus);

  if (teln->run_service) {
    /* Service is running */
    telnet_server_stop ();
  }

  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Check if Telnet server service is running.
  \return      service status:
               - true  = service is running.
               - false = service is not running.
*/
bool netTELNETs_Running (void) {
  return (teln->run_service);
}

/**
  \brief       Get current port number of Telnet server.
  \return      current port number.
*/
uint16_t netTELNETs_GetPort (void) {
  return (teln->port_num);
}

/**
  \brief       Set Telnet server port number.
  \param[in]   port  service port number.
  \return      status code as defined with netStatus.
*/
netStatus netTELNETs_SetPort (uint16_t port) {
  if (teln->run_service) {
    return (netWrongState);
  }
  teln->port_num = port;
  return (netOK);
}

/**
  \brief       Retrieve username of system administrator account.
  \return      pointer to username, a null-terminated string or
                 NULL if authentication is disabled.
*/
const char *netTELNETs_GetUsername (void) {
  return (teln->user);
}

/**
  \brief       Set username of system administrator account.
  \param[in]   username  new username.
  \return      status code as defined with netStatus.
*/
netStatus netTELNETs_SetUsername (const char *username) {
  static char user_buf [NET_USERNAME_LEN];

  START_LOCK (netStatus);

  if ((username == NULL) || strlen(username) >= NET_USERNAME_LEN) {
    RETURN (netInvalidParameter);
  }
  if (!teln->user) {
    /* Administrator account not enabled */
    RETURN (netError);
  }
  DEBUGF (TELNET,"SetUsername \"%s\"\n",username);
  EvrNetTeln_SetUsername (username, strlen(username));
  /* Use a static buffer instead of default const array */
  teln->user     = user_buf;
  teln->user_len = net_strcpy (user_buf, username) & 0xFF;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Retrieve password of system administrator account.
  \return      pointer to password, a null-terminated string or
                 NULL if authentication is disabled.
*/
const char *netTELNETs_GetPassword (void) {
  return (teln->passw);
}

/**
  \brief       Set password of system administrator account.
  \param[in]   password  new password.
  \return      status code as defined with netStatus.
*/
netStatus netTELNETs_SetPassword (const char *password) {
  static char passw_buf [NET_PASSWORD_LEN];

  START_LOCK (netStatus);

  if ((password == NULL) || strlen(password) >= NET_PASSWORD_LEN) {
    RETURN (netInvalidParameter);
  }
  if (!teln->passw) {
    /* Administrator account not enabled */
    RETURN (netError);
  }
  DEBUGF (TELNET,"SetPassword \"%s\"\n",password);
  EvrNetTeln_SetPassword (password, strlen(password));
  /* Use a static buffer instead of default const array */
  teln->passw     = passw_buf;
  teln->passw_len = net_strcpy (passw_buf, password) & 0xFF;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Determine server authentication status.
  \return      status:
               - true  = authentication enabled,
               - false = authentication not enabled.
*/
bool netTELNETs_LoginActive (void) {
  return (teln->en_auth);
}

/**
  \brief       Enable or disable server authentication.
  \param[in]   enable  - true=  enable authentication,
                       - false= disable authentication.
  \return      status code as defined with netStatus.
*/
netStatus netTELNETs_LoginOnOff (bool enable) {
  if (enable && !telnc->EnAuth) {
    /* Authentication not enabled in the configuration */
    return (netError);
  }
  teln->en_auth = enable;
  return (netOK);
}

/**
  \brief       Get IP address and port number of connected telnet client.
  \param[out]  addr      structure that will receive IP address and port number.
  \param[in]   addr_len  size of NET_ADDR structure for the client.
  \return      status code as defined with netStatus.
*/
netStatus netTELNETs_GetClient (NET_ADDR *addr, uint32_t addr_len) {
  NET_TELNET_INFO *teln_s;
  
  teln_s = &telnc->Scb[teln->cmd_session-1];
  return (net_tcp_get_peer (teln_s->Socket, (__ADDR *)addr, addr_len));
}

/**
  \brief       Get current session number of Telnet server.
  \return      current session number.
  \note        This function is called from Telnet server user interface.
               The 'cmd_session' is set and valid for the call.
*/
int32_t netTELNETs_GetSession (void) {
  return (teln->cmd_session-1);
}

/**
  \brief       Check if buffer text matches user command.
  \param[in]   cmd       buffer containing the text.
  \param[in]   user_cmd  user command to compare with.
  \return      status:
               - true  = command is found,
               - false = not found.
*/
bool netTELNETs_CheckCommand (const char *cmd, const char *user_cmd) {
  uint32_t ch;

  for ( ; *user_cmd; cmd++, user_cmd++) {
    ch = *cmd;
    if (ch >= 'a' && ch <= 'z') {
      /* Upper case of char */
      ch -= 'a' - 'A';
    }
    if (ch != *user_cmd) {
      return (false);
    }
  }
  if (*cmd & ~' ') {
    return (false);
  }
  /* Space or null-terminated command */
  return (true);
}

/**
  \brief       Request a repeated call to netTELNETs_ProcessCommand function.
  \param[in]   delay  time period to wait in number of 100ms ticks.
  \return      status code as defined with netStatus.
*/
netStatus netTELNETs_RepeatCommand (uint32_t delay) {
  NET_TELNET_INFO *teln_s;

  if ((delay == 0) || (delay > 0xFFFF)) {
    return (netInvalidParameter);
  }
  teln_s = &telnc->Scb[teln->cmd_session-1];
  /* We must be in SENDING state, to set a delay */
  if (teln_s->State != TELN_STATE_SENDING) {
    return (netWrongState);
  }
  teln_s->Tout   = delay & 0xFFFF;
  teln_s->Flags |= TELN_FLAG_DELAY;
  return (netOK);
}

/**
  \brief       Request unsolicited message processing.
  \param[in]   session       session identification.
  \return      status code as defined with netStatus.
*/
netStatus netTELNETs_RequestMessage (int32_t session) {
  NET_TELNET_INFO *teln_s;

  START_LOCK (netStatus);

  if (session <= 0 || session > telnc->NumSess) {
    RETURN (netInvalidParameter);
  }
  teln_s = &telnc->Scb[session-1];
  /* We must be in ACTIVE state for unsolicited messages */
  if (teln_s->State != TELN_STATE_ACTIVE) {
    RETURN (netWrongState);
  }
  teln_s->Flags |= TELN_FLAG_MESSAGE;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Main Telnet server scheduler.
*/
void net_telnet_server_run (void) {
  if (teln->run_service) {
    telnet_server_run ();
  }
}

/**
  \brief       Start Telnet server service.
*/
static void telnet_server_start (void) {
  NET_TELNET_INFO *teln_s;
  int32_t i;

  DEBUGF (TELNET,"Start service, port %d\n",teln->port_num);
  EvrNetTeln_StartService (teln->port_num);
  for (i = 1, teln_s = &telnc->Scb[0]; i <= telnc->NumSess; teln_s++, i++) {
    if (teln_s->State != TELN_STATE_ERROR) {
      net_tcp_listen (teln_s->Socket, teln->port_num);
      if (telnc->IdleTout != 0) {
        /* Setup idle disconnect on timeout */
        net_tcp_set_option (teln_s->Socket, netTCP_OptionTimeout, telnc->IdleTout);
      }
      else {
        /* Idle timeout == 0, activate TCP keep-alive mode */
        net_tcp_set_option (teln_s->Socket, netTCP_OptionKeepAlive, 1);
      }
      teln_s->State = TELN_STATE_IDLE;
    }
  }
  teln->run_service = true;
}

/**
  \brief       Stop Telnet server service.
*/
static void telnet_server_stop (void) {
  NET_TELNET_INFO *teln_s;
  int32_t i;

  DEBUGF (TELNET,"Stop service\n");
  EvrNetTeln_StopService ();
  for (i = 1, teln_s = &telnc->Scb[0]; i <= telnc->NumSess; teln_s++, i++) {
    if (teln_s->State != TELN_STATE_ERROR) {
      net_tcp_abort (teln_s->Socket);
      telnet_kill_session (teln_s);
    }
  }
  teln->run_service = false;
}

/**
  \brief       Run Telnet server service.
*/
static void telnet_server_run (void) {
  static uint8_t session = 0;
  NET_TELNET_INFO *teln_s;
  uint8_t id,*sendbuf = NULL;
  uint32_t i,v,len,max_dsize;

  /* Check all Telnet sessions if there is something to do */
  for (i = 0; i < telnc->NumSess; i++) {
    if (++session > telnc->NumSess) {
      session = 1;
    }
    teln->cmd_session = session;

    teln_s = &telnc->Scb[session-1];
    if (teln_s->State == TELN_STATE_IDLE ||
        teln_s->State == TELN_STATE_RESERVED) {
      /* Keep TCP sockets listening */
      if (net_tcp_get_state (teln_s->Socket) < netTCP_StateLISTEN) {
        /* This socket is not listening, activate listening again */
        net_tcp_listen (teln_s->Socket, teln->port_num);
      }
      continue;
    }
    if (teln_s->Flags & TELN_FLAG_WACK) {
      /* Data not acknowledged yet */
      if ((sys->Flags & SYS_FLAG_TICK) && (teln_s->Flags & TELN_FLAG_DELAY)) {
        if (--teln_s->Tout == 0) {
          teln_s->Flags &= ~TELN_FLAG_DELAY;
        }
      }
      continue;
    }
    if (teln_s->Flags & TELN_FLAG_CLOSE) {
      /* Close Session Requested */
      DEBUGF (TELNET," Session %d close\n",teln_s->Id);
      EvrNetTeln_CloseSession (teln_s->Id);
      net_tcp_close (teln_s->Socket);
      telnet_kill_session (teln_s);
      continue;
    }

    switch (teln_s->State) {
      case TELN_STATE_ACTIVE:
        /* Send unsolicited message if requested */
        if (teln_s->Flags & TELN_FLAG_MESSAGE) {
          teln_s->Flags &= ~TELN_FLAG_MESSAGE; 
          max_dsize = net_tcp_get_mss (teln_s->Socket);
          sendbuf   = net_tcp_get_buf (max_dsize);
          len = netTELNETs_ProcessMessage (netTELNETs_MessageUnsolicited,
                                           (char *)sendbuf, max_dsize);
          net_tcp_send (teln_s->Socket, sendbuf, len);
          teln_s->Flags |= TELN_FLAG_WACK;
          return;
        }

        /* New chars received to process? */
        if (teln_s->Ridx == teln_s->Widx) {
          break;
        }
        sys->Busy = true;

        DEBUGF (TELNET,"Process data\n");
        EvrNetTeln_ProcessData (teln_s->Id, teln_s->Widx - teln_s->Ridx);
        switch (len = proc_data (teln_s)) {
          case ASCII_CR:
            len = 0;
            max_dsize = net_tcp_get_mss (teln_s->Socket);
            sendbuf   = net_tcp_get_buf (max_dsize);
            if (teln_s->BCnt == 0) {
              goto prmpt;
            }
            DEBUGF (TELNET," Command: %s\n",teln_s->LBuf);
            EvrNetTeln_ProcessCommand (teln_s->LBuf, teln_s->BCnt);
            /* Store a command to history buffer */
            store_hist (teln_s);
            teln_s->SVar = 0;
            v = netTELNETs_ProcessCommand (teln_s->LBuf, (char *)sendbuf+len,
                                           max_dsize-len, &teln_s->SVar);
            len += (v & 0xFFFF);
            /* Bit-31 is a repeat flag */
            if (v & 0x80000000) {
              teln_s->State = TELN_STATE_SENDING;
              teln_s->BCnt  = sizeof (teln_s->LBuf);
              net_tcp_set_option (teln_s->Socket, netTCP_OptionDelayedACK, 1);
              len |= 0x80000000;
              break;
            }
            /* Bit-30 is a disconnect flag */
            if (v & 0x40000000) {
              teln_s->Flags |= TELN_FLAG_CLOSE;
            }
            else {
              /* Display a prompt string */
prmpt:        len += netTELNETs_ProcessMessage (netTELNETs_MessagePrompt,
                                                (char *)sendbuf+len, max_dsize-len);
            }
            /* Bit-31 of len is a send flag for return */
            len |= 0x80000000;
            teln_s->BCnt = 0;
            break;

          case ASCII_BS:
            /* Echo the Backspace sequence */
            if (telnc->NoEcho) {
              len = 0;
              break;
            }
            DEBUGF (TELNET," Echo backspace\n");
            EvrNetTeln_EchoBackspace (teln_s->Id);
            sendbuf = net_tcp_get_buf (3);
            sendbuf[0] = ASCII_BS;
            sendbuf[1] = ' ';
            sendbuf[2] = ASCII_BS;
            len = 3;
            break;

          case TELN_KEY_UP:
          case TELN_KEY_DOWN:
            /* Key Up/Down for command history */
            if ((len = get_hist (teln_s, len)) == 0) {
              break;
            }
            len--;
            DEBUGF (TELNET," Command history: %s\n",teln_s->LBuf);
            EvrNetTeln_CommandHistory (teln_s->LBuf, teln_s->BCnt);
            /* Overwrite previous command */
            max_dsize = len + teln_s->BCnt;
            if (len > teln_s->BCnt) {
              max_dsize = 4*len - max_dsize;
            }
            sendbuf = net_tcp_get_buf (max_dsize);
            memset (&sendbuf[0], ASCII_BS, max_dsize);
            memcpy (&sendbuf[len], &teln_s->LBuf[0], teln_s->BCnt);
            if (len > teln_s->BCnt) {
              memset (&sendbuf[len+teln_s->BCnt], ' ', len-teln_s->BCnt);
            }
            len = max_dsize;
            break;

          default:
            /* Echo the character(s) */
            if (((len >>= 8) == 0) || (telnc->NoEcho)) {
              /* No characters to echo */
              len = 0;
              break;
            }
            sendbuf = net_tcp_get_buf (len + 1);
            memcpy (&sendbuf[0], &teln_s->LBuf[teln_s->BCnt - len], len);
#ifdef __DEBUG_ENABLED
            sendbuf[len] = 0;
            DEBUGF (TELNET," Echo chars : %s\n",sendbuf);
            EvrNetTeln_EchoCharacters (sendbuf, len);
#endif
            break;
        }

        DEBUGF (TELNET," Line buffer: %d bytes\n",teln_s->BCnt);
        EvrNetTeln_LineBufferUsage (teln_s->Id, teln_s->BCnt);
        if (len) {
          len &= 0xFFFF;
          teln_s->Flags |= TELN_FLAG_WACK;
          net_tcp_send (teln_s->Socket, sendbuf, len);
          return;
        }
        break;

      case TELN_STATE_CONNECT:
        teln_s->UserId = 0;
        /* Connection established, send a header to client */
        max_dsize = net_tcp_get_mss (teln_s->Socket);
        sendbuf   = net_tcp_get_buf (max_dsize);
        /* Send Welcome message first */
        len    = netTELNETs_ProcessMessage (netTELNETs_MessageWelcome,
                                            (char *)sendbuf, max_dsize);
        if (teln->en_auth) {
          DEBUGF (TELNET,"Send authorization request\n");
          EvrNetTeln_SendAuthorizationRequest (teln_s->Id);
          len += netTELNETs_ProcessMessage (netTELNETs_MessageLogin,
                                            (char *)sendbuf+len, max_dsize-len);
          len += netTELNETs_ProcessMessage (netTELNETs_MessageUsername,
                                            (char *)sendbuf+len, max_dsize-len);
          teln_s->State = TELN_STATE_AUTHUSER;
          teln_s->Tout  = 60;
        }
        else {
          DEBUGF (TELNET,"Send initial header\n");
          EvrNetTeln_SendInitialHeader (teln_s->Id);
          len += netTELNETs_ProcessMessage (netTELNETs_MessagePrompt,
                                            (char *)sendbuf+len, max_dsize-len);
          teln_s->State = TELN_STATE_ACTIVE;
        }
send:   net_tcp_send (teln_s->Socket, sendbuf, len);
        teln_s->Flags |= TELN_FLAG_WACK;
        return;

      case TELN_STATE_AUTHUSER:
        if (sys->Flags & SYS_FLAG_SEC) {
          if (--teln_s->Tout == 0) {
            /* Login Timeout has expired */
            DEBUGF (TELNET," Login timeout\n");
            EvrNetTeln_LoginTimeoutExpired (teln_s->Id);
            max_dsize = net_tcp_get_mss (teln_s->Socket);
            sendbuf   = net_tcp_get_buf (max_dsize);
            len = netTELNETs_ProcessMessage (netTELNETs_MessageLoginTimeout,
                                             (char *)sendbuf, max_dsize);
            teln_s->Flags |= TELN_FLAG_CLOSE;
            goto send;
          }
        }
        /* New chars received to process? */
        if (teln_s->Ridx == teln_s->Widx) {
          break;
        }
        sys->Busy = true;

        switch (len = proc_data (teln_s)) {
          case ASCII_CR:
            max_dsize = net_tcp_get_mss (teln_s->Socket);
            sendbuf   = net_tcp_get_buf (max_dsize);
            if (teln_s->Flags & TELN_FLAG_PASSW) {
              DEBUGF (TELNET," Password: %s\n",teln_s->LBuf);
              EvrNetTeln_ShowPassword (teln_s->LBuf, teln_s->BCnt);
              if ((teln_s->Flags & TELN_FLAG_USEROK) == 0) {
err_auth:       ERRORF (TELNET,"Session %d, Authentication failed\n",teln_s->Id);
                EvrNetTeln_AuthenticationFailed (teln_s->Id); 
                len = netTELNETs_ProcessMessage (netTELNETs_MessageLoginFailed,
                                                 (char *)sendbuf, max_dsize);
                if (++teln_s->SVar <= 3) {
                  /* Retry authorization */
                  teln_s->SVar++;
                  teln_s->State  = TELN_STATE_CONNECT;
                  teln_s->Flags &= ~(TELN_FLAG_PASSW | TELN_FLAG_USEROK);
                  teln_s->BCnt   = 0;
                  break;
                }
                teln_s->Flags |= TELN_FLAG_CLOSE;
                break;
              }
              if (teln_s->UserId == 0) {
                /* Verify password for system administrator */
                if (teln->passw && teln->passw_len &&
                    strcmp (teln_s->LBuf, teln->passw) != 0) {
                  goto err_auth;
                }
              }
              else {
                /* Verify password for external user */
                if (!netTELNETs_CheckPassword (teln_s->UserId, teln_s->LBuf)) {
                  goto err_auth;
                }
              }

              DEBUGF (TELNET," Login success\n");
              EvrNetTeln_UserLoginSuccess (teln_s->Id);
              teln_s->State = TELN_STATE_ACTIVE;
              len  = netTELNETs_ProcessMessage (netTELNETs_MessagePrompt,
                                                (char *)sendbuf, max_dsize);
              teln_s->Flags &= ~TELN_FLAG_PASSW;
              teln_s->BCnt   = 0;
              break;
            }
            if (teln_s->BCnt == 0) {
              /* Only CR pressed */
              len = netTELNETs_ProcessMessage (netTELNETs_MessageUsername,
                                               (char *)sendbuf, max_dsize);
              break;
            }
            DEBUGF (TELNET," Username: %s\n",teln_s->LBuf);
            EvrNetTeln_ShowUsername (teln_s->LBuf, teln_s->BCnt);
            /* Check username of a system admin account */
            if (teln->user && teln->user_len &&
                strcmp (teln_s->LBuf,teln->user) == 0) {
              teln_s->Flags |= TELN_FLAG_USEROK;
            }
            /* Check username of optional external accounts */
            else if ((id = netTELNETs_CheckUsername (teln_s->LBuf)) > 0) {
              /* Username is valid, register user id */
              teln_s->UserId = id;
              teln_s->Flags |= TELN_FLAG_USEROK;
            }
            teln_s->Flags |= TELN_FLAG_PASSW;
            len = netTELNETs_ProcessMessage (netTELNETs_MessagePassword,
                                             (char *)sendbuf, max_dsize);
            teln_s->BCnt = 0;
            break;

          case ASCII_BS:
            /* Echo the Backspace sequence */
            if (telnc->NoEcho) {
              len = 0;
              break;
            }
            DEBUGF (TELNET," Echo backspace\n");
            EvrNetTeln_EchoBackspace (teln_s->Id);
            sendbuf = net_tcp_get_buf (3);
            sendbuf[0] = ASCII_BS;
            sendbuf[1] = ' ';
            sendbuf[2] = ASCII_BS;
            len = 3;
            break;

          default:
            /* Echo the character(s) */
            if (((len >>= 8) == 0) || (telnc->NoEcho)) {
              /* No characters to echo */
              len = 0;
              break;
            }
            sendbuf = net_tcp_get_buf (len + 1);
            if (teln_s->Flags & TELN_FLAG_PASSW) {
              /* Echo '*' characters for password */
              memset (&sendbuf[0], '*', len);
            }
            else {
              memcpy (&sendbuf[0], &teln_s->LBuf[teln_s->BCnt - len], len);
            }
#ifdef __DEBUG_ENABLED
            sendbuf[len] = 0;
            DEBUGF (TELNET," Echo chars : %s\n",sendbuf);
            EvrNetTeln_EchoCharacters (sendbuf, len);
#endif
            break;
        }
        if (len) {
          teln_s->Flags |= TELN_FLAG_WACK;
          net_tcp_send (teln_s->Socket, sendbuf, len);
          return;
        }
        break;

      case TELN_STATE_NEGOTIATE:
        /* Negotiate Telnet Parameters first */
        if ((teln_s->BCnt & 0x80) == 0) {
          /* First call to negotiate, send first option */
          teln_s->BCnt |= 0x80;
          teln_s->Tout  = 3*SYS_TICK_TSEC;
          DEBUGF (TELNET,"Negotiate start\n");
          EvrNetTeln_NegotiateStart (teln_s->Id);
          sendbuf = net_tcp_get_buf (12);

          sendbuf[0]  = TELN_CMD_IAC;
          sendbuf[1]  = TELN_CMD_DO;
          sendbuf[2]  = TELN_OPT_SUPGOAH;

          sendbuf[3]  = TELN_CMD_IAC;
          sendbuf[4]  = TELN_CMD_WILL;
          sendbuf[5]  = TELN_OPT_SUPGOAH;

          sendbuf[6]  = TELN_CMD_IAC;
          sendbuf[7]  = (telnc->NoEcho) ? TELN_CMD_DO   : TELN_CMD_DONT;
          sendbuf[8]  = TELN_OPT_ECHO;

          sendbuf[9]  = TELN_CMD_IAC;
          sendbuf[10] = (telnc->NoEcho) ? TELN_CMD_WONT : TELN_CMD_WILL;
          sendbuf[11] = TELN_OPT_ECHO;

          net_tcp_send (teln_s->Socket, sendbuf, 12);
          return;
        }
        if (sys->Flags & SYS_FLAG_TICK) {
          if (--teln_s->Tout == 0) {
            /* Negotiate Timeout has expired */
            teln_s->Flags |= TELN_FLAG_CLOSE;
            ERRORF (TELNET,"Session %d, Negotiate failed\n",teln_s->Id);
            EvrNetTeln_NegotiateFailed (teln_s->Id);
          }
        }
        break;

      case TELN_STATE_SENDING:
        /* Sending data from a repeated loop */
        if (teln_s->Flags & TELN_FLAG_DELAY) {
          /* When interval delays switched on */
          if (!(sys->Flags & SYS_FLAG_TICK)) {
            break;
          }
          if (--teln_s->Tout) {
            break;
          }
          teln_s->Flags &= ~TELN_FLAG_DELAY;
        }
        max_dsize = net_tcp_get_mss (teln_s->Socket);
        sendbuf   = net_tcp_get_buf (max_dsize);
        /* Repeat to use as much buffer as possible */
        for (len = 0; (len + 240) < max_dsize; ) {
          v = netTELNETs_ProcessCommand (teln_s->LBuf, (char *)sendbuf+len,
                                         max_dsize-len, &teln_s->SVar);
          len += (v & 0xFFFF);
          /* Bit-31 of return value is a repeat flag */
          if (!(v & 0x80000000) || !(v & 0xFFFF) || (
              (teln_s->Flags & TELN_FLAG_DELAY) && teln_s->Ridx != teln_s->Widx)) {
            /* Repeat flag not set, 0-length returned or keypressed */
            teln_s->BCnt  = 0;
            teln_s->Tout  = 0;
            teln_s->Ridx  = teln_s->Widx;
            teln_s->Flags &= ~TELN_FLAG_DELAY;
            teln_s->State = TELN_STATE_ACTIVE;
            net_tcp_set_option (teln_s->Socket, netTCP_OptionDelayedACK, 0);
            len += netTELNETs_ProcessMessage (netTELNETs_MessagePrompt,
                                              (char *)sendbuf+len, max_dsize-len);
            break;
          }
          if (teln_s->Flags & TELN_FLAG_DELAY) {
            /* Repeat command requested */
            break;
          }
        }
        teln_s->Flags |= TELN_FLAG_WACK;
        net_tcp_send (teln_s->Socket, sendbuf, len);
        return;
    }
  }
}

/**
  \brief       Socket event callback notification.
  \param[in]   socket  socket handle.
  \param[in]   event   event type.
  \param[in]   addr    structure containing remote IP address and port.
  \param[in]   buf     pointer to a buffer containing the data.
  \param[in]   len     length of the data.
  \return      status:
               - true  = success,
               - false = error.
*/
static uint32_t telnet_listener (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                                 const uint8_t *buf, uint32_t len) {
  NET_TELNET_INFO *teln_s;

  /* Find the right session */
  teln_s = telnet_map_session (socket);
  if (teln_s == NULL) {
    return (false);
  }
  teln->cmd_session = teln_s->Id;

  switch (event) {
    case netTCP_EventConnect:
      /* Access client filtering by IP and/or port */
      if (netTELNETs_AcceptClient (addr) == false) {
        ERRORF (TELNET,"Access denied [%s], port %d\n",
                        net_addr_ntoa((const __ADDR *)addr),addr->port);
        EvrNetTeln_UserAccessDenied (addr);
        /* User denied access for this host */
        return (false);
      }
      DEBUGF (TELNET,"Session %d open\n",teln_s->Id);
      EvrNetTeln_SessionOpen (teln_s->Id); 
      if (teln_s->State == TELN_STATE_IDLE) {
        teln_s->State = TELN_STATE_RESERVED;
      }
      break;

    case netTCP_EventAborted:
      DEBUGF (TELNET,"Session %d close, Socket aborted\n",teln_s->Id);
      EvrNetTeln_SocketAborted (teln_s->Id);
      telnet_kill_session (teln_s);
      break;

    case netTCP_EventEstablished:
      teln_s->State = TELN_STATE_NEGOTIATE;
      break;

    case netTCP_EventClosed:
      DEBUGF (TELNET,"Session %d close, Socket closed\n",teln_s->Id);
      EvrNetTeln_SocketClosed (teln_s->Id);
      telnet_kill_session (teln_s);
      break;

    case netTCP_EventACK:
      /* Data has been acknowledged by remote Telnet client */
      teln_s->Flags &= ~TELN_FLAG_WACK;
      break;

    case netTCP_EventData:
      DEBUGF (TELNET,"*** Process_frame ***\n");
      DEBUGF (TELNET," Received %d bytes\n",len);
      EvrNetTeln_ReceiveFrame (teln_s->Id, len);
      if (teln_s->State == TELN_STATE_NEGOTIATE) {
        teln_s->BCnt |= check_reply (buf, (int32_t)len);
        switch (teln_s->BCnt) {
          case 0x94:
            /* Will Echo */
          case 0x96:
            /* Will Echo & Don't Echo */
            if (telnc->NoEcho == 0) {
              return (true);
            }
            break;
          case 0x91:
            /* Do Echo */
          case 0x99:
            /* Do Echo & Won't Echo */
            if (telnc->NoEcho) {
              return (true);
            }
            break;
          default:
            return (true);
        }
        /* Negotiation is finished, continue */
        DEBUGF (TELNET,"Negotiate success\n");
        EvrNetTeln_NegotiateSuccess (teln_s->Id);
        /* Authorization attempt counter */
        teln_s->SVar  = 0;
        teln_s->BCnt  = 0;
        teln_s->State = TELN_STATE_CONNECT;
        break;
      }
      telnet_parse_cmd (teln_s, buf, (int32_t)len);
      break;
  }
  return (true);
}

/**
  \brief       Parse received telnet command.
  \param[in]   teln_s  session descriptor.
  \param[in]   buf     buffer containing telnet data.
  \param[in]   len     length of the data.
*/
static void telnet_parse_cmd (NET_TELNET_INFO *teln_s, const uint8_t *buf, int32_t len) {
  uint32_t ch;

  while (len > 0) {
    if (buf[0] == TELN_CMD_IAC) {
      /* Telnet Command starts with IAC Character */
      switch (buf[1]) {
        case TELN_CMD_DO:
          /* DO Command received */
    
        case TELN_CMD_DONT:
          /* DON'T Command received */
    
        case TELN_CMD_WILL:
          /* WILL Command received */
          
        case TELN_CMD_WONT:
          /* WON'T Command received */
          buf += 3;
          len -= 3;
          break;
    
        default:
          /* Other telnet-commands are 2-byte sequences */
          buf += 2;
          len -= 2;
          break;
      }
      continue;
    }
    /* Decode Key Code */
    switch (ch = buf[0]) {
      case ASCII_ESC:
        if (len > 2 && buf[1] == '[') {
          /* ANSI escape sequences */
          switch (buf[2]) {
            case '3':
              /* Ansi Delete key */
              if (buf[3] != '~') {
                ch = 0;
                goto inc3;
              }
              ch = ASCII_BS;
              buf += 4;
              len -= 4;
              break;

            case 'A':
              /* ANSI cursor Up key */
              ch = TELN_KEY_UP;
              goto inc3;

            case 'B':
              /* ANSI cursor Down key */
              ch = TELN_KEY_DOWN;
              goto inc3;

            case 'D':
              /* ANSI cursor Left key */

            case 'C':
              /* ANSI cursor Right key */

            default:
              /* Ignore other ANSI key codes */
              ch = 0;
inc3:         buf += 3;
              len -= 3;
              break;
          }
          break;
        }
        goto inc1;

      case ASCII_LF:
        /* Skip LF character */
        ch = 0;
        goto inc1;

      case ASCII_DEL:
        /* DEL Key pressed */
        ch = ASCII_BS;
        goto inc1;

      default:
        /* Use only 7-bit ASCII codes */
        if (ch & 0x80) {
          ch = 0;
        }
inc1:   buf++;
        len--;
        break;
    }

    if (ch != 0) {
      /* Store the character to fifo buffer */
      teln_s->Fifo[teln_s->Widx] = ch & 0xFF;
      teln_s->Widx = (teln_s->Widx + 1) & TELN_FIFOMSK;
    }
  }
}

/**
  \brief       Check negotiation reply from the client.
  \param[in]   buf  buffer containing negotiation data.
  \param[in]   len  length of the data.
  \return      negotiation status code.
*/
static uint8_t check_reply (const uint8_t *buf, int32_t len) {
  uint32_t val = 0;

  while (len > 0) {
    /* Get first parameter */
    if (buf[0] == TELN_CMD_IAC) {
      switch (buf[1]) {
        case TELN_CMD_DO:
          /* DO Command received */
          if (buf[2] == TELN_OPT_ECHO) {
            val |= 0x01;
          }
          break;

        case TELN_CMD_DONT:
          /* DON'T Command received */
          if (buf[2] == TELN_OPT_ECHO) {
            val |= 0x02;
          }
          break;

        case TELN_CMD_WILL:
          /* WILL Command received */
          if (buf[2] == TELN_OPT_ECHO) {
            val |= 0x04;
          }
          if (buf[2] == TELN_OPT_SUPGOAH) {
            val |= 0x10;
          }
          break;

        case TELN_CMD_WONT:
          /* WON'T Command received */
          if (buf[2] == TELN_OPT_ECHO) {
            val |= 0x08;
          }
          break;

        default:
          /* Other telnet commands are 2-byte sequences */
          buf += 2;
          len -= 2;
          continue;
      }
      buf += 3;
      len -= 3;
      continue;
    }
    /* Ignore garbage characters in negotiation */
    buf++;
    len--;
  }
  return (val & 0xFF);
}

/**
  \brief       Process data from the command buffer.
  \param[in]   teln_s  session descriptor.
  \return      number of characters to echo << 8 or
               control character code.
*/
static uint16_t proc_data (NET_TELNET_INFO *teln_s) {
  uint32_t ch,ri,cnt = 0;

  for (ri = teln_s->Ridx; ri != teln_s->Widx; teln_s->Ridx = ri & 0xFF) {
    ch  = teln_s->Fifo[ri++];
    ri &= TELN_FIFOMSK;
    switch (ch) {
      case ASCII_CR:
        /* Process input so far, remaining in FIFO must wait */
        if (cnt > 0) {
          return ((cnt << 8) & 0xFFFF);
        }
        teln_s->LBuf[teln_s->BCnt] = 0;
        teln_s->Ridx = ri & 0xFF;
        return (ch & 0xFF);

      case ASCII_ESC:
      case ASCII_CTRLC:
        /* Close connection request */
        teln_s->Flags |= TELN_FLAG_CLOSE;
        teln_s->Ridx   = ri & 0xFF;
        return (0);

      case ASCII_BS:
        /* Backspace, simulate line editor */
        if (cnt > 0) {
          /* Character not yet echoed */
          teln_s->BCnt--;
          cnt--;
          break;
        }
        if (teln_s->BCnt > 0) {
          /* Character already echoed */
          teln_s->BCnt--;
          teln_s->Ridx = ri & 0xFF;
          return (ch & 0xFF);
        }
        break;

      case TELN_KEY_UP:
      case TELN_KEY_DOWN:
        /* Cursor Up/Down keys */
        teln_s->Ridx = ri & 0xFF;
        return (ch & 0xFF);

      default:
        if (teln_s->BCnt < TELNET_LBUF_SZ-1) {
          /* Store the character to command buffer */
          teln_s->LBuf[teln_s->BCnt] = ch & 0xFF;
          teln_s->BCnt++;
          cnt++;
        }
        break;
    }
  }
  /* Number of characters to echo */
  return ((cnt << 8) & 0xFFFF);
}

/**
  \brief       Store a command to history buffer.
  \param[in]   teln_s  session descriptor.
*/
static void store_hist (NET_TELNET_INFO *teln_s) {
  /* Was this command from the history buffer? */
  if (strcmp(&teln_s->LBuf[0], &teln_s->Hist[teln_s->hCurr]) == 0) {
    if (teln_s->hCurr > 0) {
      /* Move the last command to the top of history buffer */
      memmove (&teln_s->Hist[teln_s->BCnt+1], &teln_s->Hist[0],teln_s->hCurr);
    }
  }
  else {
    /* Make space for a new command */
    memmove (&teln_s->Hist[teln_s->BCnt+1], &teln_s->Hist[0],
             (TELNET_HIST_SZ-1) - teln_s->BCnt);
    teln_s->Hist[TELNET_HIST_SZ-1] = 0;
  }
  net_strcpy (&teln_s->Hist[0], &teln_s->LBuf[0]);
  teln_s->hCurr = 0;
  teln_s->hNext = 0;
}

/**
  \brief       Get a command from history buffer.
  \param[in]   teln_s  session descriptor.
  \param[in]   key     direction key-code (Up/Down).
  \return      number of characters to erase+1 or
               0= no history existing yet.
*/
static uint32_t get_hist (NET_TELNET_INFO *teln_s, uint32_t key) {
  uint32_t i, ndel, pos;

  ndel = 0;
  if (key == TELN_KEY_UP) {
    /* Up Key, get older commands */
    pos = teln_s->hNext;
    if (teln_s->Hist[pos]) {
      /* Yes a history command is existing */
      ndel         = teln_s->BCnt + 1;
      teln_s->BCnt = net_strcpy (&teln_s->LBuf[0], &teln_s->Hist[pos]) & 0xFF;
      teln_s->hCurr= pos & 0xFF;
      pos         += teln_s->BCnt + 1;
      if (pos < TELNET_HIST_SZ-1) {
        /* Pos is within boundaries */
        teln_s->hNext = pos & 0xFF;
      }
    }
  }
  else {
    /* Down Key, get newer commands */
    if (teln_s->hNext > 0) {
      pos           = teln_s->hCurr;
      teln_s->hNext = pos & 0xFF;
      ndel          = teln_s->BCnt + 1;
      teln_s->BCnt  = 0; 
      if (pos > 0) {
        /* Find previous command */
        for (i = pos = 0; i < (teln_s->hCurr - 1); i++) {
          if (teln_s->Hist[i] == 0) {
            pos = i + 1;
          }
        }
        teln_s->hCurr = pos & 0xFF;
        teln_s->BCnt  = net_strcpy (&teln_s->LBuf[0], &teln_s->Hist[pos]) & 0xFF;
      }
    }
  }
  /* Return number of characters to erase+1 */
  return (ndel);
}

/**
  \brief       Kill active Telnet session.
  \param[in]   teln_s  session descriptor.
*/
static void telnet_kill_session (NET_TELNET_INFO *teln_s) {
  teln_s->State  = TELN_STATE_IDLE;
  teln_s->BCnt   = 0;
  teln_s->Flags  = 0;
  teln_s->Tout   = 0;
  teln_s->Widx   = 0;
  teln_s->Ridx   = 0;
  teln_s->hCurr  = 0;
  teln_s->hNext  = 0;
  teln_s->Hist[0]= 0;
}

/**
  \brief       Map TCP socket to existing Telnet session.
  \param[in]   socket  TCP socket handle.
  \return      session descriptor or NULL= none.
*/
static NET_TELNET_INFO *telnet_map_session (int32_t socket) {
  NET_TELNET_INFO *teln_s;
  int32_t i;

  for (teln_s = &telnc->Scb[0], i = 0; i < telnc->NumSess; teln_s++, i++) {
    if (teln_s->Socket == socket) {
      return (teln_s);
    }
  }
  return (NULL);
}

/**
  \brief       Retrieve the user identification.
  \return      user identification number (0 = system administrator)
  \note        This function is called from Telnet server user interface.
               The 'cmd_session' is set and valid for the call.
*/
uint8_t netTELNETs_GetUserId (void) {
  return (telnc->Scb[teln->cmd_session-1].UserId);
}

/* Library default function(s) */
__WEAK bool netTELNETs_AcceptClient (const NET_ADDR *addr) {
  (void)addr; return (true); }
__WEAK uint8_t netTELNETs_CheckUsername (const char *username) {
  (void)username; return (0); }
__WEAK bool netTELNETs_CheckPassword (uint8_t user_id, const char *password) {
  (void)user_id; (void)password; return (false); }
