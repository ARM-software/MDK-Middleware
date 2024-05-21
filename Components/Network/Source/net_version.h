/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_version.h
 * Purpose: Network Component Version Definitions
 *----------------------------------------------------------------------------*/

#ifndef NET_VERSION_H_
#define NET_VERSION_H_

#include <stdint.h>

/* Constants */
extern const uint32_t net_lib_version;

#ifdef Network_Debug_STDIO
  extern const char   net_ver_ascii[];
#endif

#endif /* NET_VERSION_H_ */
