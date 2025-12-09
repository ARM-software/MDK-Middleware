/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_core.c
 * Purpose: USB Device - Core module
 *----------------------------------------------------------------------------*/

#include "usbd_lib_core.h"

#include <stddef.h>
#include <string.h>

#include "usb_lib_debug.h"
#include "usb_lib_ver.h"
#include "usbd_lib_config_def.h"
#include "usbd_lib_drv.h"
#include "usbd_lib_core_device.h"
#include "usbd_lib_core_custom_class.h"
#include "usbd_lib_core_adc.h"
#include "usbd_lib_core_cdc.h"
#include "usbd_lib_core_hid.h"
#include "usbd_lib_core_msc.h"
#include "usbd_evr.h"
#include "usbd_cmsis_rtos2.h"

/************************** Constant definitions ******************************/


/************************** Macros ********************************************/


/************************** Global variable and memory definitions ************/


/************************** Local function declarations ***********************/

static uint8_t   EP_IDX                     (uint8_t ep_addr);
static uint32_t  EP_MSK                     (uint8_t ep_addr);
static usbStatus USBD_CheckDevice           (uint8_t device);
static usbStatus USBD_ConfigureEndpoint0    (uint8_t device);
static void      USBD_ResetCore             (uint8_t device);
static void      USBD_SwitchToHighSpeedCore (uint8_t device);
static usbStatus USBD_SetupStage            (uint8_t device);
static bool      USBD_ReqGetStatus          (uint8_t device);
static bool      USBD_ReqSetClrFeature      (uint8_t device, bool sc);
static bool      USBD_ReqSetAddress         (uint8_t device);
static bool      USBD_ReqGetDescriptor      (uint8_t device);
static bool      USBD_ReqGetMSDescriptor    (uint8_t device);
static bool      USBD_ReqGetConfiguration   (uint8_t device);
static bool      USBD_ReqSetConfiguration   (uint8_t device);
static bool      USBD_ReqGetInterface       (uint8_t device);
static bool      USBD_ReqSetInterface       (uint8_t device);
static void      USBD_Endpoint0             (uint8_t device, uint32_t event);
static void      USBD_Core                  (uint8_t device, uint32_t event);


/************************** User accessible functions *************************/

/// \brief Get version of USB Device stack
/// \return                             version (major.minor.revision : mmnnnrrrr decimal)
uint32_t USBD_GetVersion (void) {
  return usb_lib_version;
}

/// \brief Initialize USB Device stack and controller
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_Initialize (uint8_t device) {
  usbd_data_t *ptr_dev_data;
  usbStatus    status;
  uint8_t      i, sem_idx;

#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  USBD_Debug_Initialize(device);
#endif

  EvrUSBD_Core_Initialize(device);

  /* Make a version check just to keep the usb_lib_version variable for Component Viewer */
  if (usb_lib_version == 0U) {
    status = usbUnknownError;
    goto exit;
  }

  status = USBD_CheckDevice (device);
  if (status != usbOK) {
    goto exit;
  }

  ptr_dev_data = usbd_dev_ptr[device]->data_ptr;
  if (ptr_dev_data->init != 0U) {
    return usbOK;
  }

  usbd_drv_version[device]  = USBD_DriverGetVersion      (device);
  usbd_capabilities[device] = USBD_DriverGetCapabilities (device);

  memset((void *)ptr_dev_data, 0, sizeof(usbd_data_t));

  if (usbd_driver_semaphore_id[device] == NULL) {
    usbd_driver_semaphore_id[device] = USBD_SemaphoreCreate (usbdSemaphoreCore, device, 0U);
    if (usbd_driver_semaphore_id[device] == NULL) {
      status = usbSemaphoreError;
      goto cleanup_and_exit;
    }
  }

  for (i = 0U; i < (usbd_ep_max_num * 2U); i++) {
    sem_idx = (uint8_t)(device * usbd_ep_max_num * 2U) + i;
    if (usbd_driver_ep_semaphore_id[sem_idx] == NULL) {
      usbd_driver_ep_semaphore_id[sem_idx] = USBD_SemaphoreCreate (usbdSemaphoreEndpoint, device, i);
      if (usbd_driver_ep_semaphore_id[sem_idx] == NULL) {
        status = usbSemaphoreError;
        goto cleanup_and_exit;
      }
    }
  }

  if ((usbd_desc_ptr[device]->ser_num_string_descriptor != NULL) && 
      (usbd_dev_ptr[device]->ser_num_str_len != 0U)) {
    memset((void *)usbd_desc_ptr[device]->ser_num_string_descriptor, 0, usbd_dev_ptr[device]->ser_num_str_len);
  }

  if (fpUSBD_Device_Initialize[device] != NULL) {
    EvrUSBD_Core_OnInitialize(device);
    fpUSBD_Device_Initialize [device] ();
  }

  status = USBD_ClassInitialize (device);
  if (status != usbOK) {
    goto cleanup_and_exit;
  }

  if (usbd_core_thread_id[device] == NULL) {
    usbd_core_thread_id[device] = USBD_ThreadCreate (usbdThreadCore, device);
    if (usbd_core_thread_id[device] == NULL) {
      status = usbThreadError;
      goto cleanup_and_exit;
    }
  }

  status = USBD_DriverInitialize (device, *usbd_cb_device_event_ptr[device], *usbd_cb_endpoint_event_ptr[device]);
  if (status != usbOK) {
    goto cleanup_and_exit;
  }

  status = USBD_DriverPowerControl (device, ARM_POWER_FULL);

cleanup_and_exit:
  if (status == usbOK) {
    ptr_dev_data->init = 1U;
  } else {
    if (usbd_core_thread_id[device] != NULL) {
      (void)USBD_ThreadTerminate (usbd_core_thread_id[device]);
      usbd_core_thread_id[device] = NULL;
    }
    USBD_ClassUninitialize (device);
    if (fpUSBD_Device_Uninitialize[device] != NULL) {
      EvrUSBD_Core_OnUninitialize(device);
      fpUSBD_Device_Uninitialize [device] ();
    }
    for (i = 0U; i < (usbd_ep_max_num * 2U); i++) {
      sem_idx = (uint8_t)(device * usbd_ep_max_num * 2U) + i;
      if (usbd_driver_ep_semaphore_id[sem_idx] != NULL) {
        USBD_SemaphoreDelete (usbd_driver_ep_semaphore_id[sem_idx]);
        usbd_driver_ep_semaphore_id[sem_idx] = NULL;
      }
    }
    if (usbd_driver_semaphore_id[device] != NULL) {
      USBD_SemaphoreDelete (usbd_driver_semaphore_id[device]);
      usbd_driver_semaphore_id[device] = NULL;
    }
  }

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_Core_InitializeFailed(device, status);
  }
#endif

  return status;
}

/// \brief De-initialize USB Device stack and controller
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_Uninitialize (uint8_t device) {
  usbd_data_t *ptr_dev_data;
  uint8_t      ep_num, i, sem_idx;
  usbStatus    status, first_err_status;

  EvrUSBD_Core_Uninitialize(device);

  first_err_status = USBD_CheckDevice (device);
  if (first_err_status != usbOK) {
    goto exit;
  }

  ptr_dev_data = usbd_dev_ptr[device]->data_ptr;
  if (ptr_dev_data->init == 0U) {
    return usbOK;
  }

  for (ep_num = 0U; ep_num < 16U; ep_num++) {
    if ((ptr_dev_data->endpoint_mask & (1UL << ep_num)) != 0U) {
      USBD_EndpointStop (device, USB_ENDPOINT_OUT(ep_num));
      if (ptr_dev_data->endpoint_active[ep_num] != 0U) {
        status = USBD_DriverEndpointTransferAbort (device, USB_ENDPOINT_OUT(ep_num));
        if (status == usbOK) {
          ptr_dev_data->endpoint_active[ep_num] = 0U;
        } else {
          if (first_err_status == usbOK) {
            first_err_status = status;
          }
        }
      }
      status = USBD_DriverEndpointUnconfigure (device, USB_ENDPOINT_OUT(ep_num));
      if (first_err_status == usbOK) {
        first_err_status = status;
      }
    }
    if ((ptr_dev_data->endpoint_mask & ((1UL << 16) << ep_num)) != 0U) {
      USBD_EndpointStop (device, USB_ENDPOINT_IN(ep_num));
      if (ptr_dev_data->endpoint_active[ep_num+16U] != 0U) {
        status = USBD_DriverEndpointTransferAbort (device, USB_ENDPOINT_IN(ep_num));
        if (status == usbOK) {
          ptr_dev_data->endpoint_active[ep_num+16U] = 0U;
        } else {
          if (first_err_status == usbOK) {
            first_err_status = status;
          }
        }
      }
      status = USBD_DriverEndpointUnconfigure (device, USB_ENDPOINT_IN(ep_num));
      if (first_err_status == usbOK) {
        first_err_status = status;
      }
    }
  }

  status = USBD_DriverPowerControl (device, ARM_POWER_OFF);
  if (first_err_status == usbOK) {
    first_err_status = status;
  }

  status = USBD_DriverUninitialize (device);
  if (first_err_status == usbOK) {
    first_err_status = status;
  }

  if (usbd_core_thread_id[device] != NULL) {
    if (USBD_ThreadTerminate (usbd_core_thread_id[device]) == 0) {
      usbd_core_thread_id[device] = NULL;
    } else {
      if (first_err_status == usbOK) {
        first_err_status = usbThreadError;
      }
    }
  }

  status = USBD_ClassUninitialize (device);
  if (first_err_status == usbOK) {
    first_err_status = status;
  }

  if (fpUSBD_Device_Uninitialize[device] != NULL) {
    EvrUSBD_Core_OnUninitialize(device);
    fpUSBD_Device_Uninitialize [device] ();
  }

  for (i = 0U; i < (usbd_ep_max_num * 2U); i++) {
    sem_idx = (uint8_t)(device * usbd_ep_max_num * 2U) + i;
    if (usbd_driver_ep_semaphore_id[sem_idx] != NULL) {
      if (USBD_SemaphoreDelete (usbd_driver_ep_semaphore_id[sem_idx]) == 0) {
        usbd_driver_ep_semaphore_id[sem_idx] = NULL;
      } else {
        if (first_err_status == usbOK) {
          first_err_status = usbSemaphoreError;
        }
      }
    }
  }
  if (usbd_driver_semaphore_id[device] != NULL) {
    if (USBD_SemaphoreDelete (usbd_driver_semaphore_id[device]) == 0) {
      usbd_driver_semaphore_id[device] = NULL;
    } else {
      if (first_err_status == usbOK) {
        first_err_status = usbSemaphoreError;
      }
    }
  }

  // Clear all runtime information
  memset((void *)ptr_dev_data, 0, sizeof(usbd_data_t));

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (first_err_status != usbOK) {
    EvrUSBD_Core_UninitializeFailed(device, first_err_status);
  }
#endif

  return first_err_status;
}

/// \brief Activate pull-up on D+ or D- line to signal USB Device connection on USB Bus
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_Connect (uint8_t device) {
  usbStatus status;

  EvrUSBD_Core_Connect(device);

  status = USBD_CheckDevice (device);
  if (status == usbOK) {
    status = USBD_DriverDeviceConnect (device);
  }

#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_Core_ConnectFailed(device, status);
  }
#endif

  return status;
}

/// \brief Disconnect USB Device from USB Bus
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_Disconnect (uint8_t device) {
  usbStatus status;

  EvrUSBD_Core_Disconnect(device);

  status = USBD_CheckDevice (device);
  if (status == usbOK) {
    status = USBD_DriverDeviceDisconnect (device);
  }

#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_Core_DisconnectFailed(device, status);
  }
#endif

  return status;
}

/// \brief Retrieve USB Device configuration status
/// \param[in]     device               index of USB Device.
/// \return                             device is is in configured state and ready to communicate or is not configured and not ready to communicate :
///                                       - value true :  device is in configured state and ready to communicate
///                                       - value false : device is not configured and not ready to communicate
bool USBD_Configured (uint8_t device) {
  bool configured;

  configured = false;
  if (USBD_CheckDevice (device) == usbOK) {
    if (usbd_dev_ptr[device]->data_ptr->configuration != 0U) {
      configured = true;
    }
  }

  EvrUSBD_Core_Configured(device, configured);

  return configured;
}

/// \brief Retrieve USB Device state
/// \param[in]     device               index of USB Device.
/// \return                             structure containing device state information.
USBD_STATE USBD_GetState (uint8_t device) {
  USBD_STATE state;

  if (USBD_CheckDevice (device) == usbOK) {
    state = USBD_DriverDeviceGetState(device);
  } else {
    state.vbus     = 0U;
    state.speed    = 0U;
    state.active   = 0U;
    state.reserved = 0U;
  }

  EvrUSBD_Core_GetState(device, state);

  return state;
}

/// \brief Set USB Device serial number string
/// \param[in]     device               index of USB Device.
/// \param[in]     string               serial number string.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_SetSerialNumber (uint8_t device, const char *string) {
  const usbd_dev_t            *ptr_dev_cfg;
        USB_STRING_DESCRIPTOR *ptr_str_desc;
        uint8_t               *ptr_str_desc_str;
        uint32_t               str_len, i;
        usbStatus              status;

  EvrUSBD_Core_SetSerialNumber(device);

  status = USBD_CheckDevice (device);
  if (status != usbOK) {
    goto exit;
  }

  // Check all necessary pointers
  ptr_dev_cfg  =  usbd_dev_ptr[device];   if (ptr_dev_cfg  == NULL) { status = usbDeviceError;  goto exit; }
  ptr_str_desc =  (USB_STRING_DESCRIPTOR *)usbd_desc_ptr[device]->ser_num_string_descriptor;
                                          if (ptr_str_desc == NULL) { status = usbUnknownError; goto exit; }

  str_len = strlen(string);
  if (str_len > ptr_dev_cfg->ser_num_str_len) {
    str_len = ptr_dev_cfg->ser_num_str_len;
  }

  ptr_str_desc_str = (uint8_t *)ptr_str_desc + offsetof(USB_STRING_DESCRIPTOR, bString);

  ptr_str_desc->bLength         = (uint8_t)((str_len * 2U) + 2U);
  ptr_str_desc->bDescriptorType = USB_STRING_DESCRIPTOR_TYPE;
  for (i = 0U; i < str_len; i++) {
    *ptr_str_desc_str = string[i];
     ptr_str_desc_str += 2;
  }

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_Core_SetSerialNumberFailed(device, status);
  }
#endif

  return status;
}

/// \brief Start reception on Endpoint
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \param[out]    buf                  buffer that receives data.
/// \param[in]     len                  maximum number of bytes to receive.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_EndpointRead (uint8_t device, uint8_t ep_addr, uint8_t *buf, uint32_t len) {
  usbStatus status;

  EvrUSBD_Core_EndpointRead(device, ep_addr, len);

  status = USBD_CheckDevice (device);
  if (status == usbOK) {
    status = USBD_DriverEndpointTransfer (device, USB_ENDPOINT_OUT(ep_addr), buf, len);
  }

#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_Core_EndpointReadFailed(device, ep_addr, status);
  }
#endif

  return status;
}

/// \brief Get result of read operation on Endpoint
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \return                             number of bytes received.
uint32_t USBD_EndpointReadGetResult (uint8_t device, uint8_t ep_addr) {
  uint32_t result;

  if (USBD_CheckDevice (device) == usbOK) {
    result = USBD_DriverEndpointTransferGetResult (device, USB_ENDPOINT_OUT(ep_addr));
  } else {
    result = 0U;
  }

  EvrUSBD_Core_EndpointReadGetResult(device, ep_addr, result);

  return result;
}

/// \brief Start write on Endpoint
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \param[in]     buf                  buffer containing data bytes to write.
/// \param[in]     len                  number of bytes to write.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_EndpointWrite (uint8_t device, uint8_t ep_addr, const uint8_t *buf, uint32_t len) {
  usbStatus status;

  EvrUSBD_Core_EndpointWrite(device, ep_addr, len);

  status = USBD_CheckDevice (device);
  if (status == usbOK) {
    status = USBD_DriverEndpointTransfer (device, USB_ENDPOINT_IN(ep_addr), (uint8_t *)((uint32_t)buf), len);
  }

#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_Core_EndpointWriteFailed(device, ep_addr, status);
  }
#endif

  return status;
}

/// \brief Get result of write operation on Endpoint
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \return                             number of bytes written.
uint32_t USBD_EndpointWriteGetResult (uint8_t device, uint8_t ep_addr) {
  uint32_t result;

  if (USBD_CheckDevice (device) == usbOK) {
    result = USBD_DriverEndpointTransferGetResult (device, USB_ENDPOINT_IN(ep_addr));
  } else {
    result = 0U;
  }

  EvrUSBD_Core_EndpointWriteGetResult(device, ep_addr, result);

  return result;
}

/// \brief Set/Clear stall on Endpoint
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \param[in]     stall                operation :
///                                       - value false : clear stall
///                                       - value true :  set stall
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_EndpointStall (uint8_t device, uint8_t ep_addr, bool stall) {
  usbd_data_t *ptr_dev_data;
  usbStatus    status;
  uint32_t     ep_idx;

  EvrUSBD_Core_EndpointStall(device, ep_addr, stall);

  status = USBD_CheckDevice (device);
  if (status != usbOK) {
    goto exit;
  }

  if ((ep_addr == 0x80U) || (ep_addr == 0x00U)) {
    // Endpoint 0 can not be stalled (neither stall set nor cleared)
    status = usbInvalidParameter;
    goto exit;
  }

  ptr_dev_data = usbd_dev_ptr[device]->data_ptr;
  ep_idx       = EP_IDX(ep_addr);

  if (ptr_dev_data->endpoint_active[ep_idx] != 0U) {
    status = USBD_DriverEndpointTransferAbort (device, ep_addr);
    if (status != usbOK) {
      goto exit;
    }
  }

  status = USBD_DriverEndpointStall (device, ep_addr, stall);

  if (status == usbOK) {
    if (stall == true) {
      ptr_dev_data->endpoint_halt |=   1UL << ep_idx;
    } else {
      ptr_dev_data->endpoint_halt &= ~(1UL << ep_idx);
    }
  }

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_Core_EndpointStallFailed(device, ep_addr, stall, status);
  }
#endif

  return status;
}

/// \brief Abort read/write operation on Endpoint
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_EndpointAbort (uint8_t device, uint8_t ep_addr) {
  usbStatus status;

  EvrUSBD_Core_EndpointAbort(device, ep_addr);

  status = USBD_CheckDevice (device);
  if (status == usbOK) {
    status = USBD_DriverEndpointTransferAbort (device, ep_addr);
  }

#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_Core_EndpointAbortFailed(device, ep_addr, status);
  }
#endif

  return status;
}


/************************** Library accessible functions **********************/

/// \brief USB Device Request - Data In Stage
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_DataInStage (uint8_t device) {
  const usbd_dev_t  *ptr_dev_cfg;
        usbd_data_t *ptr_dev_data;
        usbStatus    status;

  ptr_dev_cfg  =  usbd_dev_ptr[device];   if (ptr_dev_cfg  == NULL) { return usbDeviceError; }
  ptr_dev_data =  ptr_dev_cfg->data_ptr;  if (ptr_dev_data == NULL) { return usbDeviceError; }

  if (ptr_dev_data->ep0_data.cnt != 0U) {
    if (ptr_dev_data->ep0_data.cnt > (uint32_t)ptr_dev_data->setup_packet.wLength) {
      // Limit return data length to maximum requested data length
      ptr_dev_data->ep0_data.cnt = (uint32_t)ptr_dev_data->setup_packet.wLength;
    }
    if (((ptr_dev_data->ep0_data.cnt & ((uint32_t)ptr_dev_cfg->max_packet0 - 1U)) == 0U) && 
         (ptr_dev_data->ep0_data.cnt < (uint32_t)ptr_dev_data->setup_packet.wLength)) {
      // If length of data to send is multiple of max packet size and less then requested length then terminate transfer with ZLP
      ptr_dev_data->zlp = true;
    }
    status = USBD_DriverEndpointTransfer (device, 0x80U, ptr_dev_data->ep0_data.data, ptr_dev_data->ep0_data.cnt);
    if (status != usbOK) {
      return status;
    }
    ptr_dev_data->ep0_data.data += ptr_dev_data->ep0_data.cnt;
    ptr_dev_data->ep0_data.cnt   = 0U;
  } else if (ptr_dev_data->zlp != 0U) {
    status = USBD_DriverEndpointTransfer (device, 0x80U, NULL, 0U);
    if (status != usbOK) {
      return status;
    }
    ptr_dev_data->zlp = 0U;
  } else {
    status = usbDriverError;
  }

  return status;
}

/// \brief USB Device Request - Data Out Stage (send data)
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_DataOutStage (uint8_t device) {
  usbd_data_t *ptr_dev_data;
  usbStatus    status;

  ptr_dev_data = usbd_dev_ptr[device]->data_ptr;

  status = USBD_DriverEndpointTransfer (device, USB_ENDPOINT_OUT(0U), ptr_dev_data->ep0_data.data, ptr_dev_data->ep0_data.cnt);
  if (status != usbOK) {
    return status;
  }
  ptr_dev_data->ep0_data.data += ptr_dev_data->ep0_data.cnt;
  ptr_dev_data->ep0_data.cnt   = 0U;

  return status;
}

/// \brief USB Device Request - Status In Stage (send IN ZLP)
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_StatusInStage (uint8_t device) {

  return (USBD_DriverEndpointTransfer (device, 0x80U, NULL, 0U));
}

/// \brief USB Device Request - Status In Stage done (IN ZLP sent)
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_StatusInStageDone (uint8_t device) {

  if (USBD_DriverEndpointTransferGetResult (device, 0x80U) != 0U) {
    return usbDriverError;
  }

  return usbOK;
}

/// \brief USB Device Request - Status Out Stage (receive OUT ZLP)
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_StatusOutStage (uint8_t device) {

  return (USBD_DriverEndpointTransfer (device, USB_ENDPOINT_OUT(0U), NULL, 0U));
}

/// \brief USB Device Request - Status Out Stage done (OUT ZLP received)
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_StatusOutStageDone (uint8_t device) {

  if (USBD_DriverEndpointTransferGetResult (device, USB_ENDPOINT_OUT(0U)) != 0U) {
    return usbDriverError;
  }

  if (usbd_dev_ptr[device]->data_ptr->endpoint_active[16] != 0U) {      // If sending is in progress (IN Endpoint active)
    return (USBD_DriverEndpointTransferAbort (device, 0x80U));          // abort sending
  }

  return usbOK;
}


/************************** Local functions ***********************************/

/// \brief Get index of endpoint
/// \param[in]     ep_addr              endpoint address.
/// \return                             endpoint index.
static uint8_t  EP_IDX (uint8_t ep_addr) {
  return (((ep_addr) & 0x0FU) + (((ep_addr) & 0x80U) >> 3));
}

/// \brief Get mask of endpoint
/// \param[in]     ep_addr              endpoint address.
/// \return                             endpoint mask.
static uint32_t EP_MSK (uint8_t ep_addr) {
  return (1UL << (((ep_addr) & 0x0FU) + (((ep_addr) & 0x80U) >> 3)));
}

/// \brief Check device valid
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function.
static usbStatus USBD_CheckDevice (uint8_t device) {

  if (device >= usbd_dev_num)                 { return usbDeviceError; }
  if (usbd_dev_ptr[device]           == NULL) { return usbDeviceError; }
  if (usbd_dev_ptr[device]->data_ptr == NULL) { return usbDeviceError; }
  if (usbd_dev_ptr[device]->driver   == NULL) { return usbDeviceError; }

  return usbOK;
}

/// \brief Configure Endpoint 0 for communication
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBD_ConfigureEndpoint0 (uint8_t device) {
  const USB_ENDPOINT_DESCRIPTOR *ptr_ep_desc;
        usbStatus                status;

  EvrUSBD_Core_ConfigureEp0(device);

  ptr_ep_desc = (const USB_ENDPOINT_DESCRIPTOR *)(usbd_desc_ptr[device]->ep0_descriptor);

  status = USBD_DriverEndpointConfigure (device, USB_ENDPOINT_OUT(ptr_ep_desc->bEndpointAddress), ptr_ep_desc->bmAttributes & 3U, ptr_ep_desc->wMaxPacketSize);
  if (status != usbOK) { 
    EvrUSBD_Core_ConfigureEp0Failed(device, USB_ENDPOINT_OUT(ptr_ep_desc->bEndpointAddress), status);
    return status;
  }
  status = USBD_DriverEndpointConfigure (device, USB_ENDPOINT_IN(ptr_ep_desc->bEndpointAddress), ptr_ep_desc->bmAttributes & 3U, ptr_ep_desc->wMaxPacketSize);
  if (status != usbOK) {
    EvrUSBD_Core_ConfigureEp0Failed(device, USB_ENDPOINT_IN(ptr_ep_desc->bEndpointAddress), status);
    return status;
  }

  return status;
}

/// \brief Reset Core states and variables for USB Device
/// \param[in]     device               index of USB Device.
static void USBD_ResetCore (uint8_t device) {

  memset(&usbd_dev_ptr[device]->data_ptr->device_address, 0, sizeof(usbd_data_t) - 1);

  usbd_dev_ptr[device]->data_ptr->device_status = (uint16_t)(((uint32_t)usbd_dev_ptr[device]->bmattributes >> 6) & 1U);
}

/// \brief Switch to high-speed for USB Device
/// \param[in]     device               index of USB Device.
static void USBD_SwitchToHighSpeedCore (uint8_t device) {

  usbd_dev_ptr[device]->data_ptr->high_speed = 1U;
}

/// \brief USB Device Request - Setup Stage (read received Setup Packet)
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBD_SetupStage (uint8_t device) {
  usbStatus status;

  status = USBD_DriverReadSetupPacket (device, (uint8_t *)&usbd_dev_ptr[device]->data_ptr->setup_packet);

#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status == usbOK) {
    EvrUSBD_Core_ReadSetupPacket(device, (const void *)&usbd_dev_ptr[device]->data_ptr->setup_packet);
  } else {
    EvrUSBD_Core_ReadSetupPacketFailed(device, status);
  }
#endif

  return status;
}

/// \brief USB Device Request - Get Status
/// \param[in]     device               index of USB Device.
/// \return        true                 success
/// \return        false                fail
static bool USBD_ReqGetStatus (uint8_t device) {
  const usbd_dev_t     *ptr_dev_cfg;
        usbd_data_t    *ptr_dev_data;
        usbd_ep_data_t *ptr_ep0_data;
        uint8_t        *ptr_ep0_buf;
        uint32_t        ep_msk;
        uint8_t         ep_addr;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  =  usbd_dev_ptr[device];   if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data =  ptr_dev_cfg->data_ptr;  if (ptr_dev_data == NULL) { return false; }
  ptr_ep0_data = &ptr_dev_data->ep0_data;
  ptr_ep0_buf  =  ptr_dev_cfg->ep0_buf;   if (ptr_ep0_buf  == NULL) { return false; }

  // Check the request parameters
  if ((ptr_dev_data->setup_packet.wValue  != 0U) ||
      (ptr_dev_data->setup_packet.wLength != 2U)) {
    return false;
  }

  // Execute request
  switch (ptr_dev_data->setup_packet.bmRequestType.Recipient) {
    case USB_REQUEST_TO_DEVICE:
      if (ptr_dev_data->setup_packet.wIndex == 0U) {
        ptr_ep0_buf[0] = ptr_dev_data->device_status & 0xFFU;
        ptr_ep0_buf[1] = (ptr_dev_data->device_status >> 8) & 0xFFU;
        ptr_ep0_data->data = ptr_ep0_buf;
        ptr_ep0_data->cnt  = 2U;
        return true;
      }
      break;
    case USB_REQUEST_TO_INTERFACE:
      if ((ptr_dev_data->configuration != 0U) && (((uint8_t)ptr_dev_data->setup_packet.wIndex) < (ptr_dev_data->num_interfaces))) {
        ptr_ep0_buf[0] = 0U;
        ptr_ep0_buf[1] = 0U;
        ptr_ep0_data->data = ptr_ep0_buf;
        ptr_ep0_data->cnt  = 2U;
        return true;
      }
      break;
    case USB_REQUEST_TO_ENDPOINT:
      ep_addr = (uint8_t)ptr_dev_data->setup_packet.wIndex & 0x8FU;
      ep_msk  = EP_MSK(ep_addr);
      if (((ptr_dev_data->configuration != 0U) || ((ep_addr & 0x0FU) == 0U)) && ((ptr_dev_data->endpoint_mask & ep_msk) != 0U)) {
        ptr_ep0_buf[0] = ((ptr_dev_data->endpoint_halt & ep_msk) != 0U) ? 1U : 0U;
        ptr_ep0_buf[1] = 0U;
        ptr_ep0_data->data = ptr_ep0_buf;
        ptr_ep0_data->cnt  = 2U;
        return true;
      }
      break;
    default:                            // If requested recipient is not allowed -> fail
      break;
  }

  return false;
}

/// \brief USB Device Request - Set/Clear Feature
/// \param[in]     device               index of USB Device.
/// \param[in]     sc                   false = Clear, true = Set
/// \return        true                 success
/// \return        false                fail
static bool USBD_ReqSetClrFeature (uint8_t device, bool sc) {
  usbd_data_t *ptr_dev_data;
  uint32_t     ep_msk;
  uint8_t      ep_addr, ep_idx;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_data = usbd_dev_ptr[device]->data_ptr; if (ptr_dev_data == NULL) { return false; }

  // Check the request parameters
  if (ptr_dev_data->setup_packet.wLength != 0U) {
    return false;
  }

  // Execute request
  switch (ptr_dev_data->setup_packet.bmRequestType.Recipient) {
    case USB_REQUEST_TO_DEVICE:
      if (ptr_dev_data->setup_packet.wValue == USB_FEATURE_REMOTE_WAKEUP) {
        if (sc) {
          if (fpUSBD_Device_EnableRemoteWakeup[device] != NULL) {
            EvrUSBD_Core_OnEnableRemoteWakeup(device);
            fpUSBD_Device_EnableRemoteWakeup  [device] ();
          }
          ptr_dev_data->device_status |=  USB_GETSTATUS_REMOTE_WAKEUP;
        } else {
          if (fpUSBD_Device_DisableRemoteWakeup[device] != NULL) {
            EvrUSBD_Core_OnDisableRemoteWakeup(device);
            fpUSBD_Device_DisableRemoteWakeup  [device] ();
          }
          ptr_dev_data->device_status &= ~USB_GETSTATUS_REMOTE_WAKEUP;
        }
        return true;
      }
      break;
    case USB_REQUEST_TO_INTERFACE:
      break;
    case USB_REQUEST_TO_ENDPOINT:
      ep_addr = (uint8_t)ptr_dev_data->setup_packet.wIndex & 0x8FU;
      ep_idx  = EP_IDX(ep_addr);
      ep_msk  = 1UL << ep_idx;
      if ((ptr_dev_data->configuration != 0U) && ((ptr_dev_data->endpoint_mask & ep_msk) != 0U)) {
        if (ptr_dev_data->setup_packet.wValue == USB_FEATURE_ENDPOINT_STALL) {
          if ((ep_addr & 0x0FU) == 0U) {
            return false;                                               // Control Endpoint 0 can not be stalled
          }
          if (sc) {                                                     // Set HALT
            if (ptr_dev_data->endpoint_active[ep_idx] != 0U) {
              if (USBD_DriverEndpointTransferAbort (device, ep_addr) != usbOK) {
                return false;
              }
            }
            if (USBD_DriverEndpointStall (device, ep_addr, true) != usbOK) {
              return false;
            }
            ptr_dev_data->endpoint_halt |= ep_msk;
            USBD_EndpointStop (device, ep_addr);
          } else {                                                      // Clear HALT
            if ((ptr_dev_data->endpoint_halt & ptr_dev_data->endpoint_no_halt_clr & ep_msk) != 0U) {
               // If endpoint is halted and halt clear is disabled then endpoint should stay halted
              return true;
            }
            if (USBD_DriverEndpointStall (device, ep_addr, false) != usbOK) {
              return false;
            }
            ptr_dev_data->endpoint_halt &= ~ep_msk;
            USBD_Endpoint0_ClearFeatureStall_MSC (device, ep_addr);
            USBD_EndpointStart                   (device, ep_addr);
          }
        }
        return true;
      }
      break;
    default:                            // If requested recipient is not allowed -> fail
      break;
  }

  return false;
}

/// \brief USB Device Request - Set Address
/// \param[in]     device               index of USB Device.
/// \return        true                 success
/// \return        false                fail
static bool USBD_ReqSetAddress (uint8_t device) {
  usbd_data_t *ptr_dev_data;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_data = usbd_dev_ptr[device]->data_ptr; if (ptr_dev_data == NULL) { return false; }

  // Check the request parameters
  if ((ptr_dev_data->setup_packet.wIndex  != 0U) ||
      (ptr_dev_data->setup_packet.wLength != 0U)) {
    return false;
  }

  // Execute request
  if (ptr_dev_data->setup_packet.bmRequestType.Recipient == USB_REQUEST_TO_DEVICE) {
    ptr_dev_data->device_address = (uint8_t)(0x80U | ptr_dev_data->setup_packet.wValue);
    return true;
  }

  return false;
}

/// \brief USB Device Request - Get Descriptor
/// \param[in]     device               index of USB Device.
/// \return        true                 success
/// \return        false                fail
static bool USBD_ReqGetDescriptor (uint8_t device) {
         const usbd_dev_t  *ptr_dev_cfg;
               usbd_data_t *ptr_dev_data;
  static const uint8_t     *ptr_desc;
  static       uint32_t     len;
               uint32_t     i;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return false; }

  // Check the request parameters
  if (ptr_dev_data->setup_packet.wLength == 0U) {
    return false;
  }

  // Execute request
  switch (ptr_dev_data->setup_packet.bmRequestType.Recipient) {
    case USB_REQUEST_TO_DEVICE:
      switch (ptr_dev_data->setup_packet.wValue >> 8) {         // Descriptor Type in high byte
        case USB_DEVICE_DESCRIPTOR_TYPE:
          if ((ptr_dev_data->setup_packet.wValue & 0xFFU) != 0U) { return false; }
          ptr_dev_data->ep0_data.data = (uint8_t *)((uint32_t)usbd_desc_ptr[device]->device_descriptor);
          len = sizeof(USB_DEVICE_DESCRIPTOR);
          break;
        case USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE:
          if (ptr_dev_cfg->hs == 0U) {                          // If high speed is not enabled
            return false;
          }
          if (ptr_dev_data->high_speed == 0U) {                 // If current device speed is not high-speed
            ptr_dev_data->ep0_data.data = (uint8_t *)((uint32_t)usbd_desc_ptr[device]->device_qualifier_fs);
          } else {                                              // else if current device speed is high-speed
            ptr_dev_data->ep0_data.data = (uint8_t *)((uint32_t)usbd_desc_ptr[device]->device_qualifier_hs);
          }
          len = sizeof(USB_DEVICE_QUALIFIER_DESCRIPTOR);
          break;
        case USB_CONFIGURATION_DESCRIPTOR_TYPE:
          if ((ptr_dev_cfg->hs == 0U) &&                        // If high speed is not enabled and
              (ptr_dev_data->high_speed != 0U)) {               // current device speed is high-speed
            return false;
          }
          if (ptr_dev_data->high_speed == 0U) {                 // If current device speed is not high-speed
            ptr_desc = usbd_desc_ptr[device]->config_descriptor_fs;
          } else {                                              // else if current device speed is high-speed
            ptr_desc = usbd_desc_ptr[device]->config_descriptor_hs;
          }
          for (i = 0U; i != ((uint32_t)ptr_dev_data->setup_packet.wValue & 0xFFU); i++) {
            if (((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->bLength != 0U) {
              ptr_desc += ((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->wTotalLength;
            }
          }
          if (((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->bLength == 0U) {
            return false;
          }
          ptr_dev_data->ep0_data.data = (uint8_t *)((uint32_t)ptr_desc);
          len = ((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->wTotalLength;
          break;
        case USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE:
          if (ptr_dev_cfg->hs == 0U) {                          // If high speed is not enabled
            return false;
          }
          if (ptr_dev_data->high_speed == 0U) {                 // If current device speed is not high-speed
            ptr_desc = usbd_desc_ptr[device]->other_speed_config_descriptor_fs;
          } else {                                              // else if current device speed is high-speed
            ptr_desc = usbd_desc_ptr[device]->other_speed_config_descriptor_hs;
          }
          for (i = 0U; i != ((uint32_t)ptr_dev_data->setup_packet.wValue & 0xFFU); i++) {
            if (((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->bLength != 0U) {
              ptr_desc += ((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->wTotalLength;
            }
          }
          if (((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->bLength == 0U) {
            return false;
          }
          ptr_dev_data->ep0_data.data = (uint8_t *)((uint32_t)ptr_desc);
          len = ((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->wTotalLength;
          break;
        case USB_STRING_DESCRIPTOR_TYPE:
          if ((((uint32_t)ptr_dev_data->setup_packet.wValue & 0xFFU) == 3) &&
                (usbd_desc_ptr[device]->ser_num_string_descriptor != NULL) &&
                (((const USB_STRING_DESCRIPTOR *)usbd_desc_ptr[device]->ser_num_string_descriptor)->bLength != 0U)) {
            // If requested descriptor is Serial Number String Descriptor and 
            // if it was set using USBD_SetSerialNumber function
            ptr_desc = usbd_desc_ptr[device]->ser_num_string_descriptor;
          } else if ((((uint32_t)ptr_dev_data->setup_packet.wValue & 0xFFU) == 0xEEU) &&
                       (usbd_desc_ptr[device]->ms_os_string_descriptor != NULL)) {
            // If requested descriptor is Microsoft OS String Descriptor and 
            // if it was created by configuration
            ptr_desc = usbd_desc_ptr[device]->ms_os_string_descriptor;
          } else {
            ptr_desc = usbd_desc_ptr[device]->string_descriptor;
            for (i = 0U; i != ((uint32_t)ptr_dev_data->setup_packet.wValue & 0xFFU); i++) {
              if (((const USB_STRING_DESCRIPTOR *)ptr_desc)->bLength != 0U) {
                ptr_desc += ((const USB_STRING_DESCRIPTOR *)ptr_desc)->bLength;
              }
            }
          }
          if (((USB_STRING_DESCRIPTOR *)((uint32_t)ptr_desc))->bLength == 0U) {
            return false;
          }
          ptr_dev_data->ep0_data.data = (uint8_t *)((uint32_t)ptr_desc);
          len = ((const USB_STRING_DESCRIPTOR *)((uint32_t)ptr_desc))->bLength;
          break;
        default:
          return false;
      }
      break;
    case USB_REQUEST_TO_INTERFACE:
      if (USBD_Endpoint0_ReqGetDescriptor_HID (device, &ptr_desc, &len) == false) {
        return false;
      }
      ptr_dev_data->ep0_data.data = (uint8_t *)((uint32_t)ptr_desc);
      break;
    default:
      return false;
  }

  ptr_dev_data->ep0_data.cnt = len;

  return true;
}

/// \brief USB Device Request - Get Microsoft (Vendor) Descriptor
/// \param[in]     device               index of USB Device.
/// \return        true                 success
/// \return        false                fail
static bool USBD_ReqGetMSDescriptor (uint8_t device) {
         const usbd_dev_t  *ptr_dev_cfg;
               usbd_data_t *ptr_dev_data;
  static const uint8_t     *ptr_desc;
  static       uint32_t     len;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return false; }

  // Check the request parameters
  if (ptr_dev_data->setup_packet.bRequest != ptr_dev_cfg->vendor_code) {
    // If vendor code different then configured one
    return false;
  }
  if ((ptr_dev_data->setup_packet.wValue & 0xFF00U) != 0U) {
    // If page number is different than 0
    return false;
  }
  if (ptr_dev_data->setup_packet.wLength == 0U) {
    return false;
  }

  // Execute request
  switch (ptr_dev_data->setup_packet.bmRequestType.Recipient) {
    case USB_REQUEST_TO_DEVICE:
      switch (ptr_dev_data->setup_packet.wIndex) {
        case USB_MS_OS_FEAT_EXT_COMPAT_ID_DESCRIPTOR_IDX:                                        // Is Extended Compat ID Descriptor request
          if (usbd_desc_ptr[device]->ms_os_ext_compat_id_descriptor != NULL) {                   // Extended Compat ID Descriptor pointer is valid
            ptr_desc  = usbd_desc_ptr[device]->ms_os_ext_compat_id_descriptor;
            ptr_dev_data->ep0_data.data = (uint8_t *)((uint32_t)ptr_desc);
            len = ((const USB_MS_OS_FEAT_EXT_COMPAT_ID_HEADER *)((uint32_t)ptr_desc))->dwLength;
          }
          break;
        case USB_MS_OS_FEAT_EXT_PROP_DESCRIPTOR_IDX:                                             // Is Extended Properties OS Descriptor request
          if (USBD_Endpoint0_ReqGetExtProp_CC (device, &ptr_desc) == false) {
            return false;
          }
          ptr_dev_data->ep0_data.data = (uint8_t *)((uint32_t)ptr_desc);
          len = ((const USB_MS_OS_FEAT_EXT_PROP_HEADER *)((uint32_t)ptr_desc))->dwLength;
          break;
        default:                                                                                 // Unhandled vendor request
          return false;
      }
      break;
    case USB_REQUEST_TO_INTERFACE:
      if (ptr_dev_data->setup_packet.wIndex == USB_MS_OS_FEAT_EXT_PROP_DESCRIPTOR_IDX) {         // Is Extended Properties OS Descriptor request
        if (USBD_Endpoint0_ReqGetExtProp_CC (device, &ptr_desc) == false) {
          return false;
        }
        ptr_dev_data->ep0_data.data = (uint8_t *)((uint32_t)ptr_desc);
        len = ((const USB_MS_OS_FEAT_EXT_PROP_HEADER *)((uint32_t)ptr_desc))->dwLength;
      }
      break;
    default:
      return false;
  }

  ptr_dev_data->ep0_data.cnt = len;

  return true;
}

/// \brief USB Device Request - Get Configuration
/// \param[in]     device               index of USB Device.
/// \return        true                 success
/// \return        false                fail
static bool USBD_ReqGetConfiguration (uint8_t device) {
  const usbd_dev_t  *ptr_dev_cfg;
        usbd_data_t *ptr_dev_data;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return false; }

  // Check the request parameters
  if ((ptr_dev_data->setup_packet.wValue  != 0U) ||
      (ptr_dev_data->setup_packet.wIndex  != 0U) ||
      (ptr_dev_data->setup_packet.wLength != 1U)) {
    return false;
  }

  // Execute request
  if (ptr_dev_data->setup_packet.bmRequestType.Recipient == USB_REQUEST_TO_DEVICE) {
    ptr_dev_cfg->ep0_buf[0]     = ptr_dev_data->configuration;
    ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;
    ptr_dev_data->ep0_data.cnt  = 1U;
    return true;
  }

  return false;
}

/// \brief USB Device Request - Set Configuration
/// \param[in]     device               index of USB Device.
/// \return        true                 success
/// \return        false                fail
static bool USBD_ReqSetConfiguration (uint8_t device) {
  const usbd_dev_t              *ptr_dev_cfg;
        usbd_data_t             *ptr_dev_data;
  const uint8_t                 *ptr_desc;
  const USB_ENDPOINT_DESCRIPTOR *ptr_ep_desc;
        uint32_t                 ep_msk, i;
        uint8_t                  ep_addr, ep_num, ep_idx, alt;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  =  usbd_dev_ptr[device];   if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data =  ptr_dev_cfg->data_ptr;  if (ptr_dev_data == NULL) { return false; }

  // Check the request parameters
  if ((ptr_dev_data->setup_packet.wIndex  != 0U) ||
      (ptr_dev_data->setup_packet.wLength != 0U)) {
    return false;
  }

  // Execute request
  alt = 0U;
  switch (ptr_dev_data->setup_packet.bmRequestType.Recipient) {
    case USB_REQUEST_TO_DEVICE:
      if ((ptr_dev_data->setup_packet.wValue & 0xFFU) != 0U) {
        if ((ptr_dev_cfg->hs == 0U) &&                          // If high speed is not enabled and
            (ptr_dev_data->high_speed != 0U)) {                 // current device speed is high-speed
          return false;
        }
        if (ptr_dev_data->high_speed == 0U) {                   // If current device speed is not high-speed
          ptr_desc = usbd_desc_ptr[device]->config_descriptor_fs;
        } else {                                                // else if current device speed is high-speed
          ptr_desc = usbd_desc_ptr[device]->config_descriptor_hs;
        }
        while (((USB_COMMON_DESCRIPTOR const *)ptr_desc)->bLength != 0U) {
          switch (((USB_COMMON_DESCRIPTOR const *)ptr_desc)->bDescriptorType) {
            case USB_CONFIGURATION_DESCRIPTOR_TYPE:
              if ((((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->bConfigurationValue) == ((uint8_t)(ptr_dev_data->setup_packet.wValue))) {
                if (((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->bNumInterfaces > ptr_dev_cfg->if_num) {
                  return false;
                }
                ptr_dev_data->configuration  = (uint8_t)(ptr_dev_data->setup_packet.wValue);
                ptr_dev_data->num_interfaces = ((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->bNumInterfaces;
                for (i = 0U; i < ptr_dev_cfg->if_num; i++) {
                  ptr_dev_cfg->alt_setting[i] = 0U;
                }
                for (ep_num = 1U; ep_num < 16U; ep_num++) {
                  if ((ptr_dev_data->endpoint_mask & (1UL << ep_num)) != 0U) {
                    if (ptr_dev_data->endpoint_active[ep_num] != 0U) {
                      if (USBD_DriverEndpointTransferAbort (device, USB_ENDPOINT_OUT(ep_num)) != usbOK) {
                        return false;
                      }
                    }
                    if (USBD_DriverEndpointUnconfigure (device, USB_ENDPOINT_OUT(ep_num)) != usbOK) {
                      return false;
                    }
                    USBD_EndpointStop (device, USB_ENDPOINT_OUT(ep_num));
                  }
                  if ((ptr_dev_data->endpoint_mask & ((1UL << 16) << ep_num)) != 0U) {
                    if (ptr_dev_data->endpoint_active[ep_num+16U] != 0U) {
                      if (USBD_DriverEndpointTransferAbort (device, USB_ENDPOINT_IN(ep_num)) != usbOK) {
                        return false;
                      }
                    }
                    if (USBD_DriverEndpointUnconfigure (device, USB_ENDPOINT_IN(ep_num)) != usbOK) {
                      return false;
                    }
                    USBD_EndpointStop (device, USB_ENDPOINT_IN(ep_num));
                  }
                }
                ptr_dev_data->endpoint_mask        = 0x00010001U;
                ptr_dev_data->endpoint_halt        = 0x00000000U;
                ptr_dev_data->endpoint_no_halt_clr = 0x00000000U;
                if ((((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->bmAttributes & USB_CONFIG_POWERED_MASK) != 0U) {
                  ptr_dev_data->device_status |=  USB_GETSTATUS_SELF_POWERED;
                } else {
                  ptr_dev_data->device_status &= ~USB_GETSTATUS_SELF_POWERED;
                }
              } else {
                ptr_desc += ((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->wTotalLength;
                continue;
              }
              break;
            case USB_INTERFACE_DESCRIPTOR_TYPE:
              alt = ((const USB_INTERFACE_DESCRIPTOR *)ptr_desc)->bAlternateSetting;
              break;
            case USB_ENDPOINT_DESCRIPTOR_TYPE:
              if (alt == 0U) {
                ptr_ep_desc = (const USB_ENDPOINT_DESCRIPTOR *)ptr_desc;
                ep_addr = ptr_ep_desc->bEndpointAddress & 0x8FU;
                ep_idx  = EP_IDX(ep_addr);
                ep_msk  = 1UL << ep_idx;
                if (ptr_dev_data->endpoint_active[ep_idx] != 0U) {
                  if (USBD_DriverEndpointTransferAbort (device, ep_addr) != usbOK) {
                    return false;
                  }
                }
                ptr_dev_data->endpoint_mask |= ep_msk;
                if (USBD_DriverEndpointConfigure (device, ptr_ep_desc->bEndpointAddress, (uint8_t)ptr_ep_desc->bmAttributes & 3U, ptr_ep_desc->wMaxPacketSize) != usbOK) {
                  return false;
                }
                USBD_EndpointStart (device, ptr_ep_desc->bEndpointAddress);
              }
              break;
            default:                    // Unknown descriptor type -> skip it
              break;
          }
          ptr_desc += ((const USB_COMMON_DESCRIPTOR *)ptr_desc)->bLength;
        }
      } else {
        ptr_dev_data->configuration = 0U;
        for (ep_num = 1U; ep_num < 16U; ep_num++) {
          if ((ptr_dev_data->endpoint_mask & (1UL << ep_num)) != 0U) {
            if (ptr_dev_data->endpoint_active[ep_num] != 0U) {
              if (USBD_DriverEndpointTransferAbort (device, USB_ENDPOINT_OUT(ep_num)) != usbOK) {
                return false;
              }
            }
            if (USBD_DriverEndpointUnconfigure (device, USB_ENDPOINT_OUT(ep_num)) != usbOK) {
              return false;
            }
            USBD_EndpointStop (device, USB_ENDPOINT_OUT(ep_num));
          }
          if ((ptr_dev_data->endpoint_mask & ((1UL << 16) << ep_num)) != 0U) {
            if (ptr_dev_data->endpoint_active[ep_num+16U] != 0U) {
              if (USBD_DriverEndpointTransferAbort (device, USB_ENDPOINT_IN(ep_num)) != usbOK) {
                return false;
              }
            }
            if (USBD_DriverEndpointUnconfigure (device, USB_ENDPOINT_IN(ep_num)) != usbOK) {
              return false;
            }
            USBD_EndpointStop (device, USB_ENDPOINT_IN(ep_num));
          }
        }
        ptr_dev_data->endpoint_mask        = 0x00010001U;
        ptr_dev_data->endpoint_halt        = 0x00000000U;
        ptr_dev_data->endpoint_no_halt_clr = 0x00000000U;
      }
      if (ptr_dev_data->configuration != (uint8_t)(ptr_dev_data->setup_packet.wValue)) {
        return false;
      } else {
        USBD_SetConfiguration (device, ptr_dev_data->configuration);
        if (fpUSBD_Device_ConfigurationChanged[device] != NULL) {
          EvrUSBD_Core_OnConfigurationChanged(device, ptr_dev_data->configuration);
          fpUSBD_Device_ConfigurationChanged  [device] (ptr_dev_data->configuration);
        }
      }
      break;
    default:                            // Not allowed recipient other than DEVICE
      return false;
  }

  return true;
}

/// \brief USB Device Request - Get Interface
/// \param[in]     device               index of USB Device.
/// \return        true                 success
/// \return        false                fail
static bool USBD_ReqGetInterface (uint8_t device) {
  const usbd_dev_t  *ptr_dev_cfg;
        usbd_data_t *ptr_dev_data;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return false; }

  // Check the request parameters
  if ((ptr_dev_data->setup_packet.wValue  != 0U) ||
      (ptr_dev_data->setup_packet.wLength != 1U)) {
    return false;
  }

  // Execute request
  if (ptr_dev_data->setup_packet.bmRequestType.Recipient == USB_REQUEST_TO_INTERFACE) {
    if (ptr_dev_data->setup_packet.wIndex >= (uint16_t)ptr_dev_cfg->if_num) {
      return false;
    }
    ptr_dev_cfg->ep0_buf[0]     = ptr_dev_cfg->alt_setting[ptr_dev_data->setup_packet.wIndex];
    ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;
    ptr_dev_data->ep0_data.cnt  = 1U;
    return true;
  }

  return false;

}

/// \brief USB Device Request - Set Interface
/// \param[in]     device               index of USB Device.
/// \return        true                 success
/// \return        false                fail
static bool USBD_ReqSetInterface (uint8_t device) {
  const usbd_dev_t              *ptr_dev_cfg;
        usbd_data_t             *ptr_dev_data;
  const uint8_t                 *ptr_desc;
  const USB_ENDPOINT_DESCRIPTOR *ptr_ep_desc;
        uint32_t                 ep_msk, msk;
        uint8_t                  ep_addr, ep_idx, ifn, alt, old;
        bool                     set;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  =  usbd_dev_ptr[device];   if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data =  ptr_dev_cfg->data_ptr;  if (ptr_dev_data == NULL) { return false; }

  // Check the request parameters
  if (ptr_dev_data->setup_packet.wLength != 0U) {
    return false;
  }

  // Execute request
  msk = 0U;
  ifn = 0U;
  alt = 0U;
  old = 0U;
  set = false;
  switch (ptr_dev_data->setup_packet.bmRequestType.Recipient) {
    case USB_REQUEST_TO_INTERFACE:
      if (ptr_dev_data->configuration == 0U) {
        return false;
      }
      if ((ptr_dev_cfg->hs == 0U) &&                            // If high speed is not enabled and
          (ptr_dev_data->high_speed != 0U)) {                   // current device speed is high-speed
        return false;
      }
      if (ptr_dev_data->high_speed == 0U) {
        ptr_desc = usbd_desc_ptr[device]->config_descriptor_fs;
      } else {
        ptr_desc = usbd_desc_ptr[device]->config_descriptor_hs;
      }
      while (((const USB_COMMON_DESCRIPTOR *)ptr_desc)->bLength != 0U) {
        switch (((const USB_COMMON_DESCRIPTOR *)ptr_desc)->bDescriptorType) {
          case USB_CONFIGURATION_DESCRIPTOR_TYPE:
            if (((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->bConfigurationValue != ptr_dev_data->configuration) {
              ptr_desc += ((const USB_CONFIGURATION_DESCRIPTOR *)ptr_desc)->wTotalLength;
              continue;
            }
            break;
          case USB_INTERFACE_DESCRIPTOR_TYPE:
            if (ptr_dev_data->setup_packet.wIndex >= (uint16_t)ptr_dev_cfg->if_num) {
              return false;
            }
            ifn = ((const USB_INTERFACE_DESCRIPTOR *)ptr_desc)->bInterfaceNumber;
            alt = ((const USB_INTERFACE_DESCRIPTOR *)ptr_desc)->bAlternateSetting;
            msk = 0U;
            if (((uint16_t)ifn == ptr_dev_data->setup_packet.wIndex) && ((uint16_t)alt == ptr_dev_data->setup_packet.wValue)) {
              set = true;
              old = ptr_dev_cfg->alt_setting[ifn];
              ptr_dev_cfg->alt_setting[ifn] = alt;
            }
            break;
          case USB_ENDPOINT_DESCRIPTOR_TYPE:
            if ((uint16_t)ifn == ptr_dev_data->setup_packet.wIndex) {
              ptr_ep_desc = (const USB_ENDPOINT_DESCRIPTOR *)ptr_desc;
              ep_addr = ptr_ep_desc->bEndpointAddress & 0x8FU;
              ep_idx  = EP_IDX(ep_addr);
              ep_msk  = 1UL << ep_idx;
              if ((uint16_t)alt == ptr_dev_data->setup_packet.wValue) {
                ptr_dev_data->endpoint_mask |=  ep_msk;
                ptr_dev_data->endpoint_halt &= ~ep_msk;
                if (ptr_dev_data->endpoint_active[ep_idx] != 0U) {
                  if (USBD_DriverEndpointTransferAbort (device, ep_addr) != usbOK) {
                    return false;
                  }
                }
                if (USBD_DriverEndpointConfigure (device, ptr_ep_desc->bEndpointAddress, ptr_ep_desc->bmAttributes & 3U, ptr_ep_desc->wMaxPacketSize) != usbOK) {
                  return false;
                }
                USBD_EndpointStart (device, ep_addr);
                msk |= ep_msk;
              } else {
                if ((alt == old) && ((msk & ep_msk) == 0U)) {
                  ptr_dev_data->endpoint_mask &= ~ep_msk;
                  ptr_dev_data->endpoint_halt &= ~ep_msk;
                  if (ptr_dev_data->endpoint_active[ep_idx] != 0U) {
                    if (USBD_DriverEndpointTransferAbort (device, ep_addr) != usbOK) {
                      return false;
                    }
                  }
                  if (USBD_DriverEndpointUnconfigure (device, ep_addr) != usbOK) {
                    return false;
                  }
                  USBD_EndpointStop (device, ep_addr);
                }
              }
            }
           break;
         default:                       // Unknown descriptor type -> skip it
           break;
        }
        ptr_desc += ((const USB_COMMON_DESCRIPTOR *)ptr_desc)->bLength;
      }
      if (set) {
        ptr_dev_data->interface = ifn;
      }
      break;
    default:
      return false;
  }

  return set;
}

/// \brief USB Device - Endpoint 0 Event Callback called from USB Device Core Thread
/// \param[in]     device               index of USB Device.
/// \param[in]     event                USB Device event.
/// \return                             none.
static void USBD_Endpoint0 (uint8_t device, uint32_t event) {
  const usbd_dev_t  *ptr_dev_cfg;
        usbd_data_t *ptr_dev_data;

  if (device >= usbd_dev_num) { return; }               // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return; }

  if ((event & ARM_USBD_EVENT_IN) != 0U) {                                      // IN event processing
    if (ptr_dev_data->setup_packet.bmRequestType.Dir == USB_REQUEST_DEVICE_TO_HOST) {
      if ((ptr_dev_data->ep0_data.cnt != 0U) || (ptr_dev_data->zlp != 0U)) {
        if (USBD_DataInStage (device) == usbOK) {               // Send data
          goto in_ok;
        }
        goto stall_out;
      } else {                                                  // If data sending has completed
        if (ptr_dev_data->ext_handle != 0U) {
          ptr_dev_data->ext_handle = 0U;
          switch (USBD_Core_Endpoint0_InDataSent (device)) {
            case usbdRequestOK:
            case usbdRequestNotProcessed:
              (void)USBD_StatusOutStage (device);
              goto in_ok;
            case usbdRequestNAK:
            case usbdRequestStall:
              goto stall_out;
          }
        } else {
          goto in_ok;
        }
      }
stall_out:
      // Stall OUT direction, if IN data phase was ok this code will be skipped
      (void)USBD_DriverEndpointStall (device, 0U, true);
      ptr_dev_data->ep0_data.cnt = 0U;
    } else {
      if (USBD_StatusInStageDone (device) == usbOK) {           // Acknowledge was sent
        if ((ptr_dev_data->device_address & 0x80U) != 0U) {
          ptr_dev_data->device_address &= 0x7FU;
          (void)USBD_DriverDeviceSetAddress (device, ptr_dev_data->device_address);
        }
      }
    }
in_ok:
    ;
  }                                                                             // IN event processing end

  if ((event & ARM_USBD_EVENT_OUT) != 0U) {                                     // OUT event processing
    if (ptr_dev_data->setup_packet.bmRequestType.Dir == USB_REQUEST_HOST_TO_DEVICE) {
      if (ptr_dev_data->ep0_data.cnt != 0U) {                   // If there is more data to receive
        if (USBD_DataOutStage (device) == usbOK) {              // Receive data
          goto out_ok;
        }
      } else {                                                  // If data reception has completed
        if (ptr_dev_data->ext_handle != 0U) {
          ptr_dev_data->ext_handle = 0U;
          switch (USBD_Core_Endpoint0_OutDataReceived (device)) {
            case usbdRequestOK:
            case usbdRequestNotProcessed:
              (void)USBD_StatusInStage (device);
              goto out_ok;
            case usbdRequestNAK:
            case usbdRequestStall:
              goto stall_in;
          }
        }
        switch (ptr_dev_data->setup_packet.bmRequestType.Type) {
          case USB_REQUEST_STANDARD:
            goto stall_in;
          case USB_REQUEST_CLASS:
            switch (ptr_dev_data->setup_packet.bmRequestType.Recipient) {
              case USB_REQUEST_TO_DEVICE:
                goto stall_in;
              case USB_REQUEST_TO_INTERFACE:
                if (USBD_Endpoint0_Out_HID_ReqToIF (device)) {
                  goto out_ok;
                }
                if (USBD_Endpoint0_Out_ADC_ReqToIF (device)) {
                  goto out_ok;
                }
                if (USBD_Endpoint0_Out_CDC_ReqToIF (device)) {
                  goto out_ok;
                }
                goto stall_in;
              case USB_REQUEST_TO_ENDPOINT:
                if (USBD_Endpoint0_Out_ADC_ReqToEP (device)) {
                  goto out_ok;
                }
                goto stall_in;
              default:
                goto stall_in;
            }
          default:
            goto stall_in;                                      // Unknown request
        }
      }
stall_in:
      // Stall IN direction, if OUT data phase was ok this code will be skipped
      (void)USBD_DriverEndpointStall (device, 0x80U, true);
      ptr_dev_data->ep0_data.cnt = 0U;
    } else {
      (void)USBD_StatusOutStageDone(device);                    // Acknowledged sent data
    }
out_ok:
    ;
  }                                                                             // OUT event processing end

  if ((event & ARM_USBD_EVENT_SETUP) != 0U) {                                   // SETUP event processing
    if (USBD_SetupStage (device) != usbOK) {
      return;
    }
    switch (USBD_Core_Endpoint0_SetupPacketReceived (device)) {
      case usbdRequestOK:
        if ((ptr_dev_data->buf != NULL) || (ptr_dev_data->len == 0U)) {
          if (ptr_dev_data->setup_packet.wLength != 0U) {
            ptr_dev_data->ep0_data.data = ptr_dev_data->buf;    // Pointer to data buffer
            ptr_dev_data->ep0_data.cnt  = ptr_dev_data->len;    // Number of bytes to transfer
            ptr_dev_data->ext_handle    = 1U;
            if (ptr_dev_data->setup_packet.bmRequestType.Dir == USB_REQUEST_HOST_TO_DEVICE) {
              if (USBD_DataOutStage (device) != usbOK) {        // Receive data
                goto stall;
              }
            } else {
              if (USBD_DataInStage (device) != usbOK) {         // Send data
                goto stall;
              }
            }
          } else {
            if (ptr_dev_data->setup_packet.bmRequestType.Dir == USB_REQUEST_HOST_TO_DEVICE) {
              (void)USBD_StatusInStage  (device);               // Send status IN
            } else {
              (void)USBD_StatusOutStage (device);               // Send status OUT
            }
          }
        } else {
          goto stall;
        }
        return;
      case usbdRequestNotProcessed:
        ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;                     // Pointer to data buffer
        if (ptr_dev_data->setup_packet.wLength > ptr_dev_cfg->ep0_buf_size) {
          ptr_dev_data->ep0_data.cnt = ptr_dev_cfg->ep0_buf_size;               // Number of bytes to transfer, max size of control transfer buffer
        } else {
          ptr_dev_data->ep0_data.cnt = ptr_dev_data->setup_packet.wLength;      // Number of bytes to transfer
        }
        break;
      case usbdRequestNAK:
      case usbdRequestStall:
        goto stall;
    }
    switch (ptr_dev_data->setup_packet.bmRequestType.Type) {
      case USB_REQUEST_STANDARD:
        switch (ptr_dev_data->setup_packet.bRequest) {
          case USB_REQUEST_GET_STATUS:
            if (USBD_ReqGetStatus (device)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              if (USBD_DataInStage (device) == usbOK) {
                (void)USBD_StatusOutStage (device);
                goto setup_ok;
              }
            }
            EvrUSBD_Core_ReqGetStatusFailed(device);
            break;
          case USB_REQUEST_CLEAR_FEATURE:
            if (USBD_ReqSetClrFeature (device, false)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              (void)USBD_StatusInStage (device);
              goto setup_ok;
            }
            EvrUSBD_Core_ReqSetClrFeatureFailed(device);
            break;
          case USB_REQUEST_SET_FEATURE:
            if (USBD_ReqSetClrFeature (device, true)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              (void)USBD_StatusInStage (device);
              goto setup_ok;
            }
            EvrUSBD_Core_ReqSetClrFeatureFailed(device);
            break;
          case USB_REQUEST_SET_ADDRESS:
            if (USBD_ReqSetAddress (device)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              (void)USBD_StatusInStage (device);
              goto setup_ok;
            }
            EvrUSBD_Core_ReqSetAddressFailed(device);
            break;
          case USB_REQUEST_GET_DESCRIPTOR:
            if (USBD_ReqGetDescriptor (device)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              if (USBD_DataInStage (device) == usbOK) {
                (void)USBD_StatusOutStage (device);
                goto setup_ok;
              }
            }
            EvrUSBD_Core_ReqGetDescriptorFailed(device);
            break;
          case USB_REQUEST_SET_DESCRIPTOR:
            break;
          case USB_REQUEST_GET_CONFIGURATION:
            if (USBD_ReqGetConfiguration (device)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              if (USBD_DataInStage (device) == usbOK) {
                (void)USBD_StatusOutStage (device);
                goto setup_ok;
              }
            }
            EvrUSBD_Core_ReqGetConfigurationFailed(device);
            break;
          case USB_REQUEST_SET_CONFIGURATION:
            if (USBD_ReqSetConfiguration (device)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              (void)USBD_StatusInStage (device);
              goto setup_ok;
            }
            EvrUSBD_Core_ReqSetConfigurationFailed(device);
            break;
          case USB_REQUEST_GET_INTERFACE:
            if (USBD_ReqGetInterface (device)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              if (USBD_DataInStage (device) == usbOK) {
                (void)USBD_StatusOutStage (device);
                goto setup_ok;
              }
            }
            EvrUSBD_Core_ReqGetInterfaceFailed(device);
            break;
          case USB_REQUEST_SET_INTERFACE:
            if (USBD_ReqSetInterface (device)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              (void)USBD_StatusInStage(device);
              goto setup_ok;
            }
            EvrUSBD_Core_ReqSetInterfaceFailed(device);
            break;
          default:                      // Unexpected request
            break;
        }
        break;
      case USB_REQUEST_CLASS:
        switch (ptr_dev_data->setup_packet.bmRequestType.Recipient) {
          case USB_REQUEST_TO_DEVICE:
            break;
          case USB_REQUEST_TO_INTERFACE:
            if (USBD_Endpoint0_Setup_HID_ReqToIF (device)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              goto setup_ok;
            }
            if (USBD_Endpoint0_Setup_MSC_ReqToIF (device)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              goto setup_ok;
            }
            if (USBD_Endpoint0_Setup_ADC_ReqToIF (device)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              goto setup_ok;
            }
            if (USBD_Endpoint0_Setup_CDC_ReqToIF (device)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              goto setup_ok;
            }
            break;
          case USB_REQUEST_TO_ENDPOINT:
            if (USBD_Endpoint0_Setup_ADC_ReqToEP (device)) {
              USBD_Core_Endpoint0_SetupPacketProcessed (device);
              goto setup_ok;
            }
            break;
          default:                      // Unexpected recipient
            break;
        }
        break;
      case USB_REQUEST_VENDOR:
        if (USBD_ReqGetMSDescriptor (device)) {
          USBD_Core_Endpoint0_SetupPacketProcessed (device);
          if (USBD_DataInStage (device) == usbOK) {
            (void)USBD_StatusOutStage (device);
            goto setup_ok;
          }
        }
        EvrUSBD_Core_ReqGetMSDescriptorFailed(device);
        break;
      default:                          // Unexpected request type
        break;
    }
stall:
    (void)USBD_DriverEndpointStall (device, 0x00U, true);       // Stall OUT direction
    (void)USBD_DriverEndpointStall (device, 0x80U, true);       // Stall IN  direction
    ptr_dev_data->ep0_data.cnt = 0U;                            // Clear expected data phase data
    return;
setup_ok:                                                       // SETUP packet processed successfully
    if ((ptr_dev_data->setup_packet.bmRequestType.Dir == USB_REQUEST_HOST_TO_DEVICE) && (ptr_dev_data->setup_packet.wLength != 0U)) {
      if (USBD_DataOutStage (device) != usbOK) {                // Start read of incoming data
        (void)USBD_DriverEndpointStall (device, 0x80U, true);   // Stall status IN  direction
         ptr_dev_data->ep0_data.cnt = 0U;                       // Clear data phase length
      }
    }
  }                                                                             // SETUP event processing end
}

/// \brief USB Device Signal Device Event handling
/// \param[in]     device               index of USB Device.
/// \param[in]     event                device event.
/// \return                             none.
static void USBD_Core (uint8_t device, uint32_t event) {

  // Handle Port Events
  if ((event & ARM_USBD_EVENT_VBUS_ON) != 0U) {
    if (fpUSBD_Device_VbusChanged [device] != NULL) {
      EvrUSBD_Core_OnVbusChanged(device, true);
      fpUSBD_Device_VbusChanged   [device] (true);
    }
  }
  if ((event & ARM_USBD_EVENT_VBUS_OFF) != 0U) {
    if (fpUSBD_Device_VbusChanged [device] != NULL) {
      EvrUSBD_Core_OnVbusChanged(device, false);
      fpUSBD_Device_VbusChanged   [device] (false);
    }
    USBD_ResetCore                (device);
  }
  if ((event & ARM_USBD_EVENT_RESET) != 0U) {
    USBD_ResetCore                (device);
    (void)USBD_ConfigureEndpoint0 (device);
    USBD_Reset                    (device);
    if (fpUSBD_Device_Reset[device] != NULL) {
      EvrUSBD_Core_OnReset(device);
      fpUSBD_Device_Reset         [device] ();
    }
  }
  if ((event & ARM_USBD_EVENT_HIGH_SPEED) != 0U) {
    USBD_SwitchToHighSpeedCore    (device);
    if (fpUSBD_Device_HighSpeedActivated[device] != NULL) {
      EvrUSBD_Core_OnHighSpeedActivated(device);
      fpUSBD_Device_HighSpeedActivated  [device] ();
    }
  }
  if ((event & ARM_USBD_EVENT_SUSPEND) != 0U) {
    if (fpUSBD_Device_Suspended [device] != NULL) {
      EvrUSBD_Core_OnSuspended(device);
      fpUSBD_Device_Suspended   [device] ();
    }
  }
  if ((event & ARM_USBD_EVENT_RESUME) != 0U) {
    if (fpUSBD_Device_Resumed [device] != NULL) {
      EvrUSBD_Core_OnResumed(device);
      fpUSBD_Device_Resumed   [device] ();
    }
  }

  // Handle Endpoint 0 Events
  if (((event >> 8) & (ARM_USBD_EVENT_SETUP | ARM_USBD_EVENT_OUT | ARM_USBD_EVENT_IN)) != 0U) {
    USBD_Endpoint0                (device, event >> 8);
  }
}


/************************** Thread functions **********************************/

/// \brief Thread Function: USB Device Core
/// \param[in]     arg                  index of USB Device.
void USBD_Core_Thread (void *arg) {
  const  ARM_DRIVER_USBD *ptr_dev_driver;
         uint32_t         event;
         uint32_t         millisec, polling_mask;
         uint8_t          vbus, vbus_old;
         uint8_t          device;

  device = (uint8_t)((uint32_t)arg);
  if (device >= usbd_dev_num) {
    return;
  }

  ptr_dev_driver = usbd_dev_ptr[device]->driver;
  millisec       = 0xFFFFFFFFU;
  polling_mask   = 0U;
  if (usbd_capabilities[device].vbus_detection != 0U) {
    if (usbd_capabilities[device].event_vbus_on  == 0U) {
      polling_mask |= ARM_USBD_EVENT_VBUS_ON;
      millisec      = 100U;
    }
    if (usbd_capabilities[device].event_vbus_off == 0U) {
      polling_mask |= ARM_USBD_EVENT_VBUS_OFF;
      millisec      = 100U;
    }
  }
  vbus_old = 0U;

  // If VBUS ON detection does not work activate initial VBUS ON event
  if ((polling_mask & ARM_USBD_EVENT_VBUS_ON) != 0U) {
    USBD_Core(device, ARM_USBD_EVENT_VBUS_ON);
  }

  for (;;) {
    event = USBD_ThreadFlagsWait (millisec);
    if ((event & 0x8000000U) == 0U) {
      USBD_Core (device, event);
    } else {                                           // If timeout
      vbus = ptr_dev_driver->DeviceGetState().vbus;
      if (vbus != vbus_old) {                          // If VBUS state has changed
        vbus_old = vbus;
        if ((polling_mask & ARM_USBD_EVENT_VBUS_ON)  != 0U) {
          if (vbus != 0U) {
            USBD_Core (device, ARM_USBD_EVENT_VBUS_ON);
          }
        }
        if ((polling_mask & ARM_USBD_EVENT_VBUS_OFF) != 0U) {
          if (vbus == 0U) {
            USBD_Core (device, ARM_USBD_EVENT_VBUS_OFF);
          }
        }
      }
    }
  }
}
