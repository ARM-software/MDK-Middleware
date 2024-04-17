/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device:MSC
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_MSC_0.c
 * Purpose: USB Device Mass Storage Device class (MSC) User module
 * Rev.:    V6.3.4
 *----------------------------------------------------------------------------*/
/**
 * \addtogroup usbd_mscFunctions
 *
 */
 
 
//! [code_USBD_User_MSC]
 
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rl_usb.h"
 
// If the USE_FILE_SYSTEM value is 1 then File System is used and media can
// be accessed from application code or can be accessed by USB Mass Storage
// requests, default media in that case is one selected as default in File
// System Configuration file.
// If the USE_FILE_SYSTEM value is 0 then File System is not used and media
// can only be accessed by USB Mass Storage requests, default media in that
// case is RAM memory containing dummy disk image.
#define USE_FILE_SYSTEM   0             // 1 = File System is used, 0 = File System is not used

// Definition MEDIA_DRIVE is used to define Drive to be used for media
// Available options are:
//   "R:" or "R0:" if media is RAM
//   "M:" or "M0:" if media is Memory Card 0
//           "M1:" if media is Memory Card 1
//   "N:" or "N0:" if media is NAND Flash 0
//           "N1:" if media is NAND Flash 1
#define MEDIA_DRIVE     "M0:"
 
 
#if (USE_FILE_SYSTEM == 1)              // If File System is used
#include "rl_fs.h"
#define  MEDIA_OWN_USB     (1U     )    // Media owned by USB (bit mask)
#define  MEDIA_OWN_CHG     (1U << 1)    // Media ownership change requested (bit mask)
extern 
volatile uint8_t  usbd_msc0_media_own;
volatile uint8_t  usbd_msc0_media_own;  // USB MSC0 media ownership
static   int32_t  drv_id;               // FAT drive id
static   bool     media_ok;             // Media is initialized and ok
#else
static   uint32_t memory   [8192/4];    // Memory in RAM for dummy disk image
static   uint32_t block_buf[ 512/4];    // Buffer for block read/write to media
extern
const    uint8_t  memory_disk_image[4096];  // Dummy Memory Disk Image
#endif
 
 
// Called during USBD_Initialize to initialize the USB MSC class instance.
void USBD_MSC0_Initialize (void) {
 
#if (USE_FILE_SYSTEM == 1)              // If File System is used
  uint32_t param_status;
 
  usbd_msc0_media_own = MEDIA_OWN_USB;  // Initially media is owned by USB
  media_ok            = false;          // Current media status (not initialized = not ok)
 
  if (finit (MEDIA_DRIVE) != fsOK) {    // Initialize File System
    return;                             // Exit if failed
  }
 
  drv_id = fs_ioc_get_id (MEDIA_DRIVE); // Get ID of media drive
  if (drv_id < 0)           { return; } // If ID is invalid exit
 
  param_status = 0U;                    // Parameter for function call is 0
                                        // Initialize media
  if (fs_ioc_device_ctrl (drv_id, fsDevCtrlCodeControlMedia, &param_status) != fsOK) {
    return;                             // Exit if failed
  }
 
  if (fs_ioc_lock (drv_id)) {           // Lock media for USB usage
    return;                             // Exit if failed
  }
 
  media_ok = true;                      // Media was initialized and is ok
#else
                                        // Copy the dummy image from code to RAM
  memcpy (memory, memory_disk_image, sizeof(memory_disk_image));
#endif
}
 
 
// \brief Called during USBD_Uninitialize to de-initialize the USB MSC class instance.
void USBD_MSC0_Uninitialize (void) {
  // Add code for de-initialization
}
 
 
// Get cache information.
// \param[out]    buffer               cache buffer address.
// \param[out]    size                 cache buffer size.
// \return        true                 operation succeeded.
// \return        false                operation failed.
bool USBD_MSC0_GetCacheInfo (uint32_t *buffer, uint32_t *size) {
 
#if (USE_FILE_SYSTEM == 1)              // If File System is used
  fsIOC_Cache cache_info;
 
  // Get cache settings of File System
  if (fs_ioc_get_cache(drv_id, &cache_info) != fsOK) {
    return false;                       // Exit if failed
  }
 
  // Use File Systems cache for MSC
  *buffer = (uint32_t)cache_info.buffer;// Cache buffer from File System
  *size   = cache_info.size;            // Cache size
#else
  *buffer = (uint32_t)block_buf;        // Local buffer for data
  *size   = sizeof(block_buf);          // Size of local buffer
#endif
 
  return true;
}
 
 
// Get media capacity.
// \param[out]    block_count          total number of blocks on media.
// \param[out]    block_size           media block size.
// \return        true                 operation succeeded.
// \return        false                operation failed.
bool USBD_MSC0_GetMediaCapacity (uint32_t *block_count, uint32_t *block_size) {
 
#if (USE_FILE_SYSTEM == 1)              // If File System is used
  fsMediaInfo media_info;
 
  // Read media information of actual media
  if (fs_ioc_read_info(drv_id, &media_info) != fsOK) {
    return false;                       // Exit if failed
  }
 
  *block_count = media_info.block_cnt;  // Total number of blocks on media
  *block_size  = media_info.read_blen;  // Block size of blocks on media
#else
  *block_count = sizeof(memory)/512U;   // Total number of blocks on media
  *block_size  = 512U;                  // Block size of blocks on media
#endif
 
  return true;
}
 
 
// Read data from media.
// \param[in]     lba                  logical address of first block to read.
// \param[in]     cnt                  number of contiguous blocks to read from media.
// \param[out]    buf                  data buffer for data read from media.
// \return        true                 read succeeded.
// \return        false                read failed.
bool USBD_MSC0_Read (uint32_t lba, uint32_t cnt, uint8_t *buf) {
 
#if (USE_FILE_SYSTEM == 1)              // If File System is used
                                        // Read data directly from media
  if (fs_ioc_read_sector (drv_id, lba, buf, cnt) != fsOK) {
    return false;
  }
#else
                                        // Read data from dummy image in RAM
  memcpy (buf, &memory[lba * (512U/4U)], cnt * 512U);
#endif
 
  return true;
}
 
 
// Write data to media.
// \param[in]     lba                  logical address of first block to write.
// \param[in]     cnt                  number of contiguous blocks to write to media.
// \param[out]    buf                  data buffer containing data to write to media.
// \return        true                 write succeeded.
// \return        false                write failed.
bool USBD_MSC0_Write (uint32_t lba, uint32_t cnt, const uint8_t *buf) {
 
#if (USE_FILE_SYSTEM == 1)              // If File System is used
                                        // Write data directly to media
  if (fs_ioc_write_sector (drv_id, lba, buf, cnt) != fsOK) {
    return false;
  }
#else
                                        // Write data to image in RAM
  memcpy (&memory[lba * (512U/4U)], buf, cnt * 512U);
#endif
 
  return true;
}
 
 
// Check media presence and write protect status.
//        (if media is not owned by USB it returns that media is not ready)
// \return                             media presence and write protected status
//                bit 1:               write protect bit
//                 - value 1:            media is write protected
//                 - value 0:            media is not write protected
//                bit 0:               media presence bit
//                 - value 1:            media is present
//                 - value 0:            media is not present
uint32_t USBD_MSC0_CheckMedia (void) {
 
#if (USE_FILE_SYSTEM == 1)              // If File System is used
       uint32_t param_status;
       uint8_t  media_state;            // Bit 0. media ready, Bit 1. media write protect
static uint8_t  media_ready_ex = 0U;    // Previous media ready state
       uint8_t  own;
 
  // Get current media status
  media_state = 0U;
  switch (fs_ioc_device_ctrl (drv_id, fsDevCtrlCodeCheckMedia, &param_status)) {
    case fsOK:
      if (param_status & FS_MEDIA_NOCHKMEDIA) {
        // If check media not available on hardware layer
        media_state  =  USBD_MSC_MEDIA_READY;
        break;
      }
      if (param_status & FS_MEDIA_INSERTED) {
        media_state  =  USBD_MSC_MEDIA_READY;
      }
      if (param_status & FS_MEDIA_PROTECTED) {
        media_state |=  USBD_MSC_MEDIA_PROTECTED;
      }
      break;
    case fsError:
    case fsUnsupported:
    case fsAccessDenied:
    case fsInvalidParameter:
    case fsInvalidDrive:
    case fsInvalidPath:
    case fsUninitializedDrive:
    case fsDriverError:
    case fsMediaError:
    case fsNoMedia:
    case fsNoFileSystem:
    case fsNoFreeSpace:
    case fsFileNotFound:
    case fsDirNotEmpty:
    case fsTooManyOpenFiles:
    case fsAlreadyExists:
    case fsNotDirectory:
      break;
  }
 
  // Store current owner so no new request can interfere
  own = usbd_msc0_media_own;
 
  // De-initialize media according to previous owner
  if (own & MEDIA_OWN_CHG) {                    // If owner change requested
    if (own & MEDIA_OWN_USB) {                  // If new requested owner is USB (previous owner was File System)
      (void)funmount (MEDIA_DRIVE);             // De-initialize media and dismount Drive
    } else {                                    // If new requested owner is File System (previous owner was USB)
      (void)fs_ioc_unlock (drv_id);             // Un-lock media
    }
  }
 
  // Initialize media according to current owner
  if ((own & MEDIA_OWN_CHG)        ||           // If owner change requested or
      (media_state ^ media_ready_ex)) {         // if media ready state has changed (disconnect(SD remove)/connect(SD insert))
    if (media_state & USBD_MSC_MEDIA_READY) {   // If media is ready
      if (own & MEDIA_OWN_USB){                 // If current owner is USB
        media_ok     = false;                   // Invalidate current media status (not initialized = not ok)
        param_status = 0U;                      // Parameter for function call is 0
        if (fs_ioc_device_ctrl (drv_id, fsDevCtrlCodeControlMedia, &param_status) == fsOK) {
                                                // Initialization of media has succeeded
          if (fs_ioc_lock (drv_id) == fsOK) {   // If lock media for USB usage has succeeded
            media_ok = true;                    // Media was initialized and is ok
          }
        }
      } else {                                  // If current owner is File System
        if (fmount (MEDIA_DRIVE) == fsOK) {     // Initialize media and Mount Drive for File System usage
          media_ok = true;                      // Media was initialized and is ok
        }
      }
    }
    if (own & MEDIA_OWN_CHG) {
      usbd_msc0_media_own &= ~MEDIA_OWN_CHG;    // Clear request to change media owner if it was handled
    }
    media_ready_ex = media_state & USBD_MSC_MEDIA_READY;
  }
 
  // If media is not ok or owned by File System return that it is not ready for USB
  if ((!media_ok) || (!(usbd_msc0_media_own & MEDIA_OWN_USB))) {
    return 0U;
  }
 
  return media_state;
#else
  return USBD_MSC_MEDIA_READY;
#endif
}
//! [code_USBD_User_MSC]
