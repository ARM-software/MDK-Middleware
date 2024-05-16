/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_nor_media.c
 * Purpose: NOR Flash media handling implementation
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "cmsis_compiler.h"

#include "fs_nor_flash.h"
#include "fs_evr.h"


/**
  Callback function to handle Flash driver events (IRQ context).

  \param[in]  event                     Flash driver event
  \param[in]  nor                       media description structure
*/
void nor_DrvEvent (uint32_t event, NOR_MEDIA *nor) {
  nor->Event |= (uint8_t)event;
}


/**
  Get sector start address.

  \param[in]  sector                    sector number
  \param[in]  nor                       media description structure
  \return     sector start address
*/
static uint32_t get_sector_address (uint32_t sector, NOR_MEDIA *nor) {
  FLASH_DEVICE *dev = &nor->Device;
  uint32_t addr;

  if (dev->SectArray != NULL) {
    /* Array specified sector layout */
    addr = dev->SectArray[sector].start;
  } else {
    /* Uniform sector size */
    addr = sector * dev->SectSize;
  }

  return (addr);
}

/**
  Get sector size.

  \param[in]  sector                    sector number
  \param[in]  nor                       media description structure
  \return     sector size in bytes
*/
static uint32_t get_sector_size (uint32_t sector, NOR_MEDIA *nor) {
  FLASH_DEVICE *dev = &nor->Device;
  uint32_t addr;

  if (dev->SectArray != NULL) {
    /* Array specified sector layout */
    addr = (dev->SectArray[sector].end + 1U) - dev->SectArray[sector].start;
  } else {
    /* Uniform sector size */
    addr = dev->SectSize;
  }

  return (addr);
}

/**
  Sector erase using device native commands.

  \param[in]  sector                    sector number
  \param[in]  nor                       media description structure
  \return     execution status \ref fsStatus
*/
static fsStatus sector_erase (uint32_t sector, NOR_MEDIA *nor) {
  ARM_FLASH_STATUS stat;
  fsStatus rstat;
  int32_t rval;
  uint32_t addr, tout;

  rstat = fsOK;

  addr = get_sector_address (sector, nor);

  rval = nor->Driver->EraseSector (addr);

  if (rval == ARM_DRIVER_OK) {
    /* Wait while busy */
    for (tout = nor->Device.Timeout->EraseSector*1000U; tout; tout -= 10U) {
      /* Read device status */
      stat = nor->Driver->GetStatus();

      if (stat.error == 1U) { break; }
      if (stat.busy  == 0U) { break; }
      fs_set_rtos_delay (10U);
    }

    if (tout == 0U) {
      rstat = fsMediaError;
    }
    else {
      if (stat.error == 1U) {
        rstat = fsMediaError;
      }
    }
  }
  else /* rval != ARM_DRIVER_OK */ {
    rstat = fsDriverError;
  }

  return (rstat);
}

/**
  Erase chip using device native commands.

  \param[in]  nor                       media description structure
  \return     execution status \ref fsStatus
*/
static fsStatus chip_erase_cmd (NOR_MEDIA *nor) {
  ARM_FLASH_STATUS stat;
  fsStatus rstat;
  int32_t  rval;
  uint32_t tout;

  rstat = fsOK;

  /* Use chip erase function */
  rval = nor->Driver->EraseChip ();

  if (rval == ARM_DRIVER_OK) {
    /* Wait while busy */
    for (tout = nor->Device.Timeout->EraseChip*1000U; tout; tout -= 10U) {
      /* Read device status */
      stat = nor->Driver->GetStatus();

      if (stat.error == 1U) { break; }
      if (stat.busy  == 0U) { break; }
      fs_set_rtos_delay(10U);
    }

    if (tout == 0U) {
      rstat = fsMediaError;
    }
    else {
      if (stat.error == 1U) {
        rstat = fsMediaError;
      }
    }
  }
  else /* rval != ARM_DRIVER_OK */ {
    rstat = fsDriverError;
  }

  return (rstat);
}

/**
  Erase flash chip.

  \param[in]  nor                       media description structure
  \return     execution status \ref fsStatus
*/
static fsStatus chip_erase (NOR_MEDIA *nor) {
  fsStatus status, stat;
  uint32_t sector;

  if (nor->Capabilities.erase_chip) {
    /* Use chip erase command */
    status = chip_erase_cmd (nor);
  }
  else {
    status = fsOK;

    /* Erase sectors one by one */
    for (sector = 0U; sector < nor->Device.SectCnt; sector++) {
      /* Use single sector erase */
      stat = sector_erase (sector, nor);
      
      if (stat != fsOK) {
        status = stat;
        break;
      }
    }
  }

  return (status);
}


/* ------------------------- Exported functions ----------------------------- */


/**
  Initialize NOR media layer and flash driver.

  \param[in]  nor                       media description structure
  \return     execution status \ref fsStatus
*/
fsStatus nor_Init (NOR_MEDIA *nor) {
  fsStatus status;
  ARM_FLASH_INFO *info;

  nor->Status = 0U;
  nor->Capabilities = nor->Driver->GetCapabilities();

  status = fsOK;

  if (nor->Driver->Initialize (nor->Callback) != ARM_DRIVER_OK) {
    status = fsDriverError;
  }
  else if (nor->Driver->PowerControl (ARM_POWER_FULL) != ARM_DRIVER_OK) {
    status = fsDriverError;
  }
  else {
    info = nor->Driver->GetInfo();

    nor->Device.SectCnt   = info->sector_count;
    nor->Device.SectSize  = info->sector_size;
    nor->Device.SectArray = info->sector_info;
  }

  nor->Status = 1U;

  return (status);
}

/**
  Initialize NOR media layer and flash driver.

  \param[in]  nor                       media description structure
  \return     execution status \ref fsStatus
*/
fsStatus nor_Uninit (NOR_MEDIA *nor) {

  /* Invalidate status */
  nor->Status = 0U;

  /* Power off and uninitialize the driver */
  nor->Driver->PowerControl (ARM_POWER_OFF);

  /* Uninitialize driver */
  nor->Driver->Uninitialize ();

  return (fsOK);
}


/**
  Retrieve sector start address.

  \param[in]  sector                    sector number
  \param[in]  nor                       media description structure
  \return     sector start address
*/
uint32_t nor_GetSectorAddress (uint32_t sector, NOR_MEDIA *nor) {
  uint32_t addr;
  
  addr = get_sector_address (sector, nor);

  return (addr);
}

/**
  Retrieve sector size.

  \param[in]  sector                    sector number
  \param[in]  nor                       media description structure
  \return     sector size in bytes
*/
uint32_t nor_GetSectorSize (uint32_t sector, NOR_MEDIA *nor) {
  uint32_t size;

  size = get_sector_size (sector, nor);

  return (size);
}


/**
  Write data to flash sector.

  Argument addr is always 4-byte aligned.

  \param[in]  addr                      data address
  \param[in]  buf                       data buffer
  \param[in]  cnt                       number of bytes to write
  \param[in]  nor                       media description structure
  \return     execution status \ref fsStatus
*/
fsStatus nor_DataWrite (uint32_t addr, const void *buf, uint32_t cnt, NOR_MEDIA *nor) {
  ARM_FLASH_STATUS stat;
  uint32_t tout;
  uint32_t n, num;
  int32_t  rval;
  uint8_t  data[4];

  /* First write the part that is a multiple of 4 */
  num = cnt & ~3U;

  if (num > 0U) {
    rval = nor->Driver->ProgramData (addr, buf, num >> nor->Capabilities.data_width);

    if (rval < 0) {
      return (fsError);
    }

    stat.busy  = 0U;
    stat.error = 0U;

    /* Wait while busy */
    for (tout = nor->Device.Timeout->Program*1000; tout; tout -= 2) {
      /* Read device status */
      stat = nor->Driver->GetStatus();

      if ((stat.error == 1U) || (stat.busy == 0U)) {
        break;
      }

      fs_set_rtos_delay (2);
    }

    if (stat.error == 1U) {
      return (fsError);
    }
  }

  if (num < cnt) {
    /* Write remaining part of data */
    for (n = 0U; n < 4U; n++) {
      if ((num + n) < cnt) {
        data[n] = ((const uint8_t *)buf)[num + n];
      }
      else {
        data[n] = 0U;
      }
    }

    rval = nor->Driver->ProgramData (addr + num, data, n >> nor->Capabilities.data_width);

    /* Wait while busy */
    for (tout = nor->Device.Timeout->Program*1000; tout; tout -= 2) {
      /* Read device status */
      stat = nor->Driver->GetStatus();

      if ((stat.error == 1U) || (stat.busy == 0U)) {
        break;
      }

      fs_set_rtos_delay (2);
    }

    if (stat.error == 1U) {
      return (fsError);
    }
  }

  return (fsOK);
}


/**
  Read data from flash sector.

  Arguments addr and buf may be unaligned.

  \param[in]  addr                      data address
  \param[in]  buf                       data buffer
  \param[in]  cnt                       number of bytes to read
  \param[in]  nor                       media description structure
  \return     execution status \ref fsStatus
*/
fsStatus nor_DataRead (uint32_t addr, void *buf, uint32_t cnt, NOR_MEDIA *nor) {
  ARM_FLASH_STATUS stat;
  uint32_t tout;
  uint32_t abuf, acnt, aoffs;
  int32_t  rval;
  uint8_t *pb = (uint8_t *)buf;

  /* Check for unaligned flash address */
  if ((nor->Capabilities.data_width == 1) && (addr & 1)) {
    /* 16-bit access and unaligned address */
    aoffs =  8U;
    acnt  =  1U;
    addr &= ~1U;
  }
  else if ((nor->Capabilities.data_width == 2) && (addr & 3)) {
    /* 32-bit access and unaligned address */
    aoffs =  8U * (addr & 3U);
    acnt  =  4U - (addr & 3U);
    addr &= ~3U;
  }
  else {
    /* Address is aligned */
    aoffs = 0U;
    acnt  = 0U;
  }

  if (acnt) {
    /* Read from unaligned flash address */
    rval = nor->Driver->ReadData (addr, &abuf, 1);
    if (rval < 0) {
      return (fsError);
    }

    /* Check status and/or wait until busy */
    for (tout = nor->Device.Timeout->Read*1000; tout; tout -= 2) {
      stat = nor->Driver->GetStatus();
      if (stat.error) {
        return (fsError);
      }
      if (!stat.busy) { break; }
      fs_set_rtos_delay (2);
    }
    if (tout == 0) {
      //EvrFsEFS_FlashReadTimeout (vol->DrvLet, addr, &abuf, 1);
      return (fsError);
    }

    addr += (2 * nor->Capabilities.data_width);
    cnt  -= acnt;

    /* Append unaligned data */
    while (acnt) {
      *pb++ = (uint8_t)(abuf >> aoffs);

      aoffs += 8;
      acnt  -= 1;
    }
  }

  /* Aligned read */
  rval = nor->Driver->ReadData (addr, pb, cnt >> nor->Capabilities.data_width);
  if (rval < 0) {
    return (fsError);
  }

  /* Check status and/or wait until busy */
  for (tout = nor->Device.Timeout->Read*1000; tout; tout -= 2) {
    stat = nor->Driver->GetStatus();
    if (stat.error) {
      return (fsError);
    }
    if (!stat.busy) { break; }
    fs_set_rtos_delay (2);
  }
  if (tout == 0) {
    return (fsError);
  }

  /* Check for unaligned data count */
  if      (nor->Capabilities.data_width == 0) { acnt = 0;       } /*  8-bit access */
  else if (nor->Capabilities.data_width == 1) { acnt = cnt & 1; } /* 16-bit access */
  else                                        { acnt = cnt & 3; } /* 32-bit access */

  if (acnt) {
    /* Nonaligned data read */
    cnt -= acnt;

    rval = nor->Driver->ReadData (addr + cnt, &abuf, 1);
    if (rval < 0) {
      return (fsError);
    }

    /* Check status and/or wait until busy */
    for (tout = nor->Device.Timeout->Read*1000; tout; tout -= 2) {
      stat = nor->Driver->GetStatus();
      if (stat.error) {
        return (fsError);
      }
      if (!stat.busy) { break; }
      fs_set_rtos_delay (2);
    }
    if (tout == 0) {
      return (fsError);
    }

    /* Append nonaligned data */
    for (aoffs = 0; acnt; acnt--, aoffs += 8) {
      pb[cnt++] = (uint8_t)(abuf >> aoffs);
    }
  }

  return (fsOK);
}


/**
  Erase flash block.

  \param[in]  sector                    sector to erase
  \param[in]  nor                       media description structure
  \return     execution status \ref fsStatus
*/
fsStatus nor_SectorErase (uint32_t sector, NOR_MEDIA *nor) {
  fsStatus status;

  /* Erase specified sector */
  status = sector_erase (sector, nor);

  return (status);
}


/**
  Process given device control command

  \param[in]      code                  device control command code
  \param[in,out]  p                     command argument
  \param[in]      nor                   media description structure
  \return     execution status \ref fsStatus
*/
fsStatus nor_DevCtrl (fsDevCtrlCode code, void *p, NOR_MEDIA *nor) {
  ARM_FLASH_INFO *inf;
  fsStatus status;
  uint32_t id;

  status = fsError;

  id = (uint32_t)code;

  switch (id) {
    case fsDevCtrlCodeCheckMedia:
      if (p != NULL) {
        *(uint32_t *)p = FS_MEDIA_NOCHKMEDIA;
        status = fsOK;
      }
      break;

    /* Perform a low level format */
    case fsDevCtrlCodeFormat:
      /* Note: parameter p is ignored */
      status = chip_erase (nor);
      break;

    /* Retrieve total number of blocks on the volume */
    case fsDevCtrlCodeBlockCount:
      if (p != NULL) {
        *(uint32_t *)p = nor->Device.SectCnt;

        status = fsOK;
      }
      break;

    /* Retrieve the value of erased memory content   */
    case fsDevCtrlCodeErasedValue:
      if (p != NULL) {
        inf = nor->Driver->GetInfo();

        if (inf != NULL) {
          *(uint32_t *)p = inf->erased_value * 0x01010101;

          status = fsOK;
        }
      }
      break;

    /* Retrieve smallest programmable unit in bytes  */
    case fsDevCtrlCodeProgramUnit:
      if (p != NULL) {
        inf = nor->Driver->GetInfo();

        if (inf != NULL) {
          *(uint32_t *)p = inf->program_unit;

          status = fsOK;
        }
      }
      break;

    case fsDevCtrlCodeControlMedia:
    case fsDevCtrlCodeGetCID:
    case fsDevCtrlCodeSerial:
    default:
      status = fsUnsupported;
      break;
  }

  return (status);
}
