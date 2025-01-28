/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Device
 * Copyright (c) 2018-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBD_MSC.h
 * Purpose: MDK Middleware - Component Validation - USB Device -
 *          Mass Storage Class - tests header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_USBD_MSC_H_
#define MW_CV_USBD_MSC_H_

extern void MW_CV_USBD_MSC0_Initialize           (void);
extern void MW_CV_USBD_MSC0_Uninitialize         (void);
extern void MW_CV_USBD_MSC0_GetCacheInfo         (void);
extern void MW_CV_USBD_MSC0_GetMediaCapacity     (void);
extern void MW_CV_USBD_MSC0_Write_Read           (void);
extern void MW_CV_USBD_MSC0_CheckMedia           (void);
extern void MW_CV_USBD_MSC0_GetMaxLUN            (void);
extern void MW_CV_USBD_MSC0_LUN_GetMediaCapacity (void);
extern void MW_CV_USBD_MSC0_LUN_Read             (void);
extern void MW_CV_USBD_MSC0_LUN_Write            (void);
extern void MW_CV_USBD_MSC0_LUN_CheckMedia       (void);

#endif // MW_CV_USBD_MSC_H_
