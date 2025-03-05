/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Host
 * Copyright (c) 2018-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBH_CDC_ACM.h
 * Purpose: MDK Middleware - Component Validation - USB Host -
 *          Communication Device Class - Abstract Control Model -
 *          tests header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_USBH_CDC_ACM_H_
#define MW_CV_USBH_CDC_ACM_H_

extern void MW_CV_USBH_CDC_ACM_Initialize          (void);
extern void MW_CV_USBH_CDC_ACM_GetDevice           (void);
extern void MW_CV_USBH_CDC_ACM_GetStatus           (void);
extern void MW_CV_USBH_CDC_ACM_Send_Receive        (void);
extern void MW_CV_USBH_CDC_ACM_SetGetLineCoding    (void);
extern void MW_CV_USBH_CDC_ACM_SetControlLineState (void);
extern void MW_CV_USBH_CDC_ACM_Notify              (void);
extern void MW_CV_USBH_CDC_ACM_SendBreak           (void);
extern void MW_CV_USBH_CDC_ACM_Uninitialize        (void);

#endif // MW_CV_USBH_CDC_ACM_H_
