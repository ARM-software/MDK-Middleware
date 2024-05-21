/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_adc.c
 * Purpose: USB Device - Audio Device Class (ADC) module
 *----------------------------------------------------------------------------*/

#include "usbd_lib_adc.h"

#include <string.h>

#include "usb_lib_debug.h"
#include "usbd_lib.h"
#include "usbd_lib_config_def.h"
#include "usbd_lib_drv.h"
#include "usbd_evr.h"

/************************** Local function declarations ***********************/

static usbStatus USBD_ADC_CheckInstance (uint8_t instance);
static void      USBD_ADC_EpIsoIn       (uint8_t instance);
static void      USBD_ADC_EpIsoOut      (uint8_t instance);


/************************** User accessible functions *************************/

/// \brief Set range for speaker volume control
/// \param[in]     instance             instance of ADC class.
/// \param[in]     ch                   channel index :
///                                       - value 0 : master channel
///                                       - value 1 : left speaker (in stereo mode)
///                                       - value 2 : right speaker (in stereo mode)
/// \param[in]     min                  minimum volume value.
/// \param[in]     max                  maximum volume value.
/// \param[in]     res                  volume resolution.
/// \param[in]     cur                  current volume value.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_ADC_SetSpeakerVolumeRange (uint8_t instance, uint8_t ch, uint16_t min, uint16_t max, uint16_t res, uint16_t cur) {
  usbd_adc_spkr_data_t *ptr_adc_spkr_data;
  usbStatus             status;

  status = USBD_ADC_CheckInstance(instance);
  if (status == usbOK) {
    ptr_adc_spkr_data = usbd_adc_ptr[instance]->spkr_data_ptr;

    if (ptr_adc_spkr_data != NULL) {
      if (ch <= 2U) {
        if (ch == 0U) {                 // Master channel
          ptr_adc_spkr_data->vol_min[0] = min;
          ptr_adc_spkr_data->vol_min[1] = min;
          ptr_adc_spkr_data->vol_max[0] = max;
          ptr_adc_spkr_data->vol_max[1] = max;
          ptr_adc_spkr_data->vol_res[0] = res;
          ptr_adc_spkr_data->vol_res[1] = res;
          ptr_adc_spkr_data->vol_cur[0] = cur;
          ptr_adc_spkr_data->vol_cur[1] = cur;
        } else {                        // Left or right channel
          ptr_adc_spkr_data->vol_min[ch-1U] = min;
          ptr_adc_spkr_data->vol_max[ch-1U] = max;
          ptr_adc_spkr_data->vol_res[ch-1U] = res;
          ptr_adc_spkr_data->vol_cur[ch-1U] = cur;
        }
        EvrUSBD_ADC_SetSpeakerVolumeRange(instance, ch, min, max, res, cur);
      } else {                          // Non-existent channel
        status = usbInvalidParameter;
      }
    } else {
      status = usbClassErrorADC;
    }
  }

#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_ADC_SetSpeakerVolumeRangeFailed(instance, ch, min, max, res, cur, status);
  }
#endif

  return status;
}

/// \brief Set range for microphone volume (level) control
/// \param[in]     instance             instance of ADC class.
/// \param[in]     ch                   channel index :
///                                       - value 0 : master channel
///                                       - value 1 : left microphone (in stereo mode)
///                                       - value 2 : right microphone (in stereo mode)
/// \param[in]     min                  minimum volume value.
/// \param[in]     max                  maximum volume value.
/// \param[in]     res                  volume resolution.
/// \param[in]     cur                  current volume value.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
usbStatus USBD_ADC_SetMicrophoneVolumeRange (uint8_t instance, uint8_t ch, uint16_t min, uint16_t max, uint16_t res, uint16_t cur) {
  usbd_adc_mic_data_t *ptr_adc_mic_data;
  usbStatus            status;

  status = USBD_ADC_CheckInstance(instance);
  if (status == usbOK) {
    ptr_adc_mic_data = usbd_adc_ptr[instance]->mic_data_ptr;

    if (ptr_adc_mic_data != NULL) {
      if (ch <= 2U) {
        if (ch == 0U) {                 // Master channel
          ptr_adc_mic_data->vol_min[0] = min;
          ptr_adc_mic_data->vol_min[1] = min;
          ptr_adc_mic_data->vol_max[0] = max;
          ptr_adc_mic_data->vol_max[1] = max;
          ptr_adc_mic_data->vol_res[0] = res;
          ptr_adc_mic_data->vol_res[1] = res;
          ptr_adc_mic_data->vol_cur[0] = cur;
          ptr_adc_mic_data->vol_cur[1] = cur;
        } else {                        // Left or right channel
          ptr_adc_mic_data->vol_min[ch-1U] = min;
          ptr_adc_mic_data->vol_max[ch-1U] = max;
          ptr_adc_mic_data->vol_res[ch-1U] = res;
          ptr_adc_mic_data->vol_cur[ch-1U] = cur;
        }
        EvrUSBD_ADC_SetMicrophoneVolumeRange(instance, ch, min, max, res, cur);
      } else {                          // Non-existent channel
        status = usbInvalidParameter;
      }
    } else {
      status = usbClassErrorADC;
    }
  }

#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_ADC_SetMicrophoneVolumeRangeFailed(instance, ch, min, max, res, cur, status);
  }
#endif

  return status;
}

/// \brief Number of audio samples received from USB Host and available to be read
/// \param[in]     instance             instance of ADC class.
/// \return                             number of samples available to be read.
uint32_t USBD_ADC_ReceivedSamplesAvailable (uint8_t instance) {
  const usbd_adc_t           *ptr_adc_cfg;
  const usbd_adc_spkr_data_t *ptr_adc_spkr_data;
        uint32_t              result;

  result = 0U;
  if (USBD_ADC_CheckInstance(instance) == usbOK) {
    ptr_adc_cfg       = usbd_adc_ptr[instance];
    ptr_adc_spkr_data = ptr_adc_cfg->spkr_data_ptr;

    if (ptr_adc_spkr_data != NULL) {
      result = ptr_adc_spkr_data->data_received_cnt - ptr_adc_spkr_data->data_read_cnt;
      switch (ptr_adc_cfg->out_cfg->sample_sz) {
        case 4U:
          result >>= 2U;
          break;
        case 3U:
          result  /= 3U;
          break;
        case 2U:
          result >>= 1U;
          break;
        case 1U:
          break;
        default:
          result   = 0U;
          break;
      }
    }
  }

  EvrUSBD_ADC_ReceivedSamplesAvailable(instance, result);
  return result;
}

/// \brief Number of audio samples written and pending to be sent to USB Host
/// \param[in]     instance             instance of ADC class.
/// \return                             number of samples ready to be sent.
uint32_t USBD_ADC_WrittenSamplesPending (uint8_t instance) {
  const usbd_adc_t           *ptr_adc_cfg;
  const usbd_adc_mic_data_t  *ptr_adc_mic_data;
        uint32_t              result;

  result = 0U;
  if (USBD_ADC_CheckInstance(instance) == usbOK) {
    ptr_adc_cfg      = usbd_adc_ptr[instance];
    ptr_adc_mic_data = ptr_adc_cfg->mic_data_ptr;

    if (ptr_adc_mic_data != NULL) {
      result = (uint32_t)(ptr_adc_mic_data->data_to_send_cnt - ptr_adc_mic_data->data_sent_cnt);
      switch (ptr_adc_cfg->in_cfg->sample_sz) {
        case 4:
          result >>= 2U;
          break;
        case 3:
          result  /= 3U;
          break;
        case 2:
          result >>= 1U;
          break;
        case 1U:
          break;
        default:
          result   = 0U;
          break;
      }
    }
  }

  EvrUSBD_ADC_WrittenSamplesPending(instance, result);
  return result;
}

/// \brief Read audio samples received from USB Host
/// \param[in]     instance             instance of ADC class.
/// \param[out]    buf                  buffer that receives samples.
/// \param[in]     num                  maximum number of samples to read.
/// \return                             number of samples read or execution status :
///                                       - value >= 0 : number of samples read
///                                       - value < 0 :  error occurred, -value is execution status as defined with \ref usbStatus
int32_t USBD_ADC_ReadSamples (uint8_t instance, void *buf, int32_t num) {
  const usbd_adc_t           *ptr_adc_cfg;
        usbd_adc_spkr_data_t *ptr_adc_spkr_data;
        usbd_adc_code_t      *ptr_out_cfg;
        uint8_t              *ptr_buf_8;
  const uint8_t              *ptr_src_8;
#ifdef __ARM_BIG_ENDIAN
        uint8_t              *ptr_dest_8;
#else
        uint32_t             *ptr_dest_32;
#endif
        uint32_t              len_data, len_till_wrap, len_req, len, cnt, len_cur;
        uint32_t              result;
        bool                  convert_24_to_32, wrap;
        usbStatus             status;

  status = USBD_ADC_CheckInstance(instance);
  if (status != usbOK) {
    EvrUSBD_ADC_ReadSamplesFailed(instance, status);
    return -(int32_t)status;
  }

  result            = 0U;
  ptr_adc_cfg       = usbd_adc_ptr[instance];
  ptr_adc_spkr_data = ptr_adc_cfg->spkr_data_ptr;
  ptr_out_cfg       = ptr_adc_cfg->out_cfg;

  if ((ptr_adc_spkr_data == NULL) || (ptr_out_cfg == NULL)) {
    status = usbClassErrorADC;
    EvrUSBD_ADC_ReadSamplesFailed(instance, status);
    return -(int32_t)status;
  }

  switch (ptr_out_cfg->sample_res) {
    case 8:                             // 8-bit resolution is supported
    case 16:                            // 16-bit resolution is supported
    case 24:                            // 24-bit resolution is supported
    case 32:                            // 32-bit resolution is supported
      break;
    default:                            // other resolutions are not supported
      goto exit;
  }
  if (ptr_out_cfg->sample_res > (ptr_out_cfg->sample_sz << 3)) {        // Invalid configuration
    goto exit;
  }

  if ((ptr_out_cfg->sample_res == 24U) &&
      (ptr_out_cfg->sample_sz  ==  3U)) {
    // If resolution is 24 bits and sample size in bytes is 3 then special
    // handling of data is needed to make samples in uint32_t
    convert_24_to_32 = true;
  } else {
    convert_24_to_32 = false;
  }

  len_data = (uint32_t)(ptr_adc_spkr_data->data_received_cnt - ptr_adc_spkr_data->data_read_cnt);

  if (len_data == 0U) {                 // If no data available
    len_req = 0U;
    goto chk_act;
  }
                                        // Number of bytes to read
  len_req = (uint32_t)num * ptr_out_cfg->sample_sz;

  if (len_req > len_data) {             // If more requested then available
    len_req = len_data;                 // correct to return maximum available
  }

  len_till_wrap = ((uint32_t)ptr_out_cfg->data_buf_sz - (uint32_t)ptr_out_cfg->max_packet_size) - ((uint32_t)ptr_adc_spkr_data->ptr_data_read - (uint32_t)ptr_out_cfg->data_buf);
  len           = len_req;
  ptr_buf_8     = (uint8_t *)buf;

  if (len >= len_till_wrap) {
    len_cur = len_till_wrap;
    wrap    = true;
  } else {
    len_cur = len;
    wrap    = false;
  }

  while (len_cur != 0U) {
#ifdef __ARM_BIG_ENDIAN
    ptr_src_8  = ptr_adc_spkr_data->ptr_data_read;
    ptr_dest_8 = ptr_buf_8;
    if (convert_24_to_32) {
      cnt = len_cur / 3U;
      while (cnt-- != 0U) {
        *ptr_dest_8 =   0U;                 ptr_dest_8++;   // Add dummy 0 byte
        *ptr_dest_8 = *(ptr_src_8+2U);      ptr_dest_8++;
        *ptr_dest_8 = *(ptr_src_8+1U);      ptr_dest_8++;
        *ptr_dest_8 = *(ptr_src_8);         ptr_dest_8++; ptr_src_8 += 3U;
      }
    } else {
      switch (ptr_out_cfg->sample_sz) {
        case 4:
          cnt = len_cur >> 2U;
          while (cnt-- != 0U) {
            *ptr_dest_8 = *(ptr_src_8+3U);  ptr_dest_8++;
            *ptr_dest_8 = *(ptr_src_8+2U);  ptr_dest_8++;
            *ptr_dest_8 = *(ptr_src_8+1U);  ptr_dest_8++;
            *ptr_dest_8 = *(ptr_src_8);     ptr_dest_8++; ptr_src_8 += 4U;
          }
          break;
        case 3:
          cnt = len_cur / 3U;
          while (cnt-- != 0U) {
            *ptr_dest_8 = *(ptr_src_8+2U);  ptr_dest_8++;
            *ptr_dest_8 = *(ptr_src_8+1U);  ptr_dest_8++;
            *ptr_dest_8 = *(ptr_src_8);     ptr_dest_8++; ptr_src_8 += 3U;
          }
          break;
        case 2:
          cnt = len_cur >> 1U;
          while (cnt-- != 0U) {
            *ptr_dest_8 = *(ptr_src_8+1U);  ptr_dest_8++;
            *ptr_dest_8 = *(ptr_src_8);     ptr_dest_8++; ptr_src_8 += 2U;

          }
          break;
        case 1:
          memcpy ((void *)ptr_dest_8, (const void *)ptr_src_8, len_cur);
          break;
        default:                        // Invalid sample size
          break;
      }
      ptr_buf_8 += (len_cur / 3U) * 4U;
    }
#else
    ptr_src_8   = ptr_adc_spkr_data->ptr_data_read;
    ptr_dest_32 = (uint32_t *)((uint32_t)ptr_buf_8);
    if (convert_24_to_32) {
      cnt = len_cur / 3U;
      while (cnt-- != 0U) {
        *ptr_dest_32 = (uint32_t)(__UNALIGNED_UINT32_READ(ptr_src_8)) & 0x00FFFFFFUL;
         ptr_dest_32 ++;
         ptr_src_8   += 3U;
      }
      ptr_buf_8 += (len_cur / 3U) * 4U;
    } else {
      memcpy ((void *)ptr_dest_32, (const void *)ptr_src_8, len_cur);
      ptr_buf_8 +=  len_cur;
    }
#endif
    if (wrap) {
      wrap = false;
      ptr_adc_spkr_data->ptr_data_read  = ptr_out_cfg->data_buf;
    } else {
      ptr_adc_spkr_data->ptr_data_read += len_cur;
    }
    ptr_adc_spkr_data->data_read_cnt   += len_cur;
    len     -= len_cur;
    len_cur  = len;
  }

  // Check if there is enough room to receive one maximum packet
  // Effectively buffer is used with one maximum packet less as this way
  // we ensure enough of contiguous space at end of buffer
chk_act:
  if ((ptr_adc_spkr_data->active == 1U) &&
      (ptr_adc_spkr_data->receive_active == 0U) &&
     ((ptr_out_cfg->data_buf_sz - ptr_out_cfg->max_packet_size) >= ((uint32_t)(ptr_adc_spkr_data->data_received_cnt - ptr_adc_spkr_data->data_read_cnt) + ptr_out_cfg->max_packet_size))) {
    ptr_adc_spkr_data->receive_active = 2U;     // Value 2 to start reception in thread
    (void)USBD_ThreadFlagsSet (usbd_adc_thread_id[instance], (((uint32_t)ptr_out_cfg->ep_iso) << 12) | (ARM_USBD_EVENT_OUT << 8));
  }

  // Check if there is all data was read out
  if ((len_req != 0U) && (ptr_adc_spkr_data->data_received_cnt == ptr_adc_spkr_data->data_read_cnt)) {
    ptr_adc_spkr_data->reached_zero = 1U;
  }

  // Recalculate number of bytes back to number of samples
  result = len_req;
  switch (ptr_out_cfg->sample_sz) {
    case 4:
      result >>= 2U;
      break;
    case 3:
      result  /= 3U;
      break;
    case 2:
      result >>= 1U;
      break;
    case 1:
      break;
    default:
      result   = 0U;
      break;
  }

exit:
  EvrUSBD_ADC_ReadSamples(instance, (int32_t)result);
  return (int32_t)result;
}

/// \brief Write audio samples to be transferred to USB Host
/// \param[in]     instance             instance of ADC class.
/// \param[in]     buf                  buffer containing samples to write.
/// \param[in]     num                  number of samples to write.
/// \return                             number of samples written or execution status :
///                                       - value >= 0 : number of samples written for sending
///                                       - value < 0 :  error occurred, -value is execution status as defined with \ref usbStatus
int32_t USBD_ADC_WriteSamples (uint8_t instance, const void *buf, int32_t num) {
  const usbd_adc_t           *ptr_adc_cfg;
        usbd_adc_mic_data_t  *ptr_adc_mic_data;
        usbd_adc_code_t      *ptr_in_cfg;
        const uint8_t        *ptr_buf_8;
        uint8_t              *ptr_dest_8;
        const uint8_t        *ptr_src_8;
        uint32_t              len_free, len_till_wrap, len_req, len, cnt, len_cur;
        uint32_t              result;
        bool                  convert_32_to_24, wrap;
        usbStatus             status;

  status = USBD_ADC_CheckInstance(instance);
  if (status != usbOK) {
    EvrUSBD_ADC_WriteSamplesFailed(instance, status);
    return -(int32_t)status;
  }

  result           = 0U;
  ptr_adc_cfg      = usbd_adc_ptr[instance];
  ptr_adc_mic_data = ptr_adc_cfg->mic_data_ptr;
  ptr_in_cfg       = ptr_adc_cfg->in_cfg;

  if ((ptr_adc_mic_data == NULL) || (ptr_in_cfg == NULL)) {
    status = usbClassErrorADC;
    EvrUSBD_ADC_WriteSamplesFailed(instance, status);
    return -(int32_t)status;
  }

  switch (ptr_in_cfg->sample_res) {
    case 8:                             // 8-bit resolution is supported
    case 16:                            // 16-bit resolution is supported
    case 24:                            // 24-bit resolution is supported
    case 32:                            // 32-bit resolution is supported
      break;
    default:                            // other resolutions are not supported
      goto exit;
  }
  if (ptr_in_cfg->sample_res > (ptr_in_cfg->sample_sz << 3)) {          // Invalid configuration
    goto exit;
  }

  if ((ptr_in_cfg->sample_res == 24U) &&
      (ptr_in_cfg->sample_sz  ==  3U)  ) {
    // If resolution is 24 bits and sample size in bytes is 3 then special
    // handling of data is needed to make samples from uint32_t
    convert_32_to_24 = true;
  } else {
    convert_32_to_24 = false;
  }

  len_free = ptr_in_cfg->data_buf_sz - (uint32_t)(ptr_adc_mic_data->data_to_send_cnt - ptr_adc_mic_data->data_sent_cnt);

  if (len_free == 0U) {                 // If no free space available
    len_req = 0U;
    goto chk_act;
  }

                                        // Number of bytes to write
  len_req = (uint32_t)num * ptr_in_cfg->sample_sz;

  if (len_req > len_free) {             // If more requested then free space available
    len_req = len_free;                 // correct to return maximum free space available
  }

  len_till_wrap = (uint32_t)ptr_in_cfg->data_buf_sz - ((uint32_t)ptr_adc_mic_data->ptr_data_to_send - (uint32_t)ptr_in_cfg->data_buf);
  len           = len_req;
  ptr_buf_8     = (const uint8_t *)buf;

  if (len >= len_till_wrap) {
    len_cur = len_till_wrap;
    wrap    = true;
  } else {
    len_cur = len;
    wrap    = false;
  }

  while (len_cur != 0U) {
#ifdef __ARM_BIG_ENDIAN
    ptr_src_8  = ptr_buf_8;
    ptr_dest_8 = ptr_adc_mic_data->ptr_data_to_send;
    if (convert_32_to_24) {
      cnt = len_cur / 3U;
      while (cnt-- != 0U) {
                        ptr_src_8++;                            // Skip dummy 0 byte
        *ptr_dest_8 = *(ptr_src_8+2U);      ptr_dest_8++;
        *ptr_dest_8 = *(ptr_src_8+1U);      ptr_dest_8++;
        *ptr_dest_8 = *(ptr_src_8);         ptr_dest_8++; ptr_src_8 += 3U;
      }
    } else {
      switch (ptr_in_cfg->sample_sz) {
        case 4:
          cnt = len_cur >> 2U;
          while (cnt-- != 0U) {
            *ptr_dest_8 = *(ptr_src_8+3U);  ptr_dest_8++;
            *ptr_dest_8 = *(ptr_src_8+2U);  ptr_dest_8++;
            *ptr_dest_8 = *(ptr_src_8+1U);  ptr_dest_8++;
            *ptr_dest_8 = *(ptr_src_8);     ptr_dest_8++; ptr_src_8 += 4U;
          }
          break;
        case 3:
          cnt = len_cur / 3U;
          while (cnt-- != 0U) {
            *ptr_dest_8 = *(ptr_src_8+2U);  ptr_dest_8++;
            *ptr_dest_8 = *(ptr_src_8+1U);  ptr_dest_8++;
            *ptr_dest_8 = *(ptr_src_8);     ptr_dest_8++; ptr_src_8 += 3U;
          }
          break;
        case 2:
          cnt = len_cur >> 1U;
          while (cnt-- != 0U) {
            *ptr_dest_8 = *(ptr_src_8+1U);  ptr_dest_8++;
            *ptr_dest_8 = *(ptr_src_8);     ptr_dest_8++; ptr_src_8 += 2U;
          }
          break;
        case 1:
          memcpy ((void *)ptr_dest_8, (const void *)ptr_src_8, len_cur);
          break;
        default:                        // Invalid sample size
          break;
      }
      ptr_buf_8 += (len_cur / 3U) * 4U;
    }
#else
    ptr_src_8  = ptr_buf_8;
    ptr_dest_8 = ptr_adc_mic_data->ptr_data_to_send;
    if (convert_32_to_24) {
      cnt = len_cur / 3U;
      while (cnt-- != 0U) {
                         ptr_src_8++;   // Skip LSB byte
        *ptr_dest_8 = *ptr_src_8; ptr_dest_8++; ptr_src_8++;
        *ptr_dest_8 = *ptr_src_8; ptr_dest_8++; ptr_src_8++;
        *ptr_dest_8 = *ptr_src_8; ptr_dest_8++; ptr_src_8++;
      }
      ptr_buf_8 += (len_cur / 3U) * 4U;
    } else {
      memcpy ((void *)ptr_dest_8, (const void *)ptr_src_8, len_cur);
      ptr_buf_8 +=  len_cur;
    }
#endif
    if (wrap) {
      wrap = false;
      ptr_adc_mic_data->ptr_data_to_send  = ptr_in_cfg->data_buf;
    } else {
      ptr_adc_mic_data->ptr_data_to_send += len_cur;
    }
    ptr_adc_mic_data->data_to_send_cnt   += len_cur;
    len     -= len_cur;
    len_cur  = len;
  }

  // If sending is not active and if half of buffer of data is available, activate sending
chk_act:
  if ((ptr_adc_mic_data->active == 1U) &&
      (ptr_adc_mic_data->send_active == 0U) &&
     ((uint32_t)(ptr_adc_mic_data->data_to_send_cnt - ptr_adc_mic_data->data_sent_cnt) >= (ptr_in_cfg->data_buf_sz / 2U))) {
      // Send signal to thread to start sending data
      ptr_adc_mic_data->send_active = 2U;       // Value 2 to start transmission in thread
      (void)USBD_ThreadFlagsSet (usbd_adc_thread_id[instance], (((uint32_t)ptr_in_cfg->ep_iso) << 12) | (ARM_USBD_EVENT_IN << 8));
  }

  // Recalculate number of bytes back to number of samples
  result = len_req;  
  switch (ptr_in_cfg->sample_sz) {
    case 4:
      result >>= 2U;
      break;
    case 3:
      result  /= 3U;
      break;
    case 2:
      result >>= 1U;
      break;
    case 1:
      break;
    default:
      result   = 0U;
      break;
  }

exit:
  EvrUSBD_ADC_WriteSamples(instance, (int32_t)result);
  return (int32_t)result;
}


/************************** Library accessible functions **********************/

/// \brief Audio Class (ADC) get current mute setting for out channel (speaker)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_SpeakerMuteGetCur (uint8_t instance, uint8_t ch) {
  const usbd_adc_t *ptr_adc_cfg;

  (void)ch;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  ptr_adc_cfg = usbd_adc_ptr[instance];

  usbd_dev_ptr[ptr_adc_cfg->dev_num]->ep0_buf[0] = ptr_adc_cfg->spkr_data_ptr->mute;

  return true;
}

/// \brief Audio Class (ADC) set current mute setting for out channel (speaker)
///        (calls USBD_ADCn_SpeakerMuteEvent function)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \param[in]     on            mute setting.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_SpeakerMuteSetCur (uint8_t instance, uint8_t ch, bool on) {

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  if (on) {
    usbd_adc_ptr[instance]->spkr_data_ptr->mute = 1U;
  } else {
    usbd_adc_ptr[instance]->spkr_data_ptr->mute = 0U;
  }

  if (fpUSBD_ADC_SetSpeakerMute [instance] != NULL) {
    EvrUSBD_ADC_OnSetSpeakerMute(instance,  ch, on);
    fpUSBD_ADC_SetSpeakerMute   [instance] (ch, on);
  }

  return true;
}

/// \brief Audio Class (ADC) get current volume setting for out channel (speaker)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_SpeakerVolumeGetCur (uint8_t instance, uint8_t ch) {
  const usbd_adc_t *ptr_adc_cfg;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  if ((ch == 0U) || (ch > 2U)) {        // Master channel volume or non-existent channel
    return false;
  }

  ptr_adc_cfg = usbd_adc_ptr[instance];

  *((uint16_t *)((uint32_t)usbd_dev_ptr[ptr_adc_cfg->dev_num]->ep0_buf)) = ptr_adc_cfg->spkr_data_ptr->vol_cur[ch-1U];

  return true;
}

/// \brief Audio Class (ADC) set current volume setting for out channel (speaker)
///        (calls USBD_ADCn_SpeakerVolumeEvent function)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \param[in]     vol           volume setting.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_SpeakerVolumeSetCur (uint8_t instance, uint8_t ch, uint16_t vol) {

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  if ((ch == 0U) || (ch > 2U)) {        // Master channel volume or non-existent channel
    return false;
  }

  usbd_adc_ptr[instance]->spkr_data_ptr->vol_cur[ch-1U] = vol;

  if (fpUSBD_ADC_SetSpeakerVolume [instance] != NULL) {
    EvrUSBD_ADC_OnSetSpeakerVolume(instance,  ch, vol);
    fpUSBD_ADC_SetSpeakerVolume   [instance] (ch, vol);
  }

  return true;
}

/// \brief Audio Class (ADC) get volume minimum for out channel (speaker)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_SpeakerVolumeGetMin (uint8_t instance, uint8_t ch) {
  const usbd_adc_t *ptr_adc_cfg;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  if ((ch == 0U) || (ch > 2U)) {        // Master channel volume or non-existent channel
    return false;
  }

  ptr_adc_cfg = usbd_adc_ptr[instance];

  *((uint16_t *)((uint32_t)usbd_dev_ptr[ptr_adc_cfg->dev_num]->ep0_buf)) = ptr_adc_cfg->spkr_data_ptr->vol_min[ch-1U];

  return true;
}

/// \brief Audio Class (ADC) get volume maximum for out channel (speaker)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_SpeakerVolumeGetMax (uint8_t instance, uint8_t ch) {
  const usbd_adc_t *ptr_adc_cfg;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  if ((ch == 0U) || (ch > 2U)) {        // Master channel volume or non-existent channel
    return false;
  }

  ptr_adc_cfg = usbd_adc_ptr[instance];

  *((uint16_t *)((uint32_t)usbd_dev_ptr[ptr_adc_cfg->dev_num]->ep0_buf)) = ptr_adc_cfg->spkr_data_ptr->vol_max[ch-1U];

  return true;
}

/// \brief Audio Class (ADC) get volume resolution for out channel (speaker)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_SpeakerVolumeGetRes (uint8_t instance, uint8_t ch) {
  const usbd_adc_t *ptr_adc_cfg;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  if ((ch == 0U) || (ch > 2U)) {        // Master channel volume or non-existent channel
    return false;
  }

  ptr_adc_cfg = usbd_adc_ptr[instance];

  *((uint16_t *)((uint32_t)usbd_dev_ptr[ptr_adc_cfg->dev_num]->ep0_buf)) = ptr_adc_cfg->spkr_data_ptr->vol_res[ch-1U];

  return true;
}

/// \brief Audio Class (ADC) get current mute setting for in channel (microphone)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_MicrophoneMuteGetCur (uint8_t instance, uint8_t ch) {
  const usbd_adc_t *ptr_adc_cfg;

  (void)ch;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  ptr_adc_cfg = usbd_adc_ptr[instance];

  usbd_dev_ptr[ptr_adc_cfg->dev_num]->ep0_buf[0] = ptr_adc_cfg->mic_data_ptr->mute;

  return true;
}

/// \brief Audio Class (ADC) set current mute setting for in channel (microphone)
///        (calls USBD_ADCn_MicrophoneMuteEvent function)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \param[in]     on            mute setting.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_MicrophoneMuteSetCur (uint8_t instance, uint8_t ch, bool on) {

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  if (on) {
    usbd_adc_ptr[instance]->mic_data_ptr->mute = 1U;
  } else {
    usbd_adc_ptr[instance]->mic_data_ptr->mute = 0U;
  }

  if (fpUSBD_ADC_SetMicrophoneMute [instance] != NULL) {
    EvrUSBD_ADC_OnSetMicrophoneMute(instance,  ch, on);
    fpUSBD_ADC_SetMicrophoneMute   [instance] (ch, on);
  }

  return true;
}

/// \brief Audio Class (ADC) get current volume setting for in channel (microphone)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_MicrophoneVolumeGetCur (uint8_t instance, uint8_t ch) {
  const usbd_adc_t *ptr_adc_cfg;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  if ((ch == 0U) || (ch > 2U)) {        // Master channel volume or non-existent channel
    return false;
  }

  ptr_adc_cfg = usbd_adc_ptr[instance];

  *((uint16_t *)((uint32_t)usbd_dev_ptr[ptr_adc_cfg->dev_num]->ep0_buf)) = ptr_adc_cfg->mic_data_ptr->vol_cur[ch-1U];

  return true;
}

/// \brief Audio Class (ADC) set current volume setting for in channel (microphone)
///        (calls USBD_ADCn_MicrophoneVolumeEvent function)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \param[in]     vol           volume setting.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_MicrophoneVolumeSetCur (uint8_t instance, uint8_t ch, uint16_t vol) {

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  if ((ch == 0U) || (ch > 2U)) {        // Master channel volume or non-existent channel
    return false;
  }

  usbd_adc_ptr[instance]->mic_data_ptr->vol_cur[ch-1U] = vol;

  if (fpUSBD_ADC_SetMicrophoneVolume [instance] != NULL) {
    EvrUSBD_ADC_OnSetMicrophoneVolume(instance,  ch, vol);
    fpUSBD_ADC_SetMicrophoneVolume   [instance] (ch, vol);
  }

  return true;
}

/// \brief Audio Class (ADC) get volume minimum for in channel (microphone)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_MicrophoneVolumeGetMin (uint8_t instance, uint8_t ch) {
  const usbd_adc_t *ptr_adc_cfg;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  if ((ch == 0U) || (ch > 2U)) {        // Master channel volume or non-existent channel
    return false;
  }

  ptr_adc_cfg = usbd_adc_ptr[instance];

  *((uint16_t *)((uint32_t)usbd_dev_ptr[ptr_adc_cfg->dev_num]->ep0_buf)) = ptr_adc_cfg->mic_data_ptr->vol_min[ch-1U];

  return true;
}

/// \brief Audio Class (ADC) get volume maximum for in channel (microphone)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_MicrophoneVolumeGetMax (uint8_t instance, uint8_t ch) {
  const usbd_adc_t *ptr_adc_cfg;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  if ((ch == 0U) || (ch > 2U)) {        // Master channel volume or non-existent channel
    return false;
  }

  ptr_adc_cfg = usbd_adc_ptr[instance];

  *((uint16_t *)((uint32_t)usbd_dev_ptr[ptr_adc_cfg->dev_num]->ep0_buf)) = ptr_adc_cfg->mic_data_ptr->vol_max[ch-1U];

  return true;
}

/// \brief Audio Class (ADC) get volume resolution for in channel (microphone)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
bool USBD_ADC_MicrophoneVolumeGetRes (uint8_t instance, uint8_t ch) {
  const usbd_adc_t *ptr_adc_cfg;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return false;
  }

  if ((ch == 0U) || (ch > 2U)) {        // Master channel volume or non-existent channel
    return false;
  }

  ptr_adc_cfg = usbd_adc_ptr[instance];

  *((uint16_t *)((uint32_t)usbd_dev_ptr[ptr_adc_cfg->dev_num]->ep0_buf)) = ptr_adc_cfg->mic_data_ptr->vol_res[ch-1U];

  return true;
}


/************************** Configuration accessible functions ****************/

/// \brief Initialize ADC device instance and create event handling Thread
/// \param[in]     instance      instance of ADC class.
/// \return                      status code that indicates the execution status of the function.
usbStatus USBD_ADC_Initialize (uint8_t instance) {
  usbStatus status;

  EvrUSBD_ADC_Initialize(instance);

  status = USBD_ADC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  if (fpUSBD_ADC_Initialize [instance] != NULL) {
    EvrUSBD_ADC_OnInitialize(instance);
    fpUSBD_ADC_Initialize   [instance] ();
  }

  if (usbd_adc_thread_id[instance] == NULL) {
    usbd_adc_thread_id[instance] = USBD_ThreadCreate (usbdThreadADC, instance);
    if (usbd_adc_thread_id[instance] == NULL) {
      status = usbThreadError;
    }
  }

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_ADC_InitializeFailed(instance, status);
  }
#endif

  return status;
}

/// \brief De-initialize ADC device instance and terminate event handling Thread
/// \param[in]     instance      instance of ADC class.
/// \return                      status code that indicates the execution status of the function.
usbStatus USBD_ADC_Uninitialize (uint8_t instance) {
  usbStatus status;

  EvrUSBD_ADC_Uninitialize(instance);

  status = USBD_ADC_CheckInstance(instance);
  if (status != usbOK) {
    goto exit;
  }

  if (usbd_adc_thread_id[instance] != NULL) {
    if (USBD_ThreadTerminate (usbd_adc_thread_id[instance]) != 0) {
      status = usbThreadError;
      goto exit;
    }
    usbd_adc_thread_id[instance] = NULL;
  }

  if (fpUSBD_ADC_Uninitialize [instance] != NULL) {
    EvrUSBD_ADC_OnUninitialize(instance);
    fpUSBD_ADC_Uninitialize   [instance] ();
  }

exit:
#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1))
  if (status != usbOK) {
    EvrUSBD_ADC_UninitializeFailed(instance, status);
  }
#endif

  return status;
}

/// \brief ADC USB Bus Reset event handling
/// \param[in]     instance      instance of ADC class.
void USBD_ADC_Reset (uint8_t instance) {
  const usbd_adc_t *ptr_adc_cfg;
  usbStatus   status;

  EvrUSBD_ADC_Reset(instance);

  status = USBD_ADC_CheckInstance(instance);
  if (status != usbOK) {
    EvrUSBD_ADC_ResetFailed(instance, status);
    return;
  }

  ptr_adc_cfg = usbd_adc_ptr[instance];

  if ((ptr_adc_cfg->out_cfg != NULL) && (ptr_adc_cfg->spkr_data_ptr != NULL)) {
    USBD_ADC_EndpointStop (instance, USB_ENDPOINT_OUT(ptr_adc_cfg->out_cfg->ep_iso));
  }

  if ((ptr_adc_cfg->in_cfg != NULL) && (ptr_adc_cfg->mic_data_ptr != NULL)) {
    USBD_ADC_EndpointStop (instance, USB_ENDPOINT_IN(ptr_adc_cfg->in_cfg->ep_iso));
  }
}

/// \brief ADC Endpoint Start handling
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ep_addr       endpoint address.
///                 - ep_addr.0..3: address
///                 - ep_addr.7:    direction
void USBD_ADC_EndpointStart (uint8_t instance, uint8_t ep_addr) {
  const usbd_adc_t           *ptr_adc_cfg;
        usbd_adc_spkr_data_t *ptr_adc_spkr_data;
        usbd_adc_mic_data_t  *ptr_adc_mic_data;
        usbd_adc_code_t      *ptr_out_cfg;
        usbd_adc_code_t      *ptr_in_cfg;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_adc_cfg       = usbd_adc_ptr[instance];
  ptr_adc_spkr_data = ptr_adc_cfg->spkr_data_ptr;
  ptr_adc_mic_data  = ptr_adc_cfg->mic_data_ptr;
  ptr_out_cfg       = ptr_adc_cfg->out_cfg;
  ptr_in_cfg        = ptr_adc_cfg->in_cfg;

  if ((ptr_adc_spkr_data != NULL) && (ptr_out_cfg != NULL) && (USB_ENDPOINT_OUT(ptr_out_cfg->ep_iso) == ep_addr)) {
    if ((ptr_adc_spkr_data->receive_active == 0U) &&
        (ptr_out_cfg->data_buf_sz >= ptr_out_cfg->max_packet_size)) {
      ptr_adc_spkr_data->reached_zero   = 1U;
      ptr_adc_spkr_data->receive_active = 2U;   // Value 2 to start reception in thread
      (void)USBD_ThreadFlagsSet (usbd_adc_thread_id[instance], (((uint32_t)ptr_out_cfg->ep_iso) << 12) | (ARM_USBD_EVENT_OUT << 8));
    }
    ptr_adc_spkr_data->active = 1U;
    if (fpUSBD_ADC_PlayStart [instance] != NULL) {
      EvrUSBD_ADC_OnPlayStart(instance);
      fpUSBD_ADC_PlayStart   [instance] ();
    }
  }
  if ((ptr_in_cfg != NULL) && (USB_ENDPOINT_IN(ptr_in_cfg->ep_iso) == ep_addr)) {
    ptr_adc_mic_data->active = 1U;
    if (fpUSBD_ADC_RecordStart [instance] != NULL) {
      EvrUSBD_ADC_OnRecordStart(instance);
      fpUSBD_ADC_RecordStart   [instance] ();
    }
  }
}

/// \brief ADC Endpoint Stop handling
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ep_addr       endpoint address.
///                 - ep_addr.0..3: address
///                 - ep_addr.7:    direction
void USBD_ADC_EndpointStop (uint8_t instance, uint8_t ep_addr) {
  const usbd_adc_t           *ptr_adc_cfg;
        usbd_adc_spkr_data_t *ptr_adc_spkr_data;
        usbd_adc_mic_data_t  *ptr_adc_mic_data;
        usbd_adc_code_t      *ptr_out_cfg;
        usbd_adc_code_t      *ptr_in_cfg;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_adc_cfg       = usbd_adc_ptr[instance];
  ptr_adc_spkr_data = ptr_adc_cfg->spkr_data_ptr;
  ptr_adc_mic_data  = ptr_adc_cfg->mic_data_ptr;
  ptr_out_cfg       = ptr_adc_cfg->out_cfg;
  ptr_in_cfg        = ptr_adc_cfg->in_cfg;

  if ((ptr_adc_spkr_data != NULL) && (ptr_out_cfg != NULL) && (USB_ENDPOINT_OUT(ptr_out_cfg->ep_iso) == ep_addr)) {
    ptr_adc_spkr_data->active            = 0U;
    ptr_adc_spkr_data->receive_active    = 0U;
    ptr_adc_spkr_data->ptr_data_received = ptr_out_cfg->data_buf;
    ptr_adc_spkr_data->data_received_cnt = 0U;
    ptr_adc_spkr_data->ptr_data_read     = ptr_out_cfg->data_buf;
    ptr_adc_spkr_data->data_read_cnt     = 0U;
    if (fpUSBD_ADC_PlayStop [instance] != NULL) {
      EvrUSBD_ADC_OnPlayStop(instance);
      fpUSBD_ADC_PlayStop   [instance] ();
    }
  }
  if ((ptr_adc_mic_data != NULL) && (ptr_in_cfg != NULL) && (USB_ENDPOINT_IN(ptr_in_cfg->ep_iso) == ep_addr)) {
    ptr_adc_mic_data->active             = 0U;
    ptr_adc_mic_data->send_active        = 0U;
    ptr_adc_mic_data->ptr_data_to_send   = ptr_in_cfg->data_buf;
    ptr_adc_mic_data->data_to_send_cnt   = 0U;
    ptr_adc_mic_data->ptr_data_sent      = ptr_in_cfg->data_buf;
    ptr_adc_mic_data->data_sent_cnt      = 0U;
    if (fpUSBD_ADC_RecordStop [instance] != NULL) {
      EvrUSBD_ADC_OnRecordStop(instance);
      fpUSBD_ADC_RecordStop   [instance] ();
    }
  }
}


/************************** Local functions ***********************************/

/// \brief Check all ADC device instance pointers and variables
/// \param[in]     instance      instance of ADC class.
/// \return                      status code that indicates the execution status of the function.
static usbStatus USBD_ADC_CheckInstance (uint8_t instance) {

  if (instance >= usbd_adc_num)                                        { return usbClassErrorADC; }
  if (usbd_adc_ptr[instance]                                  == NULL) { return usbClassErrorADC; }
  if (usbd_adc_ptr[instance]->dev_num >= usbd_dev_num)                 { return usbDeviceError;   }
  if (usbd_dev_ptr[usbd_adc_ptr[instance]->dev_num]           == NULL) { return usbDeviceError;   }
  if (usbd_dev_ptr[usbd_adc_ptr[instance]->dev_num]->data_ptr == NULL) { return usbDeviceError;   }

  return usbOK;
}

/// \brief ADC Isochronous Out Endpoint event handling
/// \param[in]     instance      instance of ADC class.
static void USBD_ADC_EpIsoOut (uint8_t instance) {
  const usbd_adc_t           *ptr_adc_cfg;
        usbd_adc_spkr_data_t *ptr_adc_spkr_data;
        usbd_adc_code_t      *ptr_out_cfg;
        uint32_t              len_received, len_till_wrap;
        uint8_t               device, ep_iso;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_adc_cfg       = usbd_adc_ptr[instance];
  ptr_adc_spkr_data = ptr_adc_cfg->spkr_data_ptr;
  ptr_out_cfg       = ptr_adc_cfg->out_cfg;
  device            = ptr_adc_cfg->dev_num;
  ep_iso            = ptr_out_cfg->ep_iso;

  if (ptr_adc_spkr_data->receive_active == 1U) {
    len_received = USBD_DriverEndpointTransferGetResult(device, USB_ENDPOINT_OUT(ep_iso));
    if (len_received != 0U) {
      // Handle received data
      len_till_wrap = ((uint32_t)ptr_out_cfg->data_buf_sz - (uint32_t)ptr_out_cfg->max_packet_size) - ((uint32_t)ptr_adc_spkr_data->ptr_data_received - (uint32_t)ptr_out_cfg->data_buf);
      if (len_received >= len_till_wrap) {      // If data wraps, copy wrapping data to start of buf
        if (len_received > len_till_wrap) {
          memcpy (ptr_out_cfg->data_buf, ptr_out_cfg->data_buf + ptr_out_cfg->data_buf_sz - ptr_out_cfg->max_packet_size, len_received - len_till_wrap);
        }
        ptr_adc_spkr_data->ptr_data_received  = ptr_out_cfg->data_buf + len_received - len_till_wrap;
      } else {
        ptr_adc_spkr_data->ptr_data_received += len_received;
      }
      ptr_adc_spkr_data->data_received_cnt   += len_received;
    }
  }

  // Restart new reception
  if ((ptr_out_cfg->data_buf_sz - ptr_out_cfg->max_packet_size) >= (uint32_t)((ptr_adc_spkr_data->data_received_cnt - ptr_adc_spkr_data->data_read_cnt) + ptr_out_cfg->max_packet_size)) {
    // Restart reception if there is enough space for maximum packet size in buffer
    ptr_adc_spkr_data->receive_active = 1U;
    (void)USBD_DriverEndpointTransfer(device, USB_ENDPOINT_OUT(ep_iso), ptr_adc_spkr_data->ptr_data_received, ptr_out_cfg->max_packet_size);
  } else {
    // Not enough space in buffer
    ptr_adc_spkr_data->receive_active = 0U;
  }

  // Call received callback if receive buffer has filled to half-full
  if ((fpUSBD_ADC_ReceivedSamples[instance] != NULL) && 
      (ptr_adc_spkr_data->reached_zero      != 0U)   && 
     ((uint32_t)(ptr_adc_spkr_data->data_received_cnt - ptr_adc_spkr_data->data_read_cnt) >= (ptr_out_cfg->data_buf_sz / 2U))) {
    ptr_adc_spkr_data->reached_zero = 0U;
    EvrUSBD_ADC_OnReceivedSamples(instance);
    fpUSBD_ADC_ReceivedSamples   [instance] ();
  }
}

/// \brief ADC Isochronous In Endpoint event handling
/// \param[in]     instance      instance of ADC class.
static void USBD_ADC_EpIsoIn (uint8_t instance) {
  const usbd_adc_t          *ptr_adc_cfg;
        usbd_adc_mic_data_t *ptr_adc_mic_data;
        usbd_adc_code_t     *ptr_in_cfg;
        uint32_t             len_sent, len_to_send;
        uint8_t              device, ep_iso;

  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return;
  }

  ptr_adc_cfg      = usbd_adc_ptr[instance];
  ptr_adc_mic_data = ptr_adc_cfg->mic_data_ptr;
  ptr_in_cfg       = ptr_adc_cfg->in_cfg;
  device           = ptr_adc_cfg->dev_num;
  ep_iso           = ptr_in_cfg->ep_iso;

  if (ptr_adc_mic_data->send_active == 1U) {
    len_sent = USBD_DriverEndpointTransferGetResult(device, USB_ENDPOINT_IN(ep_iso));
    if (len_sent != 0U) {
      // Handle sent data
      ptr_adc_mic_data->ptr_data_sent += len_sent;
      // If send pointer wrapped
      if (ptr_adc_mic_data->ptr_data_sent == (ptr_in_cfg->data_buf + ptr_in_cfg->data_buf_sz)) {
        ptr_adc_mic_data->ptr_data_sent = ptr_in_cfg->data_buf;
      }
      ptr_adc_mic_data->data_sent_cnt += len_sent;
    }
  }

  // Number of data bytes to send
  len_to_send = (uint32_t)(ptr_adc_mic_data->data_to_send_cnt - ptr_adc_mic_data->data_sent_cnt);
  if (len_to_send >= ptr_in_cfg->max_packet_size) {
    // If there is more data to be sent then can be sent in a single packet
    // Correct to send maximum packet size
    len_to_send = ptr_in_cfg->max_packet_size;
  }
  if ((ptr_adc_mic_data->ptr_data_sent + len_to_send) > (ptr_in_cfg->data_buf + ptr_in_cfg->data_buf_sz)) {
    len_to_send = ((uint32_t)ptr_in_cfg->data_buf - (uint32_t)ptr_adc_mic_data->ptr_data_sent) + (uint32_t)ptr_in_cfg->data_buf_sz;
  }

  if ((len_to_send != 0U) && (ptr_adc_mic_data->active == 1U)) {
    // Restart transmission if there is data to be sent
    ptr_adc_mic_data->send_active = 1U;
    (void)USBD_DriverEndpointTransfer(device, USB_ENDPOINT_IN(ep_iso), ptr_adc_mic_data->ptr_data_sent, len_to_send);
  } else {
    // No data to send
    ptr_adc_mic_data->send_active = 0U;
  }
}


/************************** Thread functions **********************************/

/// \brief Thread Function: ADC Isochronous Endpoint and Asynchronous events handling
/// \param[in]     arg                  index of ADC instance
void USBD_ADC_Thread (void *arg) {
  uint32_t event;
  uint8_t  instance;

  instance = (uint8_t)((uint32_t)arg);
  if (USBD_ADC_CheckInstance(instance) != usbOK) {
    return;
  }

  for (;;) {
    event = USBD_ThreadFlagsWait (0xFFFFFFFFU);
    if ((event & 0x8000000U) == 0U) {
      if (((event >> 8) & ARM_USBD_EVENT_OUT) != 0U) {
        USBD_ADC_EpIsoOut (instance);
      }
      if (((event >> 8) & ARM_USBD_EVENT_IN)  != 0U) {
        USBD_ADC_EpIsoIn  (instance);
      }
    }
  }
}

void USBD_ADC0_Thread (void const *arg) { (void)arg; USBD_ADC_Thread ((void *)0U); }
void USBD_ADC1_Thread (void const *arg) { (void)arg; USBD_ADC_Thread ((void *)1U); }
void USBD_ADC2_Thread (void const *arg) { (void)arg; USBD_ADC_Thread ((void *)2U); }
void USBD_ADC3_Thread (void const *arg) { (void)arg; USBD_ADC_Thread ((void *)3U); }
