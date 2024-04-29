/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_config_def_1.h
 * Purpose: USB Host (USBH) - Host Controller 1 Definitions
 * Note:    this file should only be included when USBH_Config_1.c is being 
 *          compiled as it depends on definitions from USBH_Config_1.c
 *----------------------------------------------------------------------------*/

#ifndef __USBH_CONFIG_DEF_1_H__
#define __USBH_CONFIG_DEF_1_H__

// Check settings for minimum requirements

#if     (USBH1_HC_MEM_POOL_SIZE < 512)
#warning Memory Pool Size for Host Controller 1 not recommended less than 512 bytes!
#endif

// Define macros to size and position memory used by Host Controller

#if     (USBH1_HC_MEM_POOL_RELOC == 1)
#if      defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define  __USBH1_AT_MEM__(x)          __attribute__((section(".ARM.__AT_"#x)))
#define  __USBH1_AT_MEM(x)            __USBH1_AT_MEM__(x)
#define  __USBH1_SECTION_MEM(x)
#else
#define  __USBH1_AT_MEM(x)            __attribute__((at(x)))
#define  __USBH1_SECTION_MEM(x)
#endif
#else
#define  __USBH1_AT_MEM(x)
#define  __USBH1_SECTION_MEM(x)       __attribute__((section(#x)))
#endif
#define  __USBH1_ALIGNED_MEM(x)       __attribute__((aligned(x)))

#endif // __USBH_CONFIG_DEF_1_H__
