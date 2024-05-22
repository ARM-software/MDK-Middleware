/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_fat.h 
 * Purpose: FAT File System Module Definitions
 *----------------------------------------------------------------------------*/

#ifndef FS_FAT_H__
#define FS_FAT_H__

#include "fs_core.h"

/* FAT Volume Status Flags */
#define FAT_STATUS_INIT_IO    0x00000001U   /* Media driver initialized       */
#define FAT_STATUS_INIT_MEDIA 0x00000002U   /* Media layer initialized        */
#define FAT_STATUS_READY      0x00000004U   /* Volume ready (access allowed)  */
#define FAT_STATUS_MOUNT      0x00000008U   /* Volume mounted                 */
#define FAT_STATUS_WRITE      0x00000010U   /* Write Enabled                  */
#define FAT_STATUS_REMOVABLE  0x00000020U   /* Removable drive                */
#define FAT_STATUS_JOURACT    0x00000040U   /* FS journal is active           */
#define FAT_STATUS_JOURERR    0x00000080U   /* FS journal error               */
#define FAT_STATUS_FSINFO     0x00000100U   /* FSINFO structure updated       */

#define FAT_STATUS_MASK      (FAT_STATUS_INIT_IO    | \
                              FAT_STATUS_INIT_MEDIA | \
                              FAT_STATUS_READY      | \
                              FAT_STATUS_MOUNT      | \
                              FAT_STATUS_WRITE      | \
                              FAT_STATUS_REMOVABLE  | \
                              FAT_STATUS_JOURACT    | \
                              FAT_STATUS_JOURERR    | \
                              FAT_STATUS_FSINFO     )

/* FAT File Handle Flags */
#define FAT_HANDLE_READ       0x0001    /* File opened for read               */
#define FAT_HANDLE_WRITE      0x0002    /* File opened for write              */
#define FAT_HANDLE_APPEND     0x0004    /* File opened for append             */
#define FAT_HANDLE_PLUS       0x0008    /* File opened for update             */
#define FAT_HANDLE_ERROR      0x0010    /* File operation error               */
#define FAT_HANDLE_DATA_RD    0x0020    /* File data was read                 */
#define FAT_HANDLE_DATA_WR    0x0040    /* File data was written              */
#define FAT_HANDLE_SEEK       0x0080    /* Seek performed                     */

/* FAT File Handle Flag Masks */
#define FAT_HANDLE_OPEN      (FAT_HANDLE_READ   | \
                              FAT_HANDLE_WRITE  )

#define FAT_HANDLE_MODES     (FAT_HANDLE_READ   | \
                              FAT_HANDLE_WRITE  | \
                              FAT_HANDLE_APPEND | \
                              FAT_HANDLE_PLUS   )

/* FAT Name Analyse Flags */
#define FAT_NAME_SFN          (1 << 0)  /* Name is SFN valid                  */
#define FAT_NAME_LFN          (1 << 1)  /* Name is LFN valid                  */
#define FAT_NAME_WILDCARD     (1 << 2)  /* Wildcard found in a name           */
#define FAT_NAME_DIR          (1 << 3)  /* Name of a directory                */
#define FAT_NAME_LAST         (1 << 4)  /* Last name in path                  */
#define FAT_NAME_DOT          (1 << 5)  /* Name is "."                        */
#define FAT_NAME_DOTDOT       (1 << 6)  /* Name is ".."                       */

/* FAT Function Options */
#define FAT_OPTION_S          (1 << 0)  /* Option: "/S"                       */

/* Invalid sector */
#define INVAL_SECT            0xFFFFFFFFU

/* Path length (drive + colon + backslash + path + null character) */
#define MAX_PATH_LFN          260
#define MAX_PATH_SFN          80

#ifndef FS_FAT_NO_LFN
#define FAT_MAX_PATH          MAX_PATH_LFN
#else
#define FAT_MAX_PATH          MAX_PATH_SFN
#endif

/* FAT Types */
#define FS_RAW      0
#define FS_FAT12    1
#define FS_FAT16    2
#define FS_FAT32    3

/* FAT Select */
#define FAT_1       0
#define FAT_2       1

/* Action definitions */
#define ACT_NONE    0x00
#define ACT_USEJOUR 0x01

/* Entry position set commands */
#define ENTRY_POS_INCREMENT 0
#define ENTRY_POS_CURRENT   1

/* FAT Journal Signature ("FATJ") Definition */
#define SIGN_FATJ   0x4641544A

/* Long file name attribute mask */
#define ATTR_LONG_NAME_MASK  (FS_FAT_ATTR_READ_ONLY | \
                              FS_FAT_ATTR_HIDDEN    | \
                              FS_FAT_ATTR_SYSTEM    | \
                              FS_FAT_ATTR_VOLUME_ID | \
                              FS_FAT_ATTR_DIRECTORY | \
                              FS_FAT_ATTR_ARCHIVE   )

/* Long file name attribute */
#define ATTR_LONG_NAME       (FS_FAT_ATTR_READ_ONLY | \
                              FS_FAT_ATTR_HIDDEN    | \
                              FS_FAT_ATTR_SYSTEM    | \
                              FS_FAT_ATTR_VOLUME_ID )

/* Ordinal last part of long name flag */
#define ORD_LONG_NAME_LAST  0x40

/* Directory entry type */
typedef enum _ENTRY_TYPE {
  ENTRY_TYPE_LAST_IN_DIR = 0,
  ENTRY_TYPE_FREE,
  ENTRY_TYPE_LABEL,
  ENTRY_TYPE_LFN,
  ENTRY_TYPE_SFN
} ENTRY_TYPE;

/* Short Directory Entry */
typedef struct _FILEREC {
  uint8_t  FileName[11];                /* File name in format 8.3                                  */
  uint8_t  Attr;                        /* File Attribute flags                                     */
  uint8_t  NTRsvd;                      /* NT Reserved                                              */
  uint8_t  CrtTimeTenth;                /* Millisecond stamp at file creation time                  */
  uint16_t CrtTime;                     /* Time file was created                                    */
  uint16_t CrtDate;                     /* Date file was created                                    */
  uint16_t LastAccDate;                 /* Last access Date. Date of last read or write.            */
  uint16_t FirstClusHI;                 /* First Cluster HI (0 for FAT12/16)                        */
  uint16_t WriteTime;                   /* Time of last write. File creation is considered a write. */
  uint16_t WriteDate;                   /* Date of last write. File creation is considered a write. */
  uint16_t FirstClusLO;                 /* First Cluster LO                                         */
  uint32_t FileSize;                    /* File Size in bytes                                       */
} FILEREC;

/* Long Directory Entry */
typedef struct _LFN_FILEREC {
  uint8_t  Ordinal;                     /* Ordinal field                      */
  uint8_t  Unichar0[10];                /* Unicode characters 1  ..  5        */
  uint8_t  Attr;                        /* File Attribute flags               */
  uint8_t  Type;                        /* LFN subcomponent indicator (Rsvd)  */
  uint8_t  Checksum;                    /* Checksum of short file name        */
  uint8_t  Unichar1[12];                /* Unicode characters 6  .. 11        */
  uint16_t FirstClusLO;                 /* Cluster (unused, always 0)         */
  uint8_t  Unichar2[4];                 /* Unicode characters 12 .. 13        */
} LFN_FILEREC;

/* FAT Format Info Table */
typedef struct _DEVPAR {
  uint16_t NumHeads;                    /* Number of heads for disk           */
  uint16_t NumCyl;                      /* Number of cylinders for disk       */
  uint8_t  NumSect;                     /* Number of sectors for disk         */
  uint8_t  FatType;                     /* File System Fat type               */
  uint8_t  SecClus;                     /* Number of Sectors per Cluster      */
  uint8_t  SecClus32;                   /* Sectors per Cluster for FAT32      */
  uint16_t BootSector;                  /* Boot Record Sector Offset          */
} DEVPAR;

/* Directory Entry Position */
typedef struct entry_pos {
  uint32_t Clus;                        /* Cluster number of directory entry */
  uint16_t Offs;                        /* Entry offset from start of dir    */
  uint16_t Rsvd;                        /* Reserved (padding)                */
} ENTRY_POS;

/* File Record Position */
typedef struct frec_pos {
  ENTRY_POS pos;                        /* File entry position               */
  uint8_t   cnt;                        /* Number of entries                 */
  uint8_t   rsvd[3];                    /* Reserved (padding)                */
} FREC_POS;

/* Path Information Structure */
typedef struct path_info {
  uint32_t    dir_clus;                 /* Directory cluster                 */
  FREC_POS    frec;                     /* File record position              */
  FILEREC    *sfne;                     /* Short directory entry             */
  const char *fn;                       /* Current name path                 */
  uint8_t     fn_len;                   /* Current name path length          */
  uint8_t     fn_flags;                 /* Name analyse flags                */
  uint8_t     rsvd[2];                  /* Reserved (padding)                */
} PATH_INFO;


/* External variables */
extern fsFAT_Handle  fs_fat_fh[];       /* FAT file handle pool               */
extern uint8_t const fs_fat_fh_cnt;     /* Number of FAT file handles         */


/* Functions */
extern int32_t  fat_handle_get  (fsFAT_Volume *vol);
extern fsStatus fat_attrib      (fsFAT_Volume *vol, const char *fn, uint32_t attrib);

#endif /* FS_FAT_H__ */
