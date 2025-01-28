/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Device
 * Copyright (c) 2018-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBD_HID.h
 * Purpose: MDK Middleware - Component Validation - USB Device -
 *          Human Interface Device - tests header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_USBD_HID_H_
#define MW_CV_USBD_HID_H_

extern void MW_CV_USBD_HID0_Initialize      (void);
extern void MW_CV_USBD_HID0_GetReport       (void);
extern void MW_CV_USBD_HID0_SetReport       (void);
extern void MW_CV_USBD_HID_GetReportTrigger (void);
extern void MW_CV_USBD_HID0_Uninitialize    (void);

#endif // MW_CV_USBD_HID_H_
