/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_custom_class.h
 * Purpose: USB Device - Custom Class module header file
 *----------------------------------------------------------------------------*/

#ifndef USBD_LIB_CUSTOM_CLASS_H_
#define USBD_LIB_CUSTOM_CLASS_H_

#include <stdint.h>
#include <stdbool.h>

#include "rl_usb.h"

/// \brief Initialize Custom Class device instance and create Endpoint handling Thread(s)
/// \param[in]     instance      instance of custom class.
/// \return                      status code that indicates the execution status of the function.
extern usbStatus USBD_CustomClass_Initialize (uint8_t instance);

/// \brief Uninitialize Custom Class device instance and terminate Endpoint handling thread(s)
/// \param[in]     instance      instance of custom class.
/// \return                      status code that indicates the execution status of the function.
extern usbStatus USBD_CustomClass_Uninitialize (uint8_t instance);

/// \brief Custom Class USB Bus Reset event handling
/// \param[in]     instance      instance of CDC class.
extern void USBD_CustomClass_Reset (uint8_t instance);

/// \brief Custom Class Endpoint Start handling
/// \param[in]     instance      instance of CDC class.
/// \param[in]     ep_addr       endpoint address.
///                 - ep_addr.0..3: address
///                 - ep_addr.7:    direction
extern void USBD_CustomClass_EndpointStart (uint8_t instance, uint8_t ep_addr);

/// \brief Custom Class Endpoint Stop handling
/// \param[in]     instance      instance of CDC class.
/// \param[in]     ep_addr       endpoint address.
///                 - ep_addr.0..3: address
///                 - ep_addr.7:    direction
extern void USBD_CustomClass_EndpointStop (uint8_t instance, uint8_t ep_addr);

#endif  // USBD_LIB_CUSTOM_CLASS_H_
