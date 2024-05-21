/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_hid.h
 * Purpose: USB Device - Human Interface Device (HID) module header file
 *----------------------------------------------------------------------------*/

#ifndef USBD_LIB_HID_H_
#define USBD_LIB_HID_H_

#include <stdint.h>
#include <stdbool.h>

/// \brief HID Get Report request
/// \param[in]     instance      instance of HID class.
/// \return        true          success
/// \return        false         fail, not supported request
extern bool USBD_HID_CoreGetReport (uint8_t instance);

/// \brief HID Set Report request
/// \param[in]     instance      instance of HID class.
/// \return        true          success
/// \return        false         fail, not supported request
extern bool USBD_HID_CoreSetReport (uint8_t instance);

/// \brief HID Get Idle request
/// \param[in]     instance      instance of HID class.
/// \return        true          success
/// \return        false         fail, not supported request
extern bool USBD_HID_CoreGetIdle (uint8_t instance);

/// \brief HID Set Idle request
/// \param[in]     instance      instance of HID class.
/// \return        true          success
/// \return        false         fail, not supported request
extern bool USBD_HID_CoreSetIdle (uint8_t instance);

/// \brief HID Get Protocol request
/// \param[in]     instance      instance of HID class.
/// \return        true          success
/// \return        false         fail, not supported request
extern bool USBD_HID_CoreGetProtocol (uint8_t instance);

/// \brief HID Set Protocol request
/// \param[in]     instance      instance of HID class.
/// \return        true          success
/// \return        false         fail, not supported request
extern bool USBD_HID_CoreSetProtocol (uint8_t instance);

#endif  // USBD_LIB_HID_H_
