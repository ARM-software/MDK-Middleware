/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_md5.h
 * Purpose: MD5 Message-Digest Algorithm Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_MD5_H
#define __NET_MD5_H

#include "rl_net_lib.h"

/* MD5 Context info */
typedef struct net_md5_ctx {
  uint32_t length;                      // Number of bytes processed
  uint32_t state[4];                    // Calculation state before net_md5_final
  union {
    uint8_t  buf[64];                   // Calculation input buffer
    uint32_t x[16];                     // Input buffer word (4-byte) access
  } u;
} NET_MD5_CTX;

#define __MD5_CTX(buf)          (NET_MD5_CTX *)((uint32_t)(buf))

/* Functions */
void net_md5_init   (NET_MD5_CTX *md5);
void net_md5_update (NET_MD5_CTX *md5, const char *inbuf, int32_t inlen);
void net_md5_final  (NET_MD5_CTX *md5, uint8_t *digest);

#endif /* __NET_MD5_H */
