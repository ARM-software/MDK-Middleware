/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_BSD.h
 * Purpose: BSD Sockets validation header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_BSD_H_
#define MW_CV_BSD_H_

#include "rl_net.h"

// Network library
extern const uint32_t net_lib_version;
extern int32_t net_strcpy (char *dp, const char *sp);
extern int32_t net_atoi (const char *sp, int32_t *len);
extern int32_t net_itoa (char *dp, int32_t val);

// Test functions
typedef struct {
  int32_t   (*socket)        (int32_t family, int32_t type, int32_t protocol);
  int32_t   (*bind)          (int32_t sock, const SOCKADDR *addr, int32_t addrlen);
  int32_t   (*listen)        (int32_t sock, int32_t backlog);
  int32_t   (*accept)        (int32_t sock, SOCKADDR *addr, int32_t *addrlen);
  int32_t   (*connect)       (int32_t sock, const SOCKADDR *addr, int32_t addrlen);
  int32_t   (*recv)          (int32_t sock, char *buf, int32_t len, int32_t flags);
  int32_t   (*recvfrom)      (int32_t sock, char *buf, int32_t len, int32_t flags, SOCKADDR *from, int32_t *fromlen);
  int32_t   (*recvmsg)       (int32_t sock, MSGHDR *msg, int32_t flags);
  int32_t   (*send)          (int32_t sock, const char *buf, int32_t len, int32_t flags);
  int32_t   (*sendto)        (int32_t sock, const char *buf, int32_t len, int32_t flags, const SOCKADDR *to, int32_t tolen);
  int32_t   (*sendmsg)       (int32_t sock, const MSGHDR *msg, int32_t flags);
  int32_t   (*getsockname)   (int32_t sock, SOCKADDR *name, int32_t *namelen);
  int32_t   (*getpeername)   (int32_t sock, SOCKADDR *name, int32_t *namelen);
  int32_t   (*getsockopt)    (int32_t sock, int32_t level, int32_t optname, char *optval, int32_t *optlen);
  int32_t   (*setsockopt)    (int32_t sock, int32_t level, int32_t optname, const char *optval, int32_t optlen);
  int32_t   (*closesocket)   (int32_t sock);
  int32_t   (*ioctlsocket)   (int32_t sock, long cmd, unsigned long *argp);
#if (MW_NET_VERSION_MAJOR > 8) || (MW_NET_VERSION_MINOR > 0) || (MW_NET_VERSION_PATCH > 0)
  int32_t   (*select)        (int32_t nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, const  timeval *timeout);
#else
  int32_t   (*select)        (int32_t nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout);
#endif
  HOSTENT  *(*gethostbyname) (const char *name, int32_t *err);
  netStatus (*ping)          (const char *target, uint32_t flags);
} const DRIVER_BSD;

// Test cases
extern void BSD_Initialize (void);
extern void BSD_Uninitialize (void);
extern uint32_t BSD_GetVersion (void);

extern void BSD_SocketCreate (void);
extern void BSD_SocketBind (void);
extern void BSD_SocketListen (void);
extern void BSD_SocketAccept (void);
extern void BSD_SocketAccept_nbio (void);
extern void BSD_SocketConnect (void);
extern void BSD_SocketConnect_nbio (void);
extern void BSD_SocketRecv (void);
extern void BSD_SocketRecv_nbio (void);
extern void BSD_SocketRecvFrom (void);
extern void BSD_SocketRecvFrom_nbio (void);
extern void BSD_SocketRecvMsg (void);
extern void BSD_SocketSend (void);
extern void BSD_SocketSendTo (void);
extern void BSD_SocketSendMsg (void);
extern void BSD_SocketGetSockName (void);
extern void BSD_SocketGetPeerName (void);
extern void BSD_SocketGetOpt (void);
extern void BSD_SocketSetOpt (void);
extern void BSD_SocketClose (void);
extern void BSD_SocketIoctl (void);
extern void BSD_SocketSelect (void);
extern void BSD_GetHostByName (void);
extern void BSD_PingEcho (void);

extern void BSD_Transfer_Fixed (void);
extern void BSD_Transfer_Incremental (void);
extern void BSD_Send_Fragmented (void);
extern void BSD_Recv_Fragmented (void);
extern void BSD_Test_Speed (void);
extern void BSD_Concurrent_Socket (void);
extern void BSD_Downstream_Rate (void);
extern void BSD_Upstream_Rate (void);

#endif /* MW_CV_BSD_H_ */
