/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_efs.h 
 * Purpose: Embedded File System Module Definitions
 *----------------------------------------------------------------------------*/

#ifndef FS_EFS_H__
#define FS_EFS_H__

#include "fs_core.h"

/* EFS Volume Status Flags */
#define EFS_STATUS_INIT_IO    0x00000001U   /* Media driver initialized       */
#define EFS_STATUS_MOUNT      0x00000002U   /* Volume mounted                 */

#define EFS_STATUS_MASK      (EFS_STATUS_INIT_IO | \
                              EFS_STATUS_MOUNT   )

/* EFS File Handle Flags */
#define EFS_HANDLE_READ       0x0001    /* File opened for read               */
#define EFS_HANDLE_WRITE      0x0002    /* File opened for write              */
#define EFS_HANDLE_APPEND     0x0004    /* File opened for append             */

#define EFS_HANDLE_DEFRAG     0x0020    /* EFS */
#define EFS_HANDLE_WALLOC     0x0040    /* EFS */
#define EFS_HANDLE_FSIZE      0x0080    /* EFS */

/* EFS File Handle Flag Masks */
#define EFS_HANDLE_OPEN      (EFS_HANDLE_READ   | \
                              EFS_HANDLE_WRITE  )

#define EFS_HANDLE_MODES     (EFS_HANDLE_READ   | \
                              EFS_HANDLE_WRITE  | \
                              EFS_HANDLE_APPEND )

/* Defragmentation buffer size */
#define CBUFLEN               256

/* Signature Flags (Block Usage Flags) */
#define BlockTEMP             0x03
#define BlockUSED             0x0F
#define BlockFULL             0xFF

/* Programable unit required by EFS */
#define EFS_PROG_UNIT         4U

/* File write operation states */
#define EFS_WR_DONE           0U
#define EFS_WR_ALLOC          1U
#define EFS_WR_DATA           2U
#define EFS_WR_FALLOC         3U
#define EFS_WR_ERROR          4U


/* Types */
typedef struct falloc {                 /* << File Allocation Info >>         */
  uint32_t end;                         /* Block End address                  */
  uint16_t fileID;                      /* File identification number         */
  uint16_t index;                       /* File data block index              */
} FALLOC;

/**
  Note that sector content is always as follows:

  LOW address: [    DATA    ]
               [ FREE SPACE ]
               [EMPTY FALLOC]
               [   FALLOC   ]
  HI  address: [ SIGNATURE  ]

  Sector must have enough free space for: (FALLOC + empty FALLOC + at least one byte of DATA).
  Block is full if the amount of free space is lower than or equal to (2 * sizeof(FALLOC)).
*/

/* External variables */
extern fsEFS_Handle  fs_efs_fh[];       /* EFS file handle pool               */
extern uint8_t const fs_efs_fh_cnt;     /* Number of EFS file handles         */

/* Function prototypes */
extern int32_t efs_handle_get (fsEFS_Volume *vol);

#endif /* FS_EFS_H__ */
