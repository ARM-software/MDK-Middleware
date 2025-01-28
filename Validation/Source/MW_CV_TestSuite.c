/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_TestSuite.c
 * Purpose: MDK Middleware - Component Validation - Tests Suite module
 *----------------------------------------------------------------------------*/

#include "MW_CV_TestSuite.h"

#ifdef   CMSIS_target_header
#include CMSIS_target_header
#endif

#include "MW_CV_Config.h"

#if      (MW_CV_NET != 0)
#include "MW_CV_BSD.h"
#endif

#if      ((MW_CV_USBD != 0) || (MW_CV_USBH != 0))
#include "rl_usb.h"
#endif

#if      (MW_CV_USBD != 0)
#include "MW_CV_USBD_Core.h"
#include "MW_CV_USBD_CDC_ACM.h"
#include "MW_CV_USBD_HID.h"
#include "MW_CV_USBD_MSC.h"
#endif

#if      (MW_CV_USBH != 0)
#include "MW_CV_USBH_Core.h"
#include "MW_CV_USBH_CDC_ACM.h"
#include "MW_CV_USBH_HID.h"
#include "MW_CV_USBH_MSC.h"

#include "MW_CV_USBH_MSC_Performance.h"
#endif

// Check configuration requirements for Network validation

#if    ((MW_CV_NET == 1) && !defined(CMSIS_DRIVER_ETH))
#error Network component validation requires configured Ethernet peripheral on the Board!
#endif

// Check configuration requirements for USB validation

#if    (((MW_CV_USBH == 1) || (MW_CV_USBD == 1)) && (!defined(CMSIS_DRIVER_USBD) || !defined(CMSIS_DRIVER_USBH)))
#error USB Device/Host component validation requires 2 configured USB peripherals on the Board (one as USB Device and the other as USB Host)!
#endif

#if    (((MW_CV_USBD == 1) && (MW_CV_USBH == 1)) || \
       (((MW_CV_USBD == 1) && ((MW_CV_USBD_CORE + MW_CV_USBD_CDC_ACM + MW_CV_USBD_HID + MW_CV_USBD_MSC) > 1)) || \
        ((MW_CV_USBH == 1) && ((MW_CV_USBH_CORE + MW_CV_USBH_CDC_ACM + MW_CV_USBH_HID + MW_CV_USBH_MSC + MW_CV_USBH_MSC_PERFORMANCE) > 1))))
#error Only one USB Device or Host validation can be selected at a time!
#endif

/*-----------------------------------------------------------------------------
 *      List of tests
 *----------------------------------------------------------------------------*/

static TEST_LIST_t test_list[] = {

  /**************************** Network Validation ****************************/
#if (MW_CV_NET != 0)
  TEST_UNIT_DEF ("MDK Middleware: Network"  ,  BSD_GetVersion                  ),
  TEST_CASE_DEF ( BSD_Initialize            , "BSD_Initialize"           , true),
#if (MW_CV_NET_BSD_API != 0)
  /* BSD socket API tests */
  TEST_CASE_DEF ( BSD_SocketCreate          , "BSD_SocketCreate"         , true),
  TEST_CASE_DEF ( BSD_SocketBind            , "BSD_SocketBind"           , true),
  TEST_CASE_DEF ( BSD_SocketListen          , "BSD_SocketListen"         , true),
  TEST_CASE_DEF ( BSD_SocketAccept          , "BSD_SocketAccept"         , true),
  TEST_CASE_DEF ( BSD_SocketAccept_nbio     , "BSD_SocketAccept_nbio"    , true),
  TEST_CASE_DEF ( BSD_SocketConnect         , "BSD_SocketConnect"        , true),
  TEST_CASE_DEF ( BSD_SocketConnect_nbio    , "BSD_SocketConnect_nbio"   , true),
  TEST_CASE_DEF ( BSD_SocketRecv            , "BSD_SocketRecv"           , true),
  TEST_CASE_DEF ( BSD_SocketRecv_nbio       , "BSD_SocketRecv_nbio"      , true),
  TEST_CASE_DEF ( BSD_SocketRecvFrom        , "BSD_SocketRecvFrom"       , true),
  TEST_CASE_DEF ( BSD_SocketRecvFrom_nbio   , "BSD_SocketRecvFrom_nbio"  , true),
  TEST_CASE_DEF ( BSD_SocketRecvMsg         , "BSD_SocketRecvMsg"        , true),
  TEST_CASE_DEF ( BSD_SocketSend            , "BSD_SocketSend"           , true),
  TEST_CASE_DEF ( BSD_SocketSendTo          , "BSD_SocketSendTo"         , true),
  TEST_CASE_DEF ( BSD_SocketSendMsg         , "BSD_SocketSendMsg"        , true),
  TEST_CASE_DEF ( BSD_SocketGetSockName     , "BSD_SocketGetSockName"    , true),
  TEST_CASE_DEF ( BSD_SocketGetPeerName     , "BSD_SocketGetPeerName"    , true),
  TEST_CASE_DEF ( BSD_SocketGetOpt          , "BSD_SocketGetOpt"         , true),
  TEST_CASE_DEF ( BSD_SocketSetOpt          , "BSD_SocketSetOpt"         , true),
  TEST_CASE_DEF ( BSD_SocketClose           , "BSD_SocketClose"          , true),
  TEST_CASE_DEF ( BSD_SocketIoctl           , "BSD_SocketIoctl"          , true),
  TEST_CASE_DEF ( BSD_SocketSelect          , "BSD_SocketSelect"         , true),
  TEST_CASE_DEF ( BSD_GetHostByName         , "BSD_GetHostByName"        , true),
  TEST_CASE_DEF ( BSD_PingEcho              , "BSD_PingEcho"             , true),
#endif
#if (MW_CV_NET_BSD_OPERATION != 0)
  /* BSD socket operation tests */
  TEST_CASE_DEF ( BSD_Transfer_Fixed        , "BSD_Transfer_Fixed"       , true),
  TEST_CASE_DEF ( BSD_Transfer_Incremental  , "BSD_Transfer_Incremental" , true),
  TEST_CASE_DEF ( BSD_Send_Fragmented       , "BSD_Send_Fragmented"      , true),
  TEST_CASE_DEF ( BSD_Recv_Fragmented       , "BSD_Recv_Fragmented"      , true),
  TEST_CASE_DEF ( BSD_Test_Speed            , "BSD_Test_Speed"           , true),
  TEST_CASE_DEF ( BSD_Concurrent_Socket     , "BSD_Concurrent_Socket"    , true),
  TEST_CASE_DEF ( BSD_Downstream_Rate       , "BSD_Downstream_Rate"      , true),
  TEST_CASE_DEF ( BSD_Upstream_Rate         , "BSD_Upstream_Rate"        , true),
#endif
  TEST_CASE_DEF ( BSD_Uninitialize          , "BSD_Uninitialize"         , true),
#endif

  /**************************** USB Validation ********************************/
#if (MW_CV_USBD != 0)
#if (MW_CV_USBD_CORE != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Device - Core"                ,       USBD_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBD_GetVersion                             , "API: USBD_GetVersion"                             , true),
  TEST_CASE_DEF ( MW_CV_USBD_Initialize                             , "API: USBD_Initialize"                             , true),
  TEST_CASE_DEF ( MW_CV_USBD_Connect                                , "API: USBD_Connect"                                , true),
  TEST_CASE_DEF ( MW_CV_USBD_Configured                             , "API: USBD_Configured"                             , true),
  TEST_CASE_DEF ( MW_CV_USBD_GetState                               , "API: USBD_GetState"                               , true),
  TEST_CASE_DEF ( MW_CV_USBD_SetSerialNumber                        , "API: USBD_SetSerialNumber"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_Disconnect                             , "API: USBD_Disconnect"                             , true),
  TEST_CASE_DEF ( MW_CV_USBD_Uninitialize                           , "API: USBD_Uninitialize"                           , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_Initialize                     , "CB : USBD_Device0_Initialize"                     , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_VbusChanged                    , "CB : USBD_Device0_VbusChanged"                    , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_Reset                          , "CB : USBD_Device0_Reset"                          , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_HighSpeedActivated             , "CB : USBD_Device0_HighSpeedActivated"             , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_Suspended                      , "CB : USBD_Device0_Suspended"                      , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_Resumed                        , "CB : USBD_Device0_Resumed"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_ConfigurationChanged           , "CB : USBD_Device0_ConfigurationChanged"           , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_EnableRemoteWakeup             , "CB : USBD_Device0_EnableRemoteWakeup"             , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_DisableRemoteWakeup            , "CB : USBD_Device0_DisableRemoteWakeup"            , true),
//TEST_CASE_DEF ( MW_CV_USBD_Device0_Endpoint0_SetupPacketReceived  , "CB : USBD_Device0_Endpoint0_SetupPacketReceived"  , true),
//TEST_CASE_DEF ( MW_CV_USBD_Device0_Endpoint0_SetupPacketProcessed , "CB : USBD_Device0_Endpoint0_SetupPacketProcessed" , true),
//TEST_CASE_DEF ( MW_CV_USBD_Device0_Endpoint0_OutDataReceived      , "CB : USBD_Device0_Endpoint0_OutDataReceived"      , true),
//TEST_CASE_DEF ( MW_CV_USBD_Device0_Endpoint0_InDataSent           , "CB : USBD_Device0_Endpoint0_InDataSent"           , true),
  TEST_CASE_DEF ( MW_CV_USBD_Device0_Uninitialize                   , "CB : USBD_Device0_Uninitialize"                   , true),
#endif
#if (MW_CV_USBD_CDC_ACM != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Device - CDC ACM"             ,       USBD_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_Initialize                    , "CB : USBD_CDC0_ACM_Initialize"                    , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_Reset                         , "CB : USBD_CDC0_ACM_Reset"                         , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_SetLineCoding                 , "CB : USBD_CDC0_ACM_SetLineCoding"                 , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_GetLineCoding                 , "CB : USBD_CDC0_ACM_GetLineCoding"                 , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_SetControlLineState           , "CB : USBD_CDC0_ACM_SetControlLineState"           , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_DataReceived                  , "CB : USBD_CDC0_ACM_DataReceived"                  , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_DataSent                      , "CB : USBD_CDC0_ACM_DataSent"                      , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC_ACM_GetChar                        , "API: USBD_CDC_ACM_GetChar"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC_ACM_PutChar                        , "API: USBD_CDC_ACM_PutChar"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC_ACM_WriteData                      , "API: USBD_CDC_ACM_WriteData"                      , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC_ACM_DataAvailable                  , "API: USBD_CDC_ACM_DataAvailable"                  , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC_ACM_Notify                         , "API: USBD_CDC_ACM_Notify"                         , true),
  TEST_CASE_DEF ( MW_CV_USBD_CDC0_ACM_Uninitialize                  , "CB : USBD_CDC0_ACM_Uninitialize"                  , true),
#endif
#if (MW_CV_USBD_HID != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Device - HID"                 ,       USBD_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBD_HID0_Initialize                        , "CB : USBD_HID0_Initialize"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_HID0_GetReport                         , "CB : USBD_HID0_GetReport"                         , true),
  TEST_CASE_DEF ( MW_CV_USBD_HID0_SetReport                         , "CB : USBD_HID0_SetReport"                         , true),
  TEST_CASE_DEF ( MW_CV_USBD_HID_GetReportTrigger                   , "API: USBD_HID_GetReportTrigger"                   , true),
  TEST_CASE_DEF ( MW_CV_USBD_HID0_Uninitialize                      , "CB : USBD_HID0_Uninitialize"                      , true),
#endif
#if (MW_CV_USBD_MSC != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Device - MSC"                 ,       USBD_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBD_MSC0_Initialize                        , "CB : USBD_MSC0_Initialize"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_MSC0_GetCacheInfo                      , "CB : USBD_MSC0_GetCacheInfo"                      , true),
  TEST_CASE_DEF ( MW_CV_USBD_MSC0_GetMediaCapacity                  , "CB : USBD_MSC0_GetMediaCapacity"                  , true),
  TEST_CASE_DEF ( MW_CV_USBD_MSC0_Write_Read                        , "CB : USBD_MSC0_Write/Read"                        , true),
  TEST_CASE_DEF ( MW_CV_USBD_MSC0_CheckMedia                        , "CB : USBD_MSC0_CheckMedia"                        , true),
//TEST_CASE_DEF ( MW_CV_USBD_MSC0_GetMaxLUN                         , "CB : USBD_MSC0_GetMaxLUN"                         , true),
//TEST_CASE_DEF ( MW_CV_USBD_MSC0_LUN_GetMediaCapacity              , "CB : USBD_MSC0_LUN_GetMediaCapacity"              , true),
//TEST_CASE_DEF ( MW_CV_USBD_MSC0_LUN_Read                          , "CB : USBD_MSC0_LUN_Read"                          , true),
//TEST_CASE_DEF ( MW_CV_USBD_MSC0_LUN_Write                         , "CB : USBD_MSC0_LUN_Write"                         , true),
//TEST_CASE_DEF ( MW_CV_USBD_MSC0_LUN_CheckMedia                    , "CB : USBD_MSC0_LUN_CheckMedia"                    , true),
  TEST_CASE_DEF ( MW_CV_USBD_MSC0_Uninitialize                      , "CB : USBD_MSC0_Uninitialize"                      , true),
#endif
#endif

#if (MW_CV_USBH      != 0)
#if (MW_CV_USBH_CORE != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Host - Core"                  ,       USBH_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBH_GetVersion                             , "API: USBH_GetVersion"                             , true),
  TEST_CASE_DEF ( MW_CV_USBH_Initialize                             , "API: USBH_Initialize"                             , true),
  TEST_CASE_DEF ( MW_CV_USBH_Port_Suspend                           , "API: USBH_Port_Suspend"                           , true),
  TEST_CASE_DEF ( MW_CV_USBH_Port_Resume                            , "API: USBH_Port_Resume"                            , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetController                   , "API: USBH_Device_GetController"                   , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetPort                         , "API: USBH_Device_GetPort"                         , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetStatus                       , "API: USBH_Device_GetStatus"                       , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetSpeed                        , "API: USBH_Device_GetSpeed"                        , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetAddress                      , "API: USBH_Device_GetAddress"                      , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetVID                          , "API: USBH_Device_GetVID"                          , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetPID                          , "API: USBH_Device_GetPID"                          , true),
  TEST_CASE_DEF ( MW_CV_USBH_Device_GetStringDescriptor             , "API: USBH_Device_GetStringDescriptor"             , true),
  TEST_CASE_DEF ( MW_CV_USBH_Uninitialize                           , "API: USBH_Uninitialize"                           , true),
  TEST_CASE_DEF ( MW_CV_USBH_Notify                                 , "CB : USBH_Notify"                                 , true),
#endif
#if (MW_CV_USBH_CDC_ACM != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Host - CDC ACM"               ,       USBH_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_Initialize                     , "CB : USBH_CDC_ACM_Initialize"                     , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_GetDevice                      , "API: USBH_CDC_ACM_GetDevice"                      , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_GetStatus                      , "API: USBH_CDC_ACM_GetStatus"                      , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_Send_Receive                   , "API: USBH_CDC_ACM_Send/Receive"                   , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_SetGetLineCoding               , "API: USBH_CDC_ACM_Set/GetLineCoding"              , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_SetControlLineState            , "API: USBH_CDC_ACM_SetControlLineState"            , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_Notify                         , "API: USBH_CDC_ACM_Notify"                         , true),
//TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_SendBreak                      , "API: USBH_CDC_ACM_SendBreak"                      , true),
  TEST_CASE_DEF ( MW_CV_USBH_CDC_ACM_Uninitialize                   , "CB : USBH_CDC_ACM_Uninitialize"                   , true),
#endif
#if (MW_CV_USBH_HID != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Host - HID"                   ,       USBH_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBH_HID_Initialize                         , "CB : USBH_HID_Initialize"                         , true),
  TEST_CASE_DEF ( MW_CV_USBH_HID_GetDevice                          , "API: USBH_HID_GetDevice"                          , true),
  TEST_CASE_DEF ( MW_CV_USBH_HID_GetStatus                          , "API: USBH_HID_GetStatus"                          , true),
  TEST_CASE_DEF ( MW_CV_USBH_HID_Write_Read                         , "API: USBH_HID_Write/Read"                         , true),
//TEST_CASE_DEF ( MW_CV_USBH_HID_GetKeyboardKey                     , "API: USBH_HID_GetKeyboardKey"                     , true),
//TEST_CASE_DEF ( MW_CV_USBH_HID_GetMouseState                      , "API: USBH_HID_GetMouseState"                      , true),
  TEST_CASE_DEF ( MW_CV_USBH_HID_ParseReportDescriptor              , "CB : USBH_HID_ParseReportDescriptor"              , true),
  TEST_CASE_DEF ( MW_CV_USBH_HID_DataReceived                       , "CB : USBH_HID_DataReceived"                       , true),
  TEST_CASE_DEF ( MW_CV_USBH_HID_Uninitialize                       , "CB : USBH_HID_Uninitialize"                       , true),
#endif
#if (MW_CV_USBH_MSC != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Host - MSC"                   ,       USBH_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_Initialize                         , "CB : USBH_MSC_Initialize"                         , true),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_GetDevice                          , "API: USBH_MSC_GetDevice"                          , true),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_GetStatus                          , "API: USBH_MSC_GetStatus"                          , true),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_Write_Read                         , "API: USBH_MSC_Write/Read"                         , true),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_ReadCapacity                       , "API: USBH_MSC_ReadCapacity"                       , true),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_Uninitialize                       , "CB : USBH_MSC_Uninitialize"                       , true),
#endif
#if (MW_CV_USBH_MSC_PERFORMANCE != 0)
  TEST_UNIT_DEF ("MDK Middleware: USB Host - Performance"           ,       USBH_GetVersion                                    ),
  TEST_CASE_DEF ( MW_CV_USBH_MSC_Performance                        , "MSC Performance: Write/Read performance"          , true),
#endif
#endif

  // Dummy definition to provide at least one entry in case no tests are defined, not to be executed
  TEST_UNIT_DEF ("No tests selected!"                               ,       NULL                                               )
};

/*-----------------------------------------------------------------------------
 *      Test suite description
 *----------------------------------------------------------------------------*/

TEST_SUITE_t test_suite = {
  "MDK Middleware Component Test Suite v1.0.0",
  __DATE__,
  __TIME__,
  __FILE__,
  NULL,
  test_list,
 (sizeof(test_list)/sizeof((test_list)[0])) - 1U,
};
