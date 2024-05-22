/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_ram_drv.c 
 * Purpose: RAM driver for FAT File System
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "fs_core.h"


/**
  Determine RAM sector address

  \param[in]   sect     sector number
  \param[in]   base     RAM device absolute base address
  \return absolute sector address
*/
static uint8_t *ram_sec_addr (uint32_t sect, uint8_t *base) {
  return (&base[512U * sect]);
}


/**
  Initialize RAM device interface

  \param[in]   mode     initialization mode
  \param[in]   ram      pointer to RAM device instance object
  \return true if initialization passed, false otherwise
*/
uint32_t ram_Init (uint32_t mode, RAM_DEV *ram) {
  (void)mode;
  (void)ram;

  return (true);
}


/**
  Uninitialize RAM device interface

  \param[in]   mode     uninitialization mode
  \param[in]   ram      pointer to RAM device instance object
  \return true on success, false otherwise
*/
uint32_t ram_UnInit (uint32_t mode, RAM_DEV *ram) {
  (void)mode;
  (void)ram;

  return (true);
}


/**
  Read a cnt of 512 byte sectors from RAM device

  \param[in]   sect     sector number
  \param[out]  buf      sector data buffer
  \param[in]   cnt      number of sectors to write
  \param[in]   ram      pointer to RAM device instance object
  \return true if read succeeded, false otherwise
*/
uint32_t ram_ReadSector (uint32_t sect, uint8_t *buf, uint32_t cnt, RAM_DEV *ram) {
  uint8_t *addr = ram_sec_addr (sect, ram->Base);
  uint32_t rval;

  if (buf == NULL) {
    rval = false;
  } else {
    memcpy (buf, addr, 512 * cnt);
    rval = true;
  }

  return (rval);
}


/**
  Write a cnt of 512 byte sectors to RAM device

  \param[in]   sect     sector number
  \param[in]   buf      sector data buffer
  \param[in]   cnt      number of sectors to write
  \param[in]   ram      pointer to RAM device instance object
  \return true if write succeeded, false otherwise
*/
uint32_t ram_WriteSector (uint32_t sect, const uint8_t *buf, uint32_t cnt, RAM_DEV *ram) {
  uint8_t *addr = ram_sec_addr (sect, ram->Base);
  uint32_t rval;

  if (buf == NULL) {
    rval = false;
  } else {
    memcpy (addr, buf, 512 * cnt);
    rval = true;
  }

  return (rval);
}


/**
  Read storage device configuration

  \param[out]  info     pointer to \ref FS_MEDIA_INFO object
  \param[in]   ram      pointer to RAM device instance object
  \return true on success, false otherwise
*/
uint32_t ram_ReadInfo (fsMediaInfo *info, RAM_DEV *ram) {

  info->block_cnt  = ram->Size / 512U;
  info->read_blen  = 512U;
  info->write_blen = 512U;

  return (true);
}


/**
  Process given device control command

  \param[in]    code device control command code
  \param[inout] p       pointer to command code argument
  \param[in]    ram     pointer to RAM device instance object
  \return fsStatus
*/
fsStatus ram_DevCtrl (fsDevCtrlCode code, void *p, RAM_DEV *ram) {
  fsStatus status;

  (void)ram;

  status = fsError;

  if (code == fsDevCtrlCodeCheckMedia) {
    if (p != NULL) {
      *(uint32_t *)p = FS_MEDIA_NOCHKMEDIA;
      status = fsOK;
    }
  }
  else {
    if (code == fsDevCtrlCodeControlMedia) {
      /* Control media */
      if (p != NULL) {
        if (*(uint32_t *)p == FS_CONTROL_MEDIA_INIT) {
          status = fsOK;
        }
      }
    }
  }

  return (status);
}
