/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usb_msc.h
 * Purpose: USB Mass Storage Device Class Definitions
 *----------------------------------------------------------------------------*/

#ifndef __USB_MSC_H__
#define __USB_MSC_H__

#include <stdint.h>

#include "usb_compiler.h"


// MSC Subclass Codes
#define MSC_SUBCLASS_RBC                0x01U
#define MSC_SUBCLASS_SFF8020I_MMC2      0x02U
#define MSC_SUBCLASS_QIC157             0x03U
#define MSC_SUBCLASS_UFI                0x04U
#define MSC_SUBCLASS_SFF8070I           0x05U
#define MSC_SUBCLASS_SCSI               0x06U

// MSC Protocol Codes
#define MSC_PROTOCOL_CBI_INT            0x00U
#define MSC_PROTOCOL_CBI_NOINT          0x01U
#define MSC_PROTOCOL_BULK_ONLY          0x50U

// MSC Request Codes
#define MSC_REQUEST_RESET               0xFFU
#define MSC_REQUEST_GET_MAX_LUN         0xFEU

// MSC Bulk-only Stage
#define MSC_BS_CBW                      0U      // Command Block Wrapper
#define MSC_BS_DATA_OUT                 1U      // Data Out Phase
#define MSC_BS_DATA_IN                  2U      // Data In Phase
#define MSC_BS_DATA_IN_LAST             3U      // Data In Last Phase
#define MSC_BS_DATA_IN_LAST_STALL       4U      // Data In Last Phase with Stall
#define MSC_BS_CSW                      5U      // Command Status Wrapper
#define MSC_BS_ERROR                    6U      // Error
#define MSC_BS_RESET                    7U      // Bulk-Only Mass Storage Reset

// Bulk-only Command Block Wrapper
typedef __PACKED_STRUCT {
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataLength;
  uint8_t  bmFlags;
  uint8_t  bLUN;
  uint8_t  bCBLength;
  uint8_t  CB[16];
} MSC_CBW;

// Bulk-only Command Status Wrapper
typedef __PACKED_STRUCT {
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataResidue;
  uint8_t  bStatus;
} MSC_CSW;

#define MSC_CBW_Signature               0x43425355U
#define MSC_CSW_Signature               0x53425355U

// CSW Status Definitions
#define CSW_CMD_PASSED                  0x00U
#define CSW_CMD_FAILED                  0x01U
#define CSW_PHASE_ERROR                 0x02U

// SCSI Commands
#define SCSI_TEST_UNIT_READY            0x00U
#define SCSI_REQUEST_SENSE              0x03U
#define SCSI_FORMAT_UNIT                0x04U
#define SCSI_INQUIRY                    0x12U
#define SCSI_MODE_SELECT6               0x15U
#define SCSI_MODE_SENSE6                0x1AU
#define SCSI_START_STOP_UNIT            0x1BU
#define SCSI_MEDIA_REMOVAL              0x1EU
#define SCSI_READ_FORMAT_CAPACITIES     0x23U
#define SCSI_READ_CAPACITY              0x25U
#define SCSI_READ10                     0x28U
#define SCSI_WRITE10                    0x2AU
#define SCSI_VERIFY10                   0x2FU
#define SCSI_SYNC_CACHE10               0x35U
#define SCSI_READ12                     0xA8U
#define SCSI_WRITE12                    0xAAU
#define SCSI_MODE_SELECT10              0x55U
#define SCSI_MODE_SENSE10               0x5AU
#define SCSI_SYNC_CACHE16               0x91U
#define SCSI_ATA_COMMAND_PASS_THROUGH12 0xA1U
#define SCSI_ATA_COMMAND_PASS_THROUGH16 0x85U
#define SCSI_SERVICE_ACTION_IN12        0xABU
#define SCSI_SERVICE_ACTION_IN16        0x9EU
#define SCSI_SERVICE_ACTION_OUT12       0xA9U
#define SCSI_SERVICE_ACTION_OUT16       0x9FU
#define SCSI_REPORT_ID_INFO             0xA3U

#endif
