/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_usbh_msc.c
 * Purpose: USB Host Mass Storage Interface
 *----------------------------------------------------------------------------*/

#include "fs_core.h"

/**
  Initialize USB Mass Storage interface

  \param[in]   mode       initialization mode
  \param[in]   instance   MSC device instance number
  \return true if initialization passed, false otherwise
*/
uint32_t usbh_msc_Init (uint32_t mode, uint32_t instance) {
  uint32_t status;

  if (mode == DM_IO) {
    status = 1U;
  }
  else {
    if (USBH_MSC_GetStatus (instance) == usbOK) {
      status = 1U;
    } else {
      status = 0U;
    }
  }

  return (status);
}

/**
  Uninitialize USB Mass Storage interface

  \param[in]   mode       uninitialization mode
  \param[in]   instance   MSC device instance number
  \return true on success, false otherwise
*/
uint32_t usbh_msc_UnInit (uint32_t mode, uint32_t instance) {
  (void)mode;
  (void)instance;

  return (1U);
}

/**
  Read a cnt of 512 byte sectors from USB Mass Storage

  \param[in]   sect       sector number
  \param[out]  buf        sector data buffer
  \param[in]   cnt        number of sectors to write
  \param[in]   instance   MSC device instance number
  \return true if read succeeded, false otherwise
*/
uint32_t usbh_msc_ReadSector (uint32_t sect, uint32_t cnt, uint8_t *buf, uint32_t instance) {
  uint32_t status;

  status = 0U;

  if (USBH_MSC_Read (instance, sect, cnt, buf) == usbOK) {
    status = 1U;
  }

  return (status);
}

/**
  Write a cnt of 512 byte sectors to memory card

  \param[in]   sect       sector number
  \param[in]   buf        sector data buffer
  \param[in]   cnt        number of sectors to write
  \param[in]   instance   MSC device instance number
  \return true if write succeeded, false otherwise
*/
uint32_t usbh_msc_WriteSector (uint32_t sect, uint32_t cnt, const uint8_t *buf, uint32_t instance) {
  uint32_t status;

  status = 0U;

  if (USBH_MSC_Write (instance, sect, cnt, buf) == usbOK) {
    status = 1U;
  }

  return (status);
}

/**
  Read USB Mass Storage configuration

  \param[out]  info       pointer to \ref FS_MEDIA_INFO object
  \param[in]   instance   MSC device instance number
  \return true on success, false otherwise
*/
uint32_t usbh_msc_ReadInfo (fsMediaInfo *info, uint32_t instance) {
  uint32_t status;
  uint32_t bcnt;
  uint32_t blen;

  /* Read mass storage device capacity */
  if (USBH_MSC_ReadCapacity (instance, &bcnt, &blen) == usbOK) {
    info->block_cnt  = bcnt;
    info->write_blen = (uint16_t)blen;
    info->read_blen  = (uint16_t)blen;

    status = 1U;
  }
  else {
    status = 0U;
  }

  return (status);
}

/**
  Process given device control command

  \param[in]    code       Device control command code
  \param[inout] p          Pointer to command code argument
  \param[in]    instance   MSC device instance number
  \return fsStatus
*/
fsStatus usbh_msc_DevCtrl (fsDevCtrlCode code, void *p, uint32_t instance) {
  fsStatus  status;
  usbStatus usb_status;

  if (code == fsDevCtrlCodeCheckMedia) {
    /* Check for mass storage device presence */
    usb_status = USBH_MSC_GetStatus (instance);

    if (p != NULL) {
      if (usb_status == usbOK) {
        *(uint32_t *)p = FS_MEDIA_INSERTED;
      } else {
        *(uint32_t *)p = 0U;
      }
    }
    status = fsOK;
  }
  else {
    /* Unsupported */
    status = fsUnsupported;
  }

  return (status);
}
