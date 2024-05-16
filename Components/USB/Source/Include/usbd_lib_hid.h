/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_hid.h
 * Purpose: USB Device - Human Interface Device (HID) module header file
 *----------------------------------------------------------------------------*/

#ifndef __USBD_LIB_HID_H__
#define __USBD_LIB_HID_H__

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

#endif  // __USBD_LIB_HID_H__
