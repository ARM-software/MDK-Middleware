/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_lib_adc.h
 * Purpose: USB Device - Audio Device Class (ADC) module header file
 *----------------------------------------------------------------------------*/

#ifndef USBD_LIB_ADC_H_
#define USBD_LIB_ADC_H_

#include <stdint.h>
#include <stdbool.h>

/// \brief Audio Class (ADC) get current mute setting for out channel (speaker)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_SpeakerMuteGetCur (uint8_t instance, uint8_t ch);

/// \brief Audio Class (ADC) set current mute setting for out channel (speaker)
///        (calls USBD_ADCn_SpeakerMuteEvent function)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \param[in]     on            mute setting.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_SpeakerMuteSetCur (uint8_t instance, uint8_t ch, bool on);

/// \brief Audio Class (ADC) get current volume setting for out channel (speaker)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_SpeakerVolumeGetCur (uint8_t instance, uint8_t ch);

/// \brief Audio Class (ADC) set current volume setting for out channel (speaker)
///        (calls USBD_ADCn_SpeakerVolumeEvent function)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \param[in]     vol           volume setting.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_SpeakerVolumeSetCur (uint8_t instance, uint8_t ch, uint16_t vol);

/// \brief Audio Class (ADC) get volume minimum for out channel (speaker)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_SpeakerVolumeGetMin (uint8_t instance, uint8_t ch);

/// \brief Audio Class (ADC) get volume maximum for out channel (speaker)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_SpeakerVolumeGetMax (uint8_t instance, uint8_t ch);

/// \brief Audio Class (ADC) get volume resolution for out channel (speaker)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_SpeakerVolumeGetRes (uint8_t instance, uint8_t ch);

/// \brief Audio Class (ADC) get current mute setting for in channel (microphone)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_MicrophoneMuteGetCur (uint8_t instance, uint8_t ch);

/// \brief Audio Class (ADC) set current mute setting for in channel (microphone)
///        (calls USBD_ADCn_MicrophoneMuteEvent function)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \param[in]     on            mute setting.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_MicrophoneMuteSetCur (uint8_t instance, uint8_t ch, bool on);

/// \brief Audio Class (ADC) get current volume setting for in channel (microphone)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_MicrophoneVolumeGetCur (uint8_t instance, uint8_t ch);

/// \brief Audio Class (ADC) set current volume setting for in channel (microphone)
///        (calls USBD_ADCn_MicrophoneVolumeEvent function)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \param[in]     vol           volume setting.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_MicrophoneVolumeSetCur (uint8_t instance, uint8_t ch, uint16_t vol);

/// \brief Audio Class (ADC) get volume minimum for in channel (microphone)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_MicrophoneVolumeGetMin (uint8_t instance, uint8_t ch);

/// \brief Audio Class (ADC) get volume maximum for in channel (microphone)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_MicrophoneVolumeGetMax (uint8_t instance, uint8_t ch);

/// \brief Audio Class (ADC) get volume resolution for in channel (microphone)
/// \param[in]     instance      instance of ADC class.
/// \param[in]     ch            channel.
/// \return        true          success
/// \return        false         fail
extern bool USBD_ADC_MicrophoneVolumeGetRes (uint8_t instance, uint8_t ch);

#endif  // USBD_LIB_ADC_H_
