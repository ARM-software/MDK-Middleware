/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_core_hid.h
 * Purpose: USB Device - Human Interface Device (HID) core module header file
 *----------------------------------------------------------------------------*/

#ifndef USBD_LIB_CORE_HID_H_
#define USBD_LIB_CORE_HID_H_

#include <stdint.h>
#include <stdbool.h>

/// \brief USB Device Endpoint 0 Get Descriptor HID Descriptor Request
/// \param[in]     device        device index.
/// \param[out]    pD            pointer to descriptor
/// \param[out]    len           length of descriptor
/// \return        true          success
/// \return        false         fail
extern bool USBD_Endpoint0_ReqGetDescriptor_HID (uint8_t device, const uint8_t **pD, uint32_t *len);

/// \brief USB Device Endpoint 0 Event Callback - HID specific handling (Setup Request To Interface)
/// \param[in]     device        device index.
/// \return        true          setup interface request ok
/// \return        false         setup interface request not supported
extern bool USBD_Endpoint0_Setup_HID_ReqToIF (uint8_t device);

/// \brief USB Device Endpoint 0 Event Callback - HID specific handling (Out Request To Interface)
/// \param[in]     device        device index.
/// \return        true          out interface request ok
/// \return        false         out interface request not supported
extern bool USBD_Endpoint0_Out_HID_ReqToIF (uint8_t device);

#endif  // USBD_LIB_CORE_HID_H_
