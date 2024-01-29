/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_core_cdc.c
 * Purpose: USB Device - Communication Device Class (CDC) core module
 *----------------------------------------------------------------------------*/

#include "usbd_lib_core_cdc.h"

#include "usb_cdc.h"

#include "usbd_lib_config_def.h"
#include "usbd_lib_core.h"
#include "usbd_lib_cdc.h"


/// \brief USB Device Endpoint 0 Event Callback - CDC specific handling (Setup Request To Interface)
/// \param[in]     device        device index.
/// \return        true          setup interface request ok
/// \return        false         setup interface request not supported
__WEAK bool USBD_Endpoint0_Setup_CDC_ReqToIF (uint8_t device) {
  const usbd_dev_t  *ptr_dev_cfg;
        usbd_data_t *ptr_dev_data;
  const usbd_cdc_t  *ptr_cdc_cfg;
        uint8_t      index;
        uint8_t     *buf;
        uint16_t     len;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return false; }

  for (index = 0U; index < usbd_cdc_num; index++) {
    ptr_cdc_cfg = usbd_cdc_ptr[index]; if (ptr_cdc_cfg == NULL) { continue; }
    if ((device == ptr_cdc_cfg->dev_num) &&                             // Device number correct?
       ((ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cdc_cfg->cif_num))  ||     // Interface number correct?
        (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cdc_cfg->dif_num)))) {
      switch (ptr_dev_data->setup_packet.bRequest) {
        case CDC_SEND_ENCAPSULATED_COMMAND:
          ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;           // data to be received, see USBD_EVT_OUT
          return true;
        case CDC_GET_ENCAPSULATED_RESPONSE:
          buf = NULL;
          len = 0U;
          if (USBD_CDC_ACM_CoreGetEncapsulatedResponse(index, ptr_dev_data->setup_packet.wLength, &buf, &len)) {
            if ((buf == NULL) || (len == 0U)) {
              return false;
            }
            ptr_dev_data->ep0_data.data = buf;                          // point to data to be sent
            ptr_dev_data->ep0_data.cnt  = len;
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case CDC_SET_COMM_FEATURE:
          if (ptr_dev_data->ep0_data.cnt < 2U) {
            return false;
          }
          ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;           // data to be received, see USBD_EVT_OUT
          if (ptr_dev_data->ep0_data.cnt > 2U) {
            ptr_dev_data->ep0_data.cnt = 2U;
          }
          return true;
        case CDC_GET_COMM_FEATURE:
          if (USBD_CDC_ACM_CoreGetCommFeature(index, ptr_dev_data->setup_packet.wValue)) {
            ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;         // point to data to be sent
            if (ptr_dev_data->ep0_data.cnt > 2U) {
              ptr_dev_data->ep0_data.cnt = 2U;
            }
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case CDC_CLEAR_COMM_FEATURE:
          if (ptr_dev_data->setup_packet.wLength != 0U) {
            return false;
          }
          if (USBD_CDC_ACM_CoreClearCommFeature(index, ptr_dev_data->setup_packet.wValue)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        case CDC_SET_LINE_CODING:
          if (ptr_dev_data->ep0_data.cnt < 7U) {
            return false;
          }
          ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;           // data to be received, see USBD_EVT_OUT
          if (ptr_dev_data->ep0_data.cnt > 7U) {
            ptr_dev_data->ep0_data.cnt = 7U;
          }
          return true;
        case CDC_GET_LINE_CODING:
          if (USBD_CDC_ACM_CoreGetLineCoding(index)) {
            ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;         // point to data to be sent
            if (ptr_dev_data->ep0_data.cnt > 7U) {
              ptr_dev_data->ep0_data.cnt = 7U;
            }
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case CDC_SET_CONTROL_LINE_STATE:
          if (ptr_dev_data->setup_packet.wLength != 0U) {
            return false;
          }
          if (USBD_CDC_ACM_CoreSetControlLineState(index, ptr_dev_data->setup_packet.wValue)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        case CDC_SEND_BREAK:
          if (ptr_dev_data->setup_packet.wLength != 0U) {
            return false;
          }
          if (USBD_CDC_ACM_CoreSendBreak(index, ptr_dev_data->setup_packet.wValue)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        case CDC_SET_ETHERNET_MULTICAST_FILTERS:
          if (ptr_dev_data->ep0_data.cnt < (ptr_dev_data->setup_packet.wValue * 6UL)) {
            return false;
          }
          ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;           // data to be received, see USBD_EVT_OUT
          if (ptr_dev_data->ep0_data.cnt > (ptr_dev_data->setup_packet.wValue * 6UL)) {
            ptr_dev_data->ep0_data.cnt = (ptr_dev_data->setup_packet.wValue * 6UL);
          }
          return true;
        case CDC_SET_ETHERNET_PMP_FILTER:
          ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;           // data to be received, see USBD_EVT_OUT
          return true;
        case CDC_SET_NET_ADDRESS:
          if (ptr_dev_data->ep0_data.cnt < 6U) {
            return false;
          }
          ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;           // data to be received, see USBD_EVT_OUT
          if (ptr_dev_data->ep0_data.cnt > 6U) {
            ptr_dev_data->ep0_data.cnt = 6U;
          }
          return true;
        case CDC_SET_NTB_INPUT_SIZE:
          if (ptr_dev_data->ep0_data.cnt < 4U) {
            return false;
          }
          ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;           // data to be received, see USBD_EVT_OUT
          if (ptr_dev_data->ep0_data.cnt > 4U) {
            ptr_dev_data->ep0_data.cnt = 4U;
          }
          return true;
        case CDC_SET_MAX_DATAGRAM_SIZE:
          if (ptr_dev_data->ep0_data.cnt < 2U) {
            return false;
          }
          ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;           // data to be received, see USBD_EVT_OUT
          if (ptr_dev_data->ep0_data.cnt > 2U) {
            ptr_dev_data->ep0_data.cnt = 2U;
          }
          return true;
        case CDC_GET_ETHERNET_PMP_FILTER:
          if (USBD_CDC_NCM_CoreGetEthernetPowerManagementPatternFilter (index, ptr_dev_data->setup_packet.wValue, (uint16_t *)((uint32_t)ptr_dev_cfg->ep0_buf))) {
            ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;         // point to data to be sent
            if (ptr_dev_data->ep0_data.cnt > 2U) {
              ptr_dev_data->ep0_data.cnt = 2U;
            }
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case CDC_SET_ETHERNET_PACKET_FILTER:
          if (ptr_dev_data->setup_packet.wLength != 0U) {
            return false;
          }
          if (USBD_CDC_NCM_CoreSetEthernetPacketFilter (index, ptr_dev_data->setup_packet.wValue)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        case CDC_GET_ETHERNET_STATISTIC:
          if (USBD_CDC_NCM_CoreGetEthernetStatistic (index, ptr_dev_data->setup_packet.wValue, (uint32_t *)((uint32_t)ptr_dev_cfg->ep0_buf))) {
            ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;         // point to data to be sent
            if (ptr_dev_data->ep0_data.cnt > 4U) {
              ptr_dev_data->ep0_data.cnt = 4U;
            }
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case CDC_GET_NTB_PARAMETERS:
          if (USBD_CDC_NCM_CoreGetNtbParameters (index, &ptr_cdc_cfg->data_ptr->ncm_ntb_param)) {
            ptr_dev_data->ep0_data.data = (uint8_t *)(&ptr_cdc_cfg->data_ptr->ncm_ntb_param);   // point to data to be sent
            if (ptr_dev_data->ep0_data.cnt > 28U) {
              ptr_dev_data->ep0_data.cnt = 28U;
            }
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case CDC_GET_NET_ADDRESS:
          if (USBD_CDC_NCM_CoreGetNetAddress (index, ptr_dev_cfg->ep0_buf)) {
            ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;         // point to data to be sent
            if (ptr_dev_data->ep0_data.cnt > 6U) {
              ptr_dev_data->ep0_data.cnt = 6U;
            }
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case CDC_GET_NTB_FORMAT:
          if (USBD_CDC_NCM_CoreGetNtbFormat (index, (uint16_t *)((uint32_t)ptr_dev_cfg->ep0_buf))) {
            ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;         // point to data to be sent
            if (ptr_dev_data->ep0_data.cnt > 2U) {
              ptr_dev_data->ep0_data.cnt = 2U;
            }
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case CDC_SET_NTB_FORMAT:
          if (ptr_dev_data->setup_packet.wLength != 0U) {
            return false;
          }
          if (USBD_CDC_NCM_CoreSetNtbFormat (index, ptr_cdc_cfg->data_ptr->ncm_ntb_format)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        case CDC_GET_NTB_INPUT_SIZE:
          if (USBD_CDC_NCM_CoreGetNtbInputSize (index, (uint32_t *)((uint32_t)ptr_dev_cfg->ep0_buf))) {
            ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;         // point to data to be sent
            if (ptr_dev_data->ep0_data.cnt > 4U) {
              ptr_dev_data->ep0_data.cnt = 4U;
            }
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case CDC_GET_MAX_DATAGRAM_SIZE:
          if (USBD_CDC_NCM_CoreGetMaxDatagramSize (index, (uint16_t *)((uint32_t)ptr_dev_cfg->ep0_buf))) {
            ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;         // point to data to be sent
            if (ptr_dev_data->ep0_data.cnt > 2U) {
              ptr_dev_data->ep0_data.cnt = 2U;
            }
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case CDC_GET_CRC_MODE:
          if (USBD_CDC_NCM_CoreGetCrcMode (index, (uint16_t *)((uint32_t)ptr_dev_cfg->ep0_buf))) {
            ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;         // point to data to be sent
            if (ptr_dev_data->ep0_data.cnt > 2U) {
              ptr_dev_data->ep0_data.cnt = 2U;
            }
            if (USBD_DataInStage(device) == usbOK) {                    // send requested data
              if (USBD_StatusOutStage(device) == usbOK) {               // receive ZLP OUT
                return true;
              }
            }
          }
          break;
        case CDC_SET_CRC_MODE:
          if (ptr_dev_data->setup_packet.wLength != 0U) {
            return false;
          }
          if (USBD_CDC_NCM_CoreSetCrcMode (index, ptr_dev_data->setup_packet.wValue)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        default:                                                        // Unsupported request
          break;
      }
    }
  }

  return false;
}

/// \brief USB Device Endpoint 0 Event Callback - CDC specific handling (Out Request To Interface)
/// \param[in]     device        device index.
/// \return        true          out interface request ok
/// \return        false         out interface request not supported
__WEAK bool USBD_Endpoint0_Out_CDC_ReqToIF (uint8_t device) {
  const usbd_dev_t  *ptr_dev_cfg;
  const usbd_data_t *ptr_dev_data;
  const usbd_cdc_t  *ptr_cdc_cfg;
        uint8_t      index;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return false; }

  for (index = 0U; index < usbd_cdc_num; index++) {
    ptr_cdc_cfg = usbd_cdc_ptr[index]; if (ptr_cdc_cfg == NULL) { continue; }
    if ((device == ptr_cdc_cfg->dev_num) &&                             // Port number correct?
       ((ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cdc_cfg->cif_num))  ||     // Interface number correct?
        (ptr_dev_data->setup_packet.wIndex == (uint16_t)(ptr_cdc_cfg->dif_num)))) {
      switch (ptr_dev_data->setup_packet.bRequest) {
        case CDC_SEND_ENCAPSULATED_COMMAND:
          if (USBD_CDC_ACM_CoreSendEncapsulatedCommand(index, ptr_dev_data->setup_packet.wLength)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        case CDC_SET_COMM_FEATURE:
          if (USBD_CDC_ACM_CoreSetCommFeature(index, ptr_dev_data->setup_packet.wValue)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        case CDC_SET_LINE_CODING:
          if (USBD_CDC_ACM_CoreSetLineCoding(index)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        case CDC_SET_ETHERNET_MULTICAST_FILTERS:
          if (USBD_CDC_NCM_CoreSetEthernetMulticastFilters (index, ptr_dev_cfg->ep0_buf, ptr_dev_data->setup_packet.wValue)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        case CDC_SET_ETHERNET_PMP_FILTER:
          if (USBD_CDC_NCM_CoreSetEthernetPowerManagementPatternFilter (index, ptr_dev_data->setup_packet.wValue, ptr_dev_cfg->ep0_buf, ptr_dev_data->setup_packet.wLength)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        case CDC_SET_NET_ADDRESS:
          if (USBD_CDC_NCM_CoreSetNetAddress (index, ptr_dev_cfg->ep0_buf)) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        case CDC_SET_NTB_INPUT_SIZE:
          if (USBD_CDC_NCM_CoreSetNtbInputSize (index, *((uint32_t *)((uint32_t)ptr_dev_cfg->ep0_buf)))) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        case CDC_SET_MAX_DATAGRAM_SIZE:
          if (USBD_CDC_NCM_CoreSetMaxDatagramSize (index, *((uint16_t *)((uint32_t)ptr_dev_cfg->ep0_buf)))) {
            if (USBD_StatusInStage(device) == usbOK) {                  // send ZLP IN
              return true;
            }
          }
          break;
        default:                                                        // Unsupported request
          break;
      }
    }
  }

  return false;
}
