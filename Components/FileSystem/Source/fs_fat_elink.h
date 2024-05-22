/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_fat_elink.h
 * Purpose: FAT File System Entry Link Cache definitions
 *----------------------------------------------------------------------------*/

#ifndef FS_FAT_ELINK_H__
#define FS_FAT_ELINK_H__

#include "fs_core.h"

/* ELINK_CMD Commands */
#define ELINK_CMD_DIR_REWIND      0     /* Set entry pointer to start of dir  */
#define ELINK_CMD_POS_INC         1     /* Increment position to next elink   */
#define ELINK_CMD_GET_LAST        2     /* Get last accessed entry link       */
#define ELINK_CMD_ALLOC           3     /* Allocate new entry link            */
#define ELINK_CMD_CHDIR           4     /* Set last link as current directory */
#define ELINK_CMD_FLUSH           5     /* Flush name cache                   */


/* ELINK_DIR controls */
#define ELINK_DIR_ROOT            0     /* Init path depth to root            */
#define ELINK_DIR_CD              1     /* Init path depth to current dir     */
#define ELINK_DIR_DEPTH_INC       2     /* Increment path depth               */
#define ELINK_DIR_DEPTH_DEC       3     /* Decrement path depth               */


/* ELINK direction */
#define ELINK_DIR_PREV            0     /* Direction: previous link           */
#define ELINK_DIR_NEXT            1     /* Direction: next link               */


/* Entry cache delete structure (8 bytes) */
typedef struct __elink_delete {
  struct __elink *Prev;                 /* Previous link used                 */
  struct __elink *Next;                 /* Next link used                     */
} ELINK_DEL;


/* Entry information structure (24 bytes) */
typedef struct __elink_info {
  uint32_t LongNameH;                   /* Long name hash                     */
  uint32_t ShortNameH;                  /* Short name hash                    */
  uint32_t DirClus;                     /* First cluster of entry directory   */
  uint32_t EntryClus;                   /* Entry cluster number               */
  uint32_t EntryOffs;                   /* Entry offset within a directory    */
  uint32_t EntryCount;                  /* Number of name entries             */
} ELINK_INFO;


/* Cached file entry link (48 bytes) */
typedef struct __elink {
  struct __elink *Back;                 /* Back link                          */
  struct __elink *Forw;                 /* Forward link                       */
  struct __elink *Prev;                 /* Previous link                      */
  struct __elink *Next;                 /* Next link                          */
  ELINK_DEL  Del;                       /* Entry link delete list             */
  ELINK_INFO Info;                      /* Entry informations                 */
} ELINK;


/* Used elink list (8 bytes) */
typedef struct __elink_used {
  struct __elink *Oldest;               /* Oldest elink used                  */
  struct __elink *Latest;               /* Latest elink used                  */
} ELINK_USED;


/* Elink cache control block (20 bytes) */
typedef struct __elink_cache_cb {
  ELINK_USED *Used;                     /* Oldest/latest usage info per depth */
  ELINK      *Free;                     /* Free link chain                    */
  uint32_t    Dir;                      /* User directory cluster             */
  uint8_t     Depth;                    /* User path depth                    */
  uint8_t     Depth_CD;                 /* Current directory depth            */
  uint8_t     Rsvd[6];                  /* Reserved for future use            */

} ELINK_CACHE_CB;


/* Public Functions */
uint32_t elink_init      (NCACHE *p);
uint32_t elink_dir       (uint32_t dir, uint32_t ctrl, NCACHE *p);
uint32_t elink_insert    (ELINK *nl, NCACHE *p);
uint32_t elink_delete    (ELINK *el, NCACHE *p);
uint32_t elink_scan      (uint32_t nameH, ELINK **el, NCACHE *p);
ELINK   *elink_cmd       (uint32_t cmd, NCACHE *p);

#endif /* FS_FAT_ELINK_H__ */
