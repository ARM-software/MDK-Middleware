/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_cdc.c
 * Purpose: USB Device - Communication Device Class (CDC) module
 *----------------------------------------------------------------------------*/

#include "usbd_lib_cdc.h"

#include <string.h>

#include "rl_usbd_lib.h"
#include "usb_lib_debug.h"
#include "usbd_lib_config_def.h"
#include "usbd_lib_drv.h"
#include "usbd_evr.h"


/************************** Constant definitions ******************************/

static const uint8_t NTH16[] = { (uint8_t)'N', (uint8_t)'C', (uint8_t)'M', (uint8_t)'H', 0x0CU, 0x00U, 0x00U, 0x00U, 0x0CU, 0x00U, 0x0CU, 0x00U };
static const uint8_t NTH32[] = { (uint8_t)'n', (uint8_t)'c', (uint8_t)'m', (uint8_t)'h', 0x10U, 0x00U, 0x00U, 0x00U, 0x10U, 0x00U, 0x00U, 0x00U, 0x10U, 0x00U, 0x00U, 0x00U };


/************************** Local function declarations ***********************/

static usbStatus USBD_CDC_CheckInstance (uint8_t instance);
static void      USBD_CDC_EpIntIn       (uint8_t instance);
static void      USBD_CDC_EpBulkOut     (uint8_t instance);
static void      USBD_CDC_EpBulkIn      (uint8_t instance);


/************************** User accessible functions *************************/

/// \brief Read multiple data bytes received by Communication Device from USB Host
/// \param[in]     instance             instance of CDC class.
/// \param[out]    buf                  buffer that receives data.
/// \param[in]     len                  maximum number of bytes to read.
/// \return                             number of bytes read or execution status :
///                                       - value >= 0 : number of bytes read
///                                       - value < 0 :  error occurred, -value is execution status as defined with \ref usbStatus
int32_t USBD_CDC_ACM_ReadData (uint8_t instance, uint8_t *buf, int32_t len) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
        uint32_t         max_packet_size, len_data, len_till_wrap, len_req = 0U, len_loc;
        usbStatus        status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_cdc_cfg     = usbd_cdc_ptr[instance];
  ptr_cdc_data    = ptr_cdc_cfg->data_ptr;
  max_packet_size = ptr_cdc_cfg->max_packet_size1[usbd_dev_ptr[ptr_cdc_cfg->dev_num]->data_ptr->high_speed];

  if (ptr_cdc_cfg->acm == 0U) {         // If not ACM class, exit function with error
    status = usbClassErrorCDC;
    goto exit;
  }

  if (ptr_cdc_cfg->rndis == 0U) {       // If VirtualCOM
    len_data = ptr_cdc_data->data_received_cnt - ptr_cdc_data->data_read_cnt;
    if (len_data == 0U) {               // If no data available
      len_req = 0U;
      goto chk_act;                     // Check if new receive activation is needed
    }

    len_req = (uint32_t)len;            // Number of bytes requested for reading
    if (len_req > len_data) {           // If more requested then available
      len_req = len_data;               // correct to return maximum available
    }

    len_till_wrap = ((uint32_t)ptr_cdc_cfg->receive_buf_sz - max_packet_size) - ((uint32_t)ptr_cdc_data->ptr_data_read - (uint32_t)ptr_cdc_cfg->receive_buf);
    len_loc       = len_req;
    if (len_loc >= len_till_wrap) {     // If more data then until end of buffer (wrap)
                                        // Copy data till end of buffer
      memcpy (buf, ptr_cdc_data->ptr_data_read, len_till_wrap);
      buf                         += len_till_wrap;
      len_loc                     -= len_till_wrap;
      ptr_cdc_data->ptr_data_read  = ptr_cdc_cfg->receive_buf;
      ptr_cdc_data->data_read_cnt += len_till_wrap;
    }
    if (len_loc != 0U) {                // If there is data to be copied
                                        // Copy remaining received data
      memcpy (buf, ptr_cdc_data->ptr_data_read, len_loc);
      ptr_cdc_data->ptr_data_read += len_loc;
      ptr_cdc_data->data_read_cnt += len_loc;
    }

    // Check if there is enough room to receive one maximum packet
    // Effectively buffer is used with one maximum packet less as this way
    // we ensure enough of contiguous space at end of buffer
chk_act:
    if ((ptr_cdc_data->receive_active == 0U) &&
       ((ptr_cdc_cfg->receive_buf_sz - (ptr_cdc_data->data_received_cnt - ptr_cdc_data->data_read_cnt)) >= (2U * max_packet_size))) {
      ptr_cdc_data->receive_active = 2U;  // Value 2 to start reception in thread
      (void)USBD_ThreadFlagsSet (usbd_cdc_bulk_thread_id[instance], ((((uint32_t)ptr_cdc_cfg->ep_bulk_out) << 12) | (ARM_USBD_EVENT_OUT << 8)));
    }
  } else {                              // If RNDIS
    if (ptr_cdc_data->receive_active != 0U) {   // If reception is active
      len_req = 0U;
      goto exit;
    }

    len_data = ptr_cdc_data->data_received_cnt - ptr_cdc_data->data_read_cnt;
    if (len_data == 0U) {               // If no data available
      len_req = 0U;
      goto exit;
    }

    len_req = (uint32_t)len;            // Number of bytes requested for reading
    if (len_req > len_data) {           // If more requested then available
      len_req = len_data;               // correct to return maximum available
    }

    // Copy data to user provided buffer, any data not read will be lost
    memcpy (buf, ptr_cdc_data->ptr_data_received, len_req);
    ptr_cdc_data->data_read_cnt = ptr_cdc_data->data_received_cnt;

    // Start new reception
    ptr_cdc_data->receive_active = 2U;  // Value 2 to start reception in thread
    (void)USBD_ThreadFlagsSet (usbd_cdc_bulk_thread_id[instance], ((((uint32_t)ptr_cdc_cfg->ep_bulk_out) << 12) | (ARM_USBD_EVENT_OUT << 8)));
  }

  EvrUSBD_CDC_ACM_ReadData(instance, len, len_req);
exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_CDC_ACM_ReadDataFailed(instance, len, -(int32_t)status);
  }
#endif

  if (status != usbOK) {
    return -(int32_t)status;
  }

  return (int32_t)len_req;              // Number of bytes actually read
}

/// \brief Write data from Communication Device to USB Host
/// \param[in]     instance             instance of CDC class.
/// \param[in]     buf                  buffer containing data bytes to write.
/// \param[in]     len                  number of bytes to write.
/// \return                             number of bytes accepted for writing or execution status :
///                                       - value >= 0 : number of bytes accepted for writing
///                                       - value < 0 :  error occurred, -value is execution status as defined with \ref usbStatus
int32_t USBD_CDC_ACM_WriteData (uint8_t instance, const uint8_t *buf, int32_t len) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
  const uint8_t         *ptr_buf;
        uint32_t         len_free, len_till_wrap, len_req = 0U, len_loc;
        usbStatus        status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;

  if (ptr_cdc_cfg->acm == 0U) {         // If not ACM class, exit function with error
    status = usbClassErrorCDC;
    goto exit;
  }

  if (ptr_cdc_cfg->rndis == 0U) {       // If VirtualCOM
    len_free = ptr_cdc_cfg->send_buf_sz - (ptr_cdc_data->data_to_send_cnt - ptr_cdc_data->data_sent_cnt);
    if (len_free == 0U) {               // If no data available
      len_req = 0U;
      goto chk_act;                     // Check if new transmission activation is needed
    }

    len_req = (uint32_t)len;            // Number of bytes requested for writing
    if (len_req > len_free) {           // If more requested then available
      len_req = len_free;               // correct to return maximum available
    }

    len_till_wrap = ((uint32_t)ptr_cdc_cfg->send_buf - (uint32_t)ptr_cdc_data->ptr_data_to_send) + (uint32_t)ptr_cdc_cfg->send_buf_sz;
    len_loc       = len_req;
    ptr_buf       = buf;                // Pointer to data to write
    if (len_loc >= len_till_wrap) {     // If more data then until end of buffer (wrap)
                                        // Copy data till end of buffer
      memcpy (ptr_cdc_data->ptr_data_to_send, ptr_buf, len_till_wrap);
      ptr_buf                        += len_till_wrap;
      len_loc                        -= len_till_wrap;
      ptr_cdc_data->ptr_data_to_send  = ptr_cdc_cfg->send_buf;
      ptr_cdc_data->data_to_send_cnt += len_till_wrap;
    }
    if (len_loc != 0U) {                // If there is data to be copied
                                        // Copy remaining data to send
      memcpy (ptr_cdc_data->ptr_data_to_send, ptr_buf, len_loc);
      ptr_cdc_data->ptr_data_to_send += len_loc;
      ptr_cdc_data->data_to_send_cnt += len_loc;
    }

    // If sending is not active if there is data to send, activate transmission
chk_act:
    if ((ptr_cdc_data->send_active == 0U) &&
        (ptr_cdc_data->data_to_send_cnt != ptr_cdc_data->data_sent_cnt)) {
      ptr_cdc_data->send_active = 2U;   // Value 2 to start transmission in thread
      (void)USBD_ThreadFlagsSet (usbd_cdc_bulk_thread_id[instance], (((uint32_t)ptr_cdc_cfg->ep_bulk_in) << 12) | (ARM_USBD_EVENT_IN << 8));
    }
  } else {                              // If RNDIS
    if (ptr_cdc_data->send_active != 0U) {      // If sending is active
      len_req = 0U;
      goto exit;
    }

    len_req = (uint32_t)len;            // Number of bytes requested for sending
    if (len_req > (uint32_t)ptr_cdc_cfg->send_buf_sz) {         // If more requested then available
      len_req = (uint32_t)ptr_cdc_cfg->send_buf_sz;             // correct to maximum buffer size
    }

    // Copy data for sending
    memcpy (ptr_cdc_data->ptr_data_to_send, buf, len_req);
    ptr_cdc_data->data_to_send_cnt = len_req;

    // Start new transmission
    ptr_cdc_data->send_active = 2U;     // Value 2 to start transmission in thread
    (void)USBD_ThreadFlagsSet (usbd_cdc_bulk_thread_id[instance], (((uint32_t)ptr_cdc_cfg->ep_bulk_in) << 12) | (ARM_USBD_EVENT_IN << 8));
  }

  EvrUSBD_CDC_ACM_WriteData(instance, len, len_req);
exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_CDC_ACM_WriteDataFailed(instance, len, -(int32_t)status);
  }
#endif

  if (status != usbOK) {
    return -(int32_t)status;
  }

  return (int32_t)len_req;              // Number of bytes accepted to be sent
}

/// \brief Read one character received by Communication Device from USB Host
/// \param[in]     instance             instance of CDC class.
/// \return                             value of read character or no character received :
///                                       - value >= 0 : value of first received unread character
///                                       - value -1 :   indicates no character was received
int USBD_CDC_ACM_GetChar (uint8_t instance) {
  static uint8_t ch;

  if ((USBD_CDC_ACM_ReadData (instance, &ch, 1)) == 1) {
    EvrUSBD_CDC_ACM_GetChar(instance, (int32_t)ch);
    return ((int) ch);
  }

  EvrUSBD_CDC_ACM_GetChar(instance, -1);
  return -1;
}

/// \brief Write a single character from Communication Device to USB Host
/// \param[in]     instance             instance of CDC class.
/// \param[in]     ch                   character to write.
/// \return                             value of accepted character or no character accepted :
///                                       - value ch : if character accepted for writing
///                                       - value -1 : indicates character not accepted
int USBD_CDC_ACM_PutChar (uint8_t instance, int ch) {
  static uint8_t ch_local;

  ch_local = (uint8_t)ch;

  if ((USBD_CDC_ACM_WriteData (instance, (uint8_t *)&ch_local, 1)) == 1) {
    EvrUSBD_CDC_ACM_PutChar(instance, (int32_t)ch_local);
    return ((int) ch_local);
  }

  EvrUSBD_CDC_ACM_PutChar(instance, -1);
  return -1;
}

/// \brief Retrieve number of data bytes received by Communication Device from
///        USB Host that are available to read
/// \param[in]     instance      instance of CDC class.
/// \return                      number of bytes available to read or execution status.
///                - value >= 0: number of bytes available to read
///                - value < 0:  error occurred, -value is execution status as defined with \ref usbStatus
int32_t USBD_CDC_ACM_DataAvailable (uint8_t instance) {
  const usbd_cdc_data_t *ptr_cdc_data;
        uint32_t         bytes_available;
        usbStatus        status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    EvrUSBD_CDC_ACM_DataAvailableFailed(instance, -(int32_t)status);
    return -(int32_t)status;
  }

  ptr_cdc_data = usbd_cdc_ptr[instance]->data_ptr;

  bytes_available = (uint32_t)ptr_cdc_data->data_received_cnt - (uint32_t)ptr_cdc_data->data_read_cnt;

  EvrUSBD_CDC_ACM_DataAvailable(instance, bytes_available);
  return (int32_t)bytes_available;
}

/// \brief Send notification of Communication Device status and line states to USB Host
/// \param[in]     instance             instance of CDC class.
/// \param[in]     state                error status and line states :
///                                       - bit 6 : bOverRun
///                                       - bit 5 : bParity
///                                       - bit 4 : bFraming
///                                       - bit 3 : bRingSignal
///                                       - bit 2 : bBreak
///                                       - bit 1 : bTxCarrier (DSR line state)
///                                       - bit 0 : bRxCarrier (DCD line state)
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_CDC_ACM_Notify_SerialState (uint8_t instance, uint16_t state) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
        uint8_t         *ptr_buf;
        usbStatus        status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;

  if (ptr_cdc_cfg->acm == 0U) {         // Only allowed for ACM
    status = usbClassErrorCDC;
    goto exit;
  }

  if (ptr_cdc_data->notify_active != 0U) { 
    status = usbDriverBusy;
    goto exit;
  }
  if (usbd_dev_ptr[ptr_cdc_cfg->dev_num]->data_ptr->configuration == 0U) {
    status = usbNotConfigured;
    goto exit;
  }

  ptr_buf  = ptr_cdc_cfg->notify_buf;

  *ptr_buf = 0xA1U;                         ptr_buf++;  // bmRequestType
  *ptr_buf = CDC_NOTIFICATION_SERIAL_STATE; ptr_buf++;  // bNotification (SERIAL_STATE)
  *ptr_buf = 0U;                            ptr_buf++;  // wValue
  *ptr_buf = 0U;                            ptr_buf++;
  *ptr_buf = ptr_cdc_cfg->cif_num;          ptr_buf++;  // wIndex (Comm Interface)
  *ptr_buf = 0U;                            ptr_buf++;
  *ptr_buf = 2U;                            ptr_buf++;  // wLength
  *ptr_buf = 0U;                            ptr_buf++;
  *ptr_buf = (uint8_t)(state);              ptr_buf++;  // UART State Bitmap
  *ptr_buf = (uint8_t)(state >> 8);

  ptr_cdc_data->notify_len    = 10U;            // Number of bytes this notification contains

  ptr_cdc_data->notify_active = 2U;             // Value 2 to start notification transmission in thread
  (void)USBD_ThreadFlagsSet (usbd_cdc_int_thread_id[instance], (((uint32_t)ptr_cdc_cfg->ep_int_in) << 12) | (ARM_USBD_EVENT_IN << 8));

  EvrUSBD_CDC_ACM_Notify_SerialState(instance, state);

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_CDC_ACM_Notify_SerialStateFailed(instance, state, status);
  }
#endif

  return status;
}

/// \brief Send notification to USB Host signaling that response is available
/// \param[in]     instance             instance of CDC class.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_CDC_ACM_Notify_ResponseAvailable (uint8_t instance) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
        uint8_t         *ptr_buf;
        usbStatus        status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;

  if (ptr_cdc_data->notify_active != 0U) { 
    status = usbDriverBusy;
    goto exit;
  }
  if (usbd_dev_ptr[ptr_cdc_cfg->dev_num]->data_ptr->configuration == 0U) {
    status = usbNotConfigured;
    goto exit;
  }

  ptr_buf  = ptr_cdc_cfg->notify_buf;

  *ptr_buf = 0xA1U;                         ptr_buf++;  // bmRequestType
  *ptr_buf = CDC_RESPONSE_AVAILABLE;        ptr_buf++;  // bNotification (RESPONSE_AVAILABLE)
  *ptr_buf = 0U;                            ptr_buf++;  // wValue
  *ptr_buf = 0U;                            ptr_buf++;
  *ptr_buf = ptr_cdc_cfg->cif_num;          ptr_buf++;  // wIndex (Comm Interface)
  *ptr_buf = 0U;                            ptr_buf++;
  *ptr_buf = 0U;                            ptr_buf++;  // wLength
  *ptr_buf = 0U;

  ptr_cdc_data->notify_len    = 8U;             // Number of bytes this notification contains

  ptr_cdc_data->notify_active = 2U;             // Value 2 to start notification transmission in thread
  (void)USBD_ThreadFlagsSet (usbd_cdc_int_thread_id[instance], (((uint32_t)ptr_cdc_cfg->ep_int_in) << 12) | (ARM_USBD_EVENT_IN << 8));

  EvrUSBD_CDC_ACM_Notify_ResponseAvailable(instance);

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_CDC_ACM_Notify_ResponseAvailableFailed(instance, status);
  }
#endif

  return status;
}

/// \brief Clear the active NTB (prepared for sending)
/// \param[in]     instance             Instance of CDC class.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
int32_t USBD_CDC_NCM_NTB_IN_Initialize (uint8_t instance) {
  const usbd_cdc_t        *ptr_cdc_cfg;
        usbd_cdc_data_t   *ptr_cdc_data;
        CDC_NCM_NTB_NTH16 *ptr_nth16;
        CDC_NCM_NTB_NTH32 *ptr_nth32;
        usbStatus          status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;

  if (ptr_cdc_cfg->ncm_in_buf_cnt <= (ptr_cdc_data->data_to_send_cnt - ptr_cdc_data->data_sent_cnt)) {
    status = usbDriverBusy;
    goto exit;
  }

  if (ptr_cdc_data->ncm_ntb_format == 0U) {
    ptr_nth16 = (CDC_NCM_NTB_NTH16 *)((uint32_t)ptr_cdc_data->ptr_data_to_send);
    memcpy(ptr_nth16, NTH16, 12U);
    ptr_nth16->wSequence          = (uint16_t)ptr_cdc_data->ncm_in_ntb_seq;
    ptr_cdc_data->ncm_in_ntb_seq++;
    ptr_cdc_data->ncm_addr_in_ndp = 0U;
    ptr_cdc_data->ncm_ntb_in_len  = 12U;
  } else {
    ptr_nth32 = (CDC_NCM_NTB_NTH32 *)((uint32_t)ptr_cdc_data->ptr_data_to_send);
    memcpy(ptr_nth32, NTH32, 16U);
    ptr_nth32->wSequence          = (uint16_t)ptr_cdc_data->ncm_in_ntb_seq;
    ptr_cdc_data->ncm_in_ntb_seq++;
    ptr_cdc_data->ncm_addr_in_ndp = 0U;
    ptr_cdc_data->ncm_ntb_in_len  = 16U;
  }

  EvrUSBD_CDC_NCM_NTB_IN_Initialize(instance);

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_CDC_NCM_NTB_IN_InitializeFailed(instance, -(int32_t)status);
  }
#endif

  return -(int32_t)status;
}

/// \brief Create a new NDP in the NTB (datagrams can be added to it)
/// \param[in]     instance             Instance of CDC class.
/// \param[in]     num_datagrams        Maximum number of datagrams that NDP will contain.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
int32_t USBD_CDC_NCM_NTB_IN_CreateNDP (uint8_t instance, uint8_t num_datagrams) {
  const usbd_cdc_t        *ptr_cdc_cfg;
        usbd_cdc_data_t   *ptr_cdc_data;
        CDC_NCM_NTB_NTH16 *ptr_nth16;
        CDC_NCM_NTB_NTH32 *ptr_nth32;
        CDC_NCM_NTB_NDP16 *ptr_ndp16;
        CDC_NCM_NTB_NDP16 *ptr_ndp16_prev;
        CDC_NCM_NTB_NDP32 *ptr_ndp32;
        CDC_NCM_NTB_NDP32 *ptr_ndp32_prev;
        uint8_t           *ptr_ndp;
        uint32_t           ndp_size;
        uint32_t           tot_size;
        uint32_t           ndp_index;
        usbStatus          status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;

  // Calculate next NDP position and size of NDP as well as total size including NDP position alignment
  ptr_ndp = (ptr_cdc_data->ptr_data_to_send + (((ptr_cdc_data->ncm_ntb_in_len + ptr_cdc_cfg->ncm_ndp_in_alignment - 1U) / ptr_cdc_cfg->ncm_ndp_in_alignment) * ptr_cdc_cfg->ncm_ndp_in_alignment));
  if (ptr_cdc_data->ncm_ntb_format == 0U) {
    ndp_size = (8U  + (((uint32_t)num_datagrams + 1U) * 4U));
  } else {
    ndp_size = (16U + (((uint32_t)num_datagrams + 1U) * 8U));
  }
  tot_size  = (((uint32_t)ptr_ndp + ndp_size) - ((uint32_t)ptr_cdc_data->ptr_data_to_send + ptr_cdc_data->ncm_ntb_in_len));
  ndp_index = ((uint32_t)ptr_ndp - (uint32_t)ptr_cdc_data->ptr_data_to_send);

  if (ptr_cdc_data->ncm_ntb_format == 0U) {
    if ((ndp_size >= 65536U) || (tot_size >= 65535U) || (ndp_index >= 65536U)) {
      // Check all elements for 16-bit NDP are in bounds of 16 bits
      status = usbUnknownError;
      goto exit;
    }
  }

  if ((((uint32_t)ptr_cdc_data->ptr_data_to_send + (uint32_t)ptr_cdc_data->ncm_ntb_input_size)) < ((uint32_t)ptr_ndp + ndp_size)) {
    // If not enough space for new NDP
    status = usbUnknownError;
    goto exit;
  }

  memset((void *)ptr_ndp, 0, ndp_size);

  if (ptr_cdc_data->ncm_ntb_format == 0U) {
    ptr_nth16      = (CDC_NCM_NTB_NTH16 *)((uint32_t)ptr_cdc_data->ptr_data_to_send);
    ptr_ndp16      = (CDC_NCM_NTB_NDP16 *)((uint32_t)ptr_ndp);
    ptr_ndp16_prev = (CDC_NCM_NTB_NDP16 *)(ptr_cdc_data->ncm_addr_in_ndp);
    if (ptr_ndp16_prev == NULL) {
      // Load next NDP pointer to header
      ptr_nth16->wNdpIndex = (uint16_t)ndp_index;
    } else {
      // Load next NDP pointer to previous NDP
      ptr_ndp16_prev->wNextNdpIndex = (uint16_t)ndp_index;
    }

    if (ptr_cdc_data->ncm_crc_mode == 0U) {
      memcpy(ptr_ndp, "NCM0", 4U);
    } else {
      memcpy(ptr_ndp, "NCM1", 4U);
    }

    ptr_ndp16->wLength       = (uint16_t)ndp_size;
    ptr_nth16->wBlockLength += (uint16_t)tot_size;
  } else {
    ptr_nth32      = (CDC_NCM_NTB_NTH32 *)((uint32_t)ptr_cdc_data->ptr_data_to_send);
    ptr_ndp32      = (CDC_NCM_NTB_NDP32 *)((uint32_t)ptr_ndp);
    ptr_ndp32_prev = (CDC_NCM_NTB_NDP32 *)(ptr_cdc_data->ncm_addr_in_ndp);
    if (ptr_ndp32_prev == NULL) {
      // Load next NDP pointer to header
      ptr_nth32->dwNdpIndex = ndp_index;
    } else {
      // Load next NDP pointer to previous NDP
      ptr_ndp32_prev->dwNextNdpIndex = ndp_index;
    }

    memcpy(ptr_ndp32, "ncmh", 4U);

    ptr_ndp32->wLength        = (uint16_t)ndp_size;
    ptr_nth32->dwBlockLength += tot_size;
  }

  ptr_cdc_data->ncm_addr_in_ndp       = (uint32_t)ptr_ndp;
  ptr_cdc_data->ncm_ntb_in_len       += tot_size;
  ptr_cdc_data->ncm_in_datagram_index = 0U;
  ptr_cdc_data->ncm_in_datagram_num   = num_datagrams;

  EvrUSBD_CDC_NCM_NTB_IN_CreateNDP(instance, num_datagrams);

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_CDC_NCM_NTB_IN_CreateNDPFailed(instance, num_datagrams, -(int32_t)status);
  }
#endif
 
 return -(int32_t)status;
}

/// \brief Add a datagram into the active NDP of the NTB to be sent
/// \param[in]     instance             Instance of CDC class.
/// \param[in]     buf                  Buffer containing data bytes to write.
/// \param[in]     len                  Number of bytes to write.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
int32_t USBD_CDC_NCM_NTB_IN_WriteDatagram (uint8_t instance, const uint8_t *buf, uint32_t len) {
  const usbd_cdc_t        *ptr_cdc_cfg;
        usbd_cdc_data_t   *ptr_cdc_data;
        CDC_NCM_NTB_NTH16 *ptr_nth16;
        CDC_NCM_NTB_NTH32 *ptr_nth32;
        CDC_NCM_NTB_NDP16 *ptr_ndp16;
        CDC_NCM_NTB_NDP32 *ptr_ndp32;
        uint8_t           *ptr_datagram;
        uint32_t           datagram_addr;
        uint32_t           datagram_index;
        uint32_t           tot_size;
        usbStatus          status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;

  // Calculate next datagram position
  datagram_addr  = (((((uint32_t)ptr_cdc_data->ptr_data_to_send + (uint32_t)ptr_cdc_data->ncm_ntb_in_len + 14U + (uint32_t)ptr_cdc_cfg->ncm_ndp_in_divisor - ((uint32_t)ptr_cdc_cfg->ncm_ndp_in_payload_reminder + 1U)) 
                   / (uint32_t)ptr_cdc_cfg->ncm_ndp_in_divisor) * (uint32_t)ptr_cdc_cfg->ncm_ndp_in_divisor) + (uint32_t)ptr_cdc_cfg->ncm_ndp_in_payload_reminder - 14U);
  ptr_datagram   = (uint8_t *)datagram_addr;
  datagram_index = ((uint32_t)ptr_datagram - (uint32_t)ptr_cdc_data->ptr_data_to_send);
  tot_size       = (uint32_t)(((uint32_t)ptr_datagram + len) - ((uint32_t)ptr_cdc_data->ptr_data_to_send + ptr_cdc_data->ncm_ntb_in_len));

  if (len > ptr_cdc_data->ncm_max_datagram_size) {
    // If requested size is more than maximum allowed datagram size
    status = usbUnknownError;
    goto exit;
  }
  if ((((uint32_t)ptr_cdc_data->ptr_data_to_send + (uint32_t)ptr_cdc_data->ncm_ntb_input_size)) < ((uint32_t)ptr_datagram + len)) {
    // If not enough space for new datagram
    status = usbUnknownError;
    goto exit;
  }
  if (ptr_cdc_data->ncm_in_datagram_index >= ptr_cdc_data->ncm_in_datagram_num) {
    // If no available slot for new datagram in current NDP
    status = usbUnknownError;
    goto exit;
  }

  memcpy(ptr_datagram, buf, len);

  if (ptr_cdc_data->ncm_ntb_format == 0U) {
    ptr_ndp16 = (CDC_NCM_NTB_NDP16 *)(ptr_cdc_data->ncm_addr_in_ndp);
    ptr_ndp16->datagram[ptr_cdc_data->ncm_in_datagram_index].wDatagramIndex  = (uint16_t)datagram_index;
    ptr_ndp16->datagram[ptr_cdc_data->ncm_in_datagram_index].wDatagramLength = (uint16_t)len;

    ptr_nth16 = (CDC_NCM_NTB_NTH16 *)((uint32_t)ptr_cdc_data->ptr_data_to_send);
    ptr_nth16->wBlockLength += (uint16_t)tot_size;
  } else {
    ptr_ndp32 = (CDC_NCM_NTB_NDP32 *)(ptr_cdc_data->ncm_addr_in_ndp);
    ptr_ndp32->datagram[ptr_cdc_data->ncm_in_datagram_index].dwDatagramIndex  = datagram_index;
    ptr_ndp32->datagram[ptr_cdc_data->ncm_in_datagram_index].dwDatagramLength = len;

    ptr_nth32 = (CDC_NCM_NTB_NTH32 *)((uint32_t)ptr_cdc_data->ptr_data_to_send);
    ptr_nth32->dwBlockLength += tot_size;
  }

  ptr_cdc_data->ncm_ntb_in_len += tot_size;
  ptr_cdc_data->ncm_in_datagram_index++;

  EvrUSBD_CDC_NCM_NTB_IN_WriteDatagram(instance, len);

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_CDC_NCM_NTB_IN_WriteDatagramFailed(instance, len, -(int32_t)status);
  }
#endif
  return -(int32_t)status;
}

/// \brief Send the active NTB
/// \param[in]     instance             Instance of CDC class.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
int32_t USBD_CDC_NCM_NTB_IN_Send (uint8_t instance) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
        usbStatus        status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    EvrUSBD_CDC_NCM_NTB_IN_SendFailed(instance, -(int32_t)status);
    return -(int32_t)status;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;

  ptr_cdc_data->ncm_ntb_in_buf_len[ptr_cdc_data->data_to_send_cnt & ((uint32_t)ptr_cdc_cfg->ncm_in_buf_cnt - 1U)] = ptr_cdc_data->ncm_ntb_in_len;
  ptr_cdc_data->ptr_data_to_send += ptr_cdc_cfg->ncm_ntb_in_max_size;
  if (ptr_cdc_data->ptr_data_to_send >= (ptr_cdc_cfg->send_buf + ptr_cdc_cfg->send_buf_sz)) {
    // If wrap over buffers end, move to beginning of the buffers
    ptr_cdc_data->ptr_data_to_send = ptr_cdc_cfg->send_buf;
  }
  ptr_cdc_data->data_to_send_cnt++;

  if ((ptr_cdc_data->send_active == 0U) &&
      (ptr_cdc_data->data_to_send_cnt != ptr_cdc_data->data_sent_cnt)) {
    ptr_cdc_data->send_active = 2U;     // Value 2 to start transmission in thread
    (void)USBD_ThreadFlagsSet (usbd_cdc_bulk_thread_id[instance], (((uint32_t)ptr_cdc_cfg->ep_bulk_in) << 12) | (ARM_USBD_EVENT_IN << 8));
  }

  EvrUSBD_CDC_NCM_NTB_IN_Send(instance);
  return 0;
}

/// \brief Check if NTB was sent and new NTB can be prepared
/// \param[in]     instance             Instance of CDC class.
/// \return                             NTB sent status or error code :
///                                       - value 1 :   NTB was sent
///                                       - value 0 :   NTB sending is in progress
///                                       - value < 0 : error code
int32_t USBD_CDC_NCM_NTB_IN_IsSent (uint8_t instance) {
  usbStatus status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    EvrUSBD_CDC_NCM_NTB_IN_IsSentFailed(instance, -(int32_t)status);
    return -(int32_t)status;
  }

  if (usbd_cdc_ptr[instance]->data_ptr->send_active == 0U) {
    EvrUSBD_CDC_NCM_NTB_IN_IsSent(instance, 1U);
    return 1;
  }

  EvrUSBD_CDC_NCM_NTB_IN_IsSent(instance, 0U);
  return 0;
}

/// \brief Check if NTB was received and is available for processing
/// \param[in]     instance             Instance of CDC class.
/// \return                             NTB available or error code :
///                                       - value 1 :   received NTB is available
///                                       - value 0 :   no received NTB is available
///                                       - value < 0 : error code
int32_t USBD_CDC_NCM_NTB_OUT_IsReceived (uint8_t instance) {
  const usbd_cdc_t      *ptr_cdc_cfg;
  const usbd_cdc_data_t *ptr_cdc_data;
        usbStatus        status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    EvrUSBD_CDC_NCM_NTB_OUT_IsReceivedFailed(instance, -(int32_t)status);
    return -(int32_t)status;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;

  if (ptr_cdc_data->ncm_ntb_out_buf_len[ptr_cdc_data->data_read_cnt & ((uint32_t)ptr_cdc_cfg->ncm_out_buf_cnt - 1U)] != 0U) {
    EvrUSBD_CDC_NCM_NTB_OUT_IsReceived(instance, 1U);
    return 1;
  }

  EvrUSBD_CDC_NCM_NTB_OUT_IsReceived(instance, 0U);
  return 0;
}

/// \brief Flush the received NTB and release memory for reception of a new NTB
/// \param[in]     instance             Instance of CDC class.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
int32_t USBD_CDC_NCM_NTB_OUT_Release (uint8_t instance) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
        usbStatus        status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    EvrUSBD_CDC_NCM_NTB_OUT_ReleaseFailed(instance, -(int32_t)status);
    return -(int32_t)status;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;

  ptr_cdc_data->ncm_out_ntb_seq++;

  ptr_cdc_data->ncm_addr_out_ndp       = 0U;
  ptr_cdc_data->ncm_out_datagram_index = 0U;
  ptr_cdc_data->ncm_ntb_out_buf_len[ptr_cdc_data->data_read_cnt & ((uint32_t)ptr_cdc_cfg->ncm_out_buf_cnt - 1U)] = 0U;
  ptr_cdc_data->ptr_data_read += ptr_cdc_cfg->ncm_ntb_out_max_size;
  if (ptr_cdc_data->ptr_data_read >= (ptr_cdc_cfg->receive_buf + ptr_cdc_cfg->receive_buf_sz)) {
    // If wrap over buffers end, move to beginning of the buffers
    ptr_cdc_data->ptr_data_read = ptr_cdc_cfg->receive_buf;
  }
  ptr_cdc_data->data_read_cnt++;
  if (ptr_cdc_data->receive_active == 0U) {
    if (ptr_cdc_cfg->ncm_out_buf_cnt > (ptr_cdc_data->data_received_cnt - ptr_cdc_data->data_read_cnt)) {
      ptr_cdc_data->receive_active = 2U;  // Value 2 to start reception in thread
      (void)USBD_ThreadFlagsSet (usbd_cdc_bulk_thread_id[instance], (((uint32_t)ptr_cdc_cfg->ep_bulk_out) << 12) | (ARM_USBD_EVENT_OUT << 8));
    }
  }

  EvrUSBD_CDC_NCM_NTB_OUT_Release(instance);
  return 0;
}

/// \brief Process the next NDP in the received NTB
/// \param[in]     instance             Instance of CDC class.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
int32_t USBD_CDC_NCM_NTB_OUT_ProcessNDP (uint8_t instance) {
        usbd_cdc_data_t   *ptr_cdc_data;
  const CDC_NCM_NTB_NTH16 *ptr_nth16;
  const CDC_NCM_NTB_NTH32 *ptr_nth32;
  const CDC_NCM_NTB_NDP16 *ptr_ndp16;
  const CDC_NCM_NTB_NDP32 *ptr_ndp32;
        usbStatus          status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_cdc_data = usbd_cdc_ptr[instance]->data_ptr;

  if (ptr_cdc_data->data_received_cnt <= ptr_cdc_data->data_read_cnt) {
    // No received NDP available
    status = usbUnknownError;
    goto exit;
  }

  if (ptr_cdc_data->ncm_ntb_format == 0U) {
    ptr_ndp16 = (CDC_NCM_NTB_NDP16 *)(ptr_cdc_data->ncm_addr_out_ndp);

    if (ptr_ndp16 == NULL) {
      ptr_nth16 = (CDC_NCM_NTB_NTH16 *)((uint32_t)ptr_cdc_data->ptr_data_read);
      if (ptr_nth16->wNdpIndex == 0U) {
        // No first NDP available
        status = usbUnknownError;
        goto exit;
      }
      ptr_cdc_data->ncm_addr_out_ndp = ((uint32_t)ptr_cdc_data->ptr_data_read + (uint32_t)(ptr_nth16->wNdpIndex));
    } else {
      if (ptr_ndp16->wNextNdpIndex == 0U) {
        // No next NDP available
        status = usbUnknownError;
        goto exit;
      }
      ptr_cdc_data->ncm_addr_out_ndp = ((uint32_t)ptr_cdc_data->ptr_data_read + (uint32_t)(ptr_ndp16->wNextNdpIndex));
    }
  } else {
    ptr_ndp32 = (CDC_NCM_NTB_NDP32 *)(ptr_cdc_data->ncm_addr_out_ndp);

    if (ptr_ndp32 == NULL) {
      ptr_nth32 = (CDC_NCM_NTB_NTH32 *)((uint32_t)ptr_cdc_data->ptr_data_read);
      if (ptr_nth32->dwNdpIndex == 0U) {
        // No next NDP available
        status = usbUnknownError;
        goto exit;
      }
      ptr_cdc_data->ncm_addr_out_ndp = ((uint32_t)ptr_cdc_data->ptr_data_read + (uint32_t)(ptr_nth32->dwNdpIndex));
    } else {
      if (ptr_ndp32->dwNextNdpIndex == 0U) {
        // No next NDP available
        status = usbUnknownError;
        goto exit;
      }
      ptr_cdc_data->ncm_addr_out_ndp = ((uint32_t)ptr_cdc_data->ptr_data_read + (uint32_t)(ptr_ndp32->dwNextNdpIndex));
    }
  }
  ptr_cdc_data->ncm_out_datagram_index = 0U;

  EvrUSBD_CDC_NCM_NTB_OUT_ProcessNDP(instance);

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_CDC_NCM_NTB_OUT_ProcessNDPFailed(instance, -(int32_t)status);
  }
#endif

  return -(int32_t)status;
}

/// \brief Get size of a datagram from the active NDP of the received NTB
/// \param[in]     instance             Instance of CDC class.
/// \return                             Number of bytes available in the datagram.
uint32_t USBD_CDC_NCM_NTB_OUT_GetDatagramSize (uint8_t instance) {
  const usbd_cdc_data_t   *ptr_cdc_data;
  const CDC_NCM_NTB_NDP16 *ptr_ndp16;
  const CDC_NCM_NTB_NDP32 *ptr_ndp32;
        uint32_t           len;

  len = 0U;
  if (USBD_CDC_CheckInstance(instance) != usbOK) {
    goto exit;
  }

  ptr_cdc_data = usbd_cdc_ptr[instance]->data_ptr;

  if (ptr_cdc_data->ncm_ntb_format == 0U) {
    ptr_ndp16 = (CDC_NCM_NTB_NDP16 *)(ptr_cdc_data->ncm_addr_out_ndp);
    len       = ptr_ndp16->datagram[ptr_cdc_data->ncm_out_datagram_index].wDatagramLength;
  } else {
    ptr_ndp32 = (CDC_NCM_NTB_NDP32 *)(ptr_cdc_data->ncm_addr_out_ndp);
    len       = ptr_ndp32->datagram[ptr_cdc_data->ncm_out_datagram_index].dwDatagramLength;
  }

exit:
  EvrUSBD_CDC_NCM_NTB_OUT_GetDatagramSize(instance, len);
  return len;
}

/// \brief Read a datagram from the active NDP of the received NTB
/// \param[in]     instance             Instance of CDC class.
/// \param[out]    buf                  Buffer that receives read data.
/// \param[in]     max_len              Maximum number of bytes to read.
/// \return                             number of bytes read from the datagram or error code :
///                                       - value >= 0 : number of bytes read from the datagram
///                                       - value <  0 : error code
int32_t USBD_CDC_NCM_NTB_OUT_ReadDatagram (uint8_t instance, uint8_t *buf, uint32_t max_len) {
        usbd_cdc_data_t   *ptr_cdc_data;
  const CDC_NCM_NTB_NDP16 *ptr_ndp16;
  const CDC_NCM_NTB_NDP32 *ptr_ndp32;
  const uint8_t           *ptr_data;
        uint32_t           len;
        usbStatus          status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    EvrUSBD_CDC_NCM_NTB_OUT_ReadDatagramFailed(instance, max_len, -(int32_t)status);
    return -(int32_t)status;
  }

  ptr_cdc_data = usbd_cdc_ptr[instance]->data_ptr;

  if (ptr_cdc_data->ncm_ntb_format == 0U) {
    ptr_ndp16 = (CDC_NCM_NTB_NDP16 *)(ptr_cdc_data->ncm_addr_out_ndp);

    if ((ptr_ndp16->datagram[ptr_cdc_data->ncm_out_datagram_index].wDatagramIndex == 0U) || (ptr_ndp16->datagram[ptr_cdc_data->ncm_out_datagram_index].wDatagramLength == 0U)) {
      EvrUSBD_CDC_NCM_NTB_OUT_ReadDatagram(instance, max_len, 0);
      return 0;
    }

    len      = ptr_ndp16->datagram[ptr_cdc_data->ncm_out_datagram_index].wDatagramLength;
    ptr_data = ptr_cdc_data->ptr_data_read + ptr_ndp16->datagram[ptr_cdc_data->ncm_out_datagram_index].wDatagramIndex;
  } else {
    ptr_ndp32 = (CDC_NCM_NTB_NDP32 *)ptr_cdc_data->ncm_addr_out_ndp;

    if ((ptr_ndp32->datagram[ptr_cdc_data->ncm_out_datagram_index].dwDatagramIndex == 0U) || (ptr_ndp32->datagram[ptr_cdc_data->ncm_out_datagram_index].dwDatagramLength == 0U)) {
      EvrUSBD_CDC_NCM_NTB_OUT_ReadDatagram(instance, max_len, 0);
      return 0;
    }

    len      = ptr_ndp32->datagram[ptr_cdc_data->ncm_out_datagram_index].dwDatagramLength;
    ptr_data = ptr_cdc_data->ptr_data_read + ptr_ndp32->datagram[ptr_cdc_data->ncm_out_datagram_index].dwDatagramIndex;
  }

  if (max_len < len) {
    len = max_len;
  }

  if ((buf != NULL) && (len != 0U)) {   // If buf or len == 0, do not copy data (effectively flush datagram)
    memcpy(buf, ptr_data, len);
  }
  ptr_cdc_data->ncm_out_datagram_index++;

  EvrUSBD_CDC_NCM_NTB_OUT_ReadDatagram(instance, max_len, len);
  return (int32_t)len;
}

/// \brief Send already prepared NTB (for this option value of define in configuration USBD_CDCn_NCM_RAW_ENABLE has to be 1)
/// \param[in]     instance             Instance of CDC class.
/// \param[in]     buf                  Buffer containing NTB.
/// \param[in]     len                  Size of NTB.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
int32_t USBD_CDC_NCM_NTB_IN_RawSend (uint8_t instance, const uint8_t *buf, uint32_t len) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
        uint32_t         max_packet_size;
        usbStatus        status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;

  if (ptr_cdc_data->send_active != 0U) {
    status = usbDriverBusy;
    goto exit;
  }

  max_packet_size = ptr_cdc_cfg->max_packet_size1[usbd_dev_ptr[ptr_cdc_cfg->dev_num]->data_ptr->high_speed];

  ptr_cdc_data->send_active = 1U;
  if ((len & (max_packet_size - 1U)) == 0U) {
    ptr_cdc_data->send_zlp = 1U;        // Send also ZLP
  }

  ptr_cdc_data->ptr_data_sent    = (uint8_t *)((uint32_t)buf);
  ptr_cdc_data->data_sent_cnt    = 0U;
  ptr_cdc_data->data_to_send_cnt = len;
  ptr_cdc_data->send_active      = 2U;  // Value 2 to start transmission in thread
  (void)USBD_ThreadFlagsSet (usbd_cdc_bulk_thread_id[instance], (((uint32_t)ptr_cdc_cfg->ep_bulk_in) << 12) | (ARM_USBD_EVENT_IN << 8));

  EvrUSBD_CDC_NCM_NTB_IN_RawSend(instance, len);

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_CDC_NCM_NTB_IN_RawSendFailed(instance, len, -(int32_t)status);
  }
#endif

  return -(int32_t)status;
}

/// \brief Get size of the received NTB
/// \param[in]     instance             Instance of CDC class.
/// \return                             Number of bytes available in the NTB.
uint32_t USBD_CDC_NCM_NTB_OUT_RawGetSize (uint8_t instance) {
  const usbd_cdc_t *ptr_cdc_cfg;
        uint32_t    len;

  len = 0U;
  if (USBD_CDC_CheckInstance(instance) != usbOK) {
    goto exit;
  }

  ptr_cdc_cfg = usbd_cdc_ptr[instance];

  if (ptr_cdc_cfg->data_ptr->receive_active != 0U) {
    EvrUSBD_CDC_NCM_NTB_OUT_RawGetSize(instance, 0U);
    return 0U;
  }

  len = USBD_DriverEndpointTransferGetResult(ptr_cdc_cfg->dev_num, USB_ENDPOINT_OUT(ptr_cdc_cfg->ep_bulk_out));

exit:
  EvrUSBD_CDC_NCM_NTB_OUT_RawGetSize(instance, len);
  return len;
}

/// \brief Receive an NDP (for this option value of define in configuration USBD_CDCn_NCM_RAW_ENABLE has to be 1)
/// \param[in]     instance             Instance of CDC class.
/// \param[out]    buf                  Buffer that receives NTB.
/// \param[in]     max_len              Maximum number of bytes that buffer can accept.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
int32_t USBD_CDC_NCM_NTB_OUT_RawReceive (uint8_t instance, uint8_t *buf, uint32_t max_len) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
        usbStatus        status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;

  if (ptr_cdc_data->receive_active != 0U) { status = usbDriverBusy; goto exit; }

  ptr_cdc_data->receive_active    = 2U;         // Value 2 to start reception in thread
  ptr_cdc_data->ptr_data_received = buf;
  ptr_cdc_data->ncm_raw_out_len   = max_len;
  ptr_cdc_data->ncm_ntb_out_buf_len[ptr_cdc_data->data_read_cnt & ((uint32_t)ptr_cdc_cfg->ncm_out_buf_cnt - 1U)] = 0U;
  (void)USBD_ThreadFlagsSet (usbd_cdc_bulk_thread_id[instance], (((uint32_t)ptr_cdc_cfg->ep_bulk_out) << 12) | (ARM_USBD_EVENT_OUT << 8));

  EvrUSBD_CDC_NCM_NTB_OUT_RawReceive(instance, max_len);

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_CDC_NCM_NTB_OUT_RawReceiveFailed(instance, max_len, -(int32_t)status);
  }
#endif

  return -(int32_t)status;
}

/// \brief Report whether or not the physical layer (modem, Ethernet PHY, etc.) link is up to the USB Host
/// \param[in]     instance             Instance of CDC class.
/// \param[in]     status               Connection status :
///                                       - value 0 : Disconnected
///                                       - value 1 : Connected
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
int32_t USBD_CDC_NCM_Notify_NetworkConnection (uint8_t instance, uint16_t status) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
  const usbd_data_t     *ptr_dev_data;
        uint8_t         *ptr_buf;
        usbStatus        notify_status;

  notify_status = USBD_CDC_CheckInstance(instance);
  if (notify_status != usbOK) {
    goto exit;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;
  ptr_dev_data = usbd_dev_ptr[ptr_cdc_cfg->dev_num]->data_ptr;

  if (ptr_cdc_data->notify_active != 0U) {
    notify_status = usbDriverBusy;
    goto exit;
  }
  if (ptr_dev_data->configuration == 0U) {
    notify_status = usbNotConfigured;
    goto exit;
  }

  ptr_buf  = ptr_cdc_cfg->notify_buf;

  *ptr_buf = 0xA1U;                               ptr_buf++;    // bmRequestType
  *ptr_buf = CDC_NOTIFICATION_NETWORK_CONNECTION; ptr_buf++;    // bNotification (NETWORK CONNECTION)
  *ptr_buf = (uint8_t)(status);                   ptr_buf++;    // wValue
  *ptr_buf = (uint8_t)(status >> 8);              ptr_buf++;
  *ptr_buf = ptr_cdc_cfg->cif_num;                ptr_buf++;    // wIndex (Comm Interface)
  *ptr_buf = 0U;                                  ptr_buf++;
  *ptr_buf = 0U;                                  ptr_buf++;    // wLength
  *ptr_buf = 0U;

  ptr_cdc_data->notify_len    = 8U;                     // Number of bytes this notification contains

  ptr_cdc_data->notify_active = 2U;                     // Value 2 to start notification transmission in thread
  (void)USBD_ThreadFlagsSet (usbd_cdc_int_thread_id[instance], (((uint32_t)ptr_cdc_cfg->ep_int_in) << 12) | (ARM_USBD_EVENT_IN << 8));

  EvrUSBD_CDC_NCM_Notify_NetworkConnection(instance, status);

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (notify_status != usbOK) {
    EvrUSBD_CDC_NCM_Notify_NetworkConnectionFailed(instance, status, -(int32_t)notify_status);
  }
#endif

  return -(int32_t)notify_status;
}

/// \brief Report a change in upstream or downstream speed of the networking connection to the USB Host
/// \param[in]     instance             Instance of CDC class.
/// \param[in]     us_bitrate           Upstream bitrate.
/// \param[in]     ds_bitrate           Downstream bitrate.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
int32_t USBD_CDC_NCM_Notify_ConnectionSpeedChange (uint8_t instance, uint32_t us_bitrate, uint32_t ds_bitrate) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
  const usbd_data_t     *ptr_dev_data;
        uint8_t         *ptr_buf;
        usbStatus        status;

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;
  ptr_dev_data = usbd_dev_ptr[ptr_cdc_cfg->dev_num]->data_ptr;

  if (ptr_cdc_data->notify_active != 0U) {
    status = usbDriverBusy;
    goto exit;
  }
  if (ptr_dev_data->configuration == 0U) {
    status = usbNotConfigured;
    goto exit;
  }

  ptr_buf  = ptr_cdc_cfg->notify_buf;

  *ptr_buf = 0xA1U;                       ptr_buf++;    // bmRequestType
  *ptr_buf = CDC_CONNECTION_SPEED_CHANGE; ptr_buf++;    // bNotification (CONNECTION SPEED CHANGE)
  *ptr_buf = 0U;                          ptr_buf++;    // wValue
  *ptr_buf = 0U;                          ptr_buf++;
  *ptr_buf = ptr_cdc_cfg->cif_num;        ptr_buf++;    // wIndex (Comm Interface)
  *ptr_buf = 0U;                          ptr_buf++;
  *ptr_buf = 0U;                          ptr_buf++;    // wLength
  *ptr_buf = 0U;                          ptr_buf++;
  *ptr_buf = (uint8_t)(us_bitrate);       ptr_buf++;    // USBitRate
  *ptr_buf = (uint8_t)(us_bitrate >> 8);  ptr_buf++;
  *ptr_buf = (uint8_t)(us_bitrate >> 16); ptr_buf++;
  *ptr_buf = (uint8_t)(us_bitrate >> 24); ptr_buf++;
  *ptr_buf = (uint8_t)(ds_bitrate);       ptr_buf++;    // DSBitRate
  *ptr_buf = (uint8_t)(ds_bitrate >> 8);  ptr_buf++;
  *ptr_buf = (uint8_t)(ds_bitrate >> 16); ptr_buf++;
  *ptr_buf = (uint8_t)(ds_bitrate >> 24);

  ptr_cdc_data->notify_len    = 16U;            // Number of bytes this notification contains

  ptr_cdc_data->notify_active = 2U;             // Value 2 to start notification transmission in thread
  (void)USBD_ThreadFlagsSet (usbd_cdc_int_thread_id[instance], (((uint32_t)ptr_cdc_cfg->ep_int_in) << 12) | (ARM_USBD_EVENT_IN << 8));

  EvrUSBD_CDC_NCM_Notify_ConnectionSpeedChange(instance, us_bitrate, ds_bitrate);

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_CDC_NCM_Notify_ConnectionSpeedChangeFailed(instance, us_bitrate, ds_bitrate, -(int32_t)status);
  }
#endif

  return -(int32_t)status;
}


/************************** Library accessible functions **********************/

/// \brief CDC class (ACM) set line coding (calls USBD_CDC_ACM_SetLineCoding function)
/// \param[in]     instance      instance of CDC class.
/// \return        true          success
/// \return        false         fail
bool USBD_CDC_ACM_CoreSetLineCoding (uint8_t instance) {
  const  usbd_cdc_t      *ptr_cdc_cfg;
  const  uint8_t         *ptr_ep0_buf;
  static CDC_LINE_CODING  line_coding;
         bool             result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    ptr_cdc_cfg     = usbd_cdc_ptr[instance];
    ptr_ep0_buf     = &usbd_dev_ptr[ptr_cdc_cfg->dev_num]->ep0_buf[0];

    if (fpUSBD_CDC_ACM_SetLineCoding[instance] != NULL) {
      memcpy((void *)&line_coding, (const void *)ptr_ep0_buf, sizeof(CDC_LINE_CODING));
      result = fpUSBD_CDC_ACM_SetLineCoding[instance] ((const CDC_LINE_CODING *)&line_coding);
      EvrUSBD_CDC_ACM_OnSetLineCoding(instance, line_coding.dwDTERate, result);
    }
  }

  return result;
}

/// \brief CDC class (ACM) get line coding (calls USBD_CDC_ACM_GetLineCoding function)
/// \param[in]     instance      instance of CDC class.
/// \return        true          success
/// \return        false         fail
bool USBD_CDC_ACM_CoreGetLineCoding (uint8_t instance) {
  const  usbd_cdc_t      *ptr_cdc_cfg;
         uint8_t         *ptr_ep0_buf;
  static CDC_LINE_CODING  line_coding;
         bool             result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    ptr_cdc_cfg     = usbd_cdc_ptr[instance];
    ptr_ep0_buf     = &usbd_dev_ptr[ptr_cdc_cfg->dev_num]->ep0_buf[0];

    if (fpUSBD_CDC_ACM_GetLineCoding[instance] != NULL) {
      result = fpUSBD_CDC_ACM_GetLineCoding[instance] (&line_coding);
      EvrUSBD_CDC_ACM_OnGetLineCoding(instance, line_coding.dwDTERate, result);
      if (result) {
        memcpy((void *)ptr_ep0_buf, (void *)&line_coding, sizeof(CDC_LINE_CODING));
      }
    }
  }

  return result;
}

/// \brief CDC class (ACM) send encapsulated command (calls USBD_CDC_ACM_SendEncapsulatedCommand function)
/// \param[in]     instance      instance of CDC class.
/// \param[in]     len           number of data bytes that request contains
/// \return        true          success
/// \return        false         fail
bool USBD_CDC_ACM_CoreSendEncapsulatedCommand (uint8_t instance, uint16_t len) {
  const  usbd_cdc_t *ptr_cdc_cfg;
         bool        result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    ptr_cdc_cfg = usbd_cdc_ptr[instance];

    if (fpUSBD_CDC_ACM_SendEncapsulatedCommand[instance] != NULL) {
      result = fpUSBD_CDC_ACM_SendEncapsulatedCommand[instance] ((const uint8_t *)&usbd_dev_ptr[ptr_cdc_cfg->dev_num]->ep0_buf[0], len);
      EvrUSBD_CDC_ACM_OnSendEncapsulatedCommand(instance, (uint32_t)&usbd_dev_ptr[ptr_cdc_cfg->dev_num]->ep0_buf[0], len, result);
    }
  }

  return result;
}

/// \brief CDC class (ACM) get encapsulated response (calls USBD_CDC_ACM_GetEncapsulatedResponse function)
/// \param[in]     instance      instance of CDC class.
/// \param[in]     max_len       maximum number of data bytes that USB Host expects to receive
/// \param[out]    buf           pointer to buffer containing get encapsulated response to be returned to USB Host
/// \param[out]    len           pointer to number of data bytes to be returned to USB Host
/// \return        true          success
/// \return        false         fail
bool USBD_CDC_ACM_CoreGetEncapsulatedResponse (uint8_t instance, uint16_t max_len, uint8_t **buf, uint16_t *len) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_ACM_GetEncapsulatedResponse[instance] != NULL) {
      result = fpUSBD_CDC_ACM_GetEncapsulatedResponse[instance] (max_len, buf, len);
      EvrUSBD_CDC_ACM_OnGetEncapsulatedResponse(instance, max_len, (uint32_t)(*buf), *len, result);
    }
  }

  return result;
}

bool USBD_CDC_ACM_CoreSetCommFeature (uint8_t instance, uint16_t feat) {
  (void)instance;
  (void)feat;

  return false;
}
bool USBD_CDC_ACM_CoreGetCommFeature (uint8_t instance, uint16_t feat) {
  (void)instance;
  (void)feat;

  return false;
}
bool USBD_CDC_ACM_CoreClearCommFeature (uint8_t instance, uint16_t feat) {
  (void)instance;
  (void)feat;

  return false;
}
bool USBD_CDC_ACM_CoreSendBreak (uint8_t instance, uint16_t dur) {
  (void)instance;
  (void)dur;

  return false;
}

/// \brief CDC class (ACM) set control line state (calls USBD_CDC_ACM_SetControlLineState function)
/// \param[in]     instance      instance of CDC class.
/// \param[in]     state         control line settings bitmap
///                                - bit 0: DTR state
///                                - bit 1: RTS state
/// \return        true          success
/// \return        false         fail
bool USBD_CDC_ACM_CoreSetControlLineState (uint8_t instance, uint16_t state) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_ACM_SetControlLineState[instance] != NULL) { 
      result = fpUSBD_CDC_ACM_SetControlLineState[instance] (state);
      EvrUSBD_CDC_ACM_OnSetControlLineState(instance, (uint8_t)state & 1U, ((uint8_t)state >> 1) & 1U, result);
      if (result) {
        usbd_cdc_ptr[instance]->data_ptr->control_line_state = state;
      }
    }
  }

  return result;
}

// Wrapper functions that redistribute Control Endpoint 0 interface requests to instance callback calls for user handling
// (user callbacks are defined in template file)

bool USBD_CDC_NCM_CoreSetEthernetMulticastFilters (uint8_t instance, const uint8_t *addr_list, uint16_t num_of_filters) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_SetEthernetMulticastFilters[instance] != NULL) {
      result = fpUSBD_CDC_NCM_SetEthernetMulticastFilters[instance] (addr_list, num_of_filters);
      EvrUSBD_CDC_NCM_OnSetEthMulticastFilters(instance, result);
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreSetEthernetPowerManagementPatternFilter (uint8_t instance, uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_SetEthernetPowerManagementPatternFilter[instance] != NULL) {
      result = fpUSBD_CDC_NCM_SetEthernetPowerManagementPatternFilter[instance] (filter_number, pattern_filter, pattern_filter_size);
      EvrUSBD_CDC_NCM_OnSetEthPowerMgmtPatFilter(instance, filter_number, result);
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreGetEthernetPowerManagementPatternFilter (uint8_t instance, uint16_t filter_number, uint16_t *pattern_active) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_GetEthernetPowerManagementPatternFilter[instance] != NULL) {
      result = fpUSBD_CDC_NCM_GetEthernetPowerManagementPatternFilter[instance] (filter_number, pattern_active);
      EvrUSBD_CDC_NCM_OnGetEthPowerMgmtPatFilter(instance, filter_number, result);
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreSetEthernetPacketFilter (uint8_t instance, uint16_t packet_filter_bitmap) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_SetEthernetPacketFilter[instance] != NULL) {
      result = fpUSBD_CDC_NCM_SetEthernetPacketFilter[instance] (packet_filter_bitmap);
      EvrUSBD_CDC_NCM_OnSetEthernetPacketFilter(instance, packet_filter_bitmap, result);
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreGetEthernetStatistic (uint8_t instance, uint16_t feature_selector, uint32_t *data) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_GetEthernetStatistic[instance] != NULL) {
      result = fpUSBD_CDC_NCM_GetEthernetStatistic[instance] (feature_selector, data);
      EvrUSBD_CDC_NCM_OnGetEthernetStatistic(instance, feature_selector, result);
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreGetNtbParameters (uint8_t instance, CDC_NCM_NTB_PARAM *ntb_params) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_GetNtbParameters[instance] != NULL) {
      result = fpUSBD_CDC_NCM_GetNtbParameters[instance] (ntb_params);
      EvrUSBD_CDC_NCM_OnGetNtbParameters(instance, result);
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreGetNetAddress (uint8_t instance, uint8_t *net_addr) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_GetNetAddress[instance] != NULL) {
      result = fpUSBD_CDC_NCM_GetNetAddress[instance] (net_addr);
      EvrUSBD_CDC_NCM_OnGetNetAddress(instance, result);
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreSetNetAddress (uint8_t instance, const uint8_t *net_addr) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_SetNetAddress[instance] != NULL) {
      result = fpUSBD_CDC_NCM_SetNetAddress[instance] (net_addr);
      EvrUSBD_CDC_NCM_OnSetNetAddress(instance, result);
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreGetNtbFormat (uint8_t instance, uint16_t *ntb_format) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_GetNtbFormat[instance] != NULL) {
      result = fpUSBD_CDC_NCM_GetNtbFormat[instance] (ntb_format);
      EvrUSBD_CDC_NCM_OnGetNtbFormat(instance, *ntb_format, result);
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreSetNtbFormat (uint8_t instance, uint16_t ntb_format) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_SetNtbFormat[instance] != NULL) { 
      result = fpUSBD_CDC_NCM_SetNtbFormat[instance] (ntb_format);
      EvrUSBD_CDC_NCM_OnSetNtbFormat(instance, ntb_format, result);
      if (result) {
        usbd_cdc_ptr[instance]->data_ptr->ncm_ntb_format = (uint8_t)ntb_format;
      }
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreGetNtbInputSize (uint8_t instance, uint32_t *ntb_input_size) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_GetNtbInputSize[instance] != NULL) {
      result = fpUSBD_CDC_NCM_GetNtbInputSize[instance] (ntb_input_size);
      EvrUSBD_CDC_NCM_OnGetNtbInputSize(instance, *ntb_input_size, result);
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreSetNtbInputSize (uint8_t instance, uint32_t ntb_input_size) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_SetNtbInputSize[instance] != NULL) {
      result = fpUSBD_CDC_NCM_SetNtbInputSize[instance] (ntb_input_size);
      EvrUSBD_CDC_NCM_OnSetNtbInputSize(instance, ntb_input_size, result);
      if (result) {
        usbd_cdc_ptr[instance]->data_ptr->ncm_ntb_input_size = ntb_input_size;
      }
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreGetMaxDatagramSize (uint8_t instance, uint16_t *max_datagram_size) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_GetMaxDatagramSize[instance] != NULL) {
      result = fpUSBD_CDC_NCM_GetMaxDatagramSize[instance] (max_datagram_size);
      EvrUSBD_CDC_NCM_OnGetMaxDatagramSize(instance, *max_datagram_size, result);
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreSetMaxDatagramSize (uint8_t instance, uint16_t max_datagram_size) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_SetMaxDatagramSize[instance] != NULL) {
      result = fpUSBD_CDC_NCM_SetMaxDatagramSize[instance] (max_datagram_size);
      EvrUSBD_CDC_NCM_OnSetMaxDatagramSize(instance, max_datagram_size, result);
      if (result) {
        usbd_cdc_ptr[instance]->data_ptr->ncm_max_datagram_size = max_datagram_size;
      }
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreGetCrcMode (uint8_t instance, uint16_t *crc_mode) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_GetCrcMode[instance] != NULL) {
      result = fpUSBD_CDC_NCM_GetCrcMode[instance] (crc_mode);
      EvrUSBD_CDC_NCM_OnGetCrcMode(instance, *crc_mode, result);
    }
  }

  return result;
}
bool USBD_CDC_NCM_CoreSetCrcMode (uint8_t instance, uint16_t crc_mode) {
  bool result;

  result = false;
  if (USBD_CDC_CheckInstance(instance) == usbOK) {
    if (fpUSBD_CDC_NCM_SetCrcMode[instance] != NULL) {
      result = fpUSBD_CDC_NCM_SetCrcMode[instance] (crc_mode);
      EvrUSBD_CDC_NCM_OnSetCrcMode(instance, crc_mode, result);
      if (result) {
        usbd_cdc_ptr[instance]->data_ptr->ncm_crc_mode = (uint8_t)crc_mode;
      }
    }
  }

  return result;
}


/************************** Configuration accessible functions ****************/

/// \brief Initialize CDC device instance and create event handling Threads
/// \param[in]     instance      instance of CDC class.
/// \return                      status code that indicates the execution status of the function.
usbStatus USBD_CDC_Initialize (uint8_t instance) {
  usbStatus status;

  EvrUSBD_CDC_Initialize(instance);

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  if (usbd_cdc_ptr[instance]->acm == 1U) {
    if (fpUSBD_CDC_ACM_Initialize [instance] != NULL) {
      EvrUSBD_CDC_ACM_OnInitialize(instance);
      fpUSBD_CDC_ACM_Initialize   [instance] ();
    }
  } else {
    if (fpUSBD_CDC_NCM_Initialize [instance] != NULL) {
      EvrUSBD_CDC_NCM_OnInitialize(instance);
      fpUSBD_CDC_NCM_Initialize   [instance] ();
    }
  }

  if (usbd_cdc_int_thread_id[instance] == NULL) {
    usbd_cdc_int_thread_id[instance] = USBD_ThreadCreate (usbdThreadCDC_Int, instance);
    if (usbd_cdc_int_thread_id[instance] == NULL) {
      status = usbThreadError;
      goto exit;
    }
  }
  if (usbd_cdc_bulk_thread_id[instance] == NULL) {
    usbd_cdc_bulk_thread_id[instance] = USBD_ThreadCreate (usbdThreadCDC_Bulk, instance);
    if (usbd_cdc_bulk_thread_id[instance] == NULL) {
      status = usbThreadError;
    }
  }

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_CDC_InitializeFailed(instance, status);
  }
#endif

  return status;
}

/// \brief De-initialize CDC device instance and terminate event handling Threads
/// \param[in]     instance      instance of CDC class.
/// \return                      status code that indicates the execution status of the function.
usbStatus USBD_CDC_Uninitialize (uint8_t instance) {
  usbStatus status;

  EvrUSBD_CDC_Initialize(instance);

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  if (usbd_cdc_bulk_thread_id[instance] != NULL) {
    if (USBD_ThreadTerminate (usbd_cdc_bulk_thread_id[instance]) != 0) {
      status = usbThreadError;
      goto exit;
    }
    usbd_cdc_bulk_thread_id[instance] = NULL;
  }
  if (usbd_cdc_int_thread_id[instance]  != NULL) {
    if (USBD_ThreadTerminate (usbd_cdc_int_thread_id[instance]) != 0)  {
      status = usbThreadError;
      goto exit;
    }
    usbd_cdc_int_thread_id[instance] = NULL;
  }

  if (usbd_cdc_ptr[instance]->acm == 1U) {
    if (fpUSBD_CDC_ACM_Uninitialize [instance] != NULL) {
      EvrUSBD_CDC_ACM_OnUninitialize(instance);
      fpUSBD_CDC_ACM_Uninitialize   [instance] ();
    }
  } else {
    if (fpUSBD_CDC_NCM_Uninitialize [instance] != NULL) {
      EvrUSBD_CDC_NCM_OnUninitialize(instance);
      fpUSBD_CDC_NCM_Uninitialize   [instance] ();
    }
  }

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_CDC_UninitializeFailed(instance, status);
  }
#endif

  return status;
}

/// \brief CDC USB Bus Reset event handling
/// \param[in]     instance      instance of CDC class.
void USBD_CDC_Reset (uint8_t instance) {
  const  usbd_cdc_t        *ptr_cdc_cfg;
         usbd_cdc_data_t   *ptr_cdc_data;
         CDC_NCM_NTB_PARAM *ptr_ntb_params;
  static CDC_LINE_CODING    line_coding;
         usbStatus          status;
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
         bool               result;
#endif

  EvrUSBD_CDC_Reset(instance);

  status = USBD_CDC_CheckInstance(instance);
  if (status != usbOK) {
    EvrUSBD_CDC_ResetFailed(instance, status);
    return;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;

  ptr_cdc_data->receive_active = 0U;
  ptr_cdc_data->send_active    = 0U;

  memset((void *)ptr_cdc_data, 0, sizeof(usbd_cdc_data_t));
  ptr_cdc_data->ptr_data_to_send  = ptr_cdc_cfg->send_buf;
  ptr_cdc_data->ptr_data_sent     = ptr_cdc_cfg->send_buf;
  ptr_cdc_data->ptr_data_received = ptr_cdc_cfg->receive_buf;
  ptr_cdc_data->ptr_data_read     = ptr_cdc_cfg->receive_buf;

  if (ptr_cdc_cfg->acm == 1U) {                 // If initialize CDC ACM class data
    if (ptr_cdc_cfg->rndis == 0U) {             // If VirtualCOM
      ptr_cdc_data->control_line_state        = 0U;

      // Default initial 9600, 1 stop bit, no parity, 8 data bits
      line_coding.dwDTERate                   = 9600U;
      line_coding.bCharFormat                 = 0U;
      line_coding.bParityType                 = 0U;
      line_coding.bDataBits                   = 8U;
    }

    if (fpUSBD_CDC_ACM_Reset [instance] != NULL) {
      EvrUSBD_CDC_ACM_OnReset(instance);
      fpUSBD_CDC_ACM_Reset   [instance] ();
    }

    if (ptr_cdc_cfg->rndis == 0U) {             // If VirtualCOM
      if (fpUSBD_CDC_ACM_SetLineCoding[instance] != NULL) {
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
        result = 
#else
        (void)
#endif
        fpUSBD_CDC_ACM_SetLineCoding[instance] ((const CDC_LINE_CODING *)(&line_coding));
        EvrUSBD_CDC_ACM_OnSetLineCoding(instance, line_coding.dwDTERate, result);
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
        (void)result;
#endif
      }
    }
  } else {                                      // If initialize CDC NCM class data
    ptr_cdc_data->ncm_ntb_input_size        = ptr_cdc_cfg->ncm_ntb_in_max_size;
    ptr_cdc_data->ncm_max_datagram_size     = ptr_cdc_cfg->ncm_max_segment_size;

    // Initial NTB parameters loaded with configuration settings, can be changed by user USBD_CDCn_NCM_GetNtbParameters function
    ptr_ntb_params = &ptr_cdc_data->ncm_ntb_param;
    ptr_ntb_params->wLength                 = 0x1CU;
    ptr_ntb_params->bmNtbFormatsSupported   = ptr_cdc_cfg->ncm_ntb_formats;
    ptr_ntb_params->dwNtbInMaxSize          = ptr_cdc_cfg->ncm_ntb_in_max_size;
    ptr_ntb_params->wNdpInDivisor           = ptr_cdc_cfg->ncm_ndp_in_divisor;
    ptr_ntb_params->wNdpInPayloadRemainder  = ptr_cdc_cfg->ncm_ndp_in_payload_reminder;
    ptr_ntb_params->wNdpInAlignment         = ptr_cdc_cfg->ncm_ndp_in_alignment;
    ptr_ntb_params->Reserved0               = 0U;
    ptr_ntb_params->dwNtbOutMaxSize         = ptr_cdc_cfg->ncm_ntb_out_max_size;
    ptr_ntb_params->wNdpOutDivisor          = ptr_cdc_cfg->ncm_ndp_out_divisor;
    ptr_ntb_params->wNdpOutPayloadRemainder = ptr_cdc_cfg->ncm_ndp_out_payload_reminder;
    ptr_ntb_params->wNdpOutAlignment        = ptr_cdc_cfg->ncm_ndp_out_alignment;
    ptr_ntb_params->Reserved1               = 0U;

    if (fpUSBD_CDC_NCM_Reset [instance] != NULL) {
      EvrUSBD_CDC_NCM_OnReset(instance);
      fpUSBD_CDC_NCM_Reset   [instance] ();
    }
  }
}

/// \brief CDC Endpoint Start handling
/// \param[in]     instance      instance of CDC class.
/// \param[in]     ep_addr       endpoint address.
///                 - ep_addr.0..3: address
///                 - ep_addr.7:    direction
void USBD_CDC_EndpointStart (uint8_t instance, uint8_t ep_addr) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
  const usbd_data_t     *ptr_dev_data;
        uint32_t         len_to_receive;

  if (USBD_CDC_CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;
  ptr_dev_data = usbd_dev_ptr[ptr_cdc_cfg->dev_num]->data_ptr;

  if (USB_ENDPOINT_IN (ptr_cdc_cfg->ep_int_in)   == ep_addr) { return; }      // Ignore interrupt endpoint start
  if (USB_ENDPOINT_OUT(ptr_cdc_cfg->ep_bulk_out) == ep_addr) { return; }      // Ignore OUT endpoint start

  if (ptr_cdc_cfg->acm == 1U) {         // If handling CDC ACM class
    if (ptr_cdc_cfg->rndis == 0U) {     // If VirtualCOM
      len_to_receive = ptr_cdc_cfg->max_packet_size1[ptr_dev_data->high_speed];
    } else {                            // If RNDIS
      len_to_receive = ptr_cdc_cfg->receive_buf_sz;
    }
  } else {                              // If handling CDC NCM class
    len_to_receive = ptr_cdc_cfg->ncm_ntb_out_max_size;
    if (fpUSBD_CDC_NCM_Start [instance] != NULL) {
      EvrUSBD_CDC_NCM_OnStart(instance);
      fpUSBD_CDC_NCM_Start   [instance] ();
    }
    if (ptr_cdc_cfg->ncm_raw == 1U) {   // If raw mode is active do not start reception
      return;
    }
  }
  if (ptr_cdc_cfg->receive_buf_sz < len_to_receive) {
    // If there is not enough space in buffer to receive new data
    return;
  }

  ptr_cdc_data->receive_active = 2U;    // Value 2 to start reception in thread
  (void)USBD_ThreadFlagsSet (usbd_cdc_bulk_thread_id[instance], (((uint32_t)ptr_cdc_cfg->ep_bulk_out) << 12) | (ARM_USBD_EVENT_OUT << 8));
}

/// \brief CDC Endpoint Stop handling
/// \param[in]     instance      instance of CDC class.
/// \param[in]     ep_addr       endpoint address.
///                 - ep_addr.0..3: address
///                 - ep_addr.7:    direction
void USBD_CDC_EndpointStop (uint8_t instance, uint8_t ep_addr) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;

  if (USBD_CDC_CheckInstance(instance) != usbOK) {
    return;
  }

  // Abort of ongoing transfer happens from usbd_core.c module
  // (either on SetConfiguration or on SetInterface)

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;

  if (USB_ENDPOINT_OUT(ptr_cdc_cfg->ep_bulk_out) == ep_addr) {
    ptr_cdc_data->receive_active = 0U;
    if (ptr_cdc_cfg->acm == 0U) {       // If handling CDC NCM class
      if (fpUSBD_CDC_NCM_Stop [instance] != NULL) {
        EvrUSBD_CDC_NCM_OnStop(instance);
        fpUSBD_CDC_NCM_Stop   [instance] ();
      }
    }
  }
  if (USB_ENDPOINT_IN(ptr_cdc_cfg->ep_bulk_in) == ep_addr) {
    ptr_cdc_data->send_active = 0U;
  }
}


/************************** Local functions ***********************************/

/// \brief Check all CDC device instance pointers and variables
/// \param[in]     instance      instance of CDC class.
/// \return                      status code that indicates the execution status of the function.
static usbStatus USBD_CDC_CheckInstance (uint8_t instance) {

  if (instance >= usbd_cdc_num)                                        { return usbClassErrorCDC; }
  if (usbd_cdc_ptr[instance]                                  == NULL) { return usbClassErrorCDC; }
  if (usbd_cdc_ptr[instance]->data_ptr                        == NULL) { return usbClassErrorCDC; }
  if (usbd_cdc_ptr[instance]->dev_num >= usbd_dev_num)                 { return usbDeviceError;   }
  if (usbd_dev_ptr[usbd_cdc_ptr[instance]->dev_num]           == NULL) { return usbDeviceError;   }
  if (usbd_dev_ptr[usbd_cdc_ptr[instance]->dev_num]->data_ptr == NULL) { return usbDeviceError;   }

  return usbOK;
}

/// \brief CDC Interrupt In Endpoint event handling
/// \param[in]     instance      instance of CDC class.
static void USBD_CDC_EpIntIn (uint8_t instance) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
        uint32_t         len_sent;
        usbStatus        status;
        uint8_t          device;

  if (USBD_CDC_CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;
  device       = ptr_cdc_cfg->dev_num;

  if (ptr_cdc_data->notify_active == 1U) {
    len_sent = USBD_DriverEndpointTransferGetResult(device, USB_ENDPOINT_IN(ptr_cdc_cfg->ep_int_in));
  } else {
    len_sent = 0U;
  }

  if (ptr_cdc_data->notify_active == 1U) {
    if (len_sent != 0U) {
      // Handle notification sent
      ptr_cdc_data->notify_active = 0U;
    }
  } else {
    // Start sending notification
    ptr_cdc_data->notify_active = 1U;
    status = USBD_DriverEndpointTransfer(device, USB_ENDPOINT_IN(ptr_cdc_cfg->ep_int_in), ptr_cdc_cfg->notify_buf, ptr_cdc_data->notify_len);
    if (status != usbOK) {
      ptr_cdc_data->notify_active = 0U;
    }
  }
}

/// \brief CDC Bulk Out Endpoint event handling
/// \details       The function handles Bulk Out endpoint events.
///                It handles data received on the Bulk Out endpoint, reads the
///                received data to the receive intermediate buffer and starts
///                new reception if there is enough space in intermediate buffer
///                for at least 1 maximum packet.
/// \param[in]     instance      instance of CDC class.
static void USBD_CDC_EpBulkOut (uint8_t instance) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
        uint8_t         *ptr_dest_buf;
        uint8_t         *ptr_src_buf;
        uint32_t         len_received;
        uint32_t         len_till_wrap;
        uint32_t         len;
        uint32_t         max_packet_size;
        usbStatus        status;
        uint8_t          device;

  if (USBD_CDC_CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_cdc_cfg  = usbd_cdc_ptr[instance];
  ptr_cdc_data = ptr_cdc_cfg->data_ptr;
  device       = ptr_cdc_cfg->dev_num;

  if (ptr_cdc_data->receive_active == 1U) {
    len_received = USBD_DriverEndpointTransferGetResult(device, USB_ENDPOINT_OUT(ptr_cdc_cfg->ep_bulk_out));
  } else {
    len_received = 0U;
  }

  if (ptr_cdc_cfg->acm == 1U) {                 // If handling CDC ACM class
    if (ptr_cdc_cfg->rndis == 0U) {             // If handling VirtualCOM
      max_packet_size = ptr_cdc_cfg->max_packet_size1[usbd_dev_ptr[device]->data_ptr->high_speed];

      if ((ptr_cdc_data->receive_active == 1U) &&
          (len_received != 0U)) {                  
        // Copy data from intermediate aligned buffer to receive buffer
        len_till_wrap = ((uint32_t)ptr_cdc_cfg->receive_buf_sz - max_packet_size) - ((uint32_t)ptr_cdc_data->ptr_data_received - (uint32_t)ptr_cdc_cfg->receive_buf);
        ptr_dest_buf  = ptr_cdc_data->ptr_data_received;
        ptr_src_buf   = ptr_cdc_cfg->receive_mps_buf;
        len           = len_received;
        if (len_received >= len_till_wrap) {      // If data wraps, copy wrapping data to start of buf
          memcpy(ptr_dest_buf, ptr_src_buf, len_till_wrap);
          ptr_dest_buf  = ptr_cdc_cfg->receive_buf;
          ptr_src_buf  += len_till_wrap;
          len          -= len_till_wrap;
        }
        if (len != 0U) {                         // If more data is available, copy it
          memcpy(ptr_dest_buf, ptr_src_buf, len);
          ptr_dest_buf += len;
        }
        ptr_cdc_data->ptr_data_received  = ptr_dest_buf;
        ptr_cdc_data->data_received_cnt += len_received;
      }

      // Restart new reception
      if ((ptr_cdc_cfg->receive_buf_sz - (ptr_cdc_data->data_received_cnt - ptr_cdc_data->data_read_cnt)) >= (2U * max_packet_size)) {
        // Restart reception if there is enough space for maximum packet size in buffer
        ptr_cdc_data->receive_active = 1U;
        status = USBD_DriverEndpointTransfer(device, USB_ENDPOINT_OUT(ptr_cdc_cfg->ep_bulk_out), ptr_cdc_cfg->receive_mps_buf, max_packet_size);
        if (status != usbOK) {
          ptr_cdc_data->receive_active = 0U;
        }
      } else {
        // Not enough space in buffer
        ptr_cdc_data->receive_active = 0U;
      }
    } else {                                    // If handling RNDIS
      if ((ptr_cdc_data->receive_active == 1U) &&       // If new data was received
          (len_received != 0U)) {                  
        ptr_cdc_data->receive_active = 0U;
        ptr_cdc_data->data_received_cnt += len_received;
      } else if (ptr_cdc_data->receive_active == 2U) {  // If new reception start was requested
        ptr_cdc_data->receive_active = 1U;
        status = USBD_DriverEndpointTransfer(device, USB_ENDPOINT_OUT(ptr_cdc_cfg->ep_bulk_out), ptr_cdc_data->ptr_data_received, (uint32_t)ptr_cdc_cfg->receive_buf_sz);
        if (status != usbOK) {
          ptr_cdc_data->receive_active = 0U;
        }
      }
    }

    // Call received callback
    if ((len_received != 0U) && (fpUSBD_CDC_ACM_DataReceived[instance] != NULL)) {
      fpUSBD_CDC_ACM_DataReceived[instance] (ptr_cdc_data->data_received_cnt - ptr_cdc_data->data_read_cnt);
    }
  } else {                                      // If handling CDC NCM class
    if ((ptr_cdc_data->receive_active == 1U) &&
        (len_received != 0U)) {                  
      // Handle received data
      ptr_cdc_data->ncm_ntb_out_buf_len[ptr_cdc_data->data_received_cnt & ((uint32_t)ptr_cdc_cfg->ncm_out_buf_cnt - 1U)] = len_received;
      ptr_cdc_data->ptr_data_received += ptr_cdc_cfg->ncm_ntb_out_max_size;
      if (ptr_cdc_data->ptr_data_received >= (ptr_cdc_cfg->receive_buf + ptr_cdc_cfg->receive_buf_sz)) {
        // If wrap over buffers end, move to beginning of the buffers
        ptr_cdc_data->ptr_data_received = ptr_cdc_cfg->receive_buf;
      }
      ptr_cdc_data->data_received_cnt++;
    }

    // Restart new reception
    if ((ptr_cdc_cfg->ncm_raw == 1U) &&         // If raw mode is active
        (ptr_cdc_data->receive_active == 2U)) { // and new transmission was requested
      // Activate reception
      ptr_cdc_data->receive_active = 1U;
      status = USBD_DriverEndpointTransfer(device, USB_ENDPOINT_OUT(ptr_cdc_cfg->ep_bulk_out), ptr_cdc_data->ptr_data_received, ptr_cdc_data->ncm_raw_out_len);
      if (status != usbOK) {
        ptr_cdc_data->receive_active = 0U;
      }
    } else if ((ptr_cdc_cfg->ncm_raw == 0U) &&  // or if not raw mode is active and there is free buffer for OUT NTB
               (ptr_cdc_cfg->ncm_out_buf_cnt > (ptr_cdc_data->data_received_cnt - ptr_cdc_data->data_read_cnt))) {
      // Restart reception
      ptr_cdc_data->receive_active = 1U;
      status = USBD_DriverEndpointTransfer(device, USB_ENDPOINT_OUT(ptr_cdc_cfg->ep_bulk_out), ptr_cdc_data->ptr_data_received, ptr_cdc_cfg->ncm_ntb_out_max_size);
      if (status != usbOK) {
        ptr_cdc_data->receive_active = 0U;
      }
    } else {
      // Reception not active
      ptr_cdc_data->receive_active = 0U;
    }

    // Call received callback
    if ((len_received != 0U) && (fpUSBD_CDC_NCM_NTB_OUT_Received[instance] != NULL)) {
      EvrUSBD_CDC_NCM_OnNTB_OUT_Received(instance);
      fpUSBD_CDC_NCM_NTB_OUT_Received[instance] ();
    }
  }
}

/// \brief CDC Bulk In Endpoint event handling
/// \details       The function handles Bulk In endpoint events.
///                It handles data to be sent on the Bulk In endpoint, transmits
///                pending data to be sent that is already in the send
///                intermediate buffer, and it also sends Zero Length Packet
///                if last packet sent was not a short packet.
/// \param[in]     instance      instance of CDC class.
static void USBD_CDC_EpBulkIn (uint8_t instance) {
  const usbd_cdc_t      *ptr_cdc_cfg;
        usbd_cdc_data_t *ptr_cdc_data;
        uint8_t         *ptr_dest_buf;
        uint8_t         *ptr_src_buf;
        uint32_t         len_sent;
        uint32_t         len_till_wrap;
        uint32_t         len_to_send = 0U;
        uint32_t         len;
        uint32_t         max_packet_size;
        usbStatus        status;
        uint8_t          device;
        bool             send_flag;

  if (USBD_CDC_CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_cdc_cfg     = usbd_cdc_ptr[instance];
  ptr_cdc_data    = ptr_cdc_cfg->data_ptr;
  device          = ptr_cdc_cfg->dev_num;
  max_packet_size = ptr_cdc_cfg->max_packet_size1[usbd_dev_ptr[device]->data_ptr->high_speed];

  if (ptr_cdc_data->send_active == 1U) {
    len_sent = USBD_DriverEndpointTransferGetResult(device, USB_ENDPOINT_IN(ptr_cdc_cfg->ep_bulk_in));
  } else {
    len_sent = 0U;
  }

  if (ptr_cdc_cfg->acm == 1U) {                 // If handling CDC ACM class
    if (ptr_cdc_cfg->rndis == 0U) {             // If handling VirtualCOM
      if (ptr_cdc_data->send_active == 1U) {
        if (len_sent != 0U) {
          // Handle sent data
          ptr_cdc_data->ptr_data_sent += len_sent;
          // If send pointer wrapped
          if (ptr_cdc_data->ptr_data_sent >= (ptr_cdc_cfg->send_buf + ptr_cdc_cfg->send_buf_sz)) {
            ptr_cdc_data->ptr_data_sent -= ptr_cdc_cfg->send_buf_sz;
          }
          ptr_cdc_data->data_sent_cnt += len_sent;

          if ((ptr_cdc_data->data_to_send_cnt == ptr_cdc_data->data_sent_cnt) &&
             ((len_sent % max_packet_size) == 0U)) {
            // If there are no more bytes available to be sent and if last
            // packet size was same as maximum packet size
            ptr_cdc_data->send_zlp = 1U;        // ZLP packet should be sent
          }
        } else {
          // Handle sent ZLP
          ptr_cdc_data->send_zlp = 0U;
        }
      }

      // Number of data bytes to send
      len_to_send = ptr_cdc_data->data_to_send_cnt - ptr_cdc_data->data_sent_cnt;
      if (len_to_send > max_packet_size) {
        len_to_send = max_packet_size;
      }
      len_till_wrap = 0xFFFFFFFFU;

      if (len_to_send != 0U) {                  // If there is data available to be sent
        if (ptr_cdc_data->send_zlp != 0U){      // If planned ZLP but new data is available
          ptr_cdc_data->send_zlp = 0U;          // Invalidate ZLP request
        }

        if ((ptr_cdc_data->ptr_data_sent >= ptr_cdc_data->ptr_data_to_send) &&
           ((ptr_cdc_data->ptr_data_sent + len_to_send) > (ptr_cdc_cfg->send_buf + ptr_cdc_cfg->send_buf_sz))) {
          // If data before end of buf available and if available data wraps
          // around the end of the send buffer
          // Calculate bytes to send until end of send buf
          len_till_wrap = (uint32_t)ptr_cdc_cfg->send_buf + ptr_cdc_cfg->send_buf_sz - (uint32_t)ptr_cdc_data->ptr_data_sent;
        }

        send_flag   = true;
      } else if (ptr_cdc_data->send_zlp != 0U) {// If ZLP should be sent
        len_to_send = 0U;
        send_flag   = true;
      } else {                                  // There is nothing more to send
        send_flag   = false;
      }
      if (send_flag) {
        ptr_cdc_data->send_active = 1U;
        ptr_dest_buf = ptr_cdc_cfg->send_mps_buf;
        ptr_src_buf  = ptr_cdc_data->ptr_data_sent;
        len          = len_to_send;
        if (len >= len_till_wrap) {             // If data wraps, copy wrapping data to intermediate aligned buffer
          memcpy(ptr_dest_buf, ptr_src_buf, len_till_wrap);
          ptr_dest_buf += len_till_wrap;
          ptr_src_buf   = ptr_cdc_cfg->send_buf;
          len          -= len_till_wrap;
        }
        if (len != 0U) {                        // If more data is available, copy it
          memcpy(ptr_dest_buf, ptr_src_buf, len);
        }
        status = USBD_DriverEndpointTransfer(device, USB_ENDPOINT_IN(ptr_cdc_cfg->ep_bulk_in), ptr_cdc_cfg->send_mps_buf, len_to_send);
        if (status != usbOK) {
          ptr_cdc_data->send_active = 0U;
        }
      } else {
        ptr_cdc_data->send_active = 0U;
      }
    } else {                                    // If handling RNDIS
      if (ptr_cdc_data->send_active == 1U) {
        if (len_sent != 0U) {                   // If data was sent
          ptr_cdc_data->data_to_send_cnt = 0U;
          if ((len_sent % max_packet_size) == 0U) {
            // If last packet size was same as maximum packet size
            ptr_cdc_data->send_zlp = 1U;        // ZLP packet should be sent
          }
        } else {
          // Handle sent ZLP
          ptr_cdc_data->send_zlp = 0U;
        }
      }

      if (ptr_cdc_data->send_zlp != 0U) {       // If ZLP should be sent
        len_to_send = 0U;
        send_flag   = true;
      } else if (ptr_cdc_data->data_to_send_cnt != 0U) {        // If there is data available to be sent
        len_to_send = ptr_cdc_data->data_to_send_cnt;
        send_flag   = true;
      } else {                                  // There is nothing more to send
        send_flag   = false;
      }

      if (send_flag) {
        ptr_cdc_data->send_active = 1U;
        status = USBD_DriverEndpointTransfer(device, USB_ENDPOINT_IN(ptr_cdc_cfg->ep_bulk_in), ptr_cdc_data->ptr_data_to_send, len_to_send);
        if (status != usbOK) {
          ptr_cdc_data->send_active = 0U;
        }
      } else {
        ptr_cdc_data->send_active = 0U;
      }
    }

    // Call sent callback
    if ((ptr_cdc_data->send_active == 0U) && (fpUSBD_CDC_ACM_DataSent[instance] != NULL)) {
      fpUSBD_CDC_ACM_DataSent[instance]();      // Callback when all data was sent
    }
  } else {                                      // If handling CDC NCM class
    if (ptr_cdc_data->send_active == 1U) {
      if (len_sent != 0U) {
        // Handle sent data
        if (ptr_cdc_cfg->ncm_raw == 0U) {       // If raw mode is not active
          ptr_cdc_data->ncm_ntb_in_buf_len[ptr_cdc_data->data_sent_cnt & ((uint32_t)ptr_cdc_cfg->ncm_in_buf_cnt - 1U)] = 0U;
          ptr_cdc_data->ptr_data_sent += ptr_cdc_cfg->ncm_ntb_in_max_size;
          if (ptr_cdc_data->ptr_data_sent >= (ptr_cdc_cfg->send_buf + ptr_cdc_cfg->send_buf_sz)) {
            // If wrap over buffers end, move to beginning of the buffers
            ptr_cdc_data->ptr_data_sent = ptr_cdc_cfg->send_buf;
          }
          ptr_cdc_data->data_sent_cnt++;
        }
        if ((len_sent & (max_packet_size - 1U)) == 0U) {
          ptr_cdc_data->send_zlp = 1U;
        }
      } else {
        // Handle sent ZLP
        ptr_cdc_data->send_zlp = 0U;
      }
    }

    // Number of data bytes to send
    len_to_send = 0U;
    if (ptr_cdc_data->send_zlp != 0U) {         // If ZLP should be sent
      send_flag   = true;
    } else {
      if (((ptr_cdc_cfg->ncm_raw == 0U) &&      // If raw mode is not active
           (ptr_cdc_data->data_to_send_cnt != ptr_cdc_data->data_sent_cnt)) || 
           (ptr_cdc_data->send_active == 2U)) { // If there is data to be sent
        len_to_send = ptr_cdc_data->ncm_ntb_in_buf_len[ptr_cdc_data->data_sent_cnt & ((uint32_t)ptr_cdc_cfg->ncm_in_buf_cnt - 1U)];
        send_flag = true;
      } else {
        send_flag = false;                      // There is nothing more to send
      }
    }

    if (send_flag) {
      ptr_cdc_data->send_active = 1U;
      status = USBD_DriverEndpointTransfer(device, USB_ENDPOINT_IN(ptr_cdc_cfg->ep_bulk_in), ptr_cdc_data->ptr_data_sent, len_to_send);
      if (status != usbOK) {
        ptr_cdc_data->send_active = 0U;
      }
    } else {
      ptr_cdc_data->send_active = 0U;
    }

    // Call sent callback
    if ((ptr_cdc_data->send_active == 0U) && (fpUSBD_CDC_NCM_NTB_IN_Sent[instance] != NULL)) {
      EvrUSBD_CDC_NCM_OnNTB_IN_Sent(instance);
      fpUSBD_CDC_NCM_NTB_IN_Sent[instance]();   // Callback when all data was sent
    }
  }
}


/************************** Thread functions **********************************/

/// \brief Thread Function: CDC Interrupt Endpoint event handling
/// \param[in]     arg                  index of CDC instance
void USBD_CDC_Int_Thread (void *arg) {
  uint32_t event;
  uint8_t  instance;

  instance = (uint8_t)((uint32_t)arg);
  if (USBD_CDC_CheckInstance(instance) != usbOK) {
    return;
  }

  for (;;) {
    event = USBD_ThreadFlagsWait (0xFFFFFFFFU);
    if ((event & 0x8000000U) == 0U) {
      if (((event >> 8) & ARM_USBD_EVENT_IN) != 0U) {
        USBD_CDC_EpIntIn (instance);
      }
    }
  }
}

void USBD_CDC0_Int_Thread (void const *arg) { (void)arg; USBD_CDC_Int_Thread ((void *)0U); }
void USBD_CDC1_Int_Thread (void const *arg) { (void)arg; USBD_CDC_Int_Thread ((void *)1U); }
void USBD_CDC2_Int_Thread (void const *arg) { (void)arg; USBD_CDC_Int_Thread ((void *)2U); }
void USBD_CDC3_Int_Thread (void const *arg) { (void)arg; USBD_CDC_Int_Thread ((void *)3U); }
void USBD_CDC4_Int_Thread (void const *arg) { (void)arg; USBD_CDC_Int_Thread ((void *)4U); }
void USBD_CDC5_Int_Thread (void const *arg) { (void)arg; USBD_CDC_Int_Thread ((void *)5U); }
void USBD_CDC6_Int_Thread (void const *arg) { (void)arg; USBD_CDC_Int_Thread ((void *)6U); }
void USBD_CDC7_Int_Thread (void const *arg) { (void)arg; USBD_CDC_Int_Thread ((void *)7U); }

/// \brief Thread Function: CDC Bulk Endpoints events handling
/// \param[in]     arg                  index of CDC instance
void USBD_CDC_Bulk_Thread (void *arg) {
  uint32_t event;
  uint8_t  instance;

  instance = (uint8_t)((uint32_t)arg);
  if (USBD_CDC_CheckInstance(instance) != usbOK) {
    return;
  }

  for (;;) {
    event = USBD_ThreadFlagsWait (0xFFFFFFFFU);
    if ((event & 0x8000000U) == 0U) {
      if (((event >> 8) & ARM_USBD_EVENT_OUT) != 0U) {
        USBD_CDC_EpBulkOut (instance);
      }
      if (((event >> 8) & ARM_USBD_EVENT_IN)  != 0U) {
        USBD_CDC_EpBulkIn  (instance);
      }
    }
  }
}

void USBD_CDC0_Bulk_Thread (void const *arg) { (void)arg; USBD_CDC_Bulk_Thread ((void *)0U); }
void USBD_CDC1_Bulk_Thread (void const *arg) { (void)arg; USBD_CDC_Bulk_Thread ((void *)1U); }
void USBD_CDC2_Bulk_Thread (void const *arg) { (void)arg; USBD_CDC_Bulk_Thread ((void *)2U); }
void USBD_CDC3_Bulk_Thread (void const *arg) { (void)arg; USBD_CDC_Bulk_Thread ((void *)3U); }
void USBD_CDC4_Bulk_Thread (void const *arg) { (void)arg; USBD_CDC_Bulk_Thread ((void *)4U); }
void USBD_CDC5_Bulk_Thread (void const *arg) { (void)arg; USBD_CDC_Bulk_Thread ((void *)5U); }
void USBD_CDC6_Bulk_Thread (void const *arg) { (void)arg; USBD_CDC_Bulk_Thread ((void *)6U); }
void USBD_CDC7_Bulk_Thread (void const *arg) { (void)arg; USBD_CDC_Bulk_Thread ((void *)7U); }
