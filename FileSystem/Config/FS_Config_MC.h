/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System:Drive
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FS_Config_MC_%Instance%.h
 * Purpose: File System Configuration for Memory Card Drive
 * Rev.:    V6.2.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>Memory Card Drive %Instance%
// <i>Configuration for SD/SDHC/MMC Memory Card assigned to drive letter "M%Instance%:"
#define MC%Instance%_ENABLE              1

//   <o>Connect to hardware via Driver_MCI# <0-255>
//   <i>Select driver control block for hardware interface
#define MC%Instance%_MCI_DRIVER          0

//   <o>Connect to hardware via Driver_SPI# <0-255>
//   <i>Select driver control block for hardware interface when in SPI mode
#define MC%Instance%_SPI_DRIVER          0

//   <o>Memory Card Interface Mode <0=>Native <1=>SPI
//   <i>Native uses a SD Bus with up to 8 data lines, CLK, and CMD
//   <i>SPI uses 2 data lines (MOSI and MISO), SCLK and CS
#define MC%Instance%_SPI                 0
          
//   <o>Drive Cache Size <0=>OFF <1=>1 KB <2=>2 KB <4=>4 KB
//                       <8=>8 KB <16=>16 KB <32=>32 KB
//   <i>Drive Cache stores data sectors and may be increased to speed-up
//   <i>file read/write operations on this drive (default: 4 KB)
#define MC%Instance%_CACHE_SIZE          4

//   <e>Locate Drive Cache and Drive Buffer
//   <i>Some microcontrollers support DMA only in specific memory areas and
//   <i>require to locate the drive buffers at a fixed address.
#define MC%Instance%_CACHE_RELOC         0

//     <o>Base address <0x0000-0xFFFFFE00:0x200>
//     <i>Set buffer base address to RAM areas that support DMA with the drive.
#define MC%Instance%_CACHE_ADDR          0x7FD00000

//   </e>
//   <o>Filename Cache Size <0-1000000>
//   <i>Define number of cached file or directory names.
//   <i>48 bytes of RAM is required for each cached name.
#define MC%Instance%_NAME_CACHE_SIZE     0

//   <q>Use FAT Journal
//   <i>Protect File Allocation Table and Directory Entries for
//   <i>fail-safe operation.
#define MC%Instance%_FAT_JOURNAL         0

// </h>
