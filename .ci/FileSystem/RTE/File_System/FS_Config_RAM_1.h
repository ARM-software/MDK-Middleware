/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System:Drive
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FS_Config_RAM_1.h
 * Purpose: File System Configuration for RAM Drive
 * Rev.:    V6.3.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

// <h>RAM Drive 1
// <i>Configuration for RAM assigned to drive letter "R1:"
#define RAM1_ENABLE             1

//   <o>Device Size <0x4C00-0xFFFFF000:0x400>
//   <i>Define the size of RAM device in bytes
//   <i>Default: 0x8000
#define RAM1_SIZE               0x008000

//   <e>Locate Drive Cache and Drive Buffer
//   <i>Locate RAM drive buffer at a specific address.
//   <i>If not enabled, the linker selects base address.
#define RAM1_RELOC              0

//     <s>Section Name
//     <i>Define the name of the section for the file system buffer.
//     <i>Linker script shall have this section defined.
#define RAM01_SECTION           ".filesystem.ram1"

//   </e>

// </h>
