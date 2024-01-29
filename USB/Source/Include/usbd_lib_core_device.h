/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_core_custom_device.h
 * Purpose: USB Device - Device core module header file
 *----------------------------------------------------------------------------*/

#ifndef __USBD_LIB_CORE_DEVICE_H__
#define __USBD_LIB_CORE_DEVICE_H__

#include <stdint.h>
#include <stdbool.h>

#include "rl_usb.h"


/// \brief USB Device Endpoint 0 Setup Packet Received callback
/// \param[in]   device                  device index.
/// \return      usbdRequestStatus       enumerator value indicating the function execution status
/// \return      usbdRequestNotProcessed:request was not processed; processing will be done by USB library
/// \return      usbdRequestOK:          request was processed successfully
/// \return      usbdRequestStall:       request was processed but is not supported (STALL EP)
extern usbdRequestStatus USBD_Core_Endpoint0_SetupPacketReceived (uint8_t device);

/// \brief USB Device Endpoint 0 Setup Packet processed by USB Library callback
/// \param[in]   device                  device index.
extern void USBD_Core_Endpoint0_SetupPacketProcessed (uint8_t device);

/// \brief USB Device Endpoint 0 Out Data Received callback
/// \param[in]   device                  device index.
/// \return      usbdRequestStatus       enumerator value indicating the function execution status
/// \return      usbdRequestNotProcessed:request was not processed; processing will be done by USB library
/// \return      usbdRequestOK:          request was processed successfully (send ZLP)
/// \return      usbdRequestStall:       request was processed but is not supported (STALL EP0)
/// \return      usbdRequestNAK:         request was processed but the device is busy (return NAK)
extern usbdRequestStatus USBD_Core_Endpoint0_OutDataReceived (uint8_t device);

/// \brief USB Device Endpoint 0 In Data Received callback
/// \param[in]   device                  device index.
/// \return      usbdRequestStatus       enumerator value indicating the function execution status
/// \return      usbdRequestNotProcessed:request was not processed; processing will be done by USB library
/// \return      usbdRequestOK:          request was processed successfully (return ACK)
/// \return      usbdRequestStall:       request was processed but is not supported (STALL EP0)
/// \return      usbdRequestNAK:         request was processed but the device is busy (return NAK)
extern usbdRequestStatus USBD_Core_Endpoint0_InDataSent (uint8_t device);

#endif  // __USBD_LIB_CORE_DEVICE_H__
