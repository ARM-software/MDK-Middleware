/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host:Custom Class
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBH_User_CustomClass.c
 * Purpose: USB Host (USBH) - Custom Class User template
 * Rev.:    V6.3.2
 *----------------------------------------------------------------------------*/
/*
 * USBH_User_CustomClass.c is a code template for the Custom Class driver
 * implementation on the USB Host stack.
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
 
/**
 * \addtogroup usbh_custom_classFunctions
 *
 */
 
 
//! [code_USBH_User_CustomClass]
 
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
 
  (void)ptr_dev_desc;
 
  USBH_CC_Device = device;              // Store device
  for (i = 0U; i < 8U; i++) {           // Clear all pipe handles
    USBH_CC_PipeHandle[i] = 0U;
  }
  ptr_if_desc = (USB_INTERFACE_DESCRIPTOR *)((uint32_t)ptr_cfg_desc + ptr_cfg_desc->bLength);
  num = ptr_if_desc->bNumEndpoints;     // Number of endpoints
 
  // Check if this is the supported device (VID, PID)
  // Vendor ID value:  ptr_dev_desc->idVendor
  // Product ID value: ptr_dev_desc->idProduct
 
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
 
  (void)instance;
 
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
  (void)instance;
 
  //  Add code for initializing device
 
  return usbOK;
}
 
/// \brief De-initialize Custom Class Device instance
/// \param[in]     instance             index of custom class device instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
usbStatus USBH_CustomClass_Uninitialize (uint8_t instance) {
  (void)instance;
 
  // Add code for de-initializing device
 
  return usbOK;
}
 
//! [code_USBH_User_CustomClass]
