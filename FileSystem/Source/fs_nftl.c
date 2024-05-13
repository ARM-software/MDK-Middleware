/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_nftl.c
 * Purpose: NAND FTL Implementation
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "cmsis_compiler.h"

#include "fs_nftl.h"
#include "fs_nand_media.h"
#include "fs_evr.h"

/* Access point to the NAND Media driver */
extern NAND_MEDIA_DRIVER NAND_MediaDriver;


/*-----------------------------------------------------------------------------
 *      Local function prototypes
 *----------------------------------------------------------------------------*/
static uint32_t ScanBlock     (NAND_FTL_DEV *ftl, uint16_t bn, uint32_t lsn, uint32_t *pageNum);
static uint32_t AllocBlock    (NAND_FTL_DEV *ftl, uint16_t *bn, uint32_t restriction, uint32_t area);
static uint32_t UpdateBTT     (NAND_FTL_DEV *ftl, uint16_t lbn, uint16_t *primBN, uint16_t *replBN);
static uint32_t GcRun         (NAND_FTL_DEV *ftl, uint16_t lbn, BTT_ITEM *btt, uint32_t gcArea, uint16_t allocBn);
static uint32_t MoveBlock     (NAND_FTL_DEV *ftl, uint16_t srcBN, uint16_t destBN, uint32_t endPg, uint32_t area);
static uint32_t RelocBlock    (NAND_FTL_DEV *ftl, uint16_t *pbn, uint32_t endPg, uint32_t area, uint32_t eraseBlock);
static uint32_t ForceDataGc   (NAND_FTL_DEV *ftl);
static uint32_t MarkBlockBad  (NAND_FTL_DEV *ftl, uint16_t pbn, uint32_t eraseBlock);

static void     EncodeECC     (NAND_FTL_DEV *ftl, uint8_t *pgBuf);
static uint32_t DecodeECC     (NAND_FTL_DEV *ftl, uint8_t *pgBuf);


/**
  NAND Flash Translation Layer Media event handler

  \param[in]      event     NAND media driver event flags
  \param[in]      ftl       FTL instance object
*/
void ftl_DrvEvent (uint32_t event, NAND_FTL_DEV *ftl) {
  (void)event;
  (void)ftl;
  /* Handle event here */
}


/**
  Callback function used to override the default NAND page layout setup.

  Using this function it is possible to override the default NAND page layout
  used by the NAND Flash Translation Layer.

  Setup offers the following configuration options:
  ofs_lsn: Position in spare area, where logical sector number (LSN) is
           placed. Usually, this is the first byte of spare, therefore
           ofs_lsn has value zero. LSN is a 32-bit value.
  ofs_dcm: Position of data corrupted marker in spare area. Usually, this byte
           is the fifth byte of spare and ofs_dcm has value four.
  ofs_bbm: Position of bad block marker (BBM) in spare area and is usually
           placed as the sixth byte of spare, ofs_bbm has value 5.
  ofs_ecc: Position of the first byte of Error Correction Code (ECC) bytes in
           the spare area. First ECC byte is default seventh byte of spare
           (ofs_ecc == 6). This value is used by flash translation layer only
           if ECC is encoded and decoded in software.

  sector_inc: Provides information about user data sector locations within
              page. If page contains multiple sectors, first sector always
              starts at the beginning of the page (byte zero). Second sector
              starts at sector_inc, third sector at sector_inc + sector_inc
              and so on.
  spare_ofs:  Provides information about the location of the first spare area
              byte within page.
  spare_inc:  Provides information about spare area locations within page.
              If page contains multiple sectors, first byte of the first
              spare area is determined by reading spare_ofs value. Location of
              the first byte of the second spare, can be determined by adding
              spare_inc value to the spare_ofs value.

  \param[in]     drive_num NAND drive number
  \param[in,out] layout    page layout definition structure
*/
__WEAK void fs_nand_setup_layout (uint32_t drive_num, NAND_PAGE_LAYOUT *layout) {
  (void)drive_num;
  (void)layout;
/*
  Example: default spare metadata layout
  layout->spare.ofs_lsn  = 0;
  layout->spare.ofs_dcm  = 4;
  layout->spare.ofs_bbm  = 5;
  layout->spare.ofs_ecc  = 6;

  Example: default |sector|spare| ... |sector|spare| layout
  layout->sector_inc     = 528;
  layout->spare_ofs      = 512;
  layout->spare_inc      = 528;

  Example: default |sector|sector| ... |spare|spare| layout
  layout->sector_inc     = 512;
  layout->spare_ofs      = 2112;
  layout->spare_inc      = 16;
*/
}


/**
  Callback function used to retrieve NAND feature address

  This function is called continuously until it returns -1. Parameter addr
  indicates which feature address was returned on the previous call and is
  equal to -1 when function is called for the first time.

  \param[in]  drive_num NAND drive number
  \param[in]  addr      previous feature address
  \return feature address or -1 to stop the iteration
*/
__WEAK int32_t fs_nand_feature_addr (uint32_t drive_num, int32_t addr) {
  (void)drive_num;

  /* Not implemented by default */
  return (addr);
}


/**
  Callback function used to review and modify NAND feature parameters

  This function provides feature parameters retrieved from the address which was
  previously returned by the GetFeatureAddress function.
  Application can change the parameters and return 0 to modify them. In this case
  feature parameters will be written back to the device. When function returns -1
  feature parameters are not modified and are left intact.

  \param[in]     drive_num NAND drive number
  \param[in]     addr      feature address
  \param[in,out] fp        feature parameters
  \return 0 to set the parameters or -1 to continue without changes
*/
__WEAK int32_t fs_nand_feature_param (uint32_t drive_num, int32_t addr, NAND_FEATURE_PARAMETER *fp) {
  (void)drive_num;
  (void)addr;
  (void)fp;

  /* Not implemented by default */
  return (-1);
}


/**
  Retrieve NAND feature address.
  See \ref fs_nand_feature_addr

  \param[in]  h         NAND media handle
  \param[in]  addr      previous feature address
  \return feature address or -1 to stop the iteration
*/
static int32_t get_feature_addr (NAND_MEDIA_HANDLE *h, int32_t addr) {
  int32_t next;

  /* Break function execution (default) */
  next = -1;

  if (addr == -1) {
    /* First function call */
    if (h->dev->sw_ecc == 2U) {
      /* EZ NAND */
      if (h->jedec_id == JEDEC_ID_MICRON_TECH) {
        /* Special handling for Micron devices which require "Enable ECC" bit */
        next = 0x90; /* Array operation mode */
      }
    }
  }

  return (next); /* Return value is the address in the NAND "Get Features" command */
}


/**
  Review and modify NAND feature parameters.
  See \ref fs_nand_feature_param

  \param[in]     h         NAND media handle
  \param[in]     addr      feature address
  \param[in,out] fp        feature parameters
  \return 0 to set the parameters or -1 to continue without changes
*/
static int32_t set_feature_param (NAND_MEDIA_HANDLE *h, int32_t addr, NAND_FEATURE_PARAMETER *fp) {
  int32_t modify;

  modify = -1;

  if (h->jedec_id == JEDEC_ID_MICRON_TECH) {
    /* Special handling for Micron devices which require "Enable ECC" bit */
    if (addr == 0x90U) {
      /* Received "Array operation mode" feature parameters */
      fp->P1 = 0x08U; /* Enable internal ECC */
      modify = 0;
    }
  }

  /*  0: modify parameters using NAND "Set Features" command*/
  /* -1: continue without changes */
  return (modify);
}


/* ------------------------- Media driver functions ------------------------- */


/**
  NAND media driver ReadPage wrapper

  \param[in]     row       page row address
  \param[out]    buf       page buffer
  \param[in]     ftl       FTL instance object
*/
static uint32_t Drv_ReadPage (uint32_t row, uint8_t *buf, NAND_FTL_DEV *ftl) {
  int32_t  rtv;
  uint32_t status;
  uint8_t  nand_status;

  EvrFsNFTL_PageRead (ftl->Media->instance, row / ftl->Media->dev->page_count, row % ftl->Media->dev->page_count);

  rtv = NAND_MediaDriver.ReadPage (ftl->Media, row, 0, buf, pDev->page_size);

  switch (rtv) {
    case NAND_OK:
      status = FTL_OK;

      if (pDev->sw_ecc == 2) {
        /* EZ NAND is used, check ECC status */
        NAND_MediaDriver.ReadStatus (ftl->Media, &nand_status);

        EvrFsNFTL_StatusRead (ftl->Media->instance, nand_status);

        if (nand_status & NAND_STAT_CSP) {
          status = FTL_ERROR_ECC_COR;
        }
      }
      break;

    case NAND_ERROR_ECC_CORRECTED:
      status = FTL_ERROR_ECC_COR;
      break;

    case NAND_ERROR_ECC_FAILED:
      status = FTL_ERROR_ECC_FAIL;
      break;

    default:
      /* Read page error */
      EvrFsNFTL_PageReadFailed (ftl->Media->instance, row);
      status = FTL_ERROR_DRIVER;
      break;
  }

  return (status);
}


/**
  NAND media driver WritePage wrapper

  \param[in]     row       page row address
  \param[in]     buf       page buffer
  \param[in]     ftl       FTL instance object
*/
static uint32_t Drv_WritePage (uint32_t row, uint8_t *buf, NAND_FTL_DEV *ftl) {
  int32_t  rtv;
  uint32_t tick, tout;
  uint32_t exec_status;
  uint8_t  nand_status;

  EvrFsNFTL_PageWrite (ftl->Media->instance, row / ftl->Media->dev->page_count, row % ftl->Media->dev->page_count);

  rtv = NAND_MediaDriver.WritePage (ftl->Media, row, 0, buf, pDev->page_size);

  if (rtv == NAND_OK) {
    nand_status = 0;

    /* Wait until NAND ready or timeout expires */
    tout = fs_get_sys_tick_us (NAND_WRITE_TIMEOUT);
    tick = fs_get_sys_tick();
    do {
      NAND_MediaDriver.ReadStatus (ftl->Media, &nand_status);

      if (nand_status & NAND_STAT_RDY) {
        break;
      }
    } while ((fs_get_sys_tick() - tick) < tout);

    if ((nand_status & NAND_STAT_RDY) == 0) {
      NAND_MediaDriver.ReadStatus (ftl->Media, &nand_status);
    }

    EvrFsNFTL_StatusRead (ftl->Media->instance, nand_status);

    if (nand_status & NAND_STAT_RDY) {
      if (nand_status & NAND_STAT_FAIL) {
        /* Page program failed */
        EvrFsNFTL_PageProgramStatusErr (ftl->Media->instance, row);
        exec_status = FTL_ERROR_PROGRAM;
      } else {
        /* Page program completed */
        exec_status = FTL_OK;
      }
    }
    else {
      /* Page program timeout expired */
      EvrFsNFTL_PageProgramTimeout (ftl->Media->instance, row);
      exec_status = FTL_ERROR_TIMEOUT;
    }
  }
  else {
    /* Write error */
    EvrFsNFTL_PageProgramFailed (ftl->Media->instance, row);
    /* Driver error */
    exec_status = FTL_ERROR_DRIVER;
  }

  return (exec_status);
}


/**
  NAND media driver EraseBlock wrapper

  \param[in]     row       page row address
  \param[in]     ftl       FTL instance object
*/
static uint32_t Drv_EraseBlock (uint32_t row, NAND_FTL_DEV *ftl) {
  int32_t  rtv;
  uint32_t tick, tout;
  uint32_t exec_status;
  uint8_t  nand_status;

  EvrFsNFTL_BlockErase (ftl->Media->instance, row / ftl->Media->dev->page_count);

  rtv = NAND_MediaDriver.EraseBlock (ftl->Media, row);

  if (rtv == NAND_OK) {
    nand_status = 0;

    /* Wait until NAND ready or timeout expires */
    tout = fs_get_sys_tick_us (NAND_ERASE_TIMEOUT);
    tick = fs_get_sys_tick();
    do {
      NAND_MediaDriver.ReadStatus (ftl->Media, &nand_status);

      if (nand_status & NAND_STAT_RDY) {
        break;
      }
    } while ((fs_get_sys_tick() - tick) < tout);

    if ((nand_status & NAND_STAT_RDY) == 0) {
      NAND_MediaDriver.ReadStatus (ftl->Media, &nand_status);
    }

    EvrFsNFTL_StatusRead (ftl->Media->instance, nand_status);

    if (nand_status & NAND_STAT_RDY) {
      if (nand_status & NAND_STAT_FAIL) {
        /* Block erase failed */
        EvrFsNFTL_BlockEraseStatusErr (ftl->Media->instance, row);
        exec_status = FTL_ERROR_ERASE;
      } else {
        /* Block erased */
        exec_status = FTL_OK;
      }
    }
    else {
      /* Block erase timed out */
      EvrFsNFTL_BlockEraseTimeout (ftl->Media->instance, row);
      exec_status = FTL_ERROR_TIMEOUT;
    }
  }
  else {
    /* Block erase error (driver error) */
    EvrFsNFTL_BlockEraseFailed (ftl->Media->instance, row);
    /* Driver error */
    exec_status = FTL_ERROR_DRIVER;
  }

  return (exec_status);
}


/**
  NAND media driver ResetDevice wrapper

  \param[in]     ftl       FTL instance object
*/
static uint32_t Drv_ResetDevice (NAND_FTL_DEV *ftl) {
  uint32_t tick, tout;
  uint32_t exec_status;
  uint8_t  nand_status;

  EvrFsNFTL_ResetDevice (ftl->Media->instance);

  if (NAND_MediaDriver.ResetDevice (ftl->Media) == NAND_OK) {
    /* Wait until device ready */
    tout = fs_get_sys_tick_us (NAND_RESET_TIMEOUT);
    tick = fs_get_sys_tick();
    do {
      NAND_MediaDriver.ReadStatus (ftl->Media, &nand_status);
      if (nand_status & NAND_STAT_RDY) {
        break;
      }
    } while ((fs_get_sys_tick() - tick) < tout);

    NAND_MediaDriver.ReadStatus (ftl->Media, &nand_status);

    if (nand_status & NAND_STAT_RDY) {
      exec_status = FTL_OK;
    }
    else {
      /* Timeout expired */
      EvrFsNFTL_ResetDeviceTimeout (ftl->Media->instance);
      exec_status = FTL_ERROR_TIMEOUT;
    }
  }
  else {
    /* Media reset failed */
    EvrFsNFTL_ResetDeviceFailed (ftl->Media->instance);
    exec_status = FTL_ERROR_DRIVER;
  }

  return (exec_status);
}


/**
  NAND media driver ReadParamPage wrapper

  \param[in]     ftl       FTL instance object
*/
static uint32_t Drv_ReadParameters (NAND_FTL_DEV *ftl) {
  uint32_t status;
  NAND_PARAM_PAGE *p;

  if (NAND_MediaDriver.ReadParamPage (ftl->Media, 0, ftl->PgBuf, 256) == NAND_OK) {
    /* Examine parameter page values */
    p = (NAND_PARAM_PAGE *)ftl->PgBuf;

    if ((p->signature[0] == 'O') && (p->signature[1] == 'N') &&
        (p->signature[2] == 'F') && (p->signature[3] == 'I')) {
      /* ONFI compliant device, save manufacturer ID */
      ftl->Media->jedec_id =  p->jedec_id;

      /* Remember correctability level */
      ftl->Media->ecc_req = p->ecc_correctability;
    }
    status = FTL_OK;
  }
  else {
    status = FTL_ERROR_DRIVER;
  }

  return (status);
}


/**
  NAND media driver GetFeatures/SetFeatures wrapper

  \param[in]     ftl       FTL instance object
*/
static uint32_t Drv_SetupFeatures (NAND_FTL_DEV *ftl) {
  int32_t  rtv;
  uint32_t status, mode;
  int32_t  addr, val;
  NAND_FEATURE_PARAMETER fparam;

  mode   = 0U;
  addr   = -1;
  status = FTL_OK;

  while (status == FTL_OK) {
    if (mode == 0U) {
      /* Retrieve feature address */
      addr = get_feature_addr (ftl->Media, addr);
    } else {
      /* Retrieve vendor feature address from the application */
      addr = fs_nand_feature_addr (ftl->Media->instance, addr);
    }

    if (addr == -1) {
      if (mode == 0U) {
        /* Set to application mode */
        mode = 1U;
      }
      else {
        break;
      }
    }
    else {
      /* Retrieve features from specified address */
      rtv = NAND_MediaDriver.GetFeatures (ftl->Media, (uint8_t)addr, (uint8_t *)&fparam, 4);

      if (rtv != NAND_OK) {
        status = FTL_ERROR;
      }
      else {
        /* Deliver feature parameters */
        if (mode == 0U) {
          /* To NFTL */
          val = set_feature_param (ftl->Media, addr, &fparam);
        } else {
          /* To application */
          val = fs_nand_feature_param (ftl->Media->instance, addr, &fparam);
        }

        if (val == 0) {
          /* Update feature parameters */
          rtv = NAND_MediaDriver.SetFeatures (ftl->Media, (uint8_t)addr, (uint8_t *)&fparam, 4);

          if (rtv != NAND_OK) {
            status = FTL_ERROR;
          }
        }
      }
    }
  }

  return (status);
}


/**
  Determine ECC configuration and manage page layout

  \param[in]     ftl       FTL instance object
*/
static uint32_t Drv_ManageECC (NAND_FTL_DEV *ftl) {
  const NAND_DEVICE *dev = ftl->Media->dev;
  uint32_t status;

  status = FTL_OK;

  if (dev->sw_ecc == 2) {
    /* EZ NAND on-chip ECC */
    if (dev->ecc != NULL) {
      /* ECC info structure is provided */
      if (NAND_EccToLayout (dev->page_layout, dev->ecc) != NAND_OK) {
        status = FTL_ERROR;
      }
    }
    else {
      status = FTL_ERROR;
    }
  }
  else {
    if (dev->sw_ecc == 3) {
      /* Hardware ECC, take driver provided ECC info structure */
      if (NAND_SetupLayout (ftl->Media) != NAND_OK) {
        status = FTL_ERROR;
      }
    }
  }

  /* Provide layout override support */
  fs_nand_setup_layout (ftl->Media->instance, &ftl->PgLay);

  EvrFsNFTL_SetupPageLayout  (ftl->Media->instance, ftl->PgLay.sector_inc,    ftl->PgLay.spare_ofs,     ftl->PgLay.spare_inc);
  EvrFsNFTL_SetupSpareLayout (ftl->Media->instance, ftl->PgLay.spare.ofs_lsn, ftl->PgLay.spare.ofs_dcm, ftl->PgLay.spare.ofs_bbm, ftl->PgLay.spare.ofs_ecc);

  return (status);
}


/* ------------------------- NFTL functions --------------------------------- */


/**
  Get logical sector number and block type from buffer

  \param[in]     *buf       data buffer
  \param[out]    *blType    block type
  \return logical sector number
*/
static uint32_t GetLSN(uint8_t *buf, uint32_t *blType) {
  uint32_t lsn;

  lsn  = (uint32_t)buf[0];
  lsn |= (uint32_t)buf[1] << 8;
  lsn |= (uint32_t)buf[2] << 16;
  lsn |= (uint32_t)buf[3] << 24;

  if(blType != NULL) {
    *blType = (lsn & BIT_PRIM) ? TYP_PRIM : TYP_REPL;
  }
  return (lsn & 0x7FFFFFFF);
}

/**
  Set logical sector number and block type to output buffer

  \param[in]      lsn       logical sector number
  \param[in]      blType    block type
  \param[out]     buf       output buffer
*/
static void SetLSN(uint32_t lsn, uint32_t blType, uint8_t *buf) {

  (blType == TYP_PRIM) ? (lsn |= BIT_PRIM) : (lsn &= ~BIT_PRIM);

  buf[0] = (uint8_t)(lsn);
  buf[1] = (uint8_t)(lsn >> 8);
  buf[2] = (uint8_t)(lsn >> 16);
  buf[3] = (uint8_t)(lsn >> 24);
}

/**
  Calc n = Log2(x) if x == 2^n

  \param[in]      x         number to the power of two
  \return n or INVALID for x == 0
*/
static uint32_t Log2(uint32_t x) {
  uint32_t n;

  if (x == 0) return INVALID;

  for (n = 31; n != 0; n--) {
    if (x & (1UL << n)) break;
  }
  return (n);
}

/**
  Select new slot from page cache array

  \param[in,out]  Ca        indexing cache control block
  \return slot number
*/
__STATIC_FORCEINLINE uint32_t GetPageCacheSlot (NAND_FTL_CACHE *Ca) {
  uint32_t slot;

  /* Select next slot */
  slot = Ca->CachePS++;
  if (Ca->CachePS == Ca->CachedPages) { Ca->CachePS = 0; }
  return slot;
}

/**
  Cached page read

  \param[in,out]  ftl       FTL instance object
  \param[in]      row       row address of a page
  \param[in]      col       column address of a page
  \param[in]      sz        size of data to read
  \return execution status FTL_STATUS
*/
static uint32_t CachePgRead (NAND_FTL_DEV *ftl, uint32_t row, uint32_t col, uint32_t sz) {
  uint32_t slot, rtv;
  uint32_t usecache = 0;

  EvrFsNFTL_CacheRead (ftl->Media->instance, row / ftl->Media->dev->page_count, row % ftl->Media->dev->page_count, col);

  rtv = FTL_OK;

  for (slot = 0; slot < ftl->Ca.CachedPages; slot++) {
    if ((ftl->Ca.Page[slot].row & ~BIT_UNCOR) == row) {
      usecache = 1;

      if (ftl->Ca.Page[slot].row & BIT_UNCOR) {
        /* Return ECC Error */
        rtv = FTL_ERROR_ECC;
      }
      break;
    }
  }

  /* Read from flash to cache if row isn't cached */
  if (!usecache) {
    slot = GetPageCacheSlot (&ftl->Ca);
    ftl->Ca.Page[slot].row = row;

    rtv = Drv_ReadPage (row, ftl->Ca.Page[slot].buf, ftl);

    if (rtv == FTL_OK) {
      if (pDev->sw_ecc == 1) {
        /* Hamming ECC is enabled, decode ECC */
        rtv = DecodeECC(ftl, ftl->Ca.Page[slot].buf);
      }
    }

    if (rtv == FTL_ERROR_ECC_FAIL || rtv == FTL_ERROR_ECC_COR) {
      if (rtv == FTL_ERROR_ECC_FAIL) {
        /* ECC could't correct the data - set the flag */
        ftl->Ca.Page[slot].row |= BIT_UNCOR;
      }
      #ifdef FS_DEBUG
      if (rtv == FTL_ERROR_ECC_COR) {
        /* ECC error corrected */
        EvrFsNFTL_BitErrorCorrected (ftl->Media->instance, row);
      }
      #endif
      /* Return ECC Error */
      rtv = FTL_ERROR_ECC;
    }
  }
  memcpy (ftl->PgBuf, &ftl->Ca.Page[slot].buf[col], sz);

  /* Set ECC Status */
  ftl->LastECC = ECC_NOERR;
  if (ftl->Ca.Page[slot].row & BIT_UNCOR) {
    ftl->LastECC = ECC_UNCORRECTED;
  }

  return rtv;
}

/**
  Cached page write

  \param[in,out]  ftl       FTL instance object
  \param[in]      row       row address of a page
  \return execution status FTL_STATUS
*/
static uint32_t CachePgWrite (NAND_FTL_DEV *ftl, uint32_t row) {
  uint32_t slot;
  uint32_t newslot = 1;

  EvrFsNFTL_CacheWrite (ftl->Media->instance, row / ftl->Media->dev->page_count, row % ftl->Media->dev->page_count);

  for (slot = 0; slot < ftl->Ca.CachedPages; slot++) {
    if (ftl->Ca.Page[slot].row == row) {
      newslot = 0;
      break;
    }
  }
  if (newslot) {
    slot = GetPageCacheSlot (&ftl->Ca);
    ftl->Ca.Page[slot].row = row;
  }

  if (pDev->sw_ecc == 1) {
    /* Hamming ECC is enabled, encode ECC */
    EncodeECC (ftl, ftl->PgBuf);
  }
  memcpy (ftl->Ca.Page[slot].buf, ftl->PgBuf, pDev->page_size);

  return (Drv_WritePage (row, ftl->Ca.Page[slot].buf, ftl));
}


/**
  Flush cached page from cache

  \param[in,out]  Ca        indexing cache control block
  \param[in]      row       page row address
*/
static void FlushPgCache (NAND_FTL_CACHE *Ca, uint32_t row) {
  uint32_t slot;

  for (slot = 0; slot < Ca->CachedPages; slot++) {
    if (Ca->Page[slot].row == row) {
      Ca->Page[slot].row = INVALID;
      break;
    }
  }
}

/**
  Remove cached block from cache

  \param[in,out]  Ca        indexing cache control block
  \param[in]      pbn       physical block number
*/
static void FlushIdxCache(NAND_FTL_CACHE *Ca, uint16_t pbn) {
  uint32_t slot;

  /* Check all slots */
  for (slot = 0; slot < Ca->CachedBlocks; slot++) {
    /* If physical block number match, invalidate entry */
    if (Ca->Block[slot].pbn == pbn) {
      Ca->Block[slot].pbn    = INVALID_BLOCK;
      Ca->Block[slot].nextPg = 0;
      break;
    }
  }
}

/**
  Select new slot from index cache array

  \param[in]      Ca        indexing cache control block
  \return slot number
*/
static uint32_t GetIdxCacheSlot(NAND_FTL_CACHE *Ca) {
  uint32_t slot;

  /* Select next slot */
  slot = Ca->CacheBS++;

  if (Ca->CacheBS == Ca->CachedBlocks) {
    Ca->CacheBS = 0;
  }

  return slot;
}

/**
  Counts number of blocks in queue

  \param[in]     *PbnQ      block queue
  \return number of blocks in queue
*/
static uint32_t NumBlocksInQueue(uint16_t *PbnQ) {
  uint32_t i, cnt;

  cnt = 0;

  for (i = 0; i < 3; i++) {
    if (PbnQ[i] != 0) {
      cnt++;
    }
  }

  return cnt;
}


/**
  Writes physical block number into block allocation queue

  \param[in,out] *PbnQ      block queue
  \param[in]      pbn       physical block number
  \return number of block in queue or INVALID if queue full
*/
static uint32_t PutBlockInQueue (uint16_t *PbnQ, uint32_t pbn) {
  uint16_t bn = (uint16_t)pbn;
  uint32_t num = 0;

  if (PbnQ[0] == 0) {
    PbnQ[0] = bn;
    num = 1;
  }
  else if (PbnQ[1] == 0  && PbnQ[0] != bn) {
    PbnQ[1] = bn;
    num = 2;
  }
  else if (PbnQ[2] == 0 && PbnQ[1] != bn && PbnQ[0] != bn) {
    PbnQ[2] = bn;
    num = 3;
  }
  else num = INVALID;
  return num;
}

/**
  Selects physical block number of an empty block from allocation queue

  \param[in,out] *PbnQ      block queue
  \param[out]    *pbn       pointer where physical block number will be stored
  \param[in]      restriction allocation restriction
  \return execution status FTL_STATUS
*/
static uint32_t GetBlockFromQueue (uint16_t *PbnQ, uint16_t *pbn, uint32_t restriction) {
  uint32_t cnt;

  /* Restriction: ALLOC_REST: 1, ALLOC_PENUL: 2, ALLOC_LAST: 3 */
  cnt = NumBlocksInQueue(PbnQ);

  if (cnt < restriction) {
    return FTL_ERROR_NOT_FOUND;
  }

  *pbn = PbnQ[0];

  PbnQ[0] = PbnQ[1];
  PbnQ[1] = PbnQ[2];
  PbnQ[2] = 0;

  return FTL_OK;
}


/**
  Erase block and remove it from cache. When block erase fails,
  block is marked as bad.

  \param[in,out]  ftl       FTL instance object
  \param[in,out] *bn        pointer to physical block to be erased
  \return execution status FTL_STATUS
*/
static uint32_t EraseBlock (NAND_FTL_DEV *ftl, uint16_t *pbn) {
  uint32_t rtv, row;

  /* Flush cache */
  FlushIdxCache (&ftl->Ca, *pbn);

  row = ROW(*pbn, 0);

  /* Erase block */
  rtv = Drv_EraseBlock (row, ftl);

  if (rtv != FTL_OK) {
    if (rtv == FTL_ERROR_ERASE) {
      rtv = MarkBlockBad (ftl, *pbn, false);
      *pbn = INVALID_BLOCK;
    }
  }
  return rtv;
}


/**
  Scan translation table and find a block which contains translation info
  for given logical block number (lbn) and block type (blTyp). Physical
  location of containing block is set to *pbn if block is found, otherwise
  *pbn will be invalidated.
  Blocks marked as bad are not scanned.

  \param[in,out]  ftl       FTL instance object
  \param[in]      lbn       logical block number
  \param[in]      blTyp     block type
  \param[out]    *pbn       pointer where physical block number is stored
  \returns execution status FTL_STATUS
*/
static uint32_t ScanTable (NAND_FTL_DEV *ftl, uint32_t lbn, uint32_t blTyp, uint16_t *pbn) {
  uint16_t block;
  uint32_t lsn, type, err, rtv;

  rtv = FTL_OK;
  err = 0;

  for (block = ftl->Cfg->BttStartBn; block <= ftl->Cfg->BttEndBn; block++) {
    /* Read first page */
    rtv = CachePgRead (ftl, ROW(block, 0), ftl->PgLay.spare_ofs, __SZ_SP_USED);

    if (rtv == FTL_OK) {
      err = 0;
      /* Skip bad block */
      if (ftl->PgBuf[ftl->PgLay.spare.ofs_bbm] == BB_MASK) {

        lsn = GetLSN (&ftl->PgBuf[ftl->PgLay.spare.ofs_lsn], &type);

        /* Continue if free block or LBN searched != LBN found */
        if (lsn != EMPTY && blTyp == type && lbn == LBN(lsn)) {
          /* Block found */
          *pbn = block;

          return FTL_OK;
        }
      }
      #ifdef FS_DEBUG
      else {
        EvrFsNFTL_SkipBadBlock (ftl->Media->instance, block);
      }
      #endif
    }
    else {
      err = 1;
      if (rtv == FTL_ERROR_ECC) {
        if (ftl->PgBuf[ftl->PgLay.spare.ofs_bbm] == BB_MASK) {
          /* Relocate block on ECC error */
          rtv = RelocBlock (ftl, &block, pDev->page_count, AREA_TBL, true);
          if (rtv == FTL_OK) {
            block = ftl->Cfg->BttStartBn;
          }
        }
      }
      else break;
    }
  }
  *pbn = INVALID_BLOCK;
  if (err) {
    return rtv;
  }
  return FTL_ERROR_NOT_FOUND;
}


/**
  Mark block specified by pbn as bad. If erase argument is set to true, block is first
  erased or not erased if set to false.
  All spare area bytes of the first sector in the first page are invalidated
  (set to 0). In case if write to the first page is unsuccessful, then spare area
  of the last page is invalidated (set to 0).

  \param[in,out]  ftl       FTL instance object
  \param[in]      pbn       physical block number
  \param[in]      erase     erase flag
  \returns execution status FTL_STATUS
*/
static uint32_t MarkBlockBad (NAND_FTL_DEV *ftl, uint16_t pbn, uint32_t erase) {
  uint32_t row, rtv;

  /* Marking block as bad */
  EvrFsNFTL_SetBadBlockMarker (ftl->Media->instance, pbn);

  row = ROW(pbn, 0);

  if (erase) {
    rtv = Drv_EraseBlock (row, ftl);
    if (rtv != FTL_OK && rtv != FTL_ERROR_ERASE) {
      /* Driver error */
      return rtv;
    }
  }

  /* Invalidate first spare area in page */
  memset (ftl->PgBuf, 0xFF, pDev->page_size);
  memset (&ftl->PgBuf[ftl->PgLay.spare_ofs], 0x00, __SZ_SPARE);

  ftl->BadBlockCnt++;

  /* Try to mark first page */
  rtv = Drv_WritePage (row, ftl->PgBuf, ftl);

  if (rtv == FTL_ERROR_PROGRAM) {
    /* If write to first page unsuccessful, try last page in block */
    row = ROW(pbn, pDev->page_count - 1);
    rtv = Drv_WritePage (row, ftl->PgBuf, ftl);
    if (rtv == FTL_ERROR_PROGRAM) { rtv = FTL_OK; }
  }

  return (rtv);
}

/**
  Moves block content from src block to the dest block.

  Number of pages that will be moved from src to dest block is specified by
  argument cnt. Move will always begin at page 0. If src blocks resides in
  table area, then translation table will be also updated

  \param[in,out]  ftl       FTL instance object
  \param[in]      src       physical block number of a source block
  \param[in]      dest      physical block number of a destination block
  \param[in]      cnt       number of pages to move
  \param[in]      area      source and destination block area
  \return execution status FTL_STATUS
*/
static uint32_t MoveBlock(NAND_FTL_DEV *ftl, uint16_t src, uint16_t dest, uint32_t cnt, uint32_t area) {
  uint32_t page, empty, eccWarn;
  uint32_t tsn, tsnIdx;
  uint32_t oldRow, newRow;
  uint32_t i, j, si, rtv;

  /* Moving block content from src to dest */
  EvrFsNFTL_MoveData (ftl->Media->instance, src, dest);

  tsnIdx = 0;
  rtv = FTL_OK;

  /* Move data from src (old) to dest (new) block */
  for (page = 0; page < cnt; page++) {
    eccWarn = false;

    oldRow = ROW(src, page);
    rtv = CachePgRead (ftl, oldRow, 0, pDev->page_size);
    switch (rtv) {
      case FTL_OK:
        break;

      case FTL_ERROR_ECC:
        /* Check ECC Status */
        if (ftl->LastECC & ECC_UNCORRECTED) {
          eccWarn = true;
        }
        break;

      default:
        return rtv;
    }

    /* Check first spare in page, to see if empty page reached */
    empty = false;

    si = ftl->PgLay.spare_ofs;

    if (ftl->PgBuf[si + ftl->PgLay.spare.ofs_bbm] == 0xFF) {
      si += ftl->PgLay.spare.ofs_lsn;

      if ((ftl->PgBuf[si]   == 0xFF) && (ftl->PgBuf[si+1] == 0xFF) &&
          (ftl->PgBuf[si+2] == 0xFF) && (ftl->PgBuf[si+3] == 0xFF)) {
        empty = true;
      }
    }

    /* Stop copying if current page empty */
    if (empty) {
      rtv = FTL_OK;
      break;
    }

    /* Set corruption marker in page spare areas */
    if (eccWarn) {
      for (i = ftl->PageSectors, si = ftl->PgLay.spare_ofs;
           i;
           i--,                  si += ftl->PgLay.spare_inc) {
        ftl->PgBuf[si + ftl->PgLay.spare.ofs_dcm] = 0x00;
      }
    }

    if(page == 0 && area == AREA_TBL) {
      tsn = GetLSN (&ftl->PgBuf[ftl->PgLay.spare_ofs + ftl->PgLay.spare.ofs_lsn], NULL);
      /* Align tsnIdx to the start of block */
      tsnIdx = (uint32_t)(LBN(tsn) << ftl->PPB);
    }

    newRow = ROW(dest, page);

    rtv = CachePgWrite (ftl, newRow);
    if (rtv != FTL_OK) { return rtv; }
  }

  if (area == AREA_TBL) {
    j = ftl->TsnTableSize;
    if (ftl->TsnTableSize > pDev->page_count) {
      j = tsnIdx + pDev->page_count;
    }

    for (page = 0; page < cnt; page++) {
      oldRow = ROW(src, page);

      for (i = tsnIdx; i < j; i++) {
        if (oldRow == (ftl->TsnTable[i] & 0x00FFFFFF)) {
          newRow = ROW(dest, page);
          ftl->TsnTable[i] = (ftl->TsnTable[i] & 0xFF000000) | newRow;
        }
      }
    }
  }

  return rtv;
}

/**
  Relocate block

  Function allocates new block and moves pages from given *pbn to
  allocated block. All pages from page 0 until page number given with
  untilPg variable are moved. Proper area must be specified so that
  translation table is updated when needed.
  If error appear when pages are moved, function retries until number of
  retries is exceeded.
  If pages are successfully relocated, physical block number of a new
  block is written to the *pbn variable.

  \param[in,out]  ftl       FTL instance object
  \param[in,out] *pbn       physical block number
  \param[in]      endPg     page number where copying stops
  \param[in]      area      block area
  \return execution status FTL_STATUS
*/
static uint32_t RelocBlock(NAND_FTL_DEV *ftl, uint16_t *pbn, uint32_t endPg, uint32_t area, uint32_t eraseBlock) {
  uint16_t allocBN;
  uint32_t rtv, retry;

  EvrFsNFTL_RelocateBlock (ftl->Media->instance, *pbn);

  retry = 0;
  do {

    /* Allocate new block */
    rtv = AllocBlock (ftl, &allocBN, ALLOC_REST, area);
    if (rtv == FTL_OK) {
      /* Move data */
      rtv = MoveBlock (ftl, *pbn, allocBN, endPg, area);

      if (rtv == FTL_OK) {
        /* Erase Block */
        if (eraseBlock) {
          rtv = EraseBlock(ftl, pbn);
          if (rtv != FTL_OK) {
            return rtv;
          }
        }

        /* Relocation successful */
        *pbn = allocBN;
        return FTL_OK;
      }
      else if (rtv == FTL_ERROR_PROGRAM) {
        rtv = MarkBlockBad (ftl, allocBN, true);
        if (rtv == FTL_OK) {
          /* PREVENT function returning with NO ERROR code */
          retry--;
        }
      }
      else break;
    }
    else {
      return rtv;
    }

    retry++;
  }
  while (retry < __MAX_RETRY);

  /* Failed to relocate block */
  EvrFsNFTL_RelocateBlockFailed (ftl->Media->instance, *pbn);

  return rtv;
}

/**
  Refresh data block by moving block data to new block

  \param[in,out]  ftl       FTL instance object
  \param[in]      lbn       logical block number
  \param[in]      blTyp     block type
  \param[in]      btti      translation table entry
  \param[in]      numPages  number of pages to move
  \return execution status FTL_STATUS
*/
static uint32_t RefreshDataBlock(NAND_FTL_DEV *ftl, uint16_t lbn, uint32_t blTyp, BTT_ITEM *btti, uint32_t numPages) {
  uint16_t bn, tmp;
  uint32_t rtv;

  bn = (blTyp == TYP_PRIM) ? btti->primBN : btti->replBN;

  tmp = bn;

  rtv = RelocBlock (ftl, &bn, numPages, AREA_DAT, false);
  if (rtv != FTL_OK) { return rtv; }

  (blTyp == TYP_PRIM) ? (btti->primBN = bn) : (btti->replBN = bn);

  rtv = UpdateBTT (ftl, lbn, &btti->primBN, &btti->replBN);
  if (rtv != FTL_OK) { return rtv; }

  rtv = EraseBlock(ftl, &tmp);
  if (rtv != FTL_OK) { return rtv; }

  return FTL_OK;
}

/**
  Function finds a page with specified lsn (logical sector number) within
  block with given bn (physical block number). If specified lsn equals
  to EMPTY, function searches for first free page within block.
  If page or free page found, then it's (page) number and block type are
  written to *pageNum variable.
  If sector isn't found then function returns appropriate error code
  and *pageNum variable is set to the first free page if found. If free
  page isn't found, *pageNum is set to number of pages in block.

  Search begins from first page in block and continues till the last page
  or until given lsn is found. Search function behaves differently,
  depending of the block type we are searching in. If block is primary
  and given lsn was found in it, then search immediately breaks as
  successful. In case of replacement block, search always continues till
  the end of block and reports search as unsuccessful if given lsn was
  not found or successful when at least one appearance of lsn was found
  in block. Because multiple sectors with same lsn can exist in
  replacement block, only the last valid location is returned.

  First spare area of the first page in block is also always checked for
  bad block marker and if marker set, function returns immediately.

  \param[in,out]  ftl       FTL instance object
  \param[in]      pbn       physical block number
  \param[in]      lsn       logical sector number
  \param[out]     pgNum     pointer where page number with specified lsn is stored
  \return execution status FTL_STATUS
*/
static uint32_t ScanBlock (NAND_FTL_DEV *ftl, uint16_t pbn, uint32_t lsn, uint32_t *pgNum) {
  uint32_t cLsn, cPg, rePg, type, empty, lbn, slot, rtv;
  uint32_t caRead, caWrite;

  /* Searching for 'lsn' within block 'pbn' */
  EvrFsNFTL_LsnFind (ftl->Media->instance, lsn, pbn);

  *pgNum = INVALID;

  if (pbn > pDev->block_count) {
    /* PBN is out of range (max=pDev->block_count) */
    EvrFsNFTL_PbnOutOfRange (ftl->Media->instance, pbn, pDev->block_count);
    return FTL_ERROR_RANGE;
  }

  /* Align LSN to first sector in page */
  if (lsn != EMPTY) {
    lsn &= ~(ftl->PageSectors - 1);       /* 0, SPP,..., n + SPP,..., SPB */
  }

  /* Scan cached blocks */
  caWrite = 0;
  caRead  = 0;
  slot    = 0;
  lbn     = 0;
  type    = TYP_PRIM;

  if (pbn > ftl->Cfg->BttEndBn) {
    /* Check if block already in cache */
    for (; slot < ftl->Ca.CachedBlocks; slot++) {
      if (ftl->Ca.Block[slot].pbn == pbn) {
        lbn  = ftl->Ca.Block[slot].lbn;
        type = ftl->Ca.Block[slot].typ;
        caRead = 1;
        break;
      }
    }
    /* If we are not reading from cache, we will write */
    if (caRead == 0) {
      /* Select slot to write into */
      slot = GetIdxCacheSlot(&ftl->Ca);

      ftl->Ca.Block[slot].pbn = pbn;
      ftl->Ca.Block[slot].nextPg = 0;
      caWrite = 1;
    }
  }

  /* Scan through pages */
  rePg   = INVALID;
  empty  = 0;

  for (cPg = 0; cPg < pDev->page_count; cPg++) {

    if (caRead && cPg == ftl->Ca.Block[slot].nextPg) {
      caRead  = 0;
      caWrite = 1;
    }

    if (caRead) {
      cLsn = IDX2LSN(lbn, ftl->Ca.Block[slot].pgIdx[cPg]);
    }
    else {
      rtv = CachePgRead (ftl, ROW(pbn, cPg), ftl->PgLay.spare_ofs, __SZ_SP_USED);
      if (rtv != FTL_OK) {
        return rtv;
      }

      /* Get lsn */
      cLsn = GetLSN (&ftl->PgBuf[ftl->PgLay.spare.ofs_lsn], &type);

      /* Cache write */
      if (caWrite) {
        if (cLsn != EMPTY) {
          ftl->Ca.Block[slot].lbn = LBN(cLsn);
          ftl->Ca.Block[slot].typ = (uint8_t)type;
          ftl->Ca.Block[slot].pgIdx[cPg] = LSN2IDX(cLsn);
          ftl->Ca.Block[slot].nextPg++;
        }
      }
    }

    if (cPg == 0 && !caRead) {
      /* Check for bad block */
      if (ftl->PgBuf[ftl->PgLay.spare.ofs_bbm] != BB_MASK) {
        /* This is bad block */
        return FTL_ERROR_BB;
      }

      /* Check if logical sector member of our block */
      if (lsn != EMPTY && cLsn != EMPTY) {
        if (LBN(lsn) != LBN(cLsn) && ftl->PgBuf[ftl->PgLay.spare.ofs_dcm] == 0xFF) {
          /* Misplaced sector, got cLsn instead of lsn in block bn */
          return FTL_ERROR_LBN;
        }
      }
    }

    *pgNum = cPg;

    /* Check if lsn found */
    if (lsn == cLsn) {
      if ((type & TYP_PRIM) || (lsn == EMPTY)) {
        /* LSN found in PRIM, cPG */
        EvrFsNFTL_LsnFound (ftl->Media->instance, lsn, pbn, cPg);
        return FTL_OK;
      }
      else {
        rePg  = cPg;
      }
    }
    else if (cLsn == EMPTY) {
      /* It's free page */
      empty = 1;
      break;
    }
  }

  if (rePg != INVALID) {
    *pgNum = rePg;
    /* LFN found in REPL, rePg */
    EvrFsNFTL_LsnFound (ftl->Media->instance, lsn, pbn, rePg);
    return FTL_OK;
  }

  /* Last page reached and no free page */
  if ((empty == 0) && (cPg == pDev->page_count)) {
    /* End of block */
    EvrFsNFTL_LsnNotFound (ftl->Media->instance, lsn, pbn);
    return FTL_ERROR_NOT_FOUND_EOB;
  }

  /* Free page reached */
  EvrFsNFTL_LsnNotFound (ftl->Media->instance, lsn, pbn);
  return FTL_ERROR_NOT_FOUND;
}

/**
  Find empty block within specified area

  \param[in,out]  ftl       FTL instance object
  \param[in]      area      block area
  \param[out]    *bn        block number of the empty block
  \return execution status FTL_STATUS
*/
static uint32_t FindEmptyBlock (NAND_FTL_DEV *ftl, uint32_t area, uint16_t *bn) {
  uint16_t startBN, endBN, cBN, lastBN;
  uint32_t empty;
  uint32_t i, rtv;
  uint32_t row, col;

  if (area == AREA_DAT) {
    /* Search in data area */
    startBN = ftl->Cfg->DataStartBn;
    endBN   = ftl->Cfg->DataEndBn;
    lastBN  = ftl->LastDBN;
  } else {
    /* Search in table area */
    startBN = ftl->Cfg->BttStartBn;
    endBN   = ftl->Cfg->BttEndBn;
    lastBN  = ftl->LastTBN;
  }

  /* Search for empty block within area */
  empty = false;
  cBN   = lastBN;
  do {
    (cBN == endBN) ? (cBN = startBN) : (cBN++);

    row = ROW (cBN, 0);
    col = ftl->PgLay.spare_ofs;

    rtv = CachePgRead (ftl, row, col, __SZ_SP_USED);
    if (rtv != FTL_OK) {
      if (rtv != FTL_ERROR_ECC) {
        return rtv;
      }
      /* Skip block if ECC error */
    }
    else {
      /* If block not bad, check if it is empty */
      if (ftl->PgBuf[ftl->PgLay.spare.ofs_bbm] == BB_MASK) {

        empty = true;

        /* Only check LSN bytes */
        for  (i = 0; i < 4; i++) {
          if (ftl->PgBuf[ftl->PgLay.spare.ofs_lsn + i] != 0xFF) {
              empty = false;
              break;
          }
        }
      }
      #ifdef FS_DEBUG
      else {
        EvrFsNFTL_SkipBadBlock (ftl->Media->instance, cBN);
      }
      #endif
    }
  }
  while (!empty && (cBN != lastBN));

  if (empty) {
    *bn = cBN;
    (area == AREA_DAT) ? (ftl->LastDBN = (uint16_t)cBN) : (ftl->LastTBN = (uint16_t)cBN);
    return FTL_OK;
  }
  return FTL_ERROR_NOT_FOUND;
}

/**
  Function searches for logical block in data area with allocated
  PRIM/REPL pair. When pair of blocks is found, garbage collection is
  performed on them and data are copied to specified allocBN. Block
  translation table is then updated and two free blocks are gained.
  Their address is then written to the given *firstBn and *secondBn
  variable addresses.

  \param[in,out]  ftl       FTL instance object
  \return execution status FTL_STATUS
*/
static uint32_t ForceDataGc(NAND_FTL_DEV *ftl) {
  BTT_ITEM btti;
  uint16_t lbn, pbn, emptyBN;
  uint32_t row, prev_row, col, tsn, tsnIdx, rtv;

  /* Find empty block */
  rtv = GetBlockFromQueue (ftl->PbnQ, &emptyBN, ALLOC_LAST);
  if (rtv == FTL_ERROR_NOT_FOUND) {
    rtv = FindEmptyBlock (ftl, AREA_DAT, &emptyBN);
    if (rtv != FTL_OK) { return rtv; }
  }

  prev_row = 0;

  lbn = ftl->GcLBN;
  /* Look for PRIM/REPL pair on all logical blocks */
  do {
    tsn = lbn >> BTT_EPS;
    tsnIdx = tsn >> ftl->SPP;

    row = ftl->TsnTable[tsnIdx] & 0x00FFFFFF;

    if (row != 0x00FFFFFF) {

      if (row != prev_row) {

        rtv = CachePgRead (ftl, row, 0, pDev->page_size);

        switch (rtv) {
          case FTL_OK:
            /* Lock page read until new row (page) */
            prev_row = row;
            break;

          case FTL_ERROR_ECC:
            pbn = PBN(row);
            rtv = RelocBlock (ftl, &pbn, pDev->page_count, AREA_TBL, true);

            if (rtv != FTL_OK) {
              return rtv;
            }
            continue;

          default:
            return rtv;
        }
      }

      /* Align column to block entry */
      col = ((tsn & (ftl->PageSectors - 1)) * ftl->PgLay.sector_inc) +
             (lbn & ((1 << BTT_EPS) - 1)) * BTT_ENTRY_SZ;

      btti.primBN  = ftl->PgBuf[col];
      btti.primBN |= ftl->PgBuf[col + 1] << 8;

      btti.replBN  = ftl->PgBuf[col + 2];
      btti.replBN |= ftl->PgBuf[col + 3] << 8;

      if (btti.primBN != INVALID_BLOCK && btti.replBN != INVALID_BLOCK) {
        if (lbn != ftl->CurrLBN) {
          /* Start garbage collection */
          rtv = GcRun (ftl, lbn, &btti, AREA_DAT, emptyBN);

          if (rtv == FTL_OK) {

            /* GC ok, now update translation table */
            pbn = INVALID_BLOCK;
            rtv = UpdateBTT(ftl, lbn, &emptyBN, &pbn);

            if (rtv != FTL_OK) {
              return rtv;
            }

            emptyBN = INVALID_BLOCK;

            /* Erase old blocks */
            rtv = EraseBlock (ftl, &btti.primBN);
            if (rtv != FTL_OK) { return rtv; }
            rtv = EraseBlock (ftl, &btti.replBN);
            if (rtv != FTL_OK) { return rtv; }

            /* Put blocks in queue if erase was ok */
            if (btti.primBN != INVALID_BLOCK) PutBlockInQueue(ftl->PbnQ, btti.primBN);
            if (btti.replBN != INVALID_BLOCK) PutBlockInQueue(ftl->PbnQ, btti.replBN);
          }
          else {
            return rtv;
          }

          if (emptyBN == INVALID_BLOCK) {
            /* Empty block was used, set LBN for next GC and return */
            ftl->GcLBN = lbn + 1;
            if (ftl->GcLBN == ftl->NumDataBlocks) { ftl->GcLBN = 0; }
            return FTL_OK;
          }
        }
      }
    }
    lbn++;
    if (lbn == ftl->NumDataBlocks) { lbn = 0; }
  }
  while (lbn != ftl->GcLBN);
  /* Nothing was done or found */

  /* Put empty block back in queue */
  PutBlockInQueue (ftl->PbnQ, emptyBN);

  return FTL_ERROR_NOT_FOUND;
}


/**
  Allocate a free block

  \param[in,out]  ftl       FTL instance object
  \param[in,out] *bn        pointer to block number variable
  \param[in]      restriction allocation restriction
  \param[in]      area      area specifier
  \return execution status FTL_STATUS
*/
static uint32_t AllocBlock(NAND_FTL_DEV *ftl, uint16_t *bn, uint32_t restriction, uint32_t area) {
  uint16_t pbn;
  uint32_t num, rtv = FTL_OK;

  if (area == AREA_TBL) {
    /* Allocating table block */
    EvrFsNFTL_AllocateBlock (ftl->Media->instance, 0);

    rtv = FindEmptyBlock (ftl, AREA_TBL, bn);
    if (rtv == FTL_ERROR_NOT_FOUND) {
      rtv = FTL_ERROR_ALLOC;
    }
  }
  else {
    /* Allocating data block */
    EvrFsNFTL_AllocateBlock (ftl->Media->instance, 1);

    num = 0;

    while (num < __MIN_IN_QUEUE && num != INVALID) {
      rtv = FindEmptyBlock (ftl, AREA_DAT, &pbn);
      if (rtv != FTL_OK) {
        if (rtv == FTL_ERROR_NOT_FOUND) break;
        else return rtv;
      }
      else {
        num = PutBlockInQueue (ftl->PbnQ, pbn);
      }
    }

  do {
    num = NumBlocksInQueue(ftl->PbnQ);
    if (num == 0) return FTL_ERROR_ALLOC;

    if (num < __MIN_IN_QUEUE) {
      rtv = ForceDataGc(ftl);
      switch (rtv) {
        case FTL_OK:
        case FTL_ERROR_NOT_FOUND:
        case FTL_ERROR_PROGRAM:
          break;

        default:
          return rtv;
      }
    }
  }
  while (rtv != FTL_ERROR_NOT_FOUND && num < __MIN_IN_QUEUE);

    rtv = GetBlockFromQueue(ftl->PbnQ, &pbn, restriction);
    if (rtv == FTL_OK) {
      *bn = pbn;
    }
  }
  #ifdef FS_DEBUG
  if (rtv == FTL_OK) {
    EvrFsNFTL_AllocatedBlock (ftl->Media->instance, *bn);
  }
  #endif

  return rtv;
}

/**
  Searches for physical block addresses of a given logical block number.
  Physical adresses are written to the *btt structure. If block does
  not exist then values in *btt structure are set to INVALID_BLOCK.

  Row addresses of all valid pages in block translation table area are
  gathered in nftl_TsnTable array. Array index is first determined from
  given logical block number and row address is selected from
  nftl_TsnTable. Page at that row address can then be read and physical
  block addresses can be extracted from column defined by logical block
  number.
  In case of ECC error, block addresses are written in *btt then table
  block is relocated.

  \param[in,out]  ftl       FTL instance object
  \param[in]      lbn       logical block number
  \parma[in]     *btt       pointer to entry structure
  \return execution status FTL_STATUS
*/
static uint32_t SearchBTT(NAND_FTL_DEV *ftl, uint32_t lbn, BTT_ITEM *btt) {
  uint32_t eccErr;
  uint32_t rtv, tsn, tsnIdx, row, col;
  uint16_t pbn;

  /* Retrieving translation info */
  EvrFsNFTL_TableLookup (ftl->Media->instance, lbn);

  /* Check if given LSN in range for our flash device */
  if (lbn < ftl->NumDataBlocks) {

    tsn = lbn >> BTT_EPS;

    /* Find entry in ram table */
    tsnIdx = tsn >> ftl->SPP;
    row    = ftl->TsnTable[tsnIdx] & 0x00FFFFFF;

    if (row == 0x00FFFFFF) {
      /* No entry */
      btt->primBN = INVALID_BLOCK;
      btt->replBN = INVALID_BLOCK;
      rtv = FTL_OK;

      /* No translation info for given LBN */
      EvrFsNFTL_TableEntryNotFound (ftl->Media->instance, lbn);
    }
    else {
      /* Align column to sector within page and add entry offset */
      col = ((tsn & (ftl->PageSectors - 1)) * ftl->PgLay.sector_inc) +
             (lbn & ((1 << BTT_EPS) - 1)) * BTT_ENTRY_SZ;

      eccErr = false;

      /* Read entry */
      rtv = CachePgRead (ftl, row, col, BTT_ENTRY_SZ);
      
      if ((rtv == FTL_OK) || (rtv == FTL_ERROR_ECC)) {
        if (rtv == FTL_ERROR_ECC) {
          eccErr = true;
        }

        /* Read entry */
        btt->primBN = (uint16_t)((ftl->PgBuf[1] << 8) | ftl->PgBuf[0]);
        btt->replBN = (uint16_t)((ftl->PgBuf[3] << 8) | ftl->PgBuf[2]);

        /* Found [%d, %d] translation entry for LBN %d */
        EvrFsNFTL_LbnToPbn (ftl->Media->instance, lbn, btt->primBN, btt->replBN);
      }

      if (eccErr) {
        pbn = PBN(row);
        rtv = RelocBlock (ftl, &pbn, pDev->page_count, AREA_TBL, true);
      }
    }
    return rtv;
  }
  /* LBN is out of range (max: NumDataBlocks-1) */
  EvrFsNFTL_LbnOutOfRange (ftl->Media->instance, lbn, ftl->NumDataBlocks-1);

  return FTL_ERROR_RANGE;
}


/**
  Write or update entry in block translation table.

  A logical block number and pointers to the variables containing its physical
  block numbers must be provided.

  Block translation table translates logical block numbers into physical
  block numbers. For each allocated primary/replacement block pair, one
  four bytes long entry exists, represented by the btt structure. This
  means, that one table sector (512 bytes) can hold translations for 128
  blocks.

  Translation table is updated each time new data block is allocated
  or when allocated block is moved (relocation or garbage collection).
  From logical block number, table sector is determined and also table
  block number which is needed when one block in table has to be found.
  For large page devices sector offset within page is calculated
  and table sector number is aligned to multiple of sectors per page.

  Whenever an entry in block translation table is updated, existing
  primary or replacement table block must be found and page with
  previously written table sector must be read and updated with new
  entry. Physical address of the last page written for each logical
  table sector is held in RAM array. If logical table sector was not
  yet written it's coressponding address in RAM array is invalid.
  Primary block must then be found (if exist) or allocated, else
  replacement block is used.

  In case when data block is relocated one of the *primBN and
  *replBN parameters can be NULL. This way table can be updated even if
  location of only one block is known.

  Table update is retried for __MAX_RETRY times if some NAND flash
  read/write error appear.

  \param[in,out]  ftl       FTL instance object
  \param[in]      lbn       logical block number
  \param[in]     *primBN    pointer to primary physical block number for lbn
  \param[in]     *replBN    pointer to replacement physical block number for lbn
  \return execution status FTL_STATUS
*/
static uint32_t UpdateBTT(NAND_FTL_DEV *ftl, uint16_t lbn, uint16_t *primBN, uint16_t *replBN) {
  BTT_ITEM entBl;
  uint16_t cBN, allocBN, badBN;
  uint32_t i, rtv, freePg;
  uint32_t blTyp, wrTyp, retry, err;
  uint32_t tbnS, tsnS, tsnB, tsnE, tsnIdx;
  uint32_t sOffs, row, col, primRow, replRow;

  EvrFsNFTL_TableUpdate (ftl->Media->instance, lbn, *primBN, *replBN);

  tsnS  = lbn >> BTT_EPS;                    /* TSN we are searching for */
  tbnS  = LBN(tsnS);                         /* is in block with TBN     */
  sOffs = tsnS &  (ftl->PageSectors - 1);    /* Sector number (offset)   */
  tsnS  = tsnS & ~(ftl->PageSectors - 1);    /* Sector per page multiple */

  retry = 0;
  do {
    /* Try to find PRIM and REPL block for tsnS */
    primRow = 0x00FFFFFF;
    replRow = 0x00FFFFFF;
    tsnB = tbnS << ftl->PPB;                /* First table index in block */
    tsnE = tsnB + ftl->TsnTableSize;        /* Last table index in block  */

    if (ftl->TsnTableSize > pDev->page_count) {
      tsnE = tsnB + pDev->page_count;
    }

    for (tsnIdx = tsnB; tsnIdx < tsnE; tsnIdx++) {
      blTyp = ftl->TsnTable[tsnIdx] >> 24;
      row   = ftl->TsnTable[tsnIdx] & 0x00FFFFFF;
      if (blTyp == TYP_PRIM) {
        if ((primRow == 0x00FFFFFF) || (primRow < row)) {
          primRow = row;
        }
      }
      else if (blTyp == TYP_REPL) {
        if ((replRow == 0x00FFFFFF) || (replRow < row)) {
          replRow = row;
        }
      }
    }
    /* Get tsn table index */
    tsnIdx = tsnS >> ftl->SPP;

    /* Select block to write updated table sector into */
    blTyp = ftl->TsnTable[tsnIdx] >> 24;

    wrTyp = TYP_REPL;
    if (blTyp == 0xFF) {
      wrTyp = TYP_PRIM;
    }

    freePg = INVALID;

    /* Check if we will write to PRIM */
    if (wrTyp == TYP_PRIM) {

      if (primRow == 0x00FFFFFF) {
        /* PRIM block is not known */
        rtv = ScanTable (ftl, LBN(tsnS), TYP_PRIM, &entBl.primBN);

        switch (rtv) {
          case FTL_OK:
            /* We have PRIM, find a free page in it! */
            break;

          case FTL_ERROR_NOT_FOUND:
            rtv = AllocBlock (ftl, &entBl.primBN, ALLOC_LAST, AREA_TBL);
            if (rtv != FTL_OK) {
              return rtv;
            }
            freePg = 0;
            break;

          default:
            return rtv;
        }
      }
      else {
        /* Set physical block number and free page */
        entBl.primBN = PBN(primRow);
      }
      /* We have PRIM block - find a free page in it */
      if (freePg != 0) {
        rtv = ScanBlock (ftl, entBl.primBN, EMPTY, &freePg);
        if (rtv != FTL_OK) {
          return rtv;
        }
      }
    }
    else {
      /* If no REPL found, allocate it */
      if (replRow == 0x00FFFFFF) {
        rtv = AllocBlock (ftl, &entBl.replBN, ALLOC_LAST, AREA_TBL);

        if (rtv != FTL_OK) {
          return rtv;
        }
        freePg = 0;
      }
      else {
        /* Set physical block number and free page */
        entBl.replBN = PBN(replRow);
        freePg = replRow - (uint32_t)(entBl.replBN << ftl->PPB) + 1;
      }
    }

    /* Physical block number and last written page for table block
       should be known by now. Just find a free page in it. Last valid
       sector, if exists, is written in nftl_TsnTable. All other parameters are
       also known by here.
    */

    err = 0;

    /* Find free page */
    cBN = (wrTyp == TYP_PRIM) ? entBl.primBN : entBl.replBN;

    /* Read last valid sector */
    row = ftl->TsnTable[tsnIdx] & 0x00FFFFFF;
    if (row == 0x00FFFFFF) {
      memset(ftl->PgBuf, 0xFF, pDev->page_size);
    }
    else {
      rtv = CachePgRead (ftl, row, 0, pDev->page_size);

      switch (rtv) {
        case FTL_OK:
          break;

        case FTL_ERROR_ECC:
          cBN = PBN(row);

          rtv = RelocBlock (ftl, &cBN, pDev->page_count, AREA_TBL, true);
          if (rtv != FTL_OK) {
            return rtv;
          }

          err = 1;
          retry++;
          continue;

        default:
          return rtv;
      }
    }
    /* Set entry column */
    col  = ftl->PgLay.sector_inc * sOffs +  (lbn & ((1 << BTT_EPS) - 1)) * BTT_ENTRY_SZ;

    /* Update entry in tsn page */
    if (primBN != NULL) {
      ftl->PgBuf[col]     = (uint8_t)(*primBN);
      ftl->PgBuf[col + 1] = (uint8_t)(*primBN >> 8);
    }
    if (replBN != NULL) {
      ftl->PgBuf[col + 2] = (uint8_t)(*replBN);
      ftl->PgBuf[col + 3] = (uint8_t)(*replBN >> 8);
    }

    /* Set spare */
    for (i = 0,   col = ftl->PgLay.spare_ofs;
         i < ftl->PageSectors;
         i++,     col += ftl->PgLay.spare_inc) {
      SetLSN (tsnS + i, wrTyp, &ftl->PgBuf[col + ftl->PgLay.spare.ofs_lsn]);
    }

    row = ROW(cBN, freePg);

    /* Write page */
    rtv = CachePgWrite (ftl, row);
    switch (rtv) {
      case FTL_OK:
        break;

      case FTL_ERROR_PROGRAM:
        /* Retire block? */
        badBN = cBN;
        rtv = RelocBlock (ftl, &cBN, freePg, AREA_TBL, true);
        if (rtv != FTL_OK) {
          return rtv;
        }
        rtv = MarkBlockBad (ftl, badBN, false);
        if (rtv != FTL_OK) {
          return rtv;
        }

        err = 1;
        retry++;
        continue;

      default:
        FlushPgCache (&ftl->Ca, row);
        return rtv;
    }

    /* Update entry in RAM table */
    ftl->TsnTable[tsnIdx] = row | (wrTyp << 24);

    if ((wrTyp == TYP_REPL) && (freePg == (uint32_t)(pDev->page_count-1))) {
      /* This is last page in replacement block, start GC */

      if (primRow == 0x00FFFFFF) {
        rtv = ScanTable (ftl, LBN(tsnS), TYP_PRIM, &entBl.primBN);
        switch (rtv) {
          case FTL_OK:
            break;

          case FTL_ERROR_NOT_FOUND:
            /* If PRIM can't be found, something must be seriously wrong */
            /* ECC may be the cause and consistency check should repair this */
            return FTL_ERROR;

          default:
            return rtv;
        }
      }
      else {
        entBl.primBN = PBN(primRow);
      }
      retry = 0;
gc:
      /* Allocate block */
      rtv = AllocBlock (ftl, &allocBN, ALLOC_LAST, AREA_TBL);
      if (rtv != FTL_OK) {
        return rtv;
      }

      /* Do garbage collection */
      rtv = GcRun (ftl, LBN(tsnS), &entBl, AREA_TBL, allocBN);

      switch (rtv) {
        case FTL_OK:
          break;

        case FTL_ERROR_PROGRAM:
          goto gc;

        default:
          return rtv;
      }

      /* Erase blocks */
      rtv = EraseBlock (ftl, &entBl.primBN);
      if (rtv != FTL_OK) { return rtv; }
      rtv = EraseBlock (ftl, &entBl.replBN);
      if (rtv != FTL_OK) { return rtv; }
    }
  }
  while(err && (retry < __MAX_RETRY));
  if (err) {
    return rtv;
  }

  return FTL_OK;
}

/**
  Perform garabage collection.

  Garbage collection tables are first build and flags are set according to data
  in primary and replacement blocks. Pages from both blocks are then copied to
  empty block whose number is specified in allocBn variable.

  \param[in,out]  ftl       FTL instance object
  \param[in]      lbn       logical block number
  \param[in,out] *btt       pointer to the block translation table entry
  \param[in]      gcArea    area of a logical block number
  \param[in]      allocBn   physical block number of a new primary block
  \return execution status FTL_STATUS
*/
static uint32_t GcRun(NAND_FTL_DEV *ftl, uint16_t lbn, BTT_ITEM *btt, uint32_t gcArea, uint16_t allocBn) {
  uint32_t i, cPg, oldPg, cLsn, lsn, row, col, type, eccWarn, rtv;
  uint32_t slot, primSlot, replSlot;

  /* Consolidating blocks */
  EvrFsNFTL_GarbageCollection (ftl->Media->instance, btt->primBN, btt->replBN, allocBn);

  primSlot = INVALID;
  replSlot = INVALID;

  /* Check if block in cache */
  for (slot = 0; slot < ftl->Ca.CachedBlocks; slot++) {
    if (ftl->Ca.Block[slot].pbn == btt->primBN) { primSlot = slot; }
    if (ftl->Ca.Block[slot].pbn == btt->replBN) { replSlot = slot; }
  }

  /* Assign cache slots to noncached blocks */
  if (primSlot == INVALID) {
    /* Primary block is not in cache */
    slot = GetIdxCacheSlot(&ftl->Ca);
    if (slot == replSlot) { slot = GetIdxCacheSlot(&ftl->Ca); }
    primSlot = slot;

    ftl->Ca.Block[primSlot].pbn = btt->primBN;
    ftl->Ca.Block[primSlot].lbn = lbn;
    ftl->Ca.Block[primSlot].typ = TYP_PRIM;
    ftl->Ca.Block[primSlot].nextPg = 0;
  }

  if (replSlot == INVALID) {
    /* Replacement block is not in cache */
    slot = GetIdxCacheSlot(&ftl->Ca);
    if (slot == primSlot) { slot = GetIdxCacheSlot(&ftl->Ca); }
    replSlot = slot;

    ftl->Ca.Block[replSlot].pbn = btt->replBN;
    ftl->Ca.Block[replSlot].lbn = lbn;
    ftl->Ca.Block[replSlot].typ = TYP_REPL;
    ftl->Ca.Block[replSlot].nextPg = 0;
  }

  /* Read REPL and get page indexes */
  for (cPg = 0; cPg < pDev->page_count; cPg++) {
    cLsn = INVALID;

    /* Check if we need to index the page */
    if (cPg == ftl->Ca.Block[replSlot].nextPg) {

      row = ROW (ftl->Ca.Block[replSlot].pbn, cPg);
      rtv = CachePgRead (ftl, row, ftl->PgLay.spare_ofs, __SZ_SP_USED);

      /* Ignore ECC error */
      if (rtv != FTL_OK && rtv != FTL_ERROR_ECC) { return rtv; }

      /* Get LSN */
      cLsn = GetLSN (&ftl->PgBuf[ftl->PgLay.spare.ofs_lsn], &type);

      /* Write index cache */
      if (cLsn != EMPTY) {
        ftl->Ca.Block[replSlot].pgIdx[cPg] = LSN2IDX(cLsn);
        ftl->Ca.Block[replSlot].nextPg++;
      }
    }

    /* Check if current page empty or last in block */
    if (cLsn == EMPTY || (cPg == (uint32_t)(pDev->page_count - 1))) {
      break;
    }
  }

  /* REPL block is indexed, start copying old valid pages */
  for (cPg = 0; cPg < pDev->page_count; cPg++) {
    eccWarn = false;
    oldPg   = INVALID;

    /* Check REPL block first */
    for (i = 0; i < ftl->Ca.Block[replSlot].nextPg; i++) {

      if (ftl->Ca.Block[replSlot].pgIdx[i] == cPg) {
        oldPg = i;
      }
    }
    cLsn = IDX2LSN (lbn, cPg);

    if (oldPg == INVALID) {
      /* Old page is not in REPL, go to PRIM */

      /* Try direct offset */
      row = ROW (ftl->Ca.Block[primSlot].pbn, cPg);
      rtv = CachePgRead (ftl, row, 0, pDev->page_size);

      if (rtv != FTL_OK) {
        if (rtv != FTL_ERROR_ECC) {
          return rtv;
        }
        if (ftl->LastECC & ECC_UNCORRECTED) {
          eccWarn = true;
        }
      }

      if (cLsn != GetLSN (&ftl->PgBuf[ftl->PgLay.spare_ofs + ftl->PgLay.spare.ofs_lsn], NULL)) {
        /* LSN not found at direct offset */
        eccWarn = false;

        /* Check other pages */
        for (i = 0; i < pDev->page_count; i++) {

          if (i < ftl->Ca.Block[primSlot].nextPg) {
            /* Read from cache */
            if (cLsn == IDX2LSN (lbn, ftl->Ca.Block[primSlot].pgIdx[i])) {

              row = ROW(ftl->Ca.Block[primSlot].pbn, i);
              rtv = CachePgRead (ftl, row, 0, pDev->page_size);

              if (rtv != FTL_OK) {
                if (rtv != FTL_ERROR_ECC) { return rtv; }
                if (ftl->LastECC & ECC_UNCORRECTED) {
                  eccWarn = true;
                }
              }
              /* LSN found in primary block */
              break;
            }
          }
          else {
            /* Do the indexing for PRIM block */
            row = ROW (ftl->Ca.Block[primSlot].pbn, i);
            rtv = CachePgRead (ftl, row, 0, pDev->page_size);

            if (rtv != FTL_OK) {
              if (rtv != FTL_ERROR_ECC) { return rtv; }
            }

            lsn = GetLSN (&ftl->PgBuf[ftl->PgLay.spare_ofs + ftl->PgLay.spare.ofs_lsn], NULL);

            /* Write in cache */
            if (lsn != EMPTY) {
              ftl->Ca.Block[primSlot].pgIdx[i] = LSN2IDX(lsn);
              ftl->Ca.Block[primSlot].nextPg++;
            }
            else {
              /* lsn == EMPTY */
              break;
            }

            if (lsn == cLsn) {
              if (ftl->LastECC & ECC_UNCORRECTED) {
                eccWarn = true;
              }
              /* LSN found in primary block */
              break;
            }
          }
        }
      }
    }
    else {
      /* Read old page */
      row = ROW (ftl->Ca.Block[replSlot].pbn, oldPg);
      rtv = CachePgRead (ftl, row, 0, pDev->page_size);

      if (rtv != FTL_OK) {
        if (rtv != FTL_ERROR_ECC) { return rtv; }

        if (ftl->LastECC & ECC_UNCORRECTED) {
          eccWarn = true;
        }
      }
    }
    /* Old page should be read by here */

    /* Set LSNs */
    for (i = 0,                 col = ftl->PgLay.spare_ofs;
         i < ftl->PageSectors;
         i++,                   col += ftl->PgLay.spare_inc) {
        SetLSN (cLsn + i, TYP_PRIM, &ftl->PgBuf[col + ftl->PgLay.spare.ofs_lsn]);
        if (eccWarn) {
          ftl->PgBuf[col + ftl->PgLay.spare.ofs_dcm] = 0x00;
        }
    }

    /* Write page */
    row = ROW (allocBn, cPg);
    rtv = CachePgWrite (ftl, row);

    if (rtv != FTL_OK) {
      if (rtv == FTL_ERROR_PROGRAM) {
        /* Mark old block as bad */
        MarkBlockBad (ftl, allocBn, true);
      }
      return rtv;
    }

    /* Update table if blocks in table area */
    if (gcArea == AREA_TBL) {
      /* Get TSN table index */
      i = cLsn >> ftl->SPP;
      if (i < ftl->TsnTableSize) {
        ftl->TsnTable[i] = row | (TYP_PRIM << 24);
      }
    }
  }

  ftl->Ca.Block[primSlot].pbn    = INVALID_BLOCK;
  ftl->Ca.Block[primSlot].nextPg = 0;
  ftl->Ca.Block[replSlot].pbn    = INVALID_BLOCK;
  ftl->Ca.Block[replSlot].nextPg = 0;

  return FTL_OK;
}

/**
  Reads cnt sectors from NAND flash into *buf, starting from given
  logical sector number.

  Primary and replacement blocks are first determined for given logical
  sector number by searching block translation table.
  When there was no primary block previously allocated,
  invalid or unwritten data can be reported and for the whole block
  0xFF values are returned. If either primary or replacement block
  exist, search for logical sector must be performed within. Replacement
  block is scanned first if exist. If logical sector was found in
  replacement block, data are returned to the caller. If only primary
  block exist for given logical sector, then spare area at direct sector
  offset is examined first, looking for logical sector number we are
  searching for. If sector number isn't found at direct offset, then
  whole primary block is scanned and if sector found, it's data are
  returned else empty sector is reported, by sending 0xFF values.

  \param[in]      lsn       logical sector number
  \param[in]     *buf       pointer to data buffer
  \param[in]      cnt       number of sectors to read
  \param[in,out]  ftl       FTL instance object
  \return execution status
            - true if cnt sectors successfully read
            - false otherwise
*/
uint32_t ftl_ReadSect(uint32_t lsn, uint8_t *buf, uint32_t cnt, NAND_FTL_DEV *ftl) {
  BTT_ITEM btti;
  uint32_t empty, lookup, eccWarn;
  uint32_t row, col;
  uint32_t cBN, sOffs, blTyp, sectPg, rtv;
  uint32_t di, si;

  /* Read sector */
  EvrFsNFTL_ReadSector (ftl->Media->instance, lsn, buf, cnt);

  if ((ftl->Status & FTL_STATUS_MOUNT) == 0) {
    return FTL_ERROR_UNMOUNTED;
  }

  rtv     = FTL_OK;
  blTyp   = TYP_PRIM;
  row     = INVALID;
  empty   = false;
  eccWarn = false;
  lookup  = true;
  sectPg  = 0;

  ftl->CurrLBN = LBN(lsn);

  for (; cnt; ) {
    eccWarn = false;

    /* Check table */
    if (lookup) {
      lookup = false;

      EvrFsNFTL_LsnToLbn (ftl->Media->instance, lsn, LBN(lsn));

      rtv = SearchBTT (ftl, LBN(lsn), &btti);

      if (rtv != FTL_OK) {
        return rtv;
      }
    }

    cBN = btti.primBN;

    /* Check PRIM existence */
    if (btti.primBN == INVALID_BLOCK) {
      /* PRIM does not exist, send empty sectors */
      empty = true;
    }
    else {
      /* Check REPL existence */
      if (btti.replBN != INVALID_BLOCK) {
        blTyp = TYP_REPL;
        cBN = btti.replBN;

        /* Search LSN in REPL */
        rtv = ScanBlock (ftl, btti.replBN, lsn, &sectPg);

        switch (rtv) {
          case FTL_OK:
            /* Page with specified LSN found, REPL is current block */
            break;
          case FTL_ERROR_NOT_FOUND_EOB:
            /* break; */

          case FTL_ERROR_NOT_FOUND:
            /* Set block back to PRIM */
            blTyp = TYP_PRIM;
            cBN   = btti.primBN;
            break;

          case FTL_ERROR_ECC:
            rtv = RefreshDataBlock (ftl, ftl->CurrLBN, TYP_REPL, &btti, pDev->page_count);
            if (rtv != FTL_OK) {
              return rtv;
            }
            continue;

          default:
            return rtv;
        }
      }
    }

    /* Set sector offset within page */
    sOffs = lsn & (ftl->PageSectors - 1);

    if (!empty) {
      /* Check PRIM if needed */
      if (cBN == btti.primBN) {
        blTyp = TYP_PRIM;

        /* Calculate page at direct offset */
        sectPg = LSN2IDX(lsn);

        row = ROW(cBN, sectPg);
        col = ftl->PgLay.spare_ofs + ftl->PgLay.spare_inc * sOffs;

        /* First check spare at direct offset */
        rtv = CachePgRead (ftl, row, col, __SZ_SP_USED);

        if (rtv != FTL_OK) {
          if (rtv == FTL_ERROR_ECC) {
            /* Relocate block */
            rtv = RefreshDataBlock(ftl, ftl->CurrLBN, TYP_PRIM, &btti, pDev->page_count);
            if (rtv != FTL_OK) {
              return rtv;
            }
            continue;
          }
          else return rtv;
        }

        /* Check if LSN found at direct offset */
        if (GetLSN (&ftl->PgBuf[ftl->PgLay.spare.ofs_lsn], NULL) == lsn) {
          /* Sector found, check for data corruption flag */

          if (ftl->PgBuf[ftl->PgLay.spare.ofs_dcm] != 0xFF) {
            eccWarn = true;
          }
        }
        else {
          /* Not found, so scan PRIM block */
          rtv = ScanBlock (ftl, btti.primBN, lsn, &sectPg);

          switch (rtv) {
            case FTL_OK:
              break;

            case FTL_ERROR_NOT_FOUND:
              /* Sector not found, send empty */
              empty = true;
              break;

            case FTL_ERROR_ECC:
              rtv = RefreshDataBlock(ftl, ftl->CurrLBN, TYP_PRIM, &btti, pDev->page_count);
              if (rtv != FTL_OK) {
                return rtv;
              }
              continue;

            default:
              return rtv;
          }
        }
      }

      /* Block with logical sector must be in cBN and its page (unless empty) is in sectPg */

      /* Set row if sectors not empty */
      row = ROW (cBN, sectPg);

      rtv = CachePgRead (ftl, row, 0, pDev->page_size);
      if (rtv != FTL_OK) {
        if (rtv == FTL_ERROR_ECC) {
          rtv = RefreshDataBlock(ftl, ftl->CurrLBN, blTyp, &btti, pDev->page_count);
          if (rtv != FTL_OK) {
            return rtv;
          }
          continue;
        }
        else return rtv;
      }
    }

    /* Read sectors */
    si = ftl->PgLay.spare_ofs;
    for (di = sOffs * ftl->PgLay.sector_inc,      si += sOffs * ftl->PgLay.spare_inc;
         cnt && sOffs < ftl->PageSectors;
         di += ftl->PgLay.sector_inc,             si += ftl->PgLay.spare_inc) {

      EvrFsNFTL_LoadSector (ftl->Media->instance, lsn, di, si);

      if (empty) {
        /* Send empty sector */
        memset (buf, 0xFF, __SZ_SECT);
      }
      else {
        memcpy (buf, &ftl->PgBuf[di], __SZ_SECT);

        if (ftl->PgBuf[si + ftl->PgLay.spare.ofs_dcm] != 0xFF) {
          eccWarn = true;
        }
      }
      buf += __SZ_SECT;
      lsn++;
      cnt--;
      sOffs++;
    }

    /* Check if current LSN goes to other block */
    if (LBN(lsn - 1) < LBN(lsn)) {
      lookup = true;
      ftl->CurrLBN = LBN(lsn);
    }
  }

  EvrFsNFTL_ReadSectorSuccess (ftl->Media->instance);

  if (eccWarn) {
    return FTL_WARNING;
  }
  return FTL_OK;
}

/**
  Writes cnt sectors from *buf to NAND flash starting from given logical
  sector number.

  Logical sector being written to the NAND flash device must be written
  to the corresponding primary or replacement block. So, search in the
  block translation table is first performed, looking for previously
  allocated primary and replacement block. On devices with multiple
  sectors per page, old sector must be located first, updated and copied,
  if needed, together with other valid sectors in that page, to the
  new location in memory.
  After search in the block translation table, replacement block, if
  exist, is scanned first for given logical sector number. If sector is
  found, it's position is remembered as old, and free page in replacement
  block must be found.
  If old sector location was determined by scanning primary block, then
  another scan has to be performed in replacement block (if exists) and
  if given logical sector number was also found in replacement block,
  old sector location is updated with his location in replacement block.
  A free page is located in every case. If there is no free page found
  garbage collection is started and whole search is repeated.
  If either primary or replacement block doesn't exist, new block is
  allocated when needed, sector is then written to the first page in that
  block and block entry in translation table is updated.
  If last valid (old) sector position was discovered, then it's
  corresponding page is read to buffer if needed and new or updated
  sectors are written to it together with updated spare area(s) and
  whole page is written to block in which free page was located.
  When old sector could not be located, meaning it does not exist yet,
  only current sectors are written to the located free page.

  In devices with multiple sectors per page, sector numbers within each
  page are alligned to multiple of sectors per page, meaning, for example,
  in device with four sectors per page, first sector in page has
  number which is multiple of four: 0, 4, 8, ... , 132, 136 and so on.
  Sector numbers for next sectors are then incremented sequentialy till
  the last sector in page, ti. one page with four sectors could comprise
  of sectors 8, 9, 10 and 11.

  \param[in]      lsn       logical sector number
  \param[in]     *buf       pointer to data buffer
  \param[in]      cnt       number of sectors to write
  \param[in,out]  ftl       FTL instance object
  \return execution status
            - true if cnt sectors successfully written
            - false otherwise
*/
uint32_t ftl_WriteSect(uint32_t lsn, const uint8_t *buf, uint32_t cnt, NAND_FTL_DEV *ftl) {
  BTT_ITEM btti, oldBtti;
  uint16_t cBN, allocBN;
  uint32_t lookup, alloc;
  const uint8_t *p;
  uint32_t oldBn, oldTyp;
  uint32_t sectPg, freePg, oldPg;
  uint32_t i, di, si, blTyp, rtv;
  uint32_t k, cLsn, secNum;

  /* Write sector */
  EvrFsNFTL_WriteSector (ftl->Media->instance, lsn, buf, cnt);

  if ((ftl->Status & FTL_STATUS_MOUNT) == 0) {
    return FTL_ERROR_UNMOUNTED;
  }

  lookup  = true;

  ftl->CurrLBN = LBN(lsn);

  for (; cnt; ) {
    /* Check table */
    if (lookup) {
      lookup = false;

      EvrFsNFTL_LsnToLbn (ftl->Media->instance, lsn, LBN(lsn));

      rtv = SearchBTT (ftl, LBN(lsn), &btti);
      if (rtv != FTL_OK) {
        return rtv;
      }
    }

    k      = 0;
    sectPg = 0;
    p      = NULL;
    oldBn  = INVALID_BLOCK;
    oldPg  = INVALID;
    oldTyp = INVALID;
    freePg = INVALID;
    alloc  = false;
    cLsn   = lsn & ~(ftl->PageSectors - 1);

    blTyp  = TYP_PRIM;
    cBN    = btti.primBN;

    if (btti.primBN == INVALID_BLOCK) {
      /* Allocate PRIM block */
      rtv = AllocBlock (ftl, &btti.primBN, ALLOC_REST, AREA_DAT);
      if (rtv != FTL_OK) {
        return rtv;
      }
      alloc = true;
      cBN   = btti.primBN;
      freePg = 0;
    }
    else {
      /* Check REPL block first */
      if (btti.replBN != INVALID_BLOCK) {
        cBN = btti.replBN;
        rtv = ScanBlock (ftl, btti.replBN, lsn, &sectPg);

        switch (rtv) {
          case FTL_OK:
            oldBn = btti.replBN;
            oldPg = sectPg;
            break;

          case FTL_ERROR_NOT_FOUND:
            freePg = sectPg;
            break;

          case FTL_ERROR_NOT_FOUND_EOB:
            goto gc;

          case FTL_ERROR_ECC:
            rtv = RefreshDataBlock(ftl, ftl->CurrLBN, TYP_REPL, &btti, pDev->page_count);
            if (rtv != FTL_OK) {
              return rtv;
            }
            continue;

          default:
            return rtv;
        }

        if (oldPg != INVALID) {
          /* Find empty page in REPL */
          rtv = ScanBlock (ftl, btti.replBN, EMPTY, &sectPg);

          switch (rtv) {
            case FTL_OK:
              blTyp  = TYP_REPL;
              freePg = sectPg;
              break;

            case FTL_ERROR_ECC:
              rtv = RefreshDataBlock(ftl, ftl->CurrLBN, TYP_REPL, &btti, pDev->page_count);
              if (rtv != FTL_OK) {
                return rtv;
              }
              continue;

            case FTL_ERROR_NOT_FOUND_EOB:
              /* SET BN */
              goto gc;

            default:
              return rtv;
          }
        }
        else {
          /* Find old in PRIM */
          rtv = ScanBlock (ftl, btti.primBN, lsn, &sectPg);

          switch (rtv) {
            case FTL_OK:
              oldBn = btti.primBN;
              oldPg = sectPg;
              cBN = btti.replBN;
              blTyp = TYP_REPL;
              break;

            case FTL_ERROR_NOT_FOUND:
              cBN = btti.primBN;
              freePg = sectPg;
              break;

             case FTL_ERROR_ECC:
              rtv = RefreshDataBlock(ftl, ftl->CurrLBN, TYP_PRIM, &btti, pDev->page_count);
              if (rtv != FTL_OK) {
                return rtv;
              }
              continue;

            default:
              return rtv;
          }
        }
      }
      else {
        /* REPL block does not exist, find old in PRIM */
        rtv = ScanBlock (ftl, btti.primBN, lsn, &sectPg);

        switch (rtv) {
          case FTL_OK:
            oldBn = btti.primBN;
            oldPg = sectPg;
            cBN = btti.replBN;
            blTyp = TYP_REPL;
            break;

          case FTL_ERROR_NOT_FOUND:
            freePg = sectPg;
            break;

          case FTL_ERROR_ECC:
            rtv = RefreshDataBlock(ftl, ftl->CurrLBN, TYP_PRIM, &btti, pDev->page_count);
            if (rtv != FTL_OK) {
              return rtv;
            }
            continue;

          default:
            return rtv;
        }
        if (cBN == INVALID_BLOCK) {
          rtv = AllocBlock (ftl, &btti.replBN, ALLOC_PENUL, AREA_DAT);
          if (rtv != FTL_OK) {
            return rtv;
          }
          cBN = btti.replBN;
          alloc = true;
          freePg = 0;
        }
      }
    }

    /* Read old LSN if exists or is needed */
    if (oldPg != INVALID) {
      if (!(lsn == cLsn && cnt >= ftl->PageSectors)) {
        /* Read last valid page */
        rtv = CachePgRead (ftl, ROW(oldBn, oldPg), 0, pDev->page_size);
        if (rtv != FTL_OK) {
          if (rtv != FTL_ERROR_ECC) { return rtv; }
          /* Relocate block */
          rtv = RefreshDataBlock(ftl, ftl->CurrLBN, oldTyp, &btti, pDev->page_count);
          if (rtv != FTL_OK) {
            return rtv;
          }
          continue;
        }
      }
    }
    else {
      memset (ftl->PgBuf, 0xFF, pDev->page_size);
    }

    /* Prepare buffer for writing */
    secNum = lsn & (ftl->PageSectors - 1);
    p = buf;

    for (i = 0,       di = 0,                      si = ftl->PgLay.spare_ofs;
         i < ftl->PageSectors;
         i++, cLsn++, di += ftl->PgLay.sector_inc, si += ftl->PgLay.spare_inc) {

      if (i == secNum) {
        EvrFsNFTL_LoadSector (ftl->Media->instance, cLsn, di, si);

        memcpy (&ftl->PgBuf[di], p, __SZ_SECT);
        ftl->PgBuf[si + ftl->PgLay.spare.ofs_dcm] = 0xFF;
        p += __SZ_SECT;
        k++;
        if (cnt - k) secNum++;
      }

      SetLSN (cLsn, blTyp, &ftl->PgBuf[si + ftl->PgLay.spare.ofs_lsn]);
    }

    /* Write page */
    rtv = CachePgWrite (ftl, ROW(cBN, freePg));
    if (rtv != FTL_OK) {
      if(rtv != FTL_ERROR_PROGRAM) {
        return rtv;
      }
      /* Program failed, relocate current block and repeat write */
      rtv = RefreshDataBlock (ftl, ftl->CurrLBN, blTyp, &btti, freePg);
      if (rtv != FTL_OK) {
        return rtv;
      }
      rtv = MarkBlockBad(ftl, cBN, false);
      if (rtv != FTL_OK) {
        return rtv;
      }
      continue;
    }

    /* Update table if we allocated new block */
    if (alloc) {
      rtv = UpdateBTT (ftl, LBN(lsn), &btti.primBN, &btti.replBN);
      if (rtv != FTL_OK) {
        return rtv;
      }
    }

    if ((blTyp == TYP_REPL) && (freePg == (uint32_t)(pDev->page_count - 1))) {
      /* Last page in replacement block written, start GC */
gc:
      /* Allocate block */
      rtv = AllocBlock (ftl, &allocBN, ALLOC_LAST, AREA_DAT);

      if (rtv != FTL_OK) {
        return rtv;
      }

      /* Run GC */
      rtv = GcRun (ftl, LBN(lsn), &btti, AREA_DAT, allocBN);

      switch (rtv) {
        case FTL_OK:
          break;

        case FTL_ERROR_PROGRAM:
          goto gc;

        default:
          return rtv;
      }

      oldBtti = btti;
      btti.primBN = allocBN;
      btti.replBN = INVALID_BLOCK;

      /* Update table */
      rtv = UpdateBTT (ftl, LBN(lsn), &btti.primBN, &btti.replBN);
      if (rtv != FTL_OK) {
        return rtv;
      }

      /* Erase old blocks */
      rtv = EraseBlock (ftl, &oldBtti.primBN);
      if (rtv != FTL_OK) { return rtv; }
      rtv = EraseBlock (ftl, &oldBtti.replBN);
      if (rtv != FTL_OK) { return rtv; }
    }

    /* Update counter and buffer pointer */
    if (k) {
      cnt -= k;
      lsn += k;
      buf  = p;
    }

    /* Check if current lsn goes to other block */
    if (LBN(lsn - 1) < LBN(lsn)) {
      lookup = true;
      ftl->CurrLBN = LBN(lsn);
    }
  }

  EvrFsNFTL_WriteSectorSuccess (ftl->Media->instance);

  return FTL_OK;
}

/**
  Determine number of blocks available in data area

  \param[in,out]  ftl       FTL instance object
  \param[in]      DataBBCnt number of bad blocks in data area
  \return number of blocks in data area
*/
static uint16_t CalcDataBlocks (NAND_FTL_DEV *ftl, uint16_t DataBBCnt) {
  uint16_t num;

  /* Block 0 + EndOfArea - StartOfArea - BadBlockInArea */
  num  = 1 + ftl->Cfg->DataEndBn - ftl->Cfg->DataStartBn - DataBBCnt;
  /* Reduce number of blocks for 2% overhead */
  num -= (pDev->block_count * 2) / 100;

  return num;
}

/**
  Translation table cache initialization

  Fills RAM translation table cache with default values.

  \param[in,out]  ftl       FTL instance object
*/
static void InitBtt (NAND_FTL_DEV *ftl) {
  uint32_t tsnIdx;

  /* Init table array */
  for (tsnIdx = 0; tsnIdx < ftl->TsnTableSize; tsnIdx++) {
    ftl->TsnTable[tsnIdx] = INVALID;
  }
}

/**
  Translation table initialization

  Valid state must be determined during table initialisation. Each page
  in table area is therefore read and table sector number is extracted
  from spare. Table sector number is then divided with number of sectors
  per page so that index in nftl_TsnTable array is obtained. Row
  address together with the block type of the current page is then written
  in nftl_TsnTable array at that index.
  Last page of each replacement block is also checked for valid sector
  and if is found, garbage collection is (must be) performed.

  \param[in,out]  ftl       FTL instance object
  \return execution status FTL_STATUS
*/
static uint32_t LoadBtt (NAND_FTL_DEV *ftl) {
  BTT_ITEM btti;
  uint16_t cBN, allocBN;
  uint32_t cPg, row, rdTyp, tblTyp, tsn, tsnIdx, rtv;

  /* Loading translation table */
  EvrFsNFTL_ReadTable (ftl->Media->instance);

  /* Init translation table cache */
  InitBtt (ftl);

  /* Check all table blocks */
  for (cBN = ftl->Cfg->BttStartBn; cBN < ftl->Cfg->DataStartBn; cBN++) {
    for (cPg = 0; cPg < pDev->page_count; cPg++) {
      row = ROW(cBN, cPg);

      rtv = CachePgRead (ftl, row, ftl->PgLay.spare_ofs, __SZ_SP_USED);
      if (rtv != FTL_OK) {
        /* Ignore ECC error */
        if (rtv != FTL_ERROR_ECC) {
          return rtv;
        }
      }

      /* Skip bad block */
      if (cPg == 0 && ftl->PgBuf[ftl->PgLay.spare.ofs_bbm] != BB_MASK) {
        /* Skipping bad block */
        EvrFsNFTL_SkipBadBlock (ftl->Media->instance, cBN);
        break;
      }

      /* Get TSN */
      tsn = GetLSN (&ftl->PgBuf[ftl->PgLay.spare.ofs_lsn], &rdTyp);

      /* Skip to next block if empty */
      if (tsn == EMPTY) {
        break;
      }
      else {
        /* Get table index and update row if needed */
        tsnIdx = tsn >> ftl->SPP;

        if (tsnIdx < ftl->TsnTableSize) {
          tblTyp = ftl->TsnTable[tsnIdx] >> 24;

          if (tblTyp == 0xFF || rdTyp == TYP_REPL) {
            ftl->TsnTable[tsnIdx] = row | rdTyp << 24;
          }
        }
      }

      /* Check if last page in REPL isn't empty */
      if ((rdTyp == TYP_REPL) && (cPg == (uint32_t)(pDev->page_count - 1))) {
        /* Start garbage collection */
        btti.primBN = INVALID_BLOCK;

        rtv = ScanTable (ftl, LBN(tsn), TYP_PRIM, &btti.primBN);
        if (rtv != FTL_OK) { return rtv; }

        btti.replBN = cBN;
gc:
        /* Allocate new block */
        rtv = AllocBlock (ftl, &allocBN, ALLOC_LAST, AREA_TBL);
        if (rtv != FTL_OK) { return rtv; }

        /* Start garbage collection */
        rtv = GcRun (ftl, LBN(tsn), &btti, AREA_TBL, allocBN);
        if (rtv != FTL_OK) {
          if (rtv == FTL_ERROR_PROGRAM) {
            goto gc;
          }
          return rtv;
        }

        /* Now erase old blocks */
        rtv = EraseBlock (ftl, &btti.primBN);
        if (rtv != FTL_OK) { return rtv; }
        rtv = EraseBlock (ftl, &btti.replBN);
        if (rtv != FTL_OK) { return rtv; }
      }
    }
  }
  return FTL_OK;
}

/**
  Read Boot Block

  Block 0 is first examined for FTL signature. If FTL signature is
  present then number of available blocks in data area is calculated and
  unformatted flag is cleared else number of available blocks is set to
  zero and FTL unformatted flag is set in FTL status variable.

  \param[in,out]  ftl       FTL instance object
  \return execution status FTL_STATUS
*/
static uint32_t RdBootBlock (NAND_FTL_DEV *ftl) {
  uint32_t stat, *p;

  /* Reading boot block */
  EvrFsNFTL_ReadBootBlock (ftl->Media->instance);

  stat = Drv_ReadPage (0, ftl->PgBuf, ftl);

  if (stat == FTL_OK || stat == FTL_ERROR_ECC_COR || stat == FTL_ERROR_ECC_FAIL) {
    if (pDev->sw_ecc == 1) {
      /* Hamming ECC is enabled, decode ECC */
      DecodeECC(ftl, ftl->PgBuf);
    }

    /* PgBuf is aligned, we can access it as u32 */
    p = (uint32_t *)(uint32_t)ftl->PgBuf;

    /* Check FTL format signature */
    if ((p[0] == BOOT_SIGN) && (p[9] == BOOT_SIGN)) {
      /* Boot signature is valid */
      EvrFsNFTL_BootSignatureValid (ftl->Media->instance);

      /* Calculate number of data blocks reported at format */
      ftl->NumDataBlocks = CalcDataBlocks (ftl, (uint16_t)p[8]);
      ftl->Status |= FTL_STATUS_FORMAT;

      EvrFsNFTL_DataBlockCapacity (ftl->Media->instance, ftl->NumDataBlocks);
    }
    else {
      /* Boot signature is invalid */
      EvrFsNFTL_BootSignatureInvalid (ftl->Media->instance);

      /* Default values for unformatted NAND */
      ftl->NumDataBlocks = 0;
      ftl->Status &= ~FTL_STATUS_FORMAT;
    }
    stat = FTL_OK;
  }

  return (stat);
}


/**
  Format FTL

  First page in each block of the device is checked for manufacturer bad block
  marker. If marker is found, whole page is programmed to 0x00.
  Good blocks are all erased. If erase fails, then block is marked as bad.
  At the end, signature is written to the first page of block 0, together with
  formatting informations.
  Block 0 is therefore reserved and should not be used at any other time.

  \param[in,out]  ftl       FTL instance object
  \param[par]     par       formatting parameters
  \return execution_status
            - true if format successful
            - false on read/write error
*/
static uint32_t ftl_Format (NAND_FTL_DEV *ftl, void *par) {
  char *fp = (char *)par;
  bool bad, ebb;
  uint32_t row, rtv, bbmPos;
  uint32_t block, page;
  uint16_t TableBBCnt, DataBBCnt;
  uint32_t *p;

  /* Formatting NAND flash */
  EvrFsNFTL_Format (ftl->Media->instance);

  ftl->Status &= ~(FTL_STATUS_FORMAT | FTL_STATUS_MOUNT);

  ebb = false;

  /* Check which parameter is provided */
  if (fp && fp[0] == '/') {
    if (fp[1] == 'L' && fp[2] == 'L' && fp[3] == 'E' && fp[4] == 'B') {
      /* Bad block erase enabled */
      EvrFsNFTL_FormatLLEB (ftl->Media->instance);
      ebb = true;
    }
  }

  /* Calculate spare area byte */
  bbmPos = (uint32_t)(__SZ_SECT << Log2(ftl->PageSectors));

  if (ftl->PageSectors == 1) {
    /* Small page: bad block marker is 6th byte in spare */
    bbmPos += 5;
  }

  /* Read Boot Block */
  if (RdBootBlock (ftl) != FTL_OK) {
    EvrFsNFTL_ReadBootBlockFailed (ftl->Media->instance);
    return (false);
  }

  if (ftl->Status & FTL_STATUS_FORMAT) {
    /* Override factory bad block marker position if we already have ours */
    bbmPos = ftl->PgLay.spare_ofs + ftl->PgLay.spare.ofs_bbm;
  }

  EvrFsNFTL_BBMPositionSet (ftl->Media->instance, bbmPos);

  /* Init bad block counters */
  TableBBCnt = 0;
  DataBBCnt  = 0;

  /* Clear number of user data blocks */
  ftl->NumDataBlocks = 0;

  /* Loop through all blocks, erase good and mark bad ones */
  for (block = 0; block < pDev->block_count; block++) {
    bad  = false;
    page = 0;

    do {
      /* Check page for bad block marker */
      row = ROW (block, page);

      rtv = Drv_ReadPage (row, ftl->PgBuf, ftl);
      switch (rtv) {
        case FTL_OK:
        case FTL_ERROR_ECC_COR:
        case FTL_ERROR_ECC_FAIL:
          break;

        default:
          return rtv;
      }

      if (ftl->PgBuf[bbmPos] != 0xFF) {
        /* Bad block marker is set */
        EvrFsNFTL_BadBlockDetected (ftl->Media->instance, block);
        bad = true;
        break;
      }

      if (page == 1) {
        /* Go to last page in block */
        page = pDev->page_count - 1;
      }
      else {
        page++;
      }
    }
    while (page != pDev->page_count);

    row = ROW (block, 0);

    if (bad == false || ebb == true) {
      /* Erase block if not bad or if erase of bad blocks requested */
      rtv = Drv_EraseBlock (row, ftl);
      switch (rtv) {
        case FTL_OK:
          bad = false;
          break;

        case FTL_ERROR_ERASE:
          /* Erase failed, this is bad block */
          bad = true;
          ftl->BadBlockCnt++;
          break;

        default:
          return false;
      }
    }

    if (bad) {
      /* Block is bad, mark it as defective */
      EvrFsNFTL_BadBlockMark (ftl->Media->instance, block);

      memset (ftl->PgBuf, 0x00, pDev->page_size);
      Drv_WritePage (row, ftl->PgBuf, ftl);

      /* Increment bad block counter */
      if (block >= ftl->Cfg->BttStartBn && block <= ftl->Cfg->BttEndBn) {
        TableBBCnt++;
      }
      else {
        if (block >= ftl->Cfg->DataStartBn && block <= ftl->Cfg->DataEndBn) {
          DataBBCnt++;
        }
      }
    }
  }
  memset(ftl->PgBuf, 0xFF, pDev->page_size);
  /* Write formating info in boot block */
  p = (uint32_t *)(uint32_t)&ftl->PgBuf[0];

  p[0] = BOOT_SIGN;                 /* 0. signature                       */
  p[1] = ftl->Cfg->BttStartBn;      /* 1. start of table                  */
  p[2] = ftl->Cfg->BttEndBn;        /* 2. end of table                    */
  p[3] = ftl->Cfg->DataStartBn;     /* 3. start of data                   */
  p[4] = ftl->Cfg->DataEndBn;       /* 4. end of data                     */
  p[5] = ftl->PgLay.spare.ofs_bbm;  /* 5. bad block marker position       */
  p[6] = ftl->PgLay.spare.ofs_lsn;  /* 6. logical sector number position  */
  p[7] = TableBBCnt;                /* 7. table bad block count           */
  p[8] = DataBBCnt;                 /* 8. data bad block count            */
  p[9] = BOOT_SIGN;                 /* 9. signature                       */

  if (pDev->sw_ecc == 1) {
    /* Hamming ECC is enabled, encode ECC */
    EncodeECC (ftl, ftl->PgBuf);
  }

  rtv = Drv_WritePage (0, ftl->PgBuf, ftl);
  if (rtv != FTL_OK) {
    return false;
  }

  InitBtt (ftl);

  ftl->NumDataBlocks = CalcDataBlocks (ftl, DataBBCnt);

  /* Low level formatting completed */
  EvrFsNFTL_FormatSuccess (ftl->Media->instance);

  ftl->Status |= FTL_STATUS_FORMAT | FTL_STATUS_MOUNT;
  return true;
}


/**
  Initialize storage media

  \param[in,out]  ftl       FTL instance object
  \return execution status
            - true if operation ok
            - false otherwise
*/
static uint32_t ftl_init_media (NAND_FTL_DEV *ftl) {
  uint32_t status;

  /* Mounting NFTL */
  EvrFsNFTL_Mount (ftl->Media->instance);

  status = true;

  ftl->Status &= ~(FTL_STATUS_FORMAT | FTL_STATUS_MOUNT);

  if ((ftl->Status & FTL_STATUS_INIT) == 0) {
    /* Driver not initialized */
    EvrFsNFTL_DeviceNotInitialized (ftl->Media->instance);
    status = false;
  }
  else if (Drv_ResetDevice (ftl) != FTL_OK) {
    /* Failed to reset the device */
    status = false;
  }
  else if (Drv_ReadParameters (ftl) != FTL_OK) {
    /* Failed to read parameter page */
    status = false;
  }
  else if (Drv_ManageECC (ftl) != FTL_OK) {
    /* Failed to determine ECC configuration */
    status = false;
  }
  else if (Drv_SetupFeatures (ftl) != FTL_OK) {
    /* Failed to retrieve or modify feature parameters */
    status = false;
  }
  else if (RdBootBlock (ftl) != FTL_OK) {
    /* Failed to read boot block */
    EvrFsNFTL_ReadBootBlockFailed (ftl->Media->instance);
    status = false;
  }
  else {
    if (ftl->Status & FTL_STATUS_FORMAT) {
      /* Load translation table */
      if (LoadBtt (ftl) != FTL_OK) {
        /* Failed to load translation table */
        EvrFsNFTL_ReadTableFailed (ftl->Media->instance);
        status = false;
      }
    }

    ftl->Status |= FTL_STATUS_MOUNT;
  }

#ifdef FS_DEBUG
  if (status == true) {
    EvrFsNFTL_MountSuccess (ftl->Media->instance);
  }
#endif

  return (status);
}


/**
  Initialize FTL resources

  \param[in,out]  ftl       FTL instance object
  \return execution status
            - true if operation ok
            - false otherwise
*/
static uint32_t ftl_init_resources (NAND_FTL_DEV *ftl) {
  uint32_t i, status;
  uint8_t *bp;
  NAND_FTL_CFG *cfg = ftl->Cfg;

  /* Initializing NFTL */
  EvrFsNFTL_Init            (ftl->Media->instance);
  EvrFsNFTL_LoadTableLayout (ftl->Media->instance, ftl->Cfg->BttStartBn,  ftl->Cfg->BttEndBn);
  EvrFsNFTL_LoadDataLayout  (ftl->Media->instance, ftl->Cfg->DataStartBn, ftl->Cfg->DataEndBn);

  /* Load buffer pointers */
  ftl->PgBuf    = cfg->PgBuf;
  ftl->Ca.Block = cfg->BlockCache;
  ftl->Ca.Page  = cfg->PageCache;
  ftl->TsnTable = cfg->TsnTable;

  /* Load buffer sizes */
  ftl->Ca.CachedBlocks = cfg->NumCacheBlocks;
  ftl->Ca.CachedPages  = cfg->NumCachePages;
  ftl->TsnTableSize    = cfg->TsnTableSize;

  /* Load default page layout */
  ftl->PgLay.spare.ofs_lsn  = DL_POS_LSN;
  ftl->PgLay.spare.ofs_dcm  = DL_POS_COR;
  ftl->PgLay.spare.ofs_bbm  = DL_POS_BBM;
  ftl->PgLay.spare.ofs_ecc  = DL_POS_ECC;

  ftl->PgLay.sector_inc     = DL_SECT_INC;
  ftl->PgLay.spare_ofs      = DL_SPARE_OFS;
  ftl->PgLay.spare_inc      = DL_SPARE_INC;

  /* Set FTL parameters */
  ftl->PageSectors = pDev->page_sectors;
  ftl->SPP         = (uint8_t)Log2 (pDev->page_sectors);
  ftl->PPB         = (uint8_t)Log2 (pDev->page_count);
  ftl->SPB         = (uint8_t)Log2 (pDev->block_sectors);

  ftl->LastTBN     = cfg->BttEndBn;
  ftl->LastDBN     = cfg->DataEndBn;

  ftl->GcLBN         = 0;
  ftl->BadBlockCnt   = 0;
  ftl->NumDataBlocks = 0;
  ftl->Status        = 0;

  /* Clear block queue */
  ftl->PbnQ[0]     = 0;
  ftl->PbnQ[1]     = 0;
  ftl->PbnQ[2]     = 0;

  /* Init block indexing */
  for (bp = cfg->BlockCacheBuf, i = 0; i < cfg->NumCacheBlocks; i++, bp += pDev->page_count) {
    ftl->Ca.Block[i].pbn   = INVALID_BLOCK;
    ftl->Ca.Block[i].pgIdx = bp;
  }

  /* Init page caching */
  for (bp = cfg->PageCacheBuf, i = 0; i < cfg->NumCachePages; i++, bp += pDev->page_size) {
    ftl->Ca.Page[i].row = INVALID;
    ftl->Ca.Page[i].buf = bp;
  }

  /* Init Block Translation Table Cache */
  InitBtt (ftl);

  if (NAND_MediaDriver.Initialize (ftl->Media) != NAND_OK) {
    /* Media driver initialization failed */
    EvrFsNFTL_InitMediaFailed (ftl->Media->instance);
    status = false;
  }
  else {
    ftl->Status = FTL_STATUS_INIT;
    status = true;
  }

  return (status);
}

/**
  Initialize FTL

  \param[in]      mode      initialization mode
  \param[in,out]  ftl       FTL instance object
  \return execution status
            - true if initialization passed
            - false otherwise
*/
uint32_t ftl_Init (uint32_t mode, NAND_FTL_DEV *ftl) {
  uint32_t status;

  switch (mode) {
    case DM_IO:
      status = ftl_init_resources (ftl);
      break;

    case DM_MEDIA:
      status = ftl_init_media (ftl);
      break;

    default:
      status = false;
      break;
  }

  return (status);
}


/**
  Uninitialize storage media

  \param[in]      mode      uninitialization mode (DM_IO / DN_MEDIA)
  \param[in,out]  ftl       FTL instance object
  \return execution status
            - true on success
            - false otherwise
*/
uint32_t ftl_UnInit (uint32_t mode, NAND_FTL_DEV *ftl) {
  uint32_t status;

  status = true;

  if (mode == DM_IO) {
    /* Uninit media driver */
    EvrFsNFTL_UninitDriver (ftl->Media->instance);

    if (NAND_MediaDriver.Uninitialize (ftl->Media) != NAND_OK) {
      /* Failed to uninit media driver */
      EvrFsNFTL_UninitDriverFailed (ftl->Media->instance);
      status = false;
    }
  }
  else {
    /* Uninit NFTL */
    EvrFsNFTL_Uninit (ftl->Media->instance);

    if (mode != DM_MEDIA) {
      status = false;
    }
  }

  return (status);
}


/**
  Read storage media geometry properties

  \param[out]     info      pointer to \ref FS_MEDIA_INFO object
  \param[in]      ftl       FTL instance object
  \return execution status
            - true on success
            - false otherwise
*/
uint32_t ftl_ReadInfo (fsMediaInfo *info, NAND_FTL_DEV *ftl) {
  uint32_t status;

  /* Retreiving media info */
  EvrFsNFTL_ReadInfo (ftl->Media->instance, (uint32_t)info);

  if (ftl == NULL || info == NULL) {
    /* Invalid parameters */
    EvrFsNFTL_ParameterInvalid (ftl->Media->instance);
    status = false;
  }
  else {
    info->block_cnt  = (uint32_t)(ftl->NumDataBlocks << ftl->SPB);
    info->read_blen  = __SZ_SECT;
    info->write_blen = __SZ_SECT;

    EvrFsNFTL_ReadInfoSuccess (ftl->Media->instance, info->block_cnt);
    status = true;
  }

  return (status);
}


/**
  Process given device control command

  \param[in]      code      device control command code
  \param[in,out]  p         command argument
  \param[in]      ftl       FTL instance object
  \return \ref fsStatus
*/
fsStatus ftl_DevCtrl (fsDevCtrlCode code, void *p, NAND_FTL_DEV *ftl) {
  fsStatus status;

  EvrFsNFTL_DevCtrl (ftl->Media->instance, code, p);

  status = fsError;

  if (code == fsDevCtrlCodeCheckMedia) {
    /* Check for media presence */
    if (p) {
      *(uint32_t *)p = FS_MEDIA_NOCHKMEDIA;
      status = fsOK;
    }
  }
  else if (code == fsDevCtrlCodeControlMedia) {
    /* Control media */
    if (p != NULL) {
      switch (*(uint32_t *)p) {
        case FS_CONTROL_MEDIA_INIT:
          if (ftl_Init (DM_MEDIA, ftl) == true) {
            status = fsOK;
          }
          break;

        default:
          status = fsUnsupported;
          break;
      }
    }
  }
  else if (code == fsDevCtrlCodeFormat) {
    if (ftl_Format (ftl, p) == true) {
      status = fsOK;
    }
  }
  else {
    /* Unsupported */
    EvrFsNFTL_DevCtrlUnsupported (ftl->Media->instance, code);
    status = fsUnsupported;
  }

  return (status);
}


/*-----------------------------------------------------------------------------
 *      32-bit number parity calculation
 *----------------------------------------------------------------------------*/
__STATIC_FORCEINLINE uint32_t parity32(uint32_t num) {
  num ^= num >> 16;
  num ^= num >> 8;
  num ^= num >> 4;
  num ^= num >> 2;
  num ^= num >> 1;
  return num & 0x01;
}

/*-----------------------------------------------------------------------------
 *      Odd and even Hamming parity calculation of a 32-bit number
 *----------------------------------------------------------------------------*/
__STATIC_FORCEINLINE uint32_t ham32bit(uint32_t sum) {
  uint32_t v1, v2, v3, h;

  v1  = sum ^ (sum >> 16);
  v1 ^= v1 >> 8;
  v2  = v1 ^ (v1 >> 4);

  v3 = v2 ^ (v2 >> 2);
  h  = (v3 << 15) & 0x010000;    // P1o == bit(16)
  h |= (v3 & 0x01);              // P1e == bit(0)

  v3 = v2 ^ (v2 >> 1);
  h |= (v3 << 15) & 0x020000;    // P2o == bit(17)
  h |= (v3 << 1) & 0x02;         // P2e == bit(1)

  v1 ^= v1 >> 2;
  v1 ^= v1 >> 1;
  h |= (v1 << 14) & 0x040000;    // P4o == bit(18)
  h |= (v1 << 2) & 0x04;         // P4e == bit(2)

  v1  = sum ^ (sum >> 4);
  v1 ^= v1 >> 2;
  v1 ^= v1 >> 1;
  v2 = v1 ^ (v1 >> 16);
  h |= (v2 << 11) & 0x080000;    // P8o == bit(19)
  h |= (v2 << 3) & 0x0008;       // P8e == bit(3)

  v2 = v1 ^ (v1 >> 8);
  h |= (v2 << 4) & 0x100000;     // P16o == bit(20)
  h |= (v2 << 4) & 0x0010;       // P16e == bit(4)

  return h;
}

/*-----------------------------------------------------------------------------
 *      12 bit Hamming for a 8 bytes of data
 *
 * Input data organization:
 *  ----------------------
 *  bit: 31 30 ... 2 1 0
 *  ----------------------------------------------------------------
 *                                       Row parity bits:
 *  ...      Word   0         |               P1e
 *  ...      Word   1         |               P1o
 *  ----------------------------------------------------------------
 *    Column parity bits:     |
 *    P16e P8e P4e P2e P1e    |         e == even
 *                            |
 *    P16o P8o P4o P2o P1o    |         o == odd
 *  ----------------------------------------------------------------
 *
 * Output 16 bit word organization:
 * --------------------------------------------------------------
 *     |Zero bits| Odd parity bits  |Zero bits| Even parity bits |
 * bit:| 15  14  |  13 |12   ...   8|  7  6   |  5  | 4  ...  0  |
 * ---------------------------------------------------------------
 * par:| 0   0   | P1o |P16o ... P1o|  0  0   | P1e |P16e ... P1e|
 *     | 0   0   | Row |   Column   |  0  0   | Row |   Column   |
 *----------------------------------------------------------------------------*/
static uint32_t ecc_Hamming8 (uint8_t *dataBuf, uint8_t *eccBuf) {
  uint32_t i, colSum, rowSum;
  uint32_t h, val;

  colSum = 0;
  rowSum = 0;

  for (i = 0; i < 2; i++, dataBuf += 4) {
    val = __UNALIGNED_UINT32_READ (dataBuf);

    colSum ^= val;
    rowSum |= parity32(val) << i;
  }
  /* Column parities */
  h = ham32bit (colSum);

  eccBuf[0] = (uint8_t)((h         | (rowSum << 5)) & 0x3F);    //even byte
  eccBuf[1] = (uint8_t)(((h >> 16) | ((rowSum & 0x02) << 4)));  //odd byte

  return 2;
}

/*-----------------------------------------------------------------------------
 *      Verify 12 bit Hamming for a 8 bytes of data
 *----------------------------------------------------------------------------*/
static uint32_t ecc_Hamming8Verify (uint8_t *dataBuf, uint8_t *eccBuf) {
  uint8_t ecc[2];
  uint32_t err, xor, addr, row, val;

  ecc_Hamming8(dataBuf, ecc);
  xor = ecc[0] ^ ecc[1] ^ eccBuf[0] ^ eccBuf[1];
  err = 0;
  if (xor == 0x3F) {
    /* Correctable single bit error */
    addr = ecc[1] ^ eccBuf[1];
    row = addr >> 5;

    val = __UNALIGNED_UINT32_READ (&dataBuf[row << 2]) ^ (1 << (addr & 0x1F));
    __UNALIGNED_UINT32_WRITE (&dataBuf[row << 2], val);

    err = ECC_SINGLEBITERR;
  }
  else if (xor != 0x00) {
    /* Uncorrectable error */
    err = ECC_MULTIBITERR;
  }
  else {
    err = ECC_NOERR;
  }

  return err;
}

/*-----------------------------------------------------------------------------
 *      24 bit Hamming for a 512 byte chunk of data
 *
 * Input data organization:
 *  ----------------------
 *  bit: 31 30 ... 2 1 0
 *  ----------------------------------------------------------------
 *  ...      Word   0         |
 *  ...      Word   1         |          Row parity bits:
 *  ...        ...            | P64e  P32e  P16e  P8e  P4e  P2e  P1e
 *  ...        ...            |
 *  ...        ...            | P64o  P32o  P16o  P8o  P4o  P2o  P1o
 *  ...      Word 127         |
 *  ----------------------------------------------------------------
 *    Column parity bits:     |
 *    P16e P8e P4e P2e P1e    |         e == even
 *                            |
 *    P16o P8o P4o P2o P1o    |         o == odd
 *  ----------------------------------------------------------------
 *
 * Output 32 bit word organization:
 * ------------------------------------------------------------------------
 *     |  Zero bits  |       Odd parity bits     |    Even parity bits     |
 * bit:| 31  ...  24 | 23  ...  17 | 16  ...  12 | 11  ...  5 | 4  ...  0  |
 * -------------------------------------------------------------------------
 * par:|  0  ...   0 |P64o ... P1o |P16o ... P1o |P64e ... P1e|P16e ... P1e|
 *                   |     Row     |   Column    |    Row     |   Column   |
 *----------------------------------------------------------------------------*/
static uint32_t ecc_Hamming512 (uint8_t *dataBuf, uint8_t *eccBuf) {
  uint32_t i, colSum, rowSum0, rowSum1, rowSum2, rowSum3;
  uint32_t h, val;

  colSum  = 0;
  rowSum0 = 0;
  rowSum1 = 0;
  rowSum2 = 0;
  rowSum3 = 0;

  for (i = 0; i < 32; i++, dataBuf += 4) {
    val = __UNALIGNED_UINT32_READ (dataBuf);

    /* Column sum */
    colSum ^= val;

    /* Row sum */
    rowSum0 |= parity32(val) << (i & 0x1F);
  }
  for (; i < 64; i++, dataBuf += 4) {
    val = __UNALIGNED_UINT32_READ (dataBuf);

    /* Column sum */
    colSum ^= val;

    /* Row sum */
    rowSum1 |= parity32(val) << (i & 0x1F);
  }
  for (; i < 96; i++, dataBuf += 4) {
    val = __UNALIGNED_UINT32_READ (dataBuf);

    /* Column sum */
    colSum ^= val;

    /* Row sum */
    rowSum2 |= parity32(val) << (i & 0x1F);
  }
  for (; i < 128; i++, dataBuf += 4) {
    val = __UNALIGNED_UINT32_READ (dataBuf);

    /* Column sum */
    colSum ^= val;

    /* Row sum */
    rowSum3 |= parity32(val) << (i & 0x1F);
  }

  /* Column parity bits: P16, P8, P4, P2, P1 */
  h = ham32bit (colSum);

  /* Row parity bits: P64, P32, P16, P8, P4, P2, P1 */

  /* Row: P64e */
  val = rowSum1 ^ rowSum0;
  h |= parity32 (val) << 11; // even bit(11)
  /* Row: P64o */
  val = rowSum3 ^ rowSum2;
  h |= parity32 (val) << 27; // odd  bit(27)

  /* Row: P32e */
  val = rowSum2 ^ rowSum0;
  h |= parity32 (val) << 10; // even bit(10)

  /* Row: P32o */
  val = rowSum3 ^ rowSum1;
  h |= parity32 (val) << 26; // odd  bit(26)

  /* Row: P16, P8, P4, P2, P1 */
  val = rowSum3 ^ rowSum2 ^ rowSum1 ^ rowSum0;
  h |= ham32bit (val) << 5;

  eccBuf[0] = (uint8_t)(h);
  eccBuf[1] = (uint8_t)(((h & 0x000F0000) >> 12) | (h >> 8));
  eccBuf[2] = (uint8_t)(h >> 20);

  return 3;
}

/*-----------------------------------------------------------------------------
 *      Verify 24 bit Hamming ECC for a 512 bytes chunk of data
 *----------------------------------------------------------------------------*/
static uint32_t ecc_Hamming512Verify (uint8_t *dataBuf, uint8_t *eccBuf) {
  uint8_t ecc[3];
  int32_t newEcc, oldEcc, xorEcc;
  uint32_t err, addr, row, val;

  ecc_Hamming512(dataBuf, ecc);
  newEcc = ecc[0]    | (ecc[1]    << 8) | (ecc[2]    << 16);
  oldEcc = eccBuf[0] | (eccBuf[1] << 8) | (eccBuf[2] << 16);

  xorEcc  = newEcc ^ oldEcc;
  xorEcc ^= xorEcc >> 12;
  xorEcc &= 0x0FFF;

  err = 0;

  if (xorEcc == 0x0FFF) {
    /* Correctable single bit error */
    addr = (uint32_t)((newEcc >> 12) ^ (oldEcc >> 12));
    row  = addr >> 5;

    /* Flip bit in calculated row */
    val = __UNALIGNED_UINT32_READ (&dataBuf[row << 2]) ^ (1UL << (addr & 0x1F));
    __UNALIGNED_UINT32_WRITE (&dataBuf[row << 2], val);

    err = ECC_SINGLEBITERR;
  }
  else if (xorEcc != 0x00) {
    /* Uncorrectable error */
    err = ECC_MULTIBITERR;
  }
  else {
    err = ECC_NOERR;
  }
  return err;
}

/*-----------------------------------------------------------------------------
 *     Encode Error Correction Code
 *
 * *ftl   = FTL instance pointer
 * *pgBuf = page buffer
 *
 *  Returns: none
 *----------------------------------------------------------------------------*/
static void EncodeECC (NAND_FTL_DEV *ftl, uint8_t *pgBuf) {
  uint32_t sec, ecc_len;
  uint8_t *dp, *sp;

  dp = pgBuf;
  sp = dp + ftl->PgLay.spare_ofs;

  for (sec = 0; sec < ftl->PageSectors; sec++) {
    ecc_len = ecc_Hamming512 (dp, sp + ftl->PgLay.spare.ofs_ecc);
    ecc_Hamming8 (sp, sp + ftl->PgLay.spare.ofs_ecc + ecc_len);

    /* Point to data and spare of the next sector */
    dp += ftl->PgLay.sector_inc;
    sp += ftl->PgLay.spare_inc;
  }
}

/*-----------------------------------------------------------------------------
 *     Decode Error Correction Code
 *
 * *ftl   = FTL instance pointer
 * *pgBuf = page buffer
 *
 *  Returns:  FTL_OK             - No ECC error
 *            FTL_ERROR_ECC_COR  - ECC corrected the data
 *            FTL_ERROR_ECC_FAIL - ECC was not able to correct the data
 *----------------------------------------------------------------------------*/
static uint32_t DecodeECC (NAND_FTL_DEV *ftl, uint8_t *pgBuf) {
  uint32_t sec, ecc, err_s, err_d;
  uint8_t *dp, *sp, *ep;

  dp = pgBuf;
  sp = dp + ftl->PgLay.spare_ofs;

  ecc = ECC_NOERR;
  for (sec = 0; sec < ftl->PageSectors; sec++) {
    /* Set ECC position pointer */
    ep = sp + ftl->PgLay.spare.ofs_ecc;
    /* Detect empty page */
    if ((ep[3] & 0xC0) == 0xC0) {
      /* This is an empty page */
      ep[0] = 0; ep[1] = 0; ep[2] = 0; ep[3] = 0; ep[4] = 0;
    }
    /* Verify data using ECC */
    err_s = ecc_Hamming8Verify   (sp, ep + 3);
    err_d = ecc_Hamming512Verify (dp, ep);

    /* Point to data and spare of the next sector */
    dp += ftl->PgLay.sector_inc;
    sp += ftl->PgLay.spare_inc;

    if ((err_s == ECC_SINGLEBITERR) || (err_d == ECC_SINGLEBITERR)) {
      ecc |= ECC_CORRECTED;
    }
    else if ((err_s == ECC_MULTIBITERR) || (err_d == ECC_MULTIBITERR)) {
      ecc |= ECC_UNCORRECTED;
    }
  }
  if (ecc & ECC_UNCORRECTED) {
    return FTL_ERROR_ECC_FAIL;
  }
  if (ecc & ECC_CORRECTED) {
    return FTL_ERROR_ECC_COR;
  }
  return FTL_OK;
}
