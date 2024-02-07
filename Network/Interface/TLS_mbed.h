/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    TLS_mbed.h
 * Purpose: TLS Interface definitions
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"

/* Check TLS-Server endpoint type */
#if defined(RTE_Network_Web_Server_RO_TLS) || \
    defined(RTE_Network_Web_Server_FS_TLS)
  #include "Net_Config_HTTP_Server.h"
  #define __TLS_SERVER
#else
  #define HTTP_SERVER_NUM_SESSIONS 0
#endif

/* Check TLS-Client endpoint type */
#if defined(RTE_Network_SMTP_Client_TLS)
  #define SMTP_CLIENT_NUM_SESSIONS 1
  #define __TLS_CLIENT
  #ifndef SMTPS_SERVER_VERIFY_NONE
  #define __TLS_CLIENT_CA
  #endif
#else
  #define SMTP_CLIENT_NUM_SESSIONS 0
#endif

/* Number of TLS sessions */
#define TLS_NUM_SESS    (HTTP_SERVER_NUM_SESSIONS + \
                         SMTP_CLIENT_NUM_SESSIONS)

/* Check mbedTLS configuration */
#if defined(__TLS_SERVER) && !defined(MBEDTLS_SSL_SRV_C)
  #error "mbedTLS_config: MBEDTLS_SSL_SRV_C not defined"
#endif

#if defined(__TLS_CLIENT) && !defined(MBEDTLS_SSL_CLI_C)
  #error "mbedTLS_config: MBEDTLS_SSL_CLI_C not defined"
#endif

#if !defined(MBEDTLS_SSL_TLS_C)
  #error "mbedTLS_config: MBEDTLS_SSL_TLS_C not defined"
#endif

#if !defined(MBEDTLS_RSA_C)
  #error "mbedTLS_config: MBEDTLS_RSA_C not defined"
#endif

#if !defined(MBEDTLS_BIGNUM_C)
  #error "mbedTLS_config: MBEDTLS_BIGNUM_C not defined"
#endif

#if !defined(MBEDTLS_PEM_PARSE_C)
  #error "mbedTLS_config: MBEDTLS_PEM_PARSE_C not defined"
#endif

#if !defined(MBEDTLS_X509_CRT_PARSE_C)
  #error "mbedTLS_config: MBEDTLS_X509_CRT_PARSE_C not defined"
#endif

#if !defined(MBEDTLS_ENTROPY_C)
  #error "mbedTLS_config: MBEDTLS_ENTROPY_C not defined"
#endif

#if !defined(MBEDTLS_CTR_DRBG_C)
  #error "mbedTLS_config: MBEDTLS_CTR_DRBG_C not defined"
#endif

#if defined(MBEDTLS_NET_C)
  #error "mbedTLS_config: MBEDTLS_NET_C defined"
#endif

/* TLS definitions */
#define NET_BUFFER_HLEN     8           // Size of NET_BUFFER header
#define QUEUE_HEADER_LEN    ((NET_BUFFER_HLEN - NET_HEADER_LEN) | 0x40000000)

/* TLS close mode */
#define TLS_CLOSE_NORMAL    1           // Normal TCP close
#define TLS_CLOSE_ABORT     2           // Abort TCP

/* TLS states */
typedef enum {
  TLS_STATE_FREE = 0,                   // Entry is free and unused
  TLS_STATE_IDLE,                       // Idle, but allocated
  TLS_STATE_SETUP,                      // Setup session, next is handshaking
  TLS_STATE_HANDSHAKE,                  // Handshaking in progress
  TLS_STATE_APPDATA,                    // Encrypted data exchange
  TLS_STATE_CLOSE,                      // TLS session is closing
} TLS_STATE;

/* TLS session info */
typedef struct net_tls_info {
  TLS_STATE    State;                   // Current state
  uint8_t      Socket;                  // Assigned TCP socket
  uint8_t      CloseRq;                 // Close requested by service
  NET_ADDR    *Peer;                    // Pointer to peer address structure
  netTCP_cb_t  cb_func;                 // Application callback function
  void        *ssl;                     // SSL context
  void        *conf;                    // SSL configuration
  const char  *srv_name;                // Hostname of the server (client only)
  NET_BUFFER  *t_buf;                   // Transmit data packet
  NET_BUFFER  *r_queue;                 // Receive data queue
  int32_t      ssl_error;               // Fail error code
} TLS_INFO;

/* TLS control info */
typedef struct net_tls_ctrl {
  NETOS_ID thread_id;                   // TLS thread identifier
  bool     running;                     // TLS thread currently running
  bool     busy;                        // need another call to tls_run()
  bool     init_done;                   // TLS layer initialized
  int32_t  ssl_error;                   // tls_init() error code
} TLS_CTRL;
