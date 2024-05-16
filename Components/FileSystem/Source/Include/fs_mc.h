/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_mc.h
 * Purpose: SD/MMC Card definition header
 *----------------------------------------------------------------------------*/

#ifndef __FS_MC_H
#define __FS_MC_H

#include "rl_fs_lib.h"

/**
  Memory card command layer status flags
*/
#define MC_STATUS_DRIVER_INIT (1U <<  0)    ///< Driver initialized
#define MC_STATUS_EMMC_SLEEP  (1U <<  1)    ///< eMMC device is in Sleep State
#define MC_STATUS_LOCKED      (1U <<  2)    ///< Device password protection active

/**
  Memory card default initialization bus speed (in Hz)
*/
#define MC_INIT_BUS_SPEED               400000U

/**
  ReadSector function state definitions
*/
#define MC_READ_INIT                    0U
#define MC_READ_DATA                    1U
#define MC_READ_ERROR                   2U

/**
  WriteSector function state definitions
*/
#define MC_WRITE_INIT                   0U
#define MC_WRITE_DATA                   1U
#define MC_WRITE_ERROR                  2U

/**
  GeneralCommand function state definitions
*/
#define MC_STATE_INIT                   0U
#define MC_STATE_SETUP_XFER             1U
#define MC_STATE_SEND_CMD               2U
#define MC_STATE_WAIT                   3U
#define MC_STATE_WAIT_WRITE             4U
#define MC_STATE_STBY                   5U
#define MC_STATE_ERROR                  6U

/**
  Maximum retry count
*/
#define MC_RETRY_OPERATION_COUNT        3U

/**
  Driver event masks
*/
#define MC_CMD_EVENTS (ARM_MCI_EVENT_COMMAND_COMPLETE | \
                       ARM_MCI_EVENT_COMMAND_TIMEOUT  | \
                       ARM_MCI_EVENT_COMMAND_ERROR)
#define MC_DAT_EVENTS (ARM_MCI_EVENT_TRANSFER_COMPLETE | \
                       ARM_MCI_EVENT_TRANSFER_TIMEOUT  | \
                       ARM_MCI_EVENT_TRANSFER_ERROR)
/**
  Memory Card Command Set
*/
#define MC_CMD_GO_IDLE_STATE           0    ///<  -, Reset card to idle state     MMC,SD
#define MC_CMD_SEND_OP_COND            1    ///< R1, Send Op.Cond. Register       MMC,--
#define MC_CMD_ALL_SEND_CID            2    ///< R2, Send Card CID number         MMC,SD
#define MC_CMD_SET_RELATIVE_ADDR       3    ///< R1, Assigns Relative Address     MMC,--
#define MC_CMD_SEND_RELATIVE_ADDR      3    ///< R6, Publish new relative address ---,SD
#define MC_CMD_SLEEP_AWAKE             5    ///< R1b, Toggle between SLEEP/STBY
#define MC_CMD_SWITCH                  6    ///< R1b, Switch MMC, --
#define MC_CMD_SELECT_DESELECT_CARD    7    ///< R1b, Select/Deselect the Card     MMC,SD
#define MC_CMD_SEND_IF_COND            8    ///< R7,  Send Interface Condition     ---,SD
#define MC_CMD_SEND_EXT_CSD            8    ///< R1, Send Extended CSD register   MMC,--
#define MC_CMD_SEND_CSD                9    ///< R2, Send Card Specific Data      MMC,SD
#define MC_CMD_SEND_CID               10    ///< R2, Send Card Identificat.Data   MMC,SD
#define MC_CMD_VOLTAGE_SWITCH         11    ///< R1, Switch to 1.8V bus signaling ---,SD
#define MC_CMD_STOP_TRANSMISSION      12    ///< R1b,Stop Read or Write transm.   MMC,SD
#define MC_CMD_SEND_STATUS            13    ///< R1, Send Status Register         MMC,SD
#define MC_CMD_SET_BLOCKLEN           16    ///< R1, Set Block Length in bytes    MMC,SD
#define MC_CMD_READ_SINGLE_BLOCK      17    ///< R1, Read a Single Block          MMC,SD
#define MC_CMD_READ_MULTIPLE_BLOCK    18    ///< R1, Read Multiple Blocks         MMC,SD
#define MC_CMD_SEND_TUNING_BLOCK      19    ///< R1, Send 64 bytes tuning pattern MMC,SD
#define MC_CMD_WRITE_SINGLE_BLOCK     24    ///< R1, Write a Block                MMC,SD
#define MC_CMD_WRITE_MULTIPLE_BLOCK   25    ///< R1, Write Multiple Blocks        MMC,SD
#define MC_CMD_SET_WRITE_PROT         28    ///< R1b,Sets write protection bit    MMC,SD
#define MC_CMD_CLR_WRITE_PROT         29    ///< R1b,Clears write protection bit  MMC,SD
#define MC_CMD_SEND_WRITE_PROT        30    ///< R1, Send write protection status MMC,SD
#define MC_CMD_LOCK_UNLOCK            42    ///< R1, Set/reset the password       MMC,SD
#define MC_CMD_APP_CMD                55    ///< R1, App.Specific Cmd follows     MMC,SD
#define MC_CMD_GEN_CMD                56    ///< R1, General Command              MMC,SD
#define MC_CMD_READ_OCR               58    ///< R3, Read OCR Register            MMC,SD
#define MC_CMD_CRC_ON_OFF             59    ///< R1, Turn CRC7 checking on/off    MMC,SD

#define MC_ACMD_SD_SET_BUS_WIDTH       6    ///< R1, Set Bus Width 1bit/4bits     ---,SD
#define MC_ACMD_SD_SEND_OP_COND       41    ///< R3, Send App. Op.Cond Register   ---,SD
#define MC_ACMD_SET_CLR_CARD_DETECT   42    ///< R1, Conn/Disconn pull-up on DAT3 ---,SD

/**
  Memory Card Response Type Flags
*/
#define MC_RESPONSE_NONE  ARM_MCI_RESPONSE_NONE       ///< No response expected
#define MC_RESPONSE_R1    ARM_MCI_RESPONSE_SHORT      ///< Normal response command (48-bit)
#define MC_RESPONSE_R1b   ARM_MCI_RESPONSE_SHORT_BUSY ///< R1 response with busy signal (48-bit)
#define MC_RESPONSE_R2    ARM_MCI_RESPONSE_LONG       ///< CID, CSD register response (136-bit)
#define MC_RESPONSE_R3    ARM_MCI_RESPONSE_SHORT      ///< OCR register response (48-bit)
#define MC_RESPONSE_R4    ARM_MCI_RESPONSE_SHORT      ///< Fast I/O response (48-bit)
#define MC_RESPONSE_R5    ARM_MCI_RESPONSE_SHORT      ///< Interrupt request response (48-bit)
#define MC_RESPONSE_R5b   ARM_MCI_RESPONSE_SHORT_BUSY ///< R5 response with busy signal (48-bit)
#define MC_RESPONSE_R6    ARM_MCI_RESPONSE_SHORT      ///< Published RCA response (48-bit)
#define MC_RESPONSE_R7    ARM_MCI_RESPONSE_SHORT      ///< Card interface condition (48-bit)

/**
  Card Status Bitmask Definitions (R1 Response)
*/
#define R1_OUT_OF_RANGE       (1U << 31)    ///< Command argument out of range
#define R1_ADDRESS_ERROR      (1U << 30)    ///< Misaligned address used
#define R1_BLOCK_LEN_ERROR    (1U << 29)    ///< Block length not allowed
#define R1_ERASE_SEQ_ERROR    (1U << 28)    ///< Erase command sequence invalid
#define R1_ERASE_PARAM        (1U << 27)    ///< Invalid selection of blocks
#define R1_WP_VIOLATION       (1U << 26)    ///< Attempt to write to WP block
#define R1_CARD_IS_LOCKED     (1U << 25)    ///< Card is locked by the host
#define R1_LOCK_UNLOCK_FAILED (1U << 24)    ///< Sequence or password error in lock/unlock command
#define R1_COM_CRC_ERROR      (1U << 23)    ///< CRC check of the previous command failed
#define R1_ILLEGAL_COMMAND    (1U << 22)    ///< Command not legal for the card state
#define R1_CARD_ECC_FAILED    (1U << 21)    ///< Card internal ECC failed to correct the data
#define R1_CC_ERROR           (1U << 20)    ///< Internal card controller error
#define R1_ERROR              (1U << 19)    ///< General or unknown error occurred

#define R1_CSD_OVERWRITE      (1U << 16)    ///< CSD write error
#define R1_WP_ERASE_SKIP      (1U << 15)    ///< Only partial address space erased
#define R1_CARD_ECC_DISABLED  (1U << 14)    ///< Command executed without internal ECC
#define R1_ERASE_RESET        (1U << 13)    ///< Erase sequence cleared before executing

#define R1_STATE_Msk        (0xFU <<  9)    ///< Card Status: CURRENT_STATE mask
#define R1_STATE_IDLE         (0U <<  9)    ///< Current state: IDLE
#define R1_STATE_READY        (1U <<  9)    ///< Current state: READY
#define R1_STATE_IDENT        (2U <<  9)    ///< Current state: IDENT
#define R1_STATE_STBY         (3U <<  9)    ///< Current state: STBY
#define R1_STATE_TRAN         (4U <<  9)    ///< Current state: TRAN
#define R1_STATE_DATA         (5U <<  9)    ///< Current state: DATA
#define R1_STATE_RCV          (6U <<  9)    ///< Current state: RCV
#define R1_STATE_PRG          (7U <<  9)    ///< Current state: PRG
#define R1_STATE_DIS          (8U <<  9)    ///< Current state: DIS

#define R1_READY_FOR_DATA     (1U <<  8)    ///< Buffer empty signaling
#define R1_APP_CMD            (1U <<  5)    ///< Card expects ACMD or cmd interpreted as ACMD
#define R1_AKE_SEQ_ERROR      (1U <<  3)    ///< Authentication process sequence error

/**
  Card Status Bitmask Definitions (R2 Response - SPI)
  
  B1 = byte 1, B2 = byte 2
*/
#define R2_B1_IN_IDLE_STATE        0x01
#define R2_B1_ERASE_RESET          0x02
#define R2_B1_ILLEGAL_COMMAND      0x04
#define R2_B1_COM_CRC_ERROR        0x08
#define R2_B1_ERASE_SEQUENCE_ERROR 0x01
#define R2_B1_ADDRESS ERROR        0x02
#define R2_B1_PARAMETER_ERROR      0x04

#define R2_B2_CARD_IS_LOCKED       0x01
#define R2_B2_LOCK_UNLOCK_FAILED   0x02
#define R2_B2_ERROR                0x04
#define R2_B2_CC_ERROR             0x08
#define R2_B2_CARD_ECC_ERROR       0x01
#define R2_B2_WP_VIOLATION         0x02
#define R2_B2_ERASE_PARAM          0x04

/**
  OCR Register Bitmask Definitions (R3 Response)
*/
#define R3_1V7_1V95           (1U <<  7)
#define R3_2V7_2V8            (1U << 15)
#define R3_2V8_2V9            (1U << 16)
#define R3_2V9_3V0            (1U << 17)
#define R3_3V0_3V1            (1U << 18)
#define R3_3V1_3V2            (1U << 19)
#define R3_3V2_3V3            (1U << 20)
#define R3_3V3_3V4            (1U << 21)
#define R3_3V4_3V5            (1U << 22)
#define R3_3V5_3V6            (1U << 23)
#define R3_S18A               (1U << 24)    ///< Switching to 1.8V Accepted
#define R3_UHSII              (1U << 29)    ///< UHS-II Card Status
#define R3_CCS                (1U << 30)    ///< Card Capacity Status
#define R3_BUSY               (1U << 31)    ///< Card power up status bit

/**
  SD/MMC Card Property Definitions
*/
#define MC_PROP_TYPE_SD       (1U <<  0)    ///< SD Card
#define MC_PROP_TYPE_MMC      (1U <<  1)    ///< MultiMediaCard
#define MC_PROP_SD_V2         (1U <<  2)    ///< SD Card Ver2.00 or later
#define MC_PROP_MMC_V4        (1U <<  3)    ///< Complies with MMC V4.x
#define MC_PROP_MMC_DUALV     (1U <<  4)    ///< Dual Voltage MMC
#define MC_PROP_SPEED_UHSI    (1U <<  5)    ///< UHS-I Card
#define MC_PROP_SPEED_UHSII   (1U <<  6)    ///< UHS-II Card
#define MC_PROP_ACCESS_BYTE   (1U <<  7)    ///< Card requires byte access
#define MC_PROP_DATA_BUS_1V8  (1U <<  8)    ///< 1.8V UHS signaling supported
#define MC_PROP_DATA_BUS_4BIT (1U <<  9)    ///< 4-bit data bus supported
#define MC_PROP_DATA_BUS_8BIT (1U << 10)    ///< 8-bit data bus supported
#define MC_PROP_DATA_BUS_DDR  (1U << 11)    ///< DDR data bus supported
#define MC_PROP_CCC_10        (1U << 12)    ///< Class 10 command set supported

/* ACMD6 Bus Width Argument Definition */
#define ACMD6_ARG_BUS_WIDTH_1BIT       0    ///< Set 1-bit bus width
#define ACMD6_ARG_BUS_WIDTH_4BIT       2    ///< Set 4-bit bus width

/* ACMD41 Argument Definitions */
#define ACMD41_ARG_VDD_2V7_2V8 (1U << 15)
#define ACMD41_ARG_VDD_2V8_2V9 (1U << 16)
#define ACMD41_ARG_VDD_2V9_3V0 (1U << 17)
#define ACMD41_ARG_VDD_3V0_3V1 (1U << 18)
#define ACMD41_ARG_VDD_3V1_3V2 (1U << 19)
#define ACMD41_ARG_VDD_3V2_3V3 (1U << 20)
#define ACMD41_ARG_VDD_3V3_3V4 (1U << 21)
#define ACMD41_ARG_VDD_3V4_3V5 (1U << 22)
#define ACMD41_ARG_VDD_3V5_3V6 (1U << 23)
#define ACMD41_ARG_S18R        (1U << 24)
#define ACMD41_ARG_XPC         (1U << 28)
#define ACMD41_ARG_FB          (1U << 29)
#define ACMD41_ARG_HCS         (1U << 30)
#define ACMD41_ARG_BUSY        (1U << 31)

#define ACMD41_ARG_VDD_WINDOW  (ACMD41_ARG_VDD_2V7_2V8 | \
                                ACMD41_ARG_VDD_2V8_2V9 | \
                                ACMD41_ARG_VDD_2V9_3V0 | \
                                ACMD41_ARG_VDD_3V0_3V1 | \
                                ACMD41_ARG_VDD_3V1_3V2 | \
                                ACMD41_ARG_VDD_3V2_3V3 | \
                                ACMD41_ARG_VDD_3V3_3V4 | \
                                ACMD41_ARG_VDD_3V4_3V5 | \
                                ACMD41_ARG_VDD_3V5_3V6)

/* SD CMD6 Argument Definitions */
#define CMD6_ARG_ACCESS_MODE(x)       (((x) & 0x0F) <<  0)
#define CMD6_ARG_COMMAND_SYSTEM(x)    (((x) & 0x0F) <<  4)
#define CMD6_ARG_DRIVE_STRENGTH(x)    (((x) & 0x0F) <<  8)
#define CMD6_ARG_POWER_LIMIT(x)       (((x) & 0x0F) << 12)
#define CMD6_ARG_MODE(x)              (((x) & 0x01) << 31)

/* Function Group 1 */
#define CMD6_ACCESS_MODE_SDR12        0x0 /* Default    */
#define CMD6_ACCESS_MODE_SDR25        0x1 /* High-Speed */
#define CMD6_ACCESS_MODE_SDR50        0x2
#define CMD6_ACCESS_MODE_SDR104       0x3
#define CMD6_ACCESS_MODE_DDR50        0x4
#define CMD6_ACCESS_MODE_CURRENT      0xF

/* Function Group 2 */
#define CMD6_COMMAND_SYSTEM_DEFAULT   0x0 /* Default */
#define CMD6_COMMAND_SYSTEM_EC        0x1
#define CMD6_COMMAND_SYSTEM_OTP       0x3 
#define CMD6_COMMAND_SYSTEM_ASSD      0x4
#define CMD6_COMMAND_SYSTEM_CURRENT   0xF

/* Function Group 3 */
#define CMD6_DRIVER_STRENGTH_B        0x0 /* Default */
#define CMD6_DRIVER_STRENGTH_A        0x1
#define CMD6_DRIVER_STRENGTH_C        0x2
#define CMD6_DRIVER_STRENGTH_D        0x3
#define CMD6_DRIVER_STRENGTH_CURRENT  0xF

/* Function Group 4 */
#define CMD6_POWER_LIMIT_0_72W        0x0 /* Default */
#define CMD6_POWER_LIMIT_1_44W        0x1
#define CMD6_POWER_LIMIT_2_16W        0x2
#define CMD6_POWER_LIMIT_2_88W        0x3
#define CMD6_POWER_LIMIT_1_80W        0x4
#define CMD6_POWER_LIMIT_CURRENT      0xF

/* Function Mode */
#define CMD6_MODE_CHECK               0x0
#define CMD6_MODE_SET                 0x1

typedef struct SD_CMD6_ARG {
  uint32_t mode             : 1;
  uint32_t power_limit      : 4;
  uint32_t driver_strength  : 4;
  uint32_t command_system   : 4;
  uint32_t access_mode      : 4;
  uint32_t rsvd             : 15;
} SWITCH_FUNC_ARG;

/* SD/MMC CSD register (adapted and precalculated) */
typedef struct _fsCSD_Register {
  uint32_t BlCnt;
  uint16_t BlReadLen;
  uint16_t BlWriteLen;
  uint16_t TranSpeed;
  uint16_t Rsvd;
} fsCSD_Register;

#endif /* __FS_MC_H */
