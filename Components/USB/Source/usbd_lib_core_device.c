/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_core_custom_device.c
 * Purpose: USB Device - Device core module
 *----------------------------------------------------------------------------*/

#include "usbd_lib_core_device.h"

#include "usb_lib_debug.h"
#include "usbd_lib_config_def.h"
#include "usbd_lib_core_custom_class.h"
#include "usbd_evr.h"


/// \brief USB Device Endpoint 0 Setup Packet Received callback
/// \param[in]     device               device index.
/// \return        usbdRequestStatus    enumerator value indicating the function execution status :
///                                       - value usbdRequestNotProcessed : request was not processed; processing will be done by USB library
///                                       - value usbdRequestOK :           request was processed successfully (send Zero-Length Packet if no data stage)
///                                       - value usbdRequestStall :        request was processed but is not supported (stall Endpoint 0)
__WEAK usbdRequestStatus USBD_Core_Endpoint0_SetupPacketReceived (uint8_t device) {
  usbd_data_t       *ptr_dev_data;
  usbdRequestStatus  status;

  if (device >= usbd_dev_num) { return usbdRequestNotProcessed; }

  ptr_dev_data = usbd_dev_ptr[device]->data_ptr; if (ptr_dev_data == NULL) { return usbdRequestNotProcessed; }

  // Allow SETUP PACKET processing at device level
  ptr_dev_data->buf = NULL;
  ptr_dev_data->len = 0U;
  status = fpUSBD_Device_Endpoint0_SetupPacketReceived [device] ((const USB_SETUP_PACKET *)&ptr_dev_data->setup_packet, &ptr_dev_data->buf, &ptr_dev_data->len);
  EvrUSBD_Core_OnSetupPacketReceived(device, status);
  EvrUSBD_Core_OnSetupPacketReceivedDetail(device, (const void *)&ptr_dev_data->setup_packet, ptr_dev_data->len);
  if (status != usbdRequestNotProcessed) {
    return status;
  }

  // Allow SETUP PACKET processing at custom class level
  status = USBD_Class_Endpoint0_SetupPacketReceived (device);

  return status;
}

/// \brief USB Device Endpoint 0 Setup Packet processed by USB Library callback
/// \param[in]     device               device index.
__WEAK void USBD_Core_Endpoint0_SetupPacketProcessed (uint8_t device) {
  const usbd_data_t *ptr_dev_data;

  if (device >= usbd_dev_num) { return; }

  ptr_dev_data = usbd_dev_ptr[device]->data_ptr; if (ptr_dev_data == NULL) { return; }

  // Pass SETUP PACKET processed information to device level handling
  fpUSBD_Device_Endpoint0_SetupPacketProcessed [device] ((const USB_SETUP_PACKET *)&ptr_dev_data->setup_packet);
  EvrUSBD_Core_OnSetupPacketProcessed(device);
  EvrUSBD_Core_OnSetupPacketProcessedDetail(device, (const void *)&ptr_dev_data->setup_packet);

  // Pass SETUP PACKET processed information to custom class level handling
  USBD_Class_Endpoint0_SetupPacketProcessed (device);
}

/// \brief USB Device Endpoint 0 Out Data Received callback
/// \param[in]     device               device index.
/// \return        usbdRequestStatus    enumerator value indicating the function execution status :
///                                       - value usbdRequestNotProcessed : request was not processed; processing will be done by USB library
///                                       - value usbdRequestOK :           request was processed successfully (send Zero-Length Packet)
///                                       - value usbdRequestStall :        request was processed but is not supported (stall Endpoint 0)
///                                       - value usbdRequestNAK :          request was processed but the device is busy (return NAK)
__WEAK usbdRequestStatus USBD_Core_Endpoint0_OutDataReceived (uint8_t device) {
  const usbd_data_t       *ptr_dev_data;
        usbdRequestStatus  status;

  if (device >= usbd_dev_num) { return usbdRequestNotProcessed; }

  ptr_dev_data = usbd_dev_ptr[device]->data_ptr; if (ptr_dev_data == NULL) { return usbdRequestNotProcessed; }

  // Allow OUT DATA processing at device level
  status = fpUSBD_Device_Endpoint0_OutDataReceived [device] (ptr_dev_data->len);
  EvrUSBD_Core_OnOutDataReceived(device, ptr_dev_data->len, status);
  if (status != usbdRequestNotProcessed) { return status; }

  // Allow OUT DATA processing at custom class level
  status = USBD_Class_Endpoint0_OutDataReceived (device);

  return status;
}

/// \brief USB Device Endpoint 0 In Data Received callback
/// \param[in]     device               device index.
/// \return        usbdRequestStatus    enumerator value indicating the function execution status :
///                                       - value usbdRequestNotProcessed : request was not processed; processing will be done by USB library
///                                       - value usbdRequestOK :           request was processed successfully (return ACK)
///                                       - value usbdRequestStall :        request was processed but is not supported (stall Endpoint 0)
///                                       - value usbdRequestNAK :          request was processed but the device is busy (return NAK)
__WEAK usbdRequestStatus USBD_Core_Endpoint0_InDataSent (uint8_t device) {
  const usbd_data_t       *ptr_dev_data;
        usbdRequestStatus  status;

  if (device >= usbd_dev_num) { return usbdRequestNotProcessed; }

  ptr_dev_data = usbd_dev_ptr[device]->data_ptr; if (ptr_dev_data == NULL) { return usbdRequestNotProcessed; }

  // Allow IN DATA processing at device level
  status = fpUSBD_Device_Endpoint0_InDataSent  [device] (ptr_dev_data->len);
  EvrUSBD_Core_OnInDataSent(device, ptr_dev_data->len, status);
  if (status != usbdRequestNotProcessed) { return status; }

  // Allow IN DATA processing at device level
  status = USBD_Class_Endpoint0_InDataSent (device);

  return status;
}
