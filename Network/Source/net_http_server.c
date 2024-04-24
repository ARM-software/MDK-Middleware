/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_http_server.c
 * Purpose: Web Server
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_mem.h"
#include "net_addr.h"
#include "net_base64.h"
#include "net_tcp.h"
#include "net_http_server.h"
#include "net_dbg.h"

/* Local constants */
static const char Method[8][8] = {"GET","POST","HEAD","PUT",
                                  "DELETE","CONNECT","OPTIONS","TRACE"};
/* Length of method tokens */
static const int8_t TokenLen[8]= { 3,4,4,3,6,7,7,5 };
static const char Month[12][4] = {"Jan","Feb","Mar","Apr","May","Jun",
                                  "Jul","Aug","Sep","Oct","Nov","Dec"};
static const char WkDay[7][4]  = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};

/* Days elapsed in one non-leap year at the beginning of each month */
static const uint16_t DMonth[12] = {0,31,59,90,120,151,181,212,243,273,304,334};

/* Global variables */
NET_HTTP_CTRL net_http_control;

/* Local variables */
static NET_CGI_INFO cgi;

/* Interface functions */
#define auth_io (httpc->auth_if)        // Authentication support interface
#define tls_io  (httpc->tls_if)         // mbedTLS interface

/* Local functions */
static void http_server_start (void);
static void http_server_stop (void);
static void http_server_run (void);
static uint32_t http_listener (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len);
static NET_HTTP_INFO *http_map_session (int32_t socket);
static void http_kill_session (NET_HTTP_INFO *http_s);
static int32_t http_add_header (NET_HTTP_INFO *http_s, char *buf);
static int32_t http_error_msg (NET_HTTP_INFO *http_s, char *buf, uint32_t err);
static int32_t add_server_id (char *buf);
static void http_open_file (NET_HTTP_INFO *http_s, const char *name);
static void http_set_ftype (NET_HTTP_INFO *http_s, const char *name);
static uint16_t http_load_buf (NET_HTTP_INFO *http_s, uint8_t *buf, uint32_t max_len);
static bool http_parse_method (NET_HTTP_INFO *http_s, const char *buf);
static const char *http_proc_uri (NET_HTTP_INFO *http_s, char *buf, const char **env);
static void http_parse_header (NET_HTTP_INFO *http_s, char *hdr, const char *uri);
static uint32_t http_to_utc (const char *buf) ;
static uint32_t utc_to_http (uint32_t utc, char *buf);
static const char *chk_post_req (NET_HTTP_INFO *http_s, const char *buf);
static bool http_proc_post (NET_HTTP_INFO *http_s, const char *buf);
static bool http_proc_multipart (NET_HTTP_INFO *http_s, const char *buf);
static const char *search_delim (const char *delim, const char *buf, int32_t len);
static const char *get_next_field (const char *buf, const char *end);
static const char *get_next_par (const char *buf, const char *end);
static int32_t http_strcpy (char *dp, const char *sp, int32_t max_len);
static void fix_fname (char *fname);
static void read_script_line (NET_HTTP_INFO *http_s);
static bool is_cookie_valid (const char *cookie);
#ifdef Network_Debug_STDIO
 static void debug_inf2 (const char *cookie, uint32_t len);
#endif

/**
  \brief       Initialize HTTP server.
*/
void net_http_server_init (void) {
  NET_HTTP_INFO *http_s;
  int32_t sock,i;

  DEBUGF (HTTP,"Init %d Sessions, Port %d\n",httpc->NumSess,httpc->PortNum);
  EvrNetHTTPs_InitServer (httpc->NumSess, httpc->PortNum);
#ifdef __DEBUG_ENABLED
  if (httpc->Root) {
    DEBUGF (HTTP," Root folder \"%s\"\n",httpc->Root);
    EvrNetHTTPs_ViewRootFolder (httpc->Root, strlen(httpc->Root));
  }
#endif
  /* Clear SCB for all sessions */
  memset (httpc->Scb, 0, sizeof (*httpc->Scb) * httpc->NumSess);

  for (i = 1, http_s = &httpc->Scb[0]; i <= httpc->NumSess; http_s++, i++) {
    /* Allocate TCP socket */
    sock = net_tcp_get_socket (http_listener);
    if (sock < 0) sock = 0;
    http_s->Socket = sock & 0xFF;
    if (http_s->Socket == 0) {
      http_s->State = HTTP_STATE_ERROR;
      ERRORF (HTTP,"Session %d, Get socket failed\n",i);
      EvrNetHTTPs_GetSocketFailed (i & 0xFF);
      net_sys_error (NET_ERROR_TCP_ALLOC);
      return;
    }
    if (tls_io) {
      /* Allocate and initialize TLS context for HTTPS */
      http_s->TlsId = tls_io->get_context (http_s->Socket, http_listener);
      if (http_s->TlsId == 0) {
        http_s->State = HTTP_STATE_ERROR;
        ERRORF (HTTP,"Session %d, TLS get context failed\n",i);
        EvrNetHTTPs_TlsGetContextFailed (i & 0xFF);
      }
    }
    http_s->Id = i & 0xFF;
  }

  /* Init service control */
  memset (http, 0, sizeof(*http));
  http->root     = httpc->Root;
  http->port_num = httpc->PortNum;
  if (http->port_num == 0) {
    /* Port not specified, use default server port */
    http->port_num = (tls_io) ? HTTPS_SERVER_PORT : HTTP_SERVER_PORT;
  }
  if (auth_io) auth_io->init ();
  http->hdr_access = true;
  http->en_cookie  = true;

  if (sysc->AutoStart) {
    http_server_start ();
  }
}

/**
  \brief       De-initialize HTTP server.
*/
void net_http_server_uninit (void) {
  NET_HTTP_INFO *http_s;
  int32_t i;

  DEBUGF (HTTP,"Uninit Server\n");
  EvrNetHTTPs_UninitServer ();

  for (i = 0, http_s = &httpc->Scb[0]; i < httpc->NumSess; http_s++, i++) {
    /* Close local file if open */
    if (http_s->Flags & HTTP_FLAG_DFILE) {
      netHTTPs_fclose (http_s->dFile);
    }
    if (http_s->Flags & HTTP_FLAG_SFILE) {
      netHTTPs_fclose (http_s->sFile);
    }
    /* Close upload file if upload in progress */
    if (http_s->Flags & HTTP_FLAG_UFILE) {
      netCGI_ProcessData (3, NULL, 0);
    }
    /* Clear session control block */
    memset (http_s, 0, sizeof(*http_s));
  }

  /* Uninit authentication */
  if (auth_io) auth_io->uninit ();

  /* Uninit service control */
  memset (http, 0, sizeof(*http));
}

/**
  \brief       Start HTTP server service.
  \return      status code as defined with netStatus.
*/
netStatus netHTTPs_Start (void) {

  START_LOCK (netStatus);

  if (!http->run_service) {
    /* Service is down */
    http_server_start ();
  }

  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Stop HTTP server service.
  \return      status code as defined with netStatus.
*/
netStatus netHTTPs_Stop (void) {

  START_LOCK (netStatus);

  if (http->run_service) {
    /* Service is running */
    http_server_stop ();
  }

  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Check if HTTP server service is running.
  \return      service status:
               - true  = service is running.
               - false = service is not running.
*/
bool netHTTPs_Running (void) {
  return (http->run_service);
}

/**
  \brief       Get current port number of HTTP server.
  \return      current port number.
*/
uint16_t netHTTPs_GetPort (void) {
  return (http->port_num);
}

/**
  \brief       Set HTTP server port number.
  \param[in]   port  service port number.
  \return      status code as defined with netStatus.
*/
netStatus netHTTPs_SetPort (uint16_t port) {
  if (http->run_service) {
    return (netWrongState);
  }
  http->port_num = port;
  return (netOK);
}

/**
  \brief       Retrieve current root folder path.
  \return      pointer to root folder path, a null-terminated string or
                 NULL if root folder is disabled.
*/
const char *netHTTPs_GetRootPath (void) {
  return (http->root);
}

/**
  \brief       Change root folder path.
  \param[in]   path  new root folder path.
  \return      status code as defined with netStatus.
*/
netStatus netHTTPs_SetRootPath (const char *path) {
  static char root_buf [NET_ROOT_DIR_LEN];

  START_LOCK (netStatus);

  if ((path == NULL) || strlen(path) >= NET_ROOT_DIR_LEN) {
    RETURN (netInvalidParameter);
  }
  if (!http->root) {
    /* Root folder not enabled for the service */
    RETURN (netError);
  }
  if (http->run_service) {
    /* Service is running */
    RETURN (netWrongState);
  }
  DEBUGF (HTTP,"SetRootFolder \"%s\"\n",path);
  EvrNetHTTPs_SetRootPath (path, strlen(path));
  /* Use a static buffer instead of default const array */
  http->root = root_buf;
  net_strcpy (root_buf, path);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Retrieve username of system administrator account.
  \return      pointer to username, a null-terminated string or
                 NULL if authentication is disabled.
*/
const char *netHTTPs_GetUsername (void) {
  return (http->user);
}

/**
  \brief       Set username of system administrator account.
  \param[in]   username  new username.
  \return      status code as defined with netStatus.
*/
netStatus netHTTPs_SetUsername (const char *username) {
  static char user_buf [NET_USERNAME_LEN];

  START_LOCK (netStatus);

  if ((username == NULL) || strlen(username) >= NET_USERNAME_LEN) {
    RETURN (netInvalidParameter);
  }
  if (!http->user) {
    /* Administrator account not enabled */
    RETURN (netError);
  }
  DEBUGF (HTTP,"SetUsername \"%s\"\n",username);
  EvrNetHTTPs_SetUsername (username, strlen(username));
  /* Use a static buffer instead of default const array */
  http->user     = user_buf;
  http->user_len = net_strcpy (user_buf, username) & 0xFF;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Retrieve password of system administrator account.
  \return      pointer to password, a null-terminated string or
                 NULL if authentication is disabled.
*/
const char *netHTTPs_GetPassword (void) {
  return (http->passw);
}

/**
  \brief       Set password of system administrator account.
  \param[in]   password  new password.
  \return      status code as defined with netStatus.
*/
netStatus netHTTPs_SetPassword (const char *password) {
  static char passw_buf [NET_HA1_HASH_LEN+1];
  int32_t len;

  START_LOCK (netStatus);

  if (password == NULL) {
    RETURN (netInvalidParameter);
  }
  len = (int32_t)strlen(password);
  if ((len >= NET_PASSWORD_LEN) && (len != NET_HA1_HASH_LEN)) {
    RETURN (netInvalidParameter);
  }
  if (!http->passw) {
    /* Administrator account not enabled */
    RETURN (netError);
  }
  DEBUGF (HTTP,"SetPassword \"%s\"\n",password);
  EvrNetHTTPs_SetPassword (password, (uint32_t)len);
  /* Use a static buffer instead of default const array */
  http->passw     = passw_buf;
  http->passw_len = net_strcpy (passw_buf, password) & 0xFF;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Calculate HA1 hash value for the given credentials.
  \param[in]   username  pointer to username.
  \param[in]   password  pointer to password.
  \param[out]  buf       buffer to store hexadecimal HA1 to.
  \param[in]   buf_len   length of the buffer.
  \return      status code as defined with netStatus.
  \note        Adds also a null-termination.
*/
netStatus netHTTPs_CalcHashHA1 (const char *username,
                                const char *password, char *buf, uint32_t buf_len) {
  if ((username == NULL) || (password == NULL) || (buf == NULL) || (buf_len < 33)) {
    return (netInvalidParameter);
  }
  if (!auth_io || !auth_io->calc_hash) {
    /* Digest authentication not enabled */
    return (netError);
  }
  /* Calculate MD5 hash */
  auth_io->calc_hash (username, password, (uint8_t *)&buf[16]);
  /* Convert MD5 hash to hex string HA1 */
  net_bin2hex (&buf[0], &buf[16], 16);
  return (netOK);
}

/**
  \brief       Determine server authentication status.
  \return      status:
               - true  = authentication enabled,
               - false = authentication not enabled.
*/
bool netHTTPs_LoginActive (void) {
  return (http->en_auth);
}

/**
  \brief       Enable or disable server authentication.
  \param[in]   enable  - true=  enable authentication,
                       - false= disable authentication.
  \return      status code as defined with netStatus.
*/
netStatus netHTTPs_LoginOnOff (bool enable) {
  if (enable && !auth_io) {
    /* Authentication not enabled in the configuration */
    return (netError);
  }
  http->en_auth = enable;
  return (netOK);
}

/**
  \brief       Main HTTP server scheduler.
*/
void net_http_server_run (void) {
  if (http->run_service) {
    if (auth_io) auth_io->run ();
    http_server_run ();
  }
}

/**
  \brief       Start HTTP server service.
*/
static void http_server_start (void) {
  NET_HTTP_INFO *http_s;
  int32_t i;

  DEBUGF (HTTP,"Start service, port %d\n",http->port_num);
  EvrNetHTTPs_StartService (http->port_num);
  for (i = 1, http_s = &httpc->Scb[0]; i <= httpc->NumSess; http_s++, i++) {
    if (http_s->State != HTTP_STATE_ERROR) {
      net_tcp_listen (http_s->Socket, http->port_num);
      net_tcp_set_option (http_s->Socket, netTCP_OptionDelayedACK, 1);
      if (tls_io) {
        tls_io->listen (http_s->TlsId);
      }
      http_s->State = HTTP_STATE_IDLE;
    }
  }
  http->run_service = true;
}

/**
  \brief       Stop HTTP server service.
*/
static void http_server_stop (void) {
  NET_HTTP_INFO *http_s;
  int32_t i;

  DEBUGF (HTTP,"Stop service\n");
  EvrNetHTTPs_StopService ();
  for (i = 1, http_s = &httpc->Scb[0]; i <= httpc->NumSess; http_s++, i++) {
    if (http_s->State != HTTP_STATE_ERROR) {
      if (tls_io) tls_io->close (http_s->TlsId, 0);
      else        net_tcp_abort (http_s->Socket);
      http_kill_session (http_s);
    }
  }
  http->run_service = false;
}

/**
  \brief       Run HTTP server service.
*/
static void http_server_run (void) {
  static uint8_t session = 0;
  NET_HTTP_INFO *http_s;
  uint8_t *sendbuf;
  uint32_t i,v,len,max_dsize,err;

  /* Check all sessions if there is something to do */
  for (i = 0; i < httpc->NumSess; i++) {
    if (++session > httpc->NumSess) {
      session = 1;
    }
    cgi.session = session;

    http_s = &httpc->Scb[session-1];
    switch (http_s->State) {
      case HTTP_STATE_IDLE:
      case HTTP_STATE_RESERVED:
        /* Keep TCP sockets listening */
        if (net_tcp_get_state (http_s->Socket) < netTCP_StateLISTEN) {
          /* This socket is not listening, activate listening again */
          net_tcp_listen (http_s->Socket, http->port_num);
        }
        break;

      case HTTP_STATE_FILE:
        /* Send a file to HTTP client */
        if (http_s->Flags & (HTTP_FLAG_NOFILE | HTTP_FLAG_NOACCESS | HTTP_FLAG_CACHED)) {
          /* Send an error message */
          if (!net_mem_avail_tx()) {
            /* Wait, no memory available */
            break;
          }
          if (http_s->Flags & HTTP_FLAG_NOFILE) {
            /* File not found error */
            if (http_s->pUser != NULL) {
              /* URL is redirected to new location */
              err = HTTP_ERR_301;
            }
            else {
              ERRORF (HTTP,"Session %d, File not found\n",http_s->Id);
              EvrNetHTTPs_FileNotFound (http_s->Id);
              err = HTTP_ERR_404;
            }
          }
          else if (http_s->Flags & HTTP_FLAG_NOACCESS) {
            /* File access not allowed */
            ERRORF (HTTP,"Session %d, File access forbidden\n",http_s->Id);
            EvrNetHTTPs_FileAccessForbidden (http_s->Id);
            err = HTTP_ERR_403;
          }
          else {
            /* File already cached in browser */
            DEBUGF (HTTP," Session %d, File cached\n",http_s->Id);
            EvrNetHTTPs_FileCached (http_s->Id);
            err = HTTP_ERR_304;
          }
          goto send_err;
        }
        if (http_s->BCnt != 0) {
          /* Data not acknowledged yet */
          break;
        }
        /* End of data? */
        if (http_s->Count >= http_s->DLen) {
          if (http_s->Flags & HTTP_FLAG_CGI) {
            if (http_s->Flags & HTTP_FLAG_DFILE) {
              /* Close an include file on external drive */
              http_s->Flags &= ~HTTP_FLAG_DFILE;
              netHTTPs_fclose (http_s->dFile);
            }
            read_script_line (http_s);
            http_s->State = HTTP_STATE_CGI;
            sys->Busy = true;
            return;
          }
close:    DEBUGF (HTTP,"Session %d close\n",http_s->Id);
          EvrNetHTTPs_CloseSession (http_s->Id);
          if (tls_io) tls_io->close (http_s->TlsId, 1);
          else        net_tcp_close (http_s->Socket);
          http_kill_session (http_s);
          sys->Busy = true;
          return;
        }

        /* More data to send */
        if (!net_mem_avail_tx()) {
          /* Wait, no memory available */
          break;
        }
        max_dsize = net_tcp_get_mss (http_s->Socket);
        if (tls_io) {
          max_dsize -= HTTPS_HEADER_LEN;
          sendbuf    = tls_io->get_buf (max_dsize);
        }
        else {
          sendbuf    = net_tcp_get_buf (max_dsize | 0x80000000);
        }
        if (sendbuf == NULL) {
          /* Wait, no memory available */
          break;
        }

        len = 0;
        if ((http_s->Flags & HTTP_FLAG_HEADER) == 0) {
          /* Send HTTP Server header reply before data */
          http_s->Flags |= HTTP_FLAG_HEADER;
          len = (uint32_t)http_add_header (http_s, (char *)sendbuf);
          if (http_s->Method == HTTP_METHOD_HEAD) {
            /* HEAD method, send only http header and no message-body */
            goto done;
          }
          DEBUGF (HTTP,"Session %d, Send file\n",http_s->Id);
          EvrNetHTTPs_SendFile (http_s->Id);
        }

        http_s->BCnt = http_load_buf (http_s, sendbuf+len, max_dsize-len);
        if (tls_io) {
          tls_io->write (http_s->TlsId, sendbuf, http_s->BCnt + len);
        }
        else {
          net_tcp_send (http_s->Socket, sendbuf, http_s->BCnt + len);
        }
        sys->Busy = true;
        return;

      case HTTP_STATE_CGI:
        /* Read one line of script and decide what to do next */
again:  switch (http_s->Script[0]) {
          /* First character of script is a command */
          case 't':
            /* Send a text string */
          case 'c':
            /* Call a 'CGI' format function */
            if (!net_mem_avail_tx()) {
              /* Wait, no memory available */
              break;
            }
            max_dsize = net_tcp_get_mss (http_s->Socket);
            if (tls_io) {
              max_dsize -= HTTPS_HEADER_LEN;
              sendbuf    = tls_io->get_buf (max_dsize);
            }
            else {
              sendbuf    = net_tcp_get_buf (max_dsize | 0x80000000);
            }
            if (sendbuf == NULL) {
              /* Wait, no memory available */
              break;
            }

            len = 0;
            if ((http_s->Flags & HTTP_FLAG_HEADER) == 0) {
              /* Send HTTP Server header reply before data */
              http_s->Flags |= HTTP_FLAG_HEADER;
              DEBUGF (HTTP,"Session %d, CGI send file\n",http_s->Id);
              EvrNetHTTPs_CgiSendFile (http_s->Id);
              len = (uint32_t)http_add_header (http_s, (char *)sendbuf);
            }

            while (http_s->Script[0] == 't') {
              /* Send as many 't' records as possible in one packet */
t_rep:        len += (uint32_t)http_strcpy ((char *)sendbuf+len, &http_s->Script[2],
                                            (int32_t)(max_dsize-len));
              if (len + HTTP_CGI_MAXLINE > max_dsize) {
                goto c_done;
              }
              read_script_line (http_s);
            }

            if (http_s->Script[0] != 'c') {
              http_s->Flags |= HTTP_FLAG_NEXT;
              goto c_done;
            }

            while (http_s->Script[0] == 'c') {
              if ((http_s->Flags & HTTP_FLAG_CGIREP) == 0) {
                /* First execution of the script line */
                http_s->CGIvar = 0;
              }
c_rep:        v = netCGI_Script (&http_s->Script[2], (char *)sendbuf+len, 
                                 max_dsize-len, &http_s->CGIvar);
              len += (v & 0xFFFF);
              if (v & 0x80000000) {
                /* Bit-31 of return value is a repeat flag */
                http_s->Flags |= HTTP_FLAG_CGIREP;
              }
              else {
                http_s->Flags &= ~HTTP_FLAG_CGIREP;
              }
              if (v & 0x40000000) {
                /* Bit-30 of return value is a force-send flag */
                goto c_done;
              }
              /* Check for free space in a buffer */
              if (len + 240 > max_dsize) {
                goto c_done;
              }
              if (http_s->Flags & HTTP_FLAG_CGIREP) {
                goto c_rep;
              }
              read_script_line (http_s);
            }
            /* Repeat if still space in a buffer */
            if (http_s->Script[0] == 't') {
              goto t_rep;
            }
            http_s->Flags |= HTTP_FLAG_NEXT;

c_done:     http_s->BCnt = len & 0xFFFF;
            if (tls_io) {
              tls_io->write (http_s->TlsId, sendbuf, len);
            }
            else {
              net_tcp_send (http_s->Socket, sendbuf, len);
            }
            http_s->State = HTTP_STATE_TEXT;
            sys->Busy = true;
            return;

          case 'i':
            /* Include a file */
            DEBUGF (HTTP,"Session %d, CGI include file\n",http_s->Id);
            EvrNetHTTPs_CgiIncludeFile (http_s->Id);
            if (http_s->Flags & HTTP_FLAG_SFILE) {
              fix_fname (&http_s->Script[2]);
            }
            http_open_file (http_s, &http_s->Script[2]);
            http_s->Count = 0;
            http_s->State = HTTP_STATE_FILE;
            sys->Busy = true;
            return;

          case '#':
            /* Comment line, ignore it */
            read_script_line (http_s);
            goto again;

          case '.':
            /* End of script, close session */
            DEBUGF (HTTP,"Session %d, CGI stop engine\n",http_s->Id);
            EvrNetHTTPs_CgiStopEngine (http_s->Id);
            goto close;

          default:
            /* Invalid command or unterminated script */
            ERRORF (HTTP,"Session %d, CGI script error\n",http_s->Id);
            EvrNetHTTPs_CgiScriptError (http_s->Id);
            if (tls_io) tls_io->close (http_s->TlsId, 0);
            else        net_tcp_abort (http_s->Socket);
            http_kill_session (http_s);
            sys->Busy = true;
            return;
        }
        break;

      case HTTP_STATE_TEXT:
        if (http_s->BCnt != 0) {
          /* Data not acknowledged yet */
          break;
        }
        http_s->State = HTTP_STATE_CGI;
        if ((http_s->Flags & (HTTP_FLAG_CGIREP | HTTP_FLAG_NEXT)) == 0) {
          /* Go on the next script line */
          read_script_line (http_s);
        }
        http_s->Flags &= ~HTTP_FLAG_NEXT;
        sys->Busy = true;
        return;

      case HTTP_STATE_UNAUTH:
        /* Unauthorized access to HTTP server */
      case HTTP_STATE_NOTIMP:
        /* Not implemented HTTP request method */
        if (!net_mem_avail_tx()) {
          /* Wait, no memory available */
          break;
        }
        if (http_s->State == HTTP_STATE_UNAUTH) {
          ERRORF (HTTP,"Session %d, Unauthorized access\n",http_s->Id);
          EvrNetHTTPs_UnauthorizedAccess (http_s->Id);
          err = HTTP_ERR_401;
        }
        else {
          ERRORF (HTTP,"Session %d, Method not implemented\n",http_s->Id);
          EvrNetHTTPs_MethodNotImplemented (http_s->Id);
          err = HTTP_ERR_501;
        }

send_err:
        max_dsize = net_tcp_get_mss (http_s->Socket);
        if (tls_io) {
          max_dsize -= HTTPS_HEADER_LEN;
          sendbuf    = tls_io->get_buf (max_dsize);
        }
        else {
          sendbuf    = net_tcp_get_buf (max_dsize);
        }
        len = (uint32_t)http_error_msg (http_s, (char *)sendbuf, err);

done:   DEBUGF (HTTP,"Session %d close\n",http_s->Id);
        EvrNetHTTPs_CloseSession (http_s->Id);
        if (tls_io) {
          tls_io->write (http_s->TlsId, sendbuf, len);
          tls_io->close (http_s->TlsId, 1);
        }
        else {
          net_tcp_send  (http_s->Socket, sendbuf, len);
          net_tcp_close (http_s->Socket);
        }
        http_kill_session (http_s);
        sys->Busy = true;
        return;
    }
  }
}

/**
  \brief       Process environment variables and convert to ANSI format.
  \param[in]   env      pointer to environment variables.
  \param[out]  ansi     output buffer to write decoded variable to.
  \param[in]   max_len  size of the output buffer.
  \return      status information:
               - pointer to the next environment variable for processing or
               - NULL if there are no more environment variables.
  \note        This function is called from user CGI module.
*/
const char *netCGI_GetEnvVar (const char *env, char *ansi, uint32_t max_len) {
  char *buf = ansi;
  uint8_t ch;

  /* Check environment pointer for possible overflow */
  while ((env[0] > ' ') && (env < cgi.end)) {
    switch (env[0]) {
      case '+':
        /* Decode '+' to ' ' */
        ch = ' ';
        env++;
        break;
      case '%':
        /* Decode "%xx" hex to char */
        ch = net_xtouc (&env[1]);
        env += 3;
        break;
      case '&':
        /* Stop on next variable */
        buf[0] = 0;
        return (env+1);
      default:
        /* Copy ascii char */
        ch = env[0];
        env++;
        break;
    }
    /* Check if buffer space available */
    if (max_len > 1) {
      buf[0] = ch;
      max_len--;
      buf++;
    }
  }
  /* End of HTTP query string */
  buf[0] = 0;
  return (NULL);
}

/**
  \brief       Retrieve the preferred language setting from the browser.
  \return      pointer to the language code, a null-terminated string.
  \note        The 'cgi.session' is set and valid for the call.
*/
const char *netHTTPs_GetLanguage (void) {
  /* String encoding: "en-us", "en-uk", "de", "fr", etc. */
  return (httpc->Scb[cgi.session-1].Lang);
}

/**
  \brief       Get IP address and port number of connected HTTP client.
  \param[out]  addr      structure that will receive IP address and port number.
  \param[in]   addr_len  size of NET_ADDR structure for the client.
  \return      status code as defined with netStatus.
  \note        This function is called from CGI user module.
               The 'cgi.session' is set and valid for the call.
*/
netStatus netHTTPs_GetClient (NET_ADDR *addr, uint32_t addr_len) {
  return (net_tcp_get_peer (httpc->Scb[cgi.session-1].Socket, (__ADDR *)addr, addr_len));
}

/**
  \brief       Get current session number of HTTP server.
  \return      current session number.
  \note        This function is called from CGI user module.
               The 'cgi.session' is set and valid for the call.
*/
int32_t netHTTPs_GetSession (void) {
  return (cgi.session-1);
}

/**
  \brief       Retrieve the user identification.
  \return      user identification number (0 = system administrator).
  \note        This function is called from CGI user module.
               The 'cgi.session' is set and valid for the call.
*/
uint8_t netHTTPs_GetUserId (void) {
  return (httpc->Scb[cgi.session-1].UserId);
}

/**
  \brief       Get Content-Type HTML header for other type POST requests.
  \return      pointer to content type header, a null-terminated string.
  \note        This function is called from CGI user module.
               The 'cgi.session' is set and valid for the call.
*/
const char *netHTTPs_GetContentType (void) {
  NET_HTTP_INFO *http_s = &httpc->Scb[cgi.session-1];

  if (http_s->Flags & HTTP_FLAG_XPOST) {
    /* Other MIME type: XML, JSON, plain text, etc */
    return (http_s->pDelim);
  }
  return (NULL);
}

/**
  \brief       Set HTTP cookie to send to the client.
  \param[in]   cookie  cookie definition, a null-terminated string.
  \return      status code as defined with netStatus.
  \note        This function is called from CGI user module.
               The 'cgi.session' is set and valid for the call.
*/
netStatus netCGI_SetCookie (const char *cookie) {
  NET_HTTP_INFO *http_s = &httpc->Scb[cgi.session-1];
  NET_BUFFER *netbuf,*next;
  int32_t len;

  START_LOCK (netStatus);

  if (cookie == NULL) {
    RETURN (netInvalidParameter);
  }
  len = (int32_t)strlen(cookie);
  if ((len >= 120) || !is_cookie_valid(cookie)) {
    /* Cookie too long or contains invalid characters */
    RETURN (netInvalidParameter);
  }
  if (!http->en_cookie) {
    /* netCGI_ProcessCookie function not provided */
    RETURN (netError);
  }
  if ((http_s->State != HTTP_STATE_ACTIVE) && (http_s->State != HTTP_STATE_POST)) {
    /* Invalid session state */
    RETURN (netWrongState);
  }
  DEBUGF (HTTP,"SetCookie \"%s\"\n",cookie);
  EvrNetHTTPs_SetCookie (cookie, (uint32_t)len);

  netbuf = __BUFFER(net_mem_alloc ((uint32_t)(COOKIE_HEADER_LEN + len + 1)));
  if (netbuf == NULL) {
    /* No memory */
    RETURN (netError);
  }
  len = net_strcpy ((char *)netbuf->data, cookie);
  if (netbuf->data[len-2] == '\r') {
    /* Remove cr-lf, if entered by the user */
    netbuf->data[len-2] = 0;
    len -= 2;
  }
  netbuf->length = len & 0xFFFF;
  netbuf->next   = NULL;
  /* Add cookie to the list tail */
  if (http_s->cookie_list == NULL) {
    /* First cookie added */
    http_s->cookie_list = netbuf;
  }
  else {
    /* Scan the queue chain */
    next = http_s->cookie_list;
    while (next->next != NULL) {
      next = next->next;
    }
    /* Append the cookie to list tail */
    next->next = netbuf;
  }
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Convert generic time to UTC time format.
  \param[in]   hr    hours   [0..23].
  \param[in]   min   minutes [0..59].
  \param[in]   sec   seconds [0..59].
  \param[in]   day   day     [1..31].
  \param[in]   mon   month   [1..12].
  \param[in]   year  year    [1980..2107].
  \return      time converted to UTC format.
*/
uint32_t net_http_time (uint8_t hr, uint8_t min, uint8_t sec,
                        uint8_t day, uint8_t mon, uint16_t year) {
  uint32_t utc;

  if (hr > 23 || min > 59 || sec > 59) {
    /* Time parameters invalid */
    return (0);
  }

  /* Year UTC format: 0 = 1970 .. 2095 */
  year = year - 1970;
  mon  = mon  - 1;
  day  = day  - 1;

  if (day > 30 || mon > 11 || year > 135) {
    /* Date parameters invalid */
    return (0);
  }

  /* Add one day for each leap year passed (1972 was a leap year) */
  utc  = year * 365 + (year + 1) / 4;
  utc += DMonth[mon];
  utc += day;

  /* Add a potential leap day if this is a leap year */
  if (mon > 1 && ((year + 2) % 4) == 0) utc++;

  /* Time */
  utc = utc * 24 + hr;
  utc = utc * 60 + min;
  utc = utc * 60 + sec;

  /* Time zone correction */
  // utc += (int32_t)http_TZone * 3600;
  
  /* Time in UTC format */
  return (utc);
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
static uint32_t http_listener (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len) {
  NET_HTTP_INFO *http_s;
  const char *bp,*hdr;

  /* Find the right session */
  http_s = http_map_session (socket);
  if (http_s == NULL) {
    return (false);
  }
  cgi.session = http_s->Id;

  switch (event) {
    case netTCP_EventConnect:
      /* Access client filtering by IP and/or port */
      if (netHTTPs_AcceptClient (addr) == false) {
        ERRORF (HTTP,"Access denied [%s], port %d\n",net_addr_ntoa((const __ADDR *)addr),addr->port);
        EvrNetHTTPs_UserAccessDenied (addr);
        /* User denied access for this host */
        return (false);
      }
      DEBUGF (HTTP,"Session %d open\n",http_s->Id);
      EvrNetHTTPs_SessionOpen (http_s->Id);
      if (http_s->State == HTTP_STATE_IDLE) {
        http_s->State = HTTP_STATE_RESERVED;
      }
      return (true);

    case netTCP_EventAborted:
      DEBUGF (HTTP,"Session %d abort, Socket aborted\n",http_s->Id);
      EvrNetHTTPs_SocketAborted (http_s->Id);
      http_kill_session (http_s);
      return(true);

    case netTCP_EventEstablished:
      http_s->State = HTTP_STATE_ACTIVE;
      return (true);

    case netTCP_EventClosed:
      DEBUGF (HTTP,"Session %d abort, Socket closed\n",http_s->Id);
      EvrNetHTTPs_SocketClosed (http_s->Id);
      http_kill_session (http_s);
      return (true);

    case netTCP_EventACK:
      /* Data has been acknowledged by remote HTTP client */
      http_s->Count += http_s->BCnt;
      http_s->BCnt = 0;
      return (true);

    case netTCP_EventData:
      DEBUGF (HTTP,"*** Process_frame ***\n");
      DEBUGF (HTTP," Received length: %d bytes\n",len);
      EvrNetHTTPs_ReceiveFrame (http_s->Id, len);
      /* Set global 'cgi.end' */
      cgi.end = (const char *)buf + len;

      if (http_s->State == HTTP_STATE_ACTIVE) {
        if (len < 16) {
          /* Data frame too short */
          ERRORF (HTTP,"Session %d, Frame too short\n",http_s->Id);
          EvrNetHTTPs_FrameTooShort (http_s->Id, len, 16);
          return (false);
        }
        /* Check HTTP request */
        if (!http_parse_method (http_s, (const char *)buf)) {
          /* Method unknown */
          return (false);
        }
        DEBUGF (HTTP," Request method : %s\n",Method[http_s->Method-1]);
        EvrNetHTTPs_RequestMethod (http_s->Id, http_s->Method);

        if (http_s->Method == HTTP_METHOD_HEAD) {
          /* Process HEAD request method */
          hdr = http_proc_uri (http_s, __CONST_CAST(char *)buf, NULL);
          if (hdr == NULL) {
            return (false);
          }
          http_parse_header (http_s, __CONST_CAST(char *)hdr, (const char *)buf);
          if (http_s->State == HTTP_STATE_UNAUTH) {
            /* Authentication failed */
            return (true);
          }
          http_set_ftype (http_s, (const char *)buf);
          http_open_file (http_s, (const char *)buf);
          http_s->DLen  = 1;
          http_s->State = HTTP_STATE_FILE;
          return (true);
        }
        if (http_s->Method == HTTP_METHOD_GET) {
          /* Process GET request method */
          hdr = http_proc_uri (http_s, __CONST_CAST(char *)buf, &bp);
          if (hdr == NULL) {
            return (false);
          }
          http_parse_header (http_s, __CONST_CAST(char *)hdr, (const char *)buf);
          if (http_s->State == HTTP_STATE_UNAUTH) {
            /* Authentication failed */
            return (true);
          }
          /* Check for environment variable Query_string */
          if (bp != NULL) {
            /* Query_String found, process it */
            netCGI_ProcessQuery (bp);
          }
          http_set_ftype (http_s, (const char *)buf);
          http_open_file (http_s, (const char *)buf);
          goto cgif;
        }
        if (http_s->Method == HTTP_METHOD_POST) {
          /* Process POST request method */
          hdr = http_proc_uri (http_s, __CONST_CAST(char *)buf, &bp);
          if (hdr == NULL) {
            return (false);
          }
          http_parse_header (http_s, __CONST_CAST(char *)hdr, (const char *)buf);
          if (http_s->State == HTTP_STATE_UNAUTH) {
            /* Authentication failed */
            return (true);
          }
          /* Check for Query using POST method */
          if (bp != NULL) {
            /* Query_String found, process it */
            netCGI_ProcessQuery (bp);
          }
          http_s->PostSt = 0;
          bp = chk_post_req (http_s, hdr);
          if (bp == NULL) {
            /* POST data not contained in this packet */
            http_s->State = HTTP_STATE_POST;
          }
          else if ((http_s->Flags & HTTP_FLAG_MULTI) == 0) {
            /* Process a FORM or XML POST data */
            if (http_proc_post (http_s, bp) == false) {
              /* POST data not processed, more packets needed */
              http_s->State = HTTP_STATE_POST;
            }
          }
          /* Find file and set HTTP server state variables  */
          /* Actual data is sent from net_http_server_run() */
          http_set_ftype (http_s, (const char *)buf);
          http_open_file (http_s, (const char *)buf);
          if (http_s->State == HTTP_STATE_POST) {
            return (true);
          }

          if (http_s->Flags & HTTP_FLAG_MULTI) {
            http_s->State = HTTP_STATE_UPLOAD;
            if (bp == NULL) {
              return (true);
            }
            /* Multipart POST data in this packet, process it */
            goto multi;
          }

cgif:     if (http_s->Flags & HTTP_FLAG_CGI) {
            /* CGI Script file exists */
            DEBUGF (HTTP,"Session %d, CGI start engine\n",http_s->Id);
            EvrNetHTTPs_CgiStartEngine (http_s->Id);
            http_s->Script = (char *)net_mem_alloc (HTTP_CGI_MAXLINE);
            read_script_line (http_s);
            http_s->State  = HTTP_STATE_CGI;
            return (true);
          }
          http_s->State = HTTP_STATE_FILE;
          return (true);
        }
        http_s->State = HTTP_STATE_NOTIMP;
        return (true);
      }
      if (http_s->State == HTTP_STATE_POST) {
        /* Process fragmented-multipacket POSTs. Some browsers like   */
        /* Netscape or Mozilla Firefox send data in separate frame(s) */
        bp = chk_post_req (http_s, (const char *)buf);
        if (bp == NULL) {
          return (true);
        }
        /* POST data found, process it */
        if (http_s->Flags & HTTP_FLAG_MULTI) {
          http_s->State = HTTP_STATE_UPLOAD;
          goto multi;
        }
        if (http_proc_post (http_s, bp) == false) {
          /* Multipacket POST processing in progress */
          return (false);
        }
        /* POST data processed */
        goto cgif;
      }
      if (http_s->State == HTTP_STATE_UPLOAD) {
        /* File Upload in progress */
        bp = __CONST_CAST(char *)buf;
multi:  if (http_proc_multipart (http_s, bp) == false) {
          /* Multipart processing in progress */
          return (true);
        }
        /* Upload finished */
        goto cgif;
      }
      return (false);
  }
  return (false);
}

/**
  \brief       Kill active HTTP server session.
  \param[in]   http_s  session descriptor.
*/
static void http_kill_session (NET_HTTP_INFO *http_s) {
  http_s->State = HTTP_STATE_IDLE;
  if (http_s->Flags & HTTP_FLAG_DFILE) {
    netHTTPs_fclose (http_s->dFile);
  }
  if (http_s->Flags & HTTP_FLAG_SFILE) {
    netHTTPs_fclose (http_s->sFile);
  }
  if (http_s->Flags & HTTP_FLAG_UFILE) {
    /* Send fclose event for abnormal file upload termination */
    netCGI_ProcessData (3, NULL, 0);
  }
  if (http_s->Flags & HTTP_FLAG_XPOST) {
    /* Abnormal termination of multipacket xml-post */
    netCGI_ProcessData (4, NULL, 0);
  }
  if (http_s->Script != NULL) {
    net_mem_free (__FRAME(http_s->Script));
    http_s->Script = NULL;
  }
  if (http_s->pDelim != NULL) {
    net_mem_free (__FRAME(http_s->pDelim));
    http_s->pDelim = NULL;
  }
  while (http_s->cookie_list != NULL) {
    NET_BUFFER *next = http_s->cookie_list->next;
    net_mem_free (__FRAME(http_s->cookie_list));
    http_s->cookie_list = next;
  }
  http_s->DLen  = 0;
  http_s->Count = 0;
  http_s->BCnt  = 0;
  http_s->Flags = 0;
}

/**
  \brief       Map TCP socket to existing HTTP session.
  \param[in]   socket  TCP socket handle.
  \return      session descriptor or NULL= none.
*/
static NET_HTTP_INFO * http_map_session (int32_t socket) {
  NET_HTTP_INFO *http_s;
  int32_t i;

  for (http_s = &httpc->Scb[0], i = 0; i < httpc->NumSess; http_s++, i++) {
    if (http_s->Socket == socket) {
      return (http_s);
    }
  }
  return (NULL);
}

/**
  \brief       Add HTTP header to HTTP1.1 reply message.
  \param[in]   http_s  session descriptor.
  \param[out]  buf     pointer to output buffer.
  \return      number of characters added.
*/
static int32_t http_add_header (NET_HTTP_INFO *http_s, char *buf) {
  const char *tp;
  int32_t len;

  switch (http_s->FType) {
    case HTTP_TYPE_HTML:
    case HTTP_TYPE_CGI:
    case HTTP_TYPE_PHP:
      tp = "text/html";
      break;

    case HTTP_TYPE_CSS:
      tp = "text/css";
      break;

    case HTTP_TYPE_JS:
    case HTTP_TYPE_JZ:
      tp = "application/x-javascript";
      break;

    case HTTP_TYPE_JAR:
      tp = "application/java-archive";
      break;

    case HTTP_TYPE_CGX:
      /* User definable content type */
      tp = netCGX_ContentType ();
      if (tp != NULL) {
        /* User has provided 'Content-type' */
        break;
      }
      __FALLTHROUGH;

    case HTTP_TYPE_XML:
    case HTTP_TYPE_XSLT:
      tp = "text/xml";
      break;

    case HTTP_TYPE_GIF:
      tp = "image/gif";
      break;

    case HTTP_TYPE_JPEG:
      tp = "image/jpeg";
      break;

    case HTTP_TYPE_BMP:
      tp = "image/bmp";
      break;

    case HTTP_TYPE_PNG:
      tp = "image/png";
      break;

    case HTTP_TYPE_ICO:
      tp = "image/x-icon";
      break;

    case HTTP_TYPE_XAP:
      tp = "application/x-silverlight-app";
      break;

    case HTTP_TYPE_XAML:
      tp = "application/xaml+xml";
      break;

    case HTTP_TYPE_TXT:
      tp = "text/plain";
      break;

    case HTTP_TYPE_SVG:
    case HTTP_TYPE_SVGZ:
      tp = "image/svg+xml";
      break;

    case HTTP_TYPE_ZIP:
      tp = "application/x-zip-compressed";
      break;

    case HTTP_TYPE_DLL:
      tp = "application/x-msdownload";
      break;

    case HTTP_TYPE_PDF:
      tp = "application/pdf";
      break;

    case HTTP_TYPE_USER:
      /* User extended MIME type */
      tp = http_s->pUser;
      break;

    default:
      /* Default HTTP_TYPE_BIN */
      tp = "application/octet-stream";
      break;
  }
  /* Add Status Code OK, HTTP version is 1.1 */ 
  len  = net_strcpy (buf, "HTTP/1.1 200 OK\r\n");
  len += add_server_id (buf+len);

  /* Add 'Content-Type' header */
  len += net_strcpy (buf+len, "Content-Type: ");
  len += net_strcpy (buf+len, tp);
  if (buf[len-2] == '\r') {
    /* Remove cr-lf, if entered by the user */
    len -= 2;
  }
  /* Check if text type and add optional encoding parameter */
  if (net_strcmp (tp, "text")) {
    tp = netCGI_Charset ();
    if (tp != NULL) {
      /* User has provided a 'charset' */
      len += net_strcpy (buf+len, "; charset=");
      len += net_strcpy (buf+len, tp);
    }
  }
  len += net_strcpy (buf+len, "\r\n");

  if ((http_s->FType == HTTP_TYPE_CGI) || (http_s->FType == HTTP_TYPE_CGX)) {
    /* Add 'no-cache' response header for dynamic CGI files */
    len += net_strcpy (buf+len, "Cache-Control: no-cache\r\n");
  }
  else {
    /* Add headers for static data files that don't change */
    if ((http_s->FType == HTTP_TYPE_SVGZ) || (http_s->FType == HTTP_TYPE_JZ)) {
      /* Add 'gzip' encoding type for compressed files */
      len += net_strcpy (buf+len, "Content-Encoding: gzip\r\n");
    }
    if (http_s->LMDate) {
      /* Add 'Last-Modified' header */
      len += net_strcpy  (buf+len, "Last-Modified: ");
      len += utc_to_http (http_s->LMDate, buf+len);
    }
    if (http_s->DLen && http_s->DLen < 0x80000000) {
      /* Add 'Content-Length' header */
      len += net_strcpy (buf+len, "Content-Length: ");
      len += net_ultoa  (buf+len, http_s->DLen);
      len += net_strcpy (buf+len, "\r\n");
    }
  }
  /* Add cookie(s) from the list */
  while (http_s->cookie_list != NULL) {
    NET_BUFFER *next = http_s->cookie_list->next;
    len += net_strcpy (buf+len, "Set-Cookie: ");
    len += net_strcpy (buf+len, (char *)http_s->cookie_list->data);
    len += net_strcpy (buf+len, "\r\n");
    net_mem_free (__FRAME(http_s->cookie_list));
    http_s->cookie_list = next;
  }
  /* Check user provided headers */
  tp = netCGI_CustomHeader ();
  if (tp != NULL) {
    /* Add custom HTTP response header(s) */
    len += net_strcpy (buf+len, tp);
  }

  len += net_strcpy (buf+len, "Connection: close\r\n"
                              "\r\n");
  return (len);
}

/**
  \brief       Add HTTP error reply header + message body.
  \param[in]   http_s  session descriptor.
  \param[out]  buf     pointer to output buffer.
  \param[in]   err     HTTP error code.
  \return      number of characters added.
*/
static int32_t http_error_msg (NET_HTTP_INFO *http_s, char *buf, uint32_t err) {
  const char *tp;
  int32_t len;

  /* Add HTTP Header to HTTP1.1 error message */
  switch (err) {
    case HTTP_ERR_301:
      tp = "301 Moved Permanently";
      break;
    case HTTP_ERR_304:
      tp = "304 Not Modified";
      break;
    case HTTP_ERR_401:
      tp = "401 Unauthorized";
      break;
    case HTTP_ERR_403:
      tp = "403 Forbidden";
      break;
    case HTTP_ERR_404:
      tp = "404 Not Found";
      break;
    default:
      tp = "501 Not Implemented";
      break;
  }
  len  = net_strcpy (buf, "HTTP/1.1 ");
  len += net_strcpy (buf+len, tp);
  len += net_strcpy (buf+len, "\r\n");
  len += add_server_id (buf+len);
  if (err == HTTP_ERR_301) {
    len += net_strcpy (buf+len, "Location: ");
    len += net_strcpy (buf+len, http_s->pUser);
    len += net_strcpy (buf+len, "\r\n");
  }
  if (err >= HTTP_ERR_4xx) {
    len += net_strcpy (buf+len, "Content-type: text/html\r\n");
  }
  if (err != HTTP_ERR_304) {
    tp = netCGI_CustomHeader ();
    if (tp != NULL) {
      len += net_strcpy (buf+len, tp);
    }
  }
  if (err == HTTP_ERR_401) {
    if (auth_io) len += auth_io->add_line (http_s, buf+len);
  }
  len += net_strcpy (buf+len, "Connection: close\r\n"
                              "\r\n");
  if (err < HTTP_ERR_4xx) {
    return (len);
  }

  /* Add a WEB Page to HTTP1.1 error message */
  len += net_strcpy (buf+len, net_http_error.header);
  switch (err) {
    case HTTP_ERR_401:
      tp = net_http_error.e401;
      break;
    case HTTP_ERR_403:
      tp = net_http_error.e403;
      break;
    case HTTP_ERR_404:
      tp = net_http_error.e404;
      break;
    default:
      tp = net_http_error.e501;
      break;
  }
  len += net_strcpy (buf+len, tp);
  len += net_strcpy (buf+len, net_http_error.footer);
  return (len);
}

/**
  \brief       Add HTTP server-id header.
  \param[out]  buf  pointer to output buffer.
  \return      number of characters added.
  \note        Add default or user-specified server-id.
*/
static int32_t add_server_id (char *buf) {
  const char *srv;
  int32_t n;

  srv = httpc->SrvId;
  if (srv[0] == 0) {
    srv = "Keil-EWEB/2.1";
  }
  n  = net_strcpy (buf+0, "Server: ");
  n += net_strcpy (buf+n, srv);
  n += net_strcpy (buf+n, "\r\n");
  return (n);
}

/**
  \brief       Open requested file for reading.
  \param[in]   http_s  session descriptor.
  \param[out]  name    pointer to file name.
*/
static void http_open_file (NET_HTTP_INFO *http_s, const char *name) {
  void *file;
  bool def_file;
  const char *path;

  def_file = false;
  if (*name == 0) {
    /* No name specified, use default */
    name = "index.htm";
    def_file = true;
  }

open_again:
  DEBUGF (HTTP," Requested file : %s\n",name);
  EvrNetHTTPs_RequestedFile (name, strlen(name));
  if ((http_s->UserId != 0) && ((http_s->Flags & HTTP_FLAG_CGI) == 0)) {
    /* Check access rights for external user accounts */
    if (netHTTPs_FileAccess (http_s->UserId, name) == false) {
      /* Access not allowed for this resource */
      http_s->Flags |= HTTP_FLAG_NOACCESS;
      return;
    }
  }
  /* Try external drive first */
  path = net_path (http->root, name);
  file = netHTTPs_fopen (path);
  if (file != NULL) {
    /* File opened on external drive */
    DEBUGF (HTTP," Open location  : External drive\n");
    EvrNetHTTPs_FileOpenDrive (1);
    if (((http_s->Flags & HTTP_FLAG_CGI) == 0) &&
       (http_s->FType == HTTP_TYPE_CGI || http_s->FType == HTTP_TYPE_CGX)) {
      /* CGI script File requested */
      http_s->Flags |= (HTTP_FLAG_CGI | HTTP_FLAG_SFILE);
      http_s->sFile  = file;
    }
    else {
      /* Open a data file or a file included from cgi/cgx script */
      http_s->Flags |= HTTP_FLAG_DFILE;
      http_s->dFile  = file;
      /* Use max. size 2 GB for external drive */
      http_s->DLen   = 0x80000000;
      if ((http_s->Flags & HTTP_FLAG_CGI) == 0) {
        /* Read size and last modification time of a file */
        uint32_t fsize,ftime;
        netHTTPs_fstat (path, &fsize, &ftime);
        http_s->DLen = fsize;
        DEBUGF (HTTP," Open file size : %u bytes\n",fsize);
        DEBUGF (HTTP," Last modified  : %u\n",ftime);
        EvrNetHTTPs_ViewFileStatus (fsize, ftime);
        if ((ftime != 0) && (http_s->LMDate == ftime)) {
          /* The browser has this file cached */
          http_s->Flags |= HTTP_FLAG_CACHED;
        }
        http_s->LMDate = ftime;
      }
    }
  }
  else {
    /* Try ROM-image file system */
    const uint8_t *start;
    uint32_t size = imageFileInfo (name, &start);
    if (size != 0) {
      /* File opened in ROM-image file system */
      DEBUGF (HTTP," Open location  : ROM image\n");
      EvrNetHTTPs_FileOpenDrive (0);
      if (((http_s->Flags & HTTP_FLAG_CGI) == 0) &&
          (http_s->FType == HTTP_TYPE_CGI || http_s->FType == HTTP_TYPE_CGX)) {
        http_s->Flags |= HTTP_FLAG_CGI;
        http_s->sFile  = __CONST_CAST(uint8_t *)start;
      }
      else {
        /* Open a data file or included file from cgi/cgx script */
        http_s->dFile = __CONST_CAST(uint8_t *)start;
        http_s->DLen  = size;
        DEBUGF (HTTP," Open file size : %u bytes\n",http_s->DLen);
        EvrNetHTTPs_ViewFileStatus (http_s->DLen, imageLastModified);
        if ((http_s->Flags & HTTP_FLAG_CGI) == 0) {
          if (http_s->LMDate == imageLastModified) {
            /* The browser has this file cached */
            http_s->Flags |= HTTP_FLAG_CACHED;
          }
          /* LMDate used in http Last-Modified header response */
          http_s->LMDate = imageLastModified;
        }
      }
    }
    else {
      /* File not found error */
      if (def_file == true) {
        /* Failed on "index.htm", retry with "index.cgi" */
        name = "index.cgi";
        /* Correct also a file type for CGI script */
        http_s->FType = HTTP_TYPE_CGI;
        def_file = false;
        goto open_again;
      }
      /* Check for resource file redirection */
      http_s->pUser = __CONST_CAST(char *)(netCGI_Redirect (name));
#ifdef __DEBUG_ENABLED
      if (http_s->pUser != NULL) {
        DEBUGF (HTTP," Redirection URL: %s\n",http_s->pUser);
        EvrNetHTTPs_ViewRedirectionUrl (http_s->pUser, strlen(http_s->pUser));
      }
#endif
      http_s->Flags |= HTTP_FLAG_NOFILE;
    }
  }
}

/**
  \brief       Determine file type for the requested resource.
  \param[in]   http_s  session descriptor.
  \param[in]   name    pointer to file name, null-terminated.
*/
static void http_set_ftype (NET_HTTP_INFO *http_s, const char *name) {
  static const NET_HTTP_FTYPE ftype[24] = {
    { "htm", HTTP_TYPE_HTML },      /* Finds 'htm' and 'html'  */
    { "cgx", HTTP_TYPE_CGX  },
    { "cgi", HTTP_TYPE_CGI  },
    { "css", HTTP_TYPE_CSS  },
    { "js",  HTTP_TYPE_JS   },
    { "jz",  HTTP_TYPE_JZ   },      /* The same as 'js.gz'     */
    { "jar", HTTP_TYPE_JAR  },
    { "xml", HTTP_TYPE_XML  },
    { "xsl", HTTP_TYPE_XSLT },      /* Finds 'xsl' and 'xslt'  */
    { "gif", HTTP_TYPE_GIF  },
    { "jpg", HTTP_TYPE_JPEG },
    { "jpeg",HTTP_TYPE_JPEG },
    { "bmp", HTTP_TYPE_BMP  },
    { "png", HTTP_TYPE_PNG  },
    { "ico", HTTP_TYPE_ICO  },
    { "xap", HTTP_TYPE_XAP  },      /* Silverlight content     */
    { "xaml",HTTP_TYPE_XAML },
    { "php", HTTP_TYPE_PHP  },      /* PHP script content      */
    { "txt", HTTP_TYPE_TXT  },
    { "svgz",HTTP_TYPE_SVGZ },      /* This must precede 'svg' */
    { "svg", HTTP_TYPE_SVG  },      /* for net_strcasecmp()    */
    { "zip", HTTP_TYPE_ZIP  },
    { "dll", HTTP_TYPE_DLL  },
    { "pdf", HTTP_TYPE_PDF  } };
  const char *ext, *type;
  int32_t i;

  if (name[0] == 0) {
    /* Default file 'index.htm' */
    http_s->FType = HTTP_TYPE_HTML;
    return;
  }
  i = net_strrchr (name, '.');
  if (i >= 0) {
    ext = &name[i+1];
    /* Dot found, check file extension */
    for (i = 0; i < (int32_t)(sizeof(ftype)/sizeof(NET_HTTP_FTYPE)); i++) {
      if (net_strcasecmp (ext, ftype[i].ext)) {
        http_s->FType = ftype[i].id;
        return;
      }
    }
    if (net_strcasecmp (ext-4, ".js.gz")) {
      /* Java script gzip compressed */
      http_s->FType = HTTP_TYPE_JZ;
      return;
    }
    /* Unknown type, check user MIME types */
    type = netCGI_ContentType (ext);
    if (type != NULL) {
      /* Pointer to user Content-type */
      http_s->pUser = __CONST_CAST(char *)type;
      http_s->FType = HTTP_TYPE_USER;
      return;
    }
  }
  /* Unknown file type, send as binary */
  http_s->FType = HTTP_TYPE_BIN;
}

/**
  \brief       Load frame data buffer with file data.
  \param[in]   http_s   session descriptor.
  \param[out]  buf      output data buffer.
  \param[in]   max_len  size of data buffer.
  \return      number of bytes copied.
*/
static uint16_t http_load_buf (NET_HTTP_INFO *http_s, uint8_t *buf, uint32_t max_len) {
  uint32_t pos,len,rlen;

  if (http_s->Flags & HTTP_FLAG_DFILE) {
    /* Read data from external file system */
    rlen = netHTTPs_fread (http_s->dFile, buf, max_len);
    if (rlen < max_len) {
      /* End of file reached, close a file and set end condition */
      netHTTPs_fclose (http_s->dFile);
      http_s->Flags &= ~HTTP_FLAG_DFILE;
      http_s->dFile = NULL;
      http_s->DLen  = 1;
    }
    return (rlen & 0xFFFF);
  }
  /* Read data from internal ROM file system */
  pos = http_s->Count;
  len = http_s->DLen - pos;
  if (len > max_len) {
    len = max_len;
  }
  memcpy (buf, (uint8_t *)http_s->dFile + pos, len);
  return (len & 0xFFFF);
}

/**
  \brief       Parse HTTP request method.
  \param[in]   http_s  session descriptor.
  \param[in]   buf     pointer to request line.
  \return      status:
               - true  = method found,
               - false = method unknown.
*/
static bool http_parse_method (NET_HTTP_INFO *http_s, const char *buf) {
  int32_t i;

  for (i = 0; i < 8; i++) {
    if (net_strcmp (buf, Method[i])) {
      http_s->Method = (i + 1) & 0xFF;
      return (true);
    }
  }
  return (false);
}

/**
  \brief       Process HTTP request line.
  \param[in]   http_s  session descriptor.
  \param[in]   buf     pointer to request line.
  \param[in,out] env   pointer to pointer to environment variable.
  \return      pointer to HTTP headers, or NULL if misformed request line.
  \note        'cgi.end' must be set before calling this function!
*/
static const char *http_proc_uri (NET_HTTP_INFO *http_s, char *buf, const char **env) {
  const char *fp;
  char *bp = buf;
  int32_t i;

  /* Check method token separator */
  i = TokenLen[http_s->Method-1];
  if ((bp[i] != ' ') || (bp[i+1] != '/')) {
    return (NULL);
  }
  /* Write decoded URI to the beginning of buf */
  for (fp = &bp[i+2]; fp < cgi.end-1; bp++) {
    switch (fp[0]) {
      case ' ':
        /* End of URI */
        if (env) *env = NULL;
        goto end_uri;
      case '?':
        /* Query-string found */
        if (env) *env = &fp[1];
        goto end_uri;
      case '%':
        /* Decode percent-encoded character */
        bp[0] = net_xtouc (&fp[1]);
        fp += 3;
        break;
      default:
        bp[0] = fp[0];
        fp++;
        break;
    }
  }
  /* No more data */
  return (NULL);
end_uri:
  /* Locate end of request-line */
  i = net_strnstr (fp, "HTTP/1.1\r\n", (uint32_t)(cgi.end-fp));
  if (i < 0) {
    return (NULL);
  }
  fp += i + 10;
  /* Add null-termination */
  bp[0] = 0;

  if (http->hdr_access) {
    /* HTTP request callback notification */
    i = net_strnstr (fp, "\r\n\r\n", (uint32_t)(cgi.end-fp));
    if (i < 0) {
      return (NULL);
    }
    netCGI_ProcessRequest (Method[http_s->Method-1], buf, fp, (uint32_t)i+4);
  }
  return (fp);
}

/**
  \brief       Parse http header options and update http status.
  \param[in]   http_s  session descriptor.
  \param[in]   hdr     pointer to http headers.
  \param[in]   uri     pointer to preprocessed http uri.
  \note        'cgi.end' must be set before calling this function!
*/
static void http_parse_header (NET_HTTP_INFO *http_s, char *hdr, const char *uri) {
  const char *sp;
  char *fp;
  uint32_t opt;
  int32_t i;

  /* Compose bit-mask of header options */
  opt = 0x00;
  if (http_s->Method != HTTP_METHOD_HEAD) {
    opt = 0x06;
    if (http->en_cookie) opt |= 0x08;
  }
  if (http->en_auth) opt |= 0x01;

  http_s->LMDate = 0;
  http_s->UserId = 0;
  http_s->Lang[0]= 0;

  /* Check bit-mask enabled header options */
  sp = NULL;
  for (fp = hdr; fp && opt; fp = __CONST_CAST(char *)(get_next_field (fp, cgi.end))) {
    if ((opt & 0x01U) && net_strcasecmp (fp, "Authorization: ")) {
      opt &= ~0x01U; fp += 15;
      if (!(auth_io && auth_io->parse_line (http_s, fp))) {
        /* Wrong username and/or password */
        http_s->State = HTTP_STATE_UNAUTH;
        return;
      }
      continue;
    }
    if ((opt & 0x02U) && net_strcasecmp (fp, "Accept-Language: ")) {
      opt &= ~0x02U; fp += 17;
      /* Set only the primary language */
      for (i = 0; i < 5; i++) {
        http_s->Lang[i] = fp[i];
        if (fp[i] == ',' || fp[i] == '\r') {
          break;
        }
      }
      http_s->Lang[i] = 0;
      DEBUGF (HTTP," Accept-language: %s\n",http_s->Lang);
      EvrNetHTTPs_ViewAcceptLanguage (http_s->Lang, (uint32_t)i);
      continue;
    }
    if ((opt & 0x04U) && net_strcasecmp (fp, "If-Modified-Since: ")) {
      opt &= ~0x04U; fp += 19;
      /* Remember Last-Modified date */
      http_s->LMDate = http_to_utc (fp);
      continue;
    }
    if ((opt & 0x08U) && net_strcasecmp (fp, "Cookie: ")) {
      opt &= ~0x08U; fp += 8;
      /* Remember cookie location */
      sp = fp;
      continue;
    }
  }
  if (opt & 0x01) {
    /* Client did not try to authenticate */
    http_s->State = HTTP_STATE_UNAUTH;
    return;
  }
  if (sp && http->en_cookie) {
    /* Cookie callback notification */
    i = net_strnchr (sp, '\r', (uint32_t)(cgi.end-sp));
    if (i > 0) {
      /* Valid cookie found */
      DEBUG_INF2 (sp, (uint32_t)i);
      EvrNetHTTPs_ViewCookie (sp, (uint32_t)i);
      netCGI_ProcessCookie (Method[http_s->Method-1], uri, sp, (uint32_t)i);
    }
  }
}

/**
  \brief       Convert http date string to UTC time.
  \param[in]   buf  buffer with http date string.
  \return      UTC time in binary format.
  \description Date string format "Thu, 10 Sep 2009 07:49:57 GMT"
*/
static uint32_t http_to_utc (const char *buf) {
  uint32_t utc;
  int32_t i,y;

  /* Year (UTC format: 1970 = 0) */
  y = net_atoi(&buf[12], NULL) - 1970;
  if (y > 135) {
    /* Valid range 1970 - 2095 */
    return (0);
  }
  /* Add one day for each leap year passed (1972 was a leap year) */
  utc = (uint32_t)(y * 365 + (y + 1) / 4);

  /* Month */
  for (i = 0; i < 12; i++) {
    if (net_strcmp(&buf[8],Month[i])) {
      break;
    }
  }
  if (i == 12) {
    return (0);
  }
  utc += DMonth[i];
  /* If this year is a leap year */
  if (i > 1 && ((y + 2) % 4) == 0) utc++;

  /* Date */
  i = net_atoi(&buf[5], NULL) - 1;
  if (i > 30) {
    return (0);
  }
  utc += (uint32_t)i;

  /* Hours */
  i = net_atoi(&buf[17], NULL);
  utc = utc * 24 + (uint32_t)i;

  /* Minutes */
  i = net_atoi(&buf[20], NULL);
  utc = utc * 60 + (uint32_t)i;

  /* Seconds */
  i = net_atoi(&buf[23], NULL);
  utc = utc * 60 + (uint32_t)i;

  /* Time in UTC format */
  return (utc);
}

/**
  \brief       Convert UTC time to http date string.
  \param[in]   utc  time in UTC format.
  \param[out]  buf  pointer to output buffer.
  \return      number of characters written.
  \description Date format "Thu, 10 Sep 2009 07:49:57 GMT"
*/
static uint32_t utc_to_http (uint32_t utc, char *buf) {
  static const uint16_t DYear[3] = {365,365,366};
  uint32_t hr,min,sec;
  uint32_t day,mon,year;
  uint32_t i,d;
  int32_t  n;

  /* Time */
  sec = utc % 60;
  utc = utc / 60;
  min = utc % 60;
  utc = utc / 60;
  hr  = utc % 24;
  utc = utc / 24;
  
  /* Day of week */
  d = (utc + 3) % 7;

  /* Date */
  year = utc / (4*365+1) * 4;
  utc  = utc % (4*365+1);

  /* Get current year */
  for (i = 0; utc >= DYear[i]; ) {
    utc -= DYear[i++];
  }
  year += i; 

  for (mon = 12; ; mon--) {
    day = DMonth[mon-1];
    if (mon > 2 && (i == 2)) day++;
    if (utc >= day) break;
  }
  day = utc - day + 1;

  /* Print date in the following format:     */
  /* "%s, %02d %s %d %02d:%02d:%02d GMT\r\n" */
  n  = net_strcpy  (buf+0, WkDay[d]);
  buf[n++] = ',';
  buf[n++] = ' ';
  n += net_itoa_n0 (buf+n, (int32_t)day, 2);
  buf[n++] = ' ';
  n += net_strcpy  (buf+n, Month[mon-1]);
  buf[n++] = ' ';
  n += net_itoa    (buf+n, (int32_t)year+1970);
  buf[n++] = ' ';
  n += net_itoa_n0 (buf+n, (int32_t)hr, 2);
  buf[n++] = ':';
  n += net_itoa_n0 (buf+n, (int32_t)min, 2);
  buf[n++] = ':';
  n += net_itoa_n0 (buf+n, (int32_t)sec, 2);
  n += net_strcpy  (buf+n, " GMT\r\n");
  return ((uint32_t)n);
}

/**
  \brief       Check POST request header.
  \param[in]   http_s  session descriptor.
  \param[in]   buf     buffer with http header.
  \return      pointer to start of POST data or
               NULL if not found.
  \note        'cgi.end' must be set before calling this function!
*/
static const char *chk_post_req (NET_HTTP_INFO *http_s, const char *buf) {
  const char *fp,*np;
  int32_t sz;

  if (http_s->PostSt >= 0x07) {
    /* POST header already processed, buffer holds POST data */
    return (buf);
  }

  for (fp = buf; fp; fp = np) {
    /* Check "Content-Type" header option */
    if (net_strcasecmp (fp, "Content-Type: ")) {
      fp += 14;
      if (net_strcmp (fp, "application/x-www-form-urlencoded")) {
        http_s->PostSt |= 0x01;
      }
      else if (net_strcmp (fp,"multipart/form-data")) {
        /* Find MIME parameter "boundary" */
        np = fp;
        while ((np = get_next_par (np, cgi.end)) != NULL) {
          if (net_strcmp (np, "boundary=")) {
            /* Boundary delimiter string size from 1 to 70 characters */
            /* and NOT ending with whitespace.    [RFC 2046, page 22] */
            http_s->pDelim = (char *)net_mem_alloc (72);
            sz = http_strcpy (http_s->pDelim, np+9, 72) - 2;
            /* Remove appended CR-LF, null-terminate the string */
            http_s->pDelim[sz] = 0;
            http_s->DelimSz    = sz & 0xFFFF;
            /* Properly formatted MIME header */
            http_s->PostSt |= 0x01;
            http_s->Flags  |= HTTP_FLAG_MULTI;
            break;
          }
        }
      }
      else {
        /* Other MIME type, save "Content-Type" header */
        http_s->pDelim = (char *)net_mem_alloc (64);
        /* ie: application/xml-external-parsed-entity; charset="utf-16be" */
        sz = http_strcpy (http_s->pDelim, fp, 64) - 2;
        /* Remove appended CR-LF, null-terminate the string */
        http_s->pDelim[sz] = 0;
        http_s->DelimSz    = sz & 0xFFFF;
        DEBUGF (HTTP," XML-type: %s\n",http_s->pDelim);
        EvrNetHTTPs_ViewXmlType (http_s->pDelim, http_s->DelimSz);
        /* Properly formatted MIME header */
        http_s->PostSt |= 0x01;
        http_s->Flags  |= HTTP_FLAG_XPOST;
      }
    }
    /* Check "Content-Length" header option */
    else if (net_strcasecmp (fp, "Content-Length: ")) {
      fp += 16;
      http_s->CGIvar  = (uint32_t)net_atoi(fp, NULL);
      http_s->PostSt |= 0x02;
    }
    np = get_next_field (fp, cgi.end);
    if (np == NULL && fp[0] == '\r' && fp[1] == '\n') {
      /* This is end of header */
      http_s->PostSt |= 0x04;
      if (http_s->PostSt == 0x07) {
        fp += 2;
        if (fp < cgi.end) {
          return (fp);
        }
      }
    }
  }
  return (NULL);
}

/**
  \brief       Process POST data.
  \param[in]   http_s  session descriptor.
  \param[in]   buf     buffer with POST data.
  \return      status:
               - true  = POST data complete,
               - false = more frames needed.
  \note        'cgi.end' must be set before calling this function!
  \description
  Processing depends on the content type:
    - x-www-form-urlencoded: this data is buffered. When the data is
                             complete a callback function is called.
    - xml type             : this post is larger. The data is not
                             buffered. Each packet is delivered to
                             the user when it arrives.
*/
static bool http_proc_post (NET_HTTP_INFO *http_s, const char *buf) {
  uint32_t len;

  cgi.len = (cgi.end - buf) & 0xFFFF;
  if (http_s->Flags & HTTP_FLAG_XPOST) {
    /* Handle XML POSTs with no buffering */
    if ((http_s->PostSt & 0x08) == 0) {
      DEBUGF (HTTP," POST type      : xml-encoded\n");
      DEBUGF (HTTP," Content-length : %d bytes\n",http_s->CGIvar);
      EvrNetHTTPs_PostXmlEncoded (http_s->CGIvar);
      if (http_s->CGIvar <= cgi.len) {
        /* XML data contained in a single packet */
        goto xml;
      }
      /* XML POST first packet, more to follow */
      http_s->PostSt |= 0x08;
      http_s->Count   = 0;
    }
    http_s->Count += cgi.len;
    if (http_s->Count < http_s->CGIvar) {
      /* More packets to follow */
      netCGI_ProcessData (5, buf, cgi.len);
      return (false);
    }
    DEBUGF (HTTP," Multipacket POST complete\n");
    EvrNetHTTPs_PostCompleteMultipacket (http_s->CGIvar);
xml:netCGI_ProcessData (4, buf, cgi.len);
    http_s->Flags &= ~HTTP_FLAG_XPOST;
    http_s->Count = 0;
    return (true);
  }

  /* Handle www-form-urlencoded POSTs */
  if ((http_s->PostSt & 0x08) == 0) {
    /* First call to this function */
    DEBUGF (HTTP," POST type      : www-form-urlencoded\n");
    DEBUGF (HTTP," Content-length : %d bytes\n",http_s->CGIvar);
    EvrNetHTTPs_PostFormUrlEncoded (http_s->CGIvar);
    if (http_s->CGIvar <= cgi.len) {
      /* POST data contained in single packet */
      netCGI_ProcessData (0, buf, cgi.len);
      return (true);
    }
    /* This is a multipacket POST, needs buffering */
    http_s->DelimSz = http_s->CGIvar & 0xFFFF;
    if (http_s->DelimSz > sysc->MemSize/4) {
      http_s->DelimSz = (sysc->MemSize/4) & 0xFFFF;
      ERRORF (HTTP,"POST data too large, truncated\n");
      EvrNetHTTPs_PostDataTruncated (http_s->DelimSz);
    }
    http_s->pDelim  = (char *)net_mem_alloc (http_s->DelimSz);
    memcpy (http_s->pDelim, buf, cgi.len);
    http_s->Count   = cgi.len;
    http_s->PostSt |= 0x08;
    return (false);
  }
  /* Copy the data to POST buffer */
  if (http_s->Count < http_s->DelimSz) {
    /* Fill the buffer, ignore extra bytes */
    len = http_s->DelimSz - http_s->Count;
    if (len > cgi.len) {
      len = cgi.len;
    }
    memcpy (&http_s->pDelim[http_s->Count], buf, len);
  }
  http_s->Count += cgi.len;
  if (http_s->Count < http_s->CGIvar) {
    /* More packets needed */
    return (false);
  }
  /* POST data buffering completed, extra bytes ignored */
  DEBUGF (HTTP," Multipacket POST complete\n");
  EvrNetHTTPs_PostCompleteMultipacket (http_s->CGIvar);
  /* Correct 'cgi.end' for netCGI_GetEnvVar() callback */
  cgi.end = http_s->pDelim + http_s->DelimSz;
  netCGI_ProcessData (0, http_s->pDelim, http_s->DelimSz);
  cgi.end = NULL;
  net_mem_free (__FRAME(http_s->pDelim));
  http_s->pDelim = NULL;
  http_s->Count  = 0;
  return (true);
}

/**
  \brief       Process multipart POST request "multipart/form-data" type.
  \param[in]   http_s  session descriptor.
  \param[in]   buf     buffer containing the data.
  \return      status:
               - true  = when processed (file uploaded),
               - false = mode data to come.
*/
static bool http_proc_multipart (NET_HTTP_INFO *http_s, const char *buf) {
  const char *fp;
  int32_t i;

  cgi.len = (cgi.end - buf) & 0xFFFF;
  if ((http_s->PostSt & 0x10) == 0) {
    /* Find first boundary delimiter */
    if (cgi.len <= http_s->DelimSz) {
      /* Packet too short for delimiter */
      return (false);
    }
    buf = search_delim (http_s->pDelim, buf, cgi.len - http_s->DelimSz);
    if (buf == NULL) {
      return (false);
    }

    /* Process multipart header */
    for (buf += (http_s->DelimSz + 4); buf; buf = fp) {
      /* Search for "filename" header option */
      if (net_strcasecmp (buf, "Content-Disposition: ")) {
        buf += 21;
        /* Search for parameter "filename=" */
        for (fp = buf; fp; fp = get_next_par (fp, cgi.end)) {
          if (net_strcmp (fp, "filename=\"")) {
            fp += 10;
            i = net_strnchr (fp, '\"', 128);
            if (i > 0) {
              /* Here we violate (const char *) convention */
              (__CONST_CAST(char *)fp)[i] = 0;
              /* Send null-terminated filename to the user */
              DEBUGF (HTTP," Multipart POST : %d bytes\n",http_s->CGIvar);
              DEBUGF (HTTP," Upload filename: %s\n",fp);
              EvrNetHTTPs_FileUploadRequested (fp, (uint32_t)i);
              netCGI_ProcessData (1, fp, (uint32_t)i);
              http_s->Flags  |= HTTP_FLAG_UFILE;
              http_s->PostSt |= 0x20;
            }
            break;
          }
        }
      }
      fp = get_next_field (buf, cgi.end);
      if (fp == NULL && buf[0] == '\r' && buf[1] == '\n') {
        /* This is end of header */
        http_s->PostSt |= 0x10;
        buf += 2;
        if (buf < cgi.end) {
          /* Also file data contained in this packet */
          goto fdata;
        }
      }
    }
    return (false);
  }
fdata:
  if ((http_s->PostSt & 0x30) == 0x30) {
    /* Wait for multipart POST header end (file-data start) */
    http_s->CGIvar -= cgi.len;
    if (http_s->PostSt & 0x40) {
      goto last;
    }
    fp = NULL;
    /* 'cgi.len' is invalid for small (single packet) files! */
    if ((cgi.end - buf) > http_s->DelimSz) {
      /* Packet big enough for delimiter */
      fp = search_delim (http_s->pDelim, buf, (cgi.end - buf) - http_s->DelimSz);
    }
    if (fp == NULL) {
      /* Closing boundary not found in this packet */
      fp = cgi.end;
      /* In multipart POST with multiple variables on the page, additional */
      /* packets might follow after the closing boundary was received.     */
      if (http_s->CGIvar && (http_s->CGIvar < (http_s->DelimSz + 8))) {
        /* Closing boundary split to two packets */
        fp -= (http_s->DelimSz + 8 - http_s->CGIvar);
        http_s->PostSt |= 0x40;
      }
    }
    else {
      /* Closing boundary, remove leading CR-LF sequence */
      fp -= 2;
      http_s->PostSt |= 0x40;
    }
    DEBUGF (HTTP," File data      : %d bytes\n",fp - buf);
    EvrNetHTTPs_FileUploadDataReceived ((uint32_t)(fp - buf));
    netCGI_ProcessData (2, buf, (uint32_t)(fp - buf));
last:
    DEBUGF (HTTP," Multipart left : %d bytes\n",http_s->CGIvar);
    EvrNetHTTPs_FileUploadRemaining (http_s->CGIvar);
    if (http_s->CGIvar < 4) {
      /* Multipart file Upload processed */
      DEBUGF (HTTP," Upload file close\n");
      EvrNetHTTPs_FileUploadComplete ();
      netCGI_ProcessData (3, NULL, 0);
      http_s->Flags &= ~HTTP_FLAG_UFILE;
      return (true);
    }
  }
  return (false);
}

/**
  \brief       Search for delimiter in "multipart/form-data".
  \param[in]   delim  delimiter null-terminated string (boundary string).
  \param[in]   buf    buffer containing the data.
  \param[in]   len    length of the data.
  \return      pointer to next field or NULL.
  \note        Header fields are separated with "\r\n" sequences.
*/
static const char *search_delim (const char *delim, const char *buf, int32_t len) {
  if (delim == NULL) {
    /* Failure, should never happen */
    return (NULL);
  }
  for (  ; len > 1; buf += 2, len -= 2) {
    /* Search for '--' delimiter leading char sequence */
    if (buf[1] != '-') {
      continue;
    }
    if (buf[0] == '-') {
      if (net_strcmp (buf+2, delim)) {
        return (buf);
      }
    }
    else if (buf[2] == '-') {
      if (net_strcmp (buf+3, delim)) {
        return (buf+1);
      }
    }
  }
  return (NULL);
}

/**
  \brief       Parse HTTP header and search for next field.
  \param[in]   buf  buffer containing http header.
  \param[in]   end  buffer end.
  \return      pointer to next field or NULL.
  \note        Header fields are separated with "\r\n" sequences.
*/
static const char *get_next_field (const char *buf, const char *end) {
  const char *bp;

  for (bp = buf, end--; bp < end; bp++) {
    if (bp[0] == '\r' && bp[1] == '\n') {
      break;
    }
  }
  if (bp == buf || bp == end) {
    /* Last header field only CRLF */
    return (NULL);
  }
  return (bp+2);
}

/**
  \brief       Parse HTTP header and search for next parameter.
  \param[in]   buf  buffer containing http header.
  \param[in]   end  buffer end.
  \return      pointer to next parameter or NULL.
  \note        Header parameters are separated with ';' characters.
*/
static const char *get_next_par (const char *buf, const char *end) {
  const char *bp;

  for (bp = buf, end--; bp < end; bp++) {
    if (bp[0] == ';' && bp[1] == ' ') {
      break;
    }
    if (bp[0] == '\r' || bp[0] == '\n') {
      /* End of HTTP header field */
      return (NULL);
    }
  }
  if (bp == buf || bp == end) {
    /* Last header field only CRLF */
    return (NULL);
  }
  return (bp+2);
}

/**
  \brief       Copy LF-terminated string.
  \param[out]  dp       destination string.
  \param[in]   sp       source string.
  \param[in]   max_len  size of destination buffer.
  \return      number of characters copied.
  \note        String 'sp' or 'max_len' define copy length.
*/
static int32_t http_strcpy (char *dp, const char *sp, int32_t max_len) {
  int32_t i;

  for (i = 0; i < max_len; i++) {
    char ch = dp[i] = sp[i];
    if (ch == 0) {  
      break;
    }
    if (ch == '\n') {
      i++;
      break;
    }
  }
  return (i);
}

/**
  \brief       Fix filename for the server.
  \param[in]   fname  pointer to filename.
*/
static void fix_fname (char *fname) {
  /* Remove trailing spaces and CRLF, null-terminate */
  for ( ; *fname; fname++) {
    if (*fname == ' ' || *fname == '\r') {
      break;
    }
  }
  *fname = 0;
}

/**
  \brief       Read next script line and skip comments.
  \param[in]   http_s  session descriptor.
*/
static void read_script_line (NET_HTTP_INFO *http_s) {
  uint32_t i;
  uint8_t *sp,ch;

  if (http_s->Flags & HTTP_FLAG_SFILE) {
    /* Read Script file from external FS */
    do {
      /* External fgets returns a null-terminated string */
      if (netHTTPs_fgets (http_s->sFile, http_s->Script, HTTP_CGI_MAXLINE) == NULL) {
        http_s->Script[0] = '.';
        http_s->Script[1] = 0;
        break;
      }
    } while (http_s->Script[0] == '#');
    if (http_s->FType == HTTP_TYPE_CGX) {
      i = strlen (http_s->Script);
      if (http_s->Script[i-1] == '\n') {
        i--;
        if (http_s->Script[i-1] == '\r') {
          i--;
        }
      }
      /* For CGX files ignore CR-LF sequence */
      http_s->Script[i] = 0;
    }
    return;
  }
  /* Read script file from internal ROM FS */
  sp = http_s->sFile;
  switch (*sp) {
    case CGI_CMD_T:
      ch = 't';
      break;
    case CGI_CMD_C:
      ch = 'c';
      break;
    case CGI_CMD_I:
      ch = 'i';
      break;
    default:
      /* End of script */
      http_s->Script[0] = '.';
      return;
  }
  http_s->Script[0] = ch;
  http_s->Script[1] = ' ';
  for (i = 2, sp++; i < HTTP_CGI_MAXLINE-1; i++) {
    if (*sp < CGI_CMD_END) {
      /* Found next token, this is end of line */
      break;
    }
    http_s->Script[i] = *sp++;
  }
  /* Make a string null-terminated */
  http_s->Script[i] = 0;
  http_s->sFile = sp;
}

/**
  \brief       Check if cookie is valid.
  \param[in]   cookie  pointer to cookie.
  \return      verification status:
               - true  = cookie valid,
               - false = cookie not valid.
*/
static bool is_cookie_valid (const char *cookie) {
  int32_t i,pos;

  /* Find equals sign */
  pos = net_strchr (cookie, '=');
  if (pos <= 0) {
    /* Not found */
    return (false);
  }
  /* Verify cookie-name is RFC2616 token */
  for (i = 0; i < pos; i++) {
    /* Allowed alphanums plus !#$%&'*+-.^_`|~ */
    if (!((cookie[i] >= '^' && cookie[i] <= 'z') ||
          (cookie[i] >= 'A' && cookie[i] <= 'Z') ||
          (cookie[i] >= '0' && cookie[i] <= '9') ||
          (cookie[i] >= '#' && cookie[i] <= '\'')||
          (cookie[i] >= '*' && cookie[i] <= '+') ||
          (cookie[i] >= '-' && cookie[i] <= '.') ||
          (cookie[i] == '!')                     ||
          (cookie[i] == '|')                     ||
          (cookie[i] == '~')                    )) {
      return (false);
    }
  }
  return (true);
}

#ifdef Network_Debug_STDIO
/**
  \brief       Debug print received cookie.
  \param[in]   cookie  pointer to cookie.
  \param[in]   len     length of the cookie.
*/
static void debug_inf2 (const char *cookie, uint32_t len) {
  char *buf;

  if (len > 80) len = 80;
  buf = (char *)net_mem_alloc (len);
  net_strncpy (buf, cookie, len);
  DEBUGF (HTTP," Cookie [; next]: %s\n",buf);
  net_mem_free (__FRAME(buf));
}
#endif

/* Library default function(s) */
__WEAK const char *netCGI_ContentType (const char *file_ext) {
  (void)file_ext; return (NULL); }
__WEAK const char *netCGX_ContentType (void) {
  return (NULL); }
__WEAK const char *netCGI_Charset (void) {
  return (NULL); }
__WEAK const char *netCGI_CustomHeader (void) {
  return (NULL); }
__WEAK const char *netCGI_Redirect (const char *file_name) {
  (void)file_name; return (NULL); }
__WEAK bool netHTTPs_AcceptClient (const NET_ADDR *addr) {
  (void)addr; return (true); }
__WEAK bool netHTTPs_FileAccess (uint8_t user_id, const char *fname) {
  (void)user_id; (void)fname; return (true); }
__WEAK void netCGI_ProcessQuery (const char *qstr) {
  (void)qstr; }
__WEAK void netCGI_ProcessData (uint8_t code,
                                const char *data, uint32_t len) {
  (void)code; (void)data; (void)len; }
__WEAK uint32_t netCGI_Script (const char *env, char *buf,
                               uint32_t buflen, uint32_t *pcgi) {
  (void)env; (void)buf; (void)buflen; (void)pcgi; return (0); }
__WEAK void netCGI_ProcessRequest (const char *method, const char *uri,
                               const char *header, uint32_t header_len) {
  (void)method; (void)uri; (void)header; (void)header_len;
  /* Stop processing of http headers */
  http->hdr_access = false;
}
__WEAK void netCGI_ProcessCookie (const char *method, const char *uri,
                               const char *cookie, uint32_t cookie_len) {
  (void)method; (void)uri; (void)cookie; (void)cookie_len;
  /* Stop processing of http cookies */
  http->en_cookie = false;
}
