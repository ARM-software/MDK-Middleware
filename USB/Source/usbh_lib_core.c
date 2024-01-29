/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_lib_core.c
 * Purpose: USB Host - Core module
 *----------------------------------------------------------------------------*/

#include "usbh_lib_core.h"

#include <stdio.h>
#include <string.h>

#include "usb_lib_rte.h"
#include "usb_lib_ver.h"
#include "usbh_lib_config_def.h"
#include "usbh_lib_drv.h"
#include "usbh_lib_cdc.h"
#include "usbh_lib_hid.h"
#include "usbh_lib_msc.h"
#include "usbh_lib_mem.h"
#include "usbh_evr.h"


/************************** Constant definitions ******************************/


/************************** Global variable and memory definitions ************/


/************************** Local function declarations ***********************/

static usbStatus CheckController                  (uint8_t ctrl);
static usbStatus CheckDeviceInstance              (uint8_t instance);
static usbStatus CheckDeviceInstanceConfigured    (uint8_t instance);
static usbStatus CheckPipe                        (const USBH_PIPE *ptr_pipe);
static usbStatus USBH_DefaultPipeCreate           (uint8_t ctrl);
static usbStatus USBH_DefaultPipeDelete           (uint8_t ctrl);
static usbStatus USBH_DefaultPipeModify           (uint8_t ctrl, uint8_t device);
static usbStatus USBH_PipeDoPing                  (USBH_PIPE_HANDLE pipe_hndl);
static usbStatus USBH_PipeSendSetup               (USBH_PIPE_HANDLE pipe_hndl, const USB_SETUP_PACKET *setup_packet);
static usbStatus USBH_PrepareEnumerateDevice      (uint8_t ctrl, uint8_t port, uint8_t speed);
static usbStatus USBH_EnumerateDevice             (uint8_t ctrl, uint8_t port, uint8_t speed);
static uint8_t   USBH_FindRecoveryDevice          (uint8_t ctrl, uint8_t port);
static usbStatus USBH_RecoveryEnumerateDevice     (uint8_t ctrl, uint8_t port);
static usbStatus USBH_UninitializeDevices         (uint8_t ctrl, uint8_t port);
static void      USBH_Engine                      (uint8_t ctrl, uint8_t port, uint32_t event);


/************************** User accessible functions *************************/

/// \brief Get version of USB Host stack
/// \return                             version (major.minor.revision : mmnnnrrrr decimal)
uint32_t USBH_GetVersion (void) {
  return usb_lib_version;
}

/// \brief Initialize USB Host stack and controller
/// \param[in]     ctrl                 index of USB Host controller.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_Initialize (uint8_t ctrl) {
  USBH_HCI  *ptr_hci;
  uint32_t   port_num;
  uint8_t    port;
  usbStatus  status;

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  USBH_Debug_Initialize(ctrl);
#endif

  EvrUSBH_Core_Initialize(ctrl);

  /* Make a version check just to keep the usb_lib_version variable for Component Viewer */
  if (usb_lib_version == 0U) {
    status = usbUnknownError;
    goto exit;
  }

  status = CheckController (ctrl);
  if (status != usbOK) {
    goto exit;
  }

  ptr_hci = &usbh_hci[ctrl];
  if (ptr_hci->init != 0U) {
    return usbOK;
  }

  usbh_drv_version[ctrl]  = USBH_DriverGetVersion      (ctrl);
  usbh_capabilities[ctrl] = USBH_DriverGetCapabilities (ctrl);

  memset ((void *)ptr_hci,             0, sizeof(USBH_HCI));
  memset ((void *)usbh_pipe_ptr[ctrl], 0, (*usbh_pipe_num_ptr[ctrl]) * sizeof (USBH_PIPE));

  if (usbh_driver_semaphore_id[ctrl] == NULL) {
    usbh_driver_semaphore_id[ctrl] = USBH_SemaphoreCreate (usbhSemaphoreCore, ctrl);
    if (usbh_driver_semaphore_id[ctrl] == NULL) {
      status = usbSemaphoreError;
      goto cleanup_and_exit;
    }
  }
  if (usbh_def_pipe_mutex_id[ctrl] == NULL) {
    usbh_def_pipe_mutex_id[ctrl] = USBH_MutexCreate (usbhMutexCore, ctrl);
    if (usbh_def_pipe_mutex_id[ctrl] == NULL) {
      status = usbMutexError;
      goto cleanup_and_exit;
    }
  }
  if (usbh_debounce_timer_id[ctrl] == NULL) {
    usbh_debounce_timer_id[ctrl] = USBH_TimerCreate (ctrl);
    if (usbh_debounce_timer_id[ctrl] == NULL) {
      status = usbTimerError;
      goto cleanup_and_exit;
    }
  }
  if (usbh_core_thread_id[ctrl] == NULL) {
    usbh_core_thread_id[ctrl] = USBH_ThreadCreate (usbhThreadCore, ctrl);
    if (usbh_core_thread_id[ctrl] == NULL) {
      status = usbThreadError;
      goto cleanup_and_exit;
    }
  }

  // Initialize memory pool for USB data
  status = USBH_MemoryInitialize   (ctrl);
  if (status != usbOK) {
    goto cleanup_and_exit;
  }
  status = USBH_DriverInitialize   (ctrl, *usbh_cb_port_event_ptr[ctrl], *usbh_cb_pipe_event_ptr[ctrl]);
  if (status != usbOK) {
    goto cleanup_and_exit;
  }
  status = USBH_DriverPowerControl (ctrl, ARM_POWER_FULL);
  if (status != usbOK) {
    goto cleanup_and_exit;
  }

  // Create Default Pipe
  status = USBH_DefaultPipeCreate (ctrl);
  if (status != usbOK) {
    goto cleanup_and_exit;
  }

  port_num = 0U;
  for (port = 0U; port < 15U; port++) {
    if ((usbh_capabilities[ctrl].port_mask & (1UL << port)) != 0U) {
      port_num ++;
      // Set VBUS high
      (void)USBH_DriverPortVbusOnOff (ctrl, port, true);
    } else {
      if ((usbh_capabilities[ctrl].port_mask & ~((1UL << port) - 1U)) == 0U) {
        break;
      }
    }
  }
  if (port_num == 0U) {
    port_num = 1U;
  }
  ptr_hci->port_mem_max = USBH_MemoryGetPoolSize(ctrl) / port_num;
  (void)USBH_Delay (500U);              // Delay for VBUS high to stabilize

cleanup_and_exit:
  if (status == usbOK) {
    ptr_hci->init = 1U;
  } else {
    if (usbh_core_thread_id[ctrl] != NULL) {
      (void)USBH_ThreadTerminate (usbh_core_thread_id[ctrl]);
      usbh_core_thread_id[ctrl] = NULL;
    }
    if (usbh_debounce_timer_id[ctrl] != NULL) {
      (void)USBH_TimerDelete (usbh_debounce_timer_id[ctrl]);
      usbh_debounce_timer_id[ctrl] = NULL;
    }
    if (usbh_def_pipe_mutex_id[ctrl] != NULL) {
      (void)USBH_MutexDelete (usbh_def_pipe_mutex_id[ctrl]);
      usbh_def_pipe_mutex_id[ctrl] = NULL;
    }
    if (usbh_driver_semaphore_id[ctrl] != NULL) {
      (void)USBH_SemaphoreDelete (usbh_driver_semaphore_id[ctrl]);
      usbh_driver_semaphore_id[ctrl] = NULL;
    }
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_InitializeFailed(ctrl, status);
  }
#endif
  return status;
}

/// \brief De-initialize USB Host stack and controller
/// \param[in]     ctrl                 index of USB Host controller.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_Uninitialize (uint8_t ctrl) {
  USBH_HCI  *ptr_hci;
  uint8_t    port;
  usbStatus  status, first_err_status;

  EvrUSBH_Core_Uninitialize(ctrl);

  first_err_status = CheckController (ctrl);
  if (first_err_status != usbOK) {
    goto exit;
  }

  ptr_hci = &usbh_hci[ctrl];
  if (ptr_hci->init == 0U) {
    return usbOK;
  }

  // Delete Default Pipe
  first_err_status = USBH_DefaultPipeDelete (ctrl);

  for (port = 0U; port < 15U; port++) {
    if ((usbh_capabilities[ctrl].port_mask & (1UL << port)) != 0U) {
      if ((ptr_hci->port_con & (1UL << port)) != 0U) {
        // Uninitialize devices on active ports
        status = USBH_UninitializeDevices (ctrl, port);
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
      }
      // Drive VBUS low
      status = USBH_DriverPortVbusOnOff (ctrl, port, false);
      if ((status != usbOK) && (first_err_status == usbOK)) {
        first_err_status = status;
      }
    } else {
      if ((usbh_capabilities[ctrl].port_mask & ~((1UL << port) - 1U)) == 0U) {
        break;
      }
    }
  }
  status = USBH_DriverPowerControl (ctrl, ARM_POWER_OFF);
  if ((status != usbOK) && (first_err_status == usbOK)) {
    first_err_status = status;
  }
  status = USBH_DriverUninitialize (ctrl);
  if ((status != usbOK) && (first_err_status == usbOK)) {
    first_err_status = status;
  }

  if (usbh_core_thread_id[ctrl] != NULL) {
    if (USBH_ThreadTerminate (usbh_core_thread_id[ctrl]) == 0) {
      usbh_core_thread_id[ctrl] = NULL;
    } else {
      if (first_err_status == usbOK) {
        first_err_status = usbThreadError;
      }
    }
  }
  if (usbh_debounce_timer_id[ctrl] != NULL) {
    if (USBH_TimerDelete (usbh_debounce_timer_id[ctrl]) == 0) {
      usbh_debounce_timer_id[ctrl] = NULL;
    } else {
      if (first_err_status == usbOK) {
        first_err_status = usbTimerError;
      }
    }
  }
  if (usbh_def_pipe_mutex_id[ctrl] != NULL) {
    if (USBH_MutexDelete (usbh_def_pipe_mutex_id[ctrl]) == 0) {
      usbh_def_pipe_mutex_id[ctrl] = NULL;
    } else {
      if (first_err_status == usbOK) {
        first_err_status = usbMutexError;
      }
    }
  }
  if (usbh_driver_semaphore_id[ctrl] != NULL) {
    if (USBH_SemaphoreDelete (usbh_driver_semaphore_id[ctrl]) == 0) {
      usbh_driver_semaphore_id[ctrl] = NULL;
    } else {
      if (first_err_status == usbOK) {
        first_err_status = usbSemaphoreError;
      }
    }
  }

  // De-initialize memory pool
  status = USBH_MemoryUninitialize (ctrl);
  if ((status != usbOK) && (first_err_status == usbOK)) {
    first_err_status = status;
  }

  // Clear all runtime information
  memset((void *)ptr_hci, 0, sizeof(USBH_HCI));

  (void)USBH_Delay (100U);              // Delay 100 ms

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (first_err_status != usbOK) {
    EvrUSBH_Core_UninitializeFailed(ctrl, first_err_status);
  }
#endif
  return first_err_status;
}

/// \brief Suspend a root HUB port on specified controller
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     port                 root HUB port.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_Port_Suspend (uint8_t ctrl, uint8_t port) {
  usbStatus status;

  EvrUSBH_Core_PortSuspend(ctrl, port);

  status = CheckController (ctrl);
  if (status == usbOK) {
    status = USBH_DriverPortSuspend (ctrl, port);
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_PortSuspendFailed(ctrl, port, status);
  }
#endif
  return status;
}

/// \brief Resume a root HUB port on specified controller
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     port                 root HUB port.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_Port_Resume (uint8_t ctrl, uint8_t port) {
  usbStatus status;

  EvrUSBH_Core_PortResume(ctrl, port);

  status = CheckController (ctrl);
  if (status == usbOK) {
    status = USBH_DriverPortResume (ctrl, port);
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_PortResumeFailed(ctrl, port, status);
  }
#endif
  return status;
}

/// \brief Get index of USB Host controller to which USB Device is connected
/// \param[in]     device               index of USB Device.
/// \return                             index of USB Host controller or non-existing USB Host controller :
///                                       - value != 255 : index of USB Host controller
///                                       - value 255 :    non-existing USB Host controller
uint8_t USBH_Device_GetController (uint8_t device) {
  uint8_t ctrl;

  if (CheckDeviceInstanceConfigured (device) == usbOK) { 
    ctrl = usbh_dev[device].ctrl;
  } else {
    ctrl = 255U;
  }

  EvrUSBH_Core_DeviceGetController(device, ctrl);
  return ctrl;
}

/// \brief Get index of USB Host Root HUB port to which USB Device is connected
/// \param[in]     device               index of USB Device.
/// \return                             index of USB Host Root HUB port or non-existing USB Host Root HUB port :
///                                       - value <= 15 : index of USB Host Root HUB port
///                                       - value 255 :   non-existing USB Host Root HUB port
uint8_t USBH_Device_GetPort (uint8_t device) {
  uint8_t port;

  if (CheckDeviceInstanceConfigured (device) == usbOK) {
    port = usbh_dev[device].hub_port;
  } else {
    port = 255U;
  }

  EvrUSBH_Core_DeviceGetPort(device, port);
  return port;
}

/// \brief Get status of USB Device
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_Device_GetStatus (uint8_t device) {
  usbStatus status;

  status = CheckDeviceInstance (device);

  EvrUSBH_Core_DeviceGetStatus(device, status);
  return status;
}

/// \brief Get communication speed of USB Device
/// \param[in]     device               index of USB Device.
/// \return                             communication speed :
///                                       - USB_SPEED_LOW  = low speed
///                                       - USB_SPEED_FULL = full speed
///                                       - USB_SPEED_HIGH = high speed
int32_t USBH_Device_GetSpeed (uint8_t device) {
  uint8_t speed;

  if (CheckDeviceInstanceConfigured (device) == usbOK) {
    speed = usbh_dev[device].dev_speed;
  } else {
    speed = USB_SPEED_LOW;
  }

  EvrUSBH_Core_DeviceGetSpeed(device, speed);
  return (int32_t)speed;
}

/// \brief Get communication address of USB Device
/// \param[in]     device               index of USB Device.
/// \return                             enumerated address or invalid address :
///                                       - value <= 127 : enumerated address
///                                       - value 255 :    invalid address
uint8_t USBH_Device_GetAddress (uint8_t device) {
  uint8_t address;

  if (CheckDeviceInstanceConfigured (device) == usbOK) {
    address = usbh_dev[device].dev_addr;
  } else {
    address = 255U;
  }

  EvrUSBH_Core_DeviceGetAddress(device, address);
  return address;
}

/// \brief Get Vendor ID (VID) of USB Device
/// \param[in]     device               index of USB Device.
/// \return                             Vendor ID.
uint16_t USBH_Device_GetVID (uint8_t device) {
  uint16_t vid;

  if (CheckDeviceInstanceConfigured (device) == usbOK) {
    vid = usbh_dev[device].vid;
  } else {
    vid = 0U;
  }

  EvrUSBH_Core_DeviceGetVID(device, vid);
  return vid;
}

/// \brief Get Product ID (PID) of USB Device
/// \param[in]     device               index of USB Device.
/// \return                             Product ID.
uint16_t USBH_Device_GetPID (uint8_t device) {
  uint16_t pid;

  if (CheckDeviceInstanceConfigured (device) == usbOK) {
    pid = usbh_dev[device].pid;
  } else {
    pid = 0U;
  }

  EvrUSBH_Core_DeviceGetPID(device, pid);
  return pid;
}

/// \brief Get String Descriptor of USB Device
/// \param[in]     device               index of USB Device.
/// \param[in]     index                index of string descriptor.
/// \param[in]     language_id          language ID.
/// \param[out]    descriptor_data      pointer to where descriptor data will be read.
/// \param[in]     descriptor_length    maximum descriptor length.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_Device_GetStringDescriptor (uint8_t device, uint8_t index, uint16_t language_id, uint8_t *descriptor_data, uint16_t descriptor_length) {
  usbStatus status;

  status = CheckDeviceInstance (device);
  if (status == usbOK) {
    status = USBH_DeviceRequest_GetDescriptor (device, USB_REQUEST_TO_DEVICE, USB_STRING_DESCRIPTOR_TYPE, index, language_id, descriptor_data, descriptor_length);
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_DeviceGetStringDescriptorFailed(device, index, language_id, descriptor_length, status);
  } else {
    EvrUSBH_Core_DeviceGetStringDescriptor(device, index, language_id, descriptor_length);
  }
#endif
  return status;
}

/// \brief Callback function called when some event has happened on corresponding controller and port
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     port                 index of Root HUB port.
/// \param[in]     device               index of USB Device :
///                                       - value <= 127: index of of USB Device for device notifications
///                                       - value 255: no device information for port notifications
/// \param[in]     notify               notification :
///                                       - USBH_NOTIFY_CONNECT               = Port connection happened
///                                       - USBH_NOTIFY_DISCONNECT            = Port disconnection happened
///                                       - USBH_NOTIFY_OVERCURRENT           = Port overcurrent happened
///                                       - USBH_NOTIFY_REMOTE_WAKEUP         = Port remote wakeup signaling happened
///                                       - USBH_NOTIFY_READY                 = Device was successfully enumerated, initialized and is ready for communication
///                                       - USBH_NOTIFY_UNKNOWN_DEVICE        = Device was successfully enumerated but there is no driver for it
///                                       - USBH_NOTIFY_INSUFFICIENT_POWER    = Device requires more power consumption than available
///                                       - USBH_NOTIFY_CONFIGURATION_FAILED  = Device was not successfully configured (not enough resources)
///                                       - USBH_NOTIFY_INITIALIZATION_FAILED = Device was not successfully initialized
__WEAK void USBH_Notify (uint8_t ctrl, uint8_t port, uint8_t device, USBH_NOTIFY notify) {
  // Dummy function that gets replaced by user externally
  (void)ctrl;
  (void)port;
  (void)device;
  (void)notify;
}

/// \brief Get Device instance of Custom Class Device
/// \param[in]     instance             instance of Custom Class Device.
/// \return                             instance of Device or non-existing Device instance :
///                                       - value <= 127 : instance of Device
///                                       - value 255 :    non-existing Device instance
uint8_t USBH_CustomClass_GetDevice (uint8_t instance) {
  uint8_t i, device;

  device = 255U;

  if (instance < usbh_cls_num) {
    // Find custom class device for requested custom class instance
    for (i = 0U; i <= usbh_dev_num; i++) {
      if ((usbh_dev[i].state.in_use == 1U) && (usbh_dev[i].class_custom != 0U) && (usbh_dev[i].class_instance == instance)) {
        device = i;                     // Found requested custom class instance
        break;
      }
    }
  }

  EvrUSBH_CC_GetDevice(instance, device);
  return device;
}

/// \brief Get status of Custom Class Device
/// \param[in]     instance             instance of Custom Class Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_CustomClass_GetStatus (uint8_t instance) {
  const USBH_DEV *ptr_dev;
        usbStatus status;
        uint8_t   i;

  if (instance < usbh_cls_num) {
    ptr_dev = NULL;
    status  = usbDeviceError;

    // Find custom class device for requested custom class instance
    for (i = 0U; i <= usbh_dev_num; i++) {
      if ((usbh_dev[i].class_custom != 0U) && (usbh_dev[i].class_instance == instance)) {
        ptr_dev = &usbh_dev[i];
        if ((ptr_dev->state.configured == 1U) && (ptr_dev->state.initialized == 1U)) {
          status = usbOK;
        }
        break;
      }
    }

  } else {
    status = usbClassErrorCustom;
  }

  EvrUSBH_CC_GetStatus(instance, status);
  return status;
}

/// \brief Callback function called when custom class device is connected and needs
///        to configure resources used by custom class device instance
/// \param[in]     device               index of USB Device.
/// \param[in]     ptr_dev_desc         pointer to device descriptor.
/// \param[in]     ptr_cfg_desc         pointer to configuration descriptor.
/// \return                             index of configured custom class device instance or configuration failed :
///                                       - value <= 127 : index of configured custom class device instance
///                                       - value 255 :    configuration failed
__WEAK uint8_t USBH_CustomClass_Configure (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc) {

  (void)device;
  (void)ptr_dev_desc;
  (void)ptr_cfg_desc;

  return 255U;
}

/// \brief Callback function called when custom class device is disconnected and needs
///        to de-configure resources used by custom class device instance
/// \param[in]     instance             index of custom class device instance.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
__WEAK usbStatus USBH_CustomClass_Unconfigure (uint8_t instance) {

  (void)instance;

  return usbDeviceError;
}

/// \brief Callback function called when custom class device is connected and needs
///        to initialize custom class device instance
/// \param[in]     instance             index of custom class device instance.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
__WEAK usbStatus USBH_CustomClass_Initialize (uint8_t instance) {

  (void)instance;

  return usbDeviceError;
}

/// \brief Callback function called when custom class device is disconnected and needs
///        to de-initialize custom class device instance
/// \param[in]     instance             index of custom class device instance.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
__WEAK usbStatus USBH_CustomClass_Uninitialize (uint8_t instance) {

  (void)instance;

  return usbDeviceError;
}

/// \brief Create Pipe
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \param[in]     ep_type              endpoint type.
/// \param[in]     ep_max_packet_size   endpoint maximum packet size.
/// \param[in]     ep_interval          endpoint polling interval.
/// \return                             pipe handle or pipe creation failed :
///                                       - value > 0 : pipe handle
///                                       - value 0 :   pipe creation failed
USBH_PIPE_HANDLE USBH_PipeCreate (uint8_t device, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size, uint8_t ep_interval) {
  const USBH_DEV  *ptr_dev;
        USBH_PIPE *ptr_pipe;
        uint32_t   hw_handle;

  ptr_dev  = &usbh_dev[device];
  ptr_pipe = USBH_GetFree_PIPE (ptr_dev->ctrl);
  if (ptr_pipe != NULL) {
    EvrUSBH_Core_PipeCreate(device, ep_addr, ep_type, ep_max_packet_size, ep_interval, (USBH_PIPE_HANDLE)ptr_pipe);
    hw_handle = USBH_DriverPipeCreate (ptr_dev->ctrl, ptr_dev->dev_addr, ptr_dev->dev_speed, ptr_dev->hub_addr, ptr_dev->hub_port, ep_addr, ep_type, ep_max_packet_size, ep_interval);
    if (hw_handle != 0U) {
      ptr_pipe->hw_handle        = hw_handle;
      ptr_pipe->thread_id        = NULL;
      ptr_pipe->bEndpointAddress = ep_addr;
      ptr_pipe->bmAttributes     = ep_type;
      ptr_pipe->wMaxPacketSize   = ep_max_packet_size;
      ptr_pipe->bInterval        = ep_interval;
      ptr_pipe->device           = device;
      ptr_pipe->locked           = 0U;
      ptr_pipe->transfer_active  = 0U;
      ptr_pipe->transferred      = 0U;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (ptr_pipe == NULL) {
    EvrUSBH_Core_PipeCreateFailed(device, ep_addr, ep_type, ep_max_packet_size, ep_interval);
  }
#endif
  return (USBH_PIPE_HANDLE)ptr_pipe;
}

/// \brief  Register a callback for Pipe events
/// \detail Supported only for Isochronous pipes.
/// \param[in]     pipe_hndl            pipe handle.
/// \param[in]     cb_pipe_event        pointer to pipe event callback function :
///                                       - value > 0 : pointer to pipe event callback function
///                                       - value 0 :   unregister pipe event callback
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_PipeRegisterCallback (USBH_PIPE_HANDLE pipe_hndl, USBH_PipeEvent_t cb_pipe_event) {
  USBH_PIPE *ptr_pipe;
  usbStatus  status;

  EvrUSBH_Core_PipeRegisterCallback(pipe_hndl);

  ptr_pipe = (USBH_PIPE *)pipe_hndl;
  status   = CheckPipe (ptr_pipe);

  if (status == usbOK) {
    if ((((USBH_PIPE *)ptr_pipe)->bmAttributes & USB_ENDPOINT_TYPE_MASK) != USB_ENDPOINT_TYPE_ISOCHRONOUS) {
      // If not a pipe to isochronous endpoint
      status = usbInvalidParameter;
    }
  }

  if (status == usbOK) {
    ptr_pipe->cb_pipe_event = cb_pipe_event;
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_PipeRegisterCallbackFailed(pipe_hndl, status);
  }
#endif
  return status;
}

/// \brief Update Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_PipeUpdate (USBH_PIPE_HANDLE pipe_hndl) {
  const USBH_DEV  *ptr_dev;
  const USBH_PIPE *ptr_pipe;
        usbStatus  status;

  EvrUSBH_Core_PipeUpdate(pipe_hndl);

  ptr_pipe = (USBH_PIPE *)pipe_hndl;
  status   = CheckPipe (ptr_pipe);
  if (status == usbOK) {
    ptr_dev = &usbh_dev[ptr_pipe->device];
    status  = USBH_DriverPipeModify (ptr_dev->ctrl, ptr_pipe->hw_handle, ptr_dev->dev_addr, ptr_dev->dev_speed, ptr_dev->hub_addr, ptr_dev->hub_port, ptr_pipe->wMaxPacketSize);
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_PipeUpdateFailed(pipe_hndl, status);
  }
#endif
  return status;
}

/// \brief Delete Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_PipeDelete (USBH_PIPE_HANDLE pipe_hndl) {
  const USBH_DEV  *ptr_dev;
        USBH_PIPE *ptr_pipe;
        usbStatus  status;

  EvrUSBH_Core_PipeDelete(pipe_hndl);

  ptr_pipe = (USBH_PIPE *)pipe_hndl;
  status   = CheckPipe (ptr_pipe);
  if (status == usbOK) {
    ptr_dev = &usbh_dev[ptr_pipe->device];
    status  = USBH_DriverPipeTransferAbort (ptr_dev->ctrl, ptr_pipe->hw_handle);
    if (status == usbOK) {
      status = USBH_DriverPipeDelete (ptr_dev->ctrl, ptr_pipe->hw_handle);
      if (status == usbOK) {
        memset ((void *)ptr_pipe, 0, sizeof(USBH_PIPE));
      }
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_PipeDeleteFailed(pipe_hndl, status);
  }
#endif
  return status;
}

/// \brief Reset Pipe (reset data toggle)
/// \param[in]     pipe_hndl            pipe handle.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_PipeReset (USBH_PIPE_HANDLE pipe_hndl) {
  const USBH_DEV  *ptr_dev;
  const USBH_PIPE *ptr_pipe;
        usbStatus  status;

  EvrUSBH_Core_PipeReset(pipe_hndl);

  ptr_pipe = (USBH_PIPE *)pipe_hndl;
  status   = CheckPipe (ptr_pipe);
  if (status == usbOK) {
    ptr_dev  = &usbh_dev[ptr_pipe->device];
    status = USBH_DriverPipeReset (ptr_dev->ctrl, ptr_pipe->hw_handle);
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_PipeResetFailed(pipe_hndl, status);
  }
#endif
  return status;
}

/// \brief Receive data on Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \param[out]    buf                  buffer that receives data.
/// \param[in]     len                  maximum number of bytes to receive.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_PipeReceive (USBH_PIPE_HANDLE pipe_hndl, uint8_t *buf, uint32_t len) {
  const USBH_DEV           *ptr_dev;
  const USBH_HCI           *ptr_hci;
        USBH_PIPE          *ptr_pipe;
        uint32_t            event;
        ARM_USBH_EP_HANDLE  hw_handle;
        uint8_t            *ptr_data;
        uint32_t            max_data_len;
        uint32_t            remain_len;
        uint32_t            receive_len;
        uint32_t            received_len;
        uint32_t            packet;
        uint32_t            transferred;
        uint16_t            max_packet_size;
        uint8_t             ctrl;
        bool                alloc_mem;
        usbStatus           status;
        usbStatus           mstatus;

  EvrUSBH_Core_PipeReceive(pipe_hndl, len);

  if ((buf == NULL) && (len != 0U)) {
    status = usbInvalidParameter;
    goto exit;
  }

  ptr_pipe = (USBH_PIPE *)pipe_hndl;
  status   = CheckPipe (ptr_pipe);
  if (status != usbOK) {
    goto exit;
  }
  if (ptr_pipe->locked != 0U) {
    status = usbDriverBusy;
    goto exit;
  }

  ptr_dev         = &usbh_dev[ptr_pipe->device];
  ctrl            =  ptr_dev->ctrl;
  ptr_hci         = &usbh_hci[ctrl];
  hw_handle       =  ptr_pipe->hw_handle;
  max_packet_size =  ptr_pipe->wMaxPacketSize;

  ptr_pipe->locked = 1U;

  if (((ptr_pipe->bmAttributes & 3U)==ARM_USB_ENDPOINT_ISOCHRONOUS) &&  // If Isochronous pipe
       (ptr_pipe->cb_pipe_event != NULL)) {                             // and registered callback
    // Isochronous pipe handling
    status = USBH_DriverPipeTransfer (ctrl, hw_handle, ARM_USBH_PACKET_IN, buf, len);
    ptr_pipe->locked = 0U;
    return status;
  }

  remain_len   = len;
  max_data_len = len;
  if (*usbh_mem_pool_reloc_ptr[ctrl] != 0U) {                           // If memory relocation is enabled
    if (remain_len == 0U) {                                             // If ZLP is requested
      alloc_mem = false;
    } else {
      alloc_mem = !USBH_MemoryIsInPool (ctrl, buf);
    }
    if (alloc_mem) {
      // If memory for USB must be relocated, allocate maximum memory chunk for reception
      if (max_data_len > ptr_hci->port_mem_max) {
        max_data_len = ptr_hci->port_mem_max & (~((uint32_t)max_packet_size-1U));
      }
      do {
        mstatus = USBH_MemoryAllocate (ctrl, (uint8_t **)((uint32_t)&ptr_data), max_data_len);
        if (mstatus == usbOK) {
          break;
        }
        max_data_len -= max_packet_size;
        if (max_data_len <= max_packet_size) {
          status = usbMemoryError;
          goto exit;
        }
      } while (max_data_len > max_packet_size);
    } else {                                                            // If memory is in pool it is ok
      ptr_data = buf;
    }
  } else {                                                              // If memory relocation is not enabled
    alloc_mem = false;
    ptr_data  = (uint8_t *)buf;
  }

  packet    = ARM_USBH_PACKET_IN;
  if ((ptr_pipe->bEndpointAddress & 0x0FU) == 0U) {                     // If EP0 IN on control endpoint => force DATA1
    packet &= ~ARM_USBH_PACKET_DATA_Msk;
    packet |=  ARM_USBH_PACKET_DATA1;
  }
  ptr_pipe->transferred = 0U;
  transferred           = 0U;
  received_len          = 0U;

  if (ptr_pipe->thread_id != NULL) {
    status = usbDriverBusy;
    goto mem_free_and_exit;
  }
  ptr_pipe->thread_id = USBH_ThreadGetHandle();

  for (;;) {
transfer_data:
    if (max_data_len > remain_len) {
      receive_len = remain_len;
    } else {
      receive_len = max_data_len;
    }

    // Receive Data
    for (;;) {
      if (((packet & ARM_USBH_PACKET_DATA1) != 0U) && (transferred != 0U)) {
        packet &= ~ARM_USBH_PACKET_DATA1;
      }
      status = USBH_DriverPipeTransfer (ctrl, hw_handle, packet, ptr_data + received_len, receive_len - received_len);
      if (status != usbOK) {                                            // If transfer failed clear EP to TID and exit
        goto done;
      }
      ptr_pipe->transfer_active = 1U;
      switch (ptr_pipe->bmAttributes & 3U) {                            // Pipe endpoint type
        case ARM_USB_ENDPOINT_CONTROL:
          if (remain_len != 0U) {
            event = USBH_ThreadFlagsWait (500U);                        // Time-out 500 ms for data packet
          } else {
            event = USBH_ThreadFlagsWait (100U);                        // Time-out 100 ms for ZLP
          }
          break;
        case ARM_USB_ENDPOINT_BULK:
        case ARM_USB_ENDPOINT_ISOCHRONOUS:
        case ARM_USB_ENDPOINT_INTERRUPT:
          event = USBH_ThreadFlagsWait (0xFFFFFFFFU);                           // Wait forever on bulk/interrupt/isochronous reception
          break;
        default:
          event = 0x80000000U;
          break;
      }
      ptr_pipe->transfer_active = 0U;
      transferred = USBH_DriverPipeTransferGetResult (ctrl, hw_handle);
      if ((alloc_mem) && (buf != NULL) && (ptr_data != NULL) && (transferred != 0U)) {
        memcpy ((void *)buf, (void *)(ptr_data + received_len), transferred);
      }
      ptr_pipe->transferred += transferred;
      received_len          += transferred;
      remain_len            -= transferred;
      if (buf != NULL) {
        buf                 += transferred;
      }
      switch (event) {
        default:                                                        // If transfer event happened
          if (event != 0U) {                                            // Handle pipe event
            if ((event & (~ARM_USBH_PIPE_EVENT_MASK)) != 0U) {          // If port event happened also resend it to thread and stop current transfer
              (void)USBH_ThreadFlagsSet (ptr_pipe->thread_id, event & (~ARM_USBH_PIPE_EVENT_MASK));
              if (ptr_hci->port_lock != 0U) {
                if ((ptr_hci->port_event[ptr_hci->port_lock - 1U]) != 0U) {
                  status = usbTransferError;
                  goto done;
                }
              } else {
                if ((ptr_hci->port_event[ptr_dev->hub_port - 1U]) != 0U) {
                  status = usbTransferError;
                  goto done;
                }
              }
            }
            // Analyze received pipe event
            switch (event & ARM_USBH_PIPE_EVENT_MASK) {
              case ARM_USBH_EVENT_TRANSFER_COMPLETE:
                status = usbOK;
                if ((remain_len == 0U) || ((transferred % max_packet_size) != 0U) || (transferred == 0U)) {     // If short or ZLP packet received
                  goto done;
                }
                if (received_len == max_data_len) {
                  received_len = 0U;
                  goto transfer_data;
                }
                break;
              case ARM_USBH_EVENT_BUS_ERROR:
                status = usbTransferError;
                goto done;
              case ARM_USBH_EVENT_HANDSHAKE_NAK:
                switch (ptr_pipe->bmAttributes & 3U) {                  // Pipe endpoint type
                  case ARM_USB_ENDPOINT_CONTROL:
                    break;
                  case ARM_USB_ENDPOINT_BULK:
                    break;
                  case ARM_USB_ENDPOINT_ISOCHRONOUS:
                    break;
                  case ARM_USB_ENDPOINT_INTERRUPT:
                    if (ptr_dev->dev_speed == USB_SPEED_HIGH) {         // For high-speed pipe endpoint
                      if (ptr_pipe->bInterval >= 4U) {                  // Only when time is at least 1 ms
                        (void)USBH_Delay (1UL << (ptr_pipe->bInterval - 4U));   // Delay in ms
                      }
                    } else {
                      if (ptr_pipe->bInterval != 0U) {                  // Only when time is at least 1 ms
                        (void)USBH_Delay (ptr_pipe->bInterval);         // Delay in ms
                      }
                    }
                    break;
                  default:                                              // Unknown endpoint type
                    goto abort_and_done;
                }
                continue;
              case ARM_USBH_EVENT_HANDSHAKE_STALL:
                status = usbTransferStall;
                goto done;
              case ARM_USBH_EVENT_ABORT:
                status = usbTransferError;
                goto abort_and_done;
              default:
                status = usbUnknownError;
                goto done;
            }
          }
          break;
        case 0U:                                                        // If transfer time-out happened
          status = usbTimeout;
          goto abort_and_done;
        case 0x80000000U:                                               // If transfer time-out happened
          status = usbUnknownError;
          goto done;
      }
    }
  }

abort_and_done:
  (void)USBH_DriverPipeTransferAbort (ctrl, hw_handle);

done:
  ptr_pipe->thread_id = NULL;
  ptr_pipe->locked    = 0U;

mem_free_and_exit:
  if (alloc_mem) {
    mstatus = USBH_MemoryFree (ctrl, (uint8_t  *) ptr_data);
    if (mstatus != usbOK) {
      status = mstatus;
    }
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_PipeReceiveFailed(pipe_hndl, len, status);
  }
#endif
  return status;
}

/// \brief Get result of receive data operation on Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \return                             number of successfully received data bytes.
uint32_t USBH_PipeReceiveGetResult (USBH_PIPE_HANDLE pipe_hndl) {
  const USBH_PIPE *ptr_pipe;
        uint32_t   num;

  num = 0U;

  ptr_pipe = (USBH_PIPE *)pipe_hndl;
  if (CheckPipe (ptr_pipe) == usbOK) {
    if ((ptr_pipe->bEndpointAddress & 0x80U) != 0U) {   // If pipe endpoint IN
      num = ptr_pipe->transferred;
    }
  }

  EvrUSBH_Core_PipeReceiveGetResult(pipe_hndl, num);

  return num;
}

/// \brief Send data on Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \param[in]     buf                  buffer containing data bytes to send.
/// \param[in]     len                  number of bytes to send.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_PipeSend (USBH_PIPE_HANDLE pipe_hndl, const uint8_t *buf, uint32_t len) {
  const USBH_DEV           *ptr_dev;
  const USBH_HCI           *ptr_hci;
        USBH_PIPE          *ptr_pipe;
        uint32_t            event;
        ARM_USBH_EP_HANDLE  hw_handle;
        uint8_t            *ptr_data;
        uint32_t            max_data_len;
        uint32_t            remain_len;
        uint32_t            send_len;
        uint32_t            sent_len;
        uint32_t            packet;
        uint32_t            transferred;
        uint8_t             hs;
        uint16_t            max_packet_size;
        uint8_t             ctrl;
        bool                alloc_mem;
        usbStatus           status;
        usbStatus           mstatus;

  EvrUSBH_Core_PipeSend(pipe_hndl, len);

  if ((buf == NULL) && (len != 0U)) {
    status = usbInvalidParameter;
    goto exit;
  }

  ptr_pipe = (USBH_PIPE *)pipe_hndl;
  status   = CheckPipe (ptr_pipe);
  if (status != usbOK) {
    goto exit;
  }
  if (ptr_pipe->locked != 0U) {
    status = usbDriverBusy;
    goto exit;
  }

  ptr_dev         = &usbh_dev[ptr_pipe->device];
  ctrl            =  ptr_dev->ctrl;
  ptr_hci         = &usbh_hci[ctrl];
  hw_handle       =  ptr_pipe->hw_handle;
  max_packet_size =  ptr_pipe->wMaxPacketSize;

  ptr_pipe->locked = 1U;

  if (((ptr_pipe->bmAttributes & 3U)==ARM_USB_ENDPOINT_ISOCHRONOUS) &&  // If Isochronous pipe
       (ptr_pipe->cb_pipe_event != NULL)) {                             // and registered callback
    // Isochronous pipe handling
    status = USBH_DriverPipeTransfer (ctrl, hw_handle, ARM_USBH_PACKET_OUT, (uint8_t *)((uint32_t)buf), len);
    ptr_pipe->locked = 0U;
    return status;
  }

  remain_len   = len;
  max_data_len = len;
  if (*usbh_mem_pool_reloc_ptr[ctrl] != 0U) {                           // If memory relocation is enabled
    if (len == 0U) {                                                    // If ZLP is requested
      alloc_mem = false;
    } else {
      alloc_mem = !USBH_MemoryIsInPool (ctrl, buf);
    }
    if (alloc_mem) {
      // If memory for USB must be relocated, allocate maximum memory chunk for transmission
      if (max_data_len > ptr_hci->port_mem_max) {
        max_data_len = ptr_hci->port_mem_max & (~((uint32_t)max_packet_size-1U));
      }
      do {
        mstatus = USBH_MemoryAllocate (ctrl, (uint8_t **)((uint32_t)&ptr_data), max_data_len);
        if (mstatus == usbOK) {
          break;
        }
        max_data_len -= max_packet_size;
        if (max_data_len <= max_packet_size) {
          status = usbMemoryError;
          goto exit;
        }
      } while (max_data_len > max_packet_size);
    } else {                                                            // If memory is in pool it is ok
      ptr_data = (uint8_t *)((uint32_t)buf);
    }
  } else {                                                              // If memory relocation is not enabled
    alloc_mem = false;
    ptr_data  = (uint8_t *)((uint32_t)buf);
  }

  packet    = ARM_USBH_PACKET_OUT;
  if ((ptr_pipe->bEndpointAddress & 0x0FU) == 0U) {                     // If OUT on default pipe => force DATA1
    packet &= ~ARM_USBH_PACKET_DATA_Msk;
    packet |=  ARM_USBH_PACKET_DATA1;
  }
  ptr_pipe->transferred = 0U;
  transferred           = 0U;
  sent_len              = 0U;
  if (ptr_dev->dev_speed == USB_SPEED_HIGH) {
    hs = 1U;
  } else {
    hs = 0U;
  }

  if (ptr_pipe->thread_id != NULL) {
    status = usbDriverBusy;
    goto mem_free_and_exit;
  }
  ptr_pipe->thread_id = USBH_ThreadGetHandle();

  for (;;) {
transfer_data:
    if (max_data_len > remain_len) {
      send_len = remain_len;
    } else {
      send_len = max_data_len;
    }
    if ((alloc_mem) && (ptr_data != NULL) && (buf != NULL)) {
      memcpy ((void *)(ptr_data + sent_len), (const void *)buf, send_len);
    }

    // Send Data
    for (;;) {
      if (((packet & ARM_USBH_PACKET_DATA1) != 0U) && (transferred != 0U)) {
        packet &= ~ARM_USBH_PACKET_DATA1;
      }
      status = USBH_DriverPipeTransfer (ctrl, hw_handle, packet, ptr_data + sent_len, send_len - sent_len);
      if (status != usbOK) {                                            // If transfer failed clear EP to TID and exit
        goto done;
      }
      ptr_pipe->transfer_active = 1U;
      switch (ptr_pipe->bmAttributes & 3U) {                            // Pipe endpoint type
        case ARM_USB_ENDPOINT_CONTROL:
          if (remain_len != 0U) {
            event = USBH_ThreadFlagsWait (500U);                        // Timeout 500 ms for data packet
          } else {
            event = USBH_ThreadFlagsWait (100U);                        // Timeout 100 ms for ZLP
          }
          break;
        case ARM_USB_ENDPOINT_BULK:
        case ARM_USB_ENDPOINT_ISOCHRONOUS:
        case ARM_USB_ENDPOINT_INTERRUPT:
          event = USBH_ThreadFlagsWait (0xFFFFFFFFU);                   // Wait forever on bulk/interrupt/isochronous reception
          break;
        default:
          event = 0x80000000U;
          break;
      }
      ptr_pipe->transfer_active = 0U;
      transferred = USBH_DriverPipeTransferGetResult (ctrl, hw_handle);
      ptr_pipe->transferred += transferred;
      sent_len              += transferred;
      remain_len            -= transferred;
      if (buf != NULL) {
        buf                 += transferred;
      }
      switch (event) {
        default:                                                        // If transfer event happened
          if (event != 0U) {                                            // Handle pipe event
            if ((event & (~ARM_USBH_PIPE_EVENT_MASK)) != 0U) {          // If port event happened also resend it to thread and stop current transfer
              (void)USBH_ThreadFlagsSet (ptr_pipe->thread_id, event & (~ARM_USBH_PIPE_EVENT_MASK));
              if (ptr_hci->port_lock != 0U) {
                if ((ptr_hci->port_event[ptr_hci->port_lock - 1U]) != 0U) {
                  status = usbTransferError;
                  goto done;
                }
              } else {
                if ((ptr_hci->port_event[ptr_dev->hub_port - 1U]) != 0U) {
                  status = usbTransferError;
                  goto done;
                }
              }
            }
            // Analyze received endpoint event
            switch (event & ARM_USBH_PIPE_EVENT_MASK) {
              case ARM_USBH_EVENT_TRANSFER_COMPLETE:
                status = usbOK;
                if (remain_len == 0U) {
                  goto done;
                }
                if (sent_len == max_data_len) {
                  sent_len = 0U;
                  goto transfer_data;
                }
                break;
              case ARM_USBH_EVENT_BUS_ERROR:
                status = usbTransferError;
                goto done;
              case ARM_USBH_EVENT_HANDSHAKE_NAK:
                switch (ptr_pipe->bmAttributes & 3U) {                  // Pipe endpoint type
                  case ARM_USB_ENDPOINT_CONTROL:
                  case ARM_USB_ENDPOINT_BULK:
                    if (hs == 1U) {                                     // For high-speed pipe endpoint and was not an error
                      status = USBH_PipeDoPing (pipe_hndl);
                      if (status != usbOK) {
                        goto done;
                      }
                                                                        // Meant to be used with NAK rate
//                    if (ptr_ep->bInterval >= 8) {                     // Only when time is at least 1 ms
//                      (void)USBH_Delay (ptr_ep->bInterval >> 3);      // Delay in ms
//                    }
                    }
                    break;
                  case ARM_USB_ENDPOINT_ISOCHRONOUS:
                    break;
                  case ARM_USB_ENDPOINT_INTERRUPT:
                    if (hs == 1U) {                                     // For high-speed pipe endpoint
                      if (ptr_pipe->bInterval >= 4U) {                  // Only when time is at least 1 ms
                        (void)USBH_Delay (1UL << (ptr_pipe->bInterval - 4U));   // Delay in ms
                      }
                    } else {
                      if (ptr_pipe->bInterval != 0U) {                  // Only when time is at least 1 ms
                        (void)USBH_Delay (ptr_pipe->bInterval);         // Delay in ms
                      }
                    }
                    break;
                  default:                                              // Unknown endpoint type
                    goto abort_and_done;
                }
                continue;
              case ARM_USBH_EVENT_HANDSHAKE_NYET:
                if (transferred == remain_len) {
                  status = usbOK;
                  goto done;
                }
                status = USBH_PipeDoPing (pipe_hndl);
                if (status == usbOK) {
                  goto done;
                }
                continue;
              case ARM_USBH_EVENT_HANDSHAKE_STALL:
                status = usbTransferStall;
                goto done;
              case ARM_USBH_EVENT_ABORT:
                status = usbTransferError;
                goto abort_and_done;
              default:
                status = usbUnknownError;
                goto done;
            }
          }
          break;
        case 0U:                                                        // If transfer time-out happened
          status = usbTimeout;
          goto abort_and_done;
        case 0x80000000U:                                               // If transfer time-out happened
          status = usbUnknownError;
          goto done;
      }
    }
  }

abort_and_done:
  (void)USBH_DriverPipeTransferAbort (ctrl, hw_handle);

done:
  ptr_pipe->thread_id = NULL;
  ptr_pipe->locked    = 0U;

mem_free_and_exit:
  if (alloc_mem) {
    mstatus = USBH_MemoryFree (ctrl, (uint8_t  *) ptr_data);
    if (mstatus != usbOK) {
      status = mstatus;
    }
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_PipeSendFailed(pipe_hndl, len, status);
  }
#endif
  return status;
}

/// \brief Get result of send data operation on Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \return                             number of successfully sent data bytes.
uint32_t USBH_PipeSendGetResult (USBH_PIPE_HANDLE pipe_hndl) {
  const USBH_PIPE *ptr_pipe;
        uint32_t   num;

  num = 0U;

  ptr_pipe = (USBH_PIPE *)pipe_hndl;
  if (CheckPipe (ptr_pipe) == usbOK) {
    if ((ptr_pipe->bEndpointAddress & 0x80U) == 0U) {   // If pipe endpoint OUT
      num = ptr_pipe->transferred;
    }
  }

  EvrUSBH_Core_PipeSendGetResult(pipe_hndl, num);

  return num;
}

/// \brief Abort send/receive operation on Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_PipeAbort (USBH_PIPE_HANDLE pipe_hndl) {
  const USBH_DEV  *ptr_dev;
        USBH_PIPE *ptr_pipe;
        usbStatus  status;
        uint8_t    retry;

  EvrUSBH_Core_PipeAbort(pipe_hndl);

  ptr_pipe = (USBH_PIPE *)pipe_hndl;
  status   = CheckPipe (ptr_pipe);
  ptr_dev  = &usbh_dev[ptr_pipe->device];

  if (((ptr_pipe->bmAttributes & 3U)==ARM_USB_ENDPOINT_ISOCHRONOUS) &&  // If Isochronous pipe
       (ptr_pipe->cb_pipe_event != NULL)) {                             // and registered callback
    // Isochronous pipe handling
    status = USBH_DriverPipeTransferAbort (ptr_dev->ctrl, ptr_pipe->hw_handle);
    ptr_pipe->locked = 0U;
    return status;
  }

  if (status == usbOK) {
    if (ptr_pipe->locked == 1U) {
      retry = 3U;
      while (retry-- != 0U) {
        if (ptr_pipe->transfer_active == 1U) { break; }
        (void)USBH_Delay (10U);
      }
      if (ptr_pipe->transfer_active == 1U) {
        status = USBH_DriverPipeTransferAbort (ptr_dev->ctrl, ptr_pipe->hw_handle);
        if (status == usbOK) {
          USBH_SignalPipeEvent (ptr_dev->ctrl, ptr_pipe->hw_handle, ARM_USBH_EVENT_ABORT);
        }
      }
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_PipeAbortFailed(pipe_hndl, status);
  }
#endif
  return status;
}

/// \brief Do a Control Transfer on Default Pipe
/// \param[in]     device               index of USB Device.
/// \param[in]     setup_packet         pointer to setup packet.
/// \param[in,out] data                 buffer containing data bytes to send or where data should be received in data stage of Control Transfer.
/// \param[in]     len                  number of bytes to send or receive in data stage of Control Transfer.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_ControlTransfer (uint8_t device, const USB_SETUP_PACKET *setup_packet, uint8_t *data, uint32_t len) {
  const USBH_DEV         *ptr_dev;
        USBH_PIPE_HANDLE  pipe_hndl;
        uint8_t           ctrl, retry;
        usbStatus         status;

  EvrUSBH_Core_ControlTransfer(device, (const void *)setup_packet, len);

  if ((data != NULL) || (len == 0U)) {
    ptr_dev = &usbh_dev[device];
    ctrl    = ptr_dev->ctrl;
    if (ctrl < usbh_hc_num) {
      retry = 3U;
      while (retry-- != 0U) {
        if (USBH_MutexAcquire (usbh_def_pipe_mutex_id[ctrl], 500U) == 0) { break; }
        (void)USBH_Delay (100U);
      }
      if (retry == 0U) {
        status    = usbMutexError;
      } else {
        status    = USBH_DefaultPipeModify (ctrl, device);
        pipe_hndl = usbh_hci[ctrl].def_pipe_hndl;

        if (status == usbOK) {
          retry = 3U;
          while (retry-- != 0U) {
            status = USBH_PipeSendSetup   (pipe_hndl, setup_packet);            // Send SETUP packet
            if (status != usbOK) {
              (void)USBH_Delay (50U);
              continue;
            }
            if ((data != NULL) && (len != 0U)) {
              if (setup_packet->bmRequestType.Dir == USB_REQUEST_DEVICE_TO_HOST) {
                status = USBH_PipeReceive (pipe_hndl, data, len);               // Receive Data IN
                switch (status) {
                  case usbOK:
                    status = USBH_PipeSend(pipe_hndl, NULL, 0U);                // Send Status OUT (ZLP)
                    break;
                  case usbTransferStall:
                                                                                // Clear Endpoint Halt feature
                    USBH_DeviceRequest_ClearFeature (device, USB_REQUEST_TO_ENDPOINT, 0U, 0U);
                    break;
                  case usbTimeout:
                  case usbInvalidParameter:
                  case usbThreadError:
                  case usbTimerError:
                  case usbSemaphoreError:
                  case usbMutexError:
                  case usbControllerError:
                  case usbDeviceError:
                  case usbDriverError:
                  case usbDriverBusy:
                  case usbMemoryError:
                  case usbNotConfigured:
                  case usbClassErrorADC:
                  case usbClassErrorCDC:
                  case usbClassErrorHID:
                  case usbClassErrorMSC:
                  case usbClassErrorCustom:
                  case usbUnsupportedClass:
                  case usbTransferError:
                  case usbUnknownError:
                    break;
                }
              } else {
                status = USBH_PipeSend    (pipe_hndl, data, len);               // Send Data OUT
                switch (status) {
                  case usbOK:
                    status = USBH_PipeReceive (pipe_hndl, NULL, 0U);            // Receive Status IN (ZLP)
                    break;
                  case usbTransferStall:
                                                                                // Clear Endpoint Halt feature
                    USBH_DeviceRequest_ClearFeature (device, USB_REQUEST_TO_ENDPOINT, 0U, 0U);
                    break;
                  case usbTimeout:
                  case usbInvalidParameter:
                  case usbThreadError:
                  case usbTimerError:
                  case usbSemaphoreError:
                  case usbMutexError:
                  case usbControllerError:
                  case usbDeviceError:
                  case usbDriverError:
                  case usbDriverBusy:
                  case usbMemoryError:
                  case usbNotConfigured:
                  case usbClassErrorADC:
                  case usbClassErrorCDC:
                  case usbClassErrorHID:
                  case usbClassErrorMSC:
                  case usbClassErrorCustom:
                  case usbUnsupportedClass:
                  case usbTransferError:
                  case usbUnknownError:
                    break;
                }
              }
            } else {
              status = USBH_PipeReceive   (pipe_hndl, NULL, 0U);                // Receive Status IN (ZLP)
            }

            if (status != usbOK) {
              (void)USBH_Delay (50U);
            } else {
              break;
            }
          }
        }

        retry = 3U;
        while (retry-- != 0U) {
          if (USBH_MutexRelease (usbh_def_pipe_mutex_id[ctrl]) == 0) { break; }
          (void)USBH_Delay (100U);
        }
        if (retry == 0U) {
          status = usbMutexError;
        }
      }
    } else {
      status = usbControllerError;
    }
  } else {
    status = usbInvalidParameter;
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_ControlTransferFailed(device, (const void *)setup_packet, status);
  }
#endif
  return status;
}

/// \brief Standard Device Request on Default Pipe - GET_STATUS
/// \param[in]     device               index of USB Device.
/// \param[in]     recipient            recipient.
/// \param[in]     index                interface or endpoint index.
/// \param[out]    ptr_stat_dat         pointer to where status data should be received.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_DeviceRequest_GetStatus (uint8_t device, uint8_t recipient, uint8_t index, uint8_t *ptr_stat_dat) {
  USB_SETUP_PACKET *ptr_sp;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  ctrl   = usbh_dev[device].ctrl;
  status = CheckController (ctrl);
  if (status == usbOK) {
    if (ptr_stat_dat != NULL) {
      status = USBH_MemoryAllocate      (ctrl, (uint8_t **)((uint32_t)&ptr_sp), 8U);
      if (status == usbOK) {
        PREPARE_SETUP_PACKET_DATA       (                   ptr_sp, USB_REQUEST_DEVICE_TO_HOST, USB_REQUEST_STANDARD, recipient, USB_REQUEST_GET_STATUS, 0U, index, 2U)
        status  = USBH_ControlTransfer  (device,            ptr_sp, ptr_stat_dat, 2U);
        mstatus = USBH_MemoryFree       (ctrl, (uint8_t  *) ptr_sp);
        if ((mstatus != usbOK) && (status == usbOK)) {
          status = mstatus;
        }
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status == usbOK) {
    EvrUSBH_Core_RequestGetStatus      (device, recipient, index, ((uint16_t)(*(ptr_stat_dat+1U))) | ((uint16_t)*ptr_stat_dat));
  } else {
    EvrUSBH_Core_RequestGetStatusFailed(device, recipient, index, status);
  }
#endif
  return status;
}

/// \brief Standard Device Request on Default Pipe - CLEAR_FEATURE
/// \param[in]     device               index of USB Device.
/// \param[in]     recipient            recipient.
/// \param[in]     index                interface or endpoint index.
/// \param[in]     feature_selector     feature selector.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_DeviceRequest_ClearFeature (uint8_t device, uint8_t recipient, uint8_t index, uint8_t feature_selector) {
  USB_SETUP_PACKET *ptr_sp;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_Core_RequestClearFeature(device, recipient, index, feature_selector);

  ctrl   = usbh_dev[device].ctrl;
  status = CheckController (ctrl);
  if (status == usbOK) {
    status = USBH_MemoryAllocate        (ctrl, (uint8_t **)((uint32_t)&ptr_sp), 8U);
    if (status == usbOK) {
      PREPARE_SETUP_PACKET_DATA         (                   ptr_sp, USB_REQUEST_HOST_TO_DEVICE, USB_REQUEST_STANDARD, recipient, USB_REQUEST_CLEAR_FEATURE, feature_selector, index, 0U)
      status = USBH_ControlTransfer     (device,            ptr_sp, NULL, 0U);
      mstatus = USBH_MemoryFree         (ctrl, (uint8_t  *) ptr_sp);
      if ((mstatus != usbOK) && (status == usbOK)) {
        status = mstatus;
      }
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_RequestClearFeatureFailed(device, recipient, index, feature_selector, status);
  }
#endif
  return status;
}

/// \brief Standard Device Request on Default Pipe - SET_FEATURE
/// \param[in]     device               index of USB Device.
/// \param[in]     recipient            recipient.
/// \param[in]     index                interface or endpoint index.
/// \param[in]     feature_selector     feature selector.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_DeviceRequest_SetFeature (uint8_t device, uint8_t recipient, uint8_t index, uint8_t feature_selector) {
  USB_SETUP_PACKET *ptr_sp;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_Core_RequestSetFeature(device, recipient, index, feature_selector);

  ctrl   = usbh_dev[device].ctrl;
  status = CheckController (ctrl);
  if (status == usbOK) {
    status = USBH_MemoryAllocate        (ctrl, (uint8_t **)((uint32_t)&ptr_sp), 8U);
    if (status == usbOK) {
      PREPARE_SETUP_PACKET_DATA         (                   ptr_sp, USB_REQUEST_HOST_TO_DEVICE, USB_REQUEST_STANDARD, recipient, USB_REQUEST_SET_FEATURE, feature_selector, index, 0U)
      status = USBH_ControlTransfer     (device,            ptr_sp, NULL, 0U);
      mstatus = USBH_MemoryFree         (ctrl, (uint8_t  *) ptr_sp);
      if ((mstatus != usbOK) && (status == usbOK)) {
        status = mstatus;
      }
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_RequestSetFeatureFailed(device, recipient, index, feature_selector, status);
  }
#endif
  return status;
}

/// \brief Standard Device Request on Default Pipe - SET_ADDRESS
/// \param[in]     device               index of USB Device.
/// \param[in]     device_address       device address.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_DeviceRequest_SetAddress (uint8_t device, uint8_t device_address) {
  USB_SETUP_PACKET *ptr_sp;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_Core_RequestSetAddress(device, device_address);

  ctrl   = usbh_dev[device].ctrl;
  status = CheckController (ctrl);
  if (status == usbOK) {
    status = USBH_MemoryAllocate        (ctrl, (uint8_t **)((uint32_t)&ptr_sp), 8U);
    if (status == usbOK) {
      PREPARE_SETUP_PACKET_DATA         (                   ptr_sp, USB_REQUEST_HOST_TO_DEVICE, USB_REQUEST_STANDARD, USB_REQUEST_TO_DEVICE, USB_REQUEST_SET_ADDRESS, device_address, 0U, 0U)
      status = USBH_ControlTransfer     (device,            ptr_sp, NULL, 0U);
      if (status == usbOK) {
        usbh_dev[device].dev_addr = device_address;
      }
      mstatus = USBH_MemoryFree         (ctrl, (uint8_t  *) ptr_sp);
      if ((mstatus != usbOK) && (status == usbOK)) {
        status = mstatus;
      }
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_RequestSetAddressFailed(device, device_address, status);
  }
#endif
  return status;
}

/// \brief Standard Device Request on Default Pipe - GET_DESCRIPTOR
/// \param[in]     device               index of USB Device.
/// \param[in]     recipient            recipient.
/// \param[in]     descriptor_type      descriptor type.
/// \param[in]     descriptor_index     descriptor index.
/// \param[in]     language_id          language ID.
/// \param[out]    descriptor_data      pointer to where descriptor data will be read.
/// \param[in]     descriptor_length    maximum descriptor length.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_DeviceRequest_GetDescriptor (uint8_t device, uint8_t recipient, uint8_t descriptor_type, uint8_t descriptor_index, uint16_t language_id, uint8_t *descriptor_data, uint16_t descriptor_length) {
  USB_SETUP_PACKET *ptr_sp;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_Core_RequestGetDescriptor(device, recipient, descriptor_type, descriptor_index, language_id, descriptor_length);

  ctrl   = usbh_dev[device].ctrl;
  status = CheckController (ctrl);
  if (status == usbOK) {
    if (descriptor_data != NULL) {
      status = USBH_MemoryAllocate      (ctrl, (uint8_t **)((uint32_t)&ptr_sp), 8U);
      if (status == usbOK) {
        PREPARE_SETUP_PACKET_DATA       (                   ptr_sp, USB_REQUEST_DEVICE_TO_HOST, USB_REQUEST_STANDARD, recipient, USB_REQUEST_GET_DESCRIPTOR, (((uint32_t)descriptor_type << 8) | (uint32_t)descriptor_index), language_id, descriptor_length)
        status = USBH_ControlTransfer   (device,            ptr_sp, descriptor_data, descriptor_length);
        mstatus = USBH_MemoryFree       (ctrl, (uint8_t  *) ptr_sp);
        if ((mstatus != usbOK) && (status == usbOK)) {
          status = mstatus;
        }
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_RequestGetDescriptorFailed(device, recipient, descriptor_type, descriptor_index, language_id, descriptor_length, status);
  }
#endif
  return status;
}

/// \brief Standard Device Request on Default Pipe - SET_DESCRIPTOR
/// \param[in]     device               index of USB Device.
/// \param[in]     recipient            recipient.
/// \param[in]     descriptor_type      descriptor type.
/// \param[in]     descriptor_index     descriptor index.
/// \param[in]     language_id          language ID.
/// \param[in]     descriptor_data      pointer to descriptor data to be written.
/// \param[in]     descriptor_length    descriptor length.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_DeviceRequest_SetDescriptor (uint8_t device, uint8_t recipient, uint8_t descriptor_type, uint8_t descriptor_index, uint16_t language_id, const uint8_t *descriptor_data, uint16_t descriptor_length) {
  USB_SETUP_PACKET *ptr_sp;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_Core_RequestSetDescriptor(device, recipient, descriptor_type, descriptor_index, language_id, descriptor_length);

  ctrl   = usbh_dev[device].ctrl;
  status = CheckController (ctrl);
  if (status == usbOK) {
    if (descriptor_data != NULL) {
      status = USBH_MemoryAllocate      (ctrl, (uint8_t **)((uint32_t)&ptr_sp), 8U);
      if (status == usbOK) {
        PREPARE_SETUP_PACKET_DATA       (                   ptr_sp, USB_REQUEST_HOST_TO_DEVICE, USB_REQUEST_STANDARD, recipient, USB_REQUEST_SET_DESCRIPTOR, (((uint32_t)descriptor_type << 8) | (uint32_t)descriptor_index), language_id, descriptor_length)
        status = USBH_ControlTransfer   (device,            ptr_sp, (uint8_t *)((uint32_t)descriptor_data), descriptor_length);
        mstatus = USBH_MemoryFree       (ctrl, (uint8_t  *) ptr_sp);
        if ((mstatus != usbOK) && (status == usbOK)) {
          status = mstatus;
        }
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_RequestSetDescriptorFailed(device, recipient, descriptor_type, descriptor_index, language_id, descriptor_length, status);
  }
#endif
  return status;
}

/// \brief Standard Device Request on Default Pipe - GET_CONFIGURATION
/// \param[in]     device               index of USB Device.
/// \param[out]    ptr_configuration    pointer to where configuration will be read.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_DeviceRequest_GetConfiguration (uint8_t device, uint8_t *ptr_configuration) {
  USB_SETUP_PACKET *ptr_sp;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_Core_RequestGetConfiguration(device, *ptr_configuration);

  ctrl   = usbh_dev[device].ctrl;
  status = CheckController (ctrl);
  if (status == usbOK) {
    if (ptr_configuration != NULL) {
      status = USBH_MemoryAllocate      (ctrl, (uint8_t **)((uint32_t)&ptr_sp), 8U);
      if (status == usbOK) {
        PREPARE_SETUP_PACKET_DATA       (                   ptr_sp, USB_REQUEST_DEVICE_TO_HOST, USB_REQUEST_STANDARD, USB_REQUEST_TO_DEVICE, USB_REQUEST_GET_CONFIGURATION, 0U, 0U, 1U)
        status = USBH_ControlTransfer   (device,            ptr_sp, ptr_configuration, 1U);
        mstatus = USBH_MemoryFree       (ctrl, (uint8_t  *) ptr_sp);
        if ((mstatus != usbOK) && (status == usbOK)) {
          status = mstatus;
        }
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_RequestGetConfigurationFailed(device, status);
  }
#endif
  return status;
}

/// \brief Standard Device Request on Default Pipe - SET_CONFIGURATION
/// \param[in]     device               index of USB Device.
/// \param[in]     configuration        configuration.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_DeviceRequest_SetConfiguration (uint8_t device, uint8_t configuration) {
  USB_SETUP_PACKET *ptr_sp;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_Core_RequestSetConfiguration(device, configuration);

  ctrl   = usbh_dev[device].ctrl;
  status = CheckController (ctrl);
  if (status == usbOK) {
    status = USBH_MemoryAllocate        (ctrl, (uint8_t **)((uint32_t)&ptr_sp), 8U);
    if (status == usbOK) {
      PREPARE_SETUP_PACKET_DATA         (                   ptr_sp, USB_REQUEST_HOST_TO_DEVICE, USB_REQUEST_STANDARD, USB_REQUEST_TO_DEVICE, USB_REQUEST_SET_CONFIGURATION, configuration, 0U, 0U)
      status = USBH_ControlTransfer     (device,            ptr_sp, NULL, 0U);
      if (status == usbOK) {
        usbh_dev[device].state.configured = configuration;
      }
      mstatus = USBH_MemoryFree         (ctrl, (uint8_t  *) ptr_sp);
      if ((mstatus != usbOK) && (status == usbOK)) {
        status = mstatus;
      }
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_RequestSetConfigurationFailed(device, configuration, status);
  }
#endif
  return status;
}

/// \brief Standard Device Request on Default Pipe - GET_INTERFACE
/// \param[in]     device               index of USB Device.
/// \param[in]     index                interface index.
/// \param[out]    ptr_alternate        pointer to where alternate setting data will be read.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_DeviceRequest_GetInterface (uint8_t device, uint8_t index, uint8_t *ptr_alternate) {
  USB_SETUP_PACKET *ptr_sp;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_Core_RequestGetInterface(device, index, *ptr_alternate);

  ctrl   = usbh_dev[device].ctrl;
  status = CheckController (ctrl);
  if (status == usbOK) {
    if (ptr_alternate != NULL) {
      status = USBH_MemoryAllocate      (ctrl, (uint8_t **)((uint32_t)&ptr_sp), 8U);
      if (status == usbOK) {
        PREPARE_SETUP_PACKET_DATA       (                   ptr_sp, USB_REQUEST_DEVICE_TO_HOST, USB_REQUEST_STANDARD, USB_REQUEST_TO_DEVICE, USB_REQUEST_GET_INTERFACE, 0U, index, 1U)
        status = USBH_ControlTransfer   (device,            ptr_sp, ptr_alternate, 1U);
        mstatus = USBH_MemoryFree       (ctrl, (uint8_t  *) ptr_sp);
        if ((mstatus != usbOK) && (status == usbOK)) {
          status = mstatus;
        }
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_RequestGetInterfaceFailed(device, index, status);
  }
#endif
  return status;
}

/// \brief Standard Device Request on Default Pipe - SET_INTERFACE
/// \param[in]     device               index of USB Device.
/// \param[in]     index                interface index.
/// \param[in]     alternate            alternate setting.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_DeviceRequest_SetInterface (uint8_t device, uint8_t index, uint8_t alternate) {
  USB_SETUP_PACKET *ptr_sp;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  EvrUSBH_Core_RequestSetInterface(device, index, alternate);

  ctrl   = usbh_dev[device].ctrl;
  status = CheckController (ctrl);
  if (status == usbOK) {
    status = USBH_MemoryAllocate        (ctrl, (uint8_t **)((uint32_t)&ptr_sp), 8U);
    if (status == usbOK) {
      PREPARE_SETUP_PACKET_DATA         (                   ptr_sp, USB_REQUEST_HOST_TO_DEVICE, USB_REQUEST_STANDARD, USB_REQUEST_TO_INTERFACE, USB_REQUEST_SET_INTERFACE, alternate, index, 0U)
      status = USBH_ControlTransfer     (device,            ptr_sp, NULL, 0U);
      mstatus = USBH_MemoryFree         (ctrl, (uint8_t  *) ptr_sp);
      if ((mstatus != usbOK) && (status == usbOK)) {
        status = mstatus;
      }
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_RequestSetInterfaceFailed(device, index, alternate, status);
  }
#endif
  return status;
}

/// \brief Standard Device Request on Default Pipe - SYNCH_FRAME
/// \param[in]     device               index of USB Device.
/// \param[in]     index                interface or endpoint index.
/// \param[out]    ptr_frame_number     pointer to where frame number data will be read.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_DeviceRequest_SynchFrame (uint8_t device, uint8_t index, uint8_t *ptr_frame_number) {
  USB_SETUP_PACKET *ptr_sp;
  uint8_t           ctrl;
  usbStatus         status;
  usbStatus         mstatus;

  ctrl   = usbh_dev[device].ctrl;
  status = CheckController (ctrl);
  if (status == usbOK) {
    if (ptr_frame_number != NULL) {
      status = USBH_MemoryAllocate      (ctrl, (uint8_t **)((uint32_t)&ptr_sp), 8U);
      if (status == usbOK) {
        PREPARE_SETUP_PACKET_DATA       (                   ptr_sp, USB_REQUEST_DEVICE_TO_HOST, USB_REQUEST_STANDARD, USB_REQUEST_TO_DEVICE, USB_REQUEST_SYNC_FRAME, 0U, index, 2U)
        status = USBH_ControlTransfer   (device,            ptr_sp, ptr_frame_number, 2U);
        mstatus = USBH_MemoryFree       (ctrl, (uint8_t  *) ptr_sp);
        if ((mstatus != usbOK) && (status == usbOK)) {
          status = mstatus;
        }
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status == usbOK) {
    EvrUSBH_Core_RequestSynchFrame      (device, index, ((uint16_t)(*(ptr_frame_number+1U))) | ((uint16_t)*ptr_frame_number));
  } else {
    EvrUSBH_Core_RequestSynchFrameFailed(device, index, status);
  }
#endif
  return status;
}


/************************** Library accessible functions **********************/

/// \brief Get a pointer to first free Pipe
/// \param[in]     ctrl                 index of USB Host controller.
/// \return        != 0                 valid pointer to free Pipe Handle to Thread ID array entry
/// \return        == 0                 no free Pipe Handle to Thread ID array entry
USBH_PIPE *USBH_GetFree_PIPE (uint8_t ctrl) {
  USBH_PIPE       *ptr_pipe;
  uint32_t         max_pipe;
  uint32_t         i;

  ptr_pipe =  usbh_pipe_ptr[ctrl];
  max_pipe = *usbh_pipe_num_ptr[ctrl];

  for (i = 0U; i < max_pipe; i++) {
    if (ptr_pipe->hw_handle == 0U) {
      return ptr_pipe;
    }
    ptr_pipe++;
  }

  return NULL;
}

/// \brief Get Thread ID of Pipe
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     hw_handle            hardware handle.
/// \return        != 0                 valid Thread ID
/// \return        == 0                 Thread ID for requested Pipe Hw Handle was not found
void *USBH_Get_Thread_ID_of_Pipe (uint8_t ctrl, ARM_USBH_PIPE_HANDLE hw_handle) {
  USBH_PIPE *ptr_pipe;
  uint32_t   max_pipe;
  uint32_t   i;

  if (hw_handle == 0U) {
    return NULL;
  }

  if (CheckController (ctrl) != usbOK) {
    return NULL;
  }

  ptr_pipe =  usbh_pipe_ptr[ctrl];
  max_pipe = *usbh_pipe_num_ptr[ctrl];

  for (i = 0U; i < max_pipe; i++) {
    if (ptr_pipe->hw_handle == hw_handle) {
      return ptr_pipe->thread_id;
    }
    ptr_pipe++;
  }

  return NULL;
}

/// \brief Get Pipe Callback of Pipe
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     hw_handle            hardware handle.
/// \return        != 0                 valid pointer to Pipe event callback function
/// \return        == 0                 invalid pointer to Pipe event callback function
USBH_PipeEvent_t USBH_Get_PipeCallback_of_Pipe (uint8_t ctrl, ARM_USBH_PIPE_HANDLE hw_handle) {
  USBH_PIPE *ptr_pipe;
  uint32_t   max_pipe;
  uint32_t   i;

  if (hw_handle == 0U) {
    return NULL;
  }

  if (CheckController (ctrl) != usbOK) {
    return NULL;
  }

  ptr_pipe =  usbh_pipe_ptr[ctrl];
  max_pipe = *usbh_pipe_num_ptr[ctrl];

  for (i = 0U; i < max_pipe; i++) {
    if (ptr_pipe->hw_handle == hw_handle) {
      return ptr_pipe->cb_pipe_event;
    }
    ptr_pipe++;
  }

  return NULL;
}

/// \brief Initialize memory pool
/// \param[in]     ctrl                 index of USB Host controller.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_MemoryInitialize (uint8_t ctrl) {
  usbStatus status;

  status = CheckController (ctrl);
  if (status == usbOK) {
    if (USBH_MemoryInitializeLib (ctrl) == false) {
      status = usbMemoryError;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status == usbOK) {
    EvrUSBH_Core_MemInit(ctrl);
  } else {
    EvrUSBH_Core_MemInitFailed(ctrl, status);
  }
#endif
  return status;
}

/// \brief De-initialize memory pool
/// \param[in]     ctrl                 index of USB Host controller.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_MemoryUninitialize (uint8_t ctrl) {
  usbStatus status;

  status = CheckController (ctrl);
  if (status == usbOK) {
    if (USBH_MemoryUninitializeLib (ctrl) == false) {
      status = usbMemoryError;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status == usbOK) {
    EvrUSBH_Core_MemUninit(ctrl);
  } else {
    EvrUSBH_Core_MemUninitFailed(ctrl, status);
  }
#endif
  return status;
}

/// \brief Allocate a chunk of memory from the initialized pool
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[out]    ptr                  pointer to pointer to allocated memory chunk.
/// \param[in]     size                 size of memory chunk to be allocated.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_MemoryAllocate (uint8_t ctrl, uint8_t **ptr, uint32_t size) {
  uint8_t   *mem;
  usbStatus  status;

  mem = NULL;

  status = CheckController (ctrl);
  if (status == usbOK) {
    if (ptr != NULL) {
      mem = USBH_MemoryAllocateLib (ctrl, size);
      if (mem != NULL) {
        *ptr = mem;
      } else {
        status = usbMemoryError;
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status == usbOK) {
    EvrUSBH_Core_MemAlloc(ctrl, (const uint8_t *)mem, size);
  } else {
    EvrUSBH_Core_MemAllocFailed(ctrl, size, status);
  }
#endif
  return status;
}

/// \brief Release a piece of memory back to memory pool
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     ptr                  pointer to allocated memory chunk to be released.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_MemoryFree (uint8_t ctrl, uint8_t *ptr) {
  usbStatus status;

  status = CheckController (ctrl);
  if (status == usbOK) {
    if (ptr != NULL) {
      if (USBH_MemoryFreeLib (ctrl, ptr) == false) {
        status = usbMemoryError;
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status == usbOK) {
    EvrUSBH_Core_MemFree(ctrl, (const uint8_t *)ptr);
  } else {
    EvrUSBH_Core_MemFreeFailed(ctrl, (const uint8_t *)ptr, status);
  }
#endif
  return status;
}

/// \brief Try to recover USB Device (reset and re-enumerate)
/// \param[in]     ptr_dev              pointer to USB Device instance.
/// \param[in]     port                 port index (0 .. 127).
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_RecoverDevice (USBH_DEV *ptr_dev) {
  USBH_HCI *ptr_hci;
  uint32_t  event;
  uint8_t   ctrl, port;

  ctrl    =  ptr_dev->ctrl;
  port    =  ptr_dev->hub_port;
  ptr_hci = &usbh_hci[ctrl];

  ptr_dev->recovery_thread_id = USBH_ThreadGetHandle ();
  while (ptr_hci->port_lock != 0U) {
    (void)USBH_Delay (1U);
  }
  ptr_hci->port_event[port] |= ARM_USBH_EVENT_CORE_DO_RECOVERY;
  (void)USBH_ThreadFlagsSet (usbh_core_thread_id[ctrl], ARM_USBH_EVENT_PORT);

  event = USBH_ThreadFlagsWait (5000U);     // Wait max 5 seconds for recovery, as device 
                                            // might have disconnected so we terminate this 
                                            // thread if timeout expires
  if (((event & 0x8000000U) == 0U) && (event != 0U)) {
    if ((event & ARM_USBH_EVENT_CORE_RECOVERY_OK) != 0U) {
      return usbOK;
    }
    if ((event & ARM_USBH_EVENT_CORE_RECOVERY_FAIL) != 0U) {
      return usbUnknownError;
    }
  }

  return usbUnknownError;
}

/// \brief De-bounce timer callback for connect events
/// \param[in]     arg                  index (instance) of USB Host controller.
/// \return                             none.
void USBH_ConnectDebounce (void const *arg){
  USBH_HCI            *ptr_hci;
  ARM_USBH_PORT_STATE  port_state;
  uint8_t              instance;
  uint16_t             port_mask;
  uint8_t              port;

  instance = (uint8_t)((uint32_t)arg);
  if (instance >= usbh_hc_num) {
    return;
  }

  ptr_hci   = &usbh_hci[instance];
  port_mask =  usbh_capabilities[instance].port_mask;

  if (ptr_hci->debounce_restart != 0U) {
    ptr_hci->debounce_restart       = 0U;
    ptr_hci->debounce_countdown     = usbh_debounce_in_ms[instance] / 8U;
    ptr_hci->debounce_max_countdown = usbh_debounce_in_ms[instance] / 4U;
  }
  if (ptr_hci->debounce_countdown != 0U) {
    ptr_hci->debounce_countdown--;
  }
  if (ptr_hci->debounce_max_countdown != 0U) {
    ptr_hci->debounce_max_countdown--;
  }

  for (port = 0U; port < 15U; port++) {
    if (((port_mask >> port) == 0U) || ((ptr_hci->port_debounce >> port) == 0U)) {
      break;
    }
    port_state = USBH_DriverPortGetState (instance, port);
    if (((ptr_hci->port_debounce >> port) & 1U) != 0U) {
      if (port_state.connected == ((ptr_hci->port_con >> port) & 1U)) {
        // If debouncing port state changed reload debouncing countdown value
        ptr_hci->debounce_countdown = usbh_debounce_in_ms[instance] / 10U;
      }
    }
  }

  if (ptr_hci->debounce_max_countdown == 0U) {
    ptr_hci->port_debounce = 0U;
    return;
  }
  if (ptr_hci->debounce_countdown == 0U) {
    for (port = 0U; port < 15U; port++) {
      if (((port_mask >> port) == 0U) || ((ptr_hci->port_debounce >> port) == 0U)) {
        break;
      }
      port_state = USBH_DriverPortGetState (instance, port);
      if (((ptr_hci->port_debounce >> port) & 1U) != 0U) {
        if ((port_state.connected != 0U) && ((((ptr_hci->port_con >> port) & 1U)) == 0U)) {
          ptr_hci->port_event[port] |= ARM_USBH_EVENT_CORE_CONNECT_DEBOUNCED;
          (void)USBH_ThreadFlagsSet (usbh_core_thread_id[instance], ARM_USBH_EVENT_PORT);
        }
      }
    }
    ptr_hci->port_debounce = 0U;
  }
  if (ptr_hci->port_debounce != 0U) {
    (void)USBH_TimerStart (usbh_debounce_timer_id[instance], 12U);      // Restart timer
  }
}


/************************** Local functions ***********************************/

/// \brief Check host controller valid
/// \param[in]     ctrl                 index of USB Host controller.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus CheckController (uint8_t ctrl) {

  if (ctrl >= usbh_hc_num)         { return usbControllerError; }
  if (*usbh_hcd_ptr[ctrl] == NULL) { return usbDriverError;     }

  return usbOK;
}

/// \brief Check device instance configured
/// \param[in]     instance             instance of device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus CheckDeviceInstanceConfigured (uint8_t instance) {

  if (usbh_dev[instance].dev_addr          == 0U) { return usbDeviceError; }
  if (usbh_dev[instance].state.configured  == 0U) { return usbDeviceError; }
  if (usbh_dev[instance].vid               == 0U) { return usbDeviceError; }
  if (usbh_dev[instance].pid               == 0U) { return usbDeviceError; }

  return usbOK;
}

/// \brief Check device instance
/// \param[in]     instance             instance of device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus CheckDeviceInstance (uint8_t instance) {

  if (usbh_dev[instance].dev_addr          == 0U) { return usbDeviceError; }
  if (usbh_dev[instance].state.configured  == 0U) { return usbDeviceError; }
  if (usbh_dev[instance].state.initialized == 0U) { return usbDeviceError; }
  if (usbh_dev[instance].vid               == 0U) { return usbDeviceError; }
  if (usbh_dev[instance].pid               == 0U) { return usbDeviceError; }

  return usbOK;
}

/// \brief Check device instance
/// \param[in]     ptr_pipe             pointer to pipe.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus CheckPipe (const USBH_PIPE *ptr_pipe) {

  if (ptr_pipe                        == NULL)         { return usbInvalidParameter; }
  if (ptr_pipe->hw_handle             == 0U)           { return usbInvalidParameter; }
  if (ptr_pipe->device                >= usbh_dev_num) { return usbDeviceError;      }
  if (usbh_dev[ptr_pipe->device].state.in_use == 0U)   { return usbDeviceError;      }
  if (usbh_dev[ptr_pipe->device].ctrl >= usbh_hc_num)  { return usbControllerError;  }

  return usbOK;
}

/// \brief Callback function called when some event has happened on corresponding controller and port
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     port                 index of Root HUB port.
/// \param[in]     device               index of USB Device
///                                       - value <= 127: index of of USB Device for device notifications
///                                       - value 255: no device information for port notifications
/// \param[in]     notify               notification:
///                                       - USBH_NOTIFY_CONNECT               = Port connection happened
///                                       - USBH_NOTIFY_DISCONNECT            = Port disconnection happened
///                                       - USBH_NOTIFY_OVERCURRENT           = Port overcurrent happened
///                                       - USBH_NOTIFY_REMOTE_WAKEUP         = Port remote wakeup signaling happened
///                                       - USBH_NOTIFY_READY                 = Device was successfully enumerated, initialized and is ready for communication
///                                       - USBH_NOTIFY_UNKNOWN_DEVICE        = Device was successfully enumerated but there is no driver for it
///                                       - USBH_NOTIFY_INSUFFICIENT_POWER    = Device requires more power consumption than available
///                                       - USBH_NOTIFY_CONFIGURATION_FAILED  = Device was not successfully configured (not enough resources)
///                                       - USBH_NOTIFY_INITIALIZATION_FAILED = Device was not successfully initialized
__STATIC_INLINE void USBH_Notify_Lib (uint8_t ctrl, uint8_t port, uint8_t device, USBH_NOTIFY notify) {

  EvrUSBH_Core_OnNotify    (ctrl, port, device, notify);
  USBH_Notify              (ctrl, port, device, notify);
}

/// \brief Create Default Pipe
/// \param[in]     ctrl                 index of USB Host controller.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_DefaultPipeCreate (uint8_t ctrl) {
  USBH_PIPE *ptr_pipe;
  uint32_t   hw_handle;
  usbStatus  status;

  status = CheckController (ctrl);
  if (status == usbOK) {
    ptr_pipe = USBH_GetFree_PIPE (ctrl);
    if (ptr_pipe != NULL) {
      hw_handle = USBH_DriverPipeCreate (ctrl, 0U, ARM_USB_SPEED_LOW, 0U, 0U, 0U, USB_ENDPOINT_TYPE_CONTROL, 8U, 0U);
      if (hw_handle != 0U) {
        ptr_pipe->hw_handle          = hw_handle;
        ptr_pipe->bEndpointAddress   = 0U;
        ptr_pipe->bmAttributes       = USB_ENDPOINT_TYPE_CONTROL;
        ptr_pipe->wMaxPacketSize     = 8U;
        ptr_pipe->bInterval          = 0U;
        ptr_pipe->device             = 255U;
        ptr_pipe->locked             = 0U;
        ptr_pipe->transfer_active    = 0U;
        ptr_pipe->transferred        = 0U;
        usbh_hci[ctrl].def_pipe_hndl = (USBH_PIPE_HANDLE)ptr_pipe;
      } else {
        status = usbDriverError;
      }
    } else {
      status = usbDriverError;
    }
  }

  return status;
}

/// \brief Delete Default Pipe
/// \param[in]     ctrl                 index of USB Host controller.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_DefaultPipeDelete (uint8_t ctrl) {
  USBH_PIPE *ptr_pipe;
  usbStatus  status;

  ptr_pipe = NULL;

  status = CheckController (ctrl);
  if (status == usbOK) {
    ptr_pipe = (USBH_PIPE *)usbh_hci[ctrl].def_pipe_hndl;
    EvrUSBH_Core_PipeDelete((USBH_PIPE_HANDLE)ptr_pipe);
    if (ptr_pipe != NULL) {
      status  = USBH_DriverPipeTransferAbort (ctrl, ptr_pipe->hw_handle);
      if (status == usbOK) {
        status = USBH_DriverPipeDelete (ctrl, ptr_pipe->hw_handle);
        if (status == usbOK) {
          memset ((void *)ptr_pipe, 0, sizeof(USBH_PIPE));
        }
      }
    } else {
      status = usbControllerError;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_Core_PipeDeleteFailed((USBH_PIPE_HANDLE)ptr_pipe, status);
  }
#endif
  return status;
}

/// \brief Set Maximum Packet Size for Default Pipe
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_DefaultPipeModify (uint8_t ctrl, uint8_t device) {
  const USBH_DEV  *ptr_dev;
        USBH_PIPE *ptr_pipe;
        usbStatus  status;

  ptr_pipe = (USBH_PIPE *)usbh_hci[ctrl].def_pipe_hndl;
  if (ptr_pipe == NULL) {
    return usbControllerError;
  }
  ptr_dev = &usbh_dev[device];
  if (ctrl != ptr_dev->ctrl) {
    return usbControllerError;
  }

  status = USBH_DriverPipeModify (ptr_dev->ctrl, ptr_pipe->hw_handle, ptr_dev->dev_addr, ptr_dev->dev_speed, ptr_dev->hub_addr, ptr_dev->hub_port, ptr_dev->max_packet_size);
  if (status == usbOK) {
    ptr_pipe->device = device;
    ptr_pipe->wMaxPacketSize = ptr_dev->max_packet_size;
  }

  return status;
}

/// \brief Do Ping on Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_PipeDoPing (USBH_PIPE_HANDLE pipe_hndl) {
  const USBH_DEV           *ptr_dev;
  const USBH_HCI           *ptr_hci;
        USBH_PIPE          *ptr_pipe;
        uint32_t            event;
        ARM_USBH_EP_HANDLE  hw_handle;
        uint8_t             ctrl;
        uint8_t             device;
        usbStatus           status;

  ptr_pipe = (USBH_PIPE *)pipe_hndl;
  status   = CheckPipe (ptr_pipe);
  if (status != usbOK) {
    return status;
  }

  device    =  ptr_pipe->device;
  ptr_dev   = &usbh_dev[device];
  ctrl      =  ptr_dev->ctrl;
  ptr_hci   = &usbh_hci[ctrl];
  hw_handle =  ptr_pipe->hw_handle;

  if (ptr_pipe->thread_id != NULL) {
    status = usbDriverBusy;
    return status;
  }
  ptr_pipe->thread_id = USBH_ThreadGetHandle();

  for (;;) {
    status = USBH_DriverPipeTransfer (ctrl, hw_handle, ARM_USBH_PACKET_PING, NULL, 0U);
    if (status != usbOK) {                                              // If transfer failed clear EP to TID and exit
      goto done;
    }
    event = USBH_ThreadFlagsWait (0xFFFFFFFFU);                         // No time-out
    switch (event) {
      default:                                                          // If transfer event happened
        if (event != 0U) {                                              // Handle pipe event
          if ((event & (~ARM_USBH_PIPE_EVENT_MASK)) != 0U) {            // If port event happened also resend it to thread and stop current transfer
            (void)USBH_ThreadFlagsSet (ptr_pipe->thread_id, event & (~ARM_USBH_PIPE_EVENT_MASK));
            if (ptr_hci->port_lock != 0U) {
              if ((ptr_hci->port_event[ptr_hci->port_lock - 1U]) != 0U) {
                status = usbTransferError;
                goto done;
              }
            } else {
              if ((ptr_hci->port_event[ptr_dev->hub_port - 1U]) != 0U) {
                status = usbTransferError;
                goto done;
              }
            }
          }
          // Analyze received pipe event
          switch (event & ARM_USBH_PIPE_EVENT_MASK) {
            case ARM_USBH_EVENT_TRANSFER_COMPLETE:
              status = usbOK;
              goto done;
            case ARM_USBH_EVENT_BUS_ERROR:
              status = usbTransferError;
              goto done;
            case ARM_USBH_EVENT_HANDSHAKE_NAK:
              continue;                                                 // Retry
            case ARM_USBH_EVENT_ABORT:
              status = usbTransferError;
              goto abort_and_done;
            default:
              status = usbUnknownError;
              goto done;
          }
        }
        break;
      case 0U:                                                          // If transfer time-out happened
        status = usbTimeout;
        goto abort_and_done;
      case 0x80000000U:                                                 // If unexpected happened
        status = usbUnknownError;
        goto done;
    }
  }

abort_and_done:
  (void)USBH_DriverPipeTransferAbort (ctrl, hw_handle);

done:
  ptr_pipe->thread_id = NULL;

  return status;
}

/// \brief Send Setup Packet on Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \param[in]     setup_packet         pointer to setup packet.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_PipeSendSetup (USBH_PIPE_HANDLE pipe_hndl, const USB_SETUP_PACKET *setup_packet) {
  const USBH_DEV           *ptr_dev;
  const USBH_HCI           *ptr_hci;
        USBH_PIPE          *ptr_pipe;
        uint32_t            event;
        ARM_USBH_EP_HANDLE  hw_handle;
        uint8_t            *ptr_data;
        uint8_t             ctrl;
        bool                alloc_mem;
        usbStatus           status;
        usbStatus           mstatus;

  if (setup_packet == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }

  ptr_pipe = (USBH_PIPE *)pipe_hndl;
  status   = CheckPipe (ptr_pipe);
  if (status != usbOK) {
    return status;
  }
  if (ptr_pipe->locked != 0U) {
    status = usbDriverBusy;
    return status;
  }

  ptr_dev   = &usbh_dev[ptr_pipe->device];
  ctrl      =  ptr_dev->ctrl;
  ptr_hci   = &usbh_hci[ctrl];
  hw_handle =  ptr_pipe->hw_handle;

  ptr_pipe->locked = 1U;

  if (*usbh_mem_pool_reloc_ptr[ctrl] != 0U) {                           // If memory relocation is enabled
    alloc_mem = !USBH_MemoryIsInPool (ctrl, (const uint8_t *)setup_packet);
    if (alloc_mem) {
      // If memory for USB must be relocated, allocate 8 bytes for SETUP packet
      mstatus = USBH_MemoryAllocate (ctrl, (uint8_t **)((uint32_t)&ptr_data), 8U);
      if (mstatus != usbOK) {
        status = usbMemoryError;
        goto exit;
      }
      memcpy((void *)ptr_data, (const void *)setup_packet, 8U);         // Copy SETUP data to allocated buffer
    } else {                                                            // If memory is in pool it is ok
      ptr_data = (uint8_t *)((uint32_t)setup_packet);
    }
  } else {                                                              // If memory relocation is not enabled
    alloc_mem = false;
    ptr_data  = (uint8_t *)((uint32_t)setup_packet);
  }

  // Send Setup Packet
  if (ptr_pipe->thread_id != NULL) {
    status = usbDriverBusy;
    goto mem_free_and_exit;
  }
  ptr_pipe->thread_id = USBH_ThreadGetHandle();
  status = USBH_DriverPipeTransfer (ctrl, hw_handle, ARM_USBH_PACKET_SETUP, (uint8_t *)ptr_data, 8U);
  if (status != usbOK) {                                                // If transfer failed clear EP to TID and exit
    goto done;
  }
  ptr_pipe->transfer_active = 1U;
  event = USBH_ThreadFlagsWait(5000U);                                  // 5 s time-out for setup packet response
  ptr_pipe->transfer_active = 0U;
  switch (event) {
    default:                                                            // If transfer event happened
      if (event != 0U) {                                                // Handle pipe event
        if ((event & (~ARM_USBH_PIPE_EVENT_MASK)) != 0U) {              // If port event happened also resend it to thread and stop current transfer
          (void)USBH_ThreadFlagsSet (ptr_pipe->thread_id, event & (~ARM_USBH_PIPE_EVENT_MASK));
          if (ptr_hci->port_lock != 0U) {
            if ((ptr_hci->port_event[ptr_hci->port_lock - 1U]) != 0U) {
              status = usbTransferError;
              goto done;
            }
          } else {
            if ((ptr_hci->port_event[ptr_dev->hub_port - 1U]) != 0U) {
              status = usbTransferError;
              goto done;
            }
          }
        }
        if ((event & (~ARM_USBH_PIPE_EVENT_MASK)) != 0U) {              // If event besides pipe event happened resend it to thread
          (void)USBH_ThreadFlagsSet (ptr_pipe->thread_id, event & (~ARM_USBH_PIPE_EVENT_MASK));
        }
        // Analyze received pipe event
        switch (event & ARM_USBH_PIPE_EVENT_MASK) {
          case ARM_USBH_EVENT_TRANSFER_COMPLETE:
            ptr_pipe->transferred = 8U;
            status = usbOK;
            goto done;
          case ARM_USBH_EVENT_ABORT:
            status = usbTransferError;
            goto abort_and_done;
          case ARM_USBH_EVENT_BUS_ERROR:
            status = usbTransferError;
            goto done;
          default:
            status = usbUnknownError;
            goto done;
        }
      }
      break;
    case 0U:                                                            // If transfer time-out happened
      status = usbTimeout;
      goto abort_and_done;
    case 0x80000000U:                                                   // If unexpected happened
      status = usbUnknownError;
      goto done;
  }

abort_and_done:
  (void)USBH_DriverPipeTransferAbort (ctrl, hw_handle);

done:
  ptr_pipe->thread_id = NULL;
  ptr_pipe->locked    = 0U;

mem_free_and_exit:
  if (alloc_mem) {
    mstatus = USBH_MemoryFree (ctrl, (uint8_t  *)ptr_data);
    if (mstatus != usbOK) {
      status = mstatus;
    }
  }

exit:
  return status;
}

/// \brief Enumerate preparation of USB Device, get maximum packet size for Default Pipe
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     port                 port index (0 .. 127).
/// \param[in]     speed                device speed.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_PrepareEnumerateDevice (uint8_t ctrl, uint8_t port, uint8_t speed) {
  USB_DEVICE_DESCRIPTOR *ptr_dev_desc;
  uint8_t                device;
  usbStatus              status;
  usbStatus              mstatus;

  (void)port;

  status = CheckController (ctrl);
  if (status != usbOK) {
    return status;
  }

  // Find first free device class instance block
  for (device = 0U; device <= usbh_dev_num; device++) {
    if (device == usbh_dev_num) {
      return usbDeviceError;                  // Free block not found
    }
    if (usbh_dev[device].state.in_use == 0U) {
      break;                                  // Free block found
    }
  }

  usbh_dev[device].state.in_use = 1U;
  usbh_hci[ctrl].device = device;             // Device being enumerated

  // Initialize default device communication parameters
  usbh_dev[device].ctrl              = ctrl;
  usbh_dev[device].dev_speed         = speed;
  if (speed == USB_SPEED_LOW) {
    usbh_dev[device].max_packet_size =  8U;
  } else {
    usbh_dev[device].max_packet_size = 64U;
  }

  (void)USBH_Delay (100U);

  // Get Device Descriptor (8 or 64 bytes) to determine maximum packet size
  // and set device max packet size
  mstatus = USBH_MemoryAllocate                 (ctrl, (uint8_t **)((uint32_t)&ptr_dev_desc), 64U);
  if (mstatus != usbOK) {
    return mstatus;
  }
  status = USBH_DeviceRequest_GetDescriptor     (device, USB_REQUEST_TO_DEVICE, USB_DEVICE_DESCRIPTOR_TYPE, 0U, 0U, (uint8_t *)ptr_dev_desc, 64U);
  if (status == usbOK) {
    usbh_dev[device].max_packet_size = ptr_dev_desc->bMaxPacketSize0;
  }
  mstatus = USBH_MemoryFree                     (ctrl, (uint8_t *)ptr_dev_desc);
  if ((status == usbOK) && (mstatus != usbOK)) {
    status = mstatus;
  }

  return status;
}

/// \brief Enumerate USB Device
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     port                 port index (0 .. 127).
/// \param[in]     speed                device speed.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_EnumerateDevice (uint8_t ctrl, uint8_t port, uint8_t speed) {
        USBH_HCI                     *ptr_hci;
        USBH_DEV                     *ptr_dev;
        USB_DEVICE_DESCRIPTOR        *ptr_dev_desc;
        USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc;
  const USB_INTERFACE_DESCRIPTOR     *ptr_if_desc;
  const uint8_t                      *ptr_desc;
        uint16_t                      dev_desc_len;
        uint16_t                      len;
        uint8_t                       class_instance;
        uint8_t                       device;
        uint8_t                       dev_addr;
        usbStatus                     status;
        usbStatus                     mstatus;

  status = CheckController (ctrl);
  if (status != usbOK) {
    return status;
  }

  ptr_hci = &usbh_hci[ctrl];
  ptr_dev_desc = NULL;
  ptr_cfg_desc = NULL;

  len = ((USBH_PIPE *)(ptr_hci->def_pipe_hndl))->wMaxPacketSize;

  // Check if there is an available address for new device
  if (((ptr_hci->dev_addr_mask[0] &  0xFFFFFFFEU) == 0xFFFFFFFEU) &&
       (ptr_hci->dev_addr_mask[1] == 0xFFFFFFFFU)                 &&
       (ptr_hci->dev_addr_mask[2] == 0xFFFFFFFFU)                 &&
       (ptr_hci->dev_addr_mask[3] == 0xFFFFFFFFU)) {
    return usbDeviceError;
  }

  // Find first free device address
  dev_addr = (ptr_hci->last_dev_addr + 1U) & 0x7FU;
  while (((ptr_hci->dev_addr_mask[dev_addr >> 5] & (1UL << (dev_addr & 0x1FU))) != 0U) || (dev_addr == 0U)) {
    dev_addr++;
    dev_addr &= 0x7FU;
  }
  ptr_hci->last_dev_addr = dev_addr;

  device = usbh_hci[ctrl].device;

  // 1. USB bus: Set Address
  status = USBH_DeviceRequest_SetAddress        (device, dev_addr);
  if (status != usbOK) {
    return status;
  }
  (void)USBH_Delay (50U);                       // Delay 50 ms (max allowed)

  // 2. USB bus: Get Device Descriptor
  mstatus = USBH_MemoryAllocate                 (ctrl, (uint8_t **)((uint32_t)&ptr_dev_desc), 18U);
  if (mstatus != usbOK) {
    return mstatus;
  }
  status  = USBH_DeviceRequest_GetDescriptor    (device, USB_REQUEST_TO_DEVICE, USB_DEVICE_DESCRIPTOR_TYPE, 0U, 0U, (uint8_t *)ptr_dev_desc, 18U);
  if (status == usbOK) {
    dev_desc_len = ptr_dev_desc->bLength;
  } else {
    goto mem_free_and_exit;
  }

  // 3. USB bus: Get 9 bytes of Configuration Descriptor to determine size of whole Configuration Descriptor
  mstatus = USBH_MemoryAllocate                 (ctrl, (uint8_t **)((uint32_t)&ptr_cfg_desc), 9U);
  if (mstatus != usbOK) {
    return mstatus;
  }
  status  = USBH_DeviceRequest_GetDescriptor    (device, USB_REQUEST_TO_DEVICE, USB_CONFIGURATION_DESCRIPTOR_TYPE, 0U, 0U, (uint8_t *)ptr_cfg_desc, 9U);
  if (status == usbOK) {
    len = ptr_cfg_desc->wTotalLength;
  } else {
    goto mem_free_and_exit;
  }
  mstatus = USBH_MemoryFree                     (ctrl, (uint8_t *)ptr_cfg_desc);
  if (mstatus != usbOK) {
    status = mstatus;
    goto mem_free_and_exit;
  }
  ptr_cfg_desc = NULL;
  (void)USBH_Delay (10U);

  // 4. USB bus: Get Configuration Descriptor
  mstatus = USBH_MemoryAllocate                 (ctrl,(uint8_t **)((uint32_t)&ptr_cfg_desc), len);
  if (mstatus != usbOK) {
    status = mstatus;
    goto mem_free_and_exit;
  }
  status  = USBH_DeviceRequest_GetDescriptor    (device, USB_REQUEST_TO_DEVICE, USB_CONFIGURATION_DESCRIPTOR_TYPE, 0U, 0U, (uint8_t *)ptr_cfg_desc, len);
  if (status == usbOK) {
    ptr_hci->dev_addr_mask[dev_addr >> 5] |= (1UL << (dev_addr & 0x1FU));
    ptr_desc                 = (uint8_t *)ptr_cfg_desc;
    ptr_desc                += (uint32_t)ptr_cfg_desc->bLength;
    ptr_if_desc              = (const USB_INTERFACE_DESCRIPTOR *)ptr_desc;

    ptr_dev                  = &usbh_dev[device];

    ptr_dev->ctrl            = ctrl;
    ptr_dev->dev_addr        = dev_addr;
    ptr_dev->dev_speed       = speed;
    ptr_dev->hub_addr        = 0U;
    ptr_dev->hub_port        = port;
    ptr_dev->max_packet_size = ptr_dev_desc->bMaxPacketSize0;
    ptr_dev->vid             = ptr_dev_desc->idVendor;
    ptr_dev->pid             = ptr_dev_desc->idProduct;
    ptr_dev->dev_desc_len    = (uint8_t)dev_desc_len;
    ptr_dev->cfg_desc_len    = len;

    // Check if device requires more power than available
    if (*usbh_power_ptr[ctrl] < ptr_cfg_desc->bMaxPower) {
      USBH_Notify_Lib (ctrl, port, device, USBH_NOTIFY_INSUFFICIENT_POWER);
      status = usbNotConfigured;
      goto mem_free_and_exit;
    }

    // First try to configure custom class device, so even library mass storage and HID handling can be overridden
    class_instance = USBH_CustomClass_Configure (device, ptr_dev_desc, ptr_cfg_desc);
    EvrUSBH_CC_OnConfigure(device, ptr_dev_desc->bDeviceClass, ptr_dev_desc->bDeviceSubClass, ptr_dev_desc->idVendor, ptr_dev_desc->idProduct, class_instance);
    if (class_instance < 254U) {
      ptr_dev->class_instance = class_instance;
      ptr_dev->class_custom = 1U;
    } else {
      ptr_dev->class_custom = 0U;
      switch (ptr_dev_desc->bDeviceClass) {
        case USB_DEVICE_CLASS_RESERVED:
          switch (ptr_if_desc->bInterfaceClass) {
            case USB_DEVICE_CLASS_HUMAN_INTERFACE:
              class_instance = USBH_HID_Configure (device, ptr_dev_desc, ptr_cfg_desc);
              if (class_instance < 254U) {
                ptr_dev->class_instance = class_instance;
                ptr_dev->class_driver   = USB_DEVICE_CLASS_HUMAN_INTERFACE;
              }
              break;
            case USB_DEVICE_CLASS_STORAGE:
              class_instance = USBH_MSC_Configure (device, ptr_dev_desc, ptr_cfg_desc);
              if (class_instance < 254U) {
                ptr_dev->class_instance = class_instance;
                ptr_dev->class_driver   = USB_DEVICE_CLASS_STORAGE;
              }
              break;
            default:
              class_instance = 254U;
              break;
          }
          break;
        case USB_DEVICE_CLASS_COMMUNICATIONS:
        case USB_DEVICE_CLASS_MISCELLANEOUS:
          class_instance = USBH_CDC_Configure (device, ptr_dev_desc, ptr_cfg_desc);
          if (class_instance < 254U) {
            ptr_dev->class_instance = class_instance;
            ptr_dev->class_driver   = USB_DEVICE_CLASS_COMMUNICATIONS;
          }
          break;
        case USB_DEVICE_CLASS_HUMAN_INTERFACE:
          class_instance = USBH_HID_Configure (device, ptr_dev_desc, ptr_cfg_desc);
          if (class_instance < 254U) {
            ptr_dev->class_instance = class_instance;
            ptr_dev->class_driver   = USB_DEVICE_CLASS_HUMAN_INTERFACE;
          }
          break;
        case USB_DEVICE_CLASS_STORAGE:
          class_instance = USBH_MSC_Configure (device, ptr_dev_desc, ptr_cfg_desc);
          if (class_instance < 254U) {
            ptr_dev->class_instance = class_instance;
            ptr_dev->class_driver   = USB_DEVICE_CLASS_STORAGE;
          }
          break;
        case USB_DEVICE_CLASS_AUDIO:
        case USB_DEVICE_CLASS_MONITOR:
        case USB_DEVICE_CLASS_PHYSICAL_INTERFACE:
        case USB_DEVICE_CLASS_POWER:
        case USB_DEVICE_CLASS_PRINTER:
        case USB_DEVICE_CLASS_HUB:
        case USB_DEVICE_CLASS_VENDOR_SPECIFIC:
        default:                                // Unsupported device types
          class_instance = 254U;
          break;
      }
      if (class_instance == 255U) {             // Device configuration failed
        status = usbNotConfigured;
        USBH_Notify_Lib (ctrl, port, device, USBH_NOTIFY_CONFIGURATION_FAILED);
      } else if (class_instance == 254U) {      // Driver for device class is not available
        ptr_dev->state.unsupported = 1U;
        status = usbUnsupportedClass;
        USBH_Notify_Lib (ctrl, port, device, USBH_NOTIFY_UNKNOWN_DEVICE);
      }
    }

    // Release memory so it can be reused during device initialization
    mstatus = USBH_MemoryFree (ctrl, (uint8_t *)ptr_cfg_desc);
    if (mstatus != usbOK) {
      status = mstatus;
    }
    ptr_cfg_desc = NULL;
    mstatus = USBH_MemoryFree (ctrl, (uint8_t *)ptr_dev_desc);
    if ((status == usbOK) && (mstatus != usbOK)) {
      status = mstatus;
    }
    ptr_dev_desc = NULL;

    if (status != usbOK) {
      // If device is unknown or configuration of device has failed
      goto exit;
    }

    (void)USBH_Delay (10U);

    // 5. USB bus: Set Configuration (1)
    status = USBH_DeviceRequest_SetConfiguration (device, 1U);
    if (status != usbOK) {
      goto exit;
    }

    (void)USBH_Delay (50U);
    if (ptr_dev->class_custom != 0U) {
      status = USBH_CustomClass_Initialize (class_instance);
      EvrUSBH_CC_OnInitialize(class_instance, status);
    } else {
      switch (ptr_dev->class_driver) {
        case USB_DEVICE_CLASS_STORAGE:
          status = USBH_MSC_Initialize_Lib (class_instance);
          break;
        case USB_DEVICE_CLASS_HUMAN_INTERFACE:
          status = USBH_HID_Initialize_Lib (class_instance);
          break;
        case USB_DEVICE_CLASS_COMMUNICATIONS:
          status = USBH_CDC_Initialize_Lib (class_instance);
          break;
        default:                        // Class without driver
          break;
      }
    }
    if (status == usbOK) {
      ptr_dev->state.initialized = 1U;
      USBH_Notify_Lib (ctrl, port, device, USBH_NOTIFY_READY);
    } else {
      USBH_Notify_Lib (ctrl, port, device, USBH_NOTIFY_INITIALIZATION_FAILED);
      if (ptr_dev->class_custom != 0U) {
        status = USBH_CustomClass_Unconfigure (class_instance);
        EvrUSBH_CC_OnUnconfigure(class_instance, status);
      } else {
        switch (ptr_dev->class_driver) {
          case USB_DEVICE_CLASS_STORAGE:
            (void)USBH_MSC_Unconfigure (class_instance);
            break;
          case USB_DEVICE_CLASS_HUMAN_INTERFACE:
            (void)USBH_HID_Unconfigure (class_instance);
            break;
          case USB_DEVICE_CLASS_COMMUNICATIONS:
            (void)USBH_CDC_Unconfigure (class_instance);
            break;
          default:                      // Class without driver
            break;
        }
      }
      memset ((void *)ptr_dev, 0, sizeof (USBH_DEV));
    }
  }

mem_free_and_exit:
  if (ptr_cfg_desc != NULL) {
    mstatus = USBH_MemoryFree (ctrl, (uint8_t *)ptr_cfg_desc);
    if ((status == usbOK) && (mstatus != usbOK)) {
      status = mstatus;
    }
  }
  if (ptr_dev_desc != NULL) {
    mstatus = USBH_MemoryFree (ctrl, (uint8_t *)ptr_dev_desc);
    if ((status == usbOK) && (mstatus != usbOK)) {
      status = mstatus;
    }
  }

exit:
  return status;
}

/// \brief Find recovery USB Device index
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     port                 port index (0 .. 127).
/// \return        value <= 127         index of device.
/// \return        value == 0xFF        device does not exist.
static uint8_t USBH_FindRecoveryDevice (uint8_t ctrl, uint8_t port) {
  const USBH_DEV *ptr_dev;
        uint8_t   device;

  // Find device that we are recovering
  ptr_dev = &usbh_dev[0];
  for (device = 0U; device <= usbh_dev_num; device++) {
    if (device == usbh_dev_num) {
      return 0xFFU;                     // Recovery device was not found
    }
    if ((ptr_dev->ctrl == ctrl) && (ptr_dev->hub_port == port)) {
      break;                            // Recovery device found              */
    }
    ptr_dev++;
  }

  return device;
}

/// \brief Recovery re-enumerate USB Device
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     port                 port index (0 .. 127).
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_RecoveryEnumerateDevice (uint8_t ctrl, uint8_t port) {
  USBH_HCI                     *ptr_hci;
  USBH_DEV                     *ptr_dev;
  USB_DEVICE_DESCRIPTOR        *ptr_dev_desc;
  USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc;
  uint8_t                       device;
  uint8_t                       dev_addr;
  usbStatus                     status;
  usbStatus                     mstatus;

  status = CheckController (ctrl);
  if (status != usbOK) {
    return status;
  }

  ptr_hci = &usbh_hci[ctrl];

  device = USBH_FindRecoveryDevice (ctrl, port);

  // Clear previously used address
  if (device != 0xFFU) {
    ptr_hci->dev_addr_mask[usbh_dev[device].dev_addr >> 5] |= (1UL << (usbh_dev[device].dev_addr & 0x1FU));
    ptr_dev = &usbh_dev[device];
  } else {
    return usbDeviceError;
  }

  // Check if there is an available address
  if (((ptr_hci->dev_addr_mask[0] &  0xFFFFFFFEU) == 0xFFFFFFFEU) &&
       (ptr_hci->dev_addr_mask[1] == 0xFFFFFFFFU)                 &&
       (ptr_hci->dev_addr_mask[2] == 0xFFFFFFFFU)                 &&
       (ptr_hci->dev_addr_mask[3] == 0xFFFFFFFFU)) {
    return usbDeviceError;
  }

  // Find first free device address
  dev_addr = (ptr_hci->last_dev_addr + 1U) & 0x7FU;
  while (((ptr_hci->dev_addr_mask[dev_addr >> 5] & (1UL << (dev_addr & 0x1FU))) != 0U) || (dev_addr == 0U)) {
    dev_addr++;
    dev_addr &= 0x7FU;
  }
  ptr_hci->last_dev_addr = dev_addr;

  // Reset address to 0 to restart communication with recovery device
  ptr_dev->dev_addr = 0U;

  // 1. USB bus: Get Device Descriptor
  mstatus = USBH_MemoryAllocate                 (ctrl, (uint8_t **)((uint32_t)&ptr_dev_desc), usbh_dev[device].dev_desc_len);
  if (mstatus != usbOK) {
    return mstatus;
  }
  status  = USBH_DeviceRequest_GetDescriptor    (device, USB_REQUEST_TO_DEVICE, USB_DEVICE_DESCRIPTOR_TYPE, 0U, 0U, (uint8_t *)ptr_dev_desc, usbh_dev[device].dev_desc_len);
  mstatus = USBH_MemoryFree                     (ctrl, (uint8_t *)ptr_dev_desc);
  if (mstatus != usbOK) {
    return mstatus;
  }
  if (status != usbOK) {
    return status;
  }

  // 2. USB bus: Set Address
  status  = USBH_DeviceRequest_SetAddress       (device, dev_addr);
  if (status != usbOK) {
    return status;
  }
  (void)USBH_Delay (50U);                       // Delay 50 ms (max time)

  // 3. USB bus: Get Device Descriptor
  mstatus = USBH_MemoryAllocate                 (ctrl, (uint8_t **)((uint32_t)&ptr_dev_desc), 64U);
  if (mstatus != usbOK) {
    return mstatus;
  }
  status  = USBH_DeviceRequest_GetDescriptor    (device, USB_REQUEST_TO_DEVICE, USB_DEVICE_DESCRIPTOR_TYPE, 0U, 0U, (uint8_t *)ptr_dev_desc, usbh_dev[device].dev_desc_len);
  mstatus = USBH_MemoryFree                     (ctrl, (uint8_t *)ptr_dev_desc);
  if (mstatus != usbOK) {
    return mstatus;
  }
  if (status != usbOK) {
    return status;
  }

  // 4. USB bus: Get Configuration Descriptor
  (void)USBH_Delay (50U);                       // Wait 50 ms for robustness
  mstatus = USBH_MemoryAllocate                 (ctrl,(uint8_t **)((uint32_t)&ptr_cfg_desc), usbh_dev[device].cfg_desc_len);
  if (mstatus != usbOK) {
    return mstatus;
  }
  status  = USBH_DeviceRequest_GetDescriptor    (device, USB_REQUEST_TO_DEVICE, USB_CONFIGURATION_DESCRIPTOR_TYPE, 0U, 0U, (uint8_t *)ptr_cfg_desc, usbh_dev[device].cfg_desc_len);
  mstatus = USBH_MemoryFree                     (ctrl, (uint8_t *)ptr_cfg_desc);
  if (mstatus != usbOK) {
    return mstatus;
  }
  if (status != usbOK) {
    return status;
  }

  ptr_hci->dev_addr_mask[dev_addr >> 5] |= (1UL << (dev_addr & 0x1FU));
  (void)USBH_Delay (10U);

  // 5. USB bus: Set Configuration (1)
  status = USBH_DeviceRequest_SetConfiguration  (device, 1U);
  (void)USBH_Delay (50U);

  ptr_hci->device = device;

  return status;
}

/// \brief De-initialize all USB Devices connected to specified port
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     port                 port index (0 .. 127).
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_UninitializeDevices (uint8_t ctrl, uint8_t port) {
  USBH_HCI *ptr_hci;
  USBH_DEV *ptr_dev;
  uint8_t   device;
  uint8_t   dev_addr;
  uint8_t   class_instance;
  usbStatus status, first_err_status;

  first_err_status = CheckController (ctrl);
  if (first_err_status != usbOK) {
    goto exit;
  }

  ptr_hci = &usbh_hci[ctrl];
  ptr_dev = &usbh_dev[0];
  status  = usbOK;

  for (device = 0U; device <= usbh_dev_num; device++) {
    if (device == usbh_dev_num) {
      break;
    }
    if ((ptr_dev->ctrl == ctrl) && (ptr_dev->hub_port == port)) {
      dev_addr       = ptr_dev->dev_addr;
      class_instance = ptr_dev->class_instance;
      if (ptr_dev->state.initialized != 0U) {
        if (ptr_dev->class_custom != 0U) {
          status = USBH_CustomClass_Uninitialize (class_instance);
          EvrUSBH_CC_OnUninitialize(class_instance, status);
        } else {
          switch (ptr_dev->class_driver) {
            case USB_DEVICE_CLASS_STORAGE:
              status = USBH_MSC_Uninitialize_Lib (class_instance);
              break;
            case USB_DEVICE_CLASS_HUMAN_INTERFACE:
              status = USBH_HID_Uninitialize_Lib (class_instance);
              break;
            case USB_DEVICE_CLASS_COMMUNICATIONS:
              status = USBH_CDC_Uninitialize_Lib (class_instance);
              break;
            default:                    // Class without driver
              break;
          }
        }
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
      }
      if (ptr_dev->state.configured != 0U) {
        if (ptr_dev->class_custom != 0U) {
          status = USBH_CustomClass_Unconfigure (class_instance);
          EvrUSBH_CC_OnUnconfigure(class_instance, status);
        } else {
          switch (ptr_dev->class_driver) {
            case USB_DEVICE_CLASS_STORAGE:
              status = USBH_MSC_Unconfigure (class_instance);
              break;
            case USB_DEVICE_CLASS_HUMAN_INTERFACE:
              status = USBH_HID_Unconfigure (class_instance);
              break;
            case USB_DEVICE_CLASS_COMMUNICATIONS:
              status = USBH_CDC_Unconfigure (class_instance);
              break;
            default:                    // Class without driver
              break;
          }
        }
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
      }
      memset ((void *)ptr_dev, 0, sizeof (USBH_DEV));
      ptr_hci->dev_addr_mask[dev_addr >> 5] &= ~(1UL << (dev_addr & 0x1FU));
    }
    ptr_dev++;
  }

exit:
  return first_err_status;
}

/// \brief USB Host Engine for Controller
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     port                 port index (0 .. 127).
/// \param[in]     event                USB Host event.
/// \return                             none.
static void USBH_Engine (uint8_t ctrl, uint8_t port, uint32_t event) {
  USBH_HCI            *ptr_hci;
  ARM_USBH_PORT_STATE  port_state;
  uint8_t             *ptr_port_state;
  uint8_t              device, loop_cnt;
  usbStatus            status;

  if (CheckController (ctrl) != usbOK) {
    EvrUSBH_Core_EngineFailed(ctrl, port, event);
    return;
  }

  ptr_hci        = &usbh_hci[ctrl];
  ptr_port_state = &ptr_hci->port_state[port];

  EvrUSBH_Core_Engine(ctrl, port, event, *ptr_port_state);

  if ((event & ARM_USBH_EVENT_CORE_DO_RECOVERY) != 0U) {
    *ptr_port_state = 7U;                                       // => Do reset recovery
  }

  loop_cnt = 1U;
  while (loop_cnt != 0U) {
    loop_cnt--;
    if (((event & ARM_USBH_EVENT_DISCONNECT) != 0U) ||          // If disconnect event was detected or
        ((*ptr_port_state != 0U) &&                             // (If port is in any state except Idle and 
        (USBH_DriverPortGetState (ctrl, port).connected == 0U))) {      // if state is not connected)
      if ((*ptr_port_state >= 7U) && (*ptr_port_state <= 10U)){ // If state was during reset recovery
        device = usbh_hci[ctrl].device;
        (void)USBH_ThreadFlagsSet (usbh_dev[device].recovery_thread_id, ARM_USBH_EVENT_CORE_RECOVERY_FAIL);
      }
      ptr_hci->port_con    &= (uint16_t)(~(1UL << port));
      ptr_hci->port_discon &= (uint16_t)(~(1UL << port));
      ptr_hci->port_rst     = 0U;
      (void)USBH_Delay      (100U);
      (void)USBH_UninitializeDevices (ctrl, port);
      *ptr_port_state = 0U;                                     // => Idle

      // Unlock and force check if any other device is ready for enumeration
      ptr_hci->port_lock    = 0U;
      (void)USBH_ThreadFlagsSet (usbh_core_thread_id[ctrl], ARM_USBH_EVENT_PORT);
      break;
    }
    switch (*ptr_port_state) {
      case 0:   // 0. Idle (check connection change)
        if (((~(ptr_hci->port_con) & (1UL << port)) != 0U) && ((event & ARM_USBH_EVENT_CONNECT) != 0U)) {
          ptr_hci->port_con   |= (uint16_t)(1UL <<  port);
          ptr_hci->port_retry  = 3U;
          loop_cnt++;
          *ptr_port_state += 1U;
        }
        break;
      case 1:   // 1. Activate 1st reset
        if (USBH_DriverPortReset (ctrl, port) != usbOK) {
          *ptr_port_state = 15U; loop_cnt++;                    // Port reset activation failed => Port active
        } else {
          *ptr_port_state += 1U;
        }
        break;
      case 2:   // 2. Wait for 1st reset to finish
        if ((event & ARM_USBH_EVENT_RESET) != 0U) {             // If reset done
          ptr_hci->port_rst = 0U;
          (void)USBH_Delay (35U);
          (void)USBH_DriverPortSuspend (ctrl, port);            // Suspend port
          (void)USBH_Delay (100U);                              // for 100 ms
          (void)USBH_DriverPortResume  (ctrl, port);            // Resume port
          (void)USBH_Delay (35U);
          *ptr_port_state += 1U;
        } else {
          *ptr_port_state = 15U;                                // Reset failed => Port active
        }
        loop_cnt++;
        break;
      case 3:   // 3. Activate 2st reset
        if (USBH_DriverPortReset (ctrl, port) != usbOK) {
          *ptr_port_state = 15U; loop_cnt++;                    // Port reset activation failed => Port active
        } else {
          *ptr_port_state += 1U;
        }
        break;
      case 4:   // 4. Wait for 2nd reset to finish
        if ((event & ARM_USBH_EVENT_RESET) != 0U) {             // If reset done
          ptr_hci->port_rst = 0U;
          (void)USBH_Delay (1600U);
          port_state = USBH_DriverPortGetState (ctrl, port);
          status = USBH_PrepareEnumerateDevice (ctrl, port, port_state.speed);
          if (status == usbOK) {                                // If initial pre-enumeration succeeded
            *ptr_port_state += 1U;
          } else if (status == usbDeviceError) {                // If no available resource
            *ptr_port_state = 15U;                              // => Port active
          } else if (ptr_hci->port_retry != 0U) {
            ptr_hci->port_retry--;
            *ptr_port_state = 1U;                               // Repeat 1st reset
          } else {                                              // Pre-enumeration failed 3 times
            *ptr_port_state = 15U;                              // => Port active
          }
        } else {
          *ptr_port_state = 15U;                                // No reset event => Port active
        }
        loop_cnt++;
        break;
      case 5:   // 5. Activate 3st reset
        if (USBH_DriverPortReset (ctrl, port) != usbOK) {
          *ptr_port_state = 15U; loop_cnt++;                    // Port reset activation failed => Port active
        } else {
          *ptr_port_state += 1U;
        }
        break;
      case 6:   // 6. Wait for 3rd reset to finish
        if ((event & ARM_USBH_EVENT_RESET) != 0U) {             // If reset done
          ptr_hci->port_rst = 0U;
          (void)USBH_Delay (250U);
          port_state = USBH_DriverPortGetState (ctrl, port);
          status = USBH_EnumerateDevice        (ctrl, port, port_state.speed);
          if ((status == usbOK)               ||                // If enumeration succeeded
              (status == usbUnsupportedClass) ||                // if driver not available
              (status == usbNotConfigured))    {                // if device was not configured successfully
            *ptr_port_state = 15U;                              // device enumerated ok => port active but not device
          } else if (ptr_hci->port_retry != 0U) {
            ptr_hci->port_retry--;
            *ptr_port_state = 1U;                               // Repeat 1st reset
          } else {                                              // Enumeration failed 3 times
            *ptr_port_state = 15U;                              // => Port active
          }
        } else {
          *ptr_port_state = 15U;                                // No reset event => Port active
        }
        loop_cnt++;
        break;
      case 7:   // 7. Start recovery to retry failed operation (activate 1st recovery reset)
        if (USBH_DriverPortReset (ctrl, port) != usbOK) {
          *ptr_port_state = 15U; loop_cnt++;                    // Port reset activation failed => Port active
        } else {
          *ptr_port_state += 1U;
        }
        break;
      case 8:   // 8. Wait 1st recovery reset to finish
        if ((event & ARM_USBH_EVENT_RESET) != 0U) {             // If reset done
          ptr_hci->port_rst = 0U;
          (void)USBH_Delay (100U);
          status = USBH_RecoveryEnumerateDevice (ctrl, port);
          if ((status == usbOK)               ||                // If enumeration succeeded
              (status == usbUnsupportedClass) ||                // if driver not available
              (status == usbNotConfigured))    {                // if device was not configured successfully
            *ptr_port_state = 15U;                              // device enumerated ok => port active but not device
          } else if (ptr_hci->port_retry != 0U) {
            ptr_hci->port_retry--;
            *ptr_port_state = 7U;                               // Repeat 1st recovery reset
          } else {                                              // Enumeration failed 3 times
            *ptr_port_state = 15U;                              // => Port active
          }
        } else {
          *ptr_port_state = 15U;                                // No reset event => Port active
        }
        loop_cnt++;
        break;
      case 9:   // 9. Activate 2nd recovery reset
        if (USBH_DriverPortReset (ctrl, port) != usbOK) {
          *ptr_port_state = 15U; loop_cnt++;                    // Port reset activation failed => Port active
        } else {
          *ptr_port_state += 1U;
        }
        break;
      case 10:  // 10. Wait 2nd recovery reset to finish
        if ((event & ARM_USBH_EVENT_RESET) != 0U) {             // If reset done
          ptr_hci->port_rst = 0U;
          (void)USBH_Delay (100U);
          status = USBH_RecoveryEnumerateDevice (ctrl, port);
          device = usbh_hci[ctrl].device;
          if (status == usbOK) {                                // If recovery succeeded
            (void)USBH_ThreadFlagsSet (usbh_dev[device].recovery_thread_id, ARM_USBH_EVENT_CORE_RECOVERY_OK);
            *ptr_port_state = 15U;                              // => Port active
          } else if (ptr_hci->port_retry != 0U) {
            ptr_hci->port_retry--;
            *ptr_port_state = 7U;                               // Repeat 1st recovery reset
          } else {                                              // If recovery enumeration failed
            if (device != 0xFFU) { 
              (void)USBH_ThreadFlagsSet (usbh_dev[device].recovery_thread_id, ARM_USBH_EVENT_CORE_RECOVERY_FAIL);
            }
            *ptr_port_state = 15U;                              // => Port active
          }
        } else {
          *ptr_port_state = 15U;                                // No reset event => Port active
        }
        loop_cnt++;
        break;
      case 15:  // 15. Active state, connected and enumerated, device might be non-functional if it failed to enumerate in 3 retries
        ptr_hci->port_lock = 0U;
        (void)USBH_ThreadFlagsSet (usbh_core_thread_id[ctrl], ARM_USBH_EVENT_PORT);
        break;
      default:
        *ptr_port_state = 0U;                                   // Unexpected state => Idle
        break;
    }
  }
  EvrUSBH_Core_EngineDone(ctrl, port, event, *ptr_port_state);
}


/************************** Thread functions **********************************/

/// \brief Thread Function: USB Host Core
/// \param[in]     arg                  index of USB Host controller.
void USBH_Core_Thread (void *arg) {
  USBH_HCI            *ptr_hci;
  ARM_USBH_PORT_STATE  port_state;
  uint32_t             event;
  uint32_t             millisec;
  uint32_t             events;
  uint16_t             port_mask;
  uint16_t             port_debounce;
  uint8_t              ctrl;
  uint8_t              polling_mask;
  uint8_t              port;
  uint8_t              start_timer;

  ctrl = (uint8_t)((uint32_t)arg);

  if (ctrl >= usbh_hc_num) { 
    return;
  }

  ptr_hci         = &usbh_hci[ctrl];
  port_mask       =  usbh_capabilities[ctrl].port_mask;
  millisec        =  0xFFFFFFFFU;
  polling_mask    =  0U;
  if (usbh_capabilities[ctrl].event_connect     == 0U) {
    polling_mask |=  1U;
    millisec      =  100U;
  }
  if (usbh_capabilities[ctrl].event_disconnect  == 0U) {
    polling_mask |=  (uint8_t)(1UL << 1);
    millisec      =  100U;
  }
  if (usbh_capabilities[ctrl].event_overcurrent == 0U) {
    polling_mask |=  (uint8_t)(1UL << 2);
    millisec      =  100U;
  }

  for (;;) {
    port_debounce = 0U;
    event = USBH_ThreadFlagsWait (millisec);
    if ((event & 0x8000000U) == 0U) {
      for (port = 0U; port < 15U; port++) {
        if ((port_mask >> port) == 0U) {
          break;
        }
        events = ptr_hci->port_event[port];
        if (events != 0U) {
          if ((ptr_hci->port_lock != 0U) && (ptr_hci->port_lock != (port + 1U))) {
            continue;
          }
          ptr_hci->port_event[port] ^= events;
          if ((events & ARM_USBH_EVENT_CORE_CONNECT_DEBOUNCED) != 0U) {
            ptr_hci->port_lock  = port + 1U;
            USBH_Notify_Lib (ctrl, port, 255U, USBH_NOTIFY_CONNECT);
            USBH_Engine     (ctrl, port,       ARM_USBH_EVENT_CONNECT);
          }
          if ((events & ARM_USBH_EVENT_CONNECT) != 0U) {
            if (((ptr_hci->port_con >> port) & 1U) == 0U) {
              // If port status was inactive => start debounce
              port_debounce |= (uint16_t)(1UL << port);
            }
          }
          if ((events & ARM_USBH_EVENT_DISCONNECT) != 0U) {
            if (((ptr_hci->port_con >> port) & 1U) != 0U) {
              // If port status was active => send disconnect event
              USBH_Engine     (ctrl, port,       ARM_USBH_EVENT_DISCONNECT);
              USBH_Notify_Lib (ctrl, port, 255U, USBH_NOTIFY_DISCONNECT);
            }
          }
          if ((events & ARM_USBH_EVENT_OVERCURRENT) != 0U) {
            if (((ptr_hci->port_oc >> port) & 1U) == 0U) {
              ptr_hci->port_oc |= (uint16_t)(1UL << port);
              // If port overcurrent was not active => send overcurrent event
              (void)USBH_DriverPortVbusOnOff (ctrl, port, false);
              USBH_Engine                    (ctrl, port,       ARM_USBH_EVENT_OVERCURRENT);
              USBH_Notify_Lib                (ctrl, port, 255U, USBH_NOTIFY_OVERCURRENT);
            }
          }
          if ((events & ARM_USBH_EVENT_RESET) != 0U) {
            if (usbh_hci[ctrl].port_rst == (port + 1U)) {
              USBH_Engine (ctrl, port, ARM_USBH_EVENT_RESET);
            }
          }
          if ((events & ARM_USBH_EVENT_REMOTE_WAKEUP) != 0U) {
            USBH_Notify_Lib (ctrl, port, 255U, USBH_NOTIFY_REMOTE_WAKEUP);
          }
          if ((events & ARM_USBH_EVENT_CORE_DO_RECOVERY) != 0U) {
            ptr_hci->port_lock = port + 1U;
            USBH_Engine (ctrl, port, ARM_USBH_EVENT_CORE_DO_RECOVERY);
          }
        }
      }
    } else {
      for (port = 0U; port < 15U; port++) {
        if ((port_mask >> port) == 0U) {
          break;
        }
        port_state = USBH_DriverPortGetState (ctrl, port);
        if ((polling_mask & 1U) != 0U) {
          if ((port_state.connected != 0U) && (((ptr_hci->port_con >> port) & 1U) == 0U)) {
            // If port current status is connected and was inactive => start debounce
            port_debounce |= (uint16_t)(1UL << port);
          }
        }
        if ((polling_mask & (1UL << 1)) != 0U) {
          if ((port_state.connected != 0U) && (((ptr_hci->port_con >> port) & 1U) != 0U)) {
            // If port current status is not connected and was active => send disconnect event
            USBH_Engine     (ctrl, port,       ARM_USBH_EVENT_DISCONNECT);
            USBH_Notify_Lib (ctrl, port, 255U, USBH_NOTIFY_DISCONNECT);
          }
        }
        if ((polling_mask & (1UL << 2)) != 0U) {
          if ((port_state.overcurrent != 0U) && (((ptr_hci->port_oc >> port) & 1U) == 0U)) {
            ptr_hci->port_oc |=  (uint16_t)(1UL << port);
            // If port overcurrent is now active and not active => send overcurrent event
            (void)USBH_DriverPortVbusOnOff (ctrl, port, false);
            USBH_Engine                    (ctrl, port,       ARM_USBH_EVENT_OVERCURRENT);
            USBH_Notify_Lib                (ctrl, port, 255U, USBH_NOTIFY_OVERCURRENT);
          } else {
            if ((port_state.overcurrent == 0U) && (((ptr_hci->port_oc >> port) & 1U) != 0U)) {
              ptr_hci->port_oc &= (uint16_t)(~(1UL << port));
            }
          }
        }
      }
    }
    if (port_debounce != 0U) {
      start_timer = 0U;
      if (ptr_hci->port_debounce == 0U) {
        start_timer = 1U;
      }
      ptr_hci->debounce_restart  = 1U;
      ptr_hci->port_debounce    |= port_debounce;
      if (start_timer != 0U) {
        (void)USBH_TimerStart (usbh_debounce_timer_id[ctrl], 12U);
      }
    }
  }
}

void USBH0_Core_Thread (void const *arg) { (void)arg; USBH_Core_Thread ((void *)0U); }
void USBH1_Core_Thread (void const *arg) { (void)arg; USBH_Core_Thread ((void *)1U); }
void USBH2_Core_Thread (void const *arg) { (void)arg; USBH_Core_Thread ((void *)2U); }
void USBH3_Core_Thread (void const *arg) { (void)arg; USBH_Core_Thread ((void *)3U); }
