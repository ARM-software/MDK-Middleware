/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_ftp_server.c
 * Purpose: File Transfer Server
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_mem.h"
#include "net_tcp.h"
#include "net_ftp_server.h"
#include "net_dbg.h"

/* Local variables */
static NET_FTP_CTRL net_ftp_control;
#define ftp       (&net_ftp_control)
static NET_FTP_CMD  cmd;

/* Local functions */
static void ftp_server_start (void);
static void ftp_server_stop (void);
static void ftp_server_run (void);
static uint32_t ftp_listener (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                              const uint8_t *buf, uint32_t len);
static uint32_t ftp_data_cb  (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                              const uint8_t *buf, uint32_t len);
static void ftp_parse_cmd (const char *name);
static netStatus ftp_dopen_req (NET_FTP_INFO *ftp_s);
static NET_FTP_INFO *ftp_map_session (int32_t socket);
static void ftp_kill_session (NET_FTP_INFO *ftp_s);
static uint16_t ftp_scan_dport (const char *buf, uint8_t type);
static char *make_path (NET_FTP_INFO *ftp_s, const char *name);
static void free_name (NET_FTP_INFO *ftp_s);
static void close_file (NET_FTP_INFO *ftp_s);
static void store_path (NET_FTP_INFO *ftp_s, const char *path);
static void path_dir_up (NET_FTP_INFO *ftp_s);
static void set_mask (NET_FTP_INFO *ftp_s, bool set);
static char *get_fname (char *buf);
static char *no_path (char *buf);
static bool is_ffind_busy (void);

/**
  \brief       Initialize FTP server.
  \return      none.
*/
void net_ftp_server_init (void) {
  NET_FTP_INFO *ftp_s;
  int32_t  sock,i;

  DEBUGF (FTP,"Init %d Sessions, Port %d\n",ftpc->NumSess, ftpc->PortNum);
  EvrNetFTPs_InitServer (ftpc->NumSess, ftpc->PortNum, ftpc->IdleTout);
#ifdef DEBUG_STDIO
  if (ftpc->IdleTout) {
    DEBUGF (FTP," Idle timeout %d secs\n",ftpc->IdleTout);
  }
  else {
    DEBUGF (FTP," Connection permanent\n");
  }
#endif
#ifdef __DBG_ENABLED
  if (ftpc->Root) {
    DEBUGF (FTP," Root folder \"%s\"\n",ftpc->Root);
    EvrNetFTPs_ShowRootFolder (ftpc->Root, strlen(ftpc->Root));
  }
#endif
  for (i = 1, ftp_s = &ftpc->Scb[0]; i <= ftpc->NumSess; ftp_s++, i++) {
    ftp_s->Flags  = 0;
    ftp_s->Resp   = FTP_RESP_DONE;
    ftp_s->Path   = NULL;
    ftp_s->Name   = NULL;
    /* FTP control socket */
    sock = net_tcp_get_socket (ftp_listener);
    if (sock < 0) sock = 0;
    ftp_s->Socket = sock & 0xFF;
    /* FTP data socket */
    sock = net_tcp_get_socket (ftp_data_cb);
    if (sock < 0) sock = 0;
    ftp_s->DSocket = sock & 0xFF;
    ftp_s->State   = FTP_STATE_IDLE;
    if (ftp_s->Socket == 0 || ftp_s->DSocket == 0) {
      ftp_s->State = FTP_STATE_ERROR;
      ERRORF (FTP,"Session %d, Get socket failed\n",i);
      EvrNetFTPs_GetSocketFailed (i & 0xFF);
      net_sys_error (NET_ERROR_TCP_ALLOC);
      return;
    }
    ftp_s->Id = i & 0xFF;
  }

  /* Init service control */
  memset (ftp, 0, sizeof(*ftp));
  ftp->root     = ftpc->Root;
  ftp->port_num = ftpc->PortNum;
  if (ftp->port_num == 0) {
    /* Port not specified, use default server port */
    ftp->port_num = FTP_SERVER_PORT;
  }
  if (ftpc->EnAuth) {
    ftp->user      = ftpc->User;
    ftp->passw     = ftpc->Passw;
    ftp->user_len  = ftpc->UserLen;
    ftp->passw_len = ftpc->PasswLen;
    ftp->en_auth   = true;
  }
  if (sysc->AutoStart) {
    ftp_server_start ();
  }
}

/**
  \brief       De-initialize FTP server.
  \return      none.
*/
void net_ftp_server_uninit (void) {
  NET_FTP_INFO *ftp_s;
  int32_t i;

  DEBUGF (FTP,"Uninit Server\n");
  EvrNetFTPs_UninitServer ();

  for (i = 0, ftp_s = &ftpc->Scb[0]; i < ftpc->NumSess; ftp_s++, i++) {
    /* Close local file if open */
    close_file (ftp_s);
    /* Clear session control block */
    memset (ftp_s, 0, sizeof(*ftp_s));
  }

  /* Uninit service control */
  memset (ftp, 0, sizeof(*ftp));
}

/**
  \brief       Start FTP server service.
  \return      status code as defined with netStatus.
*/
netStatus netFTPs_Start (void) {

  START_LOCK (netStatus);

  if (!ftp->run_service) {
    /* Service is down */
    ftp_server_start ();
  }

  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Stop FTP server service.
  \return      status code as defined with netStatus.
*/
netStatus netFTPs_Stop (void) {

  START_LOCK (netStatus);

  if (ftp->run_service) {
    /* Service is running */
    ftp_server_stop ();
  }

  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Check if FTP server service is running.
  \return      service status:
               - true  = service is running.
               - false = service is not running.
*/
bool netFTPs_Running (void) {
  return (ftp->run_service);
}

/**
  \brief       Get current port number of FTP server.
  \return      current port number.
*/
uint16_t netFTPs_GetPort (void) {
  return (ftp->port_num);
}

/**
  \brief       Set FTP server port number.
  \param[in]   port  service port number.
  \return      status code as defined with netStatus.
*/
netStatus netFTPs_SetPort (uint16_t port) {
  if (ftp->run_service) {
    return (netWrongState);
  }
  ftp->port_num = port;
  return (netOK);
}

/**
  \brief       Retrieve current root folder path.
  \return      pointer to root folder path, a null-terminated string or
                 NULL if root folder is disabled.
*/
const char *netFTPs_GetRootPath (void) {
  return (ftp->root);
}

/**
  \brief       Change root folder path.
  \param[in]   path  new root folder path.
  \return      status code as defined with netStatus.
*/
netStatus netFTPs_SetRootPath (const char *path) {
  static char root_buf [NET_ROOT_DIR_LEN];
  int32_t i;

  START_LOCK (netStatus);

  if ((path == NULL) || strlen(path) >= NET_ROOT_DIR_LEN) {
    RETURN (netInvalidParameter);
  }
  if (!ftp->root) {
    /* Root folder not enabled for the service */
    RETURN (netError);
  }
  for (i = 0; i < ftpc->NumSess; i++) {
    if (ftpc->Scb[i].State >= FTP_STATE_COMMAND) {
      /* User logged in, root folder change not allowed */
      RETURN (netWrongState);
    }
  }
  DEBUGF (FTP,"SetRootFolder \"%s\"\n",path);
  EvrNetFTPs_SetRootPath (path, strlen(path));
  /* Use a static buffer instead of default const array */
  ftp->root = root_buf;
  net_strcpy (root_buf, path);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Retrieve username of system administrator account.
  \return      pointer to username, a null-terminated string or
                 NULL if authentication is disabled.
*/
const char *netFTPs_GetUsername (void) {
  return (ftp->user);
}

/**
  \brief       Set username of system administrator account.
  \param[in]   username  new username.
  \return      status code as defined with netStatus.
*/
netStatus netFTPs_SetUsername (const char *username) {
  static char user_buf [NET_USERNAME_LEN];

  START_LOCK (netStatus);

  if ((username == NULL) || strlen(username) >= NET_USERNAME_LEN) {
    RETURN (netInvalidParameter);
  }
  if (!ftp->user) {
    /* Administrator account not enabled */
    RETURN (netError);
  }
  DEBUGF (FTP,"SetUsername \"%s\"\n",username);
  EvrNetFTPs_SetUsername (username, strlen(username));
  /* Use a static buffer instead of default const array */
  ftp->user     = user_buf;
  ftp->user_len = net_strcpy (user_buf, username) & 0xFF;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Retrieve password of system administrator account.
  \return      pointer to password, a null-terminated string or
                 NULL if authentication is disabled.
*/
const char *netFTPs_GetPassword (void) {
  return (ftp->passw);
}

/**
  \brief       Set password of system administrator account.
  \param[in]   password  new password.
  \return      status code as defined with netStatus.
*/
netStatus netFTPs_SetPassword (const char *password) {
  static char passw_buf [NET_PASSWORD_LEN];

  START_LOCK (netStatus);

  if ((password == NULL) || strlen(password) >= NET_PASSWORD_LEN) {
    RETURN (netInvalidParameter);
  }
  if (!ftp->passw) {
    /* Administrator account not enabled */
    RETURN (netError);
  }
  DEBUGF (FTP,"SetPassword \"%s\"\n",password);
  EvrNetFTPs_SetPassword (password, strlen(password));
  /* Use a static buffer instead of default const array */
  ftp->passw     = passw_buf;
  ftp->passw_len = net_strcpy (passw_buf, password) & 0xFF;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Determine server authentication status.
  \return      status:
               - true  = authentication enabled,
               - false = authentication not enabled.
*/
bool netFTPs_LoginActive (void) {
  return (ftp->en_auth);
}

/**
  \brief       Enable or disable server authentication.
  \param[in]   enable  - true=  enable authentication,
                       - false= disable authentication.
  \return      status code as defined with netStatus.
*/
netStatus netFTPs_LoginOnOff (bool enable) {
  if (enable && !ftpc->EnAuth) {
    /* Authentication not enabled in the configuration */
    return (netError);
  }
  ftp->en_auth = enable;
  return (netOK);
}

/**
  \brief       Retrieve the user identification.
  \return      user identification number (0 = system administrator).
  \note        This function is called from FTP server user interface.
               The 'cmd.session' is set and valid for the call.
*/
uint8_t netFTPs_GetUserId (void) {
  return (ftpc->Scb[cmd.session-1].UserId);
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
static uint32_t ftp_listener (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                              const uint8_t *buf, uint32_t len) {
  NET_FTP_INFO *ftp_s;
  const char *name;
  int32_t i;

  /* Find the right session */
  ftp_s = ftp_map_session (socket);
  if (ftp_s == NULL) {
    return (false);
  }
  cmd.session = ftp_s->Id;

  switch (event) {
    case netTCP_EventConnect:
      /* Access client filtering by IP and/or port */
      if (netFTPs_AcceptClient (addr) == false) {
        ERRORF (FTP,"Access denied [%s], port %d\n",net_addr_ntoa((const __ADDR *)addr),addr->port);
        EvrNetFTPs_UserAccessDenied (addr);
        /* User denied access for this host */
        return (false);
      }
      DEBUGF (FTP,"Session %d open\n",ftp_s->Id);
      EvrNetFTPs_SessionOpen (ftp_s->Id);
      if (ftp_s->State == FTP_STATE_IDLE) {
        ftp_s->State = FTP_STATE_RESERVED;
      }
      break;

    case netTCP_EventAborted:
      DEBUGF (FTP,"Session %d abort, Socket aborted\n",ftp_s->Id);
      EvrNetFTPs_SocketAborted (ftp_s->Id);
      ftp_kill_session (ftp_s);
      break;

    case netTCP_EventEstablished:
      /* On connect send a server ready response */
      net_addr_copy (&ftp_s->Client, (const __ADDR *)addr);
      ftp_s->DPort  = FTP_DEF_DPORT;
      ftp_s->Flags |= FTP_FLAG_MODEASCII;
      ftp_s->Resp   = FTP_RESP_READY;
      ftp_s->State  = FTP_STATE_USER;
      ftp_s->UserId = 0;
      break;

    case netTCP_EventClosed:
      DEBUGF (FTP,"Session %d abort, Socket closed\n",ftp_s->Id);
      EvrNetFTPs_SocketClosed (ftp_s->Id);
      ftp_kill_session (ftp_s);
      break;

    case netTCP_EventACK:
      /* Our message is acked */
      ftp_s->Flags &= ~FTP_FLAG_WACK;
      break;

    case netTCP_EventData:
      DEBUGF (FTP,"*** Process_frame ***\n");
      /* Do we have not acknowledged data? */
      if (ftp_s->Flags & FTP_FLAG_WACK) {
        ERRORF (FTP,"Session %d receive, Unacked data\n",ftp_s->Id);
        EvrNetFTPs_UnackedDataError (ftp_s->Id);
        return (false);
      }
      DEBUGF (FTP," Received %d bytes, Session %d\n",len,ftp_s->Id);
      EvrNetFTPs_ReceiveFrame (ftp_s->Id, len);
      if(len < 3) {
        /* Data frame too short */
        ERRORF (FTP,"Session %d receive, Frame too short\n",ftp_s->Id);
        EvrNetFTPs_FrameTooShort (ftp_s->Id, len, 3);
        return (false);
      }

      /* Make a null-terminated string, strip off CR-LF */
      (__CONST_CAST(char *)buf)[len-2] = 0;
      DEBUGF (FTP," Command %s\n",buf);
      EvrNetFTPs_ShowCommand (buf, len-2);
      ftp_parse_cmd ((const char *)buf);

      /* Process command from the client */
      switch (ftp_s->State) {
        case FTP_STATE_USER:
          /* Username required */
          if (cmd.sel == FTP_CMD_USER) {
            DEBUGF (FTP," Verifying Username\n");
            EvrNetFTPs_VerifyUsername (ftp_s->Id);
            if (ftp->en_auth) {
              /* Check username of a system admin account */
              if (ftp->user && ftp->user_len &&
                  strcmp ((const char *)&buf[5], ftp->user) == 0) {
                ftp_s->Flags |= FTP_FLAG_USEROK;
              }
              /* Check username of optional external accounts */
              else if ((i = netFTPs_CheckUsername ((const char *)&buf[5])) > 0) {
                /* Username is valid, register user id */
                ftp_s->UserId = i & 0xFF;
                ftp_s->Flags |= FTP_FLAG_USEROK;
              }
            }
            ftp_s->Resp  = FTP_RESP_PASSWREQ;
            ftp_s->State = FTP_STATE_PASSW;
            break;
          }
          if (cmd.sel == FTP_CMD_PASS) {
            ftp_s->Resp = FTP_RESP_USERFIRST;
            break;
          }
          if (cmd.sel == FTP_CMD_QUIT) {
            goto quit;
          }
          goto not_logd;

        case FTP_STATE_PASSW:
          /* Password required */
          if (cmd.sel == FTP_CMD_PASS) {
            if (ftp->en_auth) {
              DEBUGF (FTP," Verifying Password\n");
              EvrNetFTPs_VerifyPassword (ftp_s->Id);
              if ((ftp_s->Flags & FTP_FLAG_USEROK) == 0) {
err_auth:       ERRORF (FTP,"Session %d, Login failed\n",ftp_s->Id);
                EvrNetFTPs_AuthenticationFailed (ftp_s->Id);
                ftp_s->Resp  = FTP_RESP_LOGINFAIL;
                ftp_s->State = FTP_STATE_USER;
                break;
              }
              if (ftp_s->UserId == 0) {
                /* Verify password for system administrator */
                if (ftp->passw && ftp->passw_len &&
                    strcmp ((const char *)&buf[5],ftp->passw) != 0) {
                  goto err_auth;
                }
              }
              else {
                /* Verify password for external user */
                if (!netFTPs_CheckPassword (ftp_s->UserId, (const char *)&buf[5])) {
                  goto err_auth;
                }
              }
            }
            DEBUGF (FTP," Login success\n");
            EvrNetFTPs_UserLoginSuccess (ftp_s->Id);
            netFTPs_Notify (netFTPs_EventLogin);
            ftp_s->Flags |= FTP_FLAG_LOGIN;
            ftp_s->Path = (char *)net_mem_alloc (FTP_MAX_PATH+4);
            ftp_s->Path[0] = '/';
            ftp_s->Path[1] = 0;
            ftp_s->PathLen = 1;
            ftp_s->Resp  = FTP_RESP_LOGINOK;
            ftp_s->State = FTP_STATE_COMMAND;
            break;
          }
          if (cmd.sel == FTP_CMD_QUIT) {
            goto quit;
          }
not_logd: DEBUGF (FTP," Not logged in, command ignored\n");
          EvrNetFTPs_NotAuthenticated (ftp_s->Id);
          ftp_s->Resp  = FTP_RESP_NOTLOGGED;
          ftp_s->State = FTP_STATE_USER;
          break;

        case FTP_STATE_COMMAND:
          /* User logged in, process user commands */
          if (cmd.sel == FTP_CMD_SYST) {
            /* Operating system type of FTP server */
            DEBUGF (FTP," System is Windows_NT\n");
            EvrNetFTPs_ShowSystemType (ftp_s->Id);
            ftp_s->Resp = FTP_RESP_SYSTEM;
            break;
          }
          if (cmd.sel == FTP_CMD_NOOP) {
            /* No operation command */
            DEBUGF (FTP," No operation\n");
            EvrNetFTPs_NoOperation (ftp_s->Id);
            ftp_s->Resp = FTP_RESP_CMDOK;
            break;
          }
          if (cmd.sel == FTP_CMD_HELP) {
            /* Print supported commands */
            ftp_s->Resp = FTP_RESP_HELP;
            break;
          }
          if (cmd.sel == FTP_CMD_FEAT) {
            /* Print supported extended features */
            ftp_s->Resp = FTP_RESP_FEAT;
            break;
          }
          if (cmd.sel == FTP_CMD_PWD) {
            /* Print the working directory */
            DEBUGF (FTP," Current directory is \"%s\"\n",ftp_s->Path);
            EvrNetFTPs_CurrentDirectory (ftp_s->Path, ftp_s->PathLen);
            ftp_s->Resp = FTP_RESP_CURDIR;
            break;
          }
          if (cmd.sel == FTP_CMD_CWD) {
            /* Change the working directory */
            name = get_fname (__CONST_CAST(char *)&buf[4]);
            DEBUGF (FTP," Change directory to \"%s\"\n",name);
            DEBUGF (FTP," Path: %s\n",ftp_s->Path);
            EvrNetFTPs_ChangeDirectory (name, strlen(name));
            ftp_s->Name = make_path (ftp_s, name);
            if (netFTPs_chdir (ftp_s->Name) == false) {
              /* Directory not existing or not supported */
              ERRORF (FTP,"Session %d, Change directory failed\n",ftp_s->Id);
              EvrNetFTPs_ChangeDirectoryFailed (ftp_s->Id);
              ftp_s->Resp = FTP_RESP_CWDERR;
              break;
            }
            store_path (ftp_s, name);
            ftp_s->Resp = FTP_RESP_FILECMDOK;
            break;
          }
          if (cmd.sel == FTP_CMD_CDUP) {
            /* Change working directory to the parent of current directory */
            path_dir_up (ftp_s);
            DEBUGF (FTP," Change to parent directory\n");
            DEBUGF (FTP," Path: %s\n",ftp_s->Path);
            EvrNetFTPs_ChangeDirectoryLevelUp (ftp_s->Id);
            ftp_s->Resp = FTP_RESP_CMDOK;
            break;
          }
          if (cmd.sel == FTP_CMD_MKD) {
            /* Create a directory */
            name = get_fname (__CONST_CAST(char *)&buf[4]);
            DEBUGF (FTP," Make directory \"%s\"\n",name);
            EvrNetFTPs_MakeDirectory (name, strlen(name));
            /* Make an absolute path */
            ftp_s->Name = make_path (ftp_s, name);
            /* Check access rights for external user accounts */
            if (ftp_s->UserId &&
                !netFTPs_FileAccess (ftp_s->UserId, ftp_s->Name, NET_ACCESS_DIRECTORY_CREATE)) {
denied:       DEBUGF (FTP," Access denied\n");
              EvrNetFTPs_OperationDenied (ftp_s->Id);
              netFTPs_Notify (netFTPs_EventOperationDenied);
              ftp_s->Resp = FTP_RESP_DENIED;
              break;
            }
            ftp_s->State = FTP_STATE_MKDIR;
            return (true);
          }
          if (cmd.sel == FTP_CMD_RMD) {
            /* Remove a directory */
            name = get_fname (__CONST_CAST(char *)&buf[4]);
            DEBUGF (FTP," Remove directory \"%s\"\n",name);
            EvrNetFTPs_RemoveDirectory (name, strlen(name));
            ftp_s->Name = make_path (ftp_s, name);
            /* Check access rights for external user accounts */
            if (ftp_s->UserId &&
                !netFTPs_FileAccess (ftp_s->UserId, ftp_s->Name, NET_ACCESS_DIRECTORY_REMOVE)) {
              goto denied;
            }
            if (netFTPs_rmdir (net_path(ftp->root, ftp_s->Name)) == false) {
              /* Failed to remove a folder */
              ERRORF (FTP,"Session %d, Remove directory failed\n",ftp_s->Id);
              EvrNetFTPs_RemoveDirectoryFailed (ftp_s->Id);
              ftp_s->Resp = FTP_RESP_DENIED;
              break;
            }
            DEBUGF (FTP," Directory removed\n");
            EvrNetFTPs_DirectoryRemoved (ftp_s->Id);
            netFTPs_Notify (netFTPs_EventRemoveDirectory);
            ftp_s->Resp = FTP_RESP_FILECMDOK;
            break;
          }
          if (cmd.sel == FTP_CMD_TYPE) {
            /* Set data type */
            switch (buf[5]) {
              case 'A':
                /* Data type is ASCII */
                ftp_s->Flags |= FTP_FLAG_MODEASCII;
                break;
              case 'I':
                /* Data type is Image (binary) */
                ftp_s->Flags &= ~FTP_FLAG_MODEASCII;
                break;
              default:
                goto unknown;
            }
            DEBUGF (FTP," Data mode is %s\n",(ftp_s->Flags & FTP_FLAG_MODEASCII) ?
                                             "ASCII" : "Binary");
            EvrNetFTPs_ShowDataMode (ftp_s->Id, (ftp_s->Flags & FTP_FLAG_MODEASCII) ? 1 : 0);
            ftp_s->Resp = FTP_RESP_CMDOK;
            break;
          }
          if ((cmd.sel == FTP_CMD_PORT) ||
              (cmd.sel == FTP_CMD_EPRT)) {
            /* Change FTP data port */
            net_tcp_abort (ftp_s->DSocket);
            ftp_s->DPort = ftp_scan_dport ((const char *)&buf[5], cmd.sel);
            DEBUGF (FTP," Data port is %d\n",ftp_s->DPort);
            EvrNetFTPs_ActiveModeStart (ftp_s->Id, ftp_s->DPort);
            ftp_s->Flags &= ~FTP_FLAG_PASSIVE;
            ftp_s->Resp   = FTP_RESP_CMDOK;
            break;
          }
          if ((cmd.sel == FTP_CMD_PASV) ||
              (cmd.sel == FTP_CMD_EPSV)) {
            /* Change to passive mode */
            net_tcp_abort (ftp_s->DSocket);
            ftp_s->DPort = net_tcp_alloc_port ();
            DEBUGF (FTP," Passive mode, listen port %d\n",ftp_s->DPort);
            EvrNetFTPs_PassiveModeStart (ftp_s->Id, ftp_s->DPort);
            ftp_s->Flags |= FTP_FLAG_PASSIVE;
            if (cmd.sel == FTP_CMD_PASV) {
              /* Standard passive mode */
              ftp_s->Resp = FTP_RESP_PASVOK;
              break;
            }
            /* Extended passive mode */
            ftp_s->Resp = FTP_RESP_EPSVOK;
            break;
          }
          if (cmd.sel == FTP_CMD_SIZE) {
            /* Get the size of a file */
            name = get_fname (__CONST_CAST(char *)&buf[5]);
            DEBUGF (FTP," Get size of file \"%s\"\n",name);
            EvrNetFTPs_GetFileSize (name, strlen(name));
            ftp_s->State = FTP_STATE_FSIZE;
            goto fname;
          }
          if (cmd.sel == FTP_CMD_MDTM) {
            /* Get last-modified time of a file */
            name = get_fname (__CONST_CAST(char *)&buf[5]);
            DEBUGF (FTP," Get lm-time of file \"%s\"\n",name);
            EvrNetFTPs_GetFileLastModifiedTime (name, strlen(name));
            ftp_s->State = FTP_STATE_FTIME;
fname:      ftp_s->Name  = make_path (ftp_s, name);
            /* Check access rights for external user accounts */
            if (ftp_s->UserId &&
                !netFTPs_FileAccess (ftp_s->UserId, ftp_s->Name, NET_ACCESS_DIRECTORY_LIST)) {
              ftp_s->State = FTP_STATE_COMMAND;
              goto denied;
            }
            return (true);
          }
          if (cmd.sel == FTP_CMD_NLST) {
            /* List file directory in basic format */
            DEBUGF (FTP," List file names only\n");
            EvrNetFTPs_ListDirectoryBasic (ftp_s->Id);
            ftp_s->Flags |= FTP_FLAG_NAMELST;
            goto list;
          }
          if (cmd.sel == FTP_CMD_LIST) {
            /* List file directory in extended format */
            DEBUGF (FTP," List file directory\n");
            EvrNetFTPs_ListDirectoryExtended (ftp_s->Id);
            ftp_s->Flags &= ~FTP_FLAG_NAMELST;
list:       /* Check access rights for external user accounts */
            if (ftp_s->UserId &&
                !netFTPs_FileAccess (ftp_s->UserId, "*", NET_ACCESS_DIRECTORY_LIST)) {
              goto denied;
            }
            ftp_s->State = FTP_STATE_LIST;
            goto dopen;
          }
          if (cmd.sel == FTP_CMD_RETR) {
            /* Retrieve a file */
            name = get_fname (__CONST_CAST(char *)&buf[5]);
            DEBUGF (FTP," Read file \"%s\"\n",name);
            EvrNetFTPs_ReadFile (name, strlen(name));
            ftp_s->Name = make_path (ftp_s, name);
            /* Check access rights for external user accounts */
            if (ftp_s->UserId &&
                !netFTPs_FileAccess (ftp_s->UserId, ftp_s->Name, NET_ACCESS_FILE_READ)) {
              goto denied;
            }
            ftp_s->File = netFTPs_fopen (net_path(ftp->root,ftp_s->Name), "rb");
            if (ftp_s->File == NULL) {
              ERRORF (FTP,"Session %d, File not found\n",ftp_s->Id);
              EvrNetFTPs_FileNotFound (ftp_s->Id);
              ftp_s->Resp = FTP_RESP_NOTFOUND;
              break;
            }
            ftp_s->State = FTP_STATE_RDFILE;
            goto dopen;
          }
          if ((cmd.sel == FTP_CMD_STOR) ||
              (cmd.sel == FTP_CMD_APPE)) {
            /* Store or Append a file (with create) */
            name = get_fname (__CONST_CAST(char *)&buf[5]);
#ifdef __DBG_ENABLED
            if (cmd.sel == FTP_CMD_STOR) {
              DEBUGF (FTP, " Write file \"%s\"\n", name);
              EvrNetFTPs_WriteFile (name, strlen(name));
            }
            else {
              DEBUGF (FTP, " Append file \"%s\"\n", name);
              EvrNetFTPs_AppendFile (name, strlen(name));
            }
#endif
            ftp_s->Name = make_path (ftp_s, name);
            /* Check access rights for external user accounts */
            if (ftp_s->UserId &&
                !netFTPs_FileAccess (ftp_s->UserId, ftp_s->Name, NET_ACCESS_FILE_WRITE)) {
              goto denied;
            }
            ftp_s->File = netFTPs_fopen (net_path(ftp->root,ftp_s->Name),
                                         ((cmd.sel == FTP_CMD_STOR) ? "wb" : "ab"));
            if (ftp_s->File == NULL) {
              /* Failed, can not open file for writing */
              ERRORF (FTP,"Session %d, File create failed\n",ftp_s->Id);
              EvrNetFTPs_FileCreateFailed (ftp_s->Id);
              ftp_s->Resp = FTP_RESP_CREATERR;
              break;
            }
            ftp_s->State = FTP_STATE_WRFILE;
dopen:      ftp_s->Resp  = (ftp_s->Flags & FTP_FLAG_PASSIVE) ? FTP_RESP_DACCEPT :
                                                               FTP_RESP_DOPEN;
            break;
          }
          if (cmd.sel == FTP_CMD_DELE) {
            /* Delete a file */
            name = get_fname (__CONST_CAST(char *)&buf[5]);
            DEBUGF (FTP," Delete file \"%s\"\n",name);
            EvrNetFTPs_DeleteFile (name, strlen(name));
            ftp_s->Name = make_path (ftp_s, name);
            /* Check access rights for external user accounts */
            if (ftp_s->UserId &&
                !netFTPs_FileAccess (ftp_s->UserId, ftp_s->Name, NET_ACCESS_FILE_WRITE)) {
              goto denied;
            }
            if (netFTPs_fdelete (net_path(ftp->root,ftp_s->Name)) == false) {
              /* Failed to delete a file */
              ERRORF (FTP,"Session %d, File delete failed\n",ftp_s->Id);
              netFTPs_Notify (netFTPs_EventLocalFileError);
              EvrNetFTPs_FileDeleteFailed (ftp_s->Id);
              ftp_s->Resp = FTP_RESP_NOTFOUND;
              break;
            }
            DEBUGF (FTP," File deleted\n");
            EvrNetFTPs_FileDeleted (ftp_s->Id);
            netFTPs_Notify (netFTPs_EventDelete);
            ftp_s->Resp = FTP_RESP_FILECMDOK;
            break;
          }

          if (cmd.sel == FTP_CMD_RNFR) {
            /* Rename a file/folder From 'name' */
            name = get_fname (__CONST_CAST(char *)&buf[5]);
            DEBUGF (FTP," Rename file from \"%s\"\n",name);
            EvrNetFTPs_RenameFileFrom (name, strlen(name));
            free_name (ftp_s);
            ftp_s->Name = make_path (ftp_s, name);
            /* Check access rights for external user accounts */
            if (ftp_s->UserId &&
                !netFTPs_FileAccess (ftp_s->UserId, ftp_s->Name, NET_ACCESS_FILE_WRITE)) {
              goto denied;
            }
            ftp_s->Resp = FTP_RESP_PENDTO;
            return (true);
          }

          if (cmd.sel == FTP_CMD_RNTO) {
            /* Rename a file/folder to 'name' */
            name = no_path (__CONST_CAST(char *)&buf[5]);
            DEBUGF (FTP," Rename file to \"%s\"\n",name);
            EvrNetFTPs_RenameFileTo (name, strlen(name));
            if (ftp_s->Name == NULL) {
              /* Invalid command sequence */
              ftp_s->Resp = FTP_RESP_NOTFOUND;
              break;
            }
            if (netFTPs_frename (net_path(ftp->root,ftp_s->Name),name) == true) {
              DEBUGF (FTP," File renamed\n");
              EvrNetFTPs_FileRenamed (ftp_s->Id);
              netFTPs_Notify (netFTPs_EventRename);
              ftp_s->Resp = FTP_RESP_FILECMDOK;
              break;
            }
            /* Failed to rename a file/folder */
            ERRORF (FTP,"Session %d, File rename failed\n",ftp_s->Id);
            EvrNetFTPs_FileRenameFailed (ftp_s->Id);
            netFTPs_Notify (netFTPs_EventLocalFileError);
            ftp_s->Resp = FTP_RESP_NOTFOUND;
            break;
          }

          if (cmd.sel == FTP_CMD_QUIT) {
quit:       ftp_s->Resp  = FTP_RESP_QUIT;
            ftp_s->State = FTP_STATE_QUIT;
            break;
          }
unknown:  ERRORF (FTP,"Session %d, Unknown command\n",ftp_s->Id);
          EvrNetFTPs_UnknownCommand (ftp_s->Id);
          ftp_s->Resp = FTP_RESP_ERROR;
          break;

        default:
          ERRORF (FTP,"Session %d wrong state, Command ignored\n",ftp_s->Id);
          EvrNetFTPs_InvalidState (ftp_s->Id);
          break;
      }
      /* End switch (ftp_s->State) */
      free_name (ftp_s);
      break;
  }
  return (true);
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
static uint32_t ftp_data_cb (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                             const uint8_t *buf, uint32_t len) {
  NET_FTP_INFO *ftp_s;

  (void)addr;
  /* Find the right session */
  ftp_s = ftp_map_session (socket);
  if (ftp_s == NULL) {
    return (false);
  }
  cmd.session = ftp_s->Id;

  switch (event) {
    case netTCP_EventConnect:
      if (ftp_s->Flags & FTP_FLAG_PASSIVE) {
        /* Accept data connections in passive mode only */
        break;
      }
      ERRORF (FTP,"Session %d not passive, Inbound conn.rejected\n",ftp_s->Id);
      EvrNetFTPs_InboundConnRejected (ftp_s->Id);
      return (false);

    case netTCP_EventAborted:
    case netTCP_EventClosed:
      DEBUGF (FTP,"Session %d, Data socket close\n",ftp_s->Id);
      EvrNetFTPs_DataSocketClosed (ftp_s->Id);
      ftp_s->Flags &= ~(FTP_FLAG_DOPEN | FTP_FLAG_PASSIVE);
      if (ftp_s->State == FTP_STATE_WRFILE) {
        /* File is open for writing */
        if (ftp_s->Flags & FTP_FLAG_ERROR) {
          /* Let the polling function handle this */
          break;
        }
        /* No error, stop transfer and close file */
        close_file (ftp_s);
        ftp_s->Resp  = FTP_RESP_DCLOSE;
        netFTPs_Notify (netFTPs_EventUpload);
        ftp_s->State = FTP_STATE_COMMAND;
        break;
      }
      if (ftp_s->State == FTP_STATE_RDFILE) {
        /* File is open for reading */
        close_file (ftp_s);
        ftp_s->Resp = FTP_RESP_ABORTED;
        netFTPs_Notify (netFTPs_EventError);
        ftp_s->State = FTP_STATE_COMMAND;
      }
      break;

    case netTCP_EventEstablished:
      DEBUGF (FTP,"Session %d, Data socket open\n",ftp_s->Id);
      EvrNetFTPs_DataSocketOpen (ftp_s->Id);
      ftp_s->Flags |= FTP_FLAG_DOPEN;
      break;

    case netTCP_EventACK:
      /* Our message is acked */
      ftp_s->Flags &= ~FTP_FLAG_DACK;
      net_tcp_reset_tout (ftp_s->Socket);
      break;

    case netTCP_EventData:
      /* File data received */
      if (ftp_s->State != FTP_STATE_WRFILE) {
        /* Wrong FTP state, ignore data */
        break;
      }
      if ((ftp_s->File == NULL) || (ftp_s->Flags & FTP_FLAG_ERROR)) {
        /* File not opened or write error, ignore data */
        break;
      }
      /* File content received, accepts also early data */
      if (netFTPs_fwrite (ftp_s->File, buf, len) != len) {
        /* Write failure, close the connection */
        ERRORF (FTP,"Session %d, Local disk full\n");
        EvrNetFTPs_LocalDiskWriteError (ftp_s->Id);
        ftp_s->Flags |= FTP_FLAG_ERROR;
        break;
      }
      net_tcp_reset_tout (ftp_s->Socket);
      break;
  }
  return (true);
}

/**
  \brief       Main FTP server scheduler.
  \return      none.
*/
void net_ftp_server_run (void) {
  if (ftp->run_service) {
    ftp_server_run ();
  }
}

/**
  \brief       Start FTP server service.
  \return      none.
*/
static void ftp_server_start (void) {
  NET_FTP_INFO *ftp_s;
  int32_t i;

  DEBUGF (FTP,"Start service, port %d\n",ftp->port_num);
  EvrNetFTPs_StartService (ftp->port_num);
  for (i = 1, ftp_s = &ftpc->Scb[0]; i <= ftpc->NumSess; ftp_s++, i++) {
    if (ftp_s->State != FTP_STATE_ERROR) {
      net_tcp_listen (ftp_s->Socket, ftp->port_num);
      if (ftpc->IdleTout != 0) {
        /* Setup idle disconnect on timeout*/
        net_tcp_set_option (ftp_s->Socket, netTCP_OptionTimeout, ftpc->IdleTout);
      }
      else {
        /* Idle timeout == 0, activate TCP keep-alive mode */
        net_tcp_set_option (ftp_s->Socket, netTCP_OptionKeepAlive, 1);
      }
      net_tcp_set_option (ftp_s->DSocket, netTCP_OptionDelayedACK, 1);
      ftp_s->State = FTP_STATE_IDLE;
    }
  }
  ftp->run_service = true;
}

/**
  \brief       Stop FTP server service.
  \return      none.
*/
static void ftp_server_stop (void) {
  NET_FTP_INFO *ftp_s;
  int32_t i;

  DEBUGF (FTP,"Stop service\n");
  EvrNetFTPs_StopService ();
  for (i = 1, ftp_s = &ftpc->Scb[0]; i <= ftpc->NumSess; ftp_s++, i++) {
    if (ftp_s->State != FTP_STATE_ERROR) {
      net_tcp_abort (ftp_s->Socket);
      net_tcp_abort (ftp_s->DSocket);
      ftp_kill_session (ftp_s);
      ftp_s->Flags = 0;
      ftp_s->State = FTP_STATE_IDLE;
    }
  }
  ftp->run_service = false;
}

/**
  \brief       Run FTP server service.
  \return      none.
*/
static void ftp_server_run (void) {
  static uint8_t session = 0;
  NET_FTP_INFO *ftp_s;
  uint8_t *sendbuf;
  const char *tp,*path;
  uint32_t max_dsize = 0;
  int32_t i,len;
  NET_FS_TIME ftime;
  uint32_t fsize;

  /* Check FTP sessions if there is something to do */
  for (i = 0; i < ftpc->NumSess; i++) {
    if (++session > ftpc->NumSess) {
      session = 1;
    }
    cmd.session = session;
    ftp_s = &ftpc->Scb[session-1];
    if (ftp_s->State == FTP_STATE_IDLE     ||
        ftp_s->State == FTP_STATE_RESERVED) {
      /* Keep TCP sockets listening */
      if (net_tcp_get_state (ftp_s->Socket) < netTCP_StateLISTEN) {
        /* This socket is not listening, activate listening again */
        net_tcp_listen (ftp_s->Socket, ftp->port_num);
      }
      continue;
    }
    if (ftp_s->Flags & FTP_FLAG_WACK) {
      /* Data not acknowledged yet */
      continue;
    }
    if (ftp_s->Resp != FTP_RESP_DONE) {
      /* Send a server response to the client */
      switch (ftp_s->Resp) {
        case FTP_RESP_DOPEN:
          len = LSTR(tp,"150 Opening data connection\r\n");
          break;
        case FTP_RESP_DACCEPT:
          if (ftp_s->Flags & FTP_FLAG_DOPEN) {
            len = LSTR(tp,"125 Data connection already open\r\n");
            break;
          }
          len = LSTR(tp,"150 Accepted data connection\r\n");
          break;
        case FTP_RESP_HELP:
          len = LSTR(tp,"214-The following commands are recognized:\r\n"
                        " APPE\r\n"
                        " CDUP\r\n"
                        " CWD\r\n"
                        " DELE\r\n"
                        " HELP\r\n"
                        " LIST\r\n"
                        " MDTM\r\n"
                        " MKD\r\n"
                        " NLST\r\n"
                        " NOOP\r\n"
                        " PASS\r\n"
                        " PASV\r\n"
                        " PORT\r\n"
                        " PWD\r\n"
                        " QUIT\r\n"
                        " RETR\r\n"
                        " RMD\r\n"
                        " RNFR\r\n"
                        " RNTO\r\n"
                        " SIZE\r\n"
                        " STOR\r\n"
                        " SYST\r\n"
                        " TYPE\r\n"
                        " USER\r\n"
                        " XCUP\r\n"
                        " XCWD\r\n"
                        " XPWD\r\n"
                        " XMKD\r\n"
                        " XNOP\r\n"
                        " XRMD\r\n"
                        " EPSV\r\n"
                        " EPRT\r\n"
                        " FEAT\r\n"
                        "214 HELP command successful.\r\n");
          break;
        case FTP_RESP_FEAT:
          len = LSTR(tp,"211-Extensions supported:\r\n"
                        " MDTM\r\n"
                        " SIZE\r\n"
                        " EPSV\r\n"
                        " EPRT\r\n"
                        "211 End.\r\n");
          break;
        case FTP_RESP_CMDOK:
          len = LSTR(tp,"200 Command successful\r\n");
          break;
        case FTP_RESP_SYSTEM:
          len = LSTR(tp,"215 Windows_NT\r\n");
          break;
        case FTP_RESP_READY:
          if (ftpc->MsgLen != 0) {
            /* Custom welcome message */
            len = LSTR(tp,"220 %s\r\n");
            /* Add space for the message */
            len += ftpc->MsgLen;
            break;
          }
          len = LSTR(tp,"220 Keil FTP service\r\n");
          break;
        case FTP_RESP_QUIT:
          len = LSTR(tp,"221 Goodbye\r\n");
          break;
        case FTP_RESP_DCLOSE:
          len = LSTR(tp,"226 Transfer complete\r\n");
          break;
        case FTP_RESP_PASVOK:
          len = LSTR(tp,"227 Entering Passive mode ");
          /* Add space for IP address and port number */
          len += 30;
          break;
        case FTP_RESP_EPSVOK:
          len = LSTR(tp,"229 Entering Extended Passive mode ");
          /* Add space for port number */
          len += 15;
          break;
        case FTP_RESP_LOGINOK:
          len = LSTR(tp,"230 User logged in\r\n");
          break;
        case FTP_RESP_FILECMDOK:
          len = LSTR(tp,"250 File command completed\r\n");
          break;
        case FTP_RESP_CURDIR:
          len = LSTR(tp,"257 \"%s\" is the current directory\r\n");
          /* Add space for the path */
          len += ftp_s->PathLen;
          break;
        case FTP_RESP_MKDIROK:
          len = LSTR(tp,"257 Directory created\r\n");
          /* Add space for the path */
          len += ftp_s->PathLen;
          break;
        case FTP_RESP_PASSWREQ:
          len = LSTR(tp,"331 Password required\r\n");
          break;
        case FTP_RESP_PENDTO:
          len = LSTR(tp,"350 Ready for destination name\r\n");
          break;
        case FTP_RESP_CANTOPEN:
          len = LSTR(tp,"425 Can't open data connection\r\n");
          break;
        case FTP_RESP_ABORTED:
          len = LSTR(tp,"426 Transfer aborted\r\n");
          break;
        case FTP_RESP_USERFIRST:
          len = LSTR(tp,"503 Login with USER first\r\n");
          break;
        case FTP_RESP_LOGINFAIL:
          len = LSTR(tp,"530 Login incorrect\r\n");
          break;
        case FTP_RESP_NOTLOGGED:
          len = LSTR(tp,"530 Not logged in\r\n");
          break;
        case FTP_RESP_NOTFOUND:
          len = LSTR(tp,"550 File not found\r\n");
          break;
        case FTP_RESP_DENIED:
          len = LSTR(tp,"550 Access is denied\r\n");
          break;
        case FTP_RESP_CWDERR:
          len = LSTR(tp,"550 Change directory failed\r\n");
          break;
        case FTP_RESP_CREATERR:
          len = LSTR(tp,"552 Create file failed\r\n");
          break;
        case FTP_RESP_DISKFULL:
          len = LSTR(tp,"552 Exceeded storage allocation\r\n");
          break;
        default:
          len = LSTR(tp,"500 Unknown command\r\n");
          break;
      }
      sendbuf = net_tcp_get_buf ((uint32_t)len);
      if (ftp_s->Resp == FTP_RESP_CURDIR) {
        len  = net_strncpy((char *)&sendbuf[0],   tp, 5);
        len += net_strcpy ((char *)&sendbuf[len], ftp_s->Path);
        len += net_strcpy ((char *)&sendbuf[len], &tp[7]);
      }
      else if (ftp_s->Resp == FTP_RESP_READY) {
        if (ftpc->MsgLen != 0) {
          /* Custom welcome message */
          len  = net_strncpy((char *)&sendbuf[0],   tp, 4);
          len += net_strcpy ((char *)&sendbuf[len], ftpc->Msg);
          len += net_strcpy ((char *)&sendbuf[len], &tp[6]);
        }
        else {
          len  = net_strcpy ((char *)&sendbuf[0],   tp);
        }
      }
      else {
        len = net_strcpy ((char *)sendbuf, tp);
        if (ftp_s->Resp == FTP_RESP_PASVOK) {
          /* PASV: add local IP address and listening port number */
          /* Format: (%d,%d,%d,%d,%d,%d)\r\n"                     */
          int32_t ii;
          const uint8_t *ip = net_tcp_get_local_addr (ftp_s->Socket);
          sendbuf[len++] = '(';
          ii = len;
          len += net_strcpy ((char *)&sendbuf[len], net_addr4_ntoa (ip));
          /* Replace '.' with ',' */
          for ( ; ii < len; ii++) {
            if (sendbuf[ii] == '.') sendbuf[ii] = ',';
          }
          sendbuf[len++] = ',';
          len += net_itoa  ((char *)&sendbuf[len], HI_BYTE(ftp_s->DPort));
          sendbuf[len++] = ',';
          len += net_itoa  ((char *)&sendbuf[len], LO_BYTE(ftp_s->DPort));
          len += net_strcpy((char *)&sendbuf[len], ")\r\n");
        }
        else if (ftp_s->Resp == FTP_RESP_EPSVOK) {
          /* EPSV: add listening port number only */
          /* Format: "(|||%d|)\r\n"               */
          len += net_strcpy ((char *)&sendbuf[len], "(|||");
          len += net_itoa   ((char *)&sendbuf[len], ftp_s->DPort);
          len += net_strcpy ((char *)&sendbuf[len], "|)\r\n");
        }
      }
      net_tcp_send (ftp_s->Socket, sendbuf, (uint32_t)len);
      ftp_s->Flags |= FTP_FLAG_WACK;
      if (ftp_dopen_req (ftp_s) != netOK) {
        /* Failed to open a data connection */
        ftp_s->Resp  = FTP_RESP_CANTOPEN;
        ftp_s->State = FTP_STATE_COMMAND;
        return;
      }
      ftp_s->Resp = FTP_RESP_DONE;
      return;
    }

    if (ftp_s->Flags & FTP_FLAG_DACK) {
      /* No acknowledge from data socket yet */
      continue;
    }

    switch (ftp_s->State) {
      case FTP_STATE_LIST:
        /* LIST command, send data */
        if ((ftp_s->Flags & FTP_FLAG_DOPEN) == 0) {
          /* Wait for data socket to connect first */
          break;
        }
        if ((ftp_s->Flags & FTP_FLAG_LSTLOCK) == 0) {
          if (is_ffind_busy ()) {
            /* Wait until ffind() function unlocked */
            break;
          }
          /* First call to ffind() */
          set_mask (ftp_s, true);
          DEBUGF (FTP," Find mask: %s\n",ftp_s->Path);
          EvrNetFTPs_ShowFileFindMask (ftp_s->Path, strlen(ftp_s->Path));
          ftp_s->Flags |= (FTP_FLAG_LSTLOCK | FTP_FLAG_FIRST);
        }

        /* Allocate transmit frame buffer */
        sendbuf = NULL;
        if (net_mem_avail_tx()) {
          max_dsize = net_tcp_get_mss (ftp_s->DSocket);
          sendbuf   = net_tcp_get_buf (max_dsize | 0x80000000);
        }
        if (sendbuf == NULL) {
          /* Wait, no memory available */
          break;
        }
        path = net_path (ftp->root, ftp_s->Path);
        for (len = 0; len < (int32_t)(max_dsize - 255); ) {
          int32_t retv, nidx = (ftp_s->Flags & FTP_FLAG_NAMELST) ? len : len + 39;
          retv = netFTPs_ffind (path, (char *)&sendbuf[nidx], &fsize, &ftime,
                                (ftp_s->Flags & FTP_FLAG_FIRST) ? true : false);
          if (retv == 0) {
            /* No more entries, LIST completed */
            set_mask (ftp_s, false);
            ftp_s->Flags &= ~FTP_FLAG_LSTLOCK;
            ftp_s->State  = FTP_STATE_DCLOSE;
            break;
          }
          ftp_s->Flags &= ~FTP_FLAG_FIRST;
          if ((ftp_s->Flags & FTP_FLAG_NAMELST) == 0) {
            /* Extended LIST, add date info (8 bytes) */
            /* Format: "%02d-%02d-%02d  %02d:%02d%s"  */
            net_itoa_n0 ((char *)&sendbuf[len],   ftime.mon, 2);
            sendbuf[len+2] = '-';
            net_itoa_n0 ((char *)&sendbuf[len+3], ftime.day, 2);
            sendbuf[len+5] = '-';
            net_itoa_n0 ((char *)&sendbuf[len+6], ftime.year%100, 2);
            /* Add time info, converted to "AM/PM" format (9 bytes) */
            tp = (ftime.hr < 12) ? "AM" : "PM";
            if ((ftime.hr %= 12) == 0) ftime.hr = 12;
            sendbuf[len+8] = ' ';
            sendbuf[len+9] = ' ';
            net_itoa_n0 ((char *)&sendbuf[len+10], ftime.hr,  2);
            sendbuf[len+12] = ':';
            net_itoa_n0 ((char *)&sendbuf[len+13], ftime.min, 2);
            net_strcpy  ((char *)&sendbuf[len+15], tp);
            /* Add file size or directory info (21 bytes) */
            if (retv == NET_FS_ATTR_DIRECTORY) {
              net_strcpy   ((char *)&sendbuf[len+17], "       <DIR>         ");
            }
            else {
              net_ultoa_n  ((char *)&sendbuf[len+17], fsize, 21);
            }
            /* Add space (1 byte) */
            sendbuf[len+38] = ' ';
            len += 39;
          }
          /* Filename already in place, append CR-LF */
          len += strlen ((char *)&sendbuf[len]);
          len += net_strcpy ((char *)&sendbuf[len], "\r\n");
        }
        if (net_tcp_send (ftp_s->DSocket, sendbuf, (uint32_t)len) == netOK) {
          /* 0-size packets are ignored */
          ftp_s->Flags |= FTP_FLAG_DACK;
        }
        break;

      case FTP_STATE_RDFILE:
        /* Retrieve a file from a FTP server */
        if ((ftp_s->Flags & FTP_FLAG_DOPEN) == 0) {
          /* Wait for data socket to connect */
          break;
        }

        /* Allocate transmit frame buffer */
        sendbuf = NULL;
        if (net_mem_avail_tx()) {
          max_dsize = net_tcp_get_mss (ftp_s->DSocket);
          sendbuf   = net_tcp_get_buf (max_dsize | 0x80000000);
        }
        if (sendbuf == NULL) {
          /* Wait, no memory available */
          break;
        }

        len = (int32_t)netFTPs_fread (ftp_s->File, sendbuf, max_dsize);
        if (net_tcp_send (ftp_s->DSocket, sendbuf, (uint32_t)len) == netOK) {
          ftp_s->Flags |= FTP_FLAG_DACK;
          if (len == (int32_t)max_dsize) {
            break;
          }
        }
        /* End of file reached */
        close_file (ftp_s);
        netFTPs_Notify (netFTPs_EventDownload);
        ftp_s->State = FTP_STATE_DCLOSE;
        break;

      case FTP_STATE_WRFILE:
        /* Store a file on FTP server */
        if (!(ftp_s->Flags & FTP_FLAG_ERROR)) {
          break;
        }
        /* Local file write error occured */
        ftp_s->Flags &= ~(FTP_FLAG_ERROR | FTP_FLAG_DOPEN | FTP_FLAG_PASSIVE);
        ftp_s->Resp   =  FTP_RESP_DISKFULL;
        net_tcp_abort (ftp_s->DSocket);
        close_file (ftp_s);
        netFTPs_Notify (netFTPs_EventLocalFileError);
        ftp_s->State  = FTP_STATE_COMMAND;
        sys->Busy     = true;
        break;

      case FTP_STATE_MKDIR:
        /* Make a folder (remaining steps) */
        if (netFTPs_mkdir (net_path(ftp->root,ftp_s->Name)) == false) {
          /* Failed, can not create directory */
          ERRORF (FTP,"Session %d, Make directory failed\n",session);
          EvrNetFTPs_MakeDirectoryFailed (session);
          ftp_s->Resp  = FTP_RESP_CREATERR;
        }
        else {
          /* Success, directory created */
          DEBUGF (FTP," Directory created\n");
          EvrNetFTPs_DirectoryCreated (session);
          netFTPs_Notify (netFTPs_EventMakeDirectory);
          ftp_s->Resp  = FTP_RESP_MKDIROK;
        }
        free_name (ftp_s);
        ftp_s->State = FTP_STATE_COMMAND;
        sys->Busy    = true;
        break;

      case FTP_STATE_FSIZE:
      case FTP_STATE_FTIME:
        /* Get file status (size, last-modified time) */
        if (is_ffind_busy ()) {
          /* Other ftp session has locked ffind() */
          break;
        }
        if (netFTPs_ffind (net_path (ftp->root, ftp_s->Name), NULL, &fsize, &ftime, true)) {
          /* File was found, send info */
          sendbuf = net_tcp_get_buf (24);
          len = net_strcpy ((char *)sendbuf, "213 ");
          if (ftp_s->State == FTP_STATE_FSIZE) {
            len += net_ultoa ((char *)&sendbuf[len], fsize);
            DEBUGF (FTP," Size: %u bytes\n",fsize);
            EvrNetFTPs_ShowFileSize (session, fsize);
          }
          else {
            /* Last modified time in format "YYYYMMDDhhmmss" */
            /* Format: "%04d%02d%02d%02d%02d%02d"            */
            len += (uint32_t)net_itoa_n0 ((char *)&sendbuf[len], ftime.year, 4);
            len += (uint32_t)net_itoa_n0 ((char *)&sendbuf[len], ftime.mon,  2);
            len += (uint32_t)net_itoa_n0 ((char *)&sendbuf[len], ftime.day,  2);
            len += (uint32_t)net_itoa_n0 ((char *)&sendbuf[len], ftime.hr,   2);
            len += (uint32_t)net_itoa_n0 ((char *)&sendbuf[len], ftime.min,  2);
            len += (uint32_t)net_itoa_n0 ((char *)&sendbuf[len], ftime.sec,  2);
            DEBUGF (FTP," Time: %s\n",(char *)&sendbuf[4]);
            EvrNetFTPs_ShowFileLastModifiedTime (session, (char *)&sendbuf[4], 16);
          }
          /* Writes also a terminating-0 !!! */
          len += net_strcpy ((char *)&sendbuf[len], "\r\n");
          net_tcp_send (ftp_s->Socket, sendbuf, (uint32_t)len);
          ftp_s->Flags |= FTP_FLAG_WACK;
        }
        else {
          ERRORF (FTP,"Session %d, File not found\n",session);
          EvrNetFTPs_FileNotFound (session);
          ftp_s->Resp = FTP_RESP_NOTFOUND;
        }
        free_name (ftp_s);
        ftp_s->State = FTP_STATE_COMMAND;
        sys->Busy    = true;
        break;

      case FTP_STATE_DCLOSE:
        /* Done, close data connection */
        if (ftp_s->Flags & FTP_FLAG_DOPEN) {
          ftp_s->Flags &= ~(FTP_FLAG_DOPEN | FTP_FLAG_PASSIVE);
          DEBUGF (FTP,"Data connection closed\n");
          EvrNetFTPs_CloseDataConnection (session);
          net_tcp_close (ftp_s->DSocket);
        }
        ftp_s->State = FTP_STATE_COMMAND;
        ftp_s->Resp  = FTP_RESP_DCLOSE;
        sys->Busy    = true;
        break;

      case FTP_STATE_QUIT:
        /* QUIT Command received */
        DEBUGF (FTP,"Session %d close\n",session);
        EvrNetFTPs_CloseSession (session);
        net_tcp_close (ftp_s->Socket);
        ftp_kill_session (ftp_s);
        break;

      case FTP_STATE_KILL:
        /* FTP Session killed, abort also data connection */
        net_tcp_abort (ftp_s->DSocket);
        ftp_s->State = FTP_STATE_TWAIT;
        break;

      case FTP_STATE_TWAIT:
        /* Waiting for TCP sockets to close */
        if (((sys->Flags & SYS_FLAG_TICK) == 0)                       ||
            (net_tcp_get_state (ftp_s->Socket) > netTCP_StateLISTEN)  ||
            (net_tcp_get_state (ftp_s->DSocket) > netTCP_StateCLOSED)) {
          break;
        }
        /* Sockets are closed, send notification and goto idle */
        DEBUGF (FTP,"Session %d idle\n",session);
        EvrNetFTPs_SessionIdle (session);
        netFTPs_Notify ((ftp_s->Flags & FTP_FLAG_LOGIN) ? 
                        netFTPs_EventLogout : netFTPs_EventLoginFailed);
        ftp_s->Flags = 0;
        ftp_s->State = FTP_STATE_IDLE;
        break;
    }
  }
}

/**
  \brief       Parse received command and extract command index.
  \param[in]   name  buffer containing command name.
  \return      none.
  \note        Command index is returned in 'cmd.sel' variable.
*/
static void ftp_parse_cmd (const char *name) {
  static const NET_FTP_CTYPE ctype[33] = {
    { "USER", FTP_CMD_USER },
    { "PASS", FTP_CMD_PASS },
    { "QUIT", FTP_CMD_QUIT },
    { "SYST", FTP_CMD_SYST },
    { "NOOP", FTP_CMD_NOOP }, { "XNOP", FTP_CMD_NOOP },
    { "HELP", FTP_CMD_HELP },
    { "PWD",  FTP_CMD_PWD  }, { "XPWD", FTP_CMD_PWD  },
    { "CWD",  FTP_CMD_CWD  }, { "XCWD", FTP_CMD_CWD  },
    { "CDUP", FTP_CMD_CDUP }, { "XCUP", FTP_CMD_CDUP },
    { "MKD",  FTP_CMD_MKD  }, { "XMKD", FTP_CMD_MKD  },
    { "RMD",  FTP_CMD_RMD  }, { "XRMD", FTP_CMD_RMD  },
    { "TYPE", FTP_CMD_TYPE },
    { "PORT", FTP_CMD_PORT },
    { "PASV", FTP_CMD_PASV },
    { "SIZE", FTP_CMD_SIZE },
    { "MDTM", FTP_CMD_MDTM },
    { "NLST", FTP_CMD_NLST },
    { "LIST", FTP_CMD_LIST },
    { "RETR", FTP_CMD_RETR },
    { "STOR", FTP_CMD_STOR },
    { "APPE", FTP_CMD_APPE },
    { "DELE", FTP_CMD_DELE },
    { "RNFR", FTP_CMD_RNFR },
    { "RNTO", FTP_CMD_RNTO },
    { "FEAT", FTP_CMD_FEAT },
    { "EPRT", FTP_CMD_EPRT }, /* Extended commands IP4/IP6 */
    { "EPSV", FTP_CMD_EPSV } };
  uint32_t i,j;

  cmd.sel = FTP_CMD_UNKNOWN;
  for (i = 0; i < sizeof(ctype)/sizeof(NET_FTP_CTYPE); i++) {
    if (net_strcasecmp (name, ctype[i].name)) {
      j = strlen (ctype[i].name);
      switch (name[j]) {
        case ' ':
        case 0:
          /* Command is space or null-terminated */
          cmd.sel = ctype[i].id;
          break;
      }
      return;
    }
  }
}

/**
  \brief       Open a FTP data connection if required.
  \param[in]   ftp_s  session descriptor.
  \return      status code as defined with netStatus.
*/
static netStatus ftp_dopen_req (NET_FTP_INFO *ftp_s) {
  if ((ftp_s->Resp == FTP_RESP_PASVOK) || 
      (ftp_s->Resp == FTP_RESP_EPSVOK)) {
    /* Passive mode, start listening */
    return (net_tcp_listen (ftp_s->DSocket, ftp_s->DPort));
  }
  if (ftp_s->Resp == FTP_RESP_DOPEN) {
    /* Active mode, connect to remote client */
    ftp_s->Client.port = ftp_s->DPort;
    return (net_tcp_connect (ftp_s->DSocket, &ftp_s->Client, 0));
  }
  return (netOK);
}

/**
  \brief       Kill active FTP server session.
  \param[in]   ftp_s  session descriptor.
  \return      none.
*/
static void ftp_kill_session (NET_FTP_INFO *ftp_s) {
  ftp_s->Resp  = FTP_RESP_DONE;
  ftp_s->State = FTP_STATE_KILL;
  /* Login flag is used for event notification */
  ftp_s->Flags &= FTP_FLAG_LOGIN;
  if (ftp_s->Path != NULL) {
    net_mem_free (__FRAME(ftp_s->Path));
    ftp_s->Path = NULL;
  }
  close_file (ftp_s);
  free_name (ftp_s);
}

/**
  \brief       Map TCP socket to existing FTP session.
  \param[in]   socket  TCP socket handle.
  \return      session descriptor or NULL= none.
*/
static NET_FTP_INFO *ftp_map_session (int32_t socket) {
  NET_FTP_INFO *ftp_s;
  int32_t i;

  for (ftp_s = &ftpc->Scb[0], i = 0; i < ftpc->NumSess; ftp_s++, i++) {
    if (ftp_s->Socket == socket) {
      /* Control socket */
      return (ftp_s);
    }
    if (ftp_s->DSocket == socket && ftp_s->State > FTP_STATE_RESERVED) {
      /* Data socket */
      return (ftp_s);
    }
  }
  return (NULL);
}

/**
  \brief       Scan PORT/EPRT command for FTP data port number.
  \param[in]   buf   buffer containing command parameters.
  \param[in]   type  command type (PORT or EPRT).
  \return      data port or default port if not found.
*/
static uint16_t ftp_scan_dport (const char *buf, uint8_t type) {
  int32_t max,m,n,port;

  if (type == FTP_CMD_PORT) {
    /* PORT example: 192,168,0,162,19,137 */
    for (n = 0, max = 20; max; buf++, max--) {
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
    /* EPRT IP4 example: |1|192.168.0.162|5001|             */
    /*      IP6 example: |2|fe80::1c30:6cff:fea2:455e|5282| */
    for (n = 0, max = 50; max; buf++, max--) {
      if (*buf == '|') {
        if (++n == 3) {
          port = net_atoi (++buf, NULL);
          return (port & 0xFFFF);
        }
      }
    }
  }
  return (FTP_DEF_DPORT);
}

/**
  \brief       Make an absolute path for file access.
  \param[in]   ftp_s  session descriptor.
  \param[in]   name   file name.
  \return      none.
*/
static char *make_path (NET_FTP_INFO *ftp_s, const char *name) {
  char *path,ch;
  int32_t i,j,len;

  len = (int32_t)strlen (name);
  if (name[0] != '/') {
    /* We have a relative path */
    len += ftp_s->PathLen;
  }
  path = (char *)net_mem_alloc ((uint32_t)len+2);
  i = 0;
  if (name[0] != '/') {
    /* Make an absolute path */
    i = net_strcpy (path, ftp_s->Path);
    if (i > 1) {
      /* Append a folder separator */
      path[i++] = '/';
    }
  }
  /* Add name to the path */
  for (j = 0; ; j++) {
    ch = name[j];
    if (ch == 0) break;
    if (ch == '\\') ch = '/';
    path[i++] = ch;
  }
  path[i] = 0;
  DEBUGF (FTP," Path %s\n",path);
  EvrNetFTPs_ShowPath (path, (uint32_t)i);
  return (path);
}

/**
  \brief       Store path name for the session.
  \param[in]   ftp_s  session descriptor.
  \param[in]   path   pointer to path name.
  \return      none.
*/
static void store_path (NET_FTP_INFO *ftp_s, const char *path) {
  uint8_t ch;
  int32_t i;

  if (path[0] == '.') {
    if (path[1] == '.' && path[2] == 0) {
      /* This is ".." folder */
      path_dir_up (ftp_s);
    }
    return;
  }
  /* Construct the path name */
  i = ftp_s->PathLen;
  if (i == 1) i = 0;
  if (*path == '/') {
    path++;
    i = 0;
  }
  ch = '/';
  while (i < FTP_MAX_PATH) {
    ftp_s->Path[i++] = ch;
    ch = *path++;
    if (ch == 0) break;
  }
  ftp_s->Path[i] = 0;
  ftp_s->PathLen = i & 0xFF;
}

/**
  \brief       Remove last subfolder from path chain.
  \param[in]   ftp_s  session descriptor.
  \return      none.
*/
static void path_dir_up (NET_FTP_INFO *ftp_s) {
  int32_t i;

  for (i = ftp_s->PathLen-1; i >= 0; i--) {
    if (ftp_s->Path[i] == '/') {
      if (i == 0) i++;
      ftp_s->Path[i] = 0;
      ftp_s->PathLen = i & 0xFF;
      break;
    }
  }
}

/**
  \brief       Release path memory block if allocated.
  \param[in]   ftp_s  session descriptor.
  \return      none.
*/
static void free_name (NET_FTP_INFO *ftp_s) {
  if (ftp_s->Name != NULL) {
    net_mem_free (__FRAME(ftp_s->Name));
    ftp_s->Name = NULL;
  }
}

/**
  \brief       Close local file if opened.
  \param[in]   ftp_s  session descriptor.
  \return      none.
*/
static void close_file (NET_FTP_INFO *ftp_s) {
  if (ftp_s->File != NULL) {
    netFTPs_fclose (ftp_s->File);
    ftp_s->File = NULL;
  }
}

/**
  \brief       Set path mask for ffind() folder listing.
  \param[in]   ftp_s  session descriptor.
  \param[in]   set    set mode flag (true= set, false= revert).
  \return      none.
*/
static void set_mask (NET_FTP_INFO *ftp_s, bool set) {
  char *p   = ftp_s->Path;
  int32_t i = ftp_s->PathLen;

  if (set == true) {
    /* Append "*" to a path */
    if (i == 1) i--;
    p[i]   = '/';
    p[i+1] = '*';
    i += 2;
  }
  /* Revert the path change */
  p[i] = 0;
}

/**
  \brief       Clean file name.
  \param[in,out] buf  buffer containing file name.
  \return      pointer to converted name.
*/
static char *get_fname (char *buf) {
  char *p;

  /* Remove leading spaces */
  while (*buf == ' ') {
    buf++;
  }
  /* Remove trailing folder separator */
  for (p = buf; *p; p++);
  if (--p > buf) {
    if (*p == '/') {
      *p = 0;
    }
  }
  return (buf);
}

/**
  \brief       Remove path info from file name.
  \param[in,out] buf  buffer containing file name.
  \return      pointer to converted name.
*/
static char *no_path (char *buf) {
  char *p;

  /* Remove leading spaces and trailing folder separator */
  buf = get_fname (buf);

  /* Remove path info */
  for (p = buf; *p; p++) {
    if (*p == '/') {
      buf = p + 1;
    }
  }
  return (buf);
}

/**
  \brief       Check if ffind() function is busy.
  \return      check status:
               - true  = busy,
               - false = idle.
*/
static bool is_ffind_busy (void) {
  int32_t i;

  for (i = 0; i < ftpc->NumSess; i++ ) {
    /* Check if LIST command is busy */
    if (ftpc->Scb[i].Flags & FTP_FLAG_LSTLOCK) {
      return (true);
    }
  }
  return (false);
}

/* Library default function(s) */
__WEAK bool netFTPs_AcceptClient (const NET_ADDR *addr) {
  (void)addr; return (true); }
__WEAK uint8_t netFTPs_CheckUsername (const char *username) {
  (void)username; return (0); }
__WEAK bool netFTPs_CheckPassword (uint8_t user_id, const char *password) {
  (void)user_id; (void)password; return (false); }
__WEAK bool netFTPs_FileAccess (uint8_t user_id,
                                const char *fname, uint32_t access) {
  (void)user_id; (void)fname; (void)access; return (true); }
__WEAK void netFTPs_Notify (netFTPs_Event event) {
  (void)event; }
