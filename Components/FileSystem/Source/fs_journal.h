/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_journal.h 
 * Purpose: File System Journaling Module Definitions
 *----------------------------------------------------------------------------*/

#ifndef FS_JOURNAL_H__
#define FS_JOURNAL_H__

#include "fs_core.h"

/* Definitions */
#define JOUR_SIGN     0x4B46534A    /* "KFSJ" */
#define JOUR_FOOT     0x464F4F54    /* "FOOT" */
#define JOUR_TEND     0x54454E44    /* "TEND" */
     
#define JOUR_TR_SZ    3             /* Journal transaction size in sectors    */

#define __SECT_SZ 512

/* Journal entry type */
#define JTYP_DEN    0 /* Directory entries */
#define JTYP_FAT    1 /* FAT entries       */
#define JTYP_DAT    2 /* Data sector       */

/* Journal states */
#define JST_IDLE    0
#define JST_COPY    1
#define JST_FOOT    2
#define JST_CHNG    3
#define JST_TEND    4

/* Journal status flags */
#define JSF_SPACERSVD (1U << 0)
#define JSF_INIT      (1U << 1)
#define JSF_RESTORED  (1U << 2)

/* Variables */
typedef struct journal_entry {
  uint32_t TrId;
  uint32_t Mark;
  uint32_t Sect;
  uint32_t RSec;
} JTR_EN;

/* Functions */
extern uint32_t fsj_set_space (FSJOUR *fsj, uint32_t start_sect, uint32_t cnt);
extern uint32_t fsj_write     (FSJOUR *fsj, uint32_t sect, uint32_t rsec, uint8_t *buf);
extern uint32_t fsj_init      (FSJOUR *fsj, FAT_DRV *drv);

#endif /* FS_JOURNAL_H__ */
