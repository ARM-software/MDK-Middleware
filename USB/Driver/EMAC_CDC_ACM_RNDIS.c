/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device:CDC
 * Copyright (c) 2018-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    EMAC_CDC_ACM_RNDIS.c
 * Purpose: CMSIS Ethernet driver using USB Device CDC ACM (RNDIS)
 * Rev.:    V1.2
 *----------------------------------------------------------------------------*/
/* -----------------------------------------------------------------------------
 * Driver:        Driver_ETH_MACn (default: Driver_ETH_MAC0),
 *                Driver_ETH_PHYn (default: Driver_ETH_PHY0)
 * Project:       Ethernet Media Access (MAC) Driver and
 *                Ethernet Physical Layer Transceiver (PHY) Driver
 *                for Virtual Ethernet over USB CDC ACM (RNDIS) Class
 * Requirements:  USBD_User_CDC_ACM_RNDIS_VETH_n.c template
 * Configuration: via following defines:
 *                - Ethernet driver index:
 *                  ETH_NUM = 0 .. 1 (default: 0)
 *                - USB Device CDC Instance:
 *                  USB_CDC_NUM = 0 .. 3 (default: 0)
 *                - Virtual Ethernet MAC address (should be different than one
 *                  used by RNDIS device):
 *                  VETH_MAC_ADDR = "xx-xx-xx-xx-xx-xx"
 *                        (default: "1E-30-6C-A2-45-5F")
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.2
 *    Corrected compiler warnings
 *  Version 1.1
 *    Corrected MAC_str_to_addr function
 *  Version 1.0
 *    Initial release
 */

#include <string.h>

#include "rl_usb.h"

#include "Driver_ETH.h"
#include "Driver_ETH_MAC.h"
#include "Driver_ETH_PHY.h"

#define ARM_ETH_MAC_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,2) // MAC driver version
#define ARM_ETH_PHY_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,2) // PHY driver version

// Configuration defines
// Default ethernet driver number
#ifndef ETH_NUM
#define ETH_NUM         0
#endif

// USB CDC ACM (RNDIS) instance number
#ifndef USB_CDC_NUM
#define USB_CDC_NUM     0
#endif

// Virtual Ethernet MAC Address used by Embedded Device
#ifndef VETH_MAC_ADDR
#define VETH_MAC_ADDR   "1E-30-6C-A2-45-5F"
#endif

// EMAC Driver state flags
#define DRV_INIT       (1U)             // Driver initialized
#define DRV_POWER      (1U << 1)        // Driver power on

// Driver Version
static const ARM_DRIVER_VERSION MAC_DriverVersion = {
  ARM_ETH_MAC_API_VERSION,
  ARM_ETH_MAC_DRV_VERSION
};

// Driver Capabilities
static const ARM_ETH_MAC_CAPABILITIES MAC_DriverCapabilities = {
  0U,                   /* checksum_offload_rx_ip4  */
  0U,                   /* checksum_offload_rx_ip6  */
  0U,                   /* checksum_offload_rx_udp  */
  0U,                   /* checksum_offload_rx_tcp  */
  0U,                   /* checksum_offload_rx_icmp */
  0U,                   /* checksum_offload_tx_ip4  */
  0U,                   /* checksum_offload_tx_ip6  */
  0U,                   /* checksum_offload_tx_udp  */
  0U,                   /* checksum_offload_tx_tcp  */
  0U,                   /* checksum_offload_tx_icmp */
  0U,                   /* media_interface          */
  1U,                   /* mac_address              */
  0U,                   /* event_rx_frame           */
  0U,                   /* event_tx_frame           */
  0U,                   /* event_wakeup             */
  0U,                   /* precision_timer          */
  0U                    /* reserved                 */
};

#define FUNC_NAME(prefix,x,postfix)     FUNC_NAME_(prefix,x,postfix)
#define FUNC_NAME_(prefix,x,postfix)    prefix##x##postfix

// External functions from USBD_User_CDC_ACM_RNDIS_VETH_n.c module
extern ARM_ETH_LINK_STATE FUNC_NAME(RNDIS,USB_CDC_NUM,_GetLinkState  )(void);
extern int32_t            FUNC_NAME(RNDIS,USB_CDC_NUM,_SendFrame     )(const uint8_t *frame, uint32_t len);
extern int32_t            FUNC_NAME(RNDIS,USB_CDC_NUM,_ReadFrame     )(      uint8_t *frame, uint32_t len);
extern uint32_t           FUNC_NAME(RNDIS,USB_CDC_NUM,_GetRxFrameSize)(void);

// Local functions
static void               MAC_str_to_addr      (const char *mac_str, uint8_t *mac_addr);

// Local variables
static uint8_t            drv_state = 0U;

static uint32_t           packet_filter;
static ARM_ETH_MAC_ADDR   mac_address;

static bool               rx_en;
static bool               tx_en;

static ARM_ETH_LINK_INFO  phy_info;


// Local functions

// MAC Address conversion from string
// \param[in]   mac_str   Pointer to wide string.
// \param[out]  mac_addr  Pointer to address.
static void MAC_str_to_addr (const char *mac_str, uint8_t *mac_addr) {
  uint8_t  c;
  uint8_t  n;
  uint32_t i, j;
  uint32_t str_len;

  str_len = strlen(mac_str);
  j = 0U;
  for (i = 0U; i < str_len; i++) {
    c = (uint8_t)mac_str[i];
    if         (c == '-') {
      continue;
    } else if ((c >= '0') && (c <= '9')) {
      n = c - '0';
    } else if ((c >= 'A') && (c <= 'F')) {
      n = c - ('A' + 10U);
    } else if ((c >= 'a') && (c <= 'f')) {
      n = c - ('a' + 10U);
    } else {
      n = 0U;
    }
    if ((j & 1U) != 0U) {
      mac_addr[j>>1] |= n;
    } else {
      mac_addr[j>>1]  = (uint8_t)((uint32_t)n << 4);
    }
    j++;
  }
}

// Ethernet Driver functions

/**
  \fn          ARM_DRIVER_VERSION MAC_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION MAC_GetVersion (void) {
  return MAC_DriverVersion;
}

/**
  \fn          ARM_ETH_MAC_CAPABILITIES MAC_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_ETH_MAC_CAPABILITIES
*/
static ARM_ETH_MAC_CAPABILITIES MAC_GetCapabilities (void) {
  return MAC_DriverCapabilities;
}

/**
  \fn          int32_t MAC_Initialize (ARM_ETH_MAC_SignalEvent_t cb_event)
  \brief       Initialize Ethernet MAC Device.
  \param[in]   cb_event  Pointer to \ref ARM_ETH_MAC_SignalEvent
  \return      \ref execution_status
*/
static int32_t MAC_Initialize (ARM_ETH_MAC_SignalEvent_t cb_event) {

  (void)cb_event;

  if ((drv_state & DRV_INIT) != 0U) { return ARM_DRIVER_OK; }

  packet_filter = 0U;
  rx_en         = false;
  tx_en         = false;

  // Load mac_address variable with MAC address from configured wide string
  MAC_str_to_addr(VETH_MAC_ADDR, (uint8_t *)&mac_address);

  drv_state     = DRV_INIT;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t MAC_Uninitialize (void)
  \brief       De-initialize Ethernet MAC Device.
  \return      \ref execution_status
*/
static int32_t MAC_Uninitialize (void) {

  drv_state = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t MAC_PowerControl (ARM_POWER_STATE state)
  \brief       Control Ethernet MAC Device Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t MAC_PowerControl (ARM_POWER_STATE state) {

  switch ((int32_t)state) {
    case ARM_POWER_OFF:
      packet_filter = 0U;
      rx_en         = false;
      tx_en         = false;

      drv_state &= ~DRV_POWER;
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_POWER_FULL:
      if ((drv_state & DRV_INIT) == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if ((drv_state & DRV_POWER) != 0U) {
        return ARM_DRIVER_OK;
      }

      drv_state |= DRV_POWER;
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t MAC_GetMacAddress (ARM_ETH_MAC_ADDR *ptr_addr)
  \brief       Get Ethernet MAC Address.
  \param[in]   ptr_addr  Pointer to address
  \return      \ref execution_status
*/
static int32_t MAC_GetMacAddress (ARM_ETH_MAC_ADDR *ptr_addr) {

  if (ptr_addr == NULL) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((drv_state & DRV_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  memcpy((void *)ptr_addr, (void *)&mac_address, sizeof(ARM_ETH_MAC_ADDR));

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t MAC_SetMacAddress (const ARM_ETH_MAC_ADDR *ptr_addr)
  \brief       Set Ethernet MAC Address.
  \param[in]   ptr_addr  Pointer to address
  \return      \ref execution_status
*/
static int32_t MAC_SetMacAddress (const ARM_ETH_MAC_ADDR *ptr_addr) {

  if (ptr_addr == NULL) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((drv_state & DRV_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  memcpy(&mac_address, ptr_addr, sizeof(ARM_ETH_MAC_ADDR));

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t MAC_SetAddressFilter (const ARM_ETH_MAC_ADDR *ptr_addr,
                                                   uint32_t          num_addr)
  \brief       Configure Address Filter.
  \param[in]   ptr_addr  Pointer to addresses
  \param[in]   num_addr  Number of addresses to configure
  \return      \ref execution_status
*/
static int32_t MAC_SetAddressFilter (const ARM_ETH_MAC_ADDR *ptr_addr, uint32_t num_addr) {

  if ((ptr_addr == NULL) && (num_addr != 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((drv_state & DRV_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  // Virtual Ethernet does not have filtering capability

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t MAC_SendFrame (const uint8_t *frame, uint32_t len, uint32_t flags)
  \brief       Send Ethernet frame.
  \param[in]   frame  Pointer to frame buffer with data to send
  \param[in]   len    Frame buffer length in bytes
  \param[in]   flags  Frame transmit flags (see ARM_ETH_MAC_TX_FRAME_...)
  \return      \ref execution_status
*/
static int32_t MAC_SendFrame (const uint8_t *frame, uint32_t len, uint32_t flags) {
  (void)flags;

  if ((frame == NULL) || (len == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((drv_state & DRV_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  if (!tx_en) {                         // If transmission not enabled
    return ARM_DRIVER_ERROR;
  }

  return FUNC_NAME(RNDIS,USB_CDC_NUM,_SendFrame)(frame, len);
}

/**
  \fn          int32_t MAC_ReadFrame (uint8_t *frame, uint32_t len)
  \brief       Read data of received Ethernet frame.
  \param[in]   frame  Pointer to frame buffer for data to read into
  \param[in]   len    Frame buffer length in bytes
  \return      number of data bytes read or execution status
                 - value >= 0: number of data bytes read
                 - value < 0: error occurred, value is execution status as defined with \ref execution_status
*/
static int32_t MAC_ReadFrame (uint8_t *frame, uint32_t len) {

  if ((frame == NULL) && (len != 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((drv_state & DRV_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  if (!rx_en) {                         // If reception not enabled
    return ARM_DRIVER_ERROR;
  }

  return FUNC_NAME(RNDIS,USB_CDC_NUM,_ReadFrame)(frame, len);
}

/**
  \fn          uint32_t MAC_GetRxFrameSize (void)
  \brief       Get size of received Ethernet frame.
  \return      number of bytes in received frame
*/
static uint32_t MAC_GetRxFrameSize (void) {

  if ((drv_state & DRV_POWER) == 0U) {
    return 0;
  }

  if (!rx_en) {                         // If reception not enabled
    return 0;
  }

  return FUNC_NAME(RNDIS,USB_CDC_NUM,_GetRxFrameSize)();
}

/**
  \fn          int32_t MAC_GetRxFrameTime (ARM_ETH_MAC_TIME *time)
  \brief       Get time of received Ethernet frame.
  \param[in]   time  Pointer to time structure for data to read into
  \return      \ref execution_status
*/
static int32_t MAC_GetRxFrameTime (ARM_ETH_MAC_TIME *time) {
  (void)time;
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
  \fn          int32_t MAC_GetTxFrameTime (ARM_ETH_MAC_TIME *time)
  \brief       Get time of transmitted Ethernet frame.
  \param[in]   time  Pointer to time structure for data to read into
  \return      \ref execution_status
*/
static int32_t MAC_GetTxFrameTime (ARM_ETH_MAC_TIME *time) {
  (void)time;
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
  \fn          int32_t MAC_ControlTimer (uint32_t control, ARM_ETH_MAC_TIME *time)
  \brief       Control Precision Timer.
  \param[in]   control  Operation
  \param[in]   time     Pointer to time structure
  \return      \ref execution_status
*/
static int32_t MAC_ControlTimer (uint32_t control, ARM_ETH_MAC_TIME *time) {
  (void)control;
  (void)time;
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
  \fn          int32_t MAC_Control (uint32_t control, uint32_t arg)
  \brief       Control Ethernet Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \return      \ref execution_status
*/
static int32_t MAC_Control (uint32_t control, uint32_t arg) {

  if ((drv_state & DRV_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  switch (control) {
    case ARM_ETH_MAC_CONFIGURE:
      // Configure duplex mode
      switch (arg & ARM_ETH_MAC_DUPLEX_Msk) {
        case ARM_ETH_MAC_DUPLEX_HALF:
        case ARM_ETH_MAC_DUPLEX_FULL:
          // Not used in this driver
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }

      // Configure speed
      switch (arg & ARM_ETH_MAC_SPEED_Msk) {
        case ARM_ETH_MAC_SPEED_10M:
        case ARM_ETH_SPEED_100M:
          // Not used in this driver
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }

      // Configure loopback mode
      if (arg & ARM_ETH_MAC_LOOPBACK) {
        // Not supported
        return ARM_DRIVER_ERROR_UNSUPPORTED;
      }

      if ((arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_RX) ||
          (arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX)) {
        // Checksum offload is not supported by this driver
        return ARM_DRIVER_ERROR_UNSUPPORTED;
      }

      // Configure filter
      if ((arg & ARM_ETH_MAC_ADDRESS_MULTICAST) != 0U) { packet_filter |= ARM_ETH_MAC_ADDRESS_MULTICAST; }
      if ((arg & ARM_ETH_MAC_ADDRESS_BROADCAST) != 0U) { packet_filter |= ARM_ETH_MAC_ADDRESS_BROADCAST; }
      if ((arg & ARM_ETH_MAC_ADDRESS_ALL)       != 0U) { packet_filter |= ARM_ETH_MAC_ADDRESS_ALL;       }
      break;

    case ARM_ETH_MAC_CONTROL_TX:
      // Enable/disable MAC transmitter
      if (arg != 0U) {
        tx_en = true;
      } else {
        tx_en = false;
      }
      break;

    case ARM_ETH_MAC_CONTROL_RX:
      // Enable/disable MAC receiver
      if (arg != 0U) {
        rx_en = true;
      } else {
        rx_en = false;
      }
      break;

    case ARM_ETH_MAC_FLUSH:
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t PHY_Read (uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
  \brief       Read Ethernet PHY Register through Management Interface.
  \param[in]   phy_addr  5-bit device address
  \param[in]   reg_addr  5-bit register address
  \param[out]  data      Pointer where the result is written to
  \return      \ref execution_status
*/
static int32_t PHY_Read (uint8_t phy_addr, uint8_t reg_addr, uint16_t *data) {
  (void)phy_addr;
  (void)reg_addr;
  (void)data;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t PHY_Write (uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
  \brief       Write Ethernet PHY Register through Management Interface.
  \param[in]   phy_addr  5-bit device address
  \param[in]   reg_addr  5-bit register address
  \param[in]   data      16-bit data to write
  \return      \ref execution_status
*/
static int32_t PHY_Write (uint8_t phy_addr, uint8_t reg_addr, uint16_t data) {
  (void)phy_addr;
  (void)reg_addr;
  (void)data;

  return ARM_DRIVER_OK;
}


// MAC Driver Control Block
extern
ARM_DRIVER_ETH_MAC ARM_Driver_ETH_MAC_(ETH_NUM);
ARM_DRIVER_ETH_MAC ARM_Driver_ETH_MAC_(ETH_NUM) = {
  MAC_GetVersion,
  MAC_GetCapabilities,
  MAC_Initialize,
  MAC_Uninitialize,
  MAC_PowerControl,
  MAC_GetMacAddress,
  MAC_SetMacAddress,
  MAC_SetAddressFilter,
  MAC_SendFrame,
  MAC_ReadFrame,
  MAC_GetRxFrameSize,
  MAC_GetRxFrameTime,
  MAC_GetTxFrameTime,
  MAC_ControlTimer,
  MAC_Control,
  PHY_Read,
  PHY_Write
};


// Driver Version
static const ARM_DRIVER_VERSION PHY_DriverVersion = {
  ARM_ETH_PHY_API_VERSION,
  ARM_ETH_PHY_DRV_VERSION
};

/**
  \fn          ARM_DRV_VERSION PHY_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRIVER_VERSION PHY_GetVersion (void) {
  return PHY_DriverVersion;
}

/**
  \fn          int32_t PHY_Initialize (ARM_ETH_PHY_Read_t  fn_read,
                                       ARM_ETH_PHY_Write_t fn_write)
  \brief       Initialize Ethernet PHY Device.
  \param[in]   fn_read   Pointer to \ref ARM_ETH_MAC_PHY_Read
  \param[in]   fn_write  Pointer to \ref ARM_ETH_MAC_PHY_Write
  \return      \ref execution_status
*/
static int32_t PHY_Initialize (ARM_ETH_PHY_Read_t fn_read, ARM_ETH_PHY_Write_t fn_write) {
  // We have fn_read and fn_write already available
  (void)fn_read;
  (void)fn_write;

  phy_info.duplex = 0U;
  phy_info.speed  = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t PHY_Uninitialize (void)
  \brief       De-initialize Ethernet PHY Device.
  \return      \ref execution_status
*/
static int32_t PHY_Uninitialize (void) {
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t PHY_PowerControl (ARM_POWER_STATE state)
  \brief       Control Ethernet PHY Device Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t PHY_PowerControl (ARM_POWER_STATE state) {

  if ((drv_state & DRV_POWER) == 0U) { return ARM_DRIVER_ERROR; }

  switch ((int32_t)state) {
    case ARM_POWER_OFF:         // Select Power Saving Mode
      phy_info.duplex = 0U;
      phy_info.speed  = 0U;
      break;
    case ARM_POWER_FULL:        // Select Normal Operation Mode
      phy_info.duplex = ARM_ETH_DUPLEX_FULL;
      phy_info.speed  = ARM_ETH_SPEED_100M;
      break;
    case ARM_POWER_LOW:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t PHY_SetInterface (uint32_t interface)
  \brief       Set Ethernet Media Interface.
  \param[in]   interface  Media Interface type
  \return      \ref execution_status
*/
static int32_t PHY_SetInterface (uint32_t interface) {
  // Not used in this driver
  (void)interface;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t PHY_SetMode (uint32_t mode)
  \brief       Set Ethernet PHY Device Operation mode.
  \param[in]   mode  Operation Mode
  \return      \ref execution_status
*/
static int32_t PHY_SetMode (uint32_t mode) {

  if ((drv_state & DRV_POWER) == 0U) { return ARM_DRIVER_ERROR; }

  // Configure duplex mode
  switch (mode & ARM_ETH_PHY_DUPLEX_Msk) {
    case ARM_ETH_PHY_DUPLEX_FULL:
      phy_info.duplex = ARM_ETH_DUPLEX_FULL;
      break;
    case ARM_ETH_PHY_DUPLEX_HALF:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  // Configure speed
  switch (mode & ARM_ETH_PHY_SPEED_Msk) {
    case ARM_ETH_PHY_SPEED_10M:
      phy_info.speed  = ARM_ETH_SPEED_10M;
      break;
    case ARM_ETH_PHY_SPEED_100M:
      phy_info.speed  = ARM_ETH_SPEED_100M;
      break;
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  if (mode & ARM_ETH_PHY_AUTO_NEGOTIATE) {
    // Not supported by this driver
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  if (mode & ARM_ETH_PHY_LOOPBACK) {
    // Not supported by this driver
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  if (mode & ARM_ETH_PHY_ISOLATE) {
    // Not supported by this driver
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_ETH_LINK_STATE PHY_GetLinkState (void)
  \brief       Get Ethernet PHY Device Link state.
  \return      current link status \ref ARM_ETH_LINK_STATE
*/
static ARM_ETH_LINK_STATE PHY_GetLinkState (void) {

  if ((drv_state & DRV_POWER) == 0U) {
    return ARM_ETH_LINK_DOWN;
  }

  return FUNC_NAME(RNDIS,USB_CDC_NUM,_GetLinkState)();
}

/**
  \fn          ARM_ETH_LINK_INFO PHY_GetLinkInfo (void)
  \brief       Get Ethernet PHY Device Link information.
  \return      current link parameters \ref ARM_ETH_LINK_INFO
*/
static ARM_ETH_LINK_INFO PHY_GetLinkInfo (void) {
  return phy_info;
}

// PHY Driver Control Block
extern
ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH_NUM);
ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH_NUM) = {
  PHY_GetVersion,
  PHY_Initialize,
  PHY_Uninitialize,
  PHY_PowerControl,
  PHY_SetInterface,
  PHY_SetMode,
  PHY_GetLinkState,
  PHY_GetLinkInfo
};
