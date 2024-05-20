/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_core_msc.h
 * Purpose: USB Device - Mass Storage device Class (MSC) core module header
 *                       file
 *----------------------------------------------------------------------------*/

#ifndef __USBD_LIB_CORE_MSC_H__
#define __USBD_LIB_CORE_MSC_H__

#include <stdint.h>
#include <stdbool.h>


/// \brief USB Device Endpoint 0 Event Callback - MSC specific handling (Clear Halt condition on bulk in endpoint)
/// \param[in]     device        device index.
/// \param[in]     ep_addr       endpoint address.
extern void USBD_Endpoint0_ClearFeatureStall_MSC (uint8_t device, uint8_t ep_addr);

/// \brief USB Device Endpoint 0 Event Callback - MSC specific handling (Setup Request To Interface)
/// \param[in]     device        device index.
/// \return        true          setup interface request ok
/// \return        false         setup interface request not supported
extern bool USBD_Endpoint0_Setup_MSC_ReqToIF (uint8_t device);

#endif  // __USBD_LIB_CORE_MSC_H__
