/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System:Drive
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FS_Config_NAND_0.h
 * Purpose: File System Configuration for NAND Flash Drive
 * Rev.:    V6.4.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

// <h>NAND Flash Drive 0
// <i>Configuration for NAND device assigned to drive letter "N0:"
#define NAND0_ENABLE            1

//   <y>Connect to hardware via Driver_NAND#
//   <i>Select driver control block for hardware interface
#define NAND0_DRIVER            0

//   <o>Device Number <0-255>
//   <i>Selects NAND device connected to selected driver
#define NAND0_DEV_NUM           0

//   <o>Bus Width <0=>8-bit <1=>16-bit
//   <i>Define NAND device bus width
#define NAND0_BUS_WIDTH         0

//   <o>Page Size <528-18592>
//   <i>Define program Page size in bytes (User + Spare area).
#define NAND0_PAGE_SIZE         2112

//   <o>Block Size <8=>8 pages <16=>16 pages <32=>32 pages
//                 <64=>64 pages <128=>128 pages <256=>256 pages
//   <i>Define number of pages in a block.
#define NAND0_PAGE_COUNT        64

//   <o>Device Size [blocks] <512-32768>
//   <i>Define number of blocks in NAND Flash device.
#define NAND0_BLOCK_COUNT       4096

//   <o>Page Caching <0=>OFF <1=>1 page <2=>2 pages <4=>4 pages
//                   <8=>8 pages <16=>16 pages <32=>32 pages
//   <i>Device pages can be cached to speed-up sector read/write
//   <i>operations on this drive.
//   <i>Define number of cached Pages (default: 2 pages).
#define NAND0_PAGE_CACHE        2

//   <o>Block Indexing <0=>OFF <1=>1 block <2=>2 blocks <4=>4 blocks
//                     <8=>8 blocks <16=>16 blocks <32=>32 blocks
//                     <64=>64 blocks <128=>128 blocks <256=>256 blocks
//   <i>Device blocks can be indexed for faster page access time.
//   <i>Increase number of indexed blocks for better performance (default: 16 blocks).
#define NAND0_BLOCK_CACHE       16

//   <h>ECC Configuration
//     <o>Algorithm <0=>None <1=>Software 1-bit <2=>On-Chip <3=>Hardware
//     <i> - None: ECC not used
//     <i> - Software 1-bit: 1-bit Hamming calculation in software
//     <i> - On-Chip: EZ NAND compliant on-chip ECC calculation
//     <i> - Hardware: ECC calculation in hardware driver
#define NAND0_SW_ECC            1

//     <h>On-Chip Layout
//     <i>Configure ECC protection layout when on-chip ECC is used.

//       <h> Virtual Page
//       <i> Define virtual page properties
//         <o>Layout <0=>Alternating Main and Spare <1=>Contiguous Main and Spare
//         <i>Alternating: |Main0|Spare0|...|MainN-1|SpareN-1|
//         <i>Contiguous: |Main0|...|MainN-1|Spare0|...|SpareN-1|
#define NAND0_ECC_VPAGE_LAYOUT  1

//         <o>Main Size <512-16384:512>
//         <i> Main area size of the virtual page
#define NAND0_ECC_VMAIN_SIZE    512

//         <o>Spare Size
//         <i> Spare area size of the virtual page
#define NAND0_ECC_VSPARE_SIZE   16

//         <o>Page Count <0=>1 <1=>2 <2=>4 <3=>8 <4=>16 <5=>32
//         <i> Define number of virtual pages.
#define NAND0_ECC_VPAGE_COUNT   2
//       </h>

//       <h>Main Codeword
//       <i> Define ECC protected data layout in Main
//         <o> Size
//         <i> Size of protected data (in bytes)
#define NAND0_ECC_MAIN_CW_SIZE 512
//       </h>

//       <h>Spare Codeword
//       <i> Define ECC protected data layout in Spare
//         <o> Size
//         <i> Size of protected data (in bytes)
#define NAND0_ECC_SPARE_CW_SIZE 4

//         <o> Offset
//         <i> Offset where protected data starts (in bytes)
#define NAND0_ECC_SPARE_CW_OFFS 4

//         <o> Gap
//         <i> Gap till next protected data (in bytes)
#define NAND0_ECC_SPARE_CW_GAP  12
//       </h>

//       <h>ECC Data
//       <i> Define where ECC generated data is located in Spare
//         <o> Size
//         <i> Size of generated ECC (in bytes)
#define NAND0_ECC_DATA_SIZE     8

//         <o> Offset
//         <i> Offset where generated ECC starts (in bytes)
#define NAND0_ECC_DATA_OFFS     8

//         <o> Gap
//         <i> Gap till next generated ECC (in bytes)
#define NAND0_ECC_DATA_GAP      8
//       </h>
//     </h>
//   </h>

//   <o>Drive Cache Size <0=>OFF <1=>1 KB <2=>2 KB <4=>4 KB
//                       <8=>8 KB <16=>16 KB <32=>32 KB
//   <i>Drive Cache stores data sectors and may be increased to speed-up
//   <i>file read/write operations on this drive (default: 4 KB)
#define NAND0_CACHE_SIZE        4

//   <e>Locate Drive Cache and Drive Buffer
//   <i>Some microcontrollers support DMA only in specific memory areas and
//   <i>require to locate the drive buffers at a fixed address.
#define NAND0_CACHE_RELOC       0

//     <s>Section Name
//     <i>Define the name of the section for the drive cache and drive buffers.
//     <i>Linker script shall have this section defined.
#define NAND0_CACHE_SECTION     ".driver.nand0"

//   </e>
//   <o>Filename Cache Size <0-1000000>
//   <i>Define number of cached file or directory names.
//   <i>48 bytes of RAM is required for each cached name.
#define NAND0_NAME_CACHE_SIZE   0

//   <q>Use FAT Journal
//   <i>Protect File Allocation Table and Directory Entries for
//   <i>fail-safe operation.
#define NAND0_FAT_JOURNAL       0

// </h>
