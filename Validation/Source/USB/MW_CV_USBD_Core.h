/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Device
 * Copyright (c) 2018-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBD_Core.h
 * Purpose: MDK Middleware - Component Validation - USB Device - Core -
 *          tests header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_USBD_CORE_H_
#define MW_CV_USBD_CORE_H_

extern void MW_CV_USBD_GetVersion                            (void);
extern void MW_CV_USBD_Initialize                            (void);
extern void MW_CV_USBD_Connect                               (void);
extern void MW_CV_USBD_Configured                            (void);
extern void MW_CV_USBD_GetState                              (void);
extern void MW_CV_USBD_SetSerialNumber                       (void);
extern void MW_CV_USBD_Disconnect                            (void);
extern void MW_CV_USBD_Uninitialize                          (void);

extern void MW_CV_USBD_Device0_Initialize                    (void);
extern void MW_CV_USBD_Device0_Uninitialize                  (void);
extern void MW_CV_USBD_Device0_VbusChanged                   (void);
extern void MW_CV_USBD_Device0_Reset                         (void);
extern void MW_CV_USBD_Device0_HighSpeedActivated            (void);
extern void MW_CV_USBD_Device0_Suspended                     (void);
extern void MW_CV_USBD_Device0_Resumed                       (void);
extern void MW_CV_USBD_Device0_ConfigurationChanged          (void);
extern void MW_CV_USBD_Device0_EnableRemoteWakeup            (void);
extern void MW_CV_USBD_Device0_DisableRemoteWakeup           (void);
extern void MW_CV_USBD_Device0_Endpoint0_SetupPacketReceived (void);
extern void MW_CV_USBD_Device0_Endpoint0_SetupPacketProcessed(void);
extern void MW_CV_USBD_Device0_Endpoint0_OutDataReceived     (void);
extern void MW_CV_USBD_Device0_Endpoint0_InDataSent          (void);

#endif // MW_CV_USBD_CORE_H_
