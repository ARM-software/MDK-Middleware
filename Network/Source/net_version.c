/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_version.c
 * Purpose: Network Component Version
 *----------------------------------------------------------------------------*/

#include "rl_net_lib.h"
#include "net_version.h"

#define MAJOR       8
#define MINOR       0
#define BUILD       0

/* Version create macros */
#define VER_BIN(x,y,z)          (x << 24 | y << 16 | z)

#define VER_EXPAND(x,y,z)       #x"."#y"."#z
#define VER_ASCII(x,y,z)        VER_EXPAND(x,y,z)

#define ASM_EXPAND(name,x,y,z)  name __asm__(#name"_v"#x"."#y"."#z)
#define ASM_LABEL(name,x,y,z)   ASM_EXPAND(name,x,y,z)

/**
  The following version symbols are created:
  - binary version
  - string version
  - symbol name version
*/


/* Binary version: MMmmbbbb (MM:Major, mm:minor, bbbb:build) */
const uint32_t net_lib_version = VER_BIN(MAJOR, MINOR, BUILD);

#ifdef DEBUG_STDIO
  /* String version: for example "7.17.0" */
  const char   net_ver_ascii[] = VER_ASCII(MAJOR, MINOR, BUILD);
#endif

/* Symbol name: for example "network_ip4ip6_release_v7.17.0" */
#if   defined(Network_IPv6) && defined(DEBUG_EVR)
extern const uint32_t ASM_LABEL(network_ip4ip6_debug, MAJOR, MINOR, BUILD)
#elif defined(Network_IPv6) && defined(DEBUG_STDIO)
extern const uint32_t ASM_LABEL(network_ip4ip6_debug_stdio,MAJOR, MINOR, BUILD)
#elif defined(Network_IPv6)
extern const uint32_t ASM_LABEL(network_ip4ip6_release, MAJOR, MINOR, BUILD)
#elif defined(DEBUG_EVR)
extern const uint32_t ASM_LABEL(network_ip4_debug, MAJOR, MINOR, BUILD)
#elif defined(DEBUG_STDIO)
extern const uint32_t ASM_LABEL(network_ip4_debug_stdio, MAJOR, MINOR, BUILD)
#else
extern const uint32_t ASM_LABEL(network_ip4_release, MAJOR, MINOR, BUILD)
#endif
__attribute__((alias("net_lib_version")));
