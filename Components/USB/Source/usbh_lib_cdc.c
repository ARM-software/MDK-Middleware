/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates).
 * All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_lib_cdc.c
 * Purpose: USB Host - Communication Device Class (CDC) module
 *----------------------------------------------------------------------------*/

#include "usbh_lib_cdc.h"

#include <string.h>

#include "usb_lib_debug.h"
#include "usbh_config.h"
#include "usbh_lib.h"
#include "usbh_lib_core.h"
#include "usbh_evr.h"


/************************** Local function declarations ***********************/

static usbStatus CheckInstance (uint8_t instance);


/************************** User accessible functions *************************/

/// \brief Callback function called when Communication Device Class device was enumerated and is ready
/// \param[in]     instance             instance of CDC Device.
/// \return                             none.
__WEAK void USBH_CDC_ACM_Initialize (uint8_t instance) { (void)instance; }

/// \brief Callback function called when Communication Device Class device was disconnected
/// \param[in]     instance             instance of CDC Device.
/// \return                             none.
__WEAK void USBH_CDC_ACM_Uninitialize (uint8_t instance) { (void)instance; }

/// \brief Get Device instance of Communication Device Class device
/// \param[in]     instance             instance of CDC Device.
/// \return                             instance of Device or non-existing Device instance :
///                                       - value <= 127 : instance of Device
///                                       - value 255 :    non-existing Device instance
uint8_t USBH_CDC_ACM_GetDevice (uint8_t instance) {
  uint8_t device;

  if (CheckInstance (instance) == usbOK) {
    if (usbh_cdc[instance].ptr_dev == usbh_dev) {
      return 0U;
    } else {
      device = (uint8_t)(((uint32_t)usbh_cdc[instance].ptr_dev - (uint32_t)usbh_dev) / sizeof (USBH_DEV));
    }
  } else {
    device = 0xFFU;
  }

  EvrUSBH_CDC_ACM_GetDevice(instance, device);
  return device;
}

/// \brief Get status of Communication Device Class device
/// \param[in]     instance             instance of CDC Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_CDC_ACM_GetDeviceStatus (uint8_t instance) {
  const USBH_DEV *ptr_dev;
        usbStatus status;

  status = CheckInstance (instance);
  if (status == usbOK) {
    ptr_dev = usbh_cdc[instance].ptr_dev;

    if ((ptr_dev->state.configured == 0U) || (ptr_dev->state.initialized == 0U)) {
      status = usbDeviceError;
    }
  }

  EvrUSBH_CDC_ACM_GetStatus(instance, status);
  return status;
}

/// \brief Send data to Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \param[in]     data                 buffer containing data bytes to send.
/// \param[in]     num                  number of bytes to send.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_CDC_ACM_Send (uint8_t instance, const uint8_t *data, uint32_t num) {
  usbStatus status;

  EvrUSBH_CDC_ACM_Send(instance, num);

  status = CheckInstance (instance);
  if (status == usbOK) {
    status = USBH_PipeSend (usbh_cdc[instance].bulk_out_pipe_hndl, data, num);
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_CDC_ACM_SendFailed(instance, num, status);
  } else {
    EvrUSBH_CDC_ACM_SendDone(instance, num);
  }
#endif
  return status;
}

/// \brief Get result of send data to Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \return                             number of successfully sent data bytes.
uint32_t USBH_CDC_ACM_GetTxCount (uint8_t instance) {
  uint32_t num;

  if (CheckInstance (instance) == usbOK) {
    num = USBH_PipeSendGetResult (usbh_cdc[instance].bulk_out_pipe_hndl);
  } else {
    num = 0U;
  }

  EvrUSBH_CDC_ACM_GetTxCount(instance, num);
  return num;
}

/// \brief Abort data sending to Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_CDC_ACM_AbortSend (uint8_t instance) {
  usbStatus status;

  EvrUSBH_CDC_ACM_AbortSend(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {
    status = USBH_PipeAbort (usbh_cdc[instance].bulk_out_pipe_hndl);
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_CDC_ACM_AbortSendFailed(instance, status);
  } else {
    EvrUSBH_CDC_ACM_AbortSendDone(instance);
  }
#endif
  return status;
}

/// \brief Receive data from Communication Device Class device
/// \param[in]     instance             index of CDC instance
/// \param[out]    data                 buffer that receives data.
/// \param[in]     num                  maximum number of bytes to receive.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_CDC_ACM_Receive (uint8_t instance, uint8_t *data, uint32_t num) {
  usbStatus status;

  EvrUSBH_CDC_ACM_Receive(instance, num);

  status = CheckInstance (instance);
  if (status == usbOK) {
    status = USBH_PipeReceive (usbh_cdc[instance].bulk_in_pipe_hndl, data, num);
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_CDC_ACM_ReceiveFailed(instance, num, status);
  } else {
    EvrUSBH_CDC_ACM_ReceiveDone(instance, num);
  }
#endif
  return status;
}

/// \brief Get result of receive data from Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \return                             number of successfully received data bytes.
uint32_t USBH_CDC_ACM_GetRxCount (uint8_t instance) {
  uint32_t num;

  if (CheckInstance (instance) == usbOK) {
    num = USBH_PipeReceiveGetResult (usbh_cdc[instance].bulk_in_pipe_hndl);
  } else {
    num = 0U;
  }

  EvrUSBH_CDC_ACM_GetRxCount(instance, num);
  return num;
}

/// \brief Abort data reception from Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_CDC_ACM_AbortReceive (uint8_t instance) {
  usbStatus status;

  EvrUSBH_CDC_ACM_AbortReceive(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {
    status = USBH_PipeAbort (usbh_cdc[instance].bulk_in_pipe_hndl);
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_CDC_ACM_AbortReceiveFailed(instance, status);
  } else {
    EvrUSBH_CDC_ACM_AbortReceiveDone(instance);
  }
#endif
  return status;
}

/// \brief Change communication settings of Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \param[in]     line_coding          pointer to CDC_LINE_CODING structure.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_CDC_ACM_SetLineCoding (uint8_t instance, const CDC_LINE_CODING *line_coding) {
  USB_SETUP_PACKET setup_packet;
  uint8_t          device;
  usbStatus        status;

  EvrUSBH_CDC_ACM_SetLineCoding(instance, line_coding->dwDTERate);

  status = CheckInstance (instance);
  if (status == usbOK) {
    if (line_coding != NULL) {
      device = USBH_CDC_ACM_GetDevice (instance);
      if (device != 0xFFU) {
        PREPARE_SETUP_PACKET_DATA(((USB_SETUP_PACKET *)&setup_packet), USB_REQUEST_HOST_TO_DEVICE, USB_REQUEST_CLASS, USB_REQUEST_TO_INTERFACE, CDC_SET_LINE_CODING, 0U, 0U, sizeof(CDC_LINE_CODING))
        status = USBH_ControlTransfer (device, &setup_packet, (uint8_t *)((uint32_t)line_coding), sizeof(CDC_LINE_CODING));
      } else {
        status = usbDeviceError;
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_CDC_ACM_SetLineCodingFailed(instance, line_coding->dwDTERate, status);
  }
#endif
  return status;
}

/// \brief Retrieve communication settings of Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \param[out]    line_coding          pointer to CDC_LINE_CODING structure.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_CDC_ACM_GetLineCoding (uint8_t instance, CDC_LINE_CODING *line_coding) {
  USB_SETUP_PACKET setup_packet;
  uint8_t          device;
  usbStatus        status;

  status = CheckInstance (instance);
  if (status == usbOK) {
    if (line_coding != NULL) {
      device = USBH_CDC_ACM_GetDevice (instance);
      if (device != 0xFFU) {
        PREPARE_SETUP_PACKET_DATA(((USB_SETUP_PACKET *)&setup_packet), USB_REQUEST_DEVICE_TO_HOST, USB_REQUEST_CLASS, USB_REQUEST_TO_INTERFACE, CDC_GET_LINE_CODING, 0U, 0U, sizeof(CDC_LINE_CODING))
        status = USBH_ControlTransfer (device, &setup_packet, (uint8_t *)line_coding, sizeof(CDC_LINE_CODING));
      } else {
        status = usbDeviceError;
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_CDC_ACM_SetLineCodingFailed(instance, line_coding->dwDTERate, status);
  } else {
    EvrUSBH_CDC_ACM_SetLineCoding(instance, line_coding->dwDTERate);
  }
#endif
  return status;
}

/// \brief Set control line states of Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \param[in]     state                control line settings bitmap :
///                                       - bit 0 : DTR state
///                                       - bit 1 : RTS state
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_CDC_ACM_SetControlLineState (uint8_t instance, uint16_t state) {
  USB_SETUP_PACKET setup_packet;
  uint8_t          device;
  usbStatus        status;

  EvrUSBH_CDC_ACM_SetControlLineState(instance, (uint8_t)(state & 1U), (uint8_t)((state >> 1) & 1U));

  status = CheckInstance (instance);
  if (status == usbOK) {
    device = USBH_CDC_ACM_GetDevice (instance);
    if (device != 0xFFU) {
      PREPARE_SETUP_PACKET_DATA(((USB_SETUP_PACKET *)&setup_packet), USB_REQUEST_HOST_TO_DEVICE, USB_REQUEST_CLASS, USB_REQUEST_TO_INTERFACE, CDC_SET_CONTROL_LINE_STATE, state, 0U, 0U)
      status = USBH_ControlTransfer (device, &setup_packet, NULL, 0U);
    } else {
      status = usbDeviceError;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_CDC_ACM_SetControlLineStateFailed(instance, (uint8_t)(state & 1U), (uint8_t)((state >> 1) & 1U), status);
  }
#endif
  return status;
}

/// \brief Callback function called when Communication Device Class device
///        modem line or error status changes
/// \param[in]     instance             index of CDC instance.
/// \param[in]     status               error status and line states :
///                                       - bit 6 : bOverRun
///                                       - bit 5 : bParity
///                                       - bit 4 : bFraming
///                                       - bit 3 : bRingSignal
///                                       - bit 2 : bBreak
///                                       - bit 1 : bTxCarrier (DSR line state)
///                                       - bit 0 : bRxCarrier (DCD line state)
/// \return                             none.
__WEAK void USBH_CDC_ACM_Notify (uint8_t instance, uint16_t status) {
  // Dummy implementation should be overridden by external function

  (void)instance;
  (void)status;
}

/// \brief Send break on Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \param[in]     duration             duration of break (in milliseconds) :
///                                       - value 65535 : indefinite
///                                       - value     0 : immediate
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_CDC_ACM_SendBreak (uint8_t instance, uint16_t duration) {
  USB_SETUP_PACKET setup_packet;
  uint8_t          device;
  usbStatus        status;

  EvrUSBH_CDC_ACM_SendBreak(instance, duration);

  status = CheckInstance (instance);
  if (status == usbOK) {
    device = USBH_CDC_ACM_GetDevice (instance);
    if (device != 0xFFU) {
      PREPARE_SETUP_PACKET_DATA(((USB_SETUP_PACKET *)&setup_packet), USB_REQUEST_HOST_TO_DEVICE, USB_REQUEST_CLASS, USB_REQUEST_TO_INTERFACE, CDC_SEND_BREAK, duration, 0U, 0U)
      status = USBH_ControlTransfer (device, &setup_packet, NULL, 0U);
    } else {
      status = usbDeviceError;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_CDC_ACM_SendBreakFailed(instance, duration, status);
  }
#endif
  return status;
}


/************************** Library accessible functions **********************/

/// \brief Analyze device configuration and configure resources
/// \param[in]     device               index of USB Device.
/// \param[in]     ptr_dev_desc         pointer to device descriptor.
/// \param[in]     ptr_cfg_desc         pointer to configuration descriptor.
/// \return                             index of configured CDC instance or configuration failed :
///                                       - value <= 127 : index of configured CDC instance
///                                       - value 255 :    configuration failed
__WEAK uint8_t USBH_CDC_Configure (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc) {
        USBH_DEV                             *ptr_dev;
        USBH_CDC                             *ptr_cdc;
  const USB_COMMON_DESCRIPTOR                *ptr_cmn_desc;
  const USB_INTERFACE_DESCRIPTOR             *ptr_if_desc;
  const USB_INTERFACE_ASSOCIATION_DESCRIPTOR *ptr_if_assoc_desc;
  const USB_ENDPOINT_DESCRIPTOR              *ptr_ep_desc;
  const uint8_t                              *ptr_desc;
        USBH_PIPE_HANDLE                      pipe_hndl;
        uint8_t                               num;
        uint8_t                               ret, idx, if_cnt;
        usbStatus                             status;

  status = usbOK;
  ret    = 255U;
  if_cnt = 0U;

  if (device == 255U) {
    goto exit;
  }
  if (ptr_dev_desc == NULL) {
    goto exit;
  }
  if (ptr_cfg_desc == NULL) {
    goto exit;
  }

  ptr_dev = &usbh_dev[device];

  // Find first free CDC instance block
  for (idx = 0U; idx < usbh_cdc_num; idx++) {
    if (usbh_cdc[idx].ptr_dev == NULL) {
      break;                            // Free block found
    }
  }
  if (idx == usbh_cdc_num) {
    goto exit;                          // Free block not found
  }

  EvrUSBH_CDC_ACM_Configure(idx);

  ptr_cdc = &usbh_cdc[idx];
  memset ((void *)ptr_cdc, 0, sizeof (USBH_CDC));

  ptr_cdc->ptr_dev = (USBH_DEV *)ptr_dev;

  ptr_desc      = (const uint8_t *)ptr_cfg_desc;
  ptr_desc     += ptr_cfg_desc->bLength;
  ptr_cmn_desc  = (const USB_COMMON_DESCRIPTOR *)ptr_desc;
  if (ptr_cmn_desc->bDescriptorType == USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE) {
    // If this is Interface Association Descriptor
    ptr_if_assoc_desc = (const USB_INTERFACE_ASSOCIATION_DESCRIPTOR *)ptr_desc;
    if ((ptr_if_assoc_desc->bFunctionClass    != USB_DEVICE_CLASS_COMMUNICATIONS) ||
        (ptr_if_assoc_desc->bFunctionSubclass != CDC_ABSTRACT_CONTROL_MODEL)) {
      goto exit;                        // Not CDC device
    }
    ptr_desc      = (const uint8_t *)ptr_if_assoc_desc;
    ptr_desc     += ptr_if_assoc_desc->bLength;
    ptr_cmn_desc  = (const USB_COMMON_DESCRIPTOR *)ptr_desc;
  }
  if (ptr_cmn_desc->bDescriptorType != USB_INTERFACE_DESCRIPTOR_TYPE) {
    goto exit;                          // Not CDC device
  }
  ptr_if_desc = (const USB_INTERFACE_DESCRIPTOR *)ptr_desc;
  while ((uint32_t)ptr_if_desc < ((uint32_t)ptr_cfg_desc + ptr_cfg_desc->wTotalLength) && (if_cnt < 2U)) {
    num = ptr_if_desc->bNumEndpoints;   // Number of endpoints
    switch (ptr_if_desc->bInterfaceClass) {
      case CDC_COMMUNICATION_INTERFACE_CLASS:
      case CDC_DATA_INTERFACE_CLASS:
        if_cnt++;
        // Skip all descriptors until Endpoint Descriptor
        ptr_desc      = (const uint8_t *)ptr_if_desc;
        ptr_cmn_desc  = (const USB_COMMON_DESCRIPTOR *)ptr_desc;
        while (ptr_cmn_desc->bDescriptorType != USB_ENDPOINT_DESCRIPTOR_TYPE) {
          ptr_desc     += ptr_cmn_desc->bLength;
          ptr_cmn_desc  = (const USB_COMMON_DESCRIPTOR *)ptr_desc;
        }
        // Create Pipes
        ptr_ep_desc = (const USB_ENDPOINT_DESCRIPTOR *)ptr_desc;
        while (num-- != 0U) {
          pipe_hndl = USBH_PipeCreate (device, ptr_ep_desc->bEndpointAddress, (uint8_t)((uint32_t)ptr_ep_desc->bmAttributes & USB_ENDPOINT_TYPE_MASK), ptr_ep_desc->wMaxPacketSize & 0x7FFU, ptr_ep_desc->bInterval);
          if (pipe_hndl == 0U) {
            // If creation of pipe has failed delete previously created pipes
            if (ptr_cdc->bulk_in_pipe_hndl != 0U) {
              status = USBH_PipeDelete (ptr_cdc->bulk_in_pipe_hndl);
              if (status != usbOK) {
                goto exit;
              }
            }
            if (ptr_cdc->bulk_out_pipe_hndl != 0U) {
              status = USBH_PipeDelete (ptr_cdc->bulk_out_pipe_hndl);
              if (status != usbOK) {
                goto exit;
              }
            }
            if (ptr_cdc->int_in_pipe_hndl != 0U) {
              status = USBH_PipeDelete (ptr_cdc->int_in_pipe_hndl);
              if (status != usbOK) {
                goto exit;
              }
            }
            memset ((void *)ptr_cdc, 0, sizeof (USBH_CDC));
            goto exit;
          }
          switch (ptr_ep_desc->bmAttributes & USB_ENDPOINT_TYPE_MASK) {
            case USB_ENDPOINT_TYPE_BULK:        // Bulk endpoint
              if ((ptr_ep_desc->bEndpointAddress & USB_ENDPOINT_DIRECTION_MASK) != 0U) {
                ptr_cdc->bulk_in_pipe_hndl  = pipe_hndl;
              } else {
                ptr_cdc->bulk_out_pipe_hndl = pipe_hndl;
              }
              break;
            case USB_ENDPOINT_TYPE_INTERRUPT:   // Interrupt endpoint
                                        
              if ((ptr_ep_desc->bEndpointAddress & USB_ENDPOINT_DIRECTION_MASK) != 0U) {
                ptr_cdc->int_in_pipe_hndl  = pipe_hndl;
              }
              break;
            default:                            // Unexpected endpoint type
              break;
          }
          ptr_ep_desc++;
        }
        ptr_desc    = (const uint8_t *)ptr_ep_desc;
        ptr_if_desc = (const USB_INTERFACE_DESCRIPTOR *)ptr_desc;
        break;
    default:                            // Unhandled interface class
      // Skip all descriptors in current interface as it is not a CDC interface
      ptr_desc      = (const uint8_t *)ptr_if_desc;
      ptr_cmn_desc  = (const USB_COMMON_DESCRIPTOR *)ptr_desc;
      ptr_desc     += ptr_cmn_desc->bLength;    // Step over current interface descriptor
      ptr_cmn_desc  = (const USB_COMMON_DESCRIPTOR *)ptr_desc;
      while ((ptr_cmn_desc->bDescriptorType != USB_INTERFACE_DESCRIPTOR_TYPE) && 
             ((uint32_t)ptr_cmn_desc < ((uint32_t)ptr_cfg_desc + ptr_cfg_desc->wTotalLength))) {
        ptr_desc     += ptr_cmn_desc->bLength;
        ptr_cmn_desc  = (const USB_COMMON_DESCRIPTOR *)ptr_desc;
      }
      ptr_if_desc = (const USB_INTERFACE_DESCRIPTOR *)ptr_cmn_desc;
      break;
    }
  }
  
  if ((ptr_cdc->bulk_in_pipe_hndl  != 0U) &&
      (ptr_cdc->bulk_out_pipe_hndl != 0U) &&
      (ptr_cdc->int_in_pipe_hndl   != 0U)) {
    ret = idx;
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_CDC_ACM_ConfigureFailed(status);
  }
#endif
  return ret;
}

/// \brief De-configure resources
/// \param[in]     instance             index of CDC instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
__WEAK usbStatus USBH_CDC_Unconfigure (uint8_t instance) {
  const USBH_DEV *ptr_dev;
        USBH_CDC *ptr_cdc;
        usbStatus status, first_err_status;

  EvrUSBH_CDC_ACM_Unconfigure(instance);

  first_err_status = CheckInstance (instance);
  if (first_err_status == usbOK) {
    ptr_cdc = &usbh_cdc[instance];
    ptr_dev =  ptr_cdc->ptr_dev;

    if (ptr_dev->state.configured != 0U) {
      if (ptr_cdc->bulk_out_pipe_hndl != 0U) {
        status = USBH_PipeAbort  (ptr_cdc->bulk_out_pipe_hndl);
        if (status != usbOK) {
          first_err_status = status;
        }
        status = USBH_PipeDelete (ptr_cdc->bulk_out_pipe_hndl);
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
      }
      if (ptr_cdc->bulk_in_pipe_hndl != 0U) {
        status = USBH_PipeAbort  (ptr_cdc->bulk_in_pipe_hndl);
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
        status = USBH_PipeDelete (ptr_cdc->bulk_in_pipe_hndl);
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
      }
      if (ptr_cdc->int_in_pipe_hndl != 0U) {
        status = USBH_PipeAbort  (ptr_cdc->int_in_pipe_hndl);
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
        status = USBH_PipeDelete (ptr_cdc->int_in_pipe_hndl);
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
      }
    }
    if (first_err_status == usbOK) {
      memset ((void *)ptr_cdc, 0, sizeof (USBH_CDC));
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (first_err_status != usbOK) {
    EvrUSBH_CDC_ACM_UnconfigureFailed(instance, first_err_status);
  }
#endif
  return first_err_status;
}

/// \brief Initialize Communication Device Class device instance
/// \param[in]     instance             index of CDC instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
__WEAK usbStatus USBH_CDC_Initialize_Lib (uint8_t instance) {
  USBH_CDC        *ptr_cdc;
  CDC_LINE_CODING  line_coding;
  usbStatus        status;

  EvrUSBH_CDC_ACM_Initialize(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {
    ptr_cdc = &usbh_cdc[instance];

    // Initial CDC SetLineCoding request: Set 9600 baud, 8 data bits, 1 stop bit, no parity
    line_coding.dwDTERate   = 9600;
    line_coding.bCharFormat = 0;
    line_coding.bParityType = 0;
    line_coding.bDataBits   = 8;
    status = USBH_CDC_ACM_SetLineCoding (instance, (const CDC_LINE_CODING *)(&line_coding));
    if (status == usbOK) {
      // Start Interrupt In Pipe handling thread
      ptr_cdc->int_in_thread_id = USBH_ThreadCreate (usbhThreadCDC, ptr_cdc->ptr_dev->class_instance);
      if (ptr_cdc->int_in_thread_id == NULL) {
        status = usbThreadError;
      }
    }
  }

  EvrUSBH_CDC_ACM_OnInitialize(instance);
  USBH_CDC_ACM_Initialize (instance);   // User level callback

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_CDC_ACM_InitializeFailed(instance, status);
  }
#endif
  return status;
}

/// \brief De-initialize Communication Device Class device instance
/// \param[in]     instance             index of CDC instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
__WEAK usbStatus USBH_CDC_Uninitialize_Lib (uint8_t instance) {
  USBH_CDC *ptr_cdc;
  usbStatus status;

  status = CheckInstance (instance);
  if (status == usbOK) {
    ptr_cdc = &usbh_cdc[instance];

    if (ptr_cdc->int_in_thread_id != NULL) {
      if (USBH_ThreadTerminate (ptr_cdc->int_in_thread_id) == 0) {
        ptr_cdc->int_in_thread_id = NULL;
      } else {
        status = usbThreadError;
      }
    }
  }

  EvrUSBH_CDC_ACM_Uninitialize(instance);
  EvrUSBH_CDC_ACM_OnUninitialize(instance);
  USBH_CDC_ACM_Uninitialize (instance); // User level callback

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_CDC_ACM_UninitializeFailed(instance, status);
  }
#endif
  return status;
}


/************************** Local functions ***********************************/

/// \brief Check device instance pointers and variables
/// \param[in]     instance             index of CDC instance.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus CheckInstance (uint8_t instance) {

  if (instance >= usbh_cdc_num)                        { return usbClassErrorCDC;   }
  if (usbh_cdc[instance].ptr_dev == NULL)              { return usbDeviceError;     }
  if (usbh_cdc[instance].ptr_dev->state.in_use == 0U)  { return usbDeviceError;     }
  if (usbh_cdc[instance].ptr_dev->ctrl >= usbh_hc_num) { return usbControllerError; }

  return usbOK;
}


/************************** Thread functions **********************************/

/// \brief User thread for handling modem status and error feedback from CDC
///        device (from Interrupt In endpoint)
/// \param[in]     arg              index of CDC instance.
void USBH_CDC_IntIn_Thread (void *arg) {
         USBH_CDC *ptr_cdc;
         uint16_t  status;
         uint8_t   instance;
  static uint8_t   buf[16];

  instance = (uint8_t)((uint32_t)arg);
  if (instance >= usbh_cdc_num) {
    return;
  }
  if (CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_cdc = &usbh_cdc[instance];

  for (;;) {
    if (USBH_PipeReceive  (ptr_cdc->int_in_pipe_hndl, buf, ((USBH_PIPE *)ptr_cdc->int_in_pipe_hndl)->wMaxPacketSize) == usbOK) {
      // Data has been received on Interrupt In Pipe
      if ((buf[0] == 0xA1U)                         &&
          (buf[1] == CDC_NOTIFICATION_SERIAL_STATE) &&
          (buf[2] == 0x00U)                         &&
          (buf[3] == 0x00U)                         &&
          (buf[4] == 0x00U)                         &&
          (buf[5] == 0x00U)                         &&
          (buf[6] == 0x02U)                         &&
          (buf[7] == 0x00U)) {
        // If this is Notification then store new modem line and error status
        status = (uint16_t)(((uint32_t)buf[9] << 8) | (uint32_t)buf[8]);
        EvrUSBH_CDC_ACM_OnNotify (instance, status);
        USBH_CDC_ACM_Notify (instance, status);
      }
    } else {
      ptr_cdc->int_in_thread_id = NULL;
      (void)USBH_ThreadTerminate (USBH_ThreadGetHandle());
      return;
    }
  }
}
