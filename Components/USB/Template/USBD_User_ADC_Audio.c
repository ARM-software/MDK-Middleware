/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device:ADC
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_ADC_Audio_%Instance%.c
 * Purpose: USB Device Audio Device Class (ADC) User module
 *          Template for USB Audio Device Class (ADC) Sound Card
 * Rev.:    V6.3.8
 *----------------------------------------------------------------------------*/
/**
 * \addtogroup usbd_adcFunctions
 *
 * USBD_User_ADC_Audio_%Instance%.c implements the application specific
 * functionality of the Audio class and is used to create an USB Audio device
 * containing mono/stereo speakers and mono/stereo microphone.
 *
 * The implementation uses settings from USBD_Config_ADC_%Instance%.h file.
 *
 */
 
 
//! [code_USBD_User_ADC]
 
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rl_usb.h"
 
#include "USBD_Config_ADC_%Instance%.h"
 
#include "Board_Audio.h"
 
// Imported settings from USBD_Config_ADC_%Instance%.h file
#define  PLAYBACK_AVAILABLE                  (USBD_ADC%Instance%_EP_ISO_OUT_EN != 0U)
#define  PLAYBACK_CHANNELS                   (USBD_ADC%Instance%_OUT_CH_NUM)
#define  PLAYBACK_FREQ                       (USBD_ADC%Instance%_OUT_TSAM_FREQ)
#define  PLAYBACK_RESOLUTION                 (USBD_ADC%Instance%_OUT_BBITRESOLUTION)
#if     (USBD_ADC%Instance%_OUT_BSUBFRAMESIZE >= 3U)
#define  PLAYBACK_SAMPLE_SIZE                (4U)
#else
#define  PLAYBACK_SAMPLE_SIZE                (USBD_ADC%Instance%_OUT_BSUBFRAMESIZE)
#endif
#if     (PLAYBACK_CHANNELS > 1U)
#define  PLAYBACK_FORMAT                     (PLAYBACK_SAMPLE_SIZE | ((PLAYBACK_CHANNELS - 1U) << 7))
#else
#define  PLAYBACK_FORMAT                     (PLAYBACK_SAMPLE_SIZE)
#endif
#define  PLAYBACK_USB_BUFFER_SAMPLES         (USBD_ADC%Instance%_OUT_BUF_SIZE)
#define  PLAYBACK_AUDIO_BUFFER_SAMPLES       (PLAYBACK_USB_BUFFER_SAMPLES / 8U)
#define  PLAYBACK_AUDIO_BUFFER_SIZE         ((PLAYBACK_AUDIO_BUFFER_SAMPLES + PLAYBACK_CHANNELS) * PLAYBACK_SAMPLE_SIZE)
 
#define  RECORD_AVAILABLE                    (USBD_ADC%Instance%_EP_ISO_IN_EN != 0U)
#define  RECORD_CHANNELS                     (USBD_ADC%Instance%_IN_CH_NUM)
#define  RECORD_FREQ                         (USBD_ADC%Instance%_IN_TSAM_FREQ)
#if     (USBD_ADC%Instance%_IN_BSUBFRAMESIZE >= 3U)
#define  RECORD_SAMPLE_SIZE                  (4U)
#else
#define  RECORD_SAMPLE_SIZE                  (USBD_ADC%Instance%_IN_BSUBFRAMESIZE)
#endif
#if     (RECORD_CHANNELS > 1U)
#define  RECORD_FORMAT                       (RECORD_SAMPLE_SIZE | ((RECORD_CHANNELS - 1U) << 7))
#else
#define  RECORD_FORMAT                       (RECORD_SAMPLE_SIZE)
#endif
#define  RECORD_USB_BUFFER_SAMPLES           (USBD_ADC%Instance%_IN_BUF_SIZE)
#define  RECORD_AUDIO_BUFFER_SAMPLES         (RECORD_USB_BUFFER_SAMPLES / 8U)
#define  RECORD_AUDIO_BUFFER_SIZE           ((RECORD_AUDIO_BUFFER_SAMPLES + RECORD_CHANNELS) * RECORD_SAMPLE_SIZE)
 
// Supported configuration check
#if     (PLAYBACK_AVAILABLE)
#if    ((PLAYBACK_CHANNELS == 0U) || (PLAYBACK_CHANNELS > 2U))
#error   Playback channel configuration mono or stereo supported!
#endif
#endif
#if     (RECORD_AVAILABLE)
#if    ((RECORD_CHANNELS == 0U) || (RECORD_CHANNELS > 2U))
#error   Recording channel configuration mono or stereo supported!
#endif
#endif
 
 
#if     (PLAYBACK_AVAILABLE)
static   uint8_t  play_idx = 0U;
static   uint32_t play_num = 0U;
static   uint8_t  play_buf [2][PLAYBACK_AUDIO_BUFFER_SIZE] __attribute__((aligned(4U)));
#endif

#if     (RECORD_AVAILABLE)
static   uint8_t  rec_idx = 0U;
static   uint32_t rec_num = 0U;
static   uint8_t  rec_buf [2][RECORD_AUDIO_BUFFER_SIZE]    __attribute__((aligned(4U)));
#endif
 
#if ((PLAYBACK_AVAILABLE) && (PLAYBACK_RESOLUTION == 24U))
static void Repack_shl_8 (uint8_t *ptr, uint32_t num) {
  uint32_t i;
 
  for (i = 0; i < num; i ++) {
    *(__packed uint32_t *)ptr = ((*(__packed uint32_t *)ptr) << 8U) & 0xFFFFFF00U;
    ptr += 4U;
  }
}
#endif
 
// Audio Events Handling Callback function.
// \param[in]   event  notification mask
static void AudioCallback (uint32_t event) {
  uint32_t samples_available;
#if (PLAYBACK_AVAILABLE)
   int32_t num;
#endif

  // Restart new audio reception and new transmission first
#if (RECORD_AVAILABLE)
  if (event & AUDIO_EVENT_RECEIVE_COMPLETE) {
    // Start new audio data reception (double buffering)
    (void)Audio_ReceiveData ((void *)(&rec_buf[rec_idx ^ 1U][0U]), RECORD_AUDIO_BUFFER_SAMPLES);
  }
#endif
#if (PLAYBACK_AVAILABLE)
  if ((event & AUDIO_EVENT_SEND_COMPLETE) && (play_num != 0U)) {
    // Start new audio data transmission (double buffering)
    (void)Audio_SendData ((void *)(&play_buf[play_idx][0U]), (uint32_t)(play_num));
    play_idx ^= 1U;                     // Change active buffer index
  }
#endif
 
#if (RECORD_AVAILABLE)
  // Handling of recording
  if (event & AUDIO_EVENT_RECEIVE_COMPLETE) {
    // Compensate frequency difference between USB and Audio Interface
    // by adding or removing 1 sample for mono or 2 samples for stereo
    samples_available = USBD_ADC_WrittenSamplesPending (%Instance%U) + rec_num;
    rec_num = RECORD_AUDIO_BUFFER_SAMPLES;
    if (samples_available <= (2U * RECORD_AUDIO_BUFFER_SAMPLES)) {
      // Add (repeat last) 1 sample for mono or 2 samples for stereo
      // to send to USB if USB is faster than Audio Interface
      memcpy((void *)(&rec_buf[rec_idx][ RECORD_AUDIO_BUFFER_SAMPLES                    * RECORD_SAMPLE_SIZE]),
             (void *)(&rec_buf[rec_idx][(RECORD_AUDIO_BUFFER_SAMPLES - RECORD_CHANNELS) * RECORD_SAMPLE_SIZE]),
              RECORD_CHANNELS * RECORD_SAMPLE_SIZE);
      rec_num += RECORD_CHANNELS;
    } else if (samples_available >= (RECORD_USB_BUFFER_SAMPLES - (2U * RECORD_AUDIO_BUFFER_SAMPLES))) {
      // Remove 1 sample for mono or 2 samples for stereo 
      // to send to USB if USB is slower than Audio Interface
      rec_num -= RECORD_CHANNELS;
    }
 
    // Send last received buffer of samples to USB
    (void)USBD_ADC_WriteSamples (%Instance%U, (void *)(&rec_buf[rec_idx][0U]), (int32_t)(rec_num));
    rec_idx ^= 1U;                      // Change active buffer index
  }
#endif
 
#if (PLAYBACK_AVAILABLE)
  // Handling of playback
  if ((event & AUDIO_EVENT_SEND_COMPLETE) && (play_num != 0U)) {
    // Read next buffer of samples from USB
    num = USBD_ADC_ReadSamples (%Instance%U, (void *)(&play_buf[play_idx][0U]), PLAYBACK_AUDIO_BUFFER_SAMPLES);
    if (num > 0) {
      play_num = (uint32_t)(num);
    } else {
      play_num = 0U;
    }
#if (PLAYBACK_RESOLUTION == 24U)
    // If samples need to be repacked form relevant 24-bit to 32-bit
    // for 32-bit Audio Interface sending (playing)
    Repack_shl_8 (&play_buf[play_idx][0U], play_num);
#endif
 
    // Compensate frequency difference between USB and Audio Interface
    // by adding or removing 1 sample for mono or 2 samples for stereo
    samples_available = USBD_ADC_ReceivedSamplesAvailable (%Instance%U) + play_num;
    if (samples_available == 0U) {
      // Stop playback audio stream
      (void)Audio_Stop  (AUDIO_STREAM_OUT);
    } else if (samples_available <= (2U * PLAYBACK_AUDIO_BUFFER_SAMPLES)) {
      // Add (repeat last) 1 sample for mono or 2 samples for stereo
      // to send to Audio Interface if USB is slower than Audio Interface
      memcpy((void *)(&play_buf[play_idx][ play_num                      * PLAYBACK_SAMPLE_SIZE]),
             (void *)(&play_buf[play_idx][(play_num - PLAYBACK_CHANNELS) * PLAYBACK_SAMPLE_SIZE]),
              PLAYBACK_CHANNELS * PLAYBACK_SAMPLE_SIZE);
      play_num += PLAYBACK_CHANNELS;
    } else if (samples_available >= (PLAYBACK_USB_BUFFER_SAMPLES - (2U * PLAYBACK_AUDIO_BUFFER_SAMPLES))) {
      // Remove 1 sample for mono or 2 samples for stereo
      // to send to Audio Interface if USB is faster than Audio Interface
      play_num -= PLAYBACK_CHANNELS;
    }
  }
#endif
}
 
#if (PLAYBACK_AVAILABLE)
// Callback function called when data in USB buffer for speaker samples reaches half-full from empty
void USBD_ADC%Instance%_ReceivedSamples (void) {
  int32_t num;
 
  // Read initial data for playback
  num = USBD_ADC_ReadSamples (%Instance%U, (void *)(&play_buf[0U][0U]), PLAYBACK_AUDIO_BUFFER_SAMPLES);
  if (num > 0) {
    play_num = (uint32_t)(num);
  } else {
    play_num = 0U;
  }
 
#if (PLAYBACK_RESOLUTION == 24U)
  // If samples need to be repacked form relevant 24-bit to 32-bit
  // for 32-bit Audio Interface sending (playing)
  Repack_shl_8 (&play_buf[0U][0U], play_num);
#endif
  // Start initial audio data transmission (play)
  (void)Audio_SendData ((void *)(&play_buf[0U][0U]), play_num);

  // Start playback audio stream
  (void)Audio_Start (AUDIO_STREAM_OUT);

  // Read next data for playback from Audio callback
  play_idx = 1U;
  num = USBD_ADC_ReadSamples (%Instance%U, (void *)(&play_buf[1U][0U]), PLAYBACK_AUDIO_BUFFER_SAMPLES);
  if (num > 0) {
    play_num = (uint32_t)(num);
  } else {
    play_num = 0U;
  }
 
#if (PLAYBACK_RESOLUTION == 24U)
  // If samples need to be repacked form relevant 24-bit to 32-bit
  // for 32-bit Audio Interface sending (playing)
  Repack_shl_8 (&play_buf[1U][0U], play_num);
#endif
}
#endif
 
// Callback function called during USBD_Initialize to initialize the USB ADC class instance.
void USBD_ADC%Instance%_Initialize (void) {
 
  // Initialize Audio Interface driver
  (void)Audio_Initialize    (AudioCallback);
 
#if (PLAYBACK_AVAILABLE)
  // Set stereo speakers volume range to 0 to 100 with step 1, initially positioned at 50 %
  (void)USBD_ADC_SpeakerSetVolumeRange (%Instance%U, 0U, 0U, 100U, 1U, 50U);
 
  // Initialize Audio Interface for speakers according to USBD_Config_ADC_0.h file settings, not muted and volume at 50 %
  (void)Audio_SetDataFormat (AUDIO_STREAM_OUT, PLAYBACK_FORMAT);
  (void)Audio_SetFrequency  (AUDIO_STREAM_OUT, PLAYBACK_FREQ);
  (void)Audio_SetMute       (AUDIO_STREAM_OUT, AUDIO_CHANNEL_MASTER, (bool)false);
  (void)Audio_SetVolume     (AUDIO_STREAM_OUT, AUDIO_CHANNEL_MASTER, 50U);
#endif
 
#if (RECORD_AVAILABLE)
  // Set microphone volume range to 0 to 100 with step 1, initially positioned at 50 %
  (void)USBD_ADC_MicrophoneSetVolumeRange (%Instance%U, 0U, 0U, 100U, 1U, 50U);
 
  // Initialize Audio Interface for microphone according to USBD_Config_ADC_0.h file settings, not muted and volume at 50 %
  (void)Audio_SetDataFormat (AUDIO_STREAM_IN, RECORD_FORMAT);
  (void)Audio_SetFrequency  (AUDIO_STREAM_IN, RECORD_FREQ);
  (void)Audio_SetMute       (AUDIO_STREAM_IN, AUDIO_CHANNEL_MASTER, (bool)false);
  (void)Audio_SetVolume     (AUDIO_STREAM_IN, AUDIO_CHANNEL_MASTER, 50U);
#endif
}
 
// Callback function called during USBD_Uninitialize to de-initialize the USB ADC class instance.
void USBD_ADC%Instance%_Uninitialize (void) {
  (void)Audio_Uninitialize ();
}
 
#if (PLAYBACK_AVAILABLE)
// Callback function called when play stopped.
void USBD_ADC%Instance%_PlayStop (void) {
  (void)Audio_Stop  (AUDIO_STREAM_OUT); // Stop playback audio stream
}
 
// Callback function called when speaker mute setting changed.
// \param[in]     ch            channel index.
//                                - value 0: master channel
//                                - value 1: left speaker (in stereo mode)
//                                - value 2: right speaker (in stereo mode)
// \param[in]     on            current mute setting.
//                                - false :  mute off
//                                - true :   mute on
void USBD_ADC%Instance%_SetSpeakerMute (uint8_t ch, bool on) {
  (void)Audio_SetMute (AUDIO_STREAM_OUT, ch, on);
}
 
// Callback function called when speaker volume setting changed.
// \param[in]     ch            channel index.
//                                - value 0: master channel
//                                - value 1: left speaker (in stereo mode)
//                                - value 2: right speaker (in stereo mode)
// \param[in]     vol           current volume setting.
void USBD_ADC%Instance%_SetSpeakerVolume (uint8_t ch, uint16_t vol) {
  (void)Audio_SetVolume (AUDIO_STREAM_OUT, ch, (uint8_t)(vol));
}
#endif
 
#if (RECORD_AVAILABLE)
// Callback function called when recording started.
void USBD_ADC%Instance%_RecordStart (void) {
  rec_idx = 0U;                         // Current receiving buffer index is 0
  rec_num = RECORD_AUDIO_BUFFER_SAMPLES;// Current receiving length
  // Start initial audio data reception (record)
  (void)Audio_ReceiveData (&rec_buf[0U][0U], RECORD_AUDIO_BUFFER_SAMPLES);
  (void)Audio_Start (AUDIO_STREAM_IN);  // Start recording audio stream
}
 
// Callback function called when recording stopped.
void USBD_ADC%Instance%_RecordStop (void) {
  (void)Audio_Stop  (AUDIO_STREAM_IN);  // Stop recording audio stream
}
 
// Callback function called when microphone mute setting changed.
// \param[in]     ch            channel index.
//                                - value 0: master channel
//                                - value 1: left microphone (in stereo mode)
//                                - value 2: right microphone (in stereo mode)
// \param[in]     on            current mute setting.
//                                - false :  mute off
//                                - true :   mute on
void USBD_ADC%Instance%_SetMicrophoneMute (uint8_t ch, bool on) {
  (void)Audio_SetMute (AUDIO_STREAM_IN, ch, on);
}
 
// Callback function called when microphone volume setting changed.
// \param[in]     ch            channel index.
//                                - value 0: master channel
//                                - value 1: left microphone (in stereo mode)
//                                - value 2: right microphone (in stereo mode)
// \param[in]     vol           current volume setting.
void USBD_ADC%Instance%_SetMicrophoneVolume (uint8_t ch, uint16_t vol) {
  (void)Audio_SetVolume (AUDIO_STREAM_IN, ch, (uint8_t)(vol));
}
#endif
 
//! [code_USBD_User_ADC]
