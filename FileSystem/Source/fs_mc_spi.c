/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_mc_spi.c
 * Purpose: MMC/SD Card SPI-Mode Interface
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "cmsis_compiler.h"

#include "fs_mc.h"
#include "fs_evr.h"


/* Wait timeouts, in multiples of 1 byte send over SPI */
#define WR_TOUT          1000000
#define STOP_TOUT         500000
#define CMD_TOUT           25000

#define MC_XFER_TOUT              2000000 /* Event timeout in us (2 seconds) */
#define SPI_BUS_FREQ_ERROR_DELTA  1000000 /* Bus freq delta limit in Hz (1 MHz) */


/**
  Callback function used to retrieve password management information
  in order to manage locked memory card device during initialization.

  Here is the extern definition - fs_mc_mci.c contains its weak implementation.

  \param[in]  drive_num                Memory Card Drive number
  \return     pointer to fsLockUnlock structure or NULL
*/
extern fsLockUnlock *fs_mc_lock_unlock (uint32_t drive_num);

/**
  Handle SPI events.

  CMSIS SPI driver only uses three LSB bits for events and therefore
  all SPI events fit into 8-bit variable.

  \param[in,out]  mc        memory card instance object
*/
void mc_spi_DrvEvent (uint32_t event, MC_SPI *mc) {
  mc->Event |= (uint8_t)event;
}


/**
  Callback function used to read Card Detect (CD) pin state when
  Memory Card is used in SPI mode.
  \param[in]  drive_num                Memory Card Drive number
  \return     1:card detected, 0:card not detected, or error
*/
__WEAK int32_t fs_mc_read_cd (uint32_t drive_num) {
  (void)drive_num;

  return (-1);
}


/**
  Callback function used to read Write Protect (WP) pin state when
  Memory Card is used in SPI mode.
  \param[in]  drive_num                Memory Card Drive number
  \return     1:write protected, 0:not write protected, or error
*/
__WEAK int32_t fs_mc_read_wp (uint32_t drive_num) {
  (void)drive_num;

  return (-1);
}


/**
  Callback function used to control Slave Select signal when
  Memory Card is used in SPI mode.
  \param[in]  drive_num                Memory Card Drive number
  \param[in]  ss                       Slave select signal state (0=inactive, 1=active)
  \return     slave select signal state
*/
__WEAK int32_t fs_mc_spi_control_ss (uint32_t drive_num, uint32_t ss) {
  (void)drive_num;
  (void)ss;

  return (-1);
}


/**
  Update state of MCI switches

  \param[in,out]  mc        memory card instance object
*/
static void mc_spi_update_sw_state (MC_SPI *mc) {
  int32_t cd, wp;

  cd = fs_mc_read_cd(mc->Instance);
  wp = fs_mc_read_wp(mc->Instance);

  if ((cd < 0) && (wp < 0)) {
    mc->MediaStatus |= FS_MEDIA_NOCHKMEDIA;
  }
  else {
    if (cd > -1) {
      /* Get card detect switch state from bit field */
      if (cd != 0) {
        /* Media is inserted */
        EvrFsMcSPI_MediaCdActive (mc->Instance);

        mc->MediaStatus |= FS_MEDIA_INSERTED;
      } else {
        /* Media is not inserted */
        EvrFsMcSPI_MediaCdInactive (mc->Instance);

        mc->MediaStatus &= ~(FS_MEDIA_INSERTED | FS_MEDIA_INITIALIZED);
      }
    }

    if (wp > -1) {
      /* Get write protect switch state from bit field */
      if (wp != 0) {
        /* Media write protection is active */
        EvrFsMcSPI_MediaWpActive (mc->Instance);

        mc->MediaStatus |= FS_MEDIA_PROTECTED;
      } else {
        /* Media write protection is not active */
        EvrFsMcSPI_MediaWpInactive (mc->Instance);

        mc->MediaStatus &= ~FS_MEDIA_PROTECTED;
      }
    }
  }
}


/**
  Control Slave Select signal.

  \param[in]  ss        slave select signal state (0=inactive, 1=active)
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_control_ss (uint32_t ss, MC_SPI *mc) {
  if (fs_mc_spi_control_ss (mc->Instance, ss) == -1) {
    mc->Driver->Control (ARM_SPI_CONTROL_SS, ss);
  }
  return (fsOK);
}


/**
  Control (set) SPI bus clock frequency.

  \param[in]  freq      bus clock frequency in Hz
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_control_bus_speed (uint32_t freq, MC_SPI *mc) {
  fsStatus status;

  EvrFsMcSPI_BusSpeedControl (mc->Instance, freq);
  if (mc->Driver->Control (ARM_SPI_SET_BUS_SPEED, freq) == ARM_DRIVER_OK) {
    status = fsOK;
  } else {
    status = fsError;
  }

  return (status);
}


/**
  Send data over the SPI

  \param[in]      data      data buffer
  \param[in]      cnt       number of data to send
  \param[in,out]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_send (const uint8_t *data, uint32_t cnt, MC_SPI *mc) {
  uint32_t tick, tout;
  fsStatus status;

  mc->Event = 0;
  mc->Driver->Send (data, cnt);

  tout = fs_get_sys_tick_us (MC_XFER_TOUT);
  tick = fs_get_sys_tick();
  do {
    if (mc->Event & ARM_SPI_EVENT_TRANSFER_COMPLETE) {
      break;
    }
  } while ((fs_get_sys_tick() - tick) < tout);

  if (mc->Event & ARM_SPI_EVENT_TRANSFER_COMPLETE) {
    status = fsOK;
  } else {
    status = fsDriverError;
    /* SPI send failed */
    EvrFsMcSPI_DriverSendError (mc->Instance, mc->Event);
  }
  return (status);
}


/**
  Receive data over the SPI

  \param[out]     data      data buffer
  \param[in]      cnt       number of data to receive
  \param[in,out]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_receive (uint8_t *data, uint32_t cnt, MC_SPI *mc) {
  uint32_t tick, tout;
  fsStatus status;

  mc->Event = 0;
  mc->Driver->Receive (data, cnt);

  tout = fs_get_sys_tick_us(MC_XFER_TOUT);
  tick = fs_get_sys_tick();
  do {
    if (mc->Event & ARM_SPI_EVENT_TRANSFER_COMPLETE) {
      break;
    }
  } while ((fs_get_sys_tick() - tick) < tout);

  if (mc->Event & ARM_SPI_EVENT_TRANSFER_COMPLETE) {
    status = fsOK;
  } else {
    status = fsDriverError;
    /* SPI receive failed */
    EvrFsMcSPI_DriverReceiveError (mc->Instance, mc->Event);
  }
  return (status);
}


/**
  Calculate CRC7.

  Polynomial: x^7 + x^3 + 1 (0x89)
  Init: 0x00

  \param[in]  buf       data buffer (6 bytes)
  \return crc calculated over given buffer
*/
static uint8_t mc_CRC7 (const uint8_t *buf) {
  uint32_t i;
  uint32_t crc = 0;

  for (i = 0; i < (6*8); i++) {
    if (!(i & 0x07)) {
      if ((i >> 3) < 5){ crc |= buf[i >> 3]; }
    }
    if (crc & 0x8000) { crc ^= 0x8900; }
    crc <<= 1;
  }

  return (uint8_t)(crc >> 8);
}


#if 0
/**
  Calculate CCITT CRC16.

  Polynomial: x^16 + x^12 + x^5 + 1 (0x1021)
  Init: 0x0000
*/
static uint16_t mc_CRC16 (const uint8_t *buf, uint32_t cnt) {
  uint16_t crc = 0;
  uint32_t i;

  while (cnt--) {
    crc ^= *buf++ << 8;

    for (i = 0; i < 8; i++) {
      crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }
  }

  return crc;
}
#endif


/**
  Send a command to the memory card

  \param[in]  cmd       command index
  \param[in]  arg       command argument
  \param[in]  mc        pointer to memory card instance object
  \return execution status
            - 0: command sent
            - 1: command send failed
*/
static uint32_t mc_send_command (uint8_t cmd, uint32_t arg, MC_SPI *mc) {
  uint8_t cb[7];

  /* Construct command (dummy + command block) */
  cb[0] = 0xFFU;
  cb[1] = 0x40U | cmd;
  cb[2] = (uint8_t)(arg >> 24);
  cb[3] = (uint8_t)(arg >> 16);
  cb[4] = (uint8_t)(arg >>  8);
  cb[5] = (uint8_t)(arg >>  0);

  /* Calculate CRC7 for command. */
  cb[6] = mc_CRC7(&cb[1]) | 0x01;

  /* Send command with CRC. */
  if (mc_send (cb, 7, mc) != fsOK) {
    /* Command send failed */
    EvrFsMcSPI_CmdSendError (mc->Instance, cmd, arg);
    return (1U);
  }
  return (0U);
}


/**
  Read memory card response

  \param[out] buf       response data buffer
  \param[in]  cnt       number of response bytes
  \param[in]  mc        memory card instance object
  \return execution status
            - 0: response received
            - 1: no response or receive error
*/
static uint32_t mc_read_response (uint8_t *buf, uint32_t cnt, MC_SPI *mc) {
  uint32_t i, err;

  buf[0] = 0xFF;
  err    = 1;

  /* Response will come after 1 - 8 retries. */
  for (i = 0; i < 8; i++) {
    if (mc_receive (&buf[0], 1, mc) == fsOK) {

      if ((buf[0] & 0x80) == 0) {
        /* R1 received */
        err = 0;
        cnt--;

        if (cnt != 0) {
          if (mc_receive (&buf[1], cnt, mc) != fsOK) {
            err = 1;
          }
        }
        break;
      }
    }
  }
  return (err);
}

/**
  Wait while memory card signals busy

  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_wait_busy (MC_SPI *mc) {
  fsStatus status;
  uint32_t i;
  uint8_t r1b[2];

  status = fsError;

  /* Wait for non busy R1b response */
  for (i = STOP_TOUT; i; i--) {
    /* Two bytes read here is a workaround for certain SD cards, which */
    /* don't signal busy correctly immediately after command is sent   */
    if (mc_receive (&r1b[0], 2, mc) != fsOK) {
      status = fsError;
    }
    else {
      if (r1b[1] != 0x00) {
        status = fsOK;
        break;
      } else {
        status = fsError;
      }
    }
  }
  return (status);
}


/**
  Read cnt number of data blocks with size sz

  \param[out] buf       data buffer
  \param[in]  sz        block size
  \param[in]  cnt       number of blocks
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_read_data (uint8_t *buf, uint32_t sz, uint32_t cnt, MC_SPI *mc) {
  fsStatus status;
  uint32_t i;
  uint8_t  resp;
  uint8_t  crc[2];

  do {
    status = fsError;

    /* Wait for Data Start token */
    for (i = CMD_TOUT; i; i--) {
      if (mc_receive (&resp, 1, mc) != fsOK) {
        status = fsError;
      }
      else {
        if (resp == 0xFE) {
          /* Data Start token received */
          status = fsOK;
          break;
        } else {
          status = fsError;
        }
      }
    }

    if (status == fsOK) {
      /* Read data block */
      if (mc_receive (buf, sz, mc) != fsOK) {
        status = fsError;
      }
    }

    if (status == fsOK) {
      /* Read 16-bit CRC */
      if (mc_receive (crc, 2, mc) != fsOK) {
        status = fsError;
      }
    }

    buf += sz;
    cnt--;
  }
  while ((cnt > 0) && (status == fsOK));

  return (status);
}


/**
  Write cnt number of data blocks with size sz

  \param[out] buf       data buffer
  \param[in]  sz        block size
  \param[in]  cnt       number of blocks
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_write_data (const uint8_t *buf, uint32_t sz, uint32_t cnt, MC_SPI *mc) {
  fsStatus status;
  uint32_t i;
  uint8_t  token, resp;
  uint8_t  crc[2];

  /* Select multiple or single write token */
  token = (cnt > 1) ? (0xFC) : (0xFE);

  do {
    /* Send Data Start token */
    if (mc_send (&token, 1, mc) != fsOK) {
      status = fsError;
    } else {
      status = fsOK;
    }

    if (status == fsOK) {
      /* Send data block */
      if (mc_send (buf, sz, mc) != fsOK) {
        status = fsError;
      }
    }

    if (status == fsOK) {
      /* Send dummy CRC */
      crc[0] = 0xFF;
      crc[1] = 0xFF;
      if (mc_send (crc, 2, mc) != fsOK) {
        status = fsError;
      }
    }

    if (status == fsOK) {
      /* Read response */
      if (mc_receive (&resp, 1, mc) != fsOK) {
        status = fsError;
      } else {
        if ((resp & 0x0F) != 0x05) {
          status = fsError;
        }
      }
    }

    if (status == fsOK) {
      /* Wait until memory device busy */
      for (i = WR_TOUT; i; i++) {
        if (mc_receive (&resp, 1, mc) != fsOK) {
          status = fsError;
        } else {
          if (resp == 0xFF) {
            /* Write finished */
            break;
          }
        }
      }
    }

    buf += sz;
    cnt--;
  }
  while ((cnt > 0) && (status == fsOK));

  return (status);
}


/**
  (CMD0, -): Send CMD0 with CS asserted to switch card in IDLE state

  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_reset (MC_SPI *mc) {
  uint8_t  r1;
  uint32_t tout;
  fsStatus status;

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (MC_CMD_GO_IDLE_STATE, 0, mc) != 0U) {
    status = fsError;
  }
  else {
    r1 = 0x00;

    for (tout = 0; tout < 2000; tout++) {
      if (mc_read_response (&r1, 1, mc) != 0U) {
        /* No response to command */
        EvrFsMcSPI_CmdResponseError (mc->Instance, MC_CMD_GO_IDLE_STATE);
        status = fsError;
//        break;
      }
      else {
        if (r1 == 0x01) {
          break;
        }
      }
    }
    if (r1 == 0x01) {
      status = fsOK;
    } else {
      status = fsError;
    }
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  return (status);
}


/**
  (CMD1, R1): Activates the cards initialization process (MMC specific)

  \param[out] r1        R1 response buffer (1 byte)
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_send_op_cond (uint8_t *r1, MC_SPI *mc) {
  fsStatus status;

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (MC_CMD_SEND_OP_COND, 0, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (r1, 1, mc) != 0U) {
      /* No response to command */
      EvrFsMcSPI_CmdResponseError (mc->Instance, MC_CMD_SEND_OP_COND);
      status = fsError;
    }
    else {
      if (r1[0] == 0x00) {
        status = fsOK;
      } else {
        status = fsError;
      }
    }
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  return (status);
}


/**
  (CMD8, R7): Read OCR register of the addressed card

  \param[in]  arg       CMD8 argument (VHS + check pattern)
  \param[out] r7        R7 response buffer (5-byte)
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_send_if_cond (uint32_t arg, uint8_t *r7, MC_SPI *mc) {
  fsStatus status;

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (MC_CMD_SEND_IF_COND, arg, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (r7, 5, mc) != 0U) {
      /* No response to command */
      EvrFsMcSPI_CmdResponseError (mc->Instance, MC_CMD_SEND_IF_COND);
      status = fsError;
    }
    else {
      if (r7[0] == 0x01) {
        status = fsOK;
      } else {
        status = fsError;
      }
    }
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  return (status);
}


/**
  (CMD9, R1): Ask the selected card to send its card specific data (CSD)

  \param[out] buf       data buffer (16 bytes) for storing register content
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_send_csd (uint8_t *buf, MC_SPI *mc) {
  uint8_t  r1;
  fsStatus status;

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (MC_CMD_SEND_CSD, 0, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (&r1, 1, mc) != 0U) {
      /* No response to command */
      EvrFsMcSPI_CmdResponseError (mc->Instance, MC_CMD_SEND_CSD);
      status = fsError;
    }
    else {
      if (r1 == 0x00) {
        status = fsOK;
      } else {
        status = fsError;
      }
    }
  }

  if (status == fsOK) {
    /* Read CID */
    status = mc_read_data (buf, 16, 1, mc);
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  return (status);
}


/**
  (CMD10, R1): Ask the selected card to send its card identification (CID)

  \param[out] buf       data buffer (16 bytes) for storing register content
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_send_cid (uint8_t *buf, MC_SPI *mc) {
  uint8_t  r1;
  fsStatus status;

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (MC_CMD_SEND_CID, 0, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (&r1, 1, mc) != 0U) {
      /* No response to command */
      EvrFsMcSPI_CmdResponseError (mc->Instance, MC_CMD_SEND_CID);
      status = fsError;
    }
    else {
      if (r1 == 0x00) {
        status = fsOK;
      } else {
        status = fsError;
      }
    }
  }

  if (status == fsOK) {
    /* Read CID */
    status = mc_read_data (buf, 16, 1, mc);
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  return (status);
}


/**
  (CMD13, R2): Read memory card status register

  \param[out] r2        R2 response (2-byte)
  \param[in]  mc        memory card instance object
  \return
*/
static fsStatus mc_read_status (uint8_t *r2, MC_SPI *mc) {
  fsStatus status;

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);
  
  if (mc_send_command (MC_CMD_SEND_STATUS, 0, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (r2, 2, mc) != 0U) {
      /* No response to command */
      EvrFsMcSPI_CmdResponseError (mc->Instance, MC_CMD_SEND_STATUS);
      status = fsError;
    } else {
      status = fsOK;
    }
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  return (status);
}


/**
  (CMD16, R1): Set block length

  \param[in]  len       block length in bytes
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_set_block_len (uint32_t len, MC_SPI *mc) {
  uint8_t r1;
  fsStatus status;

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (MC_CMD_SET_BLOCKLEN, len, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (&r1, 1, mc) != 0U) {
      /* No response to command */
      EvrFsMcSPI_CmdResponseError (mc->Instance, MC_CMD_SET_BLOCKLEN);
      status = fsError;
    }
    else {
      if (r1 == 0x00) {
        status = fsOK;
      } else {
        status = fsError;
      }
    }
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  return (status);
}


/**
  (CMD17 | CMD18, R1): Read single or multiple blocks of data

  \param[in]  block     block number
  \param[out] buf       data buffer
  \param[in]  cnt       number of blocks to read
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_read_block (uint32_t block, uint8_t *buf, uint32_t cnt, MC_SPI *mc) {
  uint8_t r1, cmd;
  fsStatus status;

  /* CMD18 or CMD17, R1 */
  cmd = (cnt > 1) ? (MC_CMD_READ_MULTIPLE_BLOCK) : (MC_CMD_READ_SINGLE_BLOCK);

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (cmd, block, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (&r1, 1, mc) != 0U) {
      /* No response to command */
      EvrFsMcSPI_CmdResponseError (mc->Instance, cmd);
      status = fsError;
    }
    else {
      if (r1 != 0x00) {
        status = fsError;
      } else {
        status = fsOK;
      }
    }
  }

  if (status == fsOK) {
    status = mc_read_data (buf, 512, cnt, mc);
  }

  if (status == fsOK) {
    if (cnt > 1) {
      if (mc_send_command (MC_CMD_STOP_TRANSMISSION, 0, mc) != 0U) {
        status = fsError;
      }
      else {
        /* Wait while busy */
        if (mc_wait_busy (mc) != fsOK) {
          status = fsError;
        }
      }
    }
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  return (status);
}


/**
  (CMD24 | CMD25, R1): Write single or multiple blocks of data

  \param[in]  block     block number
  \param[in]  buf       data buffer
  \param[in]  cnt       number of blocks to write
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_write_block (uint32_t block, const uint8_t *buf, uint32_t cnt, MC_SPI *mc) {
  uint8_t r1, token, cmd;
  fsStatus status;

  /* CMD25 or CMD24, R1 */
  cmd = (cnt > 1) ? (MC_CMD_WRITE_MULTIPLE_BLOCK) : (MC_CMD_WRITE_SINGLE_BLOCK);

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (cmd, block, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (&r1, 1, mc) != 0U) {
      /* No response to command */
      EvrFsMcSPI_CmdResponseError (mc->Instance, cmd);
      status = fsError;
    }
    else {
      if (r1 != 0x00) {
        status = fsError;
      } else {
        status = fsOK;
      }
    }
  }

  if (status == fsOK) {
    status = mc_write_data (buf, 512, cnt, mc);
  }

  if (status == fsOK) {
    if (cnt > 1) {
      /* Send Stop Transmission token */
      token = 0xFD;
      if (mc_send (&token, 1, mc) != fsOK) {
        status = fsError;
      }
      else {
        /* Wait while busy */
        if (mc_wait_busy (mc) != fsOK) {
          status = fsError;
        }
      }
    }
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  return (status);
}


/**
  (CMD42, R1): Set/Reset the password or lock/unlock the card.

  \param[in]   buf  data buffer
  \param[in]   sz   block size
  \param[in]   mc   Pointer to memory card instance object
  \return  \ref fsStatus
*/
static fsStatus mc_lock_unlock (uint8_t *buf, uint32_t sz, MC_SPI *mc) {
  uint8_t r1;
  fsStatus status;

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (MC_CMD_LOCK_UNLOCK, 0, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (&r1, 1, mc) != 0U) {
      /* No response to command */
      EvrFsMcSPI_CmdResponseError (mc->Instance, MC_CMD_LOCK_UNLOCK);
      status = fsError;
    }
    else {
      if (r1 != 0x00) {
        status = fsError;
      } else {
        status = fsOK;
      }
    }

    if (status == fsOK) {
      status = mc_write_data (buf, sz, 1, mc);
    }
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);
  
  return (status);
}


/**
  (CMD55, R1): Send CMD55 to enable application specific commands

  \param[out] r1        R1 response buffer
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_enable_acmd (uint8_t *r1, MC_SPI *mc) {
  fsStatus status;

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (MC_CMD_APP_CMD, 0, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (r1, 1, mc) != 0U) {
      /* No response to command */
      EvrFsMcSPI_CmdResponseError (mc->Instance, MC_CMD_APP_CMD);
      status = fsError;
    }
    else {
      status = fsOK;
    }
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  return (status);
}


/**
  (CMD56, R1): General Command

  \param[in]  arg       command argument (bit 0 is direction: 1 == read, 0 == write)
  \param[out] buf       data buffer
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_gen_cmd (uint32_t arg, uint8_t *buf, MC_SPI *mc) {
  uint8_t r1;
  fsStatus status;

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (MC_CMD_GEN_CMD, arg, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (&r1, 1U, mc) != 0U) {
      /* No response to command */
      EvrFsMcSPI_CmdResponseError (mc->Instance, MC_CMD_GEN_CMD);
      status = fsError;
    }
    else {
      if (r1 != 0x00U) {
        status = fsError;
      } else {
        status = fsOK;
      }
    }
  }

  if (status == fsOK) {
    if ((arg & 1U) == 1U) {
      status = mc_read_data (buf, 512U, 1U, mc);
    } else {
      status = mc_write_data (buf, 512U, 1U, mc);
    }
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  return (status);
}


/**
  (CMD58, R3): Read the OCR register of a card

  SD argument: [31:0] stuff bits
  MMC argument: <= 2GB: None
                 > 2GB: [31],[28:0] stuff bits
                        [30:29] access mode
  For MMC, access mode 10b indicates sector access mode supported by host

  \param[out] r3        R3 response buffer (5 bytes)
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_read_ocr (uint8_t *r3, MC_SPI *mc) {
  fsStatus status;

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (MC_CMD_READ_OCR, 1 << 30, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (r3, 5, mc) != 0U) {
      /* No response to command */
      EvrFsMcSPI_CmdResponseError (mc->Instance, MC_CMD_READ_OCR);
      status = fsError;
    }
    else {
      if (r3[0] == 0x00) {
        status = fsOK;
      } else {
        status = fsError;
      }
    }
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  return (status);
}


/**
  (CMD59, R1): Turn the CRC option on or off.

  Command 59 argument bit assignment:
  [31:1]  Reserved
  [0:0]   CRC option (0 = off, 1 = on)

  \param[in]  arg       command argument
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_crc_on_off (uint32_t arg, MC_SPI *mc) {
  uint8_t r1;
  fsStatus status;

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (MC_CMD_CRC_ON_OFF, arg, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (&r1, 1, mc) != 0U) {
      /* No response to command */
      EvrFsMcSPI_CmdResponseError (mc->Instance, MC_CMD_CRC_ON_OFF);
      status = fsError;
    }
    else {
      if (r1 == 0x00) {
        status = fsOK;
      } else {
        status = fsError;
      }
    }
  }

  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  return (status);
}


/**
  (ACMD41, R1): Send operating condition register

  \param[in]  arg       command argument (HCS, XPC, S18R, Vdd Voltage Window)
  \param[in]  mc        memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_sd_send_op_cond (uint32_t arg, MC_SPI *mc) {
  uint8_t r1;
  fsStatus status;

  mc_control_ss (ARM_SPI_SS_ACTIVE, mc);

  if (mc_send_command (MC_ACMD_SD_SEND_OP_COND, arg, mc) != 0U) {
    status = fsError;
  }
  else {
    if (mc_read_response (&r1, 1, mc) != 0U) {
      /* No response to application command */
      EvrFsMcSPI_ACmdResponseError (mc->Instance, MC_ACMD_SD_SEND_OP_COND);
      status = fsError;
    }
    else {
      if (r1 == 0x00) {
        status = fsOK;
      } else {
        status = fsError;
      }
    }
  }
  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);
  
  return (status);
}


/**
  Fill extern CSD structure with content read from CSD register

  \param[out] csd       CSD structure object
  \param[in]  b         CSD register stored in uint8_t array
  \return \ref fsStatus
*/
static fsStatus mc_parse_csd (fsCSD_Register *csd, const uint8_t *b) {
  static const uint8_t tim_val[] = {
    0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80
  };
  uint32_t i, v, m;

  /* Read Block length */
  v = b[5] & 0x0F;
  csd->BlReadLen = (uint16_t)(1 << v);

  /* Write Block length */
  v = ((b[12] << 8 | b[13]) >> 6) & 0x0F;
  csd->BlWriteLen = (uint16_t)(1 << v);

  /* Max. data transfer rate */
  v = b[3];
  m = (v & 0x7) + 1;
  csd->TranSpeed = tim_val[(v >> 3) & 0x0F];
  for (i = 0; i < m; i++) {
    csd->TranSpeed *= 10;
  }

  /* Total Number of blocks */
  switch ((b[0] >> 6) & 3) {            /* Bits 126..127      */
    case 0:                             /* SD card,  CSD v1.0 */
    case 2:                             /* MMC card, CSD v1.2 */
      v = ((b[6] << 16 | b[7] << 8 | b[8]) >> 6) & 0x0FFF;
      m = ((b[9] << 8  | b[10]) >> 7) & 0x07;
      csd->BlCnt = (v + 1) << (m + 2);
      break;
    case 1:                             /* SD card,  CSD v2.0 */
      v =  ((b[7] << 16) | (b[8] << 8) | b[9]) & 0xFFFF;
      csd->BlCnt = (v + 1) << 10;
      break;
    default:
      return (fsError);
  }
  /* SD cards with tweaked CSD register are able to report its size above 1GB */
  /* Block size is still 512 (CSD V1.0), number of blocks must be corrected.  */
  switch (csd->BlReadLen) {
    case 512:                   break; /* Up to 1GB cards */
    case 1024: csd->BlCnt *= 2; break; /* 2 GB cards      */
    case 2048: csd->BlCnt *= 4; break; /* 4 GB cards      */
    
    default:
      return (fsError);                /* Block size not supported */
  }
  if (csd->BlReadLen > 512) {
    csd->BlReadLen  = 512;
    csd->BlWriteLen = 512;
  }

  return (fsOK);
}


/**
  Fill extern CID structure with content read from CID register

  \param[out] cid       CID structure object
  \param[in]  b         CID register stored in uint8_t array
  \param[in]  mmc       is MMC device? true/false
*/
static void mc_parse_cid (fsCID_Register *cid, const uint8_t *b, bool mmc) {
  memset (cid, 0, sizeof(fsCID_Register));

  if (mmc) {
    cid->MID    =  b[0];
/*  cid->CBX    =  b[1] & 0x3;  */
    cid->OID    =  b[2];
    cid->PNM[5] =  b[3];
    cid->PNM[4] =  b[4];
    cid->PNM[3] =  b[5];
    cid->PNM[2] =  b[6];
    cid->PNM[1] =  b[7];
    cid->PNM[0] =  b[8];
    cid->PRV    =  b[9];
    cid->PSN    = (uint32_t)((b[10] << 24) | (b[11] << 16) | (b[12] << 8) | b[13]);
    cid->MDT    =  b[14];
  }
  else { /* SD Card */
    cid->MID    =  b[0];
    cid->OID    = (uint16_t)((b[1] << 8) | b[2]);
    cid->PNM[4] =  b[3];
    cid->PNM[3] =  b[4];
    cid->PNM[2] =  b[5];
    cid->PNM[1] =  b[6];
    cid->PNM[0] =  b[7];
    cid->PRV    =  b[8];
    cid->PSN    = (uint32_t)((b[9] << 24) | (b[10] << 16) | (b[11] << 8) | b[12]);
    cid->MDT    =((b[13] << 8) | b[14]) & 0x0FFF;
  }
}


/**
  Perform card Lock/Unlock operation

  This function must be called when device is in TRAN state.

  \param[in]   lu     Lock/Unlock structure
  \param[in]   mc     Memory card instance object
*/
static fsStatus mc_control_lock (fsLockUnlock *lu, MC_SPI *mc) {
  __ALIGNED(4) uint8_t buf[18];
  uint32_t cnt;
  fsStatus status;

  /* Executing lock/unlock */
  EvrFsMcSPI_LockUnlockExec (mc->Instance);

  if ((lu == NULL)       ||
      (lu->length > 16U) ||
      (((lu->password == NULL) && (lu->flags & FS_DEVICE_ERASE) == 0))) {
    /* Invalid function parameters */
    EvrFsMcSPI_ParameterInvalid (mc->Instance);
    status = fsInvalidParameter;
  }
  else {
    if (lu->flags & FS_DEVICE_ERASE) {
      cnt = 1U;
    } else {
      cnt = lu->length + 2U;
    }

    /* Set block length in bytes (pass + 8-bit pass length + 8-bit mode */
    status = mc_set_block_len (cnt, mc);

    if (status == fsOK) {
      /* Prepare and send lock/unlock data structure */
      buf[0] = lu->flags & 0x0F;
      buf[1] = lu->length;
      memcpy (&buf[2], lu->password, lu->length);

      status = mc_lock_unlock (buf, cnt, mc);

      /* Clear buffer (don't leave password on stack) */
      memset (buf, 0, sizeof (buf));

      if (status == fsOK) {
        /* Check lock status */
        status = mc_read_status (buf, mc);

        if (status == fsOK) {
          if (buf[1] & R2_B2_LOCK_UNLOCK_FAILED) {
            /* Lock/Unlock cmd failed */
            EvrFsMcSPI_LockUnlockExecError (mc->Instance);
            status = fsAccessDenied;
          }
          if (buf[1] & R2_B2_CARD_IS_LOCKED) {
            /* Card is locked */
            EvrFsMcSPI_LockActive (mc->Instance);
            mc->Status |=  MC_STATUS_LOCKED;
          } else {
            /* Device is unlocked */
            EvrFsMcSPI_LockDeactivated (mc->Instance);
            mc->Status &= ~MC_STATUS_LOCKED;
          }
        }
      }
    }
  }

  return (status);
}


/**
  Set maximum SPI bus frequency.

  This function will attempt to set the SPI bus clock frequency to max transfer
  clock value retrieved from the CSD register. In case if device can no longer
  be accessed after setting the max transfer speed it will perform frequency
  adaptation.

  The return value from the CMSIS SPI driver is ignored in order to make this
  routine more persistent.

  \param[in]   f      Desired SPI bus clock frequency
  \param[in]   mc     Memory card instance object
  \return \ref fsStatus
*/
static fsStatus mc_spi_set_speed (uint32_t f, MC_SPI *mc) {
  fsStatus status;
  uint32_t fe, fn;
  uint8_t r2[2];

  /* Set SPI bus speed */
  (void)mc_control_bus_speed (f, mc);

  r2[0] = 0U;
  r2[1] = 0U;

  status = mc_read_status (r2, mc);

  if ((status != fsOK) || (r2[0] != 0U) || (r2[1] != 0U)) {
    /* Start frequency adaptation procedure */
    fe = f;
    f  = MC_INIT_BUS_SPEED; /* 400kHz is the low limit */

    do {
      fn = f + ((fe - f) >> 1);

      mc_control_bus_speed (fn, mc);

      status = mc_read_status (r2, mc);

      if ((status == fsOK) && (r2[0] == 0U) && (r2[1] == 0U)) {
        f  = fn;  /* Latest working frequency */
      } else {
        fe = fn;  /* Latest error frequency */
      }
    }
    while ((fe - f) > SPI_BUS_FREQ_ERROR_DELTA);

    /* Final clock setting */
    (void)mc_control_bus_speed (f, mc);
    status = mc_read_status (r2, mc);

    if (status == fsOK) {
      if ((r2[0] != 0U) || (r2[1] != 0U)) {
        status = fsError;
      }
    }
  }

  return (status);
}

/**
  fsDevCtrlCodeHealthStatus device control handler

  \param[in,out] p    Pointer to structure specifying health status argument and buffer
  \param[in]     mc   Pointer to memory card instance object
*/
static fsStatus mc_fsDevCtrlCodeHealthStatus (fsHealthStatus *p, MC_SPI *mc) {
  fsStatus stat;

  /* Check buffer alignment and size */
  if ((((uint32_t)p->buf & 3U) != 0U) || (p->buf_sz < 512U)) {
    /* Buffer is not 4-byte aligned or is too small */
    stat = fsError;
  }
  else {
    /* General command */
    stat = mc_gen_cmd (p->arg, p->buf, mc);
  }

  return (stat);
}

/* ------------------------- Exported functions ----------------------------- */


/**
  Initialize SPI interface and memory card

  \param[in]      mode      initialization mode
  \param[in,out]  mc        memory card instance object
  \return true if initialization passed, false otherwise
*/
uint32_t mc_spi_Init (uint32_t mode, MC_SPI *mc) {
  fsCSD_Register csd;
  fsCID_Register cid;
  uint32_t i, tout, arg, spi_ss;
  uint8_t  r1, buf[16];
  fsStatus status;

  if (mode == DM_IO) {
    mc->MediaStatus = 0;

    if ((mc->Status & MC_STATUS_DRIVER_INIT) == 0) {
      /* Initialize SPI driver */
      EvrFsMcSPI_InitDriver (mc->Instance, (uint32_t)mc->Driver);

      if (mc->Driver->Initialize (mc->Callback) != ARM_DRIVER_OK) {
        /* Driver initialization failed */
        EvrFsMcSPI_InitDriverError (mc->Instance, (uint32_t)mc->Driver);
        return (false);
      }
      mc->Status |= MC_STATUS_DRIVER_INIT;
    }
    EvrFsMcSPI_InitDriverSuccess (mc->Instance, (uint32_t)mc->Driver);
    return (true);
  }

  if (mode != DM_MEDIA) {
    return (false);
  }
  /* Initializing memory media */
  EvrFsMcSPI_InitMedia (mc->Instance);

  /* Reset media status */
  mc->MediaStatus = 0;
  mc->Property    = 0;

  /* Power-on SPI peripheral */
  if (mc->Driver->PowerControl (ARM_POWER_FULL) != ARM_DRIVER_OK) {
    /* Driver power control failed */
    EvrFsMcSPI_InitDriverPowerError (mc->Instance, (uint32_t)mc->Driver, ARM_POWER_FULL);
    return (false);
  }
  /* Set default transfer value */
  mc->Driver->Control (ARM_SPI_SET_DEFAULT_TX_VALUE, 0xFF);

  /* Configure SPI bus, set bus speed to 400kHz */
  if (fs_mc_spi_control_ss (mc->Instance, ARM_SPI_SS_INACTIVE) != -1) {
    /* User controlled slave select */
    spi_ss = ARM_SPI_SS_MASTER_UNUSED;
  } else {
    /* Function fs_mc_spi_control_ss is not implemented */
    EvrFsMcSPI_SlaveSelCtrlMissing (mc->Instance);

    /* Software controlled slave select */
    spi_ss = ARM_SPI_SS_MASTER_SW;
  }
  mc->Driver->Control (ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0  \
                                           | ARM_SPI_DATA_BITS(8) \
                                           | ARM_SPI_MSB_LSB      \
                                           | spi_ss,
                                           MC_INIT_BUS_SPEED);

#ifdef FS_DEBUG
  if (fs_mc_read_cd(mc->Instance) == -1) {
    /* Function fs_mc_read_cd is not implemented */
    EvrFsMcSPI_ReadCdMissing (mc->Instance);
  }
  if (fs_mc_read_wp(mc->Instance) == -1) {
    /* Function fs_mc_read_wp is not implemented */
    EvrFsMcSPI_ReadWpMissing (mc->Instance);
  }
#endif

  /* Initialize Memory Card in SPI mode. */
  mc_control_ss (ARM_SPI_SS_INACTIVE, mc);

  /* Send 16 dummy bytes (card not selected) */
  buf[0] = 0xFF;
  for (i = 0; i < 16; i++) {
    if (mc_send (&buf[0], 1, mc) != fsOK) {
      return (false);
    }
  }

  /* Resetting media... */
  EvrFsMcSPI_MediaReset (mc->Instance);

  /* Reset the card, send CMD0+ (CMD0 with CS asserted ("0")) */
  if (mc_reset(mc) != fsOK) {
    /* Failed to reset the card */
    EvrFsMcSPI_MediaResetError (mc->Instance);
    return (false);
  }

  /* Detecting SD device */
  EvrFsMcSPI_MediaDetectionSD (mc->Instance);

  /* Check if card is V2.00 or later SD memory card (CMD8). */
  if (mc_send_if_cond (0x1AA, buf, mc) == fsOK) {
    if ((buf[1] == 0x00) && (buf[2] == 0x00) && (buf[3] == 0x01) && (buf[4] == 0xAA)) {
      /* Response OK, this is Ver2.00 SD memory card */
      mc->Property |= MC_PROP_SD_V2;

      EvrFsMcSPI_MediaSD_V2 (mc->Instance);
    }
  }

  /* Check if SD card, send ACMD41. */
  for (tout = 1000; tout; tout -= 10) {
    if (mc_enable_acmd (&r1, mc) != fsOK) {
      return (false);
    }
    if (r1 & 0x04) {
      /* Illegal command */
      break;
    }
    if ((r1 == 0x01) || (r1 == 0x00)) {
      arg = (uint32_t)((mc->Property & MC_PROP_SD_V2) != 0) << 30;

      if (mc_sd_send_op_cond (arg, mc) == fsOK) {
        /* SD card initialized */
        mc->Property |= MC_PROP_TYPE_SD;
        
        if (mc_read_ocr (buf, mc) != fsOK) {
          return (false);
        }

        EvrFsMcSPI_MediaReady (mc->Instance, buf[1]);

        if (!(buf[1] & (1 << (30-24)))) {
          /* Card Capacity Status (CCS) bit is not set */
          mc->Property |= MC_PROP_ACCESS_BYTE;
        }

        break;
      }
    }
    /* Wait 10ms */
    fs_set_rtos_delay (10);
  }

  /* If card is not SD check if it is MMC card. */
  if ((mc->Property & MC_PROP_TYPE_SD) == 0) {
    /* Detecting MMC/eMMC device */
    EvrFsMcSPI_MediaDetectionMMC (mc->Instance);

    /* For MMC > 2GB CMD58 must be sent first */
    mc_read_ocr (buf, mc);

    /* Send CMD1 until in-idle-state bit is cleared */
    for (tout = 1000; tout; tout -= 10) {
      if (mc_send_op_cond (buf, mc) == fsOK) {
        mc->Property |= MC_PROP_ACCESS_BYTE | MC_PROP_TYPE_MMC;

        if (mc_read_ocr (buf, mc) != fsOK) {
          return (false);
        }

        EvrFsMcSPI_MediaReady (mc->Instance, buf[1]);

        if (buf[1] & (1 << (30-24))) {
          /* Sector access mode */
          mc->Property &= ~MC_PROP_ACCESS_BYTE;
        }
        break;
      }
      /* Wait 10ms */
      fs_set_rtos_delay (10);
    }
  }

  if ((mc->Property & (MC_PROP_TYPE_SD | MC_PROP_TYPE_MMC)) == 0) {
    /* Unknown device, device detection failed */
    EvrFsMcSPI_MediaDetectionError (mc->Instance);
    return (false);
  }

  mc->MediaStatus |= FS_MEDIA_INSERTED;

  /* Read CSD register */
  if (mc_send_csd (buf, mc) != fsOK) {
    /* Failed to retrieve CSD register */
    EvrFsMcSPI_CsdReadError (mc->Instance);
    return (false);
  }
  else {
    if (mc_parse_csd (&csd, buf) != fsOK) {
      return (false);
    }
    /* Remember number of sectors */
    mc->SectorCount = csd.BlCnt;
  }

  /* Read CID register */
  if (mc_send_cid(buf, mc) != fsOK) {
    /* Failed to retrieve CID register */
    EvrFsMcSPI_CidReadError (mc->Instance);
    return (false);
  }
  else {
    mc_parse_cid (&cid, buf, (mc->Property & MC_PROP_TYPE_MMC) == MC_PROP_TYPE_MMC);
    /* Remember device serial number */
    mc->SerialNumber = cid.PSN;
  }

  if (mc_read_status (buf, mc) != fsOK) {
    return (false);
  }
  else {
    if (buf[1] & R2_B2_CARD_IS_LOCKED) {
      /* Password protection is active */
      EvrFsMcSPI_MediaPasswordEnabled (mc->Instance);

      status = mc_control_lock (fs_mc_lock_unlock (mc->Instance), mc);

      if ((status == fsOK) || (status == fsAccessDenied)) {
        if (mc->Status & MC_STATUS_LOCKED) {
          return (false);
        }
      }
      else {
        return (false);
      }
    }
  }

  /* Set bus speed */
  if (mc_spi_set_speed (csd.TranSpeed * 1000, mc) != fsOK) {
    /* Failed to set the appropriate SPI bus frequency */
    return (false);
  }

  /* Set block length to 512 bytes. */
  if (mc_set_block_len (512, mc) != fsOK) {
    /* Failed to set block length */
    EvrFsMcSPI_BlenWriteError (mc->Instance, 512);
    return (false);
  }

  /* Turn Off CRC option. */
  if (mc_crc_on_off (0, mc) != fsOK) {
    /* Failed to turn off CRC check */
    EvrFsMcSPI_CrcDisableError (mc->Instance);
    return (false);
  }

  /* Success, card initialized. */
  mc->MediaStatus |= FS_MEDIA_INITIALIZED;

  EvrFsMcSPI_InitMediaSuccess (mc->Instance);

  return (true);
}


/**
  Uninitialize memory card interface

  \param[in]      mode      uninitialization mode
  \param[in,out]  mc        memory card instance object
  \return true on success, false otherwise
*/
uint32_t mc_spi_UnInit (uint32_t mode, MC_SPI *mc) {

  if (mode == DM_IO) {
    /* Uninitialize driver */
    EvrFsMcSPI_UninitDriver (mc->Instance, (uint32_t)mc->Driver);

    /* Clear driver initialization flags */
    mc->Status = 0;

    if (mc->Driver->PowerControl (ARM_POWER_OFF) == ARM_DRIVER_OK) {
      /* Uninit SPI peripheral and IO. */
      return (mc->Driver->Uninitialize () == ARM_DRIVER_OK);
    }
  }
  else {
    if (mode == DM_MEDIA) {
      /* Uninit memory device */
      EvrFsMcSPI_UninitMedia (mc->Instance);

      /* Uninit memory media */
      mc->MediaStatus &= ~FS_MEDIA_INITIALIZED;
      mc->Property     = 0U;
      mc->SerialNumber = 0U;
      return (true);
    }
  }

  return (false);
}


/**
  Read a cnt of 512 byte sectors from memory card

  \param[in]  sect      sector number
  \param[out] buf       sector data buffer
  \param[in]  cnt       number of sectors to write
  \param[in]  mc        memory card instance object
  \return true if read succeeded, false otherwise
*/
uint32_t mc_spi_ReadSector (uint32_t sect, uint8_t *buf, uint32_t cnt, MC_SPI *mc) {
  uint32_t err;

  EvrFsMcSPI_SectorRead (mc->Instance, sect, cnt);

  err = 0;

  if ((mc->MediaStatus & FS_MEDIA_INITIALIZED) == 0) {
    /* Media is not initialized */
    EvrFsMcSPI_MediaNotInitialized (mc->Instance);
    err = 1;
  }
  if (mc->Status & MC_STATUS_LOCKED) {
    /* Password protection is active */
    EvrFsMcSPI_MediaPasswordEnabled (mc->Instance);
    err = 1;
  }

  if (err == 0) {
    if (mc->Property & MC_PROP_ACCESS_BYTE) {
      sect <<= 9;
    }

    if (mc_read_block (sect, buf, cnt, mc) == fsOK) {
      return (true);
    }
  }
  /* Read failed: sector=%d, cnt=%d */
  EvrFsMcSPI_SectorReadError (mc->Instance, sect, cnt);
  return (false);
}


/**
  Write a cnt of 512 byte sectors to memory card

  \param[in]  sect      sector number
  \param[in]  buf       sector data buffer
  \param[in]  cnt       number of sectors to write
  \param[in]  mc        memory card instance object
  \return true if write succeeded, false otherwise
*/
uint32_t mc_spi_WriteSector (uint32_t sect, const uint8_t *buf, uint32_t cnt, MC_SPI *mc) {
  uint32_t err;

  EvrFsMcSPI_SectorWrite (mc->Instance, sect, cnt);

  err = 0;

  if ((mc->MediaStatus & FS_MEDIA_INITIALIZED) == 0) {
    /* Media is not initialized */
    EvrFsMcSPI_MediaNotInitialized (mc->Instance);
    err = 1;
  }
  if (mc->Status & MC_STATUS_LOCKED) {
    /* Password protection is active */
    EvrFsMcSPI_MediaPasswordEnabled (mc->Instance);
    err = 1;
  }

  if (err == 0) {
    if (mc->Property & MC_PROP_ACCESS_BYTE) {
      sect <<= 9;
    }

    if (mc_write_block (sect, buf, cnt, mc) == fsOK) {
      return (true);
    }
  }
  /* Write failed: sector=%d, cnt=%d */
  EvrFsMcSPI_SectorWriteError (mc->Instance, sect, cnt);
  return (false);
}


/**
  Read memory cards configuration

  \param[out] info      pointer to \ref FS_MEDIA_INFO object
  \param[in]  mc        memory card instance object
  \return true on success, false otherwise
*/
uint32_t mc_spi_ReadInfo (fsMediaInfo *info, MC_SPI *mc) {
  uint32_t rval;

  EvrFsMcSPI_ReadInfo (mc->Instance);

  if ((mc->MediaStatus & FS_MEDIA_INITIALIZED) == 0) {
    /* Media is not initialized */
    rval = false;
    EvrFsMcSPI_MediaNotInitialized (mc->Instance);
  }
  else {
    /* READ_BL_LEN/WRITE_BL_LEN of 512 bytes is mandatory support by all     */
    /* devices. To make things simple, total number of sectors is determined */
    /* at device initialization.                                             */

    info->read_blen  = 512U;
    info->write_blen = 512U;
    info->block_cnt  = mc->SectorCount;

    rval = true;

    EvrFsMcSPI_ReadInfoSuccess (mc->Instance, mc->SectorCount);
  }

  return (rval);
}


/**
  Process given device control command

  \param[in]      code      device control command code
  \param[in,out]  p         command argument
  \param[in]      mc        memory card instance object
  \return fsStatus
*/
fsStatus mc_spi_DevCtrl (fsDevCtrlCode code, void *p, MC_SPI *mc) {
  fsStatus status;
  bool m_init;
  uint8_t  buf[16];

  EvrFsMcSPI_DevCtrl (mc->Instance, code, (uint32_t)p);

  status = fsError;
  m_init = (mc->MediaStatus & FS_MEDIA_INITIALIZED) != 0;

  if (code == fsDevCtrlCodeCheckMedia) {
    /* Check for media presence */
    mc_spi_update_sw_state (mc);

    if (p != NULL) {
      if (fs_mc_read_cd(mc->Instance) == -1) {
        if (m_init) {
          /* Media is initialized */
          mc->MediaStatus &= ~FS_MEDIA_NOCHKMEDIA;

          if (mc_read_status (&buf[0], mc) == fsOK) {
            /* We can read status, card is present */
            mc->MediaStatus |= FS_MEDIA_INSERTED;
          } else {
            /* Status cannot be read, card was removed */
            mc->MediaStatus &= ~(FS_MEDIA_INSERTED | FS_MEDIA_INITIALIZED);
          }
        }
      }

      *(uint32_t *)p = mc->MediaStatus;
      status = fsOK;
    }
  }
  else if (code == fsDevCtrlCodeControlMedia) {
    /* Control media */
    if (p != NULL) {
      switch (*(uint32_t *)p) {
        case FS_CONTROL_MEDIA_INIT:
          if (mc_spi_Init (DM_MEDIA, mc) == true) {
            status = fsOK;
          }
          break;

        default:
          break;
      }
    }
  }
  else if (code == fsDevCtrlCodeSerial) {
    /* Return media serial number */
    if (p != NULL) {
      if (m_init) {
        *(uint32_t *)p = mc->SerialNumber;
        status = fsOK;
      } else {
        status = fsNoMedia;
      }
    }
  }
  else if (code == fsDevCtrlCodeGetCID) {
    /* Read CID register */
    if (p != NULL) {
      if (m_init) {
        if (mc_send_cid(buf, mc) != fsOK) {
          status = fsError;
        }
        else {
          mc_parse_cid ((fsCID_Register *)p, buf, (mc->Property & MC_PROP_TYPE_MMC) == MC_PROP_TYPE_MMC);
          status = fsOK;
        }
      } else {
        status = fsNoMedia;
      }
    }
  }
  else if (code == fsDevCtrlCodeLockUnlock) {
    /* Lock/Unlock the device */
    status = mc_control_lock ((fsLockUnlock *)p, mc);

    /* Set block length to 512 bytes. */
    if (mc_set_block_len (512, mc) != fsOK) {
      status = fsError;
    }
  }
  else if (code == fsDevCtrlCodeHealthStatus) {
    /* Access health status data */
    if (p != NULL) {
      status = mc_fsDevCtrlCodeHealthStatus ((fsHealthStatus *)p, mc);
    }
  }
  else {
    /* Unsupported */
    EvrFsMcSPI_DevCtrlUnsupported (mc->Instance, code);
    status = fsUnsupported;
  }

  return (status);
}
