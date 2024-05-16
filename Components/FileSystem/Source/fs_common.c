/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_common.c 
 * Purpose: Common file system functions
 *----------------------------------------------------------------------------*/

#include <string.h>
#include <ctype.h>
#include "fs_common.h"
#include "fs_sys.h"
#include "fs_evr.h"

/* Local Variables */
static uint8_t fs_cdrive_id;            /* Current drive ID */


/**
  Set current drive
*/
fsStatus fs_set_cdrive (int32_t id) {
  fsStatus status;
  int32_t max_id;

  max_id = (int32_t)fs_ndrv;

  if ((id >= 0) && (id < max_id)) {
    fs_cdrive_id = (uint8_t)id;
    status = fsOK;
  } else {
    status = fsError;
  }

  return (status);
}


/**
  Get drive id from the absolute path.
  
  Drive specification format: "X[n]:"
  Drive letter and colon are mandatory specifications. Optionally, drive
  number \a n may be specified.

  \todo Drive specification format to support partitions: "X[n][.][p]:"
        Use dot '.' character as delimiter before partition number \a p
        specification.

  \return drive id or execution status
          - value >= 0: drive id
          - value <0: error occurred, -value is execution status as defined with fsStatus
*/
int32_t fs_drive_id (const char *path, const char **fn) {
  int32_t  id, cnt;
  FS_DEV  *dev;
  char     drive[3];

  if (path == NULL) {
    EvrFsCore_InvalidDrive (0);
    id = -(int32_t)fsInvalidParameter;
  }
  else {
    if ((path[0] != '\0') && (path[1] == ':')) {
      /* Drive letter X: specified, search for X0: */
      drive[0] = path[0];
      drive[1] = '0';

      if (fn != NULL) {
        *fn = &path[2];
      }
    }
    else if ((path[0] != '\0') && (path[2] == ':')) {
      /* Drive letter Xn: specified */
      drive[0] = path[0];
      drive[1] = path[1];

      if (fn != NULL) {
        *fn = &path[3];
      }
    }
    else {
      /* Drive letter not specified, use current drive */
      drive[0] = '\0';

      if (fn != NULL) {
        *fn = &path[0];
      }
    }

    if (drive[0] == '\0') {
      /* Return current drive */
      EvrFsCore_CurrentDriveSelect (get_u32((const uint8_t *)&fs_DevPool[fs_cdrive_id].id));
      id = (int32_t)fs_cdrive_id;
    }
    else {
      /* Find drive by letter */
      cnt = (int32_t)fs_ndrv;

      /* Convert to uppercase */
      drive[0] &= ~0x20;

      for (id = 0; id < cnt; id++) {
        dev = &fs_DevPool[id];

        if (dev->id[0] == drive[0]) {
          if (dev->id[1] == drive[1]) {
              break;
          }
        }
      }

      if (id == cnt) {
        /* Invalid or disabled drive specified */
        EvrFsCore_InvalidDrive (get_u32((const uint8_t *)path));
        id = -(int32_t)fsInvalidDrive;
      }
    }
  }

  return (id);
}


/**
  Find a position of 'ch' in a string
  
  \param[in] sp  string pointer
  \param[in] ch  character to find
  \return    Character position as an integer or -1 if not found
*/
int32_t fs_strpos (const char sp[], const char ch) {
  int32_t i;

  i = 0;
  while (sp[i] != ch) {
    if (sp[i] == '\0') {
      i = -1;
      break;
    }
    i++;
  }
  return (i);
}


/**
  Case insensitive compare of two char strings.

  The function strncasecmp is not part of the standard C library - it is a POSIX
  function and may not be available on all platforms, hence we provide our own
  function i.e. fs_strncasecmp.

  \param[in] s1  string pointer
  \param[in] s2  string pointer
  \param[in] n   number of characters to compare
  \return    0 if strings are the same, 1 otherwise
*/
uint32_t fs_strncasecmp (const char s1[], const char s2[], uint32_t n) {
  uint32_t i;
  uint8_t c1, c2;

  i = 0U;

  while (i < n) {
    c1 = (uint8_t)tolower(s1[i]);
    c2 = (uint8_t)tolower(s2[i]);

    if (c1 != c2) {
      /* No match */
      return (1U);
    }
    if (c1 == '\0') {
      /* End of strings */
      break;
    }
    i++;
  }
  /* Strings are the same */
  return (0U);
}


/**
  Compare first len1 and last len2 characters of strings s1 and s2

  If len1 and len2 are both zero, function returns 0.

  \param[in] s1  string pointer
  \param[in] s2  string pointer
  \param[in] len1  number of characters to compare from the beggining of the strings
  \param[in] len2  number of characters to compare before end of strings
  \return    0 if strings are the same, 1 otherwise
*/
uint32_t fs_strmatch (const char s1[], const char s2[], uint32_t len1, uint32_t len2) {
  uint32_t n1, n2;
  uint32_t match;

  match = 0U;

  if ((len1 != 0U) || (len2 != 0U)) {
    if (len1 != 0U) {
      /* Compare from the start */
      match = fs_strncasecmp (&s1[0], &s2[0], len1);
    }

    if (len2 != 0U) {
      n1 = strlen (s1);
      n2 = strlen (s2);

      if ((n1 >= len2) && (n2 >= len2)) {
        n1 -= len2;
        n2 -= len2;
        /* Compare part at the end */
        match |= fs_strncasecmp (&s1[n1], &s2[n2], len2);
      }
      else {
        /* Not long enough, no match */
        match = 1U;
      }
    }
  }

  return (match);
}


/**
  Read U16 byte-aligned from LE byte order

  \param[in] p16  pointer to 16-bit value stored in byte array
  \return 16-bit number as uint16_t
*/
uint16_t get_u16 (const uint8_t p16[]) {
  uint16_t retv;

  retv  = (uint16_t)(p16[1] << 8);
  retv |= (uint16_t)(p16[0]);

  return (retv);
}


/**
  Write U16 byte-aligned to LE byte order

  \param[out] p16  pointer to byte array where 16-bit value will be stored
  \param[in]  val  16-bit value
*/
void set_u16 (uint8_t p16[], uint16_t val) {
  p16[1] = (uint8_t)(val >> 8);
  p16[0] = (uint8_t)(val);
}


/**
  Read U32 byte-aligned from LE byte order

  \param[in] p32  pointer to 32-bit value stored in byte array
  \return 32-bit number as uint32_t
*/
uint32_t get_u32 (const uint8_t p32[]) {
  uint32_t retv;

  retv  = (uint32_t)p32[3] << 24;
  retv |= (uint32_t)p32[2] << 16;
  retv |= (uint32_t)p32[1] <<  8;
  retv |= (uint32_t)p32[0];

  return (retv);
}


/**
  Write U32 byte-aligned to LE byte order

  \param[out] p32  pointer to byte array where 32-bit value will be stored
  \param[in]  val  32-bit value
*/
void set_u32 (uint8_t p32[], uint32_t val) {
  p32[3] = (uint8_t)(val >> 24);
  p32[2] = (uint8_t)(val >> 16);
  p32[1] = (uint8_t)(val >> 8);
  p32[0] = (uint8_t)(val);
}
