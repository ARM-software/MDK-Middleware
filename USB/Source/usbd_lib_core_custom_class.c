/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_core_custom_class.c
 * Purpose: USB Device - Custom Class core module
 *----------------------------------------------------------------------------*/

#include "usbd_lib_core_custom_class.h"

#include "usb_lib_rte.h"
#include "usbd_lib_config_def.h"
#include "usbd_evr.h"


/// \brief USB Device Endpoint 0 Get Extended Properties OS Descriptor
/// \param[in]     device               device index.
/// \param[out]    pD                   pointer to descriptor
/// \return        true                 success
/// \return        false                fail
__WEAK bool USBD_Endpoint0_ReqGetExtProp_CC (uint8_t device, const uint8_t **pD) {
  const usbd_data_t         *ptr_dev_data;
  const usbd_custom_class_t *ptr_cc_cfg;
        uint8_t              index, i;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  ptr_dev_data = usbd_dev_ptr[device]->data_ptr; if (ptr_dev_data == NULL) { return false; }

  for (index = 0U; index < usbd_custom_class_num; index++) {
    ptr_cc_cfg = usbd_custom_class_ptr[index];
    if (device == ptr_cc_cfg->dev_num) {                                        // If request is on device containing custom class
      for (i = 0U; i < 4U; i++) {
        if ((ptr_cc_cfg->cc[i].if_en != 0U) && ((ptr_dev_data->setup_packet.wValue & 0x00FFU) == (uint16_t)(ptr_cc_cfg->cc[i].if_num))) {
          *pD = (uint8_t *)((uint32_t)usbd_cc_desc_ptr[index]->ms_ext_prop_descriptor[i]);
          return true;
        }
      }
    }
  }

  return false;
}


/// \brief USB Device Endpoint 0 Custom Class Setup Packet Received callback
/// \param[in]     device               device index.
/// \return        usbdRequestStatus    enumerator value indicating the function execution status :
///                                       - value usbdRequestNotProcessed : request was not processed; processing will be done by USB library
///                                       - value usbdRequestOK :           request was processed successfully (send Zero-Length Packet if no data stage)
///                                       - value usbdRequestStall :        request was processed but is not supported (STALL EP)
__WEAK usbdRequestStatus USBD_Class_Endpoint0_SetupPacketReceived (uint8_t device) {
        usbd_data_t         *ptr_dev_data;
  const usbd_custom_class_t *ptr_cc_cfg;
        usbdRequestStatus    status;
        uint8_t              index, i;

  status = usbdRequestNotProcessed;

  if (device >= usbd_dev_num)                                              { return status; }
  if (fpUSBD_CustomClass_Endpoint0_SetupPacketReceived[device] == NULL)    { return status; }

  ptr_dev_data = usbd_dev_ptr[device]->data_ptr; if (ptr_dev_data == NULL) { return status; }

  // If SETUP PACKET was not handled at device level, allow custom class handling if request was class request
  if (ptr_dev_data->setup_packet.bmRequestType.Type == USB_REQUEST_CLASS) {     // If request is class request
    switch (ptr_dev_data->setup_packet.bmRequestType.Recipient) {
      case USB_REQUEST_TO_DEVICE:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            EvrUSBD_CC_OnEndpoint0SetupPacketReceivedDetail(device, (const void *)&ptr_dev_data->setup_packet, ptr_dev_data->len);
            status = fpUSBD_CustomClass_Endpoint0_SetupPacketReceived[index] ((const USB_SETUP_PACKET *) &ptr_dev_data->setup_packet, &ptr_dev_data->buf, &ptr_dev_data->len);
            EvrUSBD_CC_OnEndpoint0SetupPacketReceived(device, status);
            if (status != usbdRequestNotProcessed) {
              break;
            }
          }
        }
        break;

      case USB_REQUEST_TO_INTERFACE:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            for (i = 0U; i < 4U; i++) {
              if ((ptr_cc_cfg->cc[i].if_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_num))) {
                EvrUSBD_CC_OnEndpoint0SetupPacketReceivedDetail(device, (const void *)&ptr_dev_data->setup_packet, ptr_dev_data->len);
                status = fpUSBD_CustomClass_Endpoint0_SetupPacketReceived[index] ((const USB_SETUP_PACKET *) &ptr_dev_data->setup_packet, &ptr_dev_data->buf, &ptr_dev_data->len);
                EvrUSBD_CC_OnEndpoint0SetupPacketReceived(device, status);
                if (status != usbdRequestNotProcessed) {
                  break;
                }
              }
            }
          }
        }
        break;

      case USB_REQUEST_TO_ENDPOINT:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            for (i = 0U; i < 4U; i++) {
              if (ptr_cc_cfg->cc[i].if_en != 0U) {
                if (((ptr_cc_cfg->cc[i].if_ep0_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep0_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep1_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep1_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep2_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep2_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep3_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep3_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep4_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep4_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep5_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep5_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep6_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep6_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep7_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep7_addr)))) {
                  EvrUSBD_CC_OnEndpoint0SetupPacketReceivedDetail(device, (const void *)&ptr_dev_data->setup_packet, ptr_dev_data->len);
                  status = fpUSBD_CustomClass_Endpoint0_SetupPacketReceived[index] ((const USB_SETUP_PACKET *) &ptr_dev_data->setup_packet, &ptr_dev_data->buf, &ptr_dev_data->len);
                  EvrUSBD_CC_OnEndpoint0SetupPacketReceived(device, status);
                  if (status != usbdRequestNotProcessed) {
                    break;
                  }
                }
              }
            }
          }
        }
        break;

      case USB_REQUEST_TO_OTHER:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            EvrUSBD_CC_OnEndpoint0SetupPacketReceivedDetail(device, (const void *)&ptr_dev_data->setup_packet, ptr_dev_data->len);
            status = fpUSBD_CustomClass_Endpoint0_SetupPacketReceived[device] ((const USB_SETUP_PACKET *) &ptr_dev_data->setup_packet, &ptr_dev_data->buf, &ptr_dev_data->len);
            EvrUSBD_CC_OnEndpoint0SetupPacketReceived(device, status);
            if (status != usbdRequestNotProcessed) {
              break;
            }
          }
        }
        break;

      default:                          // Unexpected value -> ignore it
        break;
    }
  }

  return status;
}

/// \brief USB Device Endpoint 0 Custom Class Setup Packet processed by USB Library callback
/// \param[in]     device               device index.
__WEAK void USBD_Class_Endpoint0_SetupPacketProcessed (uint8_t device) {
  const usbd_data_t         *ptr_dev_data;
  const usbd_custom_class_t *ptr_cc_cfg;
        uint8_t              index, i;

  if (device >= usbd_dev_num)                                              { return; }
  if (fpUSBD_CustomClass_Endpoint0_SetupPacketProcessed[device] == NULL)   { return; }

  ptr_dev_data = usbd_dev_ptr[device]->data_ptr; if (ptr_dev_data == NULL) { return; }

  // Pass SETUP PACKET processed information to custom class level handling if request was class request
  if (ptr_dev_data->setup_packet.bmRequestType.Type == USB_REQUEST_CLASS) {     // If request is class request
    switch (ptr_dev_data->setup_packet.bmRequestType.Recipient) {
      case USB_REQUEST_TO_DEVICE:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            EvrUSBD_CC_OnEndpoint0SetupPacketProcessedDetail(device, (const void *)&ptr_dev_data->setup_packet);
            fpUSBD_CustomClass_Endpoint0_SetupPacketProcessed[index] ((const USB_SETUP_PACKET *) &ptr_dev_data->setup_packet);
            EvrUSBD_CC_OnEndpoint0SetupPacketProcessed(device);
          }
        }
        break;

      case USB_REQUEST_TO_INTERFACE:
        ptr_dev_data = (usbd_dev_ptr[device])->data_ptr;
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            for (i = 0U; i < 4U; i++) {
              if ((ptr_cc_cfg->cc[i].if_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_num))) {
                EvrUSBD_CC_OnEndpoint0SetupPacketProcessedDetail(device, (const void *)&ptr_dev_data->setup_packet);
                fpUSBD_CustomClass_Endpoint0_SetupPacketProcessed[index] ((const USB_SETUP_PACKET *) &ptr_dev_data->setup_packet);
                EvrUSBD_CC_OnEndpoint0SetupPacketProcessed(device);
              }
            }
          }
        }
        break;

      case USB_REQUEST_TO_ENDPOINT:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            for (i = 0U; i < 4U; i++) {
              if (ptr_cc_cfg->cc[i].if_en != 0U) {
                if (((ptr_cc_cfg->cc[i].if_ep0_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep0_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep1_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep1_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep2_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep2_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep3_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep3_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep4_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep4_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep5_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep5_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep6_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep6_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep7_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep7_addr)))) {
                  EvrUSBD_CC_OnEndpoint0SetupPacketProcessedDetail(device, (const void *)&ptr_dev_data->setup_packet);
                  fpUSBD_CustomClass_Endpoint0_SetupPacketProcessed[index] ((const USB_SETUP_PACKET *) &ptr_dev_data->setup_packet);
                  EvrUSBD_CC_OnEndpoint0SetupPacketProcessed(device);
                }
              }
            }
          }
        }
        break;

      case USB_REQUEST_TO_OTHER:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            EvrUSBD_CC_OnEndpoint0SetupPacketProcessedDetail(device, (const void *)&ptr_dev_data->setup_packet);
            fpUSBD_CustomClass_Endpoint0_SetupPacketProcessed[index] ((const USB_SETUP_PACKET *) &ptr_dev_data->setup_packet);
            EvrUSBD_CC_OnEndpoint0SetupPacketProcessed(device);
          }
        }
        break;

      default:                          // Unexpected value -> ignore it
        break;
    }
  }
}

/// \brief USB Device Endpoint 0 Custom Class Out Data Received callback
/// \param[in]     device               device index.
/// \return        usbdRequestStatus    enumerator value indicating the function execution status :
///                                       - value usbdRequestNotProcessed : request was not processed; processing will be done by USB library
///                                       - value usbdRequestOK :           request was processed successfully (send Zero-Length Packet)
///                                       - value usbdRequestStall :        request was processed but is not supported (stall endpoint 0)
///                                       - value usbdRequestNAK :          request was processed but the device is busy (return NAK)
__WEAK usbdRequestStatus USBD_Class_Endpoint0_OutDataReceived (uint8_t device) {
  const usbd_data_t         *ptr_dev_data;
  const usbd_custom_class_t *ptr_cc_cfg;
        usbdRequestStatus    status;
        uint8_t              index, i;

  status = usbdRequestNotProcessed;

  if (device >= usbd_dev_num)                                              { return status; }
  if (fpUSBD_CustomClass_Endpoint0_OutDataReceived[device] == NULL)        { return status; }

  ptr_dev_data = usbd_dev_ptr[device]->data_ptr; if (ptr_dev_data == NULL) { return status; }

  // If OUT DATA was not handled at device level, allow custom class handling if request was class request
  if (ptr_dev_data->setup_packet.bmRequestType.Type == USB_REQUEST_CLASS) {     // If request is class request
    switch (ptr_dev_data->setup_packet.bmRequestType.Recipient) {
      case USB_REQUEST_TO_DEVICE:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            status = fpUSBD_CustomClass_Endpoint0_OutDataReceived [index] (ptr_dev_data->len);
            EvrUSBD_CC_OnEndpoint0OutDataReceived(device, ptr_dev_data->len, status);
            if (status != usbdRequestNotProcessed) {
              break;
            }
          }
        }
        break;

      case USB_REQUEST_TO_INTERFACE:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            for (i = 0U; i < 4U; i++) {
              if ((ptr_cc_cfg->cc[i].if_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_num))) {
                status = fpUSBD_CustomClass_Endpoint0_OutDataReceived [index] (ptr_dev_data->len);
                EvrUSBD_CC_OnEndpoint0OutDataReceived(device, ptr_dev_data->len, status);
                if (status != usbdRequestNotProcessed) {
                  break;
                }
              }
            }
          }
        }
        break;

      case USB_REQUEST_TO_ENDPOINT:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            for (i = 0U; i < 4U; i++) {
              if (ptr_cc_cfg->cc[i].if_en != 0U) {
                if (((ptr_cc_cfg->cc[i].if_ep0_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep0_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep1_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep1_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep2_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep2_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep3_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep3_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep4_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep4_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep5_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep5_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep6_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep6_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep7_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep7_addr)))) {
                  status = fpUSBD_CustomClass_Endpoint0_OutDataReceived [index] (ptr_dev_data->len);
                  EvrUSBD_CC_OnEndpoint0OutDataReceived(device, ptr_dev_data->len, status);
                  if (status != usbdRequestNotProcessed) {
                    break;
                  }
                }
              }
            }
          }
        }
        break;

      case USB_REQUEST_TO_OTHER:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            status = fpUSBD_CustomClass_Endpoint0_OutDataReceived [index] (ptr_dev_data->len);
            EvrUSBD_CC_OnEndpoint0OutDataReceived(device, ptr_dev_data->len, status);
            if (status != usbdRequestNotProcessed) {
              break;
            }
          }
        }
        break;

      default:                          // Unexpected value -> ignore it
        break;
    }
  }

  return status;
}

/// \brief USB Device Endpoint 0 Custom Class In Data Received callback
/// \param[in]     device               device index.
/// \return        usbdRequestStatus    enumerator value indicating the function execution status :
///                                       - value usbdRequestNotProcessed : request was not processed; processing will be done by USB library
///                                       - value usbdRequestOK :           request was processed successfully (return ACK)
///                                       - value usbdRequestStall :        request was processed but is not supported (stall endpoint 0)
///                                       - value usbdRequestNAK :          request was processed but the device is busy (return NAK)
__WEAK usbdRequestStatus USBD_Class_Endpoint0_InDataSent (uint8_t device) {
  const usbd_data_t         *ptr_dev_data;
  const usbd_custom_class_t *ptr_cc_cfg;
        usbdRequestStatus    status;
        uint8_t              index, i;

  status = usbdRequestNotProcessed;

  if (device >= usbd_dev_num)                                              { return status; }
  if (fpUSBD_CustomClass_Endpoint0_InDataSent[device] == NULL)             { return status; }

  ptr_dev_data = usbd_dev_ptr[device]->data_ptr; if (ptr_dev_data == NULL) { return status; }

  // If IN DATA was not handled at device level, allow custom class handling if request was class request
  if (ptr_dev_data->setup_packet.bmRequestType.Type == USB_REQUEST_CLASS) {     // If request is class request
    switch (ptr_dev_data->setup_packet.bmRequestType.Recipient) {
      case USB_REQUEST_TO_DEVICE:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            status = fpUSBD_CustomClass_Endpoint0_InDataSent [index] (ptr_dev_data->len);
            EvrUSBD_CC_OnEndpoint0InDataSent(device, ptr_dev_data->len, status);
            if (status != usbdRequestNotProcessed) {
              break;
            }
          }
        }
        break;

      case USB_REQUEST_TO_INTERFACE:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            for (i = 0U; i < 4U; i++) {
              if ((ptr_cc_cfg->cc[i].if_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_num))) {
                status = fpUSBD_CustomClass_Endpoint0_InDataSent [index] (ptr_dev_data->len);
                EvrUSBD_CC_OnEndpoint0InDataSent(device, ptr_dev_data->len, status);
                if (status != usbdRequestNotProcessed) {
                  break;
                }
              }
            }
          }
        }
        break;

      case USB_REQUEST_TO_ENDPOINT:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            for (i = 0U; i < 4U; i++) {
              if (ptr_cc_cfg->cc[i].if_en != 0U) {
                if (((ptr_cc_cfg->cc[i].if_ep0_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep0_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep1_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep1_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep2_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep2_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep3_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep3_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep4_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep4_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep5_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep5_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep6_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep6_addr))) ||
                    ((ptr_cc_cfg->cc[i].if_ep7_en != 0U) && (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cc_cfg->cc[i].if_ep7_addr)))) {
                  status = fpUSBD_CustomClass_Endpoint0_InDataSent [index] (ptr_dev_data->len);
                  EvrUSBD_CC_OnEndpoint0InDataSent(device, ptr_dev_data->len, status);
                  if (status != usbdRequestNotProcessed) {
                    break;
                  }
                }
              }
            }
          }
        }
        break;

      case USB_REQUEST_TO_OTHER:
        for (index = 0U; index < usbd_custom_class_num; index++) {
          ptr_cc_cfg = usbd_custom_class_ptr[index];
          if (device == ptr_cc_cfg->dev_num) {                                  // If request is on device containing custom class
            status = fpUSBD_CustomClass_Endpoint0_InDataSent [index] (ptr_dev_data->len);
            EvrUSBD_CC_OnEndpoint0InDataSent(device, ptr_dev_data->len, status);
            if (status != usbdRequestNotProcessed) {
              break;
            }
          }
        }
        break;

      default:                          // Unexpected value -> ignore it
        break;
    }
  }

  return status;
}
