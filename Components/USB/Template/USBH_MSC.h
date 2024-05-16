/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host:MSC
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBH_MSC.h
 * Purpose: Functions to access USB storage device via USB Host
 * Rev.:    V6.2.1
 *----------------------------------------------------------------------------*/

#ifndef USBH_MSC_H
#define USBH_MSC_H

#include "rl_usb.h"                     // Keil.MDK-Pro::USB:CORE
#include "rl_fs.h"                      // Keil.MDK-Pro::File System:CORE

/* Execution status codes */
#define USBH_MSC_OK                   0 ///< Function succeeded
#define USBH_MSC_ERROR               -1 ///< Unspecified error
#define USBH_MSC_ERROR_DRIVE         -2 ///< USB storage device not connected
#define USBH_MSC_ERROR_FORMAT        -3 ///< USB drive mounted, but unformatted


/// \brief Get status of drive media (USB storage connected or not connected)
/// \param[in]  drive_name     USB storage drive name ("U0:", "U1:")
/// \return     execution status
///               - USBH_MSC_OK           = USB storage device connected and enumerated
///               - USBH_MSC_ERROR_DRIVE  = USB storage device not connected or not enumerated
extern int32_t USBH_MSC_DriveGetMediaStatus (const char *drive_name);


/// \brief Mount drive and initialize USB storage device for file I/O access
/// \param[in]  drive_name     USB storage drive name ("U0:", "U1:")
/// \return     execution status
///               - USBH_MSC_OK           = USB drive mounted, file system ready
///               - USBH_MSC_ERROR        = unspecified error
///               - USBH_MSC_ERROR_DRIVE  = USB storage device not connected
///               - USBH_MSC_ERROR_FORMAT = USB drive mounted, but unformatted
extern int32_t USBH_MSC_DriveMount (const char *drive_name);


/// \brief Unmount drive and de-initialize USB storage device before eject
/// \param[in]  drive_name     USB storage drive name ("U0:", "U1:")
/// \return     execution status
///               - USBH_MSC_OK           = USB drive unmounted
///               - USBH_MSC_ERROR        = unspecified error
extern int32_t USBH_MSC_DriveUnmount (const char *drive_name);


/// \brief Check the physical capacity of USB storage device
/// \param[in]  drive_name     USB storage drive name ("U0:", "U1:")
/// \return     execution status
///               - value != 0            = size of USB storage device in bytes
///               - value == 0            = no drive connected or unspecified error
extern uint64_t USBH_MSC_DriveGetCapacity (const char *drive_name);

#endif
