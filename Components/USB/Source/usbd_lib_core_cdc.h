/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_core_cdc.c
 * Purpose: USB Device - Communication Device Class (CDC) core module header
 *                       file
 *----------------------------------------------------------------------------*/

#ifndef USBD_LIB_CORE_CDC_H_
#define USBD_LIB_CORE_CDC_H_

#include <stdint.h>
#include <stdbool.h>

/// \brief USB Device Endpoint 0 Event Callback - CDC specific handling (Setup Request To Interface)
/// \param[in]     device        device index.
/// \return        true          setup interface request ok
/// \return        false         setup interface request not supported
extern bool USBD_Endpoint0_Setup_CDC_ReqToIF (uint8_t device);

/// \brief USB Device Endpoint 0 Event Callback - CDC specific handling (Out Request To Interface)
/// \param[in]     device        device index.
/// \return        true          out interface request ok
/// \return        false         out interface request not supported
extern bool USBD_Endpoint0_Out_CDC_ReqToIF (uint8_t device);

#endif  // USBD_LIB_CORE_CDC_H_
