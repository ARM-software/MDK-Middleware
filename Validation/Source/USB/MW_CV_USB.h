/*------------------------------------------------------------------------------
 * MDK Middleware - Component Validation - USB Common
 * Copyright (c) 2018-2025 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MW_CV_USB.h
 * Purpose: MDK Middleware - Component Validation - USB Common - header
 *----------------------------------------------------------------------------*/

#ifndef MW_CV_USB_H_
#define MW_CV_USB_H_

#include <stdint.h>

#define MW_CW_USB_STATUS_TXT(x)         MW_CV_USB_STATUS_STR[MW_CV_USB_STATUS_VAL_TO_STR[x]]
#define MW_CW_USB_SPEED_TXT(x)          MW_CV_USB_SPEED_STR [x]
#define MW_CW_USB_BOOL_TXT(x)           MW_CV_USB_BOOL_STR  [x]

extern const uint8_t MW_CV_USB_STATUS_VAL_TO_STR[256];
extern const char   *MW_CV_USB_STATUS_STR[22];
extern const char   *MW_CV_USB_SPEED_STR[3];
extern const char   *MW_CV_USB_BOOL_STR[2];


#endif // MW_CV_USB_H_
