/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_nand_media.c
 * Purpose: NAND Media Driver
 *----------------------------------------------------------------------------*/

#include "cmsis_compiler.h"

#include "fs_nand_media.h"
#include "fs_evr.h"


/**
  NAND device ready timeout in us
  \note Max reset timeout for raw NAND is 5ms but EZ NAND may require up to 25ms.
*/
#define NAND_READY_TIMEOUT  25000

/**
  NAND driver ready timeout in us
*/
#define NAND_DRIVER_TIMEOUT 1000000

/* ECC info page size decoding */
static const uint16_t ECC_PgSz[] = {528, 1056, 2112, 4224, 8448,
                                    540, 1080, 2160, 4320, 8640};

/**
  Callback function to handle NAND Driver events (IRQ context)

  \param[in]  event NAND interface event
  \param[in]  h     NAND Media Handle
*/
__WEAK void nand_DrvEvent (uint32_t event, NAND_MEDIA_HANDLE *h) {

  h->event |= event;
}


/**
  Setup page layout from available ECC configuration

  \param[out] layout Page layout configuration
  \param[in]  ecc    ECC information structure
  \return     execution status
*/
int32_t NAND_EccToLayout (NAND_PAGE_LAYOUT *layout, ARM_NAND_ECC_INFO const *ecc) {
  uint32_t sect;
  uint32_t pgsz;
  uint32_t spsz;
  uint32_t dasz;

  if (ecc->page_size < 15) {
    /* Member page_size value should be verified already */
    if (ecc->page_size <= 4) {
      pgsz = ECC_PgSz[ecc->page_size];
    }
    else {
      pgsz = ECC_PgSz[ecc->page_size - 4];
    }

    /* Determine number of sectors per virtual page */
    sect = pgsz / 512;

    /* Determine virtual data area size */
    dasz = 512 * sect;

    /* Determine virtual spare area size */
    spsz = (pgsz - dasz) / sect;
  }
  else {
    /* (ecc.page_size == 15)*/
    dasz = ecc->virtual_page_size[0];
    spsz = ecc->virtual_page_size[1];
  }

  /* Setup page layout */
  layout->sector_inc = ecc->codeword_size[0] + ecc->codeword_gap[0];
  layout->spare_inc  = (uint16_t)spsz;

  if (ecc->page_layout == 0) {
    /* 0=|Main0|Spare0|...|MainN-1|SpareN-1| */
    layout->spare_ofs = (uint16_t)dasz;
  } else {
    /* 1=|Main0|...|MainN-1|Spare0|...|SpareN-1| */
    layout->spare_ofs = (uint16_t)(dasz << ecc->page_count);
  }

  /* Setup spare layout */
  layout->spare.ofs_ecc = (uint8_t)ecc->ecc_offset[0];

  if (ecc->codeword_size[1] >= 6) {
    /* 6 bytes or more available, our spare metadata fits directly */
    layout->spare.ofs_bbm = (uint8_t)(0 + ecc->codeword_offset[1]);
    layout->spare.ofs_dcm = (uint8_t)(1 + ecc->codeword_offset[1]);
    layout->spare.ofs_lsn = (uint8_t)(2 + ecc->codeword_offset[1]);
  }
  else {
    /* Less than 6 bytes available, but at least 4 */
    layout->spare.ofs_lsn = (uint8_t)ecc->codeword_offset[1];

    if (ecc->codeword_offset[1] >= 2) {
      layout->spare.ofs_bbm = 0;
      layout->spare.ofs_dcm = 1;
    }
    else if ((ecc->codeword_offset[1] + ecc->codeword_size[1] + 2) < ecc->ecc_offset[0]) {
      layout->spare.ofs_bbm = (uint8_t)(0 + ecc->codeword_offset[1] + ecc->codeword_size[1]);
      layout->spare.ofs_dcm = (uint8_t)(1 + ecc->codeword_offset[1] + ecc->codeword_size[1]);
    }
    else {
      /* Unsupported */
      return (NAND_ERROR);
    }
  }

  return (NAND_OK);
}


/**
  Inquiry ECC data, select ECC mode and setup page layout

  \param[in]  h  NAND Media Handle
  \return     execution status
*/
int32_t NAND_SetupLayout (NAND_MEDIA_HANDLE *h) {
  ARM_DRIVER_NAND  *drv = h->hw->drv;
  ARM_NAND_ECC_INFO ecc;
  uint32_t sect;
  uint32_t pgsz;
  uint32_t cnt;
  int32_t  idx;
  int32_t  status;

  h->ecc = 0;

  /* Determine number of sectors per page */
  sect = h->dev->page_size / 512;

  idx = 0;
  while (drv->InquireECC (idx, &ecc) == ARM_DRIVER_OK) {
    if (ecc.correctable_bits == h->ecc_req) {
      /* Correctability level fits */
      if ((ecc.type == ECC_TYPE_ECC0_DATA_SPARE) || (ecc.type == ECC_TYPE_ECC0_DATA_ECC1_SPARE)) {
        /* We need Main codeword of 512 bytes and Spare codeword to cover at least 4 bytes */
        if ((ecc.codeword_size[0] == 512) && (ecc.codeword_size[1] >= 4)) {
          /* Check if algorithm covers all sectors within page */
          cnt = 1U << ecc.page_count;

          if (cnt == sect) {
            /* Check if page size fits */
            if (ecc.page_size < 15) {
              if (ecc.page_size <= 4) {
                pgsz = ECC_PgSz[ecc.page_size];
              }
              else if ((ecc.page_size >= 8) && (ecc.page_size <= 12)) {
                pgsz = ECC_PgSz[ecc.page_size - 4];
              }
              else {
                break;
              }
            }
            else {
              /* (ecc.page_size == 15)*/
              pgsz = (ecc.virtual_page_size[0] + ecc.virtual_page_size[1]) * cnt;
            }

            if (pgsz == h->dev->page_size) {
              /* Page size fits */
              if (ecc.type == ECC_TYPE_ECC0_DATA_SPARE) {
                h->ecc = ARM_NAND_ECC0;
              } else {
                h->ecc = ARM_NAND_ECC0 | ARM_NAND_ECC1;
              }
              h->ecc |= ARM_NAND_ECC((uint32_t)idx);
              break;
            }
          }
        }
      }
    }
    idx++;
  }

  if (h->ecc != 0) {
    /* ECC algorithm (idx) selected */
    EvrFsNAND_DriverEccSelect (h->instance, drv, idx);

    status = NAND_EccToLayout (h->dev->page_layout, &ecc);
  }
  else {
    /* ECC inquiry failed */
    EvrFsNAND_DriverEccInquireError (h->instance, drv);
    status = NAND_ERROR;
  }

  return (status);
}

/**
  \fn          bool WaitDrvBusy (ARM_DRIVER_NAND *drv, uint32_t dev_num, uint32_t us)
  \brief       Wait until device driver is ready.
  \param[in]   drv      Device driver
  \param[in]   dev_num  Device number
  \return      true  - Driver is ready
               false - Driver is busy, timeout expired
*/
static bool WaitDrvBusy (ARM_DRIVER_NAND *drv, uint32_t dev_num) {
  uint32_t tick, tout;

  /* Wait until driver is busy */
  tout = fs_get_sys_tick_us (NAND_DRIVER_TIMEOUT);
  tick = fs_get_sys_tick();
  do {
    if (drv->GetStatus (dev_num).busy == 0) {
      break;
    }
  } while ((fs_get_sys_tick() - tick) < tout);

  if (drv->GetStatus (dev_num).busy == 0) {
    return true;
  }
  /* Timeout expired, driver busy */
  return false;
}


/**
  \fn          bool WaitDevReady (ARM_DRIVER_NAND *drv, uint32_t dev_num)
  \brief       Wait until device is ready.
  \param[in]   drv      Device driver
  \param[in]   dev_num  Device number
  \return      true  - Device is ready
               false - Device is busy, timeout expired
*/
static bool WaitDevReady (ARM_DRIVER_NAND *drv, uint32_t dev_num) {
  uint32_t status;
  uint32_t tick, tout;

  /* Wait until device is busy */
  status = 0;
  tout   = fs_get_sys_tick_us (NAND_READY_TIMEOUT);
  tick   = fs_get_sys_tick();
  do {
    /* Read Status Register */
    drv->SendCommand (dev_num, NAND_CMD_STATUS);
    drv->ReadData (dev_num, &status, 1, 0);

    if (status & NAND_STAT_RDY) {
      break;
    }
  } while ((fs_get_sys_tick() - tick) < tout);

  if ((status & NAND_STAT_RDY) == 0) {
    /* Read Status Register */
    drv->SendCommand (dev_num, NAND_CMD_STATUS);
    drv->ReadData (dev_num, &status, 1, 0);
  }

  if (status & NAND_STAT_RDY) {
    return true;
  }
  /* Timeout expired, device busy */
  return false;
}


/**
  Send address bytes to the NAND device

  \param[in]   drv      Device driver
  \param[in]   dev_num  Device number
  \param[in]   addr     Address to send
  \param[in]   cycles   Number of address bytes to send
*/
static void SendAddress (ARM_DRIVER_NAND *drv, uint32_t dev_num, uint32_t addr, uint32_t cycles) {

  while (cycles--) {
    drv->SendAddress (dev_num, (uint8_t)addr);
    addr >>= 8;
  }
}


/* ------------------------- Exported functions ----------------------------- */


/**
  \fn         int32_t Initialize (NAND_MEDIA_HANDLE *h)
  \brief      Initialize NAND Interface.
  \param[in]  h  NAND Media Handle
  \return     execution status
*/
static int32_t Initialize (NAND_MEDIA_HANDLE *h) {
  ARM_DRIVER_NAND *drv     = h->hw->drv;
  uint32_t         dev_num = h->dev->device_number;
  uint32_t         volt, bus;
  int32_t          rval;

  /* Initializing NAND media layer */
  EvrFsNAND_Init (h->instance);

  rval = NAND_OK;

  if (h->hw->init == 0) {
    /* Initializing driver %S */
    EvrFsNAND_InitDriver (h->instance, drv);

    h->hw->capabilities = drv->GetCapabilities();

    if (drv->Initialize (h->hw->hw_cb) != ARM_DRIVER_OK) {
      /* Driver initialization failed */
      EvrFsNAND_InitDriverError (h->instance, drv);
      rval = NAND_ERROR;
    }
    else {
      if (drv->PowerControl (ARM_POWER_FULL) != ARM_DRIVER_OK) {
        /* Driver power control failed */
        EvrFsNAND_DriverPowerCtrlError (h->instance, drv, ARM_POWER_FULL);
        rval = NAND_ERROR;
      }
      else {
        /* Turn ON device power */
        volt = 0;
        if (h->hw->capabilities.vcc)     { volt |= ARM_NAND_POWER_VCC_3V3;  }
        if (h->hw->capabilities.vcc_1v8) { volt |= ARM_NAND_POWER_VCC_1V8;  }
        if (h->hw->capabilities.vccq)    { volt |= ARM_NAND_POWER_VCCQ_3V3; }
        if (h->hw->capabilities.vccq_1v8){ volt |= ARM_NAND_POWER_VCCQ_1V8; }

        if (volt) {
          /* Setting device power */
          EvrFsNAND_DriverDevPowerSelect (h->instance, drv, volt);
          drv->DevicePower (volt);
        }
      }
    }

    if (rval == NAND_OK) {
      /* Setting bus mode: %d */
      EvrFsNAND_DriverBusModeSelect (h->instance, drv, ARM_NAND_BUS_SDR);

      drv->Control (dev_num, ARM_NAND_BUS_MODE, ARM_NAND_BUS_SDR);

      /* Set data bus width */
      if (h->dev->bus_width) {
        if (h->hw->capabilities.data_width_16 == 0) {
          EvrFsNAND_DriverBusWidthError (h->instance, drv, ARM_NAND_BUS_DATA_WIDTH_16);
          /* Driver does not support 16 bit data bus */
          rval = NAND_ERROR_UNSUPPORTED;
        }
        bus = ARM_NAND_BUS_DATA_WIDTH_16;
      }
      else {
        bus = ARM_NAND_BUS_DATA_WIDTH_8;
      }

      if (rval == NAND_OK) {
        /* Setting bus data width */
        EvrFsNAND_DriverBusWidthSelect (h->instance, drv, bus);

        drv->Control (dev_num, ARM_NAND_BUS_DATA_WIDTH, bus);
      }
    }
  }

  if (rval == NAND_OK) {
    h->event  = 0;
    h->status = 0;
    h->ecc    = 0;

    h->hw->init++;
  }

  return (rval);
}


/**
  \fn         int32_t Uninitialize (NAND_MEDIA_HANDLE *h)
  \brief      De-initialize NAND Interface.
  \param[in]  h  NAND Media Handle
  \return     execution status
*/
static int32_t Uninitialize (NAND_MEDIA_HANDLE *h) {
  ARM_DRIVER_NAND *drv = h->hw->drv;
  uint32_t         volt;

  /* Uninitializing NAND media layer */
  EvrFsNAND_Uninit (h->instance);

  if (--h->hw->init == 0) {
    /* Turn OFF device power */
    volt = 0;
    if (h->hw->capabilities.vcc)  { volt |= ARM_NAND_POWER_VCC_OFF;  }
    if (h->hw->capabilities.vccq) { volt |= ARM_NAND_POWER_VCCQ_OFF; }
    if (volt) {
      drv->DevicePower (volt);
    }
    drv->PowerControl (ARM_POWER_OFF);
    drv->Uninitialize ();
  }

  return (NAND_OK);
}

/*
  Reset the device (ExecuteSequence interface).
*/
static int32_t ResetDevice_Seq (NAND_MEDIA_HANDLE *h) {
  ARM_DRIVER_NAND *drv = h->hw->drv;

  h->seq = 1;

  drv->ExecuteSequence (h->dev->device_number,
                        ARM_NAND_CODE_SEND_CMD1,
                        NAND_CMD_RESET,
                        0,
                        0,
                        NULL,
                        0,
                        NULL,
                        &h->seq);
  /* Wait until done */
  if (WaitDrvBusy (drv, h->dev->device_number) == false) {
    EvrFsNAND_DriverTimeoutError (h->instance, drv);
    return (NAND_ERROR_TIMEOUT);
  }

  return (NAND_OK);
}

/*
  Reset the device (Bus interface).
*/
static int32_t ResetDevice_Cmd (NAND_MEDIA_HANDLE *h) {
  ARM_DRIVER_NAND *drv     = h->hw->drv;
  uint32_t         dev_num = h->dev->device_number;

  drv->SendCommand (dev_num, NAND_CMD_RESET);

  return (NAND_OK);
}

/**
  Reset the device.

  Device reset will be performed using reset command (0xFF) which will put the
  target in its default power-up state and in the SDR data interface. Therefore
  host should be configured to the SDR data interface. Reset can take up to 5ms
  for RAW NAND device or up to 25ms for EZ NAND device.

  Function is non blocking and does not wait for device ready state.

  \param[in]  h  NAND Media Handle
  \return     execution status
*/
static int32_t ResetDevice (NAND_MEDIA_HANDLE *h) {
  int32_t rval;
  NAND_HW_DRIVER *hw = h->hw;

  /* Resetting device */
  EvrFsNAND_DeviceReset (h->instance);

  if (hw->capabilities.ce_manual) {
    hw->drv->ChipEnable (h->dev->device_number, true);
  }

  if (hw->capabilities.sequence_operation == 0) {
    rval = ResetDevice_Cmd (h);
  }
  else {
    rval = ResetDevice_Seq (h);
  }

  return (rval);
}


/*
  Read device ID (ExecuteSequence interface).
*/
static int32_t ReadID_Seq (NAND_MEDIA_HANDLE *h, uint8_t addr, uint8_t *buf, uint32_t len) {
  ARM_DRIVER_NAND *drv = h->hw->drv;
  uint32_t cmd;
  uint32_t code;

  cmd = NAND_CMD_READ_ID;

  code = ARM_NAND_CODE_SEND_CMD1      |
         ARM_NAND_CODE_SEND_ADDR_COL1 |
         ARM_NAND_CODE_READ_DATA      ;

  h->seq = 1;

  drv->ExecuteSequence (h->dev->device_number,
                        code,
                        cmd,
                        addr,
                        0,
                        (void *)buf,
                        len,
                        NULL,
                        &h->seq);
  /* Wait until done */
  if (WaitDrvBusy (drv, h->dev->device_number) == false) {
    EvrFsNAND_DriverTimeoutError (h->instance, drv);
    return (NAND_ERROR_TIMEOUT);
  }
  return (NAND_OK);
}

/*
  Read device ID (Bus interface).
*/
static int32_t ReadID_Cmd (NAND_MEDIA_HANDLE *h, uint8_t addr, uint8_t *buf, uint32_t len) {
  ARM_DRIVER_NAND *drv     = h->hw->drv;
  uint32_t         dev_num = h->dev->device_number;

  drv->SendCommand (dev_num, NAND_CMD_READ_ID);

  drv->SendAddress (dev_num, addr);

  if (drv->ReadData (dev_num, buf, len, 0) != (int32_t)len) {
    /* Wait until done */
    if (WaitDrvBusy (drv, dev_num) == false) {
      EvrFsNAND_DriverTimeoutError (h->instance, drv);
      return (NAND_ERROR_TIMEOUT);
    }
  }

  return (NAND_OK);
}
/**
  Read device ID.

  Only addresses of 0x00 and 0x20 are valid. 4 byte ONFI signature can be read
  using address of 0x20. To determine JEDEC manufacturer ID and the device ID
  (total of 2 bytes) address of 0x00 must be used.
  Reading beyond 4 or 2 bytes yields indeterminate/manufacturer specific values.

  \param[in]  h     NAND Media Handle
  \param[in]  addr  Device ID address
  \param[out] buf   Buffer for data
  \param[in]  len   Number of bytes to read
  \return     execution status
*/
static int32_t ReadID (NAND_MEDIA_HANDLE *h, uint8_t addr, uint8_t *buf, uint32_t len) {
  int32_t rval;
  NAND_HW_DRIVER *hw = h->hw;

  /* Reading ID */
  EvrFsNAND_IdRead (h->instance, addr, len);

  if (hw->capabilities.ce_manual) {
    hw->drv->ChipEnable (h->dev->device_number, true);
  }

  if (hw->capabilities.sequence_operation == 0) {
    rval = ReadID_Cmd (h, addr, buf, len);
  }
  else {
    rval = ReadID_Seq (h, addr, buf, len);
  }

  return (rval);
}


/*
  Read NAND parameter page (ExecuteSequence interface).
*/
static int32_t ReadParamPage_Seq (NAND_MEDIA_HANDLE *h, uint32_t col, uint8_t *buf, uint32_t len) {
  ARM_DRIVER_NAND *drv = h->hw->drv;
  uint32_t cmd;
  uint32_t code;

  cmd = NAND_CMD_READ_PARAM_PAGE;

  code = ARM_NAND_CODE_SEND_CMD1      |
         ARM_NAND_CODE_SEND_ADDR_COL1 |
         ARM_NAND_CODE_WAIT_BUSY      |
         ARM_NAND_CODE_READ_DATA      ;

  h->seq = 1;

  drv->ExecuteSequence (h->dev->device_number,
                        code,
                        cmd,
                        col,
                        0,
                        (void *)buf,
                        len,
                        &h->status,
                        &h->seq);
  /* Wait until done */
  if (WaitDrvBusy (drv, h->dev->device_number) == false) {
    EvrFsNAND_DriverTimeoutError (h->instance, drv);
    return (NAND_ERROR_TIMEOUT);
  }
  return (NAND_OK);
}

/*
  Read NAND parameter page (Bus interface).
*/
static int32_t ReadParamPage_Cmd (NAND_MEDIA_HANDLE *h, uint32_t col, uint8_t *buf, uint32_t len) {
  ARM_DRIVER_NAND *drv     = h->hw->drv;
  uint32_t         dev_num = h->dev->device_number;

  (void)col;

  /* Send Read Parameter Page command */
  drv->SendCommand (dev_num, NAND_CMD_READ_PARAM_PAGE);

  /* Send address */
  SendAddress (drv, dev_num, 0, 1);

  /* Wait until device ready */
  if (WaitDevReady (drv, dev_num) == false) {
    EvrFsNAND_DeviceTimeoutError (h->instance, drv, dev_num);
    return NAND_ERROR_TIMEOUT;
  }
  /* Switch back to Read operation */
  drv->SendCommand (dev_num, NAND_CMD_READ_1ST);

  /* Read parameter page */
  if (drv->ReadData (dev_num, buf, len, 0) != (int32_t)len) {
    /* Wait until done */
    if (WaitDrvBusy (drv, dev_num) == false) {
      EvrFsNAND_DriverTimeoutError (h->instance, drv);
      return (NAND_ERROR_TIMEOUT);
    }
  }

  return (NAND_OK);
}

/**
  \fn          int32_t ReadParamPage (NAND_MEDIA_HANDLE *h,
                                      uint32_t           col,
                                      uint8_t           *buf,
                                      uint32_t           len)
  \brief       Read NAND parameter page.
  \param[in]   h    NAND Media Handle
  \param[in]   col  Column address
  \param[out]  buf  Buffer for data read from NAND
  \param[in]   len  Number of bytes to read (buffer length)
  \return      execution status
*/
static int32_t ReadParamPage (NAND_MEDIA_HANDLE *h, uint32_t col, uint8_t *buf, uint32_t len) {
  int32_t rval;

  /* Reading parameter page */
  EvrFsNAND_ParamPageRead (h->instance, col, len);

  if (h->hw->capabilities.sequence_operation == 0) {
    rval = ReadParamPage_Cmd (h, col, buf, len);
  }
  else {
    rval = ReadParamPage_Seq (h, col, buf, len);
  }

  return (rval);
}


/*
  Read data from NAND page (ExecuteSequence interface).
*/
static int32_t ReadPage_Seq (NAND_MEDIA_HANDLE *h, uint32_t row, uint32_t col, const uint8_t *buf, uint32_t len) {
  ARM_DRIVER_NAND *drv = h->hw->drv;
  uint32_t cmd;
  uint32_t code;

  cmd = NAND_CMD_READ_1ST | (NAND_CMD_READ_2ND << 8);

  code = ARM_NAND_CODE_SEND_CMD1      |
         ARM_NAND_CODE_SEND_ADDR_COL1 |
         ARM_NAND_CODE_SEND_ADDR_ROW1 |
         ARM_NAND_CODE_SEND_CMD2      |
         ARM_NAND_CODE_WAIT_BUSY      |
         ARM_NAND_CODE_READ_DATA      ;

  if (h->dev->col_cycles > 1) {
    code |= ARM_NAND_CODE_SEND_ADDR_COL2;
  }
  if (h->dev->row_cycles > 2) {
    code |= ARM_NAND_CODE_SEND_ADDR_ROW2 | ARM_NAND_CODE_SEND_ADDR_ROW3;
  }
  else if (h->dev->row_cycles > 1) {
    code |= ARM_NAND_CODE_SEND_ADDR_ROW2;
  }

  h->seq = 1;

  drv->ExecuteSequence (h->dev->device_number,
                        code | h->ecc,
                        cmd,
                        col,
                        row,
                        (void *)(uint32_t)buf,
                        len,
                        &h->status,
                        &h->seq);
  /* Wait until done */
  if (WaitDrvBusy (drv, h->dev->device_number) == false) {
    EvrFsNAND_DriverTimeoutError (h->instance, drv);
    return (NAND_ERROR_TIMEOUT);
  }
  return (NAND_OK);
}

/*
  Read data from NAND page (Bus interface).
*/
static int32_t ReadPage_Cmd (NAND_MEDIA_HANDLE *h, uint32_t row, uint32_t col, uint8_t *buf, uint32_t len) {
  ARM_DRIVER_NAND *drv     = h->hw->drv;
  uint32_t         dev_num = h->dev->device_number;

  /* Send Read 1st command */
  drv->SendCommand (dev_num, NAND_CMD_READ_1ST);

  /* Send address */
  SendAddress (drv, dev_num, col, h->dev->col_cycles);
  SendAddress (drv, dev_num, row, h->dev->row_cycles);

  /* Send Read 2nd command */
  drv->SendCommand (dev_num, NAND_CMD_READ_2ND);

  /* Wait until device ready */
  if (WaitDevReady (drv, dev_num) == false) {
    EvrFsNAND_DeviceTimeoutError (h->instance, drv, dev_num);
    return NAND_ERROR_TIMEOUT;
  }

  /* Switch back to Read operation */
  drv->SendCommand (dev_num, NAND_CMD_READ_1ST);

  /* Transfer data from the NAND chip */
  if (drv->ReadData (dev_num, buf, len, ARM_NAND_DRIVER_DONE_EVENT | h->ecc) != (int32_t)len) {
    /* Wait until done */
    if (WaitDrvBusy (drv, h->dev->device_number) == false) {
      EvrFsNAND_DriverTimeoutError (h->instance, drv);
      return (NAND_ERROR_TIMEOUT);
    }
  }

  return (NAND_OK);
}

/**
  \fn          int32_t ReadPage (NAND_MEDIA_HANDLE *h,
                                 uint32_t           row,
                                 uint32_t           col,
                                 uint8_t           *buf,
                                 uint32_t           len)
  \brief       Read data from NAND page.
  \param[in]   h    NAND Media Handle
  \param[in]   row  Row address
  \param[in]   col  Column address
  \param[out]  buf  Buffer for data read from NAND
  \param[in]   len  Number of bytes to read (buffer length)
  \return      execution status
*/
static int32_t ReadPage (NAND_MEDIA_HANDLE *h, uint32_t row, uint32_t col, uint8_t *buf, uint32_t len) {
  int32_t rval;
  NAND_HW_DRIVER *hw = h->hw;

  /* Reading page */
  EvrFsNAND_PageRead (h->instance, row, col, len);

  if (hw->capabilities.ce_manual) {
    hw->drv->ChipEnable (h->dev->device_number, true);
  }

  if (h->dev->bus_width) {
    len >>= 1;
  }

  if (hw->capabilities.sequence_operation == 0) {
    rval = ReadPage_Cmd (h, row, col, buf, len);
  }
  else {
    rval = ReadPage_Seq (h, row, col, buf, len);
  }

  return (rval);
}


/*
  Write data to NAND page (ExecuteSequence interface).
*/
static int32_t WritePage_Seq (NAND_MEDIA_HANDLE *h, uint32_t row, uint32_t col, const uint8_t *buf, uint32_t len) {
  ARM_DRIVER_NAND *drv = h->hw->drv;
  uint32_t cmd;
  uint32_t code;

  cmd = NAND_CMD_PROGRAM_1ST | (NAND_CMD_PROGRAM_2ND << 8);

  code = ARM_NAND_CODE_SEND_CMD1      |
         ARM_NAND_CODE_SEND_ADDR_COL1 |
         ARM_NAND_CODE_SEND_ADDR_ROW1 |
         ARM_NAND_CODE_WRITE_DATA     |
         ARM_NAND_CODE_SEND_CMD2      ;

  if (h->dev->col_cycles > 1) {
    code |= ARM_NAND_CODE_SEND_ADDR_COL2;
  }
  if (h->dev->row_cycles > 2) {
    code |= ARM_NAND_CODE_SEND_ADDR_ROW2 | ARM_NAND_CODE_SEND_ADDR_ROW3;
  }
  else if (h->dev->row_cycles > 1) {
    code |= ARM_NAND_CODE_SEND_ADDR_ROW2;
  }

  h->seq = 1;

  drv->ExecuteSequence (h->dev->device_number,
                        code | h->ecc,
                        cmd,
                        col,
                        row,
                        (void *)(uint32_t)buf,
                        len,
                        &h->status,
                        &h->seq);

  /* Wait until done */
  if (WaitDrvBusy (drv, h->dev->device_number) == false) {
    EvrFsNAND_DriverTimeoutError (h->instance, drv);
    return (NAND_ERROR_TIMEOUT);
  }

  return (NAND_OK);
}

/*
  Write data to NAND page (Bus interface).
*/
static int32_t WritePage_Cmd (NAND_MEDIA_HANDLE *h, uint32_t row, uint32_t col, const uint8_t *buf, uint32_t len) {
  ARM_DRIVER_NAND *drv     = h->hw->drv;
  uint32_t         dev_num = h->dev->device_number;

  /* Send Program 1st command */
  drv->SendCommand (dev_num, NAND_CMD_PROGRAM_1ST);

  /* Send address */
  SendAddress (drv, dev_num, col, h->dev->col_cycles);
  SendAddress (drv, dev_num, row, h->dev->row_cycles);

  /* Transfer data to the NAND chip */
  if (drv->WriteData (dev_num, buf, len, ARM_NAND_DRIVER_DONE_EVENT | h->ecc) != (int32_t)len) {
    /* Wait until done */
    if (WaitDrvBusy (drv, h->dev->device_number) == false) {
      EvrFsNAND_DriverTimeoutError (h->instance, drv);
      return (NAND_ERROR_TIMEOUT);
    }
  }

  /* Send Program 2nd command */
  drv->SendCommand (dev_num, NAND_CMD_PROGRAM_2ND);

  return (NAND_OK);
}

/**
  \fn          int32_t WritePage (      NAND_MEDIA_HANDLE *h
                                        uint32_t  row,
                                        uint32_t  col,
                                  const uint8_t  *buf,
                                        uint32_t  len)
  \brief       Write data to NAND page.
  \param[in]   h    NAND Media Handle
  \param[in]   row  Row address
  \param[in]   col  Column address
  \param[out]  buf  Buffer with data to write to NAND
  \param[in]   len  Number of bytes to write (buffer length)
  \return      execution status
*/
static int32_t WritePage (NAND_MEDIA_HANDLE *h, uint32_t row, uint32_t col, const uint8_t *buf, uint32_t len) {
  int32_t rval;
  NAND_HW_DRIVER *hw = h->hw;

  /* Writing page */
  EvrFsNAND_PageWrite (h->instance, row, col, len);

  if (hw->capabilities.ce_manual) {
    hw->drv->ChipEnable (h->dev->device_number, true);
  }

  if (h->dev->bus_width) {
    len >>= 1;
  }

  if (hw->capabilities.sequence_operation == 0) {
    rval = WritePage_Cmd (h, row, col, buf, len);
  }
  else {
    rval = WritePage_Seq (h, row, col, buf, len);
  }

  return (rval);
}


/*
  Erase block in NAND device (ExecuteSequence interface).
*/
static int32_t EraseBlock_Seq (NAND_MEDIA_HANDLE *h, uint32_t row) {
  ARM_DRIVER_NAND *drv = h->hw->drv;
  uint32_t cmd;
  uint32_t code;

  cmd  = NAND_CMD_ERASE_1ST | (NAND_CMD_ERASE_2ND << 8);

  code = ARM_NAND_CODE_SEND_CMD1      |
         ARM_NAND_CODE_SEND_ADDR_ROW1 |
         ARM_NAND_CODE_SEND_CMD2      ;

  if (h->dev->row_cycles > 2) {
    code |= ARM_NAND_CODE_SEND_ADDR_ROW2 | ARM_NAND_CODE_SEND_ADDR_ROW3;
  }
  else if (h->dev->row_cycles > 1) {
    code |= ARM_NAND_CODE_SEND_ADDR_ROW2;
  }

  h->seq = 1;

  drv->ExecuteSequence (h->dev->device_number,
                        code,
                        cmd,
                        0,
                        row,
                        NULL,
                        0,
                        &h->status,
                        &h->seq);
  /* Wait until done */
  if (WaitDrvBusy (drv, h->dev->device_number) == false) {
    EvrFsNAND_DriverTimeoutError (h->instance, drv);
    return (NAND_ERROR_TIMEOUT);
  }

  return (NAND_OK);
}

/*
  Erase block in NAND device (Bus interface).
*/
static int32_t EraseBlock_Cmd (NAND_MEDIA_HANDLE *h, uint32_t row) {
  ARM_DRIVER_NAND *drv     = h->hw->drv;
  uint32_t         dev_num = h->dev->device_number;

  /* Send Erase 1st command */
  drv->SendCommand (dev_num, NAND_CMD_ERASE_1ST);

  /* Send address */
  SendAddress (drv, dev_num, row, h->dev->row_cycles);

  /* Send Erase 2nd command */
  drv->SendCommand (dev_num, NAND_CMD_ERASE_2ND);

  return (NAND_OK);
}

/**
  \fn          int32_t EraseBlock (NAND_MEDIA_HANDLE *h, uint32_t row)
  \brief       Erase blocks in NAND device.
  \param[in]   h    NAND Media Handle
  \param[in]   row  Block start row address
  \return      execution status
*/
static int32_t EraseBlock (NAND_MEDIA_HANDLE *h, uint32_t row) {
  int32_t rval;
  NAND_HW_DRIVER *hw = h->hw;

  /* Erasing block: row=%d */
  EvrFsNAND_BlockErase (h->instance, row);

  if (hw->capabilities.ce_manual) {
    hw->drv->ChipEnable (h->dev->device_number, true);
  }

  if (hw->capabilities.sequence_operation == 0) {
    rval = EraseBlock_Cmd (h, row);
  }
  else {
    rval = EraseBlock_Seq (h, row);
  }

  return (rval);
}


/*
  Read device status byte (ExecuteSequence interface).
*/
static int32_t ReadStatus_Seq (NAND_MEDIA_HANDLE *h, uint8_t *stat) {
  ARM_DRIVER_NAND *drv = h->hw->drv;

  h->seq = 1;

  drv->ExecuteSequence (h->dev->device_number,
                        ARM_NAND_CODE_SEND_CMD1 | ARM_NAND_CODE_READ_STATUS,
                        NAND_CMD_STATUS,
                        0,
                        0,
                        NULL,
                        0,
                        &h->status,
                        &h->seq);
  /* Wait until done */
  if (WaitDrvBusy (drv, h->dev->device_number) == false) {
    EvrFsNAND_DriverTimeoutError (h->instance, drv);
    return (NAND_ERROR_TIMEOUT);
  }
  *stat = h->status;

  return (NAND_OK);
}

/*
  Read device status byte (Bus interface).
*/
static int32_t ReadStatus_Cmd (NAND_MEDIA_HANDLE *h, uint8_t *stat) {
  ARM_DRIVER_NAND *drv     = h->hw->drv;
  uint32_t         dev_num = h->dev->device_number;
  uint32_t         status  = 0;
  int32_t          num;

  /* Send Read Status command */
  drv->SendCommand (dev_num, NAND_CMD_STATUS);

  /* Read status byte */
  num = drv->ReadData (dev_num, &status, 1, 0);

  if (num == 0) {
    /* Wait until done */
    if (WaitDrvBusy (drv, dev_num) == false) {
      EvrFsNAND_DriverTimeoutError (h->instance, drv);
      return (NAND_ERROR_TIMEOUT);
    }
  }
  *stat = (uint8_t)status;

  return (NAND_OK);
}

/**
  Read device status byte.

  Function is blocking and it waits until status byte is retrieved.

  \fn          int32_t ReadStatus (NAND_MEDIA_HANDLE *h, uint8_t *stat)
  \brief       Read NAND device status.
  \param[in]   h     NAND Media Handle
  \param[out]  stat  Pointer to store status read from NAND
  \return      execution status
*/
static int32_t ReadStatus (NAND_MEDIA_HANDLE *h, uint8_t *stat) {
  int32_t rval;
  NAND_HW_DRIVER *hw = h->hw;

  if (hw->capabilities.ce_manual) {
    hw->drv->ChipEnable (h->dev->device_number, true);
  }

  if (hw->capabilities.sequence_operation == 0) {
    rval = ReadStatus_Cmd (h, stat);
  }
  else {
    rval = ReadStatus_Seq (h, stat);
  }
  /* Got device status: %x */
  EvrFsNAND_StatusRead (h->instance, *stat);

  return (rval);
}


/*
  Get features (ExecuteSequence interface).
*/
static int32_t GetFeatures_Seq (NAND_MEDIA_HANDLE *h, uint8_t addr, uint8_t *buf, uint32_t len) {
  ARM_DRIVER_NAND *drv = h->hw->drv;
  uint32_t cmd;
  uint32_t code;

  cmd = NAND_CMD_GET_FEATURES;

  code = ARM_NAND_CODE_SEND_CMD1      |
         ARM_NAND_CODE_SEND_ADDR_COL1 |
         ARM_NAND_CODE_WAIT_BUSY      |
         ARM_NAND_CODE_READ_DATA;

  h->seq = 1;

  drv->ExecuteSequence (h->dev->device_number,
                        code,
                        cmd,
                        addr,
                        0,
                        (void *)buf,
                        len,
                        NULL,
                        &h->seq);
  /* Wait until done */
  if (WaitDrvBusy (drv, h->dev->device_number) == false) {
    EvrFsNAND_DriverTimeoutError (h->instance, drv);
    return (NAND_ERROR_TIMEOUT);
  }

  return (NAND_OK);
}

/*
  Get features (Bus interface).
*/
static int32_t GetFeatures_Cmd (NAND_MEDIA_HANDLE *h, uint8_t addr, uint8_t *buf, uint32_t len) {
  ARM_DRIVER_NAND *drv     = h->hw->drv;
  uint32_t         dev_num = h->dev->device_number;
  int32_t          num;

  /* Send Read Status command */
  drv->SendCommand (dev_num, NAND_CMD_GET_FEATURES);

  /* Send address */
  SendAddress (drv, dev_num, addr, 1);

  /* Wait until device ready */
  if (WaitDevReady (drv, dev_num) == false) {
    EvrFsNAND_DeviceTimeoutError (h->instance, drv, dev_num);
    return NAND_ERROR_TIMEOUT;
  }

  /* Switch back to Read operation */
  drv->SendCommand (dev_num, NAND_CMD_READ_1ST);

  /* Read feature parameter bytes */
  num = drv->ReadData (dev_num, buf, len, 0);

  if (num != (int32_t)len) {
    /* Wait until done */
    if (WaitDrvBusy (drv, dev_num) == false) {
      EvrFsNAND_DriverTimeoutError (h->instance, drv);
      return (NAND_ERROR_TIMEOUT);
    }
  }

  return (NAND_OK);
}


/**
  Get features to determine the current settings for a particular feature

  \param[in]   h     NAND Media Handle
  \param[in]   addr  Feature address
  \param[out]  buf   Buffer address
  \param[in]   len   Number of bytes to read (should be 4)
  \return      execution status
*/
static int32_t GetFeatures (NAND_MEDIA_HANDLE *h, uint8_t addr, uint8_t *buf, uint32_t len) {
  int32_t rval;
  NAND_HW_DRIVER *hw = h->hw;

  /* Get Features */
  //EvrFsNAND_GetFeatures (h->instance);

  if (hw->capabilities.ce_manual) {
    hw->drv->ChipEnable (h->dev->device_number, true);
  }

  if (hw->capabilities.sequence_operation == 0) {
    rval = GetFeatures_Cmd (h, addr, buf, len);
  }
  else {
    rval = GetFeatures_Seq (h, addr, buf, len);
  }

  if (hw->capabilities.ce_manual) {
    hw->drv->ChipEnable (h->dev->device_number, false);
  }

  return (rval);
}


/*
  Set features (ExecuteSequence interface).
*/
static int32_t SetFeatures_Seq (NAND_MEDIA_HANDLE *h, uint8_t addr, const uint8_t *buf, uint32_t len) {
  ARM_DRIVER_NAND *drv = h->hw->drv;
  uint32_t cmd;
  uint32_t code;

  cmd = NAND_CMD_SET_FEATURES;

  code = ARM_NAND_CODE_SEND_CMD1      |
         ARM_NAND_CODE_SEND_ADDR_COL1 |
         ARM_NAND_CODE_WRITE_DATA;

  h->seq = 1;

  drv->ExecuteSequence (h->dev->device_number,
                        code,
                        cmd,
                        addr,
                        0,
                        (void *)(uint32_t)buf,
                        len,
                        NULL,
                        &h->seq);
  /* Wait until done */
  if (WaitDrvBusy (drv, h->dev->device_number) == false) {
    EvrFsNAND_DriverTimeoutError (h->instance, drv);
    return (NAND_ERROR_TIMEOUT);
  }

  return (NAND_OK);
}

/*
  Set features (Bus interface).
*/
static int32_t SetFeatures_Cmd (NAND_MEDIA_HANDLE *h, uint8_t addr, const uint8_t *buf, uint32_t len) {
  ARM_DRIVER_NAND *drv     = h->hw->drv;
  uint32_t         dev_num = h->dev->device_number;

  /* Send Set Features command */
  drv->SendCommand (dev_num, NAND_CMD_SET_FEATURES);

  /* Send address */
  SendAddress (drv, dev_num, addr, 1);

  /* Transfer data to the NAND chip */
  if (drv->WriteData (dev_num, buf, len, ARM_NAND_DRIVER_DONE_EVENT) != (int32_t)len) {
    /* Wait until done */
    if (WaitDrvBusy (drv, h->dev->device_number) == false) {
      EvrFsNAND_DriverTimeoutError (h->instance, drv);
      return (NAND_ERROR_TIMEOUT);
    }
  }

  return (NAND_OK);
}


/**
  Set features to modify the settings of a particular feature

  \param[in]   h     NAND Media Handle
  \param[in]   addr  Feature address
  \param[out]  buf   Buffer address
  \param[in]   len   Number of bytes to write (should be 4)
  \return      execution status
*/
static int32_t SetFeatures (NAND_MEDIA_HANDLE *h, uint8_t addr, const uint8_t *buf, uint32_t len) {
  int32_t rval;
  NAND_HW_DRIVER *hw = h->hw;

  /* Set Features */
  //EvrFsNAND_SetFeatures (h->instance);

  if (hw->capabilities.ce_manual) {
    hw->drv->ChipEnable (h->dev->device_number, true);
  }

  if (hw->capabilities.sequence_operation == 0) {
    rval = SetFeatures_Cmd (h, addr, buf, len);
  }
  else {
    rval = SetFeatures_Seq (h, addr, buf, len);
  }

  if (hw->capabilities.ce_manual) {
    hw->drv->ChipEnable (h->dev->device_number, false);
  }

  return (rval);
}


__WEAK NAND_MEDIA_DRIVER NAND_MediaDriver = {
  Initialize,
  Uninitialize,
  ResetDevice,
  ReadID,
  ReadParamPage,
  ReadPage,
  WritePage,
  NULL, /* CopyPage not implemented */
  EraseBlock,
  ReadStatus,
  GetFeatures,
  SetFeatures
};
