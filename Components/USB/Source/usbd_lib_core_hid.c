/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_core_hid.c
 * Purpose: USB Device - Human Interface Device (HID) core module
 *----------------------------------------------------------------------------*/

#include "usbd_lib_core_hid.h"

#include "usbd_lib_config_def.h"
#include "usbd_lib_core.h"
#include "usbd_lib_hid.h"


/// \brief USB Device Endpoint 0 Get Descriptor HID Descriptor Request
/// \param[in]     device        device index.
/// \param[out]    pD            pointer to descriptor
/// \param[out]    len           length of descriptor
/// \return        true          success
/// \return        false         fail
__WEAK bool USBD_Endpoint0_ReqGetDescriptor_HID (uint8_t device, const uint8_t **pD, uint32_t *len) {
  const usbd_dev_t  *ptr_dev_cfg;
  const usbd_data_t *ptr_dev_data;
  const usbd_hid_t  *ptr_hid_cfg;
        uint8_t      index, cnt;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return false; }

  switch (ptr_dev_data->setup_packet.wValue >> 8) {
    case HID_HID_DESCRIPTOR_TYPE:
      cnt = 0U;
      for (index = 0U; index < usbd_hid_num; index++) {
        ptr_hid_cfg = usbd_hid_ptr[index]; if (ptr_hid_cfg == NULL) { continue; }
        if (device == ptr_hid_cfg->dev_num) {
          if (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_hid_cfg->if_num)) {   // Interface number correct?
            break;
          }
          cnt++;
        }
      }
      if (cnt == usbd_hid_num) {
        return false;                                                   // If HID Interface does not exist
      }
      if ((ptr_dev_cfg->hs == 0U) && (ptr_dev_data->high_speed != 0U)) {
        return false;                                                   // High speed request but high-speed not enabled
      }
      if (ptr_dev_data->high_speed == 0U) {
        *pD = (uint8_t *)((uint32_t)usbd_desc_ptr[device]->config_descriptor_fs);
      } else {
        *pD = (uint8_t *)((uint32_t)usbd_desc_ptr[device]->config_descriptor_hs);
      }
      *pD += usbd_hid_desc_ptr[index]->hid_descriptor_offset;
      *len = sizeof(HID_DESCRIPTOR);
      break;
    case HID_REPORT_DESCRIPTOR_TYPE:
      cnt = 0U;
      for (index = 0U; index < usbd_hid_num; index++) {
        ptr_hid_cfg = usbd_hid_ptr[index]; if (ptr_hid_cfg == NULL) { continue; }
        if (device == ptr_hid_cfg->dev_num) {
          if (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_hid_cfg->if_num)) {   // Interface number correct?
            break;
          }
          cnt++;
        }
      }
      if (cnt == usbd_hid_num) {
        return false;                   // If HID Interface does not exist
      }
      *pD  = usbd_hid_desc_ptr[index]->report_descriptor;
      *len = usbd_hid_desc_ptr[index]->report_descriptor_size;
      break;
    case HID_PHYSICAL_DESCRIPTOR_TYPE:
      return false;                     // HID Physical Descriptor is not supported
    default:
      return false;
  }

  return true;
}

/// \brief USB Device Endpoint 0 Event Callback - HID specific handling (Setup Request To Interface)
/// \param[in]     device        device index.
/// \return        true          setup interface request ok
/// \return        false         setup interface request not supported
__WEAK bool USBD_Endpoint0_Setup_HID_ReqToIF (uint8_t device) {
  const usbd_dev_t  *ptr_dev_cfg;
        usbd_data_t *ptr_dev_data;
  const usbd_hid_t  *ptr_hid_cfg;
        uint8_t      index;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return false; }

  for (index = 0U; index < usbd_hid_num; index++) {
    ptr_hid_cfg = usbd_hid_ptr[index]; if (ptr_hid_cfg == NULL) { continue; }
    if ((device == ptr_hid_cfg->dev_num) &&
        (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_hid_cfg->if_num))) {       // Interface number correct?
      switch (ptr_dev_data->setup_packet.bRequest) {
        case HID_REQUEST_GET_REPORT:
          if (USBD_HID_CoreGetReport(index)) {
            if ((ptr_dev_data->setup_packet.wValue >> 8) == HID_REPORT_INPUT) {
              if (ptr_dev_data->ep0_data.cnt > ptr_hid_cfg->in_report_max_sz) {
                return false;
              }
              ptr_dev_data->ep0_data.data = &ptr_hid_cfg->in_report[1];         // point to data to be sent (skip ReportID)
            } else {
              if ((ptr_dev_data->setup_packet.wValue >> 8) == HID_REPORT_FEATURE) {
                if (ptr_dev_data->ep0_data.cnt > ptr_hid_cfg->feat_report_max_sz) {
                  return false;
                }
                ptr_dev_data->ep0_data.data = &ptr_hid_cfg->feat_report[1];     // point to data to be sent (skip ReportID)
              }
            }
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case HID_REQUEST_SET_REPORT:
          if ((ptr_dev_data->setup_packet.wValue >> 8) == HID_REPORT_OUTPUT) {
            if (ptr_dev_data->ep0_data.cnt > ptr_hid_cfg->out_report_max_sz) {
              return false;
            }
            ptr_dev_data->ep0_data.data = &ptr_hid_cfg->out_report[1];          // out data to be received (skip ReportID)
          } else {
            if ((ptr_dev_data->setup_packet.wValue >> 8) == HID_REPORT_FEATURE) {
              if (ptr_dev_data->ep0_data.cnt > ptr_hid_cfg->feat_report_max_sz) {
                return false;
              }
              ptr_dev_data->ep0_data.data = &ptr_hid_cfg->feat_report[1];       // out data to be received (skip ReportID)
            }
          }
          return true;
        case HID_REQUEST_GET_IDLE:
          if (USBD_HID_CoreGetIdle(index)) {
            ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;         // point to data to be sent
            if (ptr_dev_data->ep0_data.cnt > 1U) {
              ptr_dev_data->ep0_data.cnt = 1U;
            }
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case HID_REQUEST_SET_IDLE:
          if (ptr_dev_data->setup_packet.wLength != 0U) {
            return false;
          }
          if (USBD_HID_CoreSetIdle(index)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        case HID_REQUEST_GET_PROTOCOL:
          if (USBD_HID_CoreGetProtocol(index)) {
            ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;         // point to data to be sent
            if (ptr_dev_data->ep0_data.cnt > 1U) {
              ptr_dev_data->ep0_data.cnt = 1U;
            }
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case HID_REQUEST_SET_PROTOCOL:
          if (ptr_dev_data->setup_packet.wLength != 0U) {
            return false;
          }
          if (USBD_HID_CoreSetProtocol(index)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        default:                        // Unsupported request
          break;
      }
    }
  }

  return false;
}

/// \brief USB Device Endpoint 0 Event Callback - HID specific handling (Out Request To Interface)
/// \param[in]     device        device index.
/// \return        true          out interface request ok
/// \return        false         out interface request not supported
__WEAK bool USBD_Endpoint0_Out_HID_ReqToIF (uint8_t device) {
  const usbd_data_t *ptr_dev_data;
  const usbd_hid_t  *ptr_hid_cfg;
        uint8_t      index;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_data = usbd_dev_ptr[device]->data_ptr; if (ptr_dev_data == NULL) { return false; }

  for (index = 0U; index < usbd_hid_num; index++) {
    ptr_hid_cfg = usbd_hid_ptr[index]; if (ptr_hid_cfg == NULL) { continue; }
    if ((device == ptr_hid_cfg->dev_num) &&                             // Device number correct?
        (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_hid_cfg->if_num))) {       // Interface number correct?
      if ((ptr_dev_data->setup_packet.bRequest) == HID_REQUEST_SET_REPORT) {
        if (USBD_HID_CoreSetReport(index)) {
          if (USBD_StatusInStage(device) == usbOK) {                    // send ZLP IN
            return true;
          }
        }
      }
    }
  }

  return false;
}
