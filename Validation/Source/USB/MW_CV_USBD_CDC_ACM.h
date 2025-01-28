/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Device
 * Copyright (c) 2018-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USBD_CDC_ACM.h
 * Purpose: MDK Middleware - Component Validation - USB Device -
 *          Communication Device Class - Abstract Control Model -
 *          tests header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_USBD_CDC_ACM_H_
#define MW_CV_USBD_CDC_ACM_H_

extern void MW_CV_USBD_CDC0_ACM_Initialize          (void);
extern void MW_CV_USBD_CDC0_ACM_Reset               (void);
extern void MW_CV_USBD_CDC0_ACM_SetLineCoding       (void);
extern void MW_CV_USBD_CDC0_ACM_GetLineCoding       (void);
extern void MW_CV_USBD_CDC0_ACM_SetControlLineState (void);
extern void MW_CV_USBD_CDC0_ACM_DataReceived        (void);
extern void MW_CV_USBD_CDC0_ACM_DataSent            (void);
extern void MW_CV_USBD_CDC0_ACM_Uninitialize        (void);

extern void MW_CV_USBD_CDC_ACM_GetChar              (void);
extern void MW_CV_USBD_CDC_ACM_PutChar              (void);
extern void MW_CV_USBD_CDC_ACM_WriteData            (void);
extern void MW_CV_USBD_CDC_ACM_DataAvailable        (void);
extern void MW_CV_USBD_CDC_ACM_Notify               (void);

#endif // MW_CV_USBD_CDC_ACM_H_
