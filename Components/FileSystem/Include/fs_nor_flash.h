/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_nor_flash.h
 * Purpose: NOR Flash Layer Definitions for File System
 *----------------------------------------------------------------------------*/

#ifndef FS_NOR_FLASH_H__
#define FS_NOR_FLASH_H__

#include "rl_fs_lib.h"
#include "Driver_Flash.h"

/* Flash Device Timeouts */
typedef struct {
  uint16_t EraseChip;                   /* Chip erase operation timeout (s)   */
  uint16_t EraseSector;                 /* Sector erase operation timeout (s) */
  uint16_t Program;                     /* Program data operation timeout (s) */
  uint16_t Read;                        /* Read data operation timeout (s)    */
} FLASH_TIMEOUT;

/* Flash Device */
typedef struct {
  ARM_FLASH_SECTOR *SectArray;          /* Flash sectors array                */
  uint32_t          SectCnt;            /* Total number of sectors            */
  uint32_t          SectSize;           /* Uniform sector size                */
  FLASH_TIMEOUT    *Timeout;            /* Common operation timeouts          */
} FLASH_DEVICE;

/* NOR Flash Media control block */
typedef struct {
  ARM_DRIVER_FLASH       *Driver;       /* Registered Flash driver            */
  ARM_Flash_SignalEvent_t Callback;     /* Driver Callback                    */
  ARM_FLASH_CAPABILITIES  Capabilities; /* Driver Capabilities                */
  FLASH_DEVICE            Device;       /* Flash device description           */
  uint8_t                 Status;       /* Device status                      */
  uint8_t volatile        Event;        /* Driver event flags                 */
  uint8_t                 Rsvd[2];      /* Reserved (alignment)               */
} NOR_MEDIA;

/* NOR flash media interface functions */
extern fsStatus nor_Init             (NOR_MEDIA *nor);
extern fsStatus nor_Uninit           (NOR_MEDIA *nor);
extern uint32_t nor_GetSectorAddress (uint32_t sector, NOR_MEDIA *nor);
extern uint32_t nor_GetSectorSize    (uint32_t sector, NOR_MEDIA *nor);
extern fsStatus nor_DataWrite        (uint32_t addr, const void *buf, uint32_t cnt, NOR_MEDIA *nor);
extern fsStatus nor_DataRead         (uint32_t addr,       void *buf, uint32_t cnt, NOR_MEDIA *nor);
extern fsStatus nor_SectorErase      (uint32_t sector, NOR_MEDIA *nor);
extern fsStatus nor_DevCtrl          (fsDevCtrlCode code, void *p, NOR_MEDIA *nor);
extern void     nor_DrvEvent         (uint32_t event, NOR_MEDIA *nor);

#ifdef RTE_FileSystem_Debug
/* Media objects */
extern NOR_MEDIA fs_nor0;
extern NOR_MEDIA fs_nor1;
#endif

#endif /* FS_NOR_FLASH_H__ */
