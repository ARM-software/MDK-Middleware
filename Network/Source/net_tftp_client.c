/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_tftp_client.c
 * Purpose: Trivial File Transfer Protocol Client
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_udp.h"
#include "net_tftp_client.h"
#include "net_dbg.h"

/* Local variables */
static NET_TFTPC_INFO tftpc_control;
#define tftpc_s     (&tftpc_control)

/* Local Functions */
static uint32_t tftpc_listener (int32_t socket, const NET_ADDR *addr,
                                                const uint8_t *buf, uint32_t len);
static void tftpc_send_req (uint16_t opcode, const char *fname);
static void tftpc_send_ack (void);
static void tftpc_send_err (uint8_t errno, const char *errm);
static void tftpc_send (uint8_t *buf, uint32_t len);
static void tftpc_set_retransmit (uint8_t *buf, uint32_t len);
static void tftpc_stop (netTFTPc_Event evt);
static bool tftpc_proc_opt (const uint8_t *buf, uint32_t len);
static const uint8_t *get_next_opt (const uint8_t *buf, const uint8_t *end);
#ifdef DEBUG_STDIO
 static void debug_info (const __ADDR *addr);
#endif

/**
  \brief       Initialize TFTP client.
  \return      none.
*/
void net_tftp_client_init (void) {
  int32_t sock;

  DEBUGF (TFTPC,"Init Client\n");
  EvrNetTFTPc_InitClient ();
  memset (tftpc_s, 0, sizeof (*tftpc_s));
  sock = net_udp_get_socket (tftpc_listener);
  if (sock < 0) sock = 0;
  tftpc_s->Socket = sock & 0xFF;
  if (tftpc_s->Socket == 0) {
    ERRORF (TFTPC,"Get socket failed\n");
    EvrNetTFTPc_GetSocketFailed ();
    net_sys_error (NET_ERROR_UDP_ALLOC);
    return;
  }
}

/**
  \brief       De-initialize TFTP client.
  \return      none.
*/
void net_tftp_client_uninit (void) {
  DEBUGF (TFTPC,"Uninit Client\n");
  EvrNetTFTPc_UninitClient ();

  /* Close local file if open */
  if (tftpc_s->File != NULL) {
    netTFTPc_fclose (tftpc_s->File);
  }
  memset (tftpc_s, 0, sizeof (*tftpc_s));
}

/**
  \brief       Put a file to remote TFTP server.
  \param[in]   addr         structure containing server IP address and port.
  \param[in]   fname        remote file name, a null-terminated string.
  \param[in]   local_fname  local file name, a null-terminated string.
  \return      status code as defined with netStatus.
*/
netStatus netTFTPc_Put (const NET_ADDR *addr,
                        const char *fname, const char *local_fname) {

  START_LOCK (netStatus);

  if ((addr == NULL) || (local_fname == NULL)) {
    ERRORF (TFTPC,"Put, Invalid parameter\n");
    EvrNetTFTPc_PutInvalidParameter ();
    RETURN (netInvalidParameter);
  }
  DEBUGF (TFTPC,"Put file '%s'\n",local_fname);
  EvrNetTFTPc_PutFile (local_fname, sizeof(local_fname));
  if (net_addr_is_unspec ((const __ADDR *)addr)) {
    /* Server IP address not specified */
    ERRORF (TFTPC,"Put, Server IP unspecified\n");
    EvrNetTFTPc_PutInvalidParameter ();
    RETURN (netInvalidParameter);
  }
  if (tftpc_s->State != TFTPC_STATE_IDLE) {
    ERRORF (TFTPC,"Put, Wrong client state\n");
    EvrNetTFTPc_PutWrongState (tftpc_s->State);
    RETURN (netWrongState);
  }
  if (fname == NULL) {
    /* Destination not specified, use the source */
    fname = local_fname;
  }
  else {
    DEBUGF (TFTPC," Remote file '%s'\n",fname);
    EvrNetTFTPc_PutRemoteName (fname, sizeof(fname));
  }
  net_addr_copy (&tftpc_s->Server, (const __ADDR *)addr);
  if (tftpc_s->Server.port == 0) {
    /* Port not specified, use default server port */
    tftpc_s->Server.port = TFTPC_SERVER_PORT;
  }
  DEBUG_INFO (&tftpc_s->Server);
  EvrNetTFTPc_ShowServerAddress (&tftpc_s->Server);

  tftpc_s->BlockSz = tftpc->BlockSize;
  tftpc_s->Flags   = 0;
  tftpc_s->BlockNr = 0;
  DEBUGF (TFTPC," Local fopen\n");
  EvrNetTFTPc_OpenLocalFile ();
  tftpc_s->File    = netTFTPc_fopen (local_fname, "rb");
  if (tftpc_s->File == NULL) {
    /* Failed to open local file. */
    ERRORF (TFTPC,"Local, fopen failed\n");
    EvrNetTFTPc_OpenLocalFileFailed ();
    RETURN (netFileError);
  }
  /* Open TFTP Client UDP port for communication. */
  net_udp_open (tftpc_s->Socket, 0);
  tftpc_send_req (TFTPC_OPCOD_WRQ, fname);
  tftpc_s->Flags |= TFTPC_FLAG_WACK;
  tftpc_s->State  = TFTPC_STATE_PUT;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Retrieve a file from remote TFTP server.
  \param[in]   addr         structure containing server IP address and port.
  \param[in]   fname        remote file name, a null-terminated string.
  \param[in]   local_fname  local file name, a null-terminated string.
  \return      status code as defined with netStatus.
*/
netStatus netTFTPc_Get (const NET_ADDR *addr,
                        const char *fname, const char *local_fname) {

  START_LOCK (netStatus);

  if ((addr == NULL) || (fname == NULL)) {
    ERRORF (TFTPC,"Get, Invalid parameter\n");
    EvrNetTFTPc_GetInvalidParameter ();
    RETURN (netInvalidParameter);
  }
  DEBUGF (TFTPC,"Get file '%s'\n",fname);
  EvrNetTFTPc_GetFile (fname, sizeof(fname));
  if (net_addr_is_unspec ((const __ADDR *)addr)) {
    /* Server IP address not specified */
    ERRORF (TFTPC,"Get, Server IP unspecified\n");
    EvrNetTFTPc_GetInvalidParameter ();
    RETURN (netInvalidParameter);
  }
  if (tftpc_s->State != TFTPC_STATE_IDLE) {
    ERRORF (TFTPC,"Get, Wrong client state\n");
    EvrNetTFTPc_GetWrongState (tftpc_s->State);
    RETURN (netWrongState);
  }
  if (local_fname == NULL) {
    /* Destination not specified, use the source. */
    local_fname = fname;
  }
  else {
    DEBUGF (TFTPC," Local file '%s'\n",local_fname);
    EvrNetTFTPc_GetLocalName (local_fname, sizeof(local_fname));
  }
  net_addr_copy (&tftpc_s->Server, (const __ADDR *)addr);
  if (tftpc_s->Server.port == 0) {
    /* Port not specified, use default server port */
    tftpc_s->Server.port = TFTPC_SERVER_PORT;
  }
  DEBUG_INFO (&tftpc_s->Server);
  EvrNetTFTPc_ShowServerAddress (&tftpc_s->Server);

  tftpc_s->BlockSz = tftpc->BlockSize;
  tftpc_s->Flags   = 0;
  tftpc_s->BlockNr = 0;
  DEBUGF (TFTPC," Local fopen\n");
  EvrNetTFTPc_OpenLocalFile ();
  tftpc_s->File    = netTFTPc_fopen (local_fname, "wb");
  if (tftpc_s->File == NULL) {
    /* Failed to open local file */
    ERRORF (TFTPC,"Local, fopen failed\n");
    EvrNetTFTPc_OpenLocalFileFailed ();
    RETURN (netFileError);
  }
  /* Open TFTP Client UDP port for communication. */
  net_udp_open (tftpc_s->Socket, 0);
  tftpc_send_req (TFTPC_OPCOD_RRQ, fname);
  tftpc_s->State = TFTPC_STATE_GET;
  RETURN (netOK);

  END_LOCK;
}

/**
  \brief       Run TFTP client main function.
  \return      none.
*/
void net_tftp_client_run (void) {
  uint8_t *sendbuf;
  int32_t len;

  if (tftpc_s->State == TFTPC_STATE_IDLE) {
    return;
  }

  if ((sys->Flags & SYS_FLAG_TICK) && tftpc_s->RetryTimer) {
    if (--tftpc_s->RetryTimer == 0) {
      if (tftpc_s->Retries != 0) {
        /* Retransmit the packet */
        DEBUGF (TFTPC,"Timeout, retransmit\n");
        EvrNetTFTPc_TimeoutBlockRetransmit (tftpc_s->BlockNr);
        tftpc_s->Retries--;
        net_udp_keep_buf (tftpc_s->Socket, tftpc_s->Buf);
        net_udp_send (tftpc_s->Socket, &tftpc_s->Server,
                      tftpc_s->Buf, tftpc_s->BufLen);
        tftpc_s->RetryTimer = tftpc->RetryTout;
        return;
      }
      /* Retries used-up, stop the client */
      tftpc_stop (netTFTPc_EventTimeout);
    }
  }

  switch (tftpc_s->State) {
    case TFTPC_STATE_PUT:
      /* Read local file data and send it */
      if (tftpc_s->Flags & TFTPC_FLAG_WACK) {
        /* Waiting for a response from server */
        break;
      }
      sendbuf = net_udp_get_buf (tftpc_s->BlockSz + 4);
      set_u16 (&sendbuf[0], TFTPC_OPCOD_DATA);
      set_u16 (&sendbuf[2], tftpc_s->BlockNr);
      len = (int32_t)netTFTPc_fread (tftpc_s->File, &sendbuf[4], tftpc_s->BlockSz);
      tftpc_s->Flags |= TFTPC_FLAG_WACK;
      if (len < tftpc_s->BlockSz) {
        /* This is the last block */
        tftpc_s->Flags |= TFTPC_FLAG_LASTB;
      }
      len += 4;
      DEBUGF (TFTPC,"Send Block %d, %d bytes\n",tftpc_s->BlockNr,len);
      EvrNetTFTPc_SendBlock (tftpc_s->BlockNr, (uint32_t)len);
      tftpc_send (sendbuf, (uint32_t)len);
      break;

    case TFTPC_STATE_GET:
      /* Write a local file */
      break;

    case TFTPC_STATE_STOP:
      /* Stop TFTP Client */
      DEBUGF (TFTPC,"Client stop, close\n");
      EvrNetTFTPc_StopClient ();
      if (tftpc_s->File != NULL) {
        DEBUGF (TFTPC," Local fclose\n");
        EvrNetTFTPc_CloseLocalFile ();
        netTFTPc_fclose (tftpc_s->File);
        tftpc_s->File = NULL;
      }
      tftpc_set_retransmit (NULL, 0);
      net_udp_close (tftpc_s->Socket);
      netTFTPc_Notify (tftpc_s->cb_event);
      tftpc_s->State = TFTPC_STATE_IDLE;
      break;
  }
}

/**
  \brief       Socket event callback notification.
  \param[in]   socket  UDP socket handle.
  \param[in]   addr    structure containing remote IP address and port.
  \param[in]   buf     pointer to a buffer containing the data.
  \param[in]   len     length of the data.
  \return      status:
               - true  = success,
               - false = error.
*/
static uint32_t tftpc_listener (int32_t socket, const NET_ADDR *addr,
                                                const uint8_t *buf, uint32_t len) {
  uint16_t opcode,blocknr,err;
  uint8_t *sendbuf;

  /* Check if this is our socket */
  if (tftpc_s->Socket != socket) {
    return (false);
  }

  DEBUGF (TFTPC,"*** Process_frame ***\n");
  DEBUGF (TFTPC," Length %d bytes\n",len);
  EvrNetTFTPc_ReceiveFrame (len);

  if (!net_addr_comp ((const __ADDR *)addr, &tftpc_s->Server)) {
    DEBUG_INFO ((const __ADDR *)addr);
    ERRORF (TFTPC,"Process, Server IP wrong\n");
    EvrNetTFTPc_WrongServerAddress (addr);
    return (false);
  }
  if (addr->port != tftpc_s->Server.port) {
    DEBUG_INFO ((const __ADDR *)addr);
    if (tftpc_s->Flags & TFTPC_FLAG_TID) {
      /* TID already assigned, send error response. */
      ERRORF (TFTPC,"Process, Wrong port\n");
      EvrNetTFTPc_WrongServerPort (addr->port);
      sendbuf = net_udp_get_buf (14);
      set_u16  (&sendbuf[0], TFTPC_OPCOD_ERR);
      set_u16  (&sendbuf[2], TFTPC_ERR_UNKNTID);
      net_strcpy ((char *)&sendbuf[4], "Wrong TID");
      net_udp_send (tftpc_s->Socket, (const __ADDR *)addr, sendbuf, 14);
      return (false);
    }
    DEBUGF (TFTPC," Server TID assigned\n");
    EvrNetTFTPc_ServerTidAssigned (addr->port);
    tftpc_s->Flags  |= TFTPC_FLAG_TID;
    tftpc_s->Server.port = addr->port;
  }
  /* Inital size checking */
  if (len < 4) {
    ERRORF (TFTPC,"Process, Frame too short\n");
    EvrNetTFTPc_FrameTooShort (len, 4);
    return (false);
  }

  /* Get the Opcode and process it */
  opcode = get_u16 (&buf[0]);
  switch (tftpc_s->State) {
    case TFTPC_STATE_PUT:
      /* Write Request command given */
      if (opcode == TFTPC_OPCOD_ERR) {
        /* Get the error code. */
        err = get_u16 (&buf[2]);
        EvrNetTFTPc_ErrorCodeReceived (err);
        switch (err) {
          case TFTPC_ERR_NOACCESS:
            ERRORF (TFTPC,"Server, Access violation\n");
            tftpc_stop (netTFTPc_EventAccessDenied);
            break;
          case TFTPC_ERR_DISKFULL:
            ERRORF (TFTPC,"Server, Disk full\n");
            tftpc_stop (netTFTPc_EventDiskFull);
            break;
          default:
            ERRORF (TFTPC,"Server, Err_%d recvd\n",err);
            tftpc_stop (netTFTPc_EventError);
            break;
        }
        return (false);
      }
      if (tftpc_s->Flags & TFTPC_FLAG_OPT) {
        tftpc_s->Flags &= ~TFTPC_FLAG_OPT;
        if (opcode == TFTPC_OPCOD_OACK) {
          if (tftpc_proc_opt (buf, len) == true) {
            /* Options ok, start data transfer */
            goto ack_ok;
          }
          break;
        }
        /* Server does not support option negotiation */
        tftpc_s->BlockSz = 512;
      }
      if (opcode != TFTPC_OPCOD_ACK) {
        /* Illegal opcode */
        break;
      }
      blocknr = get_u16(&buf[2]);
      if (tftpc_s->BlockNr == (blocknr + 1)) {
        /* Previous block acked again, maybe our packet was lost.*/
        /* Let the retransmission handle it and resend the data. */
        DEBUGF (TFTPC," Block %d acked again\n",blocknr);
        EvrNetTFTPc_DuplicateBlockAck (blocknr);
        return (true);
      }
      if (tftpc_s->BlockNr != blocknr) {
        /* Out of sequence block number */
        ERRORF (TFTPC,"Server, Invalid block %d acked\n",blocknr);
        EvrNetTFTPc_InvalidBlockAck (blocknr);
        goto err_bl;
      }
      DEBUGF (TFTPC," Block %d acked\n",blocknr);
      EvrNetTFTPc_BlockAckReceived (blocknr);
ack_ok:tftpc_s->BlockNr++;
      tftpc_s->Flags &= ~TFTPC_FLAG_WACK;
      tftpc_set_retransmit (NULL, 0);
      if (tftpc_s->Flags & TFTPC_FLAG_LASTB) {
        /* Ok, we are done with transfer. */
        tftpc_stop (netTFTPc_EventSuccess);
      }
      return (true);

    case TFTPC_STATE_GET:
      /* Read Request command given */
      if (opcode == TFTPC_OPCOD_ERR) {
        /* Get the error code. */
        err = get_u16 (&buf[2]);
        EvrNetTFTPc_ErrorCodeReceived (err);
        switch (err) {
          case TFTPC_ERR_NOTFOUND:
            ERRORF (TFTPC,"Server, File not found\n");
            tftpc_stop (netTFTPc_EventFileNotFound);
            break;
          case TFTPC_ERR_NOACCESS:
            ERRORF (TFTPC,"Server, No access\n");
            tftpc_stop (netTFTPc_EventAccessDenied);
            break;
          default:
            ERRORF (TFTPC,"Server, Err_%d recvd\n",err);
            tftpc_stop (netTFTPc_EventError);
            break;
        }
        return (false);
      }
      if (tftpc_s->Flags & TFTPC_FLAG_OPT) {
        tftpc_s->Flags &= ~TFTPC_FLAG_OPT;
        if (opcode == TFTPC_OPCOD_OACK) {
          if (tftpc_proc_opt (buf, len) == false) {
            break;
          }
          tftpc_send_ack ();
          tftpc_s->BlockNr = 1;
          return (true);
        }
        /* Server does not support option negotiation */
        tftpc_s->BlockSz = 512;
        tftpc_s->BlockNr = 1;
      }
      if (opcode != TFTPC_OPCOD_DATA) {
        /* Illegal opcode */
        break;
      }
      len -= 4;
      blocknr = get_u16(&buf[2]);
      if (tftpc_s->BlockNr == (blocknr + 1)) {
        /* Previous block received again, maybe our ack was lost */
        /* Let the retransmission handle it and resend the ack.  */
        DEBUGF (TFTPC," Block %d recvd again\n",blocknr);
        EvrNetTFTPc_DuplicateBlockReceived (blocknr);
        return (true);
      }
      if (tftpc_s->BlockNr != blocknr) {
        /* Out of sequence block number */
        ERRORF (TFTPC,"Server, Invalid block %d recvd\n",blocknr);
        EvrNetTFTPc_InvalidBlockReceived (blocknr);
err_bl: tftpc_send_err (TFTPC_ERR_NOTDEF, "Invalid block");
        tftpc_stop (netTFTPc_EventError);
        return (false);
      }
      DEBUGF (TFTPC," Block %d recvd\n",blocknr);
      EvrNetTFTPc_BlockReceived (blocknr);
      if (netTFTPc_fwrite (tftpc_s->File, &buf[4], len) != len) {
        /* Write failure, abort the connection */
        ERRORF (TFTPC,"Local, Disk full\n");
        EvrNetTFTPc_WriteErrorDiskFull (len);
        tftpc_send_err (TFTPC_ERR_DISKFULL, "Disk full");
        tftpc_stop (netTFTPc_EventLocalFileError);
        return (false);
      }
      tftpc_send_ack ();
      if (len == tftpc_s->BlockSz) {
        /* Full size block, more to come */
        tftpc_s->BlockNr++;
      }
      else {
        tftpc_stop (netTFTPc_EventSuccess);
      }
      return (true);
  }
  ERRORF (TFTPC,"Server, Illegal operation\n");
  EvrNetTFTPc_IllegalServerOperation ();
  tftpc_send_err (TFTPC_ERR_ILLOPCOD, "Illegal operation");
  tftpc_stop (netTFTPc_EventError);
  return (false);
}

/**
  \brief       Send TFTP read or write request.
  \param[in]   opcode  request opcode.
  \param[in]   fname   pointer to file name.
  \return      none.
*/
static void tftpc_send_req (uint16_t opcode, const char *fname) {
  uint8_t *sendbuf;
  int32_t len;

  DEBUGF (TFTPC,"Send %s request\n",(opcode==TFTPC_OPCOD_RRQ)?"Read":"Write");
  DEBUGF (TFTPC," Mode  : octet\n");
  EvrNetTFTPc_SendRequest (opcode);
  len = (int32_t)strlen(fname) + 22;
  sendbuf = net_udp_get_buf ((uint32_t)len);
  set_u16 (&sendbuf[0], opcode);
  len  = net_strcpy ((char *)&sendbuf[2], fname) + 3;
  len += net_strcpy ((char *)&sendbuf[len], "octet") + 1;
  if (tftpc_s->BlockSz != 512) {
    /* Append option for not default block size. */
    DEBUGF (TFTPC," BlSize: %d\n",tftpc_s->BlockSz);
    EvrNetTFTPc_OptionBlockSize (tftpc_s->BlockSz);
    len += net_strcpy ((char *)&sendbuf[len], "blksize") + 1;
    len += net_itoa   ((char *)&sendbuf[len], tftpc_s->BlockSz) + 1;
  }
  tftpc_send (sendbuf, (uint32_t)len);
  tftpc_s->Flags |= TFTPC_FLAG_OPT;
}

/**
  \brief       Send TFTP ack frame.
  \return      none.
*/
static void tftpc_send_ack (void) {
  uint8_t *sendbuf;

  DEBUGF (TFTPC,"Send Ack, Block %d\n",tftpc_s->BlockNr);
  EvrNetTFTPc_SendAck (tftpc_s->BlockNr);
  sendbuf = net_udp_get_buf (4);
  set_u16 (&sendbuf[0], TFTPC_OPCOD_ACK);
  set_u16 (&sendbuf[2], tftpc_s->BlockNr);
  tftpc_send (sendbuf, 4);
}

/**
  \brief       Send TFTP error frame.
  \param[in]   erron  error number.
  \param[in]   errm   error message text.
  \return      none.
*/
static void tftpc_send_err (uint8_t errno, const char *errm) {
  uint8_t *sendbuf;
  int32_t len;

  DEBUGF (TFTPC,"Send Err_%d, '%s'\n",errno,errm);
  EvrNetTFTPc_SendError (errno);
  len = (int32_t)strlen(errm) + 5;
  sendbuf = net_udp_get_buf ((uint32_t)len);
  set_u16 (&sendbuf[0], TFTPC_OPCOD_ERR);
  set_u16 (&sendbuf[2], errno);
  net_strcpy ((char *)&sendbuf[4], errm);
  tftpc_send (sendbuf, (uint32_t)len);
}

/**
  \brief       Send TFTP frame.
  \param[in]   buf  frame data buffer.
  \param[in]   len  length of the data.
  \return      none.
*/
static void tftpc_send (uint8_t *buf, uint32_t len) {
  net_udp_keep_buf (tftpc_s->Socket, buf);
  net_udp_send (tftpc_s->Socket, &tftpc_s->Server, buf, len);
  /* The packet was not released by net_udp_send() */
  tftpc_set_retransmit (buf, len);
  tftpc_s->RetryTimer = tftpc->RetryTout;
  tftpc_s->Retries    = tftpc->MaxRetry;
}

/**
  \brief       Register a retransmission frame.
  \param[in]   buf  buffer containing the data.
  \param[in]   len  length of data.
  \return      none.
*/
static void tftpc_set_retransmit (uint8_t *buf, uint32_t len) {
  if (tftpc_s->Buf != NULL) {
    /* Release existing packet first */
    net_udp_free_buf (tftpc_s->Buf);
  }
  tftpc_s->Buf    = buf;
  tftpc_s->BufLen = len & 0xFFFF;
}

/**
  \brief       Stop TFTP client.
  \param[in]   evt  stop notification event.
  \return      none.
*/
static void tftpc_stop (netTFTPc_Event evt) {
  tftpc_s->cb_event   = evt;
  tftpc_s->State      = TFTPC_STATE_STOP;
  tftpc_s->RetryTimer = 0;
}

/**
  \brief       Process ack frame options.
  \param[in]   buf  buffer containing the options.
  \param[in]   len  length of options.
  \return      status:
               - true  = "blksize" option valid,
               - false = not valid.
*/
static bool tftpc_proc_opt (const uint8_t *buf, uint32_t len) {
  const uint8_t *np,*end;
  int32_t val;

  DEBUGF (TFTPC," Option acked\n");
  EvrNetTFTPc_OptionAckReceived ();
  /* Process the options */
  for (np = buf, end = buf + len; np; (np = get_next_opt (np, end))) {
    if (net_strcasecmp ((const char *)np, "blksize")) {
      np += 8;
      val = net_atoi ((const char *)np, NULL);
      if (val == tftpc_s->BlockSz) {
        break;
      }
      if ((val < 8) || (val > 1428)) {
        return (false);
      }
      tftpc_s->BlockSz = val & 0xFFFF;
      break;
    }
  }
  return (true);
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

#ifdef DEBUG_STDIO
/**
  \brief       Debug print IP address and port.
  \param[in]   addr  structure containing IP address and port.
  \return      none.
*/
static void debug_info (const __ADDR *addr) {
  DEBUGF (TFTPC," Server [%s], port %d\n",net_addr_ntoa(addr),addr->port);
}
#endif
