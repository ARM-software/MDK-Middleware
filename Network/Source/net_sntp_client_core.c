/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_sntp_client.c
 * Purpose: Simple Network Time Protocol Client
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_addr.h"
#include "net_udp.h"
#include "net_sntp_client.h"
#include "net_dbg.h"

/* Global variables */
NET_SNTP_CTRL net_sntp_control;

/* Local Functions */
static uint32_t  sntp_listener (int32_t socket, const NET_ADDR *addr,
                                                const uint8_t *buf, uint32_t len);
static netStatus sntp_set_mode (netSNTPc_Mode mode);
static netStatus sntp_send_message (void);
static void sntp_stop (void);
#ifdef DEBUG_STDIO
 static void debug_info (const __ADDR *addr);
#endif

/* ==== Protected SNTP Client API functions ==== */

netStatus netSNTPc_GetTime (const NET_ADDR *addr, netSNTPc_cb_t cb_func) {
  START_LOCK (netStatus);
  RETURN (net_sntp_client_start ((const __ADDR *)addr, cb_func));
  END_LOCK;
}
netStatus netSNTPc_SetMode (netSNTPc_Mode mode) {
  START_LOCK (netStatus);
  RETURN (sntp_set_mode (mode));
  END_LOCK;
}

/* ==== Internal SNTP Client functions ==== */

/**
  \brief       Initialize SNTP client.
*/
void net_sntp_client_init (void) {
  int32_t sock;

  DEBUGF (SNTP,"Init Client, %scast mode\n",sntpc->Bcast ? "B" : "Uni");
  EvrNetSNTP_InitClient (sntpc->Bcast);
  sntp->State   = SNTP_STATE_IDLE;
  sntp->Flags   = sntpc->Bcast ? SNTP_FLAG_BCAST : 0;
  sntp->Retries = 0;
  sntp->Timer   = 0;
  sntp->cb_func = NULL;
  sock = net_udp_get_socket (sntp_listener);
  if (sock < 0) sock = 0;
  sntp->Socket = sock & 0xFF;
  if (sntp->Socket == 0) {
    ERRORF (SNTP,"Get socket failed\n");
    EvrNetSNTP_GetSocketFailed ();
    net_sys_error (NET_ERROR_UDP_ALLOC);
    return;
  }
}

/**
  \brief       De-initialize SNTP client.
*/
void net_sntp_client_uninit (void) {
  DEBUGF (SNTP,"Uninit Client\n");
  EvrNetSNTP_UninitClient ();
  /* Clear SNTP client control */
  net_sys_resume (&sntp->Thread);
  memset (sntp, 0, sizeof(*sntp));
}

/**
  \brief       Start SNTP client to get time from NTP/SNTP time server.
  \param[in]   addr     structure containing IP address of the server.
                        - NULL to use IP address from system configuration.
  \param[in]   cb_func  callback function to call, when session ends.
  \return      status code as defined with netStatus.
*/
netStatus net_sntp_client_start (const __ADDR *addr, netSNTPc_cb_t cb_func) {

  /* Only IPv4 currently supported */
  if ((addr != NULL) && (addr->addr_type != NET_ADDR_IP4)) {
    ERRORF (SNTP,"GetTime, Invalid parameter\n");
    EvrNetSNTP_GetTimeInvalidParameter ();
    return (netInvalidParameter);
  }

  if (sntp->Flags & SNTP_FLAG_BCAST) {
    /* Broadcast client mode */
    DEBUGF (SNTP,"GetTime bcast\n");
    EvrNetSNTP_GetTime (netSNTPc_ModeBroadcast);
    switch (sntp->State) {
      case SNTP_STATE_IDLE:
        if (cb_func == NULL) {
          ERRORF (SNTP,"GetTime, Invalid parameter\n");
          EvrNetSNTP_GetTimeInvalidParameter ();
          return (netInvalidParameter);
        }
        /* Register a callback notification function */
        sntp->cb_func = cb_func;
        DEBUGF (SNTP,"Service enable\n");
        /* Check server address */
        if (addr == NULL) {
          /* Use NTP Server from system configuration */
          net_addr4_aton (sntpc->SrvIp, sntp->Server.addr);
          sntp->Server.addr_type = NET_ADDR_IP4;
        }
        else {
          net_addr_copy (&sntp->Server, (const __ADDR *)addr);
        }
        EvrNetSNTP_GetTimeOpen (sntp->Server.addr);
        if (net_addr_is_unspec (&sntp->Server)) {
          sntp->Flags &= ~SNTP_FLAG_FILTER;
          DEBUGF (SNTP," Accept ANY Server\n");
        }
        else {
          sntp->Flags |= SNTP_FLAG_FILTER;
          DEBUG_INFO (&sntp->Server);
        }
        /* Open socket for receiving bcast NTP messages */
        net_udp_open (sntp->Socket, SNTP_SERVER_PORT);
        sntp->State = SNTP_STATE_BCAST;
        return (netOK);

      case SNTP_STATE_BCAST:
        if (cb_func != NULL) {
          ERRORF (SNTP,"GetTime, Invalid parameter\n");
          EvrNetSNTP_GetTimeInvalidParameter ();
          return (netInvalidParameter);
        }
        DEBUGF (SNTP,"Service disable\n");
        EvrNetSNTP_GetTimeClose ();
        net_udp_close (sntp->Socket);
        sntp->State = SNTP_STATE_IDLE;
        return (netOK);

      default:
        ERRORF (SNTP,"GetTime, Wrong client state\n");
        EvrNetSNTP_GetTimeWrongState (sntp->State);
        return (netWrongState);
    }
  }

  /* Unicast client mode */
  DEBUGF (SNTP,"GetTime request\n");
  if (cb_func == NULL) {
    ERRORF (SNTP,"GetTime, Invalid parameter\n");
    EvrNetSNTP_GetTimeInvalidParameter ();
    return (netInvalidParameter);
  }
  if (sntp->State != SNTP_STATE_IDLE) {
    ERRORF (SNTP,"GetTime, Wrong client state\n");
    EvrNetSNTP_GetTimeWrongState (sntp->State);
    return (netWrongState);
  }
  EvrNetSNTP_GetTime (netSNTPc_ModeUnicast);
  sntp->cb_func = cb_func;

  /* Check server address */
  if (addr == NULL) {
    /* Use NTP Server from system configuration */
    net_addr4_aton (sntpc->SrvIp, sntp->Server.addr);
    sntp->Server.addr_type = NET_ADDR_IP4;
  }
  else if (addr != &sntp->Server) {
    net_addr_copy (&sntp->Server, addr);
  }
  sntp->Server.port = SNTP_SERVER_PORT;
  if (net_addr_is_unspec (&sntp->Server)) {
    ERRORF (SNTP,"GetTime, Server unknown\n");
    EvrNetSNTP_GetTimeServerUnknown (sntp->Server.addr);
    return (netServerError);
  }

  /* Open UDP port for communication */
  net_udp_open (sntp->Socket, 0);
  sntp->Retries = 3;
  sntp->Timer   = 1*SYS_TICK_TSEC;
  sntp->State   = SNTP_STATE_BUSY;
  if (sntp_send_message () != netOK) {
    sntp->Timer = 1;
  }
  return (netOK);
}

/**
  \brief       Set mode of operation for SNTP client.
  \param[in]   mode  SNTP client operation mode.
  \return      status code as defined with netStatus.
*/
static netStatus sntp_set_mode (netSNTPc_Mode mode) {

  if (sntp->State != SNTP_STATE_IDLE) {
    ERRORF (SNTP,"SetMode, Wrong client state\n");
    EvrNetSNTP_SetModeWrongState (sntp->State);
    return (netWrongState);
  }
  switch ((int32_t)mode) {
    case netSNTPc_ModeUnicast:
      sntp->Flags &= ~SNTP_FLAG_BCAST;
      DEBUGF (SNTP,"Set Unicast mode\n");
      break;

    case netSNTPc_ModeBroadcast:
      sntp->Flags |= SNTP_FLAG_BCAST;
      DEBUGF (SNTP,"Set Bcast mode\n");
      break;

    default:
      ERRORF (SNTP,"SetMode, Invalid parameter\n");
      EvrNetSNTP_SetModeInvalidParameter ();
      return (netInvalidParameter);
  }
  EvrNetSNTP_SetMode (mode);
  return (netOK);
}

/**
  \brief       Run SNTP client main function.
*/
void net_sntp_client_run (void) {
  if (sntp->State != SNTP_STATE_BUSY) {
    return;
  }
  if (sntp->Timer) {
    if (sys->Flags & SYS_FLAG_TICK) {
      /* Update interval 1 tick */
      sntp->Timer--;
    }
    return;
  }
  sys->Busy = true;

  if (sntp->Retries) {
    sntp->Retries--;
    DEBUGF (SNTP,"Retransmit request\n");
    sntp_send_message ();
    sntp->Timer = 2*SYS_TICK_TSEC;
    return;
  }
  /* All retries used up, inform user */
  DEBUGF (SNTP,"Timeout, Server not responding\n");
  EvrNetSNTP_ServerNotResponding (sntp->Server.addr);
  sntp->cb_func (0, 0);
  sntp_stop ();
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
static uint32_t sntp_listener (int32_t socket, const NET_ADDR *addr,
                                               const uint8_t *buf, uint32_t len) {
  uint32_t sec;

  /* Check if this is our socket and version IP4 */
  if ((sntp->Socket != socket) || (addr->addr_type != NET_ADDR_IP4)) {
    return (false);
  }
  DEBUGF (SNTP,"*** Process_frame ***\n");
  DEBUG_INFO ((const __ADDR *)addr);
  EvrNetSNTP_ReceiveFrame (addr->addr, len);

  /* Check server port */
  if (addr->port != SNTP_SERVER_PORT) {
    ERRORF (SNTP,"Process, Wrong Server port\n");
    EvrNetSNTP_WrongServerPort (addr->port);
    return (false);
  }

  /* Initial size checking */
  if (len < 48) {
    ERRORF (SNTP,"Process, Frame too short\n");
    EvrNetSNTP_FrameTooShort (len, 48);
    return (false);
  }

  /* Check server IP and mode */
  switch (sntp->State) {
    case SNTP_STATE_BUSY:
      /* Unicast client mode */
      if (!net_addr_comp ((const __ADDR *)addr, &sntp->Server)) {
        ERRORF (SNTP,"Process, Wrong Server addr\n");
        EvrNetSNTP_WrongServerAddress (addr->addr);
        return (false);
      }
      if ((NTP_FRAME(buf)->VerMode & 0x07) != 4) {
        ERRORF (SNTP,"Process, Mode not server\n");
        EvrNetSNTP_ModeNotServer ((NTP_FRAME(buf)->VerMode & 0x07));
        return (false);
      }
      break;

    case SNTP_STATE_BCAST:
      /* Broadcast client mode */
      if ((sntp->Flags & SNTP_FLAG_FILTER) &&
          !net_addr_comp ((const __ADDR *)addr, &sntp->Server)) {
        ERRORF (SNTP,"Process, Wrong Server addr\n");
        EvrNetSNTP_WrongServerAddress (addr->addr);
        return (false);
      }
      if ((NTP_FRAME(buf)->VerMode & 0x07) != 5) {
        /* Mode field not set to broadcast */
        ERRORF (SNTP,"Process, Mode not bcast\n");
        EvrNetSNTP_ModeNotBroadcast ((NTP_FRAME(buf)->VerMode & 0x07));
        return (false);
      }
      break;

    default:
      ERRORF (SNTP,"Process, Answer in wrong state\n");
      EvrNetSNTP_AnswerInWrongState (sntp->State);
      return (false);
  }

  /* Read Transmit Timestamp integer seconds */
  sec = get_u32 (&NTP_FRAME(buf)->Tx.sec);
  if (sec > SNTP_TIME1970) {
    /* Return time in UTC time format */
    sec -= SNTP_TIME1970;
    DEBUGF (SNTP," TxTime: %u, utc=%u\n",sec+SNTP_TIME1970,sec);
    EvrNetSNTP_ShowTimeStamp (sec+SNTP_TIME1970, sec);
  }
  else {
    sec = 0;
    DEBUGF (SNTP," Timestamp invalid\n");
    EvrNetSNTP_TimeStampInvalid (sec);
  }
  sntp->cb_func (sec, get_u32 (&NTP_FRAME(buf)->Tx.frac));
  if (sntp->State == SNTP_STATE_BCAST) {
    return (true);
  }
  sntp_stop ();
  return (true);
}

/**
  \brief       Construct NTP message and send it to server.
  \return      status code as defined with netStatus.
*/
static netStatus sntp_send_message (void) {
  uint8_t *sendbuf;

  DEBUG_INFO (&sntp->Server);
  EvrNetSNTP_SendMessage (sntp->Server.addr);
  sendbuf = net_udp_get_buf (48);
  /* Initialize NTP frame to pre-specified values for SNTP */
  memset (sendbuf, 0, 48);
  NTP_FRAME(sendbuf)->VerMode = SNTP_DEF_VERMODE;

  return (net_udp_send (sntp->Socket, &sntp->Server, sendbuf, 48));
}

/**
  \brief       Stop SNTP client in unicast mode.
*/
static void sntp_stop (void) {
  net_udp_close (sntp->Socket);
  sntp->State  = SNTP_STATE_IDLE;
  sntp->Timer  = 0;
  sntp->Retries= 0;
}

#ifdef DEBUG_STDIO
/**
  \brief       Debug print IP address of NTP server.
  \param[in]   addr  structure containing IP address and port.
*/
static void debug_info (const __ADDR *addr) {
  DEBUGF (SNTP," Server [%s]\n",net_addr_ntoa(addr));
}
#endif
