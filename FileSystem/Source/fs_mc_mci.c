/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_mc_mci.c
 * Purpose: MMC/SD Card Native-Mode Interface
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "cmsis_compiler.h"

#include "Driver_MCI.h"
#include "Driver_Common.h"

#include "fs_mc.h"
#include "fs_evr.h"


/**
  Callback function used to retrieve password management information
  in order to manage locked memory card device during initialization.

  \param[in]  drive_num                Memory Card Drive number
  \return     pointer to fsLockUnlock structure or NULL
*/
__WEAK fsLockUnlock *fs_mc_lock_unlock (uint32_t drive_num) {
  (void)drive_num;

  return (NULL);
}


/**
  Callback function to handle MCI events (IRQ context)

  \param[in]  event   Memory card interface event
  \param[in]  mc      Pointer to memory card instance object
*/
void mc_mci_DrvEvent (uint32_t event, MC_MCI *mc) {

  if ((event & ARM_MCI_EVENT_CARD_INSERTED) != 0) {
    /* Memory card was inserted */
    mc->MediaStatus |= FS_MEDIA_INSERTED;

    if (mc->Capabilities.wp_state) {
      /* Check write protect switch state */
      if (mc->Driver->ReadWP()) {
        mc->MediaStatus |= FS_MEDIA_PROTECTED;
      }
      else {
        mc->MediaStatus &= ~(FS_MEDIA_PROTECTED);
      }
    }
  }

  if (event & ARM_MCI_EVENT_CARD_REMOVED) {
    /* Memory card was removed */
    mc->MediaStatus &= ~(FS_MEDIA_INSERTED    |
                         FS_MEDIA_PROTECTED   |
                         FS_MEDIA_INITIALIZED);
  }
  mc->Event |= event;
}


/**
  Wait for event from MCI driver till 3 second timeout

  \param[in]  event   Event mask to wait
  \param[in]  mc      Pointer to memory card instance object
  \return events received or 0 on timeout
*/
static uint32_t mc_wfe (uint32_t event, MC_MCI *mc) {
  uint32_t tick, tout;

  tout = fs_get_sys_tick_us(3000000);
  tick = fs_get_sys_tick();
  do {
    if (mc->Event & event) {
      break;
    }
  } while ((fs_get_sys_tick() - tick) < tout);

  return (mc->Event);
}


/**
  Update state of MCI switches

  \param[in]   mc   Pointer to memory card instance object
  \return      void
*/
static void mci_update_sw_state (MC_MCI *mc) {

  if (!mc->Capabilities.cd_state && !mc->Capabilities.wp_state) {
    mc->MediaStatus |= FS_MEDIA_NOCHKMEDIA;
  }
  else {
    if (mc->Capabilities.cd_state) {
      /* Read Card Detect switch state */
      if (mc->Driver->ReadCD()) {
        /* Media is inserted */
        EvrFsMcMCI_MediaCdActive (mc->Instance);

        mc->MediaStatus |= FS_MEDIA_INSERTED;
      } else {
        /* Media is not inserted */
        EvrFsMcMCI_MediaCdInactive (mc->Instance);

        mc->MediaStatus &= ~(FS_MEDIA_INSERTED | FS_MEDIA_INITIALIZED);
      }
    }

    if (mc->Capabilities.wp_state) {
      /* Read Write Protect switch state */
      if (mc->Driver->ReadWP()) {
        /* Media write protection is active */
        EvrFsMcMCI_MediaWpActive (mc->Instance);
        
        mc->MediaStatus |= FS_MEDIA_PROTECTED;
      } else {
        /* Media write protection is not active */
        EvrFsMcMCI_MediaWpInactive (mc->Instance);
        
        mc->MediaStatus &= ~FS_MEDIA_PROTECTED;
      }
    }
  }
}


/**
  Perform device power up/down sequence
*/
static int32_t mci_card_power (bool enable, MC_MCI *mc) {
  uint32_t arg = 0;

  if (mc->Capabilities.vdd) {
    /* Power up/down the card (logic + memory field) */
    arg |= (enable) ? (ARM_MCI_POWER_VDD_3V3) : (ARM_MCI_POWER_VDD_OFF);
  }
  if (mc->Capabilities.vccq) {
    /* Power up/down eMMC logic */
    arg |= (enable) ? (ARM_MCI_POWER_VCCQ_3V3) : (ARM_MCI_POWER_VCCQ_OFF);
  }
  if (arg) {
    if (mc->Driver->CardPower (arg) != ARM_DRIVER_OK) {
      return (-1);
    }
    if (enable) {
      /* Wait for the longest of: VDD power-up time, VCCQ power-up time */
      fs_set_rtos_delay (2);
    }
  }
  return (0);
}


/**
  Set bus clock speed
*/
static int32_t mci_bus_speed (uint32_t bps, MC_MCI *mc) {
  return (mc->Driver->Control (ARM_MCI_BUS_SPEED, bps));
}


/**
  Set bus clock speed mode
*/
static int32_t mci_bus_speed_mode (uint32_t mode, MC_MCI *mc) {
  if (mc->Driver->Control (ARM_MCI_BUS_SPEED_MODE, mode) != ARM_DRIVER_OK) {
    return (-1);
  }
  return (0);
}


/**
  Set command bus mode
*/
static int32_t mci_cmd_bus_mode (uint32_t mode, MC_MCI *mc) {
  if (mc->Driver->Control (ARM_MCI_BUS_CMD_MODE, mode) != ARM_DRIVER_OK) {
    return (-1);
  }
  return (0);
}


/**
  Set data bus width
*/
static int32_t mci_bus_width (uint32_t width, MC_MCI *mc) {
  if (mc->Driver->Control (ARM_MCI_BUS_DATA_WIDTH, width) != ARM_DRIVER_OK) {
    return (-1);
  }
  return (0);
}


/**
  Enable or disable clock generation when bus is idle

  When enable parameter is set to true, SD clock generation is enabled
  even when there is no communication on the bus. When enable is set to
  false, SD clock is stopped if there is no communication on the bus.
  
  \param[in]  enable  Clock generation enable/disable switch
  \param[in]  mc      Pointer to memory card instance object
*/
static int32_t mci_idle_clock (bool enable, MC_MCI *mc) {
  if (enable) {
    /* Enable clock generation when idle */
    mc->Driver->Control (ARM_MCI_CONTROL_CLOCK_IDLE, 1);
  }
  else {
    /* Disable clock generation when idle */
    mc->Driver->Control (ARM_MCI_CONTROL_CLOCK_IDLE, 0);
  }
  return (0);
}


/**
  Set data timeout
  
  \param[in]  ms          Timeout in miliseconds
  \param[in]  bus_speed   SD bus speed in Hz
  \param[in]  mc          Pointer to memory card instance object
*/
static int32_t mci_data_timeout (uint32_t ms, uint32_t bus_speed, MC_MCI *mc) {
  uint32_t cycles;

  cycles = ms * (bus_speed / 1000);

  if (mc->Driver->Control (ARM_MCI_DATA_TIMEOUT, cycles) != ARM_DRIVER_OK) {
    return (-1);
  }
  return (0);
}


/**
  (CMD0, -): Send 74 clock cycles and issue CMD0 to switch card in IDLE state

  \param[in]   mc   Pointer to memory card instance object
  \return
*/
static uint32_t mc_go_idle_state (MC_MCI *mc) {
  int32_t stat;
  uint32_t events;

  mc->Event = 0;

  stat = mc->Driver->SendCommand (MC_CMD_GO_IDLE_STATE, 0, ARM_MCI_CARD_INITIALIZE | ARM_MCI_RESPONSE_NONE, NULL);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_GO_IDLE_STATE, 0);
  return (1); // Error
}


/**
  (CMD1, R3): Ask the card in IDLE state to send its OCR content (MMC specific)

  \param[in]   arg  Command argument (OCR without busy)
  \param[out]  r3   R3 response buffer
  \param[in]   mc   Pointer to memory card instance object
  \return
*/
static uint32_t mc_send_op_cond (uint32_t arg, uint32_t *r3, MC_MCI *mc) {
  int32_t stat;
  uint32_t flags, events;

  mc->Event = 0;

  flags = MC_RESPONSE_R3;
  stat  = mc->Driver->SendCommand (MC_CMD_SEND_OP_COND, arg, flags, r3);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_SEND_OP_COND, arg);
  return (1); // Error
}


/**
  (CMD2, R2): Ask any card to send the CID, while in IDENT state

  \param[in]   r2   R2 response buffer
  \param[in]   mc   Pointer to memory card instance object
  \return      status bits, or 0 if status read failed
*/
static uint32_t mc_read_any_cid (uint32_t *r2, MC_MCI *mc) {
  int32_t stat;
  uint32_t flags, events;

  mc->Event = 0;

  flags = MC_RESPONSE_R2 | ARM_MCI_RESPONSE_CRC;
  stat  = mc->Driver->SendCommand (MC_CMD_ALL_SEND_CID, 0, flags, r2);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_ALL_SEND_CID, 0);
  return (1); // Error
}


/**
  (CMD3, R1): Set cards relative address - MMC specific

  \param[in]   rca  Relative card address to assign
  \param[in]   mc   Pointer to memory card instance object
  \return      true if address set successfully, false otherwise
*/
static uint32_t mc_set_rca (uint32_t rca, MC_MCI *mc) {
  int32_t stat;
  uint32_t r1, flags, events;

  mc->Event = 0;

  r1    = 0;
  flags = MC_RESPONSE_R1 | ARM_MCI_RESPONSE_CRC;
  stat  = mc->Driver->SendCommand (MC_CMD_SET_RELATIVE_ADDR, rca << 16, flags, &r1);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      /* Check response */
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_SET_RELATIVE_ADDR, rca << 16);
  return (1); //Error
}


/**
  (CMD3, R6): Read cards relative address - SD specific

  \param[out]  rca  Relative card address buffer
  \param[in]   mc   Pointer to memory card instance object
  \return      true if address set successfully, false otherwise
*/
static uint32_t mc_publish_rca (uint16_t *rca, MC_MCI *mc) {
  int32_t stat;
  uint32_t r6, flags, events;

  mc->Event = 0;

  r6    = 0;
  flags = MC_RESPONSE_R6 | ARM_MCI_RESPONSE_CRC;
  stat  = mc->Driver->SendCommand (MC_CMD_SEND_RELATIVE_ADDR, 0, flags, &r6);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      if ((r6 & 0x0F00) == 0x0500) {
        /* Write cards RCA and return */
        *rca = r6 >> 16;
        return (0); //OK
      }
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_SEND_RELATIVE_ADDR, 0);
  return (1); //Error
}


/**
  (CMD5, R1b): Toggle between Sleep and Standy state - MMC/eMMC specific

  Command 5 argument bit assignment:
  [31:16] RCA
  [15] Sleep(=1)/Awake(=0)
  [14:0] stuff bits

  \param[in]   rca  Relative card address
  \param[in]   sleep  true = switch to sleep, false = switch to standby
  \param[in]   mc     Pointer to memory card instance object
*/
static uint32_t mc_sleep_awake (uint32_t rca, bool sleep, MC_MCI *mc) {
  int32_t stat;
  uint32_t r1, flags, events, arg;

  mc->Event = 0;

  arg   = (rca << 16) | ((sleep) ? (1 << 15) : (0));
  r1    = 0;
  flags = MC_RESPONSE_R1b | ARM_MCI_RESPONSE_CRC;
  stat = mc->Driver->SendCommand (MC_CMD_SLEEP_AWAKE, arg, flags, &r1);
  
  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_SLEEP_AWAKE, arg);
  return (1); //Error
}


/**
  (CMD6, R1b): Switch command - MMC/eMMC specific

  Command 6 argument bit assignment:
  [31:26] Set to 0
  [25:24] Access (00 = command set, 01 = set bits, 10 = clear bits, 11 = write byte)
  [23:16] Index (EXT_CSD byte number)
  [15:8] Value  (bit value)
  [7:3] Set to 0
  [2:0] Cmd Set

  \param[in]  acc  Access Bits
  \param[in]  idx  Byte number from the EXT_CSD register
  \param[in]  val  Operation value
  \param[in]  mc   Pointer to memory card instance object
*/
static uint32_t mc_switch (uint32_t acc, uint32_t idx, uint32_t val, MC_MCI *mc) {
  int32_t  stat;
  uint32_t r1, flags, events, arg;

  mc->Event = 0;

  arg   = (acc << 24) | (idx << 16) | (val << 8);
  flags = MC_RESPONSE_R1b | ARM_MCI_RESPONSE_CRC;
  stat  = mc->Driver->SendCommand (MC_CMD_SWITCH, arg, flags, &r1);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_SWITCH, arg);
  return (1); //Error
}


/**
  (CMD6, R1): Switch function - SD specific

  Command 6 argument bit assignment:
  [31]    Mode (0:Check function, 1:Switch function)
  [30:24] Reserved (All '0')
  [23:20] Reserved for function group 6 (0h or Fh)
  [19:16] Reserved for function group 5 (0h or Fh)
  [15:12] Function group 4 for Power Limit
  [11:8]  Function group 3 for Drive Strength
  [7:4]   Function group 2 for Command System
  [3:0]   Function group 1 for Access Mode
*/
static uint32_t mc_switch_func (SWITCH_FUNC_ARG fargs, uint8_t *buf, MC_MCI *mc) {
  int32_t  stat;
  uint32_t r1, flags, events, arg;

  mc->Event = 0;

  stat = mc->Driver->SetupTransfer (buf, 1, 64, ARM_MCI_TRANSFER_READ | ARM_MCI_TRANSFER_BLOCK);
  if (stat != ARM_DRIVER_OK) {
    /* Got error, can't do nothing about it */ 
    EvrFsMcMCI_ReadXferSetupError (mc->Instance, buf, 1, 64);
    return (1);
  }

  arg   = (uint32_t)((fargs.mode            << 31) |
                     (fargs.power_limit     << 12) |
                     (fargs.driver_strength << 8)  |
                     (fargs.command_system  << 4)  |
                     (fargs.access_mode));
  flags = MC_RESPONSE_R1 | ARM_MCI_RESPONSE_CRC | ARM_MCI_TRANSFER_DATA;
  stat  = mc->Driver->SendCommand (MC_CMD_SWITCH, arg, flags, &r1);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for command event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      /* Wait for transfer event */
      events = mc_wfe (MC_DAT_EVENTS, mc);

      if ((events & MC_DAT_EVENTS) == ARM_MCI_EVENT_TRANSFER_COMPLETE) {
        return (0); // OK
      }
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_SWITCH, arg);
  return (1); //Error
}


/**
  (CMD7, R1b): Select/Deselect the card, interchange internal state (TRAN <-> STBY)

  RCA == 0 is reserved for deselect. No response will be expected in that case.

  \param[in]   rca  Relative card address
  \param[out]  r1   R1 response buffer (Card Status)
  \param[in]   mc   Pointer to memory card instance object
  \return
*/
static uint32_t mc_select_deselect (uint32_t rca, uint32_t *r1, MC_MCI *mc) {
  int32_t stat;
  uint32_t flags, events;

  mc->Event = 0;

  flags = ((rca == 0) ? (MC_RESPONSE_NONE) : (MC_RESPONSE_R1b)) | ARM_MCI_RESPONSE_CRC;
  stat  = mc->Driver->SendCommand (MC_CMD_SELECT_DESELECT_CARD, rca << 16, flags, r1);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_SELECT_DESELECT_CARD, rca << 16);
  return (1); //Error
}


/**
  (CMD8, R7): Read OCR register of the addressed card

  \param[in]   arg  CMD8 argument (VHS + check pattern)
  \param[out]  r7   R7 response buffer (32-bit)
  \param[in]   mc   Pointer to memory card instance object
  \return
*/
static uint32_t mc_send_if_cond (uint32_t arg, uint32_t *r7, MC_MCI *mc) {
  int32_t stat;
  uint32_t flags, events;

  mc->Event = 0;

  *r7   = 0;
  flags = MC_RESPONSE_R7 | ARM_MCI_RESPONSE_CRC;
  stat  = mc->Driver->SendCommand (MC_CMD_SEND_IF_COND, arg, flags, r7);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_SEND_IF_COND, arg);
  return (1); //Error
}


/**
  (CMD8, R1): Send Extended CSD register as a block of data
  
  \param[out]  buf  Pointer to 512 byte buffer
  \param[in]   mc   Pointer to memory card instance object
  \return
*/
static uint32_t mc_send_ext_csd (uint8_t *buf, MC_MCI *mc) {
  int32_t stat;
  uint32_t r1, flags, events;
  
  mc->Event = 0;
  
  /* Setup data transfer */
  flags = ARM_MCI_TRANSFER_READ | ARM_MCI_TRANSFER_BLOCK;
  stat = mc->Driver->SetupTransfer (buf, 1, 512, flags);
  if (stat != ARM_DRIVER_OK) {
    /* Got error, can't do nothing about it */
    EvrFsMcMCI_ReadXferSetupError (mc->Instance, buf, 1, 512);
    return (1);
  }
  
  flags = MC_RESPONSE_R1 | ARM_MCI_RESPONSE_CRC | ARM_MCI_TRANSFER_DATA;
  stat  = mc->Driver->SendCommand (MC_CMD_SEND_EXT_CSD, 0, flags, &r1);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for command event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      /* Wait for data event */
      events = mc_wfe (MC_DAT_EVENTS, mc);
      if ((events & MC_DAT_EVENTS) == ARM_MCI_EVENT_TRANSFER_COMPLETE) {
        return (0); //OK
      }
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_SEND_EXT_CSD, 0);
  return (1); //Error
}

/**
  (CMD9, R2): Read CSD register of the addressed card

  \param[in]   rca  Relative card address
  \parma[out]  r2   R2 response buffer (array of 4 x 32-bit)
  \param[in]   mc   Pointer to memory card instance object
  \return
*/
static uint32_t mc_read_csd (uint32_t rca, uint32_t *r2, MC_MCI *mc) {
  int32_t stat;
  uint32_t flags, events;

  mc->Event = 0;

  flags = MC_RESPONSE_R2 | ARM_MCI_RESPONSE_CRC;
  stat  = mc->Driver->SendCommand (MC_CMD_SEND_CSD, rca << 16, flags, r2);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_SEND_CSD, rca << 16);
  return (1); //Error
}


/**
  (CMD10, R2): Read CID register of the addressed card

  \param[in]   rca  Relative card address
  \param[in]   r2   R2 response buffer (array of 4 x 32-bit)
  \param[in]   mc   Pointer to memory card instance object
  \return
*/
static uint32_t mc_read_cid (uint32_t rca, uint32_t *r2, MC_MCI *mc) {
  int32_t stat;
  uint32_t flags, events;

  mc->Event = 0;

  flags = MC_RESPONSE_R2 | ARM_MCI_RESPONSE_CRC;
  stat  = mc->Driver->SendCommand (MC_CMD_SEND_CID, rca << 16, flags, r2);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_SEND_CID, rca << 16);
  return (1); //Error
}


/**
  (CMD12, R1b): Force the card to stop transmission

  \param[in]   mc   Pointer to memory card instance object
  \return
*/
static uint32_t mc_stop_transmission (MC_MCI *mc) {
  int32_t  stat;
  uint32_t r1, flags, events;

  mc->Event = 0;

  flags = MC_RESPONSE_R1b | ARM_MCI_RESPONSE_CRC;
  stat  = mc->Driver->SendCommand (MC_CMD_STOP_TRANSMISSION, 0, flags, &r1);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_STOP_TRANSMISSION, 0);
  return (1); //Error
}


/**
  (CMD13, R1): Read memory card status register

  \param[in]   rca  Relative card address
  \param[out]  r1   R1 response (Card Status)
  \param[in]   mc   Pointer to memory card instance object
  \return
*/
static uint32_t mc_read_status (uint32_t rca, uint32_t *r1, MC_MCI *mc) {
  int32_t  stat;
  uint32_t flags, event;

  mc->Event = 0;

  flags = MC_RESPONSE_R1 | ARM_MCI_RESPONSE_CRC;
  stat  = mc->Driver->SendCommand (MC_CMD_SEND_STATUS, rca << 16, flags, r1);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    event = mc_wfe (MC_CMD_EVENTS, mc);

    if ((event & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_SEND_STATUS, rca << 16);
  return (1); //Error
}


/**
  (CMD16, R1): Set block length.

  \parma[in]   len  block length in bytes
  \param[in]   mc   Pointer to memory card instance object
  \return
*/
static uint32_t mc_set_block_len (uint32_t len, MC_MCI *mc) {
  int32_t stat;
  uint32_t r1, flags, events;

  mc->Event = 0;

  r1    = 0;
  flags = MC_RESPONSE_R1 | ARM_MCI_RESPONSE_CRC;
  stat  = mc->Driver->SendCommand (MC_CMD_SET_BLOCKLEN, len, flags, &r1);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      /* Check response */
      if ((r1 & R1_BLOCK_LEN_ERROR) == 0) {
        /* Block length accepted */
        return (0); //OK
      }
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_SET_BLOCKLEN, len);
  return (1); //Error
}


/**
  (CMD17 | CMD18, R1): Send single or multiple block read command

  \param[in]   block    First block number to be read
  \param[in]   multiple Multiple/single read command (true/false)
  \param[in]   mc       Pointer to memory card instance object
  \return
*/
static uint32_t mc_read_block (uint32_t block, bool multiple, MC_MCI *mc) {
  int32_t  stat;
  uint32_t r1, idx, arg, flags, events;

  mc->Event = 0;

  /* CMD18 or CMD17, R1 */
  idx = (multiple) ? (MC_CMD_READ_MULTIPLE_BLOCK) : (MC_CMD_READ_SINGLE_BLOCK);
  arg = block;

  if (mc->Property & MC_PROP_ACCESS_BYTE) {
    arg <<= 9;
  }

  flags = MC_RESPONSE_R1 | ARM_MCI_RESPONSE_CRC | ARM_MCI_TRANSFER_DATA;
  stat  = mc->Driver->SendCommand (idx, arg, flags, &r1);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, idx, arg);
  return (1); //Error
}


/**
  (CMD24 | CMD25, R1): Send single or multiple block write command

  \param[in]   block    Block number to be read
  \param[in]   multiple Multiple/single write command (true/false)
  \param[in]   mc       Pointer to memory card instance object
  \return
*/
static uint32_t mc_write_block (uint32_t block, bool multiple, MC_MCI *mc) {
  int32_t  stat;
  uint32_t r1, idx, arg, flags, events;

  mc->Event = 0;

  /* CMD25 or CMD24, R1 */
  idx = (multiple) ? (MC_CMD_WRITE_MULTIPLE_BLOCK) : (MC_CMD_WRITE_SINGLE_BLOCK);
  arg = block;

  if (mc->Property & MC_PROP_ACCESS_BYTE) {
    arg <<= 9;
  }

  flags = MC_RESPONSE_R1 | ARM_MCI_RESPONSE_CRC | ARM_MCI_TRANSFER_DATA;
  stat  = mc->Driver->SendCommand (idx, arg, flags, &r1);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, idx, arg);
  return (1); //Error
}


/**
  (CMD42, R1): Set/Reset the password or lock/unlock the card.

  \param[in]   mc   Pointer to memory card instance object
  \return
*/
static uint32_t mc_lock_unlock (MC_MCI *mc) {
  int32_t stat;
  uint32_t r1, flags, events;

  mc->Event = 0;

  flags = MC_RESPONSE_R1 | ARM_MCI_RESPONSE_CRC | ARM_MCI_TRANSFER_DATA;
  stat  = mc->Driver->SendCommand (MC_CMD_LOCK_UNLOCK, 0, flags, &r1);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      /* Check response */
      if ((r1 & R1_LOCK_UNLOCK_FAILED) == 0) {
        return (0); //OK
      }
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_LOCK_UNLOCK, 0);
  return (1); //Error
}


/**
  (CMD55, R1): Send CMD55 to enable application specific commands

  \param[in]   rca  Relative card address
  \param[in]   mc   Pointer to memory card instance object
  \return
*/
static uint32_t mc_enable_acmd (uint32_t rca, MC_MCI *mc) {
  int32_t stat;
  uint32_t r1, flags, events;

  mc->Event = 0;

  r1    = 0;
  flags = MC_RESPONSE_R1 | ARM_MCI_RESPONSE_CRC;
  stat  = mc->Driver->SendCommand (MC_CMD_APP_CMD, rca << 16, flags, &r1);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      /* Check response */
      if (r1 & R1_APP_CMD) {
        return (0); //OK
      }
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_APP_CMD, rca << 16);
  return (1); //Error
}


/**
  (CMD56, R1): General Command

  \param[in]   arg    Command argument (bit 0 is direction: 1 == read, 0 == write)
  \param[in]   mc     Pointer to memory card instance object
  \return 0: OK, 1: Error
*/
static uint32_t mc_gen_cmd (uint32_t arg, MC_MCI *mc) {
  int32_t  stat;
  uint32_t r1, flags, events;

  mc->Event = 0;

  flags = MC_RESPONSE_R1 | ARM_MCI_RESPONSE_CRC | ARM_MCI_TRANSFER_DATA;
  stat  = mc->Driver->SendCommand (MC_CMD_GEN_CMD, arg, flags, &r1);

  if (stat == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_CMD_GEN_CMD, arg);
  return (1); //Error
}


/**
  (ACMD6, R1): Select device data bus width (SD specific)

  \param[in]   arg  ACMD6 argument: bus width
  \param[in]   mc   Pointer to memory card instance object
  \return
*/
static uint32_t mc_set_bus_width (uint32_t arg, MC_MCI *mc) {
  int32_t err;
  uint32_t r1, flags, events;

  mc->Event = 0;

  r1    = 0;
  flags = MC_RESPONSE_R1 | ARM_MCI_RESPONSE_CRC;
  err   = mc->Driver->SendCommand (MC_ACMD_SD_SET_BUS_WIDTH, arg, flags, &r1);

  if (err == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      /* Check response */
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_ACMD_SD_SET_BUS_WIDTH, arg);
  return (1); //Error
}

/**
  (ACMD41, R3): Send operating condition register

  \param[in]   arg  Command argument (HCS, XPC, S18R, Vdd Voltage Window)
  \param[out]  r3   R3 response buffer (32-bit)
  \param[in]   mc   Memory card instance object
*/
static uint32_t mc_sd_send_op_cond (uint32_t arg, uint32_t *r3, MC_MCI *mc) {
  int32_t err;
  uint32_t flags, events;

  mc->Event = 0;

  *r3   = 0;
  flags = MC_RESPONSE_R3;
  err   = mc->Driver->SendCommand (MC_ACMD_SD_SEND_OP_COND, arg, flags, r3);

  if (err == ARM_DRIVER_OK) {
    /* Wait for event */
    events = mc_wfe (MC_CMD_EVENTS, mc);

    if ((events & MC_CMD_EVENTS) == ARM_MCI_EVENT_COMMAND_COMPLETE) {
      return (0); //OK
    }
  }
  EvrFsMcMCI_SendCommandError (mc->Instance, MC_ACMD_SD_SEND_OP_COND, arg);
  return (1); //Error
}


/**
  Fill extern CID structure with content read from CID register

  \param[in]   cid  Pointer to CID structure object
  \param[in]   r2   Pointer to R2 response buffer (content of CID register)
  \param[in]   mmc  MultiMediaCard true/false
*/
static void fill_cid (fsCID_Register *cid, uint32_t *r2, bool mmc) {
  uint8_t *b = (uint8_t *)r2;

  memset (cid, 0, sizeof (fsCID_Register));

  if (mmc) { /* MultiMediaCard */
    cid->MID    =  b[15];
/*  cid->CBX    =  b[14] & 0x3;  */
    cid->OID    =  b[13];
    cid->PNM[5] =  b[12];
    cid->PNM[4] =  b[11];
    cid->PNM[3] =  b[10];
    cid->PNM[2] =  b[9];
    cid->PNM[1] =  b[8];
    cid->PNM[0] =  b[7];
    cid->PRV    =  b[6];
    cid->PSN    = (uint32_t)((b[5] << 24) | (b[4] << 16) | (b[3] << 8) | b[2]);
    cid->MDT    =  b[1];
  }
  else { /* SD Card */
    cid->MID    =  b[15];
    cid->OID    = (uint16_t)((b[14] << 8) | b[13]);
    cid->PNM[4] =  b[12];
    cid->PNM[3] =  b[11];
    cid->PNM[2] =  b[10];
    cid->PNM[1] =  b[9];
    cid->PNM[0] =  b[8];
    cid->PRV    =  b[7];
    cid->PSN    = (uint32_t)((b[6] << 24) | (b[5] << 16) | (b[4] << 8) | b[3]);
    cid->MDT    =((b[2] << 8) | b[1]) & 0x0FFF;
  }
}


/**
  Determine maximum transfer speed from the CSD register data.
  \return maximum transfer speed in Hz
*/
static uint32_t get_tran_speed (uint32_t *r2) {
  static const uint8_t tim_val[] = {
    0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80
  };
  uint32_t i, n, m, speed;
  uint8_t *b;

  b = (uint8_t *)r2;

  /* Max. data transfer rate (TRAN_SPEED: CSD[103:96] */
  n = (b[12] >> 3) & 0xF;
  m = (b[12] & 0x7) + 1;
  speed = tim_val[n];
  for (i = 0; i < m; i++) {
    speed *= 10;
  }
  return (speed * 1000);
}

/**
  Get card command class support bits from the CSD register data.
  \return CCC value
*/
static uint32_t get_ccc (uint32_t *r2) {
  uint8_t *b;
  uint32_t ccc;

  b = (uint8_t *)r2;

  ccc  = (uint32_t)(b[11] << 4);
  ccc |= (uint32_t)(b[10] >> 4);
  
  return (ccc);
}


/**
  Determine number of blocks from the CSD register data.
  
  \param[in]   r2   Pointer to R2 response buffer (content of CSD register)
  \param[in]   mmc  MMC device (true/false)?
  \return number of 512B blocks
*/
static uint32_t get_block_number (uint32_t *r2, bool mmc) {
  uint32_t n, m, blen, cnt, ver;
  uint8_t *b;

  b = (uint8_t *)r2;

  /* Read Block Length (READ_BL_LEN: CSD[83:80] */
  n = b[10] & 0x0F;
  blen = 1 << n;

  /* Determine CSD structure version: CSD_STRUCTURE, CSD[127:126] */
  ver = b[15] >> 6;

  if (!mmc && (ver == 1U)) { /* SD: CSD v2.0 */
    /* Get C_SIZE: CSD[69:48] */
    m = (uint32_t)(((b[8] & 0x3F) << 16) | (b[7] << 8) | b[6]);
    cnt = (m + 1) << 10;
  }
  else {                     /* SD: CSD v1.0, MMC: CSD v1.0, v1.2, ... */
    /* Get C_SIZE: CSD[73:62] */
    m = (uint32_t)(((b[9] & 0x3) << 10) | (b[8] << 2) | (b[7] >> 6));

    if (mmc && (m == 0xFFF)) {
      /* Device density > 2GB, read EXT_CSD */
      return (0U);
    }

    /* Get C_SIZE_MULT: CSD[49:47] */
    n = (uint32_t)(((b[6] & 3) << 1) | (b[5] >> 7));
    cnt = (m + 1) << (n + 2);
  }

  /* Return block count in multiple of sector size */
  switch (blen) {
    case   512: m =  1; break; /* SD, MMC (1GB) */
    case  1024: m =  2; break; /* SD, MMC (2GB) */
    case  2048: m =  4; break; /* SD, MMC (4GB) */
    case  4096: m =  8; break; /* eMMC */
    case  8192: m = 16; break; /* eMMC */
    case 16384: m = 32; break; /* eMMC */

    default:
      /* Nonstandard block length */
      return (0);
  }
  return (cnt * m);
}


/**
  Retrieve CSD Register and System Specification version from the CSD register

  \param[in]   r2   Pointer to R2 response buffer (content of CSD register)
  \param[in]   mc   Memory card instance object
  \return
*/
static uint32_t read_mmc_version (uint32_t *r2, MC_MCI *mc) {
  uint8_t *b = (uint8_t *)r2;
  
  /* Read SPEC_VERS [125:122] */
  if (((b[15] >> 2) & 0x0F) >= 4) {
    mc->Property |= MC_PROP_MMC_V4;
  }

  return (0); //OK
}


/**
  Control eMMC SLEEP/AWAKE state.
  
  SLEEP state was introduced in Jedec Standard eMMC V4.3. If device meets
  V4.3 specification, this function will put it in Sleep State if device is
  currently in Standby State. After valid command response, VCC will
  also be switched OFF.
  When device is in Sleep State, VCC will be first switched on and after
  2ms delay, device will be put in Standby State.

  \param[in]   code   Sleep control code (FS_CONTROL_EMMC_SLEEP/_AWAKE)
  \param[in]   mc     Memory card instance object
  \return      execution status \ref fsStatus
*/
static fsStatus mc_control_sleep (uint32_t code, MC_MCI *mc) {
  fsStatus status;
  uint32_t err, r1;

  EvrFsMcMCI_SleepAwakeControl (mc->Instance);

  status = fsError;

  if (mc->Status & MC_STATUS_EMMC_SLEEP) {
    /* Device is in SLP state */
    if (code == FS_CONTROL_EMMC_AWAKE) {
      /* Power up VCC */
      if (mc->Capabilities.vdd) {
        /* Setting power, VDD = 3V3 */
        EvrFsMcMCI_CardPowerControl (mc->Instance, ARM_MCI_POWER_VDD_3V3);
        mc->Driver->CardPower (ARM_MCI_POWER_VDD_3V3);
        fs_set_rtos_delay(2);
      }

      if (mc_sleep_awake (mc->RCA, false, mc) == 0U) {
        EvrFsMcMCI_AwakeActive (mc->Instance);
        /* Device is AWAKE */
        mc->Status &= ~MC_STATUS_EMMC_SLEEP;
        status = fsOK;
      }
    }
    else {
      /* Device already in SLP state */
      status = fsOK;
    }
  }
  else {
    if (code == FS_CONTROL_EMMC_SLEEP) {
      /* Device must be in STBY state in order to enter SLP */
      err = mc_read_status (mc->RCA, &r1, mc);

      if (err == 0U) {
        if ((r1 & R1_STATE_Msk) == R1_STATE_STBY) {
          if (mc_sleep_awake (mc->RCA, true, mc) == 0U) {
            EvrFsMcMCI_SleepActive (mc->Instance);
            /* Device is in SLEEP */
            mc->Status |= MC_STATUS_EMMC_SLEEP;

            /* Power down VCC */
            if (mc->Capabilities.vdd) {
              EvrFsMcMCI_CardPowerControl (mc->Instance, ARM_MCI_POWER_VDD_OFF);
              mc->Driver->CardPower (ARM_MCI_POWER_VDD_OFF);
            }
            status = fsOK;
          }
        }
      }
    }
  }
  return (status);
}


/**
  Switch card from arbitrary state to TRAN state

  Device status is first queried for current state. Depending on retrieved state,
  various operations are executed:
    - if current state is DIS or STBY: CMD7 is executed to switch device state
      from DIS -> PRG or STBY -> TRAN.
    - if current state is PRG: function waits transition to TRAN state for 500ms
      and returns with timeout error if state is not changed
    - if current state is RCV or DATA: current data transfer is aborted with CMD12,
      then function waits for transition to TRAN state

  \param[in]   mc     Memory card instance object
  \return      execution status
               - 0: success, card is in TRAN state
               - 1: timeout or command execution error
*/
static uint32_t mc_select_tran_state (MC_MCI *mc) {
  uint32_t err, status, r1, card_state, tout;

  tout   = 0;
  err    = 0;
  status = 1;

  while (err == 0 && status != 0) {
    /* Read status */
    r1    = 0;
    err   = mc_read_status (mc->RCA, &r1, mc);

    if (err == 0) {
      card_state = r1 & R1_STATE_Msk;

      switch (card_state) {
        case R1_STATE_DIS:
          /* Switch state: DIS -> PRG */
        case R1_STATE_STBY:
          /* Switch state: STBY -> TRAN */
          err = mc_select_deselect (mc->RCA, &r1, mc);
          break;

        case R1_STATE_TRAN:
          if (r1 & R1_READY_FOR_DATA) {
            status = 0;
          }
          break;

        case R1_STATE_PRG:
          /* Wait at least 500ms for TRAN state till timeout */
          if (tout == 0) {
            tout = fs_get_sys_tick();
          }
          else {
            if ((fs_get_sys_tick() - tout) >= fs_get_sys_tick_us(600000)) {
              /* Timeout while waiting for TRAN state (500ms + 100ms overhead) */
              EvrFsMcMCI_DeviceStateTimeout (mc->Instance, R1_STATE_PRG, R1_STATE_TRAN);
              err = 1;
            }
          }
          break;

        case R1_STATE_RCV:
        case R1_STATE_DATA:
          /* Device state is invalid */
          EvrFsMcMCI_DeviceStateInvalid (mc->Instance, card_state);

          /* Aborting incomplete data transfer */
          EvrFsMcMCI_TransferAbort (mc->Instance);
          mc->Driver->AbortTransfer();

          err = mc_stop_transmission (mc);
          break;

        default:
          /* Cannot select the device, device state is unknown */
          EvrFsMcMCI_DeviceStateUnknown (mc->Instance, card_state);
          err = 1;
          break;
      }
    }
  }
  return (status);
}


/**
  Perform card Lock/Unlock operation

  This function must be called when device is in TRAN state.

  \param[in]   lu     Lock/Unlock structure
  \param[in]   mc     Memory card instance object
*/
static fsStatus mc_control_lock (fsLockUnlock *lu, MC_MCI *mc) {
  __ALIGNED(32) uint8_t buf[32];
  int32_t rval;
  uint32_t err, cnt, events, r1;
  fsStatus status;

  /* Executing lock/unlock */
  EvrFsMcMCI_LockUnlockExec (mc->Instance);

  if ((lu == NULL)       ||
      (lu->length > 16U) ||
      (((lu->password == NULL) && (lu->flags & FS_DEVICE_ERASE) == 0))) {
    /* Invalid function parameters */
    EvrFsMcMCI_ParameterInvalid (mc->Instance);
    status = fsInvalidParameter;
  }
  else {
    status = fsError;

    if (lu->flags & FS_DEVICE_ERASE) {
      cnt = 1U;
    } else {
      cnt = 32U;
      /* Note: this should be cnt = lu->length + 2; but some peripherals */
      /* are not able to send custom sized blocks. Hopefully, all        */
      /* peripherals should be able to send block sized to the power of  */
      /* two, therefore we set block length to 32.                       */
    }

    /* Set block length in bytes (pass + 8-bit pass length + 8-bit mode */
    err = mc_set_block_len (cnt, mc);

    if (err == 0) {
      /* Prepare data */
      buf[0] = lu->flags & 0x0F;
      buf[1] = lu->length;
      memcpy (&buf[2], lu->password, lu->length);

      rval = mc->Driver->SetupTransfer (buf, 1, cnt, ARM_MCI_TRANSFER_WRITE | ARM_MCI_TRANSFER_BLOCK);
      if (rval != ARM_DRIVER_OK) {
        /* Got error, can't do nothing about it */
        EvrFsMcMCI_WriteXferSetupError (mc->Instance, buf, 1, cnt);
        err    = 1U;
        status = fsDriverError;
      }
    }

    if (err == 0U) {
      err = mc_lock_unlock (mc);

      if (err == 0U) {
        /* Wait here until transfer event arrives */
        events = mc_wfe (MC_DAT_EVENTS, mc);

        if ((events & MC_DAT_EVENTS) != ARM_MCI_EVENT_TRANSFER_COMPLETE) {
          /* Transfer incomplete */
          EvrFsMcMCI_TransferError (mc->Instance, events);
          err = 1U;
        }
      }
      /* Clear buffer (don't leave password on stack) */
      memset (buf, 0, sizeof (buf));

      if (err == 0U) {
        status = fsOK;

        do {
          /* Wait for state transition (RCV -> PRG -> TRAN) */
          err = mc_read_status (mc->RCA, &r1, mc);

          if (err == 0U) {
            /* Check for non-sticky LOCK_UNLOCK_FAILED flag */
            if (r1 & R1_LOCK_UNLOCK_FAILED) {
              /* Lock/Unlock failed */
              EvrFsMcMCI_LockUnlockExecError (mc->Instance);
              status = fsAccessDenied;
            }
          }
          else {
            status = fsError;
            break;
          }
        }
        while ((r1 & R1_STATE_Msk) != R1_STATE_TRAN);

        if (err == 0U) {
          if (r1 & R1_CARD_IS_LOCKED) {
            /* Device is locked */
            EvrFsMcMCI_LockActive (mc->Instance);
            mc->Status |=  MC_STATUS_LOCKED;
          } else {
            /* Device is unlocked */
            EvrFsMcMCI_LockDeactivated (mc->Instance);
            mc->Status &= ~MC_STATUS_LOCKED;
          }
        }
      }
    }
  }

  return (status);
}


/**
  Selects beetwen 4-bit and 8-bit data bus width during device initialization.

  \param[in]   mc     Memory card instance object
  \return      execution status
*/
static uint32_t mc_select_bus_width (MC_MCI *mc) {
  uint32_t err, arg, tout, r1;

  err = 0;

  if (mc->Property & MC_PROP_TYPE_SD) {
    /* SD devices can run in 4-bit data bus mode */
    if (mc->Capabilities.data_width_4) {
      mc->Property |= MC_PROP_DATA_BUS_4BIT;

      err = mc_enable_acmd (mc->RCA, mc);

      if (err == 0) {
        err = mc_set_bus_width (ACMD6_ARG_BUS_WIDTH_4BIT, mc);
      }
    }
  }
  else {
    if (mc->Property & MC_PROP_MMC_V4) {
      arg = 0;

      if (mc->Capabilities.data_width_8) {
        mc->Property |= MC_PROP_DATA_BUS_8BIT;
        arg = 2;
      }
      else {
        if (mc->Capabilities.data_width_4) {
          mc->Property |= MC_PROP_DATA_BUS_4BIT;
          arg = 1;
        }
      }

      if (arg) {
        /* Select 4-bit/8-bit SDR data bus */
        err = mc_switch (3, 183, arg, mc);

        if (err == 0) {
          /* Wait at least 500ms till in PRG state */
          tout = fs_get_sys_tick();
          do {
            mc_read_status (mc->RCA, &r1, mc);

            if ((r1 & R1_STATE_Msk) != R1_STATE_PRG) {
              break;
            }
          } while ((fs_get_sys_tick() - tout) >= fs_get_sys_tick_us(500000));
        }
      }
    }
  }
  
  if (err == 0) {
    arg = 0;

    if (mc->Property & MC_PROP_DATA_BUS_8BIT) {
      arg = ARM_MCI_BUS_DATA_WIDTH_8;
    }
    else {
      if (mc->Property & MC_PROP_DATA_BUS_4BIT) {
        arg = ARM_MCI_BUS_DATA_WIDTH_4;
      }
    }

    if (arg) {
      mci_bus_width (arg, mc);
      /* Wait a bit */
      fs_set_rtos_delay (2);
    }
  }
  else {
    /* Data bus width is still in 1-bit mode */
    mc->Property &= ~(MC_PROP_DATA_BUS_4BIT | MC_PROP_DATA_BUS_8BIT);
  }

  return (err);
}


/**
  Initialize memory card in native mode (non SPI)

  \param[in]   mc     Pointer to memory card instance object
  \return
*/
static uint32_t mc_native_init (MC_MCI *mc) {
  fsCID_Register cid;
  uint32_t err, r[4], arg, tout, speed;
  SWITCH_FUNC_ARG sw_arg;
  fsStatus status;

  /* Set CMD line to Push-Pull mode */
  mci_cmd_bus_mode (ARM_MCI_BUS_CMD_PUSH_PULL, mc);

  EvrFsMcMCI_MediaReset (mc->Instance);

  /* CMD0 */
  mc_go_idle_state (mc);

  /** SD Identification Process **/

  EvrFsMcMCI_MediaDetectionSD (mc->Instance);


  /* Physical Layer Specification Version 2.00 requests to send CMD8 */
  if (mc_send_if_cond (0x1AA, &r[0], mc) == 0) {
    if (r[0] == 0x1AA) {
      /* Response OK, this is Ver2.00 SD Memory Card */
      mc->Property |= MC_PROP_SD_V2;

      EvrFsMcMCI_MediaSD_V2 (mc->Instance);
    }
  }

  arg = ACMD41_ARG_VDD_WINDOW;

  if (mc->Property & MC_PROP_SD_V2) {
    /* Set HCS, XPC and S18R in argument of ACMD41 */
    arg |= ACMD41_ARG_HCS | ACMD41_ARG_XPC | ACMD41_ARG_S18R;
  }

  for (tout = 1000; tout; tout -= 10) {
    if (mc_enable_acmd (0, mc) == 0) {
      err = mc_sd_send_op_cond (arg, &r[0], mc);

      if (err == 0) {
        /* Got response */
        mc->Property |= MC_PROP_TYPE_SD;

        if (r[0] & R3_BUSY) {
          /* Device ready, (OCR=0x%X) */
          EvrFsMcMCI_MediaReady (mc->Instance, r[0]);

          /* SD card finished the power up routine */
          if (!(r[0] & R3_CCS)) {
            /* SD Standard Capacity Card */
            mc->Property |= MC_PROP_ACCESS_BYTE;
          }

          if (r[0] & R3_UHSII) {
            /* This is UHS-II card */
            mc->Property |= MC_PROP_SPEED_UHSII;
          }

          if (r[0] & R3_S18A) {
            /* Switching to 1.8V Accepted, this is UHS-I card */
            mc->Property |= MC_PROP_SPEED_UHSI;
          }
          break;
        }
        /* Wait 10ms */
        fs_set_rtos_delay (10);
      }
    }
    else {
      /* No response to CMD55 */
      EvrFsMcMCI_MediaNoResponse (mc->Instance);
      break;
    }
  }

  /* May be MMC card if device type still unknown */
  if (!(mc->Property & MC_PROP_TYPE_SD)) {
    /* Detecting MMC/eMMC... */
    EvrFsMcMCI_MediaDetectionMMC (mc->Instance);

    /* Set Open Drain output control for MMC. */
    mci_cmd_bus_mode (ARM_MCI_BUS_CMD_OPEN_DRAIN, mc);

    /* CMD0 */
    mc_go_idle_state (mc);

    /* Check for MMC, send CMD1 */
    for (tout = 1000; tout; tout -= 10) {
      err = mc_send_op_cond (0x40FF8000, &r[0], mc);
      
      if (err == 0) {
        /* Got response */
        mc->Property |= MC_PROP_TYPE_MMC;

        if (r[0] & R3_BUSY) {
          /* Device ready, (OCR=0x%X) */
          EvrFsMcMCI_MediaReady (mc->Instance, r[0]);

          if (r[0] & R3_1V7_1V95) {
            /* Dual Voltage device */
            if (mc->Capabilities.vccq_1v8 || mc->Capabilities.vccq_1v2) {
              /* Low-voltage power-up possible */
              mc->Property |= MC_PROP_MMC_DUALV;
            }
          }
          if (!(r[0] & R3_CCS)) {
            /* Capacity <= 2GB */
            mc->Property |= MC_PROP_ACCESS_BYTE;
          }
          break;
        }
        /* Wait 10ms */
        fs_set_rtos_delay (10);
      }
      else {
        EvrFsMcMCI_MediaNoResponse (mc->Instance);
        /* Wait 10ms */
        fs_set_rtos_delay (10);
      }
    }
  }

  if ((mc->Property & (MC_PROP_TYPE_SD | MC_PROP_TYPE_MMC)) == 0) {
    /* Unknown device */
    EvrFsMcMCI_MediaDetectionError (mc->Instance);
    return (false);
  }

  mc->MediaStatus |= FS_MEDIA_INSERTED;

  /* Initialize the Card to SD/MMC mode. */
  if (mc_read_any_cid (&r[0], mc) != 0) {
    /* CID read failed - this should succeed */
    EvrFsMcMCI_CidReadError (mc->Instance);
    return (false);
  }
  /* Reformat response and fill CID structure */
  fill_cid (&cid, &r[0], mc->Property & MC_PROP_TYPE_MMC);
  /* Remember product serial number */
  mc->SerialNumber = cid.PSN;

  /** IDENT state **/

  if (mc->Property & MC_PROP_TYPE_SD) {
    /* Request SD card relative address */
    if (mc_publish_rca (&mc->RCA, mc) != 0) {
      /* This should succeed */
      EvrFsMcMCI_RcaReadError (mc->Instance);
      return (false);
    }
  }
  else {
    /* Set MMC relative address */
    mc->RCA = 1;

    if (mc_set_rca (1, mc) != 0) {
      /* Failed to set RCA - this should succeed */
      EvrFsMcMCI_RcaWriteError (mc->Instance);
      return (false);
    }
    /* Switch command line to push pull */
    mci_cmd_bus_mode (ARM_MCI_BUS_CMD_PUSH_PULL, mc);
  }

  /** Data Transfer Mode (Stand-by State) **/

  /* Read memory card CSD register */
  if (mc_read_csd (mc->RCA, &r[0], mc) != 0) {
    /* Failed to retrieve CSD register - this should succeed */
    EvrFsMcMCI_CsdReadError (mc->Instance);
    return (false);
  }
  /* Read number of blocks (from CSD) */
  mc->SectorCount = get_block_number (&r[0], (mc->Property & MC_PROP_TYPE_MMC) != 0);

  /* Determine maximum transfer speed (from CSD) */
  speed = get_tran_speed (&r[0]);
  /* Get card command class support bits */
  arg = get_ccc (&r[0]);

  if (arg & (1U << 10)) {
    mc->Property |= MC_PROP_CCC_10;
  }

  if (mc->Property & MC_PROP_TYPE_MMC) {
    read_mmc_version (&r[0], mc);
  }

  if (mc_select_deselect(mc->RCA, &r[0], mc) != 0) {
    /* Switch to TRAN state failed - this should succeed */
    EvrFsMcMCI_TranStateError (mc->Instance);
    return (false);
  }

  /** Data Transfer Mode (Transfer State) **/

  if (r[0] & R1_CARD_IS_LOCKED) {
    /* Password protection is active */
    EvrFsMcMCI_MediaPasswordEnabled (mc->Instance);

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

  /* Set block length to 512 bytes. */
  if (mc_set_block_len (512, mc)) {
    EvrFsMcMCI_BlenWriteError (mc->Instance, 512);
    return (false);
  }

  if (mc->Property & MC_PROP_MMC_V4) {
    if (mc->SectorCount == 0U) {
      /* Load Extended CSD to read real capacity */
      if (mc_send_ext_csd (mc->ExtCSD, mc)) {
        /* Failed to retrieve EXT_CSD register */
        EvrFsMcMCI_ExtCsdReadError (mc->Instance);
        return (false);
      }
      /* Only sector count field (SEC_COUNT [215:212]) is in our interest */
      mc->SectorCount = (uint32_t)(mc->ExtCSD[215] << 24 |
                                   mc->ExtCSD[214] << 16 |
                                   mc->ExtCSD[213] <<  8 |
                                   mc->ExtCSD[212]);
    }
    /* High-speed mode selection for MMC V4.0 and higher */
    if (mc_switch (3, 185, 1, mc)) {
      /* Failed to switch to high-speed mode */
      EvrFsMcMCI_HighSpeedSwitchError (mc->Instance);
      return (false);
    }
    /* Wait at least 500ms till in "prg" state */
    tout =  fs_get_sys_tick();
    do {
      mc_read_status (mc->RCA, &r[0], mc);

      if ((r[0] & R1_STATE_Msk) != R1_STATE_PRG) {
        break;
      }
    } while ((fs_get_sys_tick() - tout) >= fs_get_sys_tick_us(500000));
  }

  /* Set bus speed */
  mci_bus_speed (speed, mc);
  /* Wait a bit for clock to stabilize */
  fs_set_rtos_delay (2);
  
  /* Set data bus width */
  if (mc_select_bus_width (mc) != 0) {
    /* Failed to switch data bus width */
    EvrFsMcMCI_DataBusSwitchError (mc->Instance);
    return (false);
  }

  if ((mc->Property & (MC_PROP_TYPE_SD | MC_PROP_CCC_10)) == (MC_PROP_TYPE_SD | MC_PROP_CCC_10)) {
    /* Switch to High-Speed mode */
    if (mc->Capabilities.high_speed) {
      sw_arg.mode            = CMD6_MODE_CHECK;
      sw_arg.power_limit     = CMD6_POWER_LIMIT_CURRENT;
      sw_arg.driver_strength = CMD6_DRIVER_STRENGTH_CURRENT;
      sw_arg.command_system  = CMD6_COMMAND_SYSTEM_CURRENT;
      sw_arg.access_mode     = CMD6_ACCESS_MODE_SDR25;

      if (mc_switch_func (sw_arg, mc->ExtCSD, mc) == 0) {
        /* Check if High-Speed/SDR25 support bit is set */
        if (mc->ExtCSD[13] & (1U << CMD6_ACCESS_MODE_SDR25)) {
          /* Switch to SDR25 (High-Speed mode) */
          sw_arg.mode = CMD6_MODE_SET;

          if (mc_switch_func (sw_arg, mc->ExtCSD, mc) == 0) {
            fs_set_rtos_delay(2);
            /* High-Speed mode enabled */
            speed = 50000000U;
            mci_bus_speed_mode (ARM_MCI_BUS_HIGH_SPEED, mc);
            mci_bus_speed      (speed, mc);
            fs_set_rtos_delay(2);
          }
          else {
            /* Failed to switch to high-speed mode */
            EvrFsMcMCI_HighSpeedSwitchError (mc->Instance);

            /* Abort incomplete data transfer */
            EvrFsMcMCI_TransferAbort (mc->Instance);
            mc->Driver->AbortTransfer();
          }
        }
      }
    }
  }

  /* Set data timeout to 550ms (500ms + 50ms overhead) */
  mci_data_timeout (550, speed, mc);

  /* Switch device to Standby state */
  if (mc_select_deselect (0, &r[0], mc)) {
    EvrFsMcMCI_StbyStateError (mc->Instance);
    return (false);
  }

  /** Data Transfer Mode (Stand-by State) **/

  /* Native mode initialization done */
  mc->MediaStatus |= FS_MEDIA_INITIALIZED;

  EvrFsMcMCI_InitSuccess (mc->Instance);

  return (true);
}


/**
  Read or write data using general command.

  \param[in]   arg      command argument, bit 0 is direction: 1=read, 0=write
  \param[out]  buf      data buffer
  \param[in]   mc       memory card instance object
  \return true if read succeeded, false otherwise
*/
static uint32_t mc_general_command (uint32_t arg, uint8_t *buf, MC_MCI *mc) {
  int32_t  rval;
  uint32_t err, r1, events;
  uint32_t mode, cnt;
  uint8_t  state, retry;

  err   = 0U;
  cnt   = 1U;
  retry = 0U;
  state = MC_STATE_INIT;

  if ((mc->MediaStatus & FS_MEDIA_INITIALIZED) == 0) {
    /* Media is not initialized */
    EvrFsMcMCI_MediaNotInitialized (mc->Instance);
    err = 1;
  }
  if (mc->Status & MC_STATUS_LOCKED) {
    /* Password protection is active */
    EvrFsMcMCI_MediaPasswordEnabled (mc->Instance);
    err = 1;
  }

  while (err == 0) {
    switch (state) {
      case MC_STATE_INIT: /* Switch device to TRAN state */
        err = mc_select_tran_state (mc);

        if (err == 0) {
          /* Set next state */
          state = MC_STATE_SETUP_XFER;
        }
        break;

      case MC_STATE_SETUP_XFER:
        if ((arg & 1U) == 1U) {
          /* Setup read transfer */
          mode = ARM_MCI_TRANSFER_READ;
        } else {
          /* Setup write transfer */
          mode = ARM_MCI_TRANSFER_WRITE;
        }

        /* Setup transfer and send read command */
        rval = mc->Driver->SetupTransfer (buf, cnt, 512U, mode | ARM_MCI_TRANSFER_BLOCK);

        if (rval != ARM_DRIVER_OK) {
          /* Got error, can't do nothing about it */
          if ((arg & 1U) == 1U) {
            EvrFsMcMCI_ReadXferSetupError  (mc->Instance, buf, cnt, 512U);
          } else {
            EvrFsMcMCI_WriteXferSetupError (mc->Instance, (uint8_t *)buf, cnt, 512U);
          }
          state = MC_READ_ERROR;
        }
        else {
          /* Set next state */
          state = MC_STATE_SEND_CMD;
        }
        break;

      case MC_STATE_SEND_CMD:
        /* Send general command */
        err = mc_gen_cmd (arg, mc);

        if (err == 0) {
          /* Set next state */
          state = MC_STATE_WAIT;
        }
        break;

      case MC_STATE_WAIT:
        /* Wait here until transfer event arrives */
        events = mc_wfe (MC_DAT_EVENTS, mc);

        if ((events & MC_DAT_EVENTS) != ARM_MCI_EVENT_TRANSFER_COMPLETE) {
          /* Transfer incomplete */
          EvrFsMcMCI_TransferError (mc->Instance, events);

          /* Reset state and retry */
          err = 1;
        }
        else {
          /* Set next state */
          if ((arg & 1U) == 1U) {
            state = MC_STATE_STBY;
          } else {
            state = MC_STATE_WAIT_WRITE;
          }
        }
        break;

      case MC_STATE_WAIT_WRITE:
        do {
          /* Wait for state transition (RCV -> PRG/TRAN) */
          err = mc_read_status (mc->RCA, &r1, mc);

          if (err != 0) {
            break;
          }
        }
        while ((r1 & R1_STATE_Msk) == R1_STATE_RCV);

        if (err == 0) {
          /* Set next state */
          state = MC_STATE_STBY;
        }
        break;

      case MC_STATE_STBY:
        /* Disconnect memory card                                      */
        /* Read, states: (TRAN -> STBY)                                */
        /* Write,states: (PRG -> DIS -> STBY) or (PRG -> TRAN -> STBY) */
        mc_select_deselect (0, &r1, mc);
        return (true);

      case MC_STATE_ERROR:
      default: /* Fatal system error */
        /* Bail out */
        err   = 1;
        retry = MC_RETRY_OPERATION_COUNT;
        break;
    }

    if (err && (retry < MC_RETRY_OPERATION_COUNT)) {
      retry++;

      EvrFsMcMCI_TransferRetry (mc->Instance, retry);

      err   = 0;
      state = MC_STATE_INIT;
    }
  }
  /* Command failed */
  return (false);
}


/**
  fsDevCtrlCodeHealthStatus device control handler

  \param[in,out] p    Pointer to structure specifying health status argument and buffer
  \param[in]     mc   Pointer to memory card instance object
*/
static fsStatus mc_fsDevCtrlCodeHealthStatus (fsHealthStatus *p, MC_MCI *mc) {
  fsStatus stat;

  /* Check buffer alignment and size */
  if ((((uint32_t)p->buf & 3U) != 0U) || (p->buf_sz < 512U)) {
    /* Buffer is not 4-byte aligned or is too small */
    stat = fsError;
  }
  else {
    /* General command */
    if (mc_general_command (p->arg, p->buf, mc) == true) {
      stat = fsOK;
    } else {
      stat = fsError;
    }
  }

  return (stat);
}

/* ------------------------- Exported functions ----------------------------- */


/**
  Initialize MCI interface and memory card

  \param[in]   mode   initialization mode
  \param[in]   mc     memory card instance object
  \return true if initialization passed, false otherwise
*/
uint32_t mc_mci_Init (uint32_t mode, MC_MCI *mc) {

  if (mode == DM_IO) {
    mc->MediaStatus = 0;

    if ((mc->Status & MC_STATUS_DRIVER_INIT) == 0) {
      /* Init MCI driver */
      EvrFsMcMCI_InitDriver (mc->Instance, (uint32_t)mc->Driver);

      if (mc->Driver->Initialize (mc->Callback) != ARM_DRIVER_OK) {
        EvrFsMcMCI_InitDriverError (mc->Instance, (uint32_t)mc->Driver);
        return (false);
      }
      mc->Status |= MC_STATUS_DRIVER_INIT;

      /* Get capabilities */
      mc->Capabilities = mc->Driver->GetCapabilities();

      /* Power-on SDIO peripheral */
      /* This must be here because upper layer calls CheckMedia */
      /* which read CD and WP pins, therefore they must be configured */
      if (mc->Driver->PowerControl (ARM_POWER_FULL) != ARM_DRIVER_OK) {
        /* Driver power control failed */
        EvrFsMcMCI_InitDriverPowerError (mc->Instance, (uint32_t)mc->Driver, ARM_POWER_FULL);
        return (false);
      }
    }
    return (true);
  }
  else if (mode == DM_MEDIA) {
    /* Initializing memory media */
    EvrFsMcMCI_InitMedia (mc->Instance);

    /* Init runtime info */
    mc->Property    = 0;
    mc->RCA         = 0;
    mc->Event       = 0;

    if (mc->Capabilities.cd_state) {
      if (mc->Driver->ReadCD() == 0) {
        /* Card is not inserted, were done! */
        EvrFsMcMCI_MediaCdInactive (mc->Instance);
        return (false);
      }
      mc->MediaStatus |= FS_MEDIA_INSERTED;
    }

    if (mc->Capabilities.rst_n) {
      /* Assert RST_n pin for ~2ms */
      mc->Driver->Control (ARM_MCI_CONTROL_RESET, 1);
      fs_set_rtos_delay (2);
      mc->Driver->Control (ARM_MCI_CONTROL_RESET, 0);
    }

    /* Set 1-bit data bus (mandatory support) */
    mci_bus_width (ARM_MCI_BUS_DATA_WIDTH_1, mc);

    /* Set bus speed mode to default (mandatory support) */
    mci_bus_speed_mode (ARM_MCI_BUS_DEFAULT_SPEED, mc);

    /* Set data timeout to 550ms (500ms + 50ms overhead) */
    mci_data_timeout (550, MC_INIT_BUS_SPEED, mc);

    /* Set bus speed to 400kHz (initialization speed) */
    mci_bus_speed (MC_INIT_BUS_SPEED, mc);
    /* Wait a bit for clock to stabilize */
    fs_set_rtos_delay (2);

    /* Device power-up sequence */
    mci_card_power (true, mc);

    /* Enable clock when bus idle */
    mci_idle_clock (true, mc);
    /* Wait a bit in order to send additional init clocks */
    fs_set_rtos_delay(2);

    /* Disable clock when bus idle */
    mci_idle_clock (false, mc);

    /* Initialize media in native mode */
    return (mc_native_init (mc));
  }
  else {
    return (false);
  }
}


/**
  Uninitialize memory card interface

  \param[in]   mode     uninitialization mode
  \param[in]   mc       memory card instance object
  \return true on success, false otherwise
*/
uint32_t mc_mci_UnInit (uint32_t mode, MC_MCI *mc) {

  if (mode == DM_IO) {
    /* Uninitialize driver */
    EvrFsMcMCI_UninitDriver (mc->Instance, (uint32_t)mc->Driver);

    /* Clear driver initialization flags */
    mc->Status = 0;

    if (mc->Driver->PowerControl (ARM_POWER_OFF) == ARM_DRIVER_OK) {
      /* Uninit MCI peripheral and IO. */
      return (mc->Driver->Uninitialize () == ARM_DRIVER_OK);
    }
  }
  else {
    if (mode == DM_MEDIA) {
      /* Uninit memory device */
      EvrFsMcMCI_UninitMedia (mc->Instance);

      mc->MediaStatus &= ~FS_MEDIA_INITIALIZED;
      mc->Property     = 0;

      /* Power down the device */
      mci_card_power (false, mc);
      return (true);
    }
  }

  return (false);
}


/**
  Read a cnt of 512 byte sectors from memory card

  \param[in]   sect     sector number
  \param[out]  buf      sector data buffer
  \param[in]   cnt      number of sectors to write
  \param[in]   mc       memory card instance object
  \return true if read succeeded, false otherwise
*/
uint32_t mc_mci_ReadSector (uint32_t sect, uint8_t *buf, uint32_t cnt, MC_MCI *mc) {
  int32_t  rval;
  uint32_t err, r1, events;
  uint8_t  state, retry;

  EvrFsMcMCI_SectorRead (mc->Instance, sect, cnt);

  err   = 0U;
  retry = 0U;
  state = MC_READ_INIT;

  if ((mc->MediaStatus & FS_MEDIA_INITIALIZED) == 0) {
    /* Media is not initialized */
    EvrFsMcMCI_MediaNotInitialized (mc->Instance);
    err = 1U;
  }
  if (mc->Status & MC_STATUS_LOCKED) {
    /* Password protection is active */
    EvrFsMcMCI_MediaPasswordEnabled (mc->Instance);
    err = 1U;
  }

  while (err == 0U) {
    switch (state) {
      case MC_READ_INIT: /* Switch device to TRAN state */
        err = mc_select_tran_state (mc);
        if (err == 0U) {
          state = MC_READ_DATA;
        }
        break;

      case MC_READ_DATA: /* Read data */
        /* Setup transfer and send write block command */
        rval = mc->Driver->SetupTransfer (buf, cnt, 512U, ARM_MCI_TRANSFER_READ | ARM_MCI_TRANSFER_BLOCK);
        if (rval != ARM_DRIVER_OK) {
          /* Got error, can't do nothing about it */
          EvrFsMcMCI_ReadXferSetupError (mc->Instance, buf, cnt, 512U);
          state = MC_READ_ERROR;
          break;
        }
        err = mc_read_block (sect, cnt > 1, mc);

        if (err == 0U) {
          /* Wait here until transfer event arrives */
          events = mc_wfe (MC_DAT_EVENTS, mc);

          if ((events & MC_DAT_EVENTS) != ARM_MCI_EVENT_TRANSFER_COMPLETE) {
            /* Transfer incomplete */
            EvrFsMcMCI_TransferError (mc->Instance, events);
            err = 1U;
          }

          if ((err != 0U) || (cnt > 1U)) {
            /* Send STOP_TRANSMISSION command */
            err = mc_stop_transmission (mc);
          }

          if (err == 0U) {
            /* Disconnect memory card (TRAN -> STBY) */
            mc_select_deselect (0U, &r1, mc);
            return (true);
          }
        }
        break;

      case MC_READ_ERROR:
      default: /* Fatal system error */
        /* Bail out */
        err   = 1U;
        retry = MC_RETRY_OPERATION_COUNT;
        break;
    }

    if (err && (retry < MC_RETRY_OPERATION_COUNT)) {
      retry++;

      EvrFsMcMCI_TransferRetry (mc->Instance, retry);

      err   = 0U;
      state = MC_READ_INIT;
    }
  }
  /* Read failed: sector=%d, cnt=%d */
  EvrFsMcMCI_SectorReadError (mc->Instance, sect, cnt);
  return (false);
}


/**
  Write a cnt of 512 byte sectors to memory card

  \param[in]   sect     sector number
  \param[in]   buf      sector data buffer
  \param[in]   cnt      number of sectors to write
  \param[in]   mc       memory card instance object
  \return true if write succeeded, false otherwise
*/
uint32_t mc_mci_WriteSector (uint32_t sect, const uint8_t *buf, uint32_t cnt, MC_MCI *mc) {
  int32_t  rval;
  uint32_t err, r1, events;
  uint8_t  state, retry;

  EvrFsMcMCI_SectorWrite (mc->Instance, sect, cnt);

  err   = 0U;
  retry = 0U;
  state = MC_WRITE_INIT;

  if ((mc->MediaStatus & FS_MEDIA_INITIALIZED) == 0) {
    /* Media is not initialized */
    EvrFsMcMCI_MediaNotInitialized (mc->Instance);
    err = 1U;
  }
  if (mc->Status & MC_STATUS_LOCKED) {
    /* Password protection is active */
    EvrFsMcMCI_MediaPasswordEnabled (mc->Instance);
    err = 1U;
  }

  while (err == 0U) {
    switch (state) {
      case MC_WRITE_INIT: /* Switch device to TRAN state */
        err = mc_select_tran_state (mc);
        if (err == 0U) {
          state = MC_WRITE_DATA;
        }
        break;

      case MC_WRITE_DATA:
        /* Setup transfer and send write block command */
        rval = mc->Driver->SetupTransfer ((uint8_t *)(uint32_t)buf, cnt, 512U, ARM_MCI_TRANSFER_WRITE | ARM_MCI_TRANSFER_BLOCK);
        if (rval != ARM_DRIVER_OK) {
          /* Got error, can't do nothing about it */
          EvrFsMcMCI_WriteXferSetupError (mc->Instance, buf, cnt, 512U);
          state = MC_WRITE_ERROR;
          break;
        }
        err = mc_write_block (sect, cnt > 1, mc);

        if (err == 0U) {
          /* Wait here until transfer event arrives */
          events = mc_wfe (MC_DAT_EVENTS, mc);

          if ((events & MC_DAT_EVENTS) != ARM_MCI_EVENT_TRANSFER_COMPLETE) {
            /* Transfer incomplete */
            EvrFsMcMCI_TransferError (mc->Instance, events);
            err = 1U;
          }

          if ((err != 0) || (cnt > 1)) {
            /* Send STOP_TRANSMISSION command */
            err = mc_stop_transmission (mc);
          }

          if (err == 0U) {
            do {
              /* Wait for state transition (RCV -> PRG/TRAN) */
              err = mc_read_status (mc->RCA, &r1, mc);

              if (err != 0U) {
                break;
              }
            }
            while ((r1 & R1_STATE_Msk) == R1_STATE_RCV);

            if (err == 0U) {
              /* Disconnect memory card                               */
              /* States: (PRG -> DIS -> STBY) or (PRG -> TRAN -> STBY)*/
              mc_select_deselect (0, &r1, mc);
              return (true);
            }
          }
        }
        break;

      case MC_WRITE_ERROR:
      default: /* Fatal system error */
        /* Bail out */
        err   = 1;
        retry = MC_RETRY_OPERATION_COUNT;
        break;
    }

    if (err && (retry < MC_RETRY_OPERATION_COUNT)) {
      retry++;

      EvrFsMcMCI_TransferRetry (mc->Instance, retry);

      err   = 0;
      state = MC_WRITE_INIT;
    }
  }
  /* Write failed: sector=%d, cnt=%d */
  EvrFsMcMCI_SectorWriteError (mc->Instance, sect, cnt);
  return (false);
}


/**
  Read memory cards configuration

  \param[out]  info pointer to \ref FS_MEDIA_INFO object
  \param[in]   mc   memory card instance object
  \return true on success, false otherwise
*/
uint32_t mc_mci_ReadInfo (fsMediaInfo *info, MC_MCI *mc) {

  EvrFsMcMCI_ReadInfo (mc->Instance);

  if ((mc->MediaStatus & FS_MEDIA_INITIALIZED) == 0) {
    /* Media is not initialized */
    EvrFsMcMCI_MediaNotInitialized (mc->Instance);
    return (false);
  }

  /* READ_BL_LEN/WRITE_BL_LEN of 512 bytes is mandatory support by all     */
  /* devices. To make things simple, total number of sectors is determined */
  /* at device initialization.                                             */

  info->read_blen  = 512U;
  info->write_blen = 512U;
  info->block_cnt  = mc->SectorCount;

  EvrFsMcMCI_ReadInfoSuccess (mc->Instance, mc->SectorCount);

  return (true);
}


/**
  Process given device control command

  \param[in]    code Device control command code
  \param[inout] p    Pointer to command code argument
  \param[in]    mc   Pointer to memory card instance object
  \return fsStatus
*/
fsStatus mc_mci_DevCtrl (fsDevCtrlCode code, void *p, MC_MCI *mc) {
  fsStatus status;
  uint32_t type;
  uint32_t r[4];
  bool m_init;

  EvrFsMcMCI_DevCtrl (mc->Instance, code, (uint32_t)p);

  status = fsError;
  m_init = (mc->MediaStatus & FS_MEDIA_INITIALIZED) != 0;

  if (code == fsDevCtrlCodeCheckMedia) {
    /* Check for media presence */
    mci_update_sw_state (mc);

    if (p != NULL) {
      if (mc->Capabilities.cd_state == 0U) {
        if (m_init) {
          /* Media is initialized */
          mc->MediaStatus &= ~FS_MEDIA_NOCHKMEDIA;

          if (mc_read_status (mc->RCA, &r[0], mc) == 0) {
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
          if (mc_mci_Init (DM_MEDIA, mc) == true) {
            status = fsOK;
          }
          break;

        case FS_CONTROL_EMMC_SLEEP:
        case FS_CONTROL_EMMC_AWAKE:
          /* Switch between STBY and SLP state */
          if (mc->Property & MC_PROP_MMC_V4) {
            status = mc_control_sleep (*(uint32_t *)p, mc);
          }
          else { status = fsUnsupported; }
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
        if (mc_read_status (mc->RCA, &r[0], mc) == 0) {
          mc_select_deselect (0, &r[0], mc);
          mc_read_cid (mc->RCA, &r[0], mc);
          type = mc->Property & MC_PROP_TYPE_MMC;
          fill_cid ((fsCID_Register *)p, &r[0], type == MC_PROP_TYPE_MMC);
          mc_select_deselect (mc->RCA, &r[0], mc);

          status = fsOK;
        }
      } else {
        status = fsNoMedia;
      }
    }
  }
  else if (code == fsDevCtrlCodeLockUnlock) {
    /* Lock/Unlock the device */
    if (mc_select_tran_state (mc) == 0) {
      status = mc_control_lock ((fsLockUnlock *)p, mc);

      /* Set block length back to 512 bytes */
      if (mc_set_block_len (512, mc) != 0) {
        status = fsError;
      }

      /* Disconnect memory card (TRAN -> STBY */
      if (mc_select_deselect (0, &r[0], mc) != 0) {
        status = fsError;
      }
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
    EvrFsMcMCI_DevCtrlUnsupported (mc->Instance, code);
    status = fsUnsupported;
  }

  return (status);
}
