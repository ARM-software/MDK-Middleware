/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_core_custom_class.h
 * Purpose: USB Device - Custom Class core module header file
 *----------------------------------------------------------------------------*/

#ifndef USBD_LIB_CORE_CUSTOM_CLASS_H_
#define USBD_LIB_CORE_CUSTOM_CLASS_H_

#include <stdint.h>
#include <stdbool.h>

#include "rl_usb.h"

/// \brief USB Device Endpoint 0 Get Extended Properties OS Descriptor
/// \param[in]   device                  device index.
/// \param[out]  pD                      pointer to descriptor
/// \return      true                    success
/// \return      false                   fail
extern bool USBD_Endpoint0_ReqGetExtProp_CC (uint8_t device, const uint8_t **pD);

/// \brief USB Device Endpoint 0 Custom Class Setup Packet Received callback
/// \param[in]   device                  device index.
/// \return      usbdRequestStatus       enumerator value indicating the function execution status
/// \return      usbdRequestNotProcessed:request was not processed; processing will be done by USB library
/// \return      usbdRequestOK:          request was processed successfully
/// \return      usbdRequestStall:       request was processed but is not supported (STALL EP)
extern usbdRequestStatus USBD_Class_Endpoint0_SetupPacketReceived (uint8_t device);

/// \brief USB Device Endpoint 0 Custom Class Setup Packet processed by USB Library callback
/// \param[in]   device                  device index.
extern void USBD_Class_Endpoint0_SetupPacketProcessed (uint8_t device);

/// \brief USB Device Endpoint 0 Custom Class Out Data Received callback
/// \param[in]   device                  device index.
/// \return      usbdRequestStatus       enumerator value indicating the function execution status
/// \return      usbdRequestNotProcessed:request was not processed; processing will be done by USB library
/// \return      usbdRequestOK:          request was processed successfully (send ZLP)
/// \return      usbdRequestStall:       request was processed but is not supported (STALL EP0)
/// \return      usbdRequestNAK:         request was processed but the device is busy (return NAK)
extern usbdRequestStatus USBD_Class_Endpoint0_OutDataReceived (uint8_t device);

/// \brief USB Device Endpoint 0 Custom Class In Data Received callback
/// \param[in]   device                  device index.
/// \return      usbdRequestStatus       enumerator value indicating the function execution status
/// \return      usbdRequestNotProcessed:request was not processed; processing will be done by USB library
/// \return      usbdRequestOK:          request was processed successfully (return ACK)
/// \return      usbdRequestStall:       request was processed but is not supported (STALL EP0)
/// \return      usbdRequestNAK:         request was processed but the device is busy (return NAK)
extern usbdRequestStatus USBD_Class_Endpoint0_InDataSent (uint8_t device);

#endif  // USBD_LIB_CORE_CUSTOM_CLASS_H_
