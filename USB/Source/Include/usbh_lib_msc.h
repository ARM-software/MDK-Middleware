/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_lib_msc.h
 * Purpose: USB Host - Mass Storage device Class (MSC) module header file
 *----------------------------------------------------------------------------*/

#ifndef __USBH_LIB_MSC_H__
#define __USBH_LIB_MSC_H__

#include <stdint.h>
#include <stdbool.h>

#include "usb_def.h"

#include "rl_usb.h"


/// \brief Analyze device configuration and configure resources
/// \param[in]     device               index of USB Device.
/// \param[in]     ptr_dev_desc         pointer to device descriptor.
/// \param[in]     ptr_cfg_desc         pointer to configuration descriptor.
/// \return                             index of configured MSC instance or configuration failed
/// \return        - value <= 127 :     index of configured MSC instance.
/// \return        - value 255 :        configuration failed.
extern uint8_t USBH_MSC_Configure (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc);

/// \brief De-configure resources
/// \param[in]     instance             index of MSC instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
extern usbStatus USBH_MSC_Unconfigure (uint8_t instance);

/// \brief Initialize Mass Storage Class device instance
/// \param[in]     instance             index of MSC instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
extern usbStatus USBH_MSC_Initialize_Lib (uint8_t instance);

/// \brief De-initialize Mass Storage Class device instance
/// \param[in]     instance             index of MSC instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
extern usbStatus USBH_MSC_Uninitialize_Lib (uint8_t instance);

#endif  // __USBH_LIB_MSC_H__
