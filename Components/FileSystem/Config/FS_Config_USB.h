/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System:Drive
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FS_Config_USB_%Instance%.h
 * Purpose: File System Configuration for USB Drive
 * Rev.:    V6.2.1
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

// <h>USB Drive %Instance%
// <i>Configuration for USB device assigned to drive letter "U%Instance%:"
#define USB%Instance%_ENABLE             1

//   <o>Drive Cache Size <0=>OFF <1=>1 KB <2=>2 KB <4=>4 KB
//                       <8=>8 KB <16=>16 KB <32=>32 KB
//   <i>Drive Cache stores data sectors and may be increased to speed-up
//   <i>file read/write operations on this drive (default: 4 KB)
#define USB%Instance%_CACHE_SIZE         4

//   <o>Filename Cache Size <0-1000000>
//   <i>Define number of cached file or directory names.
//   <i>48 bytes of RAM is required for each cached name.
#define USB%Instance%_NAME_CACHE_SIZE    0

//   <q>Use FAT Journal
//   <i>Protect File Allocation Table and Directory Entries for
//   <i>fail-safe operation.
#define USB%Instance%_FAT_JOURNAL        0

// </h>
