/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_ioc.c 
 * Purpose: Media IO Control API Functions
 *----------------------------------------------------------------------------*/

#include "fs_common.h"
#include "fs_ioc.h"
#include "fs_fat.h"
#include "fs_evr.h"

/** \page FS_MISRA_Exceptions  MISRA-C:2004 Compliance Exceptions
  IOC violates the following MISRA-C:2004 rules:

  \li Advisory Rule 6.3, use typedefs in place of the basic types<br>
     Type 'char' is used to represent ASCII character constants or string literals.

  \li Required Rule 10.1, implicit conversion changes signedness
     From the comparison of uint32_t and boolean.

  \li Advisory Rule 11.4, cast from pointer to pointer
     To ensure buffer alignment cast is used from (uint32_t *) to (uint8_t *)
*/


/**
  \brief Check if valid drive is specified and return its ID.
  \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
  \return     drive ID or execution status
                - value >= 0: drive ID as an integer when specified drive exists and allows IOC access
                - value < 0: error occurred, -value is execution status as defined with \ref fsStatus
*/
int32_t fs_ioc_get_id (const char *drive) {
  int32_t id, val;
  const char def[3] = "";
  const char *p;

  EvrFsIOC_GetId (get_u32((const uint8_t *)drive));

  p = drive;
  if (p == NULL) { p = def; }

  id = fs_drive_id (p, NULL);
  if (id < 0) {
    /* Nonexistent drive or input invalid */
    EvrFsIOC_GetIdError (get_u32((const uint8_t *)drive));
    val = id;
  }
  else if (fs_DevPool[id].attr & FS_IOC) {
    /* IOC attribute is set for this drive */
    EvrFsIOC_GetIdSuccess (get_u32((const uint8_t *)fs_DevPool[id].id), id);
    val = id;
  }
  else {
    /* I/O control is not allowed */
    EvrFsIOC_NotAllowed (get_u32((const uint8_t *)fs_DevPool[id].id));
    val = -(int32_t)fsAccessDenied;
  }

  return (val);
}


/**
  \brief Lock drive and block media access to the upper layer.
  \param[in]  drv_id                   Drive identifier obtained by \ref fs_ioc_get_id.
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsInvalidDrive     = Nonexistent drive id specified.
                - fsAccessDenied     = IOC access not allowed for specified drive.
*/
fsStatus fs_ioc_lock (int32_t drv_id) {
  fsFAT_Volume *vol;
  fsStatus status;

  EvrFsIOC_Lock (drv_id);

  if ((drv_id < 0) || ((uint32_t)drv_id >= fs_ndrv)) {
    /* Invalid drive id */
    EvrFsIOC_DriveIdInvalid (drv_id);
    status = fsInvalidDrive;
  }
  else if (fs_DevPool[drv_id].attr & FS_IOC) {
    /* IOC attribute is set for this drive */
    if (fs_DevPool[drv_id].attr & FS_FAT) {
      vol = (fsFAT_Volume *)fs_DevPool[drv_id].dcb;

      vol->Status &= ~FAT_STATUS_READY;

      /* Invalidate data cache */
      vol->ca.sect = INVAL_SECT;
      vol->ca.nrd  = 0U;
      vol->ca.nwr  = 0U;

      /* Drive locked */
      EvrFsIOC_LockSuccess (drv_id);
      status = fsOK;
    } else {
      /* Id does not specify FAT drive */
      EvrFsIOC_DriveNotFAT (drv_id);
      status = fsError;
    }
  }
  else {
    /* I/O control is not allowed */
    EvrFsIOC_NotAllowed (get_u32((const uint8_t *)fs_DevPool[drv_id].id));
    status = fsAccessDenied;
  }

  return (status);
}


/**
  \brief Unlock drive and allow media access to the upper layer.
  \param[in]  drv_id                   Drive identifier obtained by \ref fs_ioc_get_id.
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsInvalidDrive     = Nonexistent drive id specified.
                - fsAccessDenied     = IOC access not allowed for specified drive.
*/
fsStatus fs_ioc_unlock (int32_t drv_id) {
  fsFAT_Volume *vol;
  fsStatus status;

  EvrFsIOC_Unlock (drv_id);

  if ((drv_id < 0) || ((uint32_t)drv_id >= fs_ndrv)) {
    /* Invalid drive id */
    EvrFsIOC_DriveIdInvalid (drv_id);
    status = fsInvalidDrive;
  }
  else if (fs_DevPool[drv_id].attr & FS_IOC) {
    /* IOC attribute is set for this drive */
    if (fs_DevPool[drv_id].attr & FS_FAT) {
      vol = (fsFAT_Volume *)fs_DevPool[drv_id].dcb;

      vol->Status |= FAT_STATUS_READY;
      /* Drive unlocked */
      EvrFsIOC_UnlockSuccess (drv_id);
      status = fsOK;
    } else {
      /* Id does not specify FAT drive */
      EvrFsIOC_DriveNotFAT (drv_id);
      status = fsError;
    }
  }
  else {
    /* I/O control is not allowed */
    EvrFsIOC_NotAllowed (get_u32((const uint8_t *)fs_DevPool[drv_id].id));
    status = fsAccessDenied;
  }

  return (status);
}


/**
  \brief Return IOC cache buffer information.
  \param[in]  drv_id                   Drive identifier obtained by \ref fs_ioc_get_id.
  \param[out] cache_info               IOC Cache information.
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsInvalidParameter = Input parameters are not valid.
                - fsInvalidDrive     = Nonexistent drive id specified.
                - fsAccessDenied     = IOC access not allowed for specified drive.
*/
fsStatus fs_ioc_get_cache (int32_t drv_id, fsIOC_Cache *cache_info) {
  fsFAT_Volume *vol;
  fsStatus status;

  EvrFsIOC_GetCache (drv_id, (uint32_t)cache_info);

  if ((drv_id < 0) || ((uint32_t)drv_id >= fs_ndrv)) {
    /* Invalid drive id */
    EvrFsIOC_DriveIdInvalid (drv_id);
    status = fsInvalidDrive;
  }
  else if (cache_info == NULL) {
    status = fsInvalidParameter;
  }
  else if (fs_DevPool[drv_id].attr & FS_IOC) {
    vol = (fsFAT_Volume *)fs_DevPool[drv_id].dcb;

    cache_info->buffer = (uint8_t *)vol->CaBuf;
    cache_info->size   = (vol->CaSize + 2U) * 512U;

    EvrFsIOC_GetCacheSuccess (drv_id, (uint32_t)cache_info->buffer, cache_info->size);
    status = fsOK;
  }
  else {
    /* I/O control is not allowed */
    EvrFsIOC_NotAllowed (get_u32((const uint8_t *)fs_DevPool[drv_id].id));
    status = fsAccessDenied;
  }

  return (status);
}


/**
  \brief Read sector from media.
  \param[in]  drv_id                   Drive identifier obtained by \ref fs_ioc_get_id.
  \param[in]  sect                     Sector number.
  \param[out] buf                      Data buffer.
  \param[in]  cnt                      Count of sectors.
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsInvalidDrive     = Nonexistent drive id specified.
                - fsNoMedia          = Media not present.
                - fsAccessDenied     = IOC access not allowed for specified drive.
                - fsError            = Read sector failed.
*/
fsStatus fs_ioc_read_sector (int32_t drv_id, uint32_t sect, uint8_t *buf, uint32_t cnt) {
  fsFAT_Volume *vol;
  fsStatus status;

  EvrFsIOC_ReadSector (drv_id, sect, (uint32_t)buf, cnt);

  if ((drv_id < 0) || ((uint32_t)drv_id >= fs_ndrv)) {
    /* Invalid drive id */
    EvrFsIOC_DriveIdInvalid (drv_id);
    status = fsInvalidDrive;
  }
  else if (fs_DevPool[drv_id].attr & FS_IOC) {
    vol = (fsFAT_Volume *)fs_DevPool[drv_id].dcb;

    if (vol->Drv->ReadSect (sect, buf, cnt) == true) {
      EvrFsIOC_ReadSectorSuccess (drv_id, sect, (uint32_t)buf, cnt);
      status = fsOK;
    } else {
      EvrFsIOC_ReadSectorError (drv_id, sect, (uint32_t)buf, cnt);
      status = fsError;
    }
  }
  else {
    /* I/O control is not allowed */
    EvrFsIOC_NotAllowed (get_u32((const uint8_t *)fs_DevPool[drv_id].id));
    status = fsAccessDenied;
  }

  return (status);
}


/**
  \brief Write sector to media.
  \param[in]  drv_id                   Drive identifier obtained by \ref fs_ioc_get_id.
  \param[in]  sect                     Sector number.
  \param[out] buf                      Data buffer.
  \param[in]  cnt                      Count of sectors.
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsInvalidDrive     = Nonexistent drive id specified.
                - fsNoMedia          = Media not present.
                - fsAccessDenied     = IOC access not allowed for specified drive.
                - fsError            = Read sector failed.
*/
fsStatus fs_ioc_write_sector (int32_t drv_id, uint32_t sect, const uint8_t *buf, uint32_t cnt) {
  fsFAT_Volume *vol;
  fsStatus status;

  EvrFsIOC_WriteSector (drv_id, sect, (uint32_t)buf, cnt);

  if ((drv_id < 0) || ((uint32_t)drv_id >= fs_ndrv)) {
    /* Invalid drive id */
    EvrFsIOC_DriveIdInvalid (drv_id);
    status = fsInvalidDrive;
  }
  else if (fs_DevPool[drv_id].attr & FS_IOC) {
    vol = (fsFAT_Volume *)fs_DevPool[drv_id].dcb;

    if (vol->Drv->WriteSect (sect, buf, cnt) == true) {
      EvrFsIOC_WriteSectorSuccess (drv_id, sect, (uint32_t)buf, cnt);
      status = fsOK;
    } else {
      EvrFsIOC_WriteSectorError (drv_id, sect, (uint32_t)buf, cnt);
      status = fsError;
    }
  }
  else {
    /* I/O control is not allowed */
    EvrFsIOC_NotAllowed (get_u32((const uint8_t *)fs_DevPool[drv_id].id));
    status = fsAccessDenied;
  }

  return (status);
}


/**
  \brief Read media configuration info.
  \param[in]  drv_id                   Drive identifier obtained by \ref fs_ioc_get_id.
  \param[out] info                     Media information structure.
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsInvalidDrive     = Nonexistent drive id specified.
                - fsNoMedia          = Media not present.
                - fsAccessDenied     = IOC access not allowed for specified drive.
                - fsError            = Media information read failed.
*/
fsStatus fs_ioc_read_info (int32_t drv_id, fsMediaInfo *info) {
  fsFAT_Volume *vol;
  fsStatus status;

  EvrFsIOC_ReadInfo (drv_id, (uint32_t)info);

  if ((drv_id < 0) || ((uint32_t)drv_id >= fs_ndrv)) {
    /* Invalid drive id */
    EvrFsIOC_DriveIdInvalid (drv_id);
    status = fsInvalidDrive;
  }
  else if (fs_DevPool[drv_id].attr & FS_IOC) {
    vol = (fsFAT_Volume *)fs_DevPool[drv_id].dcb;

    if (vol->Drv->ReadInfo (info) == true) {
      EvrFsIOC_ReadInfoSuccess (drv_id, info->block_cnt);
      status = fsOK;
    } else {
      EvrFsIOC_ReadInfoError (drv_id);
      status = fsError;
    }
  }
  else {
    /* I/O control is not allowed */
    EvrFsIOC_NotAllowed (get_u32((const uint8_t *)fs_DevPool[drv_id].id));
    status = fsAccessDenied;
  }

  return (status);
}


/**
  \brief IOC device control access; control code is sent directly to device driver.
  \param[in]  drv_id                   Drive identifier obtained by \ref fs_ioc_get_id.
  \param[in]  code                     Device control code.
  \param[in,out] p                     Generic pointer.
  \return     execution status \ref fsStatus
*/
fsStatus fs_ioc_device_ctrl (int32_t drv_id, fsDevCtrlCode code, void *p) {
  fsFAT_Volume *vol;
  fsStatus status;

  EvrFsIOC_DeviceControl (drv_id, code, (uint32_t)p);

  if ((drv_id < 0) || ((uint32_t)drv_id >= fs_ndrv)) {
    /* Invalid drive id */
    EvrFsIOC_DriveIdInvalid (drv_id);
    status = fsInvalidDrive;
  }
  else if (fs_DevPool[drv_id].attr & FS_IOC) {
    vol = (fsFAT_Volume *)fs_DevPool[drv_id].dcb;

    status = vol->Drv->DeviceCtrl (code, p);

    #ifdef FS_DEBUG
    if (status == fsOK) {
      EvrFsIOC_DeviceControlSuccess (drv_id, code, (uint32_t)p);
    }
    #endif
  }
  else {
    /* I/O control is not allowed */
    EvrFsIOC_NotAllowed (get_u32((const uint8_t *)fs_DevPool[drv_id].id));
    status = fsAccessDenied;
  }

  return (status);
}
