/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usb_os.h
 * Purpose: USB OS Header
 *----------------------------------------------------------------------------*/

#ifndef __USB_OS_H__
#define __USB_OS_H__

#include "RTE_Components.h"


#if defined(RTE_CMSIS_RTOS2)
  #include "cmsis_os2.h"
  #define  USB_CMSIS_RTOS2         (1U)
#if defined(RTE_CMSIS_RTOS2_RTX5)
  #include "rtx_os.h"
  #define  USB_CMSIS_RTOS2_RTX5    (1U)
#endif
#endif
#if defined(RTE_CMSIS_RTOS)
  #include "cmsis_os.h"
  #define  USB_CMSIS_RTOS          (1U)
#endif

#endif // __USB_OS_H__
