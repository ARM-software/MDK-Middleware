/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device:MSC
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_MSC_%Instance%.h
 * Purpose: Functions for media ownership control between USB and File System
 * Rev.:    V6.3.1
 *----------------------------------------------------------------------------*/

#ifndef USBD_MSC%Instance%_H
#define USBD_MSC%Instance%_H

#include <stdint.h>

#define USBD_MSC%Instance%_MEDIA_OWN_USB  (1U   ) ///< Media owned by USB (bit mask)
#define USBD_MSC%Instance%_MEDIA_OWN_CHG  (1U<<1) ///< Media ownership change requested (bit mask)

#define USBD_MSC%Instance%_OK               0     ///< Function succeeded
#define USBD_MSC%Instance%_ERROR           -1     ///< Unspecified error


/// \brief Set USB as media owner
/// \return     execution status
///               - USBD_MSC%Instance%_OK           = Media ownership changed successfully
///               - USBD_MSC%Instance%_ERROR        = Media ownership change has failed (due timeout)
extern int32_t USBD_MSC%Instance%_SetMediaOwnerUSB (void);


/// \brief Set File System as media owner
/// \return     execution status
///               - USBD_MSC%Instance%_OK           = Media ownership changed successfully
///               - USBD_MSC%Instance%_ERROR        = Media ownership change has failed (due timeout)
extern int32_t USBD_MSC%Instance%_SetMediaOwnerFS (void);

#endif
