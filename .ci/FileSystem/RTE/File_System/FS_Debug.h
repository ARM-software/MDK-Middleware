/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FS_Debug.h
 * Purpose: File System Debug Configuration
 * Rev.:    V8.0.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

//   <e>File System Debug
//   <i>Enable File System event recording
#ifndef FS_DEBUG_EVR_ENABLE
#define FS_DEBUG_EVR_ENABLE     0
#endif

//   <o>Core Management <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure FsCore: Core Management event recording
#define FS_DEBUG_EVR_CORE       1

//   <o>FAT File System <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure FsFAT: FAT File System event recording
#define FS_DEBUG_EVR_FAT        1

//   <o>EFS File System <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure FsEFS: EFS File System event recording
#define FS_DEBUG_EVR_EFS        1

//   <o>I/O Control Interface <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure FsIOC: I/O Control Interface event recording
#define FS_DEBUG_EVR_IOC        1

//   <o>NAND Flash Translation Layer <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure FsNFTL: NAND Flash Translation Layer event recording
#define FS_DEBUG_EVR_NFTL       1

//   <o>NAND Device Interface <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure FsNAND: NAND Device Interface event recording
#define FS_DEBUG_EVR_NAND       1

//   <o>Memory Card MCI <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure FsMcMCI: Memory Card MCI event recording
#define FS_DEBUG_EVR_MC_MCI     1

//   <o>Memory Card SPI <0=>Off <1=>Errors <2=>Errors + API <3=>All
//   <i>Configure FsMcSPI: Memory Card SPI event recording
#define FS_DEBUG_EVR_MC_SPI     1

//   </e>
