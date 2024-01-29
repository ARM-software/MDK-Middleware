/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_lib_drv.h
 * Purpose: USB Host - Driver access module header file
 *----------------------------------------------------------------------------*/

#ifndef __USBH_LIB_DRV_H__
#define __USBH_LIB_DRV_H__

#include <stdint.h>
#include <stdbool.h>

#include "rl_usb.h"
#include "Driver_USBH.h"


/// \brief       Get driver version
/// \param[in]   ctrl  Controller Index
/// \return      \ref ARM_DRIVER_VERSION
extern ARM_DRIVER_VERSION USBH_DriverGetVersion (uint8_t ctrl);

/// \brief       Get driver capabilities
/// \param[in]   ctrl  Controller Index
/// \return      \ref ARM_USBH_CAPABILITIES
extern ARM_USBH_CAPABILITIES USBH_DriverGetCapabilities (uint8_t ctrl);

/// \brief       Initialize USB Host Interface
/// \param[in]   ctrl           Controller Index
/// \param[in]   cb_port_event  Pointer to \ref ARM_USBH_SignalPortEvent
/// \param[in]   cb_pipe_event  Pointer to \ref ARM_USBH_SignalPipeEvent
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
extern usbStatus USBH_DriverInitialize (uint8_t ctrl, ARM_USBH_SignalPortEvent_t cb_port_event, ARM_USBH_SignalPipeEvent_t cb_pipe_event);

/// \brief       De-initialize USB Host Interface
/// \param[in]   ctrl Controller Index
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
extern usbStatus USBH_DriverUninitialize (uint8_t ctrl);

/// \brief       Control USB Host Interface Power
/// \param[in]   ctrl   Controller Index
/// \param[in]   state  Power state
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
extern usbStatus USBH_DriverPowerControl (uint8_t ctrl, ARM_POWER_STATE state);

/// \brief       Root HUB Port VBUS on/off
/// \param[in]   ctrl  Controller Index
/// \param[in]   port  Root HUB Port Number
/// \param[in]   vbus
///               - \b false VBUS off
///               - \b true  VBUS on
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
extern usbStatus USBH_DriverPortVbusOnOff (uint8_t ctrl, uint8_t port, bool vbus);

/// \brief       Do Root HUB Port Reset
/// \param[in]   ctrl  Controller Index
/// \param[in]   port  Root HUB Port Number
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
extern usbStatus USBH_DriverPortReset (uint8_t ctrl, uint8_t port);

/// \brief       Suspend Root HUB Port (stop generating SOFs)
/// \param[in]   ctrl  Controller Index
/// \param[in]   port  Root HUB Port Number
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
extern usbStatus USBH_DriverPortSuspend (uint8_t ctrl, uint8_t port);

/// \brief       Resume Root HUB Port (start generating SOFs)
/// \param[in]   ctrl  Controller Index
/// \param[in]   port  Root HUB Port Number
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
extern usbStatus USBH_DriverPortResume (uint8_t ctrl, uint8_t port);

/// \brief       Get current Root HUB Port State
/// \param[in]   ctrl  Controller Index
/// \param[in]   port  Root HUB Port Number
/// \return      Port State \ref ARM_USBH_PORT_STATE
extern ARM_USBH_PORT_STATE USBH_DriverPortGetState (uint8_t ctrl, uint8_t port);

/// \brief       Create Pipe in System
/// \param[in]   ctrl       Controller Index
/// \param[in]   dev_addr   Device Address
/// \param[in]   dev_speed  Device Speed
/// \param[in]   hub_addr   Hub Address
/// \param[in]   hub_port   Hub Port
/// \param[in]   ep_addr    Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \param[in]   ep_type    Endpoint Type (ARM_USB_ENDPOINT_xxx)
/// \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
/// \param[in]   ep_interval        Endpoint Polling Interval
/// \return      Pipe Handle \ref ARM_USBH_PIPE_HANDLE
extern ARM_USBH_PIPE_HANDLE USBH_DriverPipeCreate (uint8_t ctrl, uint8_t dev_addr, uint8_t dev_speed, uint8_t hub_addr, uint8_t hub_port, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size, uint8_t ep_interval);

/// \brief       Modify Pipe in System
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \param[in]   dev_addr   Device Address
/// \param[in]   dev_speed  Device Speed
/// \param[in]   hub_addr   Hub Address
/// \param[in]   hub_port   Hub Port
/// \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
extern usbStatus USBH_DriverPipeModify (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl, uint8_t dev_addr, uint8_t dev_speed, uint8_t hub_addr, uint8_t hub_port, uint16_t ep_max_packet_size);

/// \brief       Delete Pipe from System
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
extern usbStatus USBH_DriverPipeDelete (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl);

/// \brief       Reset Pipe
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
extern usbStatus USBH_DriverPipeReset (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl);

/// \brief       Transfer packets through USB Pipe
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \param[in]   packet     Packet information
/// \param[in]   data       Pointer to buffer with data to send or for data to receive
/// \param[in]   num        Number of data bytes to transfer
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
extern usbStatus USBH_DriverPipeTransfer (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t packet, uint8_t *data, uint32_t num);

/// \brief       Get result of USB Pipe transfer
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \return      number of successfully transferred data bytes
extern uint32_t USBH_DriverPipeTransferGetResult (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl);

/// \brief       Abort current USB Pipe transfer
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
extern usbStatus USBH_DriverPipeTransferAbort (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl);

/// \brief       Get current USB Frame Number
/// \param[in]   ctrl  Controller Index
/// \return      Frame Number
extern uint16_t USBH_DriverGetFrameNumber (uint8_t ctrl);

/// \brief       Signal Root HUB Port Event
/// \param[in]   ctrl  Controller Index
/// \param[in]   port  Root HUB Port Number
/// \param[in]   port event \ref USBH_port_events
/// \return      none
extern void USBH_SignalPortEvent (uint8_t ctrl, uint8_t port, uint32_t event);

/// \brief       Signal Pipe Event
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \param[in]   pipe event \ref USBH_pipe_events
/// \return      none
extern void USBH_SignalPipeEvent (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t event);

#endif  // __USBH_LIB_DRV_H__
