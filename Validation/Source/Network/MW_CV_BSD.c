/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_BSD.c
 * Purpose: BSD Sockets validation tests
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MW_CV_Config.h"

#include "cmsis_os2.h"

#include "MW_CV_TestReport.h"

#include "MW_CV_BSD.h"
#include "Net_Config_BSD.h"
#include "rl_net.h"

/* Timeouts (in ms) */
#define BSD_TIMEOUT         800
#define BSD_TIMEOUT_LONG    5000
#define BSD_TIMEOUT_CONNECT 15500

#define GET_SYSTICK()   	osKernelGetTickCount()
#define SYSTICK_MS(ms)  	(((uint32_t)ms * osKernelGetTickFreq()) / 1000U)

/* Service ports */
#define ECHO_PORT           7               // Echo port number
#define DISCARD_PORT        9               // Discard port number
#define CHARGEN_PORT        19              // Chargen port number
#define ASSISTANT_PORT      5000            // Test Assistant port number
#define TCP_REJECTED_PORT   5001            // Rejected connection server TCP port
#define TCP_TIMEOUT_PORT    5002            // Non-responding server TCP port

/* Helper function identifiers */
#define F_CREATE            0x00000001
#define F_CREATE_TCP        F_CREATE        // Never used with CREATE
#define F_CREATE_UDP        0x00000002
#define F_CLOSE             0x00000004
#define F_BIND              0x00000008
#define F_LISTEN            0x00000010
#define F_ACCEPT            0x00000020
#define F_CONNECT           0x00000040
#define F_RECV              0x00000080
#define F_RECVFROM          0x00000100
#define F_SEND              0x00000200
#define F_SENDTO            0x00000400
#define F_GETSOCKNAME       0x00000800
#define F_GETPEERNAME       0x00001000
#define F_GETOPT            0x00002000
#define F_SETOPT            0x00004000
#define F_GETHOSTBYNAME     0x00008000
#define F_PING              0x00010000
#define F_SEND_CTRL         F_PING          // Never used with PING
#define F_RECVMSG           0x00020000
#define F_SENDMSG           0x00040000
#define F_IOCTL             0x00080000
#define F_SELECT            0x00100000
#define F_XFER_FIXED        0x00200000
#define F_XFER_INCR         0x00400000
#define F_SEND_FRAG         0x00800000
#define F_UPLOAD            F_SEND_FRAG     // Never used with SEND_FRAG
#define F_RECV_FRAG         0x01000000
#define F_DOWNLOAD          F_RECV_FRAG     // Never used with RECV_FRAG
#define F_ALL               0x01FFFFFF

/* Helper function return values */
#define TH_OK               0x01
#define TH_TOUT             0x02
#define TH_ALL              0x03

/* Concurrent thread flags */
#define X_STOP              0x01
#define TH_IDLE             0x80

/* Speed test check value in bytes/sec (0= no rate validation) */
#ifndef MIN_BYTES
#define MIN_BYTES           0
#endif

/* Socket driver functions */
static DRIVER_BSD BSD_Driver0 = {
  socket,
  bind,
  listen,
  accept,
  connect,
  recv,
  recvfrom,
  recvmsg,
  send,
  sendto,
  sendmsg,
  getsockname,
  getpeername,
  getsockopt,
  setsockopt,
  closesocket,
  ioctlsocket,
  select,
  gethostbyname,
  netPing_EchoX
};
static DRIVER_BSD *bsd;

static const uint8_t ip_unspec[4] = { 0, 0, 0, 0 };
static const uint8_t ip_bcast[4]  = { 255, 255, 255, 255 };
static uint8_t       ip_server[4];

/* Test message containing all letters of the alphabet */
static const char test_msg[44] = {
  "The quick brown fox jumps over the lazy dog."
};
/* Dummy text with normal distribution of letters */
static const char test_buf[2050] = {
  "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer nec odio. Praesent "
  "libero. Sed cursus ante dapibus diam. Sed nisi. Nulla quis sem at nibh elementum "
  "imperdiet. Duis sagittis ipsum. Praesent mauris. Fusce nec tellus sed augue semper "
  "porta. Mauris massa. Vestibulum lacinia arcu eget nulla. Class aptent taciti sociosqu "
  "ad litora torquent per conubia nostra, per inceptos himenaeos."

  "Curabitur sodales ligula in libero. Sed dignissim lacinia nunc. Curabitur tortor. "
  "Pellentesque nibh. Aenean quam. In scelerisque sem at dolor. Maecenas mattis. Sed "
  "convallis tristique sem. Proin ut ligula vel nunc egestas porttitor. Morbi lectus "
  "risus, iaculis vel, suscipit quis, luctus non, massa. Fusce ac turpis quis ligula "
  "lacinia aliquet. Mauris ipsum. Nulla metus metus, ullamcorper vel, tincidunt sed, "
  "euismod in, nibh."

  "Quisque volutpat condimentum velit. Class aptent taciti sociosqu ad litora torquent "
  "per conubia nostra, per inceptos himenaeos. Nam nec ante. Sed lacinia, urna non "
  "tincidunt mattis, tortor neque adipiscing diam, a cursus ipsum ante quis turpis. "
  "Nulla facilisi. Ut fringilla. Suspendisse potenti. Nunc feugiat mi a tellus consequat "
  "imperdiet. Vestibulum sapien. Proin quam. Etiam ultrices. Suspendisse in justo eu "
  "magna luctus suscipit. Sed lectus. Integer euismod lacus luctus magna."

  "Quisque cursus, metus vitae pharetra auctor, sem massa mattis sem, at interdum magna "
  "augue eget diam. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices "
  "posuere cubilia Curae; Morbi lacinia molestie dui. Praesent blandit dolor. Sed non "
  "quam. In vel mi sit amet augue congue elementum. Morbi in ipsum sit amet pede facilisis "
  "laoreet. Donec lacus nunc, viverra nec, blandit vel, egestas et, augue. Vestibulum "
  "tincidunt malesuada tellus. Ut ultrices ultrices enim. Curabitur sit amet mauris. "
  "Morbi in dui quis est pulvinar ullamcorper. Nulla facilisi. Integer lacinia sollicitudin "
  "massa. Cras metus."

  "Sed aliquet risus a tortor. Integer id quam. Morbi mi. Quisque nisl felis, venenatis "
  "tristique, dignissim in, ultrices sit amet augue."
};
static char buffer[2048];

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: BSD_Initialize
\details
Dummy test case used to initialize the network stack.
*/
void BSD_Initialize (void) {
  SOCKADDR_IN sa;
  uint32_t addr,ticks,tout;
  int32_t sock,rv;

  netInitialize ();
  bsd = NULL;
  rv  = netIP_aton (MW_CV_NET_SOCKSERVER_IP, NET_ADDR_IP4, ip_server);
  ASSERT_TRUE (rv, "SockServer IP address configuration error");
  if (rv == 0) return;

  /* Wait for DHCP address lease */
  tout  = SYSTICK_MS(6000);
  ticks = GET_SYSTICK();
  do {
    osDelay(10);
    netIF_GetOption(NET_IF_CLASS_ETH | 0,
                    netIF_OptionIP4_Address,
                    (uint8_t *)&addr, sizeof (addr));
    if (addr != 0U) {
      bsd = &BSD_Driver0;
      break;
    }
  } while (GET_SYSTICK() - ticks < tout);
  ASSERT_TRUE (addr, "No network, check ethernet cable");
  if (!bsd)    return;

  /* Check if SockServer is accessible */
  sock = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  sa.sin_family    = AF_INET;
  sa.sin_port      = htons(ECHO_PORT);
  memcpy(&sa.sin_addr, ip_server, 4);
  bsd->sendto (sock, "Hello", 5, 0, (SOCKADDR *)&sa, sizeof (sa));

  tout  = SYSTICK_MS(1000);
  ticks = GET_SYSTICK();
  do {
    osDelay (10);
    rv = bsd->recv (sock, buffer, sizeof(buffer), MSG_DONTWAIT);
    if (rv > 0) break;
  } while (GET_SYSTICK() - ticks < tout);
  bsd->closesocket (sock);
  ASSERT_TRUE (rv == 5, "No connection to SockServer");
  if (rv != 5) bsd = NULL;
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief Test case: BSD_Uninitialize
\details
Dummy test case used to de-initialize the network stack.
*/
void BSD_Uninitialize (void) {

  netUninitialize ();

  ASSERT_TRUE (1, "");
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/* BSD Socket tests */
/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/**
\defgroup bsd_sock_api BSD Socket API
\details
These tests verify API and operation of the BSD socket functions.
*/

/* Helper function for execution of socket test function in the worker thread */
static int32_t th_execute (osThreadId_t *id, uint32_t sig, uint32_t tout) {
  osThreadFlagsSet (id, sig);
  if (osThreadFlagsWait (TH_OK | TH_TOUT, osFlagsWaitAny, tout) == TH_OK) {
    /* Success, completed in time */
    return (1);
  }
  /* Function timeout expired */
  return (0);
}

#define TH_EXECUTE(sig,tout)                               \
  do {                                                     \
    io.xid++;                                              \
    rval = th_execute (worker, sig, tout);                 \
    ASSERT_TRUE (rval, "Execution timeout (%d ms)", tout); \
  } while (0)

#define TH_ASSERT(cond)                             \
  do {                                              \
    if (rval) ASSERT_TRUE (cond, "");               \
  } while (0)

#define ARG_INIT()                                  \
  do {                                              \
    io.owner = osThreadGetId ();                    \
    io.xid   = 0;                                   \
  } while (0)

#define SA_SET(_family,_addr,_port)                 \
  do {                                              \
    sa.sin_family    = _family;                     \
    sa.sin_port      = htons(_port);                \
    memcpy(&sa.sin_addr, _addr, 4);                 \
  } while (0)

/* Get version helper function */
uint32_t BSD_GetVersion (void) {
  int32_t major,minor,patch;

  major =  net_lib_version >> 24;
  minor = (net_lib_version >> 16) & 0xff;
  patch = (net_lib_version & 0xFFFF);

  return (major * 10000000U + minor * 10000U + patch);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* Create IO parameters */
typedef struct {
  int32_t      sock;
  int32_t      family;
  int32_t      type;
  int32_t      protocol;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
} IO_CREATE;

/* Assign arguments */
#define ARG_CREATE(_family,_type,_proto) \
  do {                                   \
    io.family   = _family;               \
    io.type     = _type;                 \
    io.protocol = _proto;                \
  } while (0)

/* Create worker thread */
__NO_RETURN static void Th_Create (IO_CREATE *io) {
  uint32_t flags, xid;

  for (;;) {
    flags = osThreadFlagsWait (F_CREATE | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE:
        /* Create socket */
        io->rc = bsd->socket (io->family, io->type, io->protocol);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketCreate
\details
The test case \b BSD_SocketCreate verifies the BSD \b socket function:
\code
int32_t (*socket) (int32_t family, int32_t type, int32_t protocol);
\endcode

Create socket test:
 - Check function parameters
 - Create multiple stream sockets
 - Gradually close stream sockets and create datagram sockets
 - Close datagram sockets
*/
void BSD_SocketCreate (void) {
  osThreadId_t worker;
  int32_t      rval;
  IO_CREATE    io;
  int32_t      sock[BSD_NUM_SOCKS], i;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Create, &io, NULL);
  ASSERT_TRUE (worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Check parameter (family = -1) */
  ARG_CREATE (-1, SOCK_STREAM, IPPROTO_TCP);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter (family = INT32_MIN) */
  ARG_CREATE (INT32_MIN, SOCK_STREAM, IPPROTO_TCP);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter (family = INT32_MAX) */
  ARG_CREATE (INT32_MAX, SOCK_STREAM, IPPROTO_TCP);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter (type = -1) */
  ARG_CREATE (AF_INET, -1, IPPROTO_TCP);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter (type = INT32_MIN) */
  ARG_CREATE (AF_INET, INT32_MIN, IPPROTO_TCP);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter (type = INT32_MAX) */
  ARG_CREATE (AF_INET, INT32_MAX, IPPROTO_TCP);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter, stream socket (protocol = -1) */
  ARG_CREATE (AF_INET, SOCK_STREAM, -1);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter, stream socket (protocol = INT32_MIN) */
  ARG_CREATE (AF_INET, SOCK_STREAM, INT32_MIN);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter, stream socket (protocol = INT32_MAX) */
  ARG_CREATE (AF_INET, SOCK_STREAM, INT32_MAX);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter, datagram socket (protocol = -1) */
  ARG_CREATE (AF_INET, SOCK_DGRAM, -1);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter, datagram socket (protocol = INT32_MIN) */
  ARG_CREATE (AF_INET, SOCK_DGRAM, INT32_MIN);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter, datagram socket (protocol = INT32_MAX) */
  ARG_CREATE (AF_INET, SOCK_DGRAM, INT32_MAX);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter, stream socket (protocol = IPPROTO_UDP) */
  ARG_CREATE (AF_INET, SOCK_STREAM, IPPROTO_UDP);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter, datagram socket (protocol = IPPROTO_TCP) */
  ARG_CREATE (AF_INET, SOCK_DGRAM, IPPROTO_TCP);
  TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Create multiple stream sockets */
  ARG_CREATE (AF_INET, SOCK_STREAM, IPPROTO_TCP);
  for (i = 0; i < BSD_NUM_SOCKS; i++) {
    TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc >= 0);
    sock[i] = io.rc;
  }
  osDelay (10);

  /* Gradually close stream sockets, create datagram sockets */
  ARG_CREATE (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  for (i = 0; i < BSD_NUM_SOCKS; i++) {
    /* Close stream socket */
    io.sock = sock[i];
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Create datagram socket */
    TH_EXECUTE (F_CREATE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc >= 0);
    sock[i] = io.rc;
  }
  osDelay (10);

  /* Close datagram sockets */
  for (i = 0; i < BSD_NUM_SOCKS; i++) {
    io.sock = sock[i];
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
  }
  osDelay (10);

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* Bind IO parameters */
typedef struct {
  int32_t         sock;
  const SOCKADDR *addr;
  int32_t         addrlen;
  int32_t         rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
} IO_BIND;

/* Assign arguments */
#define ARG_BIND(_sock,_addr,_addrlen)    \
  do {                                    \
    io.sock    = _sock;                   \
    io.addr    = (const SOCKADDR *)_addr; \
    io.addrlen = _addrlen;                \
  } while (0)

/* Bind worker thread */
__NO_RETURN static void Th_Bind (IO_BIND *io) {
  uint32_t flags,xid;

  for (;;) {
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP | F_BIND | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_BIND:
        /* Bind socket */
        io->rc = bsd->bind (io->sock, io->addr, io->addrlen);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketBind
\details
The test case \b BSD_SocketBind verifies the BSD \b bind function:
\code
int32_t (*bind) (int32_t sock, const SOCKADDR *addr, int32_t addrlen);
\endcode

Stream socket test:
 - Create stream socket
 - Check function parameters
 - Bind stream socket
 - Bind socket second time
 - Create 2nd stream socket
 - Bind 2nd socket, used port
 - Bind 2nd socket, unused port
 - Close stream sockets
 - Bind closed socket

Datagram socket test:
 - Create datagram socket
 - Bind datagram socket
 - Bind socket second time
 - Create 2nd datagram socket
 - Bind 2nd socket, used port
 - Bind 2nd socket, unused port
 - Close datagram socket
 - Bind closed socket
*/
void BSD_SocketBind (void) {
  SOCKADDR_IN  sa;
  osThreadId_t worker;
  int32_t      rval;
  IO_BIND      io;
  int32_t      sock,sock2;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Bind, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Check parameter (socket = -1) */
    SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
    ARG_BIND   (-1, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_BIND (INT32_MIN, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_BIND (INT32_MAX, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (addr = NULL) */
    ARG_BIND (sock, NULL, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = 0) */
    ARG_BIND (sock, &sa, 0);
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = INT32_MAX) */
    ARG_BIND (sock, &sa, INT32_MAX);
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = INT32_MIN) */
    ARG_BIND (sock, &sa, INT32_MIN);
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (family = AF_UNSPEC) */
    SA_SET (AF_UNSPEC, ip_unspec, DISCARD_PORT);
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (family = AF_NETBIOS) */
    SA_SET (AF_NETBIOS, ip_unspec, DISCARD_PORT);
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (family = INT16_MIN) */
    SA_SET (INT16_MIN, ip_unspec, DISCARD_PORT);
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (family = INT16_MAX) */
    SA_SET (INT16_MAX, ip_unspec, DISCARD_PORT);
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Bind socket */
    SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
    ARG_BIND (sock, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Bind socket 2nd time */
    ARG_BIND (sock, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    /* Should return error (socket already bound) */
    TH_ASSERT (io.rc == BSD_EINVAL);

    /* Create 2nd stream socket */
    TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
    ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
    sock2 = io.rc;

    /* Bind 2nd socket, used port */
    SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
    ARG_BIND (sock2, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    /* Should return error (address already used) */
    TH_ASSERT (io.rc == BSD_EADDRINUSE);

    /* Bind 2nd socket, unused port */
    SA_SET (AF_INET, ip_unspec, ECHO_PORT);
    ARG_BIND (sock2, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Close sockets */
    io.sock = sock2;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Bind again, closed socket */
    ARG_BIND (sock, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Check parameter (socket = -1) */
    SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
    ARG_BIND   (-1, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_BIND   (INT32_MIN, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_BIND   (INT32_MAX, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (addr = NULL) */
    ARG_BIND (sock, NULL, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = 0) */
    ARG_BIND (sock, &sa, 0);
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = INT32_MAX) */
    ARG_BIND (sock, &sa, INT32_MAX);
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = INT32_MIN) */
    ARG_BIND (sock, &sa, INT32_MIN);
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Bind socket */
    SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
    ARG_BIND (sock, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Bind socket 2nd time */
    ARG_BIND (sock, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    /* Should return error (socket already bound) */
    TH_ASSERT (io.rc == BSD_EINVAL);

    /* Create 2nd datagram socket */
    TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
    ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
    sock2 = io.rc;

    /* Bind 2nd socket, used port */
    ARG_BIND (sock2, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    /* Should return error (address already used) */
    TH_ASSERT (io.rc == BSD_EADDRINUSE);

    /* Bind 2nd socket, unused port */
    SA_SET (AF_INET, ip_unspec, ECHO_PORT);
    ARG_BIND (sock2, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Close sockets */
    io.sock = sock2;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Bind again, closed socket */
    ARG_BIND (sock, &sa, sizeof(sa));
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* Listen IO parameters */
typedef struct {
  int32_t      sock;
  int32_t      backlog;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
} IO_LISTEN;

/* Assign arguments */
#define ARG_LISTEN(_sock,_backlog) \
  do {                             \
    io.sock    = _sock;            \
    io.backlog = _backlog;         \
  } while (0)

/* Listen worker thread */
__NO_RETURN static void Th_Listen (IO_LISTEN *io) {
  uint32_t flags,xid;

  for (;;) {
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP |
                               F_BIND       | F_LISTEN     | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_BIND: {
        /* Bind socket */
        SOCKADDR_IN sa;
        SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
        io->rc = bsd->bind (io->sock, (SOCKADDR *)&sa, sizeof(sa));
      } break;

      case F_LISTEN:
        /* Listen on socket */
        io->rc = bsd->listen (io->sock, io->backlog);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketListen
\details
The test case \b BSD_SocketListen verifies the BSD \b listen function:
\code
int32_t (*listen) (int32_t sock, int32_t backlog);
\endcode

Stream socket test 1:
 - Create stream socket
 - Bind socket
 - Check function parameters
 - Start listening
 - Start listening 2nd time
 - Close socket

Stream socket test 2:
 - Create stream socket
 - Start listening, unbound socket
 - Close socket
 - Start listening, closed socket

Datagram socket test:
 - Create datagram socket
 - Bind socket
 - Start listening
 - Close socket
*/
void BSD_SocketListen (void) {
  osThreadId_t worker;
  int32_t      rval;
  IO_LISTEN    io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Listen, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Bind socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    ARG_LISTEN (-1, 1);
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_LISTEN (INT32_MIN, 1);
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_LISTEN (INT32_MAX, 1);
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Start listening */
    ARG_LISTEN (sock, 1);
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Start listening 2nd time */
    ARG_LISTEN (sock, 1);
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    /* Should return error (socket already listening) */
    TH_ASSERT (io.rc == BSD_EINVAL);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Start listening, unbound socket */
    ARG_LISTEN (sock, 1);
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    /* Should return error (socket not bound) */
    TH_ASSERT (io.rc == BSD_EINVAL);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Start listening, closed socket */
    ARG_LISTEN (sock, 1);
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Bind socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Start listening */
    ARG_LISTEN (sock, 1);
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    /* Should return error (operation not supported) */
    TH_ASSERT (io.rc == BSD_ENOTSUP);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* Accept IO parameters */
typedef struct {
  int32_t      sock;
  SOCKADDR    *addr;
  int32_t     *addrlen;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
  const char  *cmd;
  uint32_t     tval;
} IO_ACCEPT;

/* Assign arguments */
#define ARG_ACCEPT(_sock,_addr,_addrlen) \
  do {                                   \
    io.sock    = _sock;                  \
    io.addr    = (SOCKADDR *)_addr;      \
    io.addrlen = _addrlen;               \
  } while (0)

/* TestAssistant control */
#define TEST_PORT           2000
#define TEST_PORT_NB        2001

/* CONNECT <proto>,<ip_addr>,<port>,<delay_ms>
           <proto>    = protocol (TCP, UDP)
           <ip_addr>  = IP address (0.0.0.0 = sender address)
           <port>     = port number
           <delay_ms> = startup delay

  Example: CONNECT TCP,192.168.1.200,80,600
  (wait 600ms then connect to 192.168.1.200, port 80)
*/
#define CMD_CONNECT_TCP     "CONNECT TCP,0.0.0.0,2000,500"
#define CMD_CONNECT_UDP     "CONNECT UDP,0.0.0.0,2000,200"

#define CMD_CONNECT_TCP_NB  "CONNECT TCP,0.0.0.0,2001,500"

/* Accept worker thread */
__NO_RETURN static void Th_Accept (IO_ACCEPT *io) {
  uint32_t flags,xid;

  for (;;) {
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP | F_BIND | F_LISTEN | F_SETOPT |
                               F_ACCEPT     | F_SEND_CTRL  | F_RECV | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_BIND: {
        /* Bind socket */
        SOCKADDR_IN sa;
        SA_SET (AF_INET, ip_unspec, (uint16_t)io->tval);
        io->rc = bsd->bind (io->sock, (SOCKADDR *)&sa, sizeof(sa));
      } break;

      case F_LISTEN:
        /* Listen on socket */
        io->rc = bsd->listen (io->sock, 1);
        break;

      case F_ACCEPT:
        /* Accept on socket */
        io->rc = bsd->accept (io->sock, io->addr, io->addrlen);
        break;

      case F_RECV:
        /* Recv on socket (stream, datagram) */
        memset(buffer, 0xCC, 16);
        io->rc = bsd->recv (io->sock, buffer, 16, 0);
        if ((io->rc > 0) && (memcmp (buffer, "SockServer", 10) != 0)) {
          /* Failed if rc <= 0 */
          io->rc = 0;
        }
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;

      case F_SETOPT: {
        /* Set socket non-blocking mode */
        unsigned long nbio = 1;
        io->rc = bsd->ioctlsocket (io->sock, FIONBIO, &nbio);
      } break;

      case F_SEND_CTRL: {
        /* Send control command to TestAssistant */
        SOCKADDR_IN  sa;
        int32_t sock = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        SA_SET (AF_INET, ip_server, ASSISTANT_PORT);
        bsd->connect (sock, (SOCKADDR *)&sa, sizeof(sa));
        io->rc = bsd->send (sock, io->cmd, strlen(io->cmd), 0);
        bsd->closesocket (sock);
        osDelay (10);
      } break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketAccept
\details
The test case \b BSD_SocketAccept verifies the BSD \b accept function:
\code
int32_t (*accept) (int32_t sock, SOCKADDR *addr, int32_t *addrlen);
\endcode

Stream socket test:
 - Create stream socket
 - Bind socket
 - Start listening
 - Check function parameters
 - Accept connection, NULL parameters
 - Receive ServerId on accepted socket
 - Close accepted socket
 - Accept connection again, return IP address and port
 - Receive ServerId on accepted socket
 - Receive again, server closed connection
 - Close accepted socket
 - Close listening socket
 - Accept again, closed socket

Datagram socket test:
 - Create datagram socket
 - Bind socket
 - Start listening
 - Accept connection, provide return parameters for IP address and port
 - Receive ServerId on socket
 - Close socket
*/
void BSD_SocketAccept (void) {
  SOCKADDR_IN  sa;
  int32_t      salen;
  osThreadId_t worker;
  int32_t      rval;
  IO_ACCEPT    io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Accept, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Bind socket */
    io.sock = sock;
    io.tval = TEST_PORT;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Start listening */
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    ARG_ACCEPT (-1, NULL, NULL);
    TH_EXECUTE (F_ACCEPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_ACCEPT (INT32_MIN, NULL, NULL);
    TH_EXECUTE (F_ACCEPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_ACCEPT (INT32_MAX, NULL, NULL);
    TH_EXECUTE (F_ACCEPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Request a remote server to connect to us */
    io.cmd = CMD_CONNECT_TCP;
    TH_EXECUTE (F_SEND_CTRL, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc > 0);

    /* Accept connection with NULL parameters */
    ARG_ACCEPT (sock, NULL, NULL);
    TH_EXECUTE (F_ACCEPT, BSD_TIMEOUT_LONG);
    /* Accepted socket should be different */
    TH_ASSERT  ((io.rc != io.sock) && (io.rc >= 0));

    /* Receive SockServer id string */
    io.sock = io.rc;
    TH_EXECUTE (F_RECV, BSD_TIMEOUT_LONG);
    TH_ASSERT (io.rc > 0);

    /* Close accepted socket */
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (500);

    /* Request from remote server to connect to us */
    io.cmd = CMD_CONNECT_TCP;
    TH_EXECUTE (F_SEND_CTRL, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc > 0);

    /* Initialize buffers for return values */
    memset (&sa, 0, sizeof(sa));
    salen = sizeof(sa) + 1;

    /* Accept again, return ip address and port */
    ARG_ACCEPT (sock, &sa, &salen);
    TH_EXECUTE (F_ACCEPT, BSD_TIMEOUT_LONG);
    /* Accepted socket should be different */
    TH_ASSERT  ((io.rc != io.sock) && (io.rc >= 0));
    /* IP address should be the address of the server */
    TH_ASSERT  (memcmp (&sa.sin_addr, ip_server, 4) == 0);
    /* Address length should be updated */
    TH_ASSERT (salen == sizeof(sa));
    /* Port number of remote peer should be non-zero */
    TH_ASSERT  (sa.sin_port != 0);

    /* Receive SockServer id string */
    io.sock = io.rc;
    TH_EXECUTE (F_RECV, BSD_TIMEOUT_LONG);
    TH_ASSERT (io.rc > 0);

    /* SockServer disconnects after 500ms */

    /* Receive again, no data */
    TH_EXECUTE (F_RECV, BSD_TIMEOUT_LONG);
    /* Should return error (connection reset) */
    TH_ASSERT (io.rc == BSD_ECONNRESET);

    /* Close accepted socket */
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Close listening socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Accept again, closed socket */
    salen = sizeof(sa);
    ARG_ACCEPT (sock, &sa, &salen);
    TH_EXECUTE (F_ACCEPT, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Bind socket */
    io.sock = sock;
    io.tval = TEST_PORT;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Start listening */
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    /* Listen on datagram socket should fail */
    TH_ASSERT (io.rc == BSD_ENOTSUP);

    /* Initialize buffers for return values */
    memset (&sa, 0, sizeof(sa));
    salen = sizeof(sa) + 1;

    /* Accept on datagram socket */
    ARG_ACCEPT (sock, &sa, &salen);
    TH_EXECUTE (F_ACCEPT, BSD_TIMEOUT);
    /* Accept on datagram socket should fail */
    TH_ASSERT (io.rc == BSD_ENOTSUP);

    osDelay (500);

    /* Request from remote server to send us a test message */
    io.cmd = CMD_CONNECT_UDP;
    SA_SET (AF_INET, ip_server, ASSISTANT_PORT);
    TH_EXECUTE (F_SEND_CTRL, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc > 0);

    /* Receive SockServer id string */
    TH_EXECUTE (F_RECV, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc > 0);

    /* Close socket */
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/**
\brief  Test case: BSD_SocketAccept_nbio
\details
The test case \b BSD_SocketAccept_nbio verifies the BSD \b accept function
running in non-blocking mode.

Stream socket test:
 - Create stream socket
 - Set non-blocking mode
 - Bind socket
 - Start listening
 - Check function parameters
 - Accept connection, NULL parameters
 - Receive ServerId on accepted socket
 - Close accepted socket
 - Accept connection again, return IP address and port
 - Receive ServerId on accepted socket
 - Receive again, server closed connection
 - Close accepted socket
 - Close listening socket
 - Accept again, closed socket
*/
void BSD_SocketAccept_nbio (void) {
  SOCKADDR_IN  sa;
  int32_t      salen;
  uint32_t     ticks,tout;
  osThreadId_t worker;
  int32_t      rval;
  IO_ACCEPT    io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Accept, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Set socket non-blocking */
    io.sock = sock;
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Bind socket */
    io.sock = sock;
    io.tval = TEST_PORT_NB;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Start listening */
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    ARG_ACCEPT (-1, NULL, NULL);
    TH_EXECUTE (F_ACCEPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_ACCEPT (INT32_MIN, NULL, NULL);
    TH_EXECUTE (F_ACCEPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_ACCEPT (INT32_MAX,  NULL, NULL);
    TH_EXECUTE (F_ACCEPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Request a remote server to connect to us */
    io.cmd = CMD_CONNECT_TCP_NB;
    TH_EXECUTE (F_SEND_CTRL, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc > 0);

    /* Accept connection, polling mode */
    tout  = SYSTICK_MS(BSD_TIMEOUT_LONG);
    ticks = GET_SYSTICK();
    do {
      /* Accept connection with NULL parameters */
      ARG_ACCEPT (sock, NULL, NULL);
      TH_EXECUTE (F_ACCEPT, BSD_TIMEOUT);
      if (io.rc != BSD_EWOULDBLOCK) break;
    } while (GET_SYSTICK() - ticks < tout);
    /* Accepted socket should be different */
    TH_ASSERT  ((io.rc != io.sock) && (io.rc >= 0));

    /* Receive SockServer id string, polling mode */
    io.sock = io.rc;
    tout  = SYSTICK_MS(BSD_TIMEOUT_LONG);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_RECV, BSD_TIMEOUT);
      if (io.rc > 0) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT (io.rc > 0);

    /* Close accepted socket, polling mode */
    tout  = SYSTICK_MS(BSD_TIMEOUT);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
      if (io.rc == 0) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT (io.rc == 0);

    osDelay (500);

    /* Request from remote server to connect to us */
    io.cmd = CMD_CONNECT_TCP_NB;
    TH_EXECUTE (F_SEND_CTRL, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc > 0);

    /* Initialize buffers for return values */
    memset (&sa, 0, sizeof(sa));
    salen = sizeof(sa) + 1;

    /* Accept again, polling mode */
    tout  = SYSTICK_MS(BSD_TIMEOUT_LONG);
    ticks = GET_SYSTICK();
    do {
      /* Accept again, return ip address and port */
      ARG_ACCEPT (sock, &sa, &salen);
      TH_EXECUTE (F_ACCEPT, BSD_TIMEOUT);
      if (io.rc != BSD_EWOULDBLOCK) break;
    } while (GET_SYSTICK() - ticks < tout);
    /* Accepted socket should be different */
    TH_ASSERT  ((io.rc != io.sock) && (io.rc >= 0));
    /* IP address should be the address of the server */
    TH_ASSERT  (memcmp (&sa.sin_addr, ip_server, 4) == 0);
    /* Address length should be updated */
    TH_ASSERT (salen == sizeof(sa));
    /* Port number of remote peer should be non-zero */
    TH_ASSERT  (sa.sin_port != 0);

    /* Receive SockServer id string, polling mode */
    io.sock = io.rc;
    tout  = SYSTICK_MS(BSD_TIMEOUT_LONG);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_RECV, BSD_TIMEOUT);
      if (io.rc > 0) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT (io.rc > 0);

    /* SockServer disconnects after 500ms */
    osDelay (1000);

    /* Receive again, no data */
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    /* Should return error (connection reset) */
    TH_ASSERT (io.rc == BSD_ECONNRESET);

    /* Close accepted socket, polling mode */
    tout  = SYSTICK_MS(BSD_TIMEOUT);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
      if (io.rc == 0) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT  (io.rc == 0);

    /* Close listening socket, polling mode  */
    io.sock = sock;
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
      if (io.rc == 0) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT  (io.rc == 0);

    /* Accept again, closed socket */
    salen = sizeof(sa);
    ARG_ACCEPT (sock, &sa, &salen);
    TH_EXECUTE (F_ACCEPT, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* Connect IO parameters */
typedef struct {
  int32_t         sock;
  const SOCKADDR *addr;
  int32_t         addrlen;
  int32_t         rc;
  /* Control */
  osThreadId_t    owner;
  uint32_t        xid;
} IO_CONNECT;

/* Assign arguments */
#define ARG_CONNECT(_sock,_addr,_addrlen) \
  do {                                    \
    io.sock    = _sock;                   \
    io.addr    = (const SOCKADDR *)_addr; \
    io.addrlen = _addrlen;                \
  } while (0)

/* Connect worker thread */
__NO_RETURN static void Th_Connect (IO_CONNECT *io) {
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP | F_BIND | F_SETOPT |
                               F_CONNECT    | F_LISTEN     | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_BIND: {
        /* Bind socket */
        SOCKADDR_IN sa;
        SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
        io->rc = bsd->bind (io->sock, (SOCKADDR *)&sa, sizeof(sa));
      } break;

      case F_CONNECT:
        /* Connect on socket */
        io->rc = bsd->connect (io->sock, io->addr, io->addrlen);
        break;

      case F_LISTEN:
        /* Listen on socket */
        io->rc = bsd->listen (io->sock, 1);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;

      case F_SETOPT: {
        /* Set socket non-blocking mode */
        unsigned long nbio = 1;
        io->rc = bsd->ioctlsocket (io->sock, FIONBIO, &nbio);
      } break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketConnect
\details
The test case \b BSD_SocketConnect verifies the BSD \b connect function:
\code
int32_t (*connect) (int32_t sock, const SOCKADDR *addr, int32_t addrlen);
\endcode

Stream socket test 1:
 - Create stream socket
 - Check function parameters
 - Connect to server, blocking mode
 - Connect again, already connected
 - Bind connected socket
 - Close socket
 - Connect on closed socket

Stream socket test 2:
 - Create stream socket
 - Connect to server, connection rejected
 - Close socket

Stream socket test 3:
 - Create stream socket
 - Connect to server, non-responding or non-existent
 - Close socket

Stream socket test 4:
 - Create stream socket
 - Bind socket
 - Start listening
 - Connect to server, blocking mode
 - Close socket

Datagram socket test:
 - Create datagram socket
 - Bind socket
 - Check function parameters
 - Connect to server, enable address filtering
 - Connect to unspecified address, disable filtering
 - Close socket
 - Connect again, closed socket
*/
void BSD_SocketConnect (void) {
  SOCKADDR_IN  sa;
  osThreadId_t worker;
  int32_t      rval;
  IO_CONNECT   io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Connect, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Check parameter (socket = -1) */
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    ARG_CONNECT(-1, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_CONNECT(INT32_MIN, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_CONNECT(INT32_MAX, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (addr = NULL) */
    ARG_CONNECT(sock, NULL, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (ip = 0.0.0.0) */
    SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = 0) */
    ARG_CONNECT(sock, &sa, 0);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = 17) */
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    ARG_CONNECT(sock, &sa, sizeof(sa) + 1);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (port = 0) */
    SA_SET (AF_INET, ip_server, 0);
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (family = UNSPEC) */
    SA_SET (AF_UNSPEC, ip_unspec, DISCARD_PORT);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (family = NETBIOS) */
    SA_SET (AF_NETBIOS, ip_unspec, DISCARD_PORT);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (family = INT16_MIN) */
    SA_SET (INT16_MIN, ip_unspec, DISCARD_PORT);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (family = INT16_MAX) */
    SA_SET (INT16_MAX, ip_unspec, DISCARD_PORT);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Connect to stream server */
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Connect 2nd time */
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    /* Should return error (socket already connected) */
    TH_ASSERT (io.rc == BSD_EISCONN);

    /* Bind connected socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    /* Should return error (socket already connected) */
    TH_ASSERT (io.rc == BSD_EISCONN);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Connect again, closed socket */
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server (connection rejected) */
    SA_SET (AF_INET, ip_server, TCP_REJECTED_PORT);
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    /* Should return error (connection rejected by the peer) */
    TH_ASSERT (io.rc == BSD_ECONNREFUSED);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server (non-existent) */
    SA_SET (AF_INET, ip_server, TCP_TIMEOUT_PORT);
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    /* Should return error (connection timeout) */
    TH_ASSERT (io.rc == BSD_ETIMEDOUT);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Bind socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Start listening */
    io.sock = sock;
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Connect to stream server */
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    /* Connect on listening socket should fail */
    TH_ASSERT (io.rc == BSD_EINVAL);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Check parameter (socket = -1) */
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    ARG_CONNECT(-1, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_CONNECT(INT32_MIN, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_CONNECT(INT32_MAX, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (addr = NULL) */
    ARG_CONNECT(sock, NULL, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = 0) */
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    ARG_CONNECT(sock, &sa, 0);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = 17) */
    ARG_CONNECT(sock, &sa, sizeof(sa) + 1);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (ip = 0.0.0.0) */
    SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    /* Datagram sockets may dissolve the association */
    /* by connecting to unspecified address.         */
    TH_ASSERT (io.rc == 0);

    /* Check parameter (port = 0) */
    SA_SET (AF_INET, ip_server, 0);
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Connect to datagram server */
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Connect to unspecified address (0.0.0.0) */
    SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    /* Datagram sockets may dissolve the association */
    /* by connecting to unspecified address.         */
    /* Should return ok (socket address deleted) */
    TH_ASSERT (io.rc == 0);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Connect again, closed socket */
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/**
\brief  Test case: BSD_SocketConnect_nbio
\details
The test case \b BSD_SocketConnect_nbio verifies the BSD \b connect function
running in non-blocking mode.

Stream socket test 1:
 - Create stream socket
 - Set non-blocking mode
 - Check function parameters
 - Connect to server, non-blocking mode
 - Connect again, already connected
 - Bind connected socket
 - Close socket
 - Connect on closed socket

Stream socket test 2:
 - Create stream socket
 - Set non-blocking mode
 - Connect to server, connection rejected
 - Close socket

Stream socket test 3:
 - Create stream socket
 - Set non-blocking mode
 - Connect to server, non-responding or non-existent
 - Close socket

Stream socket test 4:
 - Create stream socket
 - Set non-blocking mode
 - Bind socket
 - Start listening
 - Connect to server, non-blocking mode
 - Close socket
*/
void BSD_SocketConnect_nbio (void) {
  SOCKADDR_IN  sa;
  uint32_t     ticks,tout;
  osThreadId_t worker;
  int32_t      rval;
  IO_CONNECT   io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Connect, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Set socket non-blocking */
    io.sock = sock;
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    ARG_CONNECT(-1, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_CONNECT(INT32_MIN, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_CONNECT(INT32_MAX, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (addr = NULL) */
    ARG_CONNECT(sock, NULL, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (ip = 0.0.0.0) */
    SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = 0) */
    ARG_CONNECT(sock, &sa, 0);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = 17) */
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    ARG_CONNECT(sock, &sa, sizeof(sa) + 1);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (port = 0) */
    SA_SET (AF_INET, ip_server, 0);
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (family = UNSPEC) */
    SA_SET (AF_UNSPEC, ip_unspec, DISCARD_PORT);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (family = NETBIOS) */
    SA_SET (AF_NETBIOS, ip_unspec, DISCARD_PORT);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (family = INT16_MIN) */
    SA_SET (INT16_MIN, ip_unspec, DISCARD_PORT);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (family = INT16_MAX) */
    SA_SET (INT16_MAX, ip_unspec, DISCARD_PORT);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Connect to stream server, start non-blocking */
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINPROGRESS);

    /* Connect, polling mode */
    tout  = SYSTICK_MS(BSD_TIMEOUT_CONNECT);
    ticks = GET_SYSTICK();
    do {
      ARG_CONNECT(sock, &sa, sizeof(sa));
      TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
      if (io.rc != BSD_EALREADY) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT  (io.rc == BSD_EISCONN);

    /* Connect 2nd time */
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    /* Should return error (socket already connected) */
    TH_ASSERT (io.rc == BSD_EISCONN);

    /* Bind connected socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    /* Should return error (socket already connected) */
    TH_ASSERT (io.rc == BSD_EISCONN);

    /* Close socket, polling mode */
    io.sock = sock;
    tout  = SYSTICK_MS(BSD_TIMEOUT);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
      if (io.rc == 0) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT  (io.rc == 0);

    /* Connect again, closed socket */
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Set socket non-blocking */
    io.sock = sock;
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Connect to stream server (connection rejected) */
    SA_SET (AF_INET, ip_server, TCP_REJECTED_PORT);
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINPROGRESS);

    /* Connect, polling mode */
    tout  = SYSTICK_MS(BSD_TIMEOUT_CONNECT);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
      if (io.rc != BSD_EALREADY) break;
    } while (GET_SYSTICK() - ticks < tout);
    /* Should return error (connection rejected by the peer) */
    TH_ASSERT (io.rc == BSD_ECONNREFUSED);

    /* Close socket, polling mode */
    io.sock = sock;
    tout  = SYSTICK_MS(BSD_TIMEOUT);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
      if (io.rc == 0) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Set socket non-blocking */
    io.sock = sock;
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Connect to stream server (non-existent), start non-blocking */
    SA_SET (AF_INET, ip_server, TCP_TIMEOUT_PORT);
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINPROGRESS);

    /* Connect, polling mode */
    tout  = SYSTICK_MS(BSD_TIMEOUT_CONNECT);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
      if (io.rc != BSD_EALREADY) break;
    } while (GET_SYSTICK() - ticks < tout);
    /* Should return error (connection timeout) */
    TH_ASSERT (io.rc == BSD_ETIMEDOUT);

    /* Close socket, polling mode */
    io.sock = sock;
    tout  = SYSTICK_MS(BSD_TIMEOUT);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
      if (io.rc == 0) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Set socket non-blocking */
    io.sock = sock;
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Bind socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Start listening */
    io.sock = sock;
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Connect to stream server */
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    ARG_CONNECT(sock, &sa, sizeof(sa));
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    /* Connect on listening socket should fail */
    TH_ASSERT (io.rc == BSD_EINVAL);

    /* Close socket, polling mode */
    io.sock = sock;
    tout  = SYSTICK_MS(BSD_TIMEOUT);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
      if (io.rc == 0) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* Recv IO parameters */
typedef struct {
  int32_t      sock;
  char        *buf;
  int32_t      len;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
  uint32_t     tval;
} IO_RECV;

/* Assign arguments */
#define ARG_RECV(_sock,_buf,_len) \
  do {                            \
    io.sock = _sock;              \
    io.buf  = _buf;               \
    io.len  = _len;               \
  } while (0)

/* Recv worker thread */
__NO_RETURN static void Th_Recv (IO_RECV *io) {
  SOCKADDR_IN sa;
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_TCP | F_BIND | F_CONNECT | F_LISTEN |
                               F_SETOPT     | F_RECV | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_BIND:
        /* Bind socket */
        SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
        io->rc = bsd->bind (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

      case F_CONNECT:
        /* Connect on socket */
        SA_SET (AF_INET, ip_server, (uint16_t)io->tval);
        io->rc = bsd->connect (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

      case F_LISTEN:
        /* Listen on socket */
        io->rc = bsd->listen (io->sock, 1);
        break;

      case F_SETOPT:
        /* Set socket options */
        if (io->tval <= 1) {
          unsigned long nbio = 1;
          io->rc = bsd->ioctlsocket (io->sock, FIONBIO, &nbio);
          break;
        }
        io->rc = bsd->setsockopt (io->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&io->tval, sizeof(io->tval));
        break;

      case F_RECV:
        /* Recv on socket */
        if (io->buf) memset(io->buf, 0xCC, io->len);
        io->rc = bsd->recv (io->sock, (char *)io->buf, io->len, 0);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketRecv
\details
Test case \b BSD_SocketRecv verifies the BSD \b recv function:
\code
int32_t (*recv) (int32_t sock, char *buf, int32_t len, int32_t flags);
\endcode

Stream socket test 1:
 - Create stream socket
 - Connect to Chargen server
 - Check function parameters
 - Receive data in blocking mode
 - Close socket
 - Receive again, closed socket

Stream socket test 2:
 - Create stream socket
 - Receive data, created socket
 - Bind socket
 - Receive data, bound socket
 - Start listening
 - Receive data, listening socket
 - Close socket

Stream socket test 3:
 - Create stream socket
 - Connect to Discard server
 - Set receive timeout to 1 sec
 - Receive data, timeout expires
 - Close socket
*/
void BSD_SocketRecv (void) {
  char         buf[4];
  uint32_t     ticks,tout;
  osThreadId_t worker;
  int32_t      rval;
  IO_RECV      io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Recv, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.sock = sock;
    io.tval = CHARGEN_PORT;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    ARG_RECV   (-1, buf, sizeof(buf));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_RECV   (INT32_MIN, buf, sizeof(buf));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_RECV   (INT32_MAX, buf, sizeof(buf));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (buf = NULL) */
    ARG_RECV   (sock, NULL, sizeof(buf));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (len = 0) */
    ARG_RECV   (sock, buf, 0);
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Receive some data */
    ARG_RECV   (sock, buffer, sizeof(buffer));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc >= 2);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive again, closed socket */
    ARG_RECV (sock, buffer, sizeof(buffer));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    /* Test server mode */
    sock = io.rc;

    /* Receive, created socket */
    ARG_RECV   (sock, buffer, sizeof(buffer));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    /* Should return error (socket not connected) */
    TH_ASSERT (io.rc == BSD_ENOTCONN);

    /* Bind socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive, bound socket */
    ARG_RECV   (sock, buffer, sizeof(buffer));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    /* Should return error (socket not connected) */
    TH_ASSERT (io.rc == BSD_ENOTCONN);

    /* Start listening */
    io.sock = sock;
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive, listening socket */
    ARG_RECV   (sock, buffer, sizeof(buffer));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    /* Should return error (socket not connected) */
    TH_ASSERT (io.rc == BSD_ENOTCONN);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.sock = sock;
    io.tval = DISCARD_PORT;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Set receive timeout to 1 sec */
    io.sock = sock;
    io.tval = 1000;
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive until timeout, no data */
    ARG_RECV   (sock, buffer, sizeof(buffer));
    ticks = GET_SYSTICK();
    TH_EXECUTE (F_RECV, 1000 + BSD_TIMEOUT);
    tout = GET_SYSTICK() - ticks;
    /* Should return ETIMEDOUT (operation timed out) */
    TH_ASSERT  (io.rc == BSD_ETIMEDOUT);
    /* Check receive timeout is in the range of 0.9 to 1.1 sec */
    TH_ASSERT  (tout > SYSTICK_MS(900) && tout < SYSTICK_MS(1100));

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/**
\brief  Test case: BSD_SocketRecv_nbio
\details
Test case \b BSD_SocketRecv_nbio verifies the BSD \b recv function
running in non-blocking mode.

Stream socket test 1:
 - Create stream socket
 - Set non-blocking mode
 - Connect to Chargen server
 - Check function parameters
 - Receive data in non-blocking mode
 - Close socket
 - Receive again, closed socket

Stream socket test 2:
 - Create stream socket
 - Set non-blocking mode
 - Receive data, created socket
 - Bind socket
 - Receive data, bound socket
 - Start listening
 - Receive data, listening socket
 - Close socket

Stream socket test 3:
 - Create stream socket
 - Set non-blocking mode
 - Connect to Discard server
 - Receive data for 1 sec, timeout expires
 - Close socket
*/
void BSD_SocketRecv_nbio (void) {
  char         buf[4];
  uint32_t     ticks,tout,npoll;
  osThreadId_t worker;
  int32_t      rval;
  IO_RECV      io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Recv, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server, blocking */
    io.sock = sock;
    io.tval = CHARGEN_PORT;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Set socket non-blocking */
    io.tval = 1;
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    ARG_RECV   (-1, buf, sizeof(buf));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_RECV   (INT32_MIN, buf, sizeof(buf));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_RECV   (INT32_MAX, buf, sizeof(buf));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (buf = NULL) */
    ARG_RECV   (sock, NULL, sizeof(buf));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (len = 0) */
    ARG_RECV   (sock, buf, 0);
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Receive some data, polling mode */
    tout  = SYSTICK_MS(BSD_TIMEOUT_LONG);
    ticks = GET_SYSTICK();
    do {
      ARG_RECV   (sock, buffer, sizeof(buffer));
      TH_EXECUTE (F_RECV, BSD_TIMEOUT);
      if (io.rc != BSD_EWOULDBLOCK) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT  (io.rc >= 2);

    /* Close socket, polling mode */
    io.sock = sock;
    tout  = SYSTICK_MS(BSD_TIMEOUT);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
      if (io.rc == 0) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT  (io.rc == 0);

    /* Receive again, closed socket */
    ARG_RECV (sock, buffer, sizeof(buffer));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    /* Test server mode */
    sock = io.rc;

    /* Set socket non-blocking */
    io.sock = sock;
    io.tval = 1;
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive, created socket */
    ARG_RECV   (sock, buffer, sizeof(buffer));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    /* Should return error (socket not connected) */
    TH_ASSERT  (io.rc == BSD_ENOTCONN);

    /* Bind socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive, bound socket */
    ARG_RECV   (sock, buffer, sizeof(buffer));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    /* Should return error (socket not connected) */
    TH_ASSERT  (io.rc == BSD_ENOTCONN);

    /* Start listening */
    io.sock = sock;
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive, listening socket */
    ARG_RECV   (sock, buffer, sizeof(buffer));
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    /* Should return error (socket not connected) */
    TH_ASSERT  (io.rc == BSD_ENOTCONN);

    /* Close socket, polling mode */
    io.sock = sock;
    tout  = SYSTICK_MS(BSD_TIMEOUT);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
      if (io.rc == 0) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.sock = sock;
    io.tval = DISCARD_PORT;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Set socket non-blocking */
    io.sock = sock;
    io.tval = 1;
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive for 1 sec, timeout no data */
    npoll = 0;
    tout  = SYSTICK_MS(1000);
    ticks = GET_SYSTICK();
    do {
      ARG_RECV   (sock, buffer, sizeof(buffer));
      TH_EXECUTE (F_RECV, BSD_TIMEOUT);
      npoll++;
      if (io.rc != BSD_EWOULDBLOCK) break;
    } while (GET_SYSTICK() - ticks < tout);
    /* Should return EAGAIN (operation timed out) */
    TH_ASSERT  (io.rc == BSD_EWOULDBLOCK);
    /* Check polling counter (max. 1000) */
    TH_ASSERT  (npoll >= 50);

    /* Close socket, polling mode */
    io.sock = sock;
    tout  = SYSTICK_MS(BSD_TIMEOUT);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
      if (io.rc == 0) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* RecvFrom IO parameters */
typedef struct {
  int32_t      sock;
  char        *buf;
  int32_t      len;
  SOCKADDR    *addr;
  int32_t     *addrlen;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
  uint32_t     tval;
} IO_RECVFROM;

/* Assign arguments */
#define ARG_RECVFROM(_sock,_buf,_len,_addr,_addrlen) \
  do {                                               \
    io.sock    = _sock;                              \
    io.buf     = _buf;                               \
    io.len     = _len;                               \
    io.addr    = (SOCKADDR *)_addr;                  \
    io.addrlen = _addrlen;                           \
  } while (0)

/* RecvFrom worker thread */
__NO_RETURN static void Th_RecvFrom (IO_RECVFROM *io) {
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_UDP | F_CONNECT | F_SETOPT |
                               F_RECVFROM   | F_SEND    | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_CONNECT: {
        /* Connect on socket */
        SOCKADDR_IN sa;
        SA_SET (AF_INET, ip_server, CHARGEN_PORT);
        io->rc = bsd->connect (io->sock, (SOCKADDR *)&sa, sizeof(sa));
      } break;

      case F_SETOPT: {
        /* Set socket options */
        if (io->tval <= 1) {
          unsigned long nbio = 1;
          io->rc = bsd->ioctlsocket (io->sock, FIONBIO, &nbio);
          break;
        }
        io->rc = bsd->setsockopt (io->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&io->tval, sizeof(io->tval));
      } break;

      case F_RECVFROM:
        /* RecvFrom on socket */
        if (io->buf) memset(io->buf, 0xCC, io->len);
        io->rc = bsd->recvfrom (io->sock, io->buf, io->len, 0, io->addr, io->addrlen);
        break;

      case F_SEND:
        /* Send on socket */
        io->rc = bsd->send (io->sock, "a", 1, 0);
        io->rc = (io->rc == 1) ? 0 : BSD_ERROR;
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketRecvFrom
\details
The test case \b BSD_SocketRecvFrom verifies the BSD \b recvfrom function:
\code
int32_t (*recvfrom)(int32_t sock, char *buf, int32_t len, int32_t flags, SOCKADDR *from, int32_t *fromlen);
\endcode

Datagram socket test 1:
 - Create datagram socket
 - Connect to Chargen server
 - Check function parameters
 - Receive data in blocking mode
 - Set receive timeout to 1 sec
 - Receive again, timeout expires
 - Close socket
 - Receive again, closed socket
*/
void BSD_SocketRecvFrom (void) {
  SOCKADDR_IN  sa;
  int32_t      salen;
  uint32_t     ticks,tout;
  char         buf[4];
  osThreadId_t worker;
  int32_t      rval;
  IO_RECVFROM  io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_RecvFrom, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to datagram server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    salen = sizeof(sa);
    ARG_RECVFROM (-1, buf, sizeof(buf), &sa, &salen);
    TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_RECVFROM (INT32_MIN, buf, sizeof(buf), &sa, &salen);
    TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_RECVFROM (INT32_MAX, buf, sizeof(buf), &sa, &salen);
    TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (buf == NULL) */
    ARG_RECVFROM (sock, NULL, sizeof(buf), &sa, &salen);
    TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (len = 0) */
    ARG_RECVFROM (sock, buf, 0, &sa, &salen);
    TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Send one byte of data to trigger a reply */
    io.sock = sock;
    TH_EXECUTE (F_SEND, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 0);

    /* Initialize buffers for return values */
    memset (&sa, 0, sizeof(sa));
    salen = sizeof(sa) + 1;

    /* Receive some data */
    ARG_RECVFROM (sock, buffer, sizeof(buffer), &sa, &salen);
    TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT_LONG);
    /* Should receive at least 2 bytes */
    TH_ASSERT  (io.rc >= 2);
    /* IP address should be the address of the server */
    TH_ASSERT  (memcmp (&sa.sin_addr, ip_server, 4) == 0);
    /* Address length should be updated */
    TH_ASSERT (salen == sizeof(sa));
    /* Port number should be the port of the CHARGEN server */
    TH_ASSERT  (sa.sin_port == htons(CHARGEN_PORT));

    /* Set receive timeout to 1 sec */
    io.sock = sock;
    io.tval = 1000;
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive until timeout, no data */
    ARG_RECVFROM (sock, buffer, sizeof(buffer), &sa, &salen);
    ticks = GET_SYSTICK();
    TH_EXECUTE (F_RECVFROM, 1000 + BSD_TIMEOUT);
    tout = GET_SYSTICK() - ticks;
    /* Should return ETIMEDOUT (operation timed out) */
    TH_ASSERT  (io.rc == BSD_ETIMEDOUT);
    /* Check receive timeout is in the range of 0.9 to 1.1 sec */
    TH_ASSERT  (tout > SYSTICK_MS(900) && tout < SYSTICK_MS(1100));

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive again, closed socket */
    ARG_RECVFROM (sock, buffer, sizeof(buffer), &sa, &salen);
    TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/**
\brief  Test case: BSD_SocketRecvFrom_nbio
\details
The test case \b BSD_SocketRecvFrom_nbio verifies the BSD \b recvfrom function
running in non-blocking mode.

Datagram socket test 1:
 - Create datagram socket
 - Set non-blocking mode
 - Connect to Chargen server
 - Check function parameters
 - Receive data in non-blocking mode
 - Receive again, timeout expires
 - Close socket
 - Receive again, closed socket
*/
void BSD_SocketRecvFrom_nbio (void) {
  SOCKADDR_IN  sa;
  int32_t      salen;
  uint32_t     ticks,tout,npoll;
  char         buf[4];
  osThreadId_t worker;
  int32_t      rval;
  IO_RECVFROM  io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_RecvFrom, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Set socket non-blocking */
    io.sock = sock;
    io.tval = 1;
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Connect to datagram server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    salen = sizeof(sa);
    ARG_RECVFROM (-1, buf, sizeof(buf), &sa, &salen);
    TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_RECVFROM (INT32_MIN, buf, sizeof(buf), &sa, &salen);
    TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_RECVFROM (INT32_MAX, buf, sizeof(buf), &sa, &salen);
    TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (buf == NULL) */
    ARG_RECVFROM (sock, NULL, sizeof(buf), &sa, &salen);
    TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (len = 0) */
    ARG_RECVFROM (sock, buf, 0, &sa, &salen);
    TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
    /* No data available to read */
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Send one byte of data to trigger a reply */
    io.sock = sock;
    TH_EXECUTE (F_SEND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Initialize buffers for return values */
    memset (&sa, 0, sizeof(sa));
    salen = sizeof(sa) + 1;

    /* Receive some data */
    tout  = SYSTICK_MS(BSD_TIMEOUT_LONG);
    ticks = GET_SYSTICK();
    do {
      ARG_RECVFROM (sock, buffer, sizeof(buffer), &sa, &salen);
      TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
      if (io.rc != BSD_EWOULDBLOCK) break;
    } while (GET_SYSTICK() - ticks < tout);
    /* Should receive at least 2 bytes */
    TH_ASSERT  (io.rc >= 2);
    /* IP address should be the address of the server */
    TH_ASSERT  (memcmp (&sa.sin_addr, ip_server, 4) == 0);
    /* Address length should be updated */
    TH_ASSERT (salen == sizeof(sa));
    /* Port number should be the port of the CHARGEN server */
    TH_ASSERT  (sa.sin_port == htons(CHARGEN_PORT));

     /* Receive for 1 sec, timeout no data */
    npoll = 0;
    tout  = SYSTICK_MS(1000);
    ticks = GET_SYSTICK();
    do {
      ARG_RECVFROM (sock, buffer, sizeof(buffer), &sa, &salen);
      TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
      npoll++;
      if (io.rc != BSD_EWOULDBLOCK) break;
    } while (GET_SYSTICK() - ticks < tout);
    /* Should return EAGAIN (operation timed out) */
    TH_ASSERT  (io.rc == BSD_EWOULDBLOCK);
    /* Check polling counter (max. 1000) */
    TH_ASSERT  (npoll >= 50);

    /* Close socket, polling mode */
    io.sock = sock;
    tout  = SYSTICK_MS(BSD_TIMEOUT);
    ticks = GET_SYSTICK();
    do {
      TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
      if (io.rc == 0) break;
    } while (GET_SYSTICK() - ticks < tout);
    TH_ASSERT  (io.rc == 0);

    /* Receive again, closed socket */
    ARG_RECVFROM (sock, buffer, sizeof(buffer), &sa, &salen);
    TH_EXECUTE (F_RECVFROM, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* RecvMsg IO parameters */
typedef struct {
  int32_t      sock;
  MSGHDR      *msg;
  int32_t      flags;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
  uint32_t     tval;
} IO_RECVMSG;

/* Assign arguments */
#define ARG_RECVMSG(_sock,_msg,_flags) \
  do {                                 \
    io.sock  = _sock;                  \
    io.msg   = _msg;                   \
    io.flags = _flags;                 \
  } while (0)

/* RecvMsg worker thread */
__NO_RETURN static void Th_RecvMsg (IO_RECVMSG *io) {
  SOCKADDR_IN sa;
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP | F_CONNECT | F_SETOPT |
                               F_SEND       | F_RECVMSG | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_CONNECT:
        /* Connect on socket */
        SA_SET (AF_INET, ip_server, (uint16_t)io->tval);
        io->rc = bsd->connect (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

      case F_SETOPT:
        /* Set socket options */
        io->rc = bsd->setsockopt (io->sock, IPPROTO_IP, IP_RECVDSTADDR, (char *)&io->tval, sizeof(io->tval));
        break;

      case F_SEND:
        /* Send on socket */
        io->rc = bsd->send (io->sock, test_msg, sizeof(test_msg), 0);
        io->rc = (io->rc == sizeof(test_msg)) ? 0 : BSD_ERROR;
        break;

      case F_RECVMSG:
        /* Recv on socket */
        io->rc = bsd->recvmsg (io->sock, io->msg, io->flags);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketRecvMsg
\details
Test case \b BSD_SocketRecvMsg verifies the BSD \b recvmsg function:
\code
int32_t (*recvmsg) (int32_t sock, MSGHDR *msg, int32_t flags);
\endcode

Stream socket test:
 - Create stream socket
 - Connect to Chargen server
 - Check function parameters
 - Receive data in blocking mode
 - Close socket
 - Receive again, closed socket

Datagram socket test:
 - Create stream socket
 - Connect to Discard server
 - Set receive timeout to 1 sec
 - Receive data, timeout expires
 - Close socket
*/
void BSD_SocketRecvMsg (void) {
  MSGHDR       msg;
  IOVEC        msg_iov[2];
  char         buf1[5];
  char         buf2[31];
  uint8_t      cmsg[16];
  osThreadId_t worker;
  int32_t      rval;
  IO_RECVMSG   io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_RecvMsg, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.sock = sock;
    io.tval = CHARGEN_PORT;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    ARG_RECVMSG(-1, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_RECVMSG(INT32_MIN, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_RECVMSG(INT32_MAX, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (msg = NULL) */
    ARG_RECVMSG(sock, NULL, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Initialize buffers for return values */
    memset (&msg, 0, sizeof(msg));
    msg_iov[0].iov_base = &buf1;
    msg_iov[0].iov_len  = sizeof(buf1);
    msg_iov[1].iov_base = &buf2;
    msg_iov[1].iov_len  = sizeof(buf2);

    /* Check message vector (msg->msg_iov = NULL) */
    msg.msg_iov    = NULL;
    msg.msg_iovlen = 3;
    ARG_RECVMSG(sock, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check message vector length (msg->msg_iovlen = 0) */
    msg.msg_iov    = &msg_iov[0];
    msg.msg_iovlen = 0;
    ARG_RECVMSG(sock, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Receive message */
    msg.msg_iov    = &msg_iov[0];
    msg.msg_iovlen = 2;
    ARG_RECVMSG(sock, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ENOTSUP);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive again, closed socket */
    ARG_RECVMSG(sock, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to datagram server */
    io.sock = sock;
    io.tval = ECHO_PORT;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    ARG_RECVMSG(-1, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_RECVMSG(INT32_MIN, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_RECVMSG(INT32_MAX, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (msg = NULL) */
    ARG_RECVMSG(sock, NULL, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Initialize buffers for return values */
    memset (&msg, 0, sizeof(msg));
    msg_iov[0].iov_base = &buf1;
    msg_iov[0].iov_len  = sizeof(buf1);
    msg_iov[1].iov_base = &buf2;
    msg_iov[1].iov_len  = sizeof(buf2);

    /* Check message vector (msg->msg_iov = NULL) */
    msg.msg_iov    = NULL;
    msg.msg_iovlen = 3;
    ARG_RECVMSG(sock, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check message vector length (msg->msg_iovlen = 0) */
    msg.msg_iov    = &msg_iov[0];
    msg.msg_iovlen = 0;
    ARG_RECVMSG(sock, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Send data to be echoed back */
    io.sock = sock;
    TH_EXECUTE (F_SEND, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 0);

    /* Receive message */
    msg.msg_iov    = &msg_iov[0];
    msg.msg_iovlen = 2;
    memset (buf1, 0xCC, sizeof(buf1));
    memset (buf2, 0xCC, sizeof(buf2));
    ARG_RECVMSG(sock, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == sizeof(buf1) + sizeof(buf2));
    /* Check buffer contents */
    TH_ASSERT  (memcmp (buf1,  test_msg, sizeof(buf1)) == 0);
    TH_ASSERT  (memcmp (buf2, &test_msg[sizeof(buf1)], sizeof(buf2)) == 0);

    /* Enable Receiving destination IP */
    io.tval = 1;
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Send data to be echoed back */
    io.sock = sock;
    TH_EXECUTE (F_SEND, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 0);

    /* Receive message with auxiliary data */
    msg.msg_iov        = &msg_iov[0];
    msg.msg_iovlen     = 2;
    msg.msg_control    = &cmsg;
    msg.msg_controllen = sizeof(cmsg) + 1;
    msg.msg_flags      = 0xCCCCCCCC;
    memset (buf1, 0xCC, sizeof(buf1));
    memset (buf2, 0xCC, sizeof(buf2));
    memset (cmsg, 0xCC, sizeof(cmsg));
    ARG_RECVMSG(sock, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == sizeof(buf1) + sizeof(buf2));
    /* Check buffer contents */
    TH_ASSERT  (memcmp (buf1,  test_msg, sizeof(buf1)) == 0);
    TH_ASSERT  (memcmp (buf2, &test_msg[sizeof(buf1)], sizeof(buf2)) == 0);
    /* Check auxilary data */
    TH_ASSERT  (msg.msg_controllen == 16);
    /* Control length was updated */
    TH_ASSERT  (msg.msg_flags == MSG_TRUNC);
    TH_ASSERT  ((CMSG_FIRSTHDR(&msg))->cmsg_len   == 16);
    TH_ASSERT  ((CMSG_FIRSTHDR(&msg))->cmsg_level == IPPROTO_IP);
    TH_ASSERT  ((CMSG_FIRSTHDR(&msg))->cmsg_type  == IP_RECVDSTADDR);
    /* Get local IP address */
    netIF_GetOption(NET_IF_CLASS_ETH | 0, netIF_OptionIP4_Address,
                                          (uint8_t *)buf1, sizeof (buf1));
    TH_ASSERT  (memcmp(CMSG_DATA(CMSG_FIRSTHDR(&msg)), buf1, 4) == 0);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive again, closed socket */
    ARG_RECVMSG(sock, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Initialize buffers for return values */
    memset (&msg, 0, sizeof(msg));
    msg_iov[0].iov_base = &buf1;
    msg_iov[0].iov_len  = sizeof(buf1);
    msg_iov[1].iov_base = &buf2;
    msg_iov[1].iov_len  = sizeof(buf2);

    /* Receive message */
    msg.msg_iov    = &msg_iov[0];
    msg.msg_iovlen = 2;
    memset (buf1, 0, sizeof(buf1));
    memset (buf2, 0, sizeof(buf2));
    ARG_RECVMSG(sock, &msg, 0);
    TH_EXECUTE (F_RECVMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ENOTCONN);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* Send IO parameters */
typedef struct {
  int32_t      sock;
  const char  *buf;
  int32_t      len;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
} IO_SEND;

/* Assign arguments */
#define ARG_SEND(_sock,_buf,_len) \
  do {                            \
    io.sock = _sock;              \
    io.buf  = _buf;               \
    io.len  = _len;               \
  } while (0)

/* Send worker thread */
__NO_RETURN static void Th_Send (IO_SEND *io) {
  SOCKADDR_IN sa;
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_TCP | F_BIND | F_CONNECT |
                               F_LISTEN     | F_SEND | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_BIND:
        /* Bind socket */
        SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
        io->rc = bsd->bind (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

      case F_CONNECT:
        /* Connect on socket */
        SA_SET (AF_INET, ip_server, DISCARD_PORT);
        io->rc = bsd->connect (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

       case F_LISTEN:
        /* Listen on socket */
        io->rc = bsd->listen (io->sock, 1);
        break;

     case F_SEND:
        /* Send on socket */
        io->rc = bsd->send (io->sock, io->buf, io->len, 0);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketSend
\details
The test case \b BSD_SocketSend verifies the BSD \b send function:
\code
int32_t (*send) (int32_t sock, const char *buf, int32_t len, int32_t flags);
\endcode

Stream socket test 1:
 - Create stream socket
 - Connect to server, blocking mode
 - Check function parameters
 - Send data, blocking mode
 - Close socket
 - Send again, closed socket

Stream socket test 2:
 - Create stream socket
 - Connect to server, blocking mode
 - Send ESC data, server disconnects
 - Send again, disconnected socket
 - Close socket

Stream socket test 3:
 - Create stream socket
 - Send data, created socket
 - Bind socket
 - Send data, bound socket
 - Start listening
 - Send data, listening socket
 - Close socket
 - Send again, closed socket
*/
void BSD_SocketSend (void) {
  osThreadId_t worker;
  int32_t      rval;
  IO_SEND      io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Send, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    ARG_SEND   (-1, test_msg, sizeof(test_msg));
    TH_EXECUTE (F_SEND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_SEND   (INT32_MIN, test_msg, sizeof(test_msg));
    TH_EXECUTE (F_SEND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_SEND   (INT32_MAX, test_msg, sizeof(test_msg));
    TH_EXECUTE (F_SEND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (buf = NULL) */
    ARG_SEND   (sock, NULL, sizeof(test_msg));
    TH_EXECUTE (F_SEND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (len = 0) */
    ARG_SEND   (sock, test_msg, 0);
    TH_EXECUTE (F_SEND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Send some data */
    ARG_SEND   (sock, test_msg, sizeof(test_msg));
    TH_EXECUTE (F_SEND, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == sizeof(test_msg));

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Send again, closed socket */
    ARG_SEND   (sock, test_msg, sizeof(test_msg));
    TH_EXECUTE (F_SEND, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    if (io.rc == BSD_ETIMEDOUT) {
      /* Second connect request might be ignored by SockServer */
      TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
      TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
      sock = io.sock = io.rc;
      TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    }
    TH_ASSERT  (io.rc == 0);

    /* Send ESC command, server disconnects */
    ARG_SEND   (sock, (char *)"\x1B", 1);
    TH_EXECUTE (F_SEND, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1);

    /* Wait for the server to disconnect */
    osDelay (200);

    /* Send again, disconnected socket */
    ARG_SEND   (sock, test_msg, sizeof(test_msg));
    TH_EXECUTE (F_SEND, BSD_TIMEOUT);
    /* Should return error (connection reset by the peer) */
    TH_ASSERT  (io.rc == BSD_ECONNRESET);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Send data, created socket */
    ARG_SEND   (sock, test_msg, sizeof(test_msg));
    TH_EXECUTE (F_SEND, BSD_TIMEOUT);
    /* Should return error (socket not connected) */
    TH_ASSERT  (io.rc == BSD_ENOTCONN);

    /* Bind socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Send data, bound socket */
    ARG_SEND (sock, test_msg, sizeof(test_msg));
    TH_EXECUTE (F_SEND, BSD_TIMEOUT);
    /* Should return error (socket not connected) */
    TH_ASSERT  (io.rc == BSD_ENOTCONN);

    /* Start listening */
    io.sock = sock;
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Send data, listening socket */
    ARG_SEND (sock, test_msg, sizeof(test_msg));
    TH_EXECUTE (F_SEND, BSD_TIMEOUT);
    /* Should return error (socket not connected) */
    TH_ASSERT  (io.rc == BSD_ENOTCONN);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Send again, closed socket */
    ARG_SEND (sock, test_msg, sizeof(test_msg));
    TH_EXECUTE (F_SEND, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* SendTo IO parameters */
typedef struct {
  int32_t         sock;
  const char     *buf;
  int32_t         len;
  const SOCKADDR *addr;
  int32_t         addrlen;
  int32_t         rc;
  /* Control */
  osThreadId_t    owner;
  uint32_t        xid;
} IO_SENDTO;

/* Assign arguments */
#define ARG_SENDTO(_sock,_buf,_len,_addr,_addrlen) \
  do {                                             \
    io.sock    = _sock;                            \
    io.buf     = _buf;                             \
    io.len     = _len;                             \
    io.addr    = (const SOCKADDR *)_addr;          \
    io.addrlen = _addrlen;                         \
  } while (0)

/* SendTo worker thread */
__NO_RETURN static void Th_SendTo (IO_SENDTO *io) {
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_UDP | F_SENDTO | F_RECV | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_SENDTO:
        /* SendTo on socket */
        io->rc = bsd->sendto (io->sock, io->buf, io->len, 0, io->addr, io->addrlen);
        break;

      case F_RECV:
        /* Recv on socket */
        memset(buffer, 0xCC, sizeof(buffer));
        io->rc = bsd->recv (io->sock, buffer, sizeof(buffer), 0);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketSendTo
\details
The test case \b BSD_SocketSend verifies the BSD \b sendto function:
\code
int32_t (*sendto) (int32_t sock, const char *buf, int32_t len, int32_t flags, const SOCKADDR *to, int32_t tolen);
\endcode

Datagram socket test:
 - Create datagram socket
 - Check function parameters
 - Send data, blocking mode
 - Receive echo data, verify if the same
 - Close socket
 - Send again, closed socket
*/
void BSD_SocketSendTo (void) {
  SOCKADDR_IN  sa;
  osThreadId_t worker;
  int32_t      rval;
  IO_SENDTO    io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_SendTo, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Check parameter (socket = -1) */
    SA_SET (AF_INET, ip_server, ECHO_PORT);
    ARG_SENDTO (-1, test_msg, sizeof(test_msg), &sa, sizeof(sa));
    TH_EXECUTE (F_SENDTO, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_SENDTO (INT32_MIN, test_msg, sizeof(test_msg), &sa, sizeof(sa));
    TH_EXECUTE (F_SENDTO, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_SENDTO (INT32_MAX, test_msg, sizeof(test_msg), &sa, sizeof(sa));
    TH_EXECUTE (F_SENDTO, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (buf == NULL) */
    ARG_SENDTO (sock, NULL, sizeof(test_msg), &sa, sizeof(sa));
    TH_EXECUTE (F_SENDTO, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (len = 0) */
    ARG_SENDTO (sock, test_msg, 0, &sa, sizeof(sa));
    TH_EXECUTE (F_SENDTO, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Send some data */
    ARG_SENDTO (sock, test_msg, sizeof(test_msg), &sa, sizeof(sa));
    TH_EXECUTE (F_SENDTO, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == sizeof(test_msg));

    /* Receive the echoed data */
    io.sock = sock;
    TH_EXECUTE (F_RECV, BSD_TIMEOUT_LONG);
    /* Should receive the same data (ECHO protocol) */
    TH_ASSERT  ((io.rc == sizeof(test_msg)) && (memcmp (test_msg, buffer, sizeof(test_msg)) == 0));

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Send again, closed socket */
    ARG_SENDTO (sock, test_msg, sizeof(test_msg), &sa, sizeof(sa));
    TH_EXECUTE (F_SENDTO, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* SendMsg IO parameters */
typedef struct {
  int32_t       sock;
  const MSGHDR *msg;
  int32_t       flags;
  int32_t       rc;
  /* Control */
  osThreadId_t  owner;
  uint32_t      xid;
  uint32_t      tval;
} IO_SENDMSG;

/* Assign arguments */
#define ARG_SENDMSG(_sock,_msg,_flags) \
  do {                                 \
    io.sock  = _sock;                  \
    io.msg   = _msg;                   \
    io.flags = _flags;                 \
  } while (0)

/* SendMsg worker thread */
__NO_RETURN static void Th_SendMsg (IO_SENDMSG *io) {
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP | F_SENDMSG |
                               F_RECV       | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_SENDMSG:
        /* SendTo on socket */
        io->rc = bsd->sendmsg (io->sock, io->msg, io->flags);
        break;

      case F_RECV:
        /* Recv on socket */
        memset(buffer, 0xCC, sizeof(buffer));
        io->rc = bsd->recv (io->sock, buffer, sizeof(buffer), 0);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketSendMsg
\details
The test case \b BSD_SocketSendMsg verifies the BSD \b sendmsg function:
\code
int32_t (*sendmsg) (int32_t sock, const MSGHDR *msg, int32_t flags);
\endcode

Stream socket test 1:
 - Create stream socket
 - Connect to Chargen server
 - Check function parameters
 - Receive data in blocking mode
 - Close socket
 - Receive again, closed socket

Datagram socket test:
 - Create datagram socket
 - Check function parameters
 - Send data, blocking mode
 - Receive echo data, verify if the same
 - Close socket
 - Send again, closed socket
*/
void BSD_SocketSendMsg (void) {
  MSGHDR       msg;
  IOVEC        msg_iov[2];
  char         buf1[5];
  char         buf2[31];
  SOCKADDR_IN  sa;
  osThreadId_t worker;
  int32_t      rval;
  IO_SENDMSG   io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_SendMsg, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Initialize buffers for send values */
    SA_SET (AF_INET, ip_server, ECHO_PORT);
    memset (&msg, 0, sizeof(msg));
    msg.msg_name        = &sa;
    msg.msg_namelen     = sizeof(sa);
    msg_iov[0].iov_base = &buf1;
    msg_iov[0].iov_len  = sizeof(buf1);
    msg_iov[1].iov_base = &buf2;
    msg_iov[1].iov_len  = sizeof(buf2);

    /* Check parameter (socket = -1) */
    SA_SET (AF_INET, ip_server, ECHO_PORT);
    ARG_SENDMSG(-1, &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_SENDMSG(INT32_MIN,  &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_SENDMSG (INT32_MAX,  &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (msg == NULL) */
    ARG_SENDMSG (sock, NULL, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check message vector (msg->msg_iov = NULL) */
    msg.msg_iov    = NULL;
    msg.msg_iovlen = 3;
    ARG_SENDMSG(sock, &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check message vector length (msg->msg_iovlen = 0) */
    msg.msg_iov    = &msg_iov[0];
    msg.msg_iovlen = 0;
    ARG_SENDMSG(sock, &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Send some data */
    memcpy (buf1,  test_msg, sizeof(buf1));
    memcpy (buf2, &test_msg[sizeof(buf1)], sizeof(buf2));
    msg.msg_iov    = &msg_iov[0];
    msg.msg_iovlen = 2;
    ARG_SENDMSG(sock, &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ENOTSUP);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Send again, closed socket */
    ARG_SENDMSG(sock, &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Initialize buffers for send values */
    SA_SET (AF_INET, ip_server, ECHO_PORT);
    memset (&msg, 0, sizeof(msg));
    msg.msg_name        = &sa;
    msg.msg_namelen     = sizeof(sa);
    msg_iov[0].iov_base = &buf1;
    msg_iov[0].iov_len  = sizeof(buf1);
    msg_iov[1].iov_base = &buf2;
    msg_iov[1].iov_len  = sizeof(buf2);

    /* Check parameter (socket = -1) */
    SA_SET (AF_INET, ip_server, ECHO_PORT);
    ARG_SENDMSG(-1, &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_SENDMSG(INT32_MIN,  &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_SENDMSG (INT32_MAX,  &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (msg == NULL) */
    ARG_SENDMSG (sock, NULL, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check message vector (msg->msg_iov = NULL) */
    msg.msg_iov    = NULL;
    msg.msg_iovlen = 3;
    ARG_SENDMSG(sock, &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check message vector length (msg->msg_iovlen = 0) */
    msg.msg_iov    = &msg_iov[0];
    msg.msg_iovlen = 0;
    ARG_SENDMSG(sock, &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Send some data */
    memcpy (buf1,  test_msg, sizeof(buf1));
    memcpy (buf2, &test_msg[sizeof(buf1)], sizeof(buf2));
    msg.msg_iov    = &msg_iov[0];
    msg.msg_iovlen = 2;
    ARG_SENDMSG(sock, &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == (sizeof(buf1) + sizeof(buf2)));

    /* Receive the echoed data */
    io.sock = sock;
    TH_EXECUTE (F_RECV, BSD_TIMEOUT_LONG);
    /* Should receive the same data (ECHO protocol) */
    TH_ASSERT  (io.rc == (sizeof(buf1) + sizeof(buf2)));
    TH_ASSERT  (memcmp (buffer, test_msg, io.rc) == 0);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Send again, closed socket */
    ARG_SENDMSG(sock, &msg, 0);
    TH_EXECUTE (F_SENDMSG, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* GetSockName IO parameters */
typedef struct {
  int32_t      sock;
  SOCKADDR    *addr;
  int32_t     *addrlen;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
} IO_GETSOCKNAME;

/* Assign arguments */
#define ARG_GETSOCKNAME(_sock,_addr,_addrlen) \
  do {                                        \
    io.sock    = _sock;                       \
    io.addr    = (SOCKADDR *)_addr;           \
    io.addrlen = _addrlen;                    \
  } while (0)

/* GetSockName worker thread */
__NO_RETURN static void Th_GetSockName (IO_GETSOCKNAME *io) {
  SOCKADDR_IN sa;
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP  | F_BIND |
                               F_CONNECT    | F_GETSOCKNAME | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_BIND:
        /* Bind socket */
        SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
        io->rc = bsd->bind (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

      case F_CONNECT:
        /* Connect on socket */
        SA_SET (AF_INET, ip_server, DISCARD_PORT);
        io->rc = bsd->connect (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

      case F_GETSOCKNAME:
        /* Get socket name */
        io->rc = bsd->getsockname (io->sock, io->addr, io->addrlen);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketGetSockName
\details
The test case \b BSD_SocketGetSockName verifies the BSD \b getsockname function:
\code
int32_t (*getsockname) (int32_t sock, SOCKADDR *name, int32_t *namelen);
\endcode

Stream socket test 1:
 - Create stream socket
 - Connect to server, blocking mode
 - Check function parameters
 - Get socket name
 - Close socket
 - Get socket name again, closed socket

Stream socket test 1:
 - Create stream socket
 - Get socket name, not bound
 - Bind socket
 - Get socket name, bound
 - Close socket

Datagram socket test 1:
 - Create datagram socket
 - Connect to server, enable packet filtering
 - Check function parameters
 - Get socket name
 - Close socket
 - Get socket name again, closed socket

Datagram socket test 1:
 - Create datagram socket
 - Get socket name, not bound
 - Bind socket
 - Get socket name, bound
 - Close socket
*/
void BSD_SocketGetSockName (void) {
  SOCKADDR_IN    sa;
  int32_t        salen;
  osThreadId_t   worker;
  int32_t        rval;
  IO_GETSOCKNAME io;
  int32_t        sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_GetSockName, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    /* Test client mode */
    sock = io.rc;

    /* Connect to stream server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    salen = sizeof(sa);
    ARG_GETSOCKNAME (-1, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_GETSOCKNAME (INT32_MIN, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_GETSOCKNAME (INT32_MAX, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (addr = NULL) */
    ARG_GETSOCKNAME (sock, NULL, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = NULL) */
    ARG_GETSOCKNAME (sock, &sa, NULL);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = 15) */
    salen = sizeof(sa) - 1;
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    ARG_GETSOCKNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Initialize buffers for return values */
    memset (&sa, 0, sizeof(sa));
    SA_SET (0, ip_bcast, 0);
    salen = sizeof(sa) + 1;

    /* Retrieve socket name */
    ARG_GETSOCKNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* IP address should be different from broadcast */
    TH_ASSERT  (memcmp (&sa.sin_addr, ip_bcast, 4) != 0);
    /* Address length should be updated */
    TH_ASSERT  (salen == sizeof(sa));
    /* Port number should be non-zero */
    TH_ASSERT  (sa.sin_port != 0);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Retrieve socket name again */
    ARG_GETSOCKNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    /* Test server mode */
    sock = io.rc;

    /* Retrieve socket name, not bound */
    salen = sizeof(sa);
    ARG_GETSOCKNAME (-1, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    /* Should return error (socket not bound) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Initialize buffers for return values */
    memset (&sa, 0, sizeof(sa));
    SA_SET (0, ip_bcast, 0);
    salen = sizeof(sa) + 1;

    /* Bind socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Retrieve socket name, bound */
    ARG_GETSOCKNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* IP address should be unspecified */
    TH_ASSERT  (memcmp (&sa.sin_addr, ip_unspec, 4) == 0);
    /* Address length should be updated */
    TH_ASSERT  (salen == sizeof(sa));
    /* Port number should be listening port */
    TH_ASSERT  (sa.sin_port == htons(DISCARD_PORT));

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    /* Test client mode */
    sock = io.rc;

    /* Connect to datagram server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

   /* Check parameter (socket = -1) */
    salen = sizeof(sa);
    ARG_GETSOCKNAME (-1, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_GETSOCKNAME (INT32_MIN, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_GETSOCKNAME (INT32_MAX, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (addr = NULL) */
    ARG_GETSOCKNAME (sock, NULL, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = NULL) */
    ARG_GETSOCKNAME (sock, &sa, NULL);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = 15) */
    salen = sizeof(sa) - 1;
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    ARG_GETSOCKNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Initialize buffers for return values */
    memset (&sa, 0, sizeof(sa));
    SA_SET (0, ip_bcast, 0);
    salen = sizeof(sa) + 1;

    /* Retrieve socket name */
    ARG_GETSOCKNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* IP address should be different from broadcast */
    TH_ASSERT  (memcmp (&sa.sin_addr, ip_bcast, 4) != 0);
    /* Address length should be updated */
    TH_ASSERT  (salen == sizeof(sa));
    /* Port number should be non-zero */
    TH_ASSERT  (sa.sin_port != 0);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Retrieve socket name again */
    ARG_GETSOCKNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    /* Test server mode */
    sock = io.rc;

    /* Retrieve socket name, not bound */
    salen = sizeof(sa);
    ARG_GETSOCKNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    /* Should return error (socket not bound) */
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Initialize buffers for return values */
    memset (&sa, 0, sizeof(sa));
    SA_SET (0, ip_bcast, 0);
    salen = sizeof(sa) + 1;

    /* Bind socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Retrieve socket name, bound */
    ARG_GETSOCKNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETSOCKNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* IP address should be unspecified */
    TH_ASSERT  (memcmp (&sa.sin_addr, ip_unspec, 4) == 0);
    /* Address length should be updated */
    TH_ASSERT  (salen == sizeof(sa));
    /* Port number should be listening port */
    TH_ASSERT  (sa.sin_port == htons(DISCARD_PORT));

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* GetPeerName IO parameters */
typedef struct {
  int32_t      sock;
  SOCKADDR    *addr;
  int32_t     *addrlen;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
} IO_GETPEERNAME;

/* Assign arguments */
#define ARG_GETPEERNAME(_sock,_addr,_addrlen) \
  do {                                        \
    io.sock    = _sock;                       \
    io.addr    = (SOCKADDR *)_addr;           \
    io.addrlen = _addrlen;                    \
  } while (0)

/* GetPeerName worker thread */
__NO_RETURN static void Th_GetPeerName (IO_GETPEERNAME *io) {
  SOCKADDR_IN sa;
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP  | F_BIND | F_CONNECT |
                               F_LISTEN     | F_GETPEERNAME | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_BIND:
        /* Bind socket */
        SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
        io->rc = bsd->bind (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

      case F_CONNECT:
        /* Connect on socket */
        SA_SET (AF_INET, ip_server, DISCARD_PORT);
        io->rc = bsd->connect (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

       case F_LISTEN:
        /* Listen on socket */
        io->rc = bsd->listen (io->sock, 1);
        break;

      case F_GETPEERNAME:
        /* Get peer name  */
        io->rc = bsd->getpeername (io->sock, io->addr, io->addrlen);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketGetPeerName
\details
The test case \b BSD_SocketGetPeerName verifies the BSD \b getpeername function:
\code
int32_t (*getpeername) (int32_t sock, SOCKADDR *name, int32_t *namelen);
\endcode

Stream socket test  1:
 - Create stream socket
 - Connect to server, blocking mode
 - Check function parameters
 - Get peer name
 - Close socket
 - Get peer name, closed socket

Stream socket test  2:
 - Create stream socket
 - Get peer name, created socket
 - Bind socket
 - Get peer name, bound socket
 - Start listening
 - Get peer name, listening socket
 - Close socket

Datagram socket test:
 - Create datagram socket
 - Connect to server, enable packet filtering
 - Check function parameters
 - Get peer name
 - Close socket
 - Get peer name, closed socket
*/
void BSD_SocketGetPeerName (void) {
  SOCKADDR_IN    sa;
  int32_t        salen;
  osThreadId_t   worker;
  int32_t        rval;
  IO_GETPEERNAME io;
  int32_t        sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_GetPeerName, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    salen = sizeof(sa);
    ARG_GETPEERNAME (-1, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_GETPEERNAME (INT32_MIN, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_GETPEERNAME (INT32_MAX, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (addr = NULL) */
    ARG_GETPEERNAME (sock, NULL, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = NULL) */
    ARG_GETPEERNAME (sock, &sa, NULL);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = 15) */
    salen = sizeof(sa) - 1;
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    ARG_GETPEERNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Initialize buffers for return values */
    memset (&sa, 0, sizeof(sa));
    SA_SET (0, ip_bcast, 0);
    salen = sizeof(sa) + 1;

    /* Retrieve peer name */
    ARG_GETPEERNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* IP address should be the address of the server */
    TH_ASSERT  (memcmp (&sa.sin_addr, ip_server, 4) == 0);
    /* Address length should be updated */
    TH_ASSERT  (salen == sizeof(sa));
    /* Port number should be the DISCARD port */
    TH_ASSERT  (sa.sin_port == htons(DISCARD_PORT));

    /* Close stream socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Retrieve peer name again */
    ARG_GETPEERNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Get peer name, created socket */
    salen = sizeof(sa);
    ARG_GETPEERNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    /* Should return error (socket not connected) */
    TH_ASSERT  (io.rc == BSD_ENOTCONN);

    /* Bind socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Get peer name, bound socket */
    ARG_GETPEERNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    /* Should return error (socket not connected) */
    TH_ASSERT  (io.rc == BSD_ENOTCONN);

    /* Start listening */
    io.sock = sock;
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Get peer name, listening socket */
    ARG_GETPEERNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    /* Should return error (socket not connected) */
    TH_ASSERT  (io.rc == BSD_ENOTCONN);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to datagram server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    salen = sizeof(sa);
    ARG_GETPEERNAME (-1, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_GETPEERNAME (INT32_MIN, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_GETPEERNAME (INT32_MAX, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (addr = NULL) */
    ARG_GETPEERNAME (sock, NULL, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = NULL) */
    ARG_GETPEERNAME (sock, &sa, NULL);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (addrlen = 15) */
    salen = sizeof(sa) - 1;
    SA_SET (AF_INET, ip_server, DISCARD_PORT);
    ARG_GETPEERNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Initialize buffers for return values */
    memset (&sa, 0, sizeof(sa));
    SA_SET (0, ip_bcast, 0);
    salen = sizeof(sa) + 1;

    /* Retrieve peer name */
    ARG_GETPEERNAME (sock, &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* IP address should be the address of the server */
    TH_ASSERT  (memcmp (&sa.sin_addr, ip_server, 4) == 0);
    /* Address length should be updated */
    TH_ASSERT  (salen == sizeof(sa));
    /* Port number should be the DISCARD port */
    TH_ASSERT  (sa.sin_port == htons(DISCARD_PORT));

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Retrieve peer name again */
    ARG_GETPEERNAME (sock,  &sa, &salen);
    TH_EXECUTE (F_GETPEERNAME, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* GetOpt IO parameters */
typedef struct {
  int32_t      sock;
  int32_t      level;
  int32_t      optname;
  char        *optval;
  int32_t     *optlen;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
} IO_GETOPT;

/* Assign arguments */
#define ARG_GETOPT(_sock,_level,_optname,_optval,_optlen) \
  do {                                                    \
    io.sock    = _sock;                                   \
    io.level   = _level;                                  \
    io.optname = _optname;                                \
    io.optval  = _optval;                                 \
    io.optlen  = _optlen;                                 \
  } while (0)

/* GetOpt worker thread */
__NO_RETURN static void Th_GetOpt (IO_GETOPT *io) {
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP |
                               F_GETOPT     | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_GETOPT:
        /* Get socket options */
        io->rc = bsd->getsockopt (io->sock, io->level, io->optname, io->optval, io->optlen);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketGetOpt
\details
The test case \b BSD_SocketGetOpt verifies the BSD \b getsockopt function:
\code
int32_t (*getsockopt) (int32_t sock, int32_t level, int32_t optname, char *optval, int32_t *optlen);
\endcode

Stream socket test:
 - Create stream socket
 - Check function parameters
 - Get socket options
 - Close socket
 - Get socket options again, closed socket

Datagram socket test:
 - Create datagram socket
 - Get socket type
 - Close socket
 - Get socket type
*/
void BSD_SocketGetOpt (void) {
  int32_t      optval;
  int32_t      optlen;
  osThreadId_t worker;
  int32_t      rval;
  IO_GETOPT    io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_GetOpt, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Check parameter (socket = -1) */
    optlen = sizeof(optval);
    ARG_GETOPT (-1, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_GETOPT (INT32_MIN, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_GETOPT (INT32_MAX, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (level = -1) */
    ARG_GETOPT (sock, -1, SO_TYPE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (level = INT32_MIN) */
    ARG_GETOPT (sock, INT32_MIN, SO_TYPE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (level = INT32_MAX) */
    ARG_GETOPT (sock, INT32_MAX, SO_TYPE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (optname = -1) */
    ARG_GETOPT (sock, SOL_SOCKET, -1, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (optname = INT32_MIN) */
    ARG_GETOPT (sock, SOL_SOCKET, INT32_MIN, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (optname = INT32_MAX) */
    ARG_GETOPT (sock, SOL_SOCKET, INT32_MAX, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (optval = NULL) */
    ARG_GETOPT (sock, SOL_SOCKET, SO_TYPE, NULL, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (optlen = NULL) */
    ARG_GETOPT (sock, SOL_SOCKET, SO_TYPE, (char *)&optval, NULL);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (*optlen = 0) */
    optlen = 0;
    ARG_GETOPT (sock, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (*optlen = 5) */
    optlen = 5;
    ARG_GETOPT (sock, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  ((io.rc == 0) && (optlen == 4));

    /* Get option RCVTIMEO */
    optlen = sizeof(optval) + 1;
    optval = 0x72A5A2E1;
    ARG_GETOPT (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* Should be different from the initial value */
    TH_ASSERT  ((optval != 0x72A5A2E1) && (optlen == 4));

    /* Get option SNDTIMEO */
    optlen = sizeof(optval) + 1;
    optval = 0x72A5A2E1;
    ARG_GETOPT (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* Should be different from the initial value */
    TH_ASSERT  ((optval != 0x72A5A2E1) && (optlen == 4));

    /* Get option KEEPALIVE */
    optlen = sizeof(optval) + 1;
    optval = 0x72A5A2E1;
    ARG_GETOPT (sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* Should be different from the initial value */
    TH_ASSERT  ((optval != 0x72A5A2E1) && (optlen == 4));

    /* Get option BINDTODEVICE */
    optlen = sizeof(optval) + 1;
    optval = 0x72A5A2E1;
    ARG_GETOPT (sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* Should be unassigned device */
    TH_ASSERT  ((optval == 0) && (optlen == 4));

    /* Get option socket TYPE */
    optlen = sizeof(optval) + 1;
    optval = UINT32_MAX;
    ARG_GETOPT (sock, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* Should be SOCK_STREAM type */
    TH_ASSERT  ((optval == SOCK_STREAM) && (optlen == 4));

    /* Get option TCP_QUICKACK */
    optlen = sizeof(optval) + 1;
    optval = 0x72A5A2E1;
    ARG_GETOPT (sock, IPPROTO_TCP , TCP_QUICKACK, (char *)&optval,  &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* Should be enabled (default) */
    TH_ASSERT  ((optval == 1) && (optlen == 4));

    /* Get option TCP_FLOWCTRL */
    optlen = sizeof(optval) + 1;
    optval = 0x72A5A2E1;
    ARG_GETOPT (sock, IPPROTO_TCP , TCP_FLOWCTRL , (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* Should be disabled (default) */
    TH_ASSERT  ((optval == 0) && (optlen == 4));

    /* Get option TCP_KEEPIDLE */
    optlen = sizeof(optval) + 1;
    optval = 0x72A5A2E1;
    ARG_GETOPT (sock, IPPROTO_TCP , TCP_KEEPIDLE , (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* Should be 120 sec (default) */
    TH_ASSERT  ((optval == 120) && (optlen == 4));

    /* Close stream socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Get option socket TYPE again */
    optlen = sizeof(optval);
    ARG_GETOPT (sock, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Get option socket TYPE */
    optlen = sizeof(optval) + 1;
    optval = UINT32_MAX;
    ARG_GETOPT (sock, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    /* Should be SOCK_DGRAM type */
    TH_ASSERT  ((optval == SOCK_DGRAM) && (optlen == 4));

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Get option socket TYPE again */
    optlen = sizeof(optval);
    ARG_GETOPT (sock, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen);
    TH_EXECUTE (F_GETOPT, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  // Todo: test IPPROTO_IP and IPPROTO_IP6 levels !!!!

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* SetOpt IO parameters */
typedef struct {
  int32_t      sock;
  int32_t      level;
  int32_t      optname;
  const char  *optval;
  int32_t      optlen;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
} IO_SETOPT;

/* Assign arguments */
#define ARG_SETOPT(_sock,_level,_optname,_optval,_optlen) \
  do {                                                    \
    io.sock    = _sock;                                   \
    io.level   = _level;                                  \
    io.optname = _optname;                                \
    io.optval  = _optval;                                 \
    io.optlen  = _optlen;                                 \
  } while (0)

/* SetOpt worker thread */
__NO_RETURN static void Th_SetOpt (IO_SETOPT *io) {
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP | F_SETOPT | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_SETOPT:
        /* Set socket options */
        io->rc = bsd->setsockopt (io->sock, io->level, io->optname, io->optval, io->optlen);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketSetOpt
\details
The test case \b BSD_SocketSetOpt verifies the BSD \b setsockopt function:
\code
int32_t (*setsockopt) (int32_t sock, int32_t level, int32_t optname, const char *optval, int32_t optlen);
\endcode

Stream socket test:
 - Create stream socket
 - Check function parameters
 - Set socket options
 - Close socket
 - Set socket option again, closed socket

Datagram socket test:
 - Create datagram socket
 - Set socket options
 - Close socket
 - Set socket option again, closed socket
*/

void BSD_SocketSetOpt (void) {
  int32_t      optval;
  osThreadId_t worker;
  int32_t      rval;
  IO_SETOPT    io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_SetOpt, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Check parameter (socket = -1) */
    optval = 5000;
    ARG_SETOPT (-1, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_SETOPT (INT32_MIN, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_SETOPT (INT32_MAX, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (level = -1) */
    ARG_SETOPT (sock, -1, SO_RCVTIMEO, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (level = INT32_MIN) */
    ARG_SETOPT (sock, INT32_MIN, SO_RCVTIMEO, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (level = INT32_MAX) */
    ARG_SETOPT (sock, INT32_MAX, SO_RCVTIMEO, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (optname = -1) */
    ARG_SETOPT (sock, SOL_SOCKET, -1, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (optname = INT32_MIN) */
    ARG_SETOPT (sock, SOL_SOCKET, INT32_MIN, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (optname = INT32_MAX) */
    ARG_SETOPT (sock, SOL_SOCKET, INT32_MAX, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (optval = NULL) */
    ARG_SETOPT (sock, SOL_SOCKET, SO_RCVTIMEO, NULL, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (optlen = 0) */
    ARG_SETOPT (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, 0);
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (optlen = 2) */
    ARG_SETOPT (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, 2);
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Set option RCVTIMEO */
    optval = 5000;
    ARG_SETOPT (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Set option SNDTIMEO */
    optval = 2000;
    ARG_SETOPT (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Set option KEEPALIVE */
    optval = 1;
    ARG_SETOPT (sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Set option BINDTODEVICE */
    optval = NET_IF_CLASS_ETH+0;
    ARG_SETOPT (sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Set option socket TYPE (get only) */
    optval = SOCK_STREAM;
    ARG_SETOPT (sock, SOL_SOCKET, SO_TYPE, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Set option TCP_QUICKACK */
    optval = 1;
    ARG_SETOPT (sock, IPPROTO_TCP, TCP_QUICKACK, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Set option TCP_FLOWCTRL */
    optval = 1;
    ARG_SETOPT (sock, IPPROTO_TCP, TCP_FLOWCTRL , (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Set option TCP_KEEPIDLE */
    optval = 600;
    ARG_SETOPT (sock, IPPROTO_TCP, TCP_KEEPIDLE , (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Close stream socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Set option RCVTIMEO again */
    optval = 5000;
    ARG_SETOPT (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Set option RCVTIMEO */
    optval = 5000;
    ARG_SETOPT (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Close socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Set option RCVTIMEO again */
    ARG_SETOPT (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&optval, sizeof(optval));
    TH_EXECUTE (F_SETOPT, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* Close IO parameters */
typedef struct {
  int32_t      sock;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
} IO_CLOSE;

/* Assign arguments */
#define ARG_CLOSE(_sock) \
  do {                   \
    io.sock = _sock;     \
  } while (0)

/* Close worker thread */
__NO_RETURN static void Th_Close (IO_CLOSE *io) {
  SOCKADDR_IN sa;
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP | F_BIND |
                               F_CONNECT    | F_LISTEN     | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_BIND:
        /* Bind socket */
        SA_SET (AF_INET, ip_unspec, DISCARD_PORT);
        io->rc = bsd->bind (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

      case F_CONNECT:
        /* Connect on socket */
        SA_SET (AF_INET, ip_server, DISCARD_PORT);
        io->rc = bsd->connect (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

      case F_LISTEN:
        /* Listen on socket */
        io->rc = bsd->listen (io->sock, 1);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketClose
\details
The test case \b BSD_SocketClose verifies the BSD \b closesocket function:
\code
int32_t (*closesocket) (int32_t sock);
\endcode

Stream socket test 1:
 - Create stream socket
 - Bind socket
 - Connect to server
 - Check function parameters
 - Close socket
 - Close socket again

Stream socket test 2:
 - Create stream socket
 - Bind socket
 - Start listening
 - Close socket
 - Close socket again

Datagram socket test:
 - Create datagram socket
 - Bind socket
 - Check function parameters
 - Close socket
 - Close socket again
*/
void BSD_SocketClose (void) {
  osThreadId_t worker;
  int32_t      rval;
  IO_CLOSE     io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Close, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    ARG_CLOSE  (-1);
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_CLOSE  (INT32_MIN);
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_CLOSE  (INT32_MAX);
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Close socket */
    ARG_CLOSE  (sock);
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Close again, closed socket */
    ARG_CLOSE  (sock);
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Bind socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Start listening */
    io.sock = sock;
    TH_EXECUTE (F_LISTEN, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Close socket */
    ARG_CLOSE (sock);
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Close again, closed socket */
    ARG_CLOSE (sock);
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Bind socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Check parameter (socket = -1) */
    ARG_CLOSE  (-1);
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_CLOSE  (INT32_MIN);
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_CLOSE  (INT32_MAX);
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Close socket */
    ARG_CLOSE  (sock);
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Close again, closed socket */
    ARG_CLOSE  (sock);
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* Ioctl IO parameters */
typedef struct {
  int32_t      sock;
  int32_t      cmd;
  uint32_t    *argp;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
} IO_IOCTL;

/* Assign arguments */
#define ARG_IOCTL(_sock,_cmd,_argp) \
  do {                              \
    io.sock = _sock;                \
    io.cmd  = _cmd;                 \
    io.argp = _argp;                \
  } while (0)

/* Ioctl worker thread */
__NO_RETURN static void Th_Ioctl (IO_IOCTL *io) {
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP |
                               F_IOCTL      | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_IOCTL:
        /* Set socket control */
        io->rc = bsd->ioctlsocket (io->sock, io->cmd, (unsigned long *)io->argp);
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketIoctl
\details
The test case \b BSD_SocketIoctl verifies the BSD \b iostlsocket function:
\code
int32_t (*ioctlsocket) (int32_t sock, long cmd, unsigned long *argp);
\endcode

Stream socket test 1:
 - Create stream socket
 - Check function parameters
 - Close socket

Datagram socket test:
 - Create stream socket
 - Check function parameters
 - Close socket
*/
void BSD_SocketIoctl (void) {
  uint32_t     arg;
  osThreadId_t worker;
  int32_t      rval;
  IO_IOCTL     io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Ioctl, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Check parameter (socket = -1) */
    arg = 0;
    ARG_IOCTL  (-1, FIONBIO, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_IOCTL  (INT32_MIN, FIONBIO, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_IOCTL  (INT32_MAX, FIONBIO, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (cmd = -1) */
    ARG_IOCTL  (sock, -1, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (cmd = INT32_MIN) */
    ARG_IOCTL  (sock, INT32_MIN, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (cmd = INT32_MAX) */
    ARG_IOCTL  (sock, INT32_MAX, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (argp = NULL) */
    ARG_IOCTL  (sock, FIONBIO, NULL);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (argp = NULL) */
    arg = 1;
    ARG_IOCTL  (sock, FIONBIO, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Close socket */
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Ioctl again, closed socket */
    ARG_IOCTL  (sock, FIONBIO, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Check parameter (socket = -1) */
    arg = 0;
    ARG_IOCTL  (-1, FIONBIO, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MIN) */
    ARG_IOCTL  (INT32_MIN, FIONBIO, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (socket = INT32_MAX) */
    ARG_IOCTL  (INT32_MAX, FIONBIO, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_ESOCK);

    /* Check parameter (cmd = -1) */
    ARG_IOCTL  (sock, -1, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (cmd = INT32_MIN) */
    ARG_IOCTL  (sock, INT32_MIN, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (cmd = INT32_MAX) */
    ARG_IOCTL  (sock, INT32_MAX, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (argp = NULL) */
    ARG_IOCTL  (sock, FIONBIO, NULL);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EINVAL);

    /* Check parameter (argp = NULL) */
    arg = 1;
    ARG_IOCTL  (sock, FIONBIO, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Close socket */
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Ioctl again, closed socket */
    ARG_IOCTL  (sock, FIONBIO, &arg);
    TH_EXECUTE (F_IOCTL, BSD_TIMEOUT);
    /* Should return error (socket not created) */
    TH_ASSERT  (io.rc == BSD_ESOCK);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* Select IO parameters */
typedef struct {
  int32_t      nfds;
  fd_set      *rdfds;
  fd_set      *wrfds;
  fd_set      *errfds;
  timeval     *tout;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
  int32_t      sock;
  const char  *cmd;
} IO_SELECT;

/* Assign arguments */
#define ARG_SELECT(_n,_rd,_wr,_err,_tout) \
  do {                                    \
    io.nfds    = _n;                      \
    io.rdfds   = _rd;                     \
    io.wrfds   = _wr;                     \
    io.errfds  = _err;                    \
    io.tout    = _tout;                   \
  } while (0)

/* Select worker thread */
__NO_RETURN static void Th_Select (IO_SELECT *io) {
  SOCKADDR_IN sa;
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP | F_BIND | F_CONNECT | F_SELECT |
                               F_RECV | F_SEND | F_CLOSE | F_SEND_CTRL, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

     case F_BIND:
        /* Bind socket */
        SA_SET (AF_INET, ip_unspec, TEST_PORT);
        io->rc = bsd->bind (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

      case F_CONNECT:
        /* Connect on socket */
        SA_SET (AF_INET, ip_server, ECHO_PORT);
        io->rc = bsd->connect (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

      case F_SELECT:
        /* Select */
        io->rc = bsd->select (io->nfds, io->rdfds, io->wrfds, io->errfds, io->tout);
        break;

      case F_RECV:
        /* Recv on socket */
        io->rc = bsd->recv (io->sock, buffer, sizeof(buffer), MSG_DONTWAIT);
        break;

      case F_SEND:
        /* Send on socket */
        io->rc = bsd->send (io->sock, test_msg, sizeof(test_msg), 0);
        io->rc = (io->rc == sizeof(test_msg)) ? 0 : BSD_ERROR;
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;

      case F_SEND_CTRL: {
        /* Send control command to TestAssistant */
        int32_t sock = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        SA_SET (AF_INET, ip_server, ASSISTANT_PORT);
        bsd->connect (sock, (SOCKADDR *)&sa, sizeof(sa));
        io->rc = bsd->send (sock, io->cmd, strlen(io->cmd), 0);
        bsd->closesocket (sock);
        osDelay (10);
      } break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_SocketSelect
\details
The test case \b BSD_SocketSelect the BSD \b select function:
\code
int32_t (*select) (int32_t nfds, fd_set *readfds, fd_set *writefds,
                                 fd_set *errorfds, timeval *timeout);
\endcode

Function test:
 - Check function parameters
 - Resolve host
 - Resolve non-existent host

\note
This test requires internet connectivity to DNS server.
*/
void BSD_SocketSelect (void) {
  fd_set       rd_fds;
  fd_set       wr_fds;
  fd_set       err_fds;
  timeval      tv;
  osThreadId_t worker;
  int32_t      rval,sock;
  IO_SELECT    io;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Select, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  FD_ZERO (&rd_fds);
  FD_ZERO (&wr_fds);
  FD_ZERO (&err_fds);

  /* Check parameter (rdfds,wrfds,errfds = NULL) */
  ARG_SELECT (2, NULL, NULL, NULL, NULL);
  TH_EXECUTE (F_SELECT, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Check parameter (nfds = 0) */
  ARG_SELECT (0, &rd_fds, &wr_fds, &err_fds, NULL);
  TH_EXECUTE (F_SELECT, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == BSD_EINVAL);

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Receive data nonblocking */
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EWOULDBLOCK);

    /* Select (tout = 500ms) */
    tv.tv_sec  = 0;
    tv.tv_usec = 500000;
    FD_SET (sock, &rd_fds);
    ARG_SELECT (sock+1, &rd_fds, NULL, NULL, &tv);
    TH_EXECUTE (F_SELECT, 500 + BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    TH_ASSERT  (FD_ISSET (sock, &rd_fds) == 0);

    /* Send data to be echoed back */
    io.sock = sock;
    TH_EXECUTE (F_SEND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Select (tout = 300ms) */
    tv.tv_sec  = 0;
    tv.tv_usec = 300000;
    FD_SET (sock, &rd_fds);
    ARG_SELECT (sock+1, &rd_fds, NULL, NULL, &tv);
    TH_EXECUTE (F_SELECT, BSD_TIMEOUT);
    /* One socket should be readable */
    TH_ASSERT  (io.rc == 1);
    /* Requested socket should be flagged */
    TH_ASSERT  (FD_ISSET (sock, &rd_fds) != 0);

    /* Receive data nonblocking */
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == sizeof(test_msg));

    /* Close socket */
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to datagram server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive data nonblocking */
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EWOULDBLOCK);

    /* Select (tout = 500ms) */
    tv.tv_sec  = 0;
    tv.tv_usec = 500000;
    FD_SET (sock, &rd_fds);
    ARG_SELECT (sock+1, &rd_fds, NULL, NULL, &tv);
    TH_EXECUTE (F_SELECT, 500 + BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    TH_ASSERT  (FD_ISSET (sock, &rd_fds) == 0);

    /* Send data to be echoed back */
    io.sock = sock;
    TH_EXECUTE (F_SEND, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 0);

    /* Select (tout = 300ms) */
    tv.tv_sec  = 0;
    tv.tv_usec = 300000;
    FD_SET (sock, &rd_fds);
    ARG_SELECT (sock+1, &rd_fds, NULL, NULL, NULL);
    TH_EXECUTE (F_SELECT, BSD_TIMEOUT);
    /* One socket should be readable */
    TH_ASSERT  (io.rc == 1);
    /* Requested socket should be flagged */
    TH_ASSERT  (FD_ISSET (sock, &rd_fds) != 0);

    /* Receive data nonblocking */
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == sizeof(test_msg));

    /* Close socket */
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Bind socket */
    io.sock = sock;
    TH_EXECUTE (F_BIND, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Receive data nonblocking */
    TH_EXECUTE (F_RECV, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == BSD_EWOULDBLOCK);

    /* Select (tout = 200ms) */
    tv.tv_sec  = 0;
    tv.tv_usec = 200000;
    FD_SET (sock, &rd_fds);
    ARG_SELECT (sock+1, &rd_fds, NULL, NULL, &tv);
    TH_EXECUTE (F_SELECT, 200 + BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);
    TH_ASSERT  (FD_ISSET (sock, &rd_fds) == 0);

    /* Request from remote server to send us a test message */
    io.cmd = CMD_CONNECT_UDP;
    TH_EXECUTE (F_SEND_CTRL, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc > 0);

    /* Select (tout = 500ms) */
    tv.tv_sec  = 0;
    tv.tv_usec = 500000;
    FD_SET (sock, &rd_fds);
    ARG_SELECT (sock+1, &rd_fds, NULL, NULL, &tv);
    TH_EXECUTE (F_SELECT, 500 + BSD_TIMEOUT);
    /* One socket should be readable */
    TH_ASSERT  (io.rc == 1);
    /* Requested socket should be flagged */
    TH_ASSERT  (FD_ISSET (sock, &rd_fds) != 0);

    /* Receive SockServer id string */
    TH_EXECUTE (F_RECV, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc > 0);

    /* Close socket */
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* GetHostByName IO parameters */
typedef struct {
  const char  *name;
  int32_t     *err;
  HOSTENT     *host;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
} IO_GETHOST;

/* Assign arguments */
#define ARG_GETHOST(_name,_err) \
  do {                          \
    io.name   = _name;          \
    io.err    = _err;           \
  } while (0)

/* GetHostByName worker thread */
__NO_RETURN static void Th_GetHostByName (IO_GETHOST *io) {
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_GETHOSTBYNAME, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_GETHOSTBYNAME:
        /* Resolve host */
        io->host = bsd->gethostbyname (io->name, io->err);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_GetHostByName
\details
The test case \b BSD_GetHostByName the BSD \b gethostbyname function:
\code
HOSTENT *(*gethostbyname) (const char *name, int32_t *err);
\endcode

Function test:
 - Check function parameters
 - Resolve host
 - Resolve non-existent host

\note
This test requires internet connectivity to DNS server.
*/
void BSD_GetHostByName (void) {
  int32_t      err;
  osThreadId_t worker;
  int32_t      rval;
  IO_GETHOST   io;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_GetHostByName, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Check parameter (name = NULL) */
  err = INT32_MAX;
  ARG_GETHOST(NULL, &err);
  TH_EXECUTE (F_GETHOSTBYNAME, BSD_TIMEOUT);
  TH_ASSERT  (err == BSD_EINVAL);
  TH_ASSERT  (io.host == NULL);

  /* Resolve valid host */
  err = INT32_MAX;
  io.host = NULL;
  ARG_GETHOST("www.arm.com", &err);
  TH_EXECUTE (F_GETHOSTBYNAME, BSD_TIMEOUT_LONG);
  TH_ASSERT (err == 0);
  /* IP address should be valid */
  TH_ASSERT (io.host != NULL);
  if (io.host != NULL) {
    TH_ASSERT (io.host->h_addrtype == AF_INET);
    TH_ASSERT (io.host->h_length   == 4);
    TH_ASSERT (((IN_ADDR *)io.host->h_addr_list[0])->s_addr != INADDR_ANY);
  }

  /* Resolve local host from IP */
  err = INT32_MAX;
  io.host = (void *)0xCCCCCCCC;
  ARG_GETHOST("localhost", &err);
  TH_EXECUTE (F_GETHOSTBYNAME, BSD_TIMEOUT);
  /* IP address should be valid */
  TH_ASSERT (err == 0);
  TH_ASSERT (io.host != NULL);
  if (io.host != NULL) {
    TH_ASSERT (io.host->h_addrtype == AF_INET);
    TH_ASSERT (io.host->h_length   == 4);
    TH_ASSERT (((IN_ADDR *)io.host->h_addr_list[0])->s_addr == htonl(INADDR_LOOPBACK));
  }

  /* Resolve non-existent host */
  err = INT32_MAX;
  io.host = (void *)0xCCCCCCCC;
  ARG_GETHOST("non.existent.host", &err);
  TH_EXECUTE (F_GETHOSTBYNAME, BSD_TIMEOUT_LONG);
  /* Should return error (host not found) */
  TH_ASSERT  (err == BSD_EHOSTNOTFOUND);
  TH_ASSERT  (io.host == NULL);

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* Ping IO parameters */
typedef struct {
  const char  *target;
  uint32_t     flags;
  int32_t        rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
} IO_PING;

/* Assign arguments */
#define ARG_PING(_target,_flags) \
  do {                           \
    io.target = _target;         \
    io.flags  = _flags;          \
  } while (0)

/* Ping worker thread */
__NO_RETURN static void Th_Ping (IO_PING *io) {
  uint32_t flags,xid;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_PING, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_PING:
        /* Ping host */
        io->rc = bsd->ping (io->target, io->flags);
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_Ping
\details
The test case \b BSD_PingEcho verifies the Network \b netPing_EchoX function:
\code
netStatus (*ping) (const char *target, uint32_t flags);
\endcode

Function test:
 - Check function parameters
 - Ping host
*/
void BSD_PingEcho (void) {
  osThreadId_t worker;
  int32_t      rval;
  IO_PING      io;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Ping, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Check parameter (target = NULL) */
  ARG_PING   (NULL, 0);
  TH_EXECUTE (F_PING, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == netInvalidParameter);

  /* Ping non-existent host */
  ARG_PING   ("non.existent.host", 0);
  TH_EXECUTE (F_PING, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == netDnsResolverError);

  /* Ping server */
  ARG_PING   (MW_CV_NET_SOCKSERVER_IP, 0);
  TH_EXECUTE (F_PING, BSD_TIMEOUT);
  TH_ASSERT  (io.rc == netOK);

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/**
\defgroup bsd_sock_op BSD Socket Operation
\details
These tests verify operation of the BSD socket functions.
*/

/* Worker thread is used for the following tests:
   - BSD_Transfer_Fixed
   - BSD_Transfer_Incremental
   - BSD_Send_Fragmented
   - BSD_Recv_Fragmented
   - BSD_Test_Speed
   - BSD_Concurrent_Socket
*/

/* Transfer IO parameters */
typedef struct {
  int32_t      sock;
  uint32_t     len;
  uint32_t     size;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
  int32_t      tcp;
} IO_TRANSFER;

/* Assign arguments */
#define ARG_TRANSFER(_sock,_len,_size) \
  do {                                 \
    io.sock = _sock;                   \
    io.len  = _len;                    \
    io.size = _size;                   \
  } while (0)

/* Transfer worker thread */
__NO_RETURN static void Th_Transfer (IO_TRANSFER *io) {
  SOCKADDR_IN sa;
  uint32_t flags,xid,i,j;
  int32_t  rc = 0;

  for (;;) {
    /* Wait for the signal to select and execute the function */
    flags = osThreadFlagsWait (F_CREATE_TCP | F_CREATE_UDP |
                               F_CONNECT    | F_CLOSE      |
                               F_XFER_FIXED | F_XFER_INCR  |
                               F_SEND_FRAG  | F_RECV_FRAG, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_CREATE_UDP:
        /* Create datagram socket */
        io->rc = bsd->socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;

      case F_CONNECT:
        /* Connect on socket */
        SA_SET (AF_INET, ip_server, ECHO_PORT);
        io->rc = bsd->connect (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;

      case F_XFER_FIXED:
        /* Transfer Fixed size blocks */
        memset (buffer, 0xCC, io->len);
        /* Send and receive in small blocks */
        for (i = 0; i < io->len; i += io->size) {
          rc = bsd->send (io->sock, &test_buf[i], io->size, 0);
          if (rc <= 0) break;
          for (j = 0; j < io->size; j += (uint32_t)rc) {
            /* Returns any data available, up to requested amount */
            rc = bsd->recv (io->sock, &buffer[i+j], io->size-j, 0);
            if ((rc <= 0) || !io->tcp) break;
          }
          if (rc <= 0) break;
        }
        if (memcmp (buffer, test_buf, io->len) == 0) {
          rc = (int32_t)i;
        }
        io->rc = rc;
        break;

      case F_XFER_INCR:
        /* Transfer Increased size blocks */
        memset (buffer, 0xCC, io->len);
        /* Send and receive in enlarged block sizes */
        for (i = 0; i < io->len; i += io->size++) {
          rc = bsd->send (io->sock, &test_buf[i], io->size, 0);
          if (rc <= 0) break;
          rc = bsd->recv (io->sock, &buffer[i], io->size, 0);
          if (rc <= 0) break;
        }
        if (memcmp (buffer, test_buf, io->len) == 0) {
          rc = (int32_t)i;
        }
        io->rc = rc;
        break;

      case F_SEND_FRAG:
        /* Send Fragmented blocks */
        memset (buffer, 0xCC, io->len);
        /* Send in small blocks */
        for (i = 0; i < io->len; i += io->size) {
          rc = bsd->send (io->sock, &test_buf[i], io->size, 0);
          if (rc <= 0) break;
        }
        /* Receive in single block */
        if (rc > 0) {
          /* Small delay that blocks are received */
          osDelay (100);
          for (i = 0; i < io->len; i += (uint32_t)rc) {
            /* Returns any data available, up to requested amount */
            rc = bsd->recv (io->sock, &buffer[i], io->len-i, 0);
            if (rc <= 0) break;
          }
          if (memcmp (buffer, test_buf, io->len) == 0) {
            rc = (int32_t)i;
          }
        }
        io->rc = rc;
        break;

      case F_RECV_FRAG:
        /* Receive Fragmented blocks */
        memset (buffer, 0xCC, io->len);
        /* Send single block */
        rc = bsd->send (io->sock, test_buf, io->len, 0);
        if (rc > 0) {
          osDelay (100);
          /* Receive in small blocks */
          for (i = 0; i < io->len; i += io->size) {
            for (j = 0; j < io->size; j += (uint32_t)rc) {
              /* Returns any data available, up to requested amount */
              rc = bsd->recv (io->sock, &buffer[i+j], io->size-j, 0);
              if (rc <= 0) break;
            }
            if (rc <= 0) break;
          }
          if (memcmp (buffer, test_buf, io->len) == 0) {
            rc = (int32_t)i;
          }
        }
        io->rc = rc;
        break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_Transfer_Fixed
\details
The test case \b BSD_Transfer_Fixed verifies data transfer in fixed size blocks.

Stream socket test:
 - Create stream socket
 - Transfer 128 blocks of   16 bytes
 - Transfer  32 blocks of   64 bytes
 - Transfer   8 blocks of  256 bytes
 - Transfer   2 blocks of 1024 bytes
 - Transfer   1 block  of 2048 bytes
 - Close socket

Datagram socket test:
 - Create datagram socket
 - Transfer 128 blocks of   16 bytes
 - Transfer  32 blocks of   64 bytes
 - Transfer   8 blocks of  256 bytes
 - Transfer   2 blocks of 1024 bytes
 - Transfer   1 block  of 1460 bytes
 - Close socket
*/
void BSD_Transfer_Fixed (void) {
  osThreadId_t worker;
  int32_t      rval;
  IO_TRANSFER  io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Transfer, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.tcp  = 1;
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Transfer 16-byte block(s) */
    ARG_TRANSFER (sock, 2048, 16);
    TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2048);

    /* Transfer 64-byte block(s) */
    ARG_TRANSFER (sock, 2048, 64);
    TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2048);

    /* Transfer 256-byte block(s) */
    ARG_TRANSFER (sock, 2048, 256);
    TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2048);

    /* Transfer 1024-byte block(s) */
    ARG_TRANSFER (sock, 2048, 1024);
    TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2048);

    /* Transfer 2048-byte block */
    ARG_TRANSFER (sock, 2048, 2048);
    TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2048);

    /* Close stream socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to datagram server */
    io.tcp  = 0;
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Transfer 16-byte block(s) */
    ARG_TRANSFER (sock, 2048, 16);
    TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2048);

    /* Transfer 64-byte block(s) */
    ARG_TRANSFER (sock, 2048, 64);
    TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2048);

    /* Transfer 256-byte block(s) */
    ARG_TRANSFER (sock, 2048, 256);
    TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2048);

    /* Transfer 1024-byte block(s) */
    ARG_TRANSFER (sock, 2048, 1024);
    TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2048);

    /* Transfer 1460-byte block */
    ARG_TRANSFER (sock, 1460, 1460);
    TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1460);

    /* Close datagram socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/**
\brief  Test case: BSD_Transfer_Incremental
\details
The test case \b BSD_Transfer_Incremental verifies data transfer in ascending size blocks.
Each subsequent block that the socket sends is one byte larger than the previous block.

Stream socket test:
 - Create stream socket
 - Transfer 51 blocks of   1 -  50 bytes
 - Transfer 30 blocks of  51 -  80 bytes
 - Transfer 20 blocks of  81 - 100 bytes
 - Transfer 13 blocks of 120 - 132 bytes
 - Transfer  8 blocks of 252 - 259 bytes
 - Transfer  4 blocks of 510 - 513 bytes
 - Close socket

Datagram socket test:
 - Create datagram socket
 - Transfer 51 blocks of   1 -  50 bytes
 - Transfer 30 blocks of  51 -  80 bytes
 - Transfer 20 blocks of  81 - 100 bytes
 - Transfer 13 blocks of 120 - 132 bytes
 - Transfer  8 blocks of 252 - 259 bytes
 - Transfer  4 blocks of 510 - 513 bytes
 - Close socket
*/
void BSD_Transfer_Incremental (void) {
  osThreadId_t worker;
  int32_t      rval;
  IO_TRANSFER  io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Transfer, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Transfer 1 byte - 50 byte blocks */
    ARG_TRANSFER (sock, 1275, 1);
    TH_EXECUTE (F_XFER_INCR, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1275);

    /* Transfer 51 byte - 80-byte blocks */
    ARG_TRANSFER (sock, 1965, 51);
    TH_EXECUTE (F_XFER_INCR, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1965);

    /* Transfer 81 byte - 100 byte blocks */
    ARG_TRANSFER (sock, 1810, 81);
    TH_EXECUTE (F_XFER_INCR, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1810);

    /* Transfer 120 byte - 132 byte blocks */
    ARG_TRANSFER (sock, 1905, 120);
    TH_EXECUTE (F_XFER_INCR, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1905);

    /* Transfer 252 byte - 259 byte blocks */
    ARG_TRANSFER (sock, 2044, 252);
    TH_EXECUTE (F_XFER_INCR, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2044);

    /* Transfer 510 byte - 513 byte blocks */
    ARG_TRANSFER (sock, 2046, 510);
    TH_EXECUTE (F_XFER_INCR, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2046);

    /* Close stream socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to datagram server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Transfer 1 byte - 50 byte blocks */
    ARG_TRANSFER (sock, 1275, 1);
    TH_EXECUTE (F_XFER_INCR, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1275);

    /* Transfer 51 byte - 80-byte blocks */
    ARG_TRANSFER (sock, 1965, 51);
    TH_EXECUTE (F_XFER_INCR, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1965);

    /* Transfer 81 byte - 100 byte blocks */
    ARG_TRANSFER (sock, 1810, 81);
    TH_EXECUTE (F_XFER_INCR, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1810);

    /* Transfer 120 byte - 132 byte blocks */
    ARG_TRANSFER (sock, 1905, 120);
    TH_EXECUTE (F_XFER_INCR, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1905);

    /* Transfer 252 byte - 259 byte blocks */
    ARG_TRANSFER (sock, 2044, 252);
    TH_EXECUTE (F_XFER_INCR, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2044);

    /* Transfer 510 byte - 513 byte blocks */
    ARG_TRANSFER (sock, 2046, 510);
    TH_EXECUTE (F_XFER_INCR, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2046);

    /* Close datagram socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/**
\brief  Test case: BSD_Send_Fragmented
\details
The test case \b BSD_Send_Fragmented verifies data transfer in chunks.

Stream socket test:
 - Create stream socket
 - Send 16 blocks of   16 bytes, receive 1 block of  256 bytes
 - Send 16 blocks of   64 bytes, receive 1 block of 1024 bytes
 - Send  5 blocks of  256 bytes, receive 1 block of 1280 bytes
 - Send  2 blocks of 1024 bytes, receive 1 block of 2048 bytes
 - Close socket
*/
void BSD_Send_Fragmented (void) {
  osThreadId_t worker;
  int32_t      rval;
  IO_TRANSFER  io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Transfer, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Transfer 16-byte block(s) */
    ARG_TRANSFER (sock, 256, 16);
    TH_EXECUTE (F_SEND_FRAG, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 256);

    /* Transfer 64-byte block(s) */
    ARG_TRANSFER (sock, 1024, 64);
    TH_EXECUTE (F_SEND_FRAG, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1024);

    /* Transfer 256-byte block(s) */
    ARG_TRANSFER (sock, 1280, 256);
    TH_EXECUTE (F_SEND_FRAG, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1280);

    /* Transfer 1024-byte block(s) */
    ARG_TRANSFER (sock, 2048, 1024);
    TH_EXECUTE (F_SEND_FRAG, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2048);

    /* Close stream socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/**
\brief  Test case: BSD_Recv_Fragmented
\details
The test case \b BSD_Recv_Fragmented verifies data transfer in chunks.

Stream socket test:
 - Create stream socket
 - Send block of  256 bytes, receive 16 blocks of   16 bytes
 - Send block of 1024 bytes, receive 16 blocks of   64 bytes
 - Send block of 1280 bytes, receive  5 blocks of  256 bytes
 - Send block of 2048 bytes, receive  2 blocks of 1024 bytes
 - Close socket
*/
void BSD_Recv_Fragmented (void) {
  osThreadId_t worker;
  int32_t      rval;
  IO_TRANSFER  io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Transfer, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Transfer 16-byte block(s) */
    ARG_TRANSFER (sock, 256, 16);
    TH_EXECUTE (F_RECV_FRAG, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 256);

    /* Transfer 64-byte block(s) */
    ARG_TRANSFER (sock, 1024, 64);
    TH_EXECUTE (F_RECV_FRAG, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1024);

    /* Transfer 256-byte block(s) */
    ARG_TRANSFER (sock, 1280, 256);
    TH_EXECUTE (F_RECV_FRAG, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 1280);

    /* Transfer 1024-byte block(s) */
    ARG_TRANSFER (sock, 2048, 1024);
    TH_EXECUTE (F_RECV_FRAG, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc == 2048);

    /* Close stream socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/**
\brief  Test case: BSD_Test_Speed
\details
The test case \b BSD_Test_Speed tests data transfer speed.

Stream socket test:
 - Create stream socket
 - Transfer for 4 seconds, send and receive
 - Calculate transfer rate
 - Close socket

Datagram socket test:
 - Create datagram socket
 - Transfer for 4 seconds, send and receive
 - Calculate transfer rate
 - Close socket
*/
void BSD_Test_Speed (void) {
  uint32_t     ticks,tout;
  osThreadId_t worker;
  int32_t      rval,n_bytes;
  IO_TRANSFER  io;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Transfer, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to stream server */
    io.tcp  = 1;
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Transfer for 4 seconds */
    tout  = SYSTICK_MS(4000);
    ticks = GET_SYSTICK();
    n_bytes = 0;
    do {
      ARG_TRANSFER (sock, 1420, 1420);
      TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT);
      if (io.rc > 0) n_bytes += io.rc;
      else           break;
    } while (GET_SYSTICK() - ticks < tout);
    /* Check transfer rate */
    TH_ASSERT (n_bytes > 10000);
#if MIN_BYTES > 0
    if ((rval != 0) && (n_bytes < MIN_BYTES*2)) {
      uint32_t rate = n_bytes / 2000;
      DETAIL_INFO("Slow Transfer rate (%d KB/s)", rate);
    }
#endif

    /* Close stream socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect to datagram server */
    io.tcp  = 0;
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Transfer for 4 seconds */
    tout  = SYSTICK_MS(4000);
    ticks = GET_SYSTICK();
    n_bytes = 0;
    do {
      ARG_TRANSFER (sock, 1460, 1460);
      TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT);
      if (io.rc > 0) n_bytes += io.rc;
      else           break;
    } while (GET_SYSTICK() - ticks < tout);
    /* Check transfer rate */
    TH_ASSERT (n_bytes > 10000);
#if MIN_BYTES > 0
    if ((rval != 0) && (n_bytes < MIN_BYTES*2)) {
      uint32_t rate = n_bytes / 2000;
      DETAIL_INFO("Slow Transfer rate (%d KB/s)", rate);
    }
#endif

    /* Close datagram socket */
    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/* Sidekick IO parameters */
typedef struct {
  int32_t  sock;
  uint32_t count;
  /* Control */
  osThreadId_t owner;
} IO_SIDEKICK;

/* Concurrent coworker thread */
__NO_RETURN static void Th_Sidekick (IO_SIDEKICK *io2) {
  uint8_t buff[48];
  int32_t rc;

  for (;;) {
    /* Loop interval 100 ms */
    if (osThreadFlagsWait (X_STOP, osFlagsWaitAny, 100) == X_STOP) {
      /* Stop command received, shutdown */
      break;
    }
    memset (buff, 0xCC, sizeof(buff));
    rc = bsd->send (io2->sock, test_msg, sizeof(test_msg), 0);
    if (rc <= 0) break;
    rc = bsd->recv (io2->sock, (char *)buff, sizeof(test_msg), 0);
    if (rc <= 0) break;
    if (memcmp (buff, test_msg, sizeof(test_msg)) == 0) {
      io2->count += sizeof(test_msg);
    }
  }
  /* Owner deletes this thread */
  osThreadFlagsSet (io2->owner, TH_IDLE);
  while (1) osDelay (osWaitForever);
}

/**
\brief  Test case: BSD_Concurrent_Socket
\details
The test case \b BSD_Concurrent_Socket verifies transfer of two concurrent sockets.

Stream socket test:
 - Create two stream sockets
 - Start transfer on 2nd socket with 100ms intervals
 - Transfer on main socket, full speed
 - Calculate transfer rate
 - Close sockets

Datagram socket test:
 - Create datagram and stream sockets
 - Start transfer on stream socket with 100ms intervals
 - Transfer on main socket, full speed
 - Calculate transfer rate
 - Close sockets
\note
The test runs with a coherent thread, that performs an additional stream socket io.
*/
void BSD_Concurrent_Socket (void) {
  uint32_t     ticks,tout;
  osThreadId_t worker,spawn;
  int32_t      rval,n_bytes;
  IO_TRANSFER  io;
  IO_SIDEKICK  io2;
  int32_t      sock;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_Transfer, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* The test connects two stream sockets to the ECHO server and then    */
  /* performs simultaneous data transfer. The main socket transmits at   */
  /* full speed, and the other socket sends messages at 100ms intervals. */
  /* Both sockets record the number of bytes of data transferred, and    */
  /* the transfer rate is calculated.                                    */

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Create 2nd stream socket */
    TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
    ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
    io2.sock  = io.rc;
    io2.count = 0;

    /* Connect sockets */
    io.tcp  = 1;
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    io.sock = io2.sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Create spawned thread */
    io2.owner = io.owner;
    spawn = osThreadNew ((osThreadFunc_t)Th_Sidekick, &io2, NULL);
    ASSERT_TRUE (spawn != NULL, "");

    /* Transfer for 4 seconds */
    tout  = SYSTICK_MS(4000);
    ticks = GET_SYSTICK();
    n_bytes = 0;
    do {
      ARG_TRANSFER (sock, 1420, 1420);
      TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT);
      if (io.rc > 0) n_bytes += io.rc;
      else           break;
    } while (GET_SYSTICK() - ticks < tout);
    /* Check transfer rate */
    TH_ASSERT (n_bytes > 10000);
#if MIN_BYTES > 0
    if ((rval != 0) && (n_bytes < MIN_BYTES*2)) {
      uint32_t rate = n_bytes / 2000;
      DETAIL_INFO("Slow Transfer rate (%d KB/s)", rate);
    }
#endif
    /* Check auxiliary transfer rate */
    ASSERT_TRUE(io2.count != 0, "Auxiliary transfer failed");
    if (io2.count < 880) {
      DETAIL_INFO("Auxiliary Transfer rate low");
    }

    /* Signal spawned thread to stop operation */
    osThreadFlagsSet (spawn, X_STOP);
    osThreadFlagsWait(TH_IDLE, osFlagsWaitAny, 1000);

    /* Terminate spawned thread */
    osThreadTerminate (spawn);

    /* Close stream sockets */
    io.sock = io2.sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* The test connects datagram and stream sockets to the ECHO server     */
  /* and then performs simultaneous data transfer. The datagram socket    */
  /* transmits at full speed, and the stream socket sends messages at     */
  /* 100ms intervals. The number of bytes of transferred data is recorded */
  /* and the rate of transmission is calculated.                          */

  /* Create datagram socket */
  TH_EXECUTE (F_CREATE_UDP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Datagram Socket not created");
  if (io.rc >= 0) {
    sock = io.rc;

    /* Connect datagram socket */
    io.tcp  = 0;
    io.sock = sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    /* Create stream socket */
    TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
    ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
    io2.sock  = io.rc;
    io2.count = 0;

    /* Connect stream socket */
    io.sock = io2.sock;
    TH_EXECUTE (F_CONNECT, BSD_TIMEOUT_CONNECT);
    TH_ASSERT  (io.rc == 0);

    /* Create spawned thread */
    io2.owner = io.owner;
    spawn = osThreadNew ((osThreadFunc_t)Th_Sidekick, &io2, NULL);
    ASSERT_TRUE (spawn != NULL, "");

    /* Transfer for 4 seconds */
    tout  = SYSTICK_MS(4000);
    ticks = GET_SYSTICK();
    n_bytes = 0;
    do {
      ARG_TRANSFER (sock, 1460, 1460);
      TH_EXECUTE (F_XFER_FIXED, BSD_TIMEOUT);
      if (io.rc > 0) n_bytes += io.rc;
      else           break;
    } while (GET_SYSTICK() - ticks < tout);
    /* Check transfer rate */
    TH_ASSERT (n_bytes > 10000);
#if MIN_BYTES > 0
    if ((rval != 0) && (n_bytes < MIN_BYTES*2)) {
      uint32_t rate = n_bytes / 2000;
      DETAIL_INFO("Slow Transfer rate (%d KB/s)", rate);
    }
#endif
    /* Check auxiliary transfer rate */
    ASSERT_TRUE(io2.count != 0, "Auxiliary transfer failed");
    if (io2.count < 880) {
      DETAIL_INFO("Auxiliary Transfer rate low");
    }

    /* Signal spawned thread to stop operation */
    osThreadFlagsSet (spawn, X_STOP);
    osThreadFlagsWait(TH_IDLE, osFlagsWaitAny, 1000);

    /* Terminate spawned thread */
    osThreadTerminate (spawn);

    /* Close sockets */
    io.sock = io2.sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    io.sock = sock;
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/

/* TestAssistant commands */
#define CMD_SEND_TCP        "SEND TCP,1420,4000"
#define CMD_RECV_TCP        "RECV TCP,1420"
#define TEST_BSIZE          1420

/* StreamRate IO parameters */
typedef struct {
  int32_t      sock;
  int32_t      rc;
  /* Control */
  osThreadId_t owner;
  uint32_t     xid;
  int32_t      loss;
  const char  *cmd;
} IO_STREAMRATE;

/* StreamRate coworker thread */
__NO_RETURN static void Th_StreamRate (IO_STREAMRATE *io) {
  uint32_t flags,xid,ticks,tout;
  int32_t  n,rc,i,val;

  for (;;) {
    flags = osThreadFlagsWait (F_CREATE_TCP | F_DOWNLOAD | F_UPLOAD |
                               F_SEND_CTRL  | F_CLOSE, osFlagsWaitAny, osWaitForever);
    xid   = io->xid;
    switch (flags) {
      case F_CREATE_TCP:
        /* Create stream socket */
        io->rc = bsd->socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;

      case F_DOWNLOAD: {
        /* Downstream test, server is sender */
        /* Enable delayed acknowledge */
        char optval = 0;
        bsd->setsockopt (io->sock, IPPROTO_TCP, TCP_QUICKACK, (char *)&optval, sizeof(optval));

        for (n = 0; ; n += rc) {
          rc = bsd->recv (io->sock, buffer, TEST_BSIZE, 0);
          if (strncmp (buffer, "STAT", 4) == 0) {
            /* Server completed the test */
            val = net_atoi (buffer+4, NULL);
            if (val > n) io->loss = val - n;
            break;
          }
          if (rc <= 0) break;
        }
        io->rc = n;
      } break;

      case F_UPLOAD:
        /* Upstream test, server is receiver */
        n = net_strcpy (buffer, "Block[0]");
        memset (buffer+n, 'a', TEST_BSIZE-n);
        osDelay (20);
        tout  = SYSTICK_MS(4000);
        ticks = GET_SYSTICK();
        i = n = 0;
        do {
          rc = net_itoa (buffer+6, ++i);
          buffer[6+rc] = ']';
          rc = bsd->send (io->sock, buffer, TEST_BSIZE, 0);
          if (rc > 0) n += rc;
        } while (GET_SYSTICK() - ticks < tout);
        osDelay (300);
        rc  = net_strcpy (buffer,    "STOP ");
        rc += net_itoa   (buffer+rc, n);
        rc += net_strcpy (buffer+rc, " bytes.");
        bsd->send (io->sock, buffer, rc, 0);
        /* Receive report from server */
        bsd->recv (io->sock, buffer, TEST_BSIZE, 0);
        if (strncmp (buffer, "STAT", 4) == 0) {
          val = net_atoi (buffer+4, NULL);
          if (n > val) io->loss = n - val;
        }
        io->rc = n;
        break;

      case F_CLOSE:
        /* Close socket */
        io->rc = bsd->closesocket (io->sock);
        break;

      case F_SEND_CTRL: {
        /* Send control command to TestAssistant */
        SOCKADDR_IN  sa;
        SA_SET (AF_INET, ip_server, ASSISTANT_PORT);
        bsd->connect (io->sock, (SOCKADDR *)&sa, sizeof(sa));
        io->rc = bsd->send (io->sock, io->cmd, strlen(io->cmd), 0);
      } break;
    }
    /* Done, send signal to owner thread */
    flags = (xid == io->xid) ? TH_OK : TH_TOUT;
    osDelay(1);
    osThreadFlagsSet (io->owner, flags);
    osThreadFlagsClear (F_ALL);
  }
}

/**
\brief  Test case: BSD_Downstream_Rate
\details
The test case \b BSD_Downstream_Rate tests the maximum rate at which the data
can be received.
*/
void BSD_Downstream_Rate (void) {
  osThreadId_t  worker;
  int32_t       rval;
  IO_STREAMRATE io;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_StreamRate, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    io.sock = io.rc;

    /* Send command to start the download */
    io.cmd = CMD_SEND_TCP;
    TH_EXECUTE (F_SEND_CTRL, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc > 0);

    /* Wait for transfer to complete */
    io.loss = 0;
    TH_EXECUTE (F_DOWNLOAD, 5000 + BSD_TIMEOUT);
    TH_ASSERT  (io.rc > 0);

    /* Check data loss */
    ASSERT_TRUE(io.loss == 0, "Data loss %d byte(s)", io.loss);
    if (rval != 0) {
      DETAIL_INFO("Speed %d KB/s", io.rc/4000);
    }

    /* Close stream socket */
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}

/**
\brief  Test case: BSD_Upstream_Rate
\details
The test case \b BSD_Upstream_Rate tests the maximum rate at which the data
can be sent.
*/
void BSD_Upstream_Rate (void) {
  osThreadId_t  worker;
  int32_t       rval;
  IO_STREAMRATE io;

  /* Create worker thread */
  if (!bsd)    return;
  worker = osThreadNew ((osThreadFunc_t)Th_StreamRate, &io, NULL);
  ASSERT_TRUE(worker, "Worker Thread not created");
  if (!worker) return;

  ARG_INIT();

  /* Create stream socket */
  TH_EXECUTE (F_CREATE_TCP, BSD_TIMEOUT);
  ASSERT_TRUE(io.rc >= 0, "Stream Socket not created");
  if (io.rc >= 0) {
    io.sock = io.rc;

    /* Send command to start the upload */
    io.cmd = CMD_RECV_TCP;
    TH_EXECUTE (F_SEND_CTRL, BSD_TIMEOUT_LONG);
    TH_ASSERT  (io.rc > 0);

    /* Wait for transfer to complete */
    io.loss = 0;
    TH_EXECUTE (F_UPLOAD, 5000 + BSD_TIMEOUT);
    TH_ASSERT  (io.rc > 0);

    /* Check data loss */
    ASSERT_TRUE(io.loss == 0, "Data loss %d byte(s)", io.loss);
    if (rval != 0) {
      DETAIL_INFO("Speed %d KB/s", io.rc/4000);
    }

    /* Close stream socket */
    TH_EXECUTE (F_CLOSE, BSD_TIMEOUT);
    TH_ASSERT  (io.rc == 0);

    osDelay (10);
  }

  /* Terminate worker thread */
  osThreadTerminate (worker);
}
