/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_memory_card.h
 * Purpose: Memory Card Layer Definitions for File System
 *----------------------------------------------------------------------------*/

#ifndef FS_MEMORY_CARD_H__
#define FS_MEMORY_CARD_H__

#include "fs_core_lib.h"
#include "Driver_MCI.h"
#include "Driver_SPI.h"

/* Memory Card Device Control block (MCI) */
typedef struct {
  ARM_DRIVER_MCI       *Driver;         /* Registered MCI driver              */
  ARM_MCI_SignalEvent_t Callback;       /* Driver Callback                    */
  ARM_MCI_CAPABILITIES  Capabilities;   /* Driver Capabilities                */
  uint8_t              *ExtCSD;         /* Pointer to 512 byte buffer         */
  uint32_t              Property;       /* Card Properties                    */
  uint32_t              SerialNumber;   /* Memory card serial number          */
  uint32_t              SectorCount;    /* Device density in 512B sectors     */
  uint16_t              RCA;            /* Relative Card Address              */
  uint16_t volatile     Event;          /* Driver event flags                 */
  uint8_t  volatile     MediaStatus;    /* Media status                       */
  uint8_t               Status;         /* Device status                      */
  uint8_t               Instance;       /* Memory Card Instance number        */
  uint8_t               Reserved;       /* Reserved for future use            */
} MC_MCI;

/* Memory Card Device Control block (SPI) */
typedef struct {
  ARM_DRIVER_SPI       *Driver;         /* Registered SPI driver              */
  ARM_SPI_SignalEvent_t Callback;       /* Driver Callback                    */
  uint8_t              *ExtCSD;         /* Pointer to 512 byte buffer         */
  uint32_t              Property;       /* Card Properties                    */
  uint32_t              SerialNumber;   /* Memory card serial number          */
  uint32_t              SectorCount;    /* Device density in 512B sectors     */
  uint16_t              RCA;            /* Relative Card Address              */
  uint8_t volatile      Event;          /* Driver event flags                 */
  uint8_t volatile      MediaStatus;    /* Media status                       */
  uint8_t               Status;         /* Device status                      */
  uint8_t               Instance;       /* Memory Card Instance number        */
  uint8_t               Reserved[2];    /* Reserved for future use            */
} MC_SPI;

/* Memory Card SPI mode interface functions */
extern uint32_t mc_spi_Init        (uint32_t mode, MC_SPI *mc);
extern uint32_t mc_spi_UnInit      (uint32_t mode, MC_SPI *mc);
extern uint32_t mc_spi_ReadSector  (uint32_t sect, uint8_t *buf, uint32_t cnt, MC_SPI *mc);
extern uint32_t mc_spi_WriteSector (uint32_t sect, const uint8_t *buf, uint32_t cnt, MC_SPI *mc);
extern uint32_t mc_spi_ReadInfo    (fsMediaInfo *info, MC_SPI *mc);
extern fsStatus mc_spi_DevCtrl     (fsDevCtrlCode code, void *p, MC_SPI *mc);
extern void     mc_spi_DrvEvent    (uint32_t event, MC_SPI *mc);

/* Memory Card Native mode interface functions */
extern uint32_t mc_mci_Init        (uint32_t mode, MC_MCI *mc);
extern uint32_t mc_mci_UnInit      (uint32_t mode, MC_MCI *mc);
extern uint32_t mc_mci_ReadSector  (uint32_t sect, uint8_t *buf, uint32_t cnt, MC_MCI *mc);
extern uint32_t mc_mci_WriteSector (uint32_t sect, const uint8_t *buf, uint32_t cnt, MC_MCI *mc);
extern uint32_t mc_mci_ReadInfo    (fsMediaInfo *info, MC_MCI *mc);
extern fsStatus mc_mci_DevCtrl     (fsDevCtrlCode code, void *p, MC_MCI *mc);
extern void     mc_mci_DrvEvent    (uint32_t event, MC_MCI *mc);

#endif /* FS_MEMORY_CARD_H__ */
