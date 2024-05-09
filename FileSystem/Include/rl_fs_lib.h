/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    rl_fs_lib.h
 * Purpose: File System Library Definitions
 *----------------------------------------------------------------------------*/

#ifndef __RL_FS_LIB_H
#define __RL_FS_LIB_H

#include <stddef.h>

#include "FS_Config.h"
#include "FS_Debug.h"

#include "rl_fs.h"
#include "rl_fs_rte.h"

#if defined(FS_MEMORY_CARD_0) || defined(FS_MEMORY_CARD_1)
  #include "fs_memory_card.h"
#endif

#if defined(FS_NAND_FLASH_0) || defined(FS_NAND_FLASH_1)
  #include "fs_nand_flash.h"
#endif

#if defined(FS_NOR_FLASH_0) || defined(FS_NOR_FLASH_1)
  #include "fs_nor_flash.h"
#endif

/* Enable debug build */
#if defined(FS_DEBUG_EVR) && (FS_DEBUG_EVR_ENABLE == 1)
  #define FS_DEBUG
#endif


/**** File Open Mode definitions ****/
#define FS_FOPEN_RD            0x0000       ///< Open file for reading only
#define FS_FOPEN_WR            0x0001       ///< Open file for writing only
#define FS_FOPEN_RDWR          0x0002       ///< Open file for reading and writing
#define FS_FOPEN_APPEND        0x0008       ///< Open file in append mode
#define FS_FOPEN_CREATE        0x0100       ///< Create file if it does not exist
#define FS_FOPEN_TRUNCATE      0x0200       ///< Truncate existing file

/**** File Seek Operation definitions ****/
#define FS_FSEEK_SET           0            ///< Seek from the start of the file
#define FS_FSEEK_CUR           1            ///< Seek from the current location
#define FS_FSEEK_END           2            ///< Seek from the end of the file


/* Switch case statement fall through compiler attribute */
#ifndef __FALLTHROUGH
  #define __FALLTHROUGH  __attribute__((fallthrough))
#endif

/* Access to the unaligned unsigned 32-bit variable */
#ifndef __UNALIGNED_UINT32
  struct __attribute__((packed)) T_UINT32 { uint32_t v; };
  #define __UNALIGNED_UINT32(x)                  (((struct T_UINT32 *)(x))->v)
#endif

/* Device System Type attribute */
#define FS_CUR         ((uint8_t)0x01)  /* Initial Current Drive attribute    */
#define FS_EMB         ((uint8_t)0x02)  /* Embedded File System (F:)          */
#define FS_FAT         ((uint8_t)0x04)  /* FAT File System      (M:,N:,R:,U:) */
#define FS_IOC         ((uint8_t)0x08)  /* IO Control allowed   (M:,N:)       */

/* Driver Init/Uninit parameter codes */
#define DM_IO                       0U  /* Init/Uninit IO peripherals         */
#define DM_MEDIA                    1U  /* Init/Uninit storage Media          */

/* Private DeviceCtrl parameter codes */
#define fsDevCtrlCodeBlockCount    255  /* Retrieve total number of blocks on the volume */
#define fsDevCtrlCodeErasedValue   254  /* Retrieve the value of erased memory content   */
#define fsDevCtrlCodeProgramUnit   253  /* Retrieve smallest programmable unit in bytes  */

#ifdef __cplusplus
extern "C"  {
#endif

/* Mutex for FileSystem */
typedef void *FS_MUTEX;

/* Device driver mapping type */
typedef struct {
  void   *dcb;                          /* Device Control block               */
  char    id[3];                        /* Drive Name Identification          */
  uint8_t attr;                         /* Drive attributes                   */
} const FS_DEV;

/* RAM interface control block */
typedef struct {
  uint8_t *Base;                        /* RAM buffer base address            */
  uint32_t Size;                        /* RAM buffer size                    */
} RAM_DEV;

/* FAT Name Cache Configuration */
typedef struct fat_nca_cfg {
  uint8_t   MaxPathDepth;               /* Maximum path depth                 */
  uint8_t   ControlBlockCnt;            /* Number of control blocks           */
  uint32_t  NameMemPoolSize;            /* Name cache memory pool size        */
  uint32_t *NameMemPool;                /* Name cache memory pool             */
} const FAT_NCACHE_CFG;

/* FAT Sector Caching structure */
typedef struct fcache {
  uint32_t sect;                        /* Cached FAT sector number           */
  uint8_t  *buf;                        /* FAT sector cache buffer            */
  uint8_t  dirty;                       /* FAT table content modified         */
  uint8_t  cfat;                        /* Current FAT                        */
  uint8_t  rsvd[2];                     /* Reserved for future use            */
} FCACHE;

/* Data Sector Caching structure */
typedef struct dcache {
  uint32_t sect;                        /* Working Data sector number         */
  uint32_t csect;                       /* Data Cache starting sector number  */
  uint8_t  *buf;                        /* Working Data sector buffer         */
  uint8_t  *cbuf;                       /* Data Cache sector buffer           */
  uint8_t  nwr;                         /* Number of buffered write sectors   */
  uint8_t  nrd;                         /* Number of cached read sectors      */
  uint8_t  rsvd[2];                     /* Reserved for future use            */
} DCACHE;

/* Name Caching structure */
typedef struct ncache {
  uint32_t  max_path_depth;             /* Maximum path depth                 */
  uint32_t  size;                       /* Name cache memory pool size        */
  uint32_t *buf;                        /* Name cache memory pool             */
} NCACHE;

/* FAT Volume Information struct */
typedef struct fatinfo {
  uint32_t FAT32_RootClus;              /* Root directory first cluster       */
  uint16_t FAT32_ExtFlags;              /* FAT extended flags                 */
  uint16_t FAT32_FSInfo;                /* FSInfo structure sector number     */
  uint16_t FAT32_BkBootSec;             /* Boot Record copy sector number     */
  uint8_t  FatType;                     /* File System Fat type.              */
  uint8_t  SecPerClus;                  /* Number of Sectors per Cluster      */
  uint32_t BootSector;                  /* Boot Sector Offset                 */
  uint32_t FatSize;                     /* Number of Sectors per FAT table    */
  uint32_t DskSize;                     /* Disk Size Sector Count             */
  uint32_t DataSecCnt;                  /* Number of Data Sectors             */
  uint16_t RootEntCnt;                  /* Maximum Root Directory entries     */
  uint16_t BytesPerSec;                 /* Sector Size in bytes               */
  uint32_t DataClusCnt;                 /* Data Cluster Count                 */
  uint32_t RootDirAddr;                 /* Root Dir First Sector              */
  uint32_t ClusSize;                    /* Cluster Size in bytes              */
  uint16_t RsvdSecCnt;                  /* Number of Reserved Sectors         */
  uint16_t RootSecCnt;                  /* Number of Sectors for Root dir     */
  uint16_t EntsPerClus;                 /* Number of entries per cluster      */
  uint8_t  NumOfFat;                    /* Number of Fat tables               */
  uint8_t  Reserved;                    /* Reserved for future use            */
} FATINFO;

/* FAT File System driver */
typedef struct {
  uint32_t (*Init)        (uint32_t mode);
  uint32_t (*UnInit)      (uint32_t mode);
  uint32_t (*ReadSect)    (uint32_t sect, uint8_t *buf, uint32_t cnt);
  uint32_t (*WriteSect)   (uint32_t sect, const uint8_t *buf, uint32_t cnt);
  uint32_t (*ReadInfo)    (fsMediaInfo *cfg);
  fsStatus (*DeviceCtrl)  (fsDevCtrlCode code, void *p);
} const FAT_DRV;

/* File System Journal Control block */
typedef struct fsjour {
  FAT_DRV *drv;                         /* Memory device driver               */
  uint8_t *buf;                         /* Journal buffer                     */
  uint32_t FirstSect;                   /* First sector of journal            */
  uint32_t JournSect;                   /* Number of sectors available        */
  uint32_t TrId;                        /* Transaction identifier             */
  uint32_t TrSect;                      /* Transaction sector                 */
  uint8_t  State;                       /* Journal state                      */
  uint8_t  Status;                      /* Journal status                     */
  uint8_t  Reserved[2];                 /* Reserved for future use            */
} FSJOUR;

/* FAT Volume Description */
typedef struct _fsFAT_Volume {
  uint32_t    DrvLet;                   /* 4-byte encoded drive letter string */
  FS_MUTEX    Mutex;                    /* Volume mutex                       */
  FAT_DRV    *Drv;                      /* Registered FAT Device Driver       */
  uint32_t    Status;                   /* Volume Status                      */
  uint32_t   *CaBuf;                    /* Cache Buffer (FAT + Data)          */
  uint32_t    CaSize;                   /* Cache Buffer size                  */
  uint32_t    free_clus_cnt;            /* FAT32: Number of free clusters     */
  uint32_t    free_clus;                /* FAT32: First free cluster          */
  uint32_t    cdir_clus;                /* Current directory cluster          */
  NCACHE     *ncache;                   /* Name cache memory                  */
  FSJOUR     *fsj;                      /* File System Journal                */
  FATINFO     cfg;                      /* FAT Volume configuration           */
  FCACHE      fat;                      /* FAT table cache control            */
  DCACHE      ca;                       /* Data cache control                 */
  uint16_t    RsvdS;                    /* Reserved sectors used by journal   */
  uint8_t     Reserved[2];              /* Reserved for future use            */
} fsFAT_Volume;

/* FAT File Handle Description */
typedef struct _fsFAT_Handle {
  fsFAT_Volume *vol;                    /* FAT volume pointer                 */
  uint32_t  fsize;                      /* File size                          */
  uint32_t  fcsz;                       /* Current file size                  */
  uint32_t  fpos;                       /* File position indicator            */
  uint16_t  flags;                      /* File status flags                  */
  uint16_t  short_ent_offs;             /* SFN: Short entry sector offset     */
  uint32_t  short_ent_clus;             /* SFN: Short entry cluster number    */
  uint32_t  first_clus;                 /* First data cluster                 */
  uint32_t  current_clus;               /* Current data cluster               */
  uint8_t   current_sect;               /* Current data sector                */
  uint8_t   rsvd[3];                    /* Reserved for future use            */
} fsFAT_Handle;

/* EFS File System driver */
typedef struct _EFS_DRV {
  fsStatus (*Init)             (void);
  fsStatus (*UnInit)           (void);
  uint32_t (*GetSectorAddress) (uint32_t sect);
  uint32_t (*GetSectorSize)    (uint32_t sect);
  fsStatus (*DataWrite)        (uint32_t addr, void *buf, uint32_t cnt);
  fsStatus (*DataRead)         (uint32_t addr, void *buf, uint32_t cnt);
  fsStatus (*SectorErase)      (uint32_t sect);
  fsStatus (*DeviceCtrl)       (fsDevCtrlCode code, void *p);
} const EFS_DRV;

/* EFS Volume Description */
typedef struct _fsEFS_Volume {
  uint32_t  DrvLet;                     /* 4-byte encoded drive letter string */
  FS_MUTEX  Mutex;                      /* Volume mutex                       */
  EFS_DRV  *Drv;                        /* Registered EFS media driver        */
  uint32_t  Status;                     /* Volume status                      */
  uint32_t  Size;                       /* Volume size                        */
  uint32_t  ErasedValue;                /* Erased memory value (0xFF or 0x00) */
  uint16_t  SectorCount;                /* Number of available memory sectors */
  uint16_t  TopID;                      /* Top used FileID                    */
} fsEFS_Volume;

/* EFS File Handle Description */
typedef struct _fsEFS_Handle {
  fsEFS_Volume *vol;                    /* EFS volume pointer                 */
  uint32_t  fsize;                      /* File Size                          */
  uint32_t  fpos;                       /* File position indicator            */
  uint16_t  flags;                      /* File status flags                  */
  uint16_t  fileID;                     /* File identification number         */
  uint16_t  fidx;                       /* Current file chunk index           */
  uint16_t  fblock;                     /* Current block index                */
  uint32_t  fbot;                       /* Block bottom position              */
  uint32_t  ftop;                       /* Block top position                 */
} fsEFS_Handle;

/* File System Configuration function */
extern void     fs_config          (const char *drive);
extern void     fs_config_evr      (void);

/* RAM drive interface functions */
extern uint32_t ram_Init           (uint32_t mode, RAM_DEV *ram);
extern uint32_t ram_UnInit         (uint32_t mode, RAM_DEV *ram);
extern uint32_t ram_ReadSector     (uint32_t sect, uint8_t *buf, uint32_t cnt, RAM_DEV *ram);
extern uint32_t ram_WriteSector    (uint32_t sect, const uint8_t *buf, uint32_t cnt, RAM_DEV *ram);
extern uint32_t ram_ReadInfo       (fsMediaInfo *info, RAM_DEV *ram);
extern fsStatus ram_DevCtrl        (fsDevCtrlCode code, void *p, RAM_DEV *ram);

/* USB Mass Storage interface functions */
extern bool     FS_USBH_MSC_Initialize  (uint8_t instance, uint32_t mode);
extern bool     FS_USBH_MSC_Uninitialize(uint8_t instance, uint32_t mode);
extern bool     FS_USBH_MSC_ReadSectors (uint8_t instance, uint32_t sector_addr, uint32_t sector_num,       uint8_t *buf);
extern bool     FS_USBH_MSC_WriteSectors(uint8_t instance, uint32_t sector_addr, uint32_t sector_num, const uint8_t *buf);
extern bool     FS_USBH_MSC_ReadInfo    (uint8_t instance, fsMediaInfo *info);
extern fsStatus FS_USBH_MSC_DeviceCtrl  (uint8_t instance, fsDevCtrlCode code, void *p);

/* File System Journal interface functions */
extern uint32_t fsj_init      (FSJOUR *fsj, FAT_DRV *drv);
extern uint32_t fsj_set_space (FSJOUR *fsj, uint32_t start_sect, uint32_t cnt);
extern uint32_t fsj_write     (FSJOUR *fsj, uint32_t sect, uint32_t rsec, uint8_t *buf);

/* Embedded File System interface functions */
extern int32_t  efs_handle_get(fsEFS_Volume *vol);
extern fsStatus efs_init      (fsEFS_Volume *vol);
extern fsStatus efs_uninit    (fsEFS_Volume *vol);
extern fsStatus efs_mount     (fsEFS_Volume *vol);
extern fsStatus efs_unmount   (fsEFS_Volume *vol);
extern fsStatus efs_open      (int32_t handle, const char *fn, int32_t openmode);
extern fsStatus efs_close     (int32_t handle);
extern int32_t  efs_read      (int32_t handle, uint8_t *buf, uint32_t len);
extern int32_t  efs_write     (int32_t handle, const uint8_t *buf, uint32_t len);
extern fsStatus efs_flush     (int32_t handle);
extern int32_t  efs_flen      (int32_t handle);
extern int32_t  efs_seek      (int32_t handle, int32_t offset, int32_t whence);

/* EFS Maintenance Routines */
extern fsStatus efs_delete    (const char *filename, fsEFS_Volume *vol);
extern fsStatus efs_ffind     (fsFileInfo *info, fsEFS_Volume *vol);
extern fsStatus efs_rename    (const char *filename, const char *newname, fsEFS_Volume *vol);

/* EFS Utility Routines */
extern int64_t  efs_free      (fsEFS_Volume *vol);
extern fsStatus efs_format    (fsEFS_Volume *vol);
extern int32_t  efs_analyse   (fsEFS_Volume *vol);
extern fsStatus efs_check     (fsEFS_Volume *vol);
extern fsStatus efs_defrag    (fsEFS_Volume *vol);
extern fsStatus efs_info      (fsDriveInfo *info, fsEFS_Volume *vol);

/* FAT File System interface functions */
extern int32_t  fat_handle_get(fsFAT_Volume *vol);
extern fsStatus fat_init      (fsFAT_Volume *vol);
extern fsStatus fat_uninit    (fsFAT_Volume *vol);
extern fsStatus fat_mount     (fsFAT_Volume *vol);
extern fsStatus fat_unmount   (fsFAT_Volume *vol);
extern fsStatus fat_open      (int32_t handle, const char *name, int32_t openmode);
extern fsStatus fat_close     (int32_t handle);
extern int32_t  fat_read      (int32_t handle, uint8_t *buf, uint32_t len);
extern int32_t  fat_write     (int32_t handle, const uint8_t *buf, uint32_t len);
extern fsStatus fat_flush     (int32_t handle);
extern int32_t  fat_flen      (int32_t handle);
extern int64_t  fat_seek      (int32_t handle, int64_t offset, int32_t whence);

/* FAT File Maintenance Routines */
extern fsStatus fat_delete    (const char *fn, const char *options, fsFAT_Volume *vol);
extern fsStatus fat_ffind     (const char *fn, fsFileInfo *info, fsFAT_Volume *vol);
extern fsStatus fat_rename    (const char *path, const char *newname, fsFAT_Volume *vol);
extern fsStatus fat_attrib    (fsFAT_Volume *vol, const char *path, uint32_t attrib);
extern fsStatus fat_vol       (char *label, uint32_t *serial, fsFAT_Volume *vol);

/* FAT Directory Support Routines */
extern fsStatus fat_mkdir     (const char *path, fsFAT_Volume *vol);
extern fsStatus fat_rmdir     (const char *path, const char *options, fsFAT_Volume *vol);
extern fsStatus fat_chdir     (const char *path, fsFAT_Volume *vol);
extern fsStatus fat_pwd       (      char *path, uint32_t len, fsFAT_Volume *vol);

/* FAT Time Support Routines */
extern fsStatus fat_time_set  (const char *path, fsTime *create, fsTime *access, fsTime *write, fsFAT_Volume *vol);
extern fsStatus fat_time_get  (const char *path, fsTime *create, fsTime *access, fsTime *write, fsFAT_Volume *vol);

/* FAT Utility Routines */
extern int64_t  fat_free      (fsFAT_Volume *vol);
extern fsStatus fat_format    (fsFAT_Volume *vol, const char *param);
extern fsStatus fat_media     (fsFAT_Volume *vol);
extern fsStatus fat_info      (fsDriveInfo *info, fsFAT_Volume *vol);

/* FAT Journal System Routines */
extern uint32_t fat_jour_init (fsFAT_Volume *vol);
extern uint32_t fat_jour_prep (fsFAT_Volume *vol);

/* FileSystem Retarget Routines */
extern int      __sys_open    (const char *fname, int openmode);
extern int      __sys_close   (int handle);
extern int      __sys_write   (int handle, const uint8_t *buf, uint32_t len);
extern int      __sys_read    (int handle, uint8_t *buf, uint32_t len);
extern int      __sys_ensure  (int handle);
extern int      __sys_seek    (int handle, uint32_t pos);
extern long     __sys_flen    (int handle);

/* File Interface API */
extern int32_t  fs_fopen      (const char *path, int32_t mode);
extern int32_t  fs_fclose     (int32_t handle);
extern int32_t  fs_fwrite     (int32_t handle, const void *buf, uint32_t cnt);
extern int32_t  fs_fread      (int32_t handle, void *buf, uint32_t cnt);
extern int32_t  fs_fflush     (int32_t handle);
extern int64_t  fs_fseek      (int32_t handle, int64_t offset, int32_t whence);
extern int64_t  fs_fsize      (int32_t handle);

/* RTOS abstraction for FileSystem */
extern FS_MUTEX fs_mutex_new     (const void *arg);
extern uint32_t fs_mutex_acquire (FS_MUTEX mutex);
extern uint32_t fs_mutex_release (FS_MUTEX mutex);
extern uint32_t fs_mutex_delete  (FS_MUTEX mutex);

extern uint32_t fs_ms_rtos_tick;
extern uint32_t fs_get_rtos_tick_freq (void);
extern uint32_t fs_set_rtos_delay (uint32_t millisec);
extern uint32_t fs_get_sys_tick (void);
extern uint32_t fs_get_sys_tick_us (uint32_t microsec);

/* FAT File Handle array definition */
extern fsFAT_Handle  fs_fat_fh[];
extern uint8_t const fs_fat_fh_cnt;

/* EFS File Handle array definition */
extern fsEFS_Handle  fs_efs_fh[];
extern uint8_t const fs_efs_fh_cnt;

/* Drives array definition */
extern FS_DEV        fs_DevPool[];
extern uint8_t const fs_ndrv;

/* FileSystem version */
extern uint32_t const fs_lib_version;

#ifdef FS_DEBUG
/* Media objects */
extern RAM_DEV fs_ram0_dev;
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RL_FS_LIB_H */
