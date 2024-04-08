/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_hid.c
 * Purpose: USB Device - Human Interface Device (HID) module
 *----------------------------------------------------------------------------*/

#include "usbd_lib_hid.h"

#include <string.h>

#include "rl_usbd_lib.h"
#include "usb_lib_rte.h"
#include "usbd_lib_config_def.h"
#include "usbd_lib_drv.h"
#include "usbd_evr.h"


/************************** Constant definitions ******************************/

extern const uint16_t INTERVAL_HS[16];
       const uint16_t INTERVAL_HS[16] = { 1, 1, 1, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 };


/************************** Local function declarations ***********************/

static usbStatus USBD_HID_CheckInstance (uint8_t instance);
static void      USBD_HID_EpIntIn       (uint8_t instance);
static void      USBD_HID_EpIntOut      (uint8_t instance);


/************************** User accessible functions *************************/

/// \brief Asynchronously prepare HID Report data to send
/// \param[in]     instance             instance of HID class.
/// \param[in]     rid                  report ID.
/// \param[in]     buf                  buffer containing report data to send.
/// \param[in]     len                  number of report data bytes to send.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_HID_GetReportTrigger (uint8_t instance, uint8_t rid, const uint8_t *buf, uint32_t len) {
  const usbd_data_t     *ptr_dev_data;
  const usbd_hid_t      *ptr_hid_cfg;
        usbd_hid_data_t *ptr_hid_data;
        usbStatus        status;

  EvrUSBD_HID_GetReportTrigger(instance, rid, buf, len);

  status = USBD_HID_CheckInstance (instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_hid_cfg  = usbd_hid_ptr[instance];
  ptr_hid_data = ptr_hid_cfg->data_ptr;
  ptr_dev_data = usbd_dev_ptr[ptr_hid_cfg->dev_num]->data_ptr;

  if (len > ptr_hid_cfg->in_report_max_sz) {
    status = usbInvalidParameter;
    goto exit;
  }

  if (ptr_dev_data->configuration == 0U) {
    status = usbNotConfigured;
    goto exit;
  }

  if (USBD_SemaphoreAcquire (usbd_hid_semaphore_id[instance], 0U) != 0) {
    status = usbDriverBusy;
    goto exit;
  }

  ptr_hid_data->data_out_async_req     = true;                  // Async data out request
  ptr_hid_cfg->in_report[0]            = rid;                   // Set first byte to report ID
  memcpy ((void *)&ptr_hid_cfg->in_report[1], buf, len);        // Copy data to buffer
  ptr_hid_data->ptr_data_out           = ptr_hid_cfg->in_report;
  ptr_hid_data->data_out_sent_len      = 0U;
  ptr_hid_data->data_out_to_send_len   = (uint16_t)len;
  if (ptr_hid_cfg->in_report_num <= 1U) {                       // If only 1 IN report skip ReportID
    ptr_hid_data->ptr_data_out++;
  } else {                                                      // If more IN reports, send ReportID
    ptr_hid_data->data_out_to_send_len++;
  }
  ptr_hid_cfg->idle_count[rid]         = 0U;
  USBD_HID_EpIntIn (instance);

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_HID_GetReportTriggerFailed(instance, status);
  }
#endif

  return status;
}


/************************** Library accessible functions **********************/

/// \brief HID Get Report request
/// \param[in]     instance      instance of HID class.
/// \return        true          success
/// \return        false         fail, not supported request
bool USBD_HID_CoreGetReport (uint8_t instance) {
  const usbd_data_t *ptr_dev_data;
        uint8_t     *ptr_buf;
        int32_t      len;

  EvrUSBD_HID_GetReport(instance);

  ptr_dev_data = usbd_dev_ptr[usbd_hid_ptr[instance]->dev_num]->data_ptr;

  if (USBD_HID_CheckInstance(instance) != usbOK) {
    goto failed;
  }

  // Report Type   = setup_packet.wValue high byte
  // Report ID     = setup_packet.wValue low byte
  // Report Length = setup_packet.wLength
  switch (ptr_dev_data->setup_packet.wValue >> 8) {
    case HID_REPORT_INPUT:
      ptr_buf = (uint8_t *)&usbd_hid_ptr[instance]->in_report[1];
      break;
    case HID_REPORT_OUTPUT:             // Not supported
      goto failed;
    case HID_REPORT_FEATURE:
      ptr_buf = (uint8_t *)&usbd_hid_ptr[instance]->feat_report[1];
      break;
    default:
      goto failed;
  }

  if (fpUSBD_HID_GetReport[instance] != NULL) {
    len = fpUSBD_HID_GetReport[instance] ((uint8_t)(ptr_dev_data->setup_packet.wValue >> 8), (uint8_t)USBD_HID_REQ_EP_CTRL, (uint8_t)(ptr_dev_data->setup_packet.wValue), ptr_buf);
    if (len >= 0) {
      EvrUSBD_HID_OnGetReport(instance, (uint8_t)(ptr_dev_data->setup_packet.wValue >> 8), (uint8_t)USBD_HID_REQ_EP_CTRL, (uint8_t)(ptr_dev_data->setup_packet.wValue), len);
      return true;
    }
  }

failed:
  EvrUSBD_HID_OnGetReportFailed(instance, (uint8_t)(ptr_dev_data->setup_packet.wValue >> 8), (uint8_t)USBD_HID_REQ_EP_CTRL, (uint8_t)(ptr_dev_data->setup_packet.wValue));
  EvrUSBD_HID_GetReportFailed(instance);
  return false;
}

/// \brief HID Set Report request
/// \param[in]     instance      instance of HID class.
/// \return        true          success
/// \return        false         fail, not supported request
bool USBD_HID_CoreSetReport (uint8_t instance) {
  const usbd_data_t *ptr_dev_data;
  const uint8_t     *ptr_buf;
        bool         result;

  EvrUSBD_HID_SetReport(instance);

  ptr_dev_data = usbd_dev_ptr[usbd_hid_ptr[instance]->dev_num]->data_ptr;

  result = false;
  if (USBD_HID_CheckInstance(instance) != usbOK) {
    goto failed;
  }

  // Report Type   = setup_packet.wValue high byte
  // Report ID     = setup_packet.wValue low byte
  // Report Length = setup_packet.wLength
  switch (ptr_dev_data->setup_packet.wValue >> 8) {
    case HID_REPORT_INPUT:              // Not supported
      goto failed;
    case HID_REPORT_OUTPUT:
      ptr_buf = &usbd_hid_ptr[instance]->out_report[1];
      break;
    case HID_REPORT_FEATURE:
      ptr_buf = &usbd_hid_ptr[instance]->feat_report[1];
      break;
    default:
      goto failed;
  }

  if (fpUSBD_HID_SetReport[instance] != NULL) {
    result = fpUSBD_HID_SetReport[instance] ((uint8_t)(ptr_dev_data->setup_packet.wValue >> 8), (uint8_t)USBD_HID_REQ_EP_CTRL, (uint8_t)(ptr_dev_data->setup_packet.wValue), ptr_buf, (int32_t)(ptr_dev_data->setup_packet.wLength));
    if (result) {
      EvrUSBD_HID_OnSetReport(instance, (uint8_t)(ptr_dev_data->setup_packet.wValue >> 8), (uint8_t)USBD_HID_REQ_EP_CTRL, (uint8_t)(ptr_dev_data->setup_packet.wValue), (int32_t)(ptr_dev_data->setup_packet.wLength));
      return true;
    }
  }

failed:
  EvrUSBD_HID_OnSetReportFailed(instance, (uint8_t)(ptr_dev_data->setup_packet.wValue >> 8), (uint8_t)USBD_HID_REQ_EP_CTRL, (uint8_t)(ptr_dev_data->setup_packet.wValue), (int32_t)(ptr_dev_data->setup_packet.wLength));
  EvrUSBD_HID_SetReportFailed(instance);
  return result;
}

/// \brief HID Get Idle request
/// \param[in]     instance      instance of HID class.
/// \return        true          success
/// \return        false         fail, not supported request
bool USBD_HID_CoreGetIdle (uint8_t instance) {
  const usbd_dev_t  *ptr_dev_cfg;
  const usbd_data_t *ptr_dev_data;
  const usbd_hid_t  *ptr_hid_cfg;

  if (USBD_HID_CheckInstance(instance) != usbOK) {
    EvrUSBD_HID_GetIdleFailed(instance);
    return false;
  }

  ptr_hid_cfg  = usbd_hid_ptr[instance];
  ptr_dev_cfg  = usbd_dev_ptr[ptr_hid_cfg->dev_num];
  ptr_dev_data = ptr_dev_cfg->data_ptr;

  EvrUSBD_HID_GetIdle(instance, (uint8_t)ptr_dev_data->setup_packet.wValue & 0xFFU, ptr_hid_cfg->idle_set[ptr_dev_data->setup_packet.wValue & 0xFFU]);
  ptr_dev_cfg->ep0_buf[0] = ptr_hid_cfg->idle_set[ptr_dev_data->setup_packet.wValue & 0xFFU];

  return true;
}

/// \brief HID Set Idle request
/// \param[in]     instance      instance of HID class.
/// \return        true          success
/// \return        false         fail, not supported request
bool USBD_HID_CoreSetIdle (uint8_t instance) {
  const usbd_data_t *ptr_dev_data;
        uint8_t      i;

  if (USBD_HID_CheckInstance(instance) != usbOK) {
    EvrUSBD_HID_SetIdleFailed(instance);
    return false;
  }

  ptr_dev_data = usbd_dev_ptr[usbd_hid_ptr[instance]->dev_num]->data_ptr;

  EvrUSBD_HID_SetIdle(instance, (uint8_t)ptr_dev_data->setup_packet.wValue & 0xFFU, (uint8_t)(ptr_dev_data->setup_packet.wValue >> 8));
  if ((ptr_dev_data->setup_packet.wValue & 0xFFU) != 0U) {      // If  > 0 Report ID specified
    usbd_hid_ptr[instance]->idle_set[(ptr_dev_data->setup_packet.wValue & 0xFFU)-1U] = (uint8_t)(ptr_dev_data->setup_packet.wValue >> 8);
  } else {                                                      // If == 0 all reports
    for (i = 0U; i < usbd_hid_ptr[instance]->in_report_num; i++) {
      usbd_hid_ptr[instance]->idle_set[i] = (uint8_t)(ptr_dev_data->setup_packet.wValue >> 8);
    }
  }

  if ((ptr_dev_data->setup_packet.wValue & 0xFFU) == 0U) {
    (void)USBD_TimerStop  (usbd_hid_timer_id[instance]);
  } else {
    (void)USBD_TimerStart (usbd_hid_timer_id[instance], 4U);
  }

  return true;
}

/// \brief HID Get Protocol request
/// \param[in]     instance      instance of HID class.
/// \return        true          success
/// \return        false         fail, not supported request
bool USBD_HID_CoreGetProtocol (uint8_t instance) {
  int8_t protocol;

  if (USBD_HID_CheckInstance(instance) != usbOK) {
    goto failed;
  }

  if (fpUSBD_HID_GetProtocol[instance] != NULL) {
    protocol = fpUSBD_HID_GetProtocol[instance] () ;
    EvrUSBD_HID_GetProtocol(instance, protocol);
    if (protocol >= 0) {
      usbd_dev_ptr[usbd_hid_ptr[instance]->dev_num]->ep0_buf[0] = (uint8_t)protocol;
      return true;
    }
  }

failed:
  EvrUSBD_HID_GetProtocolFailed(instance);
  return false;
}

/// \brief HID Set Protocol request
/// \param[in]     instance      instance of HID class.
/// \return        true          success
/// \return        false         fail, not supported request
bool USBD_HID_CoreSetProtocol (uint8_t instance) {
  const usbd_data_t *ptr_dev_data;
        bool         result;

  if (USBD_HID_CheckInstance(instance) != usbOK) {
    goto failed;
  }

  if (fpUSBD_HID_SetProtocol[instance] != NULL) {
    ptr_dev_data = usbd_dev_ptr[usbd_hid_ptr[instance]->dev_num]->data_ptr;

    result = fpUSBD_HID_SetProtocol[instance] ((uint8_t)(ptr_dev_data->setup_packet.wValue & 0xFFU));
    if (result) {
      EvrUSBD_HID_SetProtocol(instance, (uint8_t)ptr_dev_data->setup_packet.wValue & 0xFFU);
      return true;
    }
  }

failed:
  EvrUSBD_HID_SetProtocolFailed(instance);
  return false;
}


/************************** Configuration accessible functions ****************/

/// \brief Initialize HID device instance and create event handling Thread
/// \param[in]     instance      instance of HID class.
/// \return                      status code that indicates the execution status of the function.
usbStatus USBD_HID_Initialize (uint8_t instance) {
  usbStatus status;

  EvrUSBD_HID_Initialize(instance);

  status = USBD_HID_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  if (fpUSBD_HID_Initialize [instance] != NULL) {
    EvrUSBD_HID_OnInitialize(instance);
    fpUSBD_HID_Initialize   [instance] ();
  }

  if (usbd_hid_thread_id[instance] == NULL) {
    usbd_hid_thread_id[instance] = USBD_ThreadCreate (usbdThreadHID, instance);
    if (usbd_hid_thread_id[instance] == NULL) {
      status = usbThreadError;
      goto exit;
    }
  }
  if (usbd_hid_timer_id[instance] == NULL) {
    usbd_hid_timer_id[instance] = USBD_TimerCreate (instance);
    if (usbd_hid_timer_id[instance] == NULL) {
      status = usbTimerError;
      goto exit;
    }
  }
  if (usbd_hid_semaphore_id[instance] == NULL) {
    usbd_hid_semaphore_id[instance] = USBD_SemaphoreCreate (usbdSemaphoreHID, instance, 0U);
    if (usbd_hid_semaphore_id[instance] == NULL) {
      status = usbSemaphoreError;
    }
  }

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_HID_InitializeFailed(instance, status);
  }
#endif

  return status;
}

/// \brief De-initialize HID device instance and terminate event handling Thread
/// \param[in]     instance      instance of HID class.
/// \return                      status code that indicates the execution status of the function.
usbStatus USBD_HID_Uninitialize (uint8_t instance) {
  usbStatus status;

  EvrUSBD_HID_Uninitialize(instance);

  status = USBD_HID_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  if (usbd_hid_semaphore_id[instance] != NULL) {
    if (USBD_SemaphoreDelete (usbd_hid_semaphore_id[instance]) != 0) {
      status = usbSemaphoreError;
      goto exit;
    }
    usbd_hid_semaphore_id[instance] = NULL;
  }
  if (usbd_hid_thread_id[instance] != NULL) {
    if (USBD_ThreadTerminate (usbd_hid_thread_id[instance]) != 0) {
      status = usbThreadError;
      goto exit;
    }
    usbd_hid_thread_id[instance] = NULL;
  }

  if (fpUSBD_HID_Uninitialize [instance] != NULL) {
    EvrUSBD_HID_OnUninitialize(instance);
    fpUSBD_HID_Uninitialize   [instance] ();
  }

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_HID_UninitializeFailed(instance, status);
  }
#endif

  return status;
}

/// \brief HID USB Bus Reset event handling
/// \param[in]     instance      instance of HID class.
void USBD_HID_Reset (uint8_t instance) {
  const usbd_hid_t      *ptr_hid_cfg;
        usbd_hid_data_t *ptr_hid_data;
        usbStatus        status;

  EvrUSBD_HID_Reset(instance);

  status = USBD_HID_CheckInstance(instance);
  if (status != usbOK) {
    EvrUSBD_HID_ResetFailed(instance, status);
    return;
  }

  ptr_hid_cfg  = usbd_hid_ptr[instance];
  ptr_hid_data = ptr_hid_cfg->data_ptr;

  if (usbd_hid_timer_id[instance] != NULL) {
    (void)USBD_TimerStop  (usbd_hid_timer_id[instance]);
  }

  if (ptr_hid_cfg->ep_int_in != 0U) {
    (void)USBD_DriverEndpointTransferAbort(ptr_hid_cfg->dev_num, USB_ENDPOINT_IN(ptr_hid_cfg->ep_int_in));
  }
  if (ptr_hid_cfg->ep_int_out != 0U) {
    (void)USBD_DriverEndpointTransferAbort(ptr_hid_cfg->dev_num, USB_ENDPOINT_OUT(ptr_hid_cfg->ep_int_out));
  }
  if (usbd_hid_semaphore_id[instance] != NULL) {
    // Dummy acquire and release semaphore (if device got disconnected in the middle of 
    // transfer semaphore needs to be released)
    (void)USBD_SemaphoreAcquire (usbd_hid_semaphore_id[instance], 0U);
    (void)USBD_SemaphoreRelease (usbd_hid_semaphore_id[instance]);
  }

  memset ((void *)ptr_hid_data, 0, sizeof(usbd_hid_data_t));

  ptr_hid_data->ptr_data_in = ptr_hid_cfg->out_report;
}

/// \brief HID Set Configuration request handling
/// \param[in]     instance      instance of HID class.
void USBD_HID_SetConfiguration (uint8_t instance) {
  uint8_t i;

  if (USBD_HID_CheckInstance(instance) != usbOK) {
    return;
  }

  for (i = 0U; i < usbd_hid_ptr[instance]->in_report_num; i++) {
    usbd_hid_ptr[instance]->idle_set[i] = 125U;
  }

  (void)USBD_TimerStart (usbd_hid_timer_id[instance], 4U);
}

/// \brief HID Endpoint Start handling
/// \param[in]     instance      instance of HID class.
/// \param[in]     ep_addr       endpoint address.
///                 - ep_addr.0..3: address
///                 - ep_addr.7:    direction
void USBD_HID_EndpointStart (uint8_t instance, uint8_t ep_addr) {
  const usbd_hid_t *ptr_hid_cfg;

  if (USBD_HID_CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_hid_cfg  = usbd_hid_ptr[instance];

  if (USB_ENDPOINT_OUT(ptr_hid_cfg->ep_int_out) == ep_addr) {
    (void)USBD_DriverEndpointTransfer(ptr_hid_cfg->dev_num, USB_ENDPOINT_OUT(ptr_hid_cfg->ep_int_out), ptr_hid_cfg->data_ptr->ptr_data_in, ptr_hid_cfg->out_report_max_sz);
  }
}

/// \brief HID Timer event handling (handling report timings: polling and idle times)(called every 4 ms).
/// \param[in]     instance      instance of HID class.
void USBD_HID_Timer (void const *argument) {
  const usbd_data_t     *ptr_dev_data;
  const usbd_hid_t      *ptr_hid_cfg;
        usbd_hid_data_t *ptr_hid_data;
        uint16_t         polling_interval;
        uint16_t         idle_set_in_4;
        uint8_t          instance, i;
        bool             do_polling, polling_reload, idle_reload;

  instance = (uint8_t)((uint32_t)argument);

  if (USBD_HID_CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_hid_cfg  = usbd_hid_ptr[instance];
  ptr_hid_data = ptr_hid_cfg->data_ptr;
  ptr_dev_data = usbd_dev_ptr[ptr_hid_cfg->dev_num]->data_ptr;

  if (ptr_dev_data->configuration != 0U) {
    ptr_hid_data->polling_count += 4U;
    if (ptr_dev_data->high_speed != 0U) {
      if (ptr_hid_cfg->ep_int_in_interval[1] == 0U) {
        polling_interval = 1U;
      } else {
        polling_interval = INTERVAL_HS[ptr_hid_cfg->ep_int_in_interval[1]-1U];
      }
    } else {
      if (ptr_hid_cfg->ep_int_in_interval[0] == 0U) {
        polling_interval = 1U;
      } else {
        polling_interval = ptr_hid_cfg->ep_int_in_interval[0];
      }
    }
    polling_reload = false;
    if (ptr_hid_data->polling_count == polling_interval) {
      ptr_hid_data->polling_count = 0U;
      polling_reload = true;            // If polling interval expired
    }

    for (i = 0U; i < ptr_hid_cfg->in_report_num; i++) {
      idle_reload = false;
      if (ptr_hid_cfg->idle_count[i] < 0xFFFCU) {
        ptr_hid_cfg->idle_count[i] += 4U;
      }
      if (ptr_hid_cfg->idle_reload[i] != 0U) {
        if (ptr_hid_cfg->idle_count[i] >= ptr_hid_cfg->idle_reload[i]) {
          ptr_hid_cfg->idle_count[i] = 0U;
          idle_reload = true;           // If idle period expired
        }
      }
      do_polling = (polling_interval > ((uint16_t)(ptr_hid_cfg->idle_set[i]) << 2)) && (ptr_hid_cfg->idle_set[i] != 0U);
      if ((( do_polling) && (polling_reload))  ||
          ((!do_polling) && (idle_reload   )))  {
          ptr_hid_data->data_out_update_req_mask |= (1UL << i);
      }
      idle_set_in_4 = (uint16_t)((uint32_t)ptr_hid_cfg->idle_set[i] << 2);
      if (ptr_hid_cfg->idle_reload[i] != idle_set_in_4) {
                                        // If idle set changed
        if ((idle_set_in_4 != 0U) && (ptr_hid_cfg->idle_count[i] >= idle_set_in_4)) {
          ptr_hid_data->data_out_update_req_mask |= (1UL << i);
        }
        ptr_hid_cfg->idle_reload[i] = idle_set_in_4;
      }
    }

                                        // If pending
    if ((ptr_hid_data->data_out_update_req_mask != 0U) && (!ptr_hid_data->data_out_async_req)) {
                                        // refresh request and no active data
                                        // out then start data out
      if (USBD_SemaphoreAcquire (usbd_hid_semaphore_id[instance], 0U) == 0) {
        USBD_HID_EpIntIn (instance);
      }
    } else {
      ptr_hid_data->data_out_update_req_mask &= ~(1UL << i);
    }
  }
}


/************************** Local functions ***********************************/

/// \brief Check all HID device instance pointers and variables
/// \param[in]     instance      instance of HID class.
/// \return                      status code that indicates the execution status of the function.
static usbStatus USBD_HID_CheckInstance (uint8_t instance) {

  if (instance >= usbd_hid_num)                                        { return usbClassErrorHID; }
  if (usbd_hid_ptr[instance]                                  == NULL) { return usbClassErrorHID; }
  if (usbd_hid_ptr[instance]->data_ptr                        == NULL) { return usbClassErrorHID; }
  if (usbd_hid_ptr[instance]->dev_num >= usbd_dev_num)                 { return usbDeviceError;   }
  if (usbd_dev_ptr[usbd_hid_ptr[instance]->dev_num]           == NULL) { return usbDeviceError;   }
  if (usbd_dev_ptr[usbd_hid_ptr[instance]->dev_num]->data_ptr == NULL) { return usbDeviceError;   }

  return usbOK;
}

/// \brief HID Interrupt In Endpoint event handling
/// \param[in]     instance      instance of HID class.
static void USBD_HID_EpIntIn (uint8_t instance) {
  const  usbd_data_t     *ptr_dev_data;
  const  usbd_hid_t      *ptr_hid_cfg;
         usbd_hid_data_t *ptr_hid_data;
         uint16_t         bytes_to_send;
         uint8_t          i;

  if (USBD_HID_CheckInstance(instance) != usbOK) { return; }

  ptr_hid_cfg  = usbd_hid_ptr[instance];
  ptr_hid_data = ptr_hid_cfg->data_ptr;

  // Check if sending is finished
  // If all sent and short packet also
  if (( ptr_hid_data->data_out_sent_len >= ptr_hid_data->data_out_to_send_len) &&
      (!ptr_hid_data->data_out_end_with_zlp_packet)) {
    ptr_hid_data->ptr_data_out          = NULL;
    ptr_hid_data->data_out_sent_len     = 0U;
    if (fpUSBD_HID_GetReport[instance] != NULL) {
      ptr_hid_data->data_out_to_send_len  = (uint16_t)(fpUSBD_HID_GetReport[instance] (HID_REPORT_INPUT, (uint8_t)USBD_HID_REQ_EP_INT, ptr_hid_cfg->in_report[0], &ptr_hid_cfg->in_report[1]));
      EvrUSBD_HID_OnGetReport(instance, HID_REPORT_INPUT, (uint8_t)USBD_HID_REQ_EP_INT, ptr_hid_cfg->in_report[0], (int32_t)ptr_hid_data->data_out_to_send_len);
    }

    if (ptr_hid_data->data_out_to_send_len != 0U) {     // If new send should start
      ptr_hid_data->ptr_data_out        = ptr_hid_cfg->in_report;
                                        // If only in 1 report skip ReportID
      if (ptr_hid_cfg->in_report_num <= 1U) {
        ptr_hid_data->ptr_data_out++;
      } else {                          // If more in reports, send ReportID
        ptr_hid_data->data_out_to_send_len++;
      }
    }
  }
  // Check if new data out sending should be started
  if(( ptr_hid_data->data_out_to_send_len == 0U) &&     // If send not in progress
     (!ptr_hid_data->data_out_end_with_zlp_packet)) {
    if (ptr_hid_data->data_out_async_req) {             // If async send requested
      ptr_hid_data->data_out_async_req = false;
    } else {
      if (ptr_hid_data->data_out_update_req_mask != 0U){// If update requested
                                        // If only one in report in system
        if (ptr_hid_cfg->in_report_num <= 1U) {
          if (ptr_hid_data->data_out_update_req_mask != 0U) {
            ptr_hid_cfg->in_report[0]  = 0U;            // ReportID = 0
            ptr_hid_data->data_out_sent_len    = 0U;
            if (fpUSBD_HID_GetReport[instance] != NULL) {
              ptr_hid_data->data_out_to_send_len = (uint16_t)(fpUSBD_HID_GetReport[instance] (HID_REPORT_INPUT, (uint8_t)USBD_HID_REQ_PERIOD_UPDATE, 0U, &ptr_hid_cfg->in_report[1]));
              EvrUSBD_HID_OnGetReport(instance, HID_REPORT_INPUT, (uint8_t)USBD_HID_REQ_PERIOD_UPDATE, 0U, (int32_t)ptr_hid_data->data_out_to_send_len);
            }
            if (ptr_hid_data->data_out_to_send_len != 0U) {
              ptr_hid_data->ptr_data_out       = &ptr_hid_cfg->in_report[1];
            }
            ptr_hid_data->data_out_update_req_mask  = 0U;
          }
        } else {                        // If multiple reports in system
          i = ptr_hid_data->last_in_report + 1U;
          do {
            if (i > ptr_hid_cfg->in_report_num) {
              i = 0U;
            }
            if ((ptr_hid_data->data_out_update_req_mask & (1UL << i)) != 0U) {
              ptr_hid_cfg->in_report[0]= i + 1U;        // ReportID
              ptr_hid_data->data_out_sent_len    = 0U;
              if (fpUSBD_HID_GetReport[instance] != NULL) {
                ptr_hid_data->data_out_to_send_len = (uint16_t)(fpUSBD_HID_GetReport[instance] (HID_REPORT_INPUT, (uint8_t)USBD_HID_REQ_PERIOD_UPDATE, i + 1U, &ptr_hid_cfg->in_report[1]));
                EvrUSBD_HID_OnGetReport(instance, HID_REPORT_INPUT, (uint8_t)USBD_HID_REQ_PERIOD_UPDATE, i + 1U, (int32_t)ptr_hid_data->data_out_to_send_len);
              }
              if (ptr_hid_data->data_out_to_send_len != 0U) {
                ptr_hid_data->ptr_data_out       = ptr_hid_cfg->in_report;
                ptr_hid_data->data_out_to_send_len++;
              }
              ptr_hid_data->data_out_update_req_mask &= ~(1UL << i);
              break;
            }
          } while (i != ptr_hid_data->last_in_report);
        }
      }
    }
  }

  // Check if data needs to be sent
  // If sending is in progress
  if ((ptr_hid_data->data_out_to_send_len != 0U) ||
      (ptr_hid_data->data_out_end_with_zlp_packet)) {
    ptr_dev_data = usbd_dev_ptr[ptr_hid_cfg->dev_num]->data_ptr;

    bytes_to_send = ptr_hid_data->data_out_to_send_len - ptr_hid_data->data_out_sent_len;
    (void)USBD_DriverEndpointTransfer(ptr_hid_cfg->dev_num, USB_ENDPOINT_IN(ptr_hid_cfg->ep_int_in), ptr_hid_data->ptr_data_out, bytes_to_send);
    ptr_hid_data->ptr_data_out      += bytes_to_send;
    ptr_hid_data->data_out_sent_len += bytes_to_send;
    if ((ptr_hid_data->data_out_sent_len < ptr_hid_cfg->in_report_max_sz) &&
       ((bytes_to_send % ptr_hid_cfg->ep_int_in_max_packet_size[ptr_dev_data->high_speed]) == 0U)) {
                                                        // If ZLP packet should be sent also
      ptr_hid_data->data_out_end_with_zlp_packet = true;
    } else {
      ptr_hid_data->data_out_end_with_zlp_packet = false;
    }
  } else {
    (void)USBD_SemaphoreRelease (usbd_hid_semaphore_id[instance]);
  }
}

/// \brief HID Interrupt Out Endpoint event handling
/// \param[in]     instance      instance of HID class.
static void USBD_HID_EpIntOut (uint8_t instance) {
  const usbd_hid_t      *ptr_hid_cfg;
        usbd_hid_data_t *ptr_hid_data;
        uint32_t         bytes_rece;
        uint8_t          device;
        bool             result;

  if (USBD_HID_CheckInstance(instance) != usbOK) { return; }

  ptr_hid_cfg  = usbd_hid_ptr[instance];
  device       = ptr_hid_cfg->dev_num;
  ptr_hid_data = ptr_hid_cfg->data_ptr;

  if (ptr_hid_data->data_in_rece_len == 0U) {           // Check if new reception
    ptr_hid_data->ptr_data_in      = ptr_hid_cfg->out_report;
    ptr_hid_data->data_in_rece_len = 0U;
  }
  bytes_rece   = USBD_DriverEndpointTransferGetResult(device, USB_ENDPOINT_OUT(ptr_hid_cfg->ep_int_out));
  ptr_hid_data->ptr_data_in      += (uint16_t)bytes_rece;
  ptr_hid_data->data_in_rece_len += (uint16_t)bytes_rece;
                                        // If only one out report in system
  if (ptr_hid_cfg->out_report_num <= 1U) {
    if (fpUSBD_HID_SetReport[instance] != NULL) {
      result = fpUSBD_HID_SetReport[instance] (HID_REPORT_OUTPUT, (uint8_t)USBD_HID_REQ_EP_INT, 0U,  ptr_hid_cfg->out_report, (int32_t)(ptr_hid_data->data_in_rece_len));
      if (result) {
        EvrUSBD_HID_OnSetReport(instance, HID_REPORT_OUTPUT, (uint8_t)USBD_HID_REQ_EP_INT, 0U, (int32_t)(ptr_hid_data->data_in_rece_len));
      } else {
        EvrUSBD_HID_OnSetReportFailed(instance, HID_REPORT_OUTPUT, (uint8_t)USBD_HID_REQ_EP_INT, 0U, (int32_t)(ptr_hid_data->data_in_rece_len));
        return;
      }
    }
  } else {
    if (fpUSBD_HID_SetReport[instance] != NULL) {
      result = fpUSBD_HID_SetReport[instance] (HID_REPORT_OUTPUT, (uint8_t)USBD_HID_REQ_EP_INT, ptr_hid_cfg->out_report[0], (const uint8_t *)&ptr_hid_cfg->out_report[1], (int32_t)ptr_hid_data->data_in_rece_len-1);
      if (result) {
        EvrUSBD_HID_OnSetReport(instance, HID_REPORT_OUTPUT, (uint8_t)USBD_HID_REQ_EP_INT, ptr_hid_cfg->out_report[0], (int32_t)ptr_hid_data->data_in_rece_len - 1);
      } else {
        EvrUSBD_HID_OnSetReportFailed(instance, HID_REPORT_OUTPUT, (uint8_t)USBD_HID_REQ_EP_INT, ptr_hid_cfg->out_report[0], (int32_t)ptr_hid_data->data_in_rece_len-1);
        return;
      }
    }
  }
  ptr_hid_data->ptr_data_in      = ptr_hid_cfg->out_report;
  ptr_hid_data->data_in_rece_len = 0U;
  (void)USBD_DriverEndpointTransfer(device, USB_ENDPOINT_OUT(ptr_hid_cfg->ep_int_out), ptr_hid_data->ptr_data_in, ptr_hid_cfg->out_report_max_sz);
}


/************************** Thread functions **********************************/

/// \brief Thread Function: HID Interrupt Endpoint events handling
/// \param[in]     arg                  index of HID instance
void USBD_HID_Thread (void *arg) {
  uint32_t event;
  uint8_t  instance;

  instance = (uint8_t)((uint32_t)arg);
  if (USBD_HID_CheckInstance(instance) != usbOK) {
    return;
  }

  for (;;) {
    event = USBD_ThreadFlagsWait (0xFFFFFFFFU);
    if ((event & 0x8000000U) == 0U) {
      if (((event >> 8) & ARM_USBD_EVENT_OUT) != 0U) {
        USBD_HID_EpIntOut (instance);
      }
      if (((event >> 8) & ARM_USBD_EVENT_IN)  != 0U) {
        USBD_HID_EpIntIn  (instance);
      }
    }
  }
}

void USBD_HID0_Thread (void const *arg) { (void)arg; USBD_HID_Thread ((void *)0U); }
void USBD_HID1_Thread (void const *arg) { (void)arg; USBD_HID_Thread ((void *)1U); }
void USBD_HID2_Thread (void const *arg) { (void)arg; USBD_HID_Thread ((void *)2U); }
void USBD_HID3_Thread (void const *arg) { (void)arg; USBD_HID_Thread ((void *)3U); }
void USBD_HID4_Thread (void const *arg) { (void)arg; USBD_HID_Thread ((void *)4U); }
void USBD_HID5_Thread (void const *arg) { (void)arg; USBD_HID_Thread ((void *)5U); }
void USBD_HID6_Thread (void const *arg) { (void)arg; USBD_HID_Thread ((void *)6U); }
void USBD_HID7_Thread (void const *arg) { (void)arg; USBD_HID_Thread ((void *)7U); }
