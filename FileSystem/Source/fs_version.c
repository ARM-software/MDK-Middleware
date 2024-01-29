/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_version.c
 * Purpose: File System Component Version
 *----------------------------------------------------------------------------*/
#include "rl_fs_lib.h"

/* FileSystem Version */
#define MAJOR    6
#define MINOR    16
#define BUILD    6

/* BCD encoded version: 0xMMmmbbbb (MM:Major, mm:minor, bbbb:build) */
const uint32_t fs_lib_version = ((MAJOR / 10)  << 28) | ((MAJOR % 10) << 24) | \
                                ((MINOR / 10)  << 20) | ((MINOR % 10) << 16) | \
                                ((BUILD /1000) << 12) | ((BUILD /100) <<  8) | \
                                ((BUILD /  10) <<  4) |  (BUILD % 10);

/* Macro to create symbol: prefix_MM_mm_bbbb */
#define EXPAND_SYMBOL(prefix, MM, mm, bbbb) prefix##_##MM##_##mm##_##bbbb
#define CREATE_SYMBOL(prefix, MM, mm, bbbb) EXPAND_SYMBOL(prefix, MM, mm, bbbb)

/* Export a version number symbol for a version control */
extern const uint32_t CREATE_SYMBOL (fs_lib_version, MAJOR, MINOR, BUILD) __attribute__((alias("fs_lib_version")));
