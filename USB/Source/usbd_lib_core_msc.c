/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_core_msc.c
 * Purpose: USB Device - Mass Storage device Class (MSC) core module
 *----------------------------------------------------------------------------*/

#include "usbd_lib_core_msc.h"

#include "usb_msc.h"

#include "usbd_lib_config_def.h"
#include "usbd_lib_core.h"
#include "usbd_lib_msc.h"


/// \brief USB Device Endpoint 0 Event Callback - MSC specific handling (Clear Halt condition on bulk in endpoint)
/// \param[in]     device        device index.
/// \param[in]     ep_addr       endpoint address.
__WEAK void USBD_Endpoint0_ClearFeatureStall_MSC (uint8_t device, uint8_t ep_addr) {
  const usbd_msc_t *ptr_msc_cfg;
        uint8_t     index;

  for (index = 0U; index < usbd_msc_num; index++) {
    ptr_msc_cfg = usbd_msc_ptr[index]; if (ptr_msc_cfg == NULL) { continue; }
    if (device == ptr_msc_cfg->dev_num) {                               // Device number correct?
      if (ep_addr == (ptr_msc_cfg->ep_bulk_in | 0x80U)) {               // Endpoint number correct? (bulk in EP)
        USBD_MSC_EndpointClearFeatureStall (index, ep_addr);
      }
    }
  }
}

/// \brief USB Device Endpoint 0 Event Callback - MSC specific handling (Setup Request To Interface)
/// \param[in]     device        device index.
/// \return        true          setup interface request ok
/// \return        false         setup interface request not supported
__WEAK bool USBD_Endpoint0_Setup_MSC_ReqToIF (uint8_t device) {
  const usbd_dev_t  *ptr_dev_cfg;
        usbd_data_t *ptr_dev_data;
  const usbd_msc_t  *ptr_msc_cfg;
        uint8_t      index;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return false; }

  for (index = 0U; index < usbd_msc_num; index++) {
    ptr_msc_cfg = usbd_msc_ptr[index]; if (ptr_msc_cfg == NULL) { continue; }
    if ((device == ptr_msc_cfg->dev_num) &&                             // Device number correct?
        (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_msc_cfg->if_num))){// Interface number correct?
      switch (ptr_dev_data->setup_packet.bRequest) {
        case MSC_REQUEST_RESET:
          if ((ptr_dev_data->setup_packet.wValue  == 0U) &&             // RESET with invalid parameters -> STALL
              (ptr_dev_data->setup_packet.wLength == 0U)) {
            if (USBD_MSC_BulkOnlyMassStorageReset(index)) {
              if (USBD_StatusInStage(device) == usbOK) {                // send ZLP IN
                return true;
              }
            }
          }
          break;
        case MSC_REQUEST_GET_MAX_LUN:
          if ((ptr_dev_data->setup_packet.wValue  == 0U) &&             // GET_MAX_LUN with invalid parameters -> STALL
              (ptr_dev_data->setup_packet.wLength == 1U)) {
            if (USBD_MSC_GetMaxLUN(index)) {
              ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;
              if (USBD_DataInStage(device) == usbOK) {                  // send requested data
                if (USBD_StatusOutStage(device) == usbOK) {             // receive ZLP OUT
                  return true;
                }
              }
            }
          }
          break;
        default:                                                        // Unsupported command
          break;
      }
    }
  }

  return false;
}
