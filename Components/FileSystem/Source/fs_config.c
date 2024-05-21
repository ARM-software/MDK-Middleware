/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_config.h
 * Purpose: File System Library Configuration
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "fs_core.h"

/* ---------------------------------------------------------------------------*/
/* Reject MicroLib since it does not provide retargeting */
#ifdef __MICROLIB
  #error "File System does not work with MicroLIB"
#endif

/* ---------------------------------------------------------------------------*/
/* Provide definitions for undefined drives */
#ifndef NOR0_ENABLE
  #define NOR0_ENABLE   0
#endif
#ifndef NOR1_ENABLE
  #define NOR1_ENABLE   0
#endif
#ifndef MC0_ENABLE
  #define MC0_ENABLE    0
#endif
#ifndef MC1_ENABLE
  #define MC1_ENABLE    0
#endif
#ifndef NAND0_ENABLE
  #define NAND0_ENABLE  0
#endif
#ifndef NAND1_ENABLE
  #define NAND1_ENABLE  0
#endif
#ifndef RAM0_ENABLE
  #define RAM0_ENABLE   0
#endif
#ifndef RAM1_ENABLE
  #define RAM1_ENABLE   0
#endif
#ifndef USB0_ENABLE
  #define USB0_ENABLE   0
#endif
#ifndef USB1_ENABLE
  #define USB1_ENABLE   0
#endif
/* ---------------------------------------------------------------------------*/
#ifndef FS_INITIAL_CDRIVE
/* Initial Current Drive <0=>F0: <1=>F1:
                         <2=>M0: <3=>M1:
                         <4=>N0: <5=>N1:
                         <6=>R0: <9=>R1:
                         <7=>U0: <8=>U1: */
  #define FS_INITIAL_CDRIVE   -1
#else
  /* Verify correct current drive configuration */
  #if ((FS_INITIAL_CDRIVE < 0) || (FS_INITIAL_CDRIVE > 9))
    #error "Initial Current Drive configuration invalid"
  #endif
#endif
/* ---------------------------------------------------------------------------*/
/* Initial Current Drive configuration */
#define NOR0_CURRENT_DRIVE      (FS_INITIAL_CDRIVE == 0)
#define NOR1_CURRENT_DRIVE      (FS_INITIAL_CDRIVE == 1)
#define MC0_CURRENT_DRIVE       (FS_INITIAL_CDRIVE == 2)
#define MC1_CURRENT_DRIVE       (FS_INITIAL_CDRIVE == 3)
#define NAND0_CURRENT_DRIVE     (FS_INITIAL_CDRIVE == 4)
#define NAND1_CURRENT_DRIVE     (FS_INITIAL_CDRIVE == 5)
#define RAM0_CURRENT_DRIVE      (FS_INITIAL_CDRIVE == 6)
#define RAM1_CURRENT_DRIVE      (FS_INITIAL_CDRIVE == 9)
#define USB0_CURRENT_DRIVE      (FS_INITIAL_CDRIVE == 7)
#define USB1_CURRENT_DRIVE      (FS_INITIAL_CDRIVE == 8)
/* ---------------------------------------------------------------------------*/
/* Check if initial drive is enabled */
#if ((FS_INITIAL_CDRIVE == 0) && (NOR0_ENABLE  == 0)) || \
    ((FS_INITIAL_CDRIVE == 1) && (NOR1_ENABLE  == 0)) || \
    ((FS_INITIAL_CDRIVE == 2) && (MC0_ENABLE   == 0)) || \
    ((FS_INITIAL_CDRIVE == 3) && (MC1_ENABLE   == 0)) || \
    ((FS_INITIAL_CDRIVE == 4) && (NAND0_ENABLE == 0)) || \
    ((FS_INITIAL_CDRIVE == 5) && (NAND1_ENABLE == 0)) || \
    ((FS_INITIAL_CDRIVE == 6) && (RAM0_ENABLE  == 0)) || \
    ((FS_INITIAL_CDRIVE == 9) && (RAM1_ENABLE  == 0)) || \
    ((FS_INITIAL_CDRIVE == 7) && (USB0_ENABLE  == 0)) || \
    ((FS_INITIAL_CDRIVE == 8) && (USB1_ENABLE  == 0))
  #error "Initial Current ::File System:Drive is not enabled in FS_Config.c"
#endif
/* ---------------------------------------------------------------------------*/
/* Check number of enabled drives */
#define FS_NDRV (NOR0_ENABLE  + NOR1_ENABLE +   \
                 RAM0_ENABLE  + RAM1_ENABLE +   \
                 MC0_ENABLE   + MC1_ENABLE  +   \
                 USB0_ENABLE  + USB1_ENABLE +   \
                 NAND0_ENABLE + NAND1_ENABLE)
#if (FS_NDRV == 0)
  #error "No drive enabled, File System requires at least one component ::File System:Drive"
#endif

/* Count number of enabled FAT drives */
#define FS_FAT_NDRV (MC0_ENABLE   + MC1_ENABLE   + \
                     NAND0_ENABLE + NAND1_ENABLE + \
                     RAM0_ENABLE  + RAM1_ENABLE  + \
                     USB0_ENABLE  + USB1_ENABLE)
                     

/* Count number of enabled NAND devices */
#define FS_NAND_NDRV  (NAND0_ENABLE + NAND1_ENABLE)


/* Check MCI port consistency */
#if (MC0_ENABLE && MC1_ENABLE)
  #if   ((MC0_SPI == 0) && (MC1_SPI == 0))
    #if (MC0_MCI_DRIVER == MC1_MCI_DRIVER)
    #error "::File System:Drive:Memory Card: MCI ports for different drives must be different"
    #endif
  #elif ((MC0_SPI == 1) && (MC1_SPI == 1))
    #if (MC0_SPI_DRIVER == MC1_SPI_DRIVER)
    #error "::File System:Drive:Memory Card: SPI ports for different drives must be different"
    #endif
  #endif
#endif

/* ---------------------------------------------------------------------------*/
/* Check FS usage */
#define EFS_USE (NOR0_ENABLE + NOR1_ENABLE)
#define FAT_USE (MC0_ENABLE + MC1_ENABLE + NAND0_ENABLE + NAND1_ENABLE + RAM0_ENABLE + RAM1_ENABLE + USB0_ENABLE + USB1_ENABLE)

/* ---------------------------------------------------------------------------*/
/* Check journal usage */
#ifndef MC0_FAT_JOURNAL
  #define MC0_FAT_JOURNAL   0
#endif
#ifndef MC1_FAT_JOURNAL
  #define MC1_FAT_JOURNAL   0
#endif
#ifndef NAND0_FAT_JOURNAL
  #define NAND0_FAT_JOURNAL 0
#endif
#ifndef NAND1_FAT_JOURNAL
  #define NAND1_FAT_JOURNAL 0
#endif
#ifndef USB0_FAT_JOURNAL
  #define USB0_FAT_JOURNAL  0
#endif
#ifndef USB1_FAT_JOURNAL
  #define USB1_FAT_JOURNAL  0
#endif

#define FSJ_USE (MC0_ENABLE   * MC0_FAT_JOURNAL   +  \
                 MC1_ENABLE   * MC1_FAT_JOURNAL   +  \
                 NAND0_ENABLE * NAND0_FAT_JOURNAL +  \
                 NAND1_ENABLE * NAND1_FAT_JOURNAL +  \
                 USB0_ENABLE  * USB0_FAT_JOURNAL  +  \
                 USB1_ENABLE  * USB1_FAT_JOURNAL)

/* ---------------------------------------------------------------------------*/
/* Check name cache usage */
#ifndef MC0_NAME_CACHE_SIZE
  #define MC0_NAME_CACHE_SIZE 0
#endif
#ifndef MC1_NAME_CACHE_SIZE
  #define MC1_NAME_CACHE_SIZE 0
#endif
#ifndef NAND0_NAME_CACHE_SIZE
  #define NAND0_NAME_CACHE_SIZE 0
#endif
#ifndef NAND1_NAME_CACHE_SIZE
  #define NAND1_NAME_CACHE_SIZE 0
#endif
#ifndef USB0_NAME_CACHE_SIZE
  #define USB0_NAME_CACHE_SIZE 0
#endif
#ifndef USB1_NAME_CACHE_SIZE
  #define USB1_NAME_CACHE_SIZE 0
#endif

#define NCACHE_USE (MC0_NAME_CACHE_SIZE   + MC1_NAME_CACHE_SIZE   + \
                    NAND0_NAME_CACHE_SIZE + NAND1_NAME_CACHE_SIZE + \
                    USB0_NAME_CACHE_SIZE  + USB1_NAME_CACHE_SIZE)

/* Reserved sector count */
#define FAT_SECT_RSVD 64

/* ---------------------------------------------------------------------------*/
/* Support for drive buffer memory placement into linker sections */

/* Common per-drive section names */
#ifndef SECTION_MCI
#define SECTION_MCI  ".driver.mci"
#endif
#ifndef SECTION_NAND
#define SECTION_NAND ".driver.nand"
#endif
#ifndef SECTION_SPI
#define SECTION_SPI  ".driver.spi"
#endif

/* RAM file system location: the buffer that contains the file system
   is placed into the SECTION_RAM section. */
#ifndef SECTION_RAM
#define SECTION_RAM  ".filesystem.ram"
#endif

/* Macro that creates section name from string and integer value */
#ifndef __IN_SECTION_DEF
  /* Default section name */
  #define __PP_STRINGIFY(x) #x
  #define __IN_SECTION_DEF(name, num) __attribute__((section(name __PP_STRINGIFY(num))))
#endif
#ifndef __IN_SECTION_USR
  /* User specified section name */
  #define __IN_SECTION_USR(name)      __attribute__((section(name)))
#endif

/* ---------------------------------------------------------------------------*/
/* Embedded File System Operation Timeouts (absolute maximum, in seconds) */
#ifndef NOR0_TOUT_ERASE_CHIP            /* NOR0 chip erase timeout */
  #define NOR0_TOUT_ERASE_CHIP   300
#endif
#ifndef NOR0_TOUT_ERASE_SECTOR          /* NOR0 sector erase timeout */
  #define NOR0_TOUT_ERASE_SECTOR 5
#endif
#ifndef NOR0_TOUT_PROGRAM               /* NOR0 data program timeout */
  #define NOR0_TOUT_PROGRAM      1
#endif
#ifndef NOR0_TOUT_READ                  /* NOR0 data read timeout */
  #define NOR0_TOUT_READ         1
#endif

#ifndef NOR1_TOUT_ERASE_CHIP            /* NOR0 chip erase timeout */
  #define NOR1_TOUT_ERASE_CHIP   300
#endif
#ifndef NOR1_TOUT_ERASE_SECTOR          /* NOR0 sector erase timeout */
  #define NOR1_TOUT_ERASE_SECTOR 5
#endif
#ifndef NOR1_TOUT_PROGRAM               /* NOR0 data program timeout */
  #define NOR1_TOUT_PROGRAM      1
#endif
#ifndef NOR1_TOUT_READ                  /* NOR0 data read timeout */
  #define NOR1_TOUT_READ         1
#endif

/* ---------------------------------------------------------------------------*/
/* Extern definitions for media object */
#ifdef FS_DEBUG
extern MC_MCI       fs_mc0_mci;
extern MC_SPI       fs_mc0_spi;
extern MC_MCI       fs_mc1_mci;
extern MC_SPI       fs_mc1_spi;
extern NAND_FTL_DEV fs_nand0_handle;
extern NAND_FTL_DEV fs_nand1_handle;
extern NOR_MEDIA    fs_nor0;
extern NOR_MEDIA    fs_nor1;
extern RAM_DEV      fs_ram0_dev;
#endif

/* ---------------------------------------------------------------------------*/
/* File Control Blocks for the FAT File System */
#if (FAT_USE == 0 || FAT_MAX_OPEN_FILES == 0)
fsFAT_Handle  fs_fat_fh[1];
uint8_t const fs_fat_fh_cnt = 0;
#else
fsFAT_Handle  fs_fat_fh[FAT_MAX_OPEN_FILES];
uint8_t const fs_fat_fh_cnt = FAT_MAX_OPEN_FILES;
#endif

/* File Control Blocks for the Embedded File System */
#if (EFS_USE == 0 || EFS_MAX_OPEN_FILES == 0)
fsEFS_Handle  fs_efs_fh[1];
uint8_t const fs_efs_fh_cnt = 0;
#else
fsEFS_Handle  fs_efs_fh[EFS_MAX_OPEN_FILES];
uint8_t const fs_efs_fh_cnt = EFS_MAX_OPEN_FILES;
#endif

/* Number of enabled drives */
uint8_t const fs_ndrv = FS_NDRV;

 /* FAT Name Cache definitions */
#define FAT_NCACHE_LINK_SZ  (48)
#define FAT_NCACHE_STAT_SZ  (20)
#define FAT_NCACHE_USED_SZ  (8)

/* Expansion macro used to create CMSIS Driver references */
#define EXPAND_SYMBOL(name, port) name##port
#define CREATE_SYMBOL(name, port) EXPAND_SYMBOL(name, port)

/*----------------------------------------------------------------------------
 *  Drive F0: NOR Flash device NOR0 configuration
 *---------------------------------------------------------------------------*/
#if (NOR0_ENABLE)
  static fsEFS_Volume  fs_nor0_vol;

  static FLASH_TIMEOUT fs_nor0_flash_tout = {
    NOR0_TOUT_ERASE_CHIP,
    NOR0_TOUT_ERASE_SECTOR,
    NOR0_TOUT_PROGRAM,
    NOR0_TOUT_READ
  };

  #ifndef FS_DEBUG
  static
  #endif
  NOR_MEDIA fs_nor0;

  /* EFS Driver Event Callback */
  static void NOR0_Flash_SignalEvent (uint32_t event) {
    nor_DrvEvent (event, &fs_nor0);
  }

  extern ARM_DRIVER_FLASH CREATE_SYMBOL (Driver_Flash, NOR0_DRIVER);

  /* NOR0 wrapper functions */
  static fsStatus nor0_Init (void) {
    return (nor_Init (&fs_nor0));
  }
  static fsStatus nor0_UnInit (void) {
    return (nor_Uninit (&fs_nor0));
  }
  static uint32_t nor0_GetSectorAddress (uint32_t sector) {
    return (nor_GetSectorAddress (sector, &fs_nor0));
  }
  static uint32_t nor0_GetSectorSize (uint32_t sector) {
    return (nor_GetSectorSize (sector, &fs_nor0));
  }
  static fsStatus nor0_DataWrite (uint32_t addr, void *buf, uint32_t cnt) {
    return (nor_DataWrite (addr, buf, cnt, &fs_nor0));
  }
  static fsStatus nor0_DataRead (uint32_t addr, void *buf, uint32_t cnt) {
    return (nor_DataRead (addr, buf, cnt, &fs_nor0));
  }
  static fsStatus nor0_SectorErase (uint32_t sector) {
    return (nor_SectorErase (sector, &fs_nor0));
  }
  static fsStatus nor0_DevCtrl (fsDevCtrlCode code, void *p) {
    return (nor_DevCtrl (code, p, &fs_nor0));
  }

  /* NOR0 EFS device driver */
  static EFS_DRV fs_nor0_drv = {
    nor0_Init,
    nor0_UnInit,
    nor0_GetSectorAddress,
    nor0_GetSectorSize,
    nor0_DataWrite,
    nor0_DataRead,
    nor0_SectorErase,
    nor0_DevCtrl
  };

#endif

/*----------------------------------------------------------------------------
 *  Drive F1: NOR Flash device NOR1 configuration
 *---------------------------------------------------------------------------*/
#if (NOR1_ENABLE)
  static fsEFS_Volume  fs_nor1_vol;

  static FLASH_TIMEOUT fs_nor1_flash_tout = {
    NOR1_TOUT_ERASE_CHIP,
    NOR1_TOUT_ERASE_SECTOR,
    NOR1_TOUT_PROGRAM,
    NOR1_TOUT_READ
  };

  #ifndef FS_DEBUG
  static
  #endif
  NOR_MEDIA fs_nor1;

  /* EFS Driver Event Callback */
  static void NOR1_Flash_SignalEvent (uint32_t event) {
    nor_DrvEvent (event, &fs_nor1);
  }

  extern ARM_DRIVER_FLASH CREATE_SYMBOL (Driver_Flash, NOR1_DRIVER);

  /* NOR1 wrapper functions */
  static fsStatus nor1_Init (void) {
    return (nor_Init (&fs_nor1));
  }
  static fsStatus nor1_UnInit (void) {
    return (nor_Uninit (&fs_nor1));
  }
  static uint32_t nor1_GetSectorAddress (uint32_t sector) {
    return (nor_GetSectorAddress (sector, &fs_nor1));
  }
  static uint32_t nor1_GetSectorSize (uint32_t sector) {
    return (nor_GetSectorSize (sector, &fs_nor1));
  }
  static fsStatus nor1_DataWrite (uint32_t addr, void *buf, uint32_t cnt) {
    return (nor_DataWrite (addr, buf, cnt, &fs_nor1));
  }
  static fsStatus nor1_DataRead (uint32_t addr, void *buf, uint32_t cnt) {
    return (nor_DataRead (addr, buf, cnt, &fs_nor1));
  }
  static fsStatus nor1_SectorErase (uint32_t sector) {
    return (nor_SectorErase (sector, &fs_nor1));
  }
  static fsStatus nor1_DevCtrl (fsDevCtrlCode code, void *p) {
    return (nor_DevCtrl (code, p, &fs_nor1));
  }

  /* NOR1 EFS device driver */
  static EFS_DRV fs_nor1_drv = {
    nor1_Init,
    nor1_UnInit,
    nor1_GetSectorAddress,
    nor1_GetSectorSize,
    nor1_DataWrite,
    nor1_DataRead,
    nor1_SectorErase,
    nor1_DevCtrl
  };
#endif

/*----------------------------------------------------------------------------
 *  Drive M0: Memory Card device MC0 configuration
 *---------------------------------------------------------------------------*/
#if (MC0_ENABLE)
  static fsFAT_Volume fs_mc0_vol;

  #if (MC0_FAT_JOURNAL)
  static FSJOUR       fs_mc0_fsj;
  #endif

  #if (MC0_SPI == 0)
    #if (MC0_CACHE_RELOC == 1)
      #define __SECTION_MC0  __IN_SECTION_USR (MC0_CACHE_SECTION)
    #else
      #define __SECTION_MC0  __IN_SECTION_DEF (SECTION_MCI, MC0_MCI_DRIVER)
    #endif

  #ifndef FS_DEBUG
  static
  #endif
  MC_MCI fs_mc0_mci;
  #else
    #if (MC0_CACHE_RELOC == 1)
      #define __SECTION_MC0  __IN_SECTION_USR (MC0_CACHE_SECTION)
    #else
      #define __SECTION_MC0  __IN_SECTION_DEF (SECTION_SPI, MC0_SPI_DRIVER)
    #endif

  #ifndef FS_DEBUG
  static
  #endif
  MC_SPI fs_mc0_spi;
  #endif

  /* MC0 Cache Buffer for Data and FAT Caching */
  static uint32_t mc0_cache[(MC0_CACHE_SIZE+1) * 256 + MC0_FAT_JOURNAL * 128] __ALIGNED(32) __SECTION_MC0;

  #if (MC0_NAME_CACHE_SIZE > 0)
    #define MC0_NAME_CACHE_MAX_DEPTH 8
    #define MC0_NAME_CACHE_BUF_SIZE ((MC0_NAME_CACHE_SIZE      * FAT_NCACHE_LINK_SZ) + \
                                     (MC0_NAME_CACHE_MAX_DEPTH * FAT_NCACHE_USED_SZ) + \
                                                                 FAT_NCACHE_STAT_SZ)

    static uint32_t mc0_ncache_buf[MC0_NAME_CACHE_BUF_SIZE/4];

    static NCACHE mc0_ncache = {
      MC0_NAME_CACHE_MAX_DEPTH,
      MC0_NAME_CACHE_BUF_SIZE,
      mc0_ncache_buf
    };
  #endif

  #if (MC0_SPI == 0)
    /* SD Native bus mode */
    #define MC0_INIT(m)      mc_mci_Init        (m,       &fs_mc0_mci)
    #define MC0_UNINIT(m)    mc_mci_UnInit      (m,       &fs_mc0_mci)
    #define MC0_RDSEC(s,b,c) mc_mci_ReadSector  (s, b, c, &fs_mc0_mci)
    #define MC0_WRSEC(s,b,c) mc_mci_WriteSector (s, b, c, &fs_mc0_mci)
    #define MC0_RDINFO(i)    mc_mci_ReadInfo    (i,       &fs_mc0_mci)
    #define MC0_DEVCTRL(c,p) mc_mci_DevCtrl     (c, p,    &fs_mc0_mci)

    extern ARM_DRIVER_MCI CREATE_SYMBOL (Driver_MCI, MC0_MCI_DRIVER);
  #else
    /* SPI bus mode */
    #define MC0_INIT(m)      mc_spi_Init        (m,       &fs_mc0_spi)
    #define MC0_UNINIT(m)    mc_spi_UnInit      (m,       &fs_mc0_spi)
    #define MC0_RDSEC(s,b,c) mc_spi_ReadSector  (s, b, c, &fs_mc0_spi)
    #define MC0_WRSEC(s,b,c) mc_spi_WriteSector (s, b, c, &fs_mc0_spi)
    #define MC0_RDINFO(i)    mc_spi_ReadInfo    (i,       &fs_mc0_spi)
    #define MC0_DEVCTRL(c,p) mc_spi_DevCtrl     (c, p,    &fs_mc0_spi)

    extern ARM_DRIVER_SPI CREATE_SYMBOL (Driver_SPI, MC0_SPI_DRIVER);
  #endif

  /* MC0 wrapper functions */
  static uint32_t mc0_Init (uint32_t mode) {
    return (MC0_INIT (mode));
  }
  static uint32_t mc0_UnInit (uint32_t mode) {
    return (MC0_UNINIT (mode));
  }
  static uint32_t mc0_RdSect (uint32_t sect, uint8_t *buf, uint32_t cnt) {
    return (MC0_RDSEC (sect, buf, cnt));
  }
  static uint32_t mc0_WrSect (uint32_t sect, const uint8_t *buf, uint32_t cnt) {
    return (MC0_WRSEC (sect, buf, cnt));
  }
  static uint32_t mc0_RdInfo (fsMediaInfo *info) {
    return (MC0_RDINFO (info));
  }
  static fsStatus mc0_DevCtrl (fsDevCtrlCode code, void *p) {
    return (MC0_DEVCTRL (code, p));
  }

  /* MC0 FAT device driver */
  static FAT_DRV fs_mc0_drv = {
    mc0_Init,
    mc0_UnInit,
    mc0_RdSect,
    mc0_WrSect,
    mc0_RdInfo,
    mc0_DevCtrl
  };

  /* ------- MC0 MCI/SPI Callback ------- */
  #if (MC0_SPI == 0)
  static void MC0_MCI_SignalEvent (uint32_t event) {
    mc_mci_DrvEvent (event, &fs_mc0_mci);
  }
  #endif
  #if (MC0_SPI == 1)
  static void MC0_SPI_SignalEvent (uint32_t event) {
    mc_spi_DrvEvent (event, &fs_mc0_spi);
  }
  #endif
#endif

/*----------------------------------------------------------------------------
 *  Drive M1: Memory Card device MC1 configuration
 *---------------------------------------------------------------------------*/
#if (MC1_ENABLE)
  static fsFAT_Volume fs_mc1_vol;

  #if (MC1_FAT_JOURNAL)
  static FSJOUR       fs_mc1_fsj;
  #endif

  #if (MC1_SPI == 0)
    #if (MC1_CACHE_RELOC == 1)
      #define __SECTION_MC1  __IN_SECTION_USR (MC1_CACHE_SECTION)
    #else
      #define __SECTION_MC1  __IN_SECTION_DEF (SECTION_MCI, MC1_MCI_DRIVER)
    #endif

  #ifndef FS_DEBUG
  static
  #endif
  MC_MCI fs_mc1_mci;
  #else
    #if (MC1_CACHE_RELOC == 1)
      #define __SECTION_MC1  __IN_SECTION_USR (MC1_CACHE_SECTION)
    #else
      #define __SECTION_MC1  __IN_SECTION_DEF (SECTION_SPI, MC1_SPI_DRIVER)
    #endif

  #ifndef FS_DEBUG
  static
  #endif
  MC_SPI fs_mc1_spi;
  #endif

  /* MC1 Cache Buffer for Data and FAT Caching */
  static uint32_t mc1_cache[(MC1_CACHE_SIZE+1) * 256 + MC1_FAT_JOURNAL * 128] __ALIGNED(32) __SECTION_MC1;

  #if (MC1_NAME_CACHE_SIZE > 0)
    #define MC1_NAME_CACHE_MAX_DEPTH 8
    #define MC1_NAME_CACHE_BUF_SIZE ((MC1_NAME_CACHE_SIZE      * FAT_NCACHE_LINK_SZ) + \
                                     (MC1_NAME_CACHE_MAX_DEPTH * FAT_NCACHE_USED_SZ) + \
                                                                 FAT_NCACHE_STAT_SZ)

    static uint32_t mc1_ncache_buf[MC1_NAME_CACHE_BUF_SIZE/4];

    static NCACHE mc1_ncache = {
      MC1_NAME_CACHE_MAX_DEPTH,
      MC1_NAME_CACHE_BUF_SIZE,
      mc1_ncache_buf
    };
  #endif

  #if (MC1_SPI == 0)
    /* SD Native bus mode */
    #define MC1_INIT(m)      mc_mci_Init        (m,       &fs_mc1_mci)
    #define MC1_UNINIT(m)    mc_mci_UnInit      (m,       &fs_mc1_mci)
    #define MC1_RDSEC(s,b,c) mc_mci_ReadSector  (s, b, c, &fs_mc1_mci)
    #define MC1_WRSEC(s,b,c) mc_mci_WriteSector (s, b, c, &fs_mc1_mci)
    #define MC1_RDINFO(i)    mc_mci_ReadInfo    (i,       &fs_mc1_mci)
    #define MC1_DEVCTRL(c,p) mc_mci_DevCtrl     (c, p,    &fs_mc1_mci)

    extern ARM_DRIVER_MCI CREATE_SYMBOL (Driver_MCI, MC1_MCI_DRIVER);
  #else
    /* SPI bus mode */
    #define MC1_INIT(m)      mc_spi_Init        (m,       &fs_mc1_spi)
    #define MC1_UNINIT(m)    mc_spi_UnInit      (m,       &fs_mc1_spi)
    #define MC1_RDSEC(s,b,c) mc_spi_ReadSector  (s, b, c, &fs_mc1_spi)
    #define MC1_WRSEC(s,b,c) mc_spi_WriteSector (s, b, c, &fs_mc1_spi)
    #define MC1_RDINFO(i)    mc_spi_ReadInfo    (i,       &fs_mc1_spi)
    #define MC1_DEVCTRL(c,p) mc_spi_DevCtrl     (c, p,    &fs_mc1_spi)

    extern ARM_DRIVER_SPI CREATE_SYMBOL (Driver_SPI, MC1_SPI_DRIVER);
  #endif

  /* MC1 wrapper functions */
  static uint32_t mc1_Init (uint32_t mode) {
    return (MC1_INIT (mode));
  }
  static uint32_t mc1_UnInit (uint32_t mode) {
    return (MC1_UNINIT (mode));
  }
  static uint32_t mc1_RdSect (uint32_t sect, uint8_t *buf, uint32_t cnt) {
    return (MC1_RDSEC (sect, buf, cnt));
  }
  static uint32_t mc1_WrSect (uint32_t sect, const uint8_t *buf, uint32_t cnt) {
    return (MC1_WRSEC (sect, buf, cnt));
  }
  static uint32_t mc1_RdInfo (fsMediaInfo *info) {
    return (MC1_RDINFO (info));
  }
  static fsStatus mc1_DevCtrl (fsDevCtrlCode code, void *p) {
    return (MC1_DEVCTRL (code, p));
  }

  /* MC1 FAT device driver */
  static FAT_DRV fs_mc1_drv = {
    mc1_Init,
    mc1_UnInit,
    mc1_RdSect,
    mc1_WrSect,
    mc1_RdInfo,
    mc1_DevCtrl
  };

  /* ------- MC1 MCI/SPI Callback ------- */
  #if (MC1_SPI == 0)
  static void MC1_MCI_SignalEvent (uint32_t event) {
    mc_mci_DrvEvent (event, &fs_mc1_mci);
  }
  #endif
  #if (MC1_SPI == 1)
  static void MC1_SPI_SignalEvent (uint32_t event) {
    mc_spi_DrvEvent (event, &fs_mc1_spi);
  }
  #endif
#endif

/*----------------------------------------------------------------------------
 *  General NAND Flash device configuration
 *---------------------------------------------------------------------------*/
#if (NAND0_ENABLE)
  static NAND_MEDIA_HANDLE fs_nand0_media_handle;
  #ifndef FS_DEBUG
  static
  #endif
  NAND_FTL_DEV fs_nand0_handle;

  #if (NAND0_SW_ECC == 2)
    #define NAND0_ECC_ONCHIP 1
  #else
    #define NAND0_ECC_ONCHIP 0
  #endif
#endif

#if (NAND1_ENABLE)
  static NAND_MEDIA_HANDLE fs_nand1_media_handle;
  #ifndef FS_DEBUG
  static
  #endif
  NAND_FTL_DEV fs_nand1_handle;

  #if (NAND1_SW_ECC == 2)
    #define NAND1_ECC_ONCHIP 1
  #else
    #define NAND1_ECC_ONCHIP 0
  #endif
#endif

#ifdef FS_NAND_FLASH_0
  #if (NAND0_ENABLE)
  extern ARM_DRIVER_NAND   CREATE_SYMBOL (Driver_NAND, NAND0_DRIVER);
  
  static NAND_HW_DRIVER    fs_nand0_hw_driver;


  #if ((NAND1_ENABLE) && (NAND1_DRIVER == NAND0_DRIVER))
    #if (NAND1_DEV_NUM == NAND0_DEV_NUM)
      #error "::File System:Drive:NAND: NAND ports for different drives must be different"
    #endif

    #define NAND0_DEVICE1

    /* NAND0 Device1 Configuration Part */
    #define NAND1_MEDIA_EVENT_HANDLER NAND0_Media_SignalEventHandler
    #define NAND1_HW_EVENT_HANDLER    NAND0_HW_SignalEventHandler
    #define NAND1_HW_DRIVER           fs_nand0_hw_driver
    #define NAND1_DRIVER_CONFIG  0
  #endif

  static void NAND0_HW_SignalEventHandler (uint32_t dev_num, uint32_t event) {
    /* Event from "CMSIS Driver" to "NAND Media Layer" */
    switch (dev_num) {
      case NAND0_DEV_NUM: nand_DrvEvent (event, &fs_nand0_media_handle); break;

      #if defined(NAND0_DEVICE1)
      case NAND1_DEV_NUM: nand_DrvEvent (event, &fs_nand1_media_handle); break;
      #endif

      default: return;
    }
  }

  static void NAND0_Media_SignalEventHandler (uint32_t dev_num, uint32_t event) {
    (void)event;
    /* Event from "NAND Media Layer" to "NAND Flash Translation Layer" */
    switch (dev_num) {
      case NAND0_DEV_NUM: ftl_DrvEvent (event, &fs_nand0_handle); break;

      #if defined(NAND0_DEVICE1)
      case NAND1_DEV_NUM: ftl_DrvEvent (event, &fs_nand1_handle); break;
      #endif

      default: return;
    }
  }
  #endif
#endif

#ifdef FS_NAND_FLASH_1
  #if (((NAND0_ENABLE == 0) && NAND1_ENABLE) || ((NAND0_ENABLE && NAND1_ENABLE) && (NAND1_DRIVER != NAND0_DRIVER)))
  extern ARM_DRIVER_NAND  CREATE_SYMBOL (Driver_NAND, NAND1_DRIVER);

  static NAND_HW_DRIVER    fs_nand1_hw_driver;

  /* NAND1 Device0 Configuration Part */
  #define NAND1_MEDIA_EVENT_HANDLER NAND1_Media_SignalEventHandler
  #define NAND1_HW_EVENT_HANDLER    NAND1_HW_SignalEventHandler
  #define NAND1_HW_DRIVER           fs_nand1_hw_driver
  #define NAND1_DRIVER_CONFIG       1

  static void NAND1_HW_SignalEventHandler (uint32_t dev_num, uint32_t event) {
    /* Event from "CMSIS Driver" to "NAND Media Layer" */
    switch (dev_num) {
      case NAND1_DEV_NUM: nand_DrvEvent (event, &fs_nand1_media_handle); break;

      default: return;
    }
  }

  static void NAND1_Media_SignalEventHandler (uint32_t dev_num, uint32_t event) {
    (void)event;
    /* Event from "NAND Media Layer" to "NAND Flash Translation Layer" */
    switch (dev_num) {
      case NAND1_DEV_NUM: ftl_DrvEvent (event, &fs_nand1_handle); break;

      default: return;
    }
  }
  #endif
#endif


/*----------------------------------------------------------------------------
 *  Drive N0: NAND Flash device NAND0 configuration
 *---------------------------------------------------------------------------*/
#if (NAND0_ENABLE)
  extern ARM_DRIVER_NAND  CREATE_SYMBOL (Driver_NAND, NAND0_DRIVER);

  #if (NAND0_CACHE_RELOC == 1)
    #define __SECTION_NAND0  __IN_SECTION_USR (NAND0_CACHE_SECTION)
  #else
    #define __SECTION_NAND0  __IN_SECTION_DEF (SECTION_NAND, NAND0_DRIVER)
  #endif

  /* NAND Cache Buffer for FAT, Page and Block Caching */
  #define NAND0_CSZ   ((NAND0_CACHE_SIZE  + 1) * 1024             + \
                       (NAND0_PAGE_CACHE  + 2) * NAND0_PAGE_SIZE  + \
                       (NAND0_BLOCK_CACHE + 2) * NAND0_PAGE_COUNT)
  #define NAND0_FSJBUF (NAND0_FAT_JOURNAL      * 512)

  static uint32_t     nand0_cache[NAND0_CSZ/4 + NAND0_FSJBUF/4] __ALIGNED(32) __SECTION_NAND0;
  static PAGE_CACHE   nand0_capg [NAND0_PAGE_CACHE  + 1];
  static BLOCK_CACHE  nand0_cabl [NAND0_BLOCK_CACHE + 2];
  static uint32_t     nand0_ttsn [NAND_TSN_SIZE(NAND0_BLOCK_COUNT, NAND0_PAGE_SIZE)];

  static fsFAT_Volume fs_nand0_vol;
  #if (NAND0_FAT_JOURNAL)
  static FSJOUR       fs_nand0_fsj;
  #endif

  #if (NAND0_NAME_CACHE_SIZE > 0)
    #define NAND0_NAME_CACHE_MAX_DEPTH 8
    #define NAND0_NAME_CACHE_BUF_SIZE ((NAND0_NAME_CACHE_SIZE      * FAT_NCACHE_LINK_SZ) + \
                                       (NAND0_NAME_CACHE_MAX_DEPTH * FAT_NCACHE_USED_SZ) + \
                                                                     FAT_NCACHE_STAT_SZ)

    static uint32_t nand0_ncache_buf[NAND0_NAME_CACHE_BUF_SIZE/4];

    static NCACHE nand0_ncache = {
      NAND0_NAME_CACHE_MAX_DEPTH,
      NAND0_NAME_CACHE_BUF_SIZE,
      nand0_ncache_buf
    };
  #endif

  #if (NAND0_ECC_ONCHIP != 0)
    #define NAND0_ECC_TYPE         2  /* On-chip ECC uses ECC0 over Main+Spare */
    #define NAND0_ECC_PAGE_SIZE    15 /* Use virtual_page_size definitions     */
    #define NAND0_ECC_RESERVED     0  /* Reserved bits must be zero            */
    #define NAND0_ECC_CORRECT_BITS 0  /* Correctability level is not relevant  */
    #define NAND0_ECC_MAIN_CW_OFFS 0  /* Main area starts at page start        */
    #if (NAND0_ECC_VPAGE_LAYOUT == 0) /* Layout defines gap between each main  */
    #define NAND0_ECC_MAIN_CW_GAP  NAND0_ECC_VSPARE_SIZE
    #else
    #define NAND0_ECC_MAIN_CW_GAP  0
    #endif

  static const ARM_NAND_ECC_INFO nand0_ecc = {
    (NAND0_ECC_TYPE         & 0x0003),
    (NAND0_ECC_VPAGE_LAYOUT & 0x0001),
    (NAND0_ECC_VPAGE_COUNT  & 0x0007),
    (NAND0_ECC_PAGE_SIZE    & 0x000F),
    (NAND0_ECC_RESERVED     & 0x3FFF),
    (NAND0_ECC_CORRECT_BITS & 0x00FF),

    { NAND0_ECC_MAIN_CW_SIZE, NAND0_ECC_SPARE_CW_SIZE },
    { NAND0_ECC_DATA_SIZE,    0                       },
    { NAND0_ECC_DATA_OFFS,    0                       },
    { NAND0_ECC_VMAIN_SIZE,   NAND0_ECC_VSPARE_SIZE   },
    { NAND0_ECC_MAIN_CW_OFFS, NAND0_ECC_SPARE_CW_OFFS },
    { NAND0_ECC_MAIN_CW_GAP,  NAND0_ECC_SPARE_CW_GAP  },
    { NAND0_ECC_DATA_GAP,     0                       }
  };
  #endif

  static const NAND_DEVICE nand0_device = {
   &fs_nand0_handle.PgLay,
    NAND0_BUS_WIDTH,
    NAND0_DEV_NUM,
    NAND0_PAGE_SIZE,
    NAND0_BLOCK_COUNT,
    NAND0_PAGE_COUNT,
    NAND_SEC_BLOCK  (NAND0_PAGE_COUNT, NAND0_PAGE_SIZE),
    NAND_SEC_PAGE   (NAND0_PAGE_SIZE),
    NAND_ROW_CYCLES (NAND0_BLOCK_COUNT, NAND0_PAGE_COUNT),
    NAND_COL_CYCLES (NAND0_PAGE_SIZE),
    NAND0_SW_ECC,
    #if (NAND0_ECC_ONCHIP != 0)
     &nand0_ecc
    #else
      NULL
    #endif
  };

  /* NAND Flash Configuration */
  static NAND_FTL_CFG nand0_cfg = {
    /* Block Translation Table Space */
    1,
    NAND_BTT_SIZE (NAND0_BLOCK_COUNT),

    /* Data Space */
    NAND_BTT_SIZE (NAND0_BLOCK_COUNT) + 1,
    NAND0_BLOCK_COUNT - 1,

    /* Caching */
    NAND0_BLOCK_CACHE + 2,
    NAND0_PAGE_CACHE  + 1,
    NAND_TSN_SIZE(NAND0_BLOCK_COUNT, NAND0_PAGE_SIZE),
    { 0, 0 },

    /* Page buffer & Caches */
    (uint8_t *)&nand0_cache[(NAND0_CACHE_SIZE+1)*256],
    &nand0_cabl[0],
    (uint8_t *)&nand0_cache[(NAND0_CACHE_SIZE+1)*256+(NAND0_PAGE_CACHE+2)*NAND0_PAGE_SIZE/4],
    &nand0_capg[0],
    (uint8_t *)&nand0_cache[(NAND0_CACHE_SIZE+1)*256+NAND0_PAGE_SIZE/4],
    &nand0_ttsn[0]
  };

  /* NAND0 wrapper functions */
  static uint32_t nand0_Init (uint32_t mode) {
    return (ftl_Init (mode, &fs_nand0_handle));
  }
  static uint32_t nand0_UnInit (uint32_t mode) {
    return (ftl_UnInit (mode, &fs_nand0_handle));
  }
  static uint32_t nand0_RdSect (uint32_t sect, uint8_t *buf, uint32_t cnt) {
    uint32_t retv = ftl_ReadSect (sect, buf, cnt, &fs_nand0_handle);
    if (retv < 2) return (true);
    else          return (false);
  }
  static uint32_t nand0_WrSect (uint32_t sect, const uint8_t *buf, uint32_t cnt) {
    uint32_t retv = ftl_WriteSect (sect, buf, cnt, &fs_nand0_handle);
    if (retv < 2) return (true);
    else          return (false);
  }
  static uint32_t nand0_RdInfo (fsMediaInfo *info) {
    return (ftl_ReadInfo (info, &fs_nand0_handle));
  }
  static fsStatus nand0_DevCtrl (fsDevCtrlCode code, void *p) {
    return (ftl_DevCtrl (code, p, &fs_nand0_handle));
  }

  static FAT_DRV fs_nand0_drv = {
    nand0_Init,
    nand0_UnInit,
    nand0_RdSect,
    nand0_WrSect,
    nand0_RdInfo,
    nand0_DevCtrl
  };
#endif

/*----------------------------------------------------------------------------
 *  Drive N1: NAND Flash device NAND1 configuration
 *---------------------------------------------------------------------------*/
#if (NAND1_ENABLE)
  #if defined(NAND1_DEVICE0)
  extern ARM_DRIVER_NAND  CREATE_SYMBOL (Driver_NAND, NAND1_DRIVER);
  #endif

  #if (NAND1_CACHE_RELOC == 1)
    #define __SECTION_NAND1  __IN_SECTION_USR (NAND1_CACHE_SECTION)
  #else
    #define __SECTION_NAND1  __IN_SECTION_DEF (SECTION_NAND, NAND1_DRIVER)
  #endif

  /* NAND Cache Buffer for FAT, Page and Block Caching */
  #define NAND1_CSZ   ((NAND1_CACHE_SIZE  + 1) * 1024             + \
                       (NAND1_PAGE_CACHE  + 2) * NAND1_PAGE_SIZE  + \
                       (NAND1_BLOCK_CACHE + 2) * NAND1_PAGE_COUNT)
  #define NAND1_FSJBUF (NAND1_FAT_JOURNAL      * 512)
 
  static uint32_t     nand1_cache[NAND1_CSZ/4 + NAND1_FSJBUF/4] __ALIGNED(32) __SECTION_NAND1;
  static PAGE_CACHE   nand1_capg [NAND1_PAGE_CACHE  + 1];
  static BLOCK_CACHE  nand1_cabl [NAND1_BLOCK_CACHE + 2];
  static uint32_t     nand1_ttsn [NAND_TSN_SIZE(NAND1_BLOCK_COUNT, NAND1_PAGE_SIZE)];

  static fsFAT_Volume fs_nand1_vol;
  #if (NAND1_FAT_JOURNAL)
  static FSJOUR       fs_nand1_fsj;
  #endif

  #if (NAND1_NAME_CACHE_SIZE > 0)
    #define NAND1_NAME_CACHE_MAX_DEPTH 8
    #define NAND1_NAME_CACHE_BUF_SIZE ((NAND1_NAME_CACHE_SIZE      * FAT_NCACHE_LINK_SZ) + \
                                       (NAND1_NAME_CACHE_MAX_DEPTH * FAT_NCACHE_USED_SZ) + \
                                                                     FAT_NCACHE_STAT_SZ)

    static uint32_t nand1_ncache_buf[NAND1_NAME_CACHE_BUF_SIZE/4];

    static NCACHE nand1_ncache = {
      NAND1_NAME_CACHE_MAX_DEPTH,
      NAND1_NAME_CACHE_BUF_SIZE,
      nand1_ncache_buf
    };
  #endif

  #if (NAND1_ECC_ONCHIP != 0)
    #define NAND1_ECC_TYPE         2  /* On-chip ECC uses ECC0 over Main+Spare */
    #define NAND1_ECC_PAGE_SIZE    15 /* Use virtual_page_size definitions     */
    #define NAND1_ECC_RESERVED     0  /* Reserved bits must be zero            */
    #define NAND1_ECC_CORRECT_BITS 0  /* Correctability level is not relevant  */
    #define NAND1_ECC_MAIN_CW_OFFS 0  /* Main area starts at page start        */
    #if (NAND1_ECC_VPAGE_LAYOUT == 0) /* Layout defines gap between each main  */
    #define NAND1_ECC_MAIN_CW_GAP  NAND1_ECC_VSPARE_SIZE
    #else
    #define NAND1_ECC_MAIN_CW_GAP  0
    #endif

  static const ARM_NAND_ECC_INFO nand1_ecc = {
    (NAND1_ECC_TYPE         & 0x0003),
    (NAND1_ECC_VPAGE_LAYOUT & 0x0001),
    (NAND1_ECC_VPAGE_COUNT  & 0x0007),
    (NAND1_ECC_PAGE_SIZE    & 0x000F),
    (NAND1_ECC_RESERVED     & 0x3FFF),
    (NAND1_ECC_CORRECT_BITS & 0x00FF),

    { NAND1_ECC_MAIN_CW_SIZE, NAND1_ECC_SPARE_CW_SIZE },
    { NAND1_ECC_DATA_SIZE,    0                       },
    { NAND1_ECC_DATA_OFFS,    0                       },
    { NAND1_ECC_VMAIN_SIZE,   NAND1_ECC_VSPARE_SIZE   },
    { NAND1_ECC_MAIN_CW_OFFS, NAND1_ECC_SPARE_CW_OFFS },
    { NAND1_ECC_MAIN_CW_GAP,  NAND1_ECC_SPARE_CW_GAP  },
    { NAND1_ECC_DATA_GAP,     0                       }
  };
  #endif

  static const NAND_DEVICE nand1_device = {
   &fs_nand1_handle.PgLay,
    NAND1_BUS_WIDTH,
    NAND1_DEV_NUM,
    NAND1_PAGE_SIZE,
    NAND1_BLOCK_COUNT,
    NAND1_PAGE_COUNT,
    NAND_SEC_BLOCK  (NAND1_PAGE_COUNT, NAND1_PAGE_SIZE),
    NAND_SEC_PAGE   (NAND1_PAGE_SIZE),
    NAND_ROW_CYCLES (NAND1_BLOCK_COUNT, NAND1_PAGE_COUNT),
    NAND_COL_CYCLES (NAND1_PAGE_SIZE),
    NAND1_SW_ECC,
    #if (NAND1_ECC_ONCHIP != 0)
     &nand1_ecc
    #else
      NULL
    #endif
  };

  /* NAND Flash Configuration */
  static NAND_FTL_CFG nand1_cfg = {
    /* Block Translation Table Space */
    1,
    NAND_BTT_SIZE (NAND1_BLOCK_COUNT),

    /* Data Space */
    NAND_BTT_SIZE (NAND1_BLOCK_COUNT) + 1,
    NAND1_BLOCK_COUNT - 1,

    /* Caching */
    NAND1_BLOCK_CACHE + 2,
    NAND1_PAGE_CACHE  + 1,
    NAND_TSN_SIZE(NAND1_BLOCK_COUNT, NAND1_PAGE_SIZE),
    { 0, 0 },

    /* Page buffer & Caches */
    (uint8_t *)&nand1_cache[(NAND1_CACHE_SIZE+1)*256],
    &nand1_cabl[0],
    (uint8_t *)&nand1_cache[(NAND1_CACHE_SIZE+1)*256+(NAND1_PAGE_CACHE+2)*NAND1_PAGE_SIZE/4],
    &nand1_capg[0],
    (uint8_t *)&nand1_cache[(NAND1_CACHE_SIZE+1)*256+NAND1_PAGE_SIZE/4],
    &nand1_ttsn[0],
  };

  /* NAND1 wrapper functions */
  static uint32_t nand1_Init (uint32_t mode) {
    return (ftl_Init(mode, &fs_nand1_handle));
  }
  static uint32_t nand1_UnInit (uint32_t mode) {
    return (ftl_UnInit (mode, &fs_nand1_handle));
  }
  static uint32_t nand1_RdSect (uint32_t sect, uint8_t *buf, uint32_t cnt) {
    uint32_t retv = ftl_ReadSect (sect, buf, cnt, &fs_nand1_handle);
    if (retv < 2) return (true);
    else          return (false);
  }
  static uint32_t nand1_WrSect (uint32_t sect, const uint8_t *buf, uint32_t cnt) {
    uint32_t retv = ftl_WriteSect (sect, buf, cnt, &fs_nand1_handle);
    if (retv < 2) return (true);
    else          return (false);
  }
  static uint32_t nand1_RdInfo (fsMediaInfo *info) {
    return (ftl_ReadInfo (info, &fs_nand1_handle));
  }
  static fsStatus nand1_DevCtrl (fsDevCtrlCode code, void *p) {
    return (ftl_DevCtrl (code, p, &fs_nand1_handle));
  }

  static FAT_DRV fs_nand1_drv = {
    nand1_Init,
    nand1_UnInit,
    nand1_RdSect,
    nand1_WrSect,
    nand1_RdInfo,
    nand1_DevCtrl
  };
#endif

/*----------------------------------------------------------------------------
 *  Drive R0: RAM device RAM0 configuration
 *---------------------------------------------------------------------------*/
#if (RAM0_ENABLE)
  static fsFAT_Volume fs_ram0_vol;

  #if (RAM0_SIZE < 0x4A00)
    #error "::File System:Drive:RAM0 requires minimum of 0x4A00 bytes of memory"
  #endif

  #if (RAM0_RELOC == 1)
    #define __SECTION_RAM0  __IN_SECTION_USR (RAM0_SECTION)
  #else
    #define __SECTION_RAM0  __IN_SECTION_DEF (SECTION_RAM, 0)
  #endif

  /* RAM0 Device data buffer */
  static uint32_t ram0_buf[256 + (RAM0_SIZE/4)] __SECTION_RAM0;

  /* RAM0 device info */
  #ifndef FS_DEBUG
  static
  #endif
  RAM_DEV fs_ram0_dev = {
    (uint8_t *)&ram0_buf[256],
    RAM0_SIZE
  };

  /* RAM0 wrapper functions */
  static uint32_t ram0_Init (uint32_t mode) {
    return (ram_Init (mode, &fs_ram0_dev));
  }
  static uint32_t ram0_UnInit (uint32_t mode) {
    return (ram_UnInit (mode, &fs_ram0_dev));
  }
  static uint32_t ram0_RdSect (uint32_t sect, uint8_t *buf, uint32_t cnt) {
    return (ram_ReadSector (sect, buf, cnt, &fs_ram0_dev));
  }
  static uint32_t ram0_WrSect (uint32_t sect, const uint8_t *buf, uint32_t cnt) {
    return (ram_WriteSector (sect, buf, cnt, &fs_ram0_dev));
  }
  static uint32_t ram0_RdInfo (fsMediaInfo *info) {
    return (ram_ReadInfo (info, &fs_ram0_dev));
  }
  static fsStatus ram0_DevCtrl (fsDevCtrlCode code, void *p) {
    return (ram_DevCtrl (code, p, &fs_ram0_dev));
  }

  /* FAT RAM Driver Control Block */
  static FAT_DRV fs_ram0_drv = {
    ram0_Init,
    ram0_UnInit,
    ram0_RdSect,
    ram0_WrSect,
    ram0_RdInfo,
    ram0_DevCtrl
  };
#endif

/*----------------------------------------------------------------------------
 *  Drive R1: RAM device RAM1 configuration
 *---------------------------------------------------------------------------*/
#if (RAM1_ENABLE)
  static fsFAT_Volume fs_ram1_vol;

  #if (RAM1_SIZE < 0x4A00)
    #error "::File System:Drive:RAM1 requires minimum of 0x4A00 bytes of memory"
  #endif

  #if (RAM1_RELOC == 1)
    #define __SECTION_RAM1  __IN_SECTION_USR (RAM1_SECTION)
  #else
    #define __SECTION_RAM1  __IN_SECTION_DEF (SECTION_RAM, 1)
  #endif

  /* RAM1 Device data buffer */
  static uint32_t ram1_buf[256 + (RAM1_SIZE/4)] __SECTION_RAM1;

  /* RAM1 device info */
  #ifndef FS_DEBUG
  static
  #endif
  RAM_DEV fs_ram1_dev = {
    (uint8_t *)&ram1_buf[256],
    RAM1_SIZE
  };

  /* RAM0 wrapper functions */
  static uint32_t ram1_Init (uint32_t mode) {
    return (ram_Init (mode, &fs_ram1_dev));
  }
  static uint32_t ram1_UnInit (uint32_t mode) {
    return (ram_UnInit (mode, &fs_ram1_dev));
  }
  static uint32_t ram1_RdSect (uint32_t sect, uint8_t *buf, uint32_t cnt) {
    return (ram_ReadSector (sect, buf, cnt, &fs_ram1_dev));
  }
  static uint32_t ram1_WrSect (uint32_t sect, const uint8_t *buf, uint32_t cnt) {
    return (ram_WriteSector (sect, buf, cnt, &fs_ram1_dev));
  }
  static uint32_t ram1_RdInfo (fsMediaInfo *info) {
    return (ram_ReadInfo (info, &fs_ram1_dev));
  }
  static fsStatus ram1_DevCtrl (fsDevCtrlCode code, void *p) {
    return (ram_DevCtrl (code, p, &fs_ram1_dev));
  }

  /* FAT RAM Driver Control Block */
  static FAT_DRV fs_ram1_drv = {
    ram1_Init,
    ram1_UnInit,
    ram1_RdSect,
    ram1_WrSect,
    ram1_RdInfo,
    ram1_DevCtrl
  };
#endif

/*----------------------------------------------------------------------------
 *  Drive U0: USB Flash device USB0 configuration
 *---------------------------------------------------------------------------*/
#if (USB0_ENABLE)
  static fsFAT_Volume fs_usb0_vol;

  #if (USB0_FAT_JOURNAL)
  static FSJOUR       fs_usb0_fsj;
  #endif

  #if (USB0_NAME_CACHE_SIZE > 0)
    #define USB0_NAME_CACHE_MAX_DEPTH 8
    #define USB0_NAME_CACHE_BUF_SIZE ((USB0_NAME_CACHE_SIZE      * FAT_NCACHE_LINK_SZ) + \
                                      (USB0_NAME_CACHE_MAX_DEPTH * FAT_NCACHE_USED_SZ) + \
                                                                   FAT_NCACHE_STAT_SZ)

    static uint32_t usb0_ncache_buf[USB0_NAME_CACHE_BUF_SIZE/4];

    static NCACHE usb0_ncache = {
      USB0_NAME_CACHE_MAX_DEPTH,
      USB0_NAME_CACHE_BUF_SIZE,
      usb0_ncache_buf
    };
  #endif

  /* USB Cache Buffer for Data and FAT Caching */
  static uint32_t usb0_cache[(USB0_CACHE_SIZE+1) * 256 + USB0_FAT_JOURNAL * 128];

  /* USB0 wrapper functions */
  static uint32_t usb0_Init (uint32_t mode) {
    return (usbh_msc_Init (mode, 0));
  }
  static uint32_t usb0_UnInit (uint32_t mode) {
    return (usbh_msc_UnInit (mode, 0));
  }
  static uint32_t usb0_RdSect (uint32_t sect, uint8_t *buf, uint32_t cnt) {
    return (usbh_msc_ReadSector (sect, cnt, buf, 0));
  }
  static uint32_t usb0_WrSect (uint32_t sect, const uint8_t *buf, uint32_t cnt) {
    return (usbh_msc_WriteSector (sect, cnt, (const uint8_t *)buf, 0));
  }
  static uint32_t usb0_RdInfo (fsMediaInfo *info) {
    return (usbh_msc_ReadInfo (info, 0));
  }
  static fsStatus usb0_DevCtrl (fsDevCtrlCode code, void *p) {
    return (usbh_msc_DevCtrl (code, p, 0));
  }

  static FAT_DRV fs_usb0_drv = {
    usb0_Init,
    usb0_UnInit,
    usb0_RdSect,
    usb0_WrSect,
    usb0_RdInfo,
    usb0_DevCtrl
  };
#endif

/*----------------------------------------------------------------------------
 *  Drive U1: USB Flash device USB1 configuration
 *---------------------------------------------------------------------------*/
#if (USB1_ENABLE)
  static fsFAT_Volume fs_usb1_vol;

  #if (USB1_FAT_JOURNAL)
  static FSJOUR       fs_usb1_fsj;
  #endif

  /* USB Cache Buffer for Data and FAT Caching */
  static uint32_t usb1_cache[(USB1_CACHE_SIZE+1) * 256 + USB1_FAT_JOURNAL * 128];

  #if (USB1_NAME_CACHE_SIZE > 0)
    #define USB1_NAME_CACHE_MAX_DEPTH 8
    #define USB1_NAME_CACHE_BUF_SIZE ((USB1_NAME_CACHE_SIZE      * FAT_NCACHE_LINK_SZ) + \
                                      (USB1_NAME_CACHE_MAX_DEPTH * FAT_NCACHE_USED_SZ) + \
                                                                   FAT_NCACHE_STAT_SZ)

    static uint32_t usb1_ncache_buf[USB1_NAME_CACHE_BUF_SIZE/4];

    static NCACHE usb1_ncache = {
      USB1_NAME_CACHE_MAX_DEPTH,
      USB1_NAME_CACHE_BUF_SIZE,
      usb1_ncache_buf
    };
  #endif

  /* USB1 wrapper functions */
  static uint32_t usb1_Init (uint32_t mode) {
    return (usbh_msc_Init (mode, 1));
  }
  static uint32_t usb1_UnInit (uint32_t mode) {
    return (usbh_msc_UnInit (mode, 1));
  }
  static uint32_t usb1_RdSect (uint32_t sect, uint8_t *buf, uint32_t cnt) {
    return (usbh_msc_ReadSector (sect, cnt, buf, 1));
  }
  static uint32_t usb1_WrSect (uint32_t sect, const uint8_t *buf, uint32_t cnt) {
    return (usbh_msc_WriteSector (sect, cnt, (const uint8_t *)buf, 1));
  }
  static uint32_t usb1_RdInfo (fsMediaInfo *info) {
    return (usbh_msc_ReadInfo (info, 1));
  }
  static fsStatus usb1_DevCtrl (fsDevCtrlCode code, void *p) {
    return (usbh_msc_DevCtrl (code, p, 1));
  }

  static FAT_DRV fs_usb1_drv = {
    usb1_Init,
    usb1_UnInit,
    usb1_RdSect,
    usb1_WrSect,
    usb1_RdInfo,
    usb1_DevCtrl
  };
#endif


/*-----------------------------------------------------------------------------
 *  An array of installed Device drivers
 *----------------------------------------------------------------------------*/
FS_DEV fs_DevPool[FS_NDRV] = {
#if (NOR0_ENABLE)
  { &fs_nor0_vol, "F0", NOR0_CURRENT_DRIVE  | FS_EMB },
#endif
#if (NOR1_ENABLE)
  { &fs_nor1_vol, "F1", NOR1_CURRENT_DRIVE  | FS_EMB },
#endif
#if (MC0_ENABLE)
  { &fs_mc0_vol,  "M0", MC0_CURRENT_DRIVE   | FS_FAT | FS_IOC },
#endif
#if (MC1_ENABLE)
  { &fs_mc1_vol,  "M1", MC1_CURRENT_DRIVE   | FS_FAT | FS_IOC },
#endif
#if (NAND0_ENABLE)
  { &fs_nand0_vol,"N0", NAND0_CURRENT_DRIVE | FS_FAT | FS_IOC },
#endif
#if (NAND1_ENABLE)
  { &fs_nand1_vol,"N1", NAND1_CURRENT_DRIVE | FS_FAT | FS_IOC },
#endif
#if (RAM0_ENABLE)
  { &fs_ram0_vol, "R0", RAM0_CURRENT_DRIVE  | FS_FAT | FS_IOC },
#endif
#if (RAM1_ENABLE)
  { &fs_ram1_vol, "R1", RAM1_CURRENT_DRIVE  | FS_FAT | FS_IOC },
#endif
#if (USB0_ENABLE)
  { &fs_usb0_vol, "U0", USB0_CURRENT_DRIVE  | FS_FAT },
#endif
#if (USB1_ENABLE)
  { &fs_usb1_vol, "U1", USB1_CURRENT_DRIVE  | FS_FAT },
#endif
};


/*-----------------------------------------------------------------------------
 *  Configuration per drive
 *----------------------------------------------------------------------------*/
#if (NOR0_ENABLE || NOR1_ENABLE)
__STATIC_INLINE void fs_config_flash (char num) {
  switch (num) {
#if (NOR0_ENABLE)
    case '0':
      fs_nor0_vol.Mutex       = fs_mutex_new ("F0");
      fs_nor0_vol.Drv         = &fs_nor0_drv;

      fs_nor0.Driver          = &CREATE_SYMBOL (Driver_Flash, NOR0_DRIVER);
      fs_nor0.Callback        = &NOR0_Flash_SignalEvent;
      fs_nor0.Device.Timeout  = &fs_nor0_flash_tout;
      break;
#endif /* NOR0_ENABLE */
    
#if (NOR1_ENABLE)
    case '1':
      fs_nor1_vol.Mutex       = fs_mutex_new ("F1");
      fs_nor1_vol.Drv         = &fs_nor1_drv;

      fs_nor1.Driver          = &CREATE_SYMBOL (Driver_Flash, NOR1_DRIVER);
      fs_nor1.Callback        = &NOR1_Flash_SignalEvent;
      fs_nor1.Device.Timeout  = &fs_nor1_flash_tout;
      break;
#endif /* NOR1_ENABLE */
  }
}
#endif

#if (MC0_ENABLE || MC1_ENABLE)
__STATIC_INLINE void fs_config_mc (char num) {
  switch (num) {
#if (MC0_ENABLE)
    case '0':
      fs_mc0_vol.Mutex         = fs_mutex_new ("M0");
      fs_mc0_vol.Drv           = &fs_mc0_drv;
      fs_mc0_vol.CaBuf         = mc0_cache;
      fs_mc0_vol.CaSize        = MC0_CACHE_SIZE * 2;
     #if (MC0_NAME_CACHE_SIZE)
      fs_mc0_vol.ncache        = &mc0_ncache;
     #else
      fs_mc0_vol.ncache        = NULL;
     #endif

     #if   (MC0_SPI == 0)
      /* Register MCI driver for Native mode */
      fs_mc0_mci.Instance      = 0;
      fs_mc0_mci.ExtCSD        = (uint8_t *)mc0_cache;
      fs_mc0_mci.Driver        = &CREATE_SYMBOL (Driver_MCI, MC0_MCI_DRIVER);
      fs_mc0_mci.Callback      = MC0_MCI_SignalEvent;
     #elif (MC0_SPI == 1)
      /* Register SPI driver for SPI mode */
      fs_mc0_spi.Instance      = 0;
      fs_mc0_spi.ExtCSD        = (uint8_t *)mc0_cache;
      fs_mc0_spi.Driver        = &CREATE_SYMBOL (Driver_SPI, MC0_SPI_DRIVER);
      fs_mc0_spi.Callback      = MC0_SPI_SignalEvent;
     #else
      #error "Memory Card Interface Mode Invalid in FS_Config_MC_0.h"
     #endif

     #if (MC0_FAT_JOURNAL)
      /* Register file system journal */
      fs_mc0_fsj.buf           = (uint8_t *)&mc0_cache[(MC0_CACHE_SIZE+1) * 256];
      fs_mc0_vol.fsj           = &fs_mc0_fsj;
      fs_mc0_vol.RsvdS         = FAT_SECT_RSVD;
     #else
      fs_mc0_vol.fsj           = NULL;
      fs_mc0_vol.RsvdS         = 0;
     #endif
      break;
#endif /* MC0_ENABLE */

#if (MC1_ENABLE)
    case '1':
      fs_mc1_vol.Mutex         = fs_mutex_new ("M1");
      fs_mc1_vol.Drv           = &fs_mc1_drv;
      fs_mc1_vol.CaBuf         = mc1_cache;
      fs_mc1_vol.CaSize        = MC1_CACHE_SIZE * 2;
     #if (MC1_NAME_CACHE_SIZE)
      fs_mc1_vol.ncache        = &mc1_ncache;
     #else
      fs_mc1_vol.ncache        = NULL;
     #endif

     #if   (MC1_SPI == 0)
      /* Register MCI driver for Native mode */
      fs_mc1_mci.Instance      = 1;
      fs_mc1_mci.ExtCSD        = (uint8_t *)mc1_cache;
      fs_mc1_mci.Driver        = &CREATE_SYMBOL (Driver_MCI, MC1_MCI_DRIVER);
      fs_mc1_mci.Callback      = MC1_MCI_SignalEvent;
     #elif (MC1_SPI == 1)
      /* Register SPI driver for SPI mode */
      fs_mc1_spi.Instance      = 1;
      fs_mc1_spi.ExtCSD        = (uint8_t *)mc1_cache;
      fs_mc1_spi.Driver        = &CREATE_SYMBOL (Driver_SPI, MC1_SPI_DRIVER);
      fs_mc1_spi.Callback      = MC1_SPI_SignalEvent;
     #else
      #error "Memory Card Interface Mode Invalid in FS_Config_MC_1.h"
     #endif

     #if (MC1_FAT_JOURNAL)
      /* Register file system journal */
      fs_mc1_fsj.buf           = (uint8_t *)&mc1_cache[(MC1_CACHE_SIZE+1) * 256];
      fs_mc1_vol.fsj           = &fs_mc1_fsj;
      fs_mc1_vol.RsvdS         = FAT_SECT_RSVD;
     #else
      fs_mc1_vol.fsj           = NULL;
      fs_mc1_vol.RsvdS         = 0;
     #endif
      break;
#endif /* MC1_ENABLE */
  }
}
#endif

#if (NAND0_ENABLE || NAND1_ENABLE)
__STATIC_INLINE void fs_config_nand (char num) {
  switch (num) {
#if (NAND0_ENABLE)
    case '0':
      fs_nand0_vol.Mutex             = fs_mutex_new ("N0");
      fs_nand0_vol.Drv               = &fs_nand0_drv;
      fs_nand0_vol.CaBuf             = nand0_cache;
      fs_nand0_vol.CaSize            = NAND0_CACHE_SIZE * 2;
     #if (NAND0_NAME_CACHE_SIZE)
      fs_nand0_vol.ncache            = &nand0_ncache;
     #else
      fs_nand0_vol.ncache            = NULL;
     #endif

      fs_nand0_handle.Media          = &fs_nand0_media_handle;
      fs_nand0_handle.Cfg            = &nand0_cfg;

      fs_nand0_hw_driver.hw_cb       = &NAND0_HW_SignalEventHandler;
      fs_nand0_hw_driver.drv         = &CREATE_SYMBOL (Driver_NAND, NAND0_DRIVER);
      fs_nand0_hw_driver.init        = 0;

      fs_nand0_media_handle.media_cb = &NAND0_Media_SignalEventHandler;
      fs_nand0_media_handle.hw       = &fs_nand0_hw_driver;
      fs_nand0_media_handle.dev      = &nand0_device;
      fs_nand0_media_handle.status   = 0;
      fs_nand0_media_handle.instance = 0;

     #if (NAND0_FAT_JOURNAL)
      /* Register file system journal */
      fs_nand0_fsj.buf               = (uint8_t *)&nand0_cache[NAND0_CSZ/4];
      fs_nand0_vol.fsj               = &fs_nand0_fsj;
      fs_nand0_vol.RsvdS             = FAT_SECT_RSVD;
     #else
      fs_nand0_vol.fsj               = NULL;
      fs_nand0_vol.RsvdS             = 0;
     #endif
      break;
#endif /* NAND0_ENABLE */

#if (NAND1_ENABLE)
    case '1':
      fs_nand1_vol.Mutex             = fs_mutex_new ("N1");
      fs_nand1_vol.Drv               = &fs_nand1_drv;
      fs_nand1_vol.CaBuf             = nand1_cache;
      fs_nand1_vol.CaSize            = NAND1_CACHE_SIZE * 2;
     #if (NAND1_NAME_CACHE_SIZE)
      fs_nand1_vol.ncache            = &nand1_ncache;
     #else
      fs_nand1_vol.ncache            = NULL;
     #endif

      fs_nand1_handle.Media          = &fs_nand1_media_handle;
      fs_nand1_handle.Cfg            = &nand1_cfg;

      #if NAND1_DRIVER_CONFIG
      fs_nand1_hw_driver.hw_cb       = &NAND1_HW_EVENT_HANDLER;
      fs_nand1_hw_driver.drv         = &CREATE_SYMBOL (Driver_NAND, NAND1_DRIVER);
      fs_nand1_hw_driver.init        = 0;
      #endif

      fs_nand1_media_handle.media_cb = &NAND1_MEDIA_EVENT_HANDLER;
      fs_nand1_media_handle.hw       = &NAND1_HW_DRIVER;
      fs_nand1_media_handle.dev      = &nand1_device;
      fs_nand1_media_handle.status   = 0;
      fs_nand1_media_handle.instance = 1;

     #if (NAND1_FAT_JOURNAL)
      /* Register file system journal */
      fs_nand1_fsj.buf               = (uint8_t *)&nand1_cache[NAND1_CSZ/4];
      fs_nand1_vol.fsj               = &fs_nand1_fsj;
      fs_nand1_vol.RsvdS             = FAT_SECT_RSVD;
     #else
      fs_nand1_vol.fsj               = NULL;
      fs_nand1_vol.RsvdS             = 0;
     #endif
      break;
#endif /* NAND1_ENABLE */
  }
}
#endif

#if (RAM0_ENABLE || RAM1_ENABLE)
__STATIC_INLINE void fs_config_ram (char num) {
  switch (num) {
#if (RAM0_ENABLE)
    case '0':
      fs_ram0_vol.Mutex    = fs_mutex_new ("R0");
      fs_ram0_vol.Drv      = &fs_ram0_drv;
      fs_ram0_vol.CaBuf    = ram0_buf;
      fs_ram0_vol.CaSize   = 0;
      fs_ram0_vol.RsvdS    = 0;
      fs_ram0_vol.fsj      = NULL;
      break;
#endif /* RAM0_ENABLE */

#if (RAM1_ENABLE)
    case '1':
      fs_ram1_vol.Mutex    = fs_mutex_new ("R1");
      fs_ram1_vol.Drv      = &fs_ram1_drv;
      fs_ram1_vol.CaBuf    = ram1_buf;
      fs_ram1_vol.CaSize   = 0;
      fs_ram1_vol.RsvdS    = 0;
      fs_ram1_vol.fsj      = NULL;
      break;
#endif /* RAM1_ENABLE */
  }
}
#endif

#if (USB0_ENABLE || USB1_ENABLE)
__STATIC_INLINE void fs_config_usb (char num) {
  switch (num) {
#if (USB0_ENABLE)
    case '0':
      fs_usb0_vol.Mutex    = fs_mutex_new ("U0");
      fs_usb0_vol.Drv      = &fs_usb0_drv;
      fs_usb0_vol.CaBuf    = usb0_cache;
      fs_usb0_vol.CaSize   = USB0_CACHE_SIZE * 2;
     #if (USB0_NAME_CACHE_SIZE)
      fs_usb0_vol.ncache   = &usb0_ncache;
     #else
      fs_usb0_vol.ncache   = NULL;
     #endif

     #if (USB0_FAT_JOURNAL)
      /* Register file system journal */
      fs_usb0_fsj.buf      = (uint8_t *)&usb0_cache[(USB0_CACHE_SIZE+1) * 256];
      fs_usb0_vol.fsj      = &fs_usb0_fsj;
      fs_usb0_vol.RsvdS    = FAT_SECT_RSVD;
     #else
      fs_usb0_vol.fsj      = NULL;
      fs_usb0_vol.RsvdS    = 0;
     #endif
      break;
#endif /* USB0_ENABLE */

#if (USB1_ENABLE)
    case '1':
      fs_usb1_vol.Mutex    = fs_mutex_new ("U1");
      fs_usb1_vol.Drv      = &fs_usb1_drv;
      fs_usb1_vol.CaSize   = USB1_CACHE_SIZE * 2;
      fs_usb1_vol.CaBuf    = usb1_cache;
     #if (USB1_NAME_CACHE_SIZE)
      fs_usb1_vol.ncache   = &usb1_ncache;
     #else
      fs_usb1_vol.ncache   = NULL;
     #endif

     #if (USB1_FAT_JOURNAL)
      /* Register file system journal */
      fs_usb1_fsj.buf      = (uint8_t *)&usb1_cache[(USB1_CACHE_SIZE+1) * 256];
      fs_usb1_vol.fsj      = &fs_usb1_fsj;
      fs_usb1_vol.RsvdS    = FAT_SECT_RSVD;
     #else
      fs_usb1_vol.fsj      = NULL;
      fs_usb1_vol.RsvdS    = 0;
     #endif
      break;
#endif /* USB1_ENABLE */
  }
}
#endif

/*-----------------------------------------------------------------------------
 *  Configure FileSystem drives
 *----------------------------------------------------------------------------*/
void fs_config (const char *drive) {
  switch (drive[0]) {
#if (NOR0_ENABLE || NOR1_ENABLE)
    case 'F':
      fs_config_flash (drive[1]);
      break;
#endif

#if (MC0_ENABLE || MC1_ENABLE)
    case 'M':
      fs_config_mc (drive[1]);
      break;
#endif

#if (NAND0_ENABLE || NAND1_ENABLE)
    case 'N':
      fs_config_nand (drive[1]);
      break;
#endif

#if (RAM0_ENABLE || RAM1_ENABLE)
    case 'R':
      fs_config_ram (drive[1]);
      break;
#endif

#if (USB0_ENABLE || USB1_ENABLE)
    case 'U':
      fs_config_usb (drive[1]);
      break;
#endif
    default:
      break;
  }
}

#if defined(__ARMCC_VERSION)
#if (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#endif
/*-----------------------------------------------------------------------------
 *  fflush hook - Arm Standard C Library
 *
 *  Arm Standard Run-Time library only provides call to the _sys_write() when
 *  fflush() is called, which is the same mechanism as for fwrite(). This means
 *  that it is impossible to differentiate between a call to the fflush() and
 *  a call to the fwrite().
 *  Therefore, this hook is used to catch the calls to fflush and trigger flush
 *  operation in the FileSystem library after fflush returns.
 *----------------------------------------------------------------------------*/
extern _ARMABI int $Sub$$fflush  (FILE * /*stream*/);
extern         int $Super$$fflush(FILE * /*stream*/);

_ARMABI int $Sub$$fflush(FILE *stream) {
  int *fh;

  if ($Super$$fflush (stream) == 0) {
    fh = (int *)stream;
    /* File handle is at offset 5 */
    return (__sys_ensure (fh[5]));
  }
  return (1);
}
#elif defined ( __GNUC__ )
#ifdef MW_FILESYSTEM_WRAP_FFLUSH
/*-----------------------------------------------------------------------------
 *  fflush hook - GCC Newlib
 *
 *  Newlib only provides call to the _write() when fflush() is called, which
 *  is the same mechanism as for fwrite(). This means that it is impossible
 *  to differentiate between a call to the fflush() and a call to the fwrite().
 *  Therefore, this hook is used to catch the calls to fflush and trigger flush
 *  operation in the FileSystem library after fflush returns.
 *  Note:
 *    Add '-Wl,--wrap=fflush' linker command line option to enable fflush hook.
 *----------------------------------------------------------------------------*/
extern int __real_fflush (FILE * /*stream*/);

int __wrap_fflush (FILE *stream) {
  int *fh;

  if (__real_fflush (stream) == 0) {
    fh = (int *)stream;
    /* File handle is at offset 5 */
    return (fs_fflush (fh[5]));
  }
  return (1);
}
#endif /* MW_FILESYSTEM_WRAP_FFLUSH */
#endif

/*-----------------------------------------------------------------------------
 *  Empty functions to reduce image size when some components are not used
 *----------------------------------------------------------------------------*/
#if (EFS_USE == 0)
 int32_t  efs_handle_get(fsEFS_Volume *v)                          { (void)v;                   return (-1);      }
 fsStatus efs_init      (fsEFS_Volume *v)                          { (void)v;                   return (fsError); }
 fsStatus efs_uninit    (fsEFS_Volume *v)                          { (void)v;                   return (fsError); }
 fsStatus efs_mount     (fsEFS_Volume *v)                          { (void)v;                   return (fsError); }
 fsStatus efs_unmount   (fsEFS_Volume *v)                          { (void)v;                   return (fsError); }
 fsStatus efs_open  (int32_t h, const char *p, int32_t a)          { (void)h; (void)p; (void)a; return (fsError); }
 fsStatus efs_close (int32_t h)                                    { (void)h;                   return (fsError); }
 int32_t  efs_read  (int32_t h,       uint8_t *p, uint32_t a)      { (void)h; (void)p; (void)a; return (0);       }
 int32_t  efs_write (int32_t h, const uint8_t *p, uint32_t a)      { (void)h; (void)p; (void)a; return (-1);      }
 fsStatus efs_flush (int32_t h)                                    { (void)h;                   return (fsError); }
 int32_t  efs_flen  (int32_t h)                                    { (void)h;                   return (-1);      }
 int32_t  efs_seek  (int32_t h, int32_t o, int32_t w)              { (void)h; (void)o; (void)w; return (fsError); }
 fsStatus efs_ffind (fsFileInfo *i, fsEFS_Volume *v)               { (void)i; (void)v;          return (fsError); }
 fsStatus efs_rename(const char *p, const char *n, fsEFS_Volume *v){ (void)p; (void)n; (void)v; return (fsError); }
 fsStatus efs_delete(const char *p, fsEFS_Volume *v)               { (void)p; (void)v;          return (fsError); }
 int32_t  efs_analyse(fsEFS_Volume *v)                             { (void)v;                   return (0);       }
 fsStatus efs_check  (fsEFS_Volume *v)                             { (void)v;                   return (fsError); }
 fsStatus efs_format (fsEFS_Volume *v)                             { (void)v;                   return (fsError); }
 int64_t  efs_free   (fsEFS_Volume *v)                             { (void)v;                   return (-1);      }
 fsStatus efs_defrag (fsEFS_Volume *v)                             { (void)v;                   return (fsError); }
 fsStatus efs_info   (fsDriveInfo *i, fsEFS_Volume *v)             { (void)i; (void)v;          return (fsError); }
#endif /* EFS_USE */

#if (FAT_USE == 0)
 int32_t  fat_handle_get(fsFAT_Volume *v)                            { (void)v;                   return (-1);      }
 fsStatus fat_init      (fsFAT_Volume *v)                            { (void)v;                   return (fsError); }
 fsStatus fat_uninit    (fsFAT_Volume *v)                            { (void)v;                   return (fsError); }
 fsStatus fat_mount     (fsFAT_Volume *v)                            { (void)v;                   return (fsError); }
 fsStatus fat_unmount   (fsFAT_Volume *v)                            { (void)v;                   return (fsError); }
 fsStatus fat_open (int32_t h, const char *p, int32_t a)             { (void)h; (void)p; (void)a; return (fsError); }
 fsStatus fat_close(int32_t h)                                       { (void)h;                   return (fsError); }
 int32_t  fat_read (int32_t h, uint8_t *p, uint32_t a)               { (void)h; (void)p; (void)a; return (-1);      }
 int32_t  fat_write(int32_t h, const uint8_t *p, uint32_t a)         { (void)h; (void)p; (void)a; return (-1);      }
 fsStatus fat_flush(int32_t h)                                       { (void)h;                   return (fsError); }
 int32_t  fat_flen (int32_t h)                                       { (void)h;                   return (-1);      }
 int64_t  fat_seek (int32_t h, int64_t o, int32_t w)                 { (void)h; (void)o; (void)w; return (fsError); }
 fsStatus fat_ffind  (const char *p, fsFileInfo *i, fsFAT_Volume *v) { (void)p; (void)i; (void)v; return (fsError); }
 fsStatus fat_delete  (const char *p, const char *o, fsFAT_Volume *v){ (void)p; (void)o; (void)v; return (fsError); }
 fsStatus fat_rename (const char *p, const char *n, fsFAT_Volume *v) { (void)p; (void)n; (void)v; return (fsError); }
 fsStatus fat_format (fsFAT_Volume *v, const char *o)                { (void)v; (void)o;          return (fsError); }
 int64_t  fat_free   (fsFAT_Volume *v)                               { (void)v;                   return (-1);      }
 fsStatus fat_attrib (fsFAT_Volume *v, const char *p, uint32_t a)    { (void)v; (void)p; (void)a; return (fsError); }
 fsStatus fat_vol (char *l, uint32_t *s, fsFAT_Volume *v)            { (void)l; (void)s; (void)v; return (fsError); }
 fsStatus fat_pwd (char *p, uint32_t l, fsFAT_Volume *v)             { (void)p; (void)l; (void)v; return (fsError); }
 fsStatus fat_media (fsFAT_Volume *v)                                { (void)v;                   return (fsError); }
 fsStatus fat_info (fsDriveInfo *i, fsFAT_Volume *v)                 { (void)i; (void)v;          return (fsError); }
 fsStatus fat_chdir (const char *p, fsFAT_Volume *v)                 { (void)p; (void)v;          return (fsError); }
 fsStatus fat_mkdir (const char *p, fsFAT_Volume *v)                 { (void)p; (void)v;          return (fsError); }
 fsStatus fat_rmdir (const char *p, const char *o, fsFAT_Volume *v)  { (void)p; (void)o; (void)v; return (fsError); }
 fsStatus fat_time_set (const char *p, fsTime *c, fsTime *a, fsTime *w, fsFAT_Volume *v) \
                                                   { (void)p; (void)c; (void)a; (void)w; (void)v; return (fsError); }
 fsStatus fat_time_get (const char *p, fsTime *c, fsTime *a, fsTime *w, fsFAT_Volume *v) \
                                                   { (void)p; (void)c; (void)a; (void)w; (void)v; return (fsError); }
 uint32_t fat_jour_init (fsFAT_Volume *v)                            { (void)v;                   return (true);    }
#else
 #if (FSJ_USE == 0)
  uint32_t fat_jour_init (fsFAT_Volume *v)                               { (void)v;                            return (true);  }
  uint32_t fat_jour_prep (fsFAT_Volume *v)                               { (void)v;                            return (true);  }
  uint32_t fsj_set_space (FSJOUR *p, uint32_t s, uint32_t c)             { (void)p; (void)s; (void)c;          return (false); }
  uint32_t fsj_write     (FSJOUR *p, uint32_t s, uint32_t c, uint8_t *b) { (void)p; (void)s; (void)c; (void)b; return (false); }
  uint32_t fsj_init      (FSJOUR *p, FAT_DRV *d)                         { (void)p; (void)d;                   return (false); }
 #endif
#endif /* FAT_USE */

#if (NCACHE_USE == 0)
/* FAT Name Cache Routines */
  extern uint32_t elink_init      (void *p);
  extern uint32_t elink_dir       (uint32_t i, uint32_t j, void *p);
  extern uint32_t elink_cmd       (uint32_t i, void *p);
  extern uint32_t elink_insert    (void *e, void *p);
  extern uint32_t elink_delete    (void *e, void *p);
  extern uint32_t elink_scan      (uint32_t i, void **e, void *p);

  uint32_t   elink_init      (void *p)                         { (void)p;                   return (1); }
  uint32_t   elink_dir       (uint32_t i, uint32_t j, void *p) { (void)i; (void)j; (void)p; return (1); }
  uint32_t   elink_cmd       (uint32_t i, void *p)             { (void)i; (void)p;          return (0); }
  uint32_t   elink_insert    (void *e, void *p)                { (void)e; (void)p;          return (1); }
  uint32_t   elink_delete    (void *e, void *p)                { (void)e; (void)p;          return (0); }
  uint32_t   elink_scan      (uint32_t i, void **e, void *p)   { (void)i; (void)e; (void)p; return (1); }
#endif
