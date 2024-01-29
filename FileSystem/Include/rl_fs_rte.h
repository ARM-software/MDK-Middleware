/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    rl_fs_rte.h
 * Purpose: File System Run Time Environment Configuration
 *----------------------------------------------------------------------------*/

#ifndef RL_FS_RTE_H__
#define RL_FS_RTE_H__

#ifdef _RTE_
#include "RTE_Components.h"
#endif /* _RTE_ */

#if defined(RTE_FileSystem_Drive_MC_0) || defined(RTE_FileSystem_Drive_MC_1)
  #define FS_MEMORY_CARD
#endif

#if defined(RTE_FileSystem_Drive_NAND_0) || defined(RTE_FileSystem_Drive_NAND_1)
  #define FS_NAND_FLASH
#endif

#if defined(RTE_FileSystem_Drive_NOR_0) || defined(RTE_FileSystem_Drive_NOR_1)
  #define FS_NOR_FLASH
#endif

#if defined(RTE_FileSystem_LFN)
  #define FS_FAT_LFN
#endif

#if defined(RTE_FileSystem_Debug)
  #define FS_DEBUG
#endif

#endif /* RL_FS_RTE_H__ */
