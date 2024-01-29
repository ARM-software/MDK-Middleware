/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_core_adc.c
 * Purpose: USB Device - Audio Device Class (ADC) core module
 *----------------------------------------------------------------------------*/

#include "usbd_lib_core_adc.h"

#include "usb_adc.h"

#include "usbd_lib_config_def.h"
#include "usbd_lib_core.h"
#include "usbd_lib_adc.h"


/// \brief USB Device Endpoint 0 Event Callback - ADC specific handling (Setup Request To Interface)
/// \param[in]     device        device index.
/// \return        true          setup interface request ok
/// \return        false         setup interface request not supported
__WEAK bool USBD_Endpoint0_Setup_ADC_ReqToIF (uint8_t device) {
  const usbd_dev_t  *ptr_dev_cfg;
        usbd_data_t *ptr_dev_data;
  const usbd_adc_t  *ptr_adc_cfg;
        uint8_t      index;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return false; }

  for (index = 0U; index < usbd_adc_num; index++) {
    ptr_adc_cfg = usbd_adc_ptr[index]; if (ptr_adc_cfg == NULL) { continue; }
    if ((device == ptr_adc_cfg->dev_num) &&                             // Device number correct?
       ((uint8_t)(ptr_dev_data->setup_packet.wIndex) == ptr_adc_cfg->cif_num)) {// Interface number correct? (audio control IF)
      if ((ptr_adc_cfg->out_cfg != NULL) && ((uint8_t)(ptr_dev_data->setup_packet.wIndex >> 8) == ptr_adc_cfg->out_cfg->fu_id)) {
        // Addressed Feature Unit: Interface = 0, ID = Speaker Feature Unit
        switch (ptr_dev_data->setup_packet.wValue >> 8) {               // Control Selector
          case AUDIO_MUTE_CONTROL:                                      // Mute Control
            switch (ptr_dev_data->setup_packet.bRequest) {
              case AUDIO_REQUEST_SET_CUR:
                if (ptr_dev_data->ep0_data.cnt == 0U) {
                  return false;
                }
                ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;     // data to be received, see USBD_EVT_OUT
                if (ptr_dev_data->ep0_data.cnt > 1U) {
                  ptr_dev_data->ep0_data.cnt  = 1U;
                }
                return true;
              case AUDIO_REQUEST_GET_CUR:
                if (USBD_ADC_SpeakerMuteGetCur(index, (uint8_t)(ptr_dev_data->setup_packet.wValue))) {
                  ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;   // point to data to be sent
                  if (ptr_dev_data->ep0_data.cnt > 1U) {
                    ptr_dev_data->ep0_data.cnt  = 1U;
                  }
                  if (USBD_DataInStage(device) == usbOK) {              // send requested data
                    if (USBD_StatusOutStage(device) == usbOK) {         // receive ZLP OUT
                      return true;
                    }
                  }
                }
                break;
              default:                                                  // Unsupported request
                break;
            }
            break;
          case AUDIO_VOLUME_CONTROL:                                    // Volume Control
            switch (ptr_dev_data->setup_packet.bRequest) {
              case AUDIO_REQUEST_SET_CUR:
                if (ptr_dev_data->ep0_data.cnt < 2U) {
                  return false;
                }
                ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;     // data to be received, see USBD_EVT_OUT
                if (ptr_dev_data->ep0_data.cnt > 2U) {
                  ptr_dev_data->ep0_data.cnt  = 2U;
                }
                return true;
              case AUDIO_REQUEST_GET_CUR:
                if (USBD_ADC_SpeakerVolumeGetCur(index, (uint8_t)(ptr_dev_data->setup_packet.wValue))) {
                  ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;   // point to data to be sent
                  if (ptr_dev_data->ep0_data.cnt > 2U) {
                    ptr_dev_data->ep0_data.cnt  = 2U;
                  }
                  if (USBD_DataInStage(device) == usbOK) {              // send requested data
                    if (USBD_StatusOutStage(device) == usbOK) {         // receive ZLP OUT
                      return true;
                    }
                  }
                }
                break;
              case AUDIO_REQUEST_GET_MIN:
                if (USBD_ADC_SpeakerVolumeGetMin(index, (uint8_t)(ptr_dev_data->setup_packet.wValue))) {
                  ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;   // point to data to be sent
                  if (ptr_dev_data->ep0_data.cnt > 2U) {
                    ptr_dev_data->ep0_data.cnt  = 2U;
                  }
                  if (USBD_DataInStage(device) == usbOK) {              // send requested data
                    if (USBD_StatusOutStage(device) == usbOK) {         // receive ZLP OUT
                      return true;
                    }
                  }
                }
                break;
              case AUDIO_REQUEST_GET_MAX:
                if (USBD_ADC_SpeakerVolumeGetMax(index, (uint8_t)(ptr_dev_data->setup_packet.wValue))) {
                  ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;   // point to data to be sent
                  if (ptr_dev_data->ep0_data.cnt > 2U) {
                    ptr_dev_data->ep0_data.cnt  = 2U;
                  }
                  if (USBD_DataInStage(device) == usbOK) {              // send requested data
                    if (USBD_StatusOutStage(device) == usbOK) {         // receive ZLP OUT
                      return true;
                    }
                  }
                }
                break;
              case AUDIO_REQUEST_GET_RES:
                if (USBD_ADC_SpeakerVolumeGetRes(index, (uint8_t)(ptr_dev_data->setup_packet.wValue))) {
                  ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;   // point to data to be sent
                  if (ptr_dev_data->ep0_data.cnt > 2U) {
                    ptr_dev_data->ep0_data.cnt  = 2U;
                  }
                  if (USBD_DataInStage(device) == usbOK) {              // send requested data
                    if (USBD_StatusOutStage(device) == usbOK) {         // receive ZLP OUT
                      return true;
                    }
                  }
                }
                break;
              default:                                                  // Unsupported request
                break;
            }
            break;
          default:                                                      // Unsupported request
            break;
        }
      }
      if ((ptr_adc_cfg->in_cfg != NULL) && ((uint8_t)(ptr_dev_data->setup_packet.wIndex >> 8) == ptr_adc_cfg->in_cfg->fu_id)) {
        // Addressed Feature Unit: Interface = 0, ID = Microphone Feature Unit
        switch (ptr_dev_data->setup_packet.wValue >> 8) {               // Control Selector
          case AUDIO_MUTE_CONTROL:                                      // Mute Control
            switch (ptr_dev_data->setup_packet.bRequest) {
              case AUDIO_REQUEST_SET_CUR:
                if (ptr_dev_data->ep0_data.cnt == 0U) {
                  return false;
                }
                ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;     // data to be received, see USBD_EVT_OUT
                if (ptr_dev_data->ep0_data.cnt > 1U) {
                  ptr_dev_data->ep0_data.cnt  = 1U;
                }
                return true;
              case AUDIO_REQUEST_GET_CUR:
                if (USBD_ADC_MicrophoneMuteGetCur(index, (uint8_t)(ptr_dev_data->setup_packet.wValue))) {
                  ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;   // point to data to be sent
                  if (ptr_dev_data->ep0_data.cnt > 1U) {
                    ptr_dev_data->ep0_data.cnt  = 1U;
                  }
                  if (USBD_DataInStage(device) == usbOK) {              // send requested data
                    if (USBD_StatusOutStage(device) == usbOK) {         // receive ZLP OUT
                      return true;
                    }
                  }
                }
                break;
              default:                                                  // Unsupported request
                break;
            }
            break;
          case AUDIO_VOLUME_CONTROL:                                    // Volume Control
            switch (ptr_dev_data->setup_packet.bRequest) {
              case AUDIO_REQUEST_SET_CUR:
                if (ptr_dev_data->ep0_data.cnt < 2U) {
                  return false;
                }
                ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;     // data to be received, see USBD_EVT_OUT
                if (ptr_dev_data->ep0_data.cnt > 2U) {
                  ptr_dev_data->ep0_data.cnt  = 2U;
                }
                return true;
              case AUDIO_REQUEST_GET_CUR:
                if (USBD_ADC_MicrophoneVolumeGetCur(index, (uint8_t)(ptr_dev_data->setup_packet.wValue))) {
                  ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;   // point to data to be sent
                  if (ptr_dev_data->ep0_data.cnt > 2U) {
                    ptr_dev_data->ep0_data.cnt  = 2U;
                  }
                  if (USBD_DataInStage(device) == usbOK) {              // send requested data
                    if (USBD_StatusOutStage(device) == usbOK) {         // receive ZLP OUT
                      return true;
                    }
                  }
                }
                break;
              case AUDIO_REQUEST_GET_MIN:
                if (USBD_ADC_MicrophoneVolumeGetMin(index, (uint8_t)(ptr_dev_data->setup_packet.wValue))) {
                  ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;   // point to data to be sent
                  if (ptr_dev_data->ep0_data.cnt > 2U) {
                    ptr_dev_data->ep0_data.cnt  = 2U;
                  }
                  if (USBD_DataInStage(device) == usbOK) {              // send requested data
                    if (USBD_StatusOutStage(device) == usbOK) {         // receive ZLP OUT
                      return true;
                    }
                  }
                }
                break;
              case AUDIO_REQUEST_GET_MAX:
                if (USBD_ADC_MicrophoneVolumeGetMax(index, (uint8_t)(ptr_dev_data->setup_packet.wValue))) {
                  ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;   // point to data to be sent
                  if (ptr_dev_data->ep0_data.cnt > 2U) {
                    ptr_dev_data->ep0_data.cnt  = 2U;
                  }
                  if (USBD_DataInStage(device) == usbOK) {              // send requested data
                    if (USBD_StatusOutStage(device) == usbOK) {         // receive ZLP OUT
                      return true;
                    }
                  }
                }
                break;
              case AUDIO_REQUEST_GET_RES:
                if (USBD_ADC_MicrophoneVolumeGetRes(index, (uint8_t)(ptr_dev_data->setup_packet.wValue))) {
                  ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;   // point to data to be sent
                  if (ptr_dev_data->ep0_data.cnt > 2U) {
                    ptr_dev_data->ep0_data.cnt  = 2U;
                  }
                  if (USBD_DataInStage(device) == usbOK) {              // send requested data
                    if (USBD_StatusOutStage(device) == usbOK) {         // receive ZLP OUT
                      return true;
                    }
                  }
                }
                break;
              default:                                                  // Unsupported request
                break;
            }
            break;
          default:                                                      // Unsupported request
            break;
        }
      }
    }
  }

  return false;
}

/// \brief USB Device Endpoint 0 Event Callback - ADC specific handling (Setup Request To Endpoint)
/// \param[in]     device        device index.
/// \return        true          setup endpoint request ok
/// \return        false         setup endpoint request not supported
__WEAK bool USBD_Endpoint0_Setup_ADC_ReqToEP (uint8_t device) {
  const usbd_dev_t  *ptr_dev_cfg;
  const usbd_data_t *ptr_dev_data;
  const usbd_adc_t  *ptr_adc_cfg;
        uint8_t      index;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return false; }

  for (index = 0U; index < usbd_adc_num; index++) {
    ptr_adc_cfg = usbd_adc_ptr[index]; if (ptr_adc_cfg == NULL) { continue; }
    if ((device == ptr_adc_cfg->dev_num) &&                             // Device number correct?
      (((ptr_adc_cfg->out_cfg != NULL) &&
       ((uint8_t)(ptr_dev_data->setup_packet.wIndex) ==  ptr_adc_cfg->out_cfg->ep_iso))          ||     // Endpoint number correct? (streaming out EP)
       ((ptr_adc_cfg->in_cfg != NULL) &&
       ((uint8_t)(ptr_dev_data->setup_packet.wIndex) == (ptr_adc_cfg->in_cfg->ep_iso | 0x80U))))) {     // Endpoint number correct? (streaming in EP)
      switch (ptr_dev_data->setup_packet.bRequest) {
        case AUDIO_SAMPLING_FREQ_CONTROL:                               // Not supported
        case AUDIO_PITCH_CONTROL:                                       // Not supported
//        ptr_dev_data->ep0_data.data = ptr_dev_cfg->ep0_buf;           // data to be received
//        return true;
          break;
        default:                                                        // Unsupported request
          break;
      }
    }
  }

  return false;
}

/// \brief USB Device Endpoint 0 Event Callback - ADC specific handling (Out Request To Interface)
/// \param[in]     device        device index.
/// \return        true          out interface request ok
/// \return        false         out interface request not supported
__WEAK bool USBD_Endpoint0_Out_ADC_ReqToIF (uint8_t device) {
  const usbd_dev_t  *ptr_dev_cfg;
  const usbd_data_t *ptr_dev_data;
  const usbd_adc_t  *ptr_adc_cfg;
        uint8_t      index;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return false; }

  for (index = 0U; index < usbd_adc_num; index++) {
    ptr_adc_cfg = usbd_adc_ptr[index]; if (ptr_adc_cfg == NULL) { continue; }
    if ((device == ptr_adc_cfg->dev_num) &&                             // Device number correct?
       ((uint8_t)(ptr_dev_data->setup_packet.wIndex) == ptr_adc_cfg->cif_num)) {// Interface number correct? (audio control IF)
      if ((ptr_adc_cfg->out_cfg != NULL) && ((uint8_t)(ptr_dev_data->setup_packet.wIndex >> 8) == ptr_adc_cfg->out_cfg->fu_id)) {
        // Addressed Feature Unit: Interface = 0, ID = Speaker Feature Unit
        switch (ptr_dev_data->setup_packet.wValue >> 8) {               // Control Selector
          case AUDIO_MUTE_CONTROL:                                      // Mute Control
            switch (ptr_dev_data->setup_packet.bRequest) {
              case AUDIO_REQUEST_SET_CUR:
                if (USBD_ADC_SpeakerMuteSetCur(index, (uint8_t)(ptr_dev_data->setup_packet.wValue), (bool)(ptr_dev_cfg->ep0_buf[0] != 0U))) {
                  if (USBD_StatusInStage(device) == usbOK) {            // send ZLP IN
                    return true;
                  }
                }
                break;
              default:                                                  // Unsupported request
                break;
            }
            break;
          case AUDIO_VOLUME_CONTROL:                                    // Volume Control
            switch (ptr_dev_data->setup_packet.bRequest) {
              case AUDIO_REQUEST_SET_CUR:
                if (USBD_ADC_SpeakerVolumeSetCur(index, (uint8_t)(ptr_dev_data->setup_packet.wValue), *(uint16_t *)((uint32_t)ptr_dev_cfg->ep0_buf))) {
                  if (USBD_StatusInStage(device) == usbOK) {            // send ZLP IN
                    return true;
                  }
                }
                break;
              default:                                                  // Unsupported request
                break;
            }
            break;
          default:                                                      // Unsupported request
            break;
        }
      }
      if ((ptr_adc_cfg->in_cfg != NULL) && ((uint8_t)(ptr_dev_data->setup_packet.wIndex >> 8) == ptr_adc_cfg->in_cfg->fu_id)) {
        // Addressed Feature Unit: Interface = 0, ID = Microphone Feature Unit
        switch (ptr_dev_data->setup_packet.wValue >> 8) {               // Control Selector
          case AUDIO_MUTE_CONTROL:                                      // Mute Control
            switch (ptr_dev_data->setup_packet.bRequest) {
              case AUDIO_REQUEST_SET_CUR:
                if (USBD_ADC_MicrophoneMuteSetCur(index, (uint8_t)(ptr_dev_data->setup_packet.wValue), (bool)(ptr_dev_cfg->ep0_buf[0] != 0U))) {
                  if (USBD_StatusInStage(device) == usbOK) {            // send ZLP IN
                    return true;
                  }
                }
                break;
              default:                                                  // Unsupported request
                break;
            }
            break;
          case AUDIO_VOLUME_CONTROL:                                    // Volume Control
            switch (ptr_dev_data->setup_packet.bRequest) {
              case AUDIO_REQUEST_SET_CUR:
                if (USBD_ADC_MicrophoneVolumeSetCur(index, (uint8_t)(ptr_dev_data->setup_packet.wValue), *(uint16_t *)((uint32_t)ptr_dev_cfg->ep0_buf))) {
                  if (USBD_StatusInStage(device) == usbOK) {            // send ZLP IN
                    return true;
                  }
                }
                break;
              default:                                                  // Unsupported request
                break;
            }
            break;
          default:                                                      // Unsupported request
            break;
        }
      }
    }
  }

  return false;
}

/// \brief USB Device Endpoint 0 Event Callback - ADC specific handling (Out Request To Endpoint)
/// \param[in]     device        device index.
/// \return        true          out endpoint request ok
/// \return        false         out endpoint request not supported
__WEAK bool USBD_Endpoint0_Out_ADC_ReqToEP (uint8_t device) {
  const usbd_dev_t  *ptr_dev_cfg;
  const usbd_data_t *ptr_dev_data;
  const usbd_adc_t  *ptr_adc_cfg;
        uint8_t      index;

  if (device >= usbd_dev_num) { return false; }         // Check if device exists

  // Check all necessary pointers
  ptr_dev_cfg  = usbd_dev_ptr[device];  if (ptr_dev_cfg  == NULL) { return false; }
  ptr_dev_data = ptr_dev_cfg->data_ptr; if (ptr_dev_data == NULL) { return false; }

  for (index = 0U; index < usbd_adc_num; index++) {
    ptr_adc_cfg = usbd_adc_ptr[index]; if (ptr_adc_cfg == NULL) { continue; }
    if (device == ptr_adc_cfg->dev_num) {                               // Device number correct?
      if ((ptr_adc_cfg->out_cfg != NULL) &&
         (((uint8_t)(ptr_dev_data->setup_packet.wIndex) == ptr_adc_cfg->out_cfg->ep_iso))) {    // Endpoint number correct? (streaming out EP)
        switch (ptr_dev_data->setup_packet.bRequest) {
          case AUDIO_SAMPLING_FREQ_CONTROL:                             // Not supported
//          if (USBD_ADC_SpeakersSamplingFreqControl(index, ptr_dev_cfg->ep0_buf[0]) != 0U) {
//            if (USBD_StatusInStage(device) == usbOK) {                // send ZLP IN
//              return true;
//            }
//          }
            break;
          case AUDIO_PITCH_CONTROL:                                     // Not supported
//          if (USBD_ADC_SpeakersPitchControl(index, ptr_dev_cfg->ep0_buf[0]) != 0U) {
//            if (USBD_StatusInStage(device) == usbOK) {                // send ZLP IN
//              return true;
//            }
//          }
            break;
          default:                                                      // Unsupported request
            break;
        }
      }
      if ((ptr_adc_cfg->in_cfg != NULL) &&
         ((uint8_t)(ptr_dev_data->setup_packet.wIndex) == (ptr_adc_cfg->in_cfg->ep_iso | 0x80U))) {     // Endpoint number correct? (streaming in EP)
        switch (ptr_dev_data->setup_packet.bRequest) {
          case AUDIO_SAMPLING_FREQ_CONTROL:                             // Not supported
//          if (USBD_ADC_MicrophoneSamplingFreqControl(index, ptr_dev_cfg->ep0_buf[0]) != 0U) {
//            if (USBD_StatusInStage(device) == usbOK) {                // send ZLP IN
//              return true;
//            }
//          }
            break;
          case AUDIO_PITCH_CONTROL:                                     // Not supported
//          if (USBD_ADC_MicrophonePitchControl(index, ptr_dev_cfg->ep0_buf[0]) != 0U) {
//            if (USBD_StatusInStage(device) == usbOK) {                // send ZLP IN
//              return true;
//            }
//          }
            break;
          default:                                                      // Unsupported request
            break;
        }
      }
    }
  }

  return false;
}
