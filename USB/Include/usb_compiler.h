/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usb_compiler.h
 * Purpose: USB Library Compiler specific definitions
 *----------------------------------------------------------------------------*/

#ifndef __USB_COMPILER_H__
#define __USB_COMPILER_H__

#include "cmsis_compiler.h"

// Supported compilers are:
// ARM Compiler 6, Arm LLVM/Clang, GNU Compiler and IAR Compiler
#if (defined(__ARMCC_VERSION) || defined(__clang__) || defined(__GNUC__) || defined(__ICCARM__))
  #ifndef   __SECTION
    #define __SECTION__(x)              __attribute__((section(#x)))
    #define __SECTION(x)                __SECTION__(x)
  #endif
#else
  #error Unsupported compiler!
#endif

#endif  // __USB_COMPILER_H__
