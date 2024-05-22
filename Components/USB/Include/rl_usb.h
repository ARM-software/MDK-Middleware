/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    rl_usb.h
 * Purpose: USB User API
 *----------------------------------------------------------------------------*/

#ifndef RL_USB_H_
#define RL_USB_H_

#include <stdint.h>
#include <stdbool.h>

#include "RTE_Components.h"

#define MW_USB_VERSION_MAJOR           (8U)
#define MW_USB_VERSION_MINOR           (0U)
#define MW_USB_VERSION_PATCH           (0U)
#define MW_USB_VERSION                ((MW_USB_VERSION_MAJOR * 10000000U) + \
                                       (MW_USB_VERSION_MINOR * 10000U)    + \
                                       (MW_USB_VERSION_PATCH))

#include "usb_def.h"
#include "usb_adc.h"
#include "usb_cdc.h"
#include "usb_cdc_acm_rndis.h"
#include "usb_hid.h"
#include "usb_msc.h"
#include "usb_ms.h"

#ifdef    RTE_CMSIS_RTOS2
#include "cmsis_os2.h"
#endif
#ifdef    RTE_CMSIS_RTOS2_RTX5
#include "rtx_os.h"
#endif

#ifndef   MW_USB_DISABLE_DEPRECATED

// Compiler macros
#define   USB_SECTION_NAME_(x)          __attribute__((section(#x)))
#define   __SECTION(x)                  USB_SECTION_NAME_(x)

// RTOS macros
#ifdef    RTE_CMSIS_RTOS2
#define   USB_CMSIS_RTOS2              (1U)
#endif
#ifdef    RTE_CMSIS_RTOS2_RTX5
#define   USB_CMSIS_RTOS2_RTX5         (1U)
#endif

// Endianess macros
#ifdef   __ARM_BIG_ENDIAN
#ifndef   U32_LE
#define   U32_LE(v)  (uint32_t)(__REV(v))
#endif
#ifndef   U16_LE
#define   U16_LE(v)  (uint16_t)(__REV(v) >> 16)
#endif
#ifndef   U32_BE
#define   U32_BE(v)  (uint32_t)(v)
#endif
#ifndef   U16_BE
#define   U16_BE(v)  (uint16_t)(v)
#endif
#else
#ifndef   U32_BE
#define   U32_BE(v)  (uint32_t)(__REV(v))
#endif
#ifndef   U16_BE
#define   U16_BE(v)  (uint16_t)(__REV(v) >> 16)
#endif
#ifndef   U32_LE
#define   U32_LE(v)  (uint32_t)(v)
#endif
#ifndef   U16_LE
#define   U16_LE(v)  (uint16_t)(v)
#endif
#endif
#endif

#ifdef __cplusplus
extern "C"  {
#endif

//  ==== USB Constants and Defines ====

/// Status code values returned by USB library functions.
typedef enum {
  usbOK                       =    0U,  ///< Function completed with no error
  usbTimeout,                           ///< Function completed; time-out occurred
  usbInvalidParameter,                  ///< Invalid Parameter error: a mandatory parameter was missing or specified an incorrect object
  usbThreadError              = 0x10U,  ///< CMSIS-RTOS Thread creation/termination failed
  usbTimerError,                        ///< CMSIS-RTOS Timer creation/deletion failed
  usbSemaphoreError,                    ///< CMSIS-RTOS Semaphore creation failed
  usbMutexError,                        ///< CMSIS-RTOS Mutex creation failed
  usbControllerError          = 0x20U,  ///< Controller does not exist
  usbDeviceError,                       ///< Device does not exist
  usbDriverError,                       ///< Driver function produced error
  usbDriverBusy,                        ///< Driver function is busy
  usbMemoryError,                       ///< Memory management function produced error
  usbNotConfigured,                     ///< Device is not configured (is connected)
  usbClassErrorADC,                     ///< Audio Device Class (ADC) error (no device or device produced error)
  usbClassErrorCDC,                     ///< Communication Device Class (CDC) error (no device or device produced error)
  usbClassErrorHID,                     ///< Human Interface Device (HID) error (no device or device produced error)
  usbClassErrorMSC,                     ///< Mass Storage Device (MSC) error (no device or device produced error)
  usbClassErrorCustom,                  ///< Custom device Class (Class) error (no device or device produced error)
  usbUnsupportedClass,                  ///< Unsupported Class
  usbTransferStall            = 0x40U,  ///< Transfer handshake was stall
  usbTransferError,                     ///< Transfer error or transfer aborted
  usbUnknownError             = 0xFFU   ///< Unspecified USB error
} usbStatus;

//  ==== USB Device Constants and Defines ====

/// USB Device Custom Class API enumerated constants
typedef enum {
  usbdRequestNotProcessed = 0U,         ///< Request not processed
  usbdRequestOK,                        ///< Request processed and OK
  usbdRequestStall,                     ///< Request processed but unsupported
  usbdRequestNAK                        ///< Request processed but busy
} usbdRequestStatus;

/// USB Device MSC Check Media bit masks
#define USBD_MSC_MEDIA_READY     (1UL)          ///< Media Ready
#define USBD_MSC_MEDIA_PROTECTED (1UL<<1)       ///< Media Write Protected

/// USB Device HID Class API enumerated constants
typedef enum {
  USBD_HID_REQ_EP_CTRL = 0U,            ///< Request from control endpoint
  USBD_HID_REQ_EP_INT,                  ///< Request from interrupt endpoint
  USBD_HID_REQ_PERIOD_UPDATE            ///< Request from periodic update
} USBD_HID_REQ_t;

/// USB Device thread enumerator
typedef enum {
  usbdThreadCore  =  0U,
  usbdThreadCC,
  usbdThreadADC,
  usbdThreadCDC_Int,
  usbdThreadCDC_Bulk,
  usbdThreadHID,
  usbdThreadMSC
} usbdThread_t;

/// USB Device semaphore enumerator
typedef enum {
  usbdSemaphoreCore  =  0U,
  usbdSemaphoreEndpoint,
  usbdSemaphoreHID
} usbdSemaphore_t;

/* USB Device State structure */
/// @cond USBD_STATE_cond
typedef struct {
  uint32_t vbus     :  1;               ///< USB Device VBUS state
  uint32_t speed    :  2;               ///< USB Device enumerated speed (USB_SPEED_LOW, USB_SPEED_FULL or USB_SPEED_HIGH)
  uint32_t active   :  1;               ///< USB Device bus activity
  uint32_t reserved : 28;               ///< reserved
} USBD_STATE;
/// @endcond

/* USB Host Constants and Defines */

/// USB Host Notification enumerated constants
typedef enum {
  USBH_NOTIFY_CONNECT = 0U,             ///< Port connection happened
  USBH_NOTIFY_DISCONNECT,               ///< Port disconnection happened
  USBH_NOTIFY_OVERCURRENT,              ///< Port overcurrent happened
  USBH_NOTIFY_REMOTE_WAKEUP,            ///< Port remote wakeup signaling happened
  USBH_NOTIFY_READY,                    ///< Device was successfully enumerated, initialized and is ready for communication
  USBH_NOTIFY_UNKNOWN_DEVICE,           ///< Device was successfully enumerated but there is no driver for it
  USBH_NOTIFY_INSUFFICIENT_POWER,       ///< Device requires more power consumption than available
  USBH_NOTIFY_CONFIGURATION_FAILED,     ///< Device was not successfully configured (not enough resources)
  USBH_NOTIFY_INITIALIZATION_FAILED     ///< Device was not successfully initialized
} USBH_NOTIFY;

/* USB Host Pipe handle type */
typedef uint32_t USBH_PIPE_HANDLE;

/// Pointer to USB Host Pipe Event callback function
typedef void (*USBH_PipeEvent_t) (uint32_t event, const uint8_t *buf, uint32_t len);

/// USB Host thread enumerator
typedef enum {
  usbhThreadCore         =  0U,
  usbhThreadCDC,
  usbhThreadHID
} usbhThread_t;

/// USB Host mutex enumerator
typedef enum {
  usbhMutexCore          =  0U
} usbhMutex_t;

/// USB Host semaphore enumerator
typedef enum {
  usbhSemaphoreCore      =  0U
} usbhSemaphore_t;

/// Mouse State structure
typedef struct {
  uint8_t reserved;                     ///< Reserved byte (for alignment)
  uint8_t button;                       ///< Current button states
  int16_t x;                            ///< Absolute X position change
  int16_t y;                            ///< Absolute Y position change
} usbHID_MouseState;


//  ==== USB Device Functions ====

/// \brief Get version of USB Device stack
/// \return                             version (major.minor.patch : mmnnnpppp decimal)
extern uint32_t USBD_GetVersion (void);

/// \brief Initialize USB Device stack and controller
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_Initialize (uint8_t device);

/// \brief De-initialize USB Device stack and controller
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_Uninitialize (uint8_t device);

/// \brief Activate pull-up on D+ or D- line to signal USB Device connection on USB Bus
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_Connect (uint8_t device);

/// \brief Disconnect USB Device from USB Bus
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_Disconnect (uint8_t device);

/// \brief Retrieve USB Device configuration status
/// \param[in]     device               index of USB Device.
/// \return                             device is in configured state and ready to communicate or is not configured and not ready to communicate :
///                                       - value true :  device is in configured state and ready to communicate
///                                       - value false : device is not configured and not ready to communicate
extern bool USBD_Configured (uint8_t device);

/// \brief Retrieve USB Device state
/// \param[in]     device               index of USB Device.
/// \return                             structure containing device state information.
extern USBD_STATE USBD_GetState (uint8_t device);

/// \brief Set USB Device serial number string
/// \param[in]     device               index of USB Device.
/// \param[in]     string               serial number string.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_SetSerialNumber (uint8_t device, const char *string);

#ifdef __DOXYGEN__

// following functions are available for each instance of a Device.
// generic prefix USBD_Devicen is USBD_Device0 for Device instance 0.

/// \brief Callback function called during \ref USBD_Initialize to initialize the USB Device
void USBD_Devicen_Initialize (void);
 
/// \brief Callback function called during \ref USBD_Uninitialize to de-initialize the USB Device
void USBD_Devicen_Uninitialize (void);
 
/// \brief Callback function called when VBUS level changes
/// \param[in]     level                current VBUS level :
///                                       - value true :  VBUS level is high
///                                       - value false : VBUS level is low
void USBD_Devicen_VbusChanged (bool level);
 
/// \brief Callback function called upon USB Bus Reset signaling
void USBD_Devicen_Reset (void);
 
/// \brief Callback function called when USB Bus speed has changed to high-speed
void USBD_Devicen_HighSpeedActivated (void);

/// \brief Callback function called when USB Bus goes into suspend mode (no bus activity for 3 ms)
void USBD_Devicen_Suspended (void);

/// \brief Callback function called when USB Bus activity has resumed
void USBD_Devicen_Resumed (void);

/// \brief Callback function called when Device configuration has changed
/// \param[in]     val                  current configuration value :
///                                       - value 0 :   not configured
///                                       - value > 0 : active configuration
void USBD_Devicen_ConfigurationChanged (uint8_t val);

/// \brief Callback function called when Set Feature for Remote Wakeup was requested over Control Endpoint 0
void USBD_Devicen_EnableRemoteWakeup (void);

/// \brief Callback function called when Clear Feature for Remote Wakeup was requested over Control Endpoint 0
void USBD_Devicen_DisableRemoteWakeup (void);
 
/// \brief Callback function called when Device received SETUP PACKET on Control Endpoint 0
/// \param[in]     setup_packet         pointer to received setup packet.
/// \param[in,out] buf                  pointer to data buffer used for data stage requested by setup packet.
/// \param[in,out] len                  pointer to number of data bytes in data stage requested by setup packet.
/// \return        usbdRequestStatus    enumerator value indicating the function execution status :
///                                       - value usbdRequestNotProcessed : request was not processed; processing will be done by USB library
///                                       - value usbdRequestOK :           request was processed successfully (send Zero-Length Packet if no data stage)
///                                       - value usbdRequestStall :        request was processed but is not supported (stall Endpoint 0)
usbdRequestStatus USBD_Devicen_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);

/// \brief Callback function called when SETUP PACKET was processed by USB library
/// \param[in]     setup_packet         pointer to processed setup packet.
void USBD_Devicen_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet);

/// \brief Callback function called when Device received OUT DATA on Control Endpoint 0
/// \param[in]     len                  number of received data bytes.
/// \return        usbdRequestStatus    enumerator value indicating the function execution status :
///                                       - value usbdRequestNotProcessed : request was not processed; processing will be done by USB library
///                                       - value usbdRequestOK :           request was processed successfully (send Zero-Length Packet)
///                                       - value usbdRequestStall :        request was processed but is not supported (stall Endpoint 0)
///                                       - value usbdRequestNAK :          request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_Devicen_Endpoint0_OutDataReceived (uint32_t len);

/// \brief Callback function called when Device sent IN DATA on Control Endpoint 0
/// \param[in]     len                  number of sent data bytes.
/// \return        usbdRequestStatus    enumerator value indicating the function execution status :
///                                       - value usbdRequestNotProcessed : request was not processed; processing will be done by USB library
///                                       - value usbdRequestOK :           request was processed successfully (return ACK)
///                                       - value usbdRequestStall :        request was processed but is not supported (stall Endpoint 0)
///                                       - value usbdRequestNAK :          request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_Devicen_Endpoint0_InDataSent (uint32_t len);

#endif // __DOXYGEN

//  ==== USB Device Audio Device Functions ====

#ifdef __DOXYGEN__

// following functions are available for each instance of a ADC class.
// generic prefix USBD_ADCn is USBD_ADC0 for ADC class instance 0.

/// \brief Callback function called during \ref USBD_Initialize to initialize the USB ADC class instance
void USBD_ADCn_Initialize (void);

/// \brief Callback function called during \ref USBD_Uninitialize to de-initialize the USB ADC class instance
void USBD_ADCn_Uninitialize (void);

/// \brief Callback function called when play started
void USBD_ADCn_PlayStart (void);

/// \brief Callback function called when play stopped
void USBD_ADCn_PlayStop (void);

/// \brief Callback function called when speaker mute setting changed
/// \param[in]     ch                   channel index :
///                                       - value 0 : master channel
///                                       - value 1 : left speaker (in stereo mode)
///                                       - value 2 : right speaker (in stereo mode)
/// \param[in]     on                   current mute setting :
///                                       - value false : mute off
///                                       - value true :  mute on
void USBD_ADCn_SetSpeakerMute (uint8_t ch, bool on);

/// \brief Callback function called when speaker volume setting changed
/// \param[in]     ch                   channel index :
///                                       - value 0 : master channel
///                                       - value 1 : left speaker (in stereo mode)
///                                       - value 2 : right speaker (in stereo mode)
/// \param[in]     vol                  current volume setting.
void USBD_ADCn_SetSpeakerVolume (uint8_t ch, uint16_t vol);

/// \brief Callback function called when recording started
void USBD_ADCn_RecordStart (void);

/// \brief Callback function called when recording stopped
void USBD_ADCn_RecordStop (void);

/// \brief Callback function called when microphone mute setting changed
/// \param[in]     ch                   channel index :
///                                       - value 0 : master channel
///                                       - value 1 : left microphone (in stereo mode)
///                                       - value 2 : right microphone (in stereo mode)
/// \param[in]     on                   current mute setting
///                                       - value false : mute off
///                                       - value true :  mute on
void USBD_ADCn_SetMicrophoneMute (uint8_t ch, bool on);

/// \brief Callback function called when microphone volume setting changed
/// \param[in]     ch                   channel index :
///                                       - value 0 : master channel
///                                       - value 1 : left microphone (in stereo mode)
///                                       - value 2 : right microphone (in stereo mode)
/// \param[in]     vol                  current volume setting.
void USBD_ADCn_SetMicrophoneVolume (uint8_t ch, uint16_t vol);

/// \brief Callback function called when data in USB buffer for speaker samples reaches half-full from empty
void USBD_ADCn_ReceivedSamples (void);

#endif // __DOXYGEN

// Rename deprecated functions
#define USBD_ADC_SpeakerSetVolumeRange      USBD_ADC_SetSpeakerVolumeRange
#define USBD_ADC_MicrophoneSetVolumeRange   USBD_ADC_SetMicrophoneVolumeRange

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
extern usbStatus USBD_ADC_SetSpeakerVolumeRange (uint8_t instance, uint8_t ch, uint16_t min, uint16_t max, uint16_t res, uint16_t cur);

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
extern usbStatus USBD_ADC_SetMicrophoneVolumeRange (uint8_t instance, uint8_t ch, uint16_t min, uint16_t max, uint16_t res, uint16_t cur);

/// \brief Number of audio samples received from USB Host and available to be read
/// \param[in]     instance             instance of ADC class.
/// \return                             number of samples available to be read.
extern uint32_t USBD_ADC_ReceivedSamplesAvailable (uint8_t instance);

/// \brief Number of audio samples written and pending to be sent to USB Host
/// \param[in]     instance             instance of ADC class.
/// \return                             number of samples ready to be sent.
extern uint32_t USBD_ADC_WrittenSamplesPending (uint8_t instance);

/// \brief Read audio samples received from USB Host
/// \param[in]     instance             instance of ADC class.
/// \param[out]    buf                  buffer that receives samples.
/// \param[in]     num                  maximum number of samples to read.
/// \return                             number of samples read or execution status :
///                                       - value >= 0 : number of samples read
///                                       - value < 0 :  error occurred, -value is execution status as defined with \ref usbStatus
extern int32_t USBD_ADC_ReadSamples (uint8_t instance, void *buf, int32_t num);

/// \brief Write audio samples to be transferred to USB Host
/// \param[in]     instance             instance of ADC class.
/// \param[in]     buf                  buffer containing samples to write.
/// \param[in]     num                  number of samples to write.
/// \return                             number of samples written or execution status :
///                                       - value >= 0 : number of samples written for sending
///                                       - value < 0 :  error occurred, -value is execution status as defined with \ref usbStatus
extern int32_t USBD_ADC_WriteSamples (uint8_t instance, const void *buf, int32_t num);

//  ==== USB Device Communication Device (Abstract Control Model) Functions ====

#ifdef __DOXYGEN__

// following functions are available for each instance of a CDC class.
// generic prefix USBD_CDCn is USBD_CDC0 for CDC class instance 0.

/// \brief Callback function called during \ref USBD_Initialize to initialize the USB CDC class instance (ACM)
void USBD_CDCn_ACM_Initialize (void);

/// \brief Callback function called during \ref USBD_Uninitialize to de-initialize the USB CDC class instance (ACM)
void USBD_CDCn_ACM_Uninitialize (void);

/// \brief Callback function called upon USB Bus Reset signaling
void USBD_CDCn_ACM_Reset (void);

/// \brief Callback function called upon reception of request send encapsulated command sent by the USB Host.
/// \param[in]     buf                  buffer that contains send encapsulated command request.
/// \param[in]     len                  length of send encapsulated command request.
/// \return                             received send encapsulated command processed status :
///                                       - value true :  processed
///                                       - value false : not processed
bool USBD_CDCn_ACM_SendEncapsulatedCommand (const uint8_t *buf, uint16_t len);

/// \brief Callback function called upon reception of request to get encapsulated response sent by the USB Host
/// \param[in]     max_len              maximum number of data bytes that USB Host expects to receive.
/// \param[out]    buf                  pointer to buffer containing get encapsulated response to be returned to USB Host.
/// \param[out]    len                  pointer to number of data bytes to be returned to USB Host.
/// \return                             received get encapsulated response processed status :
///                                       - value true :  processed and response prepared
///                                       - value false : not processed
bool USBD_CDCn_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len);

/// \brief Callback function called upon USB Host request to change communication settings
/// \param[in]     line_coding          pointer to CDC_LINE_CODING structure.
/// \return                             set line coding request processed status :
///                                       - value true :  processed
///                                       - value false : not processed
bool USBD_CDCn_ACM_SetLineCoding (const CDC_LINE_CODING *line_coding);

/// \brief Callback function called upon USB Host request to retrieve communication settings
/// \param[out]    line_coding          pointer to CDC_LINE_CODING structure.
/// \return                             get line coding request processed status :
///                                       - value true :  processed
///                                       - value false : not processed
bool USBD_CDCn_ACM_GetLineCoding (CDC_LINE_CODING *line_coding);

/// \brief Callback function called upon USB Host request to set control line states
/// \param[in]     state                control line settings bitmap :
///                                       - bit 0 : DTR state
///                                       - bit 1 : RTS state
/// \return                             set control line state request processed status :
///                                       - value true :  processed
///                                       - value false : not processed
bool USBD_CDCn_ACM_SetControlLineState (uint16_t state);

/// \brief Callback function called when new data was received
/// \param[in]     len                  number of bytes available to read.
void USBD_CDCn_ACM_DataReceived (uint32_t len);

/// \brief Callback function called when all data was sent
void USBD_CDCn_ACM_DataSent (void);

/// \brief Callback function called during USBD_Initialize to initialize the USB CDC class instance (NCM)
void USBD_CDCn_NCM_Initialize (void);

/// \brief Callback function called during USBD_Uninitialize to de-initialize the USB CDC class instance (NCM)
void USBD_CDCn_NCM_Uninitialize (void);

/// \brief Callback function called upon USB Bus Reset signaling
void USBD_CDCn_NCM_Reset (void);

/// \brief Callback function called when USB Host changes data interface from alternate 0 to alternate 1 (activates data interface)
void USBD_CDCn_NCM_Start (void);

/// \brief Callback function called when USB Host changes data interface from alternate 1 to alternate 0 (de-activates data interface)
void USBD_CDCn_NCM_Stop (void);

/// \brief Callback function called upon USB Host request to set the Ethernet device multicast filters
/// \param[in]     addr_list            Pointer to list of 48-bit Multicast addresses.
/// \param[in]     num_of_filters       Number of filters.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_SetEthernetMulticastFilters (const uint8_t *addr_list, uint16_t num_of_filters);

/// \brief Callback function called upon USB Host request to set up the specified Ethernet power management pattern filter
/// \param[in]     filter_number        Filter number.
/// \param[in]     pattern_filter       Power management pattern filter structure.
/// \param[in]     pattern_filter_size  Size of pattern filter structure.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size);

/// \brief Callback function called upon USB Host request to retrieve the status of the specified Ethernet power management pattern filter
/// \param[in]     filter_number        Filter number.
/// \param[out]    pattern_active       Pointer to pattern active boolean.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active);

/// \brief Callback function called upon USB Host request to configure device Ethernet packet filter settings
/// \param[in]     packet_filter_bitmap Packet Filter Bitmap.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_SetEthernetPacketFilter (uint16_t packet_filter_bitmap);

/// \brief Callback function called upon USB Host request to retrieve a statistic based on the feature selector
/// \param[in]     feature_selector     Feature Selector.
/// \param[out]    data                 Pointer to Statistics Value.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_GetEthernetStatistic (uint16_t feature_selector, uint32_t *data);

/// \brief Callback function called upon USB Host request to retrieve the parameters that describe NTBs for each direction
/// \param[out]    ntb_params           Pointer to NTB Parameter Structure.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_GetNtbParameters (CDC_NCM_NTB_PARAM *ntb_params);

/// \brief Callback function called upon USB Host request to return the USB Device's current EUI-48 station address
/// \param[out]    net_addr             Pointer to EUI-48 current address, in network byte order.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_GetNetAddress (uint8_t *net_addr);

/// \brief Callback function called upon USB Host request to set the USB Device's current EUI-48 station address
/// \param[in]     net_addr             Pointer to EUI-48 address, in network byte order.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_SetNetAddress (const uint8_t *net_addr);

/// \brief Callback function called upon USB Host request to return the NTB data format currently being used
/// \param[out]    ntb_format           Pointer to NTB format code.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_GetNtbFormat (uint16_t *ntb_format);

/// \brief Callback function called upon USB Host request to select the format of NTB to be used for NTBs transmitted to the USB Host
/// \param[in]     ntb_format           NTB format selection :
///                                       - value 0 : NTB-16
///                                       - value 1 : NTB-32
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_SetNtbFormat (uint16_t ntb_format);

/// \brief Callback function called upon USB Host request to return NTB input size currently being used
/// \param[out]    ntb_input_size       Pointer to NTB input size.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_GetNtbInputSize (uint32_t *ntb_input_size);

/// \brief Callback function called upon USB Host request to select the maximum size of NTB that is permitted to be sent to the USB Host
/// \param[in]     ntb_input_size       Maximum NTB size.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_SetNtbInputSize (uint32_t ntb_input_size);

/// \brief Callback function called upon USB Host request to return the currently effective maximum datagram size
/// \param[out]    max_datagram_size    Pointer to current maximum datagram size.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_GetMaxDatagramSize (uint16_t *max_datagram_size);

/// \brief Callback function called upon USB Host request to select the maximum datagram size that can be sent in an NTB
/// \param[in]     max_datagram_size    Maximum datagram size.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_SetMaxDatagramSize (uint16_t max_datagram_size);

/// \brief Callback function called upon USB Host request to return the currently selected CRC mode
/// \param[out]    crc_mode             Pointer to current CRC mode.
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_GetCrcMode (uint16_t *crc_mode);

/// \brief Callback function called upon USB Host request to control CRC mode
/// \param[in]     crc_mode             CRC mode :
///                                       - value 0 : CRCs shall not be appended
///                                       - value 1 : CRCs shall be appended
/// \return                             request handled status :
///                                       - value true :  request was handled
///                                       - value false : request was not handled
bool USBD_CDCn_NCM_SetCrcMode (uint16_t crc_mode);

/// \brief Callback function called when NTB was successfully sent
void USBD_CDCn_NCM_NTB_IN_Sent (void);

/// \brief Callback function called when NTB was successfully received
void USBD_CDCn_NCM_NTB_OUT_Received (void);

#endif // __DOXYGEN

// Rename deprecated functions
#define USBD_CDC_ACM_Notify             USBD_CDC_ACM_Notify_SerialState

/// \brief Read one character received by Communication Device from USB Host
/// \param[in]     instance             instance of CDC class.
/// \return                             value of read character or no character received :
///                                       - value >= 0 : value of first received unread character
///                                       - value -1 :   indicates no character was received
extern int USBD_CDC_ACM_GetChar (uint8_t instance);

/// \brief Write a single character from Communication Device to USB Host
/// \param[in]     instance             instance of CDC class.
/// \param[in]     ch                   character to write.
/// \return                             value of accepted character or no character accepted :
///                                       - value ch : if character accepted for writing
///                                       - value -1 : indicates character not accepted
extern int USBD_CDC_ACM_PutChar (uint8_t instance, int ch);

/// \brief Read multiple data bytes received by Communication Device from USB Host
/// \param[in]     instance             instance of CDC class.
/// \param[out]    buf                  buffer that receives data.
/// \param[in]     len                  maximum number of bytes to read.
/// \return                             number of bytes read or execution status :
///                                       - value >= 0 : number of bytes read
///                                       - value < 0 :  error occurred, -value is execution status as defined with \ref usbStatus
extern int32_t USBD_CDC_ACM_ReadData (uint8_t instance, uint8_t *buf, int32_t len);

/// \brief Write data from Communication Device to USB Host
/// \param[in]     instance             instance of CDC class.
/// \param[in]     buf                  buffer containing data bytes to write.
/// \param[in]     len                  number of bytes to write.
/// \return                             number of bytes accepted for writing or execution status :
///                                       - value >= 0 : number of bytes accepted for writing
///                                       - value < 0 :  error occurred, -value is execution status as defined with \ref usbStatus
extern int32_t USBD_CDC_ACM_WriteData (uint8_t instance, const uint8_t *buf, int32_t len);

/// \brief Retrieve number of data bytes received by Communication Device from
///        USB Host that are available to read
/// \param[in]     instance             instance of CDC class.
/// \return                             number of bytes available to read.
extern int32_t USBD_CDC_ACM_DataAvailable (uint8_t instance);

/// \brief Send notification of Communication Device status and line states to USB Host
/// \param[in]     instance             instance of CDC class.
/// \param[in]     state                error status and line states :
///                                       - bit 6 : bOverRun
///                                       - bit 5 : bParity
///                                       - bit 4 : bFraming
///                                       - bit 3 : bRingSignal
///                                       - bit 2 : bBreak
///                                       - bit 1 : bTxCarrier (DSR line state)
///                                       - bit 0 : bRxCarrier (DCD line state)
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_CDC_ACM_Notify_SerialState (uint8_t instance, uint16_t state);

/// \brief Send notification to USB Host signaling that response is available
/// \param[in]     instance             instance of CDC class.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_CDC_ACM_Notify_ResponseAvailable (uint8_t instance);

/// \brief Clear the active NTB (prepared for sending)
/// \param[in]     instance             Instance of CDC class.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
extern int32_t USBD_CDC_NCM_NTB_IN_Initialize (uint8_t instance);

/// \brief Create a new NDP in the NTB (datagrams can be added to it)
/// \param[in]     instance             Instance of CDC class.
/// \param[in]     num_datagrams        Maximum number of datagrams that NDP will contain.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
extern int32_t USBD_CDC_NCM_NTB_IN_CreateNDP (uint8_t instance, uint8_t num_datagrams);

/// \brief Add a datagram into the active NDP of the NTB to be sent
/// \param[in]     instance             Instance of CDC class.
/// \param[in]     buf                  Buffer containing data bytes to write.
/// \param[in]     len                  Number of bytes to write.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
extern int32_t USBD_CDC_NCM_NTB_IN_WriteDatagram (uint8_t instance, const uint8_t *buf, uint32_t len);

/// \brief Send the active NTB
/// \param[in]     instance             Instance of CDC class.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
extern int32_t USBD_CDC_NCM_NTB_IN_Send (uint8_t instance);

/// \brief Check if NTB was sent and new NTB can be prepared
/// \param[in]     instance             Instance of CDC class.
/// \return                             NTB sent status or error code :
///                                       - value 1 :   NTB was sent
///                                       - value 0 :   NTB sending is in progress
///                                       - value < 0 : error code
extern int32_t USBD_CDC_NCM_NTB_IN_IsSent (uint8_t instance);

/// \brief Check if NTB was received and is available for processing
/// \param[in]     instance             Instance of CDC class.
/// \return                             NTB available or error code :
///                                       - value 1 :   received NTB is available
///                                       - value 0 :   no received NTB is available
///                                       - value < 0 : error code
extern int32_t USBD_CDC_NCM_NTB_OUT_IsReceived (uint8_t instance);

/// \brief Flush the received NTB and release memory for reception of a new NTB
/// \param[in]     instance             Instance of CDC class.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
extern int32_t USBD_CDC_NCM_NTB_OUT_Release (uint8_t instance);

/// \brief Process the next NDP in the received NTB
/// \param[in]     instance             Instance of CDC class.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
extern int32_t USBD_CDC_NCM_NTB_OUT_ProcessNDP (uint8_t instance);

/// \brief Get size of a datagram from the active NDP of the received NTB
/// \param[in]     instance             Instance of CDC class.
/// \return                             Number of bytes available in the datagram.
extern uint32_t USBD_CDC_NCM_NTB_OUT_GetDatagramSize (uint8_t instance);

/// \brief Read a datagram from the active NDP of the received NTB
/// \param[in]     instance             Instance of CDC class.
/// \param[out]    buf                  Buffer that receives read data.
/// \param[in]     max_len              Maximum number of bytes to read.
/// \return                             number of bytes read from the datagram or error code :
///                                       - value >= 0 : number of bytes read from the datagram
///                                       - value <  0 : error code
extern int32_t USBD_CDC_NCM_NTB_OUT_ReadDatagram (uint8_t instance, uint8_t *buf, uint32_t max_len);

/// \brief Send already prepared NTB (for this option value of define in configuration USBD_CDCn_NCM_RAW_ENABLE has to be 1)
/// \param[in]     instance             Instance of CDC class.
/// \param[in]     buf                  Buffer containing NTB.
/// \param[in]     len                  Size of NTB.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
extern int32_t USBD_CDC_NCM_NTB_IN_RawSend (uint8_t instance, const uint8_t *buf, uint32_t len);

/// \brief Get size of the received NTB
/// \param[in]     instance             Instance of CDC class.
/// \return                             Number of bytes available in the NTB.
extern uint32_t USBD_CDC_NCM_NTB_OUT_RawGetSize (uint8_t instance);

/// \brief Receive an NDP (for this option value of define in configuration USBD_CDCn_NCM_RAW_ENABLE has to be 1)
/// \param[in]     instance             Instance of CDC class.
/// \param[out]    buf                  Buffer that receives NTB.
/// \param[in]     max_len              Maximum number of bytes that buffer can accept.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
extern int32_t USBD_CDC_NCM_NTB_OUT_RawReceive (uint8_t instance, uint8_t *buf, uint32_t max_len);

/// \brief Report whether or not the physical layer (modem, Ethernet PHY, etc.) link is up to the USB Host
/// \param[in]     instance             Instance of CDC class.
/// \param[in]     status               Connection status :
///                                       - value 0 : Disconnected
///                                       - value 1 : Connected
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
extern int32_t USBD_CDC_NCM_Notify_NetworkConnection (uint8_t instance, uint16_t status);

/// \brief Report a change in upstream or downstream speed of the networking connection to the USB Host
/// \param[in]     instance             Instance of CDC class.
/// \param[in]     us_bitrate           Upstream bitrate.
/// \param[in]     ds_bitrate           Downstream bitrate.
/// \return                             function execution status :
///                                       - value 0 :   success
///                                       - value < 0 : error code
extern int32_t USBD_CDC_NCM_Notify_ConnectionSpeedChange (uint8_t instance, uint32_t us_bitrate, uint32_t ds_bitrate);

//  ==== USB Device Human Interface Device Functions ====

#ifdef __DOXYGEN__

// following functions are available for each instance of a HID class.
// generic prefix USBD_HIDn is USBD_HID0 for HID class instance 0.

/// \brief Callback function called during \ref USBD_Initialize to initialize the USB HID class instance
void USBD_HIDn_Initialize (void);

/// \brief Callback function called during \ref USBD_Uninitialize to de-initialize the USB HID class instance
void USBD_HIDn_Uninitialize (void);

/// \brief Callback function called to prepare HID Report data to send
/// \param[in]     rtype                report type :
///                                       - HID_REPORT_INPUT   = input report requested
///                                       - HID_REPORT_FEATURE = feature report requested
/// \param[in]     req                  request type :
///                                       - USBD_HID_REQ_EP_CTRL       = control endpoint request
///                                       - USBD_HID_REQ_PERIOD_UPDATE = idle period expiration request
///                                       - USBD_HID_REQ_EP_INT        = previously sent report on interrupt endpoint request
/// \param[in]     rid                  report ID (0 if only one report exists).
/// \param[out]    buf                  buffer containing report data to send.
/// \return                             number of report data bytes prepared to send or invalid report requested :
///                                       - value >= 0 : number of report data bytes prepared to send
///                                       - value -1 :   invalid report requested
int32_t USBD_HIDn_GetReport (uint8_t rtype, uint8_t req, uint8_t rid, uint8_t *buf);

/// \brief Callback function called to process received HID Report data
/// \param[in]     rtype                report type :
///                                       - HID_REPORT_OUTPUT    = output report received
///                                       - HID_REPORT_FEATURE   = feature report received
/// \param[in]     req                  request type :
///                                       - USBD_HID_REQ_EP_CTRL = report received on control endpoint
///                                       - USBD_HID_REQ_EP_INT  = report received on interrupt endpoint
/// \param[in]     rid                  report ID (0 if only one report exists).
/// \param[in]     buf                  buffer that receives report data.
/// \param[in]     len                  length of received report data.
/// \return                             received report data processed status :
///                                       - value true :  processed
///                                       - value false : not processed
bool USBD_HIDn_SetReport (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len);

#endif // __DOXYGEN

/// \brief Asynchronously prepare HID Report data to send
/// \param[in]     instance             instance of HID class.
/// \param[in]     rid                  report ID.
/// \param[in]     buf                  buffer containing report data to send.
/// \param[in]     len                  number of report data bytes to send.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_HID_GetReportTrigger (uint8_t instance, uint8_t rid, const uint8_t *buf, uint32_t len);

#ifdef __DOXYGEN__

//  ==== USB Device Mass Storage Device Functions ====

// following functions are available for each instance of a MSC class.
// generic prefix USBD_MSCn is USBD_MSC0 for MSC class instance 0.

/// \brief Callback function called during \ref USBD_Initialize to initialize all Logical Units of the USB MSC class instance
void USBD_MSCn_Initialize (void);

/// \brief Callback function called during \ref USBD_Uninitialize to de-initialize all Logical Units of the USB MSC class instance
void USBD_MSCn_Uninitialize (void);

/// \brief Callback function called to get cache information
/// \param[out]    buffer               cache buffer address.
/// \param[out]    size                 cache buffer size.
///                                     operation execution status :
///                                       - value true :  operation succeeded
///                                       - value false : operation failed
bool USBD_MSCn_GetCacheInfo (uint32_t *buffer, uint32_t *size);

/// \brief Callback function called to get media capacity
/// \param[out]    block_count          total number of blocks on media.
/// \param[out]    block_size           media block size.
///                                     operation execution status :
///                                       - value true :  operation succeeded
///                                       - value false : operation failed
bool USBD_MSCn_GetMediaCapacity (uint32_t *block_count, uint32_t *block_size);

/// \brief Callback function called to read data from media
/// \param[in]     lba                  logical address of first block to read.
/// \param[in]     cnt                  number of contiguous blocks to read from media.
/// \param[out]    buf                  data buffer for data read from media.
///                                     operation execution status :
///                                       - value true :  operation succeeded
///                                       - value false : operation failed
bool USBD_MSCn_Read (uint32_t lba, uint32_t cnt, uint8_t *buf);

/// \brief Callback function called to write data to media
/// \param[in]     lba                  logical address of first block to write.
/// \param[in]     cnt                  number of contiguous blocks to write to media.
/// \param[in]     buf                  data buffer containing data to write to media.
///                                     operation execution status :
///                                       - value true :  operation succeeded
///                                       - value false : operation failed
bool USBD_MSCn_Write (uint32_t lba, uint32_t cnt, const uint8_t *buf);

/// \brief Callback function called to check media presence and write protect status
/// \return                             media presence and write protected status :
///                                     - bit 1:      write protect bit
///                                       - value 1 :   media is write protected
///                                       - value 0 :   media is not write protected
///                                     - bit 0:      media presence bit
///                                       - value 1 :   media is present
///                                       - value 0 :   media is not present
uint32_t USBD_MSCn_CheckMedia (void);

/// \brief Callback function called to get maximum number of logical units
/// \return                             number of logical units that device contains
///                                       - value > 0 and <= 4 : maximum number of logical units
///                                       - value 0 :            use setting from configuration file
uint8_t USBD_MSCn_GetMaxLUN (void);

/// \brief Callback function called to get media capacity  of a logical unit
/// \param[in]     lun                  logical unit number.
/// \param[out]    block_count          total number of blocks on media.
/// \param[out]    block_size           media block size.
///                                     operation execution status :
///                                       - value true :  operation succeeded
///                                       - value false : operation failed
bool USBD_MSCn_LUN_GetMediaCapacity (uint8_t lun, uint32_t *block_count, uint32_t *block_size);

/// \brief Callback function called to read data from media of a logical unit
/// \param[in]     lun                  logical unit number.
/// \param[in]     lba                  logical address of first block to read.
/// \param[in]     cnt                  number of contiguous blocks to read from media.
/// \param[out]    buf                  data buffer for data read from media.
///                                     operation execution status :
///                                       - value true :  operation succeeded
///                                       - value false : operation failed
bool USBD_MSCn_LUN_Read (uint8_t lun, uint32_t lba, uint32_t cnt, uint8_t *buf);

/// \brief Callback function called to write data to media of a logical unit
/// \param[in]     lun                  logical unit number.
/// \param[in]     lba                  logical address of first block to write.
/// \param[in]     cnt                  number of contiguous blocks to write to media.
/// \param[in]     buf                  data buffer containing data to write to media.
///                                     operation execution status :
///                                       - value true :  operation succeeded
///                                       - value false : operation failed
bool USBD_MSCn_LUN_Write (uint8_t lun, uint32_t lba, uint32_t cnt, const uint8_t *buf);

/// \brief Callback function called to check media presence and write protect status of a logical unit
/// \param[in]     lun                  logical unit number.
/// \return                             media presence and write protected status :
///                                     - bit 1:      write protect bit
///                                       - value 1 :   media is write protected
///                                       - value 0 :   media is not write protected
///                                     - bit 0:      media presence bit
///                                       - value 1 :   media is present
///                                       - value 0 :   media is not present
uint32_t USBD_MSCn_LUN_CheckMedia (uint8_t lun);

#endif // __DOXYGEN

//  ==== USB Device Custom Class Functions ====

#ifdef __DOXYGEN__

// following functions are available for each instance of a Custom class.
// generic prefix USBD_CustomClassn is USBD_CustomClass0 for Custom class instance 0.

/// \brief Callback function called during \ref USBD_Initialize to initialize the USB Custom class instance
void USBD_CustomClassn_Initialize (void);

/// \brief Callback function called during \ref USBD_Uninitialize to de-initialize the USB Custom class instance
void USBD_CustomClassn_Uninitialize (void);

/// \brief Callback function called upon USB Bus Reset signaling
void USBD_CustomClassn_Reset (void);

/// \brief Callback function called when Endpoint Start was requested (by activating interface or configuration)
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
void USBD_CustomClassn_EndpointStart (uint8_t ep_addr);

/// \brief Callback function called when Endpoint Stop was requested (by de-activating interface or activating configuration 0)
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
void USBD_CustomClassn_EndpointStop (uint8_t ep_addr);

/// \brief Callback function called when a SETUP PACKET was received on Control Endpoint 0
/// \param[in]     setup_packet         pointer to received setup packet.
/// \param[in,out] buf                  pointer to data buffer used for data stage requested by setup packet.
/// \param[in,out] len                  pointer to number of data bytes in data stage requested by setup packet.
/// \return        usbdRequestStatus    enumerator value indicating the function execution status :
///                                       - value usbdRequestNotProcessed : request was not processed; processing will be done by USB library
///                                       - value usbdRequestOK :           request was processed successfully (send Zero-Length Packet if no data stage)
///                                       - value usbdRequestStall :        request was processed but is not supported (STALL EP)
usbdRequestStatus USBD_CustomClassn_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);

/// \brief Callback function called when a SETUP PACKET was processed by USB library
/// \param[in]     setup_packet         pointer to processed setup packet.
void USBD_CustomClassn_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet);

/// \brief Callback function called when OUT DATA was received on Control Endpoint 0
/// \param[in]     len                  number of received data bytes.
/// \return        usbdRequestStatus    enumerator value indicating the function execution status :
///                                       - value usbdRequestNotProcessed : request was not processed; processing will be done by USB library
///                                       - value usbdRequestOK :           request was processed successfully (send Zero-Length Packet)
///                                       - value usbdRequestStall :        request was processed but is not supported (stall endpoint 0)
///                                       - value usbdRequestNAK :          request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_CustomClassn_Endpoint0_OutDataReceived (uint32_t len);

/// \brief Callback function called when IN DATA was sent on Control Endpoint 0
/// \param[in]     len                  number of sent data bytes.
/// \return        usbdRequestStatus    enumerator value indicating the function execution status :
///                                       - value usbdRequestNotProcessed : request was not processed; processing will be done by USB library
///                                       - value usbdRequestOK :           request was processed successfully (return ACK)
///                                       - value usbdRequestStall :        request was processed but is not supported (stall endpoint 0)
///                                       - value usbdRequestNAK :          request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_CustomClassn_Endpoint0_InDataSent (uint32_t len);

/// \brief Callback function called when DATA was sent or received on Endpoint n
/// \param[in]     event                event on Endpoint :
///                                       - ARM_USBD_EVENT_OUT = data OUT received
///                                       - ARM_USBD_EVENT_IN  = data IN  sent
void USBD_CustomClassn_Endpoint1_Event  (uint32_t event);
void USBD_CustomClassn_Endpoint2_Event  (uint32_t event);
void USBD_CustomClassn_Endpoint3_Event  (uint32_t event);
void USBD_CustomClassn_Endpoint4_Event  (uint32_t event);
void USBD_CustomClassn_Endpoint5_Event  (uint32_t event);
void USBD_CustomClassn_Endpoint6_Event  (uint32_t event);
void USBD_CustomClassn_Endpoint7_Event  (uint32_t event);
void USBD_CustomClassn_Endpoint8_Event  (uint32_t event);
void USBD_CustomClassn_Endpoint9_Event  (uint32_t event);
void USBD_CustomClassn_Endpoint10_Event (uint32_t event);
void USBD_CustomClassn_Endpoint11_Event (uint32_t event);
void USBD_CustomClassn_Endpoint12_Event (uint32_t event);
void USBD_CustomClassn_Endpoint13_Event (uint32_t event);
void USBD_CustomClassn_Endpoint14_Event (uint32_t event);
void USBD_CustomClassn_Endpoint15_Event (uint32_t event);

#endif // __DOXYGEN

/// \brief Start reception on Endpoint
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \param[out]    buf                  buffer that receives data.
/// \param[in]     len                  maximum number of bytes to receive.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_EndpointRead (uint8_t device, uint8_t ep_addr, uint8_t *buf, uint32_t len);

/// \brief Get result of read operation on Endpoint
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \return                             number of bytes received.
extern uint32_t USBD_EndpointReadGetResult (uint8_t device, uint8_t ep_addr);

/// \brief Start write on Endpoint
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \param[in]     buf                  buffer containing data bytes to write.
/// \param[in]     len                  number of bytes to write.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_EndpointWrite (uint8_t device, uint8_t ep_addr, const uint8_t *buf, uint32_t len);

/// \brief Get result of write operation on Endpoint
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \return                             number of bytes written.
extern uint32_t USBD_EndpointWriteGetResult (uint8_t device, uint8_t ep_addr);

/// \brief Set/Clear stall on Endpoint
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \param[in]     stall                operation :
///                                       - value false : clear stall
///                                       - value true :  set stall
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_EndpointStall (uint8_t device, uint8_t ep_addr, bool stall);

/// \brief Abort read/write operation on Endpoint
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBD_EndpointAbort (uint8_t device, uint8_t ep_addr);

//  ==== USB Host Functions ====

// Rename deprecated functions
#define USBH_GetDeviceStatus            USBH_Device_GetStatus

/// \brief Get version of USB Host stack
/// \return                             version (major.minor.patch : mmnnnpppp decimal)
extern uint32_t USBH_GetVersion (void);

/// \brief Initialize USB Host stack and controller
/// \param[in]     ctrl                 index of USB Host controller.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_Initialize (uint8_t ctrl);

/// \brief De-initialize USB Host stack and controller
/// \param[in]     ctrl                 index of USB Host controller.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_Uninitialize (uint8_t ctrl);

/// \brief Suspend a root HUB port on specified controller
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     port                 root HUB port.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_Port_Suspend (uint8_t ctrl, uint8_t port);

/// \brief Resume a root HUB port on specified controller
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     port                 root HUB port.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_Port_Resume (uint8_t ctrl, uint8_t port);

/// \brief Get index of USB Host controller to which USB Device is connected
/// \param[in]     device               index of USB Device.
/// \return                             index of USB Host controller or non-existing USB Host controller :
///                                       - value != 255 : index of USB Host controller
///                                       - value 255 :    non-existing USB Host controller
extern uint8_t USBH_Device_GetController (uint8_t device);

/// \brief Get index of USB Host Root HUB port to which USB Device is connected
/// \param[in]     device               index of USB Device.
/// \return                             index of USB Host Root HUB port or non-existing USB Host Root HUB port :
///                                       - value <= 15 : index of USB Host Root HUB port
///                                       - value 255 :   non-existing USB Host Root HUB port
extern uint8_t USBH_Device_GetPort (uint8_t device);

/// \brief Get status of USB Device
/// \param[in]     device               index of USB Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_Device_GetStatus (uint8_t device);

/// \brief Get communication speed of USB Device
/// \param[in]     device               index of USB Device.
/// \return                             communication speed :
///                                       - USB_SPEED_LOW  = low speed
///                                       - USB_SPEED_FULL = full speed
///                                       - USB_SPEED_HIGH = high speed
extern int32_t USBH_Device_GetSpeed (uint8_t device);

/// \brief Get communication address of USB Device
/// \param[in]     device               index of USB Device.
/// \return                             enumerated address or invalid address :
///                                       - value <= 127 : enumerated address
///                                       - value 255 :    invalid address
extern uint8_t USBH_Device_GetAddress (uint8_t device);

/// \brief Get Vendor ID (VID) of USB Device
/// \param[in]     device               index of USB Device.
/// \return                             Vendor ID.
extern uint16_t USBH_Device_GetVID (uint8_t device);

/// \brief Get Product ID (PID) of USB Device
/// \param[in]     device               index of USB Device.
/// \return                             Product ID.
extern uint16_t USBH_Device_GetPID (uint8_t device);

/// \brief Get String Descriptor of USB Device
/// \param[in]     device               index of USB Device.
/// \param[in]     index                index of string descriptor.
/// \param[in]     language_id          language ID.
/// \param[out]    descriptor_data      pointer to where descriptor data will be read.
/// \param[in]     descriptor_length    maximum descriptor length.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_Device_GetStringDescriptor (uint8_t device, uint8_t index, uint16_t language_id, uint8_t *descriptor_data, uint16_t descriptor_length);

/// \brief Callback function called when some event has happened on corresponding controller and port
/// \param[in]     ctrl                 index of USB Host controller.
/// \param[in]     port                 index of Root HUB port.
/// \param[in]     device               index of USB Device :
///                                       - value <= 127: index of of USB Device for device notifications
///                                       - value 255: no device information for port notifications
/// \param[in]     notify               notification :
///                                       - USBH_NOTIFY_CONNECT               = Port connection happened
///                                       - USBH_NOTIFY_DISCONNECT            = Port disconnection happened
///                                       - USBH_NOTIFY_OVERCURRENT           = Port overcurrent happened
///                                       - USBH_NOTIFY_REMOTE_WAKEUP         = Port remote wakeup signaling happened
///                                       - USBH_NOTIFY_READY                 = Device was successfully enumerated, initialized and is ready for communication
///                                       - USBH_NOTIFY_UNKNOWN_DEVICE        = Device was successfully enumerated but there is no driver for it
///                                       - USBH_NOTIFY_INSUFFICIENT_POWER    = Device requires more power consumption than available
///                                       - USBH_NOTIFY_CONFIGURATION_FAILED  = Device was not successfully configured (not enough resources)
///                                       - USBH_NOTIFY_INITIALIZATION_FAILED = Device was not successfully initialized
extern void USBH_Notify (uint8_t ctrl, uint8_t port, uint8_t device, USBH_NOTIFY notify);

//  ==== USB Host Mass Storage Functions ====

// Rename deprecated functions
#define USBH_MSC_GetDeviceStatus        USBH_MSC_GetStatus

/// \brief Callback function called when Mass Storage Device was enumerated and is ready
/// \param[in]     instance             instance of MSC Device.
extern void USBH_MSC_Initialize (uint8_t instance);

/// \brief Callback function called when Mass Storage Device was disconnected
/// \param[in]     instance             instance of MSC Device.
extern void USBH_MSC_Uninitialize (uint8_t instance);

/// \brief Get Device instance of Mass Storage Device
/// \param[in]     instance             instance of MSC Device.
/// \return                             instance of Device or non-existing Device instance :
///                                       - value <= 127 : instance of Device
///                                       - value 255 :    non-existing Device instance
extern uint8_t USBH_MSC_GetDevice (uint8_t instance);

/// \brief Get status of Mass Storage Device
/// \param[in]     instance             instance of MSC Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_MSC_GetStatus (uint8_t instance);

/// \brief Read requested number of blocks from Mass Storage Device
/// \param[in]     instance             instance of MSC Device.
/// \param[in]     lba                  logical block address of first block to read.
/// \param[in]     cnt                  number of contiguous blocks to read.
/// \param[out]    buf                  data buffer in which to read data.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_MSC_Read (uint8_t instance, uint32_t lba, uint32_t cnt, uint8_t *buf);

/// \brief Write requested number of blocks to Mass Storage Device
/// \param[in]     instance             instance of MSC Device.
/// \param[in]     lba                  logical address of first block to write.
/// \param[in]     cnt                  number of contiguous blocks to write.
/// \param[in]     buf                  data buffer containing data to write.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_MSC_Write (uint8_t instance, uint32_t lba, uint32_t cnt, const uint8_t *buf);

/// \brief Read capacity of Mass Storage Device
/// \param[in]     instance             instance of MSC Device.
/// \param[out]    block_count          pointer to where total number of blocks available will be read.
/// \param[out]    block_size           pointer to where block size will be read.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_MSC_ReadCapacity (uint8_t instance, uint32_t *block_count, uint32_t *block_size);

//  ==== USB Host Human Interface Device Functions ====

// Rename deprecated functions
#define USBH_HID_GetDeviceStatus        USBH_HID_GetStatus

/// \brief Callback function called when Human Interface Device was enumerated and is ready
/// \param[in]     instance             instance of HID Device.
extern void USBH_HID_Initialize (uint8_t instance);

/// \brief Callback function called when Human Interface Device was disconnected
/// \param[in]     instance             instance of HID Device.
extern void USBH_HID_Uninitialize (uint8_t instance);

/// \brief Get Device instance of Human Interface Device
/// \param[in]     instance             instance of HID Device.
/// \return                             instance of Device or non-existing Device instance :
///                                       - value <= 127 : instance of Device
///                                       - value 255 :    non-existing Device instance
extern uint8_t USBH_HID_GetDevice (uint8_t instance);

/// \brief Get status of Human Interface Device
/// \param[in]     instance             instance of HID Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_HID_GetStatus (uint8_t instance);

/// \brief Read data received from Human Interface Device
/// \param[in]     instance             instance of HID Device.
/// \param[out]    buf                  buffer that receives data.
/// \param[in]     len                  maximum number of bytes to read.
/// \return                             number of bytes read or execution status :
///                                       - value >= 0 : number of bytes read
///                                       - value < 0 :  error occurred, -value is execution status as defined with \ref usbStatus
extern int32_t USBH_HID_Read (uint8_t instance, uint8_t *buf, int32_t len);

/// \brief Write data to Human Interface Device
/// \param[in]     instance             instance of HID Device.
/// \param[in]     buf                  data buffer containing data to write.
/// \param[in]     len                  number of data bytes to write.
/// \return                             number of bytes accepted for writing or execution status :
///                                       - value >= 0 : number of bytes accepted for writing
///                                       - value < 0 :  error occurred, -value is execution status as defined with \ref usbStatus
extern int32_t USBH_HID_Write (uint8_t instance, const uint8_t *buf, int32_t len);

/// \brief Retrieve first pending pressed keyboard key on HID Keyboard
/// \param[in]     instance             instance of HID Device.
/// \return                             code of first pressed unread key or no key was pressed :
///                                       - value >= 0 : code of first pressed unread key
///                                       - value -1 :   indicates no key was pressed since last time this function was called
extern int USBH_HID_GetKeyboardKey (uint8_t instance);

/// \brief Retrieve state change since last call of this function
/// \param[in]     instance             instance of HID Device.
/// \param[out]    state                pointer to mouse state \ref usbHID_MouseState structure.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_HID_GetMouseState (uint8_t instance, usbHID_MouseState *state);

/// \brief Callback function called for parsing of the Human Interface Device report descriptor
/// \param[in]     instance             instance index.
/// \param[in]     ptr_hid_report_desc  pointer to HID report descriptor.
/// \param[in]     len                  length of HID report descriptor.
extern void USBH_HID_ParseReportDescriptor (uint8_t instance, const uint8_t *ptr_hid_report_desc, uint32_t len);

/// \brief Callback function called when data is received from the Human Interface Device
/// \param[in]     instance             instance index.
/// \param[in]     len                  length of received data.
extern void USBH_HID_DataReceived (uint8_t instance, uint32_t len);

//  ==== USB Host Communication Device Class (Abstract Control Model) Functions ====

// Rename deprecated functions
#define USBH_CDC_ACM_GetDeviceStatus    USBH_CDC_ACM_GetStatus

/// \brief Callback function called when Communication Device Class device was enumerated and is ready
/// \param[in]     instance             instance of CDC Device.
extern void USBH_CDC_ACM_Initialize (uint8_t instance);

/// \brief Callback function called when Communication Device Class device was disconnected
/// \param[in]     instance             instance of CDC Device.
extern void USBH_CDC_ACM_Uninitialize (uint8_t instance);

/// \brief Get Device instance of Communication Device Class device
/// \param[in]     instance             instance of CDC Device.
/// \return                             instance of Device or non-existing Device instance :
///                                       - value <= 127 : instance of Device
///                                       - value 255 :    non-existing Device instance
extern uint8_t USBH_CDC_ACM_GetDevice (uint8_t instance);

/// \brief Get status of Communication Device Class device
/// \param[in]     instance             instance of CDC Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_CDC_ACM_GetStatus (uint8_t instance);

/// \brief Send data to Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \param[in]     data                 buffer containing data bytes to send.
/// \param[in]     num                  number of bytes to send.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
extern usbStatus USBH_CDC_ACM_Send (uint8_t instance, const uint8_t *data, uint32_t num);

/// \brief Get result of send data to Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \return                             number of successfully sent data bytes.
extern uint32_t USBH_CDC_ACM_GetTxCount (uint8_t instance);

/// \brief Abort data sending to Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
extern usbStatus USBH_CDC_ACM_AbortSend (uint8_t instance);

/// \brief Receive data from Communication Device Class device
/// \param[in]     instance             index of CDC instance
/// \param[out]    data                 buffer that receives data.
/// \param[in]     num                  maximum number of bytes to receive.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
extern usbStatus USBH_CDC_ACM_Receive (uint8_t instance, uint8_t *data, uint32_t num);

/// \brief Get result of receive data from Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \return                             number of successfully received data bytes.
extern uint32_t USBH_CDC_ACM_GetRxCount (uint8_t instance);

/// \brief Abort data reception from Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
extern usbStatus USBH_CDC_ACM_AbortReceive (uint8_t instance);

/// \brief Change communication settings of Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \param[in]     line_coding          pointer to CDC_LINE_CODING structure.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
extern usbStatus USBH_CDC_ACM_SetLineCoding (uint8_t instance, const CDC_LINE_CODING *line_coding);

/// \brief Retrieve communication settings of Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \param[out]    line_coding          pointer to CDC_LINE_CODING structure.
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
extern usbStatus USBH_CDC_ACM_GetLineCoding (uint8_t instance, CDC_LINE_CODING *line_coding);

/// \brief Set control line states of Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \param[in]     state                control line settings bitmap :
///                                       - bit 0 : DTR state
///                                       - bit 1 : RTS state
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
extern usbStatus USBH_CDC_ACM_SetControlLineState (uint8_t instance, uint16_t state);

/// \brief Callback function called when Communication Device Class device
///        modem line or error status changes
/// \param[in]     instance             index of CDC instance.
/// \param[in]     status               error status and line states :
///                                       - bit 6 : bOverRun
///                                       - bit 5 : bParity
///                                       - bit 4 : bFraming
///                                       - bit 3 : bRingSignal
///                                       - bit 2 : bBreak
///                                       - bit 1 : bTxCarrier (DSR line state)
///                                       - bit 0 : bRxCarrier (DCD line state)
extern void USBH_CDC_ACM_Notify (uint8_t instance, uint16_t status);

/// \brief Send break on Communication Device Class device
/// \param[in]     instance             index of CDC instance.
/// \param[in]     duration             duration of break (in milliseconds) :
///                                       - value 65535 : indefinite
///                                       - value     0 : immediate
/// \return                             status code that indicates the execution status of the function as defined with usbStatus.
extern usbStatus USBH_CDC_ACM_SendBreak (uint8_t instance, uint16_t duration);

//  ==== USB Host Custom Class Functions ====

// Rename deprecated functions
#define USBH_CustomClass_GetDeviceStatus    USBH_CustomClass_GetStatus

/// \brief Get Device instance of Custom Class Device
/// \param[in]     instance             instance of Custom Class Device.
/// \return                             instance of Device or non-existing Device instance :
///                                       - value <= 127 : instance of Device
///                                       - value 255 :    non-existing Device instance
extern uint8_t USBH_CustomClass_GetDevice (uint8_t instance);

/// \brief Get status of Custom Class Device
/// \param[in]     instance             instance of Custom Class Device.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_CustomClass_GetStatus (uint8_t instance);

/// \brief Callback function called when custom class device is connected and needs
///        to configure resources used by custom class device instance
/// \param[in]     device               index of USB Device.
/// \param[in]     ptr_dev_desc         pointer to device descriptor.
/// \param[in]     ptr_cfg_desc         pointer to configuration descriptor.
/// \return                             index of configured custom class device instance or configuration failed :
///                                       - value <= 127 : index of configured custom class device instance
///                                       - value 255 :    configuration failed
extern uint8_t USBH_CustomClass_Configure (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc);

/// \brief Callback function called when custom class device is disconnected and needs
///        to de-configure resources used by custom class device instance
/// \param[in]     instance             index of custom class device instance.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_CustomClass_Unconfigure (uint8_t instance);

/// \brief Callback function called when custom class device is connected and needs
///        to initialize custom class device instance
/// \param[in]     instance             index of custom class device instance.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_CustomClass_Initialize (uint8_t instance);

/// \brief Callback function called when custom class device is disconnected and needs
///        to de-initialize custom class device instance
/// \param[in]     instance             index of custom class device instance.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_CustomClass_Uninitialize (uint8_t instance);

/// \brief Create Pipe
/// \param[in]     device               index of USB Device.
/// \param[in]     ep_addr              endpoint address :
///                                       - ep_addr.0..3 : address
///                                       - ep_addr.7 :    direction
/// \param[in]     ep_type              endpoint type.
/// \param[in]     ep_max_packet_size   endpoint maximum packet size.
/// \param[in]     ep_interval          endpoint polling interval.
/// \return                             pipe handle or pipe creation failed :
///                                       - value > 0 : pipe handle
///                                       - value 0 :   pipe creation failed
extern USBH_PIPE_HANDLE USBH_PipeCreate (uint8_t device, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size, uint8_t ep_interval);

/// \brief Register a callback for Pipe events
/// \param[in]     pipe_hndl            pipe handle.
/// \param[in]     cb_pipe_event        pointer to pipe event callback function :
///                                       - value > 0 : pointer to pipe event callback function
///                                       - value 0 :   unregister pipe event callback
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_PipeRegisterCallback (USBH_PIPE_HANDLE pipe_hndl, USBH_PipeEvent_t cb_pipe_event);

/// \brief Update Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_PipeUpdate (USBH_PIPE_HANDLE pipe_hndl);

/// \brief Delete Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_PipeDelete (USBH_PIPE_HANDLE pipe_hndl);

/// \brief Reset Pipe (reset data toggle)
/// \param[in]     pipe_hndl            pipe handle.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_PipeReset (USBH_PIPE_HANDLE pipe_hndl);

/// \brief Receive data on Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \param[out]    buf                  buffer that receives data.
/// \param[in]     len                  maximum number of bytes to receive.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_PipeReceive (USBH_PIPE_HANDLE pipe_hndl, uint8_t *buf, uint32_t len);

/// \brief Get result of receive data operation on Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \return                             number of successfully received data bytes.
extern uint32_t USBH_PipeReceiveGetResult (USBH_PIPE_HANDLE pipe_hndl);

/// \brief Send data on Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \param[in]     buf                  buffer containing data bytes to send.
/// \param[in]     len                  number of bytes to send.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_PipeSend (USBH_PIPE_HANDLE pipe_hndl, const uint8_t *buf, uint32_t len);

/// \brief Get result of send data operation on Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \return                             number of successfully sent data bytes.
extern uint32_t USBH_PipeSendGetResult (USBH_PIPE_HANDLE pipe_hndl);

/// \brief Abort send/receive operation on Pipe
/// \param[in]     pipe_hndl            pipe handle.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_PipeAbort (USBH_PIPE_HANDLE pipe_hndl);

/// \brief Do a Control Transfer on Default Pipe
/// \param[in]     device               index of USB Device.
/// \param[in]     setup_packet         pointer to setup packet.
/// \param[in,out] data                 buffer containing data bytes to send or where data should be received in data stage of Control Transfer.
/// \param[in]     len                  number of bytes to send or receive in data stage of Control Transfer.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_ControlTransfer (uint8_t device, const USB_SETUP_PACKET *setup_packet, uint8_t *data, uint32_t len);

/// \brief Standard Device Request on Default Pipe - GET_STATUS
/// \param[in]     device               index of USB Device.
/// \param[in]     recipient            recipient.
/// \param[in]     index                interface or endpoint index.
/// \param[out]    ptr_stat_dat         pointer to where status data should be received.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_DeviceRequest_GetStatus (uint8_t device, uint8_t recipient, uint8_t index, uint8_t *ptr_stat_dat);

/// \brief Standard Device Request on Default Pipe - CLEAR_FEATURE
/// \param[in]     device               index of USB Device.
/// \param[in]     recipient            recipient.
/// \param[in]     index                interface or endpoint index.
/// \param[in]     feature_selector     feature selector.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_DeviceRequest_ClearFeature (uint8_t device, uint8_t recipient, uint8_t index, uint8_t feature_selector);

/// \brief Standard Device Request on Default Pipe - SET_FEATURE
/// \param[in]     device               index of USB Device.
/// \param[in]     recipient            recipient.
/// \param[in]     index                interface or endpoint index.
/// \param[in]     feature_selector     feature selector.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_DeviceRequest_SetFeature (uint8_t device, uint8_t recipient, uint8_t index, uint8_t feature_selector);

/// \brief Standard Device Request on Default Pipe - SET_ADDRESS
/// \param[in]     device               index of USB Device.
/// \param[in]     device_address       device address.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_DeviceRequest_SetAddress (uint8_t device, uint8_t device_address);

/// \brief Standard Device Request on Default Pipe - GET_DESCRIPTOR
/// \param[in]     device               index of USB Device.
/// \param[in]     recipient            recipient.
/// \param[in]     descriptor_type      descriptor type.
/// \param[in]     descriptor_index     descriptor index.
/// \param[in]     language_id          language ID.
/// \param[out]    descriptor_data      pointer to where descriptor data will be read.
/// \param[in]     descriptor_length    maximum descriptor length.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_DeviceRequest_GetDescriptor (uint8_t device, uint8_t recipient, uint8_t descriptor_type, uint8_t descriptor_index, uint16_t language_id, uint8_t *descriptor_data, uint16_t descriptor_length);

/// \brief Standard Device Request on Default Pipe - SET_DESCRIPTOR
/// \param[in]     device               index of USB Device.
/// \param[in]     recipient            recipient.
/// \param[in]     descriptor_type      descriptor type.
/// \param[in]     descriptor_index     descriptor index.
/// \param[in]     language_id          language ID.
/// \param[in]     descriptor_data      pointer to descriptor data to be written.
/// \param[in]     descriptor_length    descriptor length.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_DeviceRequest_SetDescriptor (uint8_t device, uint8_t recipient, uint8_t descriptor_type, uint8_t descriptor_index, uint16_t language_id, const uint8_t *descriptor_data, uint16_t descriptor_length);

/// \brief Standard Device Request on Default Pipe - GET_CONFIGURATION
/// \param[in]     device               index of USB Device.
/// \param[out]    ptr_configuration    pointer to where configuration will be read.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_DeviceRequest_GetConfiguration (uint8_t device, uint8_t *ptr_configuration);

/// \brief Standard Device Request on Default Pipe - SET_CONFIGURATION
/// \param[in]     device               index of USB Device.
/// \param[in]     configuration        configuration.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_DeviceRequest_SetConfiguration (uint8_t device, uint8_t configuration);

/// \brief Standard Device Request on Default Pipe - GET_INTERFACE
/// \param[in]     device               index of USB Device.
/// \param[in]     index                interface index.
/// \param[out]    ptr_alternate        pointer to where alternate setting data will be read.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_DeviceRequest_GetInterface (uint8_t device, uint8_t index, uint8_t *ptr_alternate);

/// \brief Standard Device Request on Default Pipe - SET_INTERFACE
/// \param[in]     device               index of USB Device.
/// \param[in]     index                interface index.
/// \param[in]     alternate            alternate setting.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_DeviceRequest_SetInterface (uint8_t device, uint8_t index, uint8_t alternate);

/// \brief Standard Device Request on Default Pipe - SYNCH_FRAME
/// \param[in]     device               index of USB Device.
/// \param[in]     index                interface or endpoint index.
/// \param[out]    ptr_frame_number     pointer to where frame number data will be read.
/// \return                             status code that indicates the execution status of the function as defined with \ref usbStatus.
extern usbStatus USBH_DeviceRequest_SynchFrame (uint8_t device, uint8_t index, uint8_t *ptr_frame_number);

// USB Device user callback function prototypes
extern void              USBD_Device0_Initialize            (void);
extern void              USBD_Device0_Uninitialize          (void);
extern void              USBD_Device0_VbusChanged           (bool level);
extern void              USBD_Device0_Reset                 (void);
extern void              USBD_Device0_HighSpeedActivated    (void);
extern void              USBD_Device0_Suspended             (void);
extern void              USBD_Device0_Resumed               (void);
extern void              USBD_Device0_ConfigurationChanged  (uint8_t val);
extern void              USBD_Device0_EnableRemoteWakeup    (void);
extern void              USBD_Device0_DisableRemoteWakeup   (void);
extern usbdRequestStatus USBD_Device0_Endpoint0_SetupPacketReceived  (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);
extern void              USBD_Device0_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet);
extern usbdRequestStatus USBD_Device0_Endpoint0_OutDataReceived      (uint32_t len);
extern usbdRequestStatus USBD_Device0_Endpoint0_InDataSent           (uint32_t len);

extern void              USBD_Device1_Initialize            (void);
extern void              USBD_Device1_Uninitialize          (void);
extern void              USBD_Device1_VbusChanged           (bool level);
extern void              USBD_Device1_Reset                 (void);
extern void              USBD_Device1_HighSpeedActivated    (void);
extern void              USBD_Device1_Suspended             (void);
extern void              USBD_Device1_Resumed               (void);
extern void              USBD_Device1_ConfigurationChanged  (uint8_t val);
extern void              USBD_Device1_EnableRemoteWakeup    (void);
extern void              USBD_Device1_DisableRemoteWakeup   (void);
extern usbdRequestStatus USBD_Device1_Endpoint0_SetupPacketReceived  (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);
extern void              USBD_Device1_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet);
extern usbdRequestStatus USBD_Device1_Endpoint0_OutDataReceived      (uint32_t len);
extern usbdRequestStatus USBD_Device1_Endpoint0_InDataSent           (uint32_t len);

extern void              USBD_Device2_Initialize            (void);
extern void              USBD_Device2_Uninitialize          (void);
extern void              USBD_Device2_VbusChanged           (bool level);
extern void              USBD_Device2_Reset                 (void);
extern void              USBD_Device2_HighSpeedActivated    (void);
extern void              USBD_Device2_Suspended             (void);
extern void              USBD_Device2_Resumed               (void);
extern void              USBD_Device2_ConfigurationChanged  (uint8_t val);
extern void              USBD_Device2_EnableRemoteWakeup    (void);
extern void              USBD_Device2_DisableRemoteWakeup   (void);
extern usbdRequestStatus USBD_Device2_Endpoint0_SetupPacketReceived  (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);
extern void              USBD_Device2_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet);
extern usbdRequestStatus USBD_Device2_Endpoint0_OutDataReceived      (uint32_t len);
extern usbdRequestStatus USBD_Device2_Endpoint0_InDataSent           (uint32_t len);

extern void              USBD_Device3_Initialize            (void);
extern void              USBD_Device3_Uninitialize          (void);
extern void              USBD_Device3_VbusChanged           (bool level);
extern void              USBD_Device3_Reset                 (void);
extern void              USBD_Device3_HighSpeedActivated    (void);
extern void              USBD_Device3_Suspended             (void);
extern void              USBD_Device3_Resumed               (void);
extern void              USBD_Device3_ConfigurationChanged  (uint8_t val);
extern void              USBD_Device3_EnableRemoteWakeup    (void);
extern void              USBD_Device3_DisableRemoteWakeup   (void);
extern usbdRequestStatus USBD_Device3_Endpoint0_SetupPacketReceived  (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);
extern void              USBD_Device3_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet);
extern usbdRequestStatus USBD_Device3_Endpoint0_OutDataReceived      (uint32_t len);
extern usbdRequestStatus USBD_Device3_Endpoint0_InDataSent           (uint32_t len);

extern void              USBD_ADC0_Initialize               (void);
extern void              USBD_ADC0_Uninitialize             (void);
extern void              USBD_ADC0_PlayStart                (void);
extern void              USBD_ADC0_PlayStop                 (void);
extern void              USBD_ADC0_SetSpeakerMute           (uint8_t ch, bool on);
extern void              USBD_ADC0_SetSpeakerVolume         (uint8_t ch, uint16_t vol);
extern void              USBD_ADC0_RecordStart              (void);
extern void              USBD_ADC0_RecordStop               (void);
extern void              USBD_ADC0_SetMicrophoneMute        (uint8_t ch, bool on);
extern void              USBD_ADC0_SetMicrophoneVolume      (uint8_t ch, uint16_t vol);
extern void              USBD_ADC0_ReceivedSamples          (void);

extern void              USBD_ADC1_Initialize               (void);
extern void              USBD_ADC1_Uninitialize             (void);
extern void              USBD_ADC1_PlayStart                (void);
extern void              USBD_ADC1_PlayStop                 (void);
extern void              USBD_ADC1_SetSpeakerMute           (uint8_t ch, bool on);
extern void              USBD_ADC1_SetSpeakerVolume         (uint8_t ch, uint16_t vol);
extern void              USBD_ADC1_RecordStart              (void);
extern void              USBD_ADC1_RecordStop               (void);
extern void              USBD_ADC1_SetMicrophoneMute        (uint8_t ch, bool on);
extern void              USBD_ADC1_SetMicrophoneVolume      (uint8_t ch, uint16_t vol);
extern void              USBD_ADC1_ReceivedSamples          (void);
 
extern void              USBD_ADC2_Initialize               (void);
extern void              USBD_ADC2_Uninitialize             (void);
extern void              USBD_ADC2_PlayStart                (void);
extern void              USBD_ADC2_PlayStop                 (void);
extern void              USBD_ADC2_SetSpeakerMute           (uint8_t ch, bool on);
extern void              USBD_ADC2_SetSpeakerVolume         (uint8_t ch, uint16_t vol);
extern void              USBD_ADC2_RecordStart              (void);
extern void              USBD_ADC2_RecordStop               (void);
extern void              USBD_ADC2_SetMicrophoneMute        (uint8_t ch, bool on);
extern void              USBD_ADC2_SetMicrophoneVolume      (uint8_t ch, uint16_t vol);
extern void              USBD_ADC2_ReceivedSamples          (void);
 
extern void              USBD_ADC3_Initialize               (void);
extern void              USBD_ADC3_Uninitialize             (void);
extern void              USBD_ADC3_PlayStart                (void);
extern void              USBD_ADC3_PlayStop                 (void);
extern void              USBD_ADC3_SetSpeakerMute           (uint8_t ch, bool on);
extern void              USBD_ADC3_SetSpeakerVolume         (uint8_t ch, uint16_t vol);
extern void              USBD_ADC3_RecordStart              (void);
extern void              USBD_ADC3_RecordStop               (void);
extern void              USBD_ADC3_SetMicrophoneMute        (uint8_t ch, bool on);
extern void              USBD_ADC3_SetMicrophoneVolume      (uint8_t ch, uint16_t vol);
extern void              USBD_ADC3_ReceivedSamples          (void);
 
extern void              USBD_CDC0_ACM_Initialize           (void);
extern void              USBD_CDC0_ACM_Uninitialize         (void);
extern void              USBD_CDC0_ACM_Reset                (void);
extern bool              USBD_CDC0_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len);
extern bool              USBD_CDC0_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len);
extern bool              USBD_CDC0_ACM_SetLineCoding        (const CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC0_ACM_GetLineCoding        (      CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC0_ACM_SetControlLineState  (uint16_t state);
extern void              USBD_CDC0_ACM_DataReceived         (uint32_t len);
extern void              USBD_CDC0_ACM_DataSent             (void);
extern void              USBD_CDC0_NCM_Initialize           (void);
extern void              USBD_CDC0_NCM_Uninitialize         (void);
extern void              USBD_CDC0_NCM_Reset                (void);
extern void              USBD_CDC0_NCM_Start                (void);
extern void              USBD_CDC0_NCM_Stop                 (void);
extern bool              USBD_CDC0_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters);
extern bool              USBD_CDC0_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size);
extern bool              USBD_CDC0_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active);
extern bool              USBD_CDC0_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap);
extern bool              USBD_CDC0_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data);
extern bool              USBD_CDC0_NCM_GetNtbParameters                        (CDC_NCM_NTB_PARAM *ntb_params);
extern bool              USBD_CDC0_NCM_GetNetAddress        (uint8_t *net_addr);
extern bool              USBD_CDC0_NCM_SetNetAddress        (const uint8_t *net_addr);
extern bool              USBD_CDC0_NCM_GetNtbFormat         (uint16_t *ntb_format);
extern bool              USBD_CDC0_NCM_SetNtbFormat         (uint16_t  ntb_format);
extern bool              USBD_CDC0_NCM_GetNtbInputSize      (uint32_t *ntb_input_size);
extern bool              USBD_CDC0_NCM_SetNtbInputSize      (uint32_t  ntb_input_size);
extern bool              USBD_CDC0_NCM_GetMaxDatagramSize   (uint16_t *max_datagram_size);
extern bool              USBD_CDC0_NCM_SetMaxDatagramSize   (uint16_t  max_datagram_size);
extern bool              USBD_CDC0_NCM_GetCrcMode           (uint16_t *crc_mode);
extern bool              USBD_CDC0_NCM_SetCrcMode           (uint16_t  crc_mode);
extern void              USBD_CDC0_NCM_NTB_IN_Sent          (void);
extern void              USBD_CDC0_NCM_NTB_OUT_Received     (void);
 
extern void              USBD_CDC1_ACM_Initialize           (void);
extern void              USBD_CDC1_ACM_Uninitialize         (void);
extern void              USBD_CDC1_ACM_Reset                (void);
extern bool              USBD_CDC1_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len);
extern bool              USBD_CDC1_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len);
extern bool              USBD_CDC1_ACM_SetLineCoding        (const CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC1_ACM_GetLineCoding        (      CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC1_ACM_SetControlLineState  (uint16_t state);
extern void              USBD_CDC1_ACM_DataReceived         (uint32_t len);
extern void              USBD_CDC1_ACM_DataSent             (void);
extern void              USBD_CDC1_NCM_Initialize           (void);
extern void              USBD_CDC1_NCM_Uninitialize         (void);
extern void              USBD_CDC1_NCM_Reset                (void);
extern void              USBD_CDC1_NCM_Start                (void);
extern void              USBD_CDC1_NCM_Stop                 (void);
extern bool              USBD_CDC1_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters);
extern bool              USBD_CDC1_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size);
extern bool              USBD_CDC1_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active);
extern bool              USBD_CDC1_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap);
extern bool              USBD_CDC1_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data);
extern bool              USBD_CDC1_NCM_GetNtbParameters                        (CDC_NCM_NTB_PARAM *ntb_params);
extern bool              USBD_CDC1_NCM_GetNetAddress        (uint8_t *net_addr);
extern bool              USBD_CDC1_NCM_SetNetAddress        (const uint8_t *net_addr);
extern bool              USBD_CDC1_NCM_GetNtbFormat         (uint16_t *ntb_format);
extern bool              USBD_CDC1_NCM_SetNtbFormat         (uint16_t  ntb_format);
extern bool              USBD_CDC1_NCM_GetNtbInputSize      (uint32_t *ntb_input_size);
extern bool              USBD_CDC1_NCM_SetNtbInputSize      (uint32_t  ntb_input_size);
extern bool              USBD_CDC1_NCM_GetMaxDatagramSize   (uint16_t *max_datagram_size);
extern bool              USBD_CDC1_NCM_SetMaxDatagramSize   (uint16_t  max_datagram_size);
extern bool              USBD_CDC1_NCM_GetCrcMode           (uint16_t *crc_mode);
extern bool              USBD_CDC1_NCM_SetCrcMode           (uint16_t  crc_mode);
extern void              USBD_CDC1_NCM_NTB_IN_Sent          (void);
extern void              USBD_CDC1_NCM_NTB_OUT_Received     (void);
 
extern void              USBD_CDC2_ACM_Initialize           (void);
extern void              USBD_CDC2_ACM_Uninitialize         (void);
extern void              USBD_CDC2_ACM_Reset                (void);
extern bool              USBD_CDC2_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len);
extern bool              USBD_CDC2_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len);
extern bool              USBD_CDC2_ACM_SetLineCoding        (const CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC2_ACM_GetLineCoding        (      CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC2_ACM_SetControlLineState  (uint16_t state);
extern void              USBD_CDC2_ACM_DataReceived         (uint32_t len);
extern void              USBD_CDC2_ACM_DataSent             (void);
extern void              USBD_CDC2_NCM_Initialize           (void);
extern void              USBD_CDC2_NCM_Uninitialize         (void);
extern void              USBD_CDC2_NCM_Reset                (void);
extern void              USBD_CDC2_NCM_Start                (void);
extern void              USBD_CDC2_NCM_Stop                 (void);
extern bool              USBD_CDC2_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters);
extern bool              USBD_CDC2_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size);
extern bool              USBD_CDC2_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active);
extern bool              USBD_CDC2_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap);
extern bool              USBD_CDC2_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data);
extern bool              USBD_CDC2_NCM_GetNtbParameters                        (CDC_NCM_NTB_PARAM *ntb_params);
extern bool              USBD_CDC2_NCM_GetNetAddress        (uint8_t *net_addr);
extern bool              USBD_CDC2_NCM_SetNetAddress        (const uint8_t *net_addr);
extern bool              USBD_CDC2_NCM_GetNtbFormat         (uint16_t *ntb_format);
extern bool              USBD_CDC2_NCM_SetNtbFormat         (uint16_t  ntb_format);
extern bool              USBD_CDC2_NCM_GetNtbInputSize      (uint32_t *ntb_input_size);
extern bool              USBD_CDC2_NCM_SetNtbInputSize      (uint32_t  ntb_input_size);
extern bool              USBD_CDC2_NCM_GetMaxDatagramSize   (uint16_t *max_datagram_size);
extern bool              USBD_CDC2_NCM_SetMaxDatagramSize   (uint16_t  max_datagram_size);
extern bool              USBD_CDC2_NCM_GetCrcMode           (uint16_t *crc_mode);
extern bool              USBD_CDC2_NCM_SetCrcMode           (uint16_t  crc_mode);
extern void              USBD_CDC2_NCM_NTB_IN_Sent          (void);
extern void              USBD_CDC2_NCM_NTB_OUT_Received     (void);
 
extern void              USBD_CDC3_ACM_Initialize           (void);
extern void              USBD_CDC3_ACM_Uninitialize         (void);
extern void              USBD_CDC3_ACM_Reset                (void);
extern bool              USBD_CDC3_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len);
extern bool              USBD_CDC3_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len);
extern bool              USBD_CDC3_ACM_SetLineCoding        (const CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC3_ACM_GetLineCoding        (      CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC3_ACM_SetControlLineState  (uint16_t state);
extern void              USBD_CDC3_ACM_DataReceived         (uint32_t len);
extern void              USBD_CDC3_ACM_DataSent             (void);
extern void              USBD_CDC3_NCM_Initialize           (void);
extern void              USBD_CDC3_NCM_Uninitialize         (void);
extern void              USBD_CDC3_NCM_Reset                (void);
extern void              USBD_CDC3_NCM_Start                (void);
extern void              USBD_CDC3_NCM_Stop                 (void);
extern bool              USBD_CDC3_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters);
extern bool              USBD_CDC3_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size);
extern bool              USBD_CDC3_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active);
extern bool              USBD_CDC3_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap);
extern bool              USBD_CDC3_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data);
extern bool              USBD_CDC3_NCM_GetNtbParameters                        (CDC_NCM_NTB_PARAM *ntb_params);
extern bool              USBD_CDC3_NCM_GetNetAddress        (uint8_t *net_addr);
extern bool              USBD_CDC3_NCM_SetNetAddress        (const uint8_t *net_addr);
extern bool              USBD_CDC3_NCM_GetNtbFormat         (uint16_t *ntb_format);
extern bool              USBD_CDC3_NCM_SetNtbFormat         (uint16_t  ntb_format);
extern bool              USBD_CDC3_NCM_GetNtbInputSize      (uint32_t *ntb_input_size);
extern bool              USBD_CDC3_NCM_SetNtbInputSize      (uint32_t  ntb_input_size);
extern bool              USBD_CDC3_NCM_GetMaxDatagramSize   (uint16_t *max_datagram_size);
extern bool              USBD_CDC3_NCM_SetMaxDatagramSize   (uint16_t  max_datagram_size);
extern bool              USBD_CDC3_NCM_GetCrcMode           (uint16_t *crc_mode);
extern bool              USBD_CDC3_NCM_SetCrcMode           (uint16_t  crc_mode);
extern void              USBD_CDC3_NCM_NTB_IN_Sent          (void);
extern void              USBD_CDC3_NCM_NTB_OUT_Received     (void);
 
extern void              USBD_CDC4_ACM_Initialize           (void);
extern void              USBD_CDC4_ACM_Uninitialize         (void);
extern void              USBD_CDC4_ACM_Reset                (void);
extern bool              USBD_CDC4_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len);
extern bool              USBD_CDC4_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len);
extern bool              USBD_CDC4_ACM_SetLineCoding        (const CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC4_ACM_GetLineCoding        (      CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC4_ACM_SetControlLineState  (uint16_t state);
extern void              USBD_CDC4_ACM_DataReceived         (uint32_t len);
extern void              USBD_CDC4_ACM_DataSent             (void);
extern void              USBD_CDC4_NCM_Initialize           (void);
extern void              USBD_CDC4_NCM_Uninitialize         (void);
extern void              USBD_CDC4_NCM_Reset                (void);
extern void              USBD_CDC4_NCM_Start                (void);
extern void              USBD_CDC4_NCM_Stop                 (void);
extern bool              USBD_CDC4_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters);
extern bool              USBD_CDC4_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size);
extern bool              USBD_CDC4_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active);
extern bool              USBD_CDC4_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap);
extern bool              USBD_CDC4_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data);
extern bool              USBD_CDC4_NCM_GetNtbParameters                        (CDC_NCM_NTB_PARAM *ntb_params);
extern bool              USBD_CDC4_NCM_GetNetAddress        (uint8_t *net_addr);
extern bool              USBD_CDC4_NCM_SetNetAddress        (const uint8_t *net_addr);
extern bool              USBD_CDC4_NCM_GetNtbFormat         (uint16_t *ntb_format);
extern bool              USBD_CDC4_NCM_SetNtbFormat         (uint16_t  ntb_format);
extern bool              USBD_CDC4_NCM_GetNtbInputSize      (uint32_t *ntb_input_size);
extern bool              USBD_CDC4_NCM_SetNtbInputSize      (uint32_t  ntb_input_size);
extern bool              USBD_CDC4_NCM_GetMaxDatagramSize   (uint16_t *max_datagram_size);
extern bool              USBD_CDC4_NCM_SetMaxDatagramSize   (uint16_t  max_datagram_size);
extern bool              USBD_CDC4_NCM_GetCrcMode           (uint16_t *crc_mode);
extern bool              USBD_CDC4_NCM_SetCrcMode           (uint16_t  crc_mode);
extern void              USBD_CDC4_NCM_NTB_IN_Sent          (void);
extern void              USBD_CDC4_NCM_NTB_OUT_Received     (void);
 
extern void              USBD_CDC5_ACM_Initialize           (void);
extern void              USBD_CDC5_ACM_Uninitialize         (void);
extern void              USBD_CDC5_ACM_Reset                (void);
extern bool              USBD_CDC5_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len);
extern bool              USBD_CDC5_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len);
extern bool              USBD_CDC5_ACM_SetLineCoding        (const CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC5_ACM_GetLineCoding        (      CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC5_ACM_SetControlLineState  (uint16_t state);
extern void              USBD_CDC5_ACM_DataReceived         (uint32_t len);
extern void              USBD_CDC5_ACM_DataSent             (void);
extern void              USBD_CDC5_NCM_Initialize           (void);
extern void              USBD_CDC5_NCM_Uninitialize         (void);
extern void              USBD_CDC5_NCM_Reset                (void);
extern void              USBD_CDC5_NCM_Start                (void);
extern void              USBD_CDC5_NCM_Stop                 (void);
extern bool              USBD_CDC5_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters);
extern bool              USBD_CDC5_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size);
extern bool              USBD_CDC5_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active);
extern bool              USBD_CDC5_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap);
extern bool              USBD_CDC5_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data);
extern bool              USBD_CDC5_NCM_GetNtbParameters                        (CDC_NCM_NTB_PARAM *ntb_params);
extern bool              USBD_CDC5_NCM_GetNetAddress        (uint8_t *net_addr);
extern bool              USBD_CDC5_NCM_SetNetAddress        (const uint8_t *net_addr);
extern bool              USBD_CDC5_NCM_GetNtbFormat         (uint16_t *ntb_format);
extern bool              USBD_CDC5_NCM_SetNtbFormat         (uint16_t  ntb_format);
extern bool              USBD_CDC5_NCM_GetNtbInputSize      (uint32_t *ntb_input_size);
extern bool              USBD_CDC5_NCM_SetNtbInputSize      (uint32_t  ntb_input_size);
extern bool              USBD_CDC5_NCM_GetMaxDatagramSize   (uint16_t *max_datagram_size);
extern bool              USBD_CDC5_NCM_SetMaxDatagramSize   (uint16_t  max_datagram_size);
extern bool              USBD_CDC5_NCM_GetCrcMode           (uint16_t *crc_mode);
extern bool              USBD_CDC5_NCM_SetCrcMode           (uint16_t  crc_mode);
extern void              USBD_CDC5_NCM_NTB_IN_Sent          (void);
extern void              USBD_CDC5_NCM_NTB_OUT_Received     (void);
 
extern void              USBD_CDC6_ACM_Initialize           (void);
extern void              USBD_CDC6_ACM_Uninitialize         (void);
extern void              USBD_CDC6_ACM_Reset                (void);
extern bool              USBD_CDC6_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len);
extern bool              USBD_CDC6_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len);
extern bool              USBD_CDC6_ACM_SetLineCoding        (const CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC6_ACM_GetLineCoding        (      CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC6_ACM_SetControlLineState  (uint16_t state);
extern void              USBD_CDC6_ACM_DataReceived         (uint32_t len);
extern void              USBD_CDC6_ACM_DataSent             (void);
extern void              USBD_CDC6_NCM_Initialize           (void);
extern void              USBD_CDC6_NCM_Uninitialize         (void);
extern void              USBD_CDC6_NCM_Reset                (void);
extern void              USBD_CDC6_NCM_Start                (void);
extern void              USBD_CDC6_NCM_Stop                 (void);
extern bool              USBD_CDC6_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters);
extern bool              USBD_CDC6_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size);
extern bool              USBD_CDC6_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active);
extern bool              USBD_CDC6_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap);
extern bool              USBD_CDC6_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data);
extern bool              USBD_CDC6_NCM_GetNtbParameters                        (CDC_NCM_NTB_PARAM *ntb_params);
extern bool              USBD_CDC6_NCM_GetNetAddress        (uint8_t *net_addr);
extern bool              USBD_CDC6_NCM_SetNetAddress        (const uint8_t *net_addr);
extern bool              USBD_CDC6_NCM_GetNtbFormat         (uint16_t *ntb_format);
extern bool              USBD_CDC6_NCM_SetNtbFormat         (uint16_t  ntb_format);
extern bool              USBD_CDC6_NCM_GetNtbInputSize      (uint32_t *ntb_input_size);
extern bool              USBD_CDC6_NCM_SetNtbInputSize      (uint32_t  ntb_input_size);
extern bool              USBD_CDC6_NCM_GetMaxDatagramSize   (uint16_t *max_datagram_size);
extern bool              USBD_CDC6_NCM_SetMaxDatagramSize   (uint16_t  max_datagram_size);
extern bool              USBD_CDC6_NCM_GetCrcMode           (uint16_t *crc_mode);
extern bool              USBD_CDC6_NCM_SetCrcMode           (uint16_t  crc_mode);
extern void              USBD_CDC6_NCM_NTB_IN_Sent          (void);
extern void              USBD_CDC6_NCM_NTB_OUT_Received     (void);
 
extern void              USBD_CDC7_ACM_Initialize           (void);
extern void              USBD_CDC7_ACM_Uninitialize         (void);
extern void              USBD_CDC7_ACM_Reset                (void);
extern bool              USBD_CDC7_ACM_SendEncapsulatedCommand (            const uint8_t  *buf, uint16_t  len);
extern bool              USBD_CDC7_ACM_GetEncapsulatedResponse (uint16_t max_len, uint8_t **buf, uint16_t *len);
extern bool              USBD_CDC7_ACM_SetLineCoding        (const CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC7_ACM_GetLineCoding        (      CDC_LINE_CODING *line_coding);
extern bool              USBD_CDC7_ACM_SetControlLineState  (uint16_t state);
extern void              USBD_CDC7_ACM_DataReceived         (uint32_t len);
extern void              USBD_CDC7_ACM_DataSent             (void);
extern void              USBD_CDC7_NCM_Initialize           (void);
extern void              USBD_CDC7_NCM_Uninitialize         (void);
extern void              USBD_CDC7_NCM_Reset                (void);
extern void              USBD_CDC7_NCM_Start                (void);
extern void              USBD_CDC7_NCM_Stop                 (void);
extern bool              USBD_CDC7_NCM_SetEthernetMulticastFilters             (const uint8_t *addr_list, uint16_t num_of_filters);
extern bool              USBD_CDC7_NCM_SetEthernetPowerManagementPatternFilter (uint16_t filter_number, const uint8_t *pattern_filter, uint16_t pattern_filter_size);
extern bool              USBD_CDC7_NCM_GetEthernetPowerManagementPatternFilter (uint16_t filter_number, uint16_t *pattern_active);
extern bool              USBD_CDC7_NCM_SetEthernetPacketFilter                 (uint16_t packet_filter_bitmap);
extern bool              USBD_CDC7_NCM_GetEthernetStatistic                    (uint16_t feature_selector, uint32_t *data);
extern bool              USBD_CDC7_NCM_GetNtbParameters                        (CDC_NCM_NTB_PARAM *ntb_params);
extern bool              USBD_CDC7_NCM_GetNetAddress        (uint8_t *net_addr);
extern bool              USBD_CDC7_NCM_SetNetAddress        (const uint8_t *net_addr);
extern bool              USBD_CDC7_NCM_GetNtbFormat         (uint16_t *ntb_format);
extern bool              USBD_CDC7_NCM_SetNtbFormat         (uint16_t  ntb_format);
extern bool              USBD_CDC7_NCM_GetNtbInputSize      (uint32_t *ntb_input_size);
extern bool              USBD_CDC7_NCM_SetNtbInputSize      (uint32_t  ntb_input_size);
extern bool              USBD_CDC7_NCM_GetMaxDatagramSize   (uint16_t *max_datagram_size);
extern bool              USBD_CDC7_NCM_SetMaxDatagramSize   (uint16_t  max_datagram_size);
extern bool              USBD_CDC7_NCM_GetCrcMode           (uint16_t *crc_mode);
extern bool              USBD_CDC7_NCM_SetCrcMode           (uint16_t  crc_mode);
extern void              USBD_CDC7_NCM_NTB_IN_Sent          (void);
extern void              USBD_CDC7_NCM_NTB_OUT_Received     (void);
 
extern void              USBD_HID0_Initialize               (void);
extern void              USBD_HID0_Uninitialize             (void);
extern int32_t           USBD_HID0_GetReport                (uint8_t rtype, uint8_t req, uint8_t rid,       uint8_t *buf);
extern bool              USBD_HID0_SetReport                (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len);
 
extern void              USBD_HID1_Initialize               (void);
extern void              USBD_HID1_Uninitialize             (void);
extern int32_t           USBD_HID1_GetReport                (uint8_t rtype, uint8_t req, uint8_t rid,       uint8_t *buf);
extern bool              USBD_HID1_SetReport                (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len);
 
extern void              USBD_HID2_Initialize               (void);
extern void              USBD_HID2_Uninitialize             (void);
extern int32_t           USBD_HID2_GetReport                (uint8_t rtype, uint8_t req, uint8_t rid,       uint8_t *buf);
extern bool              USBD_HID2_SetReport                (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len);
 
extern void              USBD_HID3_Initialize               (void);
extern void              USBD_HID3_Uninitialize             (void);
extern int32_t           USBD_HID3_GetReport                (uint8_t rtype, uint8_t req, uint8_t rid,       uint8_t *buf);
extern bool              USBD_HID3_SetReport                (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len);

extern void              USBD_HID4_Initialize               (void);
extern void              USBD_HID4_Uninitialize             (void);
extern int32_t           USBD_HID4_GetReport                (uint8_t rtype, uint8_t req, uint8_t rid,       uint8_t *buf);
extern bool              USBD_HID4_SetReport                (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len);

extern void              USBD_HID5_Initialize               (void);
extern void              USBD_HID5_Uninitialize             (void);
extern int32_t           USBD_HID5_GetReport                (uint8_t rtype, uint8_t req, uint8_t rid,       uint8_t *buf);
extern bool              USBD_HID5_SetReport                (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len);

extern void              USBD_HID6_Initialize               (void);
extern void              USBD_HID6_Uninitialize             (void);
extern int32_t           USBD_HID6_GetReport                (uint8_t rtype, uint8_t req, uint8_t rid,       uint8_t *buf);
extern bool              USBD_HID6_SetReport                (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len);

extern void              USBD_HID7_Initialize               (void);
extern void              USBD_HID7_Uninitialize             (void);
extern int32_t           USBD_HID7_GetReport                (uint8_t rtype, uint8_t req, uint8_t rid,       uint8_t *buf);
extern bool              USBD_HID7_SetReport                (uint8_t rtype, uint8_t req, uint8_t rid, const uint8_t *buf, int32_t len);
 
extern void              USBD_MSC0_Initialize               (void);
extern void              USBD_MSC0_Uninitialize             (void);
extern bool              USBD_MSC0_GetCacheInfo             (uint32_t *buffer, uint32_t *size);
extern bool              USBD_MSC0_GetMediaCapacity         (uint32_t *block_count, uint32_t *block_size);
extern bool              USBD_MSC0_Read                     (uint32_t lba, uint32_t cnt,       uint8_t *buf);
extern bool              USBD_MSC0_Write                    (uint32_t lba, uint32_t cnt, const uint8_t *buf);
extern bool              USBD_MSC0_StartStop                (bool start);
extern uint32_t          USBD_MSC0_CheckMedia               (void);
extern uint8_t           USBD_MSC0_GetMaxLUN                (void);
extern bool              USBD_MSC0_LUN_GetMediaCapacity     (uint8_t lun, uint32_t *block_count, uint32_t *block_size);
extern bool              USBD_MSC0_LUN_Read                 (uint8_t lun, uint32_t lba, uint32_t cnt,       uint8_t *buf);
extern bool              USBD_MSC0_LUN_Write                (uint8_t lun, uint32_t lba, uint32_t cnt, const uint8_t *buf);
extern bool              USBD_MSC0_LUN_StartStop            (uint8_t lun, bool start);
extern uint32_t          USBD_MSC0_LUN_CheckMedia           (uint8_t lun);
 
extern void              USBD_MSC1_Initialize               (void);
extern void              USBD_MSC1_Uninitialize             (void);
extern bool              USBD_MSC1_GetCacheInfo             (uint32_t *buffer, uint32_t *size);
extern bool              USBD_MSC1_GetMediaCapacity         (uint32_t *block_count, uint32_t *block_size);
extern bool              USBD_MSC1_Read                     (uint32_t lba, uint32_t cnt,       uint8_t *buf);
extern bool              USBD_MSC1_Write                    (uint32_t lba, uint32_t cnt, const uint8_t *buf);
extern bool              USBD_MSC1_StartStop                (bool start);
extern uint32_t          USBD_MSC1_CheckMedia               (void);
extern uint8_t           USBD_MSC1_GetMaxLUN                (void);
extern bool              USBD_MSC1_LUN_GetMediaCapacity     (uint8_t lun, uint32_t *block_count, uint32_t *block_size);
extern bool              USBD_MSC1_LUN_Read                 (uint8_t lun, uint32_t lba, uint32_t cnt,       uint8_t *buf);
extern bool              USBD_MSC1_LUN_Write                (uint8_t lun, uint32_t lba, uint32_t cnt, const uint8_t *buf);
extern bool              USBD_MSC1_LUN_StartStop            (uint8_t lun, bool start);
extern uint32_t          USBD_MSC1_LUN_CheckMedia           (uint8_t lun);
 
extern void              USBD_MSC2_Initialize               (void);
extern void              USBD_MSC2_Uninitialize             (void);
extern bool              USBD_MSC2_GetCacheInfo             (uint32_t *buffer, uint32_t *size);
extern bool              USBD_MSC2_GetMediaCapacity         (uint32_t *block_count, uint32_t *block_size);
extern bool              USBD_MSC2_Read                     (uint32_t lba, uint32_t cnt,       uint8_t *buf);
extern bool              USBD_MSC2_Write                    (uint32_t lba, uint32_t cnt, const uint8_t *buf);
extern bool              USBD_MSC2_StartStop                (bool start);
extern uint32_t          USBD_MSC2_CheckMedia               (void);
extern uint8_t           USBD_MSC2_GetMaxLUN                (void);
extern bool              USBD_MSC2_LUN_GetMediaCapacity     (uint8_t lun, uint32_t *block_count, uint32_t *block_size);
extern bool              USBD_MSC2_LUN_Read                 (uint8_t lun, uint32_t lba, uint32_t cnt,       uint8_t *buf);
extern bool              USBD_MSC2_LUN_Write                (uint8_t lun, uint32_t lba, uint32_t cnt, const uint8_t *buf);
extern bool              USBD_MSC2_LUN_StartStop            (uint8_t lun, bool start);
extern uint32_t          USBD_MSC2_LUN_CheckMedia           (uint8_t lun);
 
extern void              USBD_MSC3_Initialize               (void);
extern void              USBD_MSC3_Uninitialize             (void);
extern bool              USBD_MSC3_GetCacheInfo             (uint32_t *buffer, uint32_t *size);
extern bool              USBD_MSC3_GetMediaCapacity         (uint32_t *block_count, uint32_t *block_size);
extern bool              USBD_MSC3_Read                     (uint32_t lba, uint32_t cnt,       uint8_t *buf);
extern bool              USBD_MSC3_Write                    (uint32_t lba, uint32_t cnt, const uint8_t *buf);
extern bool              USBD_MSC3_StartStop                (bool start);
extern uint32_t          USBD_MSC3_CheckMedia               (void);
extern uint8_t           USBD_MSC3_GetMaxLUN                (void);
extern bool              USBD_MSC3_LUN_GetMediaCapacity     (uint8_t lun, uint32_t *block_count, uint32_t *block_size);
extern bool              USBD_MSC3_LUN_Read                 (uint8_t lun, uint32_t lba, uint32_t cnt,       uint8_t *buf);
extern bool              USBD_MSC3_LUN_Write                (uint8_t lun, uint32_t lba, uint32_t cnt, const uint8_t *buf);
extern bool              USBD_MSC3_LUN_StartStop            (uint8_t lun, bool start);
extern uint32_t          USBD_MSC3_LUN_CheckMedia           (uint8_t lun);
 
extern void              USBD_CustomClass0_Initialize       (void);
extern void              USBD_CustomClass0_Uninitialize     (void);
extern void              USBD_CustomClass0_Reset            (void);
extern void              USBD_CustomClass0_EndpointStart    (uint8_t ep_addr);
extern void              USBD_CustomClass0_EndpointStop     (uint8_t ep_addr);
extern usbdRequestStatus USBD_CustomClass0_Endpoint0_SetupPacketReceived  (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);
extern void              USBD_CustomClass0_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet);
extern usbdRequestStatus USBD_CustomClass0_Endpoint0_OutDataReceived      (uint32_t len);
extern usbdRequestStatus USBD_CustomClass0_Endpoint0_InDataSent           (uint32_t len);
extern void              USBD_CustomClass0_Endpoint1_Event  (uint32_t event);
extern void              USBD_CustomClass0_Endpoint2_Event  (uint32_t event);
extern void              USBD_CustomClass0_Endpoint3_Event  (uint32_t event);
extern void              USBD_CustomClass0_Endpoint4_Event  (uint32_t event);
extern void              USBD_CustomClass0_Endpoint5_Event  (uint32_t event);
extern void              USBD_CustomClass0_Endpoint6_Event  (uint32_t event);
extern void              USBD_CustomClass0_Endpoint7_Event  (uint32_t event);
extern void              USBD_CustomClass0_Endpoint8_Event  (uint32_t event);
extern void              USBD_CustomClass0_Endpoint9_Event  (uint32_t event);
extern void              USBD_CustomClass0_Endpoint10_Event (uint32_t event);
extern void              USBD_CustomClass0_Endpoint11_Event (uint32_t event);
extern void              USBD_CustomClass0_Endpoint12_Event (uint32_t event);
extern void              USBD_CustomClass0_Endpoint13_Event (uint32_t event);
extern void              USBD_CustomClass0_Endpoint14_Event (uint32_t event);
extern void              USBD_CustomClass0_Endpoint15_Event (uint32_t event);
 
extern void              USBD_CustomClass1_Initialize       (void);
extern void              USBD_CustomClass1_Uninitialize     (void);
extern void              USBD_CustomClass1_Reset            (void);
extern void              USBD_CustomClass1_EndpointStart    (uint8_t ep_addr);
extern void              USBD_CustomClass1_EndpointStop     (uint8_t ep_addr);
extern usbdRequestStatus USBD_CustomClass1_Endpoint0_SetupPacketReceived  (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);
extern void              USBD_CustomClass1_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet);
extern usbdRequestStatus USBD_CustomClass1_Endpoint0_OutDataReceived      (uint32_t len);
extern usbdRequestStatus USBD_CustomClass1_Endpoint0_InDataSent           (uint32_t len);
extern void              USBD_CustomClass1_Endpoint1_Event  (uint32_t event);
extern void              USBD_CustomClass1_Endpoint2_Event  (uint32_t event);
extern void              USBD_CustomClass1_Endpoint3_Event  (uint32_t event);
extern void              USBD_CustomClass1_Endpoint4_Event  (uint32_t event);
extern void              USBD_CustomClass1_Endpoint5_Event  (uint32_t event);
extern void              USBD_CustomClass1_Endpoint6_Event  (uint32_t event);
extern void              USBD_CustomClass1_Endpoint7_Event  (uint32_t event);
extern void              USBD_CustomClass1_Endpoint8_Event  (uint32_t event);
extern void              USBD_CustomClass1_Endpoint9_Event  (uint32_t event);
extern void              USBD_CustomClass1_Endpoint10_Event (uint32_t event);
extern void              USBD_CustomClass1_Endpoint11_Event (uint32_t event);
extern void              USBD_CustomClass1_Endpoint12_Event (uint32_t event);
extern void              USBD_CustomClass1_Endpoint13_Event (uint32_t event);
extern void              USBD_CustomClass1_Endpoint14_Event (uint32_t event);
extern void              USBD_CustomClass1_Endpoint15_Event (uint32_t event);
 
extern void              USBD_CustomClass2_Initialize       (void);
extern void              USBD_CustomClass2_Uninitialize     (void);
extern void              USBD_CustomClass2_Reset            (void);
extern void              USBD_CustomClass2_EndpointStart    (uint8_t ep_addr);
extern void              USBD_CustomClass2_EndpointStop     (uint8_t ep_addr);
extern usbdRequestStatus USBD_CustomClass2_Endpoint0_SetupPacketReceived  (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);
extern void              USBD_CustomClass2_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet);
extern usbdRequestStatus USBD_CustomClass2_Endpoint0_OutDataReceived      (uint32_t len);
extern usbdRequestStatus USBD_CustomClass2_Endpoint0_InDataSent           (uint32_t len);
extern void              USBD_CustomClass2_Endpoint1_Event  (uint32_t event);
extern void              USBD_CustomClass2_Endpoint2_Event  (uint32_t event);
extern void              USBD_CustomClass2_Endpoint3_Event  (uint32_t event);
extern void              USBD_CustomClass2_Endpoint4_Event  (uint32_t event);
extern void              USBD_CustomClass2_Endpoint5_Event  (uint32_t event);
extern void              USBD_CustomClass2_Endpoint6_Event  (uint32_t event);
extern void              USBD_CustomClass2_Endpoint7_Event  (uint32_t event);
extern void              USBD_CustomClass2_Endpoint8_Event  (uint32_t event);
extern void              USBD_CustomClass2_Endpoint9_Event  (uint32_t event);
extern void              USBD_CustomClass2_Endpoint10_Event (uint32_t event);
extern void              USBD_CustomClass2_Endpoint11_Event (uint32_t event);
extern void              USBD_CustomClass2_Endpoint12_Event (uint32_t event);
extern void              USBD_CustomClass2_Endpoint13_Event (uint32_t event);
extern void              USBD_CustomClass2_Endpoint14_Event (uint32_t event);
extern void              USBD_CustomClass2_Endpoint15_Event (uint32_t event);
 
extern void              USBD_CustomClass3_Initialize       (void);
extern void              USBD_CustomClass3_Uninitialize     (void);
extern void              USBD_CustomClass3_Reset            (void);
extern void              USBD_CustomClass3_EndpointStart    (uint8_t ep_addr);
extern void              USBD_CustomClass3_EndpointStop     (uint8_t ep_addr);
extern usbdRequestStatus USBD_CustomClass3_Endpoint0_SetupPacketReceived  (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len);
extern void              USBD_CustomClass3_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet);
extern usbdRequestStatus USBD_CustomClass3_Endpoint0_OutDataReceived      (uint32_t len);
extern usbdRequestStatus USBD_CustomClass3_Endpoint0_InDataSent           (uint32_t len);
extern void              USBD_CustomClass3_Endpoint1_Event  (uint32_t event);
extern void              USBD_CustomClass3_Endpoint2_Event  (uint32_t event);
extern void              USBD_CustomClass3_Endpoint3_Event  (uint32_t event);
extern void              USBD_CustomClass3_Endpoint4_Event  (uint32_t event);
extern void              USBD_CustomClass3_Endpoint5_Event  (uint32_t event);
extern void              USBD_CustomClass3_Endpoint6_Event  (uint32_t event);
extern void              USBD_CustomClass3_Endpoint7_Event  (uint32_t event);
extern void              USBD_CustomClass3_Endpoint8_Event  (uint32_t event);
extern void              USBD_CustomClass3_Endpoint9_Event  (uint32_t event);
extern void              USBD_CustomClass3_Endpoint10_Event (uint32_t event);
extern void              USBD_CustomClass3_Endpoint11_Event (uint32_t event);
extern void              USBD_CustomClass3_Endpoint12_Event (uint32_t event);
extern void              USBD_CustomClass3_Endpoint13_Event (uint32_t event);
extern void              USBD_CustomClass3_Endpoint14_Event (uint32_t event);
extern void              USBD_CustomClass3_Endpoint15_Event (uint32_t event);

// USB Device library function prototypes for configuration generation
extern usbStatus         USBD_CustomClass_Initialize        (uint8_t instance);
extern usbStatus         USBD_CustomClass_Uninitialize      (uint8_t instance);
extern void              USBD_CustomClass_Reset             (uint8_t instance);
extern void              USBD_CustomClass_EndpointStart     (uint8_t instance, uint8_t ep_addr);
extern void              USBD_CustomClass_EndpointStop      (uint8_t instance, uint8_t ep_addr);

extern usbStatus         USBD_ADC_Initialize                (uint8_t instance);
extern usbStatus         USBD_ADC_Uninitialize              (uint8_t instance);
extern void              USBD_ADC_Reset                     (uint8_t instance);
extern void              USBD_ADC_EndpointStart             (uint8_t instance, uint8_t ep_addr);
extern void              USBD_ADC_EndpointStop              (uint8_t instance, uint8_t ep_addr);

extern usbStatus         USBD_CDC_Initialize                (uint8_t instance);
extern usbStatus         USBD_CDC_Uninitialize              (uint8_t instance);
extern void              USBD_CDC_Reset                     (uint8_t instance);
extern void              USBD_CDC_EndpointStart             (uint8_t instance, uint8_t ep_addr);
extern void              USBD_CDC_EndpointStop              (uint8_t instance, uint8_t ep_addr);

extern usbStatus         USBD_HID_Initialize                (uint8_t instance);
extern usbStatus         USBD_HID_Uninitialize              (uint8_t instance);
extern void              USBD_HID_Reset                     (uint8_t instance);
extern void              USBD_HID_SetConfiguration          (uint8_t instance);
extern void              USBD_HID_EndpointStart             (uint8_t instance, uint8_t ep_addr);
extern void              USBD_HID_Timer                     (void const *argument);

extern usbStatus         USBD_MSC_Initialize                (uint8_t instance);
extern usbStatus         USBD_MSC_Uninitialize              (uint8_t instance);
extern void              USBD_MSC_Reset                     (uint8_t instance);
extern void              USBD_MSC_EndpointStart             (uint8_t instance, uint8_t ep_addr);

// USB Device library configuration generated function prototypes
extern void              USBD_Debug_Initialize              (uint8_t device);

extern usbStatus         USBD_ClassInitialize               (uint8_t device);
extern usbStatus         USBD_ClassUninitialize             (uint8_t device);
extern void              USBD_Reset                         (uint8_t device);
extern void              USBD_SetConfiguration              (uint8_t device, uint8_t configuration);
extern void              USBD_EndpointStart                 (uint8_t device, uint8_t ep_addr);
extern void              USBD_EndpointStop                  (uint8_t device, uint8_t ep_addr);

// USB Device thread function prototypes
extern void              USBD_Core_Thread                   (void       *arg);
extern void              USBD0_Core_Thread                  (void const *arg);
extern void              USBD1_Core_Thread                  (void const *arg);
extern void              USBD2_Core_Thread                  (void const *arg);
extern void              USBD3_Core_Thread                  (void const *arg);

extern void              USBD_ADC_Thread                    (void       *arg);
extern void              USBD_ADC0_Thread                   (void const *arg);
extern void              USBD_ADC1_Thread                   (void const *arg);
extern void              USBD_ADC2_Thread                   (void const *arg);
extern void              USBD_ADC3_Thread                   (void const *arg);

extern void              USBD_CDC_Int_Thread                (void       *arg);
extern void              USBD_CDC0_Int_Thread               (void const *arg);
extern void              USBD_CDC1_Int_Thread               (void const *arg);
extern void              USBD_CDC2_Int_Thread               (void const *arg);
extern void              USBD_CDC3_Int_Thread               (void const *arg);
extern void              USBD_CDC4_Int_Thread               (void const *arg);
extern void              USBD_CDC5_Int_Thread               (void const *arg);
extern void              USBD_CDC6_Int_Thread               (void const *arg);
extern void              USBD_CDC7_Int_Thread               (void const *arg);

extern void              USBD_CDC_Bulk_Thread               (void       *arg);
extern void              USBD_CDC0_Bulk_Thread              (void const *arg);
extern void              USBD_CDC1_Bulk_Thread              (void const *arg);
extern void              USBD_CDC2_Bulk_Thread              (void const *arg);
extern void              USBD_CDC3_Bulk_Thread              (void const *arg);
extern void              USBD_CDC4_Bulk_Thread              (void const *arg);
extern void              USBD_CDC5_Bulk_Thread              (void const *arg);
extern void              USBD_CDC6_Bulk_Thread              (void const *arg);
extern void              USBD_CDC7_Bulk_Thread              (void const *arg);

extern void              USBD_HID_Thread                    (void       *arg);
extern void              USBD_HID0_Thread                   (void const *arg);
extern void              USBD_HID1_Thread                   (void const *arg);
extern void              USBD_HID2_Thread                   (void const *arg);
extern void              USBD_HID3_Thread                   (void const *arg);
extern void              USBD_HID4_Thread                   (void const *arg);
extern void              USBD_HID5_Thread                   (void const *arg);
extern void              USBD_HID6_Thread                   (void const *arg);
extern void              USBD_HID7_Thread                   (void const *arg);

extern void              USBD_MSC_Thread                    (void       *arg);
extern void              USBD_MSC0_Thread                   (void const *arg);
extern void              USBD_MSC1_Thread                   (void const *arg);
extern void              USBD_MSC2_Thread                   (void const *arg);
extern void              USBD_MSC3_Thread                   (void const *arg);

extern void              USBD_CustomClass_EP_Thread         (void       *arg);

// USB Host library function prototypes
extern void              USBH_Debug_Initialize              (uint8_t ctrl);

extern uint8_t           USBH_CDC_Configure                 (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc);
extern usbStatus         USBH_CDC_Unconfigure               (uint8_t instance);
extern usbStatus         USBH_CDC_Initialize_Lib            (uint8_t instance);
extern usbStatus         USBH_CDC_Uninitialize_Lib          (uint8_t instance);

extern uint8_t           USBH_HID_Configure                 (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc);
extern usbStatus         USBH_HID_Unconfigure               (uint8_t instance);
extern usbStatus         USBH_HID_Initialize_Lib            (uint8_t instance);
extern usbStatus         USBH_HID_Uninitialize_Lib          (uint8_t instance);

extern uint8_t           USBH_MSC_Configure                 (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc);
extern usbStatus         USBH_MSC_Unconfigure               (uint8_t instance);
extern usbStatus         USBH_MSC_Initialize_Lib            (uint8_t instance);
extern usbStatus         USBH_MSC_Uninitialize_Lib          (uint8_t instance);

extern uint8_t           USBH_CustomClass_Configure         (uint8_t device, const USB_DEVICE_DESCRIPTOR *ptr_dev_desc, const USB_CONFIGURATION_DESCRIPTOR *ptr_cfg_desc);
extern usbStatus         USBH_CustomClass_Unconfigure       (uint8_t instance);
extern usbStatus         USBH_CustomClass_Initialize        (uint8_t instance);
extern usbStatus         USBH_CustomClass_Uninitialize      (uint8_t instance);

// USB Host thread function prototypes
extern void              USBH_Core_Thread                   (void       *arg);

extern void              USBH_CDC_IntIn_Thread              (void       *arg);

extern void              USBH_HID_IntIn_Thread              (void       *arg);

// USB Host timer function prototypes
extern void              USBH_ConnectDebounce               (void       *arg);

#ifdef __cplusplus
}
#endif

#endif  // RL_USB_H_
