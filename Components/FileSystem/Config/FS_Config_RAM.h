/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System:Drive
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FS_Config_RAM_%Instance%.h
 * Purpose: File System Configuration for RAM Drive
 * Rev.:    V6.3.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>RAM Drive %Instance%
// <i>Configuration for RAM assigned to drive letter "R%Instance%:"
#define RAM%Instance%_ENABLE             1

//   <o>Device Size <0x4C00-0xFFFFF000:0x400>
//   <i>Define the size of RAM device in bytes
//   <i>Default: 0x8000
#define RAM%Instance%_SIZE               0x008000

//   <e>Locate Drive Cache and Drive Buffer
//   <i>Locate RAM drive buffer at a specific address.
//   <i>If not enabled, the linker selects base address.
#define RAM%Instance%_RELOC              0

//     <s>Section Name
//     <i>Define the name of the section for the file system buffer.
//     <i>Linker script shall have this section defined.
#define RAM0%Instance%_SECTION           ".filesystem.ram%Instance%"

//   </e>

// </h>
