/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_msc.h
 * Purpose: USB Device - Mass Storage device Class (MSC) module header file
 *----------------------------------------------------------------------------*/

#ifndef __USBD_LIB_MSC_H__
#define __USBD_LIB_MSC_H__

#include <stdint.h>
#include <stdbool.h>


/// \brief MSC Mass Storage Bulk-Only Mass Storage Reset request
/// \param[in]     instance      instance of MSC class.
/// \return        true          success
/// \return        false         fail, not supported request
extern bool USBD_MSC_BulkOnlyMassStorageReset (uint8_t instance);

/// \brief MSC Get Max LUN request
/// \param[in]     instance      instance of MSC class.
/// \return        true          success
/// \return        false         fail, not supported request
extern bool USBD_MSC_GetMaxLUN (uint8_t instance);

/// \brief MSC Endpoint Clear Stall Feature request handling
/// \param[in]     instance      instance of MSC class.
/// \param[in]     ep_addr       endpoint address.
extern void USBD_MSC_EndpointClearFeatureStall (uint8_t instance, uint8_t ep_addr);

#endif  // __USBD_LIB_MSC_H__
