/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Host
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBH_MSC.h
 * Purpose: MDK Middleware - Component Validation - USB Host -
 *          Mass Storage Class - tests header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_USBH_MSC_H_
#define MW_CV_USBH_MSC_H_

extern void MW_CV_USBH_MSC_Initialize   (void);
extern void MW_CV_USBH_MSC_GetDevice    (void);
extern void MW_CV_USBH_MSC_GetStatus    (void);
extern void MW_CV_USBH_MSC_Write_Read   (void);
extern void MW_CV_USBH_MSC_ReadCapacity (void);
extern void MW_CV_USBH_MSC_Uninitialize (void);

#endif // MW_CV_USBH_MSC_H_
