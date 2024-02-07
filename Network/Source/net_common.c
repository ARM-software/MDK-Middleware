/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_common.c
 * Purpose: Common Network Functions
 *----------------------------------------------------------------------------*/

#include "net_common.h"

/* Local constants */
static const char hex_digit[16] = { "0123456789abcdef" };

/* Local functions */
static int32_t ul2str (char *dp, uint32_t val, int32_t n, char prefix);
static int32_t i2str  (char *dp, int32_t val, int32_t n, char prefix);
static uint8_t hex2uc (char ch);
static uint32_t crc32_8bit (uint32_t crc32, uint8_t val);

/**
  \brief       Compare two strings case sensitive.
  \param[in]   sp  source string.
  \param[in]   cp  compare null-terminated string.
  \return      check result:
               - true  = strings match,
               - false = strings are different.
  \note        String 'cp' defines length of comparison.
*/
bool net_strcmp (const char *sp, const char *cp) {
  for (  ; cp[0]; sp++, cp++) {
    if (sp[0] != cp[0]) {
      return (false);
    }
  }
  return (true);
}

/**
  \brief       Compare two strings case-insensitively.
  \param[out]  sp  source string.
  \param[in]   cp  compare null-terminated string.
  \return      check result:
               - true  = strings match,
               - false = strings are different.
  \note        String 'cp' defines length of comparison.
*/
bool net_strcasecmp (const char *sp, const char *cp) {
  int32_t i;

  for (i = 0; cp[i]; i++) {
    switch (sp[i] ^ cp[i]) {
      case 0x00:
        /* Equal characters */
        break;
      case 0x20:
        /* The same characters, different case */
        if ((cp[i] >= 'a' && cp[i] <= 'z') ||
            (cp[i] >= 'A' && cp[i] <= 'Z')) {
          break;
        }
        return (false);
      default:
        /* Different characters */
        return (false);
    }
  }
  return (true);
}

/**
  \brief       Copy a string.
  \param[out]  dp  destination string.
  \param[in]   sp  source null-terminated string.
  \return      number of characters copied.
               (null-termination not counted)
*/
int32_t net_strcpy (char *dp, const char *sp) {
  int32_t i;

  for (i = 0; ; i++) {
    if ((dp[i] = sp[i]) == 0) {
      /* Null-termination added */
      break;
    }
  }
  return (i);
}

/**
  \brief       Copy up to n characters of a string.
  \param[out]  dp  destination string.
  \param[in]   sp  source null-terminated string.
  \param[in]   n   maximum number of characters to copy.
  \return      number of characters copied.
               (null-termination not counted)
*/
int32_t net_strncpy (char *dp, const char *sp, uint32_t n) {
  int32_t i,nb = (int32_t)n;

  for (i = 0; i < nb; i++) {
    if ((dp[i] = sp[i]) == 0) {
      break;
    }
  }
  /* Add null-termination */
  dp[i] = 0;
  return (i);
}

/**
  \brief       Find a character in a string.
  \param[in]   sp  input null-terminated string.
  \param[in]   ch  character to search for.
  \return      character index in the string,
               -1  if not found.
*/
int32_t net_strchr (const char *sp, char ch) {
  int32_t i;

  for (i = 0; sp[i]; i++) {
    if (sp[i] == ch) {
      return (i);
    }
  }
  return (-1);
}

/**
  \brief       Find a character in a length limited string.
  \param[in]   sp  input string.
  \param[in]   ch  character to search for.
  \param[in]   n   number of characters to be searched.
  \return      character index in the string, 
               -1  if not found.
*/
int32_t net_strnchr (const char *sp, char ch, uint32_t n) {
  int32_t i,nb = (int32_t)n;

  for (i = 0; sp[i] && i < nb; i++) {
    if (sp[i] == ch) {
      return (i);
    }
  }
  return (-1);
}

/**
  \brief       Find last occurrence of character in a string.
  \param[in]   sp  input null-terminated string.
  \param[in]   ch  character to search for.
  \return      character index in the string,
               -1  if not found.
*/
int32_t net_strrchr (const char *sp, char ch) {
  int32_t i,idx = -1;

  for (i = 0; sp[i]; i++) {
    if (sp[i] == ch) {
      idx = i;
    }
  }
  return (idx);
}

/**
  \brief       Convert a string to upper case.
  \param[out]  dp  destination string.
  \param[in]   sp  source null-terminated string.
  \return      length of a source string.
*/
int32_t net_strtoupper (char *dp, const char *sp) {
  int32_t i;
  char ch;

  for (i = 0; ; i++) {
    ch = sp[i];
    if (ch >= 'a' && ch <= 'z') {
      ch -= 'a' - 'A';
    }
    dp[i] = ch;
    if (ch == 0) {
      break;
    }
  }
  return (i);
}

/**
  \brief       Locate a substring in a string.
  \param[in]   sp  main null-terminated string.
  \param[in]   cp  null-terminated substring.
  \return      substring position index in main string, 
               -1  if not found.
  \note        Finds the first instance of a substring in a string.
*/
int32_t net_strstr (const char *sp, const char *cp) {
  int32_t i,j;
  char ch;

  for (i = j = 0; sp[i]; i++) {
    if (sp[i] == cp[j]) {
      do {
        if (cp[++j] == 0) {
          return (i);
        }
        ch = sp[i+j];
      } while (ch == cp[j]);
      if (ch == 0) break;
      j = 0;
    }
  }
  return (-1);
}

/**
  \brief       Locate a substring in a length limited string.
  \param[in]   sp  main string.
  \param[in]   cp  null-terminated substring.
  \param[in]   n   maximum number of characters to scan.
  \return      substring position index in main string, 
               -1  if not found.
  \note        Finds the first instance of a substring in a string. Searches the
               first 'n' characters if the string length is longer than 'n'.
*/
int32_t net_strnstr (const char *sp, const char *cp, uint32_t n) {
  int32_t i,j,nb = (int32_t)n;
  char ch;

  for (j = 0; cp[j]; j++);
  nb -= j;
  for (i = j = 0; sp[i] && i <= nb; i++) {
    if (sp[i] == cp[j]) {
      do {
        if (cp[++j] == 0) {
          return (i);
        }
        ch = sp[i+j];
      } while (ch == cp[j]);
      if (ch == 0) break;
      j = 0;
    }
  }
  return (-1);
}

/**
  \brief       Calculate 32-bit CRC for a string.
  \param[in]   sp  source null-terminated string.
  \return      calculated CRC value.
*/
uint32_t net_strcrc (const char *sp) {
  uint32_t crc32;

  for (crc32 = 0xFFFFFFFF; *sp; sp++) {
    crc32 = crc32_8bit (crc32, *sp);
  }
  return (crc32);
}

/**
  \brief       Calculate 32-bit CRC for a buffer.
  \param[in]   buf    source buffer.
  \param[in]   len    buffer length.
  \param[in]   crc32  initial CRC value.
  \return      calculated CRC value.
*/
uint32_t net_crc_buf (const void *buf, uint32_t len, uint32_t crc32) {
  const uint8_t *sp;

  for (sp = buf; len > 0; sp++, len--) {
    crc32 = crc32_8bit (crc32, *sp);
  }
  return (crc32);
}

/**
  \brief       Calculate IP checksum for a buffer.
  \param[in]   buf  input buffer.
  \param[in]   len  length of data.
  \param[in]   sum  initial checksum value.
  \return      16-bit checksum.
*/
uint16_t net_chksum_buf (const void *buf, uint32_t len, uint32_t sum) {
  const uint8_t *bp = buf;
  int32_t i,nb = (int32_t)len;

  for (i = 0; i < nb-1; i += 2) {
    sum += ((uint32_t)bp[i] << 8 | bp[i+1]);
  }
  if (i != nb) {
    sum += (uint32_t)bp[i] << 8;
  }
  /* Handle 16-bit overflow */
  sum = (sum & 0xFFFF) + (sum >> 16);
  /* Add carry */
  sum += (sum >> 16);
  /* Return one's complement */
  return ((~sum) & 0xFFFF);
}

/**
  \brief       Convert buffer to hexadecimal string.
  \param[out]  dp   output hexadecimal string.
  \param[in]   buf  input binary data array.
  \param[in]   len  length of the data.
  \return      number of characters converted.
*/
int32_t net_bin2hex (char *dp, const void *buf, uint32_t len) {
  const uint8_t *sp;
  int32_t i,nb = (int32_t)len;

  for (sp = buf, i = 0; i < nb; dp += 2, i++) {
    dp[0] = hex_digit[sp[i] >> 4];
    dp[1] = hex_digit[sp[i] & 0x0F];
  }
  /* Add null-termination */
  dp[0] = 0;
  return (i*2);
}

/**
  \brief       Convert uint to hexadecimal string.
  \param[out]  dp   output hexadecimal string.
  \param[in]   val  unsigned int value.
  \return      length of the string.
*/
int32_t net_ultox (char *dp, uint32_t val) {
  char tmp[8];
  int32_t i,n = 0;

  i = 0;
  do {
    tmp[i++] = hex_digit[val & 0x0F];
    val >>= 4;
  } while (val != 0);
  do {
    dp[n++] = tmp[--i];
  } while (i > 0);
  /* Add null-termination */
  dp[n] = 0;
  return (n);
}

/**
  \brief       Convert uint to a decimal string.
  \param[out]  dp       output string.
  \param[in]   val      unsigned int value.
  \param[in]   n        requested number of characters.
  \param[in]   prefix   prefix character: ' ' or '0'.
  \return      length of the string.
*/
static int32_t ul2str (char *dp, uint32_t val, int32_t n, char prefix) {
  char tmp[12];
  int32_t i,k = 0;

  i = 0;
  do {
    tmp[i++] = (val % 10) + '0';
    val      = (val / 10);
  } while (val != 0);

  /* Add leading '0' or ' ' */
  while (n > i) {
    dp[k++] = prefix;
    n--;
  }

  do {
    dp[k++] = tmp[--i];
  } while (i > 0);

  /* Add null-termination */
  dp[k] = 0;
  return (k);
}

/**
  \brief       Convert uint to a decimal string.
  \param[out]  dp   output string.
  \param[in]   val  unsigned int value.
  \return      length of the string.
*/
int32_t net_ultoa (char *dp, uint32_t val) {
  return (ul2str (dp, val, 0, 0));
}

/**
  \brief       Convert uint to a decimal string space-prefixed.
  \param[out]  dp   output string.
  \param[in]   val  unsigned int value.
  \param[in]   n    requested string length (format "%nd").
  \return      length of the string.
*/
int32_t net_ultoa_n (char *dp, uint32_t val, int32_t n) {
  return (ul2str (dp, val, n, ' '));
}

/**
  \brief       Convert uint to a decimal string 0-prefixed.
  \param[out]  dp   output string.
  \param[in]   val  unsigned int value.
  \param[in]   n    requested string length (format "%0nd").
  \return      length of the string.
*/
int32_t net_ultoa_n0 (char *dp, uint32_t val, int32_t n) {
  return (ul2str (dp, val, n, '0'));
}

/**
  \brief       Convert int to a decimal string.
  \param[out]  dp       output string.
  \param[in]   val      int value.
  \param[in]   n        requested number of characters.
  \param[in]   prefix   prefix character: ' ' or '0'.
  \return      length of the string.
*/
static int32_t i2str (char *dp, int32_t val, int32_t n, char prefix) {
  int32_t i = 0;

  if (val < 0) {
    dp[i++] = '-';
    val     = -val;
    n--;
  }
  i += ul2str (dp, (uint32_t)val, n, prefix);
  return (i);
}

/**
  \brief       Convert int to decimal string.
  \param[out]  dp   output string.
  \param[in]   val  int value.
  \return      length of the string.
*/
int32_t net_itoa (char *dp, int32_t val) {
  return (i2str(dp, val, 0, 0));
}

/**
  \brief       Convert int to decimal string space-prefixed.
  \param[out]  dp   output string.
  \param[in]   val  int value.
  \param[in]   n    requested number of characters (format "%nd").
  \return      length of the string.
*/
int32_t net_itoa_n (char *dp, int32_t val, int32_t n) {
  return (i2str(dp, val, n, ' '));
}

/**
  \brief       Convert int to decimal string 0-prefixed.
  \param[out]  dp   output string with 0-prefix.
  \param[in]   val  int value.
  \param[in]   n    requested number of characters (format "%0nd").
  \return      length of the string.
*/
int32_t net_itoa_n0 (char *dp, int32_t val, int32_t n) {
  return (i2str(dp, val, n, '0'));
}

/**
  \brief       Convert decimal string to int.
  \param[in]   sp   input string.
  \param[out]  len  pointer to parsed length (optional, can be NULL).
  \return      converted int data.
  \note        Input range from -999999999 to +999999999.
*/
int32_t net_atoi (const char *sp, int32_t *len) {
  int32_t ch,ch0,m = 0,n = 0;
  int32_t val;

  /* Skip leading spaces, '+' and '-' */
  while ((ch0 = sp[m]) == ' ')      m++;
  if ((ch0 == '-') || (ch0 == '+')) m++;
  sp += m;

  /* Parse and convert a string */
  val = 0;
  do {
    ch = sp[n];
    if (ch >= '0' && ch <= '9') ch -= '0';
    else break;
    val = val * 10 + ch;
  } while (++n < 9);

  if (len != NULL) *len = m + n;
  if (ch0 == '-')  val = -val;
  return (val);
}

/**
  \brief       Convert ascii char to binary.
  \param[in]   ch  ascii char.
  \return      binary char.
*/
static uint8_t hex2uc (char ch) {
  if (ch >= '0' && ch <= '9') return (ch - '0');
  ch |= 0x20;
  if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 10);
  return (0);
}
/**
  \brief       Convert ascii hex to uchar.
  \param[in]   sp   input string.
  \return      converted uchar data.
*/
uint8_t net_xtouc (const char *sp) {
  return (hex2uc (sp[0]) * 16 + hex2uc (sp[1]));
}

/**
  \brief       Convert hexadecimal string to uint.
  \param[in]   sp   input string.
  \param[out]  len  pointer to parsed length (optional, can be NULL).
  \return      converted uint data.
*/
uint32_t net_xtoul (const char *sp, int32_t *len) {
  int32_t ch,m = 0,n = 0;
  uint32_t val;

  /* Skip leading spaces */
  while (sp[m] == ' ') m++;
  sp += m;

 /* Parse and convert a string */
  val = 0;
  do {
    ch = sp[n];
    if      (ch >= '0' && ch <= '9') ch -= '0';
    else if (ch >= 'a' && ch <= 'f') ch -= 'a' - 10;
    else if (ch >= 'A' && ch <= 'F') ch -= 'A' - 10;
    else break;
    val = val * 16 + (uint32_t)ch;
  } while (++n < 8);

  if (len != NULL) *len = m + n;
  return (val);
}

/**
  \brief       Calculate 32-bit CRC value.
  \param[in]   crc32  CRC initial value.
  \param[in]   val    input value.
  \return      calculated CRC value.
*/
static uint32_t crc32_8bit (uint32_t crc32, uint8_t val) {
  uint32_t i;

  crc32 ^= (uint32_t)val << 24;
  /* Partial loop unwind for better performance */
  for (i = 8; i > 0; i -= 2) {
    /* 1st iteration inlined */
    if (crc32 & 0x80000000) {
      crc32 <<= 1;
      crc32 ^= CRC32_POLY;
    }
    else {
      crc32 <<= 1;
    }
    /* 2nd iteration inlined */
    if (crc32 & 0x80000000) {
      crc32 <<= 1;
      crc32 ^= CRC32_POLY;
    }
    else {
      crc32 <<= 1;
    }
  }
  return (crc32);
}

#if !defined(__ARM_FEATURE_UNALIGNED)
/**
  \brief       Read 32-bit variable from network byte-order.
  \param[in]   addr  pointer to a variable.
  \return      32-bit result in host byte-order.
*/
uint32_t net_rd_u32 (const uint8_t *addr) {
  uint32_t retv;

  retv  = (uint32_t)addr[0] << 24;
  retv |= (uint32_t)addr[1] << 16;
  retv |= (uint32_t)addr[2] << 8;
  retv |= (uint32_t)addr[3];
  return (retv);
}

/**
  \brief       Write 32-bit variable to network byte-order.
  \param[in]   addr  pointer to a variable.
  \param[in]   val   value to write.
  \return      none.
*/
void net_wr_u32 (uint8_t *addr, uint32_t val) {
  addr[0] = (val >> 24) & 0xFF;
  addr[1] = (val >> 16) & 0xFF;
  addr[2] = (val >> 8)  & 0xFF;
  addr[3] = (val)       & 0xFF;
}
#endif
