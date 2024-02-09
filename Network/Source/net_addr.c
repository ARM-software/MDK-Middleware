/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_addr.c
 * Purpose: Address Handling
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "net_ip4.h"
#ifdef Network_IPv6
#include "net_ip6.h"
#endif
#include "net_addr.h"

/* Global constants */

/* Unspecified address: IPv4, MAC, <IPv6> */
uint8_t const net_addr_unspec[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#ifdef Network_IPv6
                                      0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#endif
};

/* Broadcast address: IPv4, MAC */
uint8_t const net_addr_bcast[NET_ADDR_ETH_LEN] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/* Local functions */
static const char *ip4_ntoa (const uint8_t *ip4_addr, char *buf);
static const char *mac_ntoa (const uint8_t *mac_addr, char *buf);
#ifdef Network_IPv6
static const char *ip6_ntoa (const uint8_t *ip6_addr, char *buf);
#endif

/**
  \brief       Convert IPv4 address from text to binary form.
  \param[in]   cp        IPv4 address string.
  \param[out]  ip4_addr  4-byte address in binary form.
  \return      conversion result:
               - true  = successfull,
               - false = failed.
  \note        Address string is in dotted decimal notation.
               Example of IPv4 address string:
                 "192.168.1.100"
*/
bool net_addr4_aton (const char *cp, uint8_t *ip4_addr) {
  int32_t i,j,d,n;

  if (ip4_addr == NULL) {
    return (false);
  }
  /* Sanity check of IPv4 address string */
  for (i = j = d = 0; cp[i]; i++) {
    if (cp[i] >= '0' && cp[i] <= '9') {
      if (++j > 3) {
        /* More than 3 digits in section */
        return (false);
      }
      continue;
    }
    if ((j == 0) || (cp[i] != '.')) {
      /* Invalid character found */
      return (false);
    }
    d++;
    j = 0;
  }
  if ((i > 15) || (d != 3) || (j == 0)) {
    return (false);
  }
  /* Sanity check is ok */
  for (i = j = 0; i < 4; i++) {
    ip4_addr[i] = net_atoi (&cp[j], &n) & 0xFF;
    j += (n + 1);
  }
  return (true);
}

#ifdef Network_IPv6
/**
  \brief       Convert IPv6 address from text to binary form.
  \param[in]   cp        IPv6 address string.
  \param[out]  ip6_addr  16-byte address in binary form.
  \return      conversion result:
               - true  = successfull,
               - false = failed.
  \note        Address string is in packed text representation. [RFC5952, page 9]
               Examples of IPv6 address string:
                 "2001:db8:85a3:0:0:0:370:7334",
                 "2001:db8:85a3::370:7334",
                 "2001::1",
                 "2001::",
                 "::1",
                 "::"
*/
bool net_addr6_aton (const char *cp, uint8_t *ip6_addr) {
  int32_t i,j,n,sympos,len;
  uint32_t val;

  if (ip6_addr == NULL) {
    return (false);
  }
  /* Sanity check of IPv6 address string */
  sympos = -1;
  for (i = j = 0; cp[i]; i++) {
    if ((cp[i] >= '0' && cp[i] <= '9') ||
        (cp[i] >= 'a' && cp[i] <= 'f') ||
        (cp[i] >= 'A' && cp[i] <= 'F')) {
      if (++j > 4) {
        /* More than 4 chars in 16-bit field */
        return (false);
      }
      continue;
    }
    j = 0;
    if (cp[i] != ':') {
      /* Invalid character found */
      return (false);
    }
    /* Check the "::" symbol */
    if (cp[i+1] == ':') {
      if (sympos >= 0) {
        return (false);
      }
      /* Remember position of "::" symbol */
      sympos = i;
    }
  }
  len = i;
  if (len < 2) {
    /* The shortest address is "::" */
    return (false);
  }
  if (sympos < 0) {
    /* Symbol "::" not found */
    sympos = len;
  }
  /* Scan the left side of the "::" symbol */
  for (j = i = 0; j < sympos && i < 16; i += 2, j++) {
    val = net_xtoul (&cp[j], &n); j += n;
    set_u16 (&ip6_addr[i], val);
  }
  if (i == 16) {
    /* All converted or terminating "::" symbol */
    return (true);
  }
  /* Clear remaining bytes of IPv6 address */
  for (  ; i < 16; i += 2) {
    set_u16 (&ip6_addr[i], 0);
  }
  sympos++;
  /* Find start position on the right */
  for (j = len-1, i = 14; j > sympos; j--) {
    if (cp[j] == ':') i -= 2;
  }
  /* Scan the right side of the "::" symbol */
  for (j = j+1; i < 16; i += 2, j++) {
    val = net_xtoul (&cp[j], &n); j += n;
    set_u16 (&ip6_addr[i], val);
  }
  return (true);
}
#endif /* Network_IPv6 */

/**
  \brief       Convert IP address from text to binary form.
  \param[in]   addr_string  network address string.
  \param[in]   addr_type    network address type.
  \param[out]  ip_addr      IPv4 or IPv6 address in binary form.
  \return      conversion result:
               - true  = successfull,
               - false = failed.
*/
bool netIP_aton (const char *addr_string, int16_t addr_type, uint8_t *ip_addr) {
  if ((addr_string == NULL) || (ip_addr == NULL)) {
    return (false);
  }
  switch (addr_type) {
    case NET_ADDR_IP4:
      return (net_addr4_aton (addr_string, ip_addr));
#ifdef Network_IPv6
    case NET_ADDR_IP6:
      return (net_addr6_aton (addr_string, ip_addr));
#endif
  }
  return (false);
}

/**
  \brief       Convert MAC address from text to binary form.
  \param[in]   cp        MAC address string.
  \param[out]  mac_addr  6-byte MAC address.
  \return      conversion result:
               - true  = successfull,
               - false = failed.
  \note        Example of MAC address string "1E-30-6C-A2-45-5E"
*/
bool netMAC_aton (const char *cp, uint8_t *mac_addr) {
  int32_t i,j;

  if ((cp == NULL) || (mac_addr == NULL)) {
    return (false);
  }
  /* Sanity check of MAC address string */
  for (i = j = 0; cp[i]; i++) {
    if ((cp[i] >= '0' && cp[i] <= '9') ||
        (cp[i] >= 'a' && cp[i] <= 'f') ||
        (cp[i] >= 'A' && cp[i] <= 'F')) {
      if (++j > 2) {
        /* More than 2 chars in a field */
        return (false);
      }
      continue;
    }
    if ((j < 2) || (cp[i] != '-')) {
      return (false);
    }
    j = 0;
  }
  if (i != 17) {
    return (false);
  }
  /* Sanity check is ok */
  for (i = 0; i < 6; i++) {
    mac_addr[i] = net_xtouc (&cp[i*3]);
  }
  return (true);
}

/**
  \brief       Convert IPv4 address from binary to string.
  \param[in]   ip4_addr  IPv4 address in binary form.
  \param[out]  buf       string buffer.
  \return      pointer to converted string, null-terminated.
*/
static const char *ip4_ntoa (const uint8_t *ip4_addr, char *buf) {
  int32_t i,n;

  for (i = n = 0; ; i++) {
    n += net_itoa (&buf[n], ip4_addr[i]);
    if (i == 3) break;
    buf[n++] = '.';
  }
  return (buf);
}

/**
  \brief       Convert IPv4 address from binary to string.
  \param[in]   ip4_addr  IPv4 address in binary form.
  \return      pointer to converted string, null-terminated.
*/
const char *net_addr4_ntoa (const uint8_t *ip4_addr) {
  static char buf[16];

  /* Result is in static buffer */
  return (ip4_ntoa (ip4_addr, buf));
}

#ifdef Network_IPv6
/**
  \brief       Convert IPv6 address from binary to string.
  \param[in]   ip6_addr  IPv6 address in binary form.
  \return      pointer to converted string, null-terminated.
  \note        Address string is compressed according to RFC5952, page 9.
*/
static const char *ip6_ntoa (const uint8_t *ip6_addr, char *buf) {
  uint16_t *v16;
  int32_t i,j,nmax,idx;

  /* Temp buffer reuses last 16-bytes of buf, 16-bit aligned */
  v16 = &__ALIGNED_UINT16(((uint32_t)&buf[24] & ~1u));

  /* Read IPv6 address to temp host-byte-ordered buffer */
  for (i = 0; i < 8; i++) {
    v16[i] = get_u16 (&ip6_addr[i<<1]);
  }

  idx = 8;
  for (i = nmax = 0; i < 8-1; i++) {
    if (v16[i] != 0) {
      continue;
    }
    /* Find the largest block of consecutive zeros */
    for (j = i; j < 8-1; j++) {
      if (v16[j+1] != 0) {
        break;
      }
    }
    if (i == j) {
      continue;
    }
    if ((j - i) >= nmax) {
      /* Remember position and count */
      nmax = j - i + 1;
      idx  = i;
    }
    /* Skip already processed zeros */
    i = j;
  }
  /* Check prefix "0:0:0:0:0:ffff:" */
  if ((nmax == 5) && (idx == 0) && (v16[5] == 0xFFFF)) {
    /* Write IPv4-mapped address "::ffff:a.b.c.d" */
    j = net_strcpy (buf, "::ffff:");
    ip4_ntoa (&ip6_addr[12], &buf[j]);
    return (buf);
  }

  for (i = j = 0; i < idx;  ) {
    j += net_ultox (&buf[j], v16[i]);
    if (++i == idx) {
      break;
    }
    buf[j++] = ':';
  }
  if (i < 8) {
    /* Right-end not yet completed */
    buf[j++] = ':';
    for (i += nmax; i < 8; i++) {
      buf[j++] = ':';
      j += net_ultox (&buf[j], v16[i]);
    }
    if (buf[j-1] == ':') {
      buf[j++] = ':';
    }
  }
  /* Make string null-terminated */
  buf[j] = 0;
  return (buf);
}

/**
  \brief       Convert IPv6 address from binary to string.
  \param[in]   ip6_addr  IPv6 address in binary form.
  \return      pointer to converted string, null-terminated.
*/
const char *net_addr6_ntoa (const uint8_t *ip6_addr) {
  static char buf[40];

  /* Result is in static buffer */
  return (ip6_ntoa (ip6_addr, buf));
}
#endif /* Network_IPv6 */

/**
  \brief       Convert IP address from binary to text form.
  \param[in]   addr  network address IPv4 or IPv6.
  \return      pointer to converted string, null-terminated.
  \note        Return result is in static buffer.
*/
const char *net_addr_ntoa (const __ADDR *addr) {
  if (addr->addr_type == NET_ADDR_IP4) {
    return (net_addr4_ntoa (addr->addr));
  }
#ifdef Network_IPv6
  return (net_addr6_ntoa (addr->addr));
#else
  return ("");
#endif
}

/**
  \brief       Convert IP address from binary to text form.
  \param[in]   addr_type   network address type.
  \param[in]   ip_addr     IPv4 or IPv6 address in binary form.
  \param[out]  string_buf  buffer to store converted IP address to.
  \param[in]   buf_len     length of a buffer, IP4= 16 bytes, IP6= 40 bytes 
  \return      pointer to converted string or NULL in case of error.
*/
const char *netIP_ntoa (int16_t addr_type, const uint8_t *ip_addr,
                                           char *string_buf, uint32_t buf_len) {
  if ((string_buf == NULL) || (ip_addr == NULL)) {
    return (NULL);
  }
  switch (addr_type) {
    case NET_ADDR_IP4:
      if (buf_len >= 16) {
        return (ip4_ntoa (ip_addr, string_buf));
      }
      break;
#ifdef Network_IPv6
    case NET_ADDR_IP6:
      if (buf_len >= 40) {
        return (ip6_ntoa (ip_addr, string_buf));
      }
      break;
#endif
  }
  return (NULL);
}

/**
  \brief       Convert MAC address from binary to string.
  \param[in]   mac_addr  MAC address in binary.
  \param[out]  buf       buffer to store the string to.
  \return      pointer to converted string, null-terminated.
*/
static const char *mac_ntoa (const uint8_t *mac_addr, char *buf) {
  char tmp[16];
  int32_t i,n;

  net_bin2hex (tmp, mac_addr, 6);
  net_strtoupper (tmp, tmp);
  for (i = n = 0; ; i += 2, n += 3) {
    memcpy (&buf[n], &tmp[i], 2);
    if (i == 10) break;
    buf[n+2] = '-';
  }
  buf[n+2] = 0;
  return (buf);
}

/**
  \brief       Convert MAC address from binary to string.
  \param[in]   mac_addr  MAC address in binary.
  \return      pointer to converted string, null-terminated.
*/
const char *net_mac_ntoa (const uint8_t *mac_addr) {
  static char buf[18];
  /* Result is in static buffer */
  return (mac_ntoa (mac_addr, buf));
}

/**
  \brief       Convert MAC address from binary to text form.
  \param[in]   mac_addr    MAC address in binary form.
  \param[out]  string_buf  buffer to store converted MAC address to.
  \param[in]   buf_len     length of a string buffer, at least 18 characters.
  \return      pointer to converted string or NULL in case of error.
*/
const char *netMAC_ntoa (const uint8_t *mac_addr, char *string_buf, uint32_t buf_len) {
  if ((mac_addr == NULL) || (string_buf == NULL) || (buf_len < 18)) {
    return (NULL);
  }
  return (mac_ntoa (mac_addr, string_buf));
}

/**
  \brief       Map IPv4 multicast address to MAC address.
  \param[in]   ip4_addr  IPv4 address.
  \param[out]  mac_addr  MAC address.
  \return      none.
*/
void net_addr4_to_mac (const uint8_t *ip4_addr, uint8_t *mac_addr) {
  mac_addr[0] = 0x01;
  mac_addr[1] = 0x00;
  mac_addr[2] = 0x5E;
  mac_addr[3] = ip4_addr[1] & 0x7F;
  mac_addr[4] = ip4_addr[2];
  mac_addr[5] = ip4_addr[3];
}

/**
  \brief       Convert IPv4 address to directed-broadcast address.
  \param[in]   lm4  local machine info.
  \return      pointer to converted address.
  \note        Function stores the result in a static buffer.
*/
const uint8_t *net_addr4_get_subcast (const NET_LOCALM *lm4) {
  static uint32_t buf[NET_ADDR_IP4_LEN/4];
  uint32_t ip_lm,net_mask;

  /* Should work for LE and BE systems */
  ip_lm    = *__CONST_CAST(uint32_t *)lm4->IpAddr;
  net_mask = *__CONST_CAST(uint32_t *)lm4->NetMask;
  buf[0] = (ip_lm & net_mask) | ~net_mask;
  return ((uint8_t *)buf);
}

/**
  \brief       Check if IPv4 address is directed-broadcast address.
  \param[in]   ip4_addr  IPv4 address to check.
  \param[in]   lm4       local machine info.
  \return      address status:
               - true  = is subnet bcast,
               - false = not subnet bcast.
*/
bool net_addr4_is_subcast (const uint8_t *ip4_addr, const NET_LOCALM *lm4) {
  uint32_t ip,ip_lm,net_mask;

  /* Should work for LE and BE systems */
  ip_lm    = *__CONST_CAST(uint32_t *)lm4->IpAddr;
  net_mask = *__CONST_CAST(uint32_t *)lm4->NetMask;
  /* ip4_addr may not be aligned */
  memcpy (&ip, ip4_addr, NET_ADDR_IP4_LEN);

  if ((ip ^ ip_lm) & net_mask) {
    /* Network address is different */
    return (false);
  }
  if ((ip | net_mask) == 0xFFFFFFFF) {
    /* This is subnet broadcast on LAN */
    return (true);
  }
  return (false);
}

#ifdef Network_IPv6
/**
  \brief       Convert IPv6 address to Solicited-node multicast address.
  \param[in]   ip_addr  IPv6 address.
  \return      pointer to converted address.
  \note        Function stores the result in a static buffer.
               Solicited-node multicast address is "FF02::1:FFxx:xxxx",
               where xx:xxxx are last 3 bytes of IPv6 address.
*/
const uint8_t *net_addr6_get_solicited (const uint8_t *ip_addr) {
  static uint32_t buf[NET_ADDR_IP6_LEN/4];

  buf[0] = HTONL(0xFF020000);
  buf[1] = HTONL(0x00000000);
  buf[2] = HTONL(0x00000001);
  buf[3] = htonl(0xFF000000 | get_u32(&ip_addr[12]));
  return ((uint8_t *)buf);
}

/**
  \brief       Check if IPv6 address is solicited-node multicast address.
  \param[in]   ip6_addr  IPv6 address.
  \param[in]   lm6       local machine info.
  \return      check result:
               - true  = is solicited-node address,
               - false = not solicited-node address.
  \note        Solicited node multicast address is "FF02::1:FFxx:xxxx",
               where xx:xxxx are last 3 bytes of IPv6 address.
*/
bool net_addr6_is_solicited (const uint8_t *ip6_addr, const NET_LOCALM6 *lm6) {
  if ((get_u32 (&ip6_addr[0]) ==  0xFF020000) &&
      (get_u32 (&ip6_addr[4]) ==  0x00000000) &&
      (get_u32 (&ip6_addr[8]) ==  0x00000001)) {
    /* 96-bit prefix is ok, check also 32-bit suffix */
    uint32_t ip12 = get_u32 (&ip6_addr[12]);
    if (ip12 == (get_u32 (&lm6->LLAddr[12]) | 0xFF000000)) {
      /* Link-local solicited node address */
      return (true);
    }
    if (ip12 == (get_u32 (&lm6->IpAddr[12]) | 0xFF000000)) {
      /* Global IP solicited node address */
      return (true);
    }
    if (ip12 == (get_u32 (&lm6->TempAddr[12]) | 0xFF000000)) {
      /* Temporary IP solicited node address */
      return (true);
    }
  }
  return (false);
}

/**
  \brief       Create IPv6 address from paramters.
  \param[out]  ip6_addr  Constructed IPv6 address.
  \param[in]   prefix    address prefix, or NULL for Link-local address.
  \param[in]   suffix    address suffix, or MAC address.
  \param[in]   eui64     suffix type: true = EUI-64,
                                    false= plain binary.
  \return      none.
  \note        Link-local address:     "FE80::(EUI-64)" or
                                       "FE80::[suffix]"
               Unicast global address: "[prefix]:(EUI-64)" or
                                       "[prefix]:[suffix]"
               EUI-64 is 64-bit Extended Unique Identifier.
*/
void net_addr6_create (uint8_t *ip6_addr, const uint8_t *prefix,
                                          const uint8_t *suffix,  bool eui64) {
  uint32_t val0,val1;

  /* Address prefix (64-bits) */
  if (prefix != NULL) {
    /* Use provided prefix */
    val0 = get_u32 (&prefix[0]);
    val1 = get_u32 (&prefix[4]);
  }
  else {
    /* Link-local prefix */
    val0 = 0xFE800000;
    val1 = 0x00000000;
  }
  set_u32 (&ip6_addr[0], val0);
  set_u32 (&ip6_addr[4], val1);

  /* Interface identifier (64-bits) */
  if (eui64) {
    /* EUI-64 Identifier (derived from MAC address) */
    val0 = 0x000000FF | (get_u32(&suffix[0]) ^ 0x02000000);
    val1 = 0xFE000000 | (get_u32(&suffix[2]) & 0x00FFFFFF);
  }
  else {
    /* Plain binary Identifier */
    val0 = get_u32 (&suffix[0]);
    val1 = get_u32 (&suffix[4]);
  }
  set_u32 (&ip6_addr[8],  val0);
  set_u32 (&ip6_addr[12], val1);
}

/**
  \brief       Check if IPv6 address is link-local address.
  \param[in]   ip6_addr  IPv6 address.
  \return      check result:
               - true  = is link-local address,
               - false = not link-local address.
  \note        Link-local address prefix is "FE80::0"/64
*/
bool net_addr6_is_linklocal (const uint8_t *ip6_addr) {
 if ((get_u32 (&ip6_addr[0]) == 0xFE800000) &&
     (get_u32 (&ip6_addr[4]) == 0x00000000)) {
    return (true);
  }
  return (false);
}

/**
  \brief       Check if IPv6 address is IPv4-mapped address.
  \param[in]   ip6_addr  IPv6 address.
  \return      check result:
               - true  = is IPv4-mapped address,
               - false = not IPv4-mapped address.
  \note        IPv4-mapped address prefix is "::FFFF"/96
*/
bool net_addr6_is_4to6 (const uint8_t *ip6_addr) {
 if ((get_u32 (&ip6_addr[0]) == 0x00000000) &&
     (get_u32 (&ip6_addr[4]) == 0x00000000) &&
     (get_u32 (&ip6_addr[8]) == 0x0000FFFF)) {
    return (true);
  }
  return (false);
}

/**
  \brief       Check if IPv6 address is on-link.
  \param[in]   ip6_addr  IPv6 address to check.
  \param[in]   lm6       local machine info.
  \return      check result:
               - true  = is on-link address,
               - false = not on-link address.
*/
bool net_addr6_is_onlink (const uint8_t *ip6_addr, const NET_LOCALM6 *lm6) {
  const uint32_t *ip2 = __CONST_CAST(uint32_t *)lm6->IpAddr;
  uint32_t preflen = lm6->PrefixLen - 1;
  uint32_t mask;

  switch (preflen >> 5) {
    case 3:
      if (get_u32(ip6_addr) ^ ntohl(ip2[0])) {
        break;
      }
      ip6_addr += 4; ip2++;
      __FALLTHROUGH;
    case 2:
      if (get_u32(ip6_addr) ^ ntohl(ip2[0])) {
        break;
      }
      ip6_addr += 4; ip2++;
      __FALLTHROUGH;
    case 1:
      if (get_u32(ip6_addr) ^ ntohl(ip2[0])) {
        break;
      }
      ip6_addr += 4; ip2++;
      __FALLTHROUGH;
    case 0:
      mask = 0xFFFFFFFF << (31 - (preflen & 0x1F));
      if ((get_u32(ip6_addr) ^ ntohl(ip2[0])) & mask) {
        break;
      }
      return (true);
  }
  return (false);
}

/**
  \brief       Create IPv4-mapped IPv6 address.
  \param[out]  ip4_addr  input IPv4 address.
  \param[in]   ip6_addr  mapped IPv6 address.
  \return      none.
  \note        IPv4 mapped address is "::FFFF:[ip4_addr]"
*/
void net_addr6_map_4to6 (const uint8_t *ip4_addr, uint8_t *ip6_addr) {
  set_u32 (&ip6_addr[0], 0x00000000);
  set_u32 (&ip6_addr[4], 0x00000000);
  set_u32 (&ip6_addr[8], 0x0000FFFF);
  net_addr4_copy (&ip6_addr[12], ip4_addr);
}
#endif /* Network_IPv6 */

#if defined(__ARM_FEATURE_UNALIGNED)
 #ifdef Network_IPv6
/**
  \brief       Copy IPv6 addresses.
  \param[out]  dst_addr  destination address.
  \param[in]   src_addr  source address.
  \return      none.
*/
void net_addr6_copy (uint8_t *dst_addr, const uint8_t *src_addr) {
  __UNALIGNED_UINT32_WRITE (&dst_addr[0], __UNALIGNED_UINT32_READ(&src_addr[0]));
  __UNALIGNED_UINT32_WRITE (&dst_addr[4], __UNALIGNED_UINT32_READ(&src_addr[4]));
  __UNALIGNED_UINT32_WRITE (&dst_addr[8], __UNALIGNED_UINT32_READ(&src_addr[8]));
  __UNALIGNED_UINT32_WRITE (&dst_addr[12],__UNALIGNED_UINT32_READ(&src_addr[12]));
}

/**
  \brief       Compare IPv6 addresses.
  \param[in]   ip_addr1  address 1
  \param[in]   ip_addr2  address 2
  \return      address compare result:
               - true  = equal,
               - false = different.
*/
bool net_addr6_comp (const uint8_t *ip_addr1, const uint8_t *ip_addr2) {
  if (__UNALIGNED_UINT32_READ(&ip_addr1[0]) == __UNALIGNED_UINT32_READ(&ip_addr2[0]) &&
      __UNALIGNED_UINT32_READ(&ip_addr1[4]) == __UNALIGNED_UINT32_READ(&ip_addr2[4]) &&
      __UNALIGNED_UINT32_READ(&ip_addr1[8]) == __UNALIGNED_UINT32_READ(&ip_addr2[8]) &&
      __UNALIGNED_UINT32_READ(&ip_addr1[12])== __UNALIGNED_UINT32_READ(&ip_addr2[12])) {
    return (true);
  }
  return (false);
}

/**
  \brief       Check if IP address is unspecified address.
  \param[in]   ip6_addr  IPv6 address.
  \return      check result:
               - true  = is unspecified address,
               - false = address defined.
  \note        Unspecified address is "::"
*/
bool net_addr6_is_unspec (const uint8_t *ip6_addr) {
  if ((__UNALIGNED_UINT32_READ(&ip6_addr[0])  == 0) &&
      (__UNALIGNED_UINT32_READ(&ip6_addr[4])  == 0) &&
      (__UNALIGNED_UINT32_READ(&ip6_addr[8])  == 0) &&
      (__UNALIGNED_UINT32_READ(&ip6_addr[12]) == 0)) {
    return (true);
  }
  return (false);
}

/**
  \brief       Map IPv6 multicast address to MAC address.
  \param[in]   ip6_addr  IPv6 address.
  \param[out]  mac_addr  MAC address.
  \return      none.
*/
void net_addr6_to_mac (const uint8_t *ip6_addr, uint8_t *mac_addr) {
  __UNALIGNED_UINT16_WRITE (&mac_addr[0], 0x3333);
  __UNALIGNED_UINT32_WRITE (&mac_addr[2], __UNALIGNED_UINT32_READ(&ip6_addr[12]));
}
 #endif /* Network_IPv6 */
#else
 #ifdef Network_IPv6
/**
  \brief       Map IPv6 multicast address to MAC address.
  \param[in]   ip6_addr  IPv6 address.
  \param[out]  mac_addr  MAC address.
  \return      none.
*/
void net_addr6_to_mac (const uint8_t *ip6_addr, uint8_t *mac_addr) {
  mac_addr[0] = 0x33;
  mac_addr[1] = 0x33;
  mac_addr[2] = ip6_addr[12];
  mac_addr[3] = ip6_addr[13];
  mac_addr[4] = ip6_addr[14];
  mac_addr[5] = ip6_addr[15];
}
 #endif /* Network_IPv6 */

/**
  \brief       Compare MAC addresses.
  \param[in]   mac1  address 1
  \param[in]   mac2  address 2
  \return      compare result:
               - true  = address match,
               - false = different.
*/
bool net_mac_comp (const uint8_t *mac1, const uint8_t *mac2) {
  if ((mac1[0] == mac2[0]) && (mac1[1] == mac2[1]) &&
      (mac1[2] == mac2[2]) && (mac1[3] == mac2[3]) &&
      (mac1[4] == mac2[4]) && (mac1[5] == mac2[5])) {
    return (true);
  }
  return (false);
}

/**
  \brief       Copy MAC address.
  \param[out]  dst  destination address.
  \param[in]   src  source address.
  \return      none.
*/
void net_mac_copy (uint8_t *dst, const uint8_t *src) {
  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
  dst[3] = src[3];
  dst[4] = src[4];
  dst[5] = src[5];
}
#endif /* __ARM_FEATURE_UNALIGNED */

/**
  \brief       Check if IP address is unspecified address.
  \param[in]   addr  network address IPv4 or IPv6.
  \return      check result:
               - true  = unspecified address,
               - false = valid address.
  \note        Unspecified addresses: IPv4= "0.0.0.0", IPv6= "::"
*/
bool net_addr_is_unspec (const __ADDR *addr) {
  if (addr == NULL) {
    return (true);
  }
  switch (addr->addr_type) {
    case NET_ADDR_IP4:
      if (get_u32 (addr->addr) == 0) {
        break;
      }
      return (false);
#ifdef Network_IPv6
    case NET_ADDR_IP6:
      return (net_addr6_is_unspec (addr->addr));
#endif
  }
  return (true);
}

/**
  \brief       Copy IP address and port.
  \param[out]  dst_addr  destination IPv4 or IPv6 address.
  \param[in]   src_addr  source IPv4 or IPv6 address.
  \return      none.
*/
void net_addr_copy (__ADDR *dst_addr, const __ADDR *src_addr) {
  switch (src_addr->addr_type) {
    case NET_ADDR_IP4:
      dst_addr->addr_type = src_addr->addr_type;
      dst_addr->port      = src_addr->port;
      net_addr4_copy (dst_addr->addr, src_addr->addr);
      break;
#ifdef Network_IPv6
    case NET_ADDR_IP6:
      dst_addr->addr_type = src_addr->addr_type;
      dst_addr->port      = src_addr->port;
      net_addr6_copy (dst_addr->addr, src_addr->addr);
      break;
#endif
  }
}

/**
  \brief       Compare two IP addresses.
  \param[in]   addr1  address 1
  \param[in]   addr2  address 2
  \return      compare result:
               - true  = equal,
               - false = different.
*/
bool net_addr_comp (const __ADDR *addr1, const __ADDR *addr2) {
  if (addr1->addr_type != addr2->addr_type) {
    return (false);
  }
  switch (addr1->addr_type) {
    case NET_ADDR_IP4:
      return (net_addr4_comp (addr1->addr, addr2->addr));
#ifdef Network_IPv6
    case NET_ADDR_IP6:
      return (net_addr6_comp (addr1->addr, addr2->addr));
#endif
  }
  return (false);
}

/**
  \brief       Find route to destination address.
  \param[in]   net_if  proposed network interface (NULL to assign).
  \param[in]   dst     Destination IPv4 or IPv6 address.
  \return      assigned network interface for the route or
               - NULL if no route found.
*/
NET_IF_CFG *net_addr_find_route (NET_IF_CFG *net_if, const __ADDR *dst) {
  switch (dst->addr_type) {
    case NET_ADDR_IP4:
      return (net_ip4_find_route (net_if, dst->addr));
#ifdef Network_IPv6
    case NET_ADDR_IP6:
      return (net_ip6_find_route (net_if, dst->addr));
#endif
  }
  return (NULL);
}

/**
  \brief       Get local IP address for the route.
  \param[in]   net_if  assigned network interface.
  \param[in]   dst     Destination IP address.
  \return      Local/source IP address.
*/
const uint8_t *net_addr_get_local (NET_IF_CFG *net_if, const __ADDR *dst) {
  switch (dst->addr_type) {
    case NET_ADDR_IP4:
      return (net_ip4_get_local (net_if, dst->addr));
#ifdef Network_IPv6
    case NET_ADDR_IP6:
      return (net_ip6_get_local (net_if, dst->addr));
#endif
  }
  return (net_addr_unspec);
}

/**
  \brief       Get loopback address for a localhost.
  \param[in]   addr_type  network address type.
  \param[out]  addr       Loopback IPv4 or IPv6 address.
  \return      none.
*/
void net_addr_get_lhost (int16_t addr_type, __ADDR *addr) {
  switch (addr_type) {
    case NET_ADDR_IP4:
      addr->addr_type = addr_type;
      net_addr4_copy (addr->addr, net_addr4_loopback);
      break;
#ifdef Network_IPv6
    case NET_ADDR_IP6:
      addr->addr_type = addr_type;
      net_addr6_copy (addr->addr, net_addr6_loopback);
      break;
#endif
  }
}
