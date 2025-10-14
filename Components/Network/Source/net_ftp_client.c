/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ftp_client.c
 * Purpose: File Transfer Client
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_mem.h"
#include "net_tcp.h"
#include "net_ftp_client.h"
#include "net_dbg.h"

/* Local variables */
static NET_FTPC_INFO  ftpc_control;
#define ftpc_s      (&ftpc_control)

/* Local Functions */
static uint32_t ftpc_listener (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len);
static uint32_t ftpc_data_cb  (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len);
static netStatus ftpc_dopen_req (void);
static uint16_t ftpc_scan_dport (const char *buf);
static void ftpc_close_file (void);
static void ftpc_transit (uint8_t state);
static void ftpc_stop (netFTPc_Event event);
static bool ftpc_proc_resp (const char *buf, const char *end);
static uint8_t get_resp (const char *buf);
static void unix_path (char *path, int32_t len);
static int32_t no_path (char *name, int32_t len);
static bool is_not_found (const char *buf, int32_t len);
#ifdef Network_Debug_STDIO
 static const char *cmd_ascii (netFTP_Command cmd);
 static const char *evt_ascii (netFTPc_Event cb_evt);
#endif

/**
  \brief       Initialize FTP client.
*/
void net_ftp_client_init (void) {
  int32_t sock;

  DEBUGF (FTPC,"Init Client, %s mode\n",ftpc->PasvMode ? "Passive" : "Active");
  EvrNetFTPc_InitClient (ftpc->PasvMode);
  memset (ftpc_s, 0, sizeof (*ftpc_s));
  /* FTP control socket */
  sock = net_tcp_get_socket (ftpc_listener);
  if (sock < 0) sock = 0;
  ftpc_s->Socket = sock & 0xFF;
  /* FTP data socket */
  sock = net_tcp_get_socket (ftpc_data_cb);
  if (sock < 0) sock = 0;
  ftpc_s->DSocket = sock & 0xFF;
  net_tcp_set_option (sock, netTCP_OptionDelayedACK, 1);
  if (ftpc_s->Socket == 0 || ftpc_s->DSocket == 0) {
    ERRORF (FTPC,"Get socket failed\n");
    EvrNetFTPc_GetSocketFailed ();
    netHandleError (netErrorTcpAlloc);
    return;
  }
}

/**
  \brief       De-initialize FTP client.
*/
void net_ftp_client_uninit (void) {
  DEBUGF (FTPC,"Uninit Client\n");
  EvrNetFTPc_UninitClient ();

  /* Close local file, if open */
  ftpc_close_file ();
  memset (ftpc_s, 0, sizeof (*ftpc_s));
}

/**
  \brief       Connect to FTP server and start file operation.
  \param[in]   addr     structure containing server IP address and port.
  \param[in]   command  file command to perform as defined with netFTP_Command.
  \return      status code as defined with netStatus.
*/
netStatus netFTPc_Connect (const NET_ADDR *addr, netFTP_Command command) {

  START_LOCK (netStatus);

  DEBUGF (FTPC,"Connect Client\n");
  if (net_addr_is_unspec ((const __ADDR *)addr)) {
    ERRORF (FTPC,"Connect, Invalid parameter\n");
    EvrNetFTPc_ConnectInvalidParameter ();
    RETURN (netInvalidParameter);
  }
  if ((ftpc_s->State != FTPC_STATE_IDLE)                         ||
      (net_tcp_get_state (ftpc_s->Socket)  > netTCP_StateCLOSED) ||
      (net_tcp_get_state (ftpc_s->DSocket) > netTCP_StateLISTEN)) {
    ERRORF (FTPC,"Connect, Client busy\n");
    EvrNetFTPc_ConnectClientBusy ();
    RETURN (netWrongState);
  }
  net_addr_copy (&ftpc_s->Server, (const __ADDR *)addr);
  if (ftpc_s->Server.port == 0) {
    /* Port not specified, use default server port */
    ftpc_s->Server.port = FTPC_SERVER_PORT;
  }
  /* Save server control port */
  ftpc_s->Port = ftpc_s->Server.port;
  DEBUGF (FTPC," Server [%s], port %d\n",net_addr_ntoa(&ftpc_s->Server),ftpc_s->Port);
  EvrNetFTPc_Connect (&ftpc_s->Server);
  ftpc_s->Command  = command;
  ftpc_s->File     = NULL;
  ftpc_s->cb_event = netFTPc_EventSuccess;
  ftpc_s->Flags    = ftpc->PasvMode ? FTPC_FLAG_PASSIVE : 0;
  ftpc_transit (FTPC_STATE_CONNRQ);
  RETURN (netOK);

  END_LOCK;
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
static uint32_t ftpc_listener (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len) {
  (void)addr;

  /* Check if this is our socket */
  if (ftpc_s->Socket != socket) {
    return (false);
  }

  switch (event) {
    case netTCP_EventConnect:
      DEBUGF (FTPC,"Inbound conn.rejected\n");
      EvrNetFTPc_InboundConnRejected (ftpc_s->Socket);
      return (false);

    case netTCP_EventAborted:
      if (ftpc_s->State == FTPC_STATE_IDLE) {
        break;
      }
      ERRORF (FTPC,"Client stop, Socket aborted\n");
      EvrNetFTPc_SocketAborted (ftpc_s->Socket);
      net_tcp_abort (ftpc_s->DSocket);
      ftpc_stop (netFTPc_EventError);
      break;

    case netTCP_EventEstablished:
      if (ftpc_s->State == FTPC_STATE_CONNRQ) {
        DEBUGF (FTPC,"Client connected\n");
        EvrNetFTPc_SocketConnected (ftpc_s->Socket);
        ftpc_transit (FTPC_STATE_OPEN);
      }
      break;

    case netTCP_EventClosed:
      DEBUGF (FTPC,"Client stop, Socket closed\n");
      EvrNetFTPc_SocketClosed (ftpc_s->Socket);
      net_tcp_abort (ftpc_s->DSocket);
      ftpc_stop (netFTPc_EventError);
      break;

    case netTCP_EventACK:
      /* Our message is acked */
      break;

    case netTCP_EventData:
      /* Get reply from server */
      DEBUGF (FTPC,"*** Process_frame ***\n");
      DEBUGF (FTPC," Length %d bytes\n",len);
      EvrNetFTPc_ReceiveFrame (ftpc_s->Socket, len);
      if(len < 5) {
        /* Data frame too short (3-char response + CRLF) */
        ERRORF (FTPC,"Process, Frame too short\n");
        EvrNetFTPc_FrameTooShort (len, 5);
        return (false);
      }
      DEBUGF (FTPC," Reply code: %3.3s\n",buf);
      EvrNetFTPc_ShowReplyCode (buf);
      if (ftpc_proc_resp ((const char *)buf, (const char *)buf+len) == false) {
        /* Multi-line response not completed, wait for more */
        DEBUGF (FTPC," Fragmented response\n");
        EvrNetFTPc_ResponseFragmented ();
        return (true);
      }
      ftpc_s->Flags &= ~(FTPC_FLAG_RESP | FTPC_FLAG_FIRST);
      switch (ftpc_s->State) {
        case FTPC_STATE_OPEN:
          /* Open, Control socket connected */
          if (ftpc_s->Resp != FTPC_RESP_READY) {
            break;
          }
          DEBUGF (FTPC," Server is ready\n");
          EvrNetFTPc_ServerReady ();
          ftpc_transit (FTPC_STATE_USER);
          return (true);

        case FTPC_STATE_USER:
          /* Username sent */
          if (ftpc_s->Resp == FTPC_RESP_USEROK) {
            goto loggedin;
          }
          if (ftpc_s->Resp != FTPC_RESP_RQPASSW) {
            goto loginfail;
          }
          DEBUGF (FTPC," User ok, need password\n");
          EvrNetFTPc_UserOkNeedPassword ();
          ftpc_transit (FTPC_STATE_PASSW);
          return (true);

        case FTPC_STATE_PASSW:
          /* Password sent */
          if (ftpc_s->Resp != FTPC_RESP_USEROK) {
loginfail:  ERRORF (FTPC,"User login failed\n");
            EvrNetFTPc_UserLoginFailed ();
            ftpc_s->cb_event = netFTPc_EventLoginFailed;
            ftpc_transit (FTPC_STATE_QUIT);
            return (true);
          }
loggedin: DEBUGF (FTPC," User logged in\n");
          EvrNetFTPc_UserLoginSuccess ();
          ftpc_transit (FTPC_STATE_CWD);
          return (true);

        case FTPC_STATE_CWD:
          /* Change working directory command sent */
          if (ftpc_s->Resp == FTPC_RESP_NOACCESS) {
            ERRORF (FTPC,"Path not found\n");
            EvrNetFTPc_WorkingDirectoryInvalid ();
            ftpc_s->cb_event = netFTPc_EventInvalidDirectory;
            ftpc_transit (FTPC_STATE_QUIT);
            return (true);
          }
          if (ftpc_s->Resp != FTPC_RESP_FCMDOK) {
            break;
          }
          DEBUGF (FTPC," Working directory set\n");
          /* Execute a user command now */
          DEBUGF (FTPC,"Executing %s command\n",cmd_ascii(ftpc_s->Command));
          EvrNetFTPc_ExecuteUserCommand (ftpc_s->Command);
          switch (ftpc_s->Command) {
            case netFTP_CommandPUT:
            case netFTP_CommandGET:
            case netFTP_CommandAPPEND:
              ftpc_transit (FTPC_STATE_TYPE_I);
              break;
            case netFTP_CommandLIST:
            case netFTP_CommandNLIST: 
              goto dopen;
            case netFTP_CommandDELETE:
              ftpc_transit (FTPC_STATE_DELE);
              break;
            case netFTP_CommandRENAME:
              ftpc_transit (FTPC_STATE_RNFR);
              break;
            case netFTP_CommandMKDIR:
              ftpc_transit (FTPC_STATE_MKD);
              break;
            case netFTP_CommandRMDIR:
              ftpc_transit (FTPC_STATE_RMD);
              break;
          }
          return (true);

        case FTPC_STATE_TYPE_I:
          /* TYPE I command sent, binary transfer mode */
          if (ftpc_s->Resp != FTPC_RESP_CMDOK) {
            break;
          }
          DEBUGF (FTPC," Binary mode enabled\n");
          EvrNetFTPc_BinaryModeEnabled ();
dopen:    ftpc_transit ((ftpc_s->Flags & FTPC_FLAG_PASSIVE) ?
                        FTPC_STATE_XPASV : FTPC_STATE_XPORT);
          return (true);

        case FTPC_STATE_XPASV:
          /* PASV/EPSV command sent, passive mode */
          if ((ftpc_s->Resp != FTPC_RESP_PASVOK) &&
              (ftpc_s->Resp != FTPC_RESP_EPSVOK)) {
            break;
          }
          ftpc_s->DPort = ftpc_scan_dport ((const char *)buf);
          if (ftpc_s->DPort == 0) {
            ERRORF (FTPC,"PASV command failed\n");
            EvrNetFTPc_PasvCommandFailed ();
            break;
          }
          DEBUGF (FTPC," Passive mode, port %d\n",ftpc_s->DPort);
          EvrNetFTPc_PassiveModeStart (ftpc_s->DPort);
          ftpc_transit (FTPC_STATE_FOPEN);
          return (true);

        case FTPC_STATE_XPORT:
          /* PORT/EPRT command sent, active mode */
          if (ftpc_s->Resp == FTPC_RESP_CMDERR) {
            /* Active mode not supported, try passive */
            DEBUGF (FTPC," PORT cmd failed, trying Passive\n");
            EvrNetFTPc_PortCommandFailed ();
            ftpc_s->Flags |= FTPC_FLAG_PASSIVE;
            ftpc_transit (FTPC_STATE_XPASV);
            return (true);
          }
          if (ftpc_s->Resp != FTPC_RESP_CMDOK) {
            break;
          }
          DEBUGF (FTPC," Port accepted\n");
          EvrNetFTPc_ActiveModeStart (ftpc_s->DPort);
          ftpc_transit (FTPC_STATE_FOPEN);
          return (true);

        case FTPC_STATE_CMDSEND:
          /* Sending a command which needs a data connection */
          if (ftpc_s->Resp == FTPC_RESP_NOACCESS) {
            if (ftpc_s->Command == netFTP_CommandGET) {
              ERRORF (FTPC,"File not found\n");
              EvrNetFTPc_FileNotFoundOnServer ();
              ftpc_s->cb_event = netFTPc_EventFileNotFound;
            }
            else {
              ERRORF (FTPC,"Operation not allowed\n");
              EvrNetFTPc_OperationNotAllowed ();
              ftpc_s->cb_event = netFTPc_EventAccessDenied;
            }
            net_tcp_close (ftpc_s->DSocket);
            ftpc_close_file ();
            ftpc_transit (FTPC_STATE_QUIT);
            return (true);
          }
          if (ftpc_s->Resp == FTPC_RESP_FSTATOK) {
            DEBUGF (FTPC," About to open data conn.\n");
            EvrNetFTPc_AboutToOpenDataConn (ftpc_s->DSocket);
          }
          else if (ftpc_s->Resp == FTPC_RESP_DSTART) {
            DEBUGF (FTPC," Data conn. already open\n");
            EvrNetFTPc_DataConnAlreadyOpen (ftpc_s->DSocket);
          }
          else {
            break;
          }
          switch ((int32_t)ftpc_s->Command) {
            case netFTP_CommandPUT:
            case netFTP_CommandAPPEND:
              ftpc_transit (FTPC_STATE_STOR);
              break;
            case netFTP_CommandGET:
              ftpc_transit (FTPC_STATE_RETR);
              break;
            case netFTP_CommandLIST:
            case netFTP_CommandNLIST:
              ftpc_transit (FTPC_STATE_LIST);
              break;
            default:
              break;
          }
          return (true);

        case FTPC_STATE_RETR:
          /* File retrieve in progress */
          if (ftpc_s->Resp == FTPC_RESP_DABORT) {
            /* Transfer abort confirmed */
            DEBUGF (FTPC," Transfer aborted\n");
            EvrNetFTPc_TransferAborted ();
            ftpc_s->Flags |= FTPC_FLAG_CDONE;
            return (true);
          }
          __FALLTHROUGH;

        case FTPC_STATE_STOREND:
          /* File store command ended */
          if ((ftpc_s->Resp != FTPC_RESP_DCLOSE) &&
              (ftpc_s->Resp != FTPC_RESP_FCMDOK)) {
            break;
          }
          DEBUGF (FTPC," Transfer completed\n");
          EvrNetFTPc_TransferCompleted ();
          ftpc_s->Flags |= FTPC_FLAG_CDONE;
          return (true);

        case FTPC_STATE_LIST:
          /* Directory listing in progress. */
          if (ftpc_s->Resp == FTPC_RESP_NOACCESS) {
            ERRORF (FTPC,"File not found\n");
            EvrNetFTPc_FileNotFoundOnServer ();
            ftpc_s->cb_event = netFTPc_EventFileNotFound;
            ftpc_s->Flags |= FTPC_FLAG_CDONE;
            return (true);
          }
          if ((ftpc_s->Resp != FTPC_RESP_DCLOSE) &&
              (ftpc_s->Resp != FTPC_RESP_FCMDOK)) {
            break;
          }
          DEBUGF (FTPC," Transfer completed\n");
          EvrNetFTPc_TransferCompleted ();
          ftpc_s->Flags |= FTPC_FLAG_CDONE;
          return (true);

        case FTPC_STATE_DELE:
          /* Delete command sent */
          if (ftpc_s->Resp == FTPC_RESP_NOACCESS) {
            goto noaccess;
          }
          if (ftpc_s->Resp != FTPC_RESP_FCMDOK) {
            break;
          }
          DEBUGF (FTPC," File deleted\n");
          EvrNetFTPc_FileDeleted ();
          ftpc_transit (FTPC_STATE_QUIT);
          return (true);

        case FTPC_STATE_RNFR:
          /* Rename from command sent */
          if (ftpc_s->Resp == FTPC_RESP_NOACCESS) {
            goto noaccess;
          }
          if (ftpc_s->Resp != FTPC_RESP_RQNAME) {
            break;
          }
          DEBUGF (FTPC," New name reqired\n");
          EvrNetFTPc_NewNameRequired ();
          ftpc_transit (FTPC_STATE_RNTO);
          return (true);

        case FTPC_STATE_RNTO:
          /* Rename to command sent */
          if (ftpc_s->Resp == FTPC_RESP_NOACCESS) {
            goto noaccess;
          }
          if (ftpc_s->Resp != FTPC_RESP_FCMDOK) {
            break;
          }
          DEBUGF (FTPC," File/directory renamed\n");
          EvrNetFTPc_FileOrDirectoryRenamed ();
          ftpc_transit (FTPC_STATE_QUIT);
          return (true);

        case FTPC_STATE_MKD:
          /* Make directory command sent */
          if (ftpc_s->Resp == FTPC_RESP_NOACCESS) {
            goto noaccess;
          }
          if (ftpc_s->Resp != FTPC_RESP_NEWDIROK) {
            break;
          }
          DEBUGF (FTPC," Directory created\n");
          EvrNetFTPc_DirectoryCreated ();
          ftpc_transit (FTPC_STATE_QUIT);
          return (true);

        case FTPC_STATE_RMD:
          /* Remove directory command sent */
          if (ftpc_s->Resp == FTPC_RESP_NOACCESS) {
noaccess:   if (is_not_found ((const char *)buf, (int32_t)len)) {
              DEBUGF (FTPC," File/path not found\n");
              EvrNetFTPc_FileOrPathNotFound ();
              ftpc_s->cb_event = netFTPc_EventFileNotFound;
            }
            else {
              ERRORF (FTPC,"Operation not allowed\n");
              EvrNetFTPc_OperationNotAllowed ();
              ftpc_s->cb_event = netFTPc_EventAccessDenied;
            }
            ftpc_transit (FTPC_STATE_QUIT);
            return (true);
          }
          if (ftpc_s->Resp != FTPC_RESP_FCMDOK) {
            break;
          }
          DEBUGF (FTPC," Directory removed\n");
          EvrNetFTPc_DirectoryRemoved ();
          ftpc_transit (FTPC_STATE_QUIT);
          return (true);

        case FTPC_STATE_QUIT:
          /* Quit command given */
          if (ftpc_s->Resp != FTPC_RESP_QUITOK) {
            break;
          }
          /* Keep already assigned callback event */
          ftpc_stop (ftpc_s->cb_event);
          return (true);

        case FTPC_STATE_STOP:
        case FTPC_STATE_TWAIT:
          /* Stop the client */
          return (true);
      }
#ifdef __DEBUG_ENABLED
      if (ftpc_s->Resp == FTPC_RESP_CMDERR) {
        ERRORF (FTPC,"Command error response\n");
        EvrNetFTPc_CommandErrorResponse ();
      }
      else {
        (__CONST_CAST(char *)buf)[len-2] = 0;
        ERRORF (FTPC,"Wrong response: %s\n",buf);
        EvrNetFTPc_WrongResponse (buf, len-2);
      }
#endif
      ftpc_stop (netFTPc_EventError);
      break;
  }
  return (false);
}

/**
  \brief       Data socket event callback notification.
  \param[in]   socket  data socket handle.
  \param[in]   event   event type as defined with netTCP_Event.
  \param[in]   addr    structure containing remote IP address and port.
  \param[in]   buf     pointer to a buffer containing the data.
  \param[in]   len     length of the data.
  \return      status:
               - true  = success,
               - false = error.
*/
static uint32_t ftpc_data_cb (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                              const uint8_t *buf, uint32_t len) {
  (void)addr;
  if (ftpc_s->DSocket != socket) {
    return (false);
  }

  switch (event) {
    case netTCP_EventConnect:
      if (ftpc_s->State == FTPC_STATE_IDLE) {
        /* Safety reject inbound connection */
        return (false);
      }
      if (ftpc_s->Flags & FTPC_FLAG_PASSIVE) {
        ERRORF (FTPC,"Passive, Inbound data conn.rejected\n");
        EvrNetFTPc_InboundConnRejected (ftpc_s->DSocket);
        return (false);
      }
      /* Accept data connections in active mode only */
      break;

    case netTCP_EventAborted:
    case netTCP_EventClosed:
      DEBUGF (FTPC,"Data socket closed\n");
      EvrNetFTPc_DataSocketClosed (ftpc_s->DSocket);
      ftpc_s->Flags &= ~FTPC_FLAG_DOPEN;
      ftpc_s->Flags |= FTPC_FLAG_DCLOSED;
      break;

    case netTCP_EventEstablished:
      DEBUGF (FTPC,"Data socket opened\n");
      EvrNetFTPc_DataSocketOpened (ftpc_s->DSocket);
      ftpc_s->Flags |= FTPC_FLAG_DOPEN;
      break;

    case netTCP_EventACK:
      /* Our message is acked */
      ftpc_s->Flags &= ~FTPC_FLAG_DACK;
      net_tcp_reset_tout (ftpc_s->Socket);
      ftpc_s->Timer = ftpc->DefTout;
      break;

    case netTCP_EventData:
      switch ((int32_t)ftpc_s->Command) {
        case netFTP_CommandGET:
          if (ftpc_s->Flags & FTPC_FLAG_ABORT) {
            break;
          }
          if ((ftpc_s->State != FTPC_STATE_RETR) &&
              (ftpc_s->State != FTPC_STATE_CMDSEND)) {
            break;
          }
          /* File content received, accepts also early data */
          if (netFTPc_fwrite (ftpc_s->File, buf, len) != len) {
            /* Write failure, abort the connection */
            ERRORF (FTPC,"Local, Disk full\n");
            EvrNetFTPc_LocalDiskWriteError ();
            ftpc_s->Flags |= FTPC_FLAG_ABORT;
            break;
          }
          net_tcp_reset_tout (ftpc_s->Socket);
          ftpc_s->Timer = ftpc->DefTout;
          break;
        case netFTP_CommandLIST:
        case netFTP_CommandNLIST:
          if ((ftpc_s->State != FTPC_STATE_LIST) &&
              (ftpc_s->State != FTPC_STATE_CMDSEND)) {
            break;
          }
          /* Directory listing received, accepts also early data */
          netFTPc_Process (netFTPc_RequestList, __CONST_CAST(char *)buf, len);
          net_tcp_reset_tout (ftpc_s->Socket);
          ftpc_s->Timer = ftpc->DefTout;
          break;
        default:
          break;
      }
      break;
  }
  return (true);
}

/**
  \brief       Run FTP client main function.
*/
void net_ftp_client_run (void) {
  uint8_t *sendbuf;
  const uint8_t *ip;
  uint32_t cblen,max_dsize = 0;
  const char *tp;
  int32_t len;

  if (ftpc_s->State == FTPC_STATE_IDLE) {
    return;
  }

  /* Client timeout supervision */
  if ((sys->Flags & SYS_FLAG_SEC) && ftpc_s->Timer) {
    if (--ftpc_s->Timer == 0) {
      /* Client timeout timer has expired */
      ERRORF (FTPC,"Client stop, Timeout expired\n");
      EvrNetFTPc_ClientStopTimeoutExpired ();
      net_tcp_abort (ftpc_s->Socket);
      net_tcp_abort (ftpc_s->DSocket);
      ftpc_stop (netFTPc_EventTimeout);
      return;
    }
  }

  if (ftpc_s->Flags & FTPC_FLAG_RESP) {
    /* Waiting for command response */
    return;
  }

  switch (ftpc_s->State) {
    case FTPC_STATE_CONNRQ:
      /* User has requested to open control connection */
      ftpc_s->Server.port = ftpc_s->Port;
      if (net_tcp_connect (ftpc_s->Socket, &ftpc_s->Server, 0) != netOK) {
        ftpc_stop (netFTPc_EventError);
        break;
      }
      ftpc_s->Flags |= FTPC_FLAG_RESP;
      break;

    case FTPC_STATE_USER:
      /* Send USER command */
      sendbuf = net_tcp_get_buf (45);
      len     = net_strcpy ((char *)sendbuf, "USER ");
      len    += netFTPc_Process (netFTPc_RequestUsername, (char *)sendbuf+len, 40);
      goto cr_send;

    case FTPC_STATE_PASSW:
      /* Send PASS command */
      sendbuf = net_tcp_get_buf (45);
      len     = net_strcpy ((char *)sendbuf, "PASS ");
      len    += netFTPc_Process (netFTPc_RequestPassword, (char *)sendbuf+len, 40);
      goto cr_send;

    case FTPC_STATE_CWD:
      /* Send CWD command, change working directory */
      sendbuf = net_tcp_get_buf (FTPC_MAX_PATH+8);
      len     = net_strcpy ((char *)sendbuf, "CWD ");
      len    += netFTPc_Process (netFTPc_RequestDirectory, (char *)sendbuf+len, FTPC_MAX_PATH);
      if (len == 4) {
        /* Path not specified by user */
        sendbuf[len++] = '/';
      }
      /* Convert path to unix-style */
      unix_path ((char *)sendbuf+4, (int32_t)len-4);
      goto cr_send;

    case FTPC_STATE_TYPE_I:
      /* Binary/image mode enable */
      sendbuf = net_tcp_get_buf (10);
      len     = net_strcpy ((char *)sendbuf, "TYPE I");
      goto cr_send;

    case FTPC_STATE_XPASV:
      /* Enter passive or extended passive mode */
      sendbuf = net_tcp_get_buf (10);
      tp = (ftpc_s->Server.addr_type == NET_ADDR_IP4) ? "PASV" : "EPSV";
      len     = net_strcpy ((char *)sendbuf, tp);
      goto cr_send;

    case FTPC_STATE_XPORT:
      /* Enter active mode, select data port */
      ftpc_s->DPort = net_tcp_alloc_port ();
      DEBUGF (FTPC,"Local data port %d\n",ftpc_s->DPort);
      EvrNetFTPc_LocalPortAssigned (ftpc_s->DPort);
      sendbuf = net_tcp_get_buf (60);
      ip = net_tcp_get_local_addr (ftpc_s->Socket);
      if (ftpc_s->Server.addr_type == NET_ADDR_IP4) {
        /* Add local IPv4 address and port number */
        /* Format: "PORT %d,%d,%d,%d,%d,%d"       */
        int32_t ii;
        len  = net_strcpy ((char *)&sendbuf[0],   "PORT ");
        len += net_strcpy ((char *)&sendbuf[len], net_addr4_ntoa (ip));
        /* Replace '.' with ',' */
        for (ii = 5; ii < len; ii++) {
          if (sendbuf[ii] == '.') sendbuf[ii] = ',';
        }
        sendbuf[len++] = ',';
        len += net_itoa ((char *)&sendbuf[len], HI_BYTE(ftpc_s->DPort));
        sendbuf[len++] = ',';
        len += net_itoa ((char *)&sendbuf[len], LO_BYTE(ftpc_s->DPort));
        goto cr_send;
      }
#ifdef Network_IPv6
      /* Add local IPv6 address and port number */
      /* Format: "EPRT |2|%s|%d|"               */
      len  = net_strcpy ((char *)&sendbuf[0],   "EPRT |2|");
      len += net_strcpy ((char *)&sendbuf[len], net_addr6_ntoa (ip));
      sendbuf[len++] = '|';
      len += net_itoa   ((char *)&sendbuf[len], ftpc_s->DPort);
      sendbuf[len++] = '|';
#else
      /* This should never happen */
      len = 0;
#endif
      goto cr_send;

    case FTPC_STATE_FOPEN:
      /* Open a local file for read or write */
      switch ((int32_t)ftpc_s->Command) {
        case netFTP_CommandPUT:
        case netFTP_CommandAPPEND:
          /* Open local file for reading */
          tp = "rb";
          break;
        case netFTP_CommandGET:
          /* Open local file for writing */
          tp = "wb";
          break;
        default:
          /* Local file not needed */
          tp = NULL;
          break;
      }
      if (tp != NULL) {
        /* Command needs to open a local file */
        char *name = (char *)net_mem_alloc (FTPC_MAX_PATH+1);
        netFTPc_Process (netFTPc_RequestLocalFilename, name, FTPC_MAX_PATH);
        DEBUGF (FTPC,"Open file: %s\n",name);
        EvrNetFTPc_OpenLocalFile (name, strlen(name));
        ftpc_s->File = netFTPc_fopen (name, tp);
        net_mem_free (__FRAME(name));
        if (ftpc_s->File == NULL) {
          if (ftpc_s->Command == netFTP_CommandGET) {
            ERRORF (FTPC,"Local file create failed\n");
            EvrNetFTPc_LocalFileCreateFailed ();
          }
          else {
            ERRORF (FTPC,"Local file not found\n");
            EvrNetFTPc_LocalFileNotFound ();
          }
          ftpc_s->cb_event = netFTPc_EventLocalFileError;
          ftpc_transit (FTPC_STATE_QUIT);
          break;
        }
      }
      ftpc_transit (FTPC_STATE_CMDSEND);
      break;

    case FTPC_STATE_CMDSEND:
      /* Send a command to server and open data connection */
      if (ftpc_dopen_req () != netOK) {
        /* Failed to open a data connection */
        ERRORF (FTPC,"Open data connection failed\n");
        EvrNetFTPc_OpenDataConnFailed (ftpc_s->DSocket);
        ftpc_s->cb_event = netFTPc_EventError;
        ftpc_transit (FTPC_STATE_QUIT);
        break;
      }
      sendbuf = net_tcp_get_buf (FTPC_MAX_PATH+8);
      switch ((int32_t)ftpc_s->Command) {
        case netFTP_CommandPUT:
          len = net_strcpy ((char *)sendbuf, "STOR ");
          break;
        case netFTP_CommandAPPEND:
          len = net_strcpy ((char *)sendbuf, "APPE ");
          break;
        case netFTP_CommandGET:
          len = net_strcpy ((char *)sendbuf, "RETR ");
          break;
        case netFTP_CommandLIST:
          len = net_strcpy ((char *)sendbuf, "LIST ");
          goto get_mask;
        case netFTP_CommandNLIST:
        default:
          len = net_strcpy ((char *)sendbuf, "NLST ");
get_mask: cblen = netFTPc_Process (netFTPc_RequestListMask, (char *)sendbuf+len, FTPC_MAX_PATH);
          if (cblen == 0) {
            /* File filter not specified by user */
            len = 4;
          }
          else {
            /* Remove the path info from the file filter */
            len += no_path ((char *)sendbuf+len, (int32_t)cblen);
          }
          goto cr_send;
      }
      cblen = netFTPc_Process (netFTPc_RequestName, (char *)sendbuf+len, FTPC_MAX_PATH);
      /* Remove the path info from the filename */
      len  += no_path ((char *)sendbuf+len, (int32_t)cblen);
      goto cr_send;

    case FTPC_STATE_STOR:
      /* Send data content to FTP server */
      if (!(ftpc_s->Flags & FTPC_FLAG_DOPEN) ||
          (ftpc_s->Flags & FTPC_FLAG_DACK)) {
        /* Wait for data socket to connect or data acknowledge */
        break;
      }
      /* Allocate transmit frame buffer */
      sendbuf = NULL;
      if (net_mem_avail_tx()) {
        max_dsize = net_tcp_get_mss (ftpc_s->DSocket);
        sendbuf   = net_tcp_get_buf (max_dsize | 0x80000000);
      }
      if (sendbuf == NULL) {
        /* Wait, no memory available */
        break;
      }
      /* Send file content */
      len = (int32_t)netFTPc_fread (ftpc_s->File, sendbuf, max_dsize);
      if (net_tcp_send (ftpc_s->DSocket, sendbuf, (uint32_t)len) == netOK) {
        ftpc_s->Flags |= FTPC_FLAG_DACK;
        break;
      }
      /* net_tcp_send will also fail if len==0 */
      net_tcp_close (ftpc_s->DSocket);
      ftpc_s->Flags |= FTPC_FLAG_DCLOSED;
      ftpc_transit (FTPC_STATE_STOREND);
      break;

    case FTPC_STATE_RETR:
      /* Get command in progress */
      if ((ftpc_s->Flags & FTPC_FLAG_ABORT) && 
         !(ftpc_s->Flags & FTPC_FLAG_DCLOSED)) {
        /* Aborted, local file write error */
        ftpc_s->Flags |= FTPC_FLAG_DCLOSED;
        ftpc_s->cb_event = netFTPc_EventLocalFileError;
        net_tcp_abort (ftpc_s->DSocket);
        break;
      }
      __FALLTHROUGH;

    case FTPC_STATE_LIST:
      /* List command in progress */
    case FTPC_STATE_STOREND:
      /* Store or append command finished */
      if ((ftpc_s->Flags & FTPC_FLAG_CDONE) && 
          (ftpc_s->Flags & FTPC_FLAG_DCLOSED)) {
        /* Response received and data socket closed, send QUIT */
        ftpc_close_file ();
        ftpc_transit (FTPC_STATE_QUIT);
      }
      break;

    case FTPC_STATE_DELE:
      /* Delete a file */
      sendbuf = net_tcp_get_buf (FTPC_MAX_PATH+8);
      len     = net_strcpy ((char *)sendbuf, "DELE ");
      cblen   = netFTPc_Process (netFTPc_RequestName, (char *)sendbuf+len, FTPC_MAX_PATH);
      len    += no_path ((char *)sendbuf+len, (int32_t)cblen);
      goto cr_send;

    case FTPC_STATE_RNFR:
      /* Rename a file from 'oldname' */
      sendbuf = net_tcp_get_buf (FTPC_MAX_PATH+8);
      len     = net_strcpy ((char *)sendbuf, "RNFR ");
      cblen   = netFTPc_Process (netFTPc_RequestName, (char *)sendbuf+len, FTPC_MAX_PATH);
      len    += no_path ((char *)sendbuf+len, (int32_t)cblen);
      goto cr_send;

    case FTPC_STATE_RNTO:
      /* Rename a file to 'newname' */
      sendbuf = net_tcp_get_buf (FTPC_MAX_PATH+8);
      len     = net_strcpy ((char *)sendbuf, "RNTO ");
      cblen   = netFTPc_Process (netFTPc_RequestNewName, (char *)sendbuf+len, FTPC_MAX_PATH);
      len    += no_path ((char *)sendbuf+len, (int32_t)cblen);
      goto cr_send;

    case FTPC_STATE_MKD:
      /* Make a directory */
      sendbuf = net_tcp_get_buf (FTPC_MAX_PATH+8);
      len     = net_strcpy ((char *)sendbuf, "MKD ");
      cblen   = netFTPc_Process (netFTPc_RequestName, (char *)sendbuf+len, FTPC_MAX_PATH);
      len    += no_path ((char *)sendbuf+len, (int32_t)cblen);
      goto cr_send;

    case FTPC_STATE_RMD:
      /* Remove a directory */
      sendbuf = net_tcp_get_buf (FTPC_MAX_PATH+8);
      len     = net_strcpy ((char *)sendbuf, "RMD ");
      cblen   = netFTPc_Process (netFTPc_RequestName, (char *)sendbuf+len, FTPC_MAX_PATH);
      len    += no_path ((char *)sendbuf+len, (int32_t)cblen);
      goto cr_send;

    case FTPC_STATE_QUIT:
      /* Send QUIT command */
      sendbuf = net_tcp_get_buf (10);
      len     = net_strcpy ((char *)sendbuf, "QUIT");
cr_send:
      len    += net_strcpy ((char *)sendbuf+len, "\r\n");
#ifdef __DEBUG_ENABLED
      DEBUGF (FTPC,"Sending: %s",sendbuf);
      EvrNetFTPc_SendCommand (sendbuf, (uint32_t)len-2);
#endif
      net_tcp_send (ftpc_s->Socket, sendbuf, (uint32_t)len);
      ftpc_s->Flags |= FTPC_FLAG_RESP;
      break;

    case FTPC_STATE_STOP:
      /* Stop FTP client */
      DEBUGF (FTPC,"Stop Client, Close socket\n");
      EvrNetFTPc_ClientCloseSocket (ftpc_s->Socket);
      ftpc_close_file ();
      net_tcp_close (ftpc_s->Socket);
      ftpc_s->State = FTPC_STATE_TWAIT;
      break;

    case FTPC_STATE_TWAIT:
      /* Waiting for TCP Sockets to close */
      if (((sys->Flags & SYS_FLAG_TICK) == 0)                        ||
          (net_tcp_get_state (ftpc_s->Socket) > netTCP_StateCLOSED)  ||
          (net_tcp_get_state (ftpc_s->DSocket) > netTCP_StateLISTEN)) {
        break;
      }
      /* Sockets are closed, notify the user and goto idle */
      DEBUGF (FTPC,"Return event %s\n",evt_ascii(ftpc_s->cb_event));
      EvrNetFTPc_ClientDone (ftpc_s->cb_event);
      netFTPc_Notify (ftpc_s->cb_event);
      DEBUGF (FTPC,"Client Idle\n");
      ftpc_s->State = FTPC_STATE_IDLE;
      break;
  }
}

/**
  \brief       Open FTP data connection if required.
  \return      status code as defined with netStatus.
*/
static netStatus ftpc_dopen_req (void) {
  if (ftpc_s->Flags & FTPC_FLAG_PASSIVE) {
    /* Connect to server, server is in passive mode */
    ftpc_s->Server.port = ftpc_s->DPort;
    return (net_tcp_connect (ftpc_s->DSocket, &ftpc_s->Server, 0));
  }
  /* Start listening, server is in active mode */
  return (net_tcp_listen (ftpc_s->DSocket, ftpc_s->DPort));
}

/**
  \brief       Scan PASV/EPSV response for FTP data port number.
  \param[in]   buf  buffer containing response parameters.
  \return      data port number or 0=not found.
*/
static uint16_t ftpc_scan_dport (const char *buf) {
  int32_t max,m,n,port;

  if (ftpc_s->Resp == FTPC_RESP_PASVOK) {
    /* PASV example: (192,168,0,162,19,137) */
    for (n = 0, max = 60; max; buf++, max--) {
      if (*buf == ',') {
        if (++n == 4) {
          port  = net_atoi (++buf, &m) << 8;
          port |= net_atoi (++buf+m, NULL);
          return (port & 0xFFFF);
        }
      }
    }
  }
  else {
    /* EPSV example: (|||5282|) */
    for (n = 0, max = 60; max; buf++, max--) {
      if (*buf == '|') {
        if (++n == 3) {
          port = net_atoi (++buf, NULL);
          return (port & 0xFFFF);
        }
      }
    }
  }
  return (0);
}

/**
  \brief       Close local file if opened.
*/
static void ftpc_close_file (void) {
  if (ftpc_s->File != NULL) {
    DEBUGF (FTPC,"Close local file\n");
    EvrNetFTPc_CloseLocalFile ();
    netFTPc_fclose (ftpc_s->File);
    ftpc_s->File = NULL;
  }
}

/**
  \brief       Change state of FTP client to a new state.
  \param[in]   state  new client state.
  \note        This function refreshes also Timeout timer.
*/
static void ftpc_transit (uint8_t state) {
  ftpc_s->State = state;
  ftpc_s->Timer = ftpc->DefTout;
  sys->Busy     = true;
}

/**
  \brief       Stop FTP client.
  \param[in]   event  user notification event.
*/
static void ftpc_stop (netFTPc_Event event) {
  switch (ftpc_s->State) {
    case FTPC_STATE_IDLE:
    case FTPC_STATE_STOP:
    case FTPC_STATE_TWAIT:
      break;
    default:
      ftpc_s->cb_event = event;
      ftpc_s->Flags &= ~FTPC_FLAG_RESP;
      ftpc_s->Timer  = 0;
      ftpc_s->State  = FTPC_STATE_STOP;
  }
}

/**
  \brief       Process a reply code received from server.
  \param[in]   buf  buffer containing a reply.
  \param[in]   end  buffer end.
  \return      status:
               - true  = processing completed,
               - false = more reply frames expected.
*/
static bool ftpc_proc_resp (const char *buf, const char *end) {
  if (!(ftpc_s->Flags & FTPC_FLAG_FIRST)) {
    /* Parse the first line, store the response code */
    ftpc_s->Flags |= FTPC_FLAG_FIRST;
    ftpc_s->Resp   = get_resp (buf);
    buf += 3;
    if ((buf[0] != '-') || (buf >= end)) {
      /* This is a single line response, we are done */
      return (true);
    }
    /* Get next line */
    for ( ; (buf[0] != '\n') && (buf < end); buf++);
    if (++buf >= end) {
      return (false);
    }
  }
  /* Process multi-line response */
  for (;;) {
    if ((get_resp (buf) == ftpc_s->Resp) && (buf[3] != '-')) {
      /* Terminating reply code */
      break;
    }
    for ( ; (buf[0] != '\n') && (buf < end); buf++);
    if (++buf >= end) {
      return (false);
    }
  }
  return (true);
}

/**
  \brief       Read response code from the server.
  \param[in]   buf  buffer containing response text.
  \return      binary response code.
*/
static uint8_t get_resp (const char *buf) {
  static const uint32_t f_resp[] = {
    RESP(1,2,5, FTPC_RESP_DSTART  ),
    RESP(1,5,0, FTPC_RESP_FSTATOK ),
    RESP(2,0,0, FTPC_RESP_CMDOK   ),
    RESP(2,2,0, FTPC_RESP_READY   ),
    RESP(2,2,1, FTPC_RESP_QUITOK  ),
    RESP(2,2,6, FTPC_RESP_DCLOSE  ),
    RESP(2,2,7, FTPC_RESP_PASVOK  ),
    RESP(2,2,9, FTPC_RESP_EPSVOK  ),
    RESP(2,3,0, FTPC_RESP_USEROK  ),
    RESP(2,5,0, FTPC_RESP_FCMDOK  ),
    RESP(2,5,7, FTPC_RESP_NEWDIROK),
    RESP(3,3,1, FTPC_RESP_RQPASSW ),
    RESP(3,5,0, FTPC_RESP_RQNAME  ),
    RESP(4,2,6, FTPC_RESP_DABORT  ),
    RESP(4,5,0, FTPC_RESP_NOACCESS),
    RESP(4,5,1, FTPC_RESP_NOACCESS),
    RESP(5,0,0, FTPC_RESP_CMDERR  ),
    RESP(5,5,0, FTPC_RESP_NOACCESS),
    RESP(5,5,3, FTPC_RESP_NOACCESS) };
  const uint32_t *fp;
  uint32_t cmp;

  /* Construct the comparison string */
  cmp = (uint32_t)(buf[0] << 16 | buf[1] << 8 | buf[2]);
  for (fp = &f_resp[0]; fp < &f_resp[sizeof(f_resp)/sizeof(uint32_t)]; fp++) {
    /* The upper 3 bytes contain the comparison string */
    if ((fp[0] >> 8) == cmp) {
      return (fp[0] & 0xFF);
    }
  }
  return (FTPC_RESP_UNKNOWN);
}

/**
  \brief       Convert path info to unix style path.
  \param[in,out] path  path name.
  \param[in]   len     length of a path name.
*/
static void unix_path (char *path, int32_t len) {
  int32_t i;

  for (i = 0; i < len; i++) {
    if (path[i] == '\\') {
      path[i] = '/';
    }
  }
}

/**
  \brief       Remove path info from file name.
  \param[in,out] name  file name.
  \param[in]   len     length of a file name.
  \return      length of converted name.
*/
static int32_t no_path (char *name, int32_t len) {
  int32_t i,n;

  /* Scan name for folder separator character */
  for (i = len-1; i >= 0; i--) {
    if ((name[i] == '/') || (name[i] == '\\')) {
      break;
    }
  }
  if (i < 0) {
    /* Not found, return the size */
    return (len);
  }
  for (n = 0, i++; i < len; i++, n++) {
    name[n] = name[i];
  }
  return (n);
}

/**
  \brief       Check NOACCESS response for "not found" text.
  \param[in]   buf  buffer with response text.
  \param[in]   len  length of buffer data.
  \return      check status:
               - true  = contains "not found" text,
               - false = no requested text found.
*/
static bool is_not_found (const char *buf, int32_t len) {
  int32_t i;

  if (len < 15) {
    /* Size of "550 not found\r\n" response */
    return (false);
  }
  for (i = 4; i <= len-11; i++) {
    if (net_strcmp (buf+i, "not found")) {
      /* Response message contains "not found" */
      return (true);
    }
  }
  return (false);
}

#ifdef Network_Debug_STDIO
/**
  \brief       Convert FTP command to ascii.
  \param[in]   cmd  FTP command id.
  \return      pointer to text representation of a command.
*/
static const char *cmd_ascii (netFTP_Command cmd) {
  switch (cmd) {
    case netFTP_CommandPUT:
      return ("PUT");
    case netFTP_CommandGET:
      return ("GET");
    case netFTP_CommandAPPEND:
      return ("APPEND");
    case netFTP_CommandDELETE:
      return ("DELETE");
    case netFTP_CommandLIST:
      return ("LIST");
    case netFTP_CommandNLIST:
      return ("NLIST");
    case netFTP_CommandRENAME:
      return ("RENAME");
    case netFTP_CommandMKDIR:
      return ("MKDIR");
    case netFTP_CommandRMDIR:
      return ("RMDIR");
  }
  return ("");
}

/**
  \brief       Convert callback event to ascii.
  \param[in]   cb_evt  callback event id.
  \return      pointer to text representation of an event.
*/
static const char *evt_ascii (netFTPc_Event cb_evt) {
  switch (cb_evt) {
    case netFTPc_EventSuccess:
      return ("SUCCESS");
    case netFTPc_EventTimeout:
      return ("TIMEOUT");
    case netFTPc_EventLoginFailed:
      return ("LOGINFAIL");
    case netFTPc_EventAccessDenied:
      return ("NOACCESS");
    case netFTPc_EventFileNotFound:
      return ("NOTFOUND");
    case netFTPc_EventInvalidDirectory:
      return ("NOPATH");
    case netFTPc_EventLocalFileError:
      return ("ERRORLOCAL");
    case netFTPc_EventError:
      return ("ERROR");
  }
  return ("");
}
#endif
