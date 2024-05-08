/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates).
 * All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_lib_hid.c
 * Purpose: USB Host - Human Interface Device (HID) module
 *----------------------------------------------------------------------------*/

#include "usbh_lib_hid.h"

#include <string.h>

#include "rl_usbh_lib.h"
#include "usb_lib_debug.h"
#include "usbh_config.h"
#include "usbh_lib_core.h"
#include "usbh_lib_hid_idtoascii.h"
#include "usbh_evr.h"


/************************** Local function declarations ***********************/

static   usbStatus CheckInstance        (uint8_t instance);
//static usbStatus USBH_HID_GetReport   (uint8_t instance, uint8_t report_type, uint8_t report_id, uint8_t interface, uint8_t *report_data, uint16_t report_length);
static   usbStatus USBH_HID_SetReport   (uint8_t instance, uint8_t report_type, uint8_t report_id, uint8_t interface, uint8_t *report_data, uint16_t report_length);
//static usbStatus USBH_HID_GetIdle     (uint8_t instance, uint8_t report_id, uint8_t interface, uint8_t *ptr_duration);
static   usbStatus USBH_HID_SetIdle     (uint8_t instance, uint8_t report_id, uint8_t interface, uint8_t duration);
//static usbStatus USBH_HID_GetProtocol (uint8_t instance, uint8_t interface, uint8_t *ptr_protocol);
//static usbStatus USBH_HID_SetProtocol (uint8_t instance, uint8_t interface, uint8_t protocol);
static   usbStatus USBH_HID_ReportOut   (uint8_t instance, const uint8_t *ptr_data, uint16_t data_len);


/************************** User accessible functions *************************/

/// \brief Callback function called when Human Interface Device was enumerated and is ready
/// \param[in]     instance             instance of HID Device.
/// \return                             none.
__WEAK void USBH_HID_Initialize (uint8_t instance) { (void)instance; }

/// \brief Callback function called when Human Interface Device was disconnected
/// \param[in]     instance             instance of HID Device.
/// \return                             none.
__WEAK void USBH_HID_Uninitialize (uint8_t instance) { (void)instance; }

/// \brief Get Device instance of Human Interface Device
/// \param[in]     instance             instance of HID Device.
/// \return                             instance of Device or non-existing Device instance :
///                                       - value <= 127 : instance of Device
///                                       - value 255 :    non-existing Device instance
uint8_t USBH_HID_GetDevice (uint8_t instance) {
  uint8_t device;

  if (CheckInstance (instance) == usbOK) {
    if (usbh_hid[instance].ptr_dev == usbh_dev) {
      return 0U;
    } else {
      device = (uint8_t)(((uint32_t)usbh_hid[instance].ptr_dev - (uint32_t)usbh_dev) / sizeof (USBH_DEV));
    }
  } else {
    device = 0xFFU;
  }

  EvrUSBH_HID_GetDevice(instance, device);
  return device;
}

/// \brief Get status of Human Interface Device
/// \param[in]     instance             instance of HID Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBH_HID_GetDeviceStatus (uint8_t instance) {
  const USBH_DEV *ptr_dev;
        usbStatus status;

  status = CheckInstance (instance);
  if (status == usbOK) {
    ptr_dev = usbh_hid[instance].ptr_dev;

    if ((ptr_dev->state.configured == 0U) || (ptr_dev->state.initialized == 0U)) {
      status = usbDeviceError;
    }
  }

  EvrUSBH_HID_GetStatus(instance, status);
  return status;
}

/// \brief Read data received from Human Interface Device
/// \param[in]     instance             instance of HID Device.
/// \param[out]    buf                  buffer that receives data.
/// \param[in]     len                  maximum number of bytes to read.
/// \return                             number of bytes read or execution status :
///                                       - value >= 0 : number of bytes read
///                                       - value < 0 :  error occurred, -value is execution status as defined with \ref usbStatus
int32_t USBH_HID_Read (uint8_t instance, uint8_t *buf, int32_t len) {
  USBH_HID *ptr_hid;
  int32_t   num;
  usbStatus status;

  EvrUSBH_HID_Read(instance, len);

  num = 0;

  status = CheckInstance (instance);
  if (status == usbOK) {
    if (buf != NULL) {
      ptr_hid = &usbh_hid[instance];

      num = (int32_t)ptr_hid->hid_in_data_len;
      if (num > len) {
        num = len;
      }

      if (num != 0) {
        memcpy ((void *)buf, (void *)ptr_hid->ptr_int_in_buf, (uint32_t)num);
        ptr_hid->hid_in_data_len = 0U;
      }
    } else {
      status = usbInvalidParameter;
    }
  }

  if (status != usbOK) {
    EvrUSBH_HID_ReadFailed(instance, len, status);
    return -(int32_t)status;
  } else {
    EvrUSBH_HID_ReadDone(instance, len, num);
    return num;
  }
}

/// \brief Write data to Human Interface Device
/// \param[in]     instance             instance of HID Device.
/// \param[in]     buf                  data buffer containing data to write.
/// \param[in]     len                  number of data bytes to write.
/// \return                             number of bytes accepted for writing or execution status :
///                                       - value >= 0 : number of bytes accepted for writing
///                                       - value < 0 :  error occurred, -value is execution status as defined with \ref usbStatus
int32_t USBH_HID_Write (uint8_t instance, const uint8_t *buf, int32_t len) {
  usbStatus status;
  int32_t   num;

  EvrUSBH_HID_Write(instance, len);

  num = 0;

  status = CheckInstance (instance);
  if (status == usbOK) {
    if (buf != NULL) {
      if (USBH_HID_ReportOut (instance, buf, (uint16_t)len) == usbOK) {
        num = len;
      }
    } else {
      status = usbInvalidParameter;
    }
  }

  if (status != usbOK) {
    EvrUSBH_HID_WriteFailed(instance, len, status);
    return -(int32_t)status;
  } else {
    EvrUSBH_HID_WriteDone(instance, len, num);
    return num;
  }
}

/// \brief Retrieve first pending pressed keyboard key on HID Keyboard
/// \param[in]     instance             instance of HID Device.
/// \return                             code of first pressed unread key or no key was pressed :
///                                       - value >= 0 : code of first pressed unread key
///                                       - value -1 :   indicates no key was pressed since last time this function was called
__WEAK int USBH_HID_GetKeyboardKey (uint8_t instance) {
  USBH_HID *ptr_hid;
  uint32_t  key;
  uint8_t   i;
  bool      new_char;
  int       result;

  EvrUSBH_HID_GetKeyboardKey(instance);

  result = -1;
  if (CheckInstance (instance) == usbOK) {
    ptr_hid = &usbh_hid[instance];

    if ((ptr_hid->hid_in_data_len < 8U)              || 
        (ptr_hid->hid_in_data_len > 9U)              || 
        (ptr_hid->keyboard_in_key_pos == 8U)         || 
        (ptr_hid->protocol != HID_PROTOCOL_KEYBOARD)) {
      return -1;
    }

    new_char = false;
    do {
      key = (uint32_t)ptr_hid->hid_in_data_buf[ptr_hid->keyboard_in_key_pos];
      ptr_hid->keyboard_in_key_pos++;
      if (key != 0U) {
        for (i = 2U; i < 8U; i++) {     // Check if char was already pressed
          if (key == (uint32_t)ptr_hid->hid_in_data_buf_ex[i]) {
            break;                      // Found char that was not pressed
          }
          if (i == 7U) {
            new_char = true;
          }
        }
      } else {
        ptr_hid->hid_in_data_len = 0U;
      }
    } while ((!new_char) && (ptr_hid->keyboard_in_key_pos != 8U));

    if (new_char) {                     // If new character was found
      if (key == 1U) {                  // Check if rollover event
        if (memcmp ((void *)&ptr_hid->hid_in_data_buf[2], (const void *)"\1\1\1\1\1\1", 6U) == 0) {
          result = -1;
          ptr_hid->keyboard_in_key_pos = 2U;
          ptr_hid->hid_in_data_len     = 0U;
        }
      } else {
        if ((key >= HID_USAGE_KEYBOARD_F1) || (HID_KEYBOARD_ID_TO_ASCII[key] == 0xFFU)) {
          key = key | 0x10000UL;        // bit 16. specifies if it is a non-ASCII translated HID ID
        } else {
          // Translate to ASCII
          key = HID_KEYBOARD_ID_TO_ASCII[key];
        }
        // Add modifier values to bits 8..15
        key |= ((uint32_t)ptr_hid->hid_in_data_buf[0]) << 8U;
        result = (int)key;
      }
    } else {
      result = -1;
      ptr_hid->keyboard_in_key_pos = 2U;
      ptr_hid->hid_in_data_len     = 0U;
    }
  }

  EvrUSBH_HID_GetKeyboardKeyDone(instance, result);
  return result;
}

/// \brief Retrieve state change since last call of this function
/// \param[in]     instance             instance of HID Device.
/// \param[out]    state                pointer to mouse state \ref usbHID_MouseState structure.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
__WEAK usbStatus USBH_HID_GetMouseState (uint8_t instance, usbHID_MouseState *state) {
  USBH_HID *ptr_hid;
  usbStatus status;
  int8_t    x, y;

  EvrUSBH_HID_GetMouseState(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {
    ptr_hid = &usbh_hid[instance];

    if (ptr_hid->hid_in_data_len != 0U) {
      ptr_hid->hid_in_data_len = 0U;
      if (ptr_hid->protocol == HID_PROTOCOL_MOUSE) {
        ptr_hid->mouse_state.button =  ptr_hid->hid_in_data_buf[0];
        x                           = (int8_t)ptr_hid->hid_in_data_buf[1];
        ptr_hid->mouse_state.x     += (int16_t)x;
        y                           = (int8_t)ptr_hid->hid_in_data_buf[2];
        ptr_hid->mouse_state.y     += (int16_t)y;
      }
    }
    *state = ptr_hid->mouse_state;
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_HID_GetMouseStateFailed(instance, status);
  } else {
    EvrUSBH_HID_GetMouseStateDone(instance, *state);
  }
#endif
  return status;
}

/// \brief Callback function called for parsing of the Human Interface Device report descriptor
/// \param[in]     instance             instance index.
/// \param[in]     ptr_hid_report_desc  pointer to HID report descriptor.
/// \param[in]     len                  length of HID report descriptor.
/// \return                             none.
__WEAK void USBH_HID_ParseReportDescriptor (uint8_t instance, const uint8_t *ptr_hid_report_desc, uint32_t len) {

  (void)instance;
  (void)ptr_hid_report_desc;
  (void)len;
}

/// \brief Callback function called when data is received from the Human Interface Device
/// \param[in]     instance             instance index.
/// \param[in]     len                  length of received data.
/// \return                             none.
__WEAK void USBH_HID_DataReceived (uint8_t instance, uint32_t len) {

  (void)instance;
  (void)len;
}


/************************** Library accessible functions **********************/

/// \brief Analyze device configuration and configure resources
/// \param[in]     device               index of USB Device.
/// \param[in]     ptr_dev_desc         pointer to device descriptor.
/// \param[in]     ptr_cfg_desc         pointer to configuration descriptor.
/// \return                             index of configured HID instance or configuration failed :
///                                       - value <= 127 : index of configured HID instance
///                                       - value 255 :    configuration failed
__WEAK uint8_t USBH_HID_Configure (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc) {
        USBH_DEV                 *ptr_dev;
        USBH_HID                 *ptr_hid;
  const USB_INTERFACE_DESCRIPTOR *ptr_if_desc;
  const HID_DESCRIPTOR           *ptr_hid_desc;
  const USB_ENDPOINT_DESCRIPTOR  *ptr_ep_desc;
  const uint8_t                  *ptr_desc;
        USBH_PIPE_HANDLE          pipe_hndl;
        uint8_t                   num;
        uint8_t                   ret, idx;
        usbStatus                 status;

  status = usbOK;
  ret    = 255U;

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

  // Find first free HID instance block
  for (idx = 0U; idx < usbh_hid_num; idx++) {
    if (usbh_hid[idx].ptr_dev == NULL) {
      break;                            // Free block found
    }
  }
  if (idx == usbh_hid_num) {
    goto exit;                          // Free block not found
  }

  EvrUSBH_HID_Configure(idx);

  ptr_hid = &usbh_hid[idx];
  memset ((void *)ptr_hid, 0, sizeof (USBH_HID));

  ptr_hid->ptr_dev = (USBH_DEV *)ptr_dev;

  ptr_desc    = (const uint8_t *)ptr_cfg_desc;
  ptr_desc   += ptr_cfg_desc->bLength;
  ptr_if_desc = (const USB_INTERFACE_DESCRIPTOR *)ptr_desc;
  num = ptr_if_desc->bNumEndpoints;     // Number of endpoints

  switch (ptr_if_desc->bInterfaceClass) {
    case USB_DEVICE_CLASS_HUMAN_INTERFACE:
      ptr_hid->protocol = ptr_if_desc->bInterfaceProtocol;

      // HID Descriptor
      ptr_desc     = (const uint8_t *)ptr_if_desc;
      ptr_desc    += ptr_if_desc->bLength;
      ptr_hid_desc = (const HID_DESCRIPTOR *)ptr_desc;
      ptr_hid->report_desc_type = ptr_hid_desc->DescriptorList[0].bDescriptorType;
      ptr_hid->report_desc_len  = ptr_hid_desc->DescriptorList[0].wDescriptorLength;

      // Create Pipes
      ptr_desc     = (const uint8_t *)ptr_hid_desc;
      ptr_desc    += ptr_hid_desc->bLength;
      ptr_ep_desc  = (const USB_ENDPOINT_DESCRIPTOR *)ptr_desc;
      while (num-- != 0U) {
        if ((ptr_ep_desc->bmAttributes & 3U) == USB_ENDPOINT_TYPE_INTERRUPT) {  // Interrupt Endpoint
          pipe_hndl = USBH_PipeCreate (device, ptr_ep_desc->bEndpointAddress, (uint8_t)((uint32_t)ptr_ep_desc->bmAttributes & USB_ENDPOINT_TYPE_MASK), ptr_ep_desc->wMaxPacketSize & 0x7FFU, ptr_ep_desc->bInterval);
          if (pipe_hndl == 0U) {
            // If creation of pipe has failed delete previously created pipes
            if (ptr_hid->int_in_pipe_hndl  != 0U) {
              status = USBH_PipeDelete (ptr_hid->int_in_pipe_hndl);
              if (status != usbOK) {
                goto exit;
              }
            }
            if (ptr_hid->int_out_pipe_hndl != 0U) {
              status = USBH_PipeDelete (ptr_hid->int_out_pipe_hndl);
              if (status != usbOK) {
                goto exit;
              }
            }
            memset ((void *)ptr_hid, 0, sizeof (USBH_HID));
            goto exit;
          }
          if ((ptr_ep_desc->bEndpointAddress & USB_ENDPOINT_DIRECTION_MASK) != 0U) {
            ptr_hid->int_in_pipe_hndl  = pipe_hndl;
          } else {
            ptr_hid->int_out_pipe_hndl = pipe_hndl;
          }
        }
        ptr_ep_desc++;
      }
      break;
    default:                            // Unhandled interface class
      break;
  }

  if (ptr_hid->int_in_pipe_hndl != 0U) {
    ret = idx;
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_HID_ConfigureFailed(status);
  }
#endif
  return ret;
}

/// \brief De-configure resources
/// \param[in]     instance             index of HID instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
__WEAK usbStatus USBH_HID_Unconfigure (uint8_t instance) {
  const USBH_DEV *ptr_dev;
        USBH_HID *ptr_hid;
        usbStatus status, first_err_status;

  EvrUSBH_HID_Unconfigure(instance);

  first_err_status = CheckInstance (instance);
  if (first_err_status == usbOK) {
    ptr_hid = &usbh_hid[instance];
    ptr_dev =  ptr_hid->ptr_dev;

    if (ptr_dev->state.configured != 0U) {
      if (ptr_hid->int_out_pipe_hndl != 0U) {
        status = USBH_PipeAbort  (ptr_hid->int_out_pipe_hndl);
        if (status != usbOK) {
          first_err_status = status;
        }
        status = USBH_PipeDelete (ptr_hid->int_out_pipe_hndl);
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
      }
      if (ptr_hid->int_in_pipe_hndl  != 0U)  { 
        status = USBH_PipeAbort  (ptr_hid->int_in_pipe_hndl);
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
        status = USBH_PipeDelete (ptr_hid->int_in_pipe_hndl);
        if ((status != usbOK) && (first_err_status == usbOK)) {
          first_err_status = status;
        }
      }
    }
    if (first_err_status == usbOK) {
      memset ((void *)ptr_hid, 0, sizeof (USBH_HID));
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (first_err_status != usbOK) {
    EvrUSBH_HID_UnconfigureFailed(instance, first_err_status);
  }
#endif
  return first_err_status;
}

/// \brief Initialize Human Interface Device instance
/// \param[in]     instance             index of HID instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
__WEAK usbStatus USBH_HID_Initialize_Lib (uint8_t instance) {
  USBH_HID *ptr_hid;
  uint8_t  *ptr_hid_report_desc;
  uint8_t   ctrl, device;
  usbStatus status;
  usbStatus mstatus;

  EvrUSBH_HID_Initialize(instance);

  status = CheckInstance (instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_hid = &usbh_hid[instance];
  ctrl    =  ptr_hid->ptr_dev->ctrl;
  device  =  USBH_HID_GetDevice (instance);

  if (device == 0xFFU) {
    status = usbDeviceError;
    goto exit;
  }

  // Set Idle Time to infinite
  status  = USBH_HID_SetIdle                (instance, 0U, 0U, 0U);
  if ((status != usbOK) && (status != usbTransferStall)) {
    goto exit;
  }

  mstatus = USBH_MemoryAllocate             (ctrl, (uint8_t **)&ptr_hid_report_desc, ptr_hid->report_desc_len);
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }

  // Get HID Report Descriptor
  status  = USBH_DeviceRequest_GetDescriptor(device, USB_REQUEST_TO_INTERFACE, ptr_hid->report_desc_type, 0U, 0U, (uint8_t *)ptr_hid_report_desc, ptr_hid->report_desc_len);
  if (status != usbOK) {
    (void)USBH_MemoryFree                   (ctrl, (uint8_t  *) ptr_hid_report_desc);
    goto exit;
  }

  // Allow HID report descriptor to be parsed externally
  EvrUSBH_HID_OnParseReportDescriptor       (instance, ptr_hid->report_desc_len);
  USBH_HID_ParseReportDescriptor            (instance, (const uint8_t *)(ptr_hid_report_desc), ptr_hid->report_desc_len);

  mstatus = USBH_MemoryFree                 (ctrl, (uint8_t  *) ptr_hid_report_desc);
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }

  // Allocate memory for Report
  mstatus = USBH_MemoryAllocate             (ctrl, (uint8_t **)&ptr_hid->ptr_int_in_buf, ((USBH_PIPE *)ptr_hid->int_in_pipe_hndl)->wMaxPacketSize);
  if (mstatus != usbOK) {
    status = mstatus;
    goto exit;
  }

  // Start HID receive thread
  if (ptr_hid->int_in_thread_id == NULL) {
    ptr_hid->int_in_thread_id = USBH_ThreadCreate (usbhThreadHID, ptr_hid->ptr_dev->class_instance);
    if (ptr_hid->int_in_thread_id == NULL) {
      status = usbThreadError;
      (void)USBH_MemoryFree                 (ctrl, (uint8_t  *) ptr_hid->ptr_int_in_buf);
      ptr_hid->ptr_int_in_buf = NULL;
      goto exit;
    }
  }

  ptr_hid->keyboard_in_key_pos = 2U;
  memset ((void *)ptr_hid->hid_in_data_buf,    0, 8U);
  memset ((void *)ptr_hid->hid_in_data_buf_ex, 0, 8U);

  USBH_HID_Initialize (instance);       // User level callback

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_HID_InitializeFailed(instance, status);
  }
#endif
  return status;
}

/// \brief De-initialize Human Interface Device instance
/// \param[in]     instance             index of HID instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
__WEAK usbStatus USBH_HID_Uninitialize_Lib (uint8_t instance) {
  USBH_HID *ptr_hid;
  uint8_t   ctrl;
  usbStatus status;
  usbStatus mstatus;

  EvrUSBH_HID_Uninitialize(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {

    ptr_hid = &usbh_hid[instance];
    ctrl    =  ptr_hid->ptr_dev->ctrl;

    // Terminate HID receive thread
    if (ptr_hid->int_in_thread_id != NULL) {
      if (USBH_ThreadTerminate (ptr_hid->int_in_thread_id) == 0) {
        ptr_hid->int_in_thread_id = NULL;
      } else {
        status = usbThreadError;
      }
    }

    // De-allocate memory for Report
    if (ptr_hid->ptr_int_in_buf != NULL) {
      mstatus = USBH_MemoryFree (ctrl, ptr_hid->ptr_int_in_buf);
      if ((status == usbOK) && (mstatus != usbOK)) {
        status = mstatus;
      }
    }
  }
  EvrUSBH_HID_OnUninitialize(instance);
  USBH_HID_Uninitialize (instance);     // User level callback

  return status;
}


/************************** Local functions ***********************************/

/// \brief Check device instance pointers and variables
/// \param[in]     instance             index of HID instance.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus CheckInstance (uint8_t instance) {

  if (instance >= usbh_hid_num)                        { return usbClassErrorHID;   }
  if (usbh_hid[instance].ptr_dev == NULL)              { return usbDeviceError;     }
  if (usbh_hid[instance].ptr_dev->state.in_use == 0U)  { return usbDeviceError;     }
  if (usbh_hid[instance].ptr_dev->ctrl >= usbh_hc_num) { return usbControllerError; }

  return usbOK;
}


/***-------------- Class Specific Requests on Default Pipe -----------------***/

#if 0
/// \brief USB Human Interface Device Class Request - Get Report
/// \param[in]     instance             index of HID instance.
/// \param[in]     report_type          report type.
/// \param[in]     report_id            report ID.
/// \param[in]     interface            interface index.
/// \param[out]    report_data          pointer to where report data will be read.
/// \param[in]     report_length        report length.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_HID_GetReport (uint8_t instance, uint8_t report_type, uint8_t report_id, uint8_t interface, uint8_t *report_data, uint16_t report_length) {
  USB_SETUP_PACKET setup_packet;
  uint8_t          device;
  usbStatus        status;

  EvrUSBH_HID_GetReport(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {
    if (report_data != NULL) {
      device = USBH_HID_GetDevice (instance);
      if (device != 0xFFU) {
        PREPARE_SETUP_PACKET_DATA(((USB_SETUP_PACKET *)&setup_packet), USB_REQUEST_DEVICE_TO_HOST, USB_REQUEST_CLASS, USB_REQUEST_TO_INTERFACE, HID_REQUEST_GET_REPORT, (((uint32_t)report_type << 8) | (uint32_t)report_id), interface, report_length)
        status = USBH_ControlTransfer (device, &setup_packet, report_data, report_length);
      } else {
        status = usbDeviceError;
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_HID_GetReportFailed(instance, status);
  }
#endif
  return status;
}
#endif

/// \brief USB Human Interface Device Class Request - Set Report
/// \param[in]     instance             index of HID instance.
/// \param[in]     report_type          report type.
/// \param[in]     report_id            report ID.
/// \param[in]     interface            interface index.
/// \param[in]     report_data          pointer from where report data to send will be read from.
/// \param[in]     report_length        report length.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_HID_SetReport (uint8_t instance, uint8_t report_type, uint8_t report_id, uint8_t interface, uint8_t *report_data, uint16_t report_length) {
  USB_SETUP_PACKET setup_packet;
  uint8_t          device;
  usbStatus        status;

  EvrUSBH_HID_SetReport(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {
    if (report_data != NULL) {
      device = USBH_HID_GetDevice (instance);
      if (device != 0xFFU) {
        PREPARE_SETUP_PACKET_DATA(((USB_SETUP_PACKET *)&setup_packet), USB_REQUEST_HOST_TO_DEVICE, USB_REQUEST_CLASS, USB_REQUEST_TO_INTERFACE, HID_REQUEST_SET_REPORT, (((uint32_t)report_type << 8) | (uint32_t)report_id), interface, report_length)
        status = USBH_ControlTransfer (device, &setup_packet, report_data, report_length);
      } else {
        status = usbDeviceError;
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_HID_SetReportFailed(instance, status);
  }
#endif
  return status;
}

#if 0
/// \brief USB Human Interface Device Class Request - Get Idle
/// \param[in]     instance             index of HID instance.
/// \param[in]     report_id            report ID.
/// \param[in]     interface            interface index.
/// \param[out]    ptr_duration         pointer to where idle rate will be read.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_HID_GetIdle (uint8_t instance, uint8_t report_id, uint8_t interface, uint8_t *ptr_duration) {
  USB_SETUP_PACKET setup_packet;
  uint8_t          device;
  usbStatus        status;

  EvrUSBH_HID_GetIdle(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {
    if (ptr_duration != NULL) {
      device = USBH_HID_GetDevice (instance);
      if (device != 0xFFU) {
        PREPARE_SETUP_PACKET_DATA(((USB_SETUP_PACKET *)&setup_packet), USB_REQUEST_DEVICE_TO_HOST, USB_REQUEST_CLASS, USB_REQUEST_TO_INTERFACE, HID_REQUEST_GET_IDLE, report_id, interface, 1U)
        status = USBH_ControlTransfer (device, &setup_packet, ptr_duration, 1U);
      } else {
        status = usbDeviceError;
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_HID_GetIdleFailed(instance, status);
  }
#endif
  return status;
}
#endif

/// \brief USB Human Interface Device Class Request - Set Idle
/// \param[in]     instance             index of HID instance.
/// \param[in]     report_id            report ID.
/// \param[in]     interface            interface index.
/// \param[in]     duration             idle rate to be set.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_HID_SetIdle (uint8_t instance, uint8_t report_id, uint8_t interface, uint8_t duration) {
  USB_SETUP_PACKET setup_packet;
  uint8_t          device;
  usbStatus        status;

  EvrUSBH_HID_SetIdle(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {
    device = USBH_HID_GetDevice (instance);
    if (device != 0xFFU) {
      PREPARE_SETUP_PACKET_DATA(((USB_SETUP_PACKET *)&setup_packet), USB_REQUEST_HOST_TO_DEVICE, USB_REQUEST_CLASS, USB_REQUEST_TO_INTERFACE, HID_REQUEST_SET_IDLE, (((uint32_t)duration << 8) | (uint32_t)report_id), interface, 0)
      status = USBH_ControlTransfer (device, &setup_packet, NULL, 0U);
    } else {
      status = usbDeviceError;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_HID_SetIdleFailed(instance, status);
  }
#endif
  return status;
}

#if 0
/// \brief USB Human Interface Device Class Request - Get Protocol
/// \param[in]     instance             index of HID instance.
/// \param[in]     interface            interface index.
/// \param[out]    ptr_protocol         pointer to where active protocol will be read.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_HID_GetProtocol (uint8_t instance, uint8_t interface, uint8_t *ptr_protocol) {
  USB_SETUP_PACKET setup_packet;
  uint8_t          device;
  usbStatus        status;

  EvrUSBH_HID_GetProtocol(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {
    if (ptr_protocol != NULL) {
      device = USBH_HID_GetDevice (instance);
      if (device != 0xFFU) {
        PREPARE_SETUP_PACKET_DATA(((USB_SETUP_PACKET *)&setup_packet), USB_REQUEST_DEVICE_TO_HOST, USB_REQUEST_CLASS, USB_REQUEST_TO_INTERFACE, HID_REQUEST_GET_PROTOCOL, 0U, interface, 1U)
        status = USBH_ControlTransfer (device, &setup_packet, ptr_protocol, 1U);
      } else {
        status = usbDeviceError;
      }
    } else {
      status = usbInvalidParameter;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_HID_GetProtocolFailed(instance, status);
  }
#endif
  return status;
}
#endif

#if 0
/// \brief USB Human Interface Device Class Request - Set Protocol
/// \param[in]     instance             index of HID instance.
/// \param[in]     interface            interface index.
/// \param[in]     protocol             protocol to be set.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_HID_SetProtocol (uint8_t instance, uint8_t interface, uint8_t protocol) {
  USB_SETUP_PACKET setup_packet;
  uint8_t          device;
  usbStatus        status;

  EvrUSBH_HID_SetProtocol(instance);

  status = CheckInstance (instance);
  if (status == usbOK) {
    device = USBH_HID_GetDevice (instance);
    if (device != 0xFFU) {
      PREPARE_SETUP_PACKET_DATA(((USB_SETUP_PACKET *)&setup_packet), USB_REQUEST_HOST_TO_DEVICE, USB_REQUEST_CLASS, USB_REQUEST_TO_INTERFACE, HID_REQUEST_SET_PROTOCOL, protocol, interface, 0)
      status = USBH_ControlTransfer (device, &setup_packet, NULL, 0U);
    } else {
      status = usbDeviceError;
    }
  }

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_HID_SetProtocolFailed(instance, status);
  }
#endif
  return status;
}
#endif

/***-------------- Class Report Output function ----------------------------***/

/// \brief Send Output Report
/// \param[in]     instance             index of HID instance.
/// \param[in]     ptr_data             pointer to data to be sent in out report.
/// \param[in]     data_len             number of bytes to be sent in out report.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
static usbStatus USBH_HID_ReportOut (uint8_t instance, const uint8_t *ptr_data, uint16_t data_len) {
  const USBH_HID *ptr_hid;
        usbStatus status;

  EvrUSBH_HID_ReportOut(instance);

  if (ptr_data == NULL) {
    status = usbInvalidParameter;
    goto exit;
  }
  status = CheckInstance (instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_hid = &usbh_hid[instance];

  if (ptr_hid->int_out_pipe_hndl != 0U) {
    status = USBH_PipeSend (ptr_hid->int_out_pipe_hndl, (uint8_t *)((uint32_t)ptr_data), data_len);
  } else {
    status = USBH_HID_SetReport (instance, 2, 0, 0, (uint8_t *)((uint32_t)ptr_data), data_len);
  }

exit:
#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBH_HID_ReportOutFailed(instance, status);
  }
#endif
  return status;
}


/************************** Threads *******************************************/

/// \brief USB Host HID Interrupt In Pipe handling Thread
/// \param[in]     arg                  index of HID instance
void USBH_HID_IntIn_Thread (void *arg) {
  USBH_HID *ptr_hid;
  uint32_t  len;
  uint8_t   instance;

  instance = (uint8_t)((uint32_t)arg);
  if (instance >= usbh_hid_num) {
    return;
  }
  if (CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_hid = &usbh_hid[instance];

  for (;;) {
    if (USBH_PipeReceive (ptr_hid->int_in_pipe_hndl, ptr_hid->ptr_int_in_buf, ((USBH_PIPE *)ptr_hid->int_in_pipe_hndl)->wMaxPacketSize) == usbOK) {
      len = USBH_PipeReceiveGetResult (ptr_hid->int_in_pipe_hndl);
      if ((len <= 9U) && (ptr_hid->hid_in_data_len == 0U)) {
        if (len <= 8U) {
          memcpy((void *)ptr_hid->hid_in_data_buf_ex, (void *)ptr_hid->hid_in_data_buf, len);
          memcpy((void *)ptr_hid->hid_in_data_buf   , (void *)ptr_hid->ptr_int_in_buf , len);
        } else {
          // If Input Report length is 9 bytes, for our HID implementation, 
          // assume first byte is Report ID and skip it
          memcpy((void *)ptr_hid->hid_in_data_buf_ex, (void *)ptr_hid->hid_in_data_buf,    8);
          memcpy((void *)ptr_hid->hid_in_data_buf   , (void *)(ptr_hid->ptr_int_in_buf+1), 8);
        }
      }
      if (len <= (((USBH_PIPE *)ptr_hid->int_in_pipe_hndl)->wMaxPacketSize)) {
        ptr_hid->hid_in_data_len = (uint16_t)len;
      }
      EvrUSBH_HID_OnDataReceived(instance, len);
      USBH_HID_DataReceived (instance, len);
    } else {
      ptr_hid->int_in_thread_id = NULL;
      (void)USBH_ThreadTerminate (USBH_ThreadGetHandle());
      return;
    }
  }
}
