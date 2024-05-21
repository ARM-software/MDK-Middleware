/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_common.h
 * Purpose: Common Network Function Definitions
 *----------------------------------------------------------------------------*/

#ifndef NET_COMMON_H_
#define NET_COMMON_H_

#include "net_lib.h"

/* Compiler specific */
#if defined(__clang__) || defined(__GNUC__)
  #define __FALLTHROUGH     __attribute__((__fallthrough__))
#else
  #define __FALLTHROUGH
#endif

#define __ALIGNED_UINT16(x) (*(uint16_t *)(void *)(x))
#define __ALIGNED_UINT32(x) (*(uint32_t *)(void *)(x))
#define __ALIGNED_UINT64(x) (*(uint64_t *)(void *)(x))

#define __ALIGN_CAST(t)     (t)(void *)
#define __CONST_CAST(t)     (t)(uint32_t)

/* Definitions */
#if defined (__ARM_BIG_ENDIAN)
  #define U32_LE(v)         (uint32_t)__REV(v)
  #define U16_LE(v)         (uint16_t)__REV16(v)
#else
  #define U32_LE(v)         (uint32_t)(v)
  #define U16_LE(v)         (uint16_t)(v)
#endif

/* Preprocessor macros only for precompiled constants */
#if defined(__ARM_BIG_ENDIAN)
  #define HTONL(v)          (uint32_t)(v)
  #define HTONS(v)          (uint16_t)(v)
#else
  #define HTONL(v)          (uint32_t)((((v) & 0xFF000000) >> 24) | \
                                       (((v) & 0x00FF0000) >>  8) | \
                                       (((v) & 0x0000FF00) <<  8) | \
                                       (((v) & 0x000000FF) << 24))
  #define HTONS(v)          (uint16_t)((((v) & 0xFF00) >> 8) | \
                                       (((v) & 0x00FF) << 8))
#endif
#define CRC32_POLY          0x04c11db7
#define U32_BE(v)           ntohl(v)            // Network byte order of uint32_t
#define U16_BE(v)           ntohs(v)            // Network byte order of uint16_t
#define HI_BYTE(v)          (uint8_t)((v) >> 8) // High byte of uint16_t
#define LO_BYTE(v)          (uint8_t)(v)        // Low byte of uint16_t
#define LSTR(p,s)           sizeof(s); p = s    // Define a string, return size
#define RESP(a,b,c,r)       (((a<<16 | b<<8 | c) | 0x303030) << 8 | r)

/* Functions */
extern bool     net_strcmp (const char *sp, const char *cp);
extern bool     net_strcasecmp (const char *sp, const char *cp);
extern int32_t  net_strcpy (char *dp, const char *sp);
extern int32_t  net_strncpy (char *dp, const char *sp, uint32_t n);
extern int32_t  net_strchr (const char *sp, char ch);
extern int32_t  net_strnchr (const char *sp, char ch, uint32_t n);
extern int32_t  net_strrchr (const char *sp, char ch);
extern int32_t  net_strtoupper (char *dp, const char *sp);
extern int32_t  net_strstr (const char *sp, const char *cp);
extern int32_t  net_strnstr (const char *sp, const char *cp, uint32_t n);
extern uint32_t net_strcrc (const char *sp);
extern uint32_t net_crc_buf (const void *buf, uint32_t len, uint32_t crc32);
extern uint16_t net_chksum_buf (const void *buf, uint32_t len, uint32_t sum);
extern int32_t  net_bin2hex (char *dp, const void *buf, uint32_t len);
extern int32_t  net_ultox (char *dp, uint32_t val);
extern int32_t  net_ultoa (char *dp, uint32_t val);
extern int32_t  net_ultoa_n (char *dp, uint32_t val, int32_t n);
extern int32_t  net_ultoa_n0 (char *dp, uint32_t val, int32_t n);
extern int32_t  net_itoa (char *dp, int32_t val);
extern int32_t  net_itoa_n (char *dp, int32_t val, int32_t n);
extern int32_t  net_itoa_n0 (char *dp, int32_t val, int32_t n);
extern int32_t  net_atoi (const char *sp, int32_t *len);
extern uint32_t net_xtoul (const char *sp, int32_t *len);
extern uint8_t  net_xtouc (const char *sp);

/* Turn off AC6 compiler warnings */
#define net_strcpy2(dp,sp)      (uint32_t)net_strcpy(dp,sp)
#define net_strncpy2(dp,sp,n)   (uint32_t)net_strncpy(dp,sp,n)

/* Avoid using generic public function names */
#define get_u16(p)          net_rd_u16((const uint8_t *)(p))
#define get_u32(p)          net_rd_u32((const uint8_t *)(p))
#define set_u16(p,v)        net_wr_u16((uint8_t *)(p),(uint16_t)(v))
#define set_u32(p,v)        net_wr_u32((uint8_t *)(p),(uint32_t)(v))

/* Unaligned access in network byte order */
#if defined(__ARM_FEATURE_UNALIGNED)
  __STATIC_FORCEINLINE uint16_t net_rd_u16 (const uint8_t *addr) {
    return (ntohs(__UNALIGNED_UINT16_READ(&addr[0])));
  }
  __STATIC_FORCEINLINE void net_wr_u16 (uint8_t *addr, uint16_t val) {
    __UNALIGNED_UINT16_WRITE(&addr[0], htons(val));
  }
  __STATIC_FORCEINLINE uint32_t net_rd_u32 (const uint8_t *addr) {
    return (ntohl(__UNALIGNED_UINT32_READ(&addr[0])));
  }
  __STATIC_FORCEINLINE void net_wr_u32 (uint8_t *addr, uint32_t val) {
    __UNALIGNED_UINT32_WRITE(&addr[0], htonl(val));
  }
#else
  __STATIC_FORCEINLINE uint16_t net_rd_u16 (const uint8_t *addr) {
    return ((uint16_t)(addr[0] << 8) | addr[1]);
  }
  __STATIC_FORCEINLINE void net_wr_u16 (uint8_t *addr, uint16_t val) {
    addr[0] = (val >> 8) & 0xFF;
    addr[1] = (val)      & 0xFF;
  }
  extern uint32_t net_rd_u32 (const uint8_t *addr);
  extern void     net_wr_u32 (uint8_t *addr, uint32_t val);
#endif

#endif /* NET_COMMON_H_ */
