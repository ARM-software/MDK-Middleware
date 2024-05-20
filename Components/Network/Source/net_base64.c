/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_base64.c
 * Purpose: Base64 Encode and Decode
 *----------------------------------------------------------------------------*/

#include "net_base64.h"

/**
  \brief       Convert the data to base64 encoded string.
  \param[out]  base64   pointer to output string.
  \param[in]   buf      buffer containing input data.
  \param[in]   len      length of the input data.
  \return      length of base64 encoded string.
*/
uint32_t net_base64_encode (char *base64, const void *buf, uint32_t len) {
  /* Base64 encoding table */
  static const char enc_map[64] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/"
  };
  const uint8_t *sp = buf;
  uint32_t i,v;

  for (i = 0; len > 2; base64 += 4, sp += 3, len -= 3, i += 4) {
    /* Expand 3 binary bytes to 4 base64 characters */
    v = (uint32_t)(sp[0] << 16 | sp[1] << 8 | sp[2]);
    base64[0] = enc_map[v >> 18];
    base64[1] = enc_map[(v >> 12) & 0x3F];
    base64[2] = enc_map[(v >> 6)  & 0x3F];
    base64[3] = enc_map[v & 0x3F];
  }
  if (len == 2) {
    /* Only 2 bytes left, add padding */
    v = (uint32_t)(sp[0] << 8 | sp[1]);
    base64[0] = enc_map[v >> 10];
    base64[1] = enc_map[(v >> 4) & 0x3F];
    base64[2] = enc_map[(v << 2) & 0x3F];
    base64[3] = '=';
    /* Add null-termination */
    base64[4] = 0;
    return (i + 4);
  }
  if (len == 1) {
    /* Only 1 byte left, add padding */
    v = (uint32_t)sp[0];
    base64[0] = enc_map[v >> 2];
    base64[1] = enc_map[(v << 4) & 0x3F];
    base64[2] = '=';
    base64[3] = '=';
    /* Add null-termination */
    base64[4] = 0;
    return (i + 4);
  }
  /* Add null-termination */
  base64[0] = 0;
  return (i);
}

/**
  \brief       Decode base64 encoded string to binary data.
  \param[out]  buf     output buffer to store the data.
  \param[in]   base64  pointer to encoded input string.
  \param[in]   len     maximum length of the input string.
  \return      length of converted binary data.
*/
uint32_t net_base64_decode (void *buf, const char *base64, uint32_t len) {
  /* Base64 decoding table: characters 43 to 122 ('+' to 'z') */
  static const uint8_t dec_map[80] = {
    "\x3E\xFF\xFF\xFF\x3F\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\xFF"
    "\xFF\xFF\xFF\xFF\xFF\xFF\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09"
    "\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"
    "\xFF\xFF\xFF\xFF\xFF\xFF\x1A\x1B\x1C\x1D\x1E\x1F\x20\x21\x22\x23"
    "\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F\x30\x31\x32\x33"
  };
  uint8_t *dp = buf;
  uint32_t i,j,v,idx;

  for (i = 0; len > 0; base64 += 4, i += 3) {
    /* Convert 4 base64 characters to 3 binary bytes */
    for (v = j = 0; j < 4; j++) {
      if (j == 2 && base64[j] == '=' && base64[j+1] == '=') {
        /* Padding, decode last byte */
        dp[i] = (v >> 4) & 0xFF;
        return (i + 1);
      }
      if (j == 3 && base64[j] == '=') {
        /* Padding, decode last 2 bytes */
        dp[i]   = (v >> 10) & 0xFF;
        dp[i+1] = (v >> 2) & 0xFF;
        return (i + 2);
      }
      idx = base64[j] - 43;
      if (idx > 79 || dec_map[idx] > 63) {
        /* End, invalid character found*/
        return (i);
      }
      v = v << 6 | dec_map[idx];
      len--;
    }
    dp[i]   = (v >> 16) & 0xFF;
    dp[i+1] = (v >> 8) & 0xFF;
    dp[i+2] = v & 0xFF;
  }
  return (i);
}
