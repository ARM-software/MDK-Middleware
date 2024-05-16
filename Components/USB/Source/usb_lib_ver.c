/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Common
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usb_lib_ver.c
 * Purpose: USB Common Device/Host - Version module
 *----------------------------------------------------------------------------*/

#include "usb_lib_ver.h"

#include "cmsis_compiler.h"

#define USB_LIB_VERSION_MAJOR           8
#define USB_LIB_VERSION_MINOR           0
#define USB_LIB_VERSION_REVISION        0

#define USB_LIB_VER_SECTION__(x,y,z)    __attribute__((section("__MW_USB_LIB_VERSION_"#x"."#y"."#z)))
#define USB_LIB_VER_SECTION(x,y,z)      USB_LIB_VER_SECTION__(x,y,z)

const uint32_t usb_lib_version USB_LIB_VER_SECTION(USB_LIB_VERSION_MAJOR,
                                                   USB_LIB_VERSION_MINOR,
                                                   USB_LIB_VERSION_REVISION) = 
                                                  (USB_LIB_VERSION_MAJOR * 10000000U) + 
                                                  (USB_LIB_VERSION_MINOR * 10000U)    + 
                                                  (USB_LIB_VERSION_REVISION);
