/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_base64.h
 * Purpose: Base64 Encode and Decode Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_BASE64_H
#define __NET_BASE64_H

#include "net_lib.h"

#define BASE64_MAX_LEN      76          // Max. length of base64 encoded line
#define BASE64_LEN(n)       ((n+2)/3)*4 // Length of base64 encoded string

/* Functions */
extern uint32_t net_base64_encode (char *base64, const void *buf, uint32_t len);
extern uint32_t net_base64_decode (void *buf, const char *base64, uint32_t len);

#endif /* __NET_BASE64_H */
