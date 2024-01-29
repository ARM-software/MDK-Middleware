/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host:Custom Class
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBH_PL2303.c
 * Purpose: USB Host (USBH) - Custom Class - Prolific PL2303 USB to serial
 *          RS232 adapter driver
 * Rev.:    V6.3.2
 *----------------------------------------------------------------------------*/
/*
 * USBH_PL2303.c is a Prolific PL2303 USB to serial RS232 adapter driver
 * for USB Host stack implemented as USB Host Custom Class driver.
 * Prolific PL2303 USB to serial RS232 adapter is similar to the CDC device
 * but using vendor specific class device with interface containing
 * 1 Bulk IN, 1 Bulk OUT and 1 Interrupt IN Endpoints.
 * This implementation supports only one instance.
 *
 * The template implements 4 callback functions called by the USB Host core
 * when device is connected or disconnected, these are:
 *   USBH_CustomClass_Configure
 *   USBH_CustomClass_Unconfigure
 *   USBH_CustomClass_Initialize
 *   USBH_CustomClass_Uninitialize
 * 
 * First to enable USB Host Controller (if not already enabled) call:
 *   USBH_Initialize (ctrl_num);
 */
 
#include <stdio.h>
#include <stdint.h>
#include "rl_usb.h"
 
// Interface class, subclass and protocol of the device that is supported
#define CUSTOM_CLASS_IF_CLASS           USB_DEVICE_CLASS_VENDOR_SPECIFIC
#define CUSTOM_CLASS_IF_SUBCLASS        0
#define CUSTOM_CLASS_IF_PROTOCOL        0
 
extern
uint8_t    USBH_CC_Device;
uint8_t    USBH_CC_Device = 0U;         // Device used for USB transfers
extern 
USBH_PIPE_HANDLE USBH_CC_PipeHandle[8];
USBH_PIPE_HANDLE USBH_CC_PipeHandle[8]; // Pipe Handles
 
 
/************************** Class Driver Functions ****************************/
 
/// \brief Analyze device configuration and configure resources
/// \param[in]     device               index of USB Device.
/// \param[in]     ptr_dev_desc         pointer to device descriptor.
/// \param[in]     ptr_cfg_desc         pointer to configuration descriptor.
/// \return                             index of configured custom class device instance or configuration failed :
///                                       - value <= 127 : index of configured custom class device instance
///                                       - value 255 :    configuration failed
uint8_t USBH_CustomClass_Configure (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc) {
  USB_INTERFACE_DESCRIPTOR *ptr_if_desc;
  USB_ENDPOINT_DESCRIPTOR  *ptr_ep_desc;
  USBH_PIPE_HANDLE          pipe_hndl;
  uint8_t                   num, i;
 
  USBH_CC_Device = device;              // Store device
  for (i = 0U; i < 8U; i++) {           // Clear all pipe handles
    USBH_CC_PipeHandle[i] = 0U;
  }
  ptr_if_desc = (USB_INTERFACE_DESCRIPTOR *)((uint32_t)ptr_cfg_desc + ptr_cfg_desc->bLength);
  num = ptr_if_desc->bNumEndpoints;     // Number of endpoints
 
  // Supported device: - Prolific PL2303 (VID = 0x067B, PID = 0x2303)
  if ((ptr_dev_desc->idVendor != 0x067BU) || (ptr_dev_desc->idProduct != 0x2303U)) {
    return 255U;
  }
 
  switch (ptr_if_desc->bInterfaceClass) {
    case CUSTOM_CLASS_IF_CLASS:                 // Interface class
      switch (ptr_if_desc->bInterfaceSubClass) {
        case CUSTOM_CLASS_IF_SUBCLASS:          // Interface subclass
          switch (ptr_if_desc->bInterfaceProtocol) {
            case CUSTOM_CLASS_IF_PROTOCOL:      // Interface protocol
              // Create Pipes
              ptr_ep_desc = (USB_ENDPOINT_DESCRIPTOR *)((uint32_t)ptr_if_desc + ptr_if_desc->bLength);
              i           = 0U;
              while (num-- != 0U) {
                pipe_hndl = USBH_PipeCreate (device, ptr_ep_desc->bEndpointAddress, ptr_ep_desc->bmAttributes & USB_ENDPOINT_TYPE_MASK, ptr_ep_desc->wMaxPacketSize & 0x7FFU, ptr_ep_desc->bInterval);
                if (pipe_hndl == 0U) {
                  // If creation of pipe has failed delete previously created pipes
                  for (i = 0U; i < 8U; i++) {
                    if (USBH_CC_PipeHandle[i] != 0U) {
                      (void)USBH_PipeDelete (USBH_CC_PipeHandle[i]);
                      USBH_CC_PipeHandle[i] = 0U;
                    }
                  }
                  return 255U;
                }
                USBH_CC_PipeHandle[i++] = pipe_hndl;
                ptr_ep_desc++;
              }
              return 0U;                // Device connected and configured
                                        // Only single instance supported
                                        // so it's instance index is 0
            default:
              break;
          }
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
 
  return 255U;                          // Device not handled
}
 
/// \brief De-configure resources
/// \param[in]     instance             index of custom class device instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_CustomClass_Unconfigure (uint8_t instance) {
  uint8_t i;
 
  (void)instance;                       // Only single instance is supported
 
  USBH_CC_Device = 0U;
 
  for (i = 0U; i < 8U; i++) {
    if (USBH_CC_PipeHandle[i] != 0U) {
      (void)USBH_PipeDelete (USBH_CC_PipeHandle[i]);
      USBH_CC_PipeHandle[i] = 0U;
    }
  }
 
  return usbOK;
}
 
/// \brief Initialize Custom Class Device instance
/// \param[in]     instance             index of custom class device instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_CustomClass_Initialize (uint8_t instance) {
  USB_SETUP_PACKET setup_packet;
  uint32_t         br;
  uint8_t          buf[8];
 
  (void)instance;                       // Only single instance is supported
 
  // Custom PL2303 initialization
  setup_packet.bmRequestType.Dir      = USB_REQUEST_DEVICE_TO_HOST;
  setup_packet.bmRequestType.Type     = USB_REQUEST_VENDOR;
  setup_packet.bmRequestType.Recipient= USB_REQUEST_TO_DEVICE;
  setup_packet.bRequest               = 1U;
  setup_packet.wValue                 = U16_LE(0x8484U);
  setup_packet.wIndex                 = U16_LE(0U);
  setup_packet.wLength                = U16_LE(1U);
  if (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, buf,  1U) != usbOK) { return usbClassErrorCustom; }
 
  setup_packet.bmRequestType.Dir      = USB_REQUEST_HOST_TO_DEVICE;
  setup_packet.wValue                 = U16_LE(0x0404U);
  if (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, buf,  1U) != usbOK) { return usbClassErrorCustom; }
 
  setup_packet.bmRequestType.Dir      = USB_REQUEST_DEVICE_TO_HOST;
  setup_packet.wValue                 = U16_LE(0x8484U);
  if (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, buf,  1U) != usbOK) { return usbClassErrorCustom; }
 
  setup_packet.wValue                 = U16_LE(0x8383U);
  if (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, buf,  1U) != usbOK) { return usbClassErrorCustom; }
 
  setup_packet.wValue                 = U16_LE(0x8484U);
  if (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, buf,  1U) != usbOK) { return usbClassErrorCustom; }
 
  setup_packet.bmRequestType.Dir      = USB_REQUEST_HOST_TO_DEVICE;
  setup_packet.wValue                 = U16_LE(0x0404U);
  setup_packet.wIndex                 = U16_LE(1U);
  setup_packet.wLength                = U16_LE(0U);
  if (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, NULL, 0U) != usbOK) { return usbClassErrorCustom; }
 
  setup_packet.bmRequestType.Dir      = USB_REQUEST_DEVICE_TO_HOST;
  setup_packet.wValue                 = U16_LE(0x8484U);
  setup_packet.wIndex                 = U16_LE(0U);
  setup_packet.wLength                = U16_LE(1U);
  if (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, buf,  1U) != usbOK) { return usbClassErrorCustom; }
 
  setup_packet.wValue                 = U16_LE(0x8383U);
  if (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, buf,  1U) != usbOK) { return usbClassErrorCustom; }
 
  setup_packet.bmRequestType.Dir      = USB_REQUEST_HOST_TO_DEVICE;
  setup_packet.wValue                 = U16_LE(0U);
  setup_packet.wIndex                 = U16_LE(1U);
  setup_packet.wLength                = U16_LE(0U);
  if (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, NULL, 0U) != usbOK) { return usbClassErrorCustom; }
 
  setup_packet.wValue                 = U16_LE(1U);
  setup_packet.wIndex                 = U16_LE(0U);
  if (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, NULL, 0U) != usbOK) { return usbClassErrorCustom; }
 
  setup_packet.wValue                 = U16_LE(2U);
  setup_packet.wIndex                 = U16_LE(0x44U);
  if (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, NULL, 0U) != usbOK) { return usbClassErrorCustom; }
 
  // Initial CDC SetLineCoding request: Set 9600 baud, 8 data bits, 1 stop bit, no parity
  setup_packet.bmRequestType.Dir      = USB_REQUEST_HOST_TO_DEVICE;
  setup_packet.bmRequestType.Type     = USB_REQUEST_CLASS;
  setup_packet.bmRequestType.Recipient= USB_REQUEST_TO_INTERFACE;
  setup_packet.bRequest               = 0x20U;
  setup_packet.wValue                 = U16_LE(0U);
  setup_packet.wIndex                 = U16_LE(0U);
  setup_packet.wLength                = U16_LE(7U);
  br                                  = U32_LE(9600U);  // Data terminal rate in bits per second = 9600 baud
  buf[0]                              = (uint8_t)( br        & 0xFFU);
  buf[1]                              = (uint8_t)((br >>  8) & 0xFFU);
  buf[2]                              = (uint8_t)((br >> 16) & 0xFFU);
  buf[3]                              = (uint8_t)((br >> 24) & 0xFFU);
  buf[4]                              = 0U;            // Number of stop bits = 1
  buf[5]                              = 0U;            // Parity bit type = None
  buf[6]                              = 8U;            // Number of data bits = 8
  if (USBH_ControlTransfer (USBH_CC_Device, &setup_packet, buf, 7U) != usbOK) { return usbClassErrorCustom; }
 
  return usbOK;
}
 
/// \brief De-initialize Custom Class Device instance
/// \param[in]     instance             index of custom class device instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_CustomClass_Uninitialize (uint8_t instance) {
 
  (void)instance;                       // Only single instance is supported
 
  return usbOK;
}
