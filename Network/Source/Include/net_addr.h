/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_addr.h
 * Purpose: Address Handling Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_ADDR_H
#define __NET_ADDR_H

#include <string.h>
#include "net_common.h"

/* Constants */
extern uint8_t const net_addr_unspec[];
extern uint8_t const net_addr_bcast[];

/* Functions */
extern bool net_addr4_aton (const char *cp, uint8_t *ip4_addr);
extern bool netIP_aton (const char *addr_string, int16_t addr_type, uint8_t *ip_addr);
extern bool netMAC_aton (const char *mac_string, uint8_t *mac_addr);
extern const char *net_addr4_ntoa (const uint8_t *ip4_addr);
extern const char *net_addr_ntoa (const __ADDR *addr);
extern const char *netIP_ntoa (int16_t addr_type, const uint8_t *ip_addr,
                                                  char *string_buf, uint32_t buf_len);
extern const char *net_mac_ntoa (const uint8_t *mac_addr);
extern const char *netMAC_ntoa (const uint8_t *mac_addr,
                                char *string_buf, uint32_t buf_len);
extern void net_addr4_to_mac (const uint8_t *ip4_addr, uint8_t *mac_addr);
extern const uint8_t *net_addr4_get_subcast (const NET_LOCALM *lm4);
extern bool net_addr4_is_subcast (const uint8_t *ip4_addr, const NET_LOCALM *lm4);
#ifdef Network_IPv6
extern bool net_addr6_aton (const char *cp, uint8_t *ip6_addr);
extern const char *net_addr6_ntoa (const uint8_t *ip6_addr);
extern const uint8_t *net_addr6_get_solicited (const uint8_t *ip6_addr);
extern bool net_addr6_is_solicited (const uint8_t *ip6_addr, const NET_LOCALM6 *lm6);
extern void net_addr6_create (uint8_t *ip6_addr, const uint8_t *prefix,
                                                 const uint8_t *suffix, bool eui64);
#define net_addr6_make_eui64(ip,pref,mac)   net_addr6_create(ip,pref,mac,true)
#define net_addr6_make_plain(ip,pref,suff)  net_addr6_create(ip,pref,suff,false)
extern bool net_addr6_is_linklocal (const uint8_t *ip6_addr);
extern bool net_addr6_is_4to6 (const uint8_t *ip6_addr);
extern bool net_addr6_is_onlink (const uint8_t *ip6_addr, const NET_LOCALM6 *lm6);
extern void net_addr6_to_mac (const uint8_t *ip6_addr, uint8_t *mac_addr);
extern void net_addr6_map_4to6 (const uint8_t *ip4_addr, uint8_t *ip6_addr);
#endif
#if defined(__ARM_FEATURE_UNALIGNED)
  __STATIC_FORCEINLINE void net_addr4_copy (uint8_t *dst, const uint8_t *src) {
    __UNALIGNED_UINT32_WRITE(&dst[0], __UNALIGNED_UINT32_READ(&src[0]));
  }
  __STATIC_FORCEINLINE void net_mac_copy (uint8_t *dst, const uint8_t *src) {
    __UNALIGNED_UINT32_WRITE(&dst[0], __UNALIGNED_UINT32_READ(&src[0]));
    __UNALIGNED_UINT16_WRITE(&dst[4], __UNALIGNED_UINT16_READ(&src[4]));
  }
  __STATIC_FORCEINLINE bool net_addr4_comp (const uint8_t *ip1, const uint8_t *ip2) {
    if (__UNALIGNED_UINT32_READ(&ip1[0]) == __UNALIGNED_UINT32_READ(&ip2[0])) {
      return (true);
    }
    return (false);
  }
  __STATIC_FORCEINLINE bool net_mac_comp (const uint8_t *mac1, const uint8_t *mac2) {
    if (__UNALIGNED_UINT32_READ(&mac1[0]) == __UNALIGNED_UINT32_READ(&mac2[0]) &&
        __UNALIGNED_UINT16_READ(&mac1[4]) == __UNALIGNED_UINT16_READ(&mac2[4])) {
      return (true);
    }
    return (false);
  }
 #ifdef Network_IPv6
  extern void net_addr6_copy (uint8_t *dst_addr, const uint8_t *src_addr);
  extern bool net_addr6_comp (const uint8_t *ip_addr1, const uint8_t *ip_addr2);
  extern bool net_addr6_is_unspec (const uint8_t *ip6_addr);
 #endif
#else
  #define net_addr4_copy(dst,src)   memcpy (dst, src, NET_ADDR_IP4_LEN)
  #define net_addr4_comp(ip1,ip2)  !memcmp (ip1, ip2, NET_ADDR_IP4_LEN)
  extern void net_mac_copy (uint8_t *dst, const uint8_t *src);
  extern bool net_mac_comp (const uint8_t *mac1, const uint8_t *mac2);
 #ifdef Network_IPv6
  #define net_addr6_copy(dst,src)   memcpy (dst, src, NET_ADDR_IP6_LEN)
  #define net_addr6_comp(ip1,ip2)  !memcmp (ip1, ip2, NET_ADDR_IP6_LEN)
  #define net_addr6_is_unspec(ip)  !memcmp (ip, net_addr_unspec, NET_ADDR_IP6_LEN)
 #endif
#endif
extern bool net_addr_is_unspec (const __ADDR *addr);
extern void net_addr_copy (__ADDR *dst_addr, const __ADDR *src_addr);
extern bool net_addr_comp (const __ADDR *addr1, const __ADDR *addr2);
extern NET_IF_CFG    *net_addr_find_route (NET_IF_CFG *net_if, const __ADDR *dst);
extern const uint8_t *net_addr_get_local (NET_IF_CFG *net_if, const __ADDR *dst);
extern void net_addr_get_lhost (int16_t addr_type, __ADDR *addr);

#endif /* __NET_ADDR_H */
