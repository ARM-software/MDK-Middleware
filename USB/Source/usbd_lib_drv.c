/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_drv.c
 * Purpose: USB Device - Driver access module
 *----------------------------------------------------------------------------*/

#include "usbd_lib_drv.h"

#include "usb_lib_rte.h"
#include "usbd_lib_config_def.h"
#include "usbd_evr.h"

// Driver Access Settings
#define USBD_DRIVER_RETRY_COUNT         3U      // Maximum number of retries
#define USBD_DRIVER_RETRY_TIMEOUT       100U    // Timeout between retries (in ms)


/// \brief       Get driver version
/// \param[in]   device Device Index
/// \return      \ref ARM_DRIVER_VERSION
ARM_DRIVER_VERSION USBD_DriverGetVersion (uint8_t device) {
  return (usbd_dev_ptr[device]->driver->GetVersion());
}

/// \brief       Get driver capabilities
/// \param[in]   device Device Index
/// \return      \ref ARM_USBD_CAPABILITIES
ARM_USBD_CAPABILITIES USBD_DriverGetCapabilities (uint8_t device) {
  return (usbd_dev_ptr[device]->driver->GetCapabilities());
}

/// \brief       Initialize USB Device Interface
/// \param[in]   device             Device Index
/// \param[in]   cb_device_event    Pointer to \ref ARM_USBD_SignalDeviceEvent
/// \param[in]   cb_endpoint_event  Pointer to \ref ARM_USBD_SignalEndpointEvent
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBD_DriverInitialize (uint8_t device, ARM_USBD_SignalDeviceEvent_t cb_device_event, ARM_USBD_SignalEndpointEvent_t cb_endpoint_event) {
  int32_t driver_status;
  uint8_t retry;

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_semaphore_id[device], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    driver_status = usbd_dev_ptr[device]->driver->Initialize (cb_device_event, cb_endpoint_event);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);

  (void)USBD_SemaphoreRelease (usbd_driver_semaphore_id[device]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBD_Driver_Initialize(device);
    return usbOK;
  } else {
    EvrUSBD_Driver_InitializeFailed(device, driver_status);
    return usbDriverError;
  }
}

/// \brief       De-initialize USB Device Interface
/// \param[in]   device Device Index
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBD_DriverUninitialize (uint8_t device) {
  int32_t driver_status;
  uint8_t retry;

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_semaphore_id[device], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    driver_status = usbd_dev_ptr[device]->driver->Uninitialize ();
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);

  (void)USBD_SemaphoreRelease (usbd_driver_semaphore_id[device]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBD_Driver_Uninitialize(device);
    return usbOK;
  } else {
    EvrUSBD_Driver_UninitializeFailed(device, driver_status);
    return usbDriverError;
  }
}

/// \brief       Control USB Device Interface Power
/// \param[in]   device Device Index
/// \param[in]   state  Power state \ref ARM_POWER_STATE
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBD_DriverPowerControl (uint8_t device, ARM_POWER_STATE state) {
  int32_t driver_status;
  uint8_t retry;

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_semaphore_id[device], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    driver_status = usbd_dev_ptr[device]->driver->PowerControl (state);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);

  (void)USBD_SemaphoreRelease (usbd_driver_semaphore_id[device]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBD_Driver_PowerControl(device, state);
    return usbOK;
  } else {
    EvrUSBD_Driver_PowerControlFailed(device, state, driver_status);
    return usbDriverError;
  }
}

/// \brief       Connect USB Device
/// \param[in]   device Device Index
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBD_DriverDeviceConnect (uint8_t device) {
  int32_t driver_status;
  uint8_t retry;

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_semaphore_id[device], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    driver_status = usbd_dev_ptr[device]->driver->DeviceConnect ();
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);

  (void)USBD_SemaphoreRelease (usbd_driver_semaphore_id[device]);

  if (driver_status != ARM_DRIVER_OK) {
    EvrUSBD_Driver_DeviceConnectFailed(device, driver_status);
    return usbDriverError;
  } else {
    EvrUSBD_Driver_DeviceConnect(device);
    return usbOK;
  }
}

/// \brief       Disconnect USB Device
/// \param[in]   device Device Index
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBD_DriverDeviceDisconnect (uint8_t device) {
  int32_t driver_status;
  uint8_t retry;

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_semaphore_id[device], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    driver_status = usbd_dev_ptr[device]->driver->DeviceDisconnect ();
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);

  (void)USBD_SemaphoreRelease (usbd_driver_semaphore_id[device]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBD_Driver_DeviceDisconnect(device);
    return usbOK;
  } else {
    EvrUSBD_Driver_DeviceDisconnectFailed(device, driver_status);
    return usbDriverError;
  }
}

/// \brief       Get current USB Device State
/// \param[in]   device Device Index
/// \return      Device State \ref ARM_USBD_STATE
USBD_STATE USBD_DriverDeviceGetState (uint8_t device) {
  ARM_USBD_STATE hw_val;
  USBD_STATE     val;

  hw_val     = usbd_dev_ptr[device]->driver->DeviceGetState ();
  val.vbus   = hw_val.vbus;
  switch (hw_val.speed) {
    case ARM_USB_SPEED_LOW:
      val.speed = USB_SPEED_LOW;
      break;
    case ARM_USB_SPEED_FULL:
      val.speed = USB_SPEED_FULL;
      break;
    case ARM_USB_SPEED_HIGH:
      val.speed = USB_SPEED_HIGH;
      break;
    default:
      val.speed = USB_SPEED_LOW;
      break;
  }
  val.active = hw_val.active;

  EvrUSBD_Driver_DeviceGetState(device, hw_val);

  return val;
}

/// \brief       Trigger USB Remote Wakeup
/// \param[in]   device Device Index
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBD_DriverDeviceRemoteWakeup (uint8_t device) {
  int32_t driver_status;
  uint8_t retry;

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_semaphore_id[device], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    driver_status = usbd_dev_ptr[device]->driver->DeviceRemoteWakeup ();
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);

  (void)USBD_SemaphoreRelease (usbd_driver_semaphore_id[device]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBD_Driver_DeviceRemoteWakeup(device);
    return usbOK;
  } else {
    EvrUSBD_Driver_DeviceRemoteWakeupFailed(device, driver_status);
    return usbDriverError;
  }
}

/// \brief       Set USB Device Address
/// \param[in]   device    Device Index
/// \param[in]   dev_addr  Device Address
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBD_DriverDeviceSetAddress (uint8_t device, uint8_t dev_addr) {
  int32_t driver_status;
  uint8_t retry;

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_semaphore_id[device], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    driver_status = usbd_dev_ptr[device]->driver->DeviceSetAddress (dev_addr);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);

  (void)USBD_SemaphoreRelease (usbd_driver_semaphore_id[device]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBD_Driver_DeviceSetAddress(device, dev_addr);
    return usbOK;
  } else {
    EvrUSBD_Driver_DeviceSetAddressFailed(device, dev_addr, driver_status);
    return usbDriverError;
  }
}

/// \brief       Read setup packet received over Control Endpoint
/// \param[in]   device Device Index
/// \param[out]  setup  Pointer to buffer for setup packet
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBD_DriverReadSetupPacket (uint8_t device, uint8_t *data) {
  int32_t driver_status;
  uint8_t retry;

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_semaphore_id[device], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    driver_status = usbd_dev_ptr[device]->driver->ReadSetupPacket (data);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);

  (void)USBD_SemaphoreRelease (usbd_driver_semaphore_id[device]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBD_Driver_ReadSetupPacket(device, (const void *)data);
    return usbOK;
  } else {
    EvrUSBD_Driver_ReadSetupPacketFailed(device, driver_status);
    return usbDriverError;
  }
}

/// \brief       Configure USB Endpoint
/// \param[in]   device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \param[in]   ep_type  Endpoint Type (ARM_USB_ENDPOINT_xxx)
/// \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBD_DriverEndpointConfigure (uint8_t device, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size) {
  int32_t driver_status;
  uint8_t ep_idx, sem_idx;
  uint8_t retry;

  ep_idx  = (ep_addr & 0x0FU) + ((ep_addr & 0x80U) >> 3);

  sem_idx = (uint8_t)(device * usbd_ep_max_num * 2U) + (ep_addr & 0x0FU);
  if (ep_idx >= 16U) {
    sem_idx += usbd_ep_max_num;
  }

  if (usbd_dev_ptr[device]->data_ptr->endpoint_active[ep_idx] != 0U) {
    if (USBD_DriverEndpointTransferAbort(device, ep_addr) != usbOK) {
      return usbDriverError;
    }
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_ep_semaphore_id[sem_idx], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    driver_status = usbd_dev_ptr[device]->driver->EndpointConfigure (ep_addr, ep_type, ep_max_packet_size);
    switch (driver_status) {
      case ARM_DRIVER_ERROR:              // Unspecified error -> retry
      case ARM_DRIVER_ERROR_BUSY:         // Driver is busy    -> retry
      case ARM_DRIVER_ERROR_TIMEOUT:      // Timeout occurred  -> retry
        retry--;
        if (retry > 0U) {
          (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
        }
        break;
      case ARM_DRIVER_OK:                 // Operation succeeded             -> exit without retry
      case ARM_DRIVER_ERROR_UNSUPPORTED:  // Operation not supported         -> exit without retry
      case ARM_DRIVER_ERROR_PARAMETER:    // Parameter error                 -> exit without retry
      case ARM_DRIVER_ERROR_SPECIFIC:     // Start of driver specific errors -> exit without retry
      default:                            // Unknown                         -> exit without retry
        retry = 0U;
        break;
    }
  } while (retry != 0U);

  (void)USBD_SemaphoreRelease (usbd_driver_ep_semaphore_id[sem_idx]);

  switch (driver_status) {
    case ARM_DRIVER_OK:
      EvrUSBD_Driver_EndpointConfigure(device, ep_addr, ep_type, ep_max_packet_size);
      return usbOK;
    case ARM_DRIVER_ERROR_BUSY:
      EvrUSBD_Driver_EndpointConfigureFailed(device, ep_addr, ep_type, ep_max_packet_size, driver_status);
      return usbDriverBusy;
    default:
      EvrUSBD_Driver_EndpointConfigureFailed(device, ep_addr, ep_type, ep_max_packet_size, driver_status);
      return usbDriverError;
  }
}

/// \brief       Unconfigure USB Endpoint
/// \param[in]   device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBD_DriverEndpointUnconfigure (uint8_t device, uint8_t ep_addr) {
  int32_t driver_status;
  uint8_t ep_idx, sem_idx;
  uint8_t retry;

  ep_idx = (ep_addr & 0x0FU) + ((ep_addr & 0x80U) >> 3);

  sem_idx = (uint8_t)(device * usbd_ep_max_num * 2U) + (ep_addr & 0x0FU);
  if (ep_idx >= 16U) {
    sem_idx += usbd_ep_max_num;
  }

  if (usbd_dev_ptr[device]->data_ptr->endpoint_active[ep_idx] != 0U) {
    if (USBD_DriverEndpointTransferAbort(device, ep_addr) != usbOK) {
      return usbDriverError;
    }
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_ep_semaphore_id[sem_idx], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    driver_status = usbd_dev_ptr[device]->driver->EndpointUnconfigure (ep_addr);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);

  (void)USBD_SemaphoreRelease (usbd_driver_ep_semaphore_id[sem_idx]);

  switch (driver_status) {
    case ARM_DRIVER_OK:
      EvrUSBD_Driver_EndpointUnconfigure(device, ep_addr);
      return usbOK;
    case ARM_DRIVER_ERROR_BUSY:
      EvrUSBD_Driver_EndpointUnconfigureFailed(device, ep_addr, driver_status);
      return usbDriverBusy;
    default:
      EvrUSBD_Driver_EndpointUnconfigureFailed(device, ep_addr, driver_status);
      return usbDriverError;
  }
}

/// \brief       Set/Clear Stall for USB Endpoint
/// \param[in]   device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \param[in]   stall  Operation
///               - \b false Clear
///               - \b true Set
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBD_DriverEndpointStall (uint8_t device, uint8_t ep_addr, bool stall) {
  int32_t driver_status;
  uint8_t ep_idx, sem_idx;
  uint8_t retry;

  ep_idx = (ep_addr & 0x0FU) + ((ep_addr & 0x80U) >> 3);

  sem_idx = (uint8_t)(device * usbd_ep_max_num * 2U) + (ep_addr & 0x0FU);
  if (ep_idx >= 16U) {
    sem_idx += usbd_ep_max_num;
  }

  if (usbd_dev_ptr[device]->data_ptr->endpoint_active[ep_idx] != 0U) {
    if (USBD_DriverEndpointTransferAbort(device, ep_addr) != usbOK) {
      return usbDriverError;
    }
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_ep_semaphore_id[sem_idx], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    driver_status = usbd_dev_ptr[device]->driver->EndpointStall (ep_addr, stall);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);

  (void)USBD_SemaphoreRelease (usbd_driver_ep_semaphore_id[sem_idx]);

  switch (driver_status) {
    case ARM_DRIVER_OK:
      EvrUSBD_Driver_EndpointStall(device, ep_addr, stall);
      return usbOK;
    case ARM_DRIVER_ERROR_BUSY:
      EvrUSBD_Driver_EndpointStallFailed(device, ep_addr, stall, driver_status);
      return usbDriverBusy;
    default:
      EvrUSBD_Driver_EndpointStallFailed(device, ep_addr, stall, driver_status);
      return usbDriverError;
  }
}

/// \brief       Read data from or Write data to USB Endpoint
/// \param[in]   device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \param[out]  data Pointer to buffer for data to read or with data to write
/// \param[in]   num  Number of data bytes to transfer
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBD_DriverEndpointTransfer (uint8_t device, uint8_t ep_addr, uint8_t *data, uint32_t num) {
  int32_t  driver_status;
  uint32_t ep_msk;
  uint8_t  ep_idx, sem_idx;
  uint8_t  retry;

  ep_idx = (ep_addr & 0x0FU) + ((ep_addr & 0x80U) >> 3);
  ep_msk =  1UL << ep_idx;

  sem_idx = (uint8_t)(device * usbd_ep_max_num * 2U) + (ep_addr & 0x0FU);
  if (ep_idx >= 16U) {
    sem_idx += usbd_ep_max_num;
  }

  if ((usbd_dev_ptr[device]->data_ptr->endpoint_halt & ep_msk) != 0U) {
    return usbDriverBusy;
  }
  if (usbd_dev_ptr[device]->data_ptr->endpoint_active[ep_idx] != 0U) {
    if ((ep_addr & 0x0FU) == 0U) {
      // For Endpoint 0 Abort active transfer so new one can be restarted
      if (USBD_DriverEndpointTransferAbort(device, ep_addr) != usbOK) {
        return usbDriverError;
      }
    } else {
      return usbDriverBusy;
    }
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_ep_semaphore_id[sem_idx], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  usbd_dev_ptr[device]->data_ptr->endpoint_active[ep_idx] = 1U;

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    driver_status = usbd_dev_ptr[device]->driver->EndpointTransfer (ep_addr, data, num);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);

  if (driver_status != ARM_DRIVER_OK) {
    usbd_dev_ptr[device]->data_ptr->endpoint_active[ep_idx] = 0U;
  }

  (void)USBD_SemaphoreRelease (usbd_driver_ep_semaphore_id[sem_idx]);

  switch (driver_status) {
    case ARM_DRIVER_OK:
      EvrUSBD_Driver_EndpointTransfer(device, ep_addr, (const uint8_t *)data, num);
      return usbOK;
    case ARM_DRIVER_ERROR_BUSY:
      EvrUSBD_Driver_EndpointTransferFailed(device, ep_addr, driver_status);
      return usbDriverBusy;
    default:
      EvrUSBD_Driver_EndpointTransferFailed(device, ep_addr, driver_status);
      return usbDriverError;
  }
}

/// \brief       Get result of USB Endpoint transfer
/// \param[in]   device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \return      number of successfully transferred data bytes
uint32_t USBD_DriverEndpointTransferGetResult (uint8_t device, uint8_t ep_addr) {
  uint32_t val;
  uint8_t  ep_idx, sem_idx;
  uint8_t  retry;

  ep_idx = (ep_addr & 0x0FU) + ((ep_addr & 0x80U) >> 3);

  sem_idx = (uint8_t)(device * usbd_ep_max_num * 2U) + (ep_addr & 0x0FU);
  if (ep_idx >= 16U) {
    sem_idx += usbd_ep_max_num;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_ep_semaphore_id[sem_idx], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return 0U;
  }

  val = usbd_dev_ptr[device]->driver->EndpointTransferGetResult (ep_addr);

  (void)USBD_SemaphoreRelease (usbd_driver_ep_semaphore_id[sem_idx]);

  EvrUSBD_Driver_EndpointTransferGetResult(device, ep_addr, val);

  return val;
}

/// \brief       Abort current USB Endpoint transfer
/// \param[in]   device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \return      status code that indicates the execution status of the function as defined with \ref usbStatus
usbStatus USBD_DriverEndpointTransferAbort (uint8_t device, uint8_t ep_addr) {
  int32_t driver_status;
  uint8_t ep_idx, sem_idx;
  uint8_t retry;

  ep_idx = (ep_addr & 0x0FU) + ((ep_addr & 0x80U) >> 3);

  sem_idx = (uint8_t)(device * usbd_ep_max_num * 2U) + (ep_addr & 0x0FU);
  if (ep_idx >= 16U) {
    sem_idx += usbd_ep_max_num;
  }

  if (usbd_dev_ptr[device]->data_ptr->endpoint_active[ep_idx] == 0U) {
    EvrUSBD_Driver_EndpointTransferAbort(device, ep_addr);
    return usbOK;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_ep_semaphore_id[sem_idx], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return usbDriverBusy;
  }

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    driver_status = usbd_dev_ptr[device]->driver->EndpointTransferAbort (ep_addr);
    if (driver_status == ARM_DRIVER_OK) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);

  if (driver_status == ARM_DRIVER_OK) {
    usbd_dev_ptr[device]->data_ptr->endpoint_active[ep_idx] = 0U;
  }

  (void)USBD_SemaphoreRelease (usbd_driver_ep_semaphore_id[sem_idx]);

  if (driver_status == ARM_DRIVER_OK) {
    EvrUSBD_Driver_EndpointTransferAbort(device, ep_addr);
    return usbOK;
  } else {
    EvrUSBD_Driver_EndpointTransferAbortFailed(device, ep_addr, driver_status);
    return usbDriverError;
  }
}

/// \brief       Get current USB Frame Number
/// \param[in]   device Device Index
/// \return      Frame Number
uint16_t USBD_DriverGetFrameNumber (uint8_t device) {
  uint16_t val;
  uint8_t  retry;

  retry = USBD_DRIVER_RETRY_COUNT;
  do {
    if (USBD_SemaphoreAcquire (usbd_driver_semaphore_id[device], 0U) == 0) { break; }
    (void)USBD_Delay (USBD_DRIVER_RETRY_TIMEOUT);
    retry--;
  } while (retry != 0U);
  if (retry == 0U) {
    return 0U;
  }

  val = usbd_dev_ptr[device]->driver->GetFrameNumber ();

  (void)USBD_SemaphoreRelease (usbd_driver_semaphore_id[device]);

  EvrUSBD_Driver_GetFrameNumber(device, val);

  return val;
}

/// \brief       Signal USB Device Event
/// \param[in]   device Device Index
/// \param[in]   event \ref USBD_dev_events
void USBD_SignalDeviceEvent(uint8_t device, uint32_t event) {
  EvrUSBD_Driver_OnSignalDeviceEvent(device, event);
  (void)USBD_ThreadFlagsSet (usbd_core_thread_id[device], event);
}

/// \brief       Signal USB Endpoint Event
/// \param[in]   device   Device Index
/// \param[in]   ep_addr  Endpoint Address
///               - ep_addr.0..3: Address
///               - ep_addr.7:    Direction
/// \param[in]   event \ref USBD_ep_events
void USBD_SignalEndpointEvent(uint8_t device, uint8_t ep_addr, uint32_t event) {
  uint8_t ep_num;
  uint8_t ep_idx;

  ep_num = (ep_addr & 0x0FU);
  ep_idx =  ep_num + ((ep_addr & 0x80U) >> 3);

  EvrUSBD_Driver_OnSignalEndpointEvent(device, ep_addr, event);

  usbd_dev_ptr[device]->data_ptr->endpoint_active[ep_idx] = 0U;
  (void)USBD_ThreadFlagsSet (*usbd_ep_thread_id_ptr[(device*usbd_ep_max_num)+ep_num], ((((uint32_t)(ep_num)) << 12) | (event << 8)));
}
