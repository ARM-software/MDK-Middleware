/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_nand_flash.h
 * Purpose: NAND Flash Layer Definitions for File System
 *----------------------------------------------------------------------------*/

#ifndef FS_NAND_FLASH_H__
#define FS_NAND_FLASH_H__

#include "fs_core_lib.h"
#include "Driver_NAND.h"

/* NAND Configuration macros */
#define NAND_ROW_CYCLES(b,p)  ((b*p > 65536) ? 3 : 2)
#define NAND_COL_CYCLES(s)    ((s   >   528) ? 2 : 1)
#define NAND_BTT_SIZE(b)      ((b*3)/100)
#define NAND_SEC_PAGE(p)      (p/512)
#define NAND_SEC_BLOCK(b,p)   ((p/512)*b)
#define _DS_(b)               (b-(b*3)/100-1)
#define _EP_(p)               ((p/512)*128)
#define NAND_TSN_SIZE(b,p)    ((_DS_(b)+_EP_(p)-1)/_EP_(p))

/* NAND Page Layout configuration */
typedef struct _NAND_PAGE_LAYOUT {
  struct {
    uint8_t  ofs_lsn;                   /* LSN position, where logical sector number (LSN) is placed. Usually, this is the first byte of Spare and has the value 0. LSN is a 32-bit value. */
    uint8_t  ofs_dcm;                   /* Page Data corrupted marker. Usually, this byte is the 5-th byte of Spare and has the value 4. */
    uint8_t  ofs_bbm;                   /* Bad Block marker position. Is usually placed as the 6-th byte of Spare and has the value 5. */
    uint8_t  ofs_ecc;                   /* Position of the first byte of Error Correction Code (ECC). Is usually the 7-th byte of Spare and has the value 6. This value is used by flash translation layer only if ECC is encoded and decoded in software. */
  } spare;                              /* Structure spare */
  uint16_t spare_ofs;                   /* Spare area offset from beginning of the page. */
  uint16_t spare_inc;                   /* Column increment till next spare. If page contains multiple sectors, then the first byte of the first spare area is determined by reading spare_ofs value. Location of the first byte of the second spare is (spare_inc + spare_ofs). */
  uint16_t sector_inc;                  /* Column increment till next sector. If page contains multiple sectors, then the first sector always starts at the beginning of the page (byte zero). Second sector starts at sect_inc, third sector at (sect_inc + sect_inc) and so on. */
} NAND_PAGE_LAYOUT;

/* NAND Flash Device information */
typedef struct _NAND_DEVICE {
  NAND_PAGE_LAYOUT *page_layout;        /* Page Layout configuration          */
  uint8_t           bus_width;          /* Device bus width (0=8bit, 1=16bit  */
  uint8_t           device_number;      /* Device number (chip select)        */
  uint16_t          page_size;          /* Page Size in bytes                 */
  uint32_t          block_count;        /* Number of Blocks in Device         */
  uint16_t          page_count;         /* Number of Pages per Block          */
  uint16_t          block_sectors;      /* Number of Sectors per Block        */
  uint8_t           page_sectors;       /* Number of Sectors per Page         */
  uint8_t           row_cycles;         /* Number of Row address cycles       */
  uint8_t           col_cycles;         /* Number of Column address cycles    */
  uint8_t           sw_ecc;             /* Type of software ECC operation     */
  ARM_NAND_ECC_INFO
  const            *ecc;                /* Device ECC configuration           */
} NAND_DEVICE;

/* NAND Driver Info */
typedef struct _NAND_HW_DRIVER {
  ARM_NAND_SignalEvent_t hw_cb;         /* Hardware event handler (callback)  */
  ARM_DRIVER_NAND       *drv;           /* NAND driver                        */
  ARM_NAND_CAPABILITIES  capabilities;  /* NAND driver capabilities           */
  uint8_t                init;          /* Initialization counter             */
  uint8_t                reserved[3];   /* Reserved for future use            */
} NAND_HW_DRIVER;

/* Signal NAND Media Driver event */
typedef void (*NAND_Media_SignalEvent_t) (uint32_t dev_num, uint32_t event);

/* NAND Media Driver Instance */
typedef struct _NFTL_DRIVER_HANDLE {
  NAND_Media_SignalEvent_t media_cb;    /* NAND Media event handler (callback)*/
  NAND_HW_DRIVER          *hw;          /* Hardware driver handle             */
  NAND_DEVICE const       *dev;         /* NAND Device description            */
  uint32_t                 seq;         /* Sequence number                    */
  uint16_t                 ecc;         /* ECC algorithm index and ECC flags  */
  uint8_t volatile         event;       /* Event flags                        */
  uint8_t                  status;      /* Device status byte                 */
  uint8_t                  instance;    /* Media handle instance              */
  uint8_t                  jedec_id;    /* JEDEC Manufacturer ID              */
  uint8_t                  ecc_req;     /* ECC correctability requirement     */
  uint8_t                  rsvd;        /* Reserved for future use            */
} NAND_MEDIA_HANDLE;

/* NAND Feature Parameters */
typedef struct _NAND_FEATURE_PARAMETER {
  uint8_t P1;
  uint8_t P2;
  uint8_t P3;
  uint8_t P4;
} NAND_FEATURE_PARAMETER;

/* NAND Media Interface Driver */
typedef struct _NAND_MEDIA_DRIVER {
  int32_t (*Initialize)     (NAND_MEDIA_HANDLE *h);
  int32_t (*Uninitialize)   (NAND_MEDIA_HANDLE *h);
  int32_t (*ResetDevice)    (NAND_MEDIA_HANDLE *h);
  int32_t (*ReadID)         (NAND_MEDIA_HANDLE *h, uint8_t addr, uint8_t *buf, uint32_t len);
  int32_t (*ReadParamPage)  (NAND_MEDIA_HANDLE *h, uint32_t col, uint8_t *buf, uint32_t len);
  int32_t (*ReadPage)       (NAND_MEDIA_HANDLE *h, uint32_t row, uint32_t col, uint8_t *buf, uint32_t len);
  int32_t (*WritePage)      (NAND_MEDIA_HANDLE *h, uint32_t row, uint32_t col, const uint8_t *buf, uint32_t len);
  int32_t (*CopyPage)       (NAND_MEDIA_HANDLE *h, uint32_t row_src, uint32_t row_dst, uint32_t row_cnt);
  int32_t (*EraseBlock)     (NAND_MEDIA_HANDLE *h, uint32_t row);
  int32_t (*ReadStatus)     (NAND_MEDIA_HANDLE *h, uint8_t *stat);
  int32_t (*GetFeatures)    (NAND_MEDIA_HANDLE *h, uint8_t addr, uint8_t *buf, uint32_t len);
  int32_t (*SetFeatures)    (NAND_MEDIA_HANDLE *h, uint8_t addr, const uint8_t *buf, uint32_t len);
} const NAND_MEDIA_DRIVER;

/* NAND FTL Block Index Cache */
typedef struct {
  uint8_t  *pgIdx;                      /* Index buffer                       */
  uint16_t pbn;                         /* Physical block number              */
  uint16_t lbn;                         /* Logical block number               */
  uint8_t  typ;                         /* Type of block in cache             */
  uint8_t  nextPg;                      /* Next page to be indexed            */
  uint8_t  rsvd[2];                     /* Reserved for future use            */
} BLOCK_CACHE;

/* NAND FTL Page Cache */
typedef struct {
  uint32_t row;                         /* Row address of the page in cache   */
  uint8_t *buf;                         /* Cached page buffer                 */
} PAGE_CACHE;

/* NAND FTL Buffer Pointers Structure */
typedef struct {
  uint32_t CacheBS;                     /* Block slot for block caching       */
  uint32_t CachePS;                     /* Page slot for page caching         */
  uint16_t CachedBlocks;                /* Number of indexed blocks           */
  uint16_t CachedPages;                 /* Number of cached pages             */
  BLOCK_CACHE *Block;                   /* Block indexing cache info struct   */
  PAGE_CACHE  *Page;                    /* Page data cache info structure     */
} NAND_FTL_CACHE;

/* NAND FTL Configuration structure */
typedef struct nand_ftl_cfg {
  /* Block Translation Table Space */
  uint16_t BttStartBn;                  /* First Physical Block               */
  uint16_t BttEndBn;                    /* Last Physical Block                */
  /* Data Space */
  uint16_t DataStartBn;                 /* First Physical Block               */
  uint16_t DataEndBn;                   /* Last Physical Block                */
  /* Buffer Sizes */  
  uint16_t NumCacheBlocks;              /* Number of indexed data blocks      */
  uint16_t NumCachePages;               /* Number of cached data pages        */
  uint16_t TsnTableSize;                /* Translation table cache size       */
  uint8_t  Reserved[2];                 /* Reserved for future use            */
  /* Page buffer & Caches */
  uint8_t     *PgBuf;                   /* Page data buffer                   */
  BLOCK_CACHE *BlockCache;              /* Block indexing cache info struct   */
  uint8_t     *BlockCacheBuf;           /* Block indexing cache buffer        */
  PAGE_CACHE  *PageCache;               /* Page data cache info structure     */
  uint8_t     *PageCacheBuf;            /* Page data cache buffer             */
  uint32_t    *TsnTable;                /* Translation table cache buffer     */
} const NAND_FTL_CFG;

/* NAND Device Control block */
typedef struct {
  NAND_MEDIA_HANDLE *Media;             /* NAND Media Driver Instance         */
  NAND_FTL_CFG      *Cfg;               /* NAND FTL configuration structure   */
  NAND_FTL_CACHE     Ca;                /* FTL cache info structure           */
  NAND_PAGE_LAYOUT   PgLay;             /* Page layout definition             */

  uint8_t   PageSectors;                /* Number of sectors/page 1,4,8, ...  */
  uint8_t   SPP;                        /* Number of sectors/page             */
                                        /*  (2^n, n = SPP)                    */
  uint8_t   PPB;                        /* Number of pages/block              */
                                        /*  (2^n, n = PPB)                    */
  uint8_t   SPB;                        /* Number of sectors/block            */
                                        /*  (2^n, n = SPB)                    */
  uint8_t   EPS;                        /* Entries/table sector               */
                                        /*  (2^n, n = EPS)                    */
  uint8_t   LastECC;                    /* ECC status after last page read    */
  uint16_t  LastDBN;                    /* Last allocated phy. block          */
                                        /*  in data area                      */
  uint16_t  LastTBN;                    /* Last allocated phy. block          */
                                        /*  in table area                     */
  uint16_t  CurrLBN;                    /* Current logical block number       */
  uint16_t  GcLBN;                      /* Current logical block number used  */
                                        /*  by forced GC                      */
  uint16_t  PbnQ[3];                    /* Empty block queue                  */
  uint16_t  BadBlockCnt;                /* Bad Block Counter                  */
  uint16_t  NumDataBlocks;              /* Number of data blocks reported     */
  uint16_t  TsnTableSize;               /* Translation table cache size       */
  uint32_t *TsnTable;                   /* Translation table cache buffer     */
  uint8_t  *PgBuf;                      /* Page data buffer                   */
  uint8_t   Status;                     /* FTL Status Flags                   */
  uint8_t   Reserved[3];                /* Reserved for future use            */
} NAND_FTL_DEV;

/* NAND Flash Translation Layer interface functions */
extern uint32_t ftl_Init           (uint32_t mode, NAND_FTL_DEV *ftl);
extern uint32_t ftl_UnInit         (uint32_t mode, NAND_FTL_DEV *ftl);
extern uint32_t ftl_ReadSect       (uint32_t lsn, uint8_t *buf, uint32_t cnt, NAND_FTL_DEV *ftl);
extern uint32_t ftl_WriteSect      (uint32_t lsn, const uint8_t *buf, uint32_t cnt, NAND_FTL_DEV *ftl);
extern uint32_t ftl_ReadInfo       (fsMediaInfo *info, NAND_FTL_DEV *ftl);
extern fsStatus ftl_DevCtrl        (fsDevCtrlCode code, void *p, NAND_FTL_DEV *ftl);
extern void     ftl_DrvEvent       (uint32_t event, NAND_FTL_DEV *ftl);

/* NAND driver event handler */
extern void     nand_DrvEvent      (uint32_t event, NAND_MEDIA_HANDLE *h);

#endif /* FS_NAND_FLASH_H__ */
