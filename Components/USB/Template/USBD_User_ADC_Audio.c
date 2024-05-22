/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device:ADC
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_ADC_Audio_%Instance%.c
 * Purpose: USB Device Audio Device Class (ADC) User module
 *          Template for USB Audio Device Class (ADC) Sound Card
 * Rev.:    V6.4.0
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
 
// Audio Events Handling Callback function.
// \param[in]   event  notification mask
static void AudioCallback (uint32_t event) {

  (void)event;

  // Add code to handle audio data sent/received events
  // - start new audio data recording
  // - continue playback of data received over the USB
  // - send previously recorded audio data over the USB

}
 
#if (PLAYBACK_AVAILABLE)
// Callback function called when data in USB buffer for speaker samples reaches half-full from empty
void USBD_ADC%Instance%_ReceivedSamples (void) {

  // Add code to handle data received over USB
  // - start playback of data received over the USB

}
#endif
 
// Callback function called during USBD_Initialize to initialize the USB ADC class instance.
void USBD_ADC%Instance%_Initialize (void) {
 
  // Add code to initialize audio interface
  // - register audio callback function (AudioCallback)

}
 
// Callback function called during USBD_Uninitialize to de-initialize the USB ADC class instance.
void USBD_ADC%Instance%_Uninitialize (void) {

  // Add code to uninitialize audio interface

}
 
#if (PLAYBACK_AVAILABLE)
// Callback function called when play started.
void USBD_ADC%Instance%_PlayStart (void) {

  // Add code to start playback

}

// Callback function called when play stopped.
void USBD_ADC%Instance%_PlayStop (void) {

  // Add code to stop playback

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

  (void)ch;
  (void)on;

  // Add code to mute/unmute speaker

}
 
// Callback function called when speaker volume setting changed.
// \param[in]     ch            channel index.
//                                - value 0: master channel
//                                - value 1: left speaker (in stereo mode)
//                                - value 2: right speaker (in stereo mode)
// \param[in]     vol           current volume setting.
void USBD_ADC%Instance%_SetSpeakerVolume (uint8_t ch, uint16_t vol) {

  (void)ch;
  (void)vol;

  // Add code to set speaker volume

}
#endif
 
#if (RECORD_AVAILABLE)
// Callback function called when recording started.
void USBD_ADC%Instance%_RecordStart (void) {

  // Add code to start recording

}
 
// Callback function called when recording stopped.
void USBD_ADC%Instance%_RecordStop (void) {

  // Add code to stop recording

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

  (void)ch;
  (void)on;

  // Add code to mute/unmute microphone

}
 
// Callback function called when microphone volume setting changed.
// \param[in]     ch            channel index.
//                                - value 0: master channel
//                                - value 1: left microphone (in stereo mode)
//                                - value 2: right microphone (in stereo mode)
// \param[in]     vol           current volume setting.
void USBD_ADC%Instance%_SetMicrophoneVolume (uint8_t ch, uint16_t vol) {

  (void)ch;
  (void)vol;

  // Add code to set microphone volume

}
#endif
 
//! [code_USBD_User_ADC]
