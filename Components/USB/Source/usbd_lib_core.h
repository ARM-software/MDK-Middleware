/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_core.h
 * Purpose: USB Device - Core module header file
 *----------------------------------------------------------------------------*/

#ifndef USBD_LIB_CORE_H_
#define USBD_LIB_CORE_H_

#include <stdint.h>

#include "rl_usb.h"

/// \brief USB Device Request - Data In Stage
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DataInStage (uint8_t device);

/// \brief USB Device Request - Data Out Stage (send data)
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DataOutStage (uint8_t device);

/// \brief USB Device Request - Status In Stage (send IN ZLP)
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_StatusInStage (uint8_t device);

/// \brief USB Device Request - Status In Stage done (IN ZLP sent)
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_StatusInStageDone (uint8_t device);

/// \brief USB Device Request - Status Out Stage (receive OUT ZLP)
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_StatusOutStage (uint8_t device);

/// \brief USB Device Request - Status Out Stage done (OUT ZLP received)
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_StatusOutStageDone (uint8_t device);

#endif  // USBD_LIB_CORE_H_
