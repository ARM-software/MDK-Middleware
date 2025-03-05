/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_tcp.c
 * Purpose: Transmission Control Protocol
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_mem.h"
#include "net_addr.h"
#include "net_ip4.h"
#ifdef Network_IPv6
#include "net_ip6.h"
#endif
#include "net_tcp.h"
#include "net_dbg.h"

/* Local Functions */
static bool tcp_write (NET_IF_CFG *net_if, NET_FRAME *frame,
                       const uint8_t *dst_addr, const uint8_t *loc_addr);
static void tcp_send_ctrl (NET_TCP_INFO *tcp_s, uint8_t flags);
static void tcp_send_data (NET_TCP_INFO *tcp_s, NET_FRAME *frame, uint32_t len);
static void tcp_send_reset (NET_IF_CFG *net_if, NET_FRAME *frame,
                            NET_TCP_HEADER *tcp_hdr,  uint8_t ip_ver);
static void tcp_parse_opt (NET_TCP_INFO *tcp_s, NET_TCP_HEADER *tcp_hdr);
static void tcp_set_cwnd (NET_TCP_INFO *tcp_s);
static void tcp_proc_dupack (NET_TCP_INFO *tcp_s);
static void tcp_proc_acknr (NET_TCP_INFO *tcp_s, uint32_t acknr);
static void tcp_que_add (NET_TCP_INFO *tcp_s, NET_FRAME *frame, uint32_t dlen);
static void tcp_que_init (NET_TCP_INFO *tcp_s);
static uint32_t tcp_que_una (NET_TCP_INFO *tcp_s);
static void tcp_que_resend (NET_TCP_INFO *tcp_s);
static void tcp_que_free (NET_TCP_INFO *tcp_s);
static NET_TCP_INFO *tcp_map_socket (NET_IF_CFG *net_if, NET_FRAME *frame,
                                     NET_TCP_HEADER *tcp_hdr, uint8_t ip_ver);
static void tcp_transit (NET_TCP_INFO *tcp_s, netTCP_State state);
static void tcp_upd_timers (void);
static bool tcp_port_in_use (uint16_t port);
#ifdef Network_Debug_STDIO
 static void debug_info (const NET_TCP_INFO *tcp_s);
 static void debug_inf2 (const NET_TCP_HEADER *tcp_hdr, int32_t len);
#endif

/* ==== Protected TCP Socket API functions ==== */

int32_t netTCP_GetSocket (netTCP_cb_t cb_func) {
  START_LOCK (int32_t);
  RETURN (net_tcp_get_socket (cb_func));
  END_LOCK;
}
netStatus netTCP_ReleaseSocket (int32_t socket) {
  START_LOCK (netStatus);
  RETURN (net_tcp_release_socket (socket));
  END_LOCK;
}
netStatus netTCP_Listen (int32_t socket, uint16_t port) {
  START_LOCK (netStatus);
  RETURN (net_tcp_listen (socket, port));
  END_LOCK;
}
netStatus netTCP_Connect (int32_t socket, const NET_ADDR *addr, uint16_t local_port) {
  START_LOCK (netStatus);
  RETURN (net_tcp_connect (socket, (const __ADDR *)addr, local_port));
  END_LOCK;
}
netStatus netTCP_Close (int32_t socket) {
  START_LOCK (netStatus);
  RETURN (net_tcp_close (socket));
  END_LOCK;
}
netStatus netTCP_Abort (int32_t socket) {
  START_LOCK (netStatus);
  RETURN (net_tcp_abort (socket));
  END_LOCK;
}
uint32_t netTCP_GetMaxSegmentSize (int32_t socket) {
  return (net_tcp_get_mss (socket));
}
uint8_t *netTCP_GetBuffer (uint32_t size) {
  START_LOCK_DBG (uint8_t *);
  RETURN_DBG (net_tcp_get_buf (size | 0x80000000));
  END_LOCK_DBG;
}
bool netTCP_SendReady (int32_t socket) {
  START_LOCK_DBG (bool);
  RETURN_DBG (net_tcp_check_send (socket));
  END_LOCK_DBG;
}
netStatus netTCP_Send (int32_t socket, uint8_t *buf, uint32_t len) {
  netStatus retv;
  START_LOCK (netStatus);
  retv = net_tcp_send (socket, buf, len);
  if (retv == netOK) net_sys_wakeup ();
  RETURN (retv);
  END_LOCK;
}
netTCP_State netTCP_GetState (int32_t socket) {
  return (net_tcp_get_state (socket));
}
netStatus netTCP_ResetReceiveWindow (int32_t socket) {
  START_LOCK (netStatus);
  RETURN (net_tcp_reset_window (socket));
  END_LOCK;
}
netStatus netTCP_SetOption (int32_t socket, netTCP_Option option, uint32_t val) {
  START_LOCK (netStatus);
  RETURN (net_tcp_set_option (socket, option, val));
  END_LOCK;
}
uint16_t netTCP_GetLocalPort (int32_t socket) {
  return (net_tcp_get_local_port (socket));
}
netStatus netTCP_GetPeer (int32_t socket, NET_ADDR *addr, uint32_t addr_len) {
  START_LOCK (netStatus);
  RETURN (net_tcp_get_peer (socket, (__ADDR *)addr, addr_len));
  END_LOCK;
}
uint32_t netTCP_GetTimer (int32_t socket) {
  return (net_tcp_get_timer (socket));
}

/* ==== Internal TCP Socket functions ==== */

/**
  \brief       Initialize TCP sockets.
*/
void net_tcp_socket_init (void) {
  DEBUGF (TCP,"Init %d Sockets\n",tcp->NumSocks);
  DEBUGF (TCP," MaxSegSize=%d\n",tcp->MaxSegSize);
  EvrNetTCP_InitSockets (tcp->NumSocks, tcp->MaxSegSize);

  /* Clear SCB for all sockets */
  memset (tcp->Scb, 0, sizeof (*tcp->Scb) * tcp->NumSocks);
}

/**
  \brief       De-initialize TCP sockets.
*/
void net_tcp_socket_uninit (void) {
  DEBUGF (TCP,"Uninit Sockets\n");
  EvrNetTCP_UninitSockets ();

  /* Clear SCB for all sockets */
  memset (tcp->Scb, 0, sizeof (*tcp->Scb) * tcp->NumSocks);
}

/**
  \brief       Allocate a free TCP socket.
  \param[in]   cb_func  event listening callback function.
  \return      socket handle number or execution status when < 0
*/
int32_t net_tcp_get_socket (netTCP_cb_t cb_func) {
  NET_TCP_INFO *tcp_s;
  int32_t i;

  if (cb_func == NULL) {
    ERRORF (TCP,"GetSocket, Invalid parameter\n");
    EvrNetTCP_GetSocketInvalidParameter ();
    return (-netInvalidParameter);
  }

  for (i = 1, tcp_s = &tcp->Scb[0]; i <= tcp->NumSocks; tcp_s++, i++) {
    if (tcp_s->State == netTCP_StateUNUSED) {
      /* Free TCP Socket found */
      DEBUGF (TCP,"Get Socket %d\n",i);
      EvrNetTCP_GetSocket (i, tcp->NumSocks);
      tcp_s->State   = netTCP_StateCLOSED;
      tcp_s->Type    = 0x00;
      tcp_s->Tos     = TCP_TOS_NORMAL;
      tcp_s->TClass  = TCP_TCLASS_NORMAL;
      tcp_s->cb_func = cb_func;
      /* Use timeout from the configuration */
      tcp_s->ConnTout= tcp->DefTout;
      tcp_s->Id      = i & 0xFF;
      /* Return socket handle */
      return (i);
    }
  }
  /* No free TCP Socket found, return error */
  ERRORF (TCP,"GetSocket, Out of sockets\n");
  EvrNetTCP_NoSocketAvailable (tcp->NumSocks);
  return (-netError);
}

/**
  \brief       Release TCP socket and free resources.
  \param[in]   socket  socket handle.
  \return      status code as defined with netStatus.
*/
netStatus net_tcp_release_socket (int32_t socket) {
  NET_TCP_INFO *tcp_s;

  DEBUGF (TCP,"Release Socket %d\n",socket);
  EvrNetTCP_ReleaseSocket (socket);
  if (socket <= 0 || socket > tcp->NumSocks) {
    ERRORF (TCP,"Release, Socket %d not valid\n",socket);
    EvrNetTCP_ReleaseSocketNotValid (socket);
    return (netInvalidParameter);
  }
  tcp_s = &tcp->Scb[socket-1];
  if (tcp_s->State > netTCP_StateCLOSED) {
    /* Wrong Socket State, can not release */
    ERRORF (TCP,"Release, Socket %d wrong state\n",socket);
    EvrNetTCP_ReleaseSocketWrongState (socket, tcp_s->State);
    return (netWrongState);
  }
  /* Clear the socket control block */
  memset (tcp_s, 0, sizeof (*tcp_s));
  return (netOK);
}

/**
  \brief       Open TCP socket for incoming connection.
  \param[in]   socket  socket handle.
  \param[in]   port    local port number.
  \return      status code as defined with netStatus.
*/
netStatus net_tcp_listen (int32_t socket, uint16_t port) {
  NET_TCP_INFO *tcp_s;

  DEBUGF (TCP,"Listen Socket %d, Port %d\n",socket,port);
  EvrNetTCP_ListenSocket (socket, port);
  if (socket <= 0 || socket > tcp->NumSocks) {
    ERRORF (TCP,"Listen, Socket %d not valid\n",socket);
    EvrNetTCP_ListenSocketNotValid (socket);
    return (netInvalidParameter);
  }
  if (port == 0) {
    ERRORF (TCP,"Listen, Socket %d Port 0\n",socket);
    EvrNetTCP_ListenPortUndefined (socket);
    return (netInvalidParameter);
  }
  tcp_s = &tcp->Scb[socket-1];
  switch (tcp_s->State) {
    case netTCP_StateLISTEN:
    case netTCP_StateCLOSED:
    case netTCP_StateTIME_WAIT:
      break;
    default:
      /* Not possible to listen, wrong socket state */
      ERRORF (TCP,"Listen, Socket %d wrong state\n",socket);
      EvrNetTCP_ListenSocketWrongState (socket, tcp_s->State);
      return (netWrongState);
  }
  /* Socket mode is Server now */
  tcp_s->Type |= TCP_TYPE_SERVER;

  /* Init socket control block */
  tcp_s->State      = netTCP_StateLISTEN;
  tcp_s->Flags      = 0;
  tcp_s->LocPort    = port;
  tcp_s->MaxSegSize = TCP_DEF_MSS;
  tcp_s->RecWin     = tcp->RecWinSize;
  tcp_s->SendWin    = 0;
  tcp_s->SendUna    = 0;
  tcp_s->SendNext   = 0;
  tcp_s->RecNext    = 0;
  tcp_s->Retries    = 0;
  tcp_s->unack_list = NULL;
  tcp_s->LocAddr    = NULL;
  memset (&tcp_s->Peer, 0, sizeof (tcp_s->Peer));
  return (netOK);
}

/**
  \brief       Initiate a TCP connection to a remote node.
  \param[in]   socket      socket handle.
  \param[in]   addr        structure containing remote IP address and port.
  \param[in]   local_port  local port number or
                           0 for system assigned local port.
  \return      status code as defined with netStatus.
*/
netStatus net_tcp_connect (int32_t socket, const __ADDR *addr, uint16_t local_port) {
  NET_TCP_INFO *tcp_s;
  NET_IF_CFG *out_if;

  DEBUGF (TCP,"Connect Socket %d, LocPort %d\n",socket,local_port);
  EvrNetTCP_ConnectSocket (socket, local_port);
  if (socket <= 0 || socket > tcp->NumSocks) {
    ERRORF (TCP,"Connect, Socket %d not valid\n",socket);
    EvrNetTCP_ConnectSocketNotValid (socket);
    return (netInvalidParameter);
  }
  if (net_addr_is_unspec (addr)) {
    /* Remote IP address not specified */
    ERRORF (TCP,"Connect, Socket %d IpAddr unspecified\n",socket);
    EvrNetTCP_ConnectAddressUnspecified (socket);
    return (netInvalidParameter);
  }
  if (addr->port == 0) {
    /* Remote TCP port not specified */
    ERRORF (TCP,"Connect, Socket %d Port undefined\n",socket);
    EvrNetTCP_ConnectPortUndefined (socket);
    return (netInvalidParameter);
  }
  DEBUGF (TCP," IpAddr [%s], port %d\n",net_addr_ntoa(addr),addr->port);
  EvrNetTCP_ShowNetAddress (addr);
  tcp_s = &tcp->Scb[socket-1];
  switch (tcp_s->State) {
    case netTCP_StateLISTEN:
      /* Listening socket, restricted local_port values */
      if (local_port == 0 || local_port == tcp_s->LocPort) {
        break;
      }
      ERRORF (TCP,"Connect, Socket %d LocPort %d not allowed\n",socket,local_port);
      EvrNetTCP_ConnectLocalPortInvalid (socket, local_port);
      return (netInvalidParameter);
    case netTCP_StateCLOSED:
      /* Check if local port is defined */
      if (local_port == 0) {
        local_port = net_tcp_alloc_port();
        DEBUGF (TCP," LocPort %d assigned\n",local_port);
        EvrNetTCP_AssignLocalPort (socket, local_port);
      }
      tcp_s->LocPort = local_port;
      break;
    default:
      /* Not possible to start CONNECT, wrong socket state */
      ERRORF (TCP,"Connect, Socket %d wrong state\n",socket);
      EvrNetTCP_ConnectSocketWrongState (socket, tcp_s->State);
      return (netWrongState);
  }

  /* Check the interface routing */
  out_if = net_addr_find_route (tcp_s->net_if, addr);
  if (!out_if || (tcp_s->net_if && (out_if != tcp_s->net_if))) {
    /* No route or the route differs from the requested */
    ERRORF (TCP,"Connect, Socket %d no route found\n",socket);
    EvrNetTCP_ConnectNoRouteFound (socket);
    return (netError);
  }
  tcp_s->net_if = out_if;

  /* Socket mode is Client now */
  tcp_s->Type      &= ~TCP_TYPE_SERVER;

  /* OK then just set parameters and send SYN frame */
  net_addr_copy (&tcp_s->Peer, addr);
  tcp_s->LocAddr    = net_addr_get_local (tcp_s->net_if, addr);
  tcp_s->Flags      = 0;

  /* Send MSS is updated when MSS option is received */
  tcp_s->MaxSegSize = TCP_DEF_MSS;
  tcp_s->RecWin     = tcp->RecWinSize;

  /* Sliding window defaults */
  tcp_s->SendWin    = TCP_DEF_WINSIZE;
  /* Initial congestion windows size (RFC5681 - page 4) */
  tcp_s->CWnd       = TCP_DEF_MSS * 4;
  tcp_s->SsThresh   = 65535;
  tcp_s->DupAcks    = 0;

  /* RTT estimation defaults */
  tcp_s->RttSa      = 0;
  tcp_s->RttSv      = (int16_t)tcp->RetryTout;
  DEBUG_INFO (tcp_s);
  EvrNetTCP_ShowRttVariables (tcp_s->Id, tcp_s->RttSa, tcp_s->RttSv);

  /* Get initial send sequence number */
  tcp_s->SendUna    = net_rand32 ();
  tcp_s->SendNext   = tcp_s->SendUna;
  tcp_s->SendChk    = tcp_s->SendUna;
  tcp_s->RecNext    = 0;

  /* SYN packet sends also MSS option */
  tcp_send_ctrl (tcp_s, TCP_FLAG_SYN);
  tcp_transit (tcp_s, netTCP_StateSYN_SENT);
  return (netOK);
}

/**
  \brief       Determine max number of data bytes for TCP packet.
  \param[in]   socket  socket handle.
  \return      maximum segment size in bytes.
*/
uint32_t net_tcp_get_mss (int32_t socket) {
  uint32_t maxsize;

  maxsize = tcp->Scb[socket-1].MaxSegSize;
  if (maxsize > TCP_MTU) {
    /* Do not send larger frames than TCP_MTU */
    maxsize = TCP_MTU;
  }
  return (maxsize);
}

/**
  \brief       Allocate memory for TCP send buffer.
  \param[in]   size  number of bytes to allocate.
  \return      pointer to the allocated memory.
  \note        If the size has high bit set, (or-ed with 0x80000000),
               the function returns NULL on error, instead of calling 
               system error function sys_error().
*/
uint8_t *net_tcp_get_buf (uint32_t size) {
  NET_FRAME *frame;
  uint32_t sz = size & 0xFFFF;

  if (size & 0x80000000) {
    /* No sys_error() call, when out of memory */
    sz |= 0x40000000;
  }
  frame = net_mem_alloc (sz + TCP_DATA_OFFS);
  if (frame == NULL) {
    /* Only if sys_error() call disabled */
    ERRORF (TCP,"GetBuffer, Out of memory\n");
    EvrNetTCP_GetBufferFailed (sz & 0xFFFF);
    return (NULL);
  }
  /* Reserve the space for protocol headers */
  frame->index = TCP_DATA_OFFS;
  return (&frame->data[TCP_DATA_OFFS]);
}

/**
  \brief       Send data to a remote node.
  \param[in]   socket  socket handle.
  \param[in]   buf     buffer containing the data.
  \param[in]   len     length of data in bytes.
  \return      status code as defined with netStatus.
*/
netStatus net_tcp_send (int32_t socket, uint8_t *buf, uint32_t len) {
  NET_TCP_INFO *tcp_s;
  NET_FRAME *frame;
  netStatus retv;

  DEBUGF (TCP,"Send Socket %d, %d bytes\n",socket,len);
  EvrNetTCP_SendFrame (socket, len);
  frame = __FRAME(buf - (TCP_DATA_OFFS + NET_HEADER_LEN));
  /* Check if this is a valid frame (allocated by net_tcp_get_buf) */
  if ((buf == NULL) || (frame->index != TCP_DATA_OFFS)) {
    /* Wrong buffer passed, do nothing and return */
    ERRORF (TCP,"Send, Socket %d invalid buffer\n",socket);
    EvrNetTCP_SendBufferInvalid (socket);
    return (netInvalidParameter);
  }

  if (socket <= 0 || socket > tcp->NumSocks) {
    ERRORF (TCP,"Send, Socket %d not valid\n",socket);
    EvrNetTCP_SendSocketNotValid (socket);
    retv = netInvalidParameter;
    goto retf;
  }
  tcp_s = &tcp->Scb[socket-1];
  if (tcp_s->State != netTCP_StateESTABLISHED) {
    ERRORF (TCP,"Send, Socket %d not connected\n",socket);
    EvrNetTCP_SendSocketNotConnected (socket);
    retv = netWrongState;
    goto retf;
  }
  if (tcp_s->Flags & TCP_IFLAG_CLOSING) {
    ERRORF (TCP,"Send, Socket %d closing\n",tcp_s->Id);
    EvrNetTCP_SendSocketClosing (tcp_s->Id);
    retv = netWrongState;
    goto retf;
  }
  if (tcp_s->Flags & TCP_IFLAG_CBACK) {
    /* Called from callback function of a socket */
    ERRORF (TCP,"Send, Socket %d re-entered call\n",tcp_s->Id);
    EvrNetTCP_SendReenteredCall (tcp_s->Id);
    retv = netBusy;
    goto retf;
  }
  if (tcp_s->Flags & TCP_IFLAG_DACK) {
    /* Some data not yet acknowledged */
    ERRORF (TCP,"Send, Socket %d has unacked data\n",tcp_s->Id);
    EvrNetTCP_SendDataUnacked (tcp_s->Id);
    retv = netBusy;
    goto retf;
  }
  if (len > tcp_s->MaxSegSize) {
    ERRORF (TCP,"Send, Socket %d MSS exceeded\n",tcp_s->Id);
    EvrNetTCP_SendMssExceeded (tcp_s->Id);
    retv = netInvalidParameter;
    goto retf;
  }
  if (len == 0) {
    /* Do not send 0-length packets */
    DEBUGF (TCP," Dumping 0-length packet\n");
    EvrNetTCP_SendZeroLengthFrame (tcp_s->Id);
    retv = netInvalidParameter;
retf:
    net_mem_free (frame);
    return (retv);
  }

  net_mem_shrink (frame, TCP_DATA_OFFS + len);
  tcp_s->Flags |= TCP_IFLAG_DACK;
  if (!(tcp_s->Flags & TCP_IFLAG_RESEND)) {
    /* Not in retransmit, send a frame now */
    tcp_s->Flags     |=  TCP_IFLAG_PUSH;
    tcp_s->Flags     &= ~TCP_IFLAG_KALIVE;
    tcp_send_data (tcp_s, frame, len);
    /* Calculate RTO and set a retransmit timer */
    tcp_s->RetryTimer = (uint16_t)((tcp_s->RttSa >> 3) + tcp_s->RttSv);
    tcp_s->Retries    = tcp->MaxRetry;
    tcp_s->AliveTimer = tcp_s->ConnTout;
  }
  else {
    /* Add this packet to the queue and let net_tcp_socket_run()*/
    /* handle it and send it in a retransmission process.       */
    tcp_s->SendNext += len;
  }
  /* The queue control variables overlap the ethernet header,*/
  /* so the frame must be added to the queue after sending.  */
  tcp_que_add (tcp_s, frame, len);
  return (netOK);
}

/**
  \brief       Stop TCP communication and start closing procedure.
  \param[in]   socket  socket handle.
  \return      status codeas defined with netStatus.
*/
netStatus net_tcp_close (int32_t socket) {
  NET_TCP_INFO *tcp_s;

  DEBUGF (TCP,"Close Socket %d\n",socket);
  EvrNetTCP_CloseSocket (socket);
  if (socket <= 0 || socket > tcp->NumSocks) {
    ERRORF (TCP,"Close, Socket %d not valid\n",socket);
    EvrNetTCP_CloseSocketNotValid (socket);
    return (netInvalidParameter);
  }

  tcp_s = &tcp->Scb[socket-1];
  switch (tcp_s->State) {
    case netTCP_StateCLOSED:
      break;

    case netTCP_StateLISTEN:
      tcp_transit (tcp_s, netTCP_StateCLOSED);
      break;

    case netTCP_StateSYN_RECEIVED:
      tcp_s->SendUna++;
      tcp_send_ctrl (tcp_s, TCP_FLAG_ACK | TCP_FLAG_FIN);
      tcp_transit (tcp_s, netTCP_StateFIN_WAIT_1);
      break;

    case netTCP_StateSYN_SENT:
      tcp_transit (tcp_s, netTCP_StateCLOSED);
      break;

    case netTCP_StateFIN_WAIT_1:
    case netTCP_StateFIN_WAIT_2:
    case netTCP_StateCLOSING:
    case netTCP_StateTIME_WAIT:
    case netTCP_StateLAST_ACK:
      /* This socket is closing already */
      break;

    case netTCP_StateESTABLISHED:
      if (tcp_s->Flags & TCP_IFLAG_CLOSING) {
        /* Socket close is already pending, do nothing now */
        break;
      }
      if (tcp_s->SendUna != tcp_s->SendNext) {
        /* Some data not acknowledged yet, set the pollable flag    */
        /* and do further socket processing in net_tcp_socket_run() */
        DEBUGF (TCP," Pended, %d bytes unacked\n",tcp_s->SendNext-tcp_s->SendUna);
        EvrNetTCP_CloseDataUnacked (tcp_s->Id, tcp_s->SendNext-tcp_s->SendUna);
        tcp_s->Flags |= TCP_IFLAG_CLOSING;
        if (tcp_s->Flags & TCP_IFLAG_RESEND) {
          /* Socket is currently retransmitting, send FIN after  */
          /* last data packet from the queue is retransmitted.   */
          /* SendNext must reflect the last sequence to be acked.*/
          /* That is count of data bytes + FIN in this case.     */
          tcp_s->SendNext++;
          break;
        }
        /* Not retransmitting, send FIN now */
        tcp_send_ctrl (tcp_s, TCP_FLAG_ACK | TCP_FLAG_FIN);
        tcp_s->RetryTimer = (uint16_t)((tcp_s->RttSa >> 3) + tcp_s->RttSv);
        tcp_s->Retries    = tcp->MaxRetry;
        tcp_s->AliveTimer = tcp_s->ConnTout;
        break;
      }
      /* All sent data has been acknowledged */
      tcp_send_ctrl (tcp_s, TCP_FLAG_ACK | TCP_FLAG_FIN);
      tcp_transit (tcp_s, netTCP_StateFIN_WAIT_1);
      break;

    default:
      /* Not possible to close, wrong socket state */
      ERRORF (TCP,"Close, Socket %d wrong state\n",socket);
      EvrNetTCP_CloseSocketWrongState (socket, tcp_s->State);
      return (netWrongState);
  }
  return (netOK);
}

/**
  \brief       Instantly stop TCP communication.
  \param[in]   socket  socket handle.
  \return      status codeas defined with netStatus.
*/
netStatus net_tcp_abort (int32_t socket) {
  NET_TCP_INFO *tcp_s;

  DEBUGF (TCP,"Abort Socket %d\n",socket);
  EvrNetTCP_AbortSocket (socket);
  if (socket <= 0 || socket > tcp->NumSocks) {
    ERRORF (TCP,"Abort, Socket %d not valid\n",socket);
    EvrNetTCP_AbortSocketNotValid (socket);
    return (netInvalidParameter);
  }

  tcp_s = &tcp->Scb[socket-1];
  switch (tcp_s->State) {
    case netTCP_StateCLOSED:
      break;

    case netTCP_StateLISTEN:
    case netTCP_StateSYN_SENT:
    case netTCP_StateCLOSING:
    case netTCP_StateLAST_ACK:
    case netTCP_StateTIME_WAIT:
      tcp_transit (tcp_s, netTCP_StateCLOSED);
      break;

    case netTCP_StateSYN_RECEIVED:
    case netTCP_StateESTABLISHED:
    case netTCP_StateFIN_WAIT_1:
    case netTCP_StateFIN_WAIT_2:
      tcp_send_ctrl (tcp_s, TCP_FLAG_RST);
      tcp_transit (tcp_s, netTCP_StateCLOSED);
      break;

    default:
      /* Not possible to abort, wrong socket state */
      ERRORF (TCP,"Abort, Socket %d wong state\n",socket);
      EvrNetTCP_AbortSocketWrongState (socket, tcp_s->State);
      return (netWrongState);
  }
  return (netOK);
}

/**
  \brief       Determine current state of a TCP socket.
  \param[in]   socket  socket handle.
  \return      status information as defined with netTCP_State.
*/
netTCP_State net_tcp_get_state (int32_t socket) {
  NET_TCP_INFO *tcp_s;

  if (socket <= 0 || socket > tcp->NumSocks) {
    return (netTCP_StateINVALID);
  }
  tcp_s = &tcp->Scb[socket-1];
  return (tcp_s->State);
}

/**
  \brief       Retrieve IP address and port number of remote peer.
  \param[in]   socket    socket handle.
  \param[out]  addr      structure that will receive IP address and port number.
  \param[in]   addr_len  size of NET_ADDR structure for remote peer.
  \return      status code as defined with netStatus.
*/
netStatus net_tcp_get_peer (int32_t socket, __ADDR *addr, uint32_t addr_len) {
  NET_TCP_INFO *tcp_s;

  if ((addr == NULL) || (addr_len < sizeof(NET_ADDR4))) {
    return (netInvalidParameter);
  }
  if (socket <= 0 || socket > tcp->NumSocks) {
    return (netInvalidParameter);
  }
  tcp_s = &tcp->Scb[socket-1];
  if (tcp_s->State <= netTCP_StateLISTEN) {
    return (netWrongState);
  }
#ifdef Network_IPv6
  if ((tcp_s->Peer.addr_type == NET_ADDR_IP6) && (addr_len < sizeof(NET_ADDR))) {
    /* Error, no space for IPv6 address in IPv4 structure */
    return (netInvalidParameter);
  }
#endif
  net_addr_copy (addr, &tcp_s->Peer);
  return (netOK);
}

/**
  \brief       Retrieve TCP socket remote peer address.
  \param[in]   socket  socket handle.
  \return      pointer to network address structure.
*/
NET_ADDR *net_tcp_get_peer_ptr (int32_t socket) {
  NET_TCP_INFO *tcp_s;

  if ((socket <= 0) || (socket > tcp->NumSocks)) {
    return (NULL);
  }
  tcp_s = &tcp->Scb[socket-1];
  return ((NET_ADDR *)&tcp_s->Peer);
}

/**
  \brief       Retrieve TCP socket local port number.
  \param[in]   socket  socket handle.
  \return      local port number or
                 0 = socket invalid or in invalid state.
*/
uint16_t net_tcp_get_local_port (int32_t socket) {
  NET_TCP_INFO *tcp_s;

  if ((socket <= 0) || (socket > tcp->NumSocks)) {
    return (0);
  }
  tcp_s = &tcp->Scb[socket-1];
  if (tcp_s->State < netTCP_StateLISTEN) {
    return (0);
  }
  return (tcp_s->LocPort);
}

/**
  \brief       Retrieve TCP socket local IP address.
  \param[in]   socket  socket handle.
  \return      pointer to local IP address.
*/
const uint8_t *net_tcp_get_local_addr (int32_t socket) {
  NET_TCP_INFO *tcp_s;

  if ((socket <= 0) || (socket > tcp->NumSocks)) {
    return (NULL);
  }
  tcp_s = &tcp->Scb[socket-1];
  if (tcp_s->State <= netTCP_StateLISTEN) {
    return (NULL);
  }
  return (tcp_s->LocAddr);
}

/**
  \brief       Determine TCP socket connection timeout.
  \param[in]   socket  socket handle.
  \return      connection timeout in seconds or
                 0 = socket invalid or in invalid state.
*/
uint32_t net_tcp_get_timer (int32_t socket) {
  NET_TCP_INFO *tcp_s;

  if ((socket <= 0) || (socket > tcp->NumSocks)) {
    return (0);
  }
  tcp_s = &tcp->Scb[socket-1];
  if (tcp_s->State <= netTCP_StateLISTEN) {
    return (0);
  }
  return (tcp_s->AliveTimer);
}

/**
  \brief       Check if TCP socket can send data.
  \param[in]   socket  socket handle.
  \return      send status:
               - true  = Ready to send data.
               - false = Not ready.
*/
bool net_tcp_check_send (int32_t socket) {
  NET_TCP_INFO *tcp_s;

  if (socket <= 0 || socket > tcp->NumSocks) {
    ERRORF (TCP,"SendReady, Socket %d not valid\n",socket);
    EvrNetTCP_SendReadySocketNotValid (socket);
    return (false);
  }
  tcp_s = &tcp->Scb[socket-1];
  if (tcp_s->State != netTCP_StateESTABLISHED) {
    return (false);
  }
  if (tcp_s->Flags & TCP_IFLAG_CBACK) {
    /* Called from callback function of a socket */
    ERRORF (TCP,"SendReady, Socket %d re-entered call\n",tcp_s->Id);
    EvrNetTCP_SendReadyReenteredCall (tcp_s->Id);
    return (false);
  }
  if (tcp_s->Flags & (TCP_IFLAG_DACK | TCP_IFLAG_CLOSING)) {
    /* Last frame not yet acked localy, or the socket is closing */
    return (false);
  }
  return (true);
}

/**
  \brief       Control socket options from BSD interface.
  \param[in]   socket   socket handle.
  \param[in]   type     socket type bit-mask.
  \param[in]   ip_opt   IP options.
*/
void net_tcp_ctrl_opt (int32_t socket, uint8_t type, uint32_t ip_opt) {
  NET_TCP_INFO *tcp_s;

  if (socket <= 0 || socket > tcp->NumSocks) {
    return;
  }
  tcp_s = &tcp->Scb[socket-1];
  if (tcp_s->State > netTCP_StateUNUSED) {
    tcp_s->Type  &= ~TCP_TYPE_MASK;
    tcp_s->Type  |= (type & TCP_TYPE_MASK);
    tcp_s->Tos    = ip_opt & 0xFF;
    tcp_s->TClass = (ip_opt >> 8) & 0xFF;
  }
}

/**
  \brief       Set or change socket callback function.
  \param[in]   socket   socket handle.
  \param[in]   cb_func  event listening callback function.
*/
void net_tcp_set_cb (int32_t socket, netTCP_cb_t cb_func) {
  NET_TCP_INFO *tcp_s;

  if (socket <= 0 || socket > tcp->NumSocks) {
    return;
  }
  tcp_s = &tcp->Scb[socket-1];
  if (tcp_s->State > netTCP_StateUNUSED) {
    tcp_s->cb_func = cb_func;
  }
}

/**
  \brief       Reset keep-alive timer for the socket.
  \param[in]   socket  socket handle.
*/
void net_tcp_reset_tout (int32_t socket) {
  NET_TCP_INFO *tcp_s;

  if (socket <= 0 || socket > tcp->NumSocks) {
    return;
  }
  tcp_s = &tcp->Scb[socket-1];
  if (tcp_s->State == netTCP_StateESTABLISHED) {
    tcp_s->AliveTimer = tcp_s->ConnTout;
  }
}

/**
  \brief       Reset receive window to full size in flow control mode.
  \param[in]   socket  socket handle.
  \return      status information as defined with netStatus.
*/
netStatus net_tcp_reset_window (int32_t socket) {
  NET_TCP_INFO *tcp_s;

  if (socket <= 0 || socket > tcp->NumSocks) {
    ERRORF (TCP,"ResetRecWindow, Socket %d not valid\n",socket);
    EvrNetTCP_ResetWindowSocketNotValid (socket);
    return (netInvalidParameter);
  }
  tcp_s = &tcp->Scb[socket-1];
  if (tcp_s->State != netTCP_StateESTABLISHED) {
    ERRORF (TCP,"ResetRecWindow, Socket %d not connected\n",socket);
    EvrNetTCP_ResetWindowNotConnected (socket);
    return (netWrongState);
  }
  if (!(tcp_s->Type & TCP_TYPE_FLOW_CTRL)) {
    ERRORF (TCP,"ResetRecWindow, Socket %d Flow-Ctrl not enabled\n",tcp_s->Id);
    EvrNetTCP_ResetWindowNoFlowControl (tcp_s->Id);
    return (netInvalidParameter);
  }
  if (tcp_s->RecWin == tcp->RecWinSize) {
    return (netOK);
  }
  tcp_s->RecWin = tcp->RecWinSize;
  DEBUGF (TCP,"Socket %d, RecWin update %d\n",tcp_s->Id,tcp_s->RecWin);
  EvrNetTCP_ResetWindowUpdate (tcp_s->Id, tcp_s->RecWin);
  if (!(tcp_s->Flags & TCP_IFLAG_CBACK)) {
    /* Send TCP Window update frame */
    tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
  }
  return (netOK);
}

/**
  \brief       Set TCP/IP options for TCP socket.
  \param[in]   socket  socket handle.
  \param[in]   option  option name as defined with netTCP_Option.
  \param[in]   val     option value.
  \return      status code as defined with netStatus.
*/
netStatus net_tcp_set_option (int32_t socket, netTCP_Option option, uint32_t val) {
  NET_TCP_INFO *tcp_s;
  NET_IF_CFG *net_if;

  DEBUGF (TCP,"SetOption Socket %d\n",socket);
  EvrNetTCP_SetOptionSocket (socket, option, val);
  if (socket <= 0 || socket > tcp->NumSocks) {
    ERRORF (TCP,"SetOption, Socket %d not valid\n",socket);
    EvrNetTCP_SetOptionSocketNotValid (socket);
    return (netInvalidParameter);
  }
  tcp_s = &tcp->Scb[socket-1];
  if (tcp_s->State == netTCP_StateUNUSED) {
wrong_state:
    ERRORF (TCP,"SetOption, Socket %d wrong state\n",socket);
    EvrNetTCP_SetOptionSocketWrongState (socket, tcp_s->State);
    return (netWrongState);
  }
  switch ((int32_t)option) {
    case netTCP_OptionTOS:
      if (val > 255) break;
      DEBUGF (TCP," TOS=%d\n",val);
      EvrNetTCP_SetOptionTos (socket, val & 0xFF);
      tcp_s->Tos = val & 0xFF;
      return (netOK);

#ifdef Network_IPv6
    case netTCP_OptionTrafficClass:
      if (val > 255) break;
      DEBUGF (TCP," Tclass=%d\n",val);
      EvrNetTCP_SetOptionTclass (socket, val & 0xFF);
      tcp_s->TClass = val & 0xFF;
      return (netOK);
#endif

    case netTCP_OptionInterface:
      if (tcp_s->State != netTCP_StateCLOSED) {
        goto wrong_state;
      }
      net_if = net_if_map_all (val);
      if (net_if == NULL) break;
      DEBUGF (TCP," Interface=%s\n",net_if->Name);
      EvrNetTCP_SetOptionInterface (socket, net_if->Id);
      tcp_s->net_if = net_if;
      return (netOK);

    case netTCP_OptionTimeout:
      if (val > 65535) break;
      if (val == 0) {
        /* Use timeout from the configuration */
        val = tcp->DefTout;
      }
      DEBUGF (TCP," Tout=%d\n",val);
      EvrNetTCP_SetOptionTimeout (socket, val & 0xFFFF);
      tcp_s->ConnTout = val & 0xFFFF;
      return (netOK);

    case netTCP_OptionKeepAlive:
      if (val > 1) break;
      DEBUGF (TCP," KeepAlive=%d\n",val);
      EvrNetTCP_SetOptionKeepAlive (socket, val);
      tcp_s->Type &= ~TCP_TYPE_KEEP_ALIVE;
      if (val != 0) {
        tcp_s->Type |= TCP_TYPE_KEEP_ALIVE;
      }
      return (netOK);

    case netTCP_OptionFlowControl:
      if (val > 1) break;
      DEBUGF (TCP," FlowCtrl=%d\n",val);
      EvrNetTCP_SetOptionFlowControl (socket, val);
      tcp_s->Type &= ~TCP_TYPE_FLOW_CTRL;
      if (val != 0) {
        tcp_s->Type |= TCP_TYPE_FLOW_CTRL;
      }
      return (netOK);

    case netTCP_OptionDelayedACK:
      if (val > 1) break;
      DEBUGF (TCP," DelayACK=%d\n",val);
      EvrNetTCP_SetOptionDelayedAck (socket, val);
      tcp_s->Type &= ~TCP_TYPE_DELAY_ACK;
      if (val != 0) {
        tcp_s->Type |= TCP_TYPE_DELAY_ACK;
      }
      return (netOK);

    default:
      ERRORF (TCP,"SetOption, Socket %d wrong option\n",socket);
      EvrNetTCP_SetOptionWrongOption (socket, option);
      return (netInvalidParameter);
  }
  ERRORF (TCP,"SetOption, Socket %d wrong val (%d)\n",socket,val);
  EvrNetTCP_SetOptionWrongValue (socket, val);
  return (netInvalidParameter);
}

/**
  \brief       Get TCP/IP options for TCP socket.
  \param[in]   socket  socket handle.
  \param[in]   option  option name as defined with netTCP_Option.
  \return      option value.
*/
uint32_t net_tcp_get_option (int32_t socket, netTCP_Option option) {
  NET_TCP_INFO *tcp_s;

  if (socket <= 0 || socket > tcp->NumSocks) {
    return (0);
  }
  tcp_s = &tcp->Scb[socket-1];
  if (tcp_s->State == netTCP_StateUNUSED) {
    return (0);
  }
  switch ((int32_t)option) {
    case netTCP_OptionTOS:
      return (tcp_s->Tos);

#ifdef Network_IPv6
    case netTCP_OptionTrafficClass:
      return (tcp_s->TClass);
#endif

    case netTCP_OptionInterface:
      if (!tcp_s->net_if) {
        /* Network interface not assigned */
        return (0);
      }
      return (tcp_s->net_if->Id);

    case netTCP_OptionTimeout:
      return (tcp_s->ConnTout);

    case netTCP_OptionKeepAlive:
      return ((tcp_s->Type & TCP_TYPE_KEEP_ALIVE) ? 1 : 0);

    case netTCP_OptionFlowControl:
      return ((tcp_s->Type & TCP_TYPE_FLOW_CTRL) ? 1 : 0);

    case netTCP_OptionDelayedACK:
      return ((tcp_s->Type & TCP_TYPE_DELAY_ACK) ? 1 : 0);

    default:
      break;
  }
  return (0);
}

/**
  \brief       Allocate a free dynamic TCP port.
  \return      port number.
*/
uint16_t net_tcp_alloc_port (void) {
  static uint16_t port;

  /* IANA categories of TCP ports       */
  /* a) well-known ports: 0     - 1023  */
  /* b) registered ports: 1024  - 49151 */
  /* c) dynamic ports   : 49152 - 65535 */
  while (1) {
    /* Scan all available private ports */
    if ((++port < tcp->DynPortStart) || (port > tcp->DynPortEnd)) {
      /* Use custom dynamic/private ports */
      port = tcp->DynPortStart;
    }
    if (tcp_port_in_use (port) == false) {
      /* This port is free, stop searching */
      return (port);
    }
  }
}

/**
  \brief       Periodically poll TCP sockets for timeouts.
*/
void net_tcp_socket_run (void) {
  static uint8_t socket = 0;
  NET_TCP_INFO *tcp_s;
  uint32_t i;

  /* Update TCP timers */
  if (sys->Flags & SYS_FLAG_TICK) {
    tcp_upd_timers ();
  }

  for (i = 0; i < tcp->NumSocks; i++) {
    if (++socket > tcp->NumSocks) {
      socket = 1;
    }

    tcp_s = &tcp->Scb[socket-1];
    switch (tcp_s->State) {
      case netTCP_StateUNUSED:
      case netTCP_StateCLOSED:
      case netTCP_StateLISTEN:
      default:
        /* Nothing to do here */
        break;

      case netTCP_StateESTABLISHED:
        /* Do some actions when there is unacknowledged data, */
        /* the connection has been IDLE for too long or       */
        /* unserved close is issued by the user.              */

        /* Is it unserved close? */
        if (tcp_s->Flags & TCP_IFLAG_CLOSING) {
          /* If all data acked, we can send the close now */
          if (tcp_s->SendUna == tcp_s->SendNext) {
            tcp_send_ctrl (tcp_s, TCP_FLAG_ACK | TCP_FLAG_FIN);
            tcp_transit (tcp_s, netTCP_StateFIN_WAIT_1);
            return;
          }
        }

        /* Check if receive delayed-ack pending? */
        if ((tcp_s->AckTimer != 0) && (sys->Flags & SYS_FLAG_TICK)) {
          if (--tcp_s->AckTimer == 0) {
            /* Send Delayed-ack here */
            DEBUGF (TCP,"Socket %d, Delayed-ack\n",tcp_s->Id);
            EvrNetTCP_SendDelayedAck (tcp_s->Id);
            tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
            return;
          }
        }

        /* Has Keep Alive timer expired and we have no unacked data? */
        if ((tcp_s->AliveTimer == 0) && (tcp_s->SendUna == tcp_s->SendNext)) {
          /* Yes, check if keep-alive attribute enabled */
          if (tcp_s->Type & TCP_TYPE_KEEP_ALIVE) {
            /* Send keep-alive packet to retain the connection */
            if ((tcp_s->Flags & TCP_IFLAG_KALIVE) == 0) {
              DEBUGF (TCP,"Socket %d, Keep-alive expired sending probe\n",tcp_s->Id);
              EvrNetTCP_SendKeepAliveProbe (tcp_s->Id);
              tcp_s->Flags     |= (TCP_IFLAG_KALIVE | TCP_IFLAG_KSEG);
              tcp_s->Retries    = tcp->MaxRetry;
              tcp_s->RetryTimer = tcp->RetryTout;
              tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
              return;
            }
            if (tcp_s->RetryTimer != 0) {
              break;
            }
            if (tcp_s->Retries != 0) {
              /* Retransmit keep-alive segment */
              tcp_s->Retries--;
              tcp_s->Flags     |= TCP_IFLAG_KSEG;
              tcp_s->RetryTimer = tcp->RetryTout;
              tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
              return;
            }
            /* No Retries left, reset the connection */
            tcp_s->Flags |= TCP_IFLAG_CBACK;
            goto no_retries;
          }
          /* Close the connection and inform the application */
          DEBUGF (TCP,"Socket %d, Keep-alive timeout closing\n",tcp_s->Id);
          EvrNetTCP_KeepAliveTimeoutClosing (tcp_s->Id);
          tcp_send_ctrl (tcp_s, TCP_FLAG_ACK | TCP_FLAG_FIN);
          tcp_s->Flags |= TCP_IFLAG_CBACK;
          tcp_s->cb_func (tcp_s->Id, netTCP_EventClosed, (NET_ADDR *)&tcp_s->Peer, NULL, 0);
          tcp_transit (tcp_s, netTCP_StateFIN_WAIT_1);
          return;
        }

        /* Is the socket retransmitting? */
        if (tcp_s->Flags & TCP_IFLAG_RESEND) {
          uint32_t una = tcp_que_una (tcp_s);
          uint32_t win = MIN(tcp_s->CWnd, tcp_s->SendWin);
          if (una + tcp_s->MaxSegSize <= win) {
            /* Resend saved frames from the queue only if the */
            /* sliding window allows to send additional data. */
            tcp_que_resend (tcp_s);
            return;
          }
        }
        /* Is the socket sending data? */
        else if (tcp_s->Flags & TCP_IFLAG_DACK) {
          uint32_t una = tcp_s->SendNext - tcp_s->SendUna;
          uint32_t win = MIN(tcp_s->CWnd, tcp_s->SendWin);
          if ((una + tcp_s->MaxSegSize <= win) && net_mem_avail_tx ()) {
            /* Send netTCP_EventACK notification only if the sliding */
            /* window allows to send additional data, and there is   */
            /* enough free memory for buffering new transmit packets.*/
            DEBUGF (TCP,"Socket %d, callback EventACK\n",tcp_s->Id);
            EvrNetTCP_CallbackEventAck (tcp_s->Id);
            tcp_s->Flags |= TCP_IFLAG_CBACK;
            tcp_s->cb_func (tcp_s->Id, netTCP_EventACK, (NET_ADDR *)&tcp_s->Peer, NULL, 0);
            tcp_s->Flags &= ~(TCP_IFLAG_CBACK | TCP_IFLAG_DACK);
          }
        }

        /* Some data not acknowledged? */
        if (tcp_s->SendUna == tcp_s->SendNext) {
          break;
        }

        /* Check if Retry timer expired? */
        if (tcp_s->RetryTimer != 0) {
          break;
        }

        DEBUGF (TCP,"Socket %d, Timeout retransmit %d bytes\n",tcp_s->Id,
                                tcp_s->SendNext-tcp_s->SendUna);
        EvrNetTCP_ResendOnTimeout (tcp_s->Id, tcp_s->SendNext-tcp_s->SendUna);
        /* Check if any retries left? */
        if (tcp_s->Retries != 0) {
          /* Retry again and restart the retry timer */
          tcp_s->Retries--;
          /* Reset Congestion control to a slow start */
          /*   ssthresh = MAX(MIN(win,cwnd)/2,2*MSS)  */
          /*   cwnd     = MSS                         */
          if (tcp_s->CWnd > tcp_s->MaxSegSize) {
            /* Update ssthresh only on first retransmission */
            uint16_t th = MIN (tcp_s->SendWin, tcp_s->CWnd) / 2;
            th          = MAX (th, tcp_s->MaxSegSize * 2);
            tcp_s->SsThresh = th;
            tcp_s->CWnd     = tcp_s->MaxSegSize;
            DEBUGF (TCP," cwnd=%d, ssth=%d\n",tcp_s->CWnd,tcp_s->SsThresh);
            EvrNetTCP_ShowCongestionVariables (tcp_s->Id, tcp_s->CWnd, tcp_s->SsThresh);
          }
          /* Make sure fast recovery mode is not active */
          tcp_s->DupAcks = 0;
          /* Retransmit all frames from the queue */
          tcp_s->Flags  &= ~TCP_IFLAG_FAST;
          tcp_que_init (tcp_s);
          tcp_que_resend (tcp_s);
          return;
        }
        /* No Retries left, reset the connection */
        tcp_que_free (tcp_s);
        tcp_s->Flags |= TCP_IFLAG_CBACK;
        goto no_retries;

      case netTCP_StateSYN_SENT:
        /* Check for a timeout! */
        if (tcp_s->RetryTimer != 0) {
          break;
        }

        DEBUGF (TCP,"Socket %d, SYN_SENT timeout\n",tcp_s->Id);
        EvrNetTCP_TimeoutInState (tcp_s->Id, netTCP_StateSYN_SENT);
        tcp_s->SendNext = tcp_s->SendUna;
        /* Timeout expired, any retries left? */
        if (tcp_s->Retries != 0) {
          tcp_s->Retries--;
          tcp_s->RetryTimer = tcp->SynRetryTout;
          /* SYN packet lost, mark a retransmission */
          tcp_s->SsThresh   = 0;
          tcp_send_ctrl (tcp_s, TCP_FLAG_SYN);
          return;
        }
        /* No Retries left, reset the connection */
        goto no_retries;

      case netTCP_StateSYN_RECEIVED:
        /* Check for a timeout! */
        if (tcp_s->RetryTimer != 0) {
          break;
        }

        DEBUGF (TCP,"Socket %d, SYN_REC timeout\n",tcp_s->Id);
        EvrNetTCP_TimeoutInState (tcp_s->Id, netTCP_StateSYN_RECEIVED);
        tcp_s->SendNext = tcp_s->SendUna;
        /* Timeout expired, any retries left? */
        if (tcp_s->Retries != 0) {
          tcp_s->Retries--;
          tcp_s->RetryTimer = tcp->RetryTout;
          /* SYN+ACK packet lost, mark a retransmission */
          tcp_s->SsThresh   = 0;
          tcp_send_ctrl (tcp_s, TCP_FLAG_SYN | TCP_FLAG_ACK);
          return;
        }
        /* No Retries left, reset the connection */
        goto no_retries;

      case netTCP_StateTIME_WAIT:
        /* Check for a timeout! */
        if (tcp_s->RetryTimer != 0) {
          break;
        }

        /* Timeout expired */
        DEBUGF (TCP,"Socket %d, TWAIT expired closing\n",tcp_s->Id);
        EvrNetTCP_TwaitTimeoutClosing (tcp_s->Id);
        tcp_transit (tcp_s, (tcp_s->Type & TCP_TYPE_SERVER) ? netTCP_StateLISTEN :
                                                              netTCP_StateCLOSED);
        break;

      case netTCP_StateLAST_ACK:
      case netTCP_StateFIN_WAIT_1:
      case netTCP_StateCLOSING:
        /* Check for a timeout! */
        if (tcp_s->RetryTimer != 0) {
          break;
        }

        DEBUGF (TCP,"Socket %d, Timeout\n",tcp_s->Id);
        EvrNetTCP_ClosingTimeout (tcp_s->Id);
        /* Timeout expired, any retries left? */
        tcp_s->SendNext = tcp_s->SendUna;
        if (tcp_s->Retries != 0) {
          tcp_s->Retries--;
          tcp_s->RetryTimer = tcp->RetryTout;
          tcp_send_ctrl (tcp_s, TCP_FLAG_FIN | TCP_FLAG_ACK);
          return;
        }
        /* No Retries left, reset the connection */
        goto no_retries;

      case netTCP_StateFIN_WAIT_2:
        /* Check for a timeout! */
        if (tcp_s->RetryTimer != 0) {
          break;
        }
        /* Timeout expired, any retries left? */
        if (tcp_s->Retries != 0) {
          /* Still keep waiting for FIN frame */
          tcp_s->Retries--;
          tcp_s->RetryTimer = tcp->RetryTout;
          break;
        }
        DEBUGF (TCP,"Socket %d, Wait for FIN timeout\n",tcp_s->Id);
        EvrNetTCP_TimeoutInState (tcp_s->Id, netTCP_StateFIN_WAIT_2);
no_retries:
        /* No Retries left, reset the connection */
        ERRORF (TCP,"Socket %d, No Retries left\n",tcp_s->Id);
        EvrNetTCP_NoRetriesLeft (tcp_s->Id);
        tcp_send_ctrl (tcp_s, TCP_FLAG_RST);
        /* Inform the application, 'buf' is non-NULL to notify a timeout */
        tcp_s->cb_func (tcp_s->Id, netTCP_EventAborted,
                                  (NET_ADDR *)&tcp_s->Peer, &tcp_s->Retries, 0);
        tcp_transit (tcp_s, (tcp_s->Type & TCP_TYPE_SERVER) ? netTCP_StateLISTEN :
                                                              netTCP_StateCLOSED);
        return;
    }
  }
}  /* End of net_tcp_socket_run () */

/**
  \brief       Process received TCP frame.
  \param[in]   net_if  network interface descriptor.
  \param[in]   frame   network frame.
  \param[in]   ip_ver  IP version.
*/
void net_tcp_process (NET_IF_CFG *net_if, NET_FRAME *frame, uint8_t ip_ver) {
  NET_TCP_INFO *tcp_s;
  NET_TCP_HEADER *tcp_hdr;
  uint16_t hlen, dlen;
  uint32_t seqnr, acknr, win_delta, retv;

  DEBUGF (TCP,"*** Process_frame ***\n");
  /* Calculate TCP layer data length and start index */
  tcp_hdr = __ALIGN_CAST(NET_TCP_HEADER *)&frame->data[frame->index];

  hlen = (tcp_hdr->HLen >> 2) & ~0x03;
  if (hlen < TCP_HEADER_LEN || hlen > frame->length) {
    /* Frame too short */
    ERRORF (TCP,"Process, Frame too short\n");
    EvrNetTCP_FrameTooShort (frame->length, TCP_HEADER_LEN);
    return;
  }
  /* Update index to start of TCP data */
  frame->index += hlen;
  dlen = frame->length - hlen;

  DEBUG_INF2 (tcp_hdr, dlen);
  EvrNetTCP_ReceiveFrame (frame->length, ip_ver);
  EvrNetTCP_ShowFrameHeader (tcp_hdr);

  /* Map the TCP socket */
  tcp_s = tcp_map_socket (net_if, frame, tcp_hdr, ip_ver);
  if (tcp_s == NULL) {
    /* No active Socket found, Reset the connection */
    DEBUGF (TCP," Discarded, Frame not mapped\n");
    EvrNetTCP_FrameNotMapped (frame->length);
    tcp_send_reset (net_if, frame, tcp_hdr, ip_ver);
    return;
  }
  DEBUGF (TCP," Mapped to Socket %d, state %s\n",tcp_s->Id,
                                                 net_tcp_ntoa(tcp_s->State));
  EvrNetTCP_MapFrameToSocket (tcp_s->Id, tcp_s->State);

  /* Calculate checksum of received frame */
  if (ip_ver == IP_VER4) {
    if (!(sys->RxOffload & SYS_OFFL_TCP4_RX) &&
        (net_ip4_chksum (IP4_FRAME(frame)->SrcAddr, IP4_FRAME(frame)->DstAddr,
                         tcp_hdr, IP4_PROT_TCP, frame->length) != 0)) {
      /* IPv4 Checksum check failed */
      ERRORF (TCP,"Socket %d, Receive checksum failed\n",tcp_s->Id);
      EvrNetTCP_ChecksumFailed (tcp_s->Id);
      return;
    }
  }
  else {
#ifdef Network_IPv6
    if (!(sys->RxOffload & SYS_OFFL_TCP6_RX) &&
        (net_ip6_chksum (IP6_FRAME(frame)->SrcAddr, IP6_FRAME(frame)->DstAddr,
                         tcp_hdr, IP6_PROT_TCP, frame->length) != 0)) {
      /* IPv6 Checksum check failed */
      ERRORF (TCP,"Socket %d, Receive checksum failed\n",tcp_s->Id);
      EvrNetTCP_ChecksumFailed (tcp_s->Id);
      return;
    }
#else
    return;
#endif
  }

  /* Read sequence and acknowledge numbers */
  seqnr = get_u32(&tcp_hdr->SeqNr);
  acknr = get_u32(&tcp_hdr->AckNr);

  /* Process the packet on TCP state machine */
  switch (tcp_s->State) {
    case netTCP_StateESTABLISHED:
      /* Check for RESET frame */
      if (tcp_hdr->Flags & TCP_FLAG_RST) {
        if (seqnr == tcp_s->RecNext) {
          /* Only exact match can reset the connection.   */
          /* This improves robustness. (RFC5961 - page 6) */
          goto conn_reset;
        }
        if (SEQ_GE (seqnr, tcp_s->RecNext) &&
            SEQ_LT (seqnr, tcp_s->RecNext + tcp_s->RecWin)) {
          /* Sequence is within current receive window. */
          /* Send a challenge ACK.   (RFC5961 - page 6) */
          DEBUGF (TCP," RST in window\n");
          EvrNetTCP_RstInWindow (tcp_s->Id);
          tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
          return;
        }
        /* Sequence is outside current receive window.  */
        /* Silently drop the packet. (RFC5961 - page 6) */
        DEBUGF (TCP," RST not valid\n");
        EvrNetTCP_RstNotValid (tcp_s->Id);
        return;
      }

      /* Check for SYN, maybe the peer didn't get our SYN+ACK or ACK? */
      if (tcp_hdr->Flags & TCP_FLAG_SYN) {
        /* Is it the SYN+ACK we have already ACKed but maybe our ACK lost? */
        if (tcp_hdr->Flags & TCP_FLAG_ACK) {
          if ((seqnr + 1) == tcp_s->RecNext  &&
               acknr      == tcp_s->SendNext) {
            /* The sequence is correct, ack the SYN frame */
            DEBUGF (TCP," Repeated SYN+ACK\n");
            EvrNetTCP_RepeatedSynAck (tcp_s->Id);
            tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
            return;
          }
        }
        /* It is maybe SYN again so remote peer didn't get our SYN+ACK? */
        /* Let our retransmission handle it in net_tcp_socket_run(). */
        return;
      }

      /* Check for ACK, the flag should be set */
      if (!(tcp_hdr->Flags & TCP_FLAG_ACK)) {
        /* Frame without ACK ignore it */
        DEBUGF (TCP," ACK not set\n");
        EvrNetTCP_AckNotSet (tcp_s->Id);
        return;
      }

      win_delta = 0;
      /* Check if send window should be updated? */
      if (SEQ_GE (acknr, tcp_s->SendUna) &&
          SEQ_LE (acknr, tcp_s->SendNext)) {
        /* if (SendUna <= acknr <= SendNext) (RFC1122 - page 94) */
        uint16_t frm_win = ntohs(tcp_hdr->Window);
        if (SEQ_LT (tcp_s->SendWl2, acknr) ||
            SEQ_LT (tcp_s->SendWl1, seqnr) ||
            (tcp_s->SendWl1 == seqnr && frm_win > tcp_s->SendWin)) {
          /* The criteria for send window update is:        */
          /*   a) the segment acknowledges some data or     */
          /*   b) the segment is new (i.e. it has a         */
          /*      higher sequence number) or                */
          /*   c) the advertised window is larger than the  */
          /*      previous advertised window                */
          win_delta = frm_win - tcp_s->SendWin;
          tcp_s->SendWin = frm_win;
          DEBUGF (TCP," SendWin: %d\n",tcp_s->SendWin);
          EvrNetTCP_ShowSendWindow (tcp_s->Id, tcp_s->SendWin);
          tcp_s->SendWl1 = seqnr;
          tcp_s->SendWl2 = acknr;
        }
      }

      /* Check if there is unacked data? */
      if (tcp_s->SendUna != tcp_s->SendNext) {
        /* Check for duplicate acks!    (RFC 5681 - page 3) */
        /* An acknowledge is considered a duplicate when:   */
        /*   a) there is outstanding unacked data           */
        /*   b) the ack packet carries no data (dlen is 0)  */
        /*   c) the SYN and FIN flags are off               */
        /*   d) the ack number is equal to SendUna          */
        /*   e) the advertised window is the same as last   */
        /*      advertised window received                  */
        if (SEQ_LE (acknr, tcp_s->SendChk) && (dlen == 0)  &&
            !(tcp_hdr->Flags & TCP_FLAG_FIN) && (win_delta == 0)) {
          /* Yes, this is a duplicate ack */
          if (tcp_s->DupAcks < 255) {
            /* Safety prevent overflows */
            tcp_s->DupAcks++;
          }
          tcp_proc_dupack (tcp_s);
        }
        else if (SEQ_GT (acknr, tcp_s->SendChk)) {
          /* Reset the count of consecutive duplicate acks */
          tcp_s->DupAcks = 0;
        }

        /* Process this ack sequence */
        tcp_proc_acknr (tcp_s, acknr);
        if (tcp_s->Flags & TCP_IFLAG_CLOSING) {
          if (acknr == tcp_s->SendNext) {
            /* FIN + data acked */
            tcp_transit (tcp_s, netTCP_StateFIN_WAIT_1);
            /* Continue packet processing in FINW1 state */
            goto state_finw1;
          }
          if ((acknr + 1) == tcp_s->SendNext) {
            /* Only data acked, wait for FIN to be acked too */
            tcp_transit (tcp_s, netTCP_StateFIN_WAIT_1);
            return;
          }
        }
      }

      tcp_s->Flags &= ~TCP_IFLAG_KALIVE;
      /* Check received data frame for the sequence numbers */
      if (tcp_s->RecNext != seqnr) {
        if (tcp_s->RecNext == (seqnr + 1)) {
          /* This is a keep-alive segment, send ack */
          DEBUGF (TCP," Keep-alive segment received\n");
          EvrNetTCP_KeepAliveSegment (tcp_s->Id);
          tcp_s->AliveTimer = tcp_s->ConnTout;
        }
        else if (tcp_s->RecNext == (seqnr + dlen)) {
          /* This is a retransmitted segment, maybe our ACK was lost */
          DEBUGF (TCP," Retransmitted segment received\n");
          EvrNetTCP_RetransmittedSegment (tcp_s->Id);
        }
        else {
          /* Out of range sequence number received */
          ERRORF (TCP,"Socket %d, Out of range segment received\n",tcp_s->Id);
          EvrNetTCP_OutOfRangeSegment (tcp_s->Id);
        }
        tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
        return;
      }

      tcp_s->Flags |= TCP_IFLAG_CBACK;
      /* If this is data frame, notify the application */
      if (dlen) {
        /* Check if Zero Window Probe (ZWP) frame */
        if (tcp_s->RecWin) {
          /* This data frame is also an ack frame, all in one. If   */
          /* the application is waiting for an ack, we need to send */
          /* netTCP_EventACK event, before delivering the data.     */
          if (tcp_s->Flags & TCP_IFLAG_DACK) {
            tcp_s->Flags &= ~TCP_IFLAG_DACK;
            DEBUGF (TCP," Callback EventACK\n");
            EvrNetTCP_CallbackEventAck (tcp_s->Id);
            tcp_s->cb_func (tcp_s->Id, netTCP_EventACK, (NET_ADDR *)&tcp_s->Peer, NULL, 0);
          }
          /* Data frame OK, generate data event to application */
          if (tcp_s->cb_func (tcp_s->Id, netTCP_EventData, (NET_ADDR *)&tcp_s->Peer,
                              &frame->data[frame->index], dlen) == TCP_NO_ACK) {
            /* BSD socket is not able to buffer the data. Do not send */
            /* an ack. The data will be resent again by remote peer.  */
            return;
          }
          tcp_s->RecNext += dlen;
        }
        else {
          /* Receive Window is closed. This is ZWP frame, */
          /* ignore the data. ZWP has typically 1 byte.   */
          DEBUGF (TCP," Zero Window Probe received\n");
          EvrNetTCP_ZeroWindowProbe (tcp_s->Id);
        }
      }

      /* Check if the FIN flag is set? */
      if (tcp_hdr->Flags & TCP_FLAG_FIN) {
        /* The remote peer want's to close the connection */
        DEBUGF (TCP," Remote peer wants to close\n");
        EvrNetTCP_RemotePeerClosing (tcp_s->Id);
        tcp_s->RecNext++;
        if (tcp_s->SendUna == tcp_s->SendNext) {
          /* We don't have unacked data, inform the application */
          tcp_s->cb_func (tcp_s->Id, netTCP_EventClosed, (NET_ADDR *)&tcp_s->Peer, NULL, 0);
          /* ACK the FIN and set our own FIN */
          tcp_send_ctrl (tcp_s, TCP_FLAG_ACK | TCP_FLAG_FIN);
          tcp_transit (tcp_s, netTCP_StateLAST_ACK);
          return;
        }
      }
      tcp_s->Flags &= ~TCP_IFLAG_CBACK;

      /* If this is data frame, ack the data? */
      if (dlen) {
        if (tcp_s->Type & TCP_TYPE_FLOW_CTRL) {
          /* Socket has FLOW Control properties, reduce Window Size */
          if (tcp_s->RecWin > dlen) {
            tcp_s->RecWin -= dlen;
          }
          else {
            /* Safety close receive window */
            tcp_s->RecWin  = 0;
          }
          DEBUGF (TCP," RecWin : %d\n",tcp_s->RecWin);
          EvrNetTCP_ShowReceiveWindow (tcp_s->Id, tcp_s->RecWin);
        }
        if ((tcp_s->Type & TCP_TYPE_DELAY_ACK) &&
            (tcp_s->AckTimer == 0) && (tcp_s->RecWin != 0)) {
          /* Send ACK delayed from net_tcp_socket_run() */
          tcp_s->AckTimer = SYS_TICK_T200MS + 1;
        }
        else {
          /* Send ACK now! */
          tcp_s->AckTimer = 0;
          tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
        }
      }
      tcp_s->AliveTimer = tcp_s->ConnTout;
      return;

    case netTCP_StateLISTEN:
      /* Check the frame Flags! */
      if (tcp_hdr->Flags & TCP_FLAG_RST) {
        tcp_transit (tcp_s, netTCP_StateLISTEN);
        return;
      }

      if (tcp_hdr->Flags & TCP_FLAG_ACK) {
        /* Error, ACK received */
        ERRORF (TCP,"Socket %d, Invalid ACK received\n",tcp_s->Id);
        EvrNetTCP_InvalidAck (tcp_s->Id);
        /* Reset the connection */
        tcp_send_reset (net_if, frame, tcp_hdr, ip_ver);
        tcp_transit (tcp_s, netTCP_StateLISTEN);
        return;
      }

      if ((tcp_hdr->Flags & TCP_FLAG_SYN) == 0) {
        /* SYN flag is missing in frame */
        ERRORF (TCP,"Socket %d, No SYN received\n",tcp_s->Id);
        EvrNetTCP_SynNotSet (tcp_s->Id);
        /* Reset the connection */
        tcp_send_reset (net_if, frame, tcp_hdr, ip_ver);
        tcp_transit (tcp_s, netTCP_StateLISTEN);
        return;
      }

      /* SYN flag received, inform the application and see if accepted */
      retv = tcp_s->cb_func (tcp_s->Id, netTCP_EventConnect, (NET_ADDR *)&tcp_s->Peer, NULL, 0);
      if (retv == 0) {
        /* Application rejected this connection */
        DEBUGF (TCP," Connection rejected by application\n");
        EvrNetTCP_UserConnectionReject (tcp_s->Id);
        tcp_send_reset (net_if, frame, tcp_hdr, ip_ver);
        tcp_transit (tcp_s, netTCP_StateLISTEN);
        return;
      }
      if (retv == TCP_NO_ACK) {
        /* BSD socket is not able to accept this connection */
        /* Do not ACK the SYN frame, remote peer will retry */
        /* to connect again after a timeout.                */
        return;
      }
      /* Process TCP option field if present */
      tcp_parse_opt (tcp_s, tcp_hdr);

      /* The connect request was accepted */
      tcp_s->net_if   = net_if;
      tcp_s->SendUna  = net_rand32 ();
      tcp_s->SendNext = tcp_s->SendUna;
      tcp_s->SendChk  = tcp_s->SendUna;
      tcp_s->SendWin  = ntohs(tcp_hdr->Window);
      DEBUGF (TCP," SendWin: %d\n",tcp_s->SendWin);
      EvrNetTCP_ShowSendWindow (tcp_s->Id, tcp_s->SendWin);

      /* Initialize to seqnr-1 to force window update */
      tcp_s->SendWl1  = seqnr - 1;
      tcp_s->RecNext  = seqnr + 1;
      tcp_s->SsThresh = 65535;
      tcp_s->DupAcks  = 0;

      /* RTT estimation defaults */
      tcp_s->RttSa    = 0;
      tcp_s->RttSv    = (int16_t)tcp->RetryTout;
      DEBUG_INFO (tcp_s);
      EvrNetTCP_ShowRttVariables (tcp_s->Id, tcp_s->RttSa, tcp_s->RttSv);

      DEBUGF (TCP," Next state SYN_REC\n");
      EvrNetTCP_NextState (tcp_s->Id, netTCP_StateSYN_RECEIVED);
      /* Ack SYN frame */
      tcp_send_ctrl (tcp_s, TCP_FLAG_SYN | TCP_FLAG_ACK);
      tcp_transit (tcp_s, netTCP_StateSYN_RECEIVED);
      return;

    case netTCP_StateSYN_RECEIVED:
      /* Check the frame Flags! */
      if (tcp_hdr->Flags & TCP_FLAG_RST) {
        goto conn_reset;
      }
      /* Is it SYN+ACK? This coud happen because of simultaneous open */
      if ((tcp_hdr->Flags & TCP_FLAG_SYN) && (tcp_hdr->Flags & TCP_FLAG_ACK)) {
        if (acknr != tcp_s->SendNext) {
          /* SYN+ACK received but wrong Ack seq number */
          ERRORF (TCP,"Socket %d, Wrong SYN+ACK received\n",tcp_s->Id);
          EvrNetTCP_WrongSynAck (tcp_s->Id);
          return;
        }
        tcp_set_cwnd (tcp_s);
        tcp_s->SendWin = ntohs(tcp_hdr->Window);
        DEBUGF (TCP," SendWin: %d\n",tcp_s->SendWin);
        EvrNetTCP_ShowSendWindow (tcp_s->Id, tcp_s->SendWin);
        /* Initialize to seqnr-1 to force window update */
        tcp_s->SendWl1 = seqnr - 1;
        tcp_s->RecNext = seqnr + 1;
        /* We have no unacked data */
        tcp_s->SendUna = tcp_s->SendNext;
        /* SYN+ACK received, this side is CONNECTED */
        DEBUGF (TCP," Next state ESTABLISHED\n");
        EvrNetTCP_NextState (tcp_s->Id, netTCP_StateESTABLISHED);
        tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
        /* Inform the application */
        tcp_s->cb_func (tcp_s->Id, netTCP_EventEstablished, (NET_ADDR *)&tcp_s->Peer, NULL, 0);
        tcp_transit (tcp_s, netTCP_StateESTABLISHED);
        return;
      }

      /* Is it ACK? */
      if (tcp_hdr->Flags & TCP_FLAG_ACK) {
        if (acknr != tcp_s->SendNext) {
          /* Wrong ACK received */
          ERRORF (TCP,"Socket %d, Wrong ACK received\n",tcp_s->Id);
          EvrNetTCP_WrongAckNumber (tcp_s->Id);
          return;
        }
        if (seqnr != tcp_s->RecNext) {
          /* ACK received with Wrong SEQ number */
          ERRORF (TCP,"Socket %d, Wrong SEQ received\n",tcp_s->Id);
          EvrNetTCP_WrongSeqNumber (tcp_s->Id);
          return;
        }
        tcp_set_cwnd (tcp_s);
        tcp_s->SendUna = tcp_s->SendNext;
        /* ACK received, this side is CONNECTED */
        DEBUGF (TCP," Next state ESTABLISHED\n");
        EvrNetTCP_NextState (tcp_s->Id, netTCP_StateESTABLISHED);
        /* Inform the application */
        tcp_s->cb_func (tcp_s->Id, netTCP_EventEstablished, (NET_ADDR *)&tcp_s->Peer, NULL, 0);
        tcp_transit (tcp_s, netTCP_StateESTABLISHED);
        return;
      }

      /* Is it SYN? */
      if (tcp_hdr->Flags & TCP_FLAG_SYN) {
        /* Repeated SYN, our SYN+ACK was lost */
        DEBUGF (TCP," Repeated SYN\n");
        EvrNetTCP_RepeatedSyn (tcp_s->Id);
        return;
      }

      /* Unrecognized frame, answer with RESET */
      ERRORF (TCP,"Socket %d, Unknown frame received\n",tcp_s->Id);
      EvrNetTCP_FrameUnrecognised (tcp_s->Id);
      tcp_send_reset (net_if, frame, tcp_hdr, ip_ver);
      return;

    case netTCP_StateSYN_SENT:
      /* Check for RESET! */
      if (tcp_hdr->Flags & TCP_FLAG_RST) {
        goto conn_reset;
      }

      /* Is it SYN+ACK? */
      if ((tcp_hdr->Flags & TCP_FLAG_SYN) && (tcp_hdr->Flags & TCP_FLAG_ACK)) {
        if (acknr != tcp_s->SendNext) {
          /* SYN+ACK received but wrong ack */
          ERRORF (TCP,"Socket %d, Wrong SYN+ACK received\n",tcp_s->Id);
          EvrNetTCP_WrongSynAck (tcp_s->Id);
          return;
        }
        /* Process TCP option field if present */
        tcp_parse_opt (tcp_s, tcp_hdr);
        tcp_set_cwnd (tcp_s);

        /* We have no unacked data */
        tcp_s->SendUna = tcp_s->SendNext;
        tcp_s->SendWin = ntohs(tcp_hdr->Window);
        DEBUGF (TCP," SendWin: %d\n",tcp_s->SendWin);
        EvrNetTCP_ShowSendWindow (tcp_s->Id, tcp_s->SendWin);

        /* Initialize to seqnr-1 to force window update */
        tcp_s->SendWl1 = seqnr - 1;
        tcp_s->RecNext = seqnr + 1;

        /* SYN+ACK received, this side is CONNECTED */
        DEBUGF (TCP," Next state ESTABLISHED\n");
        EvrNetTCP_NextState (tcp_s->Id, netTCP_StateESTABLISHED);
        tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
        /* Inform the application */
        tcp_s->cb_func (tcp_s->Id, netTCP_EventEstablished, (NET_ADDR *)&tcp_s->Peer, NULL, 0);
        tcp_transit (tcp_s, netTCP_StateESTABLISHED);
        return;
      }

      /* Is it SYN (both peers simultaneous open)? */
      if (tcp_hdr->Flags & TCP_FLAG_SYN) {
        /* Process TCP option field if present */
        tcp_parse_opt (tcp_s, tcp_hdr);
        /* Simultaneous open, next SYN received */
        DEBUGF (TCP," Simultaneous open, next state SYN_REC\n");
        EvrNetTCP_SimultOpenNextState (tcp_s->Id, netTCP_StateSYN_RECEIVED);
        /* Get remote seq number! */
        tcp_s->RecNext = seqnr + 1;
        tcp_send_ctrl (tcp_s, TCP_FLAG_SYN | TCP_FLAG_ACK);
        tcp_transit (tcp_s, netTCP_StateSYN_RECEIVED);
        return;
      }

      /* This is something we didn't understood. Maybe the other */
      /* peer has still old connection on or something, need to  */
      /* reset the connection.                                   */
      ERRORF (TCP,"Socket %d, Unknown frame received\n",tcp_s->Id);
      EvrNetTCP_FrameUnrecognised (tcp_s->Id);
      tcp_send_reset (net_if, frame, tcp_hdr, ip_ver);
      return;

    case netTCP_StateFIN_WAIT_1:
      /* Check for RESET */
      if (tcp_hdr->Flags & TCP_FLAG_RST) {
        goto conn_reset;
      }
state_finw1:
      /* Is it FIN+ACK? */
      if ((tcp_hdr->Flags & TCP_FLAG_FIN) && (tcp_hdr->Flags & TCP_FLAG_ACK)) {
        /* Simultaneous closing from both sides */
        if (acknr != tcp_s->SendNext) {
          if (acknr != tcp_s->SendUna) {
            /* FIN+ACK received but wrong ack */
            ERRORF (TCP,"Socket %d, Wrong FIN+ACK received\n",tcp_s->Id);
            EvrNetTCP_WrongFinAck (tcp_s->Id);
            return;
          }
        }
        /* ACK the FIN and all data */
        DEBUGF (TCP," Received FIN+ACK, next state TWAIT\n");
        EvrNetTCP_FinAckNextState (tcp_s->Id, netTCP_StateTIME_WAIT);
        tcp_s->RecNext = seqnr + dlen + 1;
        tcp_s->SendUna = tcp_s->SendNext;
        tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
        tcp_transit (tcp_s, netTCP_StateTIME_WAIT);
        return;
      }

      /* Is it just FIN? */
      if (tcp_hdr->Flags & TCP_FLAG_FIN) {
        /* Simultaneous close, ACK the FIN and all data */
        DEBUGF (TCP," Simultaneous close, next state CLOSING\n");
        EvrNetTCP_SimultCloseNextState (tcp_s->Id, netTCP_StateCLOSING);
        tcp_s->RecNext = seqnr + dlen + 1;
        tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
        tcp_transit (tcp_s, netTCP_StateCLOSING);
        return;
      }

      /* Is it just ACK? */
      if (tcp_hdr->Flags & TCP_FLAG_ACK) {
        if ((acknr + 1) == tcp_s->SendNext) {
          /* Ignore a duplicate ACK */
          break;
        }
        if (acknr != tcp_s->SendNext) {
          /* Wrong ACK received */
          ERRORF (TCP,"Socket %d, Wrong ACK received\n",tcp_s->Id);
          EvrNetTCP_WrongAckNumber (tcp_s->Id);
          return;
        }
        /* We have no unacked data */
        tcp_s->SendUna = tcp_s->SendNext;
        DEBUGF (TCP," Received ACK, next state FINW2\n");
        EvrNetTCP_AckNextState (tcp_s->Id, netTCP_StateFIN_WAIT_2);
        tcp_transit (tcp_s, netTCP_StateFIN_WAIT_2);
        return;
      }
      break;

    case netTCP_StateFIN_WAIT_2:
      /* Check for RESET */
      if (tcp_hdr->Flags & TCP_FLAG_RST) {
        goto conn_reset;
      }

      /* Do we finally get FIN? */
      if (tcp_hdr->Flags & TCP_FLAG_FIN) {
        /* ACK the FIN and all data */
        DEBUGF (TCP," Received FIN, next state TWAIT\n");
        EvrNetTCP_FinNextState (tcp_s->Id, netTCP_StateTIME_WAIT);
        tcp_s->RecNext = seqnr + dlen + 1;
        tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
        tcp_transit (tcp_s, netTCP_StateTIME_WAIT);
        return;
      }

      /* Is it PSH+ACK? */
      if ((tcp_hdr->Flags & TCP_FLAG_PSH) && (tcp_hdr->Flags & TCP_FLAG_ACK)) {
        if (acknr != tcp_s->SendNext) {
          /* Wrong ACK received */
          ERRORF (TCP,"Socket %d, Wrong ACK received\n",tcp_s->Id);
          EvrNetTCP_WrongAckNumber (tcp_s->Id);
          return;
        }
        if (tcp_s->RecNext != seqnr) {
          if (tcp_s->RecNext == (seqnr + dlen)) {
            /* This is a retransmitted segment, maybe our ACK was lost */
            DEBUGF (TCP," Retransmitted segment received\n");
            EvrNetTCP_RetransmittedSegment (tcp_s->Id);
          }
          else {
            /* Out of range sequence number received */
            ERRORF (TCP,"Socket %d, Out of range segment received\n",tcp_s->Id);
            EvrNetTCP_OutOfRangeSegment (tcp_s->Id);
          }
          tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
          return;
        }
        DEBUGF (TCP," Received PSH+ACK, socket half-closed\n");
        EvrNetTCP_PshAckInHalfClosed (tcp_s->Id);
        if (dlen) {
          /* Data frame, notify the application */
          tcp_s->Flags |=  TCP_IFLAG_CBACK;
          tcp_s->cb_func (tcp_s->Id, netTCP_EventData, (NET_ADDR *)&tcp_s->Peer,
                                     &frame->data[frame->index], dlen);
          tcp_s->Flags &= ~TCP_IFLAG_CBACK;
          tcp_s->RecNext += dlen;
        }
        tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
        return;
      }
      break;

    case netTCP_StateCLOSING:
      /* Check for RESET */
      if (tcp_hdr->Flags & TCP_FLAG_RST) {
        goto conn_reset;
      }

      /* Is it ACK? */
      if (tcp_hdr->Flags & TCP_FLAG_ACK) {
        if (acknr != tcp_s->SendNext) {
          /* Wrong ACK received */
          ERRORF (TCP,"Socket %d, Wrong ACK received\n",tcp_s->Id);
          EvrNetTCP_WrongAckNumber (tcp_s->Id);
          return;
        }
        /* We have no unacked data */
        DEBUGF (TCP," Received ACK, next state TWAIT\n");
        EvrNetTCP_AckNextState (tcp_s->Id, netTCP_StateTIME_WAIT);
        tcp_s->SendUna = tcp_s->SendNext;
        tcp_transit (tcp_s, netTCP_StateTIME_WAIT);
        return;
      }

      /* Is it repeated FIN? */
      if (tcp_hdr->Flags & TCP_FLAG_FIN) {
        /* ACK the FIN and all data */
        DEBUGF (TCP," Repeated FIN, repeat ACK\n");
        EvrNetTCP_RepeatedFin (tcp_s->Id);
        tcp_s->RecNext = seqnr + dlen + 1;
        tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
        return;
      }
      break;

    case netTCP_StateLAST_ACK:
      /* Check for RESET */
      if (tcp_hdr->Flags & TCP_FLAG_RST) {
        goto conn_reset;
      }

      /* Is it ACK? */
      if (tcp_hdr->Flags & TCP_FLAG_ACK) {
        if (acknr != tcp_s->SendNext) {
          ERRORF (TCP,"Socket %d, Wrong ACK received\n",tcp_s->Id);
          EvrNetTCP_WrongAckNumber (tcp_s->Id);
          return;
        }
        /* Last ACK received */
        DEBUGF (TCP," Last ACK received, next State %s\n",
                (tcp_s->Type & TCP_TYPE_SERVER) ? "LISTEN" : "CLOSED");
        EvrNetTCP_LastAckNextState (tcp_s->Id,(tcp_s->Type & TCP_TYPE_SERVER) ?
                                              netTCP_StateLISTEN : netTCP_StateCLOSED);
        tcp_s->SendUna = tcp_s->SendNext;
        tcp_transit (tcp_s, (tcp_s->Type & TCP_TYPE_SERVER) ? netTCP_StateLISTEN :
                                                              netTCP_StateCLOSED);
        return;
      }

      /* Is it repeated FIN? */
      if (tcp_hdr->Flags & TCP_FLAG_FIN) {
        /* ACK the FIN and all data */
        DEBUGF (TCP," Repeated FIN, repeat ACK\n");
        EvrNetTCP_RepeatedFin (tcp_s->Id);
        tcp_s->RecNext = seqnr + dlen + 1;
        tcp_send_ctrl (tcp_s, TCP_FLAG_FIN | TCP_FLAG_ACK);
        return;
      }
      break;

    case netTCP_StateTIME_WAIT:
      /* Check for RESET */
      if (tcp_hdr->Flags & TCP_FLAG_RST) {
        /* Inform the application */
conn_reset:
        ERRORF (TCP,"Socket %d, RST received\n",tcp_s->Id);
        EvrNetTCP_RstReceived (tcp_s->Id);
        tcp_s->cb_func (tcp_s->Id, netTCP_EventAborted, (NET_ADDR *)&tcp_s->Peer, NULL, 0);
        tcp_transit (tcp_s, (tcp_s->Type & TCP_TYPE_SERVER) ? netTCP_StateLISTEN :
                                                              netTCP_StateCLOSED);
        return;
      }

      /* Is it repeated FIN? */
      if (tcp_hdr->Flags & TCP_FLAG_FIN) {
        /* ACK the FIN and all data */
        DEBUGF (TCP," Repeated FIN, repeat ACK\n");
        EvrNetTCP_RepeatedFin (tcp_s->Id);
        tcp_s->RecNext = seqnr + dlen + 1;
        tcp_send_ctrl (tcp_s, TCP_FLAG_ACK);
        return;
      }
      break;

    default:
      /* We should never come up to here! */
      ERRORF (TCP,"Socket %d, Invalid state\n",tcp_s->Id);
      EvrNetTCP_InvalidState (tcp_s->Id);
      net_sys_error (NET_ERROR_TCP_STATE);
      break;
  }
} /* End of net_tcp_process () */

/**
  \brief       Construct TCP header and send the frame.
  \param[in]   net_if    network interface descriptor.
  \param[in]   frame     network frame.
  \param[in]   dst_addr  destination IP address.
  \param[in]   loc_addr  local IP address.
  \return      status:
               - true  = success,
               - false = error.
  \note        Paramaters for TCP header construction are located in
               memory, which is reserved for IP and ethernet headers.
               This memory area is initialized in ip_send_frame() and
               is not used in TCP layer. Location and layout structure
               is defined with TCP_WI() macro.
*/
static bool tcp_write (NET_IF_CFG *net_if, NET_FRAME *frame,
                       const uint8_t *dst_addr, const uint8_t *loc_addr) {
  NET_TCP_HEADER *tcp_hdr;

  /* There is reserved a space for protocol headers */
  tcp_hdr = __ALIGN_CAST(NET_TCP_HEADER *)&frame->data[IP6_DATA_OFFS];

  /* Construct TCP header in network byte order */
  tcp_hdr->SrcPort  = htons(TCP_WI(frame)->sport);
  tcp_hdr->DstPort  = htons(TCP_WI(frame)->dport);
  set_u32 (&tcp_hdr->SeqNr, TCP_WI(frame)->seqnr);
  set_u32 (&tcp_hdr->AckNr, TCP_WI(frame)->acknr);
  tcp_hdr->HLen     = (TCP_HEADER_LEN / 4) << 4;
  tcp_hdr->Flags    = TCP_WI(frame)->flags;
  tcp_hdr->Window   = htons(TCP_WI(frame)->window);
  tcp_hdr->Chksum   = 0;
  tcp_hdr->UrgentPtr= 0;

  frame->index = frame->length;
  /* Add MSS option to SYN packet */
  if (TCP_WI(frame)->flags & TCP_FLAG_SYN) {
    /* Option is generated for 0-size frame */
    uint16_t *dp = &__ALIGNED_UINT16(&frame->data[TCP_DATA_OFFS]);
    dp[0] = HTONS(0x0204);
    /* SYN, send our MSS option value */
    dp[1] = htons(tcp->MaxSegSize);
    tcp_hdr->HLen = ((TCP_HEADER_LEN / 4) + 1) << 4;
    frame->length = 4;
  }
  /* Calculate frame length (including TCP header) */
  frame->length += TCP_HEADER_LEN;

  /* Calculate TCP frame checksum */
  if (TCP_WI(frame)->ver == IP_VER4) {
    if (!net_ip4_tx_offl_hl (net_if, SYS_OFFL_TCP4_TX, frame->length)) {
      uint16_t cks = net_ip4_chksum (net_ip4_get_local (net_if, dst_addr), dst_addr,
                                     tcp_hdr, IP4_PROT_TCP, frame->length);
      tcp_hdr->Chksum = htons(cks);
    }
  }
#ifdef Network_IPv6
  else {
    if (!net_ip6_tx_offl_hl (net_if, SYS_OFFL_TCP6_TX)) {
      uint16_t cks = net_ip6_chksum (loc_addr, dst_addr, tcp_hdr,
                                     IP6_PROT_TCP, frame->length);
      tcp_hdr->Chksum = htons(cks);
    }
  }
#endif

  DEBUG_INF2 (tcp_hdr, frame->index);
  EvrNetTCP_ShowFrameHeader (tcp_hdr);

  /* Send the frame to IP */
  if (TCP_WI(frame)->ver == IP_VER4) {
    uint8_t tos = TCP_WI(frame)->opt;
    /* IPv4 header is shorter than IPv6, so the start of frame is  */
    /* moved to 20 byte offset within the original frame. This new */
    /* origin is used only temporary in IPv4 and ethernet layer to */
    /* construct headers and send the frame. With this concept, we */
    /* avoid large memcpy() of TCP data !!!                        */
    NET_WI4(frame)->length = frame->length;
    return (net_ip4_send_frame (net_if, NET_WI4(frame),
                                dst_addr, IP4_PROT_TCP, tos, 128));
  }
  else {
#ifdef Network_IPv6
    uint8_t tclass = TCP_WI(frame)->opt;
    /* Initial position of TCP data in a frame is calculated for   */
    /* IPv6, so the data is already in place. No memcpy() needed.  */
    return (net_ip6_send_frame (net_if, frame, loc_addr,
                                dst_addr, IP6_PROT_TCP, tclass, 0));
#else
    (void)loc_addr;
    return (false);
#endif
  }
}

/**
  \brief       Send data frame and update send sequence.
  \param[in]   tcp_s  socket descriptor.
  \param[in]   frame  network frame.
  \param[in]   len    data length.
  \note        If len is or-ed with 0x80000000, then seqnr is already
               in place for a resend.
*/
static void tcp_send_data (NET_TCP_INFO *tcp_s, NET_FRAME *frame, uint32_t len) {

  DEBUGF (TCP,"Socket %d, Sending Data\n",tcp_s->Id);
  EvrNetTCP_SendData (tcp_s->Id, len & 0xFFFF);

  frame->length = len & 0xFFFF;

  /* Write parameters for tcp_write() to reserved memory */
  TCP_WI(frame)->sport = tcp_s->LocPort;
  TCP_WI(frame)->dport = tcp_s->Peer.port;
  if (!(len & 0x80000000)) {
    /* Not in retransmission, update SendNext sequence */
    TCP_WI(frame)->seqnr = tcp_s->SendNext;
    tcp_s->SendNext   += frame->length;
  }
  TCP_WI(frame)->acknr = tcp_s->RecNext;
  TCP_WI(frame)->window= tcp_s->RecWin;
  TCP_WI(frame)->flags = TCP_FLAG_ACK;
  if (tcp_s->Flags & TCP_IFLAG_PUSH) {
    tcp_s->Flags &= ~TCP_IFLAG_PUSH;
    TCP_WI(frame)->flags = TCP_FLAG_ACK | TCP_FLAG_PSH;
  }
  if (tcp_s->Peer.addr_type == NET_ADDR_IP4) {
    TCP_WI(frame)->ver = IP_VER4;
    TCP_WI(frame)->opt = tcp_s->Tos;
  }
  else {
    TCP_WI(frame)->ver = IP_VER6;
    TCP_WI(frame)->opt = tcp_s->TClass;
  }
  /* Clear pending acknowledge request */
  tcp_s->AckTimer = 0;
  tcp_write (tcp_s->net_if, frame, tcp_s->Peer.addr, tcp_s->LocAddr);
}

/**
  \brief       Send control frame and update send sequence.
  \param[in]   tcp_s  socket descriptor.
  \param[in]   flags  TCP flags.
  \note        Control frame contains no data.
*/
static void tcp_send_ctrl (NET_TCP_INFO *tcp_s, uint8_t flags) {
  NET_FRAME *frame;

  DEBUGF (TCP,"Socket %d, Sending Control\n",tcp_s->Id);
  EvrNetTCP_SendControl (tcp_s->Id);

  /* Allocate memory for TCP frame, add space for MSS option */
  frame = net_mem_alloc (TCP_DATA_OFFS + 4);
  frame->length = 0;

  /* Write parameters for tcp_write() to reserved memory */
  TCP_WI(frame)->sport = tcp_s->LocPort;
  TCP_WI(frame)->dport = tcp_s->Peer.port;
  TCP_WI(frame)->seqnr = tcp_s->SendNext;
  if (tcp_s->Flags & TCP_IFLAG_KSEG) {
    /* Send Keep-alive segment, ack the last byte sent */
    tcp_s->Flags &= ~TCP_IFLAG_KSEG;
    TCP_WI(frame)->seqnr--;
  }
  if (flags & (TCP_FLAG_SYN | TCP_FLAG_FIN)) {
    /* Update send next sequence */
    tcp_s->SendNext++;
  }
  TCP_WI(frame)->acknr = tcp_s->RecNext;
  if ((flags & TCP_FLAG_RST) && !(flags & TCP_FLAG_ACK)) {
    /* RST but no ACK, the ack sequence must be 0 */
    TCP_WI(frame)->acknr = 0;
  }
  TCP_WI(frame)->window = tcp_s->RecWin;
  TCP_WI(frame)->flags  = flags;
  if (tcp_s->Peer.addr_type == NET_ADDR_IP4) {
    TCP_WI(frame)->ver = IP_VER4;
    TCP_WI(frame)->opt = tcp_s->Tos;
  }
  else {
    TCP_WI(frame)->ver = IP_VER6;
    TCP_WI(frame)->opt = tcp_s->TClass;
  }
  /* Clear pending ack request */
  tcp_s->AckTimer = 0;
  tcp_write (tcp_s->net_if, frame, tcp_s->Peer.addr, tcp_s->LocAddr);
  net_mem_free (frame);
}

/**
  \brief       Send RST frame to remote host.
  \param[in]   net_if   network interface descriptor.
  \param[in]   frame    network frame.
  \param[in]   tcp_hdr  pointer to TCP header.
  \param[in]   ip_ver   IP version.
  \note        Control frame contains no data.
*/
static void tcp_send_reset (NET_IF_CFG *net_if, NET_FRAME *frame,
                            NET_TCP_HEADER *tcp_hdr, uint8_t ip_ver) {
  NET_FRAME *txfrm;

  /* Is this itself a reset packet? */
  if (tcp_hdr->Flags & TCP_FLAG_RST) {
    return;
  }

  DEBUGF (TCP,"Sending Reset\n");
  EvrNetTCP_SendReset ();

  /* Allocate memory for TCP reset frame */
  txfrm = net_mem_alloc (TCP_DATA_OFFS);
  txfrm->length = 0;

  /* Set parameters for tcp_write() */
  TCP_WI(txfrm)->sport = ntohs(tcp_hdr->DstPort);
  TCP_WI(txfrm)->dport = ntohs(tcp_hdr->SrcPort);
  TCP_WI(txfrm)->seqnr = get_u32(&tcp_hdr->AckNr);
  TCP_WI(txfrm)->acknr = get_u32(&tcp_hdr->SeqNr);
  /* Check if the frame's ACK or SYN flag is set */
  if (tcp_hdr->Flags & (TCP_FLAG_ACK | TCP_FLAG_SYN)) {
    /* Yes, send RST only, the ack sequence must be 0 */
    TCP_WI(txfrm)->acknr = 0;
    TCP_WI(txfrm)->flags = TCP_FLAG_RST;
  }
  else {
    /* No, Send RST+ACK */
    TCP_WI(txfrm)->flags = TCP_FLAG_RST | TCP_FLAG_ACK;
  }
  TCP_WI(txfrm)->window = 0;

  if (ip_ver == IP_VER4) {
    TCP_WI(txfrm)->ver = IP_VER4;
    TCP_WI(txfrm)->opt = TCP_TOS_NORMAL;
    tcp_write (net_if, txfrm, IP4_FRAME(frame)->SrcAddr, IP4_FRAME(frame)->DstAddr);
  }
#ifdef Network_IPv6
  else {
    TCP_WI(txfrm)->ver = IP_VER6;
    TCP_WI(txfrm)->opt = TCP_TCLASS_NORMAL;
    tcp_write (net_if, txfrm, IP6_FRAME(frame)->SrcAddr, IP6_FRAME(frame)->DstAddr);
  }
#endif
  net_mem_free (txfrm);
}

/**
  \brief       Parse TCP header options.
  \param[in]   tcp_s    socket descriptor.
  \param[in]   tcp_hdr  pointer to TCP header.
  \note        Check for MSS option and update MaxSegSize for socket.
*/
static void tcp_parse_opt (NET_TCP_INFO *tcp_s, NET_TCP_HEADER *tcp_hdr) {
  uint32_t i,optlen;
  uint16_t maxsize;
  const uint8_t *opt;

  optlen = ((tcp_hdr->HLen >> 2) & ~0x03) - TCP_HEADER_LEN;
  if (optlen == 0) {
    return;
  }
  DEBUGF (TCP,"Process Options, %d bytes\n",optlen);
  EvrNetTCP_ParseHeaderOptions (tcp_s->Id, optlen);
  /* Check for Max. Segment Size */
  opt = (uint8_t *)tcp_hdr + TCP_HEADER_LEN;
  for (i = 0; i < optlen;  ) {
    switch (opt[i]) {
      case 0:
        /* End of Options list */
        return;

      case 1:
        /* No Operation Option Kind */
        i++;
        break;

      case 2:
        /* Max. Segment Size Option */
        if (opt[i+1] == 4) {
          maxsize = (uint16_t)(opt[i+2] << 8 | opt[i+3]);
          if (maxsize > TCP_MTU) {
            /* Upper limit of MSS is 1440 bytes */
            maxsize = TCP_MTU;
          }
          if (maxsize < TCP_DEF_MSS) {
            /* Lower limit of MSS is 536 bytes */
            maxsize = TCP_DEF_MSS;
          }
          tcp_s->MaxSegSize = maxsize;
          DEBUGF (TCP," Opt-MSS: %d\n",tcp_s->MaxSegSize);
          EvrNetTCP_OptionMss (tcp_s->Id, tcp_s->MaxSegSize);
        }
        __FALLTHROUGH;

      default:
        /* Second byte is Size of this Option Kind element */
        if (opt[i+1] == 0) {
          /* The length field is zero, the options are malformed */
          return;
        }
        i += opt[i+1];
        break;
    }
  }
}

/**
  \brief       Initialize congestion control variables.
  \param[in]   tcp_s  socket descriptor.
*/
static void tcp_set_cwnd (NET_TCP_INFO *tcp_s) {
  if (tcp_s->SsThresh == 0) {
    /* Retransmitted SYN or SYN+ACK, slow start */
    /*   ssthresh = 2*MSS   (RFC 5681 - page 4) */
    /*   cwnd     = MSS                         */
    tcp_s->SsThresh = tcp_s->MaxSegSize * 2;
    tcp_s->CWnd     = tcp_s->MaxSegSize; 
  }
  else {
    /* Update initial congestion window size (RFC5681 - page 4) */
    if (tcp_s->MaxSegSize <= 1095) {
      tcp_s->CWnd   = tcp_s->MaxSegSize * 4;
    }
    else {
      tcp_s->CWnd   = tcp_s->MaxSegSize * 3;
    }
  }
  DEBUGF (TCP," cwnd=%d, ssth=%d\n",tcp_s->CWnd,tcp_s->SsThresh);
  EvrNetTCP_ShowCongestionVariables (tcp_s->Id, tcp_s->CWnd, tcp_s->SsThresh);
}

/**
  \brief       Process duplicate data acks.
  \param[in]   tcp_s  socket descriptor.
  \note        Processing algorithm:
               a) dupacks <  3: do nothing
               b) dupacks == 3: start fast retransmit
               c) dupacks > 3 : inflate congestion window
*/
static void tcp_proc_dupack (NET_TCP_INFO *tcp_s) {
  if (tcp_s->DupAcks != 3) {
    /* Print a different message for 3 dupacks */
    DEBUGF (TCP," DupAck %d\n",tcp_s->DupAcks);
    EvrNetTCP_DuplicateAck (tcp_s->Id, tcp_s->DupAcks);
  }
  if (tcp_s->DupAcks < 3) {
    /* Do nothing this time */
    return;
  }
  if (tcp_s->DupAcks == 3) {
    /* Start Fast retransmit (RFC 5681 - page 8) */
    uint16_t th = MIN (tcp_s->SendWin, tcp_s->CWnd) / 2;
    th          = MAX (th, tcp_s->MaxSegSize * 2);
    tcp_s->SsThresh = th;
    tcp_s->CWnd     = th + tcp_s->MaxSegSize * 3;

    /* Update the check sequence number SendChk to the flight size,  */
    /* that is number of bytes that were transmitted, but not yet    */
    /* acknowledged. An ack received for the packet with acknowledge */
    /* number less than, or equal to SendChk, will be treated as a   */
    /* duplicate ack. This allows faster recovery in fast retransmit */
    /* and gives better performance on networks with low packet loss.*/
    tcp_s->SendChk  = tcp_s->SendNext;
    DEBUGF (TCP," DupAck 3, Fast retransmit %d bytes\n",
                            tcp_s->SendNext-tcp_s->SendUna);
    EvrNetTCP_FastRetransmit (tcp_s->Id, tcp_s->SendNext-tcp_s->SendUna);
    /* Initialize the queue for fast retransmit */
    tcp_s->Flags   |= TCP_IFLAG_FAST;
    tcp_que_init (tcp_s);
  }
  else if (tcp_s->Flags & TCP_IFLAG_FAST) {
    /* Fast recovery in progress (RFC 5681 - page 8) */
    /* Artificially inflate the congestion window,   */
    /* but not if it means that the value overflows. */
    uint32_t val = tcp_s->CWnd + tcp_s->MaxSegSize;
    if (val > 65535) {
      val = 65535;
    }
    tcp_s->CWnd = val & 0xFFFF;
  }
  else {
    /* Fast recovery done, DupAcks > 3, no change of CWnd */
    return;
  }
  DEBUGF (TCP," cwnd=%d, ssth=%d\n",tcp_s->CWnd,tcp_s->SsThresh);
  EvrNetTCP_ShowCongestionVariables (tcp_s->Id, tcp_s->CWnd, tcp_s->SsThresh);
}

/**
  \brief       Process data ack number, check if it is valid.
  \param[in]   tcp_s  socket descriptor.
  \param[in]   acknr  acknowledge sequence number.
*/
static void tcp_proc_acknr (NET_TCP_INFO *tcp_s, uint32_t acknr) {
  NET_FRAME *frame,*next;
  uint32_t qack = tcp_s->SendUna;

  /* Scan the queue and check if ack is valid */
  for (frame = tcp_s->unack_list; frame; frame = TCP_QUE(frame)->next) {
    qack += TCP_QUE(frame)->dlen;
    if (acknr == qack) {
      break;
    }
  }
  if (frame == NULL) {
    /* The packet is not found, the socket is maybe closing */
    if ((acknr == (qack + 1)) && (tcp_s->Flags & TCP_IFLAG_CLOSING)) {
      /* This ack is for FIN + data, but we handle only data acks here */
      acknr = qack;
    }
    else {
      /* Invalid ack, ignore it */
      return;
    }
  }
  /* Valid ack number found */
  DEBUGF (TCP," Acked %d bytes\n",acknr-tcp_s->SendUna);
  EvrNetTCP_DataAcked (tcp_s->Id, acknr - tcp_s->SendUna);
  tcp_s->SendUna = acknr;
  if (SEQ_LT (tcp_s->SendChk, acknr)) {
    /* Update also DupAck checking trigger sequence */
    tcp_s->SendChk = acknr;
  }

  /* Release the acked data from the queue */
  next = tcp_s->unack_list;
  if (TCP_QUE(next)->ticks) {
    /* Timestamp exists, do RTT estimation calculations. This */
    /* code is taken directly from VJs original code in his   */
    /* paper (Congestion avoidance and control, page 20)      */
    int32_t m = (int32_t)(sys->Ticks - TCP_QUE(next)->ticks);
    /* Update RTT Average estimator */
    m -= (tcp_s->RttSa >> 3);
    tcp_s->RttSa += m;
    /* Update RTT Deviation estimator */
    if (m < 0) m = -m;
    m -= (tcp_s->RttSv >> 2);
    tcp_s->RttSv += m;
    DEBUG_INFO (tcp_s);
    EvrNetTCP_ShowRttVariables (tcp_s->Id, tcp_s->RttSa, tcp_s->RttSv);
  }
  tcp_s->unack_list = NULL;
  if (frame != NULL) {
    /* Markup the release edge (next = NULL) */
    tcp_s->unack_list = TCP_QUE(frame)->next;
    TCP_QUE(frame)->next= NULL;
  }
  for (frame = next; frame; frame = next) {
    next = TCP_QUE(frame)->next;
    net_mem_free (frame);
  }

  /* Update congestion control (RFC 5681 - page 5) */
  if (tcp_s->Flags & TCP_IFLAG_FAST) {
    /* Fast recovery active:  cwnd += MSS     */
    /* (updated in tcp_proc_dupack() function)*/
    if (SEQ_LT (acknr, tcp_s->SendChk)) {
      /* Wait to retransmit all packets from the queue */
      return;
    }
    /* Fast recovery done:  cwnd = ssthresh */
    tcp_s->Flags &= ~TCP_IFLAG_FAST;
    tcp_s->CWnd   = tcp_s->SsThresh;
  }
  else if (tcp_s->CWnd <= tcp_s->SsThresh) {
    /* Slow Start active:  cwnd += MSS */
    uint32_t val = tcp_s->CWnd;
    val += tcp_s->MaxSegSize;
    if (val > 65535) {
      /* Upper limit for 16-bit CWnd */
      val = 65535;
    }
    tcp_s->CWnd = val & 0xFFFF;
  }
  else {
    /* Congestion avoidance:  cwnd += MSS x MSS / cwnd */
    uint32_t val = tcp_s->CWnd;
    val += tcp_s->MaxSegSize * tcp_s->MaxSegSize / tcp_s->CWnd;
    if (val > 65535) {
      /* Upper limit for 16-bit CWnd */
      val = 65535;
    }
    tcp_s->CWnd = val & 0xFFFF;
  }
  DEBUGF (TCP," cwnd=%d, ssth=%d\n",tcp_s->CWnd,tcp_s->SsThresh);
  EvrNetTCP_ShowCongestionVariables (tcp_s->Id, tcp_s->CWnd, tcp_s->SsThresh);
}

/**
  \brief       Initialize the frame queue for retransmitting.
  \param[in]   tcp_s  socket descriptor.
*/
static void tcp_que_init (NET_TCP_INFO *tcp_s) {
  NET_FRAME *frame;

  /* Reset the queue to retransmit all frames */
  tcp_s->Flags |= TCP_IFLAG_RESEND;
  for (frame = tcp_s->unack_list; frame; frame = TCP_QUE(frame)->next) {
    TCP_QUE(frame)->ticks = 0;
    TCP_QUE(frame)->delta = 0;
  }
}

/**
  \brief       Get the size of retransmitted data not yet acked.
  \param[in]   tcp_s  socket descriptor.
  \return      number of bytes in flight.
*/
static uint32_t tcp_que_una (NET_TCP_INFO *tcp_s) {
  NET_FRAME *frame;
  uint32_t una = 0;

  /* Scan the queue and find the size of data in flight */
  for (frame = tcp_s->unack_list; frame; frame = TCP_QUE(frame)->next) {
    if (TCP_QUE(frame)->delta == 0) {
      break;
    }
    una += TCP_QUE(frame)->dlen;
  }
  return (una);
}

/**
  \brief       Retransmit frame from the queue.
  \param[in]   tcp_s  socket descriptor.
  \note        Search for the frame to resend (with delta == 0).
*/
static void tcp_que_resend (NET_TCP_INFO *tcp_s) {
  NET_FRAME *frame,*next;
  uint32_t sseq,dlen,n;

  sseq = tcp_s->SendUna;
  /* Scan the queue and find a packet to retransmit */
  for (frame = tcp_s->unack_list; frame; frame = TCP_QUE(frame)->next) {
    if (TCP_QUE(frame)->delta == 0) {
      /* Send sequence number found */
      break;
    }
    sseq += TCP_QUE(frame)->dlen;
  }
  if (frame == NULL) {
    /* The sequence not found in the queue, data retransmit must */
    /* be over. Now we need to check, if the socket is closing.  */
    /* For the closing socket, we need to send also FIN packet.  */
    if (tcp_s->Flags & TCP_IFLAG_CLOSING) {
      /* The socket really is closing, send FIN now */
      tcp_s->SendNext = sseq;
      tcp_send_ctrl (tcp_s, TCP_FLAG_ACK | TCP_FLAG_FIN);
    }
    tcp_s->Flags &= ~TCP_IFLAG_RESEND;
    return;
  }

  /* Exponential retransmit timer backoff */
  /* In retransmit mode, RTT is not updated. The last RTT value, which    */
  /* was measured is used as RTO. When a retry timer expires, all packets */
  /* in the unacked queue are going to be resent. A new retry timeout is  */
  /* extended to 2*RTT. If the retry timer expires again, all frames that */
  /* are still queued unacked, are rescheduled for retransmission again.  */
  /* The retry timeout is now extended to 4*RTT, and so on until the max  */
  /* number of retries are used up. */
  n = tcp->MaxRetry - tcp_s->Retries;
  if (n > 7) n = 7;
  tcp_s->RetryTimer = (uint16_t)(((tcp_s->RttSa >> 3) + tcp_s->RttSv) << n);
  tcp_s->AliveTimer = tcp_s->ConnTout;

  tcp_s->Flags |= TCP_IFLAG_PUSH;
  next = TCP_QUE(frame)->next;
  if ((next == NULL) && !(tcp_s->Flags & TCP_IFLAG_CLOSING)) {
    /* This is the last frame in the queue */
    tcp_s->Flags &= ~TCP_IFLAG_RESEND;
  }
  dlen = TCP_QUE(frame)->dlen;
  /* A hack to provide send sequence for tcp_write() */
  TCP_WI(frame)->seqnr  = sseq;
  DEBUGF (TCP,"Resend Socket %d, %d bytes\n",tcp_s->Id,dlen);
  DEBUGF (TCP," Retry tout %dms\n",tcp_s->RetryTimer*SYS_TICK_INTERVAL);
  EvrNetTCP_ResendData (tcp_s->Id, dlen, tcp_s->RetryTimer);
  /* Warning! TCP_QUE data is lost in tcp_send_data()! */
  tcp_send_data (tcp_s, frame, dlen | 0x80000000);
  /* Warning! The overlaid data needs to be preserved! */
  TCP_QUE(frame)->next  = next;
  TCP_QUE(frame)->dlen  = dlen & 0xFFFF;
  TCP_QUE(frame)->delta = dlen & 0xFFFF;
  /* RTT not measured for retransmitted frames */
  TCP_QUE(frame)->ticks = 0;
}

/**
  \brief       Add frame to outgoing buffer queue.
  \param[in]   tcp_s  socket descriptor.
  \param[in]   frame  network frame.
  \param[in]   dlen   TCP data length.
  \note        It is important to add the frame to outgoing queue after
               calling tcp_send_data() function. This is required because
               ethernet header and TCP_QUE() variables overlap. Ethernet
               header is generated in net_eth_send_frame() function, when
               the packet is transmitted.
*/
static void tcp_que_add (NET_TCP_INFO *tcp_s, NET_FRAME *frame, uint32_t dlen) {
  NET_FRAME *next;

  TCP_QUE(frame)->next  = NULL;
  TCP_QUE(frame)->dlen  = dlen & 0xFFFF;
  TCP_QUE(frame)->delta = 0;

  /* Current tick count for RTT estimation */
  TCP_QUE(frame)->ticks = sys->Ticks;
  if (tcp_s->Flags & TCP_IFLAG_RESEND) {
    TCP_QUE(frame)->ticks = 0;
  }
  if (tcp_s->unack_list == NULL) {
    /* First frame added to unacked queue */
    tcp_s->unack_list = frame;
  }
  else {
    /* Append the frame to queue tail */
    next = tcp_s->unack_list;
    /* Scan the list chain, find the tail */
    for ( ; TCP_QUE(next)->next; next = TCP_QUE(next)->next);
    TCP_QUE(next)->next = frame;
  }
}

/**
  \brief       Release all retransmission buffers.
  \param[in]   tcp_s  socket descriptor.
*/
static void tcp_que_free (NET_TCP_INFO *tcp_s) {
  NET_FRAME *frame, *next;

  frame = tcp_s->unack_list;
  tcp_s->unack_list = NULL;
  for ( ; frame; frame = next) {
    next = TCP_QUE(frame)->next;
    net_mem_free (frame);
  }
}

/**
  \brief       Map received TCP frame to TCP socket.
  \param[in]   net_if   network interface descriptor.
  \param[in]   frame    network frame.
  \param[in]   tcp_hdr  pointer to TCP header of a frame.
  \param[in]   ip_ver   IP version.
  \return      socket handle.
*/
static NET_TCP_INFO *tcp_map_socket (NET_IF_CFG *net_if, NET_FRAME *frame,
                                     NET_TCP_HEADER *tcp_hdr, uint8_t ip_ver) {
  NET_TCP_INFO *tcp_s;
  int32_t i;

#ifndef Network_IPv6
  /* Accept only IPv4 */
  if (ip_ver != IP_VER4) {
    return (NULL);
  }
#endif
  /* Check destination port */
  if (tcp_hdr->DstPort == 0) {
    /* Port 0 is reserved, never open */
    /* Test valid, no ntohs() needed  */
    return (NULL);
  }
  /* Check if there exists an already established connection */
  for (i = tcp->NumSocks, tcp_s = &tcp->Scb[0]; i > 0; tcp_s++, i--) {
    if ((tcp_s->State   >  netTCP_StateLISTEN)         &&
        (tcp_s->LocPort   == ntohs(tcp_hdr->DstPort))  &&
        (tcp_s->Peer.port == ntohs(tcp_hdr->SrcPort))) {
      if (ip_ver == IP_VER4) {
        if (net_addr4_comp (IP4_FRAME(frame)->SrcAddr, tcp_s->Peer.addr)) {
          /* Connection found, return socket handle */
          return (tcp_s);
        }
      }
#ifdef Network_IPv6
      else if (net_addr6_comp (IP6_FRAME(frame)->SrcAddr, tcp_s->Peer.addr)) {
        /* Connection found, return socket handle */
        return (tcp_s);
      }
#endif
    }
  }

  /* Allocate listening socket, if SYN frame received */
  if ((tcp_hdr->Flags & (TCP_FLAG_FIN | TCP_FLAG_RST | TCP_FLAG_ACK))) {
    /* Flag check, wrong TCP frame flags set */
    ERRORF (TCP,"Map_Socket, Wrong TCP flags set\n");
    EvrNetTCP_MapSocketWrongFlagsSet ();
    return (NULL);
  }
  if ((tcp_hdr->Flags & TCP_FLAG_SYN) == 0) {
    ERRORF (TCP,"Map_Socket, TCP flag SYN not set\n");
    EvrNetTCP_MapSocketSynNotSet ();
    return (NULL);
  }
  /* Scan the listening sockets */
  for (i = tcp->NumSocks, tcp_s = &tcp->Scb[0]; i > 0; tcp_s++, i--) {
    if ((tcp_s->State == netTCP_StateLISTEN)   &&
        (tcp_s->LocPort == ntohs(tcp_hdr->DstPort))) {
      /* A listening socket is found, bind it now */
      tcp_s->net_if    = net_if;
      tcp_s->LocAddr   = sys->RxIpAddr;
      tcp_s->Peer.port = ntohs(tcp_hdr->SrcPort);
      if (ip_ver == IP_VER4) {
        tcp_s->Peer.addr_type = NET_ADDR_IP4;
        net_addr4_copy (tcp_s->Peer.addr, IP4_FRAME(frame)->SrcAddr);
        return (tcp_s);
      }
#ifdef Network_IPv6
      tcp_s->Peer.addr_type = NET_ADDR_IP6;
      net_addr6_copy (tcp_s->Peer.addr, IP6_FRAME(frame)->SrcAddr);
      return (tcp_s);
#endif
    }
  }
  /* Failed, socket not found */
  ERRORF (TCP,"Map_Socket, No Listen Socket found\n");
  EvrNetTCP_MapSocketNoListenSocket ();
  return (NULL);
}

/**
  \brief       Change state of TCP socket to a new state.
  \param[in]   tcp_s  socket descriptor.
  \param[in]   state  new TCP state as defined with netTCP_State.
  \note        This function refreshes also Retransmit and
               Keep-alive timers.
*/
static void tcp_transit (NET_TCP_INFO *tcp_s, netTCP_State state) {
  tcp_s->State = state;
  /* In some states we don't want to wait for many retries */
  switch (tcp_s->State) {
    case netTCP_StateTIME_WAIT:
      tcp_s->RetryTimer = SYS_TICK_T200MS;
      tcp_s->Retries    = 0;
      break;

    case netTCP_StateSYN_SENT:
      /* When we are sending SYN it's propably that ARP or NBR   */
      /* cache is not valid. Do retransmit faster on first time. */
      tcp_s->RetryTimer = tcp->InitRetryTout;
      tcp_s->Retries    = tcp->ConnRetry;
      break;

    case netTCP_StateESTABLISHED:
      /* Keep Alive timer */
      tcp_s->AliveTimer = tcp_s->ConnTout;
      tcp_s->Flags     &= ~TCP_IFLAG_KALIVE; 
      tcp_s->AckTimer   = 0;
      goto rtim;

    case netTCP_StateLAST_ACK:
    case netTCP_StateFIN_WAIT_1:
    case netTCP_StateFIN_WAIT_2:
    case netTCP_StateCLOSING:
      /* Retransmit timer */
      tcp_s->RetryTimer = tcp->RetryTout;
      tcp_s->Retries    = 1;
      break;

    case netTCP_StateUNUSED:
    case netTCP_StateCLOSED:
    case netTCP_StateLISTEN:
    default:
      /* Safety release the retransmit queue frames */
      tcp_s->Flags      = 0;
      tcp_s->MaxSegSize = TCP_DEF_MSS;
      tcp_s->RecWin     = tcp->RecWinSize;
      tcp_que_free (tcp_s);
      return;

    case netTCP_StateSYN_RECEIVED:
      /* Retransmit timer */
rtim: tcp_s->RetryTimer = tcp->RetryTout;
      tcp_s->Retries    = tcp->MaxRetry;
      return;
  }
  tcp_s->AliveTimer = 0;
}

/**
  \brief       Update TCP socket timers.
*/
static void tcp_upd_timers (void) {
  NET_TCP_INFO *tcp_s;
  int32_t i;

  for (i = 0, tcp_s = &tcp->Scb[0]; i < tcp->NumSocks; tcp_s++, i++) {
    if (tcp_s->RetryTimer) {
      /* Counting rate is sys tick */
      tcp_s->RetryTimer--;
    }
    if ((sys->Flags & SYS_FLAG_SEC) && tcp_s->AliveTimer) {
      /* Counting rate is 1 second */
      tcp_s->AliveTimer--;
    }
  }
}

/**
  \brief       Check if TCP port is in use.
  \param[in]   port  TCP port to check.
  \return      check status:
               - true  = port in use,
               - false = port free.
*/
static bool tcp_port_in_use (uint16_t port) {
  NET_TCP_INFO *tcp_s;
  int32_t i;

  for (i = 0, tcp_s = &tcp->Scb[0]; i < tcp->NumSocks; tcp_s++, i++) {
    if ((port == tcp_s->LocPort) && tcp_s->State > netTCP_StateCLOSED) {
      /* This port is in use */
      return (true);
    }
  }
  return (false);
}

/**
  \brief       Convert TCP socket state into a string.
  \param[in]   state  TCP socket state.
  \return      pointer to the text representation of a socket state.
*/
const char *net_tcp_ntoa (netTCP_State state) {
  static const char *const sa[11] = {
    "UNUSED",
    "CLOSED",
    "LISTEN",
    "SYN_REC",
    "SYN_SENT",
    "FIN_WAIT_1",
    "FIN_WAIT_2",
    "CLOSING",
    "LAST_ACK",
    "TIME_WAIT",
    "ESTABLISHED"
  };
  return (sa[state]);
}

#ifdef Network_Debug_STDIO
/**
  \brief       Debug print Round Trip Timeout information.
  \param[in]   tcp_s  socket descriptor.
*/
static void debug_info (const NET_TCP_INFO *tcp_s) {
  int32_t rto;

  rto  = (tcp_s->RttSa >> 3) + tcp_s->RttSv;
  rto *= SYS_TICK_INTERVAL;
  DEBUGF (TCP," RTO=%dms (sa=%d, sv=%d)\n",rto,tcp_s->RttSa,tcp_s->RttSv);
}

/**
  \brief       Debug print TCP header information.
  \param[in]   tcp_hdr  TCP frame header.
  \param[in]   len      frame payload length.
*/
static void debug_inf2 (const NET_TCP_HEADER *tcp_hdr, int32_t len) {
  static const char flg[][4] = {
    "FIN",
    "SYN",
    "RST",
    "PSH",
    "ACK",
    "URG"
  };
  char buf[24];
  int32_t i,msk,n;

  /* Print user friendly TCP flags */
  for (i = n = 0, msk = 0x01; msk < 0x40; i++, msk <<= 1) {
    if (tcp_hdr->Flags & msk) {
      /* Append TCP flag string */
      if (n) buf[n++] = ',';
      buf[n++] = flg[i][0];
      buf[n++] = flg[i][1];
      buf[n++] = flg[i][2];
    }
  }
  /* Add null-termination */
  buf[n] = 0;

  DEBUGF (TCP," Ports  : Src=%d, Dst=%d\n",        get_u16(&tcp_hdr->SrcPort),
                                                   get_u16(&tcp_hdr->DstPort));
  DEBUGF (TCP," Segment: Seq=0x%X, Ack=0x%X\n",    get_u32(&tcp_hdr->SeqNr),
                                                   get_u32(&tcp_hdr->AckNr));
  DEBUGF (TCP," Control: [%s] Win=%d, Cks=0x%04X\n",buf,
                                                   get_u16(&tcp_hdr->Window),
                                                   get_u16(&tcp_hdr->Chksum));
  if (len == 0) return;
  DEBUGF (TCP," Length %d bytes\n",len);
}
#endif
