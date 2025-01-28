/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_Device_0.c
 * Purpose: USB Device User module
 * Rev.:    V6.8.3
 *----------------------------------------------------------------------------*/
/*
 * USBD_User_Device_0.c is a code template for the user specific
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

#include "MW_CV_Config.h"
#if     ((MW_CV_USBD != 0U) && (MW_CV_USBD_CORE != 0U))

#include "USBD_User_Device_0.h"

#include <stdint.h>
#include <stdbool.h>

#include "rl_usb.h"


// Global structure used for testing control and results
MW_CV_USBD_CORE_t mw_cv_usbd_core;

// Local variables
static bool    handle_set_request = false;
static bool    handle_get_request = false;
static uint8_t ser_no_string_desc[32];


// \brief Callback function called during USBD_Initialize to initialize the USB Device
void USBD_Device0_Initialize (void) {
  if (!mw_cv_usbd_core.testing_active) { return; }
  mw_cv_usbd_core.cb_initialize = true;
}

// \brief Callback function called during USBD_Uninitialize to de-initialize the USB Device
void USBD_Device0_Uninitialize (void) {
  if (!mw_cv_usbd_core.testing_active) { return; }
  mw_cv_usbd_core.cb_uninitialize = true;
}

// \brief Callback function called when VBUS level changes
// \param[in]     level                current VBUS level
//                                       - true: VBUS level is high
//                                       - false: VBUS level is low
void USBD_Device0_VbusChanged (bool level) {
  (void)level;

  if (!mw_cv_usbd_core.testing_active) { return; }
  mw_cv_usbd_core.cb_vbus_changed = true;
}

// \brief Callback function called upon USB Bus Reset signaling
void USBD_Device0_Reset (void) {
  if (!mw_cv_usbd_core.testing_active) { return; }
  mw_cv_usbd_core.cb_reset = true;
}

// \brief Callback function called when USB Bus speed was changed to high-speed
void USBD_Device0_HighSpeedActivated (void) {
  if (!mw_cv_usbd_core.testing_active) { return; }
  mw_cv_usbd_core.cb_hs_activated = true;
}

// \brief Callback function called when USB Bus goes into suspend mode (no bus activity for 3 ms)
void USBD_Device0_Suspended (void) {
  if (!mw_cv_usbd_core.testing_active) { return; }
  mw_cv_usbd_core.cb_suspended = true;
}

// \brief Callback function called when USB Bus activity resumes
void USBD_Device0_Resumed (void) {
  if (!mw_cv_usbd_core.testing_active) { return; }
  mw_cv_usbd_core.cb_resumed = true;
}

// \brief Callback function called when Device was successfully enumerated
// \param[in]     val                  current configuration value
//                                       - value 0: not configured
//                                       - value > 0: active configuration
void USBD_Device0_ConfigurationChanged (uint8_t val) {
  (void)val;

  if (!mw_cv_usbd_core.testing_active) { return; }
  mw_cv_usbd_core.cb_conf_changed = true;
}

// \brief Callback function called when Set Feature for Remote Wakeup comes over Control Endpoint 0
void USBD_Device0_EnableRemoteWakeup (void) {
  if (!mw_cv_usbd_core.testing_active) { return; }
  mw_cv_usbd_core.cb_enable_remote_wakeup = true;
}

// \brief Callback function called when Clear Feature for Remote Wakeup comes over Control Endpoint 0
void USBD_Device0_DisableRemoteWakeup (void) {
  if (!mw_cv_usbd_core.testing_active) { return; }
  mw_cv_usbd_core.cb_disable_remote_wakeup = true;
}

// \brief Callback function called when Device 0 received SETUP PACKET on Control Endpoint 0
// \param[in]     setup_packet             pointer to received setup packet.
// \param[out]    buf                      pointer to data buffer used for data stage requested by setup packet.
// \param[out]    len                      pointer to number of data bytes in data stage requested by setup packet.
// \return        usbdRequestStatus        enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed: request was not processed; processing will be done by USB library
// \return        usbdRequestOK:           request was processed successfully (send Zero-Length Packet if no data stage)
// \return        usbdRequestStall:        request was processed but is not supported (stall Endpoint 0)
usbdRequestStatus USBD_Device0_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) {

  if (!mw_cv_usbd_core.testing_active) { return usbdRequestNotProcessed; }

  switch (setup_packet->bmRequestType.Type) {
    case USB_REQUEST_STANDARD:
        // Catch Set String Descriptor request for serial number string
        if ((setup_packet->bmRequestType.Dir       == USB_REQUEST_HOST_TO_DEVICE) &&    // Request to set
            (setup_packet->bmRequestType.Recipient == USB_REQUEST_TO_DEVICE     ) &&    // to device
            (setup_packet->bRequest                == USB_REQUEST_SET_DESCRIPTOR) &&    // the descriptor
           ((setup_packet->wValue >> 8)            == USB_STRING_DESCRIPTOR_TYPE) &&    // String Descriptor Type
           ((setup_packet->wValue & 0xFFU)         == 0x03U                     ) &&    // Index of String = 3
            (setup_packet->wIndex                  == 0x0409U                   )) {    // Language ID = 0x0409 = English (United States)
          handle_set_request = true;                    // This set request is being handled
          *buf = ser_no_string_desc;                    // Return pointer to where String Descriptor should be read
          *len = setup_packet->wLength;
          mw_cv_usbd_core.cb_ep0_setup_set_received = true;
          return usbdRequestOK;                         // Return status that custom handling for this request is used
        }
        // Catch Get String Descriptor request for serial number string and return desired string
        if ((setup_packet->bmRequestType.Dir       == USB_REQUEST_DEVICE_TO_HOST) &&    // Request to get
            (setup_packet->bmRequestType.Recipient == USB_REQUEST_TO_DEVICE     ) &&    // from device
            (setup_packet->bRequest                == USB_REQUEST_GET_DESCRIPTOR) &&    // the descriptor
           ((setup_packet->wValue >> 8)            == USB_STRING_DESCRIPTOR_TYPE) &&    // String Descriptor Type
           ((setup_packet->wValue & 0xFFU)         == 0x03U                     ) &&    // Index of String = 3
            (setup_packet->wIndex                  == 0x0409U                   )) {    // Language ID = 0x0409 = English (United States)
          *buf = ser_no_string_desc;                    // Return pointer to prepared String Descriptor
          if (setup_packet->wLength >= 26U) {
            *len = 26U;                                 // Number of bytes of whole String Descriptor
          } else {
            *len = setup_packet->wLength;               // Requested number of bytes of String Descriptor
          }
          handle_get_request = true;                    // This get request is being handled
          mw_cv_usbd_core.cb_ep0_setup_get_received = true;
          return usbdRequestOK;                         // Return status that custom handling for this request is used
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
void USBD_Device0_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet) {

  if (!mw_cv_usbd_core.testing_active) { return; }
  mw_cv_usbd_core.cb_ep0_setup_pocessed = true;

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

// \brief Callback function called when Device 0 received OUT DATA on Control Endpoint 0
// \param[in]     len                      number of received data bytes.
// \return        usbdRequestStatus        enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed: request was not processed; processing will be done by USB library
// \return        usbdRequestOK:           request was processed successfully (send Zero-Length Packet)
// \return        usbdRequestStall:        request was processed but is not supported (stall Endpoint 0)
// \return        usbdRequestNAK:          request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_Device0_Endpoint0_OutDataReceived (uint32_t len) {
  (void)len;

  if (!mw_cv_usbd_core.testing_active) { return usbdRequestNotProcessed; }

  if (handle_set_request) {
    handle_set_request = false;
    mw_cv_usbd_core.cb_ep0_out_data_received = true;
    return usbdRequestOK;               // Acknowledge custom handled request
  }

  return usbdRequestNotProcessed;
}

// \brief Callback function called when Device 0 sent IN DATA on Control Endpoint 0
// \param[in]     len                      number of sent data bytes.
// \return        usbdRequestStatus        enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed: request was not processed; processing will be done by USB library
// \return        usbdRequestOK:           request was processed successfully (return ACK)
// \return        usbdRequestStall:        request was processed but is not supported (stall Endpoint 0)
// \return        usbdRequestNAK:          request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_Device0_Endpoint0_InDataSent (uint32_t len) {
  (void)len;

  if (!mw_cv_usbd_core.testing_active) { return usbdRequestNotProcessed; }

  if (handle_get_request) {
    handle_get_request = false;
    mw_cv_usbd_core.cb_ep0_in_data_sent = true;
    return usbdRequestOK;               // Acknowledge custom handled request
  }

  return usbdRequestNotProcessed;
}

#endif

//! [code_USBD_User_Device]
