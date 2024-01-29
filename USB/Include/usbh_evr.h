/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_evr.h
 * Purpose: USB Host (USBH) - Event Recorder definitions
 *----------------------------------------------------------------------------*/

#ifndef __USBH_EVR_H__
#define __USBH_EVR_H__

#include "rl_usb.h"
#include "Driver_USBH.h"

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))
#include "EventRecorder.h"
#endif


// Define component numbers
#define EvtCompNo_USBH_Core                                         0xB0U   /// < USB Host Core
#define EvtCompNo_USBH_Driver                                       0xB1U   /// < USB Host Driver
#define EvtCompNo_USBH_CC                                           0xB2U   /// < USB Host Custom Class
#define EvtCompNo_USBH_CDC                                          0xB3U   /// < USB Host CDC
#define EvtCompNo_USBH_HID                                          0xB4U   /// < USB Host HID
#define EvtCompNo_USBH_MSC                                          0xB5U   /// < USB Host MSC


// Define message numbers
#define EvtMsgNo_USBH_Core_Initialize                               0x00U
#define EvtMsgNo_USBH_Core_InitializeFailed                         0x01U
#define EvtMsgNo_USBH_Core_Uninitialize                             0x02U
#define EvtMsgNo_USBH_Core_UninitializeFailed                       0x03U
#define EvtMsgNo_USBH_Core_PortSuspend                              0x04U
#define EvtMsgNo_USBH_Core_PortSuspendFailed                        0x05U
#define EvtMsgNo_USBH_Core_PortResume                               0x06U
#define EvtMsgNo_USBH_Core_PortResumeFailed                         0x07U
#define EvtMsgNo_USBH_Core_DeviceGetController                      0x08U
#define EvtMsgNo_USBH_Core_DeviceGetPort                            0x09U
#define EvtMsgNo_USBH_Core_DeviceGetStatus                          0x0AU
#define EvtMsgNo_USBH_Core_DeviceGetSpeed                           0x0BU
#define EvtMsgNo_USBH_Core_DeviceGetAddress                         0x0CU
#define EvtMsgNo_USBH_Core_DeviceGetVID                             0x0DU
#define EvtMsgNo_USBH_Core_DeviceGetPID                             0x0EU
#define EvtMsgNo_USBH_Core_OnNotify                                 0x0FU
#define EvtMsgNo_USBH_Core_PipeCreate                               0x10U
#define EvtMsgNo_USBH_Core_PipeCreateFailed                         0x11U
#define EvtMsgNo_USBH_Core_PipeUpdate                               0x12U
#define EvtMsgNo_USBH_Core_PipeUpdateFailed                         0x13U
#define EvtMsgNo_USBH_Core_PipeDelete                               0x14U
#define EvtMsgNo_USBH_Core_PipeDeleteFailed                         0x15U
#define EvtMsgNo_USBH_Core_PipeReset                                0x16U
#define EvtMsgNo_USBH_Core_PipeResetFailed                          0x17U
#define EvtMsgNo_USBH_Core_PipeReceive                              0x18U
#define EvtMsgNo_USBH_Core_PipeReceiveFailed                        0x19U
#define EvtMsgNo_USBH_Core_PipeReceiveGetResult                     0x1AU
#define EvtMsgNo_USBH_Core_PipeSend                                 0x1BU
#define EvtMsgNo_USBH_Core_PipeSendFailed                           0x1CU
#define EvtMsgNo_USBH_Core_PipeSendGetResult                        0x1DU
#define EvtMsgNo_USBH_Core_PipeAbort                                0x1EU
#define EvtMsgNo_USBH_Core_PipeAbortFailed                          0x1FU
#define EvtMsgNo_USBH_Core_ControlTransfer                          0x2AU
#define EvtMsgNo_USBH_Core_ControlTransferFailed                    0x2BU
#define EvtMsgNo_USBH_Core_RequestGetStatus                         0x2CU
#define EvtMsgNo_USBH_Core_RequestGetStatusFailed                   0x2DU
#define EvtMsgNo_USBH_Core_RequestClearFeature                      0x2EU
#define EvtMsgNo_USBH_Core_RequestClearFeatureFailed                0x2FU
#define EvtMsgNo_USBH_Core_RequestSetFeature                        0x30U
#define EvtMsgNo_USBH_Core_RequestSetFeatureFailed                  0x31U
#define EvtMsgNo_USBH_Core_RequestSetAddress                        0x32U
#define EvtMsgNo_USBH_Core_RequestSetAddressFailed                  0x33U
#define EvtMsgNo_USBH_Core_RequestGetDescriptor                     0x34U
#define EvtMsgNo_USBH_Core_RequestGetDescriptorFailed               0x35U
#define EvtMsgNo_USBH_Core_RequestSetDescriptor                     0x36U
#define EvtMsgNo_USBH_Core_RequestSetDescriptorFailed               0x37U
#define EvtMsgNo_USBH_Core_RequestGetConfiguration                  0x38U
#define EvtMsgNo_USBH_Core_RequestGetConfigurationFailed            0x39U
#define EvtMsgNo_USBH_Core_RequestSetConfiguration                  0x3AU
#define EvtMsgNo_USBH_Core_RequestSetConfigurationFailed            0x3BU
#define EvtMsgNo_USBH_Core_RequestGetInterface                      0x3CU
#define EvtMsgNo_USBH_Core_RequestGetInterfaceFailed                0x3DU
#define EvtMsgNo_USBH_Core_RequestSetInterface                      0x3EU
#define EvtMsgNo_USBH_Core_RequestSetInterfaceFailed                0x3FU
#define EvtMsgNo_USBH_Core_RequestSynchFrame                        0x40U
#define EvtMsgNo_USBH_Core_RequestSynchFrameFailed                  0x41U
#define EvtMsgNo_USBH_Core_MemInitFailed                            0x42U
#define EvtMsgNo_USBH_Core_MemInit                                  0x43U
#define EvtMsgNo_USBH_Core_MemUninitFailed                          0x44U
#define EvtMsgNo_USBH_Core_MemUninit                                0x45U
#define EvtMsgNo_USBH_Core_MemAllocFailed                           0x46U
#define EvtMsgNo_USBH_Core_MemAlloc                                 0x47U
#define EvtMsgNo_USBH_Core_MemFreeFailed                            0x48U
#define EvtMsgNo_USBH_Core_MemFree                                  0x49U
#define EvtMsgNo_USBH_Core_Engine                                   0x4AU
#define EvtMsgNo_USBH_Core_EngineFailed                             0x4BU
#define EvtMsgNo_USBH_Core_EngineDone                               0x4CU
#define EvtMsgNo_USBH_Core_DeviceGetStringDescriptorFailed          0x4DU
#define EvtMsgNo_USBH_Core_DeviceGetStringDescriptor                0x4EU
#define EvtMsgNo_USBH_Core_PipeRegisterCallback                     0x4FU
#define EvtMsgNo_USBH_Core_PipeRegisterCallbackFailed               0x50U

#define EvtMsgNo_USBH_Driver_Initialize                             0x00U
#define EvtMsgNo_USBH_Driver_InitializeFailed                       0x01U
#define EvtMsgNo_USBH_Driver_Uninitialize                           0x02U
#define EvtMsgNo_USBH_Driver_UninitializeFailed                     0x03U
#define EvtMsgNo_USBH_Driver_PowerControl                           0x04U
#define EvtMsgNo_USBH_Driver_PowerControlFailed                     0x05U
#define EvtMsgNo_USBH_Driver_PortVbusOnOff                          0x06U
#define EvtMsgNo_USBH_Driver_PortVbusOnOffFailed                    0x07U
#define EvtMsgNo_USBH_Driver_PortReset                              0x08U
#define EvtMsgNo_USBH_Driver_PortResetFailed                        0x09U
#define EvtMsgNo_USBH_Driver_PortSuspend                            0x0AU
#define EvtMsgNo_USBH_Driver_PortSuspendFailed                      0x0BU
#define EvtMsgNo_USBH_Driver_PortResume                             0x0CU
#define EvtMsgNo_USBH_Driver_PortResumeFailed                       0x0DU
#define EvtMsgNo_USBH_Driver_PortGetState                           0x0EU
#define EvtMsgNo_USBH_Driver_PipeCreate                             0x0FU
#define EvtMsgNo_USBH_Driver_PipeCreateFailed                       0x10U
#define EvtMsgNo_USBH_Driver_PipeModify                             0x11U
#define EvtMsgNo_USBH_Driver_PipeModifyFailed                       0x12U
#define EvtMsgNo_USBH_Driver_PipeDelete                             0x13U
#define EvtMsgNo_USBH_Driver_PipeDeleteFailed                       0x14U
#define EvtMsgNo_USBH_Driver_PipeReset                              0x15U
#define EvtMsgNo_USBH_Driver_PipeResetFailed                        0x16U
#define EvtMsgNo_USBH_Driver_PipeTransfer                           0x17U
#define EvtMsgNo_USBH_Driver_PipeTransferFailed                     0x18U
#define EvtMsgNo_USBH_Driver_PipeTransferGetResult                  0x19U
#define EvtMsgNo_USBH_Driver_PipeTransferAbort                      0x1AU
#define EvtMsgNo_USBH_Driver_PipeTransferAbortFailed                0x1BU
#define EvtMsgNo_USBH_Driver_GetFrameNumber                         0x1CU
#define EvtMsgNo_USBH_Driver_OnSignalPortEvent                      0x1DU
#define EvtMsgNo_USBH_Driver_OnSignalPipeEvent                      0x1EU

#define EvtMsgNo_USBH_CC_GetDevice                                  0x00U
#define EvtMsgNo_USBH_CC_GetStatus                                  0x01U
#define EvtMsgNo_USBH_CC_OnConfigure                                0x02U
#define EvtMsgNo_USBH_CC_OnUnconfigure                              0x03U
#define EvtMsgNo_USBH_CC_OnInitialize                               0x04U
#define EvtMsgNo_USBH_CC_OnUninitialize                             0x05U

#define EvtMsgNo_USBH_CDC_ACM_Configure                             0x00U
#define EvtMsgNo_USBH_CDC_ACM_ConfigureFailed                       0x01U
#define EvtMsgNo_USBH_CDC_ACM_Unconfigure                           0x02U
#define EvtMsgNo_USBH_CDC_ACM_UnconfigureFailed                     0x03U
#define EvtMsgNo_USBH_CDC_ACM_Initialize                            0x04U
#define EvtMsgNo_USBH_CDC_ACM_InitializeFailed                      0x05U
#define EvtMsgNo_USBH_CDC_ACM_OnInitialize                          0x06U
#define EvtMsgNo_USBH_CDC_ACM_Uninitialize                          0x07U
#define EvtMsgNo_USBH_CDC_ACM_UninitializeFailed                    0x08U
#define EvtMsgNo_USBH_CDC_ACM_OnUninitialize                        0x09U
#define EvtMsgNo_USBH_CDC_ACM_GetDevice                             0x0AU
#define EvtMsgNo_USBH_CDC_ACM_GetStatus                             0x0BU
#define EvtMsgNo_USBH_CDC_ACM_Send                                  0x0CU
#define EvtMsgNo_USBH_CDC_ACM_SendFailed                            0x0DU
#define EvtMsgNo_USBH_CDC_ACM_SendDone                              0x0EU
#define EvtMsgNo_USBH_CDC_ACM_GetTxCount                            0x0FU
#define EvtMsgNo_USBH_CDC_ACM_Receive                               0x10U
#define EvtMsgNo_USBH_CDC_ACM_ReceiveFailed                         0x11U
#define EvtMsgNo_USBH_CDC_ACM_ReceiveDone                           0x12U
#define EvtMsgNo_USBH_CDC_ACM_GetRxCount                            0x13U
#define EvtMsgNo_USBH_CDC_ACM_SetLineCoding                         0x14U
#define EvtMsgNo_USBH_CDC_ACM_SetLineCodingFailed                   0x15U
#define EvtMsgNo_USBH_CDC_ACM_GetLineCodingFailed                   0x16U
#define EvtMsgNo_USBH_CDC_ACM_GetLineCoding                         0x17U
#define EvtMsgNo_USBH_CDC_ACM_SetControlLineState                   0x18U
#define EvtMsgNo_USBH_CDC_ACM_SetControlLineStateFailed             0x19U
#define EvtMsgNo_USBH_CDC_ACM_OnNotify                              0x1AU
#define EvtMsgNo_USBH_CDC_ACM_SendBreak                             0x1BU
#define EvtMsgNo_USBH_CDC_ACM_SendBreakFailed                       0x1CU
#define EvtMsgNo_USBH_CDC_ACM_AbortSend                             0x1DU
#define EvtMsgNo_USBH_CDC_ACM_AbortSendFailed                       0x1EU
#define EvtMsgNo_USBH_CDC_ACM_AbortSendDone                         0x1FU
#define EvtMsgNo_USBH_CDC_ACM_AbortReceive                          0x20U
#define EvtMsgNo_USBH_CDC_ACM_AbortReceiveFailed                    0x21U
#define EvtMsgNo_USBH_CDC_ACM_AbortReceiveDone                      0x22U

#define EvtMsgNo_USBH_HID_Configure                                 0x00U
#define EvtMsgNo_USBH_HID_ConfigureFailed                           0x01U
#define EvtMsgNo_USBH_HID_Unconfigure                               0x02U
#define EvtMsgNo_USBH_HID_UnconfigureFailed                         0x03U
#define EvtMsgNo_USBH_HID_Initialize                                0x04U
#define EvtMsgNo_USBH_HID_InitializeFailed                          0x05U
#define EvtMsgNo_USBH_HID_OnInitialize                              0x06U
#define EvtMsgNo_USBH_HID_Uninitialize                              0x07U
#define EvtMsgNo_USBH_HID_UninitializeFailed                        0x08U
#define EvtMsgNo_USBH_HID_OnUninitialize                            0x09U
#define EvtMsgNo_USBH_HID_GetDevice                                 0x0AU
#define EvtMsgNo_USBH_HID_GetStatus                                 0x0BU
#define EvtMsgNo_USBH_HID_Read                                      0x0CU
#define EvtMsgNo_USBH_HID_ReadFailed                                0x0DU
#define EvtMsgNo_USBH_HID_ReadDone                                  0x0EU
#define EvtMsgNo_USBH_HID_Write                                     0x0FU
#define EvtMsgNo_USBH_HID_WriteFailed                               0x10U
#define EvtMsgNo_USBH_HID_WriteDone                                 0x11U
#define EvtMsgNo_USBH_HID_GetKeyboardKey                            0x12U
#define EvtMsgNo_USBH_HID_GetKeyboardKeyDone                        0x13U
#define EvtMsgNo_USBH_HID_GetMouseState                             0x14U
#define EvtMsgNo_USBH_HID_GetMouseStateFailed                       0x15U
#define EvtMsgNo_USBH_HID_GetMouseStateDone                         0x16U
#define EvtMsgNo_USBH_HID_OnParseReportDescriptor                   0x17U
#define EvtMsgNo_USBH_HID_OnDataReceived                            0x18U
#define EvtMsgNo_USBH_HID_GetReport                                 0x19U
#define EvtMsgNo_USBH_HID_GetReportFailed                           0x1AU
#define EvtMsgNo_USBH_HID_SetReport                                 0x1BU
#define EvtMsgNo_USBH_HID_SetReportFailed                           0x1CU
#define EvtMsgNo_USBH_HID_GetIdle                                   0x1DU
#define EvtMsgNo_USBH_HID_GetIdleFailed                             0x1EU
#define EvtMsgNo_USBH_HID_SetIdle                                   0x1FU
#define EvtMsgNo_USBH_HID_SetIdleFailed                             0x20U
#define EvtMsgNo_USBH_HID_GetProtocol                               0x21U
#define EvtMsgNo_USBH_HID_GetProtocolFailed                         0x22U
#define EvtMsgNo_USBH_HID_SetProtocol                               0x23U
#define EvtMsgNo_USBH_HID_SetProtocolFailed                         0x24U
#define EvtMsgNo_USBH_HID_ReportOut                                 0x25U
#define EvtMsgNo_USBH_HID_ReportOutFailed                           0x26U

#define EvtMsgNo_USBH_MSC_Configure                                 0x00U
#define EvtMsgNo_USBH_MSC_ConfigureFailed                           0x01U
#define EvtMsgNo_USBH_MSC_Unconfigure                               0x02U
#define EvtMsgNo_USBH_MSC_UnconfigureFailed                         0x03U
#define EvtMsgNo_USBH_MSC_Initialize                                0x04U
#define EvtMsgNo_USBH_MSC_InitializeFailed                          0x05U
#define EvtMsgNo_USBH_MSC_OnInitialize                              0x06U
#define EvtMsgNo_USBH_MSC_Uninitialize                              0x07U
#define EvtMsgNo_USBH_MSC_UninitializeFailed                        0x08U
#define EvtMsgNo_USBH_MSC_OnUninitialize                            0x09U
#define EvtMsgNo_USBH_MSC_GetDevice                                 0x0AU
#define EvtMsgNo_USBH_MSC_GetStatus                                 0x0BU
#define EvtMsgNo_USBH_MSC_Read                                      0x0CU
#define EvtMsgNo_USBH_MSC_ReadFailed                                0x0DU
#define EvtMsgNo_USBH_MSC_ReadDone                                  0x0EU
#define EvtMsgNo_USBH_MSC_Write                                     0x0FU
#define EvtMsgNo_USBH_MSC_WriteFailed                               0x10U
#define EvtMsgNo_USBH_MSC_WriteDone                                 0x11U
#define EvtMsgNo_USBH_MSC_ReadCapacity                              0x12U
#define EvtMsgNo_USBH_MSC_ReadCapacityFailed                        0x13U
#define EvtMsgNo_USBH_MSC_ReadCapacityDone                          0x14U
#define EvtMsgNo_USBH_MSC_BomReset                                  0x15U
#define EvtMsgNo_USBH_MSC_BomResetFailed                            0x16U
#define EvtMsgNo_USBH_MSC_GetMaxLun                                 0x17U
#define EvtMsgNo_USBH_MSC_GetMaxLunFailed                           0x18U
#define EvtMsgNo_USBH_MSC_ScsiTestUnitReady                         0x19U
#define EvtMsgNo_USBH_MSC_ScsiTestUnitReadyFailed                   0x1AU
#define EvtMsgNo_USBH_MSC_ScsiRequestSense                          0x1BU
#define EvtMsgNo_USBH_MSC_ScsiRequestSenseFailed                    0x1CU
#define EvtMsgNo_USBH_MSC_ScsiInquiry                               0x1DU
#define EvtMsgNo_USBH_MSC_ScsiInquiryFailed                         0x1EU
#define EvtMsgNo_USBH_MSC_ScsiReadFormatCapacities                  0x1FU
#define EvtMsgNo_USBH_MSC_ScsiReadFormatCapacitiesFailed            0x20U
#define EvtMsgNo_USBH_MSC_ScsiReadCapacity                          0x21U
#define EvtMsgNo_USBH_MSC_ScsiReadCapacityFailed                    0x22U
#define EvtMsgNo_USBH_MSC_ScsiRead10                                0x23U
#define EvtMsgNo_USBH_MSC_ScsiRead10Failed                          0x24U
#define EvtMsgNo_USBH_MSC_ScsiWrite10                               0x25U
#define EvtMsgNo_USBH_MSC_ScsiWrite10Failed                         0x26U
#define EvtMsgNo_USBH_MSC_Recover                                   0x27U
#define EvtMsgNo_USBH_MSC_RecoverFailed                             0x28U

#if (defined(USBH_DEBUG) && (USBH_DEBUG == 1))                      // Define USB Host Library debug events

// Pack parameter in byte
#define TO_BYTE0(x)                                                 (((uint32_t)(x) & 0xFFU))
#define TO_BYTE1(x)                                                 (((uint32_t)(x) & 0xFFU) <<  8)
#define TO_BYTE2(x)                                                 (((uint32_t)(x) & 0xFFU) << 16)
#define TO_BYTE3(x)                                                 (((uint32_t)(x) & 0xFFU) << 24)

// Event id list for "USBH_Core" component
#define EvtUSBH_Core_Initialize                                     EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_Initialize)
#define EvtUSBH_Core_InitializeFailed                               EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_InitializeFailed)
#define EvtUSBH_Core_Uninitialize                                   EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_Uninitialize)
#define EvtUSBH_Core_UninitializeFailed                             EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_UninitializeFailed)
#define EvtUSBH_Core_PortSuspend                                    EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PortSuspend)
#define EvtUSBH_Core_PortSuspendFailed                              EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PortSuspendFailed)
#define EvtUSBH_Core_PortResume                                     EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PortResume)
#define EvtUSBH_Core_PortResumeFailed                               EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PortResumeFailed)
#define EvtUSBH_Core_DeviceGetController                            EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_DeviceGetController)
#define EvtUSBH_Core_DeviceGetPort                                  EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_DeviceGetPort)
#define EvtUSBH_Core_DeviceGetStatus                                EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_DeviceGetStatus)
#define EvtUSBH_Core_DeviceGetSpeed                                 EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_DeviceGetSpeed)
#define EvtUSBH_Core_DeviceGetAddress                               EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_DeviceGetAddress)
#define EvtUSBH_Core_DeviceGetVID                                   EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_DeviceGetVID)
#define EvtUSBH_Core_DeviceGetPID                                   EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_DeviceGetPID)
#define EvtUSBH_Core_OnNotify                                       EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_OnNotify)
#define EvtUSBH_Core_PipeCreate                                     EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeCreate)
#define EvtUSBH_Core_PipeCreateFailed                               EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeCreateFailed)
#define EvtUSBH_Core_PipeUpdate                                     EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeUpdate)
#define EvtUSBH_Core_PipeUpdateFailed                               EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeUpdateFailed)
#define EvtUSBH_Core_PipeDelete                                     EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeDelete)
#define EvtUSBH_Core_PipeDeleteFailed                               EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeDeleteFailed)
#define EvtUSBH_Core_PipeReset                                      EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeReset)
#define EvtUSBH_Core_PipeResetFailed                                EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeResetFailed)
#define EvtUSBH_Core_PipeReceive                                    EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeReceive)
#define EvtUSBH_Core_PipeReceiveFailed                              EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeReceiveFailed)
#define EvtUSBH_Core_PipeReceiveGetResult                           EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeReceiveGetResult)
#define EvtUSBH_Core_PipeSend                                       EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeSend)
#define EvtUSBH_Core_PipeSendFailed                                 EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeSendFailed)
#define EvtUSBH_Core_PipeSendGetResult                              EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeSendGetResult)
#define EvtUSBH_Core_PipeAbort                                      EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeAbort)
#define EvtUSBH_Core_PipeAbortFailed                                EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_PipeAbortFailed)
#define EvtUSBH_Core_ControlTransfer                                EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_ControlTransfer)
#define EvtUSBH_Core_ControlTransferFailed                          EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_ControlTransferFailed)
#define EvtUSBH_Core_RequestGetStatus                               EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestGetStatus)
#define EvtUSBH_Core_RequestGetStatusFailed                         EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestGetStatusFailed)
#define EvtUSBH_Core_RequestClearFeature                            EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestClearFeature)
#define EvtUSBH_Core_RequestClearFeatureFailed                      EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestClearFeatureFailed)
#define EvtUSBH_Core_RequestSetFeature                              EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestSetFeature)
#define EvtUSBH_Core_RequestSetFeatureFailed                        EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestSetFeatureFailed)
#define EvtUSBH_Core_RequestSetAddress                              EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestSetAddress)
#define EvtUSBH_Core_RequestSetAddressFailed                        EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestSetAddressFailed)
#define EvtUSBH_Core_RequestGetDescriptor                           EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestGetDescriptor)
#define EvtUSBH_Core_RequestGetDescriptorFailed                     EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestGetDescriptorFailed)
#define EvtUSBH_Core_RequestSetDescriptor                           EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestSetDescriptor)
#define EvtUSBH_Core_RequestSetDescriptorFailed                     EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestSetDescriptorFailed)
#define EvtUSBH_Core_RequestGetConfiguration                        EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestGetConfiguration)
#define EvtUSBH_Core_RequestGetConfigurationFailed                  EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestGetConfigurationFailed)
#define EvtUSBH_Core_RequestSetConfiguration                        EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestSetConfiguration)
#define EvtUSBH_Core_RequestSetConfigurationFailed                  EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestSetConfigurationFailed)
#define EvtUSBH_Core_RequestGetInterface                            EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestGetInterface)
#define EvtUSBH_Core_RequestGetInterfaceFailed                      EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestGetInterfaceFailed)
#define EvtUSBH_Core_RequestSetInterface                            EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestSetInterface)
#define EvtUSBH_Core_RequestSetInterfaceFailed                      EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestSetInterfaceFailed)
#define EvtUSBH_Core_RequestSynchFrame                              EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestSynchFrame)
#define EvtUSBH_Core_RequestSynchFrameFailed                        EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_RequestSynchFrameFailed)
#define EvtUSBH_Core_MemInitFailed                                  EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_MemInitFailed)
#define EvtUSBH_Core_MemInit                                        EventID(EventLevelDetail, EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_MemInit)
#define EvtUSBH_Core_MemUninitFailed                                EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_MemUninitFailed)
#define EvtUSBH_Core_MemUninit                                      EventID(EventLevelDetail, EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_MemUninit)
#define EvtUSBH_Core_MemAllocFailed                                 EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_MemAllocFailed)
#define EvtUSBH_Core_MemAlloc                                       EventID(EventLevelDetail, EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_MemAlloc)
#define EvtUSBH_Core_MemFreeFailed                                  EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_MemFreeFailed)
#define EvtUSBH_Core_MemFree                                        EventID(EventLevelDetail, EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_MemFree)
#define EvtUSBH_Core_Engine                                         EventID(EventLevelDetail, EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_Engine)
#define EvtUSBH_Core_EngineFailed                                   EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_EngineFailed)
#define EvtUSBH_Core_EngineDone                                     EventID(EventLevelDetail, EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_EngineDone)
#define EvtUSBH_Core_DeviceGetStringDescriptor                      EventID(EventLevelAPI,    EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_DeviceGetStringDescriptor)
#define EvtUSBH_Core_DeviceGetStringDescriptorFailed                EventID(EventLevelError,  EvtCompNo_USBH_Core,    EvtMsgNo_USBH_Core_DeviceGetStringDescriptorFailed)

// Event id list for "USBH_Driver" component
#define EvtUSBH_Driver_Initialize                                   EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_Initialize)
#define EvtUSBH_Driver_InitializeFailed                             EventID(EventLevelError,  EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_InitializeFailed)
#define EvtUSBH_Driver_Uninitialize                                 EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_Uninitialize)
#define EvtUSBH_Driver_UninitializeFailed                           EventID(EventLevelError,  EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_UninitializeFailed)
#define EvtUSBH_Driver_PowerControl                                 EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PowerControl)
#define EvtUSBH_Driver_PowerControlFailed                           EventID(EventLevelError,  EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PowerControlFailed)
#define EvtUSBH_Driver_PortVbusOnOff                                EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PortVbusOnOff)
#define EvtUSBH_Driver_PortVbusOnOffFailed                          EventID(EventLevelError,  EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PortVbusOnOffFailed)
#define EvtUSBH_Driver_PortReset                                    EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PortReset)
#define EvtUSBH_Driver_PortResetFailed                              EventID(EventLevelError,  EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PortResetFailed)
#define EvtUSBH_Driver_PortSuspend                                  EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PortSuspend)
#define EvtUSBH_Driver_PortSuspendFailed                            EventID(EventLevelError,  EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PortSuspendFailed)
#define EvtUSBH_Driver_PortResume                                   EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PortResume)
#define EvtUSBH_Driver_PortResumeFailed                             EventID(EventLevelError,  EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PortResumeFailed)
#define EvtUSBH_Driver_PortGetState                                 EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PortGetState)
#define EvtUSBH_Driver_PipeCreate                                   EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PipeCreate)
#define EvtUSBH_Driver_PipeCreateFailed                             EventID(EventLevelError,  EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PipeCreateFailed)
#define EvtUSBH_Driver_PipeModify                                   EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PipeModify)
#define EvtUSBH_Driver_PipeModifyFailed                             EventID(EventLevelError,  EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PipeModifyFailed)
#define EvtUSBH_Driver_PipeDelete                                   EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PipeDelete)
#define EvtUSBH_Driver_PipeDeleteFailed                             EventID(EventLevelError,  EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PipeDeleteFailed)
#define EvtUSBH_Driver_PipeReset                                    EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PipeReset)
#define EvtUSBH_Driver_PipeResetFailed                              EventID(EventLevelError,  EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PipeResetFailed)
#define EvtUSBH_Driver_PipeTransfer                                 EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PipeTransfer)
#define EvtUSBH_Driver_PipeTransferFailed                           EventID(EventLevelError,  EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PipeTransferFailed)
#define EvtUSBH_Driver_PipeTransferGetResult                        EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PipeTransferGetResult)
#define EvtUSBH_Driver_PipeTransferAbort                            EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PipeTransferAbort)
#define EvtUSBH_Driver_PipeTransferAbortFailed                      EventID(EventLevelError,  EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_PipeTransferAbortFailed)
#define EvtUSBH_Driver_GetFrameNumber                               EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_GetFrameNumber)
#define EvtUSBH_Driver_OnSignalPortEvent                            EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_OnSignalPortEvent)
#define EvtUSBH_Driver_OnSignalPipeEvent                            EventID(EventLevelAPI,    EvtCompNo_USBH_Driver,  EvtMsgNo_USBH_Driver_OnSignalPipeEvent)

// Event id list for "USBH_CC" component
#define EvtUSBH_CC_GetDevice                                        EventID(EventLevelAPI,    EvtCompNo_USBH_CC,      EvtMsgNo_USBH_CC_GetDevice)
#define EvtUSBH_CC_GetStatus                                        EventID(EventLevelAPI,    EvtCompNo_USBH_CC,      EvtMsgNo_USBH_CC_GetStatus)
#define EvtUSBH_CC_OnConfigure                                      EventID(EventLevelAPI,    EvtCompNo_USBH_CC,      EvtMsgNo_USBH_CC_OnConfigure)
#define EvtUSBH_CC_OnUnconfigure                                    EventID(EventLevelAPI,    EvtCompNo_USBH_CC,      EvtMsgNo_USBH_CC_OnUnconfigure)
#define EvtUSBH_CC_OnInitialize                                     EventID(EventLevelAPI,    EvtCompNo_USBH_CC,      EvtMsgNo_USBH_CC_OnInitialize)
#define EvtUSBH_CC_OnUninitialize                                   EventID(EventLevelAPI,    EvtCompNo_USBH_CC,      EvtMsgNo_USBH_CC_OnUninitialize)

// Event id list for "USBH_CDC" component
#define EvtUSBH_CDC_ACM_Configure                                   EventID(EventLevelOp,     EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_Configure)
#define EvtUSBH_CDC_ACM_ConfigureFailed                             EventID(EventLevelError,  EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_ConfigureFailed)
#define EvtUSBH_CDC_ACM_Unconfigure                                 EventID(EventLevelOp,     EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_Unconfigure)
#define EvtUSBH_CDC_ACM_UnconfigureFailed                           EventID(EventLevelError,  EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_UnconfigureFailed)
#define EvtUSBH_CDC_ACM_Initialize                                  EventID(EventLevelOp,     EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_Initialize)
#define EvtUSBH_CDC_ACM_InitializeFailed                            EventID(EventLevelError,  EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_InitializeFailed)
#define EvtUSBH_CDC_ACM_OnInitialize                                EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_OnInitialize)
#define EvtUSBH_CDC_ACM_Uninitialize                                EventID(EventLevelOp,     EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_Uninitialize)
#define EvtUSBH_CDC_ACM_UninitializeFailed                          EventID(EventLevelError,  EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_UninitializeFailed)
#define EvtUSBH_CDC_ACM_OnUninitialize                              EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_OnUninitialize)
#define EvtUSBH_CDC_ACM_GetDevice                                   EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_GetDevice)
#define EvtUSBH_CDC_ACM_GetStatus                                   EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_GetStatus)
#define EvtUSBH_CDC_ACM_Send                                        EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_Send)
#define EvtUSBH_CDC_ACM_SendFailed                                  EventID(EventLevelError,  EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_SendFailed)
#define EvtUSBH_CDC_ACM_SendDone                                    EventID(EventLevelDetail, EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_SendDone)
#define EvtUSBH_CDC_ACM_GetTxCount                                  EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_GetTxCount)
#define EvtUSBH_CDC_ACM_Receive                                     EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_Receive)
#define EvtUSBH_CDC_ACM_ReceiveFailed                               EventID(EventLevelError,  EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_ReceiveFailed)
#define EvtUSBH_CDC_ACM_ReceiveDone                                 EventID(EventLevelDetail, EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_ReceiveDone)
#define EvtUSBH_CDC_ACM_GetRxCount                                  EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_GetRxCount)
#define EvtUSBH_CDC_ACM_SetLineCoding                               EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_SetLineCoding)
#define EvtUSBH_CDC_ACM_SetLineCodingFailed                         EventID(EventLevelError,  EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_SetLineCodingFailed)
#define EvtUSBH_CDC_ACM_GetLineCodingFailed                         EventID(EventLevelError,  EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_GetLineCodingFailed)
#define EvtUSBH_CDC_ACM_GetLineCoding                               EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_GetLineCoding)
#define EvtUSBH_CDC_ACM_SetControlLineState                         EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_SetControlLineState)
#define EvtUSBH_CDC_ACM_SetControlLineStateFailed                   EventID(EventLevelError,  EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_SetControlLineStateFailed)
#define EvtUSBH_CDC_ACM_OnNotify                                    EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_OnNotify)
#define EvtUSBH_CDC_ACM_SendBreak                                   EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_SendBreak)
#define EvtUSBH_CDC_ACM_SendBreakFailed                             EventID(EventLevelError,  EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_SendBreakFailed)
#define EvtUSBH_CDC_ACM_AbortSend                                   EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_AbortSend)
#define EvtUSBH_CDC_ACM_AbortSendFailed                             EventID(EventLevelError,  EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_AbortSendFailed)
#define EvtUSBH_CDC_ACM_AbortSendDone                               EventID(EventLevelDetail, EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_AbortSendDone)
#define EvtUSBH_CDC_ACM_AbortReceive                                EventID(EventLevelAPI,    EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_AbortReceive)
#define EvtUSBH_CDC_ACM_AbortReceiveFailed                          EventID(EventLevelError,  EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_AbortReceiveFailed)
#define EvtUSBH_CDC_ACM_AbortReceiveDone                            EventID(EventLevelDetail, EvtCompNo_USBH_CDC,      EvtMsgNo_USBH_CDC_ACM_AbortReceiveDone)

// Event id list for "USBH_HID" component
#define EvtUSBH_HID_Configure                                       EventID(EventLevelOp,     EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_Configure)
#define EvtUSBH_HID_ConfigureFailed                                 EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_ConfigureFailed)
#define EvtUSBH_HID_Unconfigure                                     EventID(EventLevelOp,     EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_Unconfigure)
#define EvtUSBH_HID_UnconfigureFailed                               EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_UnconfigureFailed)
#define EvtUSBH_HID_Initialize                                      EventID(EventLevelOp,     EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_Initialize)
#define EvtUSBH_HID_InitializeFailed                                EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_InitializeFailed)
#define EvtUSBH_HID_OnInitialize                                    EventID(EventLevelAPI,    EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_OnInitialize)
#define EvtUSBH_HID_Uninitialize                                    EventID(EventLevelOp,     EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_Uninitialize)
#define EvtUSBH_HID_UninitializeFailed                              EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_UninitializeFailed)
#define EvtUSBH_HID_OnUninitialize                                  EventID(EventLevelAPI,    EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_OnUninitialize)
#define EvtUSBH_HID_GetDevice                                       EventID(EventLevelAPI,    EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_GetDevice)
#define EvtUSBH_HID_GetStatus                                       EventID(EventLevelAPI,    EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_GetStatus)
#define EvtUSBH_HID_Read                                            EventID(EventLevelAPI,    EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_Read)
#define EvtUSBH_HID_ReadFailed                                      EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_ReadFailed)
#define EvtUSBH_HID_ReadDone                                        EventID(EventLevelDetail, EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_ReadDone)
#define EvtUSBH_HID_Write                                           EventID(EventLevelAPI,    EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_Write)
#define EvtUSBH_HID_WriteFailed                                     EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_WriteFailed)
#define EvtUSBH_HID_WriteDone                                       EventID(EventLevelDetail, EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_WriteDone)
#define EvtUSBH_HID_GetKeyboardKey                                  EventID(EventLevelAPI,    EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_GetKeyboardKey)
#define EvtUSBH_HID_GetKeyboardKeyDone                              EventID(EventLevelDetail, EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_GetKeyboardKeyDone)
#define EvtUSBH_HID_GetMouseState                                   EventID(EventLevelAPI,    EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_GetMouseState)
#define EvtUSBH_HID_GetMouseStateFailed                             EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_GetMouseStateFailed)
#define EvtUSBH_HID_GetMouseStateDone                               EventID(EventLevelDetail, EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_GetMouseStateDone)
#define EvtUSBH_HID_OnParseReportDescriptor                         EventID(EventLevelAPI,    EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_OnParseReportDescriptor)
#define EvtUSBH_HID_OnDataReceived                                  EventID(EventLevelAPI,    EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_OnDataReceived)
#define EvtUSBH_HID_GetReport                                       EventID(EventLevelOp,     EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_GetReport)
#define EvtUSBH_HID_GetReportFailed                                 EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_GetReportFailed)
#define EvtUSBH_HID_SetReport                                       EventID(EventLevelOp,     EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_SetReport)
#define EvtUSBH_HID_SetReportFailed                                 EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_SetReportFailed)
#define EvtUSBH_HID_GetIdle                                         EventID(EventLevelOp,     EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_GetIdle)
#define EvtUSBH_HID_GetIdleFailed                                   EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_GetIdleFailed)
#define EvtUSBH_HID_SetIdle                                         EventID(EventLevelOp,     EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_SetIdle)
#define EvtUSBH_HID_SetIdleFailed                                   EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_SetIdleFailed)
#define EvtUSBH_HID_GetProtocol                                     EventID(EventLevelOp,     EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_GetProtocol)
#define EvtUSBH_HID_GetProtocolFailed                               EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_GetProtocolFailed)
#define EvtUSBH_HID_SetProtocol                                     EventID(EventLevelOp,     EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_SetProtocol)
#define EvtUSBH_HID_SetProtocolFailed                               EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_SetProtocolFailed)
#define EvtUSBH_HID_ReportOut                                       EventID(EventLevelOp,     EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_ReportOut)
#define EvtUSBH_HID_ReportOutFailed                                 EventID(EventLevelError,  EvtCompNo_USBH_HID,      EvtMsgNo_USBH_HID_ReportOutFailed)

// Event id list for "USBH_MSC" component
#define EvtUSBH_MSC_Configure                                       EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_Configure)
#define EvtUSBH_MSC_ConfigureFailed                                 EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ConfigureFailed)
#define EvtUSBH_MSC_Unconfigure                                     EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_Unconfigure)
#define EvtUSBH_MSC_UnconfigureFailed                               EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_UnconfigureFailed)
#define EvtUSBH_MSC_Initialize                                      EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_Initialize)
#define EvtUSBH_MSC_InitializeFailed                                EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_InitializeFailed)
#define EvtUSBH_MSC_OnInitialize                                    EventID(EventLevelAPI,    EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_OnInitialize)
#define EvtUSBH_MSC_Uninitialize                                    EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_Uninitialize)
#define EvtUSBH_MSC_UninitializeFailed                              EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_UninitializeFailed)
#define EvtUSBH_MSC_OnUninitialize                                  EventID(EventLevelAPI,    EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_OnUninitialize)
#define EvtUSBH_MSC_GetDevice                                       EventID(EventLevelAPI,    EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_GetDevice)
#define EvtUSBH_MSC_GetStatus                                       EventID(EventLevelAPI,    EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_GetStatus)
#define EvtUSBH_MSC_Read                                            EventID(EventLevelAPI,    EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_Read)
#define EvtUSBH_MSC_ReadFailed                                      EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ReadFailed)
#define EvtUSBH_MSC_ReadDone                                        EventID(EventLevelDetail, EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ReadDone)
#define EvtUSBH_MSC_Write                                           EventID(EventLevelAPI,    EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_Write)
#define EvtUSBH_MSC_WriteFailed                                     EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_WriteFailed)
#define EvtUSBH_MSC_WriteDone                                       EventID(EventLevelDetail, EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_WriteDone)
#define EvtUSBH_MSC_ReadCapacity                                    EventID(EventLevelAPI,    EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ReadCapacity)
#define EvtUSBH_MSC_ReadCapacityFailed                              EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ReadCapacityFailed)
#define EvtUSBH_MSC_ReadCapacityDone                                EventID(EventLevelDetail, EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ReadCapacityDone)
#define EvtUSBH_MSC_BomReset                                        EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_BomReset)
#define EvtUSBH_MSC_BomResetFailed                                  EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_BomResetFailed)
#define EvtUSBH_MSC_GetMaxLun                                       EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_GetMaxLun)
#define EvtUSBH_MSC_GetMaxLunFailed                                 EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_GetMaxLunFailed)
#define EvtUSBH_MSC_ScsiTestUnitReady                               EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiTestUnitReady)
#define EvtUSBH_MSC_ScsiTestUnitReadyFailed                         EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiTestUnitReadyFailed)
#define EvtUSBH_MSC_ScsiRequestSense                                EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiRequestSense)
#define EvtUSBH_MSC_ScsiRequestSenseFailed                          EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiRequestSenseFailed)
#define EvtUSBH_MSC_ScsiInquiry                                     EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiInquiry)
#define EvtUSBH_MSC_ScsiInquiryFailed                               EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiInquiryFailed)
#define EvtUSBH_MSC_ScsiReadFormatCapacities                        EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiReadFormatCapacities)
#define EvtUSBH_MSC_ScsiReadFormatCapacitiesFailed                  EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiReadFormatCapacitiesFailed)
#define EvtUSBH_MSC_ScsiReadCapacity                                EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiReadCapacity)
#define EvtUSBH_MSC_ScsiReadCapacityFailed                          EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiReadCapacityFailed)
#define EvtUSBH_MSC_ScsiRead10                                      EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiRead10)
#define EvtUSBH_MSC_ScsiRead10Failed                                EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiRead10Failed)
#define EvtUSBH_MSC_ScsiWrite10                                     EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiWrite10)
#define EvtUSBH_MSC_ScsiWrite10Failed                               EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_ScsiWrite10Failed)
#define EvtUSBH_MSC_Recover                                         EventID(EventLevelOp,     EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_Recover)
#define EvtUSBH_MSC_RecoverFailed                                   EventID(EventLevelError,  EvtCompNo_USBH_MSC,     EvtMsgNo_USBH_MSC_RecoverFailed)

#endif // USBH_DEBUG


// USB Host Core component event record functions ------------------------------

/**
  \brief  Event on \ref USBH_Initialize start (API)
  \param  ctrl         controller index
 */
#ifdef                 EvtUSBH_Core_Initialize
  __STATIC_INLINE void EvrUSBH_Core_Initialize(  uint8_t ctrl) {
    EventRecord2(      EvtUSBH_Core_Initialize, TO_BYTE0(ctrl), 0U);
  }
#else
  #define              EvrUSBH_Core_Initialize(...)
#endif


/**
  \brief  Event on \ref USBH_Initialize failed (Error)
  \param  ctrl         controller index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_InitializeFailed
  __STATIC_INLINE void EvrUSBH_Core_InitializeFailed(  uint8_t ctrl,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_InitializeFailed, TO_BYTE0(ctrl), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_InitializeFailed(...)
#endif


/**
  \brief  Event on \ref USBH_Uninitialize start (API)
  \param  ctrl         controller index
 */
#ifdef                 EvtUSBH_Core_Uninitialize
  __STATIC_INLINE void EvrUSBH_Core_Uninitialize(  uint8_t ctrl) {
    EventRecord2(      EvtUSBH_Core_Uninitialize, TO_BYTE0(ctrl), 0U);
  }
#else
  #define              EvrUSBH_Core_Uninitialize(...)
#endif


/**
  \brief  Event on \ref USBH_Uninitialize failed (Error)
  \param  ctrl         controller index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_UninitializeFailed
  __STATIC_INLINE void EvrUSBH_Core_UninitializeFailed(  uint8_t ctrl,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_UninitializeFailed, TO_BYTE0(ctrl), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_UninitializeFailed(...)
#endif


/**
  \brief  Event on \ref USBH_Port_Suspend start (API)
  \param  ctrl         controller index
  \param  port         root HUB port
 */
#ifdef                 EvtUSBH_Core_PortSuspend
  __STATIC_INLINE void EvrUSBH_Core_PortSuspend(  uint8_t ctrl,    uint8_t port) {
    EventRecord2(      EvtUSBH_Core_PortSuspend, TO_BYTE0(ctrl) | TO_BYTE1(port), 0U);
  }
#else
  #define              EvrUSBH_Core_PortSuspend(...)
#endif


/**
  \brief  Event on \ref USBH_Port_Suspend failed (Error)
  \param  ctrl         controller index
  \param  port         root HUB port
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_PortSuspendFailed
  __STATIC_INLINE void EvrUSBH_Core_PortSuspendFailed(  uint8_t ctrl,    uint8_t port,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_PortSuspendFailed, TO_BYTE0(ctrl) | TO_BYTE1(port), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_PortSuspendFailed(...)
#endif


/**
  \brief  Event on \ref USBH_Port_Resume start (API)
  \param  ctrl         controller index
  \param  port         root HUB port
 */
#ifdef                 EvtUSBH_Core_PortResume
  __STATIC_INLINE void EvrUSBH_Core_PortResume(  uint8_t ctrl,    uint8_t port) {
    EventRecord2(      EvtUSBH_Core_PortResume, TO_BYTE0(ctrl) | TO_BYTE1(port), 0U);
  }
#else
  #define              EvrUSBH_Core_PortResume(...)
#endif


/**
  \brief  Event on \ref USBH_Port_Resume failed (Error)
  \param  ctrl         controller index
  \param  port         root HUB port
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_PortResumeFailed
  __STATIC_INLINE void EvrUSBH_Core_PortResumeFailed(  uint8_t ctrl,    uint8_t port,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_PortResumeFailed, TO_BYTE0(ctrl) | TO_BYTE1(port), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_PortResumeFailed(...)
#endif


/**
  \brief  Event on \ref USBH_Device_GetController finished (API)
  \param  device       device index
  \param  ctrl         controller index
                        - value <= 3:   index of USB Host controller
                        - value == 255: non-existing USB Host controller
 */
#ifdef                 EvtUSBH_Core_DeviceGetController
  __STATIC_INLINE void EvrUSBH_Core_DeviceGetController(  uint8_t device,   uint8_t ctrl) {
    EventRecord2(      EvtUSBH_Core_DeviceGetController, TO_BYTE0(device), TO_BYTE0(ctrl));
  }
#else
  #define              EvrUSBH_Core_DeviceGetController(...)
#endif


/**
  \brief  Event on \ref USBH_Device_GetPort finished (API)
  \param  device       device index
  \param  port         root HUB port index
                        - value != 255: index of USB Host root HUB port
                        - value == 255: non-existing USB Host root HUB port
 */
#ifdef                 EvtUSBH_Core_DeviceGetPort
  __STATIC_INLINE void EvrUSBH_Core_DeviceGetPort(  uint8_t device,   uint8_t port) {
    EventRecord2(      EvtUSBH_Core_DeviceGetPort, TO_BYTE0(device), TO_BYTE0(port));
  }
#else
  #define              EvrUSBH_Core_DeviceGetPort(...)
#endif


/**
  \brief  Event on \ref USBH_Device_GetStatus finished (API)
  \param  device       device index
  \param  status       device status \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_DeviceGetStatus
  __STATIC_INLINE void EvrUSBH_Core_DeviceGetStatus(  uint8_t device,  usbStatus status) {
    EventRecord2(      EvtUSBH_Core_DeviceGetStatus, TO_BYTE0(device), (uint32_t)status);
  }
#else
  #define              EvrUSBH_Core_DeviceGetStatus(...)
#endif


/**
  \brief  Event on \ref USBH_Device_GetSpeed finished (API)
  \param  device       device index
  \param  speed        communication speed
 */
#ifdef                 EvtUSBH_Core_DeviceGetSpeed
  __STATIC_INLINE void EvrUSBH_Core_DeviceGetSpeed(  uint8_t device,   uint8_t speed) {
    EventRecord2(      EvtUSBH_Core_DeviceGetSpeed, TO_BYTE0(device), TO_BYTE0(speed));
  }
#else
  #define              EvrUSBH_Core_DeviceGetSpeed(...)
#endif


/**
  \brief  Event on \ref USBH_Device_GetAddress finished (API)
  \param  device       device index
  \param  address      enumerated address
                        - value <= 127: enumerated address
                        - value == 255: invalid address
 */
#ifdef                 EvtUSBH_Core_DeviceGetAddress
  __STATIC_INLINE void EvrUSBH_Core_DeviceGetAddress(  uint8_t device,   uint8_t address) {
    EventRecord2(      EvtUSBH_Core_DeviceGetAddress, TO_BYTE0(device), TO_BYTE0(address));
  }
#else
  #define              EvrUSBH_Core_DeviceGetAddress(...)
#endif


/**
  \brief  Event on \ref USBH_Device_GetVID finished (API)
  \param  device       device index
  \param  vid          vendor ID
 */
#ifdef                 EvtUSBH_Core_DeviceGetVID
  __STATIC_INLINE void EvrUSBH_Core_DeviceGetVID(  uint8_t device,    uint16_t vid) {
    EventRecord2(      EvtUSBH_Core_DeviceGetVID, TO_BYTE0(device), (uint32_t)(vid));
  }
#else
  #define              EvrUSBH_Core_DeviceGetVID(...)
#endif


/**
  \brief  Event on \ref USBH_Device_GetPID finished (API)
  \param  device       device index
  \param  pid          product ID
 */
#ifdef                 EvtUSBH_Core_DeviceGetPID
  __STATIC_INLINE void EvrUSBH_Core_DeviceGetPID(  uint8_t device,    uint16_t pid) {
    EventRecord2(      EvtUSBH_Core_DeviceGetPID, TO_BYTE0(device), (uint32_t)(pid));
  }
#else
  #define              EvrUSBH_Core_DeviceGetPID(...)
#endif


/**
  \brief  Event on \ref USBH_Device_GetStringDescriptor start (API)
  \param  device       device index
  \param  index        string descriptor index
  \param  lang_id      language ID
  \param  desc_length  descriptor length
 */
#ifdef                 EvtUSBH_Core_DeviceGetStringDescriptor
  __STATIC_INLINE void EvrUSBH_Core_DeviceGetStringDescriptor(  uint8_t device,    uint8_t index,  uint16_t lang_id,    uint16_t desc_length) {
    EventRecord2(      EvtUSBH_Core_DeviceGetStringDescriptor, TO_BYTE0(device) | TO_BYTE3(index), TO_BYTE2(lang_id) | (uint32_t)desc_length);
  }
#else
  #define              EvrUSBH_Core_DeviceGetStringDescriptor(...)
#endif


/**
  \brief  Event on \ref USBH_Device_GetStringDescriptor failed (Error)
  \param  device       device index
  \param  index        string descriptor index
  \param  lang_id      language ID
  \param  desc_length  descriptor length
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_DeviceGetStringDescriptorFailed
  __STATIC_INLINE void EvrUSBH_Core_DeviceGetStringDescriptorFailed(  uint8_t device,    uint8_t index,  uint16_t lang_id,    uint16_t desc_length, usbStatus error) {
    EventRecord2(      EvtUSBH_Core_DeviceGetStringDescriptorFailed, TO_BYTE0(device) | TO_BYTE3(index), TO_BYTE2(lang_id) | (uint32_t)desc_length | TO_BYTE3(error));
  }
#else
  #define              EvrUSBH_Core_DeviceGetStringDescriptorFailed(...)
#endif


/**
  \brief  Event on \ref USBH_Notify user callback function start (API)
  \param  ctrl         index of USB Host controller.
  \param  port         index of Root HUB port.
  \param  device       index of USB Device
                        - value <= 127: index of of USB Device for device notifications
                        - value 255: no device information for port notifications
  \param  notify       notification:
                        - USBH_NOTIFY_CONNECT               = Port connection happened
                        - USBH_NOTIFY_DISCONNECT            = Port disconnection happened
                        - USBH_NOTIFY_OVERCURRENT           = Port overcurrent happened
                        - USBH_NOTIFY_REMOTE_WAKEUP         = Port remote wakeup signaling happened
                        - USBH_NOTIFY_READY                 = Device was successfully enumerated, initialized and is ready for communication
                        - USBH_NOTIFY_UNKNOWN_DEVICE        = Device was successfully enumerated but there is no driver for it
                        - USBH_NOTIFY_INSUFFICIENT_POWER    = Device requires more power consumption than available
                        - USBH_NOTIFY_CONFIGURATION_FAILED  = Device was not successfully configured (not enough resources)
                        - USBH_NOTIFY_INITIALIZATION_FAILED = Device was not successfully initialized
 */
#ifdef                 EvtUSBH_Core_OnNotify
  __STATIC_INLINE void EvrUSBH_Core_OnNotify(  uint8_t ctrl,    uint8_t port,    uint8_t device, USBH_NOTIFY notify) {
    EventRecord2(      EvtUSBH_Core_OnNotify, TO_BYTE0(ctrl) | TO_BYTE1(port) | TO_BYTE2(device),  (uint32_t)notify);
  }
#else
  #define              EvrUSBH_Core_OnNotify(...)
#endif


/**
  \brief  Event on \ref USBH_PipeCreate start (API)
  \param  device               index of USB Device.
  \param  ep_addr              endpoint address
           - ep_addr.0..3:       address
           - ep_addr.7:          direction
  \param  ep_type              endpoint type.
  \param  ep_max_packet_size   endpoint maximum packet size.
  \param  ep_interval          endpoint polling interval.
  \param  pipe_hndl            pipe handle or pipe creation failed
           - value > 0:         pipe_handle
           - value 0:           pipe creation failed
 */
#ifdef                 EvtUSBH_Core_PipeCreate
  __STATIC_INLINE void EvrUSBH_Core_PipeCreate(  uint8_t device,    uint8_t ep_addr,    uint8_t ep_type,   uint16_t ep_max_packet_size,   uint8_t ep_interval,  USBH_PIPE_HANDLE pipe_hndl) {
    EventRecord4(      EvtUSBH_Core_PipeCreate, TO_BYTE0(device) | TO_BYTE1(ep_addr) | TO_BYTE2(ep_type), (uint32_t)ep_max_packet_size | TO_BYTE2(ep_interval),        (uint32_t)pipe_hndl, 0U);
  }
#else
  #define              EvrUSBH_Core_PipeCreate(...)
#endif


/**
  \brief  Event on \ref USBH_PipeCreate failed (Error)
  \param  device               index of USB Device.
  \param  ep_addr              endpoint address
           - ep_addr.0..3:       address
           - ep_addr.7:          direction
  \param  ep_type              endpoint type.
  \param  ep_max_packet_size   endpoint maximum packet size.
  \param  ep_interval          endpoint polling interval.
 */
#ifdef                 EvtUSBH_Core_PipeCreateFailed
  __STATIC_INLINE void EvrUSBH_Core_PipeCreateFailed(  uint8_t device,    uint8_t ep_addr,    uint8_t ep_type,   uint16_t ep_max_packet_size,   uint8_t ep_interval) {
    EventRecord2(      EvtUSBH_Core_PipeCreateFailed, TO_BYTE0(device) | TO_BYTE1(ep_addr) | TO_BYTE2(ep_type), (uint32_t)ep_max_packet_size | TO_BYTE2(ep_interval));
  }
#else
  #define              EvrUSBH_Core_PipeCreateFailed(...)
#endif


/**
  \brief  Event on \ref USBH_PipeRegisterCallback start (API)
  \param  pipe_hndl    pipe handle.
 */
#ifdef                 EvtUSBH_Core_PipeRegisterCallback
  __STATIC_INLINE void EvrUSBH_Core_PipeRegisterCallback(USBH_PIPE_HANDLE pipe_hndl) {
    EventRecord2(      EvtUSBH_Core_PipeRegisterCallback,       (uint32_t)pipe_hndl, 0U);
  }
#else
  #define              EvrUSBH_Core_PipeRegisterCallback(...)
#endif


/**
  \brief  Event on \ref USBH_PipeRegisterCallback failed (Error)
  \param  pipe_hndl    pipe handle.
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_PipeRegisterCallbackFailed
  __STATIC_INLINE void EvrUSBH_Core_PipeRegisterCallbackFailed(USBH_PIPE_HANDLE pipe_hndl, usbStatus error) {
    EventRecord2(      EvtUSBH_Core_PipeRegisterCallbackFailed,       (uint32_t)pipe_hndl, (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_PipeRegisterCallbackFailed(...)
#endif


/**
  \brief  Event on \ref USBH_PipeUpdate start (API)
  \param  pipe_hndl    pipe handle.
 */
#ifdef                 EvtUSBH_Core_PipeUpdate
  __STATIC_INLINE void EvrUSBH_Core_PipeUpdate(USBH_PIPE_HANDLE pipe_hndl) {
    EventRecord2(      EvtUSBH_Core_PipeUpdate,       (uint32_t)pipe_hndl, 0U);
  }
#else
  #define              EvrUSBH_Core_PipeUpdate(...)
#endif


/**
  \brief  Event on \ref USBH_PipeUpdate failed (Error)
  \param  pipe_hndl    pipe handle.
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_PipeUpdateFailed
  __STATIC_INLINE void EvrUSBH_Core_PipeUpdateFailed(USBH_PIPE_HANDLE pipe_hndl, usbStatus error) {
    EventRecord2(      EvtUSBH_Core_PipeUpdateFailed,       (uint32_t)pipe_hndl, (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_PipeUpdateFailed(...)
#endif


/**
  \brief  Event on \ref USBH_PipeDelete start (API)
  \param  pipe_hndl    pipe handle.
 */
#ifdef                 EvtUSBH_Core_PipeDelete
  __STATIC_INLINE void EvrUSBH_Core_PipeDelete(USBH_PIPE_HANDLE pipe_hndl) {
    EventRecord2(      EvtUSBH_Core_PipeDelete,       (uint32_t)pipe_hndl, 0U);
  }
#else
  #define              EvrUSBH_Core_PipeDelete(...)
#endif


/**
  \brief  Event on \ref USBH_PipeDelete failed (Error)
  \param  pipe_hndl    pipe handle.
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_PipeDeleteFailed
  __STATIC_INLINE void EvrUSBH_Core_PipeDeleteFailed(USBH_PIPE_HANDLE pipe_hndl, usbStatus error) {
    EventRecord2(      EvtUSBH_Core_PipeDeleteFailed,       (uint32_t)pipe_hndl, (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_PipeDeleteFailed(...)
#endif


/**
  \brief  Event on \ref USBH_PipeReset start (API)
  \param  pipe_hndl    pipe handle.
 */
#ifdef                 EvtUSBH_Core_PipeReset
  __STATIC_INLINE void EvrUSBH_Core_PipeReset(USBH_PIPE_HANDLE pipe_hndl) {
    EventRecord2(      EvtUSBH_Core_PipeReset,       (uint32_t)pipe_hndl, 0U);
  }
#else
  #define              EvrUSBH_Core_PipeReset(...)
#endif


/**
  \brief  Event on \ref USBH_PipeReset failed (Error)
  \param  pipe_hndl    pipe handle.
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_PipeResetFailed
  __STATIC_INLINE void EvrUSBH_Core_PipeResetFailed(USBH_PIPE_HANDLE pipe_hndl, usbStatus error) {
    EventRecord2(      EvtUSBH_Core_PipeResetFailed,       (uint32_t)pipe_hndl, (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_PipeResetFailed(...)
#endif


/**
  \brief  Event on \ref USBH_PipeReceive start (API)
  \param  pipe_hndl    pipe handle.
  \param  len          maximum number of bytes to receive
 */
#ifdef                 EvtUSBH_Core_PipeReceive
  __STATIC_INLINE void EvrUSBH_Core_PipeReceive(USBH_PIPE_HANDLE pipe_hndl,  uint32_t len) {
    EventRecord2(      EvtUSBH_Core_PipeReceive,       (uint32_t)pipe_hndl,           len);
  }
#else
  #define              EvrUSBH_Core_PipeReceive(...)
#endif


/**
  \brief  Event on \ref USBH_PipeReceive failed (Error)
  \param  pipe_hndl    pipe handle.
  \param  len          maximum number of bytes to receive
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_PipeReceiveFailed
  __STATIC_INLINE void EvrUSBH_Core_PipeReceiveFailed(USBH_PIPE_HANDLE pipe_hndl,  uint32_t len, usbStatus error) {
    EventRecord4(      EvtUSBH_Core_PipeReceiveFailed,       (uint32_t)pipe_hndl,           len, (uint32_t)error, 0U);
  }
#else
  #define              EvrUSBH_Core_PipeReceiveFailed(...)
#endif


/**
  \brief  Event on \ref USBH_PipeReceiveGetResult finished (API)
  \param  pipe_hndl    pipe handle.
  \param  num          number of successfully received data bytes
 */
#ifdef                 EvtUSBH_Core_PipeReceiveGetResult
  __STATIC_INLINE void EvrUSBH_Core_PipeReceiveGetResult(USBH_PIPE_HANDLE pipe_hndl, uint32_t num) {
    EventRecord2(      EvtUSBH_Core_PipeReceiveGetResult,       (uint32_t)pipe_hndl,          num);
  }
#else
  #define              EvrUSBH_Core_PipeReceiveGetResult(...)
#endif


/**
  \brief  Event on \ref USBH_PipeSend start (API)
  \param  pipe_hndl    pipe handle.
  \param  len          maximum number of bytes to send
 */
#ifdef                 EvtUSBH_Core_PipeSend
  __STATIC_INLINE void EvrUSBH_Core_PipeSend(USBH_PIPE_HANDLE pipe_hndl,  uint32_t len) {
    EventRecord2(      EvtUSBH_Core_PipeSend,       (uint32_t)pipe_hndl,           len);
  }
#else
  #define              EvrUSBH_Core_PipeSend(...)
#endif


/**
  \brief  Event on \ref USBH_PipeSend failed (Error)
  \param  pipe_hndl    pipe handle.
  \param  len          number of bytes to send
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_PipeSendFailed
  __STATIC_INLINE void EvrUSBH_Core_PipeSendFailed(USBH_PIPE_HANDLE pipe_hndl,  uint32_t len, usbStatus error) {
    EventRecord4(      EvtUSBH_Core_PipeSendFailed,       (uint32_t)pipe_hndl,           len, (uint32_t)error, 0U);
  }
#else
  #define              EvrUSBH_Core_PipeSendFailed(...)
#endif


/**
  \brief  Event on \ref USBH_PipeSendGetResult finished (API)
  \param  pipe_hndl    pipe handle.
  \param  num          number of successfully sent data bytes
 */
#ifdef                 EvtUSBH_Core_PipeSendGetResult
  __STATIC_INLINE void EvrUSBH_Core_PipeSendGetResult(USBH_PIPE_HANDLE pipe_hndl, uint32_t num) {
    EventRecord2(      EvtUSBH_Core_PipeSendGetResult,       (uint32_t)pipe_hndl,          num);
  }
#else
  #define              EvrUSBH_Core_PipeSendGetResult(...)
#endif


/**
  \brief  Event on \ref USBH_PipeAbort start (API)
  \param  pipe_hndl    pipe handle.
 */
#ifdef                 EvtUSBH_Core_PipeAbort
  __STATIC_INLINE void EvrUSBH_Core_PipeAbort(USBH_PIPE_HANDLE pipe_hndl) {
    EventRecord2(      EvtUSBH_Core_PipeAbort,       (uint32_t)pipe_hndl, 0U);
  }
#else
  #define              EvrUSBH_Core_PipeAbort(...)
#endif


/**
  \brief  Event on \ref USBH_PipeAbort failed (Error)
  \param  pipe_hndl    pipe handle.
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_PipeAbortFailed
  __STATIC_INLINE void EvrUSBH_Core_PipeAbortFailed(USBH_PIPE_HANDLE pipe_hndl, usbStatus error) {
    EventRecord2(      EvtUSBH_Core_PipeAbortFailed,       (uint32_t)pipe_hndl, (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_PipeAbortFailed(...)
#endif


/**
  \brief  Event on \ref USBH_ControlTransfer start (API)
  \param  device       device index
  \param  setup_packet setup packet content
  \param  len          number of data bytes in data stage
 */
#ifdef                 EvtUSBH_Core_ControlTransfer
  __STATIC_INLINE void EvrUSBH_Core_ControlTransfer(  uint8_t device,     const void     * setup_packet,                                        uint32_t len) {
    EventRecord4(      EvtUSBH_Core_ControlTransfer, TO_BYTE0(device), *((const uint32_t *)setup_packet), *((const uint32_t *)setup_packet+1U),          len);
  }
#else
  #define              EvrUSBH_Core_ControlTransfer(...)
#endif


/**
  \brief  Event on \ref USBH_ControlTransfer failed (Error)
  \param  device       device index
  \param  setup_packet setup packet content
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_ControlTransferFailed
  __STATIC_INLINE void EvrUSBH_Core_ControlTransferFailed(  uint8_t device,     const void     * setup_packet,                                        usbStatus error) {
    EventRecord4(      EvtUSBH_Core_ControlTransferFailed, TO_BYTE0(device), *((const uint32_t *)setup_packet), *((const uint32_t *)setup_packet+1U), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_ControlTransferFailed(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_GetStatus start (API)
  \param  device       device index
  \param  recipient    recipient
  \param  index        interface or endpoint index
  \param  status       received status
 */
#ifdef                 EvtUSBH_Core_RequestGetStatus
  __STATIC_INLINE void EvrUSBH_Core_RequestGetStatus(  uint8_t device,    uint8_t recipient,    uint8_t index,   uint16_t status) {
    EventRecord2(      EvtUSBH_Core_RequestGetStatus, TO_BYTE0(device) | TO_BYTE1(recipient) | TO_BYTE2(index), (uint32_t)status);
  }
#else
  #define              EvrUSBH_Core_RequestGetStatus(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_GetStatus failed (Error)
  \param  device       device index
  \param  recipient    recipient
  \param  index        interface or endpoint index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_RequestGetStatusFailed
  __STATIC_INLINE void EvrUSBH_Core_RequestGetStatusFailed(  uint8_t device,    uint8_t recipient,    uint8_t index,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_RequestGetStatusFailed, TO_BYTE0(device) | TO_BYTE1(recipient) | TO_BYTE2(index), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_RequestGetStatusFailed(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_ClearFeature start (API)
  \param  device       device index
  \param  recipient    recipient
  \param  index        interface or endpoint index
  \param  feature      feature selector
 */
#ifdef                 EvtUSBH_Core_RequestClearFeature
  __STATIC_INLINE void EvrUSBH_Core_RequestClearFeature(  uint8_t device,    uint8_t recipient,    uint8_t index,    uint8_t feature) {
    EventRecord2(      EvtUSBH_Core_RequestClearFeature, TO_BYTE0(device) | TO_BYTE1(recipient) | TO_BYTE2(index) | TO_BYTE3(feature), 0U);
  }
#else
  #define              EvrUSBH_Core_RequestClearFeature(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_ClearFeature failed (Error)
  \param  device       device index
  \param  recipient    recipient
  \param  index        interface or endpoint index
  \param  feature      feature selector
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_RequestClearFeatureFailed
  __STATIC_INLINE void EvrUSBH_Core_RequestClearFeatureFailed(  uint8_t device,    uint8_t recipient,    uint8_t index,    uint8_t feature,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_RequestClearFeatureFailed, TO_BYTE0(device) | TO_BYTE1(recipient) | TO_BYTE2(index) | TO_BYTE3(feature), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_RequestClearFeatureFailed(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_SetFeature start (API)
  \param  device       device index
  \param  recipient    recipient
  \param  index        interface or endpoint index
  \param  feature      feature selector
 */
#ifdef                 EvtUSBH_Core_RequestSetFeature
  __STATIC_INLINE void EvrUSBH_Core_RequestSetFeature(  uint8_t device,    uint8_t recipient,    uint8_t index,    uint8_t feature) {
    EventRecord2(      EvtUSBH_Core_RequestSetFeature, TO_BYTE0(device) | TO_BYTE1(recipient) | TO_BYTE2(index) | TO_BYTE3(feature), 0U);
  }
#else
  #define              EvrUSBH_Core_RequestSetFeature(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_SetFeature failed (Error)
  \param  device       device index
  \param  recipient    recipient
  \param  index        interface or endpoint index
  \param  feature      feature selector
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_RequestSetFeatureFailed
  __STATIC_INLINE void EvrUSBH_Core_RequestSetFeatureFailed(  uint8_t device,    uint8_t recipient,    uint8_t index,    uint8_t feature,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_RequestSetFeatureFailed, TO_BYTE0(device) | TO_BYTE1(recipient) | TO_BYTE2(index) | TO_BYTE3(feature), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_RequestSetFeatureFailed(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_SetAddress start (API)
  \param  device       device index
  \param  address      device address
 */
#ifdef                 EvtUSBH_Core_RequestSetAddress
  __STATIC_INLINE void EvrUSBH_Core_RequestSetAddress(  uint8_t device,    uint8_t address) {
    EventRecord2(      EvtUSBH_Core_RequestSetAddress, TO_BYTE0(device) | TO_BYTE1(address), 0U);
  }
#else
  #define              EvrUSBH_Core_RequestSetAddress(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_SetAddress failed (Error)
  \param  device       device index
  \param  address      device address
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_RequestSetAddressFailed
  __STATIC_INLINE void EvrUSBH_Core_RequestSetAddressFailed(  uint8_t device,    uint8_t address,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_RequestSetAddressFailed, TO_BYTE0(device) | TO_BYTE1(address), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_RequestSetAddressFailed(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_GetDescriptor start (API)
  \param  device       device index
  \param  recipient    recipient
  \param  desc_type    descriptor type
  \param  desc_index   descriptor index
  \param  lang_id      language ID
  \param  desc_length  descriptor length
 */
#ifdef                 EvtUSBH_Core_RequestGetDescriptor
  __STATIC_INLINE void EvrUSBH_Core_RequestGetDescriptor(  uint8_t device,    uint8_t recipient,    uint8_t desc_type,    uint8_t desc_index,    uint16_t lang_id,          uint16_t desc_length) {
    EventRecord2(      EvtUSBH_Core_RequestGetDescriptor, TO_BYTE0(device) | TO_BYTE1(recipient) | TO_BYTE2(desc_type) | TO_BYTE3(desc_index), ((uint32_t)lang_id << 16) | (uint32_t)desc_length);
  }
#else
  #define              EvrUSBH_Core_RequestGetDescriptor(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_GetDescriptor failed (Error)
  \param  device       device index
  \param  recipient    recipient
  \param  desc_type    descriptor type
  \param  desc_index   descriptor index
  \param  lang_id      language ID
  \param  desc_length  descriptor length
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_RequestGetDescriptorFailed
  __STATIC_INLINE void EvrUSBH_Core_RequestGetDescriptorFailed(  uint8_t device,    uint8_t recipient,    uint8_t desc_type,    uint8_t desc_index,    uint16_t lang_id,          uint16_t desc_length, usbStatus error) {
    EventRecord4(      EvtUSBH_Core_RequestGetDescriptorFailed, TO_BYTE0(device) | TO_BYTE1(recipient) | TO_BYTE2(desc_type) | TO_BYTE3(desc_index), ((uint32_t)lang_id << 16) | (uint32_t)desc_length, (uint32_t)error, 0U);
  }
#else
  #define              EvrUSBH_Core_RequestGetDescriptorFailed(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_SetDescriptor start (API)
  \param  device       device index
  \param  recipient    recipient
  \param  desc_type    descriptor type
  \param  desc_index   descriptor index
  \param  lang_id      language ID
  \param  desc_length  descriptor length
 */
#ifdef                 EvtUSBH_Core_RequestSetDescriptor
  __STATIC_INLINE void EvrUSBH_Core_RequestSetDescriptor(  uint8_t device,    uint8_t recipient,    uint8_t desc_type,    uint8_t desc_index,    uint16_t lang_id,          uint16_t desc_length) {
    EventRecord2(      EvtUSBH_Core_RequestSetDescriptor, TO_BYTE0(device) | TO_BYTE1(recipient) | TO_BYTE2(desc_type) | TO_BYTE3(desc_index), ((uint32_t)lang_id << 16) | (uint32_t)desc_length);
  }
#else
  #define              EvrUSBH_Core_RequestSetDescriptor(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_SetDescriptor failed (Error)
  \param  device       device index
  \param  recipient    recipient
  \param  desc_type    descriptor type
  \param  desc_index   descriptor index
  \param  lang_id      language ID
  \param  desc_length  descriptor length
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_RequestSetDescriptorFailed
  __STATIC_INLINE void EvrUSBH_Core_RequestSetDescriptorFailed(  uint8_t device,    uint8_t recipient,    uint8_t desc_type,    uint8_t desc_index,    uint16_t lang_id,          uint16_t desc_length, usbStatus error) {
    EventRecord4(      EvtUSBH_Core_RequestSetDescriptorFailed, TO_BYTE0(device) | TO_BYTE1(recipient) | TO_BYTE2(desc_type) | TO_BYTE3(desc_index), ((uint32_t)lang_id << 16) | (uint32_t)desc_length, (uint32_t)error, 0U);
  }
#else
  #define              EvrUSBH_Core_RequestSetDescriptorFailed(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_GetConfiguration start (API)
  \param  device       device index
  \param  config       configuration
 */
#ifdef                 EvtUSBH_Core_RequestGetConfiguration
  __STATIC_INLINE void EvrUSBH_Core_RequestGetConfiguration(  uint8_t device,    uint8_t config) {
    EventRecord2(      EvtUSBH_Core_RequestGetConfiguration, TO_BYTE0(device), (uint32_t)config);
  }
#else
  #define              EvrUSBH_Core_RequestGetConfiguration(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_GetConfiguration failed (Error)
  \param  device       device index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_RequestGetConfigurationFailed
  __STATIC_INLINE void EvrUSBH_Core_RequestGetConfigurationFailed(  uint8_t device,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_RequestGetConfigurationFailed, TO_BYTE0(device), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_RequestGetConfigurationFailed(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_SetConfiguration start (API)
  \param  device       device index
  \param  config       configuration
 */
#ifdef                 EvtUSBH_Core_RequestSetConfiguration
  __STATIC_INLINE void EvrUSBH_Core_RequestSetConfiguration(  uint8_t device,    uint8_t config) {
    EventRecord2(      EvtUSBH_Core_RequestSetConfiguration, TO_BYTE0(device) | TO_BYTE1(config), 0U);
  }
#else
  #define              EvrUSBH_Core_RequestSetConfiguration(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_SetConfiguration failed (Error)
  \param  device       device index
  \param  config       configuration
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_RequestSetConfigurationFailed
  __STATIC_INLINE void EvrUSBH_Core_RequestSetConfigurationFailed(  uint8_t device,    uint8_t config,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_RequestSetConfigurationFailed, TO_BYTE0(device) | TO_BYTE1(config), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_RequestSetConfigurationFailed(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_GetInterface start (API)
  \param  device       device index
  \param  index        interface index
  \param  alt          alternate setting
 */
#ifdef                 EvtUSBH_Core_RequestGetInterface
  __STATIC_INLINE void EvrUSBH_Core_RequestGetInterface(  uint8_t device,    uint8_t index,    uint8_t alt) {
    EventRecord2(      EvtUSBH_Core_RequestGetInterface, TO_BYTE0(device) | TO_BYTE1(index), (uint32_t)alt);
  }
#else
  #define              EvrUSBH_Core_RequestGetInterface(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_GetInterface failed (Error)
  \param  device       device index
  \param  index        interface index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_RequestGetInterfaceFailed
  __STATIC_INLINE void EvrUSBH_Core_RequestGetInterfaceFailed(  uint8_t device,    uint8_t index,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_RequestGetInterfaceFailed, TO_BYTE0(device) | TO_BYTE1(index), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_RequestGetInterfaceFailed(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_SetInterface start (API)
  \param  device       device index
  \param  index        interface index
  \param  alt          alternate setting
 */
#ifdef                 EvtUSBH_Core_RequestSetInterface
  __STATIC_INLINE void EvrUSBH_Core_RequestSetInterface(  uint8_t device,    uint8_t index,    uint8_t alt) {
    EventRecord2(      EvtUSBH_Core_RequestSetInterface, TO_BYTE0(device) | TO_BYTE1(index) | TO_BYTE2(alt), 0U);
  }
#else
  #define              EvrUSBH_Core_RequestSetInterface(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_SetInterface failed (Error)
  \param  device       device index
  \param  index        interface index
  \param  alt          alternate setting
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_RequestSetInterfaceFailed
  __STATIC_INLINE void EvrUSBH_Core_RequestSetInterfaceFailed(  uint8_t device,    uint8_t index,    uint8_t alt,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_RequestSetInterfaceFailed, TO_BYTE0(device) | TO_BYTE1(index) | TO_BYTE2(alt), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_RequestSetInterfaceFailed(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_SynchFrame start (API)
  \param  device       device index
  \param  index        interface index
  \param  frame_num    frame number
 */
#ifdef                 EvtUSBH_Core_RequestSynchFrame
  __STATIC_INLINE void EvrUSBH_Core_RequestSynchFrame(  uint8_t device,    uint8_t index,   uint16_t frame_num) {
    EventRecord2(      EvtUSBH_Core_RequestSynchFrame, TO_BYTE0(device) | TO_BYTE1(index), (uint32_t)frame_num);
  }
#else
  #define              EvrUSBH_Core_RequestSynchFrame(...)
#endif


/**
  \brief  Event on \ref USBH_DeviceRequest_SynchFrame failed (Error)
  \param  device       device index
  \param  index        interface index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_RequestSynchFrameFailed
  __STATIC_INLINE void EvrUSBH_Core_RequestSynchFrameFailed(  uint8_t device,    uint8_t index,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_RequestSynchFrameFailed, TO_BYTE0(device) | TO_BYTE1(index), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_RequestSynchFrameFailed(...)
#endif


/**
  \brief  Event on internal operation memory initialization failed (Error)
  \param  ctrl         controller index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_MemInitFailed
  __STATIC_INLINE void EvrUSBH_Core_MemInitFailed(  uint8_t ctrl,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_MemInitFailed, TO_BYTE0(ctrl), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_MemInitFailed(...)
#endif


/**
  \brief  Event on internal operation memory initialization finished successfully (Detail)
  \param  ctrl         controller index
 */
#ifdef                 EvtUSBH_Core_MemInit
  __STATIC_INLINE void EvrUSBH_Core_MemInit(  uint8_t ctrl) {
    EventRecord2(      EvtUSBH_Core_MemInit, TO_BYTE0(ctrl), 0U);
  }
#else
  #define              EvrUSBH_Core_MemInit(...)
#endif


/**
  \brief  Event on internal operation memory uninitialization failed (Error)
  \param  ctrl         controller index
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_MemUninitFailed
  __STATIC_INLINE void EvrUSBH_Core_MemUninitFailed(  uint8_t ctrl,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_MemUninitFailed, TO_BYTE0(ctrl), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Core_MemUninitFailed(...)
#endif


/**
  \brief  Event on internal operation memory uninitialization finished successfully (Detail)
  \param  ctrl         controller index
 */
#ifdef                 EvtUSBH_Core_MemUninit
  __STATIC_INLINE void EvrUSBH_Core_MemUninit(  uint8_t ctrl) {
    EventRecord2(      EvtUSBH_Core_MemUninit, TO_BYTE0(ctrl), 0U);
  }
#else
  #define              EvrUSBH_Core_MemUninit(...)
#endif


/**
  \brief  Event on internal operation memory allocation failed (Error)
  \param  ctrl         controller index
  \param  size         size of memory chunk to be allocated
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_MemAllocFailed
  __STATIC_INLINE void EvrUSBH_Core_MemAllocFailed(  uint8_t ctrl,                    uint32_t size,  usbStatus error) {
    EventRecord2(      EvtUSBH_Core_MemAllocFailed, TO_BYTE0(ctrl) | TO_BYTE1(error),          size);
  }
#else
  #define              EvrUSBH_Core_MemAllocFailed(...)
#endif


/**
  \brief  Event on internal operation memory allocation finished successfully (Detail)
  \param  ctrl         controller index
  \param  ptr          pointer to allocated memory
  \param  size         size of memory chunk to be allocated
 */
#ifdef                 EvtUSBH_Core_MemAlloc
  __STATIC_INLINE void EvrUSBH_Core_MemAlloc(  uint8_t ctrl,   const uint8_t *ptr, uint32_t size) {
    EventRecord4(      EvtUSBH_Core_MemAlloc, TO_BYTE0(ctrl), (uint32_t)      ptr,          size, 0U);
  }
#else
  #define              EvrUSBH_Core_MemAlloc(...)
#endif


/**
  \brief  Event on internal operation memory free failed (Error)
  \param  ctrl         controller index
  \param  ptr          pointer to allocated memory chunk to be released
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_Core_MemFreeFailed
  __STATIC_INLINE void EvrUSBH_Core_MemFreeFailed(  uint8_t ctrl,                     const uint8_t *ptr, usbStatus error) {
    EventRecord2(      EvtUSBH_Core_MemFreeFailed, TO_BYTE0(ctrl) | TO_BYTE1(error), (uint32_t)      ptr);
  }
#else
  #define              EvrUSBH_Core_MemFreeFailed(...)
#endif


/**
  \brief  Event on internal operation memory free finished successfully (Detail)
  \param  ctrl         controller index
  \param  ptr          pointer to allocated memory chunk to be released
 */
#ifdef                 EvtUSBH_Core_MemFree
  __STATIC_INLINE void EvrUSBH_Core_MemFree(  uint8_t ctrl,   const uint8_t *ptr) {
    EventRecord2(      EvtUSBH_Core_MemFree, TO_BYTE0(ctrl), (uint32_t)      ptr);
  }
#else
  #define              EvrUSBH_Core_MemFree(...)
#endif


/**
  \brief  Event on internal operation engine start (Detail)
  \param  ctrl         controller index
  \param  port         root HUB port
  \param  event        event
  \param  port_state   current port state
 */
#ifdef                 EvtUSBH_Core_Engine
  __STATIC_INLINE void EvrUSBH_Core_Engine(  uint8_t ctrl,    uint8_t port,                         uint32_t event, uint8_t port_state) {
    EventRecord2(      EvtUSBH_Core_Engine, TO_BYTE0(ctrl) | TO_BYTE1(port) | TO_BYTE2(port_state),          event);
  }
#else
  #define              EvrUSBH_Core_Engine(...)
#endif


/**
  \brief  Event on internal operation engine failed (Error)
  \param  ctrl         controller index
  \param  port         root HUB port
  \param  event        event
 */
#ifdef                 EvtUSBH_Core_EngineFailed
  __STATIC_INLINE void EvrUSBH_Core_EngineFailed(  uint8_t ctrl,    uint8_t port,  uint32_t event) {
    EventRecord2(      EvtUSBH_Core_EngineFailed, TO_BYTE0(ctrl) | TO_BYTE1(port),          event);
  }
#else
  #define              EvrUSBH_Core_EngineFailed(...)
#endif


/**
  \brief  Event on internal operation engine finished successfully (Detail)
  \param  ctrl         controller index
  \param  port         root HUB port
  \param  event        event
  \param  port_state   current port state
 */
#ifdef                 EvtUSBH_Core_EngineDone
  __STATIC_INLINE void EvrUSBH_Core_EngineDone(  uint8_t ctrl,    uint8_t port,                         uint32_t event, uint8_t port_state) {
    EventRecord2(      EvtUSBH_Core_EngineDone, TO_BYTE0(ctrl) | TO_BYTE1(port) | TO_BYTE2(port_state),          event);
  }
#else
  #define              EvrUSBH_Core_EngineDone(...)
#endif


// USB Host Driver component event record functions --------------------------

/**
  \brief  Event on ARM_USBH_Initialize finished successfully (API)
  \param  ctrl         controller index
 */
#ifdef                 EvtUSBH_Driver_Initialize
  __STATIC_INLINE void EvrUSBH_Driver_Initialize(  uint8_t ctrl) {
    EventRecord2(      EvtUSBH_Driver_Initialize, TO_BYTE0(ctrl), 0U);
  }
#else
  #define              EvrUSBH_Driver_Initialize(...)
#endif


/**
  \brief  Event on ARM_USBH_Initialize failed (Error)
  \param  ctrl         controller index
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBH_Driver_InitializeFailed
  __STATIC_INLINE void EvrUSBH_Driver_InitializeFailed(  uint8_t ctrl,    int32_t error) {
    EventRecord2(      EvtUSBH_Driver_InitializeFailed, TO_BYTE0(ctrl), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Driver_InitializeFailed(...)
#endif


/**
  \brief  Event on ARM_USBH_Uninitialize finished successfully (API)
  \param  ctrl         controller index
 */
#ifdef                 EvtUSBH_Driver_Uninitialize
  __STATIC_INLINE void EvrUSBH_Driver_Uninitialize(  uint8_t ctrl) {
    EventRecord2(      EvtUSBH_Driver_Uninitialize, TO_BYTE0(ctrl), 0U);
  }
#else
  #define              EvrUSBH_Driver_Uninitialize(...)
#endif


/**
  \brief  Event on ARM_USBH_Uninitialize failed (Error)
  \param  ctrl         controller index
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBH_Driver_UninitializeFailed
  __STATIC_INLINE void EvrUSBH_Driver_UninitializeFailed(  uint8_t ctrl,    int32_t error) {
    EventRecord2(      EvtUSBH_Driver_UninitializeFailed, TO_BYTE0(ctrl), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Driver_UninitializeFailed(...)
#endif


/**
  \brief  Event on ARM_USBH_PowerControl finished successfully (API)
  \param  ctrl         controller index
  \param  state        power state ARM_POWER_STATE
 */
#ifdef                 EvtUSBH_Driver_PowerControl
  __STATIC_INLINE void EvrUSBH_Driver_PowerControl(  uint8_t ctrl,  ARM_POWER_STATE state) {
    EventRecord2(      EvtUSBH_Driver_PowerControl, TO_BYTE0(ctrl) |       TO_BYTE1(state), 0U);
  }
#else
  #define              EvrUSBH_Driver_PowerControl(...)
#endif


/**
  \brief  Event on ARM_USBH_PowerControl failed (Error)
  \param  ctrl         controller index
  \param  state        power state
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBH_Driver_PowerControlFailed
  __STATIC_INLINE void EvrUSBH_Driver_PowerControlFailed(  uint8_t ctrl,  ARM_POWER_STATE state,    int32_t error) {
    EventRecord2(      EvtUSBH_Driver_PowerControlFailed, TO_BYTE0(ctrl) |       TO_BYTE1(state), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Driver_PowerControlFailed(...)
#endif


/**
  \brief  Event on ARM_USBH_PortVbusOnOff finished successfully (API)
  \param  ctrl         controller index
  \param  port         root HUB port number
  \param  vbus         VBUS state
                        - \b false VBUS off
                        - \b true  VBUS on
 */
#ifdef                 EvtUSBH_Driver_PortVbusOnOff
  __STATIC_INLINE void EvrUSBH_Driver_PortVbusOnOff(  uint8_t ctrl,    uint8_t port,       bool vbus) {
    EventRecord2(      EvtUSBH_Driver_PortVbusOnOff, TO_BYTE0(ctrl) | TO_BYTE1(port) | TO_BYTE2(vbus), 0U);
  }
#else
  #define              EvrUSBH_Driver_PortVbusOnOff(...)
#endif


/**
  \brief  Event on ARM_USBH_PortVbusOnOff failed (Error)
  \param  ctrl         controller index
  \param  port         root HUB port number
  \param  vbus         VBUS state
                        - \b false VBUS off
                        - \b true  VBUS on
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBH_Driver_PortVbusOnOffFailed
  __STATIC_INLINE void EvrUSBH_Driver_PortVbusOnOffFailed(  uint8_t ctrl,    uint8_t port,       bool vbus,    int32_t error) {
    EventRecord2(      EvtUSBH_Driver_PortVbusOnOffFailed, TO_BYTE0(ctrl) | TO_BYTE1(port) | TO_BYTE2(vbus), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Driver_PortVbusOnOffFailed(...)
#endif


/**
  \brief  Event on ARM_USBH_PortReset finished successfully (API)
  \param  ctrl         controller index
  \param  port         root HUB port number
 */
#ifdef                 EvtUSBH_Driver_PortReset
  __STATIC_INLINE void EvrUSBH_Driver_PortReset(  uint8_t ctrl,    uint8_t port) {
    EventRecord2(      EvtUSBH_Driver_PortReset, TO_BYTE0(ctrl) | TO_BYTE1(port), 0U);
  }
#else
  #define              EvrUSBH_Driver_PortReset(...)
#endif


/**
  \brief  Event on ARM_USBH_PortReset failed (Error)
  \param  ctrl         controller index
  \param  port         root HUB port number
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBH_Driver_PortResetFailed
  __STATIC_INLINE void EvrUSBH_Driver_PortResetFailed(  uint8_t ctrl,    uint8_t port,    int32_t error) {
    EventRecord2(      EvtUSBH_Driver_PortResetFailed, TO_BYTE0(ctrl) | TO_BYTE1(port), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Driver_PortResetFailed(...)
#endif


/**
  \brief  Event on ARM_USBH_PortSuspend finished successfully (API)
  \param  ctrl         controller index
  \param  port         root HUB port number
 */
#ifdef                 EvtUSBH_Driver_PortSuspend
  __STATIC_INLINE void EvrUSBH_Driver_PortSuspend(  uint8_t ctrl,    uint8_t port) {
    EventRecord2(      EvtUSBH_Driver_PortSuspend, TO_BYTE0(ctrl) | TO_BYTE1(port), 0U);
  }
#else
  #define              EvrUSBH_Driver_PortSuspend(...)
#endif


/**
  \brief  Event on ARM_USBH_PortSuspend failed (Error)
  \param  ctrl         controller index
  \param  port         root HUB port number
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBH_Driver_PortSuspendFailed
  __STATIC_INLINE void EvrUSBH_Driver_PortSuspendFailed(  uint8_t ctrl,    uint8_t port,    int32_t error) {
    EventRecord2(      EvtUSBH_Driver_PortSuspendFailed, TO_BYTE0(ctrl) | TO_BYTE1(port), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Driver_PortSuspendFailed(...)
#endif


/**
  \brief  Event on ARM_USBH_PortResume finished successfully (API)
  \param  ctrl         controller index
  \param  port         root HUB port number
 */
#ifdef                 EvtUSBH_Driver_PortResume
  __STATIC_INLINE void EvrUSBH_Driver_PortResume(  uint8_t ctrl,    uint8_t port) {
    EventRecord2(      EvtUSBH_Driver_PortResume, TO_BYTE0(ctrl) | TO_BYTE1(port), 0U);
  }
#else
  #define              EvrUSBH_Driver_PortResume(...)
#endif


/**
  \brief  Event on ARM_USBH_PortResume failed (Error)
  \param  ctrl         controller index
  \param  port         root HUB port number
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBH_Driver_PortResumeFailed
  __STATIC_INLINE void EvrUSBH_Driver_PortResumeFailed(  uint8_t ctrl,    uint8_t port,    int32_t error) {
    EventRecord2(      EvtUSBH_Driver_PortResumeFailed, TO_BYTE0(ctrl) | TO_BYTE1(port), (uint32_t)error);
  }
#else
  #define              EvrUSBH_Driver_PortResumeFailed(...)
#endif


/**
  \brief  Event on ARM_USBH_PortGetState finished (API)
  \param  ctrl         controller index
  \param  port         root HUB port number
  \param  state        port state ARM_USBH_STATE
 */
#ifdef                 EvtUSBH_Driver_PortGetState
  __STATIC_INLINE void EvrUSBH_Driver_PortGetState(  uint8_t ctrl,    uint8_t port, ARM_USBH_PORT_STATE state) {
    EventRecord2(      EvtUSBH_Driver_PortGetState, TO_BYTE0(ctrl) | TO_BYTE1(port), (uint32_t)state.connected | ((uint32_t)(state.overcurrent) << 1) | ((uint32_t)(state.speed) << 2));
  }
#else
  #define              EvrUSBH_Driver_PortGetState(...)
#endif


/**
  \brief  Event on ARM_USBH_PipeCreate finished successfully (API)
  \param  ctrl                  controller index
  \param  dev_addr              device address
  \param  dev_speed             device speed
  \param  hub_addr              hub address
  \param  hub_port              hub port
  \param  ep_addr               endpoint address
                                 - ep_addr.0..3: address
                                 - ep_addr.7:    direction
  \param  ep_type               endpoint type (ARM_USB_ENDPOINT_xxx)
  \param  ep_max_packet_size    endpoint maximum packet size
  \param  ep_interval           endpoint polling interval
  \param  pipe_hndl             pipe handle ARM_USBH_PIPE_HANDLE
 */
#ifdef                 EvtUSBH_Driver_PipeCreate
  __STATIC_INLINE void EvrUSBH_Driver_PipeCreate(  uint8_t ctrl,    uint8_t dev_addr,    uint8_t dev_speed,    uint8_t hub_addr,   uint8_t hub_port,    uint8_t ep_addr,    uint8_t ep_type,   uint16_t ep_max_packet_size,   uint8_t ep_interval, ARM_USBH_EP_HANDLE pipe_hndl) {
    EventRecord4(      EvtUSBH_Driver_PipeCreate, TO_BYTE0(ctrl) | TO_BYTE1(dev_addr) | TO_BYTE2(dev_speed) | TO_BYTE3(hub_addr), TO_BYTE0(hub_port) | TO_BYTE1(ep_addr) | TO_BYTE2(ep_type), (uint32_t)ep_max_packet_size | TO_BYTE2(ep_interval),  (uint32_t)pipe_hndl);
  }
#else
  #define              EvrUSBH_Driver_PipeCreate(...)
#endif


/**
  \brief  Event on ARM_USBH_PipeCreate failed (Error)
  \param  ctrl                  controller index
  \param  dev_addr              device address
  \param  dev_speed             device speed
  \param  hub_addr              hub address
  \param  hub_port              hub port
  \param  ep_addr               endpoint address
                                 - ep_addr.0..3: address
                                 - ep_addr.7:    direction
  \param  ep_type               endpoint type (ARM_USB_ENDPOINT_xxx)
  \param  ep_max_packet_size    endpoint maximum packet size
  \param  ep_interval           endpoint polling interval
 */
#ifdef                 EvtUSBH_Driver_PipeCreateFailed
  __STATIC_INLINE void EvrUSBH_Driver_PipeCreateFailed(  uint8_t ctrl,    uint8_t dev_addr,    uint8_t dev_speed,    uint8_t hub_addr,   uint8_t hub_port,    uint8_t ep_addr,    uint8_t ep_type,   uint16_t ep_max_packet_size,   uint8_t ep_interval) {
    EventRecord4(      EvtUSBH_Driver_PipeCreateFailed, TO_BYTE0(ctrl) | TO_BYTE1(dev_addr) | TO_BYTE2(dev_speed) | TO_BYTE3(hub_addr), TO_BYTE0(hub_port) | TO_BYTE1(ep_addr) | TO_BYTE2(ep_type), (uint32_t)ep_max_packet_size | TO_BYTE2(ep_interval), 0U);
  }
#else
  #define              EvrUSBH_Driver_PipeCreateFailed(...)
#endif


/**
  \brief  Event on ARM_USBH_PipeModify finished successfully (API)
  \param  ctrl                  controller index
  \param  pipe_hndl             pipe handle
  \param  dev_addr              device address
  \param  dev_speed             device speed
  \param  hub_addr              hub address
  \param  hub_port              hub port
  \param  ep_max_packet_size    endpoint maximum packet size
 */
#ifdef                 EvtUSBH_Driver_PipeModify
  __STATIC_INLINE void EvrUSBH_Driver_PipeModify(  uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl, uint8_t dev_addr,    uint8_t dev_speed,    uint8_t hub_addr,   uint8_t hub_port,    uint16_t ep_max_packet_size) {
    EventRecord4(      EvtUSBH_Driver_PipeModify, TO_BYTE0(ctrl) |                              TO_BYTE1(dev_addr) | TO_BYTE2(dev_speed) | TO_BYTE3(hub_addr), TO_BYTE2(hub_port) | (uint32_t)ep_max_packet_size, (uint32_t)pipe_hndl, 0U);
  }
#else
  #define              EvrUSBH_Driver_PipeModify(...)
#endif


/**
  \brief  Event on ARM_USBH_PipeModify failed (Error)
  \param  ctrl                  controller index
  \param  pipe_hndl             pipe handle
  \param  dev_addr              device address
  \param  dev_speed             device speed
  \param  hub_addr              hub address
  \param  hub_port              hub port
  \param  ep_max_packet_size    endpoint maximum packet size
  \param  error                 error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBH_Driver_PipeModifyFailed
  __STATIC_INLINE void EvrUSBH_Driver_PipeModifyFailed(  uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl, uint8_t dev_addr,    uint8_t dev_speed,    uint8_t hub_addr,   uint8_t hub_port,    uint16_t ep_max_packet_size,                        int32_t error) {
    EventRecord4(      EvtUSBH_Driver_PipeModifyFailed, TO_BYTE0(ctrl) |                              TO_BYTE1(dev_addr) | TO_BYTE2(dev_speed) | TO_BYTE3(hub_addr), TO_BYTE2(hub_port) | (uint32_t)ep_max_packet_size, (uint32_t)pipe_hndl, (uint32_t)error);
  }
#else
  #define              EvrUSBH_Driver_PipeModifyFailed(...)
#endif


/**
  \brief  Event on ARM_USBH_PipeDelete finished successfully (API)
  \param  ctrl         controller index
  \param  pipe_hndl    pipe handle
 */
#ifdef                 EvtUSBH_Driver_PipeDelete
  __STATIC_INLINE void EvrUSBH_Driver_PipeDelete(  uint8_t ctrl,  ARM_USBH_PIPE_HANDLE pipe_hndl) {
    EventRecord2(      EvtUSBH_Driver_PipeDelete, TO_BYTE0(ctrl),            (uint32_t)pipe_hndl);
  }
#else
  #define              EvrUSBH_Driver_PipeDelete(...)
#endif


/**
  \brief  Event on ARM_USBH_PipeDelete failed (Error)
  \param  ctrl         controller index
  \param  pipe_hndl    pipe handle
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBH_Driver_PipeDeleteFailed
  __STATIC_INLINE void EvrUSBH_Driver_PipeDeleteFailed(  uint8_t ctrl,  ARM_USBH_PIPE_HANDLE pipe_hndl,  int32_t error) {
    EventRecord2(      EvtUSBH_Driver_PipeDeleteFailed, TO_BYTE0(ctrl) |                                TO_BYTE1(error), (uint32_t)pipe_hndl);
  }
#else
  #define              EvrUSBH_Driver_PipeDeleteFailed(...)
#endif


/**
  \brief  Event on ARM_USBH_PipeReset finished successfully (API)
  \param  ctrl         controller index
  \param  pipe_hndl    pipe handle
 */
#ifdef                 EvtUSBH_Driver_PipeReset
  __STATIC_INLINE void EvrUSBH_Driver_PipeReset(  uint8_t ctrl,  ARM_USBH_PIPE_HANDLE pipe_hndl) {
    EventRecord2(      EvtUSBH_Driver_PipeReset, TO_BYTE0(ctrl),            (uint32_t)pipe_hndl);
  }
#else
  #define              EvrUSBH_Driver_PipeReset(...)
#endif


/**
  \brief  Event on ARM_USBH_PipeReset failed (Error)
  \param  ctrl         controller index
  \param  pipe_hndl    pipe handle
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBH_Driver_PipeResetFailed
  __STATIC_INLINE void EvrUSBH_Driver_PipeResetFailed(  uint8_t ctrl,  ARM_USBH_PIPE_HANDLE pipe_hndl,  int32_t error) {
    EventRecord2(      EvtUSBH_Driver_PipeResetFailed, TO_BYTE0(ctrl) |                                TO_BYTE1(error), (uint32_t)pipe_hndl);
  }
#else
  #define              EvrUSBH_Driver_PipeResetFailed(...)
#endif


/**
  \brief  Event on ARM_USBH_PipeTransfer finished successfully (API)
  \param  ctrl         controller index
  \param  pipe_hndl    pipe handle
  \param  packet       packet information
  \param  num          number of data bytes to transfer
 */
#ifdef                 EvtUSBH_Driver_PipeTransfer
  __STATIC_INLINE void EvrUSBH_Driver_PipeTransfer(  uint8_t ctrl,  ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t packet,              uint32_t num) {
    EventRecord4(      EvtUSBH_Driver_PipeTransfer, TO_BYTE0(ctrl)                                | TO_BYTE1(packet), (uint32_t)pipe_hndl, num, 0U);
  }
#else
  #define              EvrUSBH_Driver_PipeTransfer(...)
#endif


/**
  \brief  Event on ARM_USBH_PipeTransfer failed (Error)
  \param  ctrl         controller index
  \param  pipe_hndl    pipe handle
  \param  packet       packet information
  \param  num          number of data bytes to transfer
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBH_Driver_PipeTransferFailed
  __STATIC_INLINE void EvrUSBH_Driver_PipeTransferFailed(  uint8_t ctrl,  ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t packet,              uint32_t num,   int32_t error) {
    EventRecord4(      EvtUSBH_Driver_PipeTransferFailed, TO_BYTE0(ctrl)                                | TO_BYTE1(packet), (uint32_t)pipe_hndl, num, (uint32_t)error);
  }
#else
  #define              EvrUSBH_Driver_PipeTransferFailed(...)
#endif


/**
  \brief  Event on ARM_USBH_PipeTransferGetResult finished (API)
  \param  ctrl         controller index
  \param  pipe_hndl    pipe handle
  \param  num          number of successfully transferred data bytes
 */
#ifdef                 EvtUSBH_Driver_PipeTransferGetResult
  __STATIC_INLINE void EvrUSBH_Driver_PipeTransferGetResult(  uint8_t ctrl,  ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t num) {
    EventRecord4(      EvtUSBH_Driver_PipeTransferGetResult, TO_BYTE0(ctrl),            (uint32_t)pipe_hndl,          num, 0U);
  }
#else
  #define              EvrUSBH_Driver_PipeTransferGetResult(...)
#endif


/**
  \brief  Event on ARM_USBH_PipeTransferAbort finished successfully (API)
  \param  ctrl         controller index
  \param  pipe_hndl    pipe handle
 */
#ifdef                 EvtUSBH_Driver_PipeTransferAbort
  __STATIC_INLINE void EvrUSBH_Driver_PipeTransferAbort(  uint8_t ctrl,  ARM_USBH_PIPE_HANDLE pipe_hndl) {
    EventRecord2(      EvtUSBH_Driver_PipeTransferAbort, TO_BYTE0(ctrl),            (uint32_t)pipe_hndl);
  }
#else
  #define              EvrUSBH_Driver_PipeTransferAbort(...)
#endif


/**
  \brief  Event on ARM_USBH_PipeTransferAbort failed (Error)
  \param  ctrl         controller index
  \param  pipe_hndl    pipe handle
  \param  error        error code (ARM_DRIVER_xxx)
 */
#ifdef                 EvtUSBH_Driver_PipeTransferAbortFailed
  __STATIC_INLINE void EvrUSBH_Driver_PipeTransferAbortFailed(  uint8_t ctrl,  ARM_USBH_PIPE_HANDLE pipe_hndl,  int32_t error) {
    EventRecord2(      EvtUSBH_Driver_PipeTransferAbortFailed, TO_BYTE0(ctrl) |                                TO_BYTE1(error), (uint32_t)pipe_hndl);
  }
#else
  #define              EvrUSBH_Driver_PipeTransferAbortFailed(...)
#endif


/**
  \brief  Event on ARM_USBH_GetFrameNumber finished (API)
  \param  ctrl         controller index
  \param  frame_number frame number
 */
#ifdef                 EvtUSBH_Driver_GetFrameNumber
  __STATIC_INLINE void EvrUSBH_Driver_GetFrameNumber(  uint8_t ctrl,   uint16_t frame_number) {
    EventRecord2(      EvtUSBH_Driver_GetFrameNumber, TO_BYTE0(ctrl), (uint32_t)frame_number);
  }
#else
  #define              EvrUSBH_Driver_GetFrameNumber(...)
#endif


/**
  \brief  Event on ARM_USBH_SignalPortEvent callback start (API)
  \param  ctrl         controller index
  \param  port         root HUB port number
  \param  event        port event USBH_port_events
 */
#ifdef                 EvtUSBH_Driver_OnSignalPortEvent
  __STATIC_INLINE void EvrUSBH_Driver_OnSignalPortEvent(  uint8_t ctrl,    uint8_t port,  uint32_t event) {
    EventRecord2(      EvtUSBH_Driver_OnSignalPortEvent, TO_BYTE0(ctrl) | TO_BYTE1(port),          event);
  }
#else
  #define              EvrUSBH_Driver_OnSignalPortEvent(...)
#endif


/**
  \brief  Event on ARM_USBH_SignalPipeEvent callback start (API)
  \param  ctrl         controller index
  \param  pipe_hndl    pipe handle
  \param  event        pipe event USBH_pipe_events
 */
#ifdef                 EvtUSBH_Driver_OnSignalPipeEvent
  __STATIC_INLINE void EvrUSBH_Driver_OnSignalPipeEvent(  uint8_t ctrl, ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t event) {
    EventRecord2(      EvtUSBH_Driver_OnSignalPipeEvent, TO_BYTE0(ctrl) |                               TO_BYTE1(event), (uint32_t)pipe_hndl);
  }
#else
  #define              EvrUSBH_Driver_OnSignalPipeEvent(...)
#endif


// USB Host Custom Class component event record functions --------------------

/**
  \brief  Event on \ref USBH_CustomClass_GetDevice finished (API)
  \param  instance     instance of custom class device
  \param  device       device index
                        - value <= 127: index of Device
                        - value == 255: non-existing Device index
 */
#ifdef                 EvtUSBH_CC_GetDevice
  __STATIC_INLINE void EvrUSBH_CC_GetDevice(  uint8_t instance,   uint8_t device) {
    EventRecord2(      EvtUSBH_CC_GetDevice, TO_BYTE0(instance), TO_BYTE0(device));
  }
#else
  #define              EvrUSBH_CC_GetDevice(...)
#endif


/**
  \brief  Event on \ref USBH_CustomClass_GetStatus finished (API)
  \param  instance     instance of custom class device
  \param  status       device status \ref usbStatus
 */
#ifdef                 EvtUSBH_CC_GetStatus
  __STATIC_INLINE void EvrUSBH_CC_GetStatus(  uint8_t instance,    uint8_t status) {
    EventRecord2(      EvtUSBH_CC_GetStatus, TO_BYTE0(instance), (uint32_t)status);
  }
#else
  #define              EvrUSBH_CC_GetStatus(...)
#endif


/**
  \brief  Event on \ref USBH_CustomClass_Configure user callback function finished (API)
  \param  device       device index
  \param  class        device class
  \param  sub_class    device subclass
  \param  vid          vendor ID
  \param  pid          product ID
  \param  instance     configured custom class device instance
                        - value <= 127:  index of configured custom class device instance
                        - value == 255: configuration failed
 */
#ifdef                 EvtUSBH_CC_OnConfigure
  __STATIC_INLINE void EvrUSBH_CC_OnConfigure(  uint8_t device,    uint8_t class,    uint8_t sub_class,                         uint16_t vid,          uint16_t pid, uint8_t instance) {
    EventRecord2(      EvtUSBH_CC_OnConfigure, TO_BYTE0(device) | TO_BYTE1(class) | TO_BYTE2(sub_class) | TO_BYTE3(instance), ((uint32_t)vid << 16) | (uint32_t)pid);
  }
#else
  #define              EvrUSBH_CC_OnConfigure(...)
#endif


/**
  \brief  Event on \ref USBH_CustomClass_Unconfigure user callback function finished (API)
  \param  instance     custom class device instance
  \param  status       status code \ref usbStatus
 */
#ifdef                 EvtUSBH_CC_OnUnconfigure
  __STATIC_INLINE void EvrUSBH_CC_OnUnconfigure(  uint8_t instance,  usbStatus status) {
    EventRecord2(      EvtUSBH_CC_OnUnconfigure, TO_BYTE0(instance), (uint32_t)status);
  }
#else
  #define              EvrUSBH_CC_OnUnconfigure(...)
#endif


/**
  \brief  Event on \ref USBH_CustomClass_Initialize user callback function finished (API)
  \param  instance     custom class device instance
  \param  status       status code \ref usbStatus
 */
#ifdef                 EvtUSBH_CC_OnInitialize
  __STATIC_INLINE void EvrUSBH_CC_OnInitialize(  uint8_t instance,  usbStatus status) {
    EventRecord2(      EvtUSBH_CC_OnInitialize, TO_BYTE0(instance), (uint32_t)status);
  }
#else
  #define              EvrUSBH_CC_OnInitialize(...)
#endif


/**
  \brief  Event on \ref USBH_CustomClass_Uninitialize user callback function finished (API)
  \param  instance     custom class device instance
  \param  status       status code \ref usbStatus
 */
#ifdef                 EvtUSBH_CC_OnUninitialize
  __STATIC_INLINE void EvrUSBH_CC_OnUninitialize(  uint8_t instance,  usbStatus status) {
    EventRecord2(      EvtUSBH_CC_OnUninitialize, TO_BYTE0(instance), (uint32_t)status);
  }
#else
  #define              EvrUSBH_CC_OnUninitialize(...)
#endif


// USB Host CDC component event record functions -----------------------------

/**
  \brief  Event on internal operation CDC ACM device configuration start (Operation)
  \param  instance     instance of CDC ACM device
 */
#ifdef                 EvtUSBH_CDC_ACM_Configure
  __STATIC_INLINE void EvrUSBH_CDC_ACM_Configure(  uint8_t instance) {
    EventRecord2(      EvtUSBH_CDC_ACM_Configure, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_CDC_ACM_Configure(...)
#endif


/**
  \brief  Event on internal operation CDC ACM device configuration failed (Error)
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_CDC_ACM_ConfigureFailed
  __STATIC_INLINE void EvrUSBH_CDC_ACM_ConfigureFailed( usbStatus error) {
    EventRecord2(      EvtUSBH_CDC_ACM_ConfigureFailed, (uint32_t)error, 0U);
  }
#else
  #define              EvrUSBH_CDC_ACM_ConfigureFailed(...)
#endif


/**
  \brief  Event on internal operation CDC ACM device unconfigure start (Operation)
  \param  instance     instance of CDC ACM device
 */
#ifdef                 EvtUSBH_CDC_ACM_Unconfigure
  __STATIC_INLINE void EvrUSBH_CDC_ACM_Unconfigure(  uint8_t instance) {
    EventRecord2(      EvtUSBH_CDC_ACM_Unconfigure, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_CDC_ACM_Unconfigure(...)
#endif


/**
  \brief  Event on internal operation CDC ACM device uninitialization failed (Error)
  \param  instance     instance of CDC ACM device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_CDC_ACM_UnconfigureFailed
  __STATIC_INLINE void EvrUSBH_CDC_ACM_UnconfigureFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_CDC_ACM_UnconfigureFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_CDC_ACM_UnconfigureFailed(...)
#endif


/**
  \brief  Event on internal operation CDC ACM device initialization start (Operation)
  \param  instance     instance of CDC ACM device
 */
#ifdef                 EvtUSBH_CDC_ACM_Initialize
  __STATIC_INLINE void EvrUSBH_CDC_ACM_Initialize(  uint8_t instance) {
    EventRecord2(      EvtUSBH_CDC_ACM_Initialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_CDC_ACM_Initialize(...)
#endif


/**
  \brief  Event on internal operation CDC ACM device initialization failed (Error)
  \param  instance     instance of CDC ACM device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_CDC_ACM_InitializeFailed
  __STATIC_INLINE void EvrUSBH_CDC_ACM_InitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_CDC_ACM_InitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_CDC_ACM_InitializeFailed(...)
#endif

/**
  \brief  Event on \ref USBH_CDC_ACM_Initialize user callback function start (API)
  \param  instance     instance of CDC ACM device
 */
#ifdef                 EvtUSBH_CDC_ACM_OnInitialize
  __STATIC_INLINE void EvrUSBH_CDC_ACM_OnInitialize(  uint8_t instance) {
    EventRecord2(      EvtUSBH_CDC_ACM_OnInitialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_CDC_ACM_OnInitialize(...)
#endif


/**
  \brief  Event on internal operation CDC ACM device uninitialization start (Operation)
  \param  instance     instance of CDC ACM device
 */
#ifdef                 EvtUSBH_CDC_ACM_Uninitialize
  __STATIC_INLINE void EvrUSBH_CDC_ACM_Uninitialize(  uint8_t instance) {
    EventRecord2(      EvtUSBH_CDC_ACM_Uninitialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_CDC_ACM_Uninitialize(...)
#endif


/**
  \brief  Event on internal operation CDC ACM device uninitialization failed (Error)
  \param  instance     instance of CDC ACM device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_CDC_ACM_UninitializeFailed
  __STATIC_INLINE void EvrUSBH_CDC_ACM_UninitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_CDC_ACM_UninitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_CDC_ACM_UninitializeFailed(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_Uninitialize user callback function start (API)
  \param  instance     instance of CDC ACM device
 */
#ifdef                 EvtUSBH_CDC_ACM_OnUninitialize
  __STATIC_INLINE void EvrUSBH_CDC_ACM_OnUninitialize(  uint8_t instance) {
    EventRecord2(      EvtUSBH_CDC_ACM_OnUninitialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_CDC_ACM_OnUninitialize(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_GetDevice finished (API)
  \param  instance     instance of CDC ACM device
  \param  device       device index
                        - value <= 127: index of Device
                        - value == 255: non-existing Device index
 */
#ifdef                 EvtUSBH_CDC_ACM_GetDevice
  __STATIC_INLINE void EvrUSBH_CDC_ACM_GetDevice(  uint8_t instance,   uint8_t device) {
    EventRecord2(      EvtUSBH_CDC_ACM_GetDevice, TO_BYTE0(instance), TO_BYTE0(device));
  }
#else
  #define              EvrUSBH_CDC_ACM_GetDevice(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_GetStatus finished (API)
  \param  instance     instance of CDC ACM device
  \param  status       device status \ref usbStatus
 */
#ifdef                 EvtUSBH_CDC_ACM_GetStatus
  __STATIC_INLINE void EvrUSBH_CDC_ACM_GetStatus(  uint8_t instance,    uint8_t status) {
    EventRecord2(      EvtUSBH_CDC_ACM_GetStatus, TO_BYTE0(instance), (uint32_t)status);
  }
#else
  #define              EvrUSBH_CDC_ACM_GetStatus(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_Send start (API)
  \param  instance     instance of CDC ACM device
  \param  num          number of bytes to send
 */
#ifdef                 EvtUSBH_CDC_ACM_Send
  __STATIC_INLINE void EvrUSBH_CDC_ACM_Send(  uint8_t instance,   uint32_t num) {
    EventRecord2(      EvtUSBH_CDC_ACM_Send, TO_BYTE0(instance),           num);
  }
#else
  #define              EvrUSBH_CDC_ACM_Send(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_Send failed (Error)
  \param  instance     instance of CDC ACM device
  \param  num          number of bytes to send
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_CDC_ACM_SendFailed
  __STATIC_INLINE void EvrUSBH_CDC_ACM_SendFailed(  uint8_t instance,                     uint32_t num, usbStatus error) {
    EventRecord2(      EvtUSBH_CDC_ACM_SendFailed, TO_BYTE0(instance) | TO_BYTE1(error),           num);
  }
#else
  #define              EvrUSBH_CDC_ACM_SendFailed(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_Send finished successfully (Detail)
  \param  instance     instance of CDC ACM device
  \param  num          number of bytes to send
 */
#ifdef                 EvtUSBH_CDC_ACM_SendDone
  __STATIC_INLINE void EvrUSBH_CDC_ACM_SendDone(  uint8_t instance,   uint32_t num) {
    EventRecord2(      EvtUSBH_CDC_ACM_SendDone, TO_BYTE0(instance),           num);
  }
#else
  #define              EvrUSBH_CDC_ACM_SendDone(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_GetTxCount finished successfully (API)
  \param  instance     instance of CDC ACM device
  \param  num          number of successfully sent data bytes
 */
#ifdef                 EvtUSBH_CDC_ACM_GetTxCount
  __STATIC_INLINE void EvrUSBH_CDC_ACM_GetTxCount(  uint8_t instance,   uint32_t num) {
    EventRecord2(      EvtUSBH_CDC_ACM_GetTxCount, TO_BYTE0(instance),           num);
  }
#else
  #define              EvrUSBH_CDC_ACM_GetTxCount(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_AbortSend start (API)
  \param  instance     instance of CDC ACM device
 */
#ifdef                 EvtUSBH_CDC_ACM_AbortSend
  __STATIC_INLINE void EvrUSBH_CDC_ACM_AbortSend(  uint8_t instance) {
    EventRecord2(      EvtUSBH_CDC_ACM_AbortSend, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_CDC_ACM_AbortSend(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_AbortSend failed (Error)
  \param  instance     instance of CDC ACM device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_CDC_ACM_AbortSendFailed
  __STATIC_INLINE void EvrUSBH_CDC_ACM_AbortSendFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_CDC_ACM_AbortSendFailed, TO_BYTE0(instance) | TO_BYTE1(error), 0U);
  }
#else
  #define              EvrUSBH_CDC_ACM_AbortSendFailed(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_AbortSend finished successfully (Detail)
  \param  instance     instance of CDC ACM device
 */
#ifdef                 EvtUSBH_CDC_ACM_AbortSendDone
  __STATIC_INLINE void EvrUSBH_CDC_ACM_AbortSendDone(  uint8_t instance) {
    EventRecord2(      EvtUSBH_CDC_ACM_AbortSendDone, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_CDC_ACM_AbortSendDone(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_Receive start (API)
  \param  instance     instance of CDC ACM device
  \param  num          maximum number of bytes to receive
 */
#ifdef                 EvtUSBH_CDC_ACM_Receive
  __STATIC_INLINE void EvrUSBH_CDC_ACM_Receive(  uint8_t instance,   uint32_t num) {
    EventRecord2(      EvtUSBH_CDC_ACM_Receive, TO_BYTE0(instance),           num);
  }
#else
  #define              EvrUSBH_CDC_ACM_Receive(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_Receive failed (Error)
  \param  instance     instance of CDC ACM device
  \param  num          maximum number of bytes to receive
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_CDC_ACM_ReceiveFailed
  __STATIC_INLINE void EvrUSBH_CDC_ACM_ReceiveFailed(  uint8_t instance,                     uint32_t num, usbStatus error) {
    EventRecord2(      EvtUSBH_CDC_ACM_ReceiveFailed, TO_BYTE0(instance) | TO_BYTE1(error),           num);
  }
#else
  #define              EvrUSBH_CDC_ACM_ReceiveFailed(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_Receive finished successfully (Detail)
  \param  instance     instance of CDC ACM device
  \param  num          maximum number of bytes to receive
 */
#ifdef                 EvtUSBH_CDC_ACM_ReceiveDone
  __STATIC_INLINE void EvrUSBH_CDC_ACM_ReceiveDone(  uint8_t instance,   uint32_t num) {
    EventRecord2(      EvtUSBH_CDC_ACM_ReceiveDone, TO_BYTE0(instance),           num);
  }
#else
  #define              EvrUSBH_CDC_ACM_ReceiveDone(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_GetRxCount finished successfully (API)
  \param  instance     instance of CDC ACM device
  \param  num          number of successfully received data bytes
 */
#ifdef                 EvtUSBH_CDC_ACM_GetRxCount
  __STATIC_INLINE void EvrUSBH_CDC_ACM_GetRxCount(  uint8_t instance,   uint32_t num) {
    EventRecord2(      EvtUSBH_CDC_ACM_GetRxCount, TO_BYTE0(instance),           num);
  }
#else
  #define              EvrUSBH_CDC_ACM_GetRxCount(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_AbortReceive start (API)
  \param  instance     instance of CDC ACM device
 */
#ifdef                 EvtUSBH_CDC_ACM_AbortReceive
  __STATIC_INLINE void EvrUSBH_CDC_ACM_AbortReceive(  uint8_t instance) {
    EventRecord2(      EvtUSBH_CDC_ACM_AbortReceive, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_CDC_ACM_AbortReceive(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_AbortReceive failed (Error)
  \param  instance     instance of CDC ACM device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_CDC_ACM_AbortReceiveFailed
  __STATIC_INLINE void EvrUSBH_CDC_ACM_AbortReceiveFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_CDC_ACM_AbortReceiveFailed, TO_BYTE0(instance) | TO_BYTE1(error), 0U);
  }
#else
  #define              EvrUSBH_CDC_ACM_AbortReceiveFailed(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_AbortReceive finished successfully (Detail)
  \param  instance     instance of CDC ACM device
 */
#ifdef                 EvtUSBH_CDC_ACM_AbortReceiveDone
  __STATIC_INLINE void EvrUSBH_CDC_ACM_AbortReceiveDone(  uint8_t instance) {
    EventRecord2(      EvtUSBH_CDC_ACM_AbortReceiveDone, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_CDC_ACM_AbortReceiveDone(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_SetLineCoding start (API)
  \param  instance     instance of CDC ACM device
  \param  rate         data terminal rate in bits per second
 */
#ifdef                 EvtUSBH_CDC_ACM_SetLineCoding
  __STATIC_INLINE void EvrUSBH_CDC_ACM_SetLineCoding(  uint8_t instance,  uint32_t rate) {
    EventRecord2(      EvtUSBH_CDC_ACM_SetLineCoding, TO_BYTE0(instance),          rate);
  }
#else
  #define              EvrUSBH_CDC_ACM_SetLineCoding(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_SetLineCoding failed (Error)
  \param  instance     instance of CDC ACM device
  \param  rate         data terminal rate in bits per second
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_CDC_ACM_SetLineCodingFailed
  __STATIC_INLINE void EvrUSBH_CDC_ACM_SetLineCodingFailed(  uint8_t instance,                     uint32_t rate, usbStatus error) {
    EventRecord2(      EvtUSBH_CDC_ACM_SetLineCodingFailed, TO_BYTE0(instance) | TO_BYTE1(error),           rate);
  }
#else
  #define              EvrUSBH_CDC_ACM_SetLineCodingFailed(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_GetLineCoding failed (Error)
  \param  instance     instance of CDC ACM device
  \param  rate         data terminal rate in bits per second
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_CDC_ACM_GetLineCodingFailed
  __STATIC_INLINE void EvrUSBH_CDC_ACM_GetLineCodingFailed(  uint8_t instance,                     uint32_t rate, usbStatus error) {
    EventRecord2(      EvtUSBH_CDC_ACM_GetLineCodingFailed, TO_BYTE0(instance) | TO_BYTE1(error),           rate);
  }
#else
  #define              EvrUSBH_CDC_ACM_GetLineCodingFailed(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_GetLineCoding finished successfully (API)
  \param  instance     instance of CDC ACM device
  \param  rate         data terminal rate in bits per second
 */
#ifdef                 EvtUSBH_CDC_ACM_GetLineCoding
  __STATIC_INLINE void EvrUSBH_CDC_ACM_GetLineCoding(  uint8_t instance,  uint32_t rate) {
    EventRecord2(      EvtUSBH_CDC_ACM_GetLineCoding, TO_BYTE0(instance),          rate);
  }
#else
  #define              EvrUSBH_CDC_ACM_GetLineCoding(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_SetControlLineState start (API)
  \param  instance     instance of CDC ACM device
  \param  dtr          DTR control line state
  \param  rts          RTS control line state
 */
#ifdef                 EvtUSBH_CDC_ACM_SetControlLineState
  __STATIC_INLINE void EvrUSBH_CDC_ACM_SetControlLineState(  uint8_t instance,   uint8_t dtr,    uint8_t rts) {
    EventRecord2(      EvtUSBH_CDC_ACM_SetControlLineState, TO_BYTE0(instance), TO_BYTE0(dtr) | TO_BYTE1(rts));
  }
#else
  #define              EvrUSBH_CDC_ACM_SetControlLineState(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_SetControlLineState failed (Error)
  \param  instance     instance of CDC ACM device
  \param  dtr          DTR control line state
  \param  rts          RTS control line state
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_CDC_ACM_SetControlLineStateFailed
  __STATIC_INLINE void EvrUSBH_CDC_ACM_SetControlLineStateFailed(  uint8_t instance,                     uint8_t dtr,    uint8_t rts,  usbStatus error) {
    EventRecord2(      EvtUSBH_CDC_ACM_SetControlLineStateFailed, TO_BYTE0(instance) | TO_BYTE1(error), TO_BYTE0(dtr) | TO_BYTE1(rts));
  }
#else
  #define              EvrUSBH_CDC_ACM_SetControlLineStateFailed(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_Notify user callback function start (API)
  \param  instance     instance of CDC ACM device
  \param  state        error status and line states:
                        - bit 6: bOverRun
                        - bit 5: bParity
                        - bit 4: bFraming
                        - bit 3: bRingSignal
                        - bit 2: bBreak
                        - bit 1: bTxCarrier (DSR line state)
                        - bit 0: bRxCarrier (DCD line state)
 */
#ifdef                 EvtUSBH_CDC_ACM_OnNotify
  __STATIC_INLINE void EvrUSBH_CDC_ACM_OnNotify(  uint8_t instance,   uint16_t state) {
    EventRecord2(      EvtUSBH_CDC_ACM_OnNotify, TO_BYTE0(instance), (uint32_t)state);
  }
#else
  #define              EvrUSBH_CDC_ACM_OnNotify(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_SendBreak start (API)
  \param  instance     instance of CDC ACM device
  \param  duration     duration of break (in milliseconds):
                        - value = 0xFFFF: indefinite
                        - value =      0: immediate
 */
#ifdef                 EvtUSBH_CDC_ACM_SendBreak
  __STATIC_INLINE void EvrUSBH_CDC_ACM_SendBreak(  uint8_t instance,   uint16_t duration) {
    EventRecord2(      EvtUSBH_CDC_ACM_SendBreak, TO_BYTE0(instance), (uint32_t)duration);
  }
#else
  #define              EvrUSBH_CDC_ACM_SendBreak(...)
#endif


/**
  \brief  Event on \ref USBH_CDC_ACM_SendBreak failed (Error)
  \param  instance     instance of CDC ACM device
  \param  duration     duration of break (in milliseconds):
                        - value = 0xFFFF: indefinite
                        - value =      0: immediate
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_CDC_ACM_SendBreakFailed
  __STATIC_INLINE void EvrUSBH_CDC_ACM_SendBreakFailed(  uint8_t instance,                     uint16_t duration, usbStatus error) {
    EventRecord2(      EvtUSBH_CDC_ACM_SendBreakFailed, TO_BYTE0(instance) | TO_BYTE1(error), (uint32_t)duration);
  }
#else
  #define              EvrUSBH_CDC_ACM_SendBreakFailed(...)
#endif


// USB Host HID component event record functions -------------------------------

/**
  \brief  Event on internal operation HID device configuration start (Operation)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_Configure
  __STATIC_INLINE void EvrUSBH_HID_Configure(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_Configure, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_Configure(...)
#endif


/**
  \brief  Event on internal operation HID device configuration failed (Error)
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_ConfigureFailed
  __STATIC_INLINE void EvrUSBH_HID_ConfigureFailed( usbStatus error) {
    EventRecord2(      EvtUSBH_HID_ConfigureFailed, (uint32_t)error, 0U);
  }
#else
  #define              EvrUSBH_HID_ConfigureFailed(...)
#endif


/**
  \brief  Event on internal operation HID device unconfigure start (Operation)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_Unconfigure
  __STATIC_INLINE void EvrUSBH_HID_Unconfigure(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_Unconfigure, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_Unconfigure(...)
#endif


/**
  \brief  Event on internal operation HID device uninitialization failed (Error)
  \param  instance     instance of HID device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_UnconfigureFailed
  __STATIC_INLINE void EvrUSBH_HID_UnconfigureFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_HID_UnconfigureFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_HID_UnconfigureFailed(...)
#endif


/**
  \brief  Event on internal operation HID device initialization start (Operation)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_Initialize
  __STATIC_INLINE void EvrUSBH_HID_Initialize(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_Initialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_Initialize(...)
#endif


/**
  \brief  Event on internal operation HID device initialization failed (Error)
  \param  instance     instance of HID device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_InitializeFailed
  __STATIC_INLINE void EvrUSBH_HID_InitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_HID_InitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_HID_InitializeFailed(...)
#endif

/**
  \brief  Event on \ref USBH_HID_Initialize user callback function start (API)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_OnInitialize
  __STATIC_INLINE void EvrUSBH_HID_OnInitialize(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_OnInitialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_OnInitialize(...)
#endif


/**
  \brief  Event on internal operation HID device uninitialization start (Operation)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_Uninitialize
  __STATIC_INLINE void EvrUSBH_HID_Uninitialize(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_Uninitialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_Uninitialize(...)
#endif


/**
  \brief  Event on internal operation HID device uninitialization failed (Error)
  \param  instance     instance of HID device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_UninitializeFailed
  __STATIC_INLINE void EvrUSBH_HID_UninitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_HID_UninitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_HID_UninitializeFailed(...)
#endif


/**
  \brief  Event on \ref USBH_HID_Uninitialize user callback function start (API)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_OnUninitialize
  __STATIC_INLINE void EvrUSBH_HID_OnUninitialize(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_OnUninitialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_OnUninitialize(...)
#endif


/**
  \brief  Event on \ref USBH_HID_GetDevice finished (API)
  \param  instance     instance of HID device
  \param  device       device index
                        - value <= 127: index of Device
                        - value == 255: non-existing Device index
 */
#ifdef                 EvtUSBH_HID_GetDevice
  __STATIC_INLINE void EvrUSBH_HID_GetDevice(  uint8_t instance,   uint8_t device) {
    EventRecord2(      EvtUSBH_HID_GetDevice, TO_BYTE0(instance), TO_BYTE0(device));
  }
#else
  #define              EvrUSBH_HID_GetDevice(...)
#endif


/**
  \brief  Event on \ref USBH_HID_GetStatus finished (API)
  \param  instance     instance of HID device
  \param  status       device status \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_GetStatus
  __STATIC_INLINE void EvrUSBH_HID_GetStatus(  uint8_t instance,    uint8_t status) {
    EventRecord2(      EvtUSBH_HID_GetStatus, TO_BYTE0(instance), (uint32_t)status);
  }
#else
  #define              EvrUSBH_HID_GetStatus(...)
#endif


/**
  \brief  Event on \ref USBH_HID_Read start (API)
  \param  instance     instance of HID device
  \param  len          number of bytes to read
 */
#ifdef                 EvtUSBH_HID_Read
  __STATIC_INLINE void EvrUSBH_HID_Read(  uint8_t instance,    int32_t len) {
    EventRecord2(      EvtUSBH_HID_Read, TO_BYTE0(instance), (uint32_t)len);
  }
#else
  #define              EvrUSBH_HID_Read(...)
#endif


/**
  \brief  Event on \ref USBH_HID_Read failed (Error)
  \param  instance     instance of HID device
  \param  len          number of bytes to read
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_ReadFailed
  __STATIC_INLINE void EvrUSBH_HID_ReadFailed(  uint8_t instance,                      int32_t len, usbStatus error) {
    EventRecord2(      EvtUSBH_HID_ReadFailed, TO_BYTE0(instance) | TO_BYTE1(error), (uint32_t)len);
  }
#else
  #define              EvrUSBH_HID_ReadFailed(...)
#endif


/**
  \brief  Event on \ref USBH_HID_Read finished successfully (Detail)
  \param  instance     instance of HID device
  \param  len          number of bytes to read
  \param  result       number of bytes read
 */
#ifdef                 EvtUSBH_HID_ReadDone
  __STATIC_INLINE void EvrUSBH_HID_ReadDone(  uint8_t instance,    int32_t len,   int32_t result) {
    EventRecord4(      EvtUSBH_HID_ReadDone, TO_BYTE0(instance), (uint32_t)len, (uint32_t)result, 0U);
  }
#else
  #define              EvrUSBH_HID_ReadDone(...)
#endif


/**
  \brief  Event on \ref USBH_HID_Write start (API)
  \param  instance     instance of HID device
  \param  len          number of bytes to write
 */
#ifdef                 EvtUSBH_HID_Write
  __STATIC_INLINE void EvrUSBH_HID_Write(  uint8_t instance,    int32_t len) {
    EventRecord2(      EvtUSBH_HID_Write, TO_BYTE0(instance), (uint32_t)len);
  }
#else
  #define              EvrUSBH_HID_Write(...)
#endif


/**
  \brief  Event on \ref USBH_HID_Write failed (Error)
  \param  instance     instance of HID device
  \param  len          number of bytes to write
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_WriteFailed
  __STATIC_INLINE void EvrUSBH_HID_WriteFailed(  uint8_t instance,                      int32_t len, usbStatus error) {
    EventRecord2(      EvtUSBH_HID_WriteFailed, TO_BYTE0(instance) | TO_BYTE1(error), (uint32_t)len);
  }
#else
  #define              EvrUSBH_HID_WriteFailed(...)
#endif


/**
  \brief  Event on \ref USBH_HID_Write finished successfully (Detail)
  \param  instance     instance of HID device
  \param  len          number of bytes to write
  \param  result       number of bytes accepted for writing
 */
#ifdef                 EvtUSBH_HID_WriteDone
  __STATIC_INLINE void EvrUSBH_HID_WriteDone(  uint8_t instance,    int32_t len,   int32_t result) {
    EventRecord4(      EvtUSBH_HID_WriteDone, TO_BYTE0(instance), (uint32_t)len, (uint32_t)result, 0U);
  }
#else
  #define              EvrUSBH_HID_WriteDone(...)
#endif


/**
  \brief  Event on \ref USBH_HID_GetKeyboardKey start (API)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_GetKeyboardKey
  __STATIC_INLINE void EvrUSBH_HID_GetKeyboardKey(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_GetKeyboardKey, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_GetKeyboardKey(...)
#endif


/**
  \brief  Event on \ref USBH_HID_GetKeyboardKey finished successfully (Detail)
  \param  instance     instance of HID device
  \param  ch           value of first received unread character
 */
#ifdef                 EvtUSBH_HID_GetKeyboardKeyDone
  __STATIC_INLINE void EvrUSBH_HID_GetKeyboardKeyDone(  uint8_t instance,    int32_t ch) {
    EventRecord2(      EvtUSBH_HID_GetKeyboardKeyDone, TO_BYTE0(instance), (uint32_t)ch);
  }
#else
  #define              EvrUSBH_HID_GetKeyboardKeyDone(...)
#endif


/**
  \brief  Event on \ref USBH_HID_GetMouseState start (API)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_GetMouseState
  __STATIC_INLINE void EvrUSBH_HID_GetMouseState(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_GetMouseState, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_GetMouseState(...)
#endif


/**
  \brief  Event on \ref USBH_HID_GetMouseState failed (Error)
  \param  instance     instance of HID device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_GetMouseStateFailed
  __STATIC_INLINE void EvrUSBH_HID_GetMouseStateFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_HID_GetMouseStateFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_HID_GetMouseStateFailed(...)
#endif


/**
  \brief  Event on \ref USBH_HID_GetMouseState finished successfully (Detail)
  \param  instance     instance of HID device
  \param  state        mouse state \ref usbHID_MouseState structure
 */
#ifdef                 EvtUSBH_HID_GetMouseStateDone
  __STATIC_INLINE void EvrUSBH_HID_GetMouseStateDone(  uint8_t instance,   usbHID_MouseState state) {
    EventRecord4(      EvtUSBH_HID_GetMouseStateDone, TO_BYTE0(instance), (uint32_t)state.button, ((uint32_t)state.x << 16) | (uint32_t)state.y, 0U);
  }
#else
  #define              EvrUSBH_HID_GetMouseStateDone(...)
#endif


/**
  \brief  Event on \ref USBH_HID_ParseReportDescriptor user callback function start (API)
  \param  instance     instance of HID device
  \param  len          length of HID report descriptor
 */
#ifdef                 EvtUSBH_HID_OnParseReportDescriptor
  __STATIC_INLINE void EvrUSBH_HID_OnParseReportDescriptor(  uint8_t instance, uint32_t len) {
    EventRecord2(      EvtUSBH_HID_OnParseReportDescriptor, TO_BYTE0(instance),         len);
  }
#else
  #define              EvrUSBH_HID_OnParseReportDescriptor(...)
#endif


/**
  \brief  Event on \ref USBH_HID_DataReceived user callback function start (API)
  \param  instance     instance of HID device
  \param  len          length of received data
 */
#ifdef                 EvtUSBH_HID_OnDataReceived
  __STATIC_INLINE void EvrUSBH_HID_OnDataReceived(  uint8_t instance, uint32_t len) {
    EventRecord2(      EvtUSBH_HID_OnDataReceived, TO_BYTE0(instance),         len);
  }
#else
  #define              EvrUSBH_HID_OnDataReceived(...)
#endif


/**
  \brief  Event on internal operation Get Report start (Operation)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_GetReport
  __STATIC_INLINE void EvrUSBH_HID_GetReport(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_GetReport, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_GetReport(...)
#endif


/**
  \brief  Event on internal operation Get Report failed (Error)
  \param  instance     instance of HID device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_GetReportFailed
  __STATIC_INLINE void EvrUSBH_HID_GetReportFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_HID_GetReportFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_HID_GetReportFailed(...)
#endif


/**
  \brief  Event on internal operation Set Report start (Operation)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_SetReport
  __STATIC_INLINE void EvrUSBH_HID_SetReport(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_SetReport, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_SetReport(...)
#endif


/**
  \brief  Event on internal operation Set Report failed (Error)
  \param  instance     instance of HID device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_SetReportFailed
  __STATIC_INLINE void EvrUSBH_HID_SetReportFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_HID_SetReportFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_HID_SetReportFailed(...)
#endif


/**
  \brief  Event on internal operation Get Idle start (Operation)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_GetIdle
  __STATIC_INLINE void EvrUSBH_HID_GetIdle(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_GetIdle, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_GetIdle(...)
#endif


/**
  \brief  Event on internal operation Get Idle failed (Error)
  \param  instance     instance of HID device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_GetIdleFailed
  __STATIC_INLINE void EvrUSBH_HID_GetIdleFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_HID_GetIdleFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_HID_GetIdleFailed(...)
#endif


/**
  \brief  Event on internal operation Set Idle start (Operation)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_SetIdle
  __STATIC_INLINE void EvrUSBH_HID_SetIdle(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_SetIdle, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_SetIdle(...)
#endif


/**
  \brief  Event on internal operation Set Idle failed (Error)
  \param  instance     instance of HID device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_SetIdleFailed
  __STATIC_INLINE void EvrUSBH_HID_SetIdleFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_HID_SetIdleFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_HID_SetIdleFailed(...)
#endif


/**
  \brief  Event on internal operation Get Protocol start (Operation)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_GetProtocol
  __STATIC_INLINE void EvrUSBH_HID_GetProtocol(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_GetProtocol, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_GetProtocol(...)
#endif


/**
  \brief  Event on internal operation Get Protocol failed (Error)
  \param  instance     instance of HID device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_GetProtocolFailed
  __STATIC_INLINE void EvrUSBH_HID_GetProtocolFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_HID_GetProtocolFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_HID_GetProtocolFailed(...)
#endif


/**
  \brief  Event on internal operation Set Protocol start (Operation)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_SetProtocol
  __STATIC_INLINE void EvrUSBH_HID_SetProtocol(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_SetProtocol, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_SetProtocol(...)
#endif


/**
  \brief  Event on internal operation Set Protocol failed (Error)
  \param  instance     instance of HID device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_SetProtocolFailed
  __STATIC_INLINE void EvrUSBH_HID_SetProtocolFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_HID_SetProtocolFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_HID_SetProtocolFailed(...)
#endif


/**
  \brief  Event on internal operation Report Out start (Operation)
  \param  instance     instance of HID device
 */
#ifdef                 EvtUSBH_HID_ReportOut
  __STATIC_INLINE void EvrUSBH_HID_ReportOut(  uint8_t instance) {
    EventRecord2(      EvtUSBH_HID_ReportOut, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_HID_ReportOut(...)
#endif


/**
  \brief  Event on internal operation Report Out failed (Error)
  \param  instance     instance of HID device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_HID_ReportOutFailed
  __STATIC_INLINE void EvrUSBH_HID_ReportOutFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_HID_ReportOutFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_HID_ReportOutFailed(...)
#endif


// USB Host MSC component event record functions -------------------------------

/**
  \brief  Event on internal operation MSC device configuration start (Operation)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_Configure
  __STATIC_INLINE void EvrUSBH_MSC_Configure(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_Configure, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_Configure(...)
#endif


/**
  \brief  Event on internal operation MSC device configuration failed (Error)
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_ConfigureFailed
  __STATIC_INLINE void EvrUSBH_MSC_ConfigureFailed( usbStatus error) {
    EventRecord2(      EvtUSBH_MSC_ConfigureFailed, (uint32_t)error, 0U);
  }
#else
  #define              EvrUSBH_MSC_ConfigureFailed(...)
#endif


/**
  \brief  Event on internal operation MSC device unconfigure start (Operation)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_Unconfigure
  __STATIC_INLINE void EvrUSBH_MSC_Unconfigure(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_Unconfigure, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_Unconfigure(...)
#endif


/**
  \brief  Event on internal operation MSC device uninitialization failed (Error)
  \param  instance     instance of MSC device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_UnconfigureFailed
  __STATIC_INLINE void EvrUSBH_MSC_UnconfigureFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_MSC_UnconfigureFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_UnconfigureFailed(...)
#endif


/**
  \brief  Event on internal operation MSC device initialization start (Operation)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_Initialize
  __STATIC_INLINE void EvrUSBH_MSC_Initialize(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_Initialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_Initialize(...)
#endif


/**
  \brief  Event on internal operation MSC device initialization failed (Error)
  \param  instance     instance of MSC device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_InitializeFailed
  __STATIC_INLINE void EvrUSBH_MSC_InitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_MSC_InitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_InitializeFailed(...)
#endif

/**
  \brief  Event on \ref USBH_MSC_Initialize user callback function start (API)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_OnInitialize
  __STATIC_INLINE void EvrUSBH_MSC_OnInitialize(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_OnInitialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_OnInitialize(...)
#endif


/**
  \brief  Event on internal operation MSC device uninitialization start (Operation)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_Uninitialize
  __STATIC_INLINE void EvrUSBH_MSC_Uninitialize(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_Uninitialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_Uninitialize(...)
#endif


/**
  \brief  Event on internal operation MSC device uninitialization failed (Error)
  \param  instance     instance of MSC device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_UninitializeFailed
  __STATIC_INLINE void EvrUSBH_MSC_UninitializeFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_MSC_UninitializeFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_UninitializeFailed(...)
#endif


/**
  \brief  Event on \ref USBH_MSC_Uninitialize user callback function start (API)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_OnUninitialize
  __STATIC_INLINE void EvrUSBH_MSC_OnUninitialize(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_OnUninitialize, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_OnUninitialize(...)
#endif


/**
  \brief  Event on \ref USBH_MSC_GetDevice finished (API)
  \param  instance     instance of MSC device
  \param  device       device index
                        - value <= 127: index of Device
                        - value == 255: non-existing Device index
 */
#ifdef                 EvtUSBH_MSC_GetDevice
  __STATIC_INLINE void EvrUSBH_MSC_GetDevice(  uint8_t instance,   uint8_t device) {
    EventRecord2(      EvtUSBH_MSC_GetDevice, TO_BYTE0(instance), TO_BYTE0(device));
  }
#else
  #define              EvrUSBH_MSC_GetDevice(...)
#endif


/**
  \brief  Event on \ref USBH_MSC_GetStatus finished (API)
  \param  instance     instance of MSC device
  \param  status       device status \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_GetStatus
  __STATIC_INLINE void EvrUSBH_MSC_GetStatus(  uint8_t instance,    uint8_t status) {
    EventRecord2(      EvtUSBH_MSC_GetStatus, TO_BYTE0(instance), (uint32_t)status);
  }
#else
  #define              EvrUSBH_MSC_GetStatus(...)
#endif


/**
  \brief  Event on \ref USBH_MSC_Read start (API)
  \param  instance     instance of MSC device
  \param  lba          logical block address of first block to read
  \param  cnt          number of contiguous blocks to read
 */
#ifdef                 EvtUSBH_MSC_Read
  __STATIC_INLINE void EvrUSBH_MSC_Read(  uint8_t instance,  uint32_t lba, uint32_t cnt) {
    EventRecord4(      EvtUSBH_MSC_Read, TO_BYTE0(instance),          lba,          cnt, 0U);
  }
#else
  #define              EvrUSBH_MSC_Read(...)
#endif


/**
  \brief  Event on \ref USBH_MSC_Read failed (Error)
  \param  instance     instance of MSC device
  \param  lba          logical block address of first block to read
  \param  cnt          number of contiguous blocks to read
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_ReadFailed
  __STATIC_INLINE void EvrUSBH_MSC_ReadFailed(  uint8_t instance,  uint32_t lba, uint32_t cnt, usbStatus error) {
    EventRecord4(      EvtUSBH_MSC_ReadFailed, TO_BYTE0(instance),          lba,          cnt, (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_ReadFailed(...)
#endif


/**
  \brief  Event on \ref USBH_MSC_Read finished successfully (Detail)
  \param  instance     instance of MSC device
  \param  lba          logical block address of first block to read
  \param  cnt          number of contiguous blocks to read
 */
#ifdef                 EvtUSBH_MSC_ReadDone
  __STATIC_INLINE void EvrUSBH_MSC_ReadDone(  uint8_t instance,  uint32_t lba, uint32_t cnt) {
    EventRecord4(      EvtUSBH_MSC_ReadDone, TO_BYTE0(instance),          lba,          cnt, 0U);
  }
#else
  #define              EvrUSBH_MSC_ReadDone(...)
#endif


/**
  \brief  Event on \ref USBH_MSC_Write start (API)
  \param  instance     instance of MSC device
  \param  lba          logical block address of first block to write
  \param  cnt          number of contiguous blocks to write
 */
#ifdef                 EvtUSBH_MSC_Write
  __STATIC_INLINE void EvrUSBH_MSC_Write(  uint8_t instance,  uint32_t lba, uint32_t cnt) {
    EventRecord4(      EvtUSBH_MSC_Write, TO_BYTE0(instance),          lba,          cnt, 0U);
  }
#else
  #define              EvrUSBH_MSC_Write(...)
#endif


/**
  \brief  Event on \ref USBH_MSC_Write failed (Error)
  \param  instance     instance of MSC device
  \param  lba          logical block address of first block to write
  \param  cnt          number of contiguous blocks to write
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_WriteFailed
  __STATIC_INLINE void EvrUSBH_MSC_WriteFailed(  uint8_t instance,  uint32_t lba, uint32_t cnt, usbStatus error) {
    EventRecord4(      EvtUSBH_MSC_WriteFailed, TO_BYTE0(instance),          lba,          cnt, (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_WriteFailed(...)
#endif


/**
  \brief  Event on \ref USBH_MSC_Write finished successfully (Detail)
  \param  instance     instance of MSC device
  \param  lba          logical block address of first block to write
  \param  cnt          number of contiguous blocks to write
 */
#ifdef                 EvtUSBH_MSC_WriteDone
  __STATIC_INLINE void EvrUSBH_MSC_WriteDone(  uint8_t instance,  uint32_t lba, uint32_t cnt) {
    EventRecord4(      EvtUSBH_MSC_WriteDone, TO_BYTE0(instance),          lba,          cnt, 0U);
  }
#else
  #define              EvrUSBH_MSC_WriteDone(...)
#endif


/**
  \brief  Event on \ref USBH_MSC_ReadCapacity start (API)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_ReadCapacity
  __STATIC_INLINE void EvrUSBH_MSC_ReadCapacity(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_ReadCapacity, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_ReadCapacity(...)
#endif


/**
  \brief  Event on \ref USBH_MSC_ReadCapacity failed (Error)
  \param  instance     instance of MSC device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_ReadCapacityFailed
  __STATIC_INLINE void EvrUSBH_MSC_ReadCapacityFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_MSC_ReadCapacityFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_ReadCapacityFailed(...)
#endif


/**
  \brief  Event on \ref USBH_MSC_ReadCapacity finished successfully (Detail)
  \param  instance     instance of MSC device
  \param  block_count  total number of blocks available
  \param  block_size   block size
 */
#ifdef                 EvtUSBH_MSC_ReadCapacityDone
  __STATIC_INLINE void EvrUSBH_MSC_ReadCapacityDone(  uint8_t instance,  uint32_t block_count, uint32_t block_size) {
    EventRecord4(      EvtUSBH_MSC_ReadCapacityDone, TO_BYTE0(instance),          block_count,          block_size, 0U);
  }
#else
  #define              EvrUSBH_MSC_ReadCapacityDone(...)
#endif


/**
  \brief  Event on internal operation Bulk-Only Mass Storage Reset start (Operation)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_BomReset
  __STATIC_INLINE void EvrUSBH_MSC_BomReset(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_BomReset, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_BomReset(...)
#endif


/**
  \brief  Event on internal operation Bulk-Only Mass Storage Reset failed (Error)
  \param  instance     instance of MSC device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_BomResetFailed
  __STATIC_INLINE void EvrUSBH_MSC_BomResetFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_MSC_BomResetFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_BomResetFailed(...)
#endif


/**
  \brief  Event on internal operation Get Max LUN start (Operation)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_GetMaxLun
  __STATIC_INLINE void EvrUSBH_MSC_GetMaxLun(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_GetMaxLun, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_GetMaxLun(...)
#endif


/**
  \brief  Event on internal operation Get Max LUN failed (Error)
  \param  instance     instance of MSC device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_GetMaxLunFailed
  __STATIC_INLINE void EvrUSBH_MSC_GetMaxLunFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_MSC_GetMaxLunFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_GetMaxLunFailed(...)
#endif


/**
  \brief  Event on internal operation SCSI Test Unit Ready start (Operation)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_ScsiTestUnitReady
  __STATIC_INLINE void EvrUSBH_MSC_ScsiTestUnitReady(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_ScsiTestUnitReady, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_ScsiTestUnitReady(...)
#endif


/**
  \brief  Event on internal operation SCSI Test Unit Ready failed (Error)
  \param  instance     instance of MSC device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_ScsiTestUnitReadyFailed
  __STATIC_INLINE void EvrUSBH_MSC_ScsiTestUnitReadyFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_MSC_ScsiTestUnitReadyFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_ScsiTestUnitReadyFailed(...)
#endif


/**
  \brief  Event on internal operation SCSI Request Sense start (Operation)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_ScsiRequestSense
  __STATIC_INLINE void EvrUSBH_MSC_ScsiRequestSense(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_ScsiRequestSense, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_ScsiRequestSense(...)
#endif


/**
  \brief  Event on internal operation SCSI Request Sense failed (Error)
  \param  instance     instance of MSC device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_ScsiRequestSenseFailed
  __STATIC_INLINE void EvrUSBH_MSC_ScsiRequestSenseFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_MSC_ScsiRequestSenseFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_ScsiRequestSenseFailed(...)
#endif


/**
  \brief  Event on internal operation SCSI Inquiry start (Operation)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_ScsiInquiry
  __STATIC_INLINE void EvrUSBH_MSC_ScsiInquiry(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_ScsiInquiry, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_ScsiInquiry(...)
#endif


/**
  \brief  Event on internal operation SCSI Inquiry failed (Error)
  \param  instance     instance of MSC device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_ScsiInquiryFailed
  __STATIC_INLINE void EvrUSBH_MSC_ScsiInquiryFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_MSC_ScsiInquiryFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_ScsiInquiryFailed(...)
#endif


/**
  \brief  Event on internal operation SCSI Read Format Capacities start (Operation)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_ScsiReadFormatCapacities
  __STATIC_INLINE void EvrUSBH_MSC_ScsiReadFormatCapacities(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_ScsiReadFormatCapacities, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_ScsiReadFormatCapacities(...)
#endif


/**
  \brief  Event on internal operation SCSI Read Format Capacities failed (Error)
  \param  instance     instance of MSC device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_ScsiReadFormatCapacitiesFailed
  __STATIC_INLINE void EvrUSBH_MSC_ScsiReadFormatCapacitiesFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_MSC_ScsiReadFormatCapacitiesFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_ScsiReadFormatCapacitiesFailed(...)
#endif


/**
  \brief  Event on internal operation SCSI Read Capacity start (Operation)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_ScsiReadCapacity
  __STATIC_INLINE void EvrUSBH_MSC_ScsiReadCapacity(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_ScsiReadCapacity, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_ScsiReadCapacity(...)
#endif


/**
  \brief  Event on internal operation SCSI Read Capacity failed (Error)
  \param  instance     instance of MSC device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_ScsiReadCapacityFailed
  __STATIC_INLINE void EvrUSBH_MSC_ScsiReadCapacityFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_MSC_ScsiReadCapacityFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_ScsiReadCapacityFailed(...)
#endif


/**
  \brief  Event on internal operation SCSI Read10 start (Operation)
  \param  instance     instance of MSC device
  \param  block_addr   address of first block to be read
  \param  block_num    number of blocks to be read
 */
#ifdef                 EvtUSBH_MSC_ScsiRead10
  __STATIC_INLINE void EvrUSBH_MSC_ScsiRead10(  uint8_t instance,  uint32_t block_addr, uint32_t block_num) {
    EventRecord4(      EvtUSBH_MSC_ScsiRead10, TO_BYTE0(instance),          block_addr,          block_num, 0U);
  }
#else
  #define              EvrUSBH_MSC_ScsiRead10(...)
#endif


/**
  \brief  Event on internal operation SCSI Read10 failed (Error)
  \param  instance     instance of MSC device
  \param  block_addr   address of first block to be read
  \param  block_num    number of blocks to be read
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_ScsiRead10Failed
  __STATIC_INLINE void EvrUSBH_MSC_ScsiRead10Failed(  uint8_t instance,  uint32_t block_addr, uint32_t block_num, usbStatus error) {
    EventRecord4(      EvtUSBH_MSC_ScsiRead10Failed, TO_BYTE0(instance),          block_addr,          block_num, (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_ScsiRead10Failed(...)
#endif


/**
  \brief  Event on internal operation SCSI Write10 start (Operation)
  \param  instance     instance of MSC device
  \param  block_addr   address of first block to be written
  \param  block_num    number of blocks to be written
 */
#ifdef                 EvtUSBH_MSC_ScsiWrite10
  __STATIC_INLINE void EvrUSBH_MSC_ScsiWrite10(  uint8_t instance,  uint32_t block_addr, uint32_t block_num) {
    EventRecord4(      EvtUSBH_MSC_ScsiWrite10, TO_BYTE0(instance),          block_addr,          block_num, 0U);
  }
#else
  #define              EvrUSBH_MSC_ScsiWrite10(...)
#endif


/**
  \brief  Event on internal operation SCSI Write10 failed (Error)
  \param  instance     instance of MSC device
  \param  block_addr   address of first block to be written
  \param  block_num    number of blocks to be written
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_ScsiWrite10Failed
  __STATIC_INLINE void EvrUSBH_MSC_ScsiWrite10Failed(  uint8_t instance,  uint32_t block_addr, uint32_t block_num, usbStatus error) {
    EventRecord4(      EvtUSBH_MSC_ScsiWrite10Failed, TO_BYTE0(instance),          block_addr,          block_num, (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_ScsiWrite10Failed(...)
#endif


/**
  \brief  Event on internal operation recover start (Operation)
  \param  instance     instance of MSC device
 */
#ifdef                 EvtUSBH_MSC_Recover
  __STATIC_INLINE void EvrUSBH_MSC_Recover(  uint8_t instance) {
    EventRecord2(      EvtUSBH_MSC_Recover, TO_BYTE0(instance), 0U);
  }
#else
  #define              EvrUSBH_MSC_Recover(...)
#endif


/**
  \brief  Event on internal operation recover failed (Error)
  \param  instance     instance of MSC device
  \param  error        error code \ref usbStatus
 */
#ifdef                 EvtUSBH_MSC_RecoverFailed
  __STATIC_INLINE void EvrUSBH_MSC_RecoverFailed(  uint8_t instance,  usbStatus error) {
    EventRecord2(      EvtUSBH_MSC_RecoverFailed, TO_BYTE0(instance), (uint32_t)error);
  }
#else
  #define              EvrUSBH_MSC_RecoverFailed(...)
#endif

#endif // __USBH_EVR_H__
