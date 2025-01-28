/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Config.h
 * Purpose: Network Core Configuration
 * Rev.:    V8.0.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

// <h>Network Core
//   <o>Variant <0=>IPv4 only
//              <1=>IPv4/IPv6 dual stack
//   <i>Configure variant of the network library
#define NET_CORE_VARIANT        1

//   <s.15>Local Host Name
//   <i>This is the name under which embedded host can be
//   <i>accessed on a local area network.
//   <i>Default: "my_host"
#define NET_HOST_NAME           "my_host"

//   <o>Memory Pool Size <1536-262144:4>
//   <i>This is the size of a memory pool in bytes. Buffers for
//   <i>network packets are allocated from this memory pool.
//   <i>Default: 12000 bytes
#define NET_MEM_POOL_SIZE       12000

//   <q>Start System Services
//   <i>If enabled, the system will automatically start server services
//   <i>(HTTP, FTP, TFTP server, ...) when initializing the network system.
//   <i>Default: Enabled
#define NET_START_SERVICE       1

//   <h>OS Resource Settings
//   <i>These settings are used to optimize usage of OS resources.
//     <o>Core Thread Stack Size <512-65535:4>
//     <i>Default: 2048 bytes
#define NET_THREAD_STACK_SIZE   2048

//      Core Thread Priority
#define NET_THREAD_PRIORITY     osPriorityNormal
//   </h>
// </h>
