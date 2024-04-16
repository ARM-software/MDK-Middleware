/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_tftp_server.c
 * Purpose: Trivial File Transfer Protocol Server
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_udp.h"
#include "net_tftp_server.h"
#include "net_dbg.h"

/* Local variables */
static uint16_t tftp_port_num;
static uint8_t  tftp_Socket;
static bool     run_service;
static const char *tftp_root;

/* Local functions */
static void tftp_server_start (void);
static void tftp_server_stop (void);
static void tftp_server_run (void);
static uint32_t tftp_listener (int32_t socket, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len);
static uint32_t tftp_data_cb  (int32_t socket, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len);
static bool tftp_process (NET_TFTP_INFO *tftp_s, const uint8_t *buf, uint32_t len);
static void tftp_proc_data (NET_TFTP_INFO *tftp_s, const uint8_t *buf, uint32_t len);
static void tftp_proc_ack (NET_TFTP_INFO *tftp_s, const uint8_t *buf, uint32_t len);
static void tftp_proc_opt (NET_TFTP_INFO *tftp_s, const uint8_t *buf, uint32_t len);
static const uint8_t *get_next_opt (const uint8_t *buf, const uint8_t *end);
static void tftp_ack_opt (NET_TFTP_INFO *tftp_s);
static void tftp_ack_block (NET_TFTP_INFO *tftp_s);
static void tftp_abort_session (NET_TFTP_INFO *tftp_s, uint8_t errno, const char *errm);
static void tftp_kill_session (NET_TFTP_INFO *tftp_s);
static void tftp_close_file (NET_TFTP_INFO *tftp_s);
static void tftp_free_buf (NET_TFTP_INFO *tftp_s);
static NET_TFTP_INFO *tftp_map_session (const __ADDR *addr);
static NET_TFTP_INFO *tftp_open_session (const __ADDR *addr);
static void tftp_set_retransmit (NET_TFTP_INFO *tftp_s, uint8_t *buf, uint32_t len);
#ifdef DEBUG_STDIO
 static void debug_info (uint16_t opcode);
#endif

/**
  \brief       Initialize TFTP server.
*/
void net_tftp_server_init (void) {
  NET_TFTP_INFO *tftp_s;
  int32_t sock,i;

  DEBUGF (TFTP,"Init %d Sessions, Port %d\n",tftp->NumSess,tftp->PortNum);
  EvrNetTFTPs_InitServer (tftp->NumSess, tftp->PortNum, tftp->EnFwall);
  memset (tftp->Scb, 0, sizeof (*tftp->Scb) * tftp->NumSess);
#ifdef DEBUG_STDIO
  if (tftp->EnFwall) {
    DEBUGF (TFTP," Firewall support enabled\n");
  }
#endif
#ifdef __DBG_ENABLED
  if (tftp->Root) {
    DEBUGF (TFTP," Root folder \"%s\"\n",tftp->Root);
    EvrNetTFTPs_ShowRootFolder (tftp->Root, strlen(tftp->Root));
  }
#endif
  /* Allocate TFTP Control socket */
  sock = net_udp_get_socket (tftp_listener);
  if (sock < 0) sock = 0;
  tftp_Socket = sock & 0xFF;
  if (tftp_Socket == 0) {
    ERRORF (TFTP,"Get socket failed\n");
    EvrNetTFTPs_GetSocketFailed (0);
    net_sys_error (NET_ERROR_UDP_ALLOC);
    return;
  }
  for (i = 1, tftp_s = &tftp->Scb[0]; i <= tftp->NumSess; tftp_s++, i++) {
    if (tftp->EnFwall) {
      /* Firewall support uses one UDP socket for all transfers */
      tftp_s->Socket = tftp_Socket;
    }
    else {
      /* RFC compliant mode uses one UDP socket per session */
      sock = net_udp_get_socket (tftp_data_cb);
      if (sock < 0) sock = 0;
      tftp_s->Socket = sock & 0xFF;
      if (tftp_s->Socket == 0) {
        ERRORF (TFTP,"Session %d, Get socket failed\n",i);
        EvrNetTFTPs_GetSocketFailed (i & 0xFF);
        net_sys_error (NET_ERROR_UDP_ALLOC);
        return;
      }
    }
    tftp_s->Id = i & 0xFF;
  }

  /* Init service control */
  run_service   = false;
  tftp_root     = tftp->Root;
  tftp_port_num = tftp->PortNum;
  if (sysc->AutoStart) {
    tftp_server_start ();
  }
}

/**
  \brief       De-initialize TFTP server.
*/
void net_tftp_server_uninit (void) {
  NET_TFTP_INFO *tftp_s;
  int32_t i;

  DEBUGF (TFTP,"Uninit Server\n");
  EvrNetTFTPs_UninitServer ();

  for (i = 0, tftp_s = &tftp->Scb[0]; i < tftp->NumSess; tftp_s++, i++) {
    /* Close local file if open */
    tftp_close_file (tftp_s);
    /* Clear session control block */
    memset (tftp_s, 0, sizeof (*tftp_s));
  }

  /* Uninit service control */
  run_service   = false;
  tftp_root     = NULL;
  tftp_port_num = 0;
  tftp_Socket   = 0;
}

/**
  \brief       Start TFTP server service.
  \return      status code as defined with netStatus.
*/
netStatus netTFTPs_Start (void) {

  START_LOCK (netStatus);

  if (!run_service) {
    /* Service is down */
    tftp_server_start ();
  }

  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Stop TFTP server service.
  \return      status code as defined with netStatus.
*/
netStatus netTFTPs_Stop (void) {

  START_LOCK (netStatus);

  if (run_service) {
    /* Service is running */
    tftp_server_stop ();
  }

  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Check if TFTP server service is running.
  \return      service status:
               - true  = service is running.
               - false = service is not running.
*/
bool netTFTPs_Running (void) {
  return (run_service);
}

/**
  \brief       Get current port number of TFTP server.
  \return      current port number.
*/
uint16_t netTFTPs_GetPort (void) {
  return (tftp_port_num);
}

/**
  \brief       Set TFTP server port number.
  \param[in]   port  service port number.
  \return      status code as defined with netStatus.
*/
netStatus netTFTPs_SetPort (uint16_t port) {
  if (run_service) {
    return (netWrongState);
  }
  tftp_port_num = port;
  return (netOK);
}

/**
  \brief       Retrieve current root folder path.
  \return      pointer to root folder path, a null-terminated string or
                 NULL if root folder is disabled.
*/
const char *netTFTPs_GetRootPath (void) {
  return (tftp_root);
}

/**
  \brief       Change root folder path.
  \param[in]   path  new root folder path.
  \return      status code as defined with netStatus.
*/
netStatus netTFTPs_SetRootPath (const char *path) {
  static char dir_buf [NET_ROOT_DIR_LEN];

  START_LOCK (netStatus);

  if ((path == NULL) || strlen(path) >= NET_ROOT_DIR_LEN) {
    RETURN (netInvalidParameter);
  }
  if (!tftp_root) {
    /* Root folder not enabled for the service */
    RETURN (netError);
  }
  if (run_service) {
    /* Service is running */
    RETURN (netWrongState);
  }
  DEBUGF (TFTP,"SetRootFolder \"%s\"\n",path);
  EvrNetTFTPs_SetRootPath (path, strlen(path));
  /* From now on use a static buffer instead of default const array */
  tftp_root = dir_buf;
  net_strcpy (dir_buf, path);
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Main TFTP server scheduler.
*/
void net_tftp_server_run (void) {
  if (run_service) {
    tftp_server_run ();
  }
}

/**
  \brief       Start TFTP server service.
*/
static void tftp_server_start (void) {
  DEBUGF (TFTP,"Start service, port %d\n",tftp_port_num);
  EvrNetTFTPs_StartService (tftp_port_num);
  /* Open Command socket for communication */
  net_udp_open (tftp_Socket, tftp_port_num);
  run_service = true;
}

/**
  \brief       Stop TFTP server service.
*/
static void tftp_server_stop (void) {
  NET_TFTP_INFO *tftp_s;
  int32_t i;

  DEBUGF (TFTP,"Stop service\n");
  EvrNetTFTPs_StopService ();
  for (i = 1, tftp_s = &tftp->Scb[0]; i <= tftp->NumSess; tftp_s++, i++) {
    tftp_kill_session (tftp_s);
  }
  if (tftp->EnFwall) {
    net_udp_close (tftp_Socket);
  }
  run_service = false;
}

/**
  \brief       Run TFTP server service.
*/
static void tftp_server_run (void) {
  static uint8_t session = 0;
  NET_TFTP_INFO *tftp_s;
  uint8_t *sendbuf;
  uint32_t i,len;

  /* Check all TFTP sessions if there is something to do */
  for (i = 0; i < tftp->NumSess; i++) {
    if (++session > tftp->NumSess) {
      session = 1;
    }
    tftp_s = &tftp->Scb[session-1];
    if (tftp_s->State == TFTP_STATE_IDLE) {
      continue;
    }
    if ((sys->Flags & SYS_FLAG_SEC) && tftp_s->Timer) {
      if (--tftp_s->Timer == 0) {
        /* Kill the expired session */
        ERRORF (TFTP,"Session %d, Timeout expired\n",tftp_s->Id);
        EvrNetTFTPs_TimeoutExpiredAbort (tftp_s->Id);
        tftp_abort_session (tftp_s, TFTP_ERR_NOTDEF, "Timeout");
        return;
      }
    }
    if (tftp_s->Flags & TFTP_FLAG_WACK) {
      /* Waiting for a response from server */
      continue;
    }
    switch (tftp_s->State) {
      case TFTP_STATE_READ:
        /* Read local file and send it to client */
        DEBUGF (TFTP,"Session %d, Send block %d\n",tftp_s->Id,tftp_s->BlockNr);
        sendbuf = net_udp_get_buf (tftp_s->BlockSz + 4);
        set_u16 (&sendbuf[0], TFTP_OPCOD_DATA);
        set_u16 (&sendbuf[2], tftp_s->BlockNr);
        len = netTFTPs_fread (tftp_s->File, &sendbuf[4], tftp_s->BlockSz);
        tftp_s->Flags |= TFTP_FLAG_WACK;
        if (len < tftp_s->BlockSz) {
          /* This is the last block */
          tftp_s->Flags |= TFTP_FLAG_LASTB;
        }
        len += 4;
        DEBUGF (TFTP," Length: %d bytes\n",len);
        EvrNetTFTPs_SendBlock (tftp_s->Id, tftp_s->BlockNr, (uint32_t)len);
        /* Keep the packet in memory for retransmission */
        net_udp_keep_buf (tftp_s->Socket, sendbuf);
        net_udp_send (tftp_s->Socket, &tftp_s->Client, sendbuf, len);
        /* The packet was not released by net_udp_send() */
        tftp_set_retransmit (tftp_s, sendbuf, len);
        tftp_s->Retries = tftp->MaxRetry;
        tftp_s->Timer   = tftp->DefTout;
        return;
    }
  }
}

/**
  \brief       Control event callback notification.
  \param[in]   socket  control socket handle.
  \param[in]   addr    structure containing remote IP address and port.
  \param[in]   buf     pointer to a buffer containing the data.
  \param[in]   len     length of the data.
  \return      status:
               - true  = success,
               - false = error.
*/
static uint32_t tftp_listener (int32_t socket, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len) {
  NET_TFTP_INFO *tftp_s;
  uint16_t opcode;
  uint8_t *sendbuf;

  /* Check if this is our socket */
  if (tftp_Socket != socket) {
    return (false);
  }

  DEBUGF (TFTP,"*** Process_frame ***\n");
  DEBUGF (TFTP," Main, %d bytes\n",len);
  EvrNetTFTPs_ReceiveFrame (tftp_Socket, len);
  if (len < 4) {
    /* Ignore too short packet */
    ERRORF (TFTP,"Main, Frame too short\n");
    EvrNetTFTPs_FrameTooShort (len, 4);
    return (false);
  }

  tftp_s = tftp_map_session ((const __ADDR *)addr);
  if ((tftp_s != NULL) && tftp->EnFwall) {
    /* An existing session found in Firewall Support mode */
    return (tftp_process (tftp_s, buf, len));
  }

  if (tftp_s == NULL) {
    /* Check client access only for a new connection */
    /* Access client filtering by IP and/or port     */
    if (netTFTPs_AcceptClient (addr) == false) {
      ERRORF (TFTP,"Access denied [%s], port %d\n",net_addr_ntoa((const __ADDR *)addr),
                                                   addr->port);
      EvrNetTFTPs_UserAccessDenied (addr);
      /* User denied access for this host */
      return (false);
    }
    /* Open a new session */
    tftp_s = tftp_open_session ((const __ADDR *)addr);
  }
  if (tftp_s == NULL) {
    /* No resources free, reject the request */
    ERRORF (TFTP,"Busy, Access denied\n");
    EvrNetTFTPs_AccessDeniedNoResources ();
    sendbuf = net_udp_get_buf (16);
    set_u16 (&sendbuf[0], TFTP_OPCOD_ERR);
    set_u16 (&sendbuf[2], TFTP_ERR_NOTDEF);
    net_strcpy ((char *)&sendbuf[4], "Server Busy");
    net_udp_send (tftp_Socket, (const __ADDR *)addr, sendbuf, 16);
    return (false);
  }
  tftp_s->Flags = 0;
  if (tftp_s->State == TFTP_STATE_OPEN) {
    /* Open a virtual channel for data transfer */
    DEBUGF (TFTP," Client [%s], port %d\n",net_addr_ntoa((const __ADDR *)addr),
                                           addr->port);
    DEBUGF (TFTP,"Session %d open\n",tftp_s->Id);
    EvrNetTFTPs_ShowClientAddress (addr);
    if (!tftp->EnFwall) {
      /* Open socket for communication, autoselect port (STID) */
      net_udp_open (tftp_s->Socket, 0);
    }
  }

  /* Read the opcode and process it */
  opcode = get_u16 (&buf[0]);
  DEBUG_INFO (opcode);
  EvrNetTFTPs_OperationRequest (tftp_s->Id, opcode);
  switch (opcode) {
    case TFTP_OPCOD_RRQ:
    case TFTP_OPCOD_WRQ:
      /* Valid request opcodes */
      if (tftp_s->State == TFTP_STATE_OPEN) {
        /* First try request */
        break;
      }
      /* Retransmitted request, maybe our response was lost */
      DEBUGF (TFTP,"Session %d restart\n",tftp_s->Id);
      EvrNetTFTPs_SessionRestart (tftp_s->Id);
      tftp_close_file (tftp_s);
      tftp_free_buf (tftp_s);
      break;

    case TFTP_OPCOD_DATA:
    case TFTP_OPCOD_ACK:
      /* This belongs to virtual channel STID <==> CTID */
      ERRORF (TFTP,"Session %d, Invalid transfer ID\n",tftp_s->Id);
      EvrNetTFTPs_InvalidTransferId (tftp_s->Id);
      tftp_abort_session (tftp_s, TFTP_ERR_UNKNTID, "Invalid TID");
      return (false);

    case TFTP_OPCOD_ERR:
      /* Client encountered an error */
      ERRORF (TFTP,"Session %d, Err_%d recvd\n",tftp_s->Id,get_u16(&buf[2]));
      EvrNetTFTPs_ErrorCodeReceived (tftp_s->Id, get_u16(&buf[2]));
      tftp_abort_session (tftp_s, 0, NULL);
      return (false);

    default:
      /* Illegal opcode, send an error reply */
      ERRORF (TFTP,"Session %d, Illegal opcode recvd\n",tftp_s->Id);
      EvrNetTFTPs_IllegalOpcodeReceived (tftp_s->Id, opcode);
      tftp_abort_session (tftp_s, TFTP_ERR_ILLOPCOD, "Illegal opcode");
      return (false);
  }

  buf += 2;
  len -= 2;
  DEBUGF (TFTP," File  : %s\n",buf);
  EvrNetTFTPs_FileRequested ((const char *)buf, strlen((const char *)buf));

  /* Default block size */
  tftp_s->BlockSz = 512;
  tftp_proc_opt (tftp_s, buf, len);
  DEBUGF (TFTP," BlSize: %d\n",tftp_s->BlockSz);
  EvrNetTFTPs_TransferBlockSize (tftp_s->Id, tftp_s->BlockSz);
  if ((tftp_s->Flags & TFTP_FLAG_MODEOK) == 0) {
    /* Only binary mode is supported */
    ERRORF (TFTP,"Session %d, Invalid mode\n",tftp_s->Id);
    EvrNetTFTPs_TransferModeNotBinary (tftp_s->Id);
    tftp_abort_session (tftp_s, TFTP_ERR_NOTDEF, "Invalid mode");
    return (false);
  }

  switch (opcode) {
    case TFTP_OPCOD_RRQ:
      /* Read file requested */
      tftp_s->File = netTFTPs_fopen (net_path(tftp_root,(const char *)buf), "rb");
      if (tftp_s->File == NULL) {
        /* Failed, file does not exist or is locked */
        ERRORF (TFTP,"Session %d, File not found\n",tftp_s->Id);
        EvrNetTFTPs_LocalFileNotFound (tftp_s->Id);
        tftp_abort_session (tftp_s, TFTP_ERR_NOTFOUND, "File not found");
        return (false);
      }
      if (tftp_s->Flags & TFTP_FLAG_OPT) {
        /* Ack the options first */
        EvrNetTFTPs_SendOptionAck (tftp_s->Id, tftp_s->BlockSz);
        tftp_ack_opt (tftp_s);
        tftp_s->Flags |= TFTP_FLAG_WACK;
      }
      tftp_s->BlockNr = 1;
      tftp_s->State   = TFTP_STATE_READ;
      break;

    case TFTP_OPCOD_WRQ:
      /* Write file requested */
      tftp_s->File = netTFTPs_fopen (net_path(tftp_root,(const char *)buf), "wb");
      if (tftp_s->File == NULL) {
        /* Failed, can not open file for writing */
        ERRORF (TFTP,"Session %d, File create failed\n",tftp_s->Id);
        EvrNetTFTPs_LocalFileCreateFailed (tftp_s->Id);
        tftp_abort_session (tftp_s, TFTP_ERR_NOACCESS, "Access violation");
        return (false);
      }
      if (tftp_s->Flags & TFTP_FLAG_OPT) {
        /* Ack the options first */
        EvrNetTFTPs_SendOptionAck (tftp_s->Id, tftp_s->BlockSz);
        tftp_ack_opt (tftp_s);
      }
      else {
        tftp_s->BlockNr = 0;
        EvrNetTFTPs_SendBlockAck (tftp_s->Id, tftp_s->BlockNr);
        tftp_ack_block (tftp_s);
      }
      tftp_s->BlockNr = 1;
      tftp_s->State   = TFTP_STATE_WRITE;
      break;
  }
  return (true);
}

/**
  \brief       Data event callback notification.
  \param[in]   socket  data socket handle.
  \param[in]   addr    structure containing remote IP address and port.
  \param[in]   buf     pointer to a buffer containing the data.
  \param[in]   len     length of the data.
  \return      status:
               - true  = success,
               - false = error.
*/
static uint32_t tftp_data_cb (int32_t socket, const NET_ADDR *addr,
                                              const uint8_t *buf, uint32_t len) {
  NET_TFTP_INFO *tftp_s;
  int32_t i;

  (void)addr;
  /* Find the right session */
  for (tftp_s = &tftp->Scb[0], i = 0; i < tftp->NumSess; tftp_s++, i++) {
    if (tftp_s->Socket == socket) {
      goto data;
    }
  }
  return (false);
data:
  DEBUGF (TFTP,"*** Process_frame DATA ***\n");
  DEBUGF (TFTP," Session %d, %d bytes\n",tftp_s->Id,len);
  EvrNetTFTPs_ReceiveDataFrame (tftp_s->Id, len);
  if (len < 4) {
    /* Ignore too short packet */
    ERRORF (TFTP,"Session %d, Frame too short\n",tftp_s->Id);
    EvrNetTFTPs_DataFrameTooShort (tftp_s->Id, len, 4);
    return (false);
  }

  return (tftp_process (tftp_s, buf, len));
}

/**
  \brief       Process received TFTP frame.
  \param[in]   tftp_s  session descriptor.
  \param[in]   buf     buffer containing data or options.
  \param[in]   len     length of data or options.
*/
static bool tftp_process (NET_TFTP_INFO *tftp_s, const uint8_t *buf, uint32_t len) {
  uint16_t opcode;

  /* Get the TFTP Opcode and process it */
  opcode = get_u16 (&buf[0]);
  DEBUG_INFO (opcode);
  EvrNetTFTPs_OperationRequest (tftp_s->Id, opcode);
  switch (opcode) {
    case TFTP_OPCOD_DATA:
      /* Data packet received */
      tftp_proc_data (tftp_s, buf, len);
      break;

    case TFTP_OPCOD_ACK:
      /* Ack packet received */
      tftp_proc_ack (tftp_s, buf, len);
      break;

    case TFTP_OPCOD_ERR:
      /* Client encountered an error */
      ERRORF (TFTP,"Session %d, Err_%d recvd\n",tftp_s->Id,get_u16(&buf[2]));
      EvrNetTFTPs_ErrorCodeReceived (tftp_s->Id, get_u16(&buf[2]));
      tftp_abort_session (tftp_s, 0, NULL);
      break;

    default:
      /* Illegal opcode, send an error reply */
      ERRORF (TFTP,"Session %d, Illegal opcode recvd\n",tftp_s->Id);
      EvrNetTFTPs_IllegalOpcodeReceived (tftp_s->Id, opcode);
      tftp_abort_session (tftp_s, TFTP_ERR_ILLOPCOD, "Illegal opcode");
      return (false);
  }
  return (true);
}

/**
  \brief       Process received data frame.
  \param[in]   tftp_s  session descriptor.
  \param[in]   buf     buffer containing data.
  \param[in]   len     length of data.
*/
static void tftp_proc_data (NET_TFTP_INFO *tftp_s, const uint8_t *buf, uint32_t len) {
  uint16_t blocknr;

  if (tftp_s->State != TFTP_STATE_WRITE) {
    /* Wrong state for data */
    goto err;
  }
  blocknr = get_u16 (&buf[2]);
  if (blocknr == 0) {
    goto err;
  }
  if (tftp_s->BlockNr == (blocknr + 1)) {
    /* Previous block received again, maybe our ack was lost*/
    DEBUGF (TFTP," Block %d recvd again\n",blocknr);
    EvrNetTFTPs_DuplicateBlockReceived (tftp_s->Id, blocknr);
    if (tftp_s->Retries == 0) {
      ERRORF (TFTP,"Session %d, Too many retries\n",tftp_s->Id);
      EvrNetTFTPs_TooManyRetries (tftp_s->Id);
      tftp_abort_session (tftp_s, TFTP_ERR_NOTDEF, "Too many retries");
    }
    else {
      tftp_s->Retries--;
    }
    return;
  }
  DEBUGF (TFTP," Block : %d\n",blocknr);
  EvrNetTFTPs_BlockReceived (tftp_s->Id, blocknr);
  if (tftp_s->BlockNr != blocknr) {
    /* Out of sequence block number */
    ERRORF (TFTP,"Session %d, Invalid block recvd\n",tftp_s->Id);
    EvrNetTFTPs_InvalidBlockReceived (tftp_s->Id, blocknr);
err:tftp_abort_session (tftp_s, TFTP_ERR_NOTDEF, "Invalid block");
    return;
  }
  len -= 4;
  if (netTFTPs_fwrite (tftp_s->File, &buf[4], len) != len) {
    /* Write failure, abort the connection */
    ERRORF (TFTP,"Session %d, Local disk full\n",tftp_s->Id);
    EvrNetTFTPs_WriteErrorDiskFull (tftp_s->Id, len);
    tftp_abort_session (tftp_s, TFTP_ERR_DISKFULL, "Disk full");
    return;
  }
  EvrNetTFTPs_SendBlockAck (tftp_s->Id, tftp_s->BlockNr);
  tftp_ack_block (tftp_s);
  if (len < tftp_s->BlockSz) {
    /* Other side Wants to close */
    tftp_kill_session (tftp_s);
    return;
  }
  /* All is OK */
  tftp_s->Retries = tftp->MaxRetry;
  tftp_s->BlockNr++;
}

/**
  \brief       Process received ack frame.
  \param[in]   tftp_s  session descriptor.
  \param[in]   buf     buffer containing options.
  \param[in]   len     length of options.
  \note        Function is called from control socket and data socket handler.
*/
static void tftp_proc_ack (NET_TFTP_INFO *tftp_s, const uint8_t *buf, uint32_t len) {
  uint16_t blocknr;
  (void)len;

  if (tftp_s->State != TFTP_STATE_READ) {
    /* Wrong state for ack */
    goto err;
  }
  blocknr = get_u16 (&buf[2]);
  tftp_s->Flags &= ~TFTP_FLAG_WACK;
  if (tftp_s->Flags & TFTP_FLAG_OPT) {
    if (blocknr != 0) {
      /* Invalid option ack */
      goto err;
    }
    tftp_s->Flags &= ~TFTP_FLAG_OPT;
    return;
  }
  DEBUGF (TFTP," Block %d acked\n",blocknr);
  EvrNetTFTPs_BlockAckReceived (tftp_s->Id, blocknr);
  if (tftp_s->BlockNr == (blocknr + 1)) {
    /* Previous block acked again, maybe our packet was lost */
    if (tftp_s->Retries == 0) {
      ERRORF (TFTP,"Session %d, Too many retries\n",tftp_s->Id);
      EvrNetTFTPs_TooManyRetries (tftp_s->Id);
      tftp_abort_session (tftp_s, TFTP_ERR_NOTDEF, "Too many retries");
      return;
    }
    DEBUGF (TFTP," Retransmit block\n");
    EvrNetTFTPs_BlockRetransmit (tftp_s->Id, blocknr);
    net_udp_keep_buf (tftp_s->Socket, tftp_s->Buf);
    net_udp_send (tftp_s->Socket, &tftp_s->Client, tftp_s->Buf, tftp_s->BufLen);
    tftp_s->Timer = tftp->DefTout;
    tftp_s->Retries--;
    return;
  }
  if (tftp_s->BlockNr != blocknr) {
    /* Out of sequence block number */
    ERRORF (TFTP,"Session %d, Invalid block %d acked\n",tftp_s->Id,blocknr);
    EvrNetTFTPs_InvalidBlockAck (tftp_s->Id, blocknr);
err:tftp_abort_session (tftp_s, TFTP_ERR_NOTDEF, "Wrong ack");
    return;
  }
  if (tftp_s->Flags & TFTP_FLAG_LASTB) {
    tftp_kill_session (tftp_s);
    return;
  }
  tftp_set_retransmit (tftp_s, NULL, 0);
  tftp_s->BlockNr++;
}

/**
  \brief       Process request frame options.
  \param[in]   tftp_s  session descriptor.
  \param[in]   buf     buffer containing the options.
  \param[in]   len     length of options.
*/
static void tftp_proc_opt (NET_TFTP_INFO *tftp_s, const uint8_t *buf, uint32_t len) {
  const uint8_t *np,*end = buf + len;
  int32_t val;

  /* Skip the file name */
  np = get_next_opt (buf, end);
  if (np == NULL) {
    return;
  }
  /* Check mode parameter */
  DEBUGF (TFTP," Mode  : %s\n",np);
  EvrNetTFTPs_ShowRequestMode (np, strlen((const char *)np));
  if (net_strcasecmp ((const char *)np, "octet") ||
      net_strcasecmp ((const char *)np, "netascii")) {
    tftp_s->Flags |= TFTP_FLAG_MODEOK;
  }

  /* Process the options */
  while ((np = get_next_opt (np, end)) != NULL) {
    if (net_strcasecmp ((const char *)np, "blksize")) {
      np += 8;
      val = net_atoi ((const char *)np, NULL);
      if (val > 1428) {
        val = 1428;
      }
      tftp_s->BlockSz = val & 0xFFFF;
      tftp_s->Flags  |= TFTP_FLAG_OPT;
      break;
    }
  }
}

/**
  \brief       Parse TFTP options and search for next option.
  \param[in]   buf  buffer containing options.
  \param[in]   end  buffer end.
  \return      pointer to next option or NULL.
  \note        Options are separated with null characters.
*/
static const uint8_t *get_next_opt (const uint8_t *buf, const uint8_t *end) {
  const uint8_t *bp;

  for (bp = buf; bp < end; bp++) {
    if (bp[0] == 0) {
      bp++;
      break;
    }
  }
  if (bp == end) {
    /* Last option field */
    return (NULL);
  }
  return (bp);
}

/**
  \brief       Send option acknowledge.
  \param[in]   tftp_s  session descriptor.
*/
static void tftp_ack_opt (NET_TFTP_INFO *tftp_s) {
  uint8_t *sendbuf;
  int32_t len;

  DEBUGF (TFTP,"Send Opt-Ack\n");
  sendbuf = net_udp_get_buf (20);
  set_u16 (&sendbuf[0], TFTP_OPCOD_OACK);
  len  = net_strcpy ((char *)&sendbuf[2], "blksize") + 3;
  len += net_itoa   ((char *)&sendbuf[len], tftp_s->BlockSz) + 1;
  net_udp_send (tftp_s->Socket, &tftp_s->Client, sendbuf, (uint32_t)len);
  tftp_s->Timer = tftp->DefTout;
}

/**
  \brief       Send data block acknowledge.
  \param[in]   tftp_s  session descriptor.
*/
static void tftp_ack_block (NET_TFTP_INFO *tftp_s) {
  uint8_t *sendbuf;

  DEBUGF (TFTP,"Send Ack, Block %d\n",tftp_s->BlockNr);
  sendbuf = net_udp_get_buf (4);
  set_u16 (&sendbuf[0], TFTP_OPCOD_ACK);
  set_u16 (&sendbuf[2], tftp_s->BlockNr);
  net_udp_send (tftp_s->Socket, &tftp_s->Client, sendbuf, 4);
  tftp_s->Timer = tftp->DefTout;
}

/**
  \brief       Abort active TFTP session.
  \param[in]   tftp_s  session descriptor.
  \param[in]   erron   error number.
  \param[in]   errm    error message text.
*/
static void tftp_abort_session (NET_TFTP_INFO *tftp_s, uint8_t errno, const char *errm) {
  uint8_t *sendbuf;
  int32_t len;

  /* Send an error message */
  if (errm != NULL) {
    DEBUGF (TFTP," Send Err_%d, '%s'\n",errno,errm);
    EvrNetTFTPs_SendError (tftp_s->Id, errno);
    sendbuf = net_udp_get_buf (strlen(errm) + 5);
    set_u16 (&sendbuf[0], TFTP_OPCOD_ERR);
    set_u16 (&sendbuf[2], errno);
    len = net_strcpy ((char *)&sendbuf[4], errm) + 5;
    net_udp_send (tftp_s->Socket, &tftp_s->Client, sendbuf, (uint32_t)len);
  }

  /* Abort the session, release resources */
  DEBUGF (TFTP,"Session %d abort\n",tftp_s->Id);
  EvrNetTFTPs_AbortSession (tftp_s->Id);
  tftp_close_file (tftp_s);
  tftp_free_buf (tftp_s);
  if (!tftp->EnFwall) {
    net_udp_close (tftp_s->Socket);
  }
  tftp_s->State = TFTP_STATE_IDLE;
}

/**
  \brief       Kill active TFTP session.
  \param[in]   tftp_s  session descriptor.
*/
static void tftp_kill_session (NET_TFTP_INFO *tftp_s) {
  DEBUGF (TFTP,"Session %d close\n",tftp_s->Id);
  EvrNetTFTPs_CloseSession (tftp_s->Id);
  tftp_close_file (tftp_s);
  tftp_free_buf (tftp_s);
  if (!tftp->EnFwall) {
    net_udp_close (tftp_s->Socket);
  }
  tftp_s->State = TFTP_STATE_IDLE;
}

/**
  \brief       Close a file if opened in session.
  \param[in]   tftp_s  session descriptor.
*/
static void tftp_close_file (NET_TFTP_INFO *tftp_s) {
  if (tftp_s->File != NULL) {
    /* File has been opened, close it now */
    netTFTPs_fclose (tftp_s->File);
    tftp_s->File = NULL;
  }
}

/**
  \brief       Release a retransmit buffer if allocated.
  \param[in]   tftp_s  session descriptor.
*/
static void tftp_free_buf (NET_TFTP_INFO *tftp_s) {
  if (tftp_s->Buf != NULL) {
    /* Release existing retransmit packet */
    net_udp_free_buf (tftp_s->Buf);
    tftp_s->Buf = NULL;
  }
}

/**
  \brief       Map remote client to existing TFTP session.
  \param[in]   addr  structure containing client IP address and port.
  \return      session descriptor or NULL= none.
*/
static NET_TFTP_INFO *tftp_map_session (const __ADDR *addr) {
  NET_TFTP_INFO *tftp_s;
  int32_t i;

  for (tftp_s = &tftp->Scb[0], i = 0; i < tftp->NumSess; tftp_s++, i++) {
    if ((tftp_s->State != TFTP_STATE_IDLE)  &&
        (tftp_s->Client.port == addr->port) &&
         net_addr_comp (&tftp_s->Client, addr)) {
      /* Existing session found */
      return (tftp_s);
    }
  }
  return (NULL);
}

/**
  \brief       Allocate and open new TFTP session for the client.
  \param[in]   addr  structure containing client IP address and port.
  \return      session descriptor or NULL= none.
*/
static NET_TFTP_INFO *tftp_open_session (const __ADDR *addr) {
  NET_TFTP_INFO *tftp_s;
  int32_t i;

  for (tftp_s = &tftp->Scb[0], i = 0; i < tftp->NumSess; tftp_s++, i++) {
    if (tftp_s->State == TFTP_STATE_IDLE) {
      /* Register remote IP and port */
      net_addr_copy (&tftp_s->Client, addr);
      tftp_s->State = TFTP_STATE_OPEN;
      /* New session allocated */
      tftp_s->Id = (i + 1) & 0xFF;
      return (tftp_s);
    }
  }
  /* No free sessions */
  return (NULL);
}

/**
  \brief       Register a retransmission frame.
  \param[in]   tftp_s  session descriptor.
  \param[in]   buf     buffer containing the data.
  \param[in]   len     length of data.
*/
static void tftp_set_retransmit (NET_TFTP_INFO *tftp_s, uint8_t *buf, uint32_t len) {
  if (tftp_s->Buf != NULL) {
    /* Release existing packet first */
    net_udp_free_buf (tftp_s->Buf);
  }
  tftp_s->Buf    = buf;
  tftp_s->BufLen = len & 0xFFFF;
}

#ifdef DEBUG_STDIO
/**
  \brief       Debug print user friendly Opcode.
  \param[in]   opcode  TFTP operation code.
*/
static void debug_info (uint16_t opcode) {
  static const char p_asc[][5] = {
    "RRQ",
    "WRQ",
    "DATA",
    "ACK",
    "ERR",
    "OACK"
  };

  if ((opcode < TFTP_OPCOD_RRQ) || (opcode > TFTP_OPCOD_OACK)) {
    DEBUGF (TFTP," Opcode Unknown (%d)\n",opcode);
    return;
  }
  DEBUGF (TFTP," Opcode %s\n",p_asc[opcode-1]);
}
#endif

/* Library default function(s) */
__WEAK bool netTFTPs_AcceptClient (const NET_ADDR *addr) {
  (void)addr; return (true); }
