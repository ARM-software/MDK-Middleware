/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Host
 * Copyright (c) 2018-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBH_Core.h
 * Purpose: MDK Middleware - Component Validation - USB Host - Core -
 *          tests header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_USBH_CORE_H_
#define MW_CV_USBH_CORE_H_

extern void MW_CV_USBH_GetVersion                 (void);
extern void MW_CV_USBH_Initialize                 (void);
extern void MW_CV_USBH_Port_Suspend               (void);
extern void MW_CV_USBH_Port_Resume                (void);
extern void MW_CV_USBH_Device_GetController       (void);
extern void MW_CV_USBH_Device_GetPort             (void);
extern void MW_CV_USBH_Device_GetStatus           (void);
extern void MW_CV_USBH_Device_GetSpeed            (void);
extern void MW_CV_USBH_Device_GetAddress          (void);
extern void MW_CV_USBH_Device_GetVID              (void);
extern void MW_CV_USBH_Device_GetPID              (void);
extern void MW_CV_USBH_Device_GetStringDescriptor (void);
extern void MW_CV_USBH_Notify                     (void);
extern void MW_CV_USBH_Uninitialize               (void);

#endif // MW_CV_USBH_CORE_H_
