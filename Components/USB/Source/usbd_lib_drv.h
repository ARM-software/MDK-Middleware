/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_drv.h
 * Purpose: USB Device - Driver access module header file
 *----------------------------------------------------------------------------*/

#ifndef USBD_LIB_DRV_H_
#define USBD_LIB_DRV_H_

#include <stdint.h>
#include <stdbool.h>

#include "rl_usb.h"
#include "Driver_USBD.h"

/// \brief       Get driver version.
/// \param       device Device Index
/// \return      \ref ARM_DRIVER_VERSION
extern ARM_DRIVER_VERSION USBD_DriverGetVersion (uint8_t device);

/// \brief       Get driver capabilities.
/// \param       device Device Index
/// \return      \ref ARM_USBD_CAPABILITIES
extern ARM_USBD_CAPABILITIES USBD_DriverGetCapabilities (uint8_t device);

/// \brief       Initialize USB Device Interface.
/// \param       device             Device Index
/// \param[in]   cb_device_event    Pointer to \ref ARM_USBD_SignalDeviceEvent
/// \param[in]   cb_endpoint_event  Pointer to \ref ARM_USBD_SignalEndpointEvent
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DriverInitialize (uint8_t device, ARM_USBD_SignalDeviceEvent_t cb_device_event, ARM_USBD_SignalEndpointEvent_t cb_endpoint_event);

/// \brief       De-initialize USB Device Interface.
/// \param       device Device Index
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DriverUninitialize (uint8_t device);

/// \brief       Control USB Device Interface Power.
/// \param       device Device Index
/// \param[in]   state  Power state \ref ARM_POWER_STATE
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DriverPowerControl (uint8_t device, ARM_POWER_STATE state);

/// \brief       Connect USB Device.
/// \param       device Device Index
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DriverDeviceConnect (uint8_t device);

/// \brief       Disconnect USB Device.
/// \param       device Device Index
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DriverDeviceDisconnect (uint8_t device);

/// \brief       Get current USB Device State.
/// \param       device Device Index
/// \return      Device State \ref ARM_USBD_STATE
extern USBD_STATE USBD_DriverDeviceGetState (uint8_t device);

/// \brief       Trigger USB Remote Wakeup.
/// \param       device Device Index
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DriverDeviceRemoteWakeup (uint8_t device);

/// \brief       Set USB Device Address.
/// \param       device    Device Index
/// \param[in]   dev_addr  Device Address
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DriverDeviceSetAddress (uint8_t device, uint8_t dev_addr);

/// \brief       Read setup packet received over Control Endpoint.
/// \param       device Device Index
/// \param[out]  setup  Pointer to buffer for setup packet
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DriverReadSetupPacket (uint8_t device, uint8_t *data);

/// \brief       Configure USB Endpoint.
/// \param       device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \param[in]   ep_type  Endpoint Type (ARM_USB_ENDPOINT_xxx)
/// \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DriverEndpointConfigure (uint8_t device, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size);

/// \brief       Unconfigure USB Endpoint.
/// \param       device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DriverEndpointUnconfigure (uint8_t device, uint8_t ep_addr);

/// \brief       Set/Clear Stall for USB Endpoint.
/// \param       device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \param[in]   stall  Operation
///               - \b false Clear
///               - \b true Set
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DriverEndpointStall (uint8_t device, uint8_t ep_addr, bool stall);

/// \brief       Read data from or Write data to USB Endpoint.
/// \param       device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \param[out]  data Pointer to buffer for data to read or with data to write
/// \param[in]   num  Number of data bytes to transfer
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DriverEndpointTransfer (uint8_t device, uint8_t ep_addr, uint8_t *data, uint32_t num);

/// \brief       Get result of USB Endpoint transfer.
/// \param       device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \return      number of successfully transferred data bytes
extern uint32_t USBD_DriverEndpointTransferGetResult (uint8_t device, uint8_t ep_addr);

/// \brief       Abort current USB Endpoint transfer.
/// \param       device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_DriverEndpointTransferAbort (uint8_t device, uint8_t ep_addr);

/// \brief       Get current USB Frame Number.
/// \param       device Device Index
/// \return      Frame Number
extern uint16_t USBD_DriverGetFrameNumber (uint8_t device);

/// \brief       Signal USB Device Event.
/// \param       device Device Index
/// \param[in]   event \ref USBD_dev_events
extern void USBD_SignalDeviceEvent(uint8_t device, uint32_t event);

/// \brief       Signal USB Endpoint Event.
/// \param       device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \param[in]   event \ref USBD_ep_events
extern void USBD_SignalEndpointEvent(uint8_t device, uint8_t ep_addr, uint32_t event);

#endif  // USBD_LIB_DRV_H_
