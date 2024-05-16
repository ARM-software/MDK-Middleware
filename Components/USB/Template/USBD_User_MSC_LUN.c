/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device:MSC
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_MSC_LUN_%Instance%.c
 * Purpose: USB Device Mass Storage Device class (MSC) with 2 Logical
 *          Units User module
 * Rev.:    V1.1.1
 *----------------------------------------------------------------------------*/
/**
 * \addtogroup usbd_mscFunctions
 *
 */
 
 
//! [code_USBD_User_MSC_LUN]
 
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rl_usb.h"
 
 
static   uint32_t memory   [2][8192/4];         // Memory in RAM for dummy disk image
static   uint32_t block_buf[    512/4];         // Buffer for block read/write to media
extern
const    uint8_t  memory_disk_image[4096];      // Dummy Memory Disk Image
 
 
// Called during USBD_Initialize to initialize all Logical Units of the USB MSC class instance.
void USBD_MSC%Instance%_Initialize (void) {
 
                                        // Copy the dummy image from code to RAM
  memcpy (&memory[0][0], memory_disk_image, sizeof(memory_disk_image));
  memcpy (&memory[1][0], memory_disk_image, sizeof(memory_disk_image));
}
 
 
// Called during USBD_Uninitialize to de-initialize all Logical Units of the USB MSC class instance.
void USBD_MSC%Instance%_Uninitialize (void) {
  // Add code for de-initialization
}
 
 
// Get cache information.
// \param[out]    buffer               cache buffer address.
// \param[out]    size                 cache buffer size.
// \return        true                 operation succeeded.
// \return        false                operation failed.
bool USBD_MSC%Instance%_GetCacheInfo (uint32_t *buffer, uint32_t *size) {
 
  *buffer = (uint32_t)block_buf;        // Local buffer for data
  *size   = sizeof(block_buf);          // Size of local buffer
 
  return true;
}
 
 
// Get maximum number of logical units.
// \return                             number of logical units that device contains
//                                       - value > 0 and <= 4 : maximum number of logical units
//                                       - value 0 :            use setting from configuration file
uint8_t USBD_MSC%Instance%_GetMaxLUN (void) {
 
  return 2U;                            // Device contains 2 logical units
}
 
 
// Get media capacity of a logical unit.
// \param[in]     lun                  logical unit number.
// \param[out]    block_count          total number of blocks on media.
// \param[out]    block_size           media block size.
// \return        true                 operation succeeded.
// \return        false                operation failed.
bool USBD_MSC%Instance%_LUN_GetMediaCapacity (uint8_t lun, uint32_t *block_count, uint32_t *block_size) {
 
  (void)lun;
 
  *block_count =(sizeof(memory)/2)/512; // Total number of blocks on media
  *block_size  = 512U;                  // Block size of blocks on media
 
  return true;
}
 
 
// Read data from media of a logical unit.
// \param[in]     lun                  logical unit number.
// \param[in]     lba                  logical address of first block to read.
// \param[in]     cnt                  number of contiguous blocks to read from media.
// \param[out]    buf                  data buffer for data read from media.
// \return        true                 read succeeded.
// \return        false                read failed.
bool USBD_MSC%Instance%_LUN_Read (uint8_t lun, uint32_t lba, uint32_t cnt, uint8_t *buf) {
 
                                        // Read data from dummy image in RAM
  memcpy (buf, &memory[lun][lba * (512U/4U)], cnt * 512U);
 
  return true;
}
 
 
// Write data to media of a logical unit.
// \param[in]     lun                  logical unit number.
// \param[in]     lba                  logical address of first block to write.
// \param[in]     cnt                  number of contiguous blocks to write to media.
// \param[out]    buf                  data buffer containing data to write to media.
// \return        true                 write succeeded.
// \return        false                write failed.
bool USBD_MSC%Instance%_LUN_Write (uint8_t lun, uint32_t lba, uint32_t cnt, const uint8_t *buf) {
 
                                        // Write data to image in RAM
  memcpy (&memory[lun][lba * (512U/4U)], buf, cnt * 512U);
 
  return true;
}
 
 
// Check media presence and write protect status of a logical unit.
//        (if media is not owned by USB it returns that media is not ready)
// \param[in]     lun                  logical unit number.
// \return                             media presence and write protected status
//                bit 1:               write protect bit
//                 - value 1:            media is write protected
//                 - value 0:            media is not write protected
//                bit 0:               media presence bit
//                 - value 1:            media is present
//                 - value 0:            media is not present
uint32_t USBD_MSC%Instance%_LUN_CheckMedia (uint8_t lun) {
 
  (void)lun;
 
  return USBD_MSC_MEDIA_READY;
}
//! [code_USBD_User_MSC_LUN]
