/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device:Custom Class
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_CustomClass_%Instance%.c
 * Purpose: USB Device Custom Class User module
 * Rev.:    V6.7.4
 *----------------------------------------------------------------------------*/
/*
 * USBD_User_CustomClass_%Instance%.c is a code template for the Custom Class %Instance% 
 * class request handling. It allows user to handle all Custom Class class 
 * requests.
 *
 * Uncomment "Example code" lines to see example that receives data on 
 * Endpoint 1 OUT and echoes it back on Endpoint 1 IN.
 * To try the example you also have to enable Bulk Endpoint 1 IN/OUT in Custom 
 * Class configuration in USBD_Config_CustomClass_%Instance%.h file.
 */

/**
 * \addtogroup usbd_custom_classFunctions
 *
 */
 
 
//! [code_USBD_User_CustomClass]
 
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rl_usb.h"
#include "Driver_USBD.h"

//// Example code variable declaration/definition: 
//uint8_t class%Instance%_bulk_out_buf[64] = { 0 };
//uint8_t class%Instance%_bulk_in_buf [64] = { 0 };
 
// \brief Callback function called during USBD_Initialize to initialize the USB Custom class instance
void USBD_CustomClass%Instance%_Initialize (void) {
  // Handle Custom Class Initialization
}
 
// \brief Callback function called during USBD_Uninitialize to de-initialize the USB Custom class instance
void USBD_CustomClass%Instance%_Uninitialize (void) {
  // Handle Custom Class De-initialization
}
 
// \brief Callback function called upon USB Bus Reset signaling
void USBD_CustomClass%Instance%_Reset (void) {
  // Handle USB Bus Reset Event
}
 
// \brief Callback function called when Endpoint Start was requested (by activating interface or configuration)
// \param[in]     ep_addr       endpoint address.
void USBD_CustomClass%Instance%_EndpointStart (uint8_t ep_addr) {
  // Start communication on Endpoint
  (void)ep_addr;
 
//  // Example code start reception on Endpoint 1 OUT: 
//  if (ep_addr == USB_ENDPOINT_OUT(1)) {
//    USBD_EndpointRead(0, USB_ENDPOINT_OUT(1), class%Instance%_bulk_out_buf, 64);
//  }
}
 
// \brief Callback function called when Endpoint Stop was requested (by de-activating interface or activating configuration 0)
// \param[in]     ep_addr       endpoint address.
void USBD_CustomClass%Instance%_EndpointStop (uint8_t ep_addr) {
  // Handle Endpoint communication stopped
  (void)ep_addr;
}
 
// \brief Callback function called when Custom Class %Instance% received SETUP PACKET on Control Endpoint 0
//        (this callback will be called only for Class Requests (USB_REQUEST_CLASS) if it was not processed 
//         previously by Device callback)
// \param[in]     setup_packet            pointer to received setup packet.
// \param[out]    buf                     pointer to data buffer used for data stage requested by setup packet.
// \param[out]    len                     pointer to number of data bytes in data stage requested by setup packet.
// \return        usbdRequestStatus       enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed:request was not processed; processing will be done by USB library
// \return        usbdRequestOK:          request was processed successfully (send Zero-Length Packet if no data stage)
// \return        usbdRequestStall:       request was processed but is not supported (stall Endpoint 0)
usbdRequestStatus USBD_CustomClass%Instance%_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) {
  (void)buf;
  (void)len;
 
  switch (setup_packet->bmRequestType.Recipient) {
    case USB_REQUEST_TO_DEVICE:
      break;
    case USB_REQUEST_TO_INTERFACE:
      break;
    case USB_REQUEST_TO_ENDPOINT:
      break;
    default:
      break;
  }
 
  return usbdRequestNotProcessed;
}
 
// \brief Callback function called when SETUP PACKET was processed by USB library
//        (this callback will be called only for Class Requests (USB_REQUEST_CLASS) if it was not processed 
//         previously by Device callback nor by Custom Class callback)
// \param[in]     setup_packet            pointer to processed setup packet.
void USBD_CustomClass%Instance%_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet) {
 
  switch (setup_packet->bmRequestType.Recipient) {
    case USB_REQUEST_TO_DEVICE:
      break;
    case USB_REQUEST_TO_INTERFACE:
      break;
    case USB_REQUEST_TO_ENDPOINT:
      break;
    default:
      break;
  }
}
 
// \brief Callback function called when Custom Class %Instance% received OUT DATA on Control Endpoint 0
//        (this callback will be called only for Class Requests (USB_REQUEST_CLASS) if it was not processed 
//         previously by Device callback)
// \param[in]     len                     number of received data bytes.
// \return        usbdRequestStatus       enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed:request was not processed; processing will be done by USB library
// \return        usbdRequestOK:          request was processed successfully (send Zero-Length Packet)
// \return        usbdRequestStall:       request was processed but is not supported (stall Endpoint 0)
// \return        usbdRequestNAK:         request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_CustomClass%Instance%_Endpoint0_OutDataReceived (uint32_t len) {
  (void)len;
 
  return usbdRequestNotProcessed;
}
 
// \brief Callback function called when Custom Class %Instance% sent IN DATA on Control Endpoint 0
//        (this callback will be called only for Class Requests (USB_REQUEST_CLASS) if it was not processed 
//         previously by Device callback)
// \param[in]     len                     number of sent data bytes.
// \return        usbdRequestStatus       enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed:request was not processed; processing will be done by USB library
// \return        usbdRequestOK:          request was processed successfully (return ACK)
// \return        usbdRequestStall:       request was processed but is not supported (stall Endpoint 0)
// \return        usbdRequestNAK:         request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_CustomClass%Instance%_Endpoint0_InDataSent (uint32_t len) {
  (void)len;
 
  return usbdRequestNotProcessed;
}
 
// \brief Callback function called when DATA was sent or received on Endpoint n
// \param[in]     event                   event on Endpoint:
//                                          - ARM_USBD_EVENT_OUT = data OUT received
//                                          - ARM_USBD_EVENT_IN  = data IN  sent
void USBD_CustomClass%Instance%_Endpoint1_Event  (uint32_t event) {
////Example code variable declaration/definition
//  uint32_t len;

  // Handle Endpoint 1 events
  if (event & ARM_USBD_EVENT_OUT) {     // OUT event

//    // Example code upon reception on Endpoint 1 OUT echo received data on 
//    // Endpoint 1 IN: 
//    // Get number of received data bytes
//    len = USBD_EndpointReadGetResult(0, USB_ENDPOINT_OUT(1));
//    // Copy received data bytes to transmit buffer
//    memcpy(class%Instance%_bulk_in_buf, class%Instance%_bulk_out_buf, len);
//    // Transmit back data bytes
//    USBD_EndpointWrite(0, USB_ENDPOINT_IN(1) , class%Instance%_bulk_in_buf, len);

//    // Restart reception on Endpoint 1 OUT: 
//    USBD_EndpointRead (0, USB_ENDPOINT_OUT(1), class%Instance%_bulk_out_buf, 64);
  }
  if (event & ARM_USBD_EVENT_IN) {      // IN event

  }
}
void USBD_CustomClass%Instance%_Endpoint2_Event  (uint32_t event) {
  // Handle Endpoint 2 events
  (void)event;
}
void USBD_CustomClass%Instance%_Endpoint3_Event  (uint32_t event) {
  // Handle Endpoint 3 events
  (void)event;
}
void USBD_CustomClass%Instance%_Endpoint4_Event  (uint32_t event) {
  // Handle Endpoint 4 events
  (void)event;
}
void USBD_CustomClass%Instance%_Endpoint5_Event  (uint32_t event) {
  // Handle Endpoint 5 events
  (void)event;
}
void USBD_CustomClass%Instance%_Endpoint6_Event  (uint32_t event) {
  // Handle Endpoint 6 events
  (void)event;
}
void USBD_CustomClass%Instance%_Endpoint7_Event  (uint32_t event) {
  // Handle Endpoint 7 events
  (void)event;
}
void USBD_CustomClass%Instance%_Endpoint8_Event  (uint32_t event) {
  // Handle Endpoint 8 events
  (void)event;
}
void USBD_CustomClass%Instance%_Endpoint9_Event  (uint32_t event) {
  // Handle Endpoint 9 events
  (void)event;
}
void USBD_CustomClass%Instance%_Endpoint10_Event (uint32_t event) {
  // Handle Endpoint 10 events
  (void)event;
}
void USBD_CustomClass%Instance%_Endpoint11_Event (uint32_t event) {
  // Handle Endpoint 11 events
  (void)event;
}
void USBD_CustomClass%Instance%_Endpoint12_Event (uint32_t event) {
  // Handle Endpoint 12 events
  (void)event;
}
void USBD_CustomClass%Instance%_Endpoint13_Event (uint32_t event) {
  // Handle Endpoint 13 events
  (void)event;
}
void USBD_CustomClass%Instance%_Endpoint14_Event (uint32_t event) {
  // Handle Endpoint 14 events
  (void)event;
}
void USBD_CustomClass%Instance%_Endpoint15_Event (uint32_t event) {
  // Handle Endpoint 15 events
  (void)event;
}
 
//! [code_USBD_User_CustomClass]
