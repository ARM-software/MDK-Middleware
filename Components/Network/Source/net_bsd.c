/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_bsd.c
 * Purpose: Berkeley Socket API
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_lib.h"
#include "net_sys.h"
#include "net_mem.h"
#include "net_common.h"
#include "net_addr.h"
#ifdef Network_IPv6
#include "net_ip6.h"
#else
#include "net_ip4.h"
#endif
#include "net_udp.h"
#include "net_tcp.h"
#include "net_bsd.h"
#include "net_dbg.h"

/* Global constants */
#ifdef Network_IPv6
const IN6_ADDR in6addr_any      = { .s6_b = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }};
const IN6_ADDR in6addr_loopback = { .s6_b = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }};
#endif

/* Local variables */

/* Local Functions */
static uint32_t bsd_cb_tcp (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                            const uint8_t *buf, uint32_t len);
static uint32_t bsd_cb_udp (int32_t socket, const NET_ADDR *addr,
                                            const uint8_t *buf, uint32_t len);
static NET_BSD_INFO *bsd_map_socket (int32_t socket, int32_t type);
static NET_BSD_INFO *list_get_first (NET_BSD_INFO *srv_s);
static void list_add_tail (NET_BSD_INFO *bsd_s);
static void que_add_buf (NET_BSD_INFO *bsd_s, NET_BUFFER *netbuf);
static void que_free_list (NET_BSD_INFO *bsd_s);
static void bsd_kill_socket (NET_BSD_INFO *bsd_s);
static void bsd_open_udp (NET_BSD_INFO *bsd_s);
static int32_t bsd_select_scan (int32_t n_max, fd_set *const rd_fds, fd_set *const wr_fds,
                                  fd_set *const err_fds, fd_set *out_fds);
static int32_t bsd_evt_subscribe (NETOS_ID thread, int32_t n_max, fd_set *const rd_fds,
                                  fd_set *const wr_fds, fd_set *const err_fds);
static void bsd_evt_unsubscribe (NETOS_ID thread, int32_t nfds);
static bool addr_net_to_bsd (const __ADDR *net_addr,
                                  uint8_t family, SOCKADDR *addr, int32_t *addrlen);
static bool is_addr_valid (uint8_t family, const SOCKADDR *addr, int32_t addrlen);
static bool is_addr_unspec (const SOCKADDR *addr);
static bool is_port_inuse (NET_BSD_INFO *bsd_s, uint16_t port);
static void addr_bsd_to_net (const SOCKADDR *addr, __ADDR *net_addr);
static uint16_t addr_get_port (const SOCKADDR *addr);
static void set_sock_type (NET_BSD_INFO *bsd_s);
static uint8_t bsd_evt_wait (NET_BSD_INFO *bsd_s, uint8_t evt, uint16_t tout);
static void bsd_evt_send (NET_BSD_INFO *bsd_s, uint8_t evt);
#ifdef Network_Debug_STDIO
 static const char *evt_ascii (netTCP_Event evt);
 static void debug_info (const __ADDR *addr);
#endif

/**
  \brief       Initialize BSD socket API.
*/
void net_bsd_socket_init (void) {
  DEBUGF (BSD,"Init %d Sockets\n",bsd->NumSocks);
  EvrNetBSD_InitSockets (bsd->NumSocks);

  /* Clear SCB for all sockets */
  memset (bsd->Scb, 0, sizeof (*bsd->Scb) * bsd->NumSocks);
}

/**
  \brief       De-initialize BSD socket API.
*/
void net_bsd_socket_uninit (void) {
  NET_BSD_INFO *bsd_s;
  int32_t i;

  DEBUGF (BSD,"Uninit Sockets\n");
  EvrNetBSD_UninitSockets ();

  for (i = 0, bsd_s = &bsd->Scb[0]; i < bsd->NumSocks; bsd_s++, i++) {
    /* Resume blocked threads, if any */
    if ((bsd_s->Thread) || (bsd_s->xThread)) {
      /* Thread : accept(), connect(), recv/from() */
      /* xThread: send/to()                        */
      bsd_evt_send (bsd_s, BSD_EVT_KILL);
    }
    /* Clear socket control block */
    memset (bsd_s, 0, sizeof (*bsd_s));
    /* Return events were cleared too */
    bsd_s->Event  = BSD_EVT_KILL;
    bsd_s->xEvent = BSD_EVT_KILL;
  }
}

/**
  \brief       Create a communication socket.
  \param[in]   family    address family.
  \param[in]   type      connection type of a socket.
  \param[in]   protocol  protocol type or 0= system autoselect.
  \return      socket descriptor or status information on error.
*/
int32_t socket (int32_t family, int32_t type, int32_t protocol) {
  NET_BSD_INFO *bsd_s;
  int32_t sock,i;

  START_LOCK(int32_t);

  /* Check address family */
  switch (family) {
    case AF_INET:
#ifdef Network_IPv6
    case AF_INET6:
#endif
      break;
    default:
      goto inv_arg;
  }
  /* Check type and protocol */
  switch (type) {
    case SOCK_DGRAM:
      /* Datagram connection (UDP) */
      if (protocol && (protocol != IPPROTO_UDP)) {
        goto inv_arg;
      }
      break;

    case SOCK_STREAM:
      /* Stream connection (TCP) */
      if (protocol && (protocol != IPPROTO_TCP)) {
        goto inv_arg;
      }
      break;

    default:
inv_arg:/* Invalid argument provided */
      ERRORF (BSD,"Socket, Invalid parameter\n");
      EvrNetBSD_SocketInvalidParameter ();
      RETURN (BSD_EINVAL);
  }

  for (i = 1, bsd_s = &bsd->Scb[0]; i <= bsd->NumSocks; bsd_s++, i++) {
    if (bsd_s->State == BSD_STATE_FREE) {
      DEBUGF (BSD,"Socket %d create\n",i);
      DEBUGF (BSD," Type SOCK_%s\n",(type == SOCK_STREAM) ? "STREAM" : "DGRAM");
      EvrNetBSD_SocketCreate (i, type);
      sock = 0;
      if (type == SOCK_DGRAM) {
        sock = net_udp_get_socket (bsd_cb_udp);
      }
      if (type == SOCK_STREAM) {
        sock = net_tcp_get_socket (bsd_cb_tcp);
      }
      if (sock < 0) sock = 0;
      bsd_s->Socket = sock & 0xFF;
      if (bsd_s->Socket == 0) {
        /* Failed to allocate TCP/UDP socket */
        break;
      }
      bsd_s->Family   = family & 0xFF;
      bsd_s->Type     = type & 0xFF;
      bsd_s->Flags    = (family == AF_INET6)  ? BSD_FLAG_IP6ONLY  : 0;
      bsd_s->Tos      = (type == SOCK_STREAM) ? TCP_TOS_NORMAL    : UDP_TOS_NORMAL;
      bsd_s->TClass   = (type == SOCK_STREAM) ? TCP_TCLASS_NORMAL : UDP_TCLASS_NORMAL;
      bsd_s->RecvTout = bsd->RecvTout;
      bsd_s->SendTout = BSD_SEND_TOUT;
      bsd_s->State    = BSD_STATE_CREATED;
      bsd_s->Id       = i & 0xFF;
      RETURN (i);
    }
  }
  ERRORF (BSD,"Socket, Out of sockets\n");
  EvrNetBSD_NoSocketAvailable (bsd->NumSocks);
  RETURN (BSD_ENOMEM);

  END_LOCK;
}

/**
  \brief       Assign a local address and port to a socket.
  \param[in]   sock     socket descriptor.
  \param[in]   addr     structure containing local IP address and port.
  \param[in]   addrlen  length of SOCKADDR structure.
  \return      status information.
*/
int32_t bind (int32_t sock, const SOCKADDR *addr, int32_t addrlen) {
  NET_BSD_INFO *bsd_s;
  uint16_t port;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Bind Socket %d\n",sock);
  EvrNetBSD_BindSocket (sock);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Bind, Socket %d not valid\n",sock);
    EvrNetBSD_BindSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  bsd_s = &bsd->Scb[sock-1];
  if (bsd_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Bind, Socket %d not created\n",sock);
    EvrNetBSD_BindSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }

  if (!is_addr_valid (bsd_s->Family, addr, addrlen)) {
    /* Bind address parameter error */
#ifdef Network_IPv6
    goto inv_arg;
  }
  if ((bsd_s->Flags & BSD_FLAG_IP6ONLY) &&
       net_addr6_is_4to6 ((SOCKADDR6(addr)->sin6_addr.s6_b))) {
    /* IPv4 bind address error */
inv_arg:
#endif
    ERRORF (BSD,"Bind, Socket %d invalid parameter\n",sock);
    EvrNetBSD_BindInvalidParameter (sock);
    RETURN (BSD_EINVAL);
  }

  if ((bsd_s->State == BSD_STATE_CONNECTED) || (bsd_s->Flags & BSD_FLAG_FILTER)) {
    ERRORF (BSD,"Bind, Socket %d connected\n",sock);
    EvrNetBSD_BindSocketConnected (sock);
    RETURN (BSD_EISCONN);
  }

  if (bsd_s->State >= BSD_STATE_BOUND) {
    ERRORF (BSD,"Bind, Socket %d already bound\n",sock);
    EvrNetBSD_BindSocketAlreadyBound (sock);
    RETURN (BSD_EINVAL);
  }

  port = addr_get_port (addr);
  if (port && is_port_inuse (bsd_s, port)) {
    ERRORF (BSD,"Bind, Socket %d port in use\n",sock);
    EvrNetBSD_BindPortInUse (sock);
    RETURN (BSD_EADDRINUSE);
  }

  addr_bsd_to_net (addr, &bsd_s->LocHost);
  bsd_s->State = BSD_STATE_BOUND;

  if (bsd_s->Type == SOCK_DGRAM) {
    /* Datagram Socket (UDP), also handles port 0 */
    bsd_open_udp (bsd_s);
  }
  else {
    if (bsd_s->LocHost.port == 0) {
      /* Allocate dynamic ephemeral TCP port */
      bsd_s->LocHost.port = net_tcp_alloc_port ();
    }
  }
  DEBUG_INFO (&bsd_s->LocHost);
  EvrNetBSD_ViewNetAddress (&bsd_s->LocHost);
  RETURN (0);

  END_LOCK;
}

/**
  \brief       Set a socket in a listening mode.
  \param[in]   sock     socket descriptor.
  \param[in]   backlog  number of connection requests that can be queued.
  \return      status information.
*/
int32_t listen (int32_t sock, int32_t backlog) {
  NET_BSD_INFO *srv_s,*bsd_s;
  int32_t i,sck;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Listen Socket %d, backlog %d\n",sock,backlog);
  EvrNetBSD_ListenSocket (sock, backlog);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Listen, Socket %d not valid\n",sock);
    EvrNetBSD_ListenSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  if (backlog <= 0 || backlog > tcp->NumSocks) {
    ERRORF (BSD,"Listen, Socket %d invalid parameter\n",sock);
    EvrNetBSD_ListenInvalidParameter (sock);
    RETURN (BSD_EINVAL);
  }

  srv_s = &bsd->Scb[sock-1];
  if (srv_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Listen, Socket %d not created\n",sock);
    EvrNetBSD_ListenSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }

  if (srv_s->Type != SOCK_STREAM) {
    ERRORF (BSD,"Listen, Socket %d not STREAM type\n",sock);
    EvrNetBSD_ListenSocketNotStreamType (sock);
    RETURN (BSD_ENOTSUP);
  }

  if (srv_s->State == BSD_STATE_SERVER) {
    ERRORF (BSD,"Listen, Socket %d already listens\n",sock);
    EvrNetBSD_ListenSockAlreadyListens (sock);
    RETURN (BSD_EINVAL);
  }

  if (srv_s->State != BSD_STATE_BOUND) {
    ERRORF (BSD,"Listen, Socket %d not bound\n",sock);
    EvrNetBSD_ListenSocketNotBound (sock);
    RETURN (BSD_EINVAL);
  }

  /* Server socket does not need assigned TCP socket */
  net_tcp_release_socket (srv_s->Socket);
  srv_s->State  = BSD_STATE_SERVER;
  srv_s->Socket = 0;

  /* Create backlog, assign native TCP sockets */
  for (i = 1, bsd_s = &bsd->Scb[0]; i <= bsd->NumSocks; bsd_s++, i++) {
    if (bsd_s->State == BSD_STATE_FREE) {
      bsd_s->Family   = srv_s->Family;
      bsd_s->Type     = srv_s->Type;
      bsd_s->Tos      = srv_s->Tos;
      bsd_s->TClass   = srv_s->TClass;
      bsd_s->RecvTout = srv_s->RecvTout;
      bsd_s->SendTout = srv_s->SendTout;
      bsd_s->State    = BSD_STATE_LISTEN;
      bsd_s->Id       = i & 0xFF;
      net_addr_copy (&bsd_s->LocHost, &srv_s->LocHost);
      bsd_s->ParentSock = srv_s->Id;
      sck = net_tcp_get_socket (bsd_cb_tcp);
      if (sck < 0) sck = 0;
      bsd_s->Socket   = sck & 0xFF;
      /* If listen fails here, will be restarted in net_bsd_socket_run() */
      net_tcp_listen (bsd_s->Socket, bsd_s->LocHost.port);
      if (--backlog == 0) {
        RETURN (0);
      }
    }
  }
  ERRORF (BSD,"Listen, Socket %d backlog not created\n",srv_s->Id);
  EvrNetBSD_ListenBacklogCreateFailed (srv_s->Id);
  RETURN (BSD_ERROR);

  END_LOCK;
}

/**
  \brief       Accept connect request for a listening socket.
  \param[in]   sock     socket descriptor.
  \param[out]  addr     structure that will receive IP address and port.
  \param[in]   addrlen  length of SOCKADDR structure.
  \return      status information.
*/
int32_t accept (int32_t sock, SOCKADDR *addr, int32_t *addrlen) {
  NET_BSD_INFO *srv_s,*bsd_s;
  int32_t evt;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Accept Socket %d\n",sock);
  EvrNetBSD_AcceptSocket (sock);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Accept, Socket %d not valid\n",sock);
    EvrNetBSD_AcceptSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  srv_s = &bsd->Scb[sock-1];
  if (srv_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Accept, Socket %d not created\n",sock);
    EvrNetBSD_AcceptSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }

  if (srv_s->Type != SOCK_STREAM) {
    ERRORF (BSD,"Accept, Socket %d not STREAM type\n",sock);
    EvrNetBSD_AcceptSocketNotStreamType (sock);
    RETURN (BSD_ENOTSUP);
  }

  if (srv_s->State != BSD_STATE_SERVER) {
    ERRORF (BSD,"Accept, Socket %d not listening\n",sock);
    EvrNetBSD_AcceptSocketNotListening (sock);
    RETURN (BSD_EINVAL);
  }

  if (srv_s->Thread) {
    /* Locked, other thread is the owner */
    ERRORF (BSD,"Accept, Socket %d locked\n",sock);
    EvrNetBSD_AcceptSocketLocked (sock);
    RETURN (BSD_ELOCKED);
  }

  bsd_s = list_get_first (srv_s);
  if (bsd_s != NULL) {
    /* Socket connect request is pending */
    goto ret;
  }

  if (srv_s->Flags & BSD_FLAG_NONBLOCK) {
    /* Return error for non-blocking socket */
    RETURN (BSD_EWOULDBLOCK);
  }

  /* Wait for connect or close event */
  evt = bsd_evt_wait (srv_s, BSD_EVT_CONNECT | BSD_EVT_CLOSE, 0);
  if (evt & BSD_EVT_CONNECT) {
    /* A socket is connected */
    bsd_s = list_get_first (srv_s);
    /* Clone child socket settings from the parent socket */
ret:bsd_s->Flags    = (bsd_s->Flags & ~BSD_FLAGS_CHILD) |
                      (srv_s->Flags & BSD_FLAGS_CHILD);
    bsd_s->Tos      = srv_s->Tos;
    bsd_s->TClass   = srv_s->TClass;
    bsd_s->RecvTout = srv_s->RecvTout;
    bsd_s->SendTout = srv_s->SendTout;
    set_sock_type (bsd_s);
    addr_net_to_bsd (&bsd_s->RemHost, bsd_s->Family, addr, addrlen);
    DEBUGF (BSD," Child Socket %d connected\n",bsd_s->Id);
    EvrNetBSD_AcceptChildSockConnected (bsd_s->Id);
    RETURN (bsd_s->Id);
  }
  if (evt & BSD_EVT_CLOSE) {
    /* A socket has been closed */
    ERRORF (BSD,"Accept, Socket %d closed\n",sock);
    EvrNetBSD_AcceptSocketClosed (sock);
    RETURN (BSD_ECONNRESET);
  }
  if (evt & BSD_EVT_KILL) {
    /* A socket has been closed locally */
    ERRORF (BSD,"Accept, Socket %d killed\n",sock);
    EvrNetBSD_AcceptSocketKilled (sock);
    RETURN (BSD_ECONNABORTED);
  }
  /* Should never come here */
  RETURN (BSD_ERROR);

  END_LOCK;
}

/**
  \brief       Connect a socket to a remote host.
  \param[in]   sock     socket descriptor.
  \param[in]   addr     structure containing remote IP address and port.
  \param[in]   addrlen  length of SOCKADDR structure.
  \return      status information.
*/
int32_t connect (int32_t sock, const SOCKADDR *addr, int32_t addrlen) {
  NET_BSD_INFO *bsd_s;
  int32_t evt;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Connect Socket %d\n",sock);
  EvrNetBSD_ConnectSocket (sock);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Connect, Socket %d not valid\n",sock);
    EvrNetBSD_ConnectSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  bsd_s = &bsd->Scb[sock-1];
  if (bsd_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Connect, Socket %d not created\n",sock);
    EvrNetBSD_ConnectSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }

  if (!is_addr_valid (bsd_s->Family, addr, addrlen)) {
    /* Destination address parameter error */
inv_arg:
    ERRORF (BSD,"Connect, Socket %d invalid parameter\n",sock);
    EvrNetBSD_ConnectInvalidParameter (sock);
    RETURN (BSD_EINVAL);
  }

#ifdef Network_IPv6
  if ((bsd_s->Flags & BSD_FLAG_IP6ONLY) &&
       net_addr6_is_4to6 ((SOCKADDR6(addr)->sin6_addr.s6_b))) {
    /* IPv4 destination address error */
    goto inv_arg;
  }
#endif

  if ((bsd_s->Type == SOCK_STREAM) && is_addr_unspec (addr)) {
    goto inv_arg;
  }

  if (bsd_s->Thread) {
    /* Locked, other thread is the owner */
    ERRORF (BSD,"Connect, Socket %d locked\n",sock);
    EvrNetBSD_ConnectSocketLocked (sock);
    RETURN (BSD_ELOCKED);
  }

  switch (bsd_s->State) {
    case BSD_STATE_CREATED:
    case BSD_STATE_BOUND:
       break;

    /* States valid for Stream socket */
    case BSD_STATE_CONNREQ:
      /* Connecting, non-blocking call */
      RETURN (BSD_EALREADY);

    case BSD_STATE_CONNECTED:
      /* Already connected, non-blocking call */
      RETURN (BSD_EISCONN);

    case BSD_STATE_CLOSED:
      /* Closed after 'connect()', non-blocking call */
      goto closed;

    default:
      /* Not possible to start connect(), wrong socket state */
      ERRORF (BSD,"Connect, Socket %d wrong state\n",sock);
      EvrNetBSD_ConnectSocketWrongState (sock);
      RETURN (BSD_EINVAL);
  }

  addr_bsd_to_net (addr, &bsd_s->RemHost);

  if (bsd_s->Type == SOCK_DGRAM) {
    /* Datagram Socket (UDP) */
    bsd_s->Flags &= ~BSD_FLAG_FILTER;
    if (!net_addr_is_unspec(&bsd_s->RemHost)) {
      /* Datagram sockets may dissolve the association */
      /* by connecting to unspecified address.         */
      bsd_s->Flags |= BSD_FLAG_FILTER;
    }
    /* Implicit binding */
    bsd_s->State = BSD_STATE_BOUND;
    bsd_open_udp (bsd_s);
    DEBUGF (BSD," Dgram socket %d connected\n",sock);
    EvrNetBSD_ConnectDatagramSuccess (sock);
    RETURN (0);
  }

  /* Stream Socket (TCP) */
  if (bsd_s->LocHost.port == 0) {
    /* Allocate dynamic ephemeral TCP port */
    bsd_s->LocHost.port = net_tcp_alloc_port ();
  }
  if (net_tcp_connect (bsd_s->Socket, &bsd_s->RemHost,
                       bsd_s->LocHost.port) != netOK) {
    RETURN (BSD_ERROR);
  }
  bsd_s->State = BSD_STATE_CONNREQ;

  if (bsd_s->Flags & BSD_FLAG_NONBLOCK) {
    /* Return Error for Non Blocking socket */
    RETURN (BSD_EINPROGRESS);
  }

  /* Wait for connect or close event */
  evt = bsd_evt_wait (bsd_s, BSD_EVT_CONNECT | BSD_EVT_CLOSE, 0);
  if (evt & BSD_EVT_CONNECT) {
    /* A socket is connected */
    DEBUGF (BSD," Stream socket %d connected\n",sock);
    EvrNetBSD_ConnectStreamSuccess (sock);
    RETURN (0);
  }
  if (evt & BSD_EVT_CLOSE) {
closed:/* A socket has been closed */
    if (bsd_s->Flags & BSD_FLAG_TIMEOUT) {
      /* Timeout, target not responding */
      ERRORF (BSD,"Connect, Socket %d timeout\n",sock);
      EvrNetBSD_ConnectStreamTimeout (sock);
      RETURN (BSD_ETIMEDOUT);
    }
    /* Target refused connection */
    ERRORF (BSD,"Connect, Socket %d refused\n",sock);
    EvrNetBSD_ConnectStreamRefused (sock);
    RETURN (BSD_ECONNREFUSED);
  }
  if (evt & BSD_EVT_KILL) {
    /* A socket has been closed locally */
    ERRORF (BSD,"Connect, Socket %d killed\n",sock);
    EvrNetBSD_ConnectSocketKilled (sock);
    RETURN (BSD_ECONNABORTED);
  }
  /* Should never come here */
  RETURN (BSD_ERROR);

  END_LOCK;
}

/**
  \brief       Send data on already connected socket.
  \param[in]   sock   socket descriptor.
  \param[in]   buf    pointer to application data buffer to transmit.
  \param[in]   len    length of data (in bytes).
  \param[in]   flags  message flags.
  \return      status information.
*/
int32_t send (int32_t sock, const char *buf, int32_t len, int32_t flags) {
  return (sendto (sock, buf, len, flags, NULL, 0));
}

/**
  \brief       Send data to endpoint node.
  \param[in]   sock   socket descriptor.
  \param[in]   buf    pointer to application data buffer to transmit.
  \param[in]   len    length of data (in bytes).
  \param[in]   flags  message flags.
  \param[in]   to     structure containing remote IP address and port.
  \param[in]   tolen  length of SOCKADDR structure.
  \return      status information.
*/
int32_t sendto (int32_t sock, const char *buf, int32_t len,
                              int32_t flags, const SOCKADDR *to, int32_t tolen) {
  NET_BSD_INFO *bsd_s;
  uint8_t *sendbuf;
  uint32_t evt,nw,dlen;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Send/to Socket %d\n",sock);
  EvrNetBSD_SendSocket (sock, (uint32_t)len);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Send, Socket %d not valid\n",sock);
    EvrNetBSD_SendSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  if (buf == NULL || len <= 0) {
    ERRORF (BSD,"Send, Socket %d invalid parameter\n",sock);
    EvrNetBSD_SendInvalidParameter (sock);
    RETURN (BSD_EINVAL);
  }

  bsd_s = &bsd->Scb[sock-1];
  if (bsd_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Send, Socket %d not created\n",sock);
    EvrNetBSD_SendSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }

  if (bsd_s->xThread) {
    /* Locked, other thread is sending */
    ERRORF (BSD,"Send, Socket %d locked\n",sock);
    EvrNetBSD_SendSocketLocked (sock);
    RETURN (BSD_ELOCKED);
  }

  if (bsd_s->Type == SOCK_STREAM) {
    /* Stream Socket (TCP) */
    switch (bsd_s->State) {
      case BSD_STATE_CONNECTED:
        break;

      case BSD_STATE_BOUND:
      case BSD_STATE_LISTEN:
      case BSD_STATE_CONNREQ:
      case BSD_STATE_CLOSING:
        ERRORF (BSD,"Send, Socket %d not connected\n",sock);
        EvrNetBSD_SendSocketNotConnected (sock);
        RETURN (BSD_ENOTCONN);

      case BSD_STATE_CLOSED:
        /* Closed by remote peer, non-blocking call */
        goto closed;

      default:
        /* Not possible to send, wrong socket state */
        ERRORF (BSD,"Send, Socket %d wrong state\n",sock);
        EvrNetBSD_SendSocketWrongState (sock);
        RETURN (BSD_ENOTCONN);
    }

    /* Parameter 'to' and 'tolen' are ignored here */
    for (nw = dlen = 0; nw < (uint32_t)len; nw += dlen) {
      if (net_tcp_check_send (bsd_s->Socket) == false) {
        if ((flags & MSG_DONTWAIT) || (bsd_s->Flags & BSD_FLAG_NONBLOCK)) {
          /* Return Code for non-blocking call is:               */
          /* - BSD_EWOULDBLOCK  (no data sent, call would block) */
          /* - num.bytes sent   (some data sent, but not all)    */
          if (nw == 0) {
            /* No bytes sent yet */
            RETURN (BSD_EWOULDBLOCK);
          }
          DEBUGF (BSD," Sent %d out of %d bytes\n",nw,len);
          EvrNetBSD_SendCompleteNonBlocking (sock, nw, (uint32_t)len);
          /* Return number of bytes sent */
          RETURN ((int32_t)nw);
        }

        /* Wait for Data Ack event */
        evt = bsd_evt_wait (bsd_s, BSD_EVT_SEND | BSD_EVT_ACK | BSD_EVT_CLOSE,
                                   bsd_s->SendTout);
        if (evt & BSD_EVT_CLOSE) {
closed:   /* A socket has been closed */
          ERRORF (BSD,"Send, Socket %d closed\n",sock);
          EvrNetBSD_SendSocketClosed (sock);
          RETURN (BSD_ECONNRESET);
        }
        if (evt & BSD_EVT_TIMEOUT) {
          /* Return Code for blocking timeout is:                */
          /* - BSD_EWOULDBLOCK  (no data sent, call would block) */
          /* - num.bytes sent   (some data sent, but not all)    */
          DEBUGF (BSD," Sent %d out of %d bytes\n",nw,len);
          EvrNetBSD_SendTimeoutBlocking (sock, nw, (uint32_t)len);
          if (nw != 0) {
            RETURN ((int32_t)nw);
          }
          /* No bytes sent yet */
          RETURN (BSD_ETIMEDOUT);
        }
        if (evt & BSD_EVT_KILL) {
          /* A socket has been closed locally */
          ERRORF (BSD,"Send, Socket %d killed\n",sock);
          EvrNetBSD_SendSocketKilled (sock);
          RETURN (BSD_ECONNABORTED);
        }
        if (!(evt & BSD_EVT_ACK)) {
          /* Should never come here */
          RETURN (BSD_ERROR);
        }
      }   /* net_tcp_check_send */

      /* Ready, send TCP data */
      sendbuf = NULL;
      if (net_mem_avail_tx()) {
        dlen = net_tcp_get_mss (bsd_s->Socket);
        if (dlen > ((uint32_t)len - nw)) {
          dlen = (uint32_t)len - nw;
        }
        sendbuf = net_tcp_get_buf (dlen | 0x80000000);
      }
      if (sendbuf == NULL) {
        DEBUGF (BSD," Sent %d out of %d bytes\n",nw,len);
        EvrNetBSD_SendNoMemory (sock, nw, (uint32_t)len);
        if (nw != 0) {
          RETURN ((int32_t)nw);
        }
        /* No bytes sent yet */
        RETURN (BSD_ENOMEM);
      }
      memcpy (sendbuf, (const uint8_t *)&buf[nw], dlen);
      if (net_tcp_send (bsd_s->Socket, sendbuf, dlen) != netOK) {
        ERRORF (BSD,"Send, Socket %d failed\n",sock);
        EvrNetBSD_SendStreamError (sock);
        RETURN (BSD_ERROR);
      }
      /* Wake up the Network core from sleep */
      net_sys_wakeup ();
    }
    if ((flags & MSG_DONTWAIT) || (bsd_s->Flags & BSD_FLAG_NONBLOCK)) {
      DEBUGF (BSD," Sent %d out of %d bytes\n",nw,len);
      EvrNetBSD_SendCompleteNonBlocking (sock, nw, (uint32_t)len);
      /* Return number of bytes sent */
      RETURN ((int32_t)nw);
    }
  }
  else  {
    /* Datagram Socket (UDP) */
    if (to && (!is_addr_valid (bsd_s->Family, to, tolen) || is_addr_unspec (to))) {
      /* 'sendto()' address parameter error */
#ifdef Network_IPv6
      goto inv_arg;
    }
    if ((bsd_s->Flags & BSD_FLAG_IP6ONLY) &&
        net_addr6_is_4to6 ((SOCKADDR6(to)->sin6_addr.s6_b))) {
      /* IPv4 destination address error */
inv_arg:
#endif
      ERRORF (BSD,"Sendto, Socket %d invalid parameter\n",sock);
      EvrNetBSD_SendtoInvalidParameter (sock);
      RETURN (BSD_EINVAL);
    }
    if (!to && !(bsd_s->Flags & BSD_FLAG_FILTER)) {
      /* 'send()' destination address undefined */
      /* 'connect()' not called previously      */
      ERRORF (BSD,"Send, Socket %d dst-addr unknown\n",sock);
      EvrNetBSD_SendDestAddressUndefined (sock);
      RETURN (BSD_EDESTADDRREQ);
    }
    if (len > UDP_MTU) {
      ERRORF (BSD,"Sendto, Socket %d msg too large\n",sock);
      EvrNetBSD_SendtoMsgTooLarge (sock, (uint32_t)len);
      RETURN (BSD_EMSGSIZE);
    }
    /* Implicit binding */
    bsd_s->State = BSD_STATE_BOUND;
    bsd_open_udp (bsd_s);

    for (evt = 0;; evt = 1) {
      /* Use "static" to avoid using a local stack */
      static __ADDR to_addr;
      __ADDR *targ;
      if (to) {
        /* Use 'to' target address */
        addr_bsd_to_net (to, &to_addr);
        targ = &to_addr;
      }
      else {
        /* Use target address from 'connect()' */
        targ = &bsd_s->RemHost;
      }

      sendbuf = net_udp_get_buf ((uint32_t)len | 0x80000000);
      if (sendbuf == NULL) {
        ERRORF (BSD,"Sendto, Socket %d no memory\n",sock);
        EvrNetBSD_SendtoNoMemory (sock, (uint32_t)len);
        RETURN (BSD_ENOMEM);
      }
      memcpy (sendbuf, buf, (uint32_t)len);
      if (net_udp_send (bsd_s->Socket, targ, sendbuf, (uint32_t)len) == netOK) {
        break;
      }
      /* Maybe send failed because ARP/NDP is not cached */
      if (evt != 0) {
        /* Retry also failed */
        ERRORF (BSD,"Send, Socket %d failed\n",sock);
        EvrNetBSD_SendDatagramError (sock);
        RETURN (BSD_ERROR);
      }
      if (bsd_s->Flags & BSD_FLAG_NONBLOCK) {
        /* Return error for non-blocking call */
        RETURN (BSD_EWOULDBLOCK);
      }
      /* Wait 200 ms for ARP/NDP resolver and try again */
      evt = bsd_evt_wait (bsd_s, BSD_EVT_SEND, SYS_TICK_T200MS);
      if (evt & BSD_EVT_KILL) {
        /* Socket has been closed locally */
        ERRORF (BSD,"Send, Socket %d killed\n",sock);
        EvrNetBSD_SendSocketKilled (sock);
        RETURN (BSD_ECONNABORTED);
      }
    }
  }
  DEBUGF (BSD," Sent %d bytes\n",len);
  EvrNetBSD_SendCompleteBlocking (sock, (uint32_t)len);
  /* Return number of bytes sent */
  RETURN (len);

  END_LOCK;
}

/**
  \brief       Send a message to endpoint node.
  \param[in]   sock   socket descriptor.
  \param[in]   msg  pointer to msghdr structure containing:
                      - pointer to target address (NULL=none).
                      - array of application buffer(s) containing the message.
                      - pointer to the ancillary data (NULL=none).
  \param[in]   flags  message flags.
  \return      status information.
  \note        Usage currently limited to SOCK_DGRAM only, support for
               SOCK_STREAM not yet implemented.
*/
int32_t sendmsg (int32_t sock, const MSGHDR *msg, int32_t flags) {
  NET_BSD_INFO *bsd_s;
  uint8_t *sendbuf;
  uint32_t msg_len;
  uint32_t i,nw,dlen,evt;

  START_LOCK(int32_t);

  (void)flags;
  DEBUGF (BSD,"Sendmsg Socket %d\n",sock);
  EvrNetBSD_SendmsgSocket (sock);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Sendmsg, Socket %d not valid\n",sock);
    EvrNetBSD_SendmsgSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  if (msg == NULL || msg->msg_iov == NULL || msg->msg_iovlen == 0) {
inv_arg:/* Invalid argument provided */
    ERRORF (BSD,"Sendmsg, Socket %d invalid parameter\n",sock);
    EvrNetBSD_SendmsgInvalidParameter (sock);
    RETURN (BSD_EINVAL);
  }
  /* Calculate message length */
  for (msg_len = i = 0; i < (uint32_t)msg->msg_iovlen; i++) {
    /* Skip NULL buffers */
    if (msg->msg_iov[i].iov_base != NULL) {
      msg_len += msg->msg_iov[i].iov_len;
    }
  }
  if (msg_len == 0) {
    goto inv_arg;
  }

  bsd_s = &bsd->Scb[sock-1];
  if (bsd_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Sendmsg, Socket %d not created\n",sock);
    EvrNetBSD_SendmsgSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }

  if (bsd_s->xThread) {
    /* Locked, other thread is sending */
    ERRORF (BSD,"Sendmsg, Socket %d locked\n",sock);
    EvrNetBSD_SendmsgSocketLocked (sock);
    RETURN (BSD_ELOCKED);
  }

  if (bsd_s->Type == SOCK_STREAM) {
    /* STREAM socket currently not supported */
    RETURN (BSD_ENOTSUP);
  }

  /* Datagram Socket (UDP) */
  if (msg->msg_name &&
      (!is_addr_valid (bsd_s->Family, msg->msg_name, (int32_t)msg->msg_namelen) ||
        is_addr_unspec (msg->msg_name))) {
    /* Destination address parameter error */
    goto inv_arg;
  }
#ifdef Network_IPv6
  if ((bsd_s->Flags & BSD_FLAG_IP6ONLY) &&
       net_addr6_is_4to6 ((SOCKADDR6(msg->msg_name)->sin6_addr.s6_b))) {
    /* IPv4 destination address error */
    goto inv_arg;
  }
#endif
  if (!msg->msg_name && !(bsd_s->Flags & BSD_FLAG_FILTER)) {
    /* Destination address parameter not defined */
    /* and 'connect()' not called previously     */
    ERRORF (BSD,"Sendmsg, Socket %d dst-addr unknown\n",sock);
    EvrNetBSD_SendmsgDestAddrUndefined (sock);
    RETURN (BSD_EDESTADDRREQ);
  }
  if (msg_len > UDP_MTU) {
    ERRORF (BSD,"Sendmsg, Socket %d msg too large\n",sock);
    EvrNetBSD_SendmsgMsgTooLarge (sock, msg_len);
    RETURN (BSD_EMSGSIZE);
  }
  /* Implicit binding */
  bsd_s->State = BSD_STATE_BOUND;
  bsd_open_udp (bsd_s);

  for (evt = 0;; evt = 1) {
    /* Use "static" to avoid using a local stack */
    static __ADDR to_addr;
    __ADDR *targ;
    if (msg->msg_name) {
      /* Use 'msg_name' target address */
      addr_bsd_to_net (msg->msg_name, &to_addr);
      targ = &to_addr;
    }
    else {
      /* Use target address from 'connect()' */
      targ = &bsd_s->RemHost;
    }
    sendbuf = net_udp_get_buf (msg_len | 0x80000000);
    if (sendbuf == NULL) {
      ERRORF (BSD,"Sendmsg, Socket %d no memory\n",sock);
      EvrNetBSD_SendmsgNoMemory (sock, msg_len);
      RETURN (BSD_ENOMEM);
    }
    /* Scatter message copy */
    for (nw = i = 0; nw < msg_len; i++) {
      /* Skip invalid buffers */
      if (msg->msg_iov[i].iov_base != NULL) {
        dlen = msg->msg_iov[i].iov_len;
        if (dlen > (msg_len - nw)) {
          dlen = msg_len - nw;
        }
        memcpy (&sendbuf[nw], msg->msg_iov[i].iov_base, dlen);
        nw += dlen;
      }
    }
    if (net_udp_send (bsd_s->Socket, targ, sendbuf, msg_len) == netOK) {
      break;
    }
    /* Maybe send failed because ARP/NDP is not cached */
    if (evt != 0) {
      /* Retry also failed */
      ERRORF (BSD,"Sendmsg, Socket %d failed\n",sock);
      EvrNetBSD_SendmsgDatagramError (sock);
      RETURN (BSD_ERROR);
    }
    if (bsd_s->Flags & BSD_FLAG_NONBLOCK) {
      /* Return error for non-blocking call */
      RETURN (BSD_EWOULDBLOCK);
    }
    /* Wait 200 ms for ARP/NDP resolver and try again */
    evt = bsd_evt_wait (bsd_s, BSD_EVT_SEND, SYS_TICK_T200MS);
    if (evt & BSD_EVT_KILL) {
      /* Socket has been closed locally */
      ERRORF (BSD,"Sendmsg, Socket %d killed\n",sock);
      EvrNetBSD_SendmsgSocketKilled (sock);
      RETURN (BSD_ECONNABORTED);
    }
  }
  DEBUGF (BSD," Sent %d bytes\n",msg_len);
  EvrNetBSD_SendmsgComplete (sock, msg_len);
  /* Return number of bytes sent */
  RETURN ((int32_t)msg_len);

  END_LOCK;
}

/**
  \brief       Receive data on already connected socket.
  \param[in]   sock   socket descriptor.
  \param[out]  buf    pointer to application data buffer to store the data to.
  \param[in]   len    size of application data buffer (in bytes).
  \param[in]   flags  message flags.
  \return      status information.
*/
int32_t recv (int32_t sock, char *buf, int32_t len, int32_t flags) {
  return (recvfrom (sock, buf, len, flags, NULL, NULL));
}

/**
  \brief       Receive data from endpoint node.
  \param[in]   sock     socket descriptor.
  \param[out]  buf      pointer to application data buffer to store the data to.
  \param[in]   len      size of application data buffer (in bytes).
  \param[in]   flags    message flags.
  \param[out]  from     structure containing remote IP address and port.
  \param[in,out] fromlen length of SOCKADDR structure.
  \return      status information.
  \note        For datagram sockets source address and port are stored,
               if 'from' buffer is provided and valid.
*/
int32_t recvfrom (int32_t sock, char *buf, int32_t len,
                                int32_t flags, SOCKADDR *from, int32_t *fromlen) {
  NET_BUFFER *netbuf,*next;
  NET_BSD_INFO *bsd_s;
  uint32_t nr,dlen,evt;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Recv/from Socket %d\n",sock);
  EvrNetBSD_RecvSocket (sock, (uint32_t)len);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Recv, Socket %d not valid\n",sock);
    EvrNetBSD_RecvSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  if (buf == NULL || len <= 0) {
    ERRORF (BSD,"Recv, Socket %d invalid parameter\n",sock);
    EvrNetBSD_RecvInvalidParameter (sock);
    RETURN (BSD_EINVAL);
  }

  bsd_s = &bsd->Scb[sock-1];
  if (bsd_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Recv, Socket %d not created\n",sock);
    EvrNetBSD_RecvSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }

  if (bsd_s->Thread) {
    /* Locked, other thread is receiving */
    ERRORF (BSD,"Recv, Socket %d locked\n",sock);
    EvrNetBSD_RecvSocketLocked (sock);
    RETURN (BSD_ELOCKED);
  }

  switch (bsd_s->State) {
    /* States valid for Stream socket */
    case BSD_STATE_CONNECTED:
      break;

    case BSD_STATE_LISTEN:
      if (bsd_s->buf_list != NULL) {
        /* Some data still queued after remote close */
        break;
      }
      __FALLTHROUGH;

    case BSD_STATE_CONNREQ:
    case BSD_STATE_CLOSING:
      ERRORF (BSD,"Recv, Socket %d not connected\n",sock);
      EvrNetBSD_RecvSocketNotConnected (sock);
      RETURN (BSD_ENOTCONN);

    case BSD_STATE_CLOSED:
      /* Closed by remote peer, non-blocking call */
      if (bsd_s->buf_list != NULL) {
        /* Some data still queued after remote close */
        break;
      }
      goto closed;

    case BSD_STATE_BOUND:
      if (bsd_s->Type == SOCK_DGRAM) {
        /* DGRAM socket must be bound */
        break;
      }
      __FALLTHROUGH;

    default:
      /* Not possible to receive, wrong socket state */
      ERRORF (BSD,"Recv, Socket %d wrong state\n",sock);
      EvrNetBSD_RecvSocketWrongState (sock);
      RETURN (BSD_ENOTCONN);
  }

  if (bsd_s->buf_list == NULL) {
    /* No data available */
    if ((flags & MSG_DONTWAIT) || (bsd_s->Flags & BSD_FLAG_NONBLOCK)) {
      /* Return Error for Non Blocking call */
      RETURN (BSD_EWOULDBLOCK);
    }

    /* Wait for Data available event */
    evt = bsd_evt_wait (bsd_s, BSD_EVT_DATA | BSD_EVT_CLOSE, bsd_s->RecvTout);
    if (evt & BSD_EVT_CLOSE) {
closed:/* A socket has been closed */
      ERRORF (BSD,"Recv, Socket %d closed\n",sock);
      EvrNetBSD_RecvSocketClosed (sock);
      RETURN (BSD_ECONNRESET);
    }
    if (evt & BSD_EVT_TIMEOUT) {
      /* Receive timeout expired */
      DEBUGF (BSD,"Recv, Socket %d timeout\n",sock);
      EvrNetBSD_RecvTimeout (sock);
      RETURN (BSD_ETIMEDOUT);
    }
    if (evt & BSD_EVT_KILL) {
      /* A socket has been closed locally */
      ERRORF (BSD,"Recv, Socket %d killed\n",sock);
      EvrNetBSD_RecvSocketKilled (sock);
      RETURN (BSD_ECONNABORTED);
    }
    if (!(evt & BSD_EVT_DATA)) {
      /* Should never come here */
      RETURN (BSD_ERROR);
    }
  }

  /* Read available data from the queue to a buffer */
  if (bsd_s->Type == SOCK_DGRAM) {
    /* Datagram socket reads the first available message. If the message is  */
    /* larger than the buffer specified, the buffer is filled with the first */
    /* part of the datagram, the rest of this datagram is silently discarded.*/
    netbuf = bsd_s->buf_list;
    nr = netbuf->length;
    if (nr > (uint32_t)len) {
      nr = (uint32_t)len;
    }
    memcpy (buf, &netbuf->data[netbuf->index], nr);
    addr_net_to_bsd (__ALIGN_CAST(__ADDR *)netbuf->data, bsd_s->Family, from, fromlen);

    if (!(flags & MSG_PEEK)) {
      /* Release the buffer. */
      DEBUGF (BSD,"Que_free %d bytes, Socket %d\n",netbuf->length,sock);
      EvrNetBSD_RecvQueueFree (sock, netbuf->length);
      bsd_s->buf_list = netbuf->next; 
      net_mem_free (__FRAME(netbuf));
    }
  }
  else {
    /* Stream socket reads as much information as currently available up to */
    /* the size of the buffer specified. It returns number of bytes read.   */
    netbuf = bsd_s->buf_list;
    for (nr = 0;;) {
      dlen = netbuf->length - netbuf->index;
      if (dlen > ((uint32_t)len - nr)) {
        dlen = (uint32_t)len - nr;
      }
      memcpy (&buf[nr], &netbuf->data[netbuf->index], dlen);
      nr += dlen;
      if ((netbuf->index + dlen) < netbuf->length) {
        /* User buffer is full, data still remaining in this block */
        if (!(flags & MSG_PEEK)) {
          netbuf->index  += dlen;
          bsd_s->buf_list = netbuf;
        }
        break;
      }
      /* This buffer is done, go get next from the queue */
      next = netbuf->next;
      if (!(flags & MSG_PEEK)) {
        DEBUGF (BSD,"Que_free %d bytes, Socket %d\n",netbuf->length,sock);
        EvrNetBSD_RecvQueueFree (sock, netbuf->length);
        net_mem_free (__FRAME(netbuf));
      }
      netbuf = next;
      if ((netbuf == NULL) || (nr == (uint32_t)len)) {
        /* End of list or user buffer full */
        if (!(flags & MSG_PEEK)) {
          bsd_s->buf_list = netbuf;
          if ((netbuf == NULL) && (bsd_s->Flags & BSD_FLAG_FLOW)) {
            /* Flow-control enabled and no data, reset receive window */
            net_tcp_reset_window (bsd_s->Socket);
          }
        }
        break;
      }
    }
  }
  DEBUGF (BSD," Received %d bytes\n",nr);
  EvrNetBSD_RecvComplete (sock, nr);
  RETURN ((int32_t)nr);

  END_LOCK;
}

/**
  \brief       Receive a message from a socket.
  \param[in]   sock   socket descriptor.
  \param[in,out] msg  pointer to msghdr structure containing:
                      - pointer to buffer to store the source address to (NULL=none).
                      - array of application buffer(s) for the incomming message.
                      - pointer to buffer for the ancillary data (NULL=none).
  \param[in]   flags  message flags.
  \return      status information.
  \note        Usage currently limited to SOCK_DGRAM only, support for
               SOCK_STREAM not yet implemented.
*/
int32_t recvmsg (int32_t sock, MSGHDR *msg, int32_t flags) {
  NET_BUFFER *netbuf;
  NET_BSD_INFO *bsd_s;
  uint32_t msg_ctrl_len;
  uint32_t i,nr,evt;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Recvmsg Socket %d\n",sock);
  EvrNetBSD_RecvmsgSocket (sock);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Recvmsg, Socket %d not valid\n",sock);
    EvrNetBSD_RecvmsgSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  if (msg == NULL || msg->msg_iov == NULL || msg->msg_iovlen == 0) {
    ERRORF (BSD,"Recvmsg, Socket %d invalid parameter\n",sock);
    EvrNetBSD_RecvmsgInvalidParameter (sock);
    RETURN (BSD_EINVAL);
  }

  bsd_s = &bsd->Scb[sock-1];
  if (bsd_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Recvmsg, Socket %d not created\n",sock);
    EvrNetBSD_RecvmsgSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }

  if (bsd_s->Thread) {
    /* Locked, other thread is receiving */
    ERRORF (BSD,"Recvmsg, Socket %d locked\n",sock);
    EvrNetBSD_RecvmsgSocketLocked (sock);
    RETURN (BSD_ELOCKED);
  }

  switch (bsd_s->State) {
    case BSD_STATE_BOUND:
      if (bsd_s->Type == SOCK_DGRAM) {
        /* DGRAM socket must be bound */
        break;
      }
      __FALLTHROUGH;

    default:
      /* Not possible to receive, wrong socket state */
      ERRORF (BSD,"Recvmsg, Socket %d wrong state\n",sock);
      EvrNetBSD_RecvmsgSocketWrongState (sock);
      if (bsd_s->Type == SOCK_STREAM) {
        /* STREAM socket currently not supported */
        RETURN (BSD_ENOTSUP);
      }
      RETURN (BSD_ENOTCONN);
  }

  if (bsd_s->buf_list == NULL) {
    /* No data available */
    if ((flags & MSG_DONTWAIT) || (bsd_s->Flags & BSD_FLAG_NONBLOCK)) {
      /* Return Error for Non Blocking call */
      RETURN (BSD_EWOULDBLOCK);
    }

    /* Wait for Data available event */
    evt = bsd_evt_wait (bsd_s, BSD_EVT_DATA, bsd_s->RecvTout);
    if (evt & BSD_EVT_TIMEOUT) {
      /* Receive timeout expired */
      DEBUGF (BSD,"Recvmsg, Socket %d timeout\n",sock);
      EvrNetBSD_RecvmsgTimeout (sock);
      RETURN (BSD_ETIMEDOUT);
    }
    if (evt & BSD_EVT_KILL) {
      /* A socket has been closed locally */
      ERRORF (BSD,"Recvmsg, Socket %d killed\n",sock);
      EvrNetBSD_RecvmsgSocketKilled (sock);
      RETURN (BSD_ECONNABORTED);
    }
    if (!(evt & BSD_EVT_DATA)) {
      /* Should never come here */
      RETURN (BSD_ERROR);
    }
  }

  /* Read available data from the queue to a buffer */
  netbuf = bsd_s->buf_list;

  /* Copy the source address first */
  addr_net_to_bsd (__ALIGN_CAST(__ADDR *)netbuf->data, bsd_s->Family,
                   msg->msg_name, (int32_t *)&msg->msg_namelen);

  msg->msg_flags = 0;
  msg_ctrl_len   = 0;

  /* Copy control data */
  if (msg->msg_control != NULL) { 
    CMSGHDR *cmsg = (CMSGHDR *)msg->msg_control;
    if (netbuf->index == sizeof(NET_ADDR4) + NET_ADDR_IP4_LEN) {
      /* IP4 destination address is available */
      if (msg->msg_controllen >= CMSG_LEN(NET_ADDR_IP4_LEN)) {
        cmsg->cmsg_len   = CMSG_LEN(NET_ADDR_IP4_LEN);
        cmsg->cmsg_level = IPPROTO_IP;
        cmsg->cmsg_type  = IP_RECVDSTADDR;
        net_addr4_copy (CMSG_DATA(cmsg), &netbuf->data[sizeof(NET_ADDR4)]);
        msg_ctrl_len     = CMSG_LEN(NET_ADDR_IP4_LEN);
      }
      else {
        /* Return flag: Control data truncated */
        msg->msg_flags  |= MSG_CTRUNC;
      }
    }
#ifdef Network_IPv6
    if (netbuf->index == sizeof(NET_ADDR) + NET_ADDR_IP6_LEN) {
      /* IP6 destination address is available */
      if (msg->msg_controllen >= CMSG_LEN(NET_ADDR_IP6_LEN)) {
        cmsg->cmsg_len   = CMSG_LEN(NET_ADDR_IP6_LEN);
        cmsg->cmsg_level = IPPROTO_IPV6;
        cmsg->cmsg_type  = IPV6_RECVDSTADDR;
        net_addr6_copy (CMSG_DATA(cmsg), &netbuf->data[sizeof(NET_ADDR)]);
        msg_ctrl_len     = CMSG_LEN(NET_ADDR_IP6_LEN);
      }
      else {
        /* Return flag: Control data truncated */
        msg->msg_flags  |= MSG_CTRUNC;
      }
    }
#endif
  }
  msg->msg_controllen = msg_ctrl_len;
  
  /* Datagram socket reads the first available message. If the message is  */
  /* larger than the buffer specified, the buffer is filled with the first */
  /* part of the datagram, the rest of this datagram is silently discarded.*/
  for (nr = i = 0; i < (uint32_t)msg->msg_iovlen; i++) {
    /* Use IO buffer from the message header structure */
    uint8_t *buf  = msg->msg_iov[i].iov_base;
    uint32_t len  = msg->msg_iov[i].iov_len;
    /* Skip invalid IO buffers */
    if ((buf != NULL) && (len != 0)) {
      uint32_t dlen = netbuf->length - nr;
      if (dlen > len) {
        dlen = len;
      }
      memcpy (buf, &netbuf->data[netbuf->index+nr], dlen);
      nr += dlen;
    }
  }
  if (nr < netbuf->length) {
    /* Return flag: Normal data truncated */
    msg->msg_flags |= MSG_TRUNC;
  }

  if (!(flags & MSG_PEEK)) {
    /* Release the buffer */
    DEBUGF (BSD,"Que_free %d bytes, Socket %d\n",netbuf->length,sock);
    EvrNetBSD_RecvQueueFree (sock, netbuf->length);
    bsd_s->buf_list = netbuf->next;
    net_mem_free (__FRAME(netbuf));
  }

  DEBUGF (BSD," Received %d bytes\n",nr);
  EvrNetBSD_RecvmsgComplete (sock, nr);
  RETURN ((int32_t)nr);

  END_LOCK;
}

/**
  \brief       Close socket and release socket descriptor.
  \param[in]   sock  socket descriptor.
  \return      status information.
*/
int32_t closesocket (int32_t sock) {
  NET_BSD_INFO *bsd_s, *cli_s;
  int32_t i,evt;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Closesocket %d\n",sock);
  EvrNetBSD_Closesocket(sock);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Close, Socket %d not valid\n",sock);
    EvrNetBSD_CloseSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  bsd_s = &bsd->Scb[sock-1];
  if (bsd_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Close, Socket %d not created\n",sock);
    EvrNetBSD_CloseSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }
 
  if (bsd_s->Type == SOCK_DGRAM) {
    /* Datagram Socket (UDP) */
    bsd_kill_socket (bsd_s);
    RETURN (0);
  }

  /* Stream Socket (TCP) */
  if (bsd_s->State == BSD_STATE_SERVER) {
    /* This is a Server-parent socket */
    while ((cli_s = list_get_first (bsd_s)) != NULL) {
      /* Kill all queued Connect pending sockets */
      bsd_kill_socket (cli_s);
    }
    for (i = 1, cli_s = &bsd->Scb[0]; i <= bsd->NumSocks; cli_s++, i++) {
      if (cli_s->ParentSock == bsd_s->Id) {
        cli_s->ParentSock = 0;
        /* Close all non-active child sockets */
        if (cli_s->State == BSD_STATE_LISTEN) {
          /* Close and release listening child socket */
          bsd_kill_socket (cli_s);
        }
      }
    }
  }
  else {
    /* Close a Server-child or Client socket */
    if (bsd_s->State == BSD_STATE_CLOSING) {
      /* A socket is closing already */
      RETURN (BSD_EWOULDBLOCK);
    }
    if (bsd_s->State == BSD_STATE_CLOSED) {
      goto closed;
    }
    net_tcp_close (bsd_s->Socket);
    /* Resume blocked threads, if any */
    if ((bsd_s->Thread) || (bsd_s->xThread)) {
      /* Thread : accept(), connect(), recv/from() */
      /* xThread: send/to()                        */
      bsd_evt_send (bsd_s, BSD_EVT_KILL);
    }
    if (net_tcp_get_state (bsd_s->Socket) > netTCP_StateLISTEN) {
      /* Need to wait more */
      bsd_s->State = BSD_STATE_CLOSING;
      if (bsd_s->Flags & BSD_FLAG_NONBLOCK) {
        /* Return Error for Non Blocking socket */
        RETURN (BSD_EWOULDBLOCK);
      }
      /* Wait for Close event */
      evt = bsd_evt_wait (bsd_s, BSD_EVT_CLOSE, 0);
      if (!(evt & BSD_EVT_CLOSE)) {
        /* Should never come here */
        RETURN (BSD_ERROR);
      }
    }
closed: /* A socket has been closed */
    /* Check if this is a Server-child socket */
    if (bsd_s->ParentSock != 0) {
      /* Parent socket exists, re-enter listen mode */
      bsd_s->State = BSD_STATE_LISTEN;
      RETURN (0);
    }
  }
  bsd_kill_socket (bsd_s);
  RETURN (0);

  END_LOCK;
}

/**
  \brief       Scan sockets for readability, writability and errors.
  \param[in]   n_max    range of sockets to be scanned.
  \param[in]   rd_fds   pointer to the set of sockets to be checked for read.
                         - NULL for none.
  \param[in]   wr_fds   pointer to the set of sockets to be checked for write.
                         - NULL for none.
  \param[in]   err_fds  pointer to the set of sockets to be checked for error.
                         - NULL for none.
  \param[out]  out_fds  pointer to the array of three output sets.
  \return      number of ready sockets (>0).
  \note        Ready socket can perform I/O operation without blocking.
*/
static int32_t bsd_select_scan (int32_t n_max, fd_set *const rd_fds, fd_set *const wr_fds,
                                               fd_set *const err_fds, fd_set *out_fds) {
  NET_BSD_INFO *bsd_s;
  int32_t i,n_ready = 0;

  /* Check sockets ready to read */
  if (rd_fds != NULL) {
    /* Socket is ready to read when the recv(), recvfrom() or accept() */
    /* functions do not block when called in blocking mode.            */
    /* This happens when:                                              */
    /*   - receive data is available,                                  */
    /*   - a connection request is received in listening mode,         */
    /*   - the remote peer has closed the connection.                  */
    FD_ZERO (out_fds);
    for (i = 1, bsd_s = &bsd->Scb[0]; i <= n_max; bsd_s++, i++) {
      if (__FD_ISSET(i, rd_fds) && (bsd_s->Thread == NULL)) {
        if ((bsd_s->buf_list != NULL) || (bsd_s->AcceptSock != 0) || 
            (bsd_s->State == BSD_STATE_CLOSED)) {
          /* Data available or connect request pending */
          __FD_SET (i, out_fds);
          n_ready++;
        }
      }
    }
  }

  /* Check sockets ready to write */
  out_fds++;
  if (wr_fds != NULL) {
    /* Socket is ready to write, if a stream socket is connected */
    /* or a datagram socket is at least created.                 */
    FD_ZERO (out_fds);
    for (i = 1, bsd_s = &bsd->Scb[0]; i <= n_max; bsd_s++, i++) {
      if (__FD_ISSET(i, wr_fds) && (bsd_s->xThread == NULL)) {
        if (((bsd_s->Type == SOCK_DGRAM)  && (bsd_s->State >= BSD_STATE_CREATED)) ||
            ((bsd_s->Type == SOCK_STREAM) && (bsd_s->State == BSD_STATE_CONNECTED))) {
          /* Datagram socket must be created, stream socket must be connected */
          __FD_SET (i, out_fds);
          n_ready++;
        }
      }
    }
  }

  /* Check sockets for errors */
  out_fds++;
  if (err_fds != NULL) {
    FD_ZERO (out_fds);
    for (i = 1, bsd_s = &bsd->Scb[0]; i <= n_max; bsd_s++, i++) {
      if (__FD_ISSET(i, err_fds)) {
        if (bsd_s->State == BSD_STATE_CLOSED) {
          /* 'connect()' has failed */
          __FD_SET (i, out_fds);
          n_ready++;
        }
      }
    }
  }
  return (n_ready);
}

/**
  \brief       Subscribe to resume events for one or more sockets.
  \param[in]   thread   current running thread.
  \param[in]   n_max    range of sockets to check for subscribtion.
  \param[in]   rd_fds   set of sockets to check for read (NULL=none).
  \param[in]   wr_fds   set of sockets to check for write (NULL=none).
  \param[in]   err_fds  set of sockets to check for error (NULL=none).
  \return      number of subscribed sockets.
*/
static int32_t bsd_evt_subscribe (NETOS_ID thread, int32_t n_max, fd_set *const rd_fds,
                                  fd_set *const wr_fds, fd_set *const err_fds) {
  NET_BSD_INFO *bsd_s;
  uint8_t evt;
  int32_t i,n_evt = 0;

  for (i = 1, bsd_s = &bsd->Scb[0]; i <= n_max; bsd_s++, i++) {
    if (bsd_s->Thread == NULL) {
      evt = 0;
      if (rd_fds && __FD_ISSET(i, rd_fds)) {
        evt |= (BSD_EVT_CONNECT | BSD_EVT_DATA | BSD_EVT_CLOSE);
      }
      if (wr_fds && __FD_ISSET(i, wr_fds) && (bsd_s->xThread == NULL)) {
        evt |= BSD_EVT_SEND;
      }
      if (err_fds && __FD_ISSET(i, err_fds)) {
        evt |= BSD_EVT_CLOSE;
      }
      if (evt != 0) {
        bsd_s->Event  = evt | BSD_EVT_KILL;
        bsd_s->Thread = thread;
        n_evt++;
      }
    }
  }
  return (n_evt);
}

/**
  \brief       Cancel subscribtion to resume events.
  \param[in]   thread   current running thread.
  \param[in]   n_max    range of sockets to cancel subscribtion.
*/
static void bsd_evt_unsubscribe (NETOS_ID thread, int32_t n_max) {
  NET_BSD_INFO *bsd_s;
  int32_t i;

  for (i = 1, bsd_s = &bsd->Scb[0]; i <= n_max; bsd_s++, i++) {
    if (bsd_s->Thread == thread) {
      bsd_s->Thread = NULL;
      bsd_s->Event  = 0;
    }
  }
}

/**
  \brief       Check the status of one or more sockets.
  \param[in]     nfds      range of sockets to be checked.
  \param[in,out] readfds   pointer to the set of sockets to check for read.
                           - NULL for none.
  \param[in,out] writefds  pointer to the set of sockets to check for write.
                           - NULL for none.
  \param[in,out] errorfds  pointer to the set of sockets to check for error.
                           - NULL for none.
  \param[in]     timeout   maximum time for select to wait.
                           - NULL for blocking wait for event.
  \return      number of ready sockets or status information on error.
*/
int32_t select (int32_t nfds, fd_set *readfds, fd_set *writefds,
                              fd_set *errorfds, struct timeval *timeout) {
  /* Use "static" to avoid using a local stack */
  static fd_set scan_fds[3];
  int32_t n_max, retv;
  uint32_t time;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Select nfds=%d\n",nfds);
  EvrNetBSD_Select (nfds);
  /* At least one set must be non-null */
  if ((nfds < 2) || (nfds > (FD_SETSIZE+1)) ||
      ((readfds == NULL) && (writefds == NULL) && (errorfds == NULL))) {
    ERRORF (BSD,"Select, Invalid parameter\n");
    EvrNetBSD_SelectInvalidParameter (nfds);
    RETURN (BSD_EINVAL);
  }

  /* Range of sockets to check */
  n_max = nfds - 1;

  /* Limit to number of BSD sockets */
  if (n_max > bsd->NumSocks) {
    n_max = bsd->NumSocks;
  }

  /* Retrieve timeout value */
  time = NETOS_WAIT_FOREVER;
  if (timeout != NULL) {
    time = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
  }

  /* Scan sockets and check for requested status */
  retv = bsd_select_scan (n_max, readfds, writefds, errorfds, scan_fds);
  if ((retv != 0) || (time == 0)) {
    /* Non-blocking mode, update the sets on return */
    if (readfds)  *readfds  = scan_fds[0];
    if (writefds) *writefds = scan_fds[1];
    if (errorfds) *errorfds = scan_fds[2];
    DEBUGF (BSD,"Select done, n_ready=%d\n",retv);
    EvrNetBSD_SelectComplete (retv);
    RETURN (retv);
  }

  /* Subscribe to resume events */
  retv = bsd_evt_subscribe (netos_thread_id(), n_max, readfds, writefds, errorfds);
  if (retv == 0) {
    ERRORF (BSD,"Select, Suspend failed\n");
    EvrNetBSD_SelectSuspendFailed (n_max+1);
    RETURN (BSD_ERROR);
  }

  /* Suspend current thread until an event occurs */
  net_sys_wait (time);

  /* Cancel subscribtion for remaining sockets */
  bsd_evt_unsubscribe (netos_thread_id(), n_max);

  retv = bsd_select_scan (n_max, readfds, writefds, errorfds, scan_fds);
  /* Blocking mode, update the sets on return */
  if (readfds)  *readfds  = scan_fds[0];
  if (writefds) *writefds = scan_fds[1];
  if (errorfds) *errorfds = scan_fds[2];
  DEBUGF (BSD,"Select blocked, n_ready=%d\n",retv);
  EvrNetBSD_SelectCompleteBlocking (retv);
  RETURN (retv);

  END_LOCK;
}

/**
  \brief       Retrieve IP address and port number of the endpoint node.
  \param[in]   sock     socket descriptor.
  \param[out]  name     structure that will receive IP address and port number.
  \param[in,out] namelen length of SOCKADDR structure.
  \return      status information.
*/
int32_t getpeername (int32_t sock, SOCKADDR *name, int32_t *namelen) {
  NET_BSD_INFO *bsd_s;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Getpeername %d\n",sock);
  EvrNetBSD_Getpeername (sock);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Getpeername, Socket %d not valid\n",sock);
    EvrNetBSD_GetpeerSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  bsd_s = &bsd->Scb[sock-1];
  if (bsd_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Getpeername, Socket %d not created\n",sock);
    EvrNetBSD_GetpeerSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }

  switch (bsd_s->Type) {
    case SOCK_DGRAM:
      if (!(bsd_s->Flags & BSD_FLAG_FILTER)) {
        goto not_conn;
      }
      break;
    case SOCK_STREAM:
      if (bsd_s->State != BSD_STATE_CONNECTED) {
        goto not_conn;
      }
      break;
    default:
not_conn:/* Socket not connected */
      ERRORF (BSD,"Getpeername, Socket %d not connected\n",sock);
      EvrNetBSD_GetpeerSocketNotConnected (sock);
      RETURN (BSD_ENOTCONN);
  }

  /* For Dgram sockets address given with 'connect()' is returned */
  if (!addr_net_to_bsd (&bsd_s->RemHost, bsd_s->Family, name, namelen)) {
    ERRORF (BSD,"Getpeername, Socket %d invalid parameter\n",sock);
    EvrNetBSD_GetpeerInvalidParameter (sock);
    RETURN (BSD_EINVAL);
  }
  RETURN (0);

  END_LOCK;
}

/**
  \brief       Retrieve local IP address and port number.
  \param[in]   sock     socket descriptor.
  \param[out]  name     structure that will receive IP address and port number.
  \param[in,out] namelen length of SOCKADDR structure.
  \return      status information.
*/
int32_t getsockname (int32_t sock, SOCKADDR *name, int32_t *namelen) {
  NET_BSD_INFO *bsd_s;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Getsockname %d\n",sock);
  EvrNetBSD_Getsockname (sock);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Getsockname, Socket %d not valid\n",sock);
    EvrNetBSD_GetsockSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  bsd_s = &bsd->Scb[sock-1];
  if (bsd_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Getsockname, Socket %d not created\n",sock);
    EvrNetBSD_GetsockSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }
  if (bsd_s->State < BSD_STATE_BOUND) {
    ERRORF (BSD,"Getsockname, Socket %d not bound\n",sock);
    EvrNetBSD_GetsockSocketNotBound (sock);
    RETURN (BSD_EINVAL);
  }

  if (!addr_net_to_bsd (&bsd_s->LocHost, bsd_s->Family, name, namelen)) {
    ERRORF (BSD,"Getsockname, Socket %d invalid parameter\n",sock);
    EvrNetBSD_GetsockInvalidParameter (sock);
    RETURN (BSD_EINVAL);
  }
  RETURN (0);

  END_LOCK;
}

/**
  \brief       Manipulate options for the socket.
  \param[in]   sock     socket descriptor.
  \param[in]   level    level at which the option is defined.
  \param[in]   optname  socket option for which the value is to be set.
  \param[in]   optval   pointer to the buffer containing the option value.
  \param[in]   optlen   size of the buffer containing the option value.
  \return      status information.
*/
int32_t setsockopt (int32_t sock, int32_t level,
                                  int32_t optname, const char *optval, int32_t optlen) {
  NET_BSD_INFO *bsd_s;
  uint32_t bval;
  uint16_t bval16;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Setsockopt Socket %d\n",sock);
  EvrNetBSD_Setsockopt (sock, level, optname);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Setsockopt, Socket %d not valid\n",sock);
    EvrNetBSD_SetoptSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  /* Check option parameter */
  if (optval == NULL) {
inv_arg:/* Invalid argument provided */
    ERRORF (BSD,"Setsockopt, Socket %d invalid parameter\n",sock);
    EvrNetBSD_SetoptInvalidParameter (sock);
    RETURN (BSD_EINVAL);
  }

  /* Read option value from buffer (might be unaligned) */
  switch (optlen) {
    case 1:
      /* Single byte value */
      bval = optval[0];
      break;

    case 2:
      /* 2-byte value */
      /* Possibly non-aligned buffer */
      memcpy (&bval16, optval, 2);
      bval = bval16;
      break;

    case 4:
      /* 4-byte value */
      /* Possibly non-aligned buffer */
      memcpy (&bval, optval, 4);
      break;

    default:
      goto inv_arg;
  }

  bsd_s = &bsd->Scb[sock-1];
  if (bsd_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Setsockopt, Socket %d not created\n",sock);
    EvrNetBSD_SetoptSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }

  /* Check level parameter */
  switch (level) {
    case SOL_SOCKET:
      /* Socket level */
      switch (optname) {
        case SO_KEEPALIVE:
          /* Enable/disable keep-alive mode */
          if (bsd_s->Type != SOCK_STREAM) {
not_supp:   ERRORF (BSD,"Setsockopt, Socket %d opt not supported\n",sock);
            EvrNetBSD_SetoptOptionNotSupported (sock, level, optname);
            RETURN (BSD_ENOTSUP);
          }
          bsd_s->Flags &= ~BSD_FLAG_KEEP;
          if (bval) {
            bsd_s->Flags |= BSD_FLAG_KEEP;
            bval = 1;
          }
          DEBUGF (BSD," %sable Keep-alive mode\n", bval ? "En" : "Dis");
          EvrNetBSD_SetoptKeepAlive (sock, bval);
          net_tcp_set_option (bsd_s->Socket, netTCP_OptionKeepAlive, bval);
          break;

        case SO_RCVTIMEO:
          /* Receive timeout in blocking mode */
          /* Timeout 0=infinite timeout       */
          if (bval) {
            /* Timeout limited from 500ms to 600sec */
            if (bval <    500) bval =    500;
            if (bval > 600000) bval = 600000;
            bval = ((uint32_t)bval + SYS_TICK_INTERVAL-1) / SYS_TICK_INTERVAL;
          }
          bsd_s->RecvTout = bval & 0xFFFF;
          DEBUGF (BSD," RecvTimeout=%dms\n", bval*SYS_TICK_INTERVAL);
          EvrNetBSD_SetoptRecvTimeout (sock, bval*SYS_TICK_INTERVAL);
          break;

        case SO_SNDTIMEO:
          /* Send timeout in blocking mode */
          /* Timeout 0=infinite timeout    */
          if (bval) {
            /* Timeout limited from 500ms to 25.5sec */
            if (bval <   500) bval =   500;
            if (bval > 25500) bval = 25500;
            bval = ((uint32_t)bval + SYS_TICK_INTERVAL-1) / SYS_TICK_INTERVAL;
          }
          bsd_s->SendTout = bval & 0xFF;
          DEBUGF (BSD," SendTimeout=%dms\n", bval*SYS_TICK_INTERVAL);
          EvrNetBSD_SetoptSendTimeout (sock, bval*SYS_TICK_INTERVAL);
          break;

        case SO_BINDTODEVICE:
          /* Bind a socket to network interface */
          if (bsd_s->Type != SOCK_DGRAM) {
            goto not_supp;
          }
          if (net_udp_set_option (bsd_s->Socket, netUDP_OptionInterface, bval) != netOK) {
            /* Invalid interface id */
            goto inv_arg;
          }
          DEBUGF (BSD," Bind to %s\n", net_if_map_lan(bval)->Name);
          EvrNetBSD_SetoptBindToDevice (sock, bval);
          break;

        default:
          goto inv_arg;
      }
      break;

    case IPPROTO_IP:
      /* IPv4 protocol level */
#ifdef Network_IPv6
      if (bsd_s->Flags & BSD_FLAG_IP6ONLY) {
        /* Not supported on IPv6-only sockets */
        goto not_supp;
      }
#endif
      switch (optname) {
        case IP_TOS:
          if (bval > 255) goto inv_arg;
          DEBUGF (BSD," IP4-TOS=%d\n", bval);
          EvrNetBSD_SetoptIp4Tos (sock, bval & 0xFF);
          bsd_s->Tos = bval & 0xFF;
          if (bsd_s->Socket == 0) {
            /* Stream server socket */
            break;
          }
          if (bsd_s->Type == SOCK_STREAM) {
            net_tcp_set_option (bsd_s->Socket, netTCP_OptionTOS, bval & 0xFF);
            break;
          }
          net_udp_set_option (bsd_s->Socket, netUDP_OptionTOS, bval & 0xFF);
          break;

        case IP_TTL:
          if (bsd_s->Type != SOCK_DGRAM) {
            goto not_supp;
          }
          if (bval > 255) goto inv_arg;
          DEBUGF (BSD," IP4-TTL=%d\n", bval);
          EvrNetBSD_SetoptIp4Ttl (sock, bval & 0xFF);
          net_udp_set_option (bsd_s->Socket, netUDP_OptionTTL, bval & 0xFF);
          break;

        case IP_RECVDSTADDR:
          if (bsd_s->Type != SOCK_DGRAM) {
            goto not_supp;
          }
          bsd_s->Flags &= ~BSD_FLAG_DESTIP4;
          if (bval) {
            bsd_s->Flags |= BSD_FLAG_DESTIP4;
            bval = 1;
          }
          DEBUGF (BSD," %sable IP4-RecvDstAddr\n", bval ? "En" : "Dis");
          EvrNetBSD_SetoptIp4RecvDstAddr (sock, bval);
          break;

        default:
          goto inv_arg;
      }
      break;

#ifdef Network_IPv6
    case IPPROTO_IPV6:
      /* IPv6 protocol level */
      if (bsd_s->Family != AF_INET6) {
        /* Not supported on IPv4 sockets */
        goto not_supp;
      }
      switch (optname) {
        case IPV6_TCLASS:
          if (bval > 255) goto inv_arg;
          DEBUGF (BSD," IP6-TrafficClass=%d\n", bval);
          EvrNetBSD_SetoptIp6Tclass (sock, bval & 0xFF);
          bsd_s->TClass = bval & 0xFF;
          if (bsd_s->Socket == 0) {
            /* Stream server socket */
            break;
          }
          if (bsd_s->Type == SOCK_STREAM) {
            net_tcp_set_option (bsd_s->Socket, netTCP_OptionTrafficClass, bval & 0xFF);
            break;
          }
          net_udp_set_option (bsd_s->Socket, netUDP_OptionTrafficClass, bval & 0xFF);
          break;

        case IPV6_MULTICAST_HOPS:
          if (bsd_s->Type != SOCK_DGRAM) {
            goto not_supp;
          }
          if (bval > 255) goto inv_arg;
          DEBUGF (BSD," IP6-HopLimit=%d\n", bval);
          EvrNetBSD_SetoptIp6HopLimit (sock, bval & 0xFF);
          net_udp_set_option (bsd_s->Socket, netUDP_OptionHopLimit, bval & 0xFF);
          break;

        case IPV6_RECVDSTADDR:
          if (bsd_s->Type != SOCK_DGRAM) {
            goto not_supp;
          }
          bsd_s->Flags &= ~BSD_FLAG_DESTIP6;
          if (bval) {
            bsd_s->Flags |= BSD_FLAG_DESTIP6;
            bval = 1;
          }
          DEBUGF (BSD," %sable IP6-RecvDstAddr\n", bval ? "En" : "Dis");
          EvrNetBSD_SetoptIp6RecvDstAddr (sock, bval);
          break;

        case IPV6_V6ONLY:
          if (bsd_s->State != BSD_STATE_CREATED) {
            ERRORF (BSD,"Setsockopt, Socket %d already bound\n",sock);
            EvrNetBSD_SetoptSocketBound (sock);
            RETURN (BSD_ERROR);
          }
          /* Change possible before binding */
          bsd_s->Flags &= ~BSD_FLAG_IP6ONLY;
          if (bval) {
            bsd_s->Flags |= BSD_FLAG_IP6ONLY;
            bval = 1;
          }
          DEBUGF (BSD," %sable IP6-Only\n", bval ? "En" : "Dis");
          EvrNetBSD_SetoptIp6Only (sock, bval);
          break;

        default:
          goto inv_arg;
      }
      break;
#endif

    default:
      goto inv_arg;
  }

  RETURN (0);

  END_LOCK;
}

/**
  \brief       Retrieve options for the socket.
  \param[in]   sock     socket descriptor.
  \param[in]   level    level at which the option is defined.
  \param[in]   optname  socket option for which the value is to be retrived.
  \param[out]  optval   pointer to the buffer that will receive the option value.
  \param[in,out] optlen pointer to the size of the buffer in bytes.
  \return      status information.
*/
int32_t getsockopt (int32_t sock, int32_t level,
                                  int32_t optname, char *optval, int32_t *optlen) {
  NET_BSD_INFO *bsd_s;
  uint16_t retv16;
  uint32_t retv;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Getsockopt Socket %d\n",sock);
  EvrNetBSD_Getsockopt (sock, level, optname);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Getsockopt, Socket %d not valid\n",sock);
    EvrNetBSD_GetoptSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  /* Check option parameters */
  if ((optval == NULL) || (optlen == NULL) || (*optlen <= 0)) {
inv_arg:/* Invalid argument provided */
    ERRORF (BSD,"Getsockopt, Socket %d invalid parameter\n",sock);
    EvrNetBSD_GetoptInvalidParameter (sock);
    RETURN (BSD_EINVAL);
  }

  bsd_s = &bsd->Scb[sock-1];
  if (bsd_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Getsockopt, Socket %d not created\n",sock);
    EvrNetBSD_GetoptSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }

  /* Check level parameter */
  switch (level) {
    case SOL_SOCKET:
      /* Socket level */
      switch (optname) {
        case SO_KEEPALIVE:
          /* Keep-alive mode */
          if (bsd_s->Type != SOCK_STREAM) {
not_supp:   ERRORF (BSD,"Getsockopt, Socket %d opt not supported\n",sock);
            EvrNetBSD_GetoptOptionNotSupported (sock, level, optname);
            RETURN (BSD_ENOTSUP);
          }
          retv = (bsd_s->Flags & BSD_FLAG_KEEP) ? 1 : 0;
          break;

        case SO_RCVTIMEO:
          /* Receive timeout in blocking mode (ms) */
          retv = bsd_s->RecvTout * SYS_TICK_INTERVAL;
          break;

        case SO_SNDTIMEO:
          /* Send timeout in blocking mode (ms) */
          retv = bsd_s->SendTout * SYS_TICK_INTERVAL;
          break;

        case SO_TYPE:
          /* Socket type */
          retv = bsd_s->Type;
          break;

        case SO_BINDTODEVICE:
          /* Bound network interface */
          if (bsd_s->Type != SOCK_DGRAM) {
            goto not_supp;
          }
          retv = net_udp_get_option (bsd_s->Socket, netUDP_OptionInterface);
          break;

        default:
          goto inv_arg;
      }
      break;

    case IPPROTO_IP:
      /* IPv4 protocol level */
#ifdef Network_IPv6
      if (bsd_s->Flags & BSD_FLAG_IP6ONLY) {
        /* Not supported on IPv6-only sockets */
        goto not_supp;
      }
#endif
      switch (optname) {
        case IP_TOS:
          retv = bsd_s->Tos;
          break;

        case IP_TTL:
          if (bsd_s->Type != SOCK_DGRAM) {
            goto not_supp;
          }
          retv = net_udp_get_option (bsd_s->Socket, netUDP_OptionTTL);
          break;

        case IP_RECVDSTADDR:
          if (bsd_s->Type != SOCK_DGRAM) {
            goto not_supp;
          }
          retv = (bsd_s->Flags & BSD_FLAG_DESTIP4) ? 1 : 0;
          break;

        default:
          goto inv_arg;
      }
      break;

#ifdef Network_IPv6
    case IPPROTO_IPV6:
      /* IPv6 protocol level */
      if (bsd_s->Family != AF_INET6) {
        /* Not supported on IPv4 sockets */
        goto not_supp;
      }
      switch (optname) {
        case IPV6_TCLASS:
          retv = bsd_s->TClass;
          break;

        case IPV6_MULTICAST_HOPS:
          if (bsd_s->Type != SOCK_DGRAM) {
            goto not_supp;
          }
          retv = net_udp_get_option (bsd_s->Socket, netUDP_OptionHopLimit);
          break;

        case IPV6_RECVDSTADDR:
          if (bsd_s->Type != SOCK_DGRAM) {
            goto not_supp;
          }
          retv = (bsd_s->Flags & BSD_FLAG_DESTIP6) ? 1 : 0;
          break;

        case IPV6_V6ONLY:
          retv = (bsd_s->Flags & BSD_FLAG_IP6ONLY) ? 1 : 0;
          break;

        default:
          goto inv_arg;
      }
      break;
#endif

    default:
      goto inv_arg;
  }

  /* Write option value to return buffer (might be unaligned) */
  switch (*optlen) {
    case 1:
      if (retv >= 0x100) {
        /* Buffer too small for the result */
        goto inv_arg;
      }
      /* Single byte result */
      optval[0] = retv & 0xFF;
      break;

    case 2:
    case 3:
      if (retv >= 0x10000) {
        /* Buffer too small for the result */
        goto inv_arg;
      }
      /* 2-byte result */
      *optlen = 2;
      /* Possibly non-aligned buffer */
      retv16  = retv & 0xFFFF;
      memcpy (optval, &retv16, 2);
      break;

    default:
      /* 4-byte result */
      *optlen = 4;
      /* Possibly non-aligned buffer */
      memcpy (optval, &retv, 4);
      break;
  }
  RETURN (0);

  END_LOCK;
}

/**
  \brief       Control IO mode of a socket.
  \param[in]   sock  socket descriptor.
  \param[in]   cmd   command to perform.
  \param[in]   argp  command's parameter.
  \return      status information.
*/
int32_t ioctlsocket (int32_t sock, long cmd, unsigned long *argp) {
  NET_BSD_INFO *bsd_s;
  uint32_t bval;

  START_LOCK(int32_t);

  DEBUGF (BSD,"Ioctl Socket %d\n",sock);
  EvrNetBSD_Ioctlsocket (sock);
  if (sock <= 0 || sock > bsd->NumSocks) {
    ERRORF (BSD,"Ioctl, Socket %d not valid\n",sock);
    EvrNetBSD_IoctlSocketNotValid (sock);
    RETURN (BSD_ESOCK);
  }

  if (argp == NULL) {
inv_arg:/* Invalid argument provided */
    ERRORF (BSD,"Ioctl, Socket %d invalid parameter\n",sock);
    EvrNetBSD_IoctlInvalidParameter (sock);
    RETURN (BSD_EINVAL);
  }

  bsd_s = &bsd->Scb[sock-1];
  if (bsd_s->State < BSD_STATE_CREATED) {
    ERRORF (BSD,"Ioctl, Socket %d not created\n",sock);
    EvrNetBSD_IoctlSocketNotCreated (sock);
    RETURN (BSD_ESOCK);
  }

  /* Check command parameter */
  switch (cmd) {
    case FIONBIO:
      break;

    case FIO_DELAY_ACK:
    case FIO_KEEP_ALIVE:
    case FIO_FLOW_CTRL:
      if (bsd_s->Type == SOCK_STREAM) {
        break;
      }
      ERRORF (BSD,"Ioctl, Socket %d not STREAM type\n",sock);
      EvrNetBSD_IoctlSocketNotStreamType (sock);
      RETURN (BSD_ENOTSUP);

    default:
      goto inv_arg;
  }

  bval = *argp;
  switch (cmd) {
    case FIONBIO:
      /* Enable/disable non-blocking mode */
      DEBUGF (BSD," %sable Non-blocking mode\n", bval ? "En" : "Dis");
      EvrNetBSD_IoctlNonBlocking (sock, bval);
      bsd_s->Flags &= ~BSD_FLAG_NONBLOCK;
      if (bval) {
        bsd_s->Flags |= BSD_FLAG_NONBLOCK;
      }
      break;

    case FIO_DELAY_ACK:
      /* Enable/disable delay-ack mode */
      DEBUGF (BSD," %sable Delay-ack mode\n", bval ? "En" : "Dis");
      EvrNetBSD_IoctlDelayAck (sock, bval);
      bsd_s->Flags &= ~BSD_FLAG_DACK;
      if (bval) {
        bsd_s->Flags |= BSD_FLAG_DACK;
      }
      set_sock_type (bsd_s);
      break;

    case FIO_KEEP_ALIVE:
      /* Enable/disable keep-alive mode. */
      DEBUGF (BSD," %sable Keep-alive mode\n", bval ? "En" : "Dis");
      EvrNetBSD_IoctlKeepAlive (sock, bval);
      bsd_s->Flags &= ~BSD_FLAG_KEEP;
      if (bval) {
        bsd_s->Flags |= BSD_FLAG_KEEP;
      }
      set_sock_type (bsd_s);
      break;

    case FIO_FLOW_CTRL:
      /* Enable/disable flow-control mode */
      DEBUGF (BSD," %sable Flow-control mode\n", bval ? "En" : "Dis");
      EvrNetBSD_IoctlFlowControl (sock, bval);
      bsd_s->Flags &= ~BSD_FLAG_FLOW;
      if (bval) {
        bsd_s->Flags |= BSD_FLAG_FLOW;
      }
      set_sock_type (bsd_s);
      break;
  }
  RETURN (0);

  END_LOCK;
}

/**
  \brief       Periodically check sockets for timeouts.
*/
void net_bsd_socket_run (void) {
  NET_BSD_INFO *bsd_s;
  int32_t i;

  if (!(sys->Flags & SYS_FLAG_TICK)) {
    return;
  }
  /* Execution interval is 1 tick */
  bsd_s = &bsd->Scb[0];
  /* Check BSD sockets if there is something to do */
  for (i = 0; i < bsd->NumSocks; bsd_s++, i++) {
    switch (bsd_s->State) {
      case BSD_STATE_LISTEN:
        /* Keep TCP sockets listening */
        if (net_tcp_get_state (bsd_s->Socket) < netTCP_StateLISTEN) {
          /* This socket is not listening, activate listening again */
          net_tcp_listen (bsd_s->Socket, bsd_s->LocHost.port);
        }
        break;

      case BSD_STATE_CLOSING:
        /* Poll when the TCP socket is closed */
        if (net_tcp_get_state (bsd_s->Socket) > netTCP_StateLISTEN) {
          break;
        }
        bsd_evt_send (bsd_s, BSD_EVT_CLOSE);
        bsd_s->State = BSD_STATE_CLOSED;
        break;

      case BSD_STATE_BOUND:
      case BSD_STATE_CONNECTED:
        /* Process timeout for blocking calls */
        if (bsd_s->Timer != 0) {
          /* Receiver timer */
          if (--bsd_s->Timer == 0) {
            bsd_s->Event = BSD_EVT_TIMEOUT;
            net_sys_resume (&bsd_s->Thread);
          }
        }
        if (bsd_s->xTimer != 0) {
          /* Sender timer */
          if (--bsd_s->xTimer == 0) {
            bsd_s->xEvent = BSD_EVT_TIMEOUT;
            net_sys_resume (&bsd_s->xThread);
          }
        }
        break;
    }
  }
}

/**
  \brief       Convert text address to a network address.
  \param[in]   cp   text address in standard dot notation.
  \return      status information:
               - internet address on success.
               - INADDR_NONE = on error.
*/
IN_ADDR inet_addr (const char *cp) {
  IN_ADDR ip4_addr;

  if (!netIP_aton (cp, NET_ADDR_IP4, (uint8_t *)&ip4_addr)) {
    /* Invalid input */
    ip4_addr.s_addr = INADDR_NONE;
  }
  return (ip4_addr);
}

/**
  \brief       Convert text address to a network address.
  \param[in]   cp    text address in standard dot notation.
  \param[out]  addr  buffer where the converted IPv4 address is to be stored.
  \return      status:
               - 1 (true) on success,
               - 0 (false) on error.
*/
int32_t inet_aton (const char *cp, IN_ADDR *addr) {
  return (netIP_aton (cp, NET_ADDR_IP4, (uint8_t *)addr));
}

/**
  \brief       Convert network address to a text string.
  \param[in]   in    internet IPv4 host address to convert.
  \return      pointer to the formatted string.
*/
const char *inet_ntoa (IN_ADDR in) {
  static char buf[INET_ADDRSTRLEN];

  return (netIP_ntoa (NET_ADDR_IP4, (uint8_t *)&in, buf, sizeof(buf)));
}

/**
  \brief       Convert text address to a binary network address.
  \param[in]   af    address family:
                     - AF_INET  = Internet Address Family (IPv4).
                     - AF_INET6 = Internet Address Family version 6 (IPv6).
  \param[in]   src   text address to be converted.
  \param[out]  dst   buffer where the converted address is to be stored.
  \return      status:
               - 1 (true)  on success,
               - 0 (false) on error.
*/
int32_t inet_pton (int32_t af, const char *src, void *dst) {
  switch (af) {
    case AF_INET:
      return (netIP_aton (src, NET_ADDR_IP4, dst));
#ifdef Network_IPv6
    case AF_INET6:
      return (netIP_aton (src, NET_ADDR_IP6, dst));
#endif
  }
  return (0);
}

/**
  \brief       Convert binary network address to a text string.
  \param[in]   af    address family:
                     - AF_INET  = Internet Address Family (IPv4).
                     - AF_INET6 = Internet Address Family version 6 (IPv6).
  \param[in]   src   binary address in network byte order to be converted.
  \param[out]  dst   buffer where the converted text address is to be stored.
  \param[in]   size  size of the buffer.
  \return      pointer to the formatted string.
               - NULL in case of error.
*/
const char *inet_ntop (int32_t af, const void *src, char *dst, int32_t size) {
  switch (af) {
    case AF_INET:
      return (netIP_ntoa (NET_ADDR_IP4, src, dst, (uint32_t)size));
#ifdef Network_IPv6
    case AF_INET6:
      return (netIP_ntoa (NET_ADDR_IP6, src, dst, (uint32_t)size));
#endif
  }
  return (NULL);
}

/**
  \brief       Socket TCP event callback notification.
  \param[in]   socket  native TCP socket handle.
  \param[in]   event   notification event.
  \param[in]   addr    structure containing remote IP address and port.
  \param[in]   buf     pointer to a buffer containing the data.
  \param[in]   len     length of the data.
  \return      status:
               - true on success,
               - false on error.
*/
static uint32_t bsd_cb_tcp (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                            const uint8_t *buf, uint32_t len) {
  NET_BSD_INFO *bsd_s, *srv_s;
  NET_BUFFER *netbuf;

  /* Find the owner BSD socket */
  bsd_s = bsd_map_socket (socket, SOCK_STREAM);
  if (bsd_s == NULL) {
    return (false);
  }

  switch (event) {
    case netTCP_EventConnect:
      if (bsd_s->ParentSock == 0) {
        /* Reject connections for client sockets */
        return (false);
      }
      srv_s = &bsd->Scb[bsd_s->ParentSock - 1];
#ifdef Network_IPv6
      /* Check dual-stack socket */
      if (addr->addr_type == NET_ADDR_IP4) {
        if (srv_s->Flags & BSD_FLAG_IP6ONLY) {
          /* Reject IPv4 on IPv6-restricted socket */
          return (false);
        }
      }
      else {
        if (srv_s->Family == AF_INET) {
          /* Reject IPv6 on IPv4-only socket */
          return (false);
        }
      }
#endif
      if ((srv_s->Thread == NULL) && !(srv_s->Flags & BSD_FLAG_NONBLOCK)) {
        /* No thread is waiting for connection in blocking mode, */
        /* so do not acknowledge a connect request from endpont  */
        return (TCP_NO_ACK);
      }
      /* Store remote IP address and port */
      DEBUGF (BSD,"Socket %d, Evt_%s\n",bsd_s->Id,evt_ascii(event));
      DEBUG_INFO ((const __ADDR *)addr);
      EvrNetBSD_CbfuncTcpEvent (bsd_s->Id, event);
      EvrNetBSD_ViewNetAddress (addr);
      net_addr_copy (&bsd_s->RemHost, (const __ADDR *)addr);
      break;

    case netTCP_EventAborted:
      if (buf != NULL) {
        /* Non-null on net_tcp_connect() timeout */
        /* Timeout signalled only in client mode */
        bsd_s->Flags |= BSD_FLAG_TIMEOUT;
      }
      __FALLTHROUGH;

    case netTCP_EventClosed:
      /* Socket aborted or closed */
      DEBUGF (BSD,"Socket %d, Evt_%s\n",bsd_s->Id,evt_ascii(event));
      EvrNetBSD_CbfuncTcpEvent (bsd_s->Id, event);
      memset (&bsd_s->RemHost, 0, sizeof (bsd_s->RemHost));
      bsd_s->State = BSD_STATE_CLOSED;
      bsd_evt_send (bsd_s, BSD_EVT_CLOSE);
      break;

    case netTCP_EventEstablished:
      /* Socket is connected */
      DEBUGF (BSD,"Socket %d, Evt_%s\n",bsd_s->Id,evt_ascii(event));
      EvrNetBSD_CbfuncTcpEvent (bsd_s->Id, event);
      bsd_s->State = BSD_STATE_CONNECTED;
      if (bsd_s->ParentSock != 0) {
        list_add_tail (bsd_s);
        que_free_list (bsd_s);
        /* Notify parent socket of CONNECT event */
        bsd_s = &bsd->Scb[bsd_s->ParentSock - 1];
      }
      bsd_evt_send (bsd_s, BSD_EVT_CONNECT);
      break;

    case netTCP_EventACK:
      /* Data has been acknowledged */
      bsd_evt_send (bsd_s, BSD_EVT_ACK);
      break;

    case netTCP_EventData:
      /* Data has been received */
      netbuf = __BUFFER(net_mem_alloc (BSD_HEADER_LEN + len));
      if (netbuf == NULL) {
        /* No memory, do not ack the packet */
        ERRORF (BSD,"Que_add, Socket %d no memory\n",bsd_s->Id);
        EvrNetBSD_CbfuncTcpNoMemory (bsd_s->Id);
        return (TCP_NO_ACK);
      }
      DEBUGF (BSD,"Que_add %d bytes, Socket %d\n",len,bsd_s->Id);
      EvrNetBSD_CbfuncTcpQueueAdd (bsd_s->Id, len);
      netbuf->length = len & 0xFFFF;
      netbuf->index  = 0;
      memcpy (&netbuf->data[0], buf, len);
      que_add_buf (bsd_s, netbuf);

      bsd_evt_send (bsd_s, BSD_EVT_DATA);
      break;
  }
  return(true);
}

/**
  \brief       Socket UDP event callback notification.
  \param[in]   socket   native UDP socket handle.
  \param[in]   addr     structure containing remote IP address and port.
  \param[in]   buf      pointer to a buffer containing the data.
  \param[in]   len      length of the data.
  \return      status:
               - true  = success,
               - false = error.
*/
static uint32_t bsd_cb_udp (int32_t socket, const NET_ADDR *addr,
                                            const uint8_t *buf, uint32_t len) {
  NET_BSD_INFO *bsd_s;
  NET_BUFFER *netbuf;
  uint32_t addr_sz;

  bsd_s = bsd_map_socket (socket, SOCK_DGRAM);
  if (bsd_s == NULL) {
    return (false);
  }

#ifdef Network_IPv6
  /* Check dual-stack socket */
  if (addr->addr_type == NET_ADDR_IP4) {
    if (bsd_s->Flags & BSD_FLAG_IP6ONLY) {
      /* Ignore IPv4 on IPv6-restricted socket */
      return (false);
    }
  }
  else {
    if (bsd_s->Family == AF_INET) {
      /* Ignore IPv6 on IPv4-only socket */
      return (false);
    }
  }
#endif

  if (bsd_s->Flags & BSD_FLAG_FILTER) {
    /* Packet filtering, check remote IP address */
    if (!net_addr_comp (&bsd_s->RemHost, (const __ADDR *)addr)) {
      /* Ignore, remote address different */
      return (false);
    }
  }

  /* Calculate address size for the buffer.      */
  /* Source net address and optional destination */
  /* IP address are written before the data.     */
  if (addr->addr_type == NET_ADDR_IP4) {
    addr_sz = sizeof (NET_ADDR4);
    if (bsd_s->Flags & BSD_FLAG_DESTIP4) {
      /* Add space for IPv4 destination address */
      addr_sz += NET_ADDR_IP4_LEN;
    }
  }
  else {
#ifdef Network_IPv6
    addr_sz = sizeof (NET_ADDR);
    if (bsd_s->Flags & BSD_FLAG_DESTIP6) {
      /* Add space for IPv6 destination address */
      addr_sz += NET_ADDR_IP6_LEN;
    }
#else
    /* IPv6 not supported, ignore packet */
    return (false);
#endif
  }
  netbuf = __BUFFER(net_mem_alloc (BSD_HEADER_LEN + addr_sz + len));
  if (netbuf == NULL) {
    /* No memory, ignore packet */
    ERRORF (BSD,"Que_add, Socket %d dumped %d bytes\n",bsd_s->Id,len);
    EvrNetBSD_CbfuncUdpDumpData (bsd_s->Id, len);
    return (false);
  }
  DEBUGF (BSD,"Que_add %d bytes, Socket %d\n",len,bsd_s->Id);
  EvrNetBSD_CbfuncUdpQueueAdd (bsd_s->Id, len);
  /* Copy the addresses before starting the data */
  net_addr_copy (__ALIGN_CAST(__ADDR *)netbuf->data, (const __ADDR *)addr);
  if (addr->addr_type == NET_ADDR_IP4) {
    if (bsd_s->Flags & BSD_FLAG_DESTIP4) {
      net_addr4_copy (&netbuf->data[sizeof(NET_ADDR4)],(__ADDR_X(addr))->dst_addr);
    }
  }
#ifdef Network_IPv6
  else {
    if (bsd_s->Flags & BSD_FLAG_DESTIP6) {
      net_addr6_copy (&netbuf->data[sizeof(NET_ADDR)],(__ADDR_X(addr))->dst_addr);
    }
  }
#endif
  /* Copy the data */
  memcpy (&netbuf->data[addr_sz], buf, len);
  netbuf->length = len     & 0xFFFF;
  netbuf->index  = addr_sz & 0xFFFF;
  que_add_buf (bsd_s, netbuf);

  bsd_evt_send (bsd_s, BSD_EVT_DATA);
  return (true);
}

/**
  \brief       Map a native socket to a BSD socket.
  \param[in]   socket  native TCP/UDP socket handle.
  \param[in]   type    BSD socket type.
  \return      pointer to BSD socket control block.
*/
static NET_BSD_INFO *bsd_map_socket (int32_t socket, int32_t type) {
  NET_BSD_INFO *bsd_s;
  int32_t i;

  for (i = 1, bsd_s = &bsd->Scb[0]; i <= bsd->NumSocks; bsd_s++, i++) {
    if (bsd_s->Socket == socket && bsd_s->Type == type) {
      return (bsd_s);
    }
  }
  return (NULL);
}

/**
  \brief       Get pending connect request socket from the queue.
  \param[in]   srv_s  pointer to a socket control block.
  \return      pointer to BSD socket control block.
  \note        Sorting order is based on indexes, not pointers!
*/
static NET_BSD_INFO *list_get_first (NET_BSD_INFO *srv_s) {
  NET_BSD_INFO *bsd_s = NULL;

  if (srv_s->AcceptSock != 0) {
    bsd_s = &bsd->Scb[srv_s->AcceptSock-1];
    srv_s->AcceptSock = bsd_s->AcceptSock;
  }
  return (bsd_s);
}

/**
  \brief       Add pending socket to the accept queue.
  \param[in]   bsd_s  pointer to a socket control block.
  \note        Sorting order is based on indexes, not pointers!
*/
static void list_add_tail (NET_BSD_INFO *bsd_s) {
  NET_BSD_INFO *next_s;

  bsd_s->AcceptSock = 0;
  /* Scan the queue chain */
  next_s = &bsd->Scb[bsd_s->ParentSock-1];
  while (next_s->AcceptSock != 0) {
    next_s = &bsd->Scb[next_s->AcceptSock-1];
  }
  next_s->AcceptSock = bsd_s->Id;
}

/**
  \brief       Add data buffer to a socket queue tail.
  \param[in]   bsd_s   pointer to a socket control block.
  \param[in]   netbuf  pointer to netbuf containing the data.
*/
static void que_add_buf (NET_BSD_INFO *bsd_s, NET_BUFFER *netbuf) {
  NET_BUFFER *next_buf;

  netbuf->next = NULL;
  if (bsd_s->buf_list == NULL) {
    /* First data block added */
    bsd_s->buf_list = netbuf;
  }
  else {
    /* Scan the queue chain */
    next_buf = bsd_s->buf_list;
    while (next_buf->next != NULL) {
      next_buf = next_buf->next;
    }
    /* Append the buffer to queue tail */
    next_buf->next = netbuf;
  }
}

/**
  \brief       Release chained netbuf data buffers.
  \param[in]   bsd_s  pointer to a socket control block.
*/
static void que_free_list (NET_BSD_INFO *bsd_s) {
  NET_BUFFER *netbuf, *next_buf;
  uint32_t size = 0;

  netbuf = bsd_s->buf_list;
  bsd_s->buf_list = NULL;
  for ( ; netbuf; netbuf = next_buf) {
    size    += netbuf->length;
    next_buf = netbuf->next;
    net_mem_free (__FRAME(netbuf));
  }
  if (size != 0) {
    DEBUGF (BSD," Que_free %d bytes\n",size);
    EvrNetBSD_RecvQueueFree (bsd_s->Id, size);
  }
}

/**
  \brief       Kill active BSD socket.
  \param[in]   bsd_s  pointer to a socket control block.
*/
static void bsd_kill_socket (NET_BSD_INFO *bsd_s) {
  /* Close and release native TCP/UDP sockets */
  if (bsd_s->Socket) {
    if (bsd_s->Type == SOCK_DGRAM) {
      net_udp_close (bsd_s->Socket);
      net_udp_release_socket (bsd_s->Socket);
    }
    else {
      net_tcp_abort (bsd_s->Socket);
      net_tcp_release_socket (bsd_s->Socket);
    }
    bsd_s->Socket = 0;
  }

  /* Release the chain buffers, if any */
  que_free_list (bsd_s);

  /* Resume blocked threads, if any */
  if ((bsd_s->Thread) || (bsd_s->xThread)) {
    /* Thread : accept(), connect(), recv/from() */
    /* xThread: send/to()                        */
    bsd_evt_send (bsd_s, BSD_EVT_KILL);
  }
  /* Clear socket control block */
  memset (bsd_s, 0, sizeof (*bsd_s));
  /* Return events were cleared too */
  bsd_s->Event  = BSD_EVT_KILL;
  bsd_s->xEvent = BSD_EVT_KILL;
}

/**
  \brief       Open native UDP socket for communication.
  \param[in]   bsd_s  pointer to a socket control block.
*/
static void bsd_open_udp (NET_BSD_INFO *bsd_s) {
  if (bsd_s->Flags & BSD_FLAG_OPEN) {
    /* UDP socket already open */
    return;
  }
  bsd_s->Flags |= BSD_FLAG_OPEN;
  net_udp_open (bsd_s->Socket, bsd_s->LocHost.port);
  if (bsd_s->LocHost.port == 0) {
    /* Read assigned ephemeral port */
    bsd_s->LocHost.port = net_udp_get_local_port (bsd_s->Socket);
  }
}

/**
  \brief       Copy network address and convert to a socket address.
  \param[in]   net_addr  source network address.
  \param[in]   family    socket address family.
  \param[out]  addr      destination socket address.
  \param[in,out] addrlen length of SOCKADDR structure.
  \return      status:
               - true  = success,
               - false = failure.
*/
static bool addr_net_to_bsd (const __ADDR *net_addr,
                             uint8_t family, SOCKADDR *addr, int32_t *addrlen) {
  if ((addr == NULL) || (addrlen == NULL)) {
    return (false);
  }
  if (family == AF_INET) {
    if (*addrlen >= (int32_t)sizeof (SOCKADDR_IN)) {
      if (net_addr->addr_type == NET_ADDR_IP4) {
        *addrlen = (int32_t)sizeof (SOCKADDR_IN);
        SOCKADDR4(addr)->sin_family = AF_INET;
        SOCKADDR4(addr)->sin_port   = htons(net_addr->port);
        net_addr4_copy (&SOCKADDR4(addr)->sin_addr.s_b1, net_addr->addr);
        return (true);
      }
    }
  }
#ifdef Network_IPv6
  else {
    if (*addrlen >= (int32_t)sizeof (SOCKADDR_IN6)) {
      *addrlen = (int32_t)sizeof (SOCKADDR_IN6);
      SOCKADDR6(addr)->sin6_family = AF_INET6;
      SOCKADDR6(addr)->sin6_port   = htons(net_addr->port);
      if ((net_addr->addr_type == NET_ADDR_IP4)) {
        /* Write IPv4-mapped IPv6 address */
        net_addr6_map_4to6 (net_addr->addr, SOCKADDR6(addr)->sin6_addr.s6_b);
        return (true);
      }
      /* Write native IPv6 address */
      net_addr6_copy (SOCKADDR6(addr)->sin6_addr.s6_b, net_addr->addr);
      return (true);
    }
  }
#endif
  return (false);
}

/**
  \brief       Check if socket address is valid.
  \param[in]   family   socket address family.
  \param[in]   addr     socket address to check.
  \param[in]   addrlen  length of address structure.
  \return      status:
               - true  = address is valid,
               - false = error.
  \note        Unspecified address is not checked.
*/
static bool is_addr_valid (uint8_t family, const SOCKADDR *addr, int32_t addrlen) {
  if (addr == NULL) {
    return (false);
  }
  /* Check if address length is ok */
  switch (addr->sa_family) {
    case AF_INET:
      if (addrlen == (int32_t)sizeof(SOCKADDR_IN)) {
        break;
      }
      return (false);
#ifdef Network_IPv6
    case AF_INET6:
      if (addrlen == (int32_t)sizeof(SOCKADDR_IN6)) {
        break;
      }
      return (false);
#endif
  }
  /* Check if address family match */
  if (addr->sa_family != family) {
    return (false);
  }
  return (true);
}

/**
  \brief       Check if socket address is unspecified address.
  \param[in]   addr  socket address to check.
  \return      check result:
               - true  = unspecified address,
               - false = valid address.
*/
static bool is_addr_unspec (const SOCKADDR *addr) {
  switch (addr->sa_family) {
    case AF_INET:
      if ((SOCKADDR4(addr)->sin_addr.s_addr == 0) ||
          (SOCKADDR4(addr)->sin_port == 0))       {
        return (true);
      }
      break;
#ifdef Network_IPv6
    case AF_INET6:
      if (net_addr6_is_unspec (SOCKADDR6(addr)->sin6_addr.s6_b) ||
          (SOCKADDR6(addr)->sin6_port == 0))      {
        return (true);
      }
      break;
#endif
  }
  return (false);
}

/**
  \brief       Check if another socket is already using this port.
  \param[in]   bsd_s  pointer to a socket control block.
  \param[in]   port   port number to check.
  \return      status:
               - true  = port in-use,
               - false = port free.
*/
static bool is_port_inuse (NET_BSD_INFO *bsd_s, uint16_t port) {
  NET_BSD_INFO *tst_s;
  int32_t i;

  for (i = 0, tst_s = &bsd->Scb[0]; i < bsd->NumSocks; tst_s++, i++) {
    if (tst_s->State < BSD_STATE_BOUND) {
      continue;
    }
    if ((tst_s->LocHost.port == port) && (tst_s->Type == bsd_s->Type)) {
      return (true);
    }
  }
  return (false);
}

/**
  \brief       Copy and convert socket address to network address.
  \param[in]   addr      source socket address.
  \param[out]  net_addr  destination network address.
*/
static void addr_bsd_to_net (const SOCKADDR *addr, __ADDR *net_addr) {
  /* IP address should be in net byte order */
  if (addr->sa_family == AF_INET) {
    net_addr->addr_type = NET_ADDR_IP4;
    net_addr->port      = ntohs(SOCKADDR4(addr)->sin_port);
    net_addr4_copy (net_addr->addr, &SOCKADDR4(addr)->sin_addr.s_b1);
  }
#ifdef Network_IPv6
  else {
    if (net_addr6_is_4to6 (SOCKADDR6(addr)->sin6_addr.s6_b)) {
      /* IPv4-mapped IPv6 address */
      net_addr->addr_type = NET_ADDR_IP4;
      net_addr->port      = ntohs(SOCKADDR6(addr)->sin6_port);
      net_addr4_copy (net_addr->addr, &SOCKADDR6(addr)->sin6_addr.s6_b[12]);
      return;
    }
    net_addr->addr_type = NET_ADDR_IP6;
    net_addr->port      = ntohs(SOCKADDR6(addr)->sin6_port);
    net_addr6_copy (net_addr->addr, SOCKADDR6(addr)->sin6_addr.s6_b);
  }
#endif
}

/**
  \brief       Get port from the socket address.
  \param[in]   addr  socket address.
  \return      port number.
*/
static uint16_t addr_get_port (const SOCKADDR *addr) {
#ifdef Network_IPv6
  if (addr->sa_family == AF_INET) {
    return (ntohs(SOCKADDR4(addr)->sin_port));
  }
  else {
    return (ntohs(SOCKADDR6(addr)->sin6_port));
  }
#else 
  return (ntohs(SOCKADDR4(addr)->sin_port));
#endif
}

/**
  \brief       Set native TCP socket attributes.
  \param[in]   bsd_s  pointer to a socket control block.
*/
static void set_sock_type (NET_BSD_INFO *bsd_s) {
  uint32_t ip_opt;
  uint8_t  type = 0x00;

  if (bsd_s->Flags & BSD_FLAG_DACK) {
    type |= TCP_TYPE_DELAY_ACK;
  }
  if (bsd_s->Flags & BSD_FLAG_KEEP) {
    type |= TCP_TYPE_KEEP_ALIVE;
  }
  if (bsd_s->Flags & BSD_FLAG_FLOW) {
    type |= TCP_TYPE_FLOW_CTRL;
  }
  ip_opt = (uint32_t)bsd_s->TClass << 8 | bsd_s->Tos;
  net_tcp_ctrl_opt (bsd_s->Socket, type, ip_opt);
}

/**
  \brief       Suspend running thread and wait for BSD event.
  \param[in]   bsd_s  pointer to a socket control block.
  \param[in]   evt    event(s) to wait for.
  \param[in]   tout   waiting time (0= indefinite).
  \return      received wake-up event(s).
  \details     Returns event BSD_EVT_TIMEOUT on timeout.
*/
static uint8_t bsd_evt_wait (NET_BSD_INFO *bsd_s, uint8_t evt, uint16_t tout) {
  evt |= BSD_EVT_KILL;
  if (evt & BSD_EVT_SEND) {
    bsd_s->xEvent = evt;
    bsd_s->xTimer = tout & 0xFF;
    net_sys_suspend (&bsd_s->xThread);
    evt = bsd_s->xEvent;
  }
  else {
    bsd_s->Event = evt;
    bsd_s->Timer = tout;
    net_sys_suspend (&bsd_s->Thread);
    evt = bsd_s->Event;
  }
  return (evt);
}

/**
  \brief       Send BSD event and resume suspended thread(s).
  \param[in]   bsd_s  pointer to a socket control block.
  \param[in]   evt    generated event(s).
*/
static void bsd_evt_send (NET_BSD_INFO *bsd_s, uint8_t evt) {
  if (bsd_s->Event & evt) {
    /* Socket owner thread */
    bsd_s->Timer = 0;
    bsd_s->Event = evt;
    net_sys_resume (&bsd_s->Thread);
  }
  if (bsd_s->xEvent & evt) {
    /* Socket sender thread */
    bsd_s->xTimer = 0;
    bsd_s->xEvent = evt;
    net_sys_resume (&bsd_s->xThread);
  }
  /* Used for mbed-client events */
  net_bsd_notify (bsd_s->Id, evt);
}

#ifdef Network_Debug_STDIO
/**
  \brief       Convert TCP event to ascii.
  \param[in]   evt  TCP event.
  \return      pointer to text representation of an event.
*/
static const char *evt_ascii (netTCP_Event evt) {
  switch (evt) {
    case netTCP_EventConnect:
      return ("Connect");
    case netTCP_EventEstablished:
      return ("Established");
    case netTCP_EventClosed:
      return ("Closed");
    case netTCP_EventAborted:
      return ("Aborted");
    case netTCP_EventACK:
      return ("ACK");
    case netTCP_EventData:
      return ("Data");
  }
  return ("");
}

/**
  \brief       Debug print IP address and port.
  \param[in]   addr  structure containing IP address and port.
*/
static void debug_info (const __ADDR *addr) {
  DEBUGF (BSD," Address [%s], port %d\n",net_addr_ntoa(addr),addr->port);
}
#endif

/* Library default function(s) */
__WEAK void net_bsd_notify (int32_t sock, uint8_t evt) {
  (void)sock; (void)evt; }
