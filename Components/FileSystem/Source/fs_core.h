/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_core.h
 * Purpose: File system core definitions
 *----------------------------------------------------------------------------*/

#ifndef __FS_CORE_H
#define __FS_CORE_H

#include "rl_fs.h"
#include "rl_fs_lib.h"
#include "rl_fs_rte.h"

#include "FS_Config.h"
#include "FS_Debug.h"

#if defined(FS_RAM_0)
#include "FS_Config_RAM_0.h"
#endif
#if defined(FS_RAM_1)
#include "FS_Config_RAM_1.h"
#endif

#if defined(FS_NOR_FLASH_0)
#include "FS_Config_NOR_0.h"
#endif
#if defined(FS_NOR_FLASH_1)
#include "FS_Config_NOR_1.h"
#endif

#if defined(FS_NAND_FLASH_0)
#include "FS_Config_NAND_0.h"
#endif
#if defined(FS_NAND_FLASH_1)
#include "FS_Config_NAND_1.h"
#endif

#if defined(FS_MEMORY_CARD_0)
#include "FS_Config_MC_0.h"
#endif
#if defined(FS_MEMORY_CARD_1)
#include "FS_Config_MC_1.h"
#endif

#if defined(FS_USB_0)
#include "FS_Config_USB_0.h"
#endif
#if defined(FS_USB_1)
#include "FS_Config_USB_1.h"
#endif

#if defined(FS_USB_0) || defined (FS_USB_1)
#include "rl_usb.h"
#endif

#if defined(CMSIS_device_header)
#include "cmsis_compiler.h"
#endif

#if defined(FS_RTOS_CMSIS_RTOS2)
#include "cmsis_os2.h"
#if defined(FS_RTOS_RTX5)
#include "rtx_os.h"
#endif
#endif

#if defined(FS_DEBUG_EVR)
#include "EventRecorder.h"
#endif

#if (FS_DEBUG_EVR_ENABLE == 1)
  #if !defined(FS_DEBUG_EVR)
    #error "FileSystem: debug events require CMSIS-View:Event Recorder component."
  #endif
  #define FS_DEBUG
#endif

#endif /* _FS_CORE_H */
