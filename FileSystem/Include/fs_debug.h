/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_debug.h
 * Purpose: File System Debug Configuration
 *----------------------------------------------------------------------------*/

#include <stdint.h>

#include "rl_fs_lib.h"
#include "EventRecorder.h"              // Keil.ARM Compiler::Compiler:Event Recorder
#include "fs_evr.h"

/*
  Debug Configuration values

  0 (Off):          EventRecordNone
  1 (Errors):       EventRecordError
  2 (Errors + API): EventRecordError | EventRecordAPI
  3 (All):          EventRecordAll
*/

#define FS_DEBUG_EVR_CFG (FS_DEBUG_EVR_CORE   | \
                          FS_DEBUG_EVR_FAT    | \
                          FS_DEBUG_EVR_EFS    | \
                          FS_DEBUG_EVR_IOC    | \
                          FS_DEBUG_EVR_NFTL   | \
                          FS_DEBUG_EVR_NAND   | \
                          FS_DEBUG_EVR_MC_MCI | \
                          FS_DEBUG_EVR_MC_SPI )

#if (FS_DEBUG_EVR_CFG > 3)
  #error "FS_Debug.c contains invalid Event Recorder filter configuration."
#endif

#define Level_Msk(cfg)                             \
 ((cfg == 0) ? EventRecordNone  :                  \
  (cfg == 1) ? EventRecordError :                  \
  (cfg == 2) ? EventRecordError | EventRecordAPI : \
               EventRecordAll)

/*-----------------------------------------------------------------------------
 *  Configure Event Recorder event level
 *----------------------------------------------------------------------------*/
void fs_config_evr (void) {

  EventRecorderDisable(EventRecordAll, EvtFsCore_No, EvtFsMcSPI_No);

#if (FS_DEBUG_EVR_ENABLE != 0)
  /* Apply Event Recorder Filter configuration */
  EventRecorderEnable (Level_Msk(FS_DEBUG_EVR_CORE),   EvtFsCore_No,  EvtFsCore_No);
  EventRecorderEnable (Level_Msk(FS_DEBUG_EVR_FAT),    EvtFsFAT_No,   EvtFsFAT_No);
  EventRecorderEnable (Level_Msk(FS_DEBUG_EVR_EFS),    EvtFsEFS_No,   EvtFsEFS_No);
  EventRecorderEnable (Level_Msk(FS_DEBUG_EVR_IOC),    EvtFsIOC_No,   EvtFsIOC_No);
  EventRecorderEnable (Level_Msk(FS_DEBUG_EVR_NFTL),   EvtFsNFTL_No,  EvtFsNFTL_No);
  EventRecorderEnable (Level_Msk(FS_DEBUG_EVR_NAND),   EvtFsNAND_No,  EvtFsNAND_No);
  EventRecorderEnable (Level_Msk(FS_DEBUG_EVR_MC_MCI), EvtFsMcMCI_No, EvtFsMcMCI_No);
  EventRecorderEnable (Level_Msk(FS_DEBUG_EVR_MC_SPI), EvtFsMcSPI_No, EvtFsMcSPI_No);
#endif /* (FS_DEBUG_EVR_ENABLE != 0) */
}
