/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_Device_SerNum_%Instance%.c
 * Purpose: USB Device User module
 * Rev.:    V1.2.1
 *----------------------------------------------------------------------------*/
/*
 * USBD_User_Device_SerNum_%Instance%.c is a code template for the user specific 
 * Device events and Control Endpoint 0 requests handling. It demonstrates how 
 * to specify serial number at runtime instead of fixed one specified in 
 * USBD_Config_%Instance%.h file.
 */
 
/**
 * \addtogroup usbd_coreFunctions_api
 *
 */
 
 
//! [code_USBD_User_Device]
 
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rl_usb.h"
 
 
static bool    handle_request;
static uint8_t ser_no_string_desc[32];  // String Descriptor runtime value
 
// \brief Callback function called during USBD_Initialize to initialize the USB Device
void USBD_Device%Instance%_Initialize (void) {
  // Handle Device Initialization
 
  handle_request = false;
}
 
// \brief Callback function called during USBD_Uninitialize to de-initialize the USB Device
void USBD_Device%Instance%_Uninitialize (void) {
  // Handle Device De-initialization
}
 
// \brief Callback function called when VBUS level changes
// \param[in]     level                current VBUS level
//                                       - true: VBUS level is high
//                                       - false: VBUS level is low
void USBD_Device%Instance%_VbusChanged (bool level) {
  (void)level;
}
 
// \brief Callback function called upon USB Bus Reset signaling
void USBD_Device%Instance%_Reset (void) {
}
 
// \brief Callback function called when USB Bus speed was changed to high-speed
void USBD_Device%Instance%_HighSpeedActivated (void) {
}
 
// \brief Callback function called when USB Bus goes into suspend mode (no bus activity for 3 ms)
void USBD_Device%Instance%_Suspended (void) {
}
 
// \brief Callback function called when USB Bus activity resumes
void USBD_Device%Instance%_Resumed (void) {
}
 
// \brief Callback function called when Device was successfully enumerated
// \param[in]     val                  current configuration value
//                                       - value 0: not configured
//                                       - value > 0: active configuration
void USBD_Device%Instance%_ConfigurationChanged (uint8_t val) {
  (void)val;
}
 
// \brief Callback function called when Set Feature for Remote Wakeup comes over Control Endpoint 0
void USBD_Device%Instance%_EnableRemoteWakeup (void) {
}
 
// \brief Callback function called when Clear Feature for Remote Wakeup comes over Control Endpoint 0
void USBD_Device%Instance%_DisableRemoteWakeup (void) {
}
 
// \brief Callback function called when Device %Instance% received SETUP PACKET on Control Endpoint 0
// \param[in]     setup_packet             pointer to received setup packet.
// \param[out]    buf                      pointer to data buffer used for data stage requested by setup packet.
// \param[out]    len                      pointer to number of data bytes in data stage requested by setup packet.
// \return        usbdRequestStatus        enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed: request was not processed; processing will be done by USB library
// \return        usbdRequestOK:           request was processed successfully (send Zero-Length Packet if no data stage)
// \return        usbdRequestStall:        request was processed but is not supported (stall Endpoint 0)
usbdRequestStatus USBD_Device%Instance%_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) {
 
  switch (setup_packet->bmRequestType.Type) {
    case USB_REQUEST_STANDARD:
      // Catch Get String Descriptor request for serial number string and return desired string:
      if ((setup_packet->bmRequestType.Dir       == USB_REQUEST_DEVICE_TO_HOST) &&      // Request to get
          (setup_packet->bmRequestType.Recipient == USB_REQUEST_TO_DEVICE     ) &&      // from device
          (setup_packet->bRequest                == USB_REQUEST_GET_DESCRIPTOR) &&      // the descriptor
         ((setup_packet->wValue >> 8)            == USB_STRING_DESCRIPTOR_TYPE) &&      // String Descriptor Type
         ((setup_packet->wValue & 0xFFU)         == 0x03U                     ) &&      // Index of String = 3
          (setup_packet->wIndex                  == 0x0409U                   )) {      // Language ID = 0x0409 = English (United States)
        ser_no_string_desc[0] = 26U;    // Total size of String Descriptor
        ser_no_string_desc[1] = USB_STRING_DESCRIPTOR_TYPE;   // String Descriptor Type
        memcpy(&ser_no_string_desc[2], u"0001A0000001", 24);  // Serial Number String value
        *buf = ser_no_string_desc;      // Return pointer to prepared String Descriptor
        if (setup_packet->wLength >= 26) {
          *len = 26U;                   // Number of bytes of whole String Descriptor
        } else {
          *len = setup_packet->wLength; // Requested number of bytes of String Descriptor
        }
        handle_request = true;          // This request is handled
        return usbdRequestOK;           // Return status that custom handling for this request is used
      }
      break;
    case USB_REQUEST_CLASS:
      break;
    case USB_REQUEST_VENDOR:
      break;
    case USB_REQUEST_RESERVED:
      break;
    default:
      break;
  }
 
  return usbdRequestNotProcessed;
}
 
// \brief Callback function called when SETUP PACKET was processed by USB library
// \param[in]     setup_packet            pointer to processed setup packet.
void USBD_Device%Instance%_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet) {
 
  switch (setup_packet->bmRequestType.Type) {
    case USB_REQUEST_STANDARD:
      break;
    case USB_REQUEST_CLASS:
      break;
    case USB_REQUEST_VENDOR:
      break;
    case USB_REQUEST_RESERVED:
      break;
    default:
      break;
  }
}
 
// \brief Callback function called when Device %Instance% received OUT DATA on Control Endpoint 0
// \param[in]     len                      number of received data bytes.
// \return        usbdRequestStatus        enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed: request was not processed; processing will be done by USB library
// \return        usbdRequestOK:           request was processed successfully (send Zero-Length Packet)
// \return        usbdRequestStall:        request was processed but is not supported (stall Endpoint 0)
// \return        usbdRequestNAK:          request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_Device%Instance%_Endpoint0_OutDataReceived (uint32_t len) {
 
  (void)len;
 
  return usbdRequestNotProcessed;
}
 
// \brief Callback function called when Device %Instance% sent IN DATA on Control Endpoint 0
// \param[in]     len                      number of sent data bytes.
// \return        usbdRequestStatus        enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed: request was not processed; processing will be done by USB library
// \return        usbdRequestOK:           request was processed successfully (return ACK)
// \return        usbdRequestStall:        request was processed but is not supported (stall Endpoint 0)
// \return        usbdRequestNAK:          request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_Device%Instance%_Endpoint0_InDataSent (uint32_t len) {
 
  (void)len;
 
  if (handle_request) {                 // If String Descriptor was sent
    handle_request = false;
    return usbdRequestOK;               // Acknowledge custom handled request
  }
 
  return usbdRequestNotProcessed;
}
 
//! [code_USBD_User_Device]
