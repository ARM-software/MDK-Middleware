/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    rl_net_rte.h
 * Purpose: Network Run-Time-Environment Definitions
 *----------------------------------------------------------------------------*/

#ifndef __RL_NET_RTE_H
#define __RL_NET_RTE_H

#include "RTE_Components.h"

/* Network Core */
#ifdef RTE_Network_IPv6
  #define Network_IPv6
#endif

/* Library Variant */
#ifdef RTE_Network_Source
  #define Network_Source
#endif

/* Network Debug */
#ifdef RTE_Network_Debug
  #define DEBUG_EVR
#endif
#ifdef RTE_Network_Debug_STDIO
  #define DEBUG_STDIO
#endif

#endif /* __RL_NET_RTE_H */
