/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_section.h
 * Purpose: USB Host (USBH) - Section definitions
 *----------------------------------------------------------------------------*/

#ifndef  USBH_SECTION_
#define  USBH_SECTION_

/*------------------------------------------------------------------------------
 *      USB Host 0..3 section defines
 *----------------------------------------------------------------------------*/

#define USBHn_HC_MEM_POOL_SECTION_(str) __attribute__((section(str)))

#ifdef  USBH0_HC_NUM
#if    (USBH0_HC_MEM_POOL_LOCATE == 1)
#define USBH0_HC_MEM_POOL_SECTION       USBHn_HC_MEM_POOL_SECTION_(USBH0_HC_MEM_POOL_SECTION_NAME)
#else 
#define USBH0_HC_MEM_POOL_SECTION
#endif
#endif

#ifdef  USBH1_HC_NUM
#if    (USBH1_HC_MEM_POOL_LOCATE == 1)
#define USBH1_HC_MEM_POOL_SECTION       USBHn_HC_MEM_POOL_SECTION_(USBH1_HC_MEM_POOL_SECTION_NAME)
#else 
#define USBH1_HC_MEM_POOL_SECTION
#endif
#endif

#ifdef  USBH2_HC_NUM
#if    (USBH2_HC_MEM_POOL_LOCATE == 1)
#define USBH2_HC_MEM_POOL_SECTION       USBHn_HC_MEM_POOL_SECTION_(USBH2_HC_MEM_POOL_SECTION_NAME)
#else 
#define USBH2_HC_MEM_POOL_SECTION
#endif
#endif

#ifdef  USBH3_HC_NUM
#if    (USBH3_HC_MEM_POOL_LOCATE == 1)
#define USBH3_HC_MEM_POOL_SECTION       USBHn_HC_MEM_POOL_SECTION_(USBH3_HC_MEM_POOL_SECTION_NAME)
#else 
#define USBH3_HC_MEM_POOL_SECTION
#endif
#endif

#endif  // USBH_SECTION_
