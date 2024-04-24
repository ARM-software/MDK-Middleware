/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbd_evr.h
 * Purpose: USB Device (USBD) - Event Recorder definitions
 *----------------------------------------------------------------------------*/

#ifndef __USBD_EVR_H__
#define __USBD_EVR_H__

#include "RTE_Components.h"

#include "rl_usb.h"
#include "Driver_USBD.h"

#ifdef    RTE_CMSIS_View_EventRecorder
#include "EventRecorder.h"
#endif

// Define component numbers
#define EvtCompNo_USBD_Core                                         0xA0U   /// < USB Device Core
#define EvtCompNo_USBD_Driver                                       0xA1U   /// < USB Device Driver
#define EvtCompNo_USBD_CC                                           0xA2U   /// < USB Device Custom Class
#define EvtCompNo_USBD_ADC                                          0xA3U   /// < USB Device ADC
#define EvtCompNo_USBD_CDC                                          0xA4U   /// < USB Device CDC
#define EvtCompNo_USBD_HID                                          0xA5U   /// < USB Device HID
#define EvtCompNo_USBD_MSC                                          0xA6U   /// < USB Device MSC

#if (defined(USBD_DEBUG) && (USBD_DEBUG == 1) && defined(RTE_CMSIS_View_EventRecorder))
#define USBD_EVR_USED                   1
#else
#define USBD_EVR_USED                   0
#endif

#if    (USBD_EVR_USED == 1)                                         // Define USB Device Library debug events

// Define message numbers
#define EvtMsgNo_USBD_Core_Initialize                               0x00U
#define EvtMsgNo_USBD_Core_InitializeFailed                         0x01U
#define EvtMsgNo_USBD_Core_Uninitialize                             0x02U
#define EvtMsgNo_USBD_Core_UninitializeFailed                       0x03U
#define EvtMsgNo_USBD_Core_Connect                                  0x04U
#define EvtMsgNo_USBD_Core_ConnectFailed                            0x05U
#define EvtMsgNo_USBD_Core_Disconnect                               0x06U
#define EvtMsgNo_USBD_Core_DisconnectFailed                         0x07U
#define EvtMsgNo_USBD_Core_Configured                               0x08U
#define EvtMsgNo_USBD_Core_GetState                                 0x09U
#define EvtMsgNo_USBD_Core_SetSerialNumber                          0x0AU
#define EvtMsgNo_USBD_Core_SetSerialNumberFailed                    0x0BU
#define EvtMsgNo_USBD_Core_EndpointRead                             0x0CU
#define EvtMsgNo_USBD_Core_EndpointReadFailed                       0x0DU
#define EvtMsgNo_USBD_Core_EndpointReadGetResult                    0x0EU
#define EvtMsgNo_USBD_Core_EndpointWrite                            0x0FU
#define EvtMsgNo_USBD_Core_EndpointWriteFailed                      0x10U
#define EvtMsgNo_USBD_Core_EndpointWriteGetResult                   0x11U
#define EvtMsgNo_USBD_Core_EndpointStall                            0x12U
#define EvtMsgNo_USBD_Core_EndpointStallFailed                      0x13U
#define EvtMsgNo_USBD_Core_EndpointAbort                            0x14U
#define EvtMsgNo_USBD_Core_EndpointAbortFailed                      0x15U
#define EvtMsgNo_USBD_Core_ConfigureEp0                             0x16U
#define EvtMsgNo_USBD_Core_ConfigureEp0Failed                       0x17U
#define EvtMsgNo_USBD_Core_ReadSetupPacket                          0x18U
#define EvtMsgNo_USBD_Core_ReadSetupPacketFailed                    0x19U
#define EvtMsgNo_USBD_Core_ReqGetStatusFailed                       0x1AU
#define EvtMsgNo_USBD_Core_ReqSetClrFeatureFailed                   0x1BU
#define EvtMsgNo_USBD_Core_ReqSetAddressFailed                      0x1CU
#define EvtMsgNo_USBD_Core_ReqGetDescriptorFailed                   0x1DU
#define EvtMsgNo_USBD_Core_ReqGetMSDescriptorFailed                 0x1EU
#define EvtMsgNo_USBD_Core_ReqGetConfigurationFailed                0x1FU
#define EvtMsgNo_USBD_Core_ReqSetConfigurationFailed                0x20U
#define EvtMsgNo_USBD_Core_ReqGetInterfaceFailed                    0x21U
#define EvtMsgNo_USBD_Core_ReqSetInterfaceFailed                    0x22U
#define EvtMsgNo_USBD_Core_OnInitialize                             0x23U
#define EvtMsgNo_USBD_Core_OnUninitialize                           0x24U
#define EvtMsgNo_USBD_Core_OnVbusChanged                            0x25U
#define EvtMsgNo_USBD_Core_OnReset                                  0x26U
#define EvtMsgNo_USBD_Core_OnHighSpeedActivated                     0x27U
#define EvtMsgNo_USBD_Core_OnSuspended                              0x28U
#define EvtMsgNo_USBD_Core_OnResumed                                0x29U
#define EvtMsgNo_USBD_Core_OnConfigurationChanged                   0x2AU
#define EvtMsgNo_USBD_Core_OnEnableRemoteWakeup                     0x2BU
#define EvtMsgNo_USBD_Core_OnDisableRemoteWakeup                    0x2CU
#define EvtMsgNo_USBD_Core_OnSetupPacketReceived                    0x2DU
#define EvtMsgNo_USBD_Core_OnSetupPacketReceivedDetail              0x2EU
#define EvtMsgNo_USBD_Core_OnSetupPacketProcessed                   0x2FU
#define EvtMsgNo_USBD_Core_OnSetupPacketProcessedDetail             0x30U
#define EvtMsgNo_USBD_Core_OnOutDataReceived                        0x31U
#define EvtMsgNo_USBD_Core_OnInDataSent                             0x32U

#define EvtMsgNo_USBD_Driver_Initialize                             0x00U
#define EvtMsgNo_USBD_Driver_InitializeFailed                       0x01U
#define EvtMsgNo_USBD_Driver_Uninitialize                           0x02U
#define EvtMsgNo_USBD_Driver_UninitializeFailed                     0x03U
#define EvtMsgNo_USBD_Driver_PowerControl                           0x04U
#define EvtMsgNo_USBD_Driver_PowerControlFailed                     0x05U
#define EvtMsgNo_USBD_Driver_DeviceConnect                          0x06U
#define EvtMsgNo_USBD_Driver_DeviceConnectFailed                    0x07U
#define EvtMsgNo_USBD_Driver_DeviceDisconnect                       0x08U
#define EvtMsgNo_USBD_Driver_DeviceDisconnectFailed                 0x09U
#define EvtMsgNo_USBD_Driver_DeviceGetState                         0x0AU
#define EvtMsgNo_USBD_Driver_DeviceRemoteWakeup                     0x0BU
#define EvtMsgNo_USBD_Driver_DeviceRemoteWakeupFailed               0x0CU
#define EvtMsgNo_USBD_Driver_DeviceSetAddress                       0x0DU
#define EvtMsgNo_USBD_Driver_DeviceSetAddressFailed                 0x0EU
#define EvtMsgNo_USBD_Driver_ReadSetupPacket                        0x0FU
#define EvtMsgNo_USBD_Driver_ReadSetupPacketFailed                  0x10U
#define EvtMsgNo_USBD_Driver_EndpointConfigure                      0x11U
#define EvtMsgNo_USBD_Driver_EndpointConfigureFailed                0x12U
#define EvtMsgNo_USBD_Driver_EndpointUnconfigure                    0x13U
#define EvtMsgNo_USBD_Driver_EndpointUnconfigureFailed              0x14U
#define EvtMsgNo_USBD_Driver_EndpointStall                          0x15U
#define EvtMsgNo_USBD_Driver_EndpointStallFailed                    0x16U
#define EvtMsgNo_USBD_Driver_EndpointTransfer                       0x17U
#define EvtMsgNo_USBD_Driver_EndpointTransferFailed                 0x18U
#define EvtMsgNo_USBD_Driver_EndpointTransferGetResult              0x19U
#define EvtMsgNo_USBD_Driver_EndpointTransferAbort                  0x1AU
#define EvtMsgNo_USBD_Driver_EndpointTransferAbortFailed            0x1BU
#define EvtMsgNo_USBD_Driver_GetFrameNumber                         0x1CU
#define EvtMsgNo_USBD_Driver_OnSignalDeviceEvent                    0x1DU
#define EvtMsgNo_USBD_Driver_OnSignalEndpointEvent                  0x1EU

#define EvtMsgNo_USBD_CC_Initialize                                 0x00U
#define EvtMsgNo_USBD_CC_Uninitialize                               0x01U
#define EvtMsgNo_USBD_CC_Reset                                      0x02U
#define EvtMsgNo_USBD_CC_EndpointStart                              0x03U
#define EvtMsgNo_USBD_CC_EndpointStop                               0x04U
#define EvtMsgNo_USBD_CC_Endpoint0SetupPacketReceived               0x05U
#define EvtMsgNo_USBD_CC_Endpoint0SetupPacketReceivedDetail         0x06U
#define EvtMsgNo_USBD_CC_Endpoint0SetupPacketProcessed              0x07U
#define EvtMsgNo_USBD_CC_Endpoint0SetupPacketProcessedDetail        0x08U
#define EvtMsgNo_USBD_CC_Endpoint0OutDataReceived                   0x09U
#define EvtMsgNo_USBD_CC_Endpoint0InDataSent                        0x0AU
#define EvtMsgNo_USBD_CC_EndpointmEvent                             0x0BU

#define EvtMsgNo_USBD_ADC_Initialize                                0x00U
#define EvtMsgNo_USBD_ADC_InitializeFailed                          0x01U
#define EvtMsgNo_USBD_ADC_Uninitialize                              0x02U
#define EvtMsgNo_USBD_ADC_UninitializeFailed                        0x03U
#define EvtMsgNo_USBD_ADC_Reset                                     0x04U
#define EvtMsgNo_USBD_ADC_ResetFailed                               0x05U
#define EvtMsgNo_USBD_ADC_OnInitialize                              0x06U
#define EvtMsgNo_USBD_ADC_OnUninitialize                            0x07U
#define EvtMsgNo_USBD_ADC_OnPlayStart                               0x08U
#define EvtMsgNo_USBD_ADC_OnPlayStop                                0x09U
#define EvtMsgNo_USBD_ADC_OnSetSpeakerMute                          0x0AU
#define EvtMsgNo_USBD_ADC_OnSetSpeakerVolume                        0x0BU
#define EvtMsgNo_USBD_ADC_OnRecordStart                             0x0CU
#define EvtMsgNo_USBD_ADC_OnRecordStop                              0x0DU
#define EvtMsgNo_USBD_ADC_OnSetMicrophoneMute                       0x0EU
#define EvtMsgNo_USBD_ADC_OnSetMicrophoneVolume                     0x0FU
#define EvtMsgNo_USBD_ADC_OnReceivedSamples                         0x10U
#define EvtMsgNo_USBD_ADC_SetSpeakerVolumeRange                     0x11U
#define EvtMsgNo_USBD_ADC_SetSpeakerVolumeRangeFailed               0x12U
#define EvtMsgNo_USBD_ADC_SetMicrophoneVolumeRange                  0x13U
#define EvtMsgNo_USBD_ADC_SetMicrophoneVolumeRangeFailed            0x14U
#define EvtMsgNo_USBD_ADC_ReceivedSamplesAvailable                  0x15U
#define EvtMsgNo_USBD_ADC_WrittenSamplesPending                     0x16U
#define EvtMsgNo_USBD_ADC_ReadSamples                               0x17U
#define EvtMsgNo_USBD_ADC_ReadSamplesFailed                         0x18U
#define EvtMsgNo_USBD_ADC_WriteSamples                              0x19U
#define EvtMsgNo_USBD_ADC_WriteSamplesFailed                        0x1AU

#define EvtMsgNo_USBD_CDC_Initialize                                0x00U
#define EvtMsgNo_USBD_CDC_InitializeFailed                          0x01U
#define EvtMsgNo_USBD_CDC_Uninitialize                              0x02U
#define EvtMsgNo_USBD_CDC_UninitializeFailed                        0x03U
#define EvtMsgNo_USBD_CDC_Reset                                     0x04U
#define EvtMsgNo_USBD_CDC_ResetFailed                               0x05U
#define EvtMsgNo_USBD_CDC_ACM_OnInitialize                          0x06U
#define EvtMsgNo_USBD_CDC_ACM_OnUninitialize                        0x07U
#define EvtMsgNo_USBD_CDC_ACM_OnReset                               0x08U
#define EvtMsgNo_USBD_CDC_ACM_OnSetLineCoding                       0x09U
#define EvtMsgNo_USBD_CDC_ACM_OnGetLineCoding                       0x0AU
#define EvtMsgNo_USBD_CDC_ACM_OnSetControlLineState                 0x0BU
#define EvtMsgNo_USBD_CDC_ACM_ReadData                              0x0CU
#define EvtMsgNo_USBD_CDC_ACM_ReadDataFailed                        0x0DU
#define EvtMsgNo_USBD_CDC_ACM_WriteData                             0x0EU
#define EvtMsgNo_USBD_CDC_ACM_WriteDataFailed                       0x0FU
#define EvtMsgNo_USBD_CDC_ACM_GetChar                               0x10U
#define EvtMsgNo_USBD_CDC_ACM_PutChar                               0x11U
#define EvtMsgNo_USBD_CDC_ACM_DataAvailable                         0x12U
#define EvtMsgNo_USBD_CDC_ACM_DataAvailableFailed                   0x13U
#define EvtMsgNo_USBD_CDC_ACM_Notify_SerialState                    0x14U
#define EvtMsgNo_USBD_CDC_ACM_Notify_SerialStateFailed              0x15U
#define EvtMsgNo_USBD_CDC_NCM_OnInitialize                          0x16U
#define EvtMsgNo_USBD_CDC_NCM_OnUninitialize                        0x17U
#define EvtMsgNo_USBD_CDC_NCM_OnReset                               0x18U
#define EvtMsgNo_USBD_CDC_NCM_OnStart                               0x19U
#define EvtMsgNo_USBD_CDC_NCM_OnStop                                0x1AU
#define EvtMsgNo_USBD_CDC_NCM_OnSetEthMulticastFilters              0x1BU
#define EvtMsgNo_USBD_CDC_NCM_OnSetEthPowerMgmtPatFilter            0x1CU
#define EvtMsgNo_USBD_CDC_NCM_OnGetEthPowerMgmtPatFilter            0x1DU
#define EvtMsgNo_USBD_CDC_NCM_OnSetEthernetPacketFilter             0x1EU
#define EvtMsgNo_USBD_CDC_NCM_OnGetEthernetStatistic                0x1FU
#define EvtMsgNo_USBD_CDC_NCM_OnGetNtbParameters                    0x20U
#define EvtMsgNo_USBD_CDC_NCM_OnGetNetAddress                       0x21U
#define EvtMsgNo_USBD_CDC_NCM_OnSetNetAddress                       0x22U
#define EvtMsgNo_USBD_CDC_NCM_OnGetNtbFormat                        0x23U
#define EvtMsgNo_USBD_CDC_NCM_OnSetNtbFormat                        0x24U
#define EvtMsgNo_USBD_CDC_NCM_OnGetNtbInputSize                     0x25U
#define EvtMsgNo_USBD_CDC_NCM_OnSetNtbInputSize                     0x26U
#define EvtMsgNo_USBD_CDC_NCM_OnGetMaxDatagramSize                  0x27U
#define EvtMsgNo_USBD_CDC_NCM_OnSetMaxDatagramSize                  0x28U
#define EvtMsgNo_USBD_CDC_NCM_OnGetCrcMode                          0x29U
#define EvtMsgNo_USBD_CDC_NCM_OnSetCrcMode                          0x2AU
#define EvtMsgNo_USBD_CDC_NCM_OnNTB_IN_Sent                         0x2BU
#define EvtMsgNo_USBD_CDC_NCM_OnNTB_OUT_Received                    0x2CU
#define EvtMsgNo_USBD_CDC_NCM_NTB_IN_Initialize                     0x2DU
#define EvtMsgNo_USBD_CDC_NCM_NTB_IN_InitializeFailed               0x2EU
#define EvtMsgNo_USBD_CDC_NCM_NTB_IN_CreateNDP                      0x2FU
#define EvtMsgNo_USBD_CDC_NCM_NTB_IN_CreateNDPFailed                0x30U
#define EvtMsgNo_USBD_CDC_NCM_NTB_IN_WriteDatagram                  0x31U
#define EvtMsgNo_USBD_CDC_NCM_NTB_IN_WriteDatagramFailed            0x32U
#define EvtMsgNo_USBD_CDC_NCM_NTB_IN_Send                           0x33U
#define EvtMsgNo_USBD_CDC_NCM_NTB_IN_SendFailed                     0x34U
#define EvtMsgNo_USBD_CDC_NCM_NTB_IN_IsSent                         0x35U
#define EvtMsgNo_USBD_CDC_NCM_NTB_IN_IsSentFailed                   0x36U
#define EvtMsgNo_USBD_CDC_NCM_NTB_OUT_IsReceived                    0x37U
#define EvtMsgNo_USBD_CDC_NCM_NTB_OUT_IsReceivedFailed              0x38U
#define EvtMsgNo_USBD_CDC_NCM_NTB_OUT_Release                       0x39U
#define EvtMsgNo_USBD_CDC_NCM_NTB_OUT_ReleaseFailed                 0x3AU
#define EvtMsgNo_USBD_CDC_NCM_NTB_OUT_ProcessNDP                    0x3BU
#define EvtMsgNo_USBD_CDC_NCM_NTB_OUT_ProcessNDPFailed              0x3CU
#define EvtMsgNo_USBD_CDC_NCM_NTB_OUT_GetDatagramSize               0x3DU
#define EvtMsgNo_USBD_CDC_NCM_NTB_OUT_ReadDatagram                  0x3EU
#define EvtMsgNo_USBD_CDC_NCM_NTB_OUT_ReadDatagramFailed            0x3FU
#define EvtMsgNo_USBD_CDC_NCM_NTB_IN_RawSend                        0x40U
#define EvtMsgNo_USBD_CDC_NCM_NTB_IN_RawSendFailed                  0x41U
#define EvtMsgNo_USBD_CDC_NCM_NTB_OUT_RawGetSize                    0x42U
#define EvtMsgNo_USBD_CDC_NCM_NTB_OUT_RawReceive                    0x43U
#define EvtMsgNo_USBD_CDC_NCM_NTB_OUT_RawReceiveFailed              0x44U
#define EvtMsgNo_USBD_CDC_NCM_Notify_NetworkConnection              0x45U
#define EvtMsgNo_USBD_CDC_NCM_Notify_NetworkConnectionFailed        0x46U
#define EvtMsgNo_USBD_CDC_NCM_Notify_ConnectionSpeedChange          0x47U
#define EvtMsgNo_USBD_CDC_NCM_Notify_ConnectionSpeedChangeFailed    0x48U
#define EvtMsgNo_USBD_CDC_ACM_Notify_ResponseAvailable              0x49U
#define EvtMsgNo_USBD_CDC_ACM_Notify_ResponseAvailableFailed        0x50U
#define EvtMsgNo_USBD_CDC_ACM_OnSendEncapsulatedCommand             0x51U
#define EvtMsgNo_USBD_CDC_ACM_OnGetEncapsulatedResponse             0x52U

#define EvtMsgNo_USBD_HID_Initialize                                0x00U
#define EvtMsgNo_USBD_HID_InitializeFailed                          0x01U
#define EvtMsgNo_USBD_HID_Uninitialize                              0x02U
#define EvtMsgNo_USBD_HID_UninitializeFailed                        0x03U
#define EvtMsgNo_USBD_HID_Reset                                     0x04U
#define EvtMsgNo_USBD_HID_ResetFailed                               0x05U
#define EvtMsgNo_USBD_HID_GetReport                                 0x06U
#define EvtMsgNo_USBD_HID_GetReportFailed                           0x07U
#define EvtMsgNo_USBD_HID_SetReport                                 0x08U
#define EvtMsgNo_USBD_HID_SetReportFailed                           0x09U
#define EvtMsgNo_USBD_HID_GetIdle                                   0x0AU
#define EvtMsgNo_USBD_HID_GetIdleFailed                             0x0BU
#define EvtMsgNo_USBD_HID_SetIdle                                   0x0CU
#define EvtMsgNo_USBD_HID_SetIdleFailed                             0x0DU
#define EvtMsgNo_USBD_HID_GetProtocol                               0x0EU
#define EvtMsgNo_USBD_HID_GetProtocolFailed                         0x0FU
#define EvtMsgNo_USBD_HID_SetProtocol                               0x10U
#define EvtMsgNo_USBD_HID_SetProtocolFailed                         0x11U
#define EvtMsgNo_USBD_HID_OnInitialize                              0x12U
#define EvtMsgNo_USBD_HID_OnUninitialize                            0x13U
#define EvtMsgNo_USBD_HID_OnGetReport                               0x14U
#define EvtMsgNo_USBD_HID_OnGetReportFailed                         0x15U
#define EvtMsgNo_USBD_HID_OnSetReport                               0x16U
#define EvtMsgNo_USBD_HID_OnSetReportFailed                         0x17U
#define EvtMsgNo_USBD_HID_OnGetProtocol                             0x18U
#define EvtMsgNo_USBD_HID_OnGetProtocolFailed                       0x19U
#define EvtMsgNo_USBD_HID_OnSetProtocol                             0x1AU
#define EvtMsgNo_USBD_HID_OnSetProtocolFailed                       0x1BU
#define EvtMsgNo_USBD_HID_GetReportTrigger                          0x1CU
#define EvtMsgNo_USBD_HID_GetReportTriggerFailed                    0x1DU

#define EvtMsgNo_USBD_MSC_Initialize                                0x00U
#define EvtMsgNo_USBD_MSC_InitializeFailed                          0x01U
#define EvtMsgNo_USBD_MSC_Uninitialize                              0x02U
#define EvtMsgNo_USBD_MSC_UninitializeFailed                        0x03U
#define EvtMsgNo_USBD_MSC_Reset                                     0x04U
#define EvtMsgNo_USBD_MSC_ResetFailed                               0x05U
#define EvtMsgNo_USBD_MSC_BOMSReset                                 0x06U
#define EvtMsgNo_USBD_MSC_BOMSResetFailed                           0x07U
#define EvtMsgNo_USBD_MSC_GetMaxLUN                                 0x08U
#define EvtMsgNo_USBD_MSC_GetMaxLUNFailed                           0x09U
#define EvtMsgNo_USBD_MSC_TestUnitReady                             0x0AU
#define EvtMsgNo_USBD_MSC_RequestSense                              0x0BU
#define EvtMsgNo_USBD_MSC_Inquiry                                   0x0CU
#define EvtMsgNo_USBD_MSC_StartStopUnit                             0x0DU
#define EvtMsgNo_USBD_MSC_MediaRemoval                              0x0EU
#define EvtMsgNo_USBD_MSC_ModeSense                                 0x0FU
#define EvtMsgNo_USBD_MSC_ReadFormatCapacities                      0x10U
#define EvtMsgNo_USBD_MSC_ReadCapacity                              0x11U
#define EvtMsgNo_USBD_MSC_Read                                      0x12U
#define EvtMsgNo_USBD_MSC_ReadDone                                  0x13U
#define EvtMsgNo_USBD_MSC_Write                                     0x14U
#define EvtMsgNo_USBD_MSC_WriteDone                                 0x15U
#define EvtMsgNo_USBD_MSC_Verify                                    0x16U
#define EvtMsgNo_USBD_MSC_VerifyDone                                0x17U
#define EvtMsgNo_USBD_MSC_SyncCache                                 0x18U
#define EvtMsgNo_USBD_MSC_UnsupportedCommand                        0x19U
#define EvtMsgNo_USBD_MSC_OnInitialize                              0x1AU
#define EvtMsgNo_USBD_MSC_OnUninitialize                            0x1BU
#define EvtMsgNo_USBD_MSC_OnGetCacheInfo                            0x1CU
#define EvtMsgNo_USBD_MSC_OnGetCacheInfoFailed                      0x1DU
#define EvtMsgNo_USBD_MSC_OnGetMediaCapacity                        0x1EU
#define EvtMsgNo_USBD_MSC_OnGetMediaCapacityFailed                  0x1FU
#define EvtMsgNo_USBD_MSC_OnRead                                    0x20U
#define EvtMsgNo_USBD_MSC_OnReadFailed                              0x21U
#define EvtMsgNo_USBD_MSC_OnWrite                                   0x22U
#define EvtMsgNo_USBD_MSC_OnWriteFailed                             0x23U
#define EvtMsgNo_USBD_MSC_OnCheckMedia                              0x24U

// Pack parameter in byte
#define TO_BYTE0(x)                                                 (((uint32_t)(x) & 0xFFU))
#define TO_BYTE1(x)                                                 (((uint32_t)(x) & 0xFFU) <<  8)
#define TO_BYTE2(x)                                                 (((uint32_t)(x) & 0xFFU) << 16)
#define TO_BYTE3(x)                                                 (((uint32_t)(x) & 0xFFU) << 24)

// Event id list for "USBD_Core" component
#define EvtUSBD_Core_Initialize                                     EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_Initialize)
#define EvtUSBD_Core_InitializeFailed                               EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_InitializeFailed)
#define EvtUSBD_Core_Uninitialize                                   EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_Uninitialize)
#define EvtUSBD_Core_UninitializeFailed                             EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_UninitializeFailed)
#define EvtUSBD_Core_Connect                                        EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_Connect)
#define EvtUSBD_Core_ConnectFailed                                  EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ConnectFailed)
#define EvtUSBD_Core_Disconnect                                     EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_Disconnect)
#define EvtUSBD_Core_DisconnectFailed                               EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_DisconnectFailed)
#define EvtUSBD_Core_Configured                                     EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_Configured)
#define EvtUSBD_Core_GetState                                       EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_GetState)
#define EvtUSBD_Core_SetSerialNumber                                EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_SetSerialNumber)
#define EvtUSBD_Core_SetSerialNumberFailed                          EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_SetSerialNumberFailed)
#define EvtUSBD_Core_EndpointRead                                   EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_EndpointRead)
#define EvtUSBD_Core_EndpointReadFailed                             EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_EndpointReadFailed)
#define EvtUSBD_Core_EndpointReadGetResult                          EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_EndpointReadGetResult)
#define EvtUSBD_Core_EndpointWrite                                  EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_EndpointWrite)
#define EvtUSBD_Core_EndpointWriteFailed                            EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_EndpointWriteFailed)
#define EvtUSBD_Core_EndpointWriteGetResult                         EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_EndpointWriteGetResult)
#define EvtUSBD_Core_EndpointStall                                  EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_EndpointStall)
#define EvtUSBD_Core_EndpointStallFailed                            EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_EndpointStallFailed)
#define EvtUSBD_Core_EndpointAbort                                  EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_EndpointAbort)
#define EvtUSBD_Core_EndpointAbortFailed                            EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_EndpointAbortFailed)
#define EvtUSBD_Core_ConfigureEp0                                   EventID(EventLevelOp,     EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ConfigureEp0)
#define EvtUSBD_Core_ConfigureEp0Failed                             EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ConfigureEp0Failed)
#define EvtUSBD_Core_ReadSetupPacket                                EventID(EventLevelDetail, EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ReadSetupPacket)
#define EvtUSBD_Core_ReadSetupPacketFailed                          EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ReadSetupPacketFailed)
#define EvtUSBD_Core_ReqGetStatusFailed                             EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ReqGetStatusFailed)
#define EvtUSBD_Core_ReqSetClrFeatureFailed                         EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ReqSetClrFeatureFailed)
#define EvtUSBD_Core_ReqSetAddressFailed                            EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ReqSetAddressFailed)
#define EvtUSBD_Core_ReqGetDescriptorFailed                         EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ReqGetDescriptorFailed)
#define EvtUSBD_Core_ReqGetMSDescriptorFailed                       EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ReqGetMSDescriptorFailed)
#define EvtUSBD_Core_ReqGetConfigurationFailed                      EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ReqGetConfigurationFailed)
#define EvtUSBD_Core_ReqSetConfigurationFailed                      EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ReqSetConfigurationFailed)
#define EvtUSBD_Core_ReqGetInterfaceFailed                          EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ReqGetInterfaceFailed)
#define EvtUSBD_Core_ReqSetInterfaceFailed                          EventID(EventLevelError,  EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_ReqSetInterfaceFailed)
#define EvtUSBD_Core_OnInitialize                                   EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnInitialize)
#define EvtUSBD_Core_OnUninitialize                                 EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnUninitialize)
#define EvtUSBD_Core_OnVbusChanged                                  EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnVbusChanged)
#define EvtUSBD_Core_OnReset                                        EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnReset)
#define EvtUSBD_Core_OnHighSpeedActivated                           EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnHighSpeedActivated)
#define EvtUSBD_Core_OnSuspended                                    EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnSuspended)
#define EvtUSBD_Core_OnResumed                                      EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnResumed)
#define EvtUSBD_Core_OnConfigurationChanged                         EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnConfigurationChanged)
#define EvtUSBD_Core_OnEnableRemoteWakeup                           EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnEnableRemoteWakeup)
#define EvtUSBD_Core_OnDisableRemoteWakeup                          EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnDisableRemoteWakeup)
#define EvtUSBD_Core_OnSetupPacketReceived                          EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnSetupPacketReceived)
#define EvtUSBD_Core_OnSetupPacketReceivedDetail                    EventID(EventLevelDetail, EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnSetupPacketReceivedDetail)
#define EvtUSBD_Core_OnSetupPacketProcessed                         EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnSetupPacketProcessed)
#define EvtUSBD_Core_OnSetupPacketProcessedDetail                   EventID(EventLevelDetail, EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnSetupPacketProcessedDetail)
#define EvtUSBD_Core_OnOutDataReceived                              EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnOutDataReceived)
#define EvtUSBD_Core_OnInDataSent                                   EventID(EventLevelAPI,    EvtCompNo_USBD_Core,    EvtMsgNo_USBD_Core_OnInDataSent)

// Event id list for "USBD_Driver" component
#define EvtUSBD_Driver_Initialize                                   EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_Initialize)
#define EvtUSBD_Driver_InitializeFailed                             EventID(EventLevelError,  EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_InitializeFailed)
#define EvtUSBD_Driver_Uninitialize                                 EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_Uninitialize)
#define EvtUSBD_Driver_UninitializeFailed                           EventID(EventLevelError,  EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_UninitializeFailed)
#define EvtUSBD_Driver_PowerControl                                 EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_PowerControl)
#define EvtUSBD_Driver_PowerControlFailed                           EventID(EventLevelError,  EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_PowerControlFailed)
#define EvtUSBD_Driver_DeviceConnect                                EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_DeviceConnect)
#define EvtUSBD_Driver_DeviceConnectFailed                          EventID(EventLevelError,  EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_DeviceConnectFailed)
#define EvtUSBD_Driver_DeviceDisconnect                             EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_DeviceDisconnect)
#define EvtUSBD_Driver_DeviceDisconnectFailed                       EventID(EventLevelError,  EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_DeviceDisconnectFailed)
#define EvtUSBD_Driver_DeviceGetState                               EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_DeviceGetState)
#define EvtUSBD_Driver_DeviceRemoteWakeup                           EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_DeviceRemoteWakeup)
#define EvtUSBD_Driver_DeviceRemoteWakeupFailed                     EventID(EventLevelError,  EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_DeviceRemoteWakeupFailed)
#define EvtUSBD_Driver_DeviceSetAddress                             EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_DeviceSetAddress)
#define EvtUSBD_Driver_DeviceSetAddressFailed                       EventID(EventLevelError,  EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_DeviceSetAddressFailed)
#define EvtUSBD_Driver_ReadSetupPacket                              EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_ReadSetupPacket)
#define EvtUSBD_Driver_ReadSetupPacketFailed                        EventID(EventLevelError,  EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_ReadSetupPacketFailed)
#define EvtUSBD_Driver_EndpointConfigure                            EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_EndpointConfigure)
#define EvtUSBD_Driver_EndpointConfigureFailed                      EventID(EventLevelError,  EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_EndpointConfigureFailed)
#define EvtUSBD_Driver_EndpointUnconfigure                          EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_EndpointUnconfigure)
#define EvtUSBD_Driver_EndpointUnconfigureFailed                    EventID(EventLevelError,  EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_EndpointUnconfigureFailed)
#define EvtUSBD_Driver_EndpointStall                                EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_EndpointStall)
#define EvtUSBD_Driver_EndpointStallFailed                          EventID(EventLevelError,  EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_EndpointStallFailed)
#define EvtUSBD_Driver_EndpointTransfer                             EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_EndpointTransfer)
#define EvtUSBD_Driver_EndpointTransferFailed                       EventID(EventLevelError,  EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_EndpointTransferFailed)
#define EvtUSBD_Driver_EndpointTransferGetResult                    EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_EndpointTransferGetResult)
#define EvtUSBD_Driver_EndpointTransferAbort                        EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_EndpointTransferAbort)
#define EvtUSBD_Driver_EndpointTransferAbortFailed                  EventID(EventLevelError,  EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_EndpointTransferAbortFailed)
#define EvtUSBD_Driver_GetFrameNumber                               EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_GetFrameNumber)
#define EvtUSBD_Driver_OnSignalDeviceEvent                          EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_OnSignalDeviceEvent)
#define EvtUSBD_Driver_OnSignalEndpointEvent                        EventID(EventLevelAPI,    EvtCompNo_USBD_Driver,  EvtMsgNo_USBD_Driver_OnSignalEndpointEvent)

// Event id list for "USBD_CC" component
#define EvtUSBD_CC_OnInitialize                                     EventID(EventLevelAPI,    EvtCompNo_USBD_CC,      EvtMsgNo_USBD_CC_Initialize)
#define EvtUSBD_CC_OnUninitialize                                   EventID(EventLevelAPI,    EvtCompNo_USBD_CC,      EvtMsgNo_USBD_CC_Uninitialize)
#define EvtUSBD_CC_OnReset                                          EventID(EventLevelAPI,    EvtCompNo_USBD_CC,      EvtMsgNo_USBD_CC_Reset)
#define EvtUSBD_CC_OnEndpointStart                                  EventID(EventLevelAPI,    EvtCompNo_USBD_CC,      EvtMsgNo_USBD_CC_EndpointStart)
#define EvtUSBD_CC_OnEndpointStop                                   EventID(EventLevelAPI,    EvtCompNo_USBD_CC,      EvtMsgNo_USBD_CC_EndpointStop)
#define EvtUSBD_CC_OnEndpoint0SetupPacketReceived                   EventID(EventLevelAPI,    EvtCompNo_USBD_CC,      EvtMsgNo_USBD_CC_Endpoint0SetupPacketReceived)
#define EvtUSBD_CC_OnEndpoint0SetupPacketReceivedDetail             EventID(EventLevelDetail, EvtCompNo_USBD_CC,      EvtMsgNo_USBD_CC_Endpoint0SetupPacketReceivedDetail)
#define EvtUSBD_CC_OnEndpoint0SetupPacketProcessed                  EventID(EventLevelAPI,    EvtCompNo_USBD_CC,      EvtMsgNo_USBD_CC_Endpoint0SetupPacketProcessed)
#define EvtUSBD_CC_OnEndpoint0SetupPacketProcessedDetail            EventID(EventLevelDetail, EvtCompNo_USBD_CC,      EvtMsgNo_USBD_CC_Endpoint0SetupPacketProcessedDetail)
#define EvtUSBD_CC_OnEndpoint0OutDataReceived                       EventID(EventLevelAPI,    EvtCompNo_USBD_CC,      EvtMsgNo_USBD_CC_Endpoint0OutDataReceived)
#define EvtUSBD_CC_OnEndpoint0InDataSent                            EventID(EventLevelAPI,    EvtCompNo_USBD_CC,      EvtMsgNo_USBD_CC_Endpoint0InDataSent)
#define EvtUSBD_CC_OnEndpointmEvent                                 EventID(EventLevelAPI,    EvtCompNo_USBD_CC,      EvtMsgNo_USBD_CC_EndpointmEvent)

// Event id list for "USBD_ADC" component
#define EvtUSBD_ADC_Initialize                                      EventID(EventLevelOp,     EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_Initialize)
#define EvtUSBD_ADC_InitializeFailed                                EventID(EventLevelError,  EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_InitializeFailed)
#define EvtUSBD_ADC_Uninitialize                                    EventID(EventLevelOp,     EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_Uninitialize)
#define EvtUSBD_ADC_UninitializeFailed                              EventID(EventLevelError,  EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_UninitializeFailed)
#define EvtUSBD_ADC_Reset                                           EventID(EventLevelOp,     EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_Reset)
#define EvtUSBD_ADC_ResetFailed                                     EventID(EventLevelError,  EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_ResetFailed)
#define EvtUSBD_ADC_OnInitialize                                    EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_OnInitialize)
#define EvtUSBD_ADC_OnUninitialize                                  EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_OnUninitialize)
#define EvtUSBD_ADC_OnPlayStart                                     EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_OnPlayStart)
#define EvtUSBD_ADC_OnPlayStop                                      EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_OnPlayStop)
#define EvtUSBD_ADC_OnSetSpeakerMute                                EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_OnSetSpeakerMute)
#define EvtUSBD_ADC_OnSetSpeakerVolume                              EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_OnSetSpeakerVolume)
#define EvtUSBD_ADC_OnRecordStart                                   EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_OnRecordStart)
#define EvtUSBD_ADC_OnRecordStop                                    EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_OnRecordStop)
#define EvtUSBD_ADC_OnSetMicrophoneMute                             EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_OnSetMicrophoneMute)
#define EvtUSBD_ADC_OnSetMicrophoneVolume                           EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_OnSetMicrophoneVolume)
#define EvtUSBD_ADC_OnReceivedSamples                               EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_OnReceivedSamples)
#define EvtUSBD_ADC_SetSpeakerVolumeRange                           EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_SetSpeakerVolumeRange)
#define EvtUSBD_ADC_SetSpeakerVolumeRangeFailed                     EventID(EventLevelError,  EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_SetSpeakerVolumeRangeFailed)
#define EvtUSBD_ADC_SetMicrophoneVolumeRange                        EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_SetMicrophoneVolumeRange)
#define EvtUSBD_ADC_SetMicrophoneVolumeRangeFailed                  EventID(EventLevelError,  EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_SetMicrophoneVolumeRangeFailed)
#define EvtUSBD_ADC_ReceivedSamplesAvailable                        EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_ReceivedSamplesAvailable)
#define EvtUSBD_ADC_WrittenSamplesPending                           EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_WrittenSamplesPending)
#define EvtUSBD_ADC_ReadSamples                                     EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_ReadSamples)
#define EvtUSBD_ADC_ReadSamplesFailed                               EventID(EventLevelError,  EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_ReadSamplesFailed)
#define EvtUSBD_ADC_WriteSamples                                    EventID(EventLevelAPI,    EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_WriteSamples)
#define EvtUSBD_ADC_WriteSamplesFailed                              EventID(EventLevelError,  EvtCompNo_USBD_ADC,     EvtMsgNo_USBD_ADC_WriteSamplesFailed)

// Event id list for "USBD_CDC" component
#define EvtUSBD_CDC_Initialize                                      EventID(EventLevelOp,     EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_Initialize)
#define EvtUSBD_CDC_InitializeFailed                                EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_InitializeFailed)
#define EvtUSBD_CDC_Uninitialize                                    EventID(EventLevelOp,     EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_Uninitialize)
#define EvtUSBD_CDC_UninitializeFailed                              EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_UninitializeFailed)
#define EvtUSBD_CDC_Reset                                           EventID(EventLevelOp,     EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_Reset)
#define EvtUSBD_CDC_ResetFailed                                     EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ResetFailed)
#define EvtUSBD_CDC_ACM_OnInitialize                                EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_OnInitialize)
#define EvtUSBD_CDC_ACM_OnUninitialize                              EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_OnUninitialize)
#define EvtUSBD_CDC_ACM_OnReset                                     EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_OnReset)
#define EvtUSBD_CDC_ACM_OnSendEncapsulatedCommand                   EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_OnSendEncapsulatedCommand)
#define EvtUSBD_CDC_ACM_OnGetEncapsulatedResponse                   EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_OnGetEncapsulatedResponse)
#define EvtUSBD_CDC_ACM_OnSetLineCoding                             EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_OnSetLineCoding)
#define EvtUSBD_CDC_ACM_OnGetLineCoding                             EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_OnGetLineCoding)
#define EvtUSBD_CDC_ACM_OnSetControlLineState                       EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_OnSetControlLineState)
#define EvtUSBD_CDC_ACM_ReadData                                    EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_ReadData)
#define EvtUSBD_CDC_ACM_ReadDataFailed                              EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_ReadDataFailed)
#define EvtUSBD_CDC_ACM_WriteData                                   EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_WriteData)
#define EvtUSBD_CDC_ACM_WriteDataFailed                             EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_WriteDataFailed)
#define EvtUSBD_CDC_ACM_GetChar                                     EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_GetChar)
#define EvtUSBD_CDC_ACM_PutChar                                     EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_PutChar)
#define EvtUSBD_CDC_ACM_DataAvailable                               EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_DataAvailable)
#define EvtUSBD_CDC_ACM_DataAvailableFailed                         EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_DataAvailableFailed)
#define EvtUSBD_CDC_ACM_Notify_SerialState                          EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_Notify_SerialState)
#define EvtUSBD_CDC_ACM_Notify_SerialStateFailed                    EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_Notify_SerialStateFailed)
#define EvtUSBD_CDC_ACM_Notify_ResponseAvailable                    EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_Notify_ResponseAvailable)
#define EvtUSBD_CDC_ACM_Notify_ResponseAvailableFailed              EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_ACM_Notify_ResponseAvailableFailed)
#define EvtUSBD_CDC_NCM_OnInitialize                                EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnInitialize)
#define EvtUSBD_CDC_NCM_OnUninitialize                              EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnUninitialize)
#define EvtUSBD_CDC_NCM_OnReset                                     EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnReset)
#define EvtUSBD_CDC_NCM_OnStart                                     EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnStart)
#define EvtUSBD_CDC_NCM_OnStop                                      EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnStop)
#define EvtUSBD_CDC_NCM_OnSetEthMulticastFilters                    EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnSetEthMulticastFilters)
#define EvtUSBD_CDC_NCM_OnSetEthPowerMgmtPatFilter                  EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnSetEthPowerMgmtPatFilter)
#define EvtUSBD_CDC_NCM_OnGetEthPowerMgmtPatFilter                  EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnGetEthPowerMgmtPatFilter)
#define EvtUSBD_CDC_NCM_OnSetEthernetPacketFilter                   EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnSetEthernetPacketFilter)
#define EvtUSBD_CDC_NCM_OnGetEthernetStatistic                      EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnGetEthernetStatistic)
#define EvtUSBD_CDC_NCM_OnGetNtbParameters                          EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnGetNtbParameters)
#define EvtUSBD_CDC_NCM_OnGetNetAddress                             EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnGetNetAddress)
#define EvtUSBD_CDC_NCM_OnSetNetAddress                             EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnSetNetAddress)
#define EvtUSBD_CDC_NCM_OnGetNtbFormat                              EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnGetNtbFormat)
#define EvtUSBD_CDC_NCM_OnSetNtbFormat                              EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnSetNtbFormat)
#define EvtUSBD_CDC_NCM_OnGetNtbInputSize                           EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnGetNtbInputSize)
#define EvtUSBD_CDC_NCM_OnSetNtbInputSize                           EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnSetNtbInputSize)
#define EvtUSBD_CDC_NCM_OnGetMaxDatagramSize                        EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnGetMaxDatagramSize)
#define EvtUSBD_CDC_NCM_OnSetMaxDatagramSize                        EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnSetMaxDatagramSize)
#define EvtUSBD_CDC_NCM_OnGetCrcMode                                EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnGetCrcMode)
#define EvtUSBD_CDC_NCM_OnSetCrcMode                                EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnSetCrcMode)
#define EvtUSBD_CDC_NCM_OnNTB_IN_Sent                               EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnNTB_IN_Sent)
#define EvtUSBD_CDC_NCM_OnNTB_OUT_Received                          EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_OnNTB_OUT_Received)
#define EvtUSBD_CDC_NCM_NTB_IN_Initialize                           EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_IN_Initialize)
#define EvtUSBD_CDC_NCM_NTB_IN_InitializeFailed                     EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_IN_InitializeFailed)
#define EvtUSBD_CDC_NCM_NTB_IN_CreateNDP                            EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_IN_CreateNDP)
#define EvtUSBD_CDC_NCM_NTB_IN_CreateNDPFailed                      EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_IN_CreateNDPFailed)
#define EvtUSBD_CDC_NCM_NTB_IN_WriteDatagram                        EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_IN_WriteDatagram)
#define EvtUSBD_CDC_NCM_NTB_IN_WriteDatagramFailed                  EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_IN_WriteDatagramFailed)
#define EvtUSBD_CDC_NCM_NTB_IN_Send                                 EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_IN_Send)
#define EvtUSBD_CDC_NCM_NTB_IN_SendFailed                           EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_IN_SendFailed)
#define EvtUSBD_CDC_NCM_NTB_IN_IsSent                               EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_IN_IsSent)
#define EvtUSBD_CDC_NCM_NTB_IN_IsSentFailed                         EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_IN_IsSentFailed)
#define EvtUSBD_CDC_NCM_NTB_OUT_IsReceived                          EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_OUT_IsReceived)
#define EvtUSBD_CDC_NCM_NTB_OUT_IsReceivedFailed                    EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_OUT_IsReceivedFailed)
#define EvtUSBD_CDC_NCM_NTB_OUT_Release                             EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_OUT_Release)
#define EvtUSBD_CDC_NCM_NTB_OUT_ReleaseFailed                       EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_OUT_ReleaseFailed)
#define EvtUSBD_CDC_NCM_NTB_OUT_ProcessNDP                          EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_OUT_ProcessNDP)
#define EvtUSBD_CDC_NCM_NTB_OUT_ProcessNDPFailed                    EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_OUT_ProcessNDPFailed)
#define EvtUSBD_CDC_NCM_NTB_OUT_GetDatagramSize                     EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_OUT_GetDatagramSize)
#define EvtUSBD_CDC_NCM_NTB_OUT_ReadDatagram                        EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_OUT_ReadDatagram)
#define EvtUSBD_CDC_NCM_NTB_OUT_ReadDatagramFailed                  EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_OUT_ReadDatagramFailed)
#define EvtUSBD_CDC_NCM_NTB_IN_RawSend                              EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_IN_RawSend)
#define EvtUSBD_CDC_NCM_NTB_IN_RawSendFailed                        EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_IN_RawSendFailed)
#define EvtUSBD_CDC_NCM_NTB_OUT_RawGetSize                          EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_OUT_RawGetSize)
#define EvtUSBD_CDC_NCM_NTB_OUT_RawReceive                          EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_OUT_RawReceive)
#define EvtUSBD_CDC_NCM_NTB_OUT_RawReceiveFailed                    EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_NTB_OUT_RawReceiveFailed)
#define EvtUSBD_CDC_NCM_Notify_NetworkConnection                    EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_Notify_NetworkConnection)
#define EvtUSBD_CDC_NCM_Notify_NetworkConnectionFailed              EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_Notify_NetworkConnectionFailed)
#define EvtUSBD_CDC_NCM_Notify_ConnectionSpeedChange                EventID(EventLevelAPI,    EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_Notify_ConnectionSpeedChange)
#define EvtUSBD_CDC_NCM_Notify_ConnectionSpeedChangeFailed          EventID(EventLevelError,  EvtCompNo_USBD_CDC,     EvtMsgNo_USBD_CDC_NCM_Notify_ConnectionSpeedChangeFailed)

// Event id list for "USBD_HID" component
#define EvtUSBD_HID_Initialize                                      EventID(EventLevelOp,     EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_Initialize)
#define EvtUSBD_HID_InitializeFailed                                EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_InitializeFailed)
#define EvtUSBD_HID_Uninitialize                                    EventID(EventLevelOp,     EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_Uninitialize)
#define EvtUSBD_HID_UninitializeFailed                              EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_UninitializeFailed)
#define EvtUSBD_HID_Reset                                           EventID(EventLevelOp,     EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_Reset)
#define EvtUSBD_HID_ResetFailed                                     EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_ResetFailed)
#define EvtUSBD_HID_GetReport                                       EventID(EventLevelOp,     EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_GetReport)
#define EvtUSBD_HID_GetReportFailed                                 EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_GetReportFailed)
#define EvtUSBD_HID_SetReport                                       EventID(EventLevelOp,     EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_SetReport)
#define EvtUSBD_HID_SetReportFailed                                 EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_SetReportFailed)
#define EvtUSBD_HID_GetIdle                                         EventID(EventLevelOp,     EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_GetIdle)
#define EvtUSBD_HID_GetIdleFailed                                   EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_GetIdleFailed)
#define EvtUSBD_HID_SetIdle                                         EventID(EventLevelOp,     EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_SetIdle)
#define EvtUSBD_HID_SetIdleFailed                                   EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_SetIdleFailed)
#define EvtUSBD_HID_GetProtocol                                     EventID(EventLevelOp,     EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_GetProtocol)
#define EvtUSBD_HID_GetProtocolFailed                               EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_GetProtocolFailed)
#define EvtUSBD_HID_SetProtocol                                     EventID(EventLevelOp,     EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_SetProtocol)
#define EvtUSBD_HID_SetProtocolFailed                               EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_SetProtocolFailed)
#define EvtUSBD_HID_OnInitialize                                    EventID(EventLevelAPI,    EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_OnInitialize)
#define EvtUSBD_HID_OnUninitialize                                  EventID(EventLevelAPI,    EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_OnUninitialize)
#define EvtUSBD_HID_OnGetReport                                     EventID(EventLevelAPI,    EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_OnGetReport)
#define EvtUSBD_HID_OnGetReportFailed                               EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_OnGetReportFailed)
#define EvtUSBD_HID_OnSetReport                                     EventID(EventLevelAPI,    EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_OnSetReport)
#define EvtUSBD_HID_OnSetReportFailed                               EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_OnSetReportFailed)
#define EvtUSBD_HID_OnGetProtocol                                   EventID(EventLevelAPI,    EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_OnGetProtocol)
#define EvtUSBD_HID_OnGetProtocolFailed                             EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_OnGetProtocolFailed)
#define EvtUSBD_HID_OnSetProtocol                                   EventID(EventLevelAPI,    EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_OnSetProtocol)
#define EvtUSBD_HID_OnSetProtocolFailed                             EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_OnSetProtocolFailed)
#define EvtUSBD_HID_GetReportTrigger                                EventID(EventLevelAPI,    EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_GetReportTrigger)
#define EvtUSBD_HID_GetReportTriggerFailed                          EventID(EventLevelError,  EvtCompNo_USBD_HID,     EvtMsgNo_USBD_HID_GetReportTriggerFailed)

// Event id list for "USBD_MSC" component
#define EvtUSBD_MSC_Initialize                                      EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_Initialize)
#define EvtUSBD_MSC_InitializeFailed                                EventID(EventLevelError,  EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_InitializeFailed)
#define EvtUSBD_MSC_Uninitialize                                    EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_Uninitialize)
#define EvtUSBD_MSC_UninitializeFailed                              EventID(EventLevelError,  EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_UninitializeFailed)
#define EvtUSBD_MSC_Reset                                           EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_Reset)
#define EvtUSBD_MSC_ResetFailed                                     EventID(EventLevelError,  EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_ResetFailed)
#define EvtUSBD_MSC_BOMSReset                                       EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_BOMSReset)
#define EvtUSBD_MSC_BOMSResetFailed                                 EventID(EventLevelError,  EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_BOMSResetFailed)
#define EvtUSBD_MSC_GetMaxLUN                                       EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_GetMaxLUN)
#define EvtUSBD_MSC_GetMaxLUNFailed                                 EventID(EventLevelError,  EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_GetMaxLUNFailed)
#define EvtUSBD_MSC_TestUnitReady                                   EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_TestUnitReady)
#define EvtUSBD_MSC_RequestSense                                    EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_RequestSense)
#define EvtUSBD_MSC_Inquiry                                         EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_Inquiry)
#define EvtUSBD_MSC_StartStopUnit                                   EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_StartStopUnit)
#define EvtUSBD_MSC_MediaRemoval                                    EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_MediaRemoval)
#define EvtUSBD_MSC_ModeSense                                       EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_ModeSense)
#define EvtUSBD_MSC_ReadFormatCapacities                            EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_ReadFormatCapacities)
#define EvtUSBD_MSC_ReadCapacity                                    EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_ReadCapacity)
#define EvtUSBD_MSC_Read                                            EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_Read)
#define EvtUSBD_MSC_ReadDone                                        EventID(EventLevelDetail, EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_ReadDone)
#define EvtUSBD_MSC_Write                                           EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_Write)
#define EvtUSBD_MSC_WriteDone                                       EventID(EventLevelDetail, EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_WriteDone)
#define EvtUSBD_MSC_Verify                                          EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_Verify)
#define EvtUSBD_MSC_VerifyDone                                      EventID(EventLevelDetail, EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_VerifyDone)
#define EvtUSBD_MSC_SyncCache                                       EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_SyncCache)
#define EvtUSBD_MSC_UnsupportedCommand                              EventID(EventLevelOp,     EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_UnsupportedCommand)
#define EvtUSBD_MSC_OnInitialize                                    EventID(EventLevelAPI,    EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_OnInitialize)
#define EvtUSBD_MSC_OnUninitialize                                  EventID(EventLevelAPI,    EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_OnUninitialize)
#define EvtUSBD_MSC_OnGetCacheInfo                                  EventID(EventLevelAPI,    EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_OnGetCacheInfo)
#define EvtUSBD_MSC_OnGetCacheInfoFailed                            EventID(EventLevelError,  EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_OnGetCacheInfoFailed)
#define EvtUSBD_MSC_OnGetMediaCapacity                              EventID(EventLevelAPI,    EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_OnGetMediaCapacity)
#define EvtUSBD_MSC_OnGetMediaCapacityFailed                        EventID(EventLevelError,  EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_OnGetMediaCapacityFailed)
#define EvtUSBD_MSC_OnRead                                          EventID(EventLevelAPI,    EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_OnRead)
#define EvtUSBD_MSC_OnReadFailed                                    EventID(EventLevelError,  EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_OnReadFailed)
#define EvtUSBD_MSC_OnWrite                                         EventID(EventLevelAPI,    EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_OnWrite)
#define EvtUSBD_MSC_OnWriteFailed                                   EventID(EventLevelError,  EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_OnWriteFailed)
#define EvtUSBD_MSC_OnCheckMedia                                    EventID(EventLevelDetail, EvtCompNo_USBD_MSC,     EvtMsgNo_USBD_MSC_OnCheckMedia)

#endif // (USBD_EVR_USED == 1)

// USB Device Core component event record functions ----------------------------

/**
  \brief  Event on \ref USBD_Initialize start (API)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_Initialize
  __STATIC_INLINE void EvrUSBD_Core_Initialize(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_Initialize, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_Initialize(...)
#endif


/**
  \brief  Event on \ref USBD_Initialize failed (Error)
  \param  device       device index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_Core_InitializeFailed
  __STATIC_INLINE void EvrUSBD_Core_InitializeFailed(  uint8_t device,  usbStatus error) {
    EventRecord2(      EvtUSBD_Core_InitializeFailed, TO_BYTE0(device), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Core_InitializeFailed(...)
#endif


/**
  \brief  Event on \ref USBD_Uninitialize start (API)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_Uninitialize
  __STATIC_INLINE void EvrUSBD_Core_Uninitialize(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_Uninitialize, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_Uninitialize(...)
#endif


/**
  \brief  Event on \ref USBD_Uninitialize failed (Error)
  \param  device       device index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_Core_UninitializeFailed
  __STATIC_INLINE void EvrUSBD_Core_UninitializeFailed(  uint8_t device,  usbStatus error) {
    EventRecord2(      EvtUSBD_Core_UninitializeFailed, TO_BYTE0(device), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Core_UninitializeFailed(...)
#endif


/**
  \brief  Event on \ref USBD_Connect start (API)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_Connect
  __STATIC_INLINE void EvrUSBD_Core_Connect(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_Connect, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_Connect(...)
#endif


/**
  \brief  Event on \ref USBD_Connect failed (Error)
  \param  device       device index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_Core_ConnectFailed
  __STATIC_INLINE void EvrUSBD_Core_ConnectFailed(  uint8_t device,  usbStatus error) {
    EventRecord2(      EvtUSBD_Core_ConnectFailed, TO_BYTE0(device), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Core_ConnectFailed(...)
#endif


/**
  \brief  Event on \ref USBD_Disconnect start (API)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_Disconnect
  __STATIC_INLINE void EvrUSBD_Core_Disconnect(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_Disconnect, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_Disconnect(...)
#endif


/**
  \brief  Event on \ref USBD_Disconnect failed (Error)
  \param  device       device index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_Core_DisconnectFailed
  __STATIC_INLINE void EvrUSBD_Core_DisconnectFailed(  uint8_t device,  usbStatus error) {
    EventRecord2(      EvtUSBD_Core_DisconnectFailed, TO_BYTE0(device), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Core_DisconnectFailed(...)
#endif


/**
  \brief  Event on \ref USBD_Configured finished (API)
  \param  device       device index
  \param  configured   configuration state
                        - false: not configured
                        - true:  configured
 */
#ifdef                 EvtUSBD_Core_Configured
  __STATIC_INLINE void EvrUSBD_Core_Configured(  uint8_t device,       bool configured) {
    EventRecord2(      EvtUSBD_Core_Configured, TO_BYTE0(device), (uint32_t)configured);
  }
#else
  #define              EvrUSBD_Core_Configured(...)
#endif


/**
  \brief  Event on \ref USBD_GetState finished (API)
  \param  device       device index
  \param  state        device state \ref USBD_STATE
 */
#ifdef                 EvtUSBD_Core_GetState
  __STATIC_INLINE void EvrUSBD_Core_GetState(  uint8_t device,  USBD_STATE state) {
    EventRecord2(      EvtUSBD_Core_GetState, TO_BYTE0(device), (uint32_t)state.vbus | ((uint32_t)state.speed << 1) | ((uint32_t)state.active << 3));
  }
#else
  #define              EvrUSBD_Core_GetState(...)
#endif


/**
  \brief  Event on \ref USBD_SetSerialNumber finished (API)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_SetSerialNumber
  __STATIC_INLINE void EvrUSBD_Core_SetSerialNumber(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_SetSerialNumber, TO_BYTE0(device), 0);
  }
#else
  #define              EvrUSBD_Core_SetSerialNumber(...)
#endif


/**
  \brief  Event on \ref USBD_SetSerialNumber failed (Error)
  \param  device       device index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_Core_SetSerialNumberFailed
  __STATIC_INLINE void EvrUSBD_Core_SetSerialNumberFailed(  uint8_t device,  usbStatus error) {
    EventRecord2(      EvtUSBD_Core_SetSerialNumberFailed, TO_BYTE0(device), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Core_SetSerialNumberFailed(...)
#endif


/**
  \brief  Event on \ref USBD_EndpointRead start (API)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  len          maximum number of bytes to receive
 */
#ifdef                 EvtUSBD_Core_EndpointRead
  __STATIC_INLINE void EvrUSBD_Core_EndpointRead(  uint8_t device,    uint8_t ep_addr,  uint32_t len) {
    EventRecord2(      EvtUSBD_Core_EndpointRead, TO_BYTE0(device) | TO_BYTE1(ep_addr),          len);
  }
#else
  #define              EvrUSBD_Core_EndpointRead(...)
#endif


/**
  \brief  Event on \ref USBD_EndpointRead failed (Error)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_Core_EndpointReadFailed
  __STATIC_INLINE void EvrUSBD_Core_EndpointReadFailed(  uint8_t device,    uint8_t ep_addr,  usbStatus error) {
    EventRecord2(      EvtUSBD_Core_EndpointReadFailed, TO_BYTE0(device) | TO_BYTE1(ep_addr), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Core_EndpointReadFailed(...)
#endif


/**
  \brief  Event on \ref USBD_EndpointReadGetResult finished (API)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  result       number of bytes received
 */
#ifdef                 EvtUSBD_Core_EndpointReadGetResult
  __STATIC_INLINE void EvrUSBD_Core_EndpointReadGetResult(  uint8_t device,    uint8_t ep_addr,   uint32_t result) {
    EventRecord2(      EvtUSBD_Core_EndpointReadGetResult, TO_BYTE0(device) | TO_BYTE1(ep_addr),           result);
  }
#else
  #define              EvrUSBD_Core_EndpointReadGetResult(...)
#endif


/**
  \brief  Event on \ref USBD_EndpointWrite start (API)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  len          maximum number of bytes to write
 */
#ifdef                 EvtUSBD_Core_EndpointWrite
  __STATIC_INLINE void EvrUSBD_Core_EndpointWrite(  uint8_t device,    uint8_t ep_addr,  uint32_t len) {
    EventRecord2(      EvtUSBD_Core_EndpointWrite, TO_BYTE0(device) | TO_BYTE1(ep_addr),          len);
  }
#else
  #define              EvrUSBD_Core_EndpointWrite(...)
#endif


/**
  \brief  Event on \ref USBD_EndpointWrite failed (Error)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_Core_EndpointWriteFailed
  __STATIC_INLINE void EvrUSBD_Core_EndpointWriteFailed(  uint8_t device,    uint8_t ep_addr,  usbStatus error) {
    EventRecord2(      EvtUSBD_Core_EndpointWriteFailed, TO_BYTE0(device) | TO_BYTE1(ep_addr), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Core_EndpointWriteFailed(...)
#endif


/**
  \brief  Event on \ref USBD_EndpointWriteGetResult finished (API)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  result       number of bytes written
 */
#ifdef                 EvtUSBD_Core_EndpointWriteGetResult
  __STATIC_INLINE void EvrUSBD_Core_EndpointWriteGetResult(  uint8_t device,    uint8_t ep_addr,   uint32_t result) {
    EventRecord2(      EvtUSBD_Core_EndpointWriteGetResult, TO_BYTE0(device) | TO_BYTE1(ep_addr),           result);
  }
#else
  #define              EvrUSBD_Core_EndpointWriteGetResult(...)
#endif


/**
  \brief  Event on \ref USBD_EndpointStall start (API)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  stall        operation
                        - false: clear
                        - true:  set
 */
#ifdef                 EvtUSBD_Core_EndpointStall
  __STATIC_INLINE void EvrUSBD_Core_EndpointStall(  uint8_t device,    uint8_t ep_addr,       bool stall) {
    EventRecord2(      EvtUSBD_Core_EndpointStall, TO_BYTE0(device) | TO_BYTE1(ep_addr) | TO_BYTE2(stall), 0U);
  }
#else
  #define              EvrUSBD_Core_EndpointStall(...)
#endif


/**
  \brief  Event on \ref USBD_EndpointStall failed (Error)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  stall        operation
                        - false: clear
                        - true:  set
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_Core_EndpointStallFailed
  __STATIC_INLINE void EvrUSBD_Core_EndpointStallFailed(  uint8_t device,    uint8_t ep_addr,       bool stall,  usbStatus error) {
    EventRecord2(      EvtUSBD_Core_EndpointStallFailed, TO_BYTE0(device) | TO_BYTE1(ep_addr) | TO_BYTE2(stall), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Core_EndpointStallFailed(...)
#endif


/**
  \brief  Event on \ref USBD_EndpointAbort start (API)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
 */
#ifdef                 EvtUSBD_Core_EndpointAbort
  __STATIC_INLINE void EvrUSBD_Core_EndpointAbort(  uint8_t device,    uint8_t ep_addr) {
    EventRecord2(      EvtUSBD_Core_EndpointAbort, TO_BYTE0(device) | TO_BYTE1(ep_addr), 0U);
  }
#else
  #define              EvrUSBD_Core_EndpointAbort(...)
#endif


/**
  \brief  Event on \ref USBD_EndpointAbort failed (Error)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_Core_EndpointAbortFailed
  __STATIC_INLINE void EvrUSBD_Core_EndpointAbortFailed(  uint8_t device,    uint8_t ep_addr,  usbStatus error) {
    EventRecord2(      EvtUSBD_Core_EndpointAbortFailed, TO_BYTE0(device) | TO_BYTE1(ep_addr), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Core_EndpointAbortFailed(...)
#endif


/**
  \brief  Event on internal operation configuration of Endpoint 0 start (Operation)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_ConfigureEp0
  __STATIC_INLINE void EvrUSBD_Core_ConfigureEp0(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_ConfigureEp0, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_ConfigureEp0(...)
#endif


/**
  \brief  Event on internal operation configuration of Endpoint 0 failed (Error)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_Core_ConfigureEp0Failed
  __STATIC_INLINE void EvrUSBD_Core_ConfigureEp0Failed(  uint8_t device,    uint8_t ep_addr,  usbStatus error) {
    EventRecord2(      EvtUSBD_Core_ConfigureEp0Failed, TO_BYTE0(device) | TO_BYTE1(ep_addr), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Core_ConfigureEp0Failed(...)
#endif


/**
  \brief  Event on internal operation read of setup packet finished (Detail)
  \param  device       device index
  \param  setup_packet setup packet content
 */
#ifdef                 EvtUSBD_Core_ReadSetupPacket
  __STATIC_INLINE void EvrUSBD_Core_ReadSetupPacket(  uint8_t device,     const void     * setup_packet) {
    EventRecord4(      EvtUSBD_Core_ReadSetupPacket, TO_BYTE0(device), *((const uint32_t *)setup_packet), *((const uint32_t *)setup_packet+1U), 0U);
  }
#else
  #define              EvrUSBD_Core_ReadSetupPacket(...)
#endif


/**
  \brief  Event on internal operation read of setup packet failed (Error)
  \param  device       device index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_Core_ReadSetupPacketFailed
  __STATIC_INLINE void EvrUSBD_Core_ReadSetupPacketFailed(  uint8_t device,  usbStatus error) {
    EventRecord2(      EvtUSBD_Core_ReadSetupPacketFailed, TO_BYTE0(device), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Core_ReadSetupPacketFailed(...)
#endif


/**
  \brief  Event on internal operation Control Endpoint request Get Status failed (Error)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_ReqGetStatusFailed
  __STATIC_INLINE void EvrUSBD_Core_ReqGetStatusFailed(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_ReqGetStatusFailed, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_ReqGetStatusFailed(...)
#endif


/**
  \brief  Event on internal operation Control Endpoint request Set/Clear Feature failed (Error)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_ReqSetClrFeatureFailed
  __STATIC_INLINE void EvrUSBD_Core_ReqSetClrFeatureFailed(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_ReqSetClrFeatureFailed, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_ReqSetClrFeatureFailed(...)
#endif


/**
  \brief  Event on internal operation Control Endpoint request Set Address failed (Error)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_ReqSetAddressFailed
  __STATIC_INLINE void EvrUSBD_Core_ReqSetAddressFailed(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_ReqSetAddressFailed, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_ReqSetAddressFailed(...)
#endif


/**
  \brief  Event on internal operation Control Endpoint request Get Descriptor failed (Error)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_ReqGetDescriptorFailed
  __STATIC_INLINE void EvrUSBD_Core_ReqGetDescriptorFailed(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_ReqGetDescriptorFailed, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_ReqGetDescriptorFailed(...)
#endif


/**
  \brief  Event on internal operation Control Endpoint request Get Microsoft Descriptor failed (Error)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_ReqGetMSDescriptorFailed
  __STATIC_INLINE void EvrUSBD_Core_ReqGetMSDescriptorFailed(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_ReqGetMSDescriptorFailed, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_ReqGetMSDescriptorFailed(...)
#endif


/**
  \brief  Event on internal operation Control Endpoint request Get Configuration failed (Error)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_ReqGetConfigurationFailed
  __STATIC_INLINE void EvrUSBD_Core_ReqGetConfigurationFailed(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_ReqGetConfigurationFailed, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_ReqGetConfigurationFailed(...)
#endif


/**
  \brief  Event on internal operation Control Endpoint request Set Configuration failed (Error)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_ReqSetConfigurationFailed
  __STATIC_INLINE void EvrUSBD_Core_ReqSetConfigurationFailed(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_ReqSetConfigurationFailed, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_ReqSetConfigurationFailed(...)
#endif


/**
  \brief  Event on internal operation Control Endpoint request Get Interface failed (Error)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_ReqGetInterfaceFailed
  __STATIC_INLINE void EvrUSBD_Core_ReqGetInterfaceFailed(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_ReqGetInterfaceFailed, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_ReqGetInterfaceFailed(...)
#endif


/**
  \brief  Event on internal operation Control Endpoint request Set Interface failed (Error)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Core_ReqSetInterfaceFailed
  __STATIC_INLINE void EvrUSBD_Core_ReqSetInterfaceFailed(  uint8_t device) {
    EventRecord2(      EvtUSBD_Core_ReqSetInterfaceFailed, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Core_ReqSetInterfaceFailed(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_Initialize user callback function start (API)
  \param  n            device index
 */
#ifdef                 EvtUSBD_Core_OnInitialize
  __STATIC_INLINE void EvrUSBD_Core_OnInitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_Core_OnInitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_Core_OnInitialize(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_Uninitialize user callback function start (API)
  \param  n            device index
 */
#ifdef                 EvtUSBD_Core_OnUninitialize
  __STATIC_INLINE void EvrUSBD_Core_OnUninitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_Core_OnUninitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_Core_OnUninitialize(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_VbusChanged user callback function start (API)
  \param  n            device index
  \param  level        current VBUS level
                        - true:  VBUS level is high
                        - false: VBUS level is low
 */
#ifdef                 EvtUSBD_Core_OnVbusChanged
  __STATIC_INLINE void EvrUSBD_Core_OnVbusChanged(  uint8_t n,       bool level) {
    EventRecord2(      EvtUSBD_Core_OnVbusChanged, TO_BYTE0(n), (uint32_t)level);
  }
#else
  #define              EvrUSBD_Core_OnVbusChanged(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_Reset user callback function start (API)
  \param  n            device index
 */
#ifdef                 EvtUSBD_Core_OnReset
  __STATIC_INLINE void EvrUSBD_Core_OnReset(  uint8_t n) {
    EventRecord2(      EvtUSBD_Core_OnReset, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_Core_OnReset(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_HighSpeedActivated user callback function start (API)
  \param  n            device index
 */
#ifdef                 EvtUSBD_Core_OnHighSpeedActivated
  __STATIC_INLINE void EvrUSBD_Core_OnHighSpeedActivated(  uint8_t n) {
    EventRecord2(      EvtUSBD_Core_OnHighSpeedActivated, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_Core_OnHighSpeedActivated(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_Suspended user callback function start (API)
  \param  n            device index
 */
#ifdef                 EvtUSBD_Core_OnSuspended
  __STATIC_INLINE void EvrUSBD_Core_OnSuspended(  uint8_t n) {
    EventRecord2(      EvtUSBD_Core_OnSuspended, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_Core_OnSuspended(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_Resumed user callback function start (API)
  \param  n            device index
 */
#ifdef                 EvtUSBD_Core_OnResumed
  __STATIC_INLINE void EvrUSBD_Core_OnResumed(  uint8_t n) {
    EventRecord2(      EvtUSBD_Core_OnResumed, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_Core_OnResumed(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_ConfigurationChanged user callback function start (API)
  \param  n              device index
  \param  configuration  current configuration value
                          - value = 0: not configured
                          - value > 0: active configuration
 */
#ifdef                 EvtUSBD_Core_OnConfigurationChanged
  __STATIC_INLINE void EvrUSBD_Core_OnConfigurationChanged(  uint8_t n,    uint8_t configuration) {
    EventRecord2(      EvtUSBD_Core_OnConfigurationChanged, TO_BYTE0(n), (uint32_t)configuration);
  }
#else
  #define              EvrUSBD_Core_OnConfigurationChanged(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_EnableRemoteWakeup user callback function start (API)
  \param  n            device index
 */
#ifdef                 EvtUSBD_Core_OnEnableRemoteWakeup
  __STATIC_INLINE void EvrUSBD_Core_OnEnableRemoteWakeup(  uint8_t n) {
    EventRecord2(      EvtUSBD_Core_OnEnableRemoteWakeup, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_Core_OnEnableRemoteWakeup(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_DisableRemoteWakeup user callback function start (API)
  \param  n            device index
 */
#ifdef                 EvtUSBD_Core_OnDisableRemoteWakeup
  __STATIC_INLINE void EvrUSBD_Core_OnDisableRemoteWakeup(  uint8_t n) {
    EventRecord2(      EvtUSBD_Core_OnDisableRemoteWakeup, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_Core_OnDisableRemoteWakeup(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_Endpoint0_SetupPacketReceived user callback function finished (API)
  \param  n            device index
  \param  result       enumerator value \ref usbdRequestStatus indicating the function execution status
                        - usbdRequestNotProcessed:request was not processed; processing will be done by USB library
                        - usbdRequestOK:          request was processed successfully (send Zero-Length Packet)
                        - usbdRequestStall:       request was processed but is not supported (stall Endpoint 0)
                        - usbdRequestNAK:         request was processed but the device is busy (return NAK)
 */
#ifdef                 EvtUSBD_Core_OnSetupPacketReceived
  __STATIC_INLINE void EvrUSBD_Core_OnSetupPacketReceived(  uint8_t n,  usbdRequestStatus result) {
    EventRecord2(      EvtUSBD_Core_OnSetupPacketReceived, TO_BYTE0(n),         (uint32_t)result);
  }
#else
  #define              EvrUSBD_Core_OnSetupPacketReceived(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_Endpoint0_SetupPacketReceived user callback function finished (Detail)
  \param  n            device index
  \param  setup_packet setup packet content
  \param  len          number of data bytes in data stage that were provided by the user in a callback
 */
#ifdef                 EvtUSBD_Core_OnSetupPacketReceivedDetail
  __STATIC_INLINE void EvrUSBD_Core_OnSetupPacketReceivedDetail(  uint8_t n,     const void     * setup_packet,                                        uint32_t len) {
    EventRecord4(      EvtUSBD_Core_OnSetupPacketReceivedDetail, TO_BYTE0(n), *((const uint32_t *)setup_packet), *((const uint32_t *)setup_packet+1U),          len);
  }
#else
  #define              EvrUSBD_Core_OnSetupPacketReceivedDetail(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_Endpoint0_SetupPacketProcessed user callback function finished (API)
  \param  n            device index
 */
#ifdef                 EvtUSBD_Core_OnSetupPacketProcessed
  __STATIC_INLINE void EvrUSBD_Core_OnSetupPacketProcessed(  uint8_t n) {
    EventRecord2(      EvtUSBD_Core_OnSetupPacketProcessed, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_Core_OnSetupPacketProcessed(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_Endpoint0_SetupPacketProcessed user callback function finished (Detail)
  \param  n            device index
  \param  setup_packet setup packet content
 */
#ifdef                 EvtUSBD_Core_OnSetupPacketProcessedDetail
  __STATIC_INLINE void EvrUSBD_Core_OnSetupPacketProcessedDetail(  uint8_t n,     const void     * setup_packet) {
    EventRecord4(      EvtUSBD_Core_OnSetupPacketProcessedDetail, TO_BYTE0(n), *((const uint32_t *)setup_packet), *((const uint32_t *)setup_packet+1U), 0U);
  }
#else
  #define              EvrUSBD_Core_OnSetupPacketProcessedDetail(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_Endpoint0_OutDataReceived user callback function finished (API)
  \param  n            device index
  \param  len          number of received data bytes
  \param  result       enumerator value \ref usbdRequestStatus indicating the function execution status
                        - usbdRequestNotProcessed:request was not processed; processing will be done by USB library
                        - usbdRequestOK:          request was processed successfully (send Zero-Length Packet)
                        - usbdRequestStall:       request was processed but is not supported (stall Endpoint 0)
                        - usbdRequestNAK:         request was processed but the device is busy (return NAK)
 */
#ifdef                 EvtUSBD_Core_OnOutDataReceived
  __STATIC_INLINE void EvrUSBD_Core_OnOutDataReceived(  uint8_t n,            uint32_t len, usbdRequestStatus result) {
    EventRecord2(      EvtUSBD_Core_OnOutDataReceived, TO_BYTE0(n) | TO_BYTE1(result), len);
  }
#else
  #define              EvrUSBD_Core_OnOutDataReceived(...)
#endif


/**
  \brief  Event on \ref USBD_Devicen_Endpoint0_InDataSent user callback function finished (API)
  \param  n            device index
  \param  len          number of sent data bytes
  \param  result       enumerator value \ref usbdRequestStatus indicating the function execution status
                        - usbdRequestNotProcessed:request was not processed; processing will be done by USB library
                        - usbdRequestOK:          request was processed successfully (send Zero-Length Packet)
                        - usbdRequestStall:       request was processed but is not supported (stall Endpoint 0)
                        - usbdRequestNAK:         request was processed but the device is busy (return NAK)
 */
#ifdef                 EvtUSBD_Core_OnInDataSent
  __STATIC_INLINE void EvrUSBD_Core_OnInDataSent(  uint8_t n,            uint32_t len, usbdRequestStatus result) {
    EventRecord2(      EvtUSBD_Core_OnInDataSent, TO_BYTE0(n) | TO_BYTE1(result), len);
  }
#else
  #define              EvrUSBD_Core_OnInDataSent(...)
#endif


// USB Device Driver component event record functions --------------------------

/**
  \brief  Event on ARM_USBD_Initialize finished successfully (API)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Driver_Initialize
  __STATIC_INLINE void EvrUSBD_Driver_Initialize(  uint8_t device) {
    EventRecord2(      EvtUSBD_Driver_Initialize, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Driver_Initialize(...)
#endif


/**
  \brief  Event on ARM_USBD_Initialize failed (Error)
  \param  device       device index
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBD_Driver_InitializeFailed
  __STATIC_INLINE void EvrUSBD_Driver_InitializeFailed(  uint8_t device,    int32_t error) {
    EventRecord2(      EvtUSBD_Driver_InitializeFailed, TO_BYTE0(device), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Driver_InitializeFailed(...)
#endif


/**
  \brief  Event on ARM_USBD_Uninitialize finished successfully (API)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Driver_Uninitialize
  __STATIC_INLINE void EvrUSBD_Driver_Uninitialize(  uint8_t device) {
    EventRecord2(      EvtUSBD_Driver_Uninitialize, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Driver_Uninitialize(...)
#endif


/**
  \brief  Event on ARM_USBD_Uninitialize failed (Error)
  \param  device       device index
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBD_Driver_UninitializeFailed
  __STATIC_INLINE void EvrUSBD_Driver_UninitializeFailed(  uint8_t device,    int32_t error) {
    EventRecord2(      EvtUSBD_Driver_UninitializeFailed, TO_BYTE0(device), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Driver_UninitializeFailed(...)
#endif


/**
  \brief  Event on ARM_USBD_PowerControl finished successfully (API)
  \param  device       device index
  \param  state        power state ARM_POWER_STATE
 */
#ifdef                 EvtUSBD_Driver_PowerControl
  __STATIC_INLINE void EvrUSBD_Driver_PowerControl(  uint8_t device,  ARM_POWER_STATE state) {
    EventRecord2(      EvtUSBD_Driver_PowerControl, TO_BYTE0(device) |       TO_BYTE1(state), 0U);
  }
#else
  #define              EvrUSBD_Driver_PowerControl(...)
#endif


/**
  \brief  Event on ARM_USBD_PowerControl failed (Error)
  \param  device       device index
  \param  state        power state ARM_POWER_STATE
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBD_Driver_PowerControlFailed
  __STATIC_INLINE void EvrUSBD_Driver_PowerControlFailed(  uint8_t device,  ARM_POWER_STATE state,    int32_t error) {
    EventRecord2(      EvtUSBD_Driver_PowerControlFailed, TO_BYTE0(device) |       TO_BYTE1(state), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Driver_PowerControlFailed(...)
#endif


/**
  \brief  Event on ARM_USBD_DeviceConnect finished successfully (API)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Driver_DeviceConnect
  __STATIC_INLINE void EvrUSBD_Driver_DeviceConnect(  uint8_t device) {
    EventRecord2(      EvtUSBD_Driver_DeviceConnect, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Driver_DeviceConnect(...)
#endif


/**
  \brief  Event on ARM_USBD_DeviceConnect failed (Error)
  \param  device       device index
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBD_Driver_DeviceConnectFailed
  __STATIC_INLINE void EvrUSBD_Driver_DeviceConnectFailed(  uint8_t device,    int32_t error) {
    EventRecord2(      EvtUSBD_Driver_DeviceConnectFailed, TO_BYTE0(device), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Driver_DeviceConnectFailed(...)
#endif


/**
  \brief  Event on ARM_USBD_DeviceDisconnect finished successfully (API)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Driver_DeviceDisconnect
  __STATIC_INLINE void EvrUSBD_Driver_DeviceDisconnect(  uint8_t device) {
    EventRecord2(      EvtUSBD_Driver_DeviceDisconnect, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Driver_DeviceDisconnect(...)
#endif


/**
  \brief  Event on ARM_USBD_DeviceDisconnect failed (Error)
  \param  device       device index
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBD_Driver_DeviceDisconnectFailed
  __STATIC_INLINE void EvrUSBD_Driver_DeviceDisconnectFailed(  uint8_t device,    int32_t error) {
    EventRecord2(      EvtUSBD_Driver_DeviceDisconnectFailed, TO_BYTE0(device), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Driver_DeviceDisconnectFailed(...)
#endif


/**
  \brief  Event on ARM_USBD_DeviceGetState finished (API)
  \param  device       device index
  \param  state        device state ARM_USBD_STATE
 */
#ifdef                 EvtUSBD_Driver_DeviceGetState
  __STATIC_INLINE void EvrUSBD_Driver_DeviceGetState(  uint8_t device,  ARM_USBD_STATE state) {
    EventRecord2(      EvtUSBD_Driver_DeviceGetState, TO_BYTE0(device), (uint32_t)state.vbus | ((uint32_t)state.speed << 1) | ((uint32_t)state.active << 3));
  }
#else
  #define              EvrUSBD_Driver_DeviceGetState(...)
#endif


/**
  \brief  Event on ARM_USBD_DeviceRemoteWakeup finished successfully (API)
  \param  device       device index
 */
#ifdef                 EvtUSBD_Driver_DeviceRemoteWakeup
  __STATIC_INLINE void EvrUSBD_Driver_DeviceRemoteWakeup(  uint8_t device) {
    EventRecord2(      EvtUSBD_Driver_DeviceRemoteWakeup, TO_BYTE0(device), 0U);
  }
#else
  #define              EvrUSBD_Driver_DeviceRemoteWakeup(...)
#endif


/**
  \brief  Event on ARM_USBD_DeviceRemoteWakeup failed (Error)
  \param  device       device index
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBD_Driver_DeviceRemoteWakeupFailed
  __STATIC_INLINE void EvrUSBD_Driver_DeviceRemoteWakeupFailed(  uint8_t device,    int32_t error) {
    EventRecord2(      EvtUSBD_Driver_DeviceRemoteWakeupFailed, TO_BYTE0(device), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Driver_DeviceRemoteWakeupFailed(...)
#endif


/**
  \brief  Event on ARM_USBD_DeviceSetAddress finished successfully (API)
  \param  device       device index
  \param  dev_addr     device address
 */
#ifdef                 EvtUSBD_Driver_DeviceSetAddress
  __STATIC_INLINE void EvrUSBD_Driver_DeviceSetAddress(  uint8_t device,    uint8_t dev_addr) {
    EventRecord2(      EvtUSBD_Driver_DeviceSetAddress, TO_BYTE0(device) | TO_BYTE1(dev_addr), 0U);
  }
#else
  #define              EvrUSBD_Driver_DeviceSetAddress(...)
#endif


/**
  \brief  Event on ARM_USBD_DeviceSetAddress failed (Error)
  \param  device       device index
  \param  dev_addr     device address
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBD_Driver_DeviceSetAddressFailed
  __STATIC_INLINE void EvrUSBD_Driver_DeviceSetAddressFailed(  uint8_t device,    uint8_t dev_addr,    int32_t error) {
    EventRecord2(      EvtUSBD_Driver_DeviceSetAddressFailed, TO_BYTE0(device) | TO_BYTE1(dev_addr), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Driver_DeviceSetAddressFailed(...)
#endif


/**
  \brief  Event on ARM_USBD_ReadSetupPacket finished successfully (API)
  \param  device       device index
  \param  setup_packet setup packet content
 */
#ifdef                 EvtUSBD_Driver_ReadSetupPacket
  __STATIC_INLINE void EvrUSBD_Driver_ReadSetupPacket(  uint8_t device,     const void     * setup_packet) {
    EventRecord4(      EvtUSBD_Driver_ReadSetupPacket, TO_BYTE0(device), *((const uint32_t *)setup_packet), *((const uint32_t *)setup_packet+1U), 0U);
  }
#else
  #define              EvrUSBD_Driver_ReadSetupPacket(...)
#endif


/**
  \brief  Event on ARM_USBD_ReadSetupPacket failed (Error)
  \param  device       device index
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBD_Driver_ReadSetupPacketFailed
  __STATIC_INLINE void EvrUSBD_Driver_ReadSetupPacketFailed(  uint8_t device,    int32_t error) {
    EventRecord2(      EvtUSBD_Driver_ReadSetupPacketFailed, TO_BYTE0(device), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Driver_ReadSetupPacketFailed(...)
#endif


/**
  \brief  Event on ARM_USBD_EndpointConfigure finished successfully (API)
  \param  device              device index
  \param  ep_addr             endpoint address
                               - ep_addr.0..3: address
                               - ep_addr.7:    direction
  \param  ep_type             endpoint type (ARM_USB_ENDPOINT_xxx)
  \param  ep_max_packet_size  endpoint maximum packet size
 */
#ifdef                 EvtUSBD_Driver_EndpointConfigure
  __STATIC_INLINE void EvrUSBD_Driver_EndpointConfigure(  uint8_t device,    uint8_t ep_addr,    uint8_t ep_type,   uint16_t ep_max_packet_size) {
    EventRecord2(      EvtUSBD_Driver_EndpointConfigure, TO_BYTE0(device) | TO_BYTE1(ep_addr) | TO_BYTE2(ep_type), (uint32_t)ep_max_packet_size);
  }
#else
  #define              EvrUSBD_Driver_EndpointConfigure(...)
#endif


/**
  \brief  Event on ARM_USBD_EndpointConfigure failed (Error)
  \param  device              device index
  \param  ep_addr             endpoint address
                               - ep_addr.0..3: address
                               - ep_addr.7:    direction
  \param  ep_type             endpoint type (ARM_USB_ENDPOINT_xxx)
  \param  ep_max_packet_size  endpoint maximum packet size
  \param  error               error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBD_Driver_EndpointConfigureFailed
  __STATIC_INLINE void EvrUSBD_Driver_EndpointConfigureFailed(  uint8_t device,    uint8_t ep_addr,    uint8_t ep_type,                     uint16_t ep_max_packet_size, int32_t error) {
    EventRecord2(      EvtUSBD_Driver_EndpointConfigureFailed, TO_BYTE0(device) | TO_BYTE1(ep_addr) | TO_BYTE2(ep_type) | TO_BYTE3(error), (uint32_t)ep_max_packet_size);
  }
#else
  #define              EvrUSBD_Driver_EndpointConfigureFailed(...)
#endif


/**
  \brief  Event on ARM_USBD_EndpointUnconfigure finished successfully (API)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
 */
#ifdef                 EvtUSBD_Driver_EndpointUnconfigure
  __STATIC_INLINE void EvrUSBD_Driver_EndpointUnconfigure(  uint8_t device,    uint8_t ep_addr) {
    EventRecord2(      EvtUSBD_Driver_EndpointUnconfigure, TO_BYTE0(device) | TO_BYTE1(ep_addr), 0U);
  }
#else
  #define              EvrUSBD_Driver_EndpointUnconfigure(...)
#endif


/**
  \brief  Event on ARM_USBD_EndpointUnconfigure failed (Error)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBD_Driver_EndpointUnconfigureFailed
  __STATIC_INLINE void EvrUSBD_Driver_EndpointUnconfigureFailed(  uint8_t device,    uint8_t ep_addr,    int32_t error) {
    EventRecord2(      EvtUSBD_Driver_EndpointUnconfigureFailed, TO_BYTE0(device) | TO_BYTE1(ep_addr), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Driver_EndpointUnconfigureFailed(...)
#endif


/**
  \brief  Event on ARM_USBD_EndpointStall finished successfully (API)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  stall        operation
                        - false: clear
                        - true:  set
 */
#ifdef                 EvtUSBD_Driver_EndpointStall
  __STATIC_INLINE void EvrUSBD_Driver_EndpointStall(  uint8_t device,    uint8_t ep_addr,       bool stall) {
    EventRecord2(      EvtUSBD_Driver_EndpointStall, TO_BYTE0(device) | TO_BYTE1(ep_addr) | TO_BYTE2(stall), 0U);
  }
#else
  #define              EvrUSBD_Driver_EndpointStall(...)
#endif


/**
  \brief  Event on ARM_USBD_EndpointStall failed (Error)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  stall        operation
                        - false: clear
                        - true:  set
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBD_Driver_EndpointStallFailed
  __STATIC_INLINE void EvrUSBD_Driver_EndpointStallFailed(  uint8_t device,    uint8_t ep_addr,       bool stall,    int32_t error) {
    EventRecord2(      EvtUSBD_Driver_EndpointStallFailed, TO_BYTE0(device) | TO_BYTE1(ep_addr) | TO_BYTE2(stall), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Driver_EndpointStallFailed(...)
#endif


/**
  \brief  Event on ARM_USBD_EndpointTransfer finished successfully (API)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  data         pointer to buffer for data to read or with data to write
  \param  num          number of data bytes to transfer
 */
#ifdef                 EvtUSBD_Driver_EndpointTransfer
  __STATIC_INLINE void EvrUSBD_Driver_EndpointTransfer(  uint8_t device,    uint8_t ep_addr,   const uint8_t *data, uint32_t num) {
    EventRecord4(      EvtUSBD_Driver_EndpointTransfer, TO_BYTE0(device) | TO_BYTE1(ep_addr), (uint32_t)      data,          num, 0U);
  }
#else
  #define              EvrUSBD_Driver_EndpointTransfer(...)
#endif


/**
  \brief  Event on ARM_USBD_EndpointTransfer failed (Error)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBD_Driver_EndpointTransferFailed
  __STATIC_INLINE void EvrUSBD_Driver_EndpointTransferFailed(  uint8_t device,    uint8_t ep_addr,    int32_t error) {
    EventRecord2(      EvtUSBD_Driver_EndpointTransferFailed, TO_BYTE0(device) | TO_BYTE1(ep_addr), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Driver_EndpointTransferFailed(...)
#endif


/**
  \brief  Event on ARM_USBD_EndpointTransferGetResult finished (API)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  num          number of successfully transferred data bytes
 */
#ifdef                 EvtUSBD_Driver_EndpointTransferGetResult
  __STATIC_INLINE void EvrUSBD_Driver_EndpointTransferGetResult(  uint8_t device,    uint8_t ep_addr, uint32_t num) {
    EventRecord2(      EvtUSBD_Driver_EndpointTransferGetResult, TO_BYTE0(device) | TO_BYTE1(ep_addr),         num);
  }
#else
  #define              EvrUSBD_Driver_EndpointTransferGetResult(...)
#endif


/**
  \brief  Event on ARM_USBD_EndpointTransferAbort finished successfully (API)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
 */
#ifdef                 EvtUSBD_Driver_EndpointTransferAbort
  __STATIC_INLINE void EvrUSBD_Driver_EndpointTransferAbort(  uint8_t device,    uint8_t ep_addr) {
    EventRecord2(      EvtUSBD_Driver_EndpointTransferAbort, TO_BYTE0(device) | TO_BYTE1(ep_addr), 0U);
  }
#else
  #define              EvrUSBD_Driver_EndpointTransferAbort(...)
#endif


/**
  \brief  Event on ARM_USBD_EndpointTransferAbort failed (Error)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBD_Driver_EndpointTransferAbortFailed
  __STATIC_INLINE void EvrUSBD_Driver_EndpointTransferAbortFailed(  uint8_t device,    uint8_t ep_addr,    int32_t error) {
    EventRecord2(      EvtUSBD_Driver_EndpointTransferAbortFailed, TO_BYTE0(device) | TO_BYTE1(ep_addr), (uint32_t)error);
  }
#else
  #define              EvrUSBD_Driver_EndpointTransferAbortFailed(...)
#endif


/**
  \brief  Event on ARM_USBD_GetFrameNumber finished (API)
  \param  device       device index
  \param  frame_number frame number
 */
#ifdef                 EvtUSBD_Driver_GetFrameNumber
  __STATIC_INLINE void EvrUSBD_Driver_GetFrameNumber(  uint8_t device,   uint16_t frame_number) {
    EventRecord2(      EvtUSBD_Driver_GetFrameNumber, TO_BYTE0(device), (uint32_t)frame_number);
  }
#else
  #define              EvrUSBD_Driver_GetFrameNumber(...)
#endif


/**
  \brief  Event on ARM_USBD_SignalDeviceEvent callback start (API)
  \param  device       device index
  \param  event        device event USBD_dev_events
 */
#ifdef                 EvtUSBD_Driver_OnSignalDeviceEvent
  __STATIC_INLINE void EvrUSBD_Driver_OnSignalDeviceEvent(  uint8_t device,  uint32_t event) {
    EventRecord2(      EvtUSBD_Driver_OnSignalDeviceEvent, TO_BYTE0(device),          event);
  }
#else
  #define              EvrUSBD_Driver_OnSignalDeviceEvent(...)
#endif


/**
  \brief  Event on ARM_USBD_SignalEndpointEvent callback start (API)
  \param  device       device index
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
  \param  event        endpoint event USBD_ep_events
 */
#ifdef                 EvtUSBD_Driver_OnSignalEndpointEvent
  __STATIC_INLINE void EvrUSBD_Driver_OnSignalEndpointEvent(  uint8_t device,    uint8_t ep_addr, uint32_t event) {
    EventRecord2(      EvtUSBD_Driver_OnSignalEndpointEvent, TO_BYTE0(device) | TO_BYTE1(ep_addr),         event);
  }
#else
  #define              EvrUSBD_Driver_OnSignalEndpointEvent(...)
#endif


// USB Device Custom Class component event record functions ------------------

/**
  \brief  Event on \ref USBD_CustomClassn_Initialize user callback function start (API)
  \param  n            instance of custom class
 */
#ifdef                 EvtUSBD_CC_OnInitialize
  __STATIC_INLINE void EvrUSBD_CC_OnInitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_CC_OnInitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CC_OnInitialize(...)
#endif


/**
  \brief  Event on \ref USBD_CustomClassn_Uninitialize user callback function start (API)
  \param  n            instance of custom class
 */
#ifdef                 EvtUSBD_CC_OnUninitialize
  __STATIC_INLINE void EvrUSBD_CC_OnUninitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_CC_OnUninitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CC_OnUninitialize(...)
#endif


/**
  \brief  Event on \ref USBD_CustomClassn_Reset user callback function start (API)
  \param  n            instance of custom class
 */
#ifdef                 EvtUSBD_CC_OnReset
  __STATIC_INLINE void EvrUSBD_CC_OnReset(  uint8_t n) {
    EventRecord2(      EvtUSBD_CC_OnReset, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CC_OnReset(...)
#endif


/**
  \brief  Event on \ref USBD_CustomClassn_EndpointStart user callback function start (API)
  \param  n            instance of custom class
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
 */
#ifdef                 EvtUSBD_CC_OnEndpointStart
  __STATIC_INLINE void EvrUSBD_CC_OnEndpointStart(  uint8_t n,   uint8_t ep_addr) {
    EventRecord2(      EvtUSBD_CC_OnEndpointStart, TO_BYTE0(n), TO_BYTE0(ep_addr));
  }
#else
  #define              EvrUSBD_CC_OnEndpointStart(...)
#endif


/**
  \brief  Event on \ref USBD_CustomClassn_EndpointStop user callback function start (API)
  \param  n            instance of custom class
  \param  ep_addr      endpoint address
                        - ep_addr.0..3: address
                        - ep_addr.7:    direction
 */
#ifdef                 EvtUSBD_CC_OnEndpointStop
  __STATIC_INLINE void EvrUSBD_CC_OnEndpointStop(  uint8_t n,   uint8_t ep_addr) {
    EventRecord2(      EvtUSBD_CC_OnEndpointStop, TO_BYTE0(n), TO_BYTE0(ep_addr));
  }
#else
  #define              EvrUSBD_CC_OnEndpointStop(...)
#endif


/**
  \brief  Event on \ref USBD_CustomClassn_Endpoint0_SetupPacketReceived user callback function finished (API)
  \param  n            instance of custom class
  \param  result       enumerator value \ref usbdRequestStatus indicating the function execution status
                        - usbdRequestNotProcessed:request was not processed; processing will be done by USB library
                        - usbdRequestOK:          request was processed successfully (send Zero-Length Packet)
                        - usbdRequestStall:       request was processed but is not supported (stall Endpoint 0)
                        - usbdRequestNAK:         request was processed but the device is busy (return NAK)
 */
#ifdef                 EvtUSBD_CC_OnEndpoint0SetupPacketReceived
  __STATIC_INLINE void EvrUSBD_CC_OnEndpoint0SetupPacketReceived(  uint8_t n,  usbdRequestStatus result) {
    EventRecord2(      EvtUSBD_CC_OnEndpoint0SetupPacketReceived, TO_BYTE0(n),         (uint32_t)result);
  }
#else
  #define              EvrUSBD_CC_OnEndpoint0SetupPacketReceived(...)
#endif


/**
  \brief  Event on \ref USBD_CustomClassn_Endpoint0_SetupPacketReceived user callback function start (Detail)
  \param  n            instance of custom class
  \param  setup_packet received setup packet content
  \param  len          number of data bytes in data stage that were provided by the user in a callback
 */
#ifdef                 EvtUSBD_CC_OnEndpoint0SetupPacketReceivedDetail
  __STATIC_INLINE void EvrUSBD_CC_OnEndpoint0SetupPacketReceivedDetail(  uint8_t n,     const void     * setup_packet,                                        uint32_t len) {
    EventRecord4(      EvtUSBD_CC_OnEndpoint0SetupPacketReceivedDetail, TO_BYTE0(n), *((const uint32_t *)setup_packet), *((const uint32_t *)setup_packet+1U),          len);
  }
#else
  #define              EvrUSBD_CC_OnEndpoint0SetupPacketReceivedDetail(...)
#endif


/**
  \brief  Event on \ref USBD_CustomClassn_Endpoint0_SetupPacketProcessed user callback function finished (API)
  \param  n            instance of custom class
 */
#ifdef                 EvtUSBD_CC_OnEndpoint0SetupPacketProcessed
  __STATIC_INLINE void EvrUSBD_CC_OnEndpoint0SetupPacketProcessed(  uint8_t n) {
    EventRecord2(      EvtUSBD_CC_OnEndpoint0SetupPacketProcessed, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CC_OnEndpoint0SetupPacketProcessed(...)
#endif


/**
  \brief  Event on \ref USBD_CustomClassn_Endpoint0_SetupPacketProcessed user callback function start (Detail)
  \param  n            instance of custom class
  \param  setup_packet processed setup packet content
 */
#ifdef                 EvtUSBD_CC_OnEndpoint0SetupPacketProcessedDetail
  __STATIC_INLINE void EvrUSBD_CC_OnEndpoint0SetupPacketProcessedDetail(  uint8_t n,     const void     * setup_packet) {
    EventRecord4(      EvtUSBD_CC_OnEndpoint0SetupPacketProcessedDetail, TO_BYTE0(n), *((const uint32_t *)setup_packet), *((const uint32_t *)setup_packet+1U), 0U);
  }
#else
  #define              EvrUSBD_CC_OnEndpoint0SetupPacketProcessedDetail(...)
#endif


/**
  \brief  Event on \ref USBD_CustomClassn_Endpoint0_OutDataReceived user callback function finished (API)
  \param  n            instance of custom class
  \param  len          number of received data bytes
  \param  result       enumerator value \ref usbdRequestStatus indicating the function execution status
                        - usbdRequestNotProcessed:request was not processed; processing will be done by USB library
                        - usbdRequestOK:          request was processed successfully (send Zero-Length Packet)
                        - usbdRequestStall:       request was processed but is not supported (stall Endpoint 0)
                        - usbdRequestNAK:         request was processed but the device is busy (return NAK)
 */
#ifdef                 EvtUSBD_CC_OnEndpoint0OutDataReceived
  __STATIC_INLINE void EvrUSBD_CC_OnEndpoint0OutDataReceived(  uint8_t n,            uint32_t len, usbdRequestStatus result) {
    EventRecord2(      EvtUSBD_CC_OnEndpoint0OutDataReceived, TO_BYTE0(n) | TO_BYTE1(result), len);
  }
#else
  #define              EvrUSBD_CC_OnEndpoint0OutDataReceived(...)
#endif


/**
  \brief  Event on \ref USBD_CustomClassn_Endpoint0_InDataSent user callback function finished (API)
  \param  n            instance of custom class
  \param  len          number of sent data bytes
  \param  result       enumerator value \ref usbdRequestStatus indicating the function execution status
                        - usbdRequestNotProcessed:request was not processed; processing will be done by USB library
                        - usbdRequestOK:          request was processed successfully (send Zero-Length Packet)
                        - usbdRequestStall:       request was processed but is not supported (stall Endpoint 0)
                        - usbdRequestNAK:         request was processed but the device is busy (return NAK)
 */
#ifdef                 EvtUSBD_CC_OnEndpoint0InDataSent
  __STATIC_INLINE void EvrUSBD_CC_OnEndpoint0InDataSent(  uint8_t n,            uint32_t len, usbdRequestStatus result) {
    EventRecord2(      EvtUSBD_CC_OnEndpoint0InDataSent, TO_BYTE0(n) | TO_BYTE1(result), len);
  }
#else
  #define              EvrUSBD_CC_OnEndpoint0InDataSent(...)
#endif


/**
  \brief  Event on \ref USBD_CustomClassn_Endpoint1_Event user callback function start (API)
  \param  n            instance of custom class
  \param  ep_num       endpoint number
  \param  event        event on endpoint:
                        - ARM_USBD_EVENT_OUT = data OUT received
                        - ARM_USBD_EVENT_IN  = data IN  sent
 */
#ifdef                 EvtUSBD_CC_OnEndpointmEvent
  __STATIC_INLINE void EvrUSBD_CC_OnEndpointmEvent(  uint8_t n,    uint8_t ep_num,  uint32_t event) {
    EventRecord2(      EvtUSBD_CC_OnEndpointmEvent, TO_BYTE0(n) | TO_BYTE1(ep_num),          event);
  }
#else
  #define              EvrUSBD_CC_OnEndpointmEvent(...)
#endif


// USB Device ADC component event record functions -----------------------------

/**
  \brief  Event on internal library operation ADC initialize start (Operation)
  \param  instance     instance of ADC class
 */
#ifdef                 EvtUSBD_ADC_Initialize
  __STATIC_INLINE void EvrUSBD_ADC_Initialize(  uint8_t instance) {
    EventRecord2(      EvtUSBD_ADC_Initialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_ADC_Initialize(...)
#endif


/**
  \brief  Event on internal library operation ADC initialize failed (Error)
  \param  instance     instance of ADC class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_ADC_InitializeFailed
  __STATIC_INLINE void EvrUSBD_ADC_InitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_ADC_InitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_ADC_InitializeFailed(...)
#endif


/**
  \brief  Event on internal library operation ADC uninitialize start (Operation)
  \param  instance     instance of ADC class
 */
#ifdef                 EvtUSBD_ADC_Uninitialize
  __STATIC_INLINE void EvrUSBD_ADC_Uninitialize(  uint8_t instance) {
    EventRecord2(      EvtUSBD_ADC_Uninitialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_ADC_Uninitialize(...)
#endif


/**
  \brief  Event on internal library operation ADC uninitialize failed (Error)
  \param  instance     instance of ADC class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_ADC_UninitializeFailed
  __STATIC_INLINE void EvrUSBD_ADC_UninitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_ADC_UninitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_ADC_UninitializeFailed(...)
#endif


/**
  \brief  Event on internal library operation ADC reset start (Operation)
  \param  instance     instance of ADC class
 */
#ifdef                 EvtUSBD_ADC_Reset
  __STATIC_INLINE void EvrUSBD_ADC_Reset(  uint8_t instance) {
    EventRecord2(      EvtUSBD_ADC_Reset, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_ADC_Reset(...)
#endif


/**
  \brief  Event on internal library operation ADC reset failed (Error)
  \param  instance     instance of ADC class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_ADC_ResetFailed
  __STATIC_INLINE void EvrUSBD_ADC_ResetFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_ADC_ResetFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_ADC_ResetFailed(...)
#endif


/**
  \brief  Event on \ref USBD_ADCn_Initialize user callback function start (API)
  \param  n            instance of ADC class
 */
#ifdef                 EvtUSBD_ADC_OnInitialize
  __STATIC_INLINE void EvrUSBD_ADC_OnInitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_ADC_OnInitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_ADC_OnInitialize(...)
#endif


/**
  \brief  Event on \ref USBD_ADCn_Uninitialize user callback function start (API)
  \param  n            instance of ADC class
 */
#ifdef                 EvtUSBD_ADC_OnUninitialize
  __STATIC_INLINE void EvrUSBD_ADC_OnUninitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_ADC_OnUninitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_ADC_OnUninitialize(...)
#endif


/**
  \brief  Event on \ref USBD_ADCn_PlayStart user callback function start (API)
  \param  n            instance of ADC class
 */
#ifdef                 EvtUSBD_ADC_OnPlayStart
  __STATIC_INLINE void EvrUSBD_ADC_OnPlayStart(  uint8_t n) {
    EventRecord2(      EvtUSBD_ADC_OnPlayStart, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_ADC_OnPlayStart(...)
#endif


/**
  \brief  Event on \ref USBD_ADCn_PlayStop user callback function start (API)
  \param  n            instance of ADC class
 */
#ifdef                 EvtUSBD_ADC_OnPlayStop
  __STATIC_INLINE void EvrUSBD_ADC_OnPlayStop(  uint8_t n) {
    EventRecord2(      EvtUSBD_ADC_OnPlayStop, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_ADC_OnPlayStop(...)
#endif


/**
  \brief  Event on \ref USBD_ADCn_SetSpeakerMute user callback function start (API)
  \param  n            instance of ADC class
  \param  ch           channel index
                        - value 0: master channel
                        - value 1: left speaker (in stereo mode)
                        - value 2: right speaker (in stereo mode)
  \param  on           current mute setting
                        - false :  mute off
                        - true :   mute on
 */
#ifdef                 EvtUSBD_ADC_OnSetSpeakerMute
  __STATIC_INLINE void EvrUSBD_ADC_OnSetSpeakerMute(  uint8_t n,    uint8_t ch,       bool on) {
    EventRecord2(      EvtUSBD_ADC_OnSetSpeakerMute, TO_BYTE0(n) | TO_BYTE1(ch), (uint32_t)on);
  }
#else
  #define              EvrUSBD_ADC_OnSetSpeakerMute(...)
#endif


/**
  \brief  Event on \ref USBD_ADCn_SetSpeakerVolume user callback function start (API)
  \param  n            instance of ADC class
  \param  ch           channel index
                        - value 0: master channel
                        - value 1: left speaker (in stereo mode)
                        - value 2: right speaker (in stereo mode)
  \param  vol          current volume setting
 */
#ifdef                 EvtUSBD_ADC_OnSetSpeakerVolume
  __STATIC_INLINE void EvrUSBD_ADC_OnSetSpeakerVolume(  uint8_t n,    uint8_t ch,   uint16_t vol) {
    EventRecord2(      EvtUSBD_ADC_OnSetSpeakerVolume, TO_BYTE0(n) | TO_BYTE1(ch), (uint32_t)vol);
  }
#else
  #define              EvrUSBD_ADC_OnSetSpeakerVolume(...)
#endif


/**
  \brief  Event on \ref USBD_ADCn_RecordStart user callback function start (API)
  \param  n            instance of ADC class
 */
#ifdef                 EvtUSBD_ADC_OnRecordStart
  __STATIC_INLINE void EvrUSBD_ADC_OnRecordStart(  uint8_t n) {
    EventRecord2(      EvtUSBD_ADC_OnRecordStart, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_ADC_OnRecordStart(...)
#endif


/**
  \brief  Event on \ref USBD_ADCn_RecordStop user callback function start (API)
  \param  n            instance of ADC class
 */
#ifdef                 EvtUSBD_ADC_OnRecordStop
  __STATIC_INLINE void EvrUSBD_ADC_OnRecordStop(  uint8_t n) {
    EventRecord2(      EvtUSBD_ADC_OnRecordStop, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_ADC_OnRecordStop(...)
#endif


/**
  \brief  Event on \ref USBD_ADCn_SetMicrophoneMute user callback function start (API)
  \param  n            instance of ADC class
  \param  ch           channel index
                        - value 0: master channel
                        - value 1: left microphone (in stereo mode)
                        - value 2: right microphone (in stereo mode)
  \param  on           current mute setting
                        - false :  mute off
                        - true :   mute on
 */
#ifdef                 EvtUSBD_ADC_OnSetMicrophoneMute
  __STATIC_INLINE void EvrUSBD_ADC_OnSetMicrophoneMute(  uint8_t n,    uint8_t ch,       bool on) {
    EventRecord2(      EvtUSBD_ADC_OnSetMicrophoneMute, TO_BYTE0(n) | TO_BYTE1(ch), (uint32_t)on);
  }
#else
  #define              EvrUSBD_ADC_OnSetMicrophoneMute(...)
#endif


/**
  \brief  Event on \ref USBD_ADCn_SetMicrophoneVolume user callback function start (API)
  \param  n            instance of ADC class
  \param  ch           channel index
                        - value 0: master channel
                        - value 1: left microphone (in stereo mode)
                        - value 2: right microphone (in stereo mode)
  \param  vol          current volume setting
 */
#ifdef                 EvtUSBD_ADC_OnSetMicrophoneVolume
  __STATIC_INLINE void EvrUSBD_ADC_OnSetMicrophoneVolume(  uint8_t n,    uint8_t ch,   uint16_t vol) {
    EventRecord2(      EvtUSBD_ADC_OnSetMicrophoneVolume, TO_BYTE0(n) | TO_BYTE1(ch), (uint32_t)vol);
  }
#else
  #define              EvrUSBD_ADC_OnSetMicrophoneVolume(...)
#endif


/**
  \brief  Event on \ref USBD_ADCn_ReceivedSamples user callback function start (API)
  \param  n            instance of ADC class
 */
#ifdef                 EvtUSBD_ADC_OnReceivedSamples
  __STATIC_INLINE void EvrUSBD_ADC_OnReceivedSamples(  uint8_t n) {
    EventRecord2(      EvtUSBD_ADC_OnReceivedSamples, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_ADC_OnReceivedSamples(...)
#endif


/**
  \brief  Event on \ref USBD_ADC_SetSpeakerVolumeRange finished successfully (API)
  \param  instance     instance of ADC class
  \param  ch           channel index
                        - value 0: master channel
                        - value 1: left speaker (in stereo mode)
                        - value 2: right speaker (in stereo mode)
  \param  min          minimum volume value
  \param  max          maximum volume value
  \param  res          volume resolution
  \param  cur          current volume value
 */
#ifdef                 EvtUSBD_ADC_SetSpeakerVolumeRange
  __STATIC_INLINE void EvrUSBD_ADC_SetSpeakerVolumeRange(  uint8_t instance,    uint8_t ch,   uint16_t min,    uint16_t max,          uint16_t res,    uint16_t cur) {
    EventRecord4(      EvtUSBD_ADC_SetSpeakerVolumeRange, TO_BYTE0(instance) | TO_BYTE1(ch), (uint32_t)min | ((uint32_t)max << 16),  (uint32_t)res | ((uint32_t)cur << 16), 0U);
  }
#else
  #define              EvrUSBD_ADC_SetSpeakerVolumeRange(...)
#endif


/**
  \brief  Event on \ref USBD_ADC_SetSpeakerVolumeRange failed (Error)
  \param  instance     instance of ADC class
  \param  ch           channel index
                        - value 0: master channel
                        - value 1: left speaker (in stereo mode)
                        - value 2: right speaker (in stereo mode)
  \param  min          minimum volume value
  \param  max          maximum volume value
  \param  res          volume resolution
  \param  cur          current volume value
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_ADC_SetSpeakerVolumeRangeFailed
  __STATIC_INLINE void EvrUSBD_ADC_SetSpeakerVolumeRangeFailed(  uint8_t instance,    uint8_t ch,   uint16_t min,    uint16_t max,          uint16_t res,    uint16_t cur,        usbStatus error) {
    EventRecord4(      EvtUSBD_ADC_SetSpeakerVolumeRangeFailed, TO_BYTE0(instance) | TO_BYTE1(ch), (uint32_t)min | ((uint32_t)max << 16),  (uint32_t)res | ((uint32_t)cur << 16), (uint32_t)error);
  }
#else
  #define              EvrUSBD_ADC_SetSpeakerVolumeRangeFailed(...)
#endif


/**
  \brief  Event on \ref USBD_ADC_SetMicrophoneVolumeRange finished successfully (API)
  \param  instance     instance of ADC class
  \param  ch           channel index
                        - value 0: master channel
                        - value 1: left microphone (in stereo mode)
                        - value 2: right microphone (in stereo mode)
  \param  min          minimum volume value
  \param  max          maximum volume value
  \param  res          volume resolution
  \param  cur          current volume value
 */
#ifdef                 EvtUSBD_ADC_SetMicrophoneVolumeRange
  __STATIC_INLINE void EvrUSBD_ADC_SetMicrophoneVolumeRange(  uint8_t instance,    uint8_t ch,   uint16_t min,    uint16_t max,          uint16_t res,    uint16_t cur) {
    EventRecord4(      EvtUSBD_ADC_SetMicrophoneVolumeRange, TO_BYTE0(instance) | TO_BYTE1(ch), (uint32_t)min | ((uint32_t)max << 16),  (uint32_t)res | ((uint32_t)cur << 16), 0U);
  }
#else
  #define              EvrUSBD_ADC_SetMicrophoneVolumeRange(...)
#endif


/**
  \brief  Event on \ref USBD_ADC_SetMicrophoneVolumeRange failed (Error)
  \param  instance     instance of ADC class
  \param  ch           channel index
                        - value 0: master channel
                        - value 1: left microphone (in stereo mode)
                        - value 2: right microphone (in stereo mode)
  \param  min          minimum volume value
  \param  max          maximum volume value
  \param  res          volume resolution
  \param  cur          current volume value
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_ADC_SetMicrophoneVolumeRangeFailed
  __STATIC_INLINE void EvrUSBD_ADC_SetMicrophoneVolumeRangeFailed(  uint8_t instance,    uint8_t ch,   uint16_t min,    uint16_t max,          uint16_t res,    uint16_t cur,        usbStatus error) {
    EventRecord4(      EvtUSBD_ADC_SetMicrophoneVolumeRangeFailed, TO_BYTE0(instance) | TO_BYTE1(ch), (uint32_t)min | ((uint32_t)max << 16),  (uint32_t)res | ((uint32_t)cur << 16), (uint32_t)error);
  }
#else
  #define              EvrUSBD_ADC_SetMicrophoneVolumeRangeFailed(...)
#endif


/**
  \brief  Event on \ref USBD_ADC_ReceivedSamplesAvailable finished (API)
  \param  instance     instance of ADC class
  \param  result       number of samples available to be read
 */
#ifdef                 EvtUSBD_ADC_ReceivedSamplesAvailable
  __STATIC_INLINE void EvrUSBD_ADC_ReceivedSamplesAvailable(  uint8_t instance,  uint32_t result) {
    EventRecord2(      EvtUSBD_ADC_ReceivedSamplesAvailable, TO_BYTE0(instance),          result);
  }
#else
  #define              EvrUSBD_ADC_ReceivedSamplesAvailable(...)
#endif


/**
  \brief  Event on \ref USBD_ADC_WrittenSamplesPending finished (API)
  \param  instance     instance of ADC class
  \param  result       number of samples ready to be sent
 */
#ifdef                 EvtUSBD_ADC_WrittenSamplesPending
  __STATIC_INLINE void EvrUSBD_ADC_WrittenSamplesPending(  uint8_t instance,  uint32_t result) {
    EventRecord2(      EvtUSBD_ADC_WrittenSamplesPending, TO_BYTE0(instance),          result);
  }
#else
  #define              EvrUSBD_ADC_WrittenSamplesPending(...)
#endif


/**
  \brief  Event on \ref USBD_ADC_ReadSamples finished successfully (API)
  \param  instance     instance of ADC class
  \param  result       number of samples read
 */
#ifdef                 EvtUSBD_ADC_ReadSamples
  __STATIC_INLINE void EvrUSBD_ADC_ReadSamples(  uint8_t instance,    int32_t result) {
    EventRecord2(      EvtUSBD_ADC_ReadSamples, TO_BYTE0(instance), (uint32_t)result);
  }
#else
  #define              EvrUSBD_ADC_ReadSamples(...)
#endif


/**
  \brief  Event on \ref USBD_ADC_ReadSamples failed (Error)
  \param  instance     instance of ADC class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_ADC_ReadSamplesFailed
  __STATIC_INLINE void EvrUSBD_ADC_ReadSamplesFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_ADC_ReadSamplesFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_ADC_ReadSamplesFailed(...)
#endif


/**
  \brief  Event on \ref USBD_ADC_WriteSamples finished successfully (API)
  \param  instance     instance of ADC class
  \param  result       number of samples written for sending
 */
#ifdef                 EvtUSBD_ADC_WriteSamples
  __STATIC_INLINE void EvrUSBD_ADC_WriteSamples(  uint8_t instance,    int32_t result) {
    EventRecord2(      EvtUSBD_ADC_WriteSamples, TO_BYTE0(instance), (uint32_t)result);
  }
#else
  #define              EvrUSBD_ADC_WriteSamples(...)
#endif


/**
  \brief  Event on \ref USBD_ADC_WriteSamples failed (Error)
  \param  instance     instance of ADC class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_ADC_WriteSamplesFailed
  __STATIC_INLINE void EvrUSBD_ADC_WriteSamplesFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_ADC_WriteSamplesFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_ADC_WriteSamplesFailed(...)
#endif


// USB Device CDC component event record functions -----------------------------

/**
  \brief  Event on internal library operation CDC initialize start (Operation)
  \param  instance     instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_Initialize
  __STATIC_INLINE void EvrUSBD_CDC_Initialize(  uint8_t instance) {
    EventRecord2(      EvtUSBD_CDC_Initialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_CDC_Initialize(...)
#endif


/**
  \brief  Event on internal library operation CDC initialize failed (Error)
  \param  instance     instance of CDC class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_CDC_InitializeFailed
  __STATIC_INLINE void EvrUSBD_CDC_InitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_CDC_InitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_CDC_InitializeFailed(...)
#endif


/**
  \brief  Event on internal library operation CDC uninitialize start (Operation)
  \param  instance     instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_Uninitialize
  __STATIC_INLINE void EvrUSBD_CDC_Uninitialize(  uint8_t instance) {
    EventRecord2(      EvtUSBD_CDC_Uninitialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_CDC_Uninitialize(...)
#endif


/**
  \brief  Event on internal library operation CDC uninitialize failed (Error)
  \param  instance     instance of CDC class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_CDC_UninitializeFailed
  __STATIC_INLINE void EvrUSBD_CDC_UninitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_CDC_UninitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_CDC_UninitializeFailed(...)
#endif


/**
  \brief  Event on internal library operation CDC reset start (Operation)
  \param  instance     instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_Reset
  __STATIC_INLINE void EvrUSBD_CDC_Reset(  uint8_t instance) {
    EventRecord2(      EvtUSBD_CDC_Reset, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_CDC_Reset(...)
#endif


/**
  \brief  Event on internal library operation CDC reset failed (Error)
  \param  instance     instance of CDC class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_CDC_ResetFailed
  __STATIC_INLINE void EvrUSBD_CDC_ResetFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_CDC_ResetFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_CDC_ResetFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_ACM_Initialize user callback function start (API)
  \param  n            instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_ACM_OnInitialize
  __STATIC_INLINE void EvrUSBD_CDC_ACM_OnInitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_CDC_ACM_OnInitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CDC_ACM_OnInitialize(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_ACM_Uninitialize user callback function start (API)
  \param  n            instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_ACM_OnUninitialize
  __STATIC_INLINE void EvrUSBD_CDC_ACM_OnUninitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_CDC_ACM_OnUninitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CDC_ACM_OnUninitialize(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_ACM_Reset user callback function start (API)
  \param  n            instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_ACM_OnReset
  __STATIC_INLINE void EvrUSBD_CDC_ACM_OnReset(  uint8_t n) {
    EventRecord2(      EvtUSBD_CDC_ACM_OnReset, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CDC_ACM_OnReset(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_ACM_SendEncapsulatedCommand user callback function finished (API)
  \param  n            instance of CDC class
  \param  buf          buffer that contains send encapsulated command request
  \param  len          length of send encapsulated command request
  \param  result       operation result
                        - true:  request processed
                        - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_ACM_OnSendEncapsulatedCommand
  __STATIC_INLINE void EvrUSBD_CDC_ACM_OnSendEncapsulatedCommand(  uint8_t n, uint32_t buf, uint16_t len, bool result) {
    EventRecord2(      EvtUSBD_CDC_ACM_OnSendEncapsulatedCommand, TO_BYTE0(n) | TO_BYTE1(result) | (((uint32_t)(len)) << 16), buf);
  }
#else
  #define              EvrUSBD_CDC_ACM_OnSendEncapsulatedCommand(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_ACM_GetEncapsulatedResponse user callback function finished (API)
  \param  n            instance of CDC class
  \param  max_len      maximum number of data bytes that USB Host expects to receive
  \param  buf          pointer to buffer containing get encapsulated response to be returned to USB Host
  \param  len          pointer to number of data bytes to be returned to USB Host
  \param  result       operation result
                        - true:  request processed
                        - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_ACM_OnGetEncapsulatedResponse
  __STATIC_INLINE void EvrUSBD_CDC_ACM_OnGetEncapsulatedResponse(  uint8_t n, uint16_t max_len, uint32_t buf, uint16_t len, bool result) {
    EventRecord4(      EvtUSBD_CDC_ACM_OnGetEncapsulatedResponse, TO_BYTE0(n) | TO_BYTE1(result) | (((uint32_t)(len)) << 16), buf, (uint32_t)max_len, 0U);
  }
#else
  #define              EvrUSBD_CDC_ACM_OnGetEncapsulatedResponse(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_ACM_SetLineCoding user callback function finished (API)
  \param  n            instance of CDC class
  \param  rate         requested data terminal bit rate
  \param  result       operation result
                        - true:  request processed
                        - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_ACM_OnSetLineCoding
  __STATIC_INLINE void EvrUSBD_CDC_ACM_OnSetLineCoding(  uint8_t n,            uint32_t rate, bool result) {
    EventRecord2(      EvtUSBD_CDC_ACM_OnSetLineCoding, TO_BYTE0(n) | TO_BYTE1(result), rate);
  }
#else
  #define              EvrUSBD_CDC_ACM_OnSetLineCoding(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_ACM_GetLineCoding user callback function finished (API)
  \param  n            instance of CDC class
  \param  rate         current data terminal bit rate
  \param  result       operation result
                        - true:  request processed
                        - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_ACM_OnGetLineCoding
  __STATIC_INLINE void EvrUSBD_CDC_ACM_OnGetLineCoding(  uint8_t n,            uint32_t rate, bool result) {
    EventRecord2(      EvtUSBD_CDC_ACM_OnGetLineCoding, TO_BYTE0(n) | TO_BYTE1(result), rate);
  }
#else
  #define              EvrUSBD_CDC_ACM_OnGetLineCoding(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_ACM_SetControlLineState user callback function finished (API)
  \param  n            instance of CDC class
  \param  dtr          DTR control line state
  \param  rts          RTS control line state
  \param  result       operation result
                        - true:  request processed
                        - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_ACM_OnSetControlLineState
  __STATIC_INLINE void EvrUSBD_CDC_ACM_OnSetControlLineState(  uint8_t n,    uint8_t dtr,    uint8_t rts,       bool result) {
    EventRecord2(      EvtUSBD_CDC_ACM_OnSetControlLineState, TO_BYTE0(n) | TO_BYTE1(dtr) | TO_BYTE2(rts), (uint32_t)result);
  }
#else
  #define              EvrUSBD_CDC_ACM_OnSetControlLineState(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_ACM_ReadData finished successfully (API)
  \param  instance     instance of CDC class
  \param  len          maximum number of bytes to read
  \param  result       number of bytes read
 */
#ifdef                 EvtUSBD_CDC_ACM_ReadData
  __STATIC_INLINE void EvrUSBD_CDC_ACM_ReadData(  uint8_t instance,    int32_t len,  uint32_t result) {
    EventRecord4(      EvtUSBD_CDC_ACM_ReadData, TO_BYTE0(instance), (uint32_t)len,           result, 0U);
  }
#else
  #define              EvrUSBD_CDC_ACM_ReadData(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_ACM_ReadData failed (Error)
  \param  instance     instance of CDC class
  \param  len          maximum number of bytes to read
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_CDC_ACM_ReadDataFailed
  __STATIC_INLINE void EvrUSBD_CDC_ACM_ReadDataFailed(  uint8_t instance,    int32_t len,   int32_t error) {
    EventRecord4(      EvtUSBD_CDC_ACM_ReadDataFailed, TO_BYTE0(instance), (uint32_t)len, (uint32_t)error, 0U);
  }
#else
  #define              EvrUSBD_CDC_ACM_ReadDataFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_ACM_WriteData finished successfully (API)
  \param  instance     instance of CDC class
  \param  len          maximum number of bytes to write
  \param  result       number of bytes accepted for writing
 */
#ifdef                 EvtUSBD_CDC_ACM_WriteData
  __STATIC_INLINE void EvrUSBD_CDC_ACM_WriteData(  uint8_t instance,    int32_t len,  uint32_t result) {
    EventRecord4(      EvtUSBD_CDC_ACM_WriteData, TO_BYTE0(instance), (uint32_t)len,           result, 0U);
  }
#else
  #define              EvrUSBD_CDC_ACM_WriteData(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_ACM_WriteData failed (Error)
  \param  instance     instance of CDC class
  \param  len          maximum number of bytes to write
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_CDC_ACM_WriteDataFailed
  __STATIC_INLINE void EvrUSBD_CDC_ACM_WriteDataFailed(  uint8_t instance,    int32_t len,   int32_t error) {
    EventRecord4(      EvtUSBD_CDC_ACM_WriteDataFailed, TO_BYTE0(instance), (uint32_t)len, (uint32_t)error, 0U);
  }
#else
  #define              EvrUSBD_CDC_ACM_WriteDataFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_ACM_GetChar finished (API)
  \param  instance     instance of CDC class
  \param  result       value of read character or no character received
                        - value >= 0: value of first received unread character
                        - value -1:   indicates no character was received
 */
#ifdef                 EvtUSBD_CDC_ACM_GetChar
  __STATIC_INLINE void EvrUSBD_CDC_ACM_GetChar(  uint8_t instance,    int32_t result) {
    EventRecord2(      EvtUSBD_CDC_ACM_GetChar, TO_BYTE0(instance), (uint32_t)result);
  }
#else
  #define              EvrUSBD_CDC_ACM_GetChar(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_ACM_PutChar finished (API)
  \param  instance     instance of CDC class
  \param  result       value of accepted character or no character accepted
                        - value >= 0: if character accepted for writing
                        - value -1:   indicates character not accepted
 */
#ifdef                 EvtUSBD_CDC_ACM_PutChar
  __STATIC_INLINE void EvrUSBD_CDC_ACM_PutChar(  uint8_t instance,    int32_t result) {
    EventRecord2(      EvtUSBD_CDC_ACM_PutChar, TO_BYTE0(instance), (uint32_t)result);
  }
#else
  #define              EvrUSBD_CDC_ACM_PutChar(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_ACM_DataAvailable finished successfully (API)
  \param  instance     instance of CDC class
  \param  result       number of bytes available to read
 */
#ifdef                 EvtUSBD_CDC_ACM_DataAvailable
  __STATIC_INLINE void EvrUSBD_CDC_ACM_DataAvailable(  uint8_t instance,  uint32_t result) {
    EventRecord2(      EvtUSBD_CDC_ACM_DataAvailable, TO_BYTE0(instance),          result);
  }
#else
  #define              EvrUSBD_CDC_ACM_DataAvailable(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_ACM_DataAvailable failed (Error)
  \param  instance     instance of CDC class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_CDC_ACM_DataAvailableFailed
  __STATIC_INLINE void EvrUSBD_CDC_ACM_DataAvailableFailed(  uint8_t instance,    int32_t error) {
    EventRecord2(      EvtUSBD_CDC_ACM_DataAvailableFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_CDC_ACM_DataAvailableFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_ACM_Notify_SerialState finished successfully (API)
  \param  instance     instance of CDC class
  \param  state        error status and line states:
                        - bit 6: bOverRun
                        - bit 5: bParity
                        - bit 4: bFraming
                        - bit 3: bRingSignal
                        - bit 2: bBreak
                        - bit 1: bTxCarrier (DSR line state)
                        - bit 0: bRxCarrier (DCD line state)
 */
#ifdef                 EvtUSBD_CDC_ACM_Notify_SerialState
  __STATIC_INLINE void EvrUSBD_CDC_ACM_Notify_SerialState(  uint8_t instance,   uint16_t state) {
    EventRecord2(      EvtUSBD_CDC_ACM_Notify_SerialState, TO_BYTE0(instance), (uint32_t)state);
  }
#else
  #define              EvrUSBD_CDC_ACM_Notify_SerialState(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_ACM_Notify_SerialState failed (Error)
  \param  instance     instance of CDC class
  \param  state        error status and line states:
                        - bit 6: bOverRun
                        - bit 5: bParity
                        - bit 4: bFraming
                        - bit 3: bRingSignal
                        - bit 2: bBreak
                        - bit 1: bTxCarrier (DSR line state)
                        - bit 0: bRxCarrier (DCD line state)
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_CDC_ACM_Notify_SerialStateFailed
  __STATIC_INLINE void EvrUSBD_CDC_ACM_Notify_SerialStateFailed(  uint8_t instance,   uint16_t state,  usbStatus error) {
    EventRecord2(      EvtUSBD_CDC_ACM_Notify_SerialStateFailed, TO_BYTE0(instance) | TO_BYTE3(error), (uint32_t)state);
  }
#else
  #define              EvrUSBD_CDC_ACM_Notify_SerialStateFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_ACM_Notify_ResponseAvailable finished successfully (API)
  \param  instance     instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_ACM_Notify_ResponseAvailable
  __STATIC_INLINE void EvrUSBD_CDC_ACM_Notify_ResponseAvailable(  uint8_t instance) {
    EventRecord2(      EvtUSBD_CDC_ACM_Notify_ResponseAvailable, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_CDC_ACM_Notify_ResponseAvailable(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_ACM_Notify_ResponseAvailable failed (Error)
  \param  instance     instance of CDC class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_CDC_ACM_Notify_ResponseAvailableFailed
  __STATIC_INLINE void EvrUSBD_CDC_ACM_Notify_ResponseAvailableFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_CDC_ACM_Notify_ResponseAvailableFailed, TO_BYTE0(instance) | TO_BYTE3(error), 0U);
  }
#else
  #define              EvrUSBD_CDC_ACM_Notify_ResponseAvailableFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_Initialize user callback function start (API)
  \param  n            instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_NCM_OnInitialize
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnInitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnInitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnInitialize(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_Uninitialize user callback function start (API)
  \param  n            instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_NCM_OnUninitialize
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnUninitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnUninitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnUninitialize(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_Reset user callback function start (API)
  \param  n            instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_NCM_OnReset
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnReset(  uint8_t n) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnReset, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnReset(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_Start user callback function start (API)
  \param  n            instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_NCM_OnStart
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnStart(  uint8_t n) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnStart, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnStart(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_Stop user callback function start (API)
  \param  n            instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_NCM_OnStop
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnStop(  uint8_t n) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnStop, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnStop(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_SetEthernetMulticastFilters user callback function finished (API)
  \param  n            instance of CDC class
  \param  result       operation result
                        - true:  request processed
                        - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnSetEthMulticastFilters
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnSetEthMulticastFilters(  uint8_t n,       bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnSetEthMulticastFilters, TO_BYTE0(n) | TO_BYTE1(result), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnSetEthMulticastFilters(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_SetEthernetPowerManagementPatternFilter user callback function finished (API)
  \param  n             instance of CDC class
  \param  filter_number filter number
  \param  result        operation result
                         - true:  request processed
                         - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnSetEthPowerMgmtPatFilter
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnSetEthPowerMgmtPatFilter(  uint8_t n,                      uint16_t filter_number, bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnSetEthPowerMgmtPatFilter, TO_BYTE0(n) | TO_BYTE1(result), (uint32_t)filter_number);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnSetEthPowerMgmtPatFilter(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_GetEthernetPowerManagementPatternFilter user callback function finished (API)
  \param  n             instance of CDC class
  \param  filter_number filter number
  \param  result        operation result
                         - true:  request processed
                         - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnGetEthPowerMgmtPatFilter
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnGetEthPowerMgmtPatFilter(  uint8_t n,                      uint16_t filter_number, bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnGetEthPowerMgmtPatFilter, TO_BYTE0(n) | TO_BYTE1(result), (uint32_t)filter_number);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnGetEthPowerMgmtPatFilter(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_SetEthernetPacketFilter user callback function finished (API)
  \param  n                    instance of CDC class
  \param  packet_filter_bitmap packet filter bitmap
  \param  result               operation result
                                - true:  request processed
                                - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnSetEthernetPacketFilter
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnSetEthernetPacketFilter(  uint8_t n,                      uint16_t packet_filter_bitmap, bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnSetEthernetPacketFilter, TO_BYTE0(n) | TO_BYTE1(result), (uint32_t)packet_filter_bitmap);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnSetEthernetPacketFilter(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_GetEthernetStatistic user callback function finished (API)
  \param  n                instance of CDC class
  \param  feature_selector feature selector
  \param  result           operation result
                            - true:  request processed
                            - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnGetEthernetStatistic
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnGetEthernetStatistic(  uint8_t n,                      uint16_t feature_selector, bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnGetEthernetStatistic, TO_BYTE0(n) | TO_BYTE1(result), (uint32_t)feature_selector);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnGetEthernetStatistic(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_GetNtbParameters user callback function finished (API)
  \param  n            instance of CDC class
  \param  result       operation result
                        - true:  request processed
                        - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnGetNtbParameters
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnGetNtbParameters(  uint8_t n,       bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnGetNtbParameters, TO_BYTE0(n) | TO_BYTE1(result), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnGetNtbParameters(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_GetNetAddress user callback function finished (API)
  \param  n            instance of CDC class
  \param  result       operation result
                        - true:  request processed
                        - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnGetNetAddress
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnGetNetAddress(  uint8_t n,       bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnGetNetAddress, TO_BYTE0(n) | TO_BYTE1(result), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnGetNetAddress(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_SetNetAddress user callback function finished (API)
  \param  n            instance of CDC class
  \param  result       operation result
                        - true:  request processed
                        - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnSetNetAddress
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnSetNetAddress(  uint8_t n,       bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnSetNetAddress, TO_BYTE0(n) | TO_BYTE1(result), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnSetNetAddress(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_GetNtbFormat user callback function finished (API)
  \param  n            instance of CDC class
  \param  ntb_format   NTB format
                        - value 0 : NTB-16
                        - value 1 : NTB-32
  \param  result       operation result
                        - true:  request processed
                        - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnGetNtbFormat
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnGetNtbFormat(  uint8_t n,                      uint16_t ntb_format, bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnGetNtbFormat, TO_BYTE0(n) | TO_BYTE1(result), (uint32_t)ntb_format);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnGetNtbFormat(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_SetNtbFormat user callback function finished (API)
  \param  n            instance of CDC class
  \param  ntb_format   NTB format
                        - value 0 : NTB-16
                        - value 1 : NTB-32
  \param  result       operation result
                        - true:  request processed
                        - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnSetNtbFormat
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnSetNtbFormat(  uint8_t n,                      uint16_t ntb_format, bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnSetNtbFormat, TO_BYTE0(n) | TO_BYTE1(result), (uint32_t)ntb_format);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnSetNtbFormat(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_GetNtbInputSize user callback function finished (API)
  \param  n                instance of CDC class
  \param  ntb_input_size   maximum NTB size
  \param  result           operation result
                            - true:  request processed
                            - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnGetNtbInputSize
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnGetNtbInputSize(  uint8_t n,            uint32_t ntb_input_size, bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnGetNtbInputSize, TO_BYTE0(n) | TO_BYTE1(result), ntb_input_size);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnGetNtbInputSize(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_SetNtbInputSize user callback function finished (API)
  \param  n                instance of CDC class
  \param  ntb_input_size   maximum NTB size
  \param  result           operation result
                            - true:  request processed
                            - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnSetNtbInputSize
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnSetNtbInputSize(  uint8_t n,            uint32_t ntb_input_size, bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnSetNtbInputSize, TO_BYTE0(n) | TO_BYTE1(result), ntb_input_size);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnSetNtbInputSize(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_GetMaxDatagramSize user callback function finished (API)
  \param  n                  instance of CDC class
  \param  max_datagram_size  maximum datagram size
  \param  result             operation result
                              - true:  request processed
                              - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnGetMaxDatagramSize
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnGetMaxDatagramSize(  uint8_t n,                      uint16_t max_datagram_size, bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnGetMaxDatagramSize, TO_BYTE0(n) | TO_BYTE1(result), (uint32_t)max_datagram_size);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnGetMaxDatagramSize(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_SetMaxDatagramSize user callback function finished (API)
  \param  n                  instance of CDC class
  \param  max_datagram_size  maximum datagram size
  \param  result             operation result
                              - true:  request processed
                              - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnSetMaxDatagramSize
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnSetMaxDatagramSize(  uint8_t n,                      uint16_t max_datagram_size, bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnSetMaxDatagramSize, TO_BYTE0(n) | TO_BYTE1(result), (uint32_t)max_datagram_size);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnSetMaxDatagramSize(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_GetCrcMode user callback function finished (API)
  \param  n            instance of CDC class
  \param  crc_mode     current CRC mode
  \param  result       operation result
                        - true:  request processed
                        - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnGetCrcMode
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnGetCrcMode(  uint8_t n,                      uint16_t crc_mode, bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnGetCrcMode, TO_BYTE0(n) | TO_BYTE1(result), (uint32_t)crc_mode);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnGetCrcMode(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_SetCrcMode user callback function finished (API)
  \param  n            instance of CDC class
  \param  crc_mode     CRC mode to set
  \param  result       operation result
                        - true:  request processed
                        - false: request not supported or not processed
 */
#ifdef                 EvtUSBD_CDC_NCM_OnSetCrcMode
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnSetCrcMode(  uint8_t n,                      uint16_t crc_mode, bool result) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnSetCrcMode, TO_BYTE0(n) | TO_BYTE1(result), (uint32_t)crc_mode);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnSetCrcMode(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_NTB_IN_Sent user callback function finished (API)
  \param  n            instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_NCM_OnNTB_IN_Sent
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnNTB_IN_Sent(  uint8_t n) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnNTB_IN_Sent, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnNTB_IN_Sent(...)
#endif


/**
  \brief  Event on \ref USBD_CDCn_NCM_NTB_OUT_Received user callback function finished (API)
  \param  n            instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_NCM_OnNTB_OUT_Received
  __STATIC_INLINE void EvrUSBD_CDC_NCM_OnNTB_OUT_Received(  uint8_t n) {
    EventRecord2(      EvtUSBD_CDC_NCM_OnNTB_OUT_Received, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_OnNTB_OUT_Received(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_IN_Initialize finished successfully (API)
  \param  instance     instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_IN_Initialize
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_IN_Initialize(  uint8_t instance) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_IN_Initialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_IN_Initialize(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_IN_Initialize failed (Error)
  \param  instance     instance of CDC class
  \param  error        error code
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_IN_InitializeFailed
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_IN_InitializeFailed(  uint8_t instance,    int32_t error) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_IN_InitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_IN_InitializeFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_IN_CreateNDP finished successfully (API)
  \param  instance      instance of CDC class
  \param  num_datagrams maximum number of datagrams that NDP will contain
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_IN_CreateNDP
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_IN_CreateNDP(  uint8_t instance,    uint8_t num_datagrams) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_IN_CreateNDP, TO_BYTE0(instance) | TO_BYTE1(num_datagrams), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_IN_CreateNDP(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_IN_CreateNDP failed (Error)
  \param  instance      instance of CDC class
  \param  num_datagrams maximum number of datagrams that NDP will contain
  \param  error         error code
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_IN_CreateNDPFailed
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_IN_CreateNDPFailed(  uint8_t instance,    uint8_t num_datagrams,    int32_t error) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_IN_CreateNDPFailed, TO_BYTE0(instance) | TO_BYTE1(num_datagrams), (uint32_t)error);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_IN_CreateNDPFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_IN_WriteDatagram finished successfully (API)
  \param  instance     instance of CDC class
  \param  len          number of bytes to write
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_IN_WriteDatagram
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_IN_WriteDatagram(  uint8_t instance,  uint32_t len) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_IN_WriteDatagram, TO_BYTE0(instance),          len);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_IN_WriteDatagram(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_IN_WriteDatagram failed (Error)
  \param  instance     instance of CDC class
  \param  len          number of bytes to write
  \param  error        error code
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_IN_WriteDatagramFailed
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_IN_WriteDatagramFailed(  uint8_t instance,                     uint32_t len,    int32_t error) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_IN_WriteDatagramFailed, TO_BYTE0(instance) | TO_BYTE3(error),           len);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_IN_WriteDatagramFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_IN_Send finished successfully (API)
  \param  instance     instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_IN_Send
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_IN_Send(  uint8_t instance) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_IN_Send, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_IN_Send(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_IN_Send failed (Error)
  \param  instance     instance of CDC class
  \param  error        error code
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_IN_SendFailed
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_IN_SendFailed(  uint8_t instance,    int32_t error) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_IN_SendFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_IN_SendFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_IN_IsSent finished successfully (API)
  \param  instance     instance of CDC class
  \param  result       NTB sending status
                        - value 1: NTB was sent
                        - value 0: NTB sending is in progress
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_IN_IsSent
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_IN_IsSent(  uint8_t instance,  uint32_t result) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_IN_IsSent, TO_BYTE0(instance),          result);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_IN_IsSent(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_IN_IsSent failed (Error)
  \param  instance     instance of CDC class
  \param  error        error code
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_IN_IsSentFailed
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_IN_IsSentFailed(  uint8_t instance,    int32_t error) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_IN_IsSentFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_IN_IsSentFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_OUT_IsReceived finished successfully (API)
  \param  instance     instance of CDC class
  \param  result       NTB receiving status
                        - value 1: NTB was received
                        - value 0: NTB reception is in progress
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_OUT_IsReceived
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_OUT_IsReceived(  uint8_t instance,  uint32_t result) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_OUT_IsReceived, TO_BYTE0(instance),          result);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_OUT_IsReceived(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_OUT_IsReceived failed (Error)
  \param  instance     instance of CDC class
  \param  error        error code
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_OUT_IsReceivedFailed
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_OUT_IsReceivedFailed(  uint8_t instance,    int32_t error) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_OUT_IsReceivedFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_OUT_IsReceivedFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_OUT_Release finished successfully (API)
  \param  instance     instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_OUT_Release
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_OUT_Release(  uint8_t instance) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_OUT_Release, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_OUT_Release(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_OUT_Release failed (Error)
  \param  instance     instance of CDC class
  \param  error        error code
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_OUT_ReleaseFailed
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_OUT_ReleaseFailed(  uint8_t instance,    int32_t error) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_OUT_ReleaseFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_OUT_ReleaseFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_OUT_ProcessNDP finished successfully (API)
  \param  instance     instance of CDC class
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_OUT_ProcessNDP
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_OUT_ProcessNDP(  uint8_t instance) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_OUT_ProcessNDP, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_OUT_ProcessNDP(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_OUT_ProcessNDP failed (Error)
  \param  instance     instance of CDC class
  \param  error        error code
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_OUT_ProcessNDPFailed
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_OUT_ProcessNDPFailed(  uint8_t instance,    int32_t error) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_OUT_ProcessNDPFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_OUT_ProcessNDPFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_OUT_GetDatagramSize finished (API)
  \param  instance     instance of CDC class
  \param  result       number of bytes available in the datagram
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_OUT_GetDatagramSize
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_OUT_GetDatagramSize(  uint8_t instance,  uint32_t result) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_OUT_GetDatagramSize, TO_BYTE0(instance),          result);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_OUT_GetDatagramSize(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_OUT_ReadDatagram finished successfully (API)
  \param  instance     instance of CDC class
  \param  max_len      maximum number of bytes to read
  \param  result       number of bytes read from the datagram
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_OUT_ReadDatagram
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_OUT_ReadDatagram(  uint8_t instance,  uint32_t max_len, uint32_t result) {
    EventRecord4(      EvtUSBD_CDC_NCM_NTB_OUT_ReadDatagram, TO_BYTE0(instance),          max_len,          result, 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_OUT_ReadDatagram(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_OUT_ReadDatagram failed (Error)
  \param  instance     instance of CDC class
  \param  max_len      maximum number of bytes to read
  \param  error        error code
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_OUT_ReadDatagramFailed
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_OUT_ReadDatagramFailed(  uint8_t instance,                    uint32_t max_len,   int32_t error) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_OUT_ReadDatagramFailed, TO_BYTE0(instance) | TO_BYTE3(error),          max_len);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_OUT_ReadDatagramFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_IN_RawSend finished successfully (API)
  \param  instance     instance of CDC class
  \param  len          size of NTB
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_IN_RawSend
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_IN_RawSend(  uint8_t instance,  uint32_t len) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_IN_RawSend, TO_BYTE0(instance),          len);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_IN_RawSend(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_IN_RawSend failed (Error)
  \param  instance     instance of CDC class
  \param  len          size of NTB
  \param  error        error code
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_IN_RawSendFailed
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_IN_RawSendFailed(  uint8_t instance,                   uint32_t len,   int32_t error) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_IN_RawSendFailed, TO_BYTE0(instance) | TO_BYTE1(error),         len);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_IN_RawSendFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_OUT_RawGetSize finished (API)
  \param  instance     instance of CDC class
  \param  result       number of bytes available in the NTB
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_OUT_RawGetSize
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_OUT_RawGetSize(  uint8_t instance,  uint32_t result) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_OUT_RawGetSize, TO_BYTE0(instance),          result);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_OUT_RawGetSize(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_OUT_RawReceive finished successfully (API)
  \param  instance     instance of CDC class
  \param  max_len      maximum number of bytes that buffer can accept
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_OUT_RawReceive
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_OUT_RawReceive(  uint8_t instance,  uint32_t max_len) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_OUT_RawReceive, TO_BYTE0(instance),          max_len);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_OUT_RawReceive(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_NTB_OUT_RawReceive failed (Error)
  \param  instance     instance of CDC class
  \param  max_len      maximum number of bytes that buffer can accept
  \param  error        error code
 */
#ifdef                 EvtUSBD_CDC_NCM_NTB_OUT_RawReceiveFailed
  __STATIC_INLINE void EvrUSBD_CDC_NCM_NTB_OUT_RawReceiveFailed(  uint8_t instance,                   uint32_t max_len, int32_t error) {
    EventRecord2(      EvtUSBD_CDC_NCM_NTB_OUT_RawReceiveFailed, TO_BYTE0(instance) | TO_BYTE3(error),         max_len);
  }
#else
  #define              EvrUSBD_CDC_NCM_NTB_OUT_RawReceiveFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_Notify_NetworkConnection finished successfully (API)
  \param  instance     instance of CDC class
  \param  status       connection status
                        - value 1: connected
                        - value 0: disconnected
 */
#ifdef                 EvtUSBD_CDC_NCM_Notify_NetworkConnection
  __STATIC_INLINE void EvrUSBD_CDC_NCM_Notify_NetworkConnection(  uint8_t instance,   uint16_t status) {
    EventRecord2(      EvtUSBD_CDC_NCM_Notify_NetworkConnection, TO_BYTE0(instance), (uint32_t)status);
  }
#else
  #define              EvrUSBD_CDC_NCM_Notify_NetworkConnection(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_Notify_NetworkConnection failed (Error)
  \param  instance     instance of CDC class
  \param  status       connection status
                        - value 1: connected
                        - value 0: disconnected
  \param  error        error code
 */
#ifdef                 EvtUSBD_CDC_NCM_Notify_NetworkConnectionFailed
  __STATIC_INLINE void EvrUSBD_CDC_NCM_Notify_NetworkConnectionFailed(  uint8_t instance,                     uint16_t status, int32_t error) {
    EventRecord2(      EvtUSBD_CDC_NCM_Notify_NetworkConnectionFailed, TO_BYTE0(instance) | TO_BYTE3(error), (uint32_t)status);
  }
#else
  #define              EvrUSBD_CDC_NCM_Notify_NetworkConnectionFailed(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_Notify_ConnectionSpeedChange finished successfully (API)
  \param  instance     instance of CDC class
  \param  us_bitrate   upstream bitrate
  \param  ds_bitrate   downstream bitrate
 */
#ifdef                 EvtUSBD_CDC_NCM_Notify_ConnectionSpeedChange
  __STATIC_INLINE void EvrUSBD_CDC_NCM_Notify_ConnectionSpeedChange(  uint8_t instance,  uint32_t us_bitrate, uint32_t ds_bitrate) {
    EventRecord4(      EvtUSBD_CDC_NCM_Notify_ConnectionSpeedChange, TO_BYTE0(instance),          us_bitrate,          ds_bitrate, 0U);
  }
#else
  #define              EvrUSBD_CDC_NCM_Notify_ConnectionSpeedChange(...)
#endif


/**
  \brief  Event on \ref USBD_CDC_NCM_Notify_ConnectionSpeedChange failed (Error)
  \param  instance     instance of CDC class
  \param  us_bitrate   upstream bitrate
  \param  ds_bitrate   downstream bitrate
  \param  error        error code
 */
#ifdef                 EvtUSBD_CDC_NCM_Notify_ConnectionSpeedChangeFailed
  __STATIC_INLINE void EvrUSBD_CDC_NCM_Notify_ConnectionSpeedChangeFailed(  uint8_t instance,  uint32_t us_bitrate, uint32_t ds_bitrate,   int32_t error) {
    EventRecord4(      EvtUSBD_CDC_NCM_Notify_ConnectionSpeedChangeFailed, TO_BYTE0(instance),          us_bitrate,          ds_bitrate, (uint32_t)error);
  }
#else
  #define              EvrUSBD_CDC_NCM_Notify_ConnectionSpeedChangeFailed(...)
#endif


// USB Device HID component event record functions -----------------------------

/**
  \brief  Event on internal library operation HID initialize start (Operation)
  \param  instance     instance of HID class
 */
#ifdef                 EvtUSBD_HID_Initialize
  __STATIC_INLINE void EvrUSBD_HID_Initialize(  uint8_t instance) {
    EventRecord2(      EvtUSBD_HID_Initialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_HID_Initialize(...)
#endif


/**
  \brief  Event on internal library operation HID initialize failed (Error)
  \param  instance     instance of HID class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_HID_InitializeFailed
  __STATIC_INLINE void EvrUSBD_HID_InitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_HID_InitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_HID_InitializeFailed(...)
#endif


/**
  \brief  Event on internal library operation HID uninitialize start (Operation)
  \param  instance     instance of HID class
 */
#ifdef                 EvtUSBD_HID_Uninitialize
  __STATIC_INLINE void EvrUSBD_HID_Uninitialize(  uint8_t instance) {
    EventRecord2(      EvtUSBD_HID_Uninitialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_HID_Uninitialize(...)
#endif


/**
  \brief  Event on internal library operation HID uninitialize failed (Error)
  \param  instance     instance of HID class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_HID_UninitializeFailed
  __STATIC_INLINE void EvrUSBD_HID_UninitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_HID_UninitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_HID_UninitializeFailed(...)
#endif


/**
  \brief  Event on internal library operation HID reset start (Operation)
  \param  instance     instance of HID class
 */
#ifdef                 EvtUSBD_HID_Reset
  __STATIC_INLINE void EvrUSBD_HID_Reset(  uint8_t instance) {
    EventRecord2(      EvtUSBD_HID_Reset, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_HID_Reset(...)
#endif


/**
  \brief  Event on internal library operation HID reset failed (Error)
  \param  instance     instance of HID class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_HID_ResetFailed
  __STATIC_INLINE void EvrUSBD_HID_ResetFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_HID_ResetFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_HID_ResetFailed(...)
#endif


/**
  \brief  Event on internal library operation Get Report start (Operation)
  \param  instance     instance of HID class
 */
#ifdef                 EvtUSBD_HID_GetReport
  __STATIC_INLINE void EvrUSBD_HID_GetReport(  uint8_t instance) {
    EventRecord2(      EvtUSBD_HID_GetReport, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_HID_GetReport(...)
#endif


/**
  \brief  Event on internal library operation Get Report failed (Error)
  \param  instance     instance of HID class
 */
#ifdef                 EvtUSBD_HID_GetReportFailed
  __STATIC_INLINE void EvrUSBD_HID_GetReportFailed(  uint8_t instance) {
    EventRecord2(      EvtUSBD_HID_GetReportFailed, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_HID_GetReportFailed(...)
#endif


/**
  \brief  Event on internal library operation Set Report start (Operation)
  \param  instance     instance of HID class
 */
#ifdef                 EvtUSBD_HID_SetReport
  __STATIC_INLINE void EvrUSBD_HID_SetReport(  uint8_t instance) {
    EventRecord2(      EvtUSBD_HID_SetReport, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_HID_SetReport(...)
#endif


/**
  \brief  Event on internal library operation Set Report failed (Error)
  \param  instance     instance of HID class
 */
#ifdef                 EvtUSBD_HID_SetReportFailed
  __STATIC_INLINE void EvrUSBD_HID_SetReportFailed(  uint8_t instance) {
    EventRecord2(      EvtUSBD_HID_SetReportFailed, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_HID_SetReportFailed(...)
#endif


/**
  \brief  Event on internal library operation Get Idle start (Operation)
  \param  instance     instance of HID class
  \param  rid          report ID
  \param  idle         current idle setting
 */
#ifdef                 EvtUSBD_HID_GetIdle
  __STATIC_INLINE void EvrUSBD_HID_GetIdle(  uint8_t instance,    uint8_t rid,    uint8_t idle) {
    EventRecord2(      EvtUSBD_HID_GetIdle, TO_BYTE0(instance) | TO_BYTE1(rid), (uint32_t)idle);
  }
#else
  #define              EvrUSBD_HID_GetIdle(...)
#endif


/**
  \brief  Event on internal library operation Get Idle failed (Error)
  \param  instance     instance of HID class
 */
#ifdef                 EvtUSBD_HID_GetIdleFailed
  __STATIC_INLINE void EvrUSBD_HID_GetIdleFailed(  uint8_t instance) {
    EventRecord2(      EvtUSBD_HID_GetIdleFailed, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_HID_GetIdleFailed(...)
#endif


/**
  \brief  Event on internal library operation Set Idle start (Operation)
  \param  instance     instance of HID class
  \param  rid          report ID
  \param  idle         idle to set
 */
#ifdef                 EvtUSBD_HID_SetIdle
  __STATIC_INLINE void EvrUSBD_HID_SetIdle(  uint8_t instance,    uint8_t rid,    uint8_t idle) {
    EventRecord2(      EvtUSBD_HID_SetIdle, TO_BYTE0(instance) | TO_BYTE1(rid), (uint32_t)idle);
  }
#else
  #define              EvrUSBD_HID_SetIdle(...)
#endif


/**
  \brief  Event on internal library operation Set Idle failed (Error)
  \param  instance     instance of HID class
 */
#ifdef                 EvtUSBD_HID_SetIdleFailed
  __STATIC_INLINE void EvrUSBD_HID_SetIdleFailed(  uint8_t instance) {
    EventRecord2(      EvtUSBD_HID_SetIdleFailed, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_HID_SetIdleFailed(...)
#endif


/**
  \brief  Event on internal library operation Get Protocol start (Operation)
  \param  instance     instance of HID class
  \param  protocol     current protocol
 */
#ifdef                 EvtUSBD_HID_GetProtocol
  __STATIC_INLINE void EvrUSBD_HID_GetProtocol(  uint8_t instance,     int8_t protocol) {
    EventRecord2(      EvtUSBD_HID_GetProtocol, TO_BYTE0(instance), (uint32_t)protocol);
  }
#else
  #define              EvrUSBD_HID_GetProtocol(...)
#endif


/**
  \brief  Event on internal library operation Get Protocol failed (Error)
  \param  instance     instance of HID class
 */
#ifdef                 EvtUSBD_HID_GetProtocolFailed
  __STATIC_INLINE void EvrUSBD_HID_GetProtocolFailed(  uint8_t instance) {
    EventRecord2(      EvtUSBD_HID_GetProtocolFailed, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_HID_GetProtocolFailed(...)
#endif


/**
  \brief  Event on internal library operation Set Protocol start (Operation)
  \param  instance     instance of HID class
  \param  protocol     protocol to set
 */
#ifdef                 EvtUSBD_HID_SetProtocol
  __STATIC_INLINE void EvrUSBD_HID_SetProtocol(  uint8_t instance,    uint8_t protocol) {
    EventRecord2(      EvtUSBD_HID_SetProtocol, TO_BYTE0(instance), (uint32_t)protocol);
  }
#else
  #define              EvrUSBD_HID_SetProtocol(...)
#endif


/**
  \brief  Event on internal library operation Set Protocol failed (Error)
  \param  instance     instance of HID class
 */
#ifdef                 EvtUSBD_HID_SetProtocolFailed
  __STATIC_INLINE void EvrUSBD_HID_SetProtocolFailed(  uint8_t instance) {
    EventRecord2(      EvtUSBD_HID_SetProtocolFailed, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_HID_SetProtocolFailed(...)
#endif


/**
  \brief  Event on USBD_HIDn_Initialize user callback function start (API)
  \param  n            instance of HID class
 */
#ifdef                 EvtUSBD_HID_OnInitialize
  __STATIC_INLINE void EvrUSBD_HID_OnInitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_HID_OnInitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_HID_OnInitialize(...)
#endif


/**
  \brief  Event on USBD_HIDn_Uninitialize user callback function start (API)
  \param  n            instance of HID class
 */
#ifdef                 EvtUSBD_HID_OnUninitialize
  __STATIC_INLINE void EvrUSBD_HID_OnUninitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_HID_OnUninitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_HID_OnUninitialize(...)
#endif


/**
  \brief  Event on USBD_HIDn_GetReport user callback function finished successfully (API)
  \param  n            instance of HID class
  \param  rtype        report type:
                        - HID_REPORT_INPUT           = input report requested
                        - HID_REPORT_FEATURE         = feature report requested
  \param  req          request type:
                        - USBD_HID_REQ_EP_CTRL       = control endpoint request
                        - USBD_HID_REQ_PERIOD_UPDATE = idle period expiration request
                        - USBD_HID_REQ_EP_INT        = previously sent report on interrupt endpoint request
  \param  rid          report ID (0 if only one report exists)
  \param  len          number of report data bytes prepared to send
 */
#ifdef                 EvtUSBD_HID_OnGetReport
  __STATIC_INLINE void EvrUSBD_HID_OnGetReport(  uint8_t n,    uint8_t rtype,    uint8_t req,    uint8_t rid,    int32_t len) {
    EventRecord2(      EvtUSBD_HID_OnGetReport, TO_BYTE0(n) | TO_BYTE1(rtype) | TO_BYTE2(req) | TO_BYTE3(rid), (uint32_t)len);
  }
#else
  #define              EvrUSBD_HID_OnGetReport(...)
#endif


/**
  \brief  Event on USBD_HIDn_GetReport user callback function failed (Error)
  \param  n            instance of HID class
  \param  rtype        report type:
                        - HID_REPORT_INPUT           = input report requested
                        - HID_REPORT_FEATURE         = feature report requested
  \param  req          request type:
                        - USBD_HID_REQ_EP_CTRL       = control endpoint request
                        - USBD_HID_REQ_PERIOD_UPDATE = idle period expiration request
                        - USBD_HID_REQ_EP_INT        = previously sent report on interrupt endpoint request
  \param  rid          report ID (0 if only one report exists)
 */
#ifdef                 EvtUSBD_HID_OnGetReportFailed
  __STATIC_INLINE void EvrUSBD_HID_OnGetReportFailed(  uint8_t n,    uint8_t rtype,    uint8_t req,    uint8_t rid) {
    EventRecord2(      EvtUSBD_HID_OnGetReportFailed, TO_BYTE0(n) | TO_BYTE1(rtype) | TO_BYTE2(req) | TO_BYTE3(rid), 0U);
  }
#else
  #define              EvrUSBD_HID_OnGetReportFailed(...)
#endif


/**
  \brief  Event on USBD_HIDn_SetReport user callback function finished successfully (API)
  \param  n            instance of HID class
  \param  rtype        report type:
                        - HID_REPORT_OUTPUT          = output report requested
                        - HID_REPORT_FEATURE         = feature report requested
  \param  req          request type:
                        - USBD_HID_REQ_EP_CTRL       = report received on control endpoint
                        - USBD_HID_REQ_EP_INT        = report received on interrupt endpoint
  \param  rid          report ID (0 if only one report exists)
  \param  len          length of received report data
 */
#ifdef                 EvtUSBD_HID_OnSetReport
  __STATIC_INLINE void EvrUSBD_HID_OnSetReport(  uint8_t n,    uint8_t rtype,    uint8_t req,    uint8_t rid,    int32_t len) {
    EventRecord2(      EvtUSBD_HID_OnSetReport, TO_BYTE0(n) | TO_BYTE1(rtype) | TO_BYTE2(req) | TO_BYTE3(rid), (uint32_t)len);
  }
#else
  #define              EvrUSBD_HID_OnSetReport(...)
#endif


/**
  \brief  Event on USBD_HIDn_SetReport user callback function failed (Error)
  \param  n            instance of HID class
  \param  rtype        report type:
                        - HID_REPORT_OUTPUT          = output report requested
                        - HID_REPORT_FEATURE         = feature report requested
  \param  req          request type:
                        - USBD_HID_REQ_EP_CTRL       = report received on control endpoint
                        - USBD_HID_REQ_EP_INT        = report received on interrupt endpoint
  \param  rid          report ID (0 if only one report exists)
  \param  len          length of received report data
 */
#ifdef                 EvtUSBD_HID_OnSetReportFailed
  __STATIC_INLINE void EvrUSBD_HID_OnSetReportFailed(  uint8_t n,    uint8_t rtype,    uint8_t req,    uint8_t rid,    int32_t len) {
    EventRecord2(      EvtUSBD_HID_OnSetReportFailed, TO_BYTE0(n) | TO_BYTE1(rtype) | TO_BYTE2(req) | TO_BYTE3(rid), (uint32_t)len);
  }
#else
  #define              EvrUSBD_HID_OnSetReportFailed(...)
#endif


/**
  \brief  Event on USBD_HIDn_GetProtocol user callback function finished successfully (API)
  \param  n            instance of HID class
  \param  protocol     current protocol
 */
#ifdef                 EvtUSBD_HID_OnGetProtocol
  __STATIC_INLINE void EvrUSBD_HID_OnGetProtocol(  uint8_t n,     int8_t protocol) {
    EventRecord2(      EvtUSBD_HID_OnGetProtocol, TO_BYTE0(n), (uint32_t)protocol);
  }
#else
  #define              EvrUSBD_HID_OnGetProtocol(...)
#endif


/**
  \brief  Event on USBD_HIDn_GetProtocol user callback function failed (Error)
  \param  n            instance of HID class
 */
#ifdef                 EvtUSBD_HID_OnGetProtocolFailed
  __STATIC_INLINE void EvrUSBD_HID_OnGetProtocolFailed(  uint8_t n) {
    EventRecord2(      EvtUSBD_HID_OnGetProtocolFailed, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_HID_OnGetProtocolFailed(...)
#endif


/**
  \brief  Event on USBD_HIDn_SetProtocol user callback function finished successfully (API)
  \param  n            instance of HID class
  \param  protocol     protocol to set
 */
#ifdef                 EvtUSBD_HID_OnSetProtocol
  __STATIC_INLINE void EvrUSBD_HID_OnSetProtocol(  uint8_t n,    uint8_t protocol) {
    EventRecord2(      EvtUSBD_HID_OnSetProtocol, TO_BYTE0(n), (uint32_t)protocol);
  }
#else
  #define              EvrUSBD_HID_OnSetProtocol(...)
#endif


/**
  \brief  Event on USBD_HIDn_SetProtocol user callback function failed (Error)
  \param  n            instance of HID class
  \param  protocol     protocol to set
 */
#ifdef                 EvtUSBD_HID_OnSetProtocolFailed
  __STATIC_INLINE void EvrUSBD_HID_OnSetProtocolFailed(  uint8_t n,    uint8_t protocol) {
    EventRecord2(      EvtUSBD_HID_OnSetProtocolFailed, TO_BYTE0(n), (uint32_t)protocol);
  }
#else
  #define              EvrUSBD_HID_OnSetProtocolFailed(...)
#endif


/**
  \brief  Event on \ref USBD_HID_GetReportTrigger start (API)
  \param  instance     instance of HID class
  \param  rid          report ID
  \param  buf          pointer to data buffer with data to be sent
  \param  len          number of bytes to be sent
 */
#ifdef                 EvtUSBD_HID_GetReportTrigger
  __STATIC_INLINE void EvrUSBD_HID_GetReportTrigger(  uint8_t instance,  uint32_t rid, const uint8_t *buf, uint32_t len) {
    EventRecord4(      EvtUSBD_HID_GetReportTrigger, TO_BYTE0(instance),          rid,      (uint32_t)buf,          len);
  }
#else
  #define              EvrUSBD_HID_GetReportTrigger(...)
#endif


/**
  \brief  Event on \ref USBD_HID_GetReportTrigger failed (Error)
  \param  instance     instance of HID class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_HID_GetReportTriggerFailed
  __STATIC_INLINE void EvrUSBD_HID_GetReportTriggerFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_HID_GetReportTriggerFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_HID_GetReportTriggerFailed(...)
#endif



// USB Device MSC component event record functions -----------------------------

/**
  \brief  Event on internal library operation MSC initialize start (Operation)
  \param  instance     instance of MSC class
 */
#ifdef                 EvtUSBD_MSC_Initialize
  __STATIC_INLINE void EvrUSBD_MSC_Initialize(  uint8_t instance) {
    EventRecord2(      EvtUSBD_MSC_Initialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_MSC_Initialize(...)
#endif


/**
  \brief  Event on internal library operation MSC initialize failed (Error)
  \param  instance     instance of MSC class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_MSC_InitializeFailed
  __STATIC_INLINE void EvrUSBD_MSC_InitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_MSC_InitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_MSC_InitializeFailed(...)
#endif


/**
  \brief  Event on internal library operation MSC uninitialize start (Operation)
  \param  instance     instance of MSC class
 */
#ifdef                 EvtUSBD_MSC_Uninitialize
  __STATIC_INLINE void EvrUSBD_MSC_Uninitialize(  uint8_t instance) {
    EventRecord2(      EvtUSBD_MSC_Uninitialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_MSC_Uninitialize(...)
#endif


/**
  \brief  Event on internal library operation MSC uninitialize failed (Error)
  \param  instance     instance of MSC class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_MSC_UninitializeFailed
  __STATIC_INLINE void EvrUSBD_MSC_UninitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_MSC_UninitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_MSC_UninitializeFailed(...)
#endif


/**
  \brief  Event on internal library operation MSC reset start (Operation)
  \param  instance     instance of MSC class
 */
#ifdef                 EvtUSBD_MSC_Reset
  __STATIC_INLINE void EvrUSBD_MSC_Reset(  uint8_t instance) {
    EventRecord2(      EvtUSBD_MSC_Reset, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_MSC_Reset(...)
#endif


/**
  \brief  Event on internal library operation MSC reset failed (Error)
  \param  instance     instance of MSC class
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBD_MSC_ResetFailed
  __STATIC_INLINE void EvrUSBD_MSC_ResetFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBD_MSC_ResetFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBD_MSC_ResetFailed(...)
#endif


/**
  \brief  Event on internal library operation Bulk Only Mass Storage Reset finished successfully (Operation)
  \param  instance     instance of MSC class
 */
#ifdef                 EvtUSBD_MSC_BOMSReset
  __STATIC_INLINE void EvrUSBD_MSC_BOMSReset(  uint8_t instance) {
    EventRecord2(      EvtUSBD_MSC_BOMSReset, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_MSC_BOMSReset(...)
#endif


/**
  \brief  Event on internal library operation Bulk Only Mass Storage Reset failed (Error)
  \param  instance     instance of MSC class
 */
#ifdef                 EvtUSBD_MSC_BOMSResetFailed
  __STATIC_INLINE void EvrUSBD_MSC_BOMSResetFailed(  uint8_t instance) {
    EventRecord2(      EvtUSBD_MSC_BOMSResetFailed, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_MSC_BOMSResetFailed(...)
#endif


/**
  \brief  Event on internal library operation Get Max LUN finished successfully (Operation)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
 */
#ifdef                 EvtUSBD_MSC_GetMaxLUN
  __STATIC_INLINE void EvrUSBD_MSC_GetMaxLUN(  uint8_t instance,    uint8_t lun) {
    EventRecord2(      EvtUSBD_MSC_GetMaxLUN, TO_BYTE0(instance) | TO_BYTE1(lun), 0U);
  }
#else
  #define              EvrUSBD_MSC_GetMaxLUN(...)
#endif


/**
  \brief  Event on internal library operation Get Max LUN failed (Error)
  \param  instance     instance of MSC class
 */
#ifdef                 EvtUSBD_MSC_GetMaxLUNFailed
  __STATIC_INLINE void EvrUSBD_MSC_GetMaxLUNFailed(  uint8_t instance) {
    EventRecord2(      EvtUSBD_MSC_GetMaxLUNFailed, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBD_MSC_GetMaxLUNFailed(...)
#endif


/**
  \brief  Event on internal library operation SCSI Test Unit Ready finished (Operation)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  status       request execution status
                        - value 0: No error
                        - value 1: Error - Not Ready - medium not present
                        - value 2: Error - Medium Error - write fault
                        - value 3: Error - Medium Error - unrecoverable read error
                        - value 4: Error - Illegal Request - invalid/unsupported command code
                        - value 5: Error - Illegal Request - invalid field in CDB (Command Descriptor Block)
                        - value 6: Error - Illegal Request - invalid parameter value
                        - value 7: Error - Write Protect - command not allowed
 */
#ifdef                 EvtUSBD_MSC_TestUnitReady
  __STATIC_INLINE void EvrUSBD_MSC_TestUnitReady(  uint8_t instance,    uint8_t lun,  uint32_t status) {
    EventRecord2(      EvtUSBD_MSC_TestUnitReady, TO_BYTE0(instance) | TO_BYTE1(lun),          status);
  }
#else
  #define              EvrUSBD_MSC_TestUnitReady(...)
#endif


/**
  \brief  Event on internal library operation SCSI Request Sense finished (Operation)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  status       request execution status
                        - value 0: No error
                        - value 1: Error - Not Ready - medium not present
                        - value 2: Error - Medium Error - write fault
                        - value 3: Error - Medium Error - unrecoverable read error
                        - value 4: Error - Illegal Request - invalid/unsupported command code
                        - value 5: Error - Illegal Request - invalid field in CDB (Command Descriptor Block)
                        - value 6: Error - Illegal Request - invalid parameter value
                        - value 7: Error - Write Protect - command not allowed
 */
#ifdef                 EvtUSBD_MSC_RequestSense
  __STATIC_INLINE void EvrUSBD_MSC_RequestSense(  uint8_t instance,    uint8_t lun,   uint32_t status) {
    EventRecord2(      EvtUSBD_MSC_RequestSense, TO_BYTE0(instance) | TO_BYTE1(lun),           status);
  }
#else
  #define              EvrUSBD_MSC_RequestSense(...)
#endif


/**
  \brief  Event on internal library operation SCSI Inquiry finished (Operation)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  status       request execution status
                        - value 0: No error
                        - value 1: Error - Not Ready - medium not present
                        - value 2: Error - Medium Error - write fault
                        - value 3: Error - Medium Error - unrecoverable read error
                        - value 4: Error - Illegal Request - invalid/unsupported command code
                        - value 5: Error - Illegal Request - invalid field in CDB (Command Descriptor Block)
                        - value 6: Error - Illegal Request - invalid parameter value
                        - value 7: Error - Write Protect - command not allowed
 */
#ifdef                 EvtUSBD_MSC_Inquiry
  __STATIC_INLINE void EvrUSBD_MSC_Inquiry(  uint8_t instance,    uint8_t lun,  uint32_t status) {
    EventRecord2(      EvtUSBD_MSC_Inquiry, TO_BYTE0(instance) | TO_BYTE1(lun),          status);
  }
#else
  #define              EvrUSBD_MSC_Inquiry(...)
#endif


/**
  \brief  Event on internal library operation SCSI Start/Stop Unit finished (Operation)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  status       request execution status
                        - value 0: No error
                        - value 1: Error - Not Ready - medium not present
                        - value 2: Error - Medium Error - write fault
                        - value 3: Error - Medium Error - unrecoverable read error
                        - value 4: Error - Illegal Request - invalid/unsupported command code
                        - value 5: Error - Illegal Request - invalid field in CDB (Command Descriptor Block)
                        - value 6: Error - Illegal Request - invalid parameter value
                        - value 7: Error - Write Protect - command not allowed
 */
#ifdef                 EvtUSBD_MSC_StartStopUnit
  __STATIC_INLINE void EvrUSBD_MSC_StartStopUnit(  uint8_t instance,    uint8_t lun,  uint32_t status) {
    EventRecord2(      EvtUSBD_MSC_StartStopUnit, TO_BYTE0(instance) | TO_BYTE1(lun),          status);
  }
#else
  #define              EvrUSBD_MSC_StartStopUnit(...)
#endif


/**
  \brief  Event on internal library operation SCSI Media Removal finished (Operation)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  status       request execution status
                        - value 0: No error
                        - value 1: Error - Not Ready - medium not present
                        - value 2: Error - Medium Error - write fault
                        - value 3: Error - Medium Error - unrecoverable read error
                        - value 4: Error - Illegal Request - invalid/unsupported command code
                        - value 5: Error - Illegal Request - invalid field in CDB (Command Descriptor Block)
                        - value 6: Error - Illegal Request - invalid parameter value
                        - value 7: Error - Write Protect - command not allowed
 */
#ifdef                 EvtUSBD_MSC_MediaRemoval
  __STATIC_INLINE void EvrUSBD_MSC_MediaRemoval(  uint8_t instance,    uint8_t lun,  uint32_t status) {
    EventRecord2(      EvtUSBD_MSC_MediaRemoval, TO_BYTE0(instance) | TO_BYTE1(lun),          status);
  }
#else
  #define              EvrUSBD_MSC_MediaRemoval(...)
#endif


/**
  \brief  Event on internal library operation SCSI Mode Sense 6/10 finished (Operation)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  status       request execution status
                        - value 0: No error
                        - value 1: Error - Not Ready - medium not present
                        - value 2: Error - Medium Error - write fault
                        - value 3: Error - Medium Error - unrecoverable read error
                        - value 4: Error - Illegal Request - invalid/unsupported command code
                        - value 5: Error - Illegal Request - invalid field in CDB (Command Descriptor Block)
                        - value 6: Error - Illegal Request - invalid parameter value
                        - value 7: Error - Write Protect - command not allowed
 */
#ifdef                 EvtUSBD_MSC_ModeSense
  __STATIC_INLINE void EvrUSBD_MSC_ModeSense(  uint8_t instance,    uint8_t lun,  uint32_t status) {
    EventRecord2(      EvtUSBD_MSC_ModeSense, TO_BYTE0(instance) | TO_BYTE1(lun),          status);
  }
#else
  #define              EvrUSBD_MSC_ModeSense(...)
#endif


/**
  \brief  Event on internal library operation SCSI Read Format Capacities finished (Operation)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  status       request execution status
                        - value 0: No error
                        - value 1: Error - Not Ready - medium not present
                        - value 2: Error - Medium Error - write fault
                        - value 3: Error - Medium Error - unrecoverable read error
                        - value 4: Error - Illegal Request - invalid/unsupported command code
                        - value 5: Error - Illegal Request - invalid field in CDB (Command Descriptor Block)
                        - value 6: Error - Illegal Request - invalid parameter value
                        - value 7: Error - Write Protect - command not allowed
 */
#ifdef                 EvtUSBD_MSC_ReadFormatCapacities
  __STATIC_INLINE void EvrUSBD_MSC_ReadFormatCapacities(  uint8_t instance,    uint8_t lun,  uint32_t status) {
    EventRecord2(      EvtUSBD_MSC_ReadFormatCapacities, TO_BYTE0(instance) | TO_BYTE1(lun),          status);
  }
#else
  #define              EvrUSBD_MSC_ReadFormatCapacities(...)
#endif


/**
  \brief  Event on internal library operation SCSI Read Capacity finished (Operation)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  status       request execution status
                        - value 0: No error
                        - value 1: Error - Not Ready - medium not present
                        - value 2: Error - Medium Error - write fault
                        - value 3: Error - Medium Error - unrecoverable read error
                        - value 4: Error - Illegal Request - invalid/unsupported command code
                        - value 5: Error - Illegal Request - invalid field in CDB (Command Descriptor Block)
                        - value 6: Error - Illegal Request - invalid parameter value
                        - value 7: Error - Write Protect - command not allowed
 */
#ifdef                 EvtUSBD_MSC_ReadCapacity
  __STATIC_INLINE void EvrUSBD_MSC_ReadCapacity(  uint8_t instance,    uint8_t lun,  uint32_t status) {
    EventRecord2(      EvtUSBD_MSC_ReadCapacity, TO_BYTE0(instance) | TO_BYTE1(lun),          status);
  }
#else
  #define              EvrUSBD_MSC_ReadCapacity(...)
#endif


/**
  \brief  Event on internal library operation SCSI Read 10/12 start (Operation)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  lba          logical address of first block to read
  \param  cnt          number of contiguous blocks to read from media
 */
#ifdef                 EvtUSBD_MSC_Read
  __STATIC_INLINE void EvrUSBD_MSC_Read(  uint8_t instance,    uint8_t lun,  uint32_t lba,  uint32_t cnt) {
    EventRecord4(      EvtUSBD_MSC_Read, TO_BYTE0(instance) | TO_BYTE1(lun),          lba,           cnt, 0U);
  }
#else
  #define              EvrUSBD_MSC_Read(...)
#endif


/**
  \brief  Event on internal library operation SCSI Read 10/12 finished (Detail)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  status       request execution status
                        - value 0: No error
                        - value 1: Error - Not Ready - medium not present
                        - value 2: Error - Medium Error - write fault
                        - value 3: Error - Medium Error - unrecoverable read error
                        - value 4: Error - Illegal Request - invalid/unsupported command code
                        - value 5: Error - Illegal Request - invalid field in CDB (Command Descriptor Block)
                        - value 6: Error - Illegal Request - invalid parameter value
                        - value 7: Error - Write Protect - command not allowed
 */
#ifdef                 EvtUSBD_MSC_ReadDone
  __STATIC_INLINE void EvrUSBD_MSC_ReadDone(  uint8_t instance,    uint8_t lun,  uint32_t status) {
    EventRecord2(      EvtUSBD_MSC_ReadDone, TO_BYTE0(instance) | TO_BYTE1(lun),          status);
  }
#else
  #define              EvrUSBD_MSC_ReadDone(...)
#endif


/**
  \brief  Event on internal library operation SCSI Write 10/12 start (Operation)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  lba          logical address of first block to write
  \param  cnt          number of contiguous blocks to write to media
 */
#ifdef                 EvtUSBD_MSC_Write
  __STATIC_INLINE void EvrUSBD_MSC_Write(  uint8_t instance,    uint8_t lun,  uint32_t lba,  uint32_t cnt) {
    EventRecord4(      EvtUSBD_MSC_Write, TO_BYTE0(instance) | TO_BYTE1(lun),          lba,           cnt, 0U);
  }
#else
  #define              EvrUSBD_MSC_Write(...)
#endif


/**
  \brief  Event on internal library operation SCSI Write 10/12 finished (Detail)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  status       request execution status
                        - value 0: No error
                        - value 1: Error - Not Ready - medium not present
                        - value 2: Error - Medium Error - write fault
                        - value 3: Error - Medium Error - unrecoverable read error
                        - value 4: Error - Illegal Request - invalid/unsupported command code
                        - value 5: Error - Illegal Request - invalid field in CDB (Command Descriptor Block)
                        - value 6: Error - Illegal Request - invalid parameter value
                        - value 7: Error - Write Protect - command not allowed
 */
#ifdef                 EvtUSBD_MSC_WriteDone
  __STATIC_INLINE void EvrUSBD_MSC_WriteDone(  uint8_t instance,    uint8_t lun,  uint32_t status) {
    EventRecord2(      EvtUSBD_MSC_WriteDone, TO_BYTE0(instance) | TO_BYTE1(lun),          status);
  }
#else
  #define              EvrUSBD_MSC_WriteDone(...)
#endif


/**
  \brief  Event on internal library operation SCSI Verify 10 start (Operation)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  lba          logical address of first block to verify
  \param  cnt          number of contiguous blocks to verify
 */
#ifdef                 EvtUSBD_MSC_Verify
  __STATIC_INLINE void EvrUSBD_MSC_Verify(  uint8_t instance,    uint8_t lun,  uint32_t lba,  uint32_t cnt) {
    EventRecord4(      EvtUSBD_MSC_Verify, TO_BYTE0(instance) | TO_BYTE1(lun),          lba,           cnt, 0U);
  }
#else
  #define              EvrUSBD_MSC_Verify(...)
#endif


/**
  \brief  Event on internal library operation SCSI Verify 10 finished (Detail)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  status       request execution status
                        - value 0: No error
                        - value 1: Error - Not Ready - medium not present
                        - value 2: Error - Medium Error - write fault
                        - value 3: Error - Medium Error - unrecoverable read error
                        - value 4: Error - Illegal Request - invalid/unsupported command code
                        - value 5: Error - Illegal Request - invalid field in CDB (Command Descriptor Block)
                        - value 6: Error - Illegal Request - invalid parameter value
                        - value 7: Error - Write Protect - command not allowed
 */
#ifdef                 EvtUSBD_MSC_VerifyDone
  __STATIC_INLINE void EvrUSBD_MSC_VerifyDone(  uint8_t instance,    uint8_t lun,  uint32_t status) {
    EventRecord2(      EvtUSBD_MSC_VerifyDone, TO_BYTE0(instance) | TO_BYTE1(lun),          status);
  }
#else
  #define              EvrUSBD_MSC_VerifyDone(...)
#endif


/**
  \brief  Event on internal library operation SCSI Synchronize Cache 10/16 finished (Operation)
  \param  instance     instance of MSC class
  \param  lun          logical unit number
  \param  status       request execution status
                        - value 0: No error
                        - value 1: Error - Not Ready - medium not present
                        - value 2: Error - Medium Error - write fault
                        - value 3: Error - Medium Error - unrecoverable read error
                        - value 4: Error - Illegal Request - invalid/unsupported command code
                        - value 5: Error - Illegal Request - invalid field in CDB (Command Descriptor Block)
                        - value 6: Error - Illegal Request - invalid parameter value
                        - value 7: Error - Write Protect - command not allowed
 */
#ifdef                 EvtUSBD_MSC_SyncCache
  __STATIC_INLINE void EvrUSBD_MSC_SyncCache(  uint8_t instance,    uint8_t lun,  uint32_t status) {
    EventRecord2(      EvtUSBD_MSC_SyncCache, TO_BYTE0(instance) | TO_BYTE1(lun),          status);
  }
#else
  #define              EvrUSBD_MSC_SyncCache(...)
#endif


/**
  \brief  Event on internal library operation SCSI unsupported command processing (Operation)
  \param  instance     instance of MSC class
  \param  op_code      SCSI operation code
 */
#ifdef                 EvtUSBD_MSC_UnsupportedCommand
  __STATIC_INLINE void EvrUSBD_MSC_UnsupportedCommand(  uint8_t instance,    uint8_t op_code) {
    EventRecord2(      EvtUSBD_MSC_UnsupportedCommand, TO_BYTE0(instance), (uint32_t)op_code);
  }
#else
  #define              EvrUSBD_MSC_UnsupportedCommand(...)
#endif


/**
  \brief  Event on \ref USBD_MSCn_Initialize user callback function start (API)
  \param  n            instance of MSC class
 */
#ifdef                 EvtUSBD_MSC_OnInitialize
  __STATIC_INLINE void EvrUSBD_MSC_OnInitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_MSC_OnInitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_MSC_OnInitialize(...)
#endif


/**
  \brief  Event on \ref USBD_MSCn_Uninitialize user callback function start (API)
  \param  n            instance of MSC class
 */
#ifdef                 EvtUSBD_MSC_OnUninitialize
  __STATIC_INLINE void EvrUSBD_MSC_OnUninitialize(  uint8_t n) {
    EventRecord2(      EvtUSBD_MSC_OnUninitialize, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_MSC_OnUninitialize(...)
#endif


/**
  \brief  Event on \ref USBD_MSCn_GetCacheInfo user callback function finished (API)
  \param  n            instance of MSC class
  \param  size         cache buffer size
 */
#ifdef                 EvtUSBD_MSC_OnGetCacheInfo
  __STATIC_INLINE void EvrUSBD_MSC_OnGetCacheInfo(  uint8_t n,  uint32_t size) {
    EventRecord2(      EvtUSBD_MSC_OnGetCacheInfo, TO_BYTE0(n),          size);
  }
#else
  #define              EvrUSBD_MSC_OnGetCacheInfo(...)
#endif


/**
  \brief  Event on \ref USBD_MSCn_GetCacheInfo user callback function failed (Error)
  \param  n            instance of MSC class
 */
#ifdef                 EvtUSBD_MSC_OnGetCacheInfoFailed
  __STATIC_INLINE void EvrUSBD_MSC_OnGetCacheInfoFailed(  uint8_t n) {
    EventRecord2(      EvtUSBD_MSC_OnGetCacheInfoFailed, TO_BYTE0(n), 0U);
  }
#else
  #define              EvrUSBD_MSC_OnGetCacheInfoFailed(...)
#endif


/**
  \brief  Event on \ref USBD_MSCn_GetMediaCapacity user callback function finished (API)
  \param  n            instance of MSC class
  \param  lun          logical unit number
  \param  block_count  total number of blocks on media
  \param  block_size   media block size
 */
#ifdef                 EvtUSBD_MSC_OnGetMediaCapacity
  __STATIC_INLINE void EvrUSBD_MSC_OnGetMediaCapacity(  uint8_t n,    uint8_t lun,  uint32_t block_count, uint32_t block_size) {
    EventRecord4(      EvtUSBD_MSC_OnGetMediaCapacity, TO_BYTE0(n) | TO_BYTE1(lun),          block_count,          block_size, 0U);
  }
#else
  #define              EvrUSBD_MSC_OnGetMediaCapacity(...)
#endif


/**
  \brief  Event on \ref USBD_MSCn_GetMediaCapacity user callback function failed (Error)
  \param  n            instance of MSC class
  \param  lun          logical unit number
 */
#ifdef                 EvtUSBD_MSC_OnGetMediaCapacityFailed
  __STATIC_INLINE void EvrUSBD_MSC_OnGetMediaCapacityFailed(  uint8_t n,    uint8_t lun) {
    EventRecord2(      EvtUSBD_MSC_OnGetMediaCapacityFailed, TO_BYTE0(n) | TO_BYTE1(lun), 0U);
  }
#else
  #define              EvrUSBD_MSC_OnGetMediaCapacityFailed(...)
#endif


/**
  \brief  Event on \ref USBD_MSCn_Read user callback function finished (API)
  \param  n            instance of MSC class
  \param  lun          logical unit number
  \param  lba          logical address of first block to read
  \param  cnt          number of contiguous blocks to read from media
  \param  buf          data buffer for data read from media
 */
#ifdef                 EvtUSBD_MSC_OnRead
  __STATIC_INLINE void EvrUSBD_MSC_OnRead(  uint8_t n,    uint8_t lun,  uint32_t lba,  uint32_t cnt,  const uint8_t *buf) {
    EventRecord4(      EvtUSBD_MSC_OnRead, TO_BYTE0(n) | TO_BYTE1(lun),          lba,           cnt, (uint32_t)      buf);
  }
#else
  #define              EvrUSBD_MSC_OnRead(...)
#endif


/**
  \brief  Event on \ref USBD_MSCn_Read user callback function failed (Error)
  \param  n            instance of MSC class
  \param  lun          logical unit number
  \param  lba          logical address of first block to read
 */
#ifdef                 EvtUSBD_MSC_OnReadFailed
  __STATIC_INLINE void EvrUSBD_MSC_OnReadFailed(  uint8_t n,    uint8_t lun,  uint32_t lba) {
    EventRecord2(      EvtUSBD_MSC_OnReadFailed, TO_BYTE0(n) | TO_BYTE1(lun),          lba);
  }
#else
  #define              EvrUSBD_MSC_OnReadFailed(...)
#endif


/**
  \brief  Event on \ref USBD_MSCn_Write user callback function finished (API)
  \param  n            instance of MSC class
  \param  lun          logical unit number
  \param  lba          logical address of first block to write
  \param  cnt          number of contiguous blocks to write to media
  \param  buf          data buffer containing data to write to media
 */
#ifdef                 EvtUSBD_MSC_OnWrite
  __STATIC_INLINE void EvrUSBD_MSC_OnWrite(  uint8_t n,    uint8_t lun,  uint32_t lba,  uint32_t cnt,  const uint8_t *buf) {
    EventRecord4(      EvtUSBD_MSC_OnWrite, TO_BYTE0(n) | TO_BYTE1(lun),          lba,           cnt, (uint32_t)      buf);
  }
#else
  #define              EvrUSBD_MSC_OnWrite(...)
#endif


/**
  \brief  Event on \ref USBD_MSCn_Write user callback function failed (Error)
  \param  n            instance of MSC class
  \param  lun          logical unit number
  \param  lba          logical address of first block to write
 */
#ifdef                 EvtUSBD_MSC_OnWriteFailed
  __STATIC_INLINE void EvrUSBD_MSC_OnWriteFailed(  uint8_t n,    uint8_t lun,  uint32_t lba) {
    EventRecord2(      EvtUSBD_MSC_OnWriteFailed, TO_BYTE0(n) | TO_BYTE1(lun),          lba);
  }
#else
  #define              EvrUSBD_MSC_OnWriteFailed(...)
#endif


/**
  \brief  Event on \ref USBD_MSCn_CheckMedia user callback function finished (API)
  \param  n            instance of MSC class
  \param  lun          logical unit number
  \param  wp           media write protect status
  \param  mp           media present status
 */
#ifdef                 EvtUSBD_MSC_OnCheckMedia
  __STATIC_INLINE void EvrUSBD_MSC_OnCheckMedia(  uint8_t n,    uint8_t lun,        bool wp,             bool mp) {
    EventRecord2(      EvtUSBD_MSC_OnCheckMedia, TO_BYTE0(n) | TO_BYTE1(lun), ((uint32_t)wp << 1) | (uint32_t)mp);
  }
#else
  #define              EvrUSBD_MSC_OnCheckMedia(...)
#endif

#endif // __USBD_EVR_H__
