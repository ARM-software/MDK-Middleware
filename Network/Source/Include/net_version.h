/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_version.h
 * Purpose: Network Component Version Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_VERSION_H
#define __NET_VERSION_H

#include <stdint.h>

/* Constants */
extern const uint32_t net_lib_version;

#ifdef DEBUG_STDIO
  extern const char   net_ver_ascii[];
#endif

#endif /* __NET_VERSION_H */
