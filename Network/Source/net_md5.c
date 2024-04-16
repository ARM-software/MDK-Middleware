/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_md5.c
 * Purpose: MD5 Message-Digest Algorithm
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_common.h"
#include "net_md5.h"


/* F, G, H and I are basic MD5 functions */
#define F(x,y,z)        (((x) & (y)) | ((~x) & (z)))
#define G(x,y,z)        (((x) & (z)) | ((y) & (~z)))
#define H(x,y,z)        ((x) ^ (y) ^ (z))
#define I(x,y,z)        ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits */
#define ROTATE_LEFT(x,n) (((x) << (n)) | ((x) >> (32-(n))))

/* Local Functions */
static void md5_transform (NET_MD5_CTX *md5);

/**
  \brief       Initialize MD5 context, load magic constants.
  \param[in]   md5  pointer to MD5 context.
*/
void net_md5_init (NET_MD5_CTX *md5) {
  md5->length   = 0;
  md5->state[0] = 0x67452301;
  md5->state[1] = 0xefcdab89;
  md5->state[2] = 0x98badcfe;
  md5->state[3] = 0x10325476;
}

/**
  \brief       Update MD5 context, calculate message digest.
  \param[in]   md5    pointer to MD5 context.
  \param[in]   inbuf  input data buffer.
  \param[in]   inlen  length of the data.
  \note        MD5 is calculated on block of 64-bytes.
*/
void net_md5_update (NET_MD5_CTX *md5, const char *inbuf, int32_t inlen) {
  uint32_t bpos, dlen = (uint32_t)inlen;

  bpos = md5->length & 0x3F;
  md5->length += dlen;

  if (bpos + dlen < 64) {
    /* Buffer not filled completely */
    memcpy (&md5->u.buf[bpos], inbuf, dlen);
    return;
  }

  memcpy (&md5->u.buf[bpos], inbuf, 64 - bpos);
  md5_transform (md5);

  inbuf += 64 - bpos;
  dlen  -= 64 - bpos;

  while (dlen >= 64) {
    memcpy (&md5->u.buf[0], inbuf, 64);
    md5_transform (md5);
    inbuf += 64;
    dlen  -= 64;
  }
  memcpy (&md5->u.buf[0], inbuf, dlen);
}

/**
  \brief       Terminate MD5 calculation and return result.
  \param[in]   md5     pointer to MD5 context.
  \param[out]  digest  output MD5 digest buffer.
  \note        Pad to 64-byte boundary with bit pattern 1 0 0 0...
               (bit MSB first).
*/
void net_md5_final (NET_MD5_CTX *md5, uint8_t *digest) {
  uint32_t bpos;

  bpos = md5->length & 0x3F;
  /* Add padding bytes to the buffer */
  md5->u.buf[bpos++] = 0x80;
  memset (&md5->u.buf[bpos], 0, 64 - bpos);
  if (bpos > 56) {
    md5_transform (md5);
    memset (&md5->u.buf[0], 0, 64);
  }
  
  /* For MD5 calculation take a bit-count */
  md5->u.x[14] = U32_LE(md5->length << 3);
  md5_transform (md5);

  /* Digest is native Little endian */
  md5->state[0] = U32_LE(md5->state[0]); 
  md5->state[1] = U32_LE(md5->state[1]); 
  md5->state[2] = U32_LE(md5->state[2]); 
  md5->state[3] = U32_LE(md5->state[3]); 
  memcpy (digest, md5->state, 16);
}

/**
  \brief       Transform MD5-state based on input buf.
  \param[in]   md5    pointer to MD5 context.
*/
static void md5_transform (NET_MD5_CTX *md5) {
  static const uint32_t T[64] = {
    /* Table for fast MD5 transform calculation */
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };
  static const uint8_t s1[4] = { 7, 12, 17, 22 };
  static const uint8_t s2[4] = { 5,  9, 14, 20 };
  static const uint8_t s3[4] = { 4, 11, 16, 23 };
  static const uint8_t s4[4] = { 6, 10, 15, 21 };
  uint32_t a,b,c,d,tmp;
  int32_t i,j;

  a = md5->state[0];
  b = md5->state[1];
  c = md5->state[2];
  d = md5->state[3];

  /* Round 1 */
  for (i = 0; i < 16; i++) {
    tmp = a + F (b, c, d) + U32_LE(md5->u.x[i]) + T[i];
    tmp = ROTATE_LEFT (tmp, s1[i & 3]);
    tmp += b;
    a = d; d = c; c = b; b = tmp;
  }
  /* Round 2 */
  for (i = 0, j = 1; i < 16; i++, j += 5) {
    tmp = a + G (b, c, d) + U32_LE(md5->u.x[j & 0xF]) + T[i+16];
    tmp = ROTATE_LEFT (tmp, s2[i & 3]);
    tmp += b;
    a = d; d = c; c = b; b = tmp;
  }
  /* Round 3 */
  for (i = 0, j = 5; i < 16; i++, j += 3) {
    tmp = a + H (b, c, d) + U32_LE(md5->u.x[j & 0xF]) + T[i+32];
    tmp = ROTATE_LEFT (tmp, s3[i & 3]);
    tmp += b;
    a = d; d = c; c = b; b = tmp;
  }
  /* Round 4 */
  for (i = 0, j = 0; i < 16; i++, j += 7) {
    tmp = a + I (b, c, d) + U32_LE(md5->u.x[j & 0xF]) + T[i+48];
    tmp = ROTATE_LEFT (tmp, s4[i & 3]);
    tmp += b;
    a = d; d = c; c = b; b = tmp;
  }

  md5->state[0] += a;
  md5->state[1] += b;
  md5->state[2] += c;
  md5->state[3] += d;
}
