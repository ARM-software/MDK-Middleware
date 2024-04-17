/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host:MSC
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBH_MSC.c
 * Purpose: Functions to access USB storage device via USB Host
 * Rev.:    V6.4.3
 *----------------------------------------------------------------------------*/
/*
 * USBH_MSC.c is a code template for the application specific functionality of
 * the USB Host MSC class. It implements the access to a USB storage device and
 * allows file I/O via the File System component.
 *
 * USBH_MSC.h is the related header file.
 *
 * First to enable USB Host Controller (if not already enabled) call:
 *   USBH_Initialize (ctrl_num);
 *
 * To access files on a USB storage device use below code sample:
 *   int32_t media_status, media_status_previous = USBH_MSC_ERROR_DRIVE;
 *   for (;;) {
 *     media_status = USBH_MSC_DriveGetMediaStatus (drive_name);
 *     if ((media_status          == USBH_MSC_OK) && 
 *         (media_status_previous != USBH_MSC_OK)) {
 *       switch (USBH_MSC_DriveMount (drive_name)) {
 *         case USBH_MSC_OK:
 *           fopen (...);
 *           break;
 *         case USBH_MSC_ERROR_FORMAT:
 *           fformat (drive_name, "/FAT32");
 *           fopen (...);
 *           break;
 *         case USBH_MSC_ERROR:
 *           // Mount error
 *           break;
 *       }
 *     }
 *     media_status_previous = media_status;
 *     osWait (1000);   // polling interval for media status (1 second)
 *   }
 *
 * Now file I/O can be performed using fopen, fread, fwrite, fclose and other
 * functions of the File System component
 *
 * When drive is not to be used any more call:
 *   USBH_MSC_DriveUnmount (drive_name);
 *
 * When USB Host Controller is not to be used any more call:
 *   USBH_Uninitialize (ctrl_num);
 */
 
#include <stdint.h>
 
#include "RTE_Components.h"             // Component selection
#include "USBH_MSC.h"                   // Access storage via USB Host
 
#if (!defined (RTE_FileSystem_Drive_USB_0) && !defined (RTE_FileSystem_Drive_USB_1))
  #error "Project does not contain USB storage device support"
#endif


//! [usbh_msc_drive_getmediastatus]
/// \brief Get status of drive media (USB storage connected or not connected)
/// \param[in]  drive_name     USB storage drive name ("U0:", "U1:")
/// \return     execution status
///               - USBH_MSC_OK           = USB storage device connected and enumerated
///               - USBH_MSC_ERROR_DRIVE  = USB storage device not connected or not enumerated
int32_t USBH_MSC_DriveGetMediaStatus (const char *drive_name) {
  usbStatus ustatus;
  uint8_t   drive_num;
 
  drive_num = (uint8_t)(drive_name[1] - '0');   // get drive number from drive name
 
  ustatus = USBH_MSC_GetStatus (drive_num);
  if (ustatus != usbOK) return USBH_MSC_ERROR_DRIVE;
 
  return USBH_MSC_OK;
}
//! [usbh_msc_drive_getmediastatus]
 
 
//! [usbh_msc_drive_mount]
/// \brief Mount drive and initialize USB storage device for file I/O access
/// \param[in]  drive_name     USB storage drive name ("U0:", "U1:")
/// \return     execution status
///               - USBH_MSC_OK           = USB drive mounted, file system ready
///               - USBH_MSC_ERROR        = unspecified error
///               - USBH_MSC_ERROR_DRIVE  = USB storage device not connected
///               - USBH_MSC_ERROR_FORMAT = USB drive mounted, but unformatted
int32_t USBH_MSC_DriveMount (const char *drive_name) {
  fsStatus  fstatus;
 
  fstatus = finit (drive_name);
  if (fstatus != fsOK)   return USBH_MSC_ERROR;
 
  fstatus = fmount (drive_name);
  switch (fstatus)  {
    case fsOK:
      break;
    case fsNoFileSystem:
      return USBH_MSC_ERROR_FORMAT;
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
    case fsNoFreeSpace:
    case fsFileNotFound:
    case fsDirNotEmpty:
    case fsTooManyOpenFiles:
    case fsAlreadyExists:
    case fsNotDirectory:
      return USBH_MSC_ERROR;
  }
 
  return USBH_MSC_OK;
}
//! [usbh_msc_drive_mount]
 
 
//! [usbh_msc_drive_unmount]
/// \brief Unmount drive and de-initialize USB storage device before eject
/// \param[in]  drive_name     USB storage drive name ("U0:", "U1:")
/// \return     execution status
///               - USBH_MSC_OK           = USB drive unmounted
///               - USBH_MSC_ERROR        = unspecified error
int32_t USBH_MSC_DriveUnmount (const char *drive_name) {
  fsStatus fstatus;
 
  fstatus = funmount (drive_name);
  if (fstatus != fsOK) return USBH_MSC_ERROR;
 
  fstatus = funinit (drive_name);
  if (fstatus != fsOK) return USBH_MSC_ERROR;
 
  return USBH_MSC_OK;
}
//! [usbh_msc_drive_unmount]
 
 
//! [usbh_msc_drive_getcapacity]
/// \brief Check the physical capacity of USB storage device
/// \param[in]  drive_name     USB storage drive name ("U0:", "U1:")
/// \return     execution status
///               - value != 0            = size of USB storage device in bytes
///               - value == 0            = no drive connected or unspecified error
uint64_t USBH_MSC_DriveGetCapacity (const char *drive_name) {
  usbStatus ustatus;
  uint32_t  block_count;
  uint32_t  block_size;
  uint8_t   drive_num;
 
  drive_num = (uint8_t)(drive_name[1] - '0');   // get drive number from drive name
 
  ustatus = USBH_MSC_GetStatus (drive_num);
  if (ustatus != usbOK) return 0;
 
  ustatus = USBH_MSC_ReadCapacity (drive_num, &block_count, &block_size);
  if (ustatus != usbOK) return 0;
 
  return (((uint64_t)block_count) * ((uint64_t)block_size));
}
//! [usbh_msc_drive_getcapacity]
