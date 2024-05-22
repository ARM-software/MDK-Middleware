/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_nand_media.h
 * Purpose: NAND Media Driver Definitions
 *----------------------------------------------------------------------------*/

#ifndef FS_NAND_MEDIA_H__
#define FS_NAND_MEDIA_H__

#include "fs_core.h"

/**
  ECC Info Structure Definitions
*/
#define ECC_TYPE_ECC0_DATA              1
#define ECC_TYPE_ECC0_DATA_SPARE        2
#define ECC_TYPE_ECC0_DATA_ECC1_SPARE   3


/**
  NAND Media Driver Instance Status Flags
*/
#define NAND_MEDIA_STATUS_NOTIFY  (1 << 0)     ///< NFTL notification enabled


/**
  NAND Media Driver Return Codes
*/
#define NAND_OK                         0      ///< No error
#define NAND_ERROR                      1      ///< Unspecified error
#define NAND_ERROR_PARAMETER            2      ///< Invalid parameter
#define NAND_ERROR_UNSUPPORTED          3      ///< Functionality not supported
#define NAND_ERROR_TIMEOUT              4      ///< NAND hardware timeout
#define NAND_ERROR_ECC_CORRECTED        5      ///< ECC corrected the data
#define NAND_ERROR_ECC_FAILED           6      ///< ECC could not correct the data
#define NAND_ERROR_PROGRAM_FAILED       7      ///< Programming failed
#define NAND_ERROR_ERASE_FAILED         8      ///< Erase verify failed


/**
  NAND Status Flags Masks (ONFI V1.0 or higher)
*/
#define NAND_STAT_FAIL                  0x01   ///< Last command failed
#define NAND_STAT_FAILC                 0x02   ///< Command prior last failed
#define NAND_STAT_CSP                   0x08   ///< Command specific
#define NAND_STAT_VSP                   0x10   ///< Vendor specific
#define NAND_STAT_ARDY                  0x20   ///< Array operation in progress
#define NAND_STAT_RDY                   0x40   ///< LUN ready for another command
#define NAND_STAT_WP                    0x80   ///< Write protected


/**
  NAND ID addresses (ONFI V1.0 or higher)
*/
#define NAND_ID_ADDR_ONFI               0x20   ///< ONFI signature address for Read ID command
#define NAND_ID_ADDR_JEDEC              0x00   ///< JEDEC signature address for the Read ID command


/**
  Mandatory NAND Flash Commands (ONFI V1.0 or higher)
*/
#define NAND_CMD_READ_1ST               0x00   ///< Read 1st Cycle
#define NAND_CMD_CHANGE_RD_COL_1ST      0x05   ///< Change Read Column 1st Cycle
#define NAND_CMD_PROGRAM_2ND            0x10   ///< Page Program 2nd Cycle
#define NAND_CMD_READ_2ND               0x30   ///< Read 2nd Cycle
#define NAND_CMD_ERASE_1ST              0x60   ///< Block Erase 1st Cycle
#define NAND_CMD_STATUS                 0x70   ///< Read Status
#define NAND_CMD_PROGRAM_1ST            0x80   ///< Page Program 1st Cycle
#define NAND_CMD_CHANGE_WR_COL          0x85   ///< Change Write Column
#define NAND_CMD_READ_ID                0x90   ///< Read ID
#define NAND_CMD_ERASE_2ND              0xD0   ///< Block Erase 2nd cycle
#define NAND_CMD_CHANGE_RD_COL_2ND      0xE0   ///< Change Read Column 2nd Cycle
#define NAND_CMD_READ_PARAM_PAGE        0xEC   ///< Read Parameter Page
#define NAND_CMD_RESET                  0xFF   ///< Reset Command


/**
  Optional NAND Flash Commands (ONFI V1.0 or higher)
*/
#define NAND_CMD_COPYBACK_READ_1ST      0x00  ///< Copyback Read 1st Cycle
#define NAND_CMD_COPYBACK_READ_2ND      0x35  ///< Copyback Read 2nd Cycle
#define NAND_CMD_READ_CACHE_SEQUENTIAL  0x31  ///< Read Cache Sequential
#define NAND_CMD_READ_CACHE_END         0x3F  ///< Read Cache End
#define NAND_CMD_READ_STATUS_ENHANCED   0x78  ///< Read Status Enhanced
#define NAND_CMD_PAGE_CACHE_PROGRAM_1ST 0x80  ///< Page Cache Program 1st Cycle
#define NAND_CMD_PAGE_CACHE_PROGRAM_2ND 0x15  ///< Page Cache Program 2nd Cycle
#define NAND_CMD_COPYBACK_PROGRAM_1ST   0x85  ///< Copyback Program 1st Cycle
#define NAND_CMD_COPYBACK_PROGRAM_2ND   0x10  ///< Copyback Program 2nd Cycle
#define NAND_CMD_READ_UNIQUE_ID         0xED  ///< Read Unique ID
#define NAND_CMD_GET_FEATURES           0xEE  ///< Get Features
#define NAND_CMD_SET_FEATURES           0xEF  ///< Set Features


/* JEDEC manufacturer ID */
#define JEDEC_ID_MICRON_TECH            0x2C  /* Micron Technology           */

/**
  NAND Parameter Page (ONFI V1.0 or higher)
*/
typedef struct NAND_PARAM_PAGE_ {
  uint8_t signature[4];
  uint8_t revision[2];
  uint8_t features[2];
  uint8_t optional[2];
  uint8_t rsvd0[2];
  uint8_t extended[2];
  uint8_t num_pages;
  uint8_t rsvd1[17];
  /* Manufacturer information block */
  uint8_t manufacturer[12];
  uint8_t model[20];
  uint8_t jedec_id;
  uint8_t date[2];
  uint8_t rsvd2[13];
  /* Memory organization block */
  uint8_t data_per_page[4];
  uint8_t spare_per_page[2];
  uint8_t data_per_part_page[4];
  uint8_t spare_per_part_page[2];
  uint8_t pages_per_block[4];
  uint8_t block_per_lun[4];
  uint8_t lun_count;
  uint8_t addr_cycles;
  uint8_t bit_per_cell;
  uint8_t bad_block_per_lun[2];
  uint8_t block_endurance[2];
  uint8_t valid_blocks;
  uint8_t valid_blocks_endurance[2];
  uint8_t program_per_page;
  uint8_t part_program_attr;
  uint8_t ecc_correctability;
  uint8_t plane_addr_bits;
  uint8_t plane_op_attr;
  uint8_t ez_nand_support;
  uint8_t rsvd3[12];
  /* Electrical parameters block */
  uint8_t io_cap_max;
  uint8_t async_timing_mode[2];
  uint8_t async_prog_cache_timing_mode[2];
  uint8_t program_time[2];
  uint8_t erase_time[2];
  uint8_t read_time[2];
  uint8_t ch_col_time[2];
  uint8_t sync_timing_mode[2];
  uint8_t sync_features;
  uint8_t clk_cap[2];
  uint8_t io_cap[2];
  uint8_t i_cap[2];
  uint8_t i_cap_max;
  uint8_t driver_strength;
  uint8_t mplane_read_time[2];
  uint8_t clear_enh_time[2];
  uint8_t read_time_ez[2];
  uint8_t rsvd4[6];
  /* Vendor block */
  uint8_t vendor_revision[2];
  uint8_t vendor_specific[88];
  uint8_t crc[2];
} NAND_PARAM_PAGE;

/* NAND Media Driver */
extern NAND_MEDIA_DRIVER NAND_MediaDriver;

extern int32_t NAND_SetupLayout (NAND_MEDIA_HANDLE *h);
extern int32_t NAND_EccToLayout (NAND_PAGE_LAYOUT *layout, ARM_NAND_ECC_INFO const *ecc);

#endif /* FS_NAND_MEDIA_H__ */
