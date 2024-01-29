/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_Device_%Instance%.c
 * Purpose: USB Device User module
 * Rev.:    V6.8.2
 *----------------------------------------------------------------------------*/
/*
 * USBD_User_Device_%Instance%.c is a code template for the user specific 
 * Device events and Control Endpoint 0 requests handling. It allows user to 
 * handle all user level events as well as to intercept all Control Endpoint 0 
 * requests and handle them overriding the default USB Library handling.
 *
 * Uncomment "Example code" lines to override USB Device Descriptor.
 *
 * IMPORTANT NOTE!!!: Maximum packet size must not be set to less that 32 bytes!
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
 
 
//// Example code specifying USB Device Descriptor: 
//// Device %Instance% USB Device Descriptor
//static const uint8_t device%Instance%_dev_desc[] = {
//  18,                                   /* bLength            = 18 bytes      */
//  USB_DEVICE_DESCRIPTOR_TYPE,           /* bDescriptorType    = Device Desc   */
//  0x00, 0x02,                           /* bcdUSB             = 2.00          */
//  0x00,                                 /* bDeviceClass       = Defined in IF */
//  0x00,                                 /* bDeviceSubClass    = Defined in IF */
//  0x00,                                 /* bDeviceProtocol    = Defined in IF */
//  64,                                   /* bMaxPacketSize0    = 64 bytes !!! Must be same as USBD%Instance%_MAX_PACKET0 in USBD_Config_%Instance%.c */
//  0x51, 0xC2,                           /* idVendor           = 0xC251        */
//  0x01, 0x00,                           /* idProduct          = 1             */
//  0x00, 0x01,                           /* bcdDevice          = 1.00          */
//  0x01,                                 /* iManufacturer      = String1       */
//  0x02,                                 /* iProduct           = String2       */
//  0x03,                                 /* iSerialNumber      = String3       */
//  0x01                                  /* bNumConfigurations = 1 config      */
//};
 
//static bool handle_request;
 
// \brief Callback function called during USBD_Initialize to initialize the USB Device
void USBD_Device%Instance%_Initialize (void) {
  // Handle Device Initialization
 
//  handle_request = false;
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
 
  (void)buf;
  (void)len;
 
  switch (setup_packet->bmRequestType.Type) {
    case USB_REQUEST_STANDARD:
//      // Example code handling Get Device Descriptor request:
//      if ((setup_packet->bmRequestType.Dir       == USB_REQUEST_DEVICE_TO_HOST) &&      // Request to get
//          (setup_packet->bmRequestType.Recipient == USB_REQUEST_TO_DEVICE     ) &&      // from device
//          (setup_packet->bRequest                == USB_REQUEST_GET_DESCRIPTOR) &&      // the descriptor
//         ((setup_packet->wValue >> 8)            == USB_DEVICE_DESCRIPTOR_TYPE) &&      // Device Descriptor Type
//          (setup_packet->wIndex                  == 0U                        )) {      // Index = 0
//        *buf = (uint8_t *)((uint32_t)device%Instance%_dev_desc);
//        *len = sizeof               (device%Instance%_dev_desc);
//        handle_request = true;
//        return usbdRequestOK;
//      }
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
 
//  // Example code after Get Device Descriptor was sent:
//  if (handle_request) {                 // If Device Descriptor was sent
//    handle_request = false;
//    return usbdRequestOK;               // Acknowledge custom handled request
//  }
 
  return usbdRequestNotProcessed;
}
 
//! [code_USBD_User_Device]
