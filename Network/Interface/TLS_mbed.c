/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2022 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    TLS_mbed.c
 * Purpose: TLS Interface for mbedTLS
 * Rev.:    V7.18.0
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "mbedTLS_config.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/net_sockets.h"
#if defined(MBEDTLS_SSL_CACHE_C)
#include "mbedtls/ssl_cache.h"
#endif
#if defined(MBEDTLS_DEBUG_C)
#include <stdio.h>
#endif

#include "rl_net_lib.h"
#include "TLS_mbed.h"

#ifndef TLS_THREAD_STACK_SIZE
#define TLS_THREAD_STACK_SIZE   4096
#endif

#ifndef TLS_THREAD_PRIORITY
#define TLS_THREAD_PRIORITY     osPriorityNormal
#endif

/* mbedTLS debug level */
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL             1
#endif

/* Local variables */
static TLS_CTRL ctrl;
static TLS_INFO tls_scb[TLS_NUM_SESS];
static uint8_t  tls_buf[1460];

/* mbedTLS resources */
static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;
#ifdef __TLS_SERVER
static mbedtls_ssl_config conf_srv;
static mbedtls_x509_crt   srvcert;
static mbedtls_pk_context pkey_srv;
#if defined(MBEDTLS_SSL_CACHE_C)
static mbedtls_ssl_cache_context cache;
#endif
#endif
#ifdef __TLS_CLIENT
static mbedtls_ssl_config conf_cli;
#endif
#ifdef __TLS_CLIENT_CA
static mbedtls_x509_crt   cacert;
#endif
static mbedtls_ssl_context ssl[TLS_NUM_SESS];

/* Local Functions */
static void netTLS_Thread (void const *arg) __attribute__((noreturn));
static int32_t tls_init (void);
static void    tls_uninit (void);
static void    tls_run (void);
static void    thread_wakeup (void);
static TLS_INFO *tls_map_session (int32_t socket);
static uint32_t cb_tls (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                        const uint8_t *buf, uint32_t len);
static int32_t bio_recv (void *ctx, unsigned char *buf, uint32_t len);
static int32_t bio_send (void *ctx, const uint8_t *buf, uint32_t len);
static void queue_add_buf (TLS_INFO *tls_s, NET_BUFFER *netbuf);
static void queue_release (TLS_INFO *tls_s);
static void free_tx_buf (TLS_INFO *tls_s);

/* RTOS resources */
#if defined(RTE_CMSIS_RTOS)
  #include "cmsis_os.h"
  extern const osThreadDef_t os_thread_def_netTLS_Thread;
  osThreadDef(netTLS_Thread, TLS_THREAD_PRIORITY, 1, TLS_THREAD_STACK_SIZE);
  #define OS_THREAD_NEW(func)   osThreadCreate (osThread(func), NULL)
  #define OS_THREAD_DELETE(id)  osThreadTerminate (id)
  #define OS_FLAG_WAIT(flag)    osSignalWait (flag, osWaitForever)
  #define OS_FLAG_SET(flag)     osSignalSet (ctrl.thread_id, flag)
#elif defined(RTE_CMSIS_RTOS2)
  #include "cmsis_os2.h"
  #define OS_THREAD_NEW(func)   osThreadNew ((osThreadFunc_t)&func, NULL, &tls_thread)
  #define OS_THREAD_DELETE(id)  osThreadTerminate (id)
  #define OS_FLAG_WAIT(flag)    osThreadFlagsWait (flag, osFlagsWaitAny, osWaitForever)
  #define OS_FLAG_SET(flag)     osThreadFlagsSet (ctrl.thread_id, flag)
  #ifdef RTE_CMSIS_RTOS2_RTX5
    #include "rtx_os.h"
    static osRtxThread_t        tls_thread_cb
                                        __attribute__((section(".bss.os.thread.cb")));
    static uint64_t             tls_stack[TLS_THREAD_STACK_SIZE/8]
                                        __attribute__((section(".bss.os.thread.stack")));
    #define __TLS_TCB           &tls_thread_cb, sizeof(tls_thread_cb)
    #define __TLS_STACK         &tls_stack,     sizeof(tls_stack)
  #else
    #define __TLS_TCB           NULL, 0
    #define __TLS_STACK         NULL, TLS_THREAD_STACK_SIZE
  #endif

  static const osThreadAttr_t tls_thread = {
    "netTLS_Thread",
    osThreadDetached,
    __TLS_TCB,
    __TLS_STACK,
    TLS_THREAD_PRIORITY,
    0,
    0
  };
#else
  #error "::CMSIS:RTOS selection invalid"
#endif

/**
  \brief       mbedTLS debug output.
  \return      none.
*/
#if defined(MBEDTLS_DEBUG_C)
static void out_debug (void *ctx, int level, const char *file,
                                             int line, const char *str ) {
#ifndef DEBUG_FULL_PATH
  /* Skip path ie. "C:/Keil/ARM/Pack/ARM/mbedTLS/1.0.0/library/" */
  const char *pos = strrchr (file, '/');
  if (pos != NULL) file = pos + 1;
#endif
  (void)level;
  fprintf ((FILE *)ctx, "%s:%04d: %s", file, line, str);
  fflush ((FILE *)ctx);
}
#endif

/**
  \brief       Initialize TLS interface.
  \return      none.
*/
void netTLS_InterfaceInit (void) {
  memset (&tls_scb, 0, sizeof (tls_scb));
  memset (&ctrl, 0, sizeof (ctrl));
  ctrl.thread_id = OS_THREAD_NEW (netTLS_Thread);
}

/**
  \brief       De-initialize TLS interface.
  \return      none.
*/
void netTLS_InterfaceUninit (void) {
  OS_THREAD_DELETE (ctrl.thread_id);
  tls_uninit ();
  memset (&tls_scb, 0, sizeof (tls_scb));
  memset (&ctrl, 0, sizeof (ctrl));
}

/**
  \brief       Allocate secure TLS context.
  \param[in]   socket   associated TCP socket.
  \param[in]   cb_func  event listening TCP callback function.
  \return      Secure session identifier or 0=none.
*/
uint8_t netTLS_GetContext (int32_t socket, netTCP_cb_t cb_func) {
  TLS_INFO *tls_s;
  int32_t id;

  if ((socket == 0) || (cb_func == NULL)) {
    return (0);
  }
  for (id = 1, tls_s = &tls_scb[0]; id <= TLS_NUM_SESS; tls_s++, id++) {
    if (tls_s->State == TLS_STATE_FREE) {
      tls_s->Socket   = (uint8_t)socket;
      tls_s->ssl      = &ssl[id-1];
      tls_s->cb_func  = cb_func;
      tls_s->Peer     = net_tcp_get_peer_ptr (socket);
      tls_s->conf     = NULL;
      tls_s->State    = TLS_STATE_IDLE;
      return (id & 0xFF);
    }
  }
  return (0);
}

/**
  \brief       Establish encrypted TLS connection to remote endpoint.
  \param[in]   tls_id    TLS session id.
  \param[in]   srv_name  hostname of the server.
  \return      none.
*/
void netTLS_Connect (uint8_t tls_id, const char *srv_name) {
#ifdef __TLS_CLIENT
  TLS_INFO *tls_s;

  if ((tls_id == 0) || (tls_id > TLS_NUM_SESS) || (srv_name == NULL)) {
    return;
  }
  tls_s = &tls_scb[tls_id-1];
  if (tls_s->State != TLS_STATE_IDLE) {
    return;
  }
  /* Use client configuration */
  tls_s->conf     = &conf_cli;
  tls_s->srv_name = srv_name;
  tls_s->State    = TLS_STATE_SETUP;
  net_tcp_set_cb (tls_s->Socket, cb_tls);
  thread_wakeup ();
#else
  (void)tls_id;
  (void)srv_name;
#endif
}

/**
  \brief       Start TLS server listening for encrypted connection.
  \param[in]   tls_id    TLS session id.
  \return      none.
*/
void netTLS_Listen (uint8_t tls_id) {
#ifdef __TLS_SERVER
  TLS_INFO *tls_s;

  if ((tls_id == 0) || (tls_id > TLS_NUM_SESS)) {
    return;
  }
  tls_s = &tls_scb[tls_id-1];
  if (tls_s->State != TLS_STATE_IDLE) {
    return;
  }
  /* Use server configuration */
  tls_s->conf     = &conf_srv;
  tls_s->srv_name = NULL;
  net_tcp_set_cb (tls_s->Socket, cb_tls);
#else
  (void)tls_id;
#endif
}

/**
  \brief       Allocate memory for TLS send buffer.
  \param[in]   size  number of bytes to allocate.
  \return      pointer to allocated memory.
*/
uint8_t *netTLS_GetBuffer (uint32_t size) {
  NET_BUFFER *netbuf;

  netbuf = __BUFFER(net_mem_alloc (QUEUE_HEADER_LEN + size));
  if (netbuf == NULL) {
    /* This is for safety, actually not used */
    return (tls_buf);
  }
  /* Marker used for invalid buffer check */
  netbuf->next = netbuf;
  return (&netbuf->data[0]);
}

/**
  \brief       Close TLS session request.
  \param[in]   tls_id     TLS session id.
  \param[in]   sock_mode  socket close mode: 1=normal, 0=abort.
  \return      none.
*/
void netTLS_Close (uint8_t tls_id, uint8_t sock_mode) {
  TLS_INFO *tls_s;

  if ((tls_id == 0) || (tls_id > TLS_NUM_SESS)) {
    return;
  }
  tls_s = &tls_scb[tls_id-1];
  tls_s->CloseRq = (sock_mode) ? TLS_CLOSE_NORMAL : TLS_CLOSE_ABORT;
  thread_wakeup ();
}

/**
  \brief       Write data to TLS session.
  \param[in]   tls_id  TLS session id.
  \param[in]   buf     pointer to data buffer.
  \param[in]   len     length of data (in bytes).
  \return      none.
*/
void netTLS_Write (uint8_t tls_id, const uint8_t *buf, uint32_t len) {
  TLS_INFO *tls_s;
  NET_BUFFER *netbuf;

  if ((tls_id == 0) || (tls_id > TLS_NUM_SESS)) {
    /* Invalid session identifier */
    return;
  }
  if ((buf == NULL) || (buf == tls_buf)) {
    /* Invalid parameters */
    return;
  }
  netbuf = __BUFFER(buf - NET_BUFFER_HLEN);
  /* Check the marker */
  if (netbuf->next != netbuf) {
    /* Invalid buffer, ignore it */
    return;
  }
  tls_s = &tls_scb[tls_id-1];
  if ((tls_s->t_buf != NULL) || (tls_s->State != TLS_STATE_APPDATA)) {
    /* Previous frame still pending or invalid state */
    /* This is unexpected, should never happen       */
    net_mem_free ((NET_FRAME *)netbuf);
    return;
  }
  netbuf->next   = NULL;
  netbuf->length = (uint16_t)len;
  netbuf->index  = 0;
  tls_s->t_buf   = netbuf;
  thread_wakeup ();
}

/**
  \brief       TLS helper thread.
  \param[in]   arg  dummy parameter.
  \return      none.
*/
static void netTLS_Thread (void const *arg) {
  int32_t ret;

  (void)arg;
  ret = tls_init ();
  if (ret != 0) {
    /* TLS layer failed to initialize, reasons: */
    /*   - heap size too small                  */
    /*   - server certificate or key error      */
    /*   - random number generator problem      */
    /* Check MBEDTLS_ERR_x code for the reason. */
    ctrl.ssl_error = ret;
    ctrl.init_done = false;
  }
  else {
    /* Initialize successful */
    ctrl.init_done = true;
  }
  while (1) {
    OS_FLAG_WAIT (0x0001);
    do {
      ctrl.running = true;
      tls_run ();
      ctrl.running = false;
    } while (ctrl.busy);
  }
}

/**
  \brief       Wake-up TLS helper thread.
  \return      none.
*/
static void thread_wakeup (void) {
  if (ctrl.running) {
    /* Thread currently running */
    ctrl.busy = true;
    return;
  }
  OS_FLAG_SET (0x0001);
}

/**
  \brief       Initialize TLS session layer.
  \return      TLS error code (0= no error).
*/
static int32_t tls_init (void) {
  const char *pers;
  int32_t ret;

  /* Central random generator */
  mbedtls_entropy_init (&entropy);
  mbedtls_ctr_drbg_init (&ctr_drbg);

#ifdef __TLS_SERVER
  mbedtls_ssl_config_init (&conf_srv);
  mbedtls_x509_crt_init (&srvcert);
  mbedtls_pk_init (&pkey_srv);
#if defined(MBEDTLS_SSL_CACHE_C)
  mbedtls_ssl_cache_init (&cache);
#endif
#endif
#ifdef __TLS_CLIENT
  mbedtls_ssl_config_init (&conf_cli);
#ifdef __TLS_CLIENT_CA
  mbedtls_x509_crt_init (&cacert);
#endif
#endif

/* Configure mbedTLS debug */
#if defined(MBEDTLS_DEBUG_C)
  mbedtls_debug_set_threshold (DEBUG_LEVEL);
#ifdef __TLS_SERVER
  mbedtls_ssl_conf_dbg (&conf_srv, out_debug, stdout);
#endif
#ifdef __TLS_CLIENT
  mbedtls_ssl_conf_dbg (&conf_cli, out_debug, stdout);
#endif
#endif

  /* 1. Load the certificates and private key */
#ifdef __TLS_SERVER
  ret = mbedtls_x509_crt_parse (&srvcert, NetSecurity_ServerCert,
                                          NetSecurity_ServerCert_Len);
  if (ret != 0) {
    /* Server certificate parse failed */
    return (ret);
  }

  ret = mbedtls_x509_crt_parse (&srvcert, NetSecurity_ServerCA,
                                          NetSecurity_ServerCA_Len);
  if (ret != 0) {
    /* Server CA certificate(s) parse failed */
    return (ret);
  }

  ret = mbedtls_pk_parse_key (&pkey_srv,  NetSecurity_ServerKey,
                                          NetSecurity_ServerKey_Len,
                                          NULL, 0,
                                          mbedtls_ctr_drbg_random,
                                          &ctr_drbg);
  if (ret != 0) {
    /* Server key parse failed */
    return (ret);
  }
#endif

#ifdef __TLS_CLIENT_CA
  /* No user verification for the client mode */
  ret = mbedtls_x509_crt_parse (&cacert,  NetSecurity_EmailServerCA,
                                          NetSecurity_EmailServerCA_Len);
  if (ret != 0) {
    /* Trusted CA certificate parse failed */
    return (ret);
  }
#endif

  /* 2. Seed the RNG */
  pers = netSYS_GetHostName ();
  ret = mbedtls_ctr_drbg_seed (&ctr_drbg, mbedtls_entropy_func, &entropy,
                               (const uint8_t *)pers, strlen (pers));
  if (ret != 0) {
    /* Random number generator seed failed */
    return (ret);
  }

  /* 3. Setup stuff */
#ifdef __TLS_SERVER
  ret = mbedtls_ssl_config_defaults (&conf_srv, MBEDTLS_SSL_IS_SERVER,
                                                MBEDTLS_SSL_TRANSPORT_STREAM,
                                                MBEDTLS_SSL_PRESET_DEFAULT);
  if (ret != 0) {
    /* Configure defaults failed */
    return (ret);
  }
#endif

#ifdef __TLS_CLIENT
  ret = mbedtls_ssl_config_defaults (&conf_cli, MBEDTLS_SSL_IS_CLIENT,
                                                MBEDTLS_SSL_TRANSPORT_STREAM,
                                                MBEDTLS_SSL_PRESET_DEFAULT);
  if (ret != 0) {
    /* Configure defaults failed */
    return (ret);
  }
#ifndef __TLS_CLIENT_CA
  mbedtls_ssl_conf_authmode (&conf_cli, MBEDTLS_SSL_VERIFY_OPTIONAL);
#endif
#endif

#ifdef __TLS_SERVER
  mbedtls_ssl_conf_rng (&conf_srv, mbedtls_ctr_drbg_random, &ctr_drbg);

#if defined(MBEDTLS_SSL_CACHE_C)
  mbedtls_ssl_conf_session_cache (&conf_srv, &cache,
                                  mbedtls_ssl_cache_get,
                                  mbedtls_ssl_cache_set);
#endif

  mbedtls_ssl_conf_ca_chain (&conf_srv, srvcert.next, NULL);
  ret = mbedtls_ssl_conf_own_cert (&conf_srv, &srvcert, &pkey_srv);
  if (ret != 0) {
    /* Set own certificate chain and private key failed */
    return (ret);
  }
#endif

#ifdef __TLS_CLIENT
  mbedtls_ssl_conf_rng (&conf_cli, mbedtls_ctr_drbg_random, &ctr_drbg);
#ifdef __TLS_CLIENT_CA
  mbedtls_ssl_conf_ca_chain (&conf_cli, &cacert, NULL);
#endif
#endif
  return (0);
}

/**
  \brief       De-initialize TLS session layer.
  \return      none.
*/
static void tls_uninit (void) {
  TLS_INFO *tls_s;
  int32_t id;

  /* Uninit TLS sessions */
  for (id = 0, tls_s = &tls_scb[0]; id < TLS_NUM_SESS; tls_s++, id++) {
    if (tls_s->State > TLS_STATE_IDLE) {
      mbedtls_ssl_free (tls_s->ssl);
    }
    mbedtls_ssl_init (tls_s->ssl);
  }

  /* Unload certificates and keys */
#ifdef __TLS_CLIENT
#ifdef __TLS_CLIENT_CA
  mbedtls_x509_crt_free (&cacert);
#endif
  mbedtls_ssl_config_free (&conf_cli);
#endif
#ifdef __TLS_SERVER
  mbedtls_x509_crt_free (&srvcert);
  mbedtls_pk_free (&pkey_srv);
  mbedtls_ssl_config_free (&conf_srv);
#if defined(MBEDTLS_SSL_CACHE_C)
  mbedtls_ssl_cache_free (&cache);
#endif
#endif

  /* Uninit central random generator */
  mbedtls_ctr_drbg_free( &ctr_drbg );
  mbedtls_entropy_free( &entropy );
}

/**
  \brief       Run TLS service.
  \return      none.
  \note        Generates native TCP callbacks for the service.
*/
static void tls_run (void) {
  TLS_INFO *tls_s;
  NET_BUFFER *tbuf;
  int32_t i,ret;

  ctrl.busy = false;
  for (i = 0, tls_s = &tls_scb[0]; i < TLS_NUM_SESS; tls_s++, i++) {
    switch (tls_s->State) {
      case TLS_STATE_FREE:
      case TLS_STATE_IDLE:
        break;

      case TLS_STATE_SETUP:
        /* Socket is connected, now start SSL handshake */
        tls_s->CloseRq   = 0;
        tls_s->ssl_error = 0;
        mbedtls_ssl_init (tls_s->ssl);
        ret = mbedtls_ssl_setup (tls_s->ssl, tls_s->conf);
        if (ret != 0) {
          /* Setup failed, probably out of memory */
          tls_s->ssl_error = ret;
          goto close;
        }
        if (tls_s->srv_name) {
          /* Set hostname for client mode only */
          ret = mbedtls_ssl_set_hostname (tls_s->ssl, tls_s->srv_name);
          if (ret != 0) {
            tls_s->ssl_error = ret;
            goto close;
          }
        }
        mbedtls_ssl_set_bio (tls_s->ssl, tls_s, bio_send, bio_recv, NULL);
        tls_s->State = TLS_STATE_HANDSHAKE;
        ctrl.busy    = true;
        break;

      case TLS_STATE_HANDSHAKE:
        ret = mbedtls_ssl_handshake (tls_s->ssl);
        if ((ret == MBEDTLS_ERR_SSL_WANT_READ) ||
            (ret == MBEDTLS_ERR_SSL_WANT_WRITE)) {
          break;
        }
        if (ret != 0) {
          /* Handshake error, close and reset session */
          tls_s->ssl_error = ret;
          goto close;
        }
        net_sys_lock ();
        tls_s->cb_func (tls_s->Socket, netTCP_EventEstablished, tls_s->Peer, NULL, 0);
        net_sys_unlock ();
        tls_s->State = TLS_STATE_APPDATA;
        ctrl.busy    = true;
        break;

      case TLS_STATE_APPDATA:
        /* Send and receive encrypted data */
        if (tls_s->t_buf != NULL) {
          /* Data available for sending */
          tbuf = tls_s->t_buf;
          ret  = mbedtls_ssl_write (tls_s->ssl,
                                    &tbuf->data[tbuf->index], tbuf->length);
          if (ret > 0) {
            /* Some data sent successfully */
            tbuf->index += ret;
            if (tbuf->index < tbuf->length) {
              /* Send not complete, try again later */
              if (tls_s->CloseRq) {
                break;
              }
            }
            else {
              /* Send complete, release buffer */
              free_tx_buf (tls_s);
              if (tls_s->CloseRq) {
                goto close;
              }
              net_sys_lock ();
              tls_s->cb_func (tls_s->Socket, netTCP_EventACK, tls_s->Peer, NULL, 0);
              net_sys_unlock ();
            }
          }
          else if ((ret != MBEDTLS_ERR_SSL_WANT_READ) &&
                   (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            /* Error, close session */
            tls_s->ssl_error = ret;
            goto close;
          }
        }
        if (tls_s->CloseRq) {
          goto close;
        }
        ret = mbedtls_ssl_read (tls_s->ssl, tls_buf, sizeof(tls_buf));
        if (ret > 0) {
          /* Data received, send a notification */
          net_sys_lock ();
          tls_s->cb_func (tls_s->Socket, netTCP_EventData, tls_s->Peer, tls_buf, (uint32_t)ret);
          net_sys_unlock ();
          ctrl.busy = true;
        }
        else if ((ret != MBEDTLS_ERR_SSL_WANT_READ) &&
                 (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
          /* Error, close session */
          if (ret != MBEDTLS_ERR_NET_CONN_RESET) {
            tls_s->ssl_error = ret;
          }
close:    tls_s->State = TLS_STATE_CLOSE;
          ctrl.busy    = true;
        }
        break;

      case TLS_STATE_CLOSE:
        /* Session has been closed */
        if (tls_s->CloseRq == TLS_CLOSE_ABORT || tls_s->ssl_error) {
          netTCP_Abort (tls_s->Socket);
        }
        else {
          netTCP_Close (tls_s->Socket);
        }
        net_sys_lock ();
        if (!tls_s->CloseRq) {
          /* Application is not aware of socket closing */
          tls_s->cb_func (tls_s->Socket, netTCP_EventClosed, tls_s->Peer, NULL, 0);
        }
        queue_release (tls_s);
        net_sys_unlock ();
        free_tx_buf (tls_s);
        mbedtls_ssl_free (tls_s->ssl);
        tls_s->CloseRq = 0;
        tls_s->State   = TLS_STATE_IDLE;
        break;
    }
  }
}

/**
  \brief       Map TCP socket to existing TLS session.
  \param[in]   socket  TCP socket handle.
  \return      pointer to TLS session control block.
*/
static TLS_INFO *tls_map_session (int32_t socket) {
  TLS_INFO *tls_s;
  int32_t i;

  if (!ctrl.init_done) {
    /* TLS layer not yet initialized */
    return (NULL);
  }
  for (i = 0, tls_s = &tls_scb[0]; i < TLS_NUM_SESS; tls_s++, i++) {
    if (tls_s->Socket == (uint8_t)socket) {
      return (tls_s);
    }
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
  \note        This function is called from protected environment.
               No need to call net_sys_lock/net_sys_unlock.
*/
static uint32_t cb_tls (int32_t socket, netTCP_Event event, const NET_ADDR *addr,
                                        const uint8_t *buf, uint32_t len) {
  TLS_INFO *tls_s;
  NET_BUFFER *netbuf;

  /* Find the right session */
  tls_s = tls_map_session (socket);
  if (tls_s == NULL) {
    return (false);
  }
  switch (event) {
    case netTCP_EventConnect:
      if (tls_s->State > TLS_STATE_IDLE) {
        /* Ignore new connection if SSL state not idle yet */
        return (NET_TCP_NO_ACK);
      }
      /* Let the service handle connection accept/deny */
      return (tls_s->cb_func (tls_s->Socket, event, addr, NULL, 0));

    case netTCP_EventAborted:
    case netTCP_EventClosed:
      /* Connection closed, virtual netTCP_EventClose will */
      /* be generated to the application from TLS thread.  */
      thread_wakeup ();
      break;

    case netTCP_EventEstablished:
      /* Connected, handle TLS handshaking in this module.  */
      /* Generate virtual 'netTCP_EventEstablished' to the  */
      /* application when the handshaking is over.          */
      tls_s->State = TLS_STATE_SETUP;
      thread_wakeup ();
      break;

    case netTCP_EventACK:
      /* Data has been acknowledged */
      thread_wakeup ();
      break;

    case netTCP_EventData:
      /* Data has been received, store the data to receive */
      /* queue and let mbedTLS process it.                 */
      netbuf = __BUFFER(net_mem_alloc (QUEUE_HEADER_LEN + len));
      if (netbuf == NULL) {
        /* No memory, do not ack this packet */
        return (NET_TCP_NO_ACK);
      }
      netbuf->length = (uint16_t)len;
      netbuf->index  = 0;
      memcpy (&netbuf->data[0], buf, len);
      queue_add_buf (tls_s, netbuf);
      thread_wakeup ();
      break;
  }
  return (true);
}

/**
  \brief       Read data from TCP socket.
  \param[in]   ctx  TLS session context.
  \param[out]  buf  pointer to application data buffer to store the data to.
  \param[in]   len  size of application data buffer (in bytes).
  \return      number of bytes read or status information (retv < 0).
  \note        Reads data that is buffered for the session.
*/
static int32_t bio_recv (void *ctx, unsigned char *buf, uint32_t len) {
  TLS_INFO *tls_s = ctx;
  NET_BUFFER *netbuf;
  uint32_t nr,dlen;

  if (tls_s->r_queue == NULL) {
    /* No data available */
    if (netTCP_GetState (tls_s->Socket) != netTCP_StateESTABLISHED) {
      return (MBEDTLS_ERR_NET_CONN_RESET);
    }
    return (MBEDTLS_ERR_SSL_WANT_READ);
  }

  for (nr = 0; (nr < len) && tls_s->r_queue; ) {
    netbuf = tls_s->r_queue;
    dlen   = netbuf->length - netbuf->index;
    if (dlen > (len - nr)) {
      dlen = len - nr;
    }
    memcpy (&buf[nr], &netbuf->data[netbuf->index], dlen);
    nr += dlen;
    if ((netbuf->index + dlen) < netbuf->length) {
      /* Application buffer is full, block not fully read */
      netbuf->index += dlen;
      break;
    }
    /* This buffer done, get next from the queue */
    tls_s->r_queue = netbuf->next;
    net_mem_free ((NET_FRAME *)netbuf);
  }
  return ((int32_t)nr);
}

/**
  \brief       Send data to TCP socket.
  \param[in]   ctx  TLS session context.
  \param[in]   buf  pointer to application data buffer to transmit.
  \param[in]   len  length of data (in bytes).
  \return      number of bytes sent or status information (retv < 0).
*/
static int32_t bio_send (void *ctx, const uint8_t *buf, uint32_t len) {
  TLS_INFO *tls_s = ctx;
  uint32_t maxsize;
  uint8_t *tcp_buf;
  netStatus retv;

  if (!netTCP_SendReady (tls_s->Socket)) {
    if (netTCP_GetState (tls_s->Socket) != netTCP_StateESTABLISHED) {
      /* Socket is not connected */
      return (MBEDTLS_ERR_NET_CONN_RESET);
    }
    /* Socket is waiting for ACK */
    return (MBEDTLS_ERR_SSL_WANT_WRITE);
  }
  maxsize = netTCP_GetMaxSegmentSize (tls_s->Socket);
  if (len > maxsize) {
    len = maxsize;
  }
  tcp_buf = netTCP_GetBuffer (len);
  if (tcp_buf == NULL) {
    /* No memory, try again later */
    return (MBEDTLS_ERR_SSL_WANT_WRITE);
  }
  memcpy (tcp_buf, buf, len);
  retv = netTCP_Send (tls_s->Socket, tcp_buf, len);
  if (retv == netOK) {
    /* Return number of bytes sent */
    return ((int32_t)len);
  }
  if (retv == netBusy) {
    return (MBEDTLS_ERR_SSL_WANT_WRITE);
  }
  if (retv == netInvalidParameter) {
    return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
  }
  if (retv == netWrongState) {
    return (MBEDTLS_ERR_NET_CONN_RESET);
  }
  return (MBEDTLS_ERR_NET_SEND_FAILED);
}

/**
  \brief       Add data buffer to socket receive queue tail.
  \param[in]   tls_s   TLS session descriptor.
  \param[in]   netbuf  pointer to netbuf containing the data.
  \return      none.
*/
static void queue_add_buf (TLS_INFO *tls_s, NET_BUFFER *netbuf) {
  NET_BUFFER *next_buf;

  netbuf->next = NULL;
  if (tls_s->r_queue == NULL) {
    /* First data block added */
    tls_s->r_queue = netbuf;
  }
  else {
    /* Scan the queue chain */
    next_buf = tls_s->r_queue;
    while (next_buf->next != NULL) {
      next_buf = next_buf->next;
    }
    /* Append the buffer to queue tail */
    next_buf->next = netbuf;
  }
}

/**
  \brief       Release chained RX data buffers.
  \param[in]   tls_s  TLS session descriptor.
  \return      none.
*/
static void queue_release (TLS_INFO *tls_s) {
  NET_BUFFER *netbuf, *next_buf;

  netbuf = tls_s->r_queue;
  tls_s->r_queue = NULL;
  for ( ; netbuf; netbuf = next_buf) {
    next_buf = netbuf->next;
    net_mem_free ((NET_FRAME *)netbuf);
  }
}

/**
  \brief       Release TX data buffer.
  \param[in]   tls_s  TLS session descriptor.
  \return      none.
*/
static void free_tx_buf (TLS_INFO *tls_s) {
  if (tls_s->t_buf) {
    net_mem_free ((NET_FRAME *)tls_s->t_buf);
    tls_s->t_buf = NULL;
  }
}
