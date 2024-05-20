/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_version.c
 * Purpose: File System Component Version
 *----------------------------------------------------------------------------*/
#include "fs_core.h"

/* FileSystem Version */
#define MAJOR    (MW_FS_VERSION_MAJOR)
#define MINOR    (MW_FS_VERSION_MINOR)
#define PATCH    (MW_FS_VERSION_PATCH)

/* BCD encoded version: 0xMMmmbbbb (MM:Major, mm:minor, bbbb:build) */
const uint32_t fs_lib_version = ((MAJOR / 10)  << 28) | ((MAJOR % 10) << 24) | \
                                ((MINOR / 10)  << 20) | ((MINOR % 10) << 16) | \
                                ((PATCH /1000) << 12) | ((PATCH /100) <<  8) | \
                                ((PATCH /  10) <<  4) |  (PATCH % 10);
