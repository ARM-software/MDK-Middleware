/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_serial.c
 * Purpose: Serial Driver Interface
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_common.h"
#include "net_sys.h"
#include "net_serial.h"
#include "Driver_USART.h"

/* Global variables */
NET_COM_CTRL net_ppp0_com_control;
NET_COM_CTRL net_slip0_com_control;

/* Code shortening macros */
#define ctrl        (com->Ctrl)
#define drv_usart   ((ARM_DRIVER_USART *)com->DrvUsart)


/**
  \brief       Initialize serial driver interface.
  \param[in]   com  serial device handle.
  \return      none.
*/
void net_com_init (NET_COM_CFG *com) {
  uint32_t flow_ctrl;

  ctrl->com  = com;
  ctrl->Ridx = 0;
  ctrl->Tidx = 0;
  ctrl->Rlen = 0;

  drv_usart->Initialize (com->cb_event);
  drv_usart->PowerControl (ARM_POWER_FULL);

  switch (com->FlowCtrl) {
    default:
      flow_ctrl = ARM_USART_FLOW_CONTROL_NONE;
      break;
    case 1:
      flow_ctrl = ARM_USART_FLOW_CONTROL_RTS_CTS;
      break;
  }
  /* USART mode: 'Speed',N,8,1,'flow_ctrl' */
  drv_usart->Control (ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8       |
                      ARM_USART_PARITY_NONE       |
                      ARM_USART_STOP_BITS_1       |
                      flow_ctrl, com->Speed);
  /* USART enable Tx, Rx */
  drv_usart->Control (ARM_USART_CONTROL_TX, 1);
  drv_usart->Control (ARM_USART_CONTROL_RX, 1);
  drv_usart->Receive (ctrl->rbuf, sizeof(ctrl->rbuf));
}

/**
  \brief       De-initialize serial driver interface.
  \param[in]   com  serial device handle.
  \return      none.
*/
void net_com_uninit (NET_COM_CFG *com) {
  /* De-initialize USART */
  drv_usart->PowerControl (ARM_POWER_OFF);
  drv_usart->Uninitialize ();
  memset (ctrl, 0, sizeof (*ctrl));
}

/**
  \brief       Send notification on serial driver event.
  \param[in]   event  driver event mask.
  \param[in]   ctx    serial device context.
  \return      none.
  \note        This function is called from serial interrupt.
*/
void net_com_callback (uint32_t event, NET_COM_CTRL *ctx) {
  NET_COM_CFG *com = ctx->com;

  if (event & ARM_USART_EVENT_SEND_COMPLETE) {
    ctrl->Tidx = 0;
  }
  if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) {
    /* Implement circular receive buffer */
    ctrl->Rlen += sizeof(ctrl->rbuf);
    drv_usart->Receive (ctrl->rbuf, sizeof(ctrl->rbuf));
  }
}

/**
  \brief       Send character to serial driver.
  \param[in]   com  serial device handle.
  \param[in]   ch   character to send.
  \return      status:
               - true  = character accepted,
               - false = buffer full.
*/
bool net_com_send_char (NET_COM_CFG *com, int32_t ch) {
  if (ctrl->Tidx < sizeof(ctrl->tbuf)) {
    ctrl->tbuf[ctrl->Tidx++] = ch & 0xFF;
    if (ctrl->Tidx == sizeof(ctrl->tbuf)) {
      /* Buffer is full, sent it to uart now */
      drv_usart->Send (ctrl->tbuf, ctrl->Tidx);
    }
    return (true);
  }
  return (false);
}

/**
  \brief       Flush serial output buffer.
  \param[in]   com  serial device handle.
  \return      none.
*/
void net_com_flush_buf (NET_COM_CFG *com) {
  if (ctrl->Tidx && ctrl->Tidx < sizeof(ctrl->tbuf)) {
    uint32_t n = ctrl->Tidx;
    ctrl->Tidx  = sizeof(ctrl->tbuf);
    drv_usart->Send (ctrl->tbuf, n);
  }
}

/**
  \brief       Check if transmission in progress.
  \param[in]   com  serial device handle.
  \return      Tx status:
               - true  = in progress,
               - false = completed.
*/
bool net_com_tx_active (NET_COM_CFG *com) {
  if (ctrl->Tidx == 0) {
    return (false);
  }
  if (ctrl->Tidx < sizeof(ctrl->tbuf)) {
    /* Data buffered for transmit */
    net_com_flush_buf (com);
  }
  return (true);
}

/**
  \brief       Receive character from input buffer.
  \param[in]   com  serial device handle.
  \return      character received.
*/
int32_t net_com_recv_char (NET_COM_CFG *com) {
  uint32_t n;

  n = ctrl->Rlen + drv_usart->GetRxCount ();
  if (n != ctrl->Ridx) {
    return (ctrl->rbuf[ctrl->Ridx++ & (sizeof(ctrl->rbuf)-1)]);
  }
  /* No data available */
  return (-1);
}
