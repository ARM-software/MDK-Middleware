/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_nftl.h 
 * Purpose: NAND FTL Implementation Module Definitions
 *----------------------------------------------------------------------------*/

#ifndef __FS_NFTL_H
#define __FS_NFTL_H

#include "fs_core.h"

/* FTL Status (8-bit) */
#define FTL_STATUS_INIT   0x01U         /* Translation layer is initialized   */
#define FTL_STATUS_FORMAT 0x02U         /* Device is formatted                */
#define FTL_STATUS_MOUNT  0x04U         /* Device is mounted                  */

/* Definitions */
#define __MIN_IN_QUEUE  3               /* Min number of blocks in queue      */
#define __MAX_RETRY     5               /* Max number of retries on errors    */
#define __SZ_SP_USED    10              /* Num spare bytes used by FTL        */
#define __SZ_SPARE      16              /* Spare Area Size (scaled to Sector) */
#define __SZ_SECT       512             /* Sector Size */

#define EMPTY           0x7FFFFFFFU
#define INVALID         0xFFFFFFFFU
#define INVALID_BLOCK   0x0000FFFFU
#define BOOT_SIGN       0x4B46544CU

#define BIT_SWECC       0x00000100U
#define BIT_PRIM        0x80000000U
#define BIT_UNCOR       0x80000000U
#define BB_MASK         0x000000FFU

/* ECC Bit Errors */
#define ECC_SINGLEBITERR 1
#define ECC_MULTIBITERR  2

/* ECC Status Codes */
#define ECC_NOERR       0               /* ECC passed, no errors detected     */
#define ECC_CORRECTED   1               /* ECC failed, data was corrected     */
#define ECC_UNCORRECTED 2               /* ECC failed, data was not corrected */

#define TYP_REPL        0
#define TYP_PRIM        1

#define BTT_ENTRY_SZ    4               /* Entry size                         */
#define BTT_EPS         7               /* Entries per sector: 2^BTT_EPS      */

/* Block Allocation Masks */
#define AREA_TBL        0               /* Table Area */
#define AREA_DAT        1               /* Data Area  */

#define ALLOC_REST      3               /* Restricted Allocation              */
#define ALLOC_PENUL     2               /* Penultimate Allocation allowed     */
#define ALLOC_LAST      1               /* Last Block Allocation allowed      */

#define pDev            (ftl->Media->dev)
#define pCfg            (ftl->Cfg)

/* Default page layout definition */
#define DL_POS_LSN      0               /* NAND: 0, OneNAND: 2 */
#define DL_POS_COR      4               /* NAND: 4, OneNAND: 1 */
#define DL_POS_BBM      5               /* NAND: 5, OneNAND: 0 */
#define DL_POS_ECC      6               /* NAND: 6, OneNAND: 8 */

#define DL_SECT_INC     528             /* Spare after each sector            */
#define DL_SPARE_OFS    512
#define DL_SPARE_INC    528

/* Wait timeout flags */
#define NAND_TOUT_ASSERTED (1U << 0)
#define NAND_TOUT_EXPIRED  (1U << 1)

/* Wait timeouts in microseconds */
#define NAND_WRITE_TIMEOUT      1000    /*  1ms - Program operation timeout   */
#define NAND_ERASE_TIMEOUT     10000    /* 10ms - Erase operation timeout     */
#define NAND_RESET_TIMEOUT     25000    /* 25ms - Reset operation timeout     */

/*-----------------------------------------------------------------------------
 *      Macro for calculating logical block number from logical sector number
 *  lsn = logical sector number
 *
 *  Result: lbn = lsn / SectorsPerBlock
 *----------------------------------------------------------------------------*/
#define LBN(lsn)    (uint16_t)((lsn) >> (ftl->SPB))

/*-----------------------------------------------------------------------------
 *      Macro for calculating physical block number from row (page) address
 *  row = device row address
 *
 *  Result: pbn = row / PagesPerBlock
 *----------------------------------------------------------------------------*/
#define PBN(row)    (uint16_t)((row) >> (ftl->PPB))

/*-----------------------------------------------------------------------------
 *      Macro for calculating flash device row (page) address
 *  bn = physical block number
 *  pg = physical page number
 *
 *  Result: row = bn * PagesPerBlock + PageInBlock
 *----------------------------------------------------------------------------*/
#define ROW(bn, pg) (uint32_t)(((uint32_t)(bn) << ftl->PPB) + (pg))

/*-----------------------------------------------------------------------------
 *      Macro for calculating page index from logical sector number
 *  lsn = logical sector number
 *
 *  Result: pgIdx = (lsn % SectorsPerBlock) / SectorsPerPage
 *----------------------------------------------------------------------------*/
#define LSN2IDX(lsn) (uint8_t)(((lsn) & ((1 << ftl->SPB)-1)) >> ftl->SPP)

/*-----------------------------------------------------------------------------
 *      Macro for calculating logical sector number from page index
 *  lbn   = logical block number
 *  pgIdx = page index (result of LSN2IDX macro)
 *
 *  Result: lsn = lbn * SectorsPerBlock + pgIdx * SectorsPerPage
 *----------------------------------------------------------------------------*/
#define IDX2LSN(lbn, pgIdx) (((uint32_t)(lbn) << ftl->SPB) + ((uint32_t)(pgIdx) << ftl->SPP))


/* NFTL Status Codes */
typedef enum {
  FTL_OK            = 0,                /* No error                           */
  FTL_WARNING       = 1,                /* Warning, data not reliable         */
  FTL_ERROR,                            /* Undefined error                    */
  FTL_ERROR_PARAMETER,                  /* Invalid parameter                  */
  FTL_ERROR_UNMOUNTED,                  /* FTL is not mounted                 */
  FTL_ERROR_ECC,                        /* ECC error on last page read        */
  FTL_ERROR_BB,                         /* Bad block                          */
  FTL_ERROR_LBN,                        /* Wrong LBN                          */
  FTL_ERROR_ALLOC,                      /* Block allocation error             */
  FTL_ERROR_RANGE,                      /* BN to high                         */
  FTL_ERROR_NOT_FOUND,                  /* Item not found                     */
  FTL_ERROR_NOT_FOUND_EOB,              /* LSN not found, end of block        */
  FTL_ERROR_ECC_COR,                    /* ECC corrected the data             */
  FTL_ERROR_ECC_FAIL,                   /* ECC could not correct the data     */
  FTL_ERROR_PROGRAM,                    /* Programming failed (flash status)  */
  FTL_ERROR_ERASE,                      /* Erase verify failed (flash status) */
  FTL_ERROR_TIMEOUT,                    /* NAND hardware timeout              */
  FTL_ERROR_DRIVER                      /* Unrecoverable driver error         */
} FTL_STATUS;


/* NTFL Block Translation Table Entry Structure */
typedef struct {
  uint16_t primBN;
  uint16_t replBN;
} BTT_ITEM;

/* Public functions */
extern void    fs_nand_setup_layout  (uint32_t drive_num, NAND_PAGE_LAYOUT *layout);
extern int32_t fs_nand_feature_addr  (uint32_t drive_num, int32_t addr);
extern int32_t fs_nand_feature_param (uint32_t drive_num, int32_t addr, NAND_FEATURE_PARAMETER *fp);

#endif /* __FS_NFTL_H */
