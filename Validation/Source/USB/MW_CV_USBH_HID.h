/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Host
 * Copyright (c) 2018-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBH_HID.h
 * Purpose: MDK Middleware - Component Validation - USB Host -
 *          Human Interface Device - tests header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_USBH_HID_H_
#define MW_CV_USBH_HID_H_

extern void MW_CV_USBH_HID_Initialize            (void);
extern void MW_CV_USBH_HID_GetDevice             (void);
extern void MW_CV_USBH_HID_GetStatus             (void);
extern void MW_CV_USBH_HID_Write_Read            (void);
extern void MW_CV_USBH_HID_GetKeyboardKey        (void);
extern void MW_CV_USBH_HID_GetMouseState         (void);
extern void MW_CV_USBH_HID_ParseReportDescriptor (void);
extern void MW_CV_USBH_HID_DataReceived          (void);
extern void MW_CV_USBH_HID_Uninitialize          (void);

#endif // MW_CV_USBH_HID_H_
