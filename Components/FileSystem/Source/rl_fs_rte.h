/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    rl_fs_rte.h
 * Purpose: File System Run Time Environment Configuration
 *----------------------------------------------------------------------------*/

#ifndef RL_FS_RTE_H__
#define RL_FS_RTE_H__

#ifdef _RTE_
#include "RTE_Components.h"
#endif /* _RTE_ */

#if defined(RTE_FileSystem_Drive_RAM_0)
  #define FS_RAM_0
#endif
#if defined(RTE_FileSystem_Drive_RAM_1)
  #define FS_RAM_1
#endif

#if defined(RTE_FileSystem_Drive_NOR_0)
  #define FS_NOR_FLASH_0
#endif
#if defined(RTE_FileSystem_Drive_NOR_1)
  #define FS_NOR_FLASH_1
#endif

#if defined(RTE_FileSystem_Drive_NAND_0)
  #define FS_NAND_FLASH_0
#endif
#if defined(RTE_FileSystem_Drive_NAND_1)
  #define FS_NAND_FLASH_1
#endif

#if defined(RTE_FileSystem_Drive_MC_0)
  #define FS_MEMORY_CARD_0
#endif
#if defined(RTE_FileSystem_Drive_MC_1)
  #define FS_MEMORY_CARD_1
#endif

#if defined(RTE_FileSystem_Drive_USB_0)
  #define FS_USB_0
#endif
#if defined(RTE_FileSystem_Drive_USB_1)
  #define FS_USB_1
#endif

#if defined(RTE_CMSIS_View_EventRecorder)
  #define FS_DEBUG_EVR
#endif

#if defined(RTE_CMSIS_RTOS2)
  #define FS_RTOS_CMSIS_RTOS2

  #if defined(RTE_CMSIS_RTOS2_RTX5)
    #define FS_RTOS_RTX5
  #endif
#endif

#endif /* RL_FS_RTE_H__ */
