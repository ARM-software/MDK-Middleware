/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_version.c
 * Purpose: Network Component Version
 *----------------------------------------------------------------------------*/

#include "net_lib.h"
#include "net_version.h"

#define MAJOR       MW_NET_VERSION_MAJOR
#define MINOR       MW_NET_VERSION_MINOR
#define PATCH       MW_NET_VERSION_PATCH

/* Version create macros */
#define VER_BIN(x,y,z)          (x << 24 | y << 16 | z)

#define VER_EXPAND(x,y,z)       #x"."#y"."#z
#define VER_ASCII(x,y,z)        VER_EXPAND(x,y,z)

/**
  The following version symbols are created:
  - binary version
  - string version
  - symbol name version
*/


/* Binary version: MMmmbbbb (MM:Major, mm:minor, bbbb:build) */
const uint32_t net_lib_version = VER_BIN(MAJOR, MINOR, PATCH);

#ifdef Network_Debug_STDIO
  /* String version: for example "7.17.0" */
  const char   net_ver_ascii[] = VER_ASCII(MAJOR, MINOR, PATCH);
#endif
