/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates).
 * All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_lib_drv.c
 * Purpose: USB Host - Driver access module
 *----------------------------------------------------------------------------*/

#include "usbh_lib_drv.h"

#include "usb_lib_debug.h"
#include "usbh_config.h"
#include "usbh_lib_core.h"
#include "usbh_evr.h"
#include "usbh_cmsis_rtos2.h"

/// \brief       Get driver version
/// \param[in]   ctrl  Controller Index
/// \return      \ref ARM_DRIVER_VERSION
ARM_DRIVER_VERSION USBH_DriverGetVersion (uint8_t ctrl) {
  return ((*usbh_hcd_ptr[ctrl])->GetVersion());
}

/// \brief       Get driver capabilities
/// \param[in]   ctrl  Controller Index
/// \return      \ref ARM_USBH_CAPABILITIES
ARM_USBH_CAPABILITIES USBH_DriverGetCapabilities (uint8_t ctrl) {
  return ((*usbh_hcd_ptr[ctrl])->GetCapabilities());
}

/// \brief       Initialize USB Host Interface
/// \param[in]   ctrl           Controller Index
/// \param[in]   cb_port_event  Pointer to \ref ARM_USBH_SignalPortEvent
/// \param[in]   cb_pipe_event  Pointer to \ref ARM_USBH_SignalPipeEvent
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBH_DriverInitialize (uint8_t ctrl, ARM_USBH_SignalPortEvent_t cb_port_event, ARM_USBH_SignalPipeEvent_t cb_pipe_event) {
  int32_t driver_status;
  uint8_t retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = 3U;
  do {
    driver_status = (*usbh_hcd_ptr[ctrl])->Initialize (cb_port_event, cb_pipe_event);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBH_Driver_Initialize(ctrl);
    return usbOK;
  } else {
    EvrUSBH_Driver_InitializeFailed(ctrl, driver_status);
    return usbDriverError;
  }
}

/// \brief       De-initialize USB Host Interface
/// \param[in]   ctrl Controller Index
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBH_DriverUninitialize (uint8_t ctrl) {
  int32_t driver_status;
  uint8_t retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = 3U;
  do {
    driver_status = (*usbh_hcd_ptr[ctrl])->Uninitialize ();
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBH_Driver_Uninitialize(ctrl);
    return usbOK;
  } else {
    EvrUSBH_Driver_UninitializeFailed(ctrl, driver_status);
    return usbDriverError;
  }
}

/// \brief       Control USB Host Interface Power
/// \param[in]   ctrl   Controller Index
/// \param[in]   state  Power state
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBH_DriverPowerControl (uint8_t ctrl, ARM_POWER_STATE state) {
  int32_t driver_status;
  uint8_t retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = 3U;
  do {
    driver_status = (*usbh_hcd_ptr[ctrl])->PowerControl (state);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBH_Driver_PowerControl(ctrl, state);
    return usbOK;
  } else {
    EvrUSBH_Driver_PowerControlFailed(ctrl, state, driver_status);
    return usbDriverError;
  }
}

/// \brief       Root HUB Port VBUS on/off
/// \param[in]   ctrl  Controller Index
/// \param[in]   port  Root HUB Port Number
/// \param[in]   vbus
///               - \b false VBUS off
///               - \b true  VBUS on
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBH_DriverPortVbusOnOff (uint8_t ctrl, uint8_t port, bool vbus) {
  int32_t driver_status;
  uint8_t retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = 3U;
  do {
    driver_status = (*usbh_hcd_ptr[ctrl])->PortVbusOnOff (port, vbus);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  if (driver_status != ARM_DRIVER_OK) {
    EvrUSBH_Driver_PortVbusOnOffFailed(ctrl, port, vbus, driver_status);
    return usbDriverError;
  } else {
    EvrUSBH_Driver_PortVbusOnOff(ctrl, port, vbus);
    return usbOK;
  }
}

/// \brief       Do Root HUB Port Reset
/// \param[in]   ctrl  Controller Index
/// \param[in]   port  Root HUB Port Number
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBH_DriverPortReset (uint8_t ctrl, uint8_t port) {
  int32_t driver_status;
  uint8_t retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  usbh_hc[ctrl].port_rst = port + 1U;
  retry = 3U;
  do {
    driver_status = (*usbh_hcd_ptr[ctrl])->PortReset (port);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBH_Driver_PortReset(ctrl, port);
    return usbOK;
  } else {
    EvrUSBH_Driver_PortResetFailed(ctrl, port, driver_status);
    return usbDriverError;
  }
}

/// \brief       Suspend Root HUB Port (stop generating SOFs)
/// \param[in]   ctrl  Controller Index
/// \param[in]   port  Root HUB Port Number
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBH_DriverPortSuspend (uint8_t ctrl, uint8_t port) {
  int32_t driver_status;
  uint8_t retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = 3U;
  do {
    driver_status = (*usbh_hcd_ptr[ctrl])->PortSuspend (port);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBH_Driver_PortSuspend(ctrl, port);
    return usbOK;
  } else {
    EvrUSBH_Driver_PortSuspendFailed(ctrl, port, driver_status);
    return usbDriverError;
  }
}

/// \brief       Resume Root HUB Port (start generating SOFs)
/// \param[in]   ctrl  Controller Index
/// \param[in]   port  Root HUB Port Number
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBH_DriverPortResume (uint8_t ctrl, uint8_t port) {
  int32_t driver_status;
  uint8_t retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = 3U;
  do {
    driver_status = (*usbh_hcd_ptr[ctrl])->PortResume (port);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBH_Driver_PortResume(ctrl, port);
    return usbOK;
  } else {
    EvrUSBH_Driver_PortResumeFailed(ctrl, port, driver_status);
    return usbDriverError;
  }
}

/// \brief       Get current Root HUB Port State
/// \param[in]   ctrl  Controller Index
/// \param[in]   port  Root HUB Port Number
/// \return      Port State \ref ARM_USBH_PORT_STATE
ARM_USBH_PORT_STATE USBH_DriverPortGetState (uint8_t ctrl, uint8_t port) {
  ARM_USBH_PORT_STATE port_state;

  port_state = (*usbh_hcd_ptr[ctrl])->PortGetState (port);
  if ((usbh_hc[ctrl].port_discon & (1UL << port)) != 0U) {
    port_state.connected = 0U;
  }

  EvrUSBH_Driver_PortGetState(ctrl, port, port_state);
  return port_state;
}

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
ARM_USBH_PIPE_HANDLE USBH_DriverPipeCreate (uint8_t ctrl, uint8_t dev_addr, uint8_t dev_speed, uint8_t hub_addr, uint8_t hub_port, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size, uint8_t ep_interval) {
  ARM_USBH_PIPE_HANDLE pipe_hndl;
  uint8_t              retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return 0U;
  }

  pipe_hndl = (*usbh_hcd_ptr[ctrl])->PipeCreate (dev_addr, dev_speed, hub_addr, hub_port, ep_addr, ep_type, ep_max_packet_size, ep_interval);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (pipe_hndl == 0U) {
    EvrUSBH_Driver_PipeCreateFailed(ctrl, dev_addr, dev_speed, hub_addr, hub_port, ep_addr, ep_type, ep_max_packet_size, ep_interval);
  } else {
    EvrUSBH_Driver_PipeCreate(ctrl, dev_addr, dev_speed, hub_addr, hub_port, ep_addr, ep_type, ep_max_packet_size, ep_interval, pipe_hndl);
  }
#endif
  return pipe_hndl;
}

/// \brief       Modify Pipe in System
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \param[in]   dev_addr   Device Address
/// \param[in]   dev_speed  Device Speed
/// \param[in]   hub_addr   Hub Address
/// \param[in]   hub_port   Hub Port
/// \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBH_DriverPipeModify (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl, uint8_t dev_addr, uint8_t dev_speed, uint8_t hub_addr, uint8_t hub_port, uint16_t ep_max_packet_size) {
  int32_t driver_status;
  uint8_t retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = 3U;
  do {
    driver_status = (*usbh_hcd_ptr[ctrl])->PipeModify (pipe_hndl, dev_addr, dev_speed, hub_addr, hub_port, ep_max_packet_size);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBH_Driver_PipeModify(ctrl, pipe_hndl, dev_addr, dev_speed, hub_addr, hub_port, ep_max_packet_size);
    return usbOK;
  } else {
    EvrUSBH_Driver_PipeModifyFailed(ctrl, pipe_hndl, dev_addr, dev_speed, hub_addr, hub_port, ep_max_packet_size, driver_status);
    return usbDriverError;
  }
}

/// \brief       Delete Pipe from System
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBH_DriverPipeDelete (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl) {
  int32_t driver_status;
  uint8_t retry;

  retry = 3U;
  while (retry-- != 0U) {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
  }
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = 3U;
  do {
    driver_status = (*usbh_hcd_ptr[ctrl])->PipeDelete (pipe_hndl);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBH_Driver_PipeDelete(ctrl, pipe_hndl);
    return usbOK;
  } else {
    EvrUSBH_Driver_PipeDeleteFailed(ctrl, pipe_hndl, driver_status);
    return usbDriverError;
  }
}

/// \brief       Reset Pipe
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBH_DriverPipeReset (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl) {
  int32_t driver_status;
  uint8_t retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = 3U;
  do {
    driver_status = (*usbh_hcd_ptr[ctrl])->PipeReset (pipe_hndl);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBH_Driver_PipeReset(ctrl, pipe_hndl);
    return usbOK;
  } else {
    EvrUSBH_Driver_PipeResetFailed(ctrl, pipe_hndl, driver_status);
    return usbDriverError;
  }
}

/// \brief       Transfer packets through USB Pipe
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \param[in]   packet     Packet information
/// \param[in]   data       Pointer to buffer with data to send or for data to receive
/// \param[in]   num        Number of data bytes to transfer
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBH_DriverPipeTransfer (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t packet, uint8_t *data, uint32_t num) {
  int32_t driver_status;
  uint8_t retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = 3U;
  do {
    driver_status = (*usbh_hcd_ptr[ctrl])->PipeTransfer (pipe_hndl, packet, data, num);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  switch (driver_status) {
    case ARM_DRIVER_OK:
      EvrUSBH_Driver_PipeTransfer(ctrl, pipe_hndl, packet, num);
      return usbOK;
    case ARM_DRIVER_ERROR_BUSY:
      EvrUSBH_Driver_PipeTransferFailed(ctrl, pipe_hndl, packet, num, driver_status);
      return usbDriverBusy;
    default:
      EvrUSBH_Driver_PipeTransferFailed(ctrl, pipe_hndl, packet, num, driver_status);
      return usbDriverError;
  }
}

/// \brief       Get result of USB Pipe transfer
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \return      number of successfully transferred data bytes
uint32_t USBH_DriverPipeTransferGetResult (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl) {
  uint32_t val;
  uint8_t  retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return 0U;
  }

  val = (*usbh_hcd_ptr[ctrl])->PipeTransferGetResult (pipe_hndl);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  EvrUSBH_Driver_PipeTransferGetResult(ctrl, pipe_hndl, val);

  return val;
}

/// \brief       Abort current USB Pipe transfer
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBH_DriverPipeTransferAbort (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl) {
  int32_t driver_status;
  uint8_t retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = 3U;
  do {
    driver_status = (*usbh_hcd_ptr[ctrl])->PipeTransferAbort (pipe_hndl);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBH_Driver_PipeTransferAbort(ctrl, pipe_hndl);
    return usbOK;
  } else {
    EvrUSBH_Driver_PipeTransferAbortFailed(ctrl, pipe_hndl, driver_status);
    return usbDriverError;
  }
}

/// \brief       Get current USB Frame Number
/// \param[in]   ctrl  Controller Index
/// \return      Frame Number
uint16_t USBH_DriverGetFrameNumber (uint8_t ctrl) {
  uint16_t val;
  uint8_t  retry;

  retry = 3U;
  do {
    if (USBH_SemaphoreAcquire (usbh_driver_semaphore_id[ctrl], 100U) == 0) { break; }
    (void)USBH_Delay (10U);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return 0U;
  }

  val = (*usbh_hcd_ptr[ctrl])->GetFrameNumber ();

  (void)USBH_SemaphoreRelease (usbh_driver_semaphore_id[ctrl]);

  EvrUSBH_Driver_GetFrameNumber(ctrl, val);

  return val;
}

/// \brief       Signal Root HUB Port Event
/// \param[in]   ctrl  Controller Index
/// \param[in]   port  Root HUB Port Number
/// \param[in]   port event \ref USBH_port_events
/// \return      none
void USBH_SignalPortEvent (uint8_t ctrl, uint8_t port, uint32_t event) {
  USBH_HC *ptr_hc;

  ptr_hc = &usbh_hc[ctrl];

  if (((event & ARM_USBH_EVENT_DISCONNECT) != 0U) && ((usbh_hc[ctrl].port_con & (1UL << port)) != 0U) && (usbh_hc[ctrl].port_rst != (port + 1U))) {
    // If disconnect happened while port was connected and reset driving was not
    // active then store disconnect state.
    // Point of this additional disconnect state is to terminate enumeration if
    // it was in progress when disconnect happened.
    usbh_hc[ctrl].port_discon |= (uint16_t)(1UL << port);
  }

  ptr_hc->port_event[port] |= event;
  if ((ptr_hc->port_lock == 0U) || (ptr_hc->port_lock == (port + 1U))) {
    EvrUSBH_Driver_OnSignalPortEvent(ctrl, port, event);
    (void)USBH_ThreadFlagsSet (usbh_core_thread_id[ctrl], ARM_USBH_EVENT_PORT);
  }
}

/// \brief       Signal Pipe Event
/// \param[in]   ctrl       Controller Index
/// \param[in]   pipe_hndl  Pipe Handle
/// \param[in]   pipe event \ref USBH_pipe_events
/// \return      none
void USBH_SignalPipeEvent (uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t event) {
  void                 *thread_id;
  USBH_PipeEvent_t      cb_pipe_event;
  USBH_PipeEventInfo_t *ptr_pipe_event_info;

  ptr_pipe_event_info = (USBH_PipeEventInfo_t *)(event & 0xFFFFFF80U);
  cb_pipe_event       = USBH_Get_PipeCallback_of_Pipe (ctrl, pipe_hndl);

  if ((cb_pipe_event != NULL) && (ptr_pipe_event_info != NULL)) {
    EvrUSBH_Driver_OnSignalPipeEvent(ctrl, pipe_hndl, event);
    cb_pipe_event (event & 0x7FU, ptr_pipe_event_info->buf, ptr_pipe_event_info->len);
  } else {
    thread_id = USBH_Get_Thread_ID_of_Pipe (ctrl, pipe_hndl);
    if ((thread_id != NULL) && (event != 0U)) {
      EvrUSBH_Driver_OnSignalPipeEvent(ctrl, pipe_hndl, event);
      (void)USBH_ThreadFlagsSet (thread_id, event);
    }
  }
}
