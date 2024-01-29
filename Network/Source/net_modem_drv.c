/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_modem_drv.c
 * Purpose: Modem Driver Interface
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_common.h"
#include "net_sys.h"
#include "net_serial.h"
#include "net_modem.h"
#include "net_modem_drv.h"

/* Global variables */
NET_MODEM_CTRL net_ppp0_modem_control;
NET_MODEM_CTRL net_slip0_modem_control;

/* Code shortening macros */
#define ctrl        (com->ModemCtrl)
#define drv_modem   ((DRIVER_MODEM *)com->DrvModem)

/* Local Functions */
static void modem_send_cmd (NET_COM_CFG *com, const char *cmd);

/**
  \brief       Initialize modem device interface.
  \param[in]   com  serial device handle.
  \return      none.
*/
void net_modem_init (NET_COM_CFG *com) {
  const char *init_str = NULL;

  ctrl->com   = com;
  ctrl->Cmd   = NULL;
  ctrl->Resp  = NULL;
  ctrl->Delay = 0;
  ctrl->WrPos = 0;
  if (com->InitString[0] != 0) {
    init_str = com->InitString;
  }
  /* Provide NULL instead of "" to the driver */
  drv_modem->Initialize (com->cb_request, init_str);
}

/**
  \brief       De-initialize modem device interface.
  \param[in]   com  serial device handle.
  \return      none.
*/
void net_modem_uninit (NET_COM_CFG *com) {
  drv_modem->Uninitialize ();
  memset (ctrl, 0, sizeof(*ctrl));
}

/**
  \brief       Dial the target number.
  \param[in]   com       serial device handle.
  \param[in]   dial_num  Target number to dial.
  \return      status from modem driver:
               - true  = dialing started successfully,
               - false = failed to start dialer.
*/
bool net_modem_dial (NET_COM_CFG *com, const char *dial_num) {
  if (drv_modem->Dial (dial_num) == MODEM_OK) {
    return (true);
  }
  return (false);
}

/**
  \brief       Start modem answering mode.
  \param[in]   com  serial device handle.
  \return      status from modem driver:
               - true  = answering mode started successfully,
               - false = failed to start answering mode.
*/
bool net_modem_listen (NET_COM_CFG *com) {
  if (drv_modem->Listen () == MODEM_OK) {
    return (true);
  }
  return (false);
}

/**
  \brief       Hangup the line and go to command mode.
  \param[in]   com  serial device handle.
  \return      status from modem driver:
               - true  = hangup mode started successfully,
               - false = failed to start hangup mode.
*/
bool net_modem_hangup (NET_COM_CFG *com) {
  if (drv_modem->Hangup () == MODEM_OK) {
    return (true);
  }
  return (true);
}

/**
  \brief       Check if modem is online.
  \param[in]   com  serial device handle.
  \return      modem status:
               - true  = online in transparent mode,
               - false = offline in command mode.
*/
bool net_modem_online (NET_COM_CFG *com) {
  return (drv_modem->IsOnline ());
}

/**
  \brief       Send command string to modem.
  \param[in]   com  serial device handle.
  \param[in]   cmd  command string.
  \return      none.
*/
static void modem_send_cmd (NET_COM_CFG *com, const char *cmd) {
  while (*cmd) {
    while (!net_com_send_char (com, *cmd)) {
      netos_delay (1);
    }
    cmd++;
  }
}

/**
  \brief       Request callback from the modem driver.
  \param[in]   cmd       command to send to a modem driver.
  \param[in]   response  expected response to wait for.
  \param[in]   timeout   response timeout.
  \param[in]   retries   number of retries on failure.
  \param[in]   ctx       modem device context.
  \return      none.
*/
void net_modem_callback (const char *cmd, const char *response,
                         uint32_t timeout, uint32_t retries, NET_MODEM_CTRL *ctx) {
  NET_COM_CFG *com = ctx->com;

  ctrl->Cmd   = cmd;
  ctrl->Resp  = response;
  ctrl->WrPos = 0;
  if (timeout) {
    /* Convert timeout from ms to ticks */
    timeout = (timeout / SYS_TICK_INTERVAL) + 1;
    if (timeout > 0xFFFF) timeout = 0xFFFF;
  }
  ctrl->RespTout = timeout & 0xFFFF;
  ctrl->Delay    = timeout & 0xFFFF;
  if (retries > 0xFF) retries = 0xFF;
  ctrl->Retries  = retries & 0xFF;
  if (ctrl->Cmd != NULL) {
    modem_send_cmd (com, ctrl->Cmd);
    if (ctrl->Resp != NULL) {
      /* Response flushes transmit buffer */
      if (ctrl->Resp[0] == 0) {
        ctrl->Resp = NULL;
      }
      net_com_flush_buf (com);
    }
  }
}

/**
  \brief       Process character in modem command mode.
  \param[in]   com  serial device handle.
  \param[in]   ch   received character to process.
  \return      modem status:
               - true  = online in transparent mode,
               - false = offline in command mode.
*/
bool net_modem_process (NET_COM_CFG *com, int32_t ch) {
  uint32_t pos;

  if (ctrl->Resp == NULL) {
    /* Response trigger not defined */
    return (false);
  }

  pos = ctrl->WrPos;
  if ((pos == 0) && (ch < ' ')) {
    /* Ignore leading CR/LF's */
    return (false);
  }
  if (pos >= (sizeof(ctrl->Buf)-1)) {
    /* No space in response buffer */
    goto ex;
  }
  /* Store to buffer, add null-termination */
  ctrl->Buf[pos]   = (char)ch;
  ctrl->Buf[pos+1] = 0;
  ctrl->WrPos = (pos + 1) & 0xFF;

  /* Check modem response string */
  if ((ctrl->Resp[0] == '#') && (ctrl->Resp[1] == 0)) {
    /* String "#" redirects response to the user */
    drv_modem->Notify (MODEM_EVENT_RESPONSE, ctrl->Buf);
  }
  else if (net_strcmp (ctrl->Buf, ctrl->Resp)) {
    /* Expected response is received */
    ctrl->Resp  = NULL;
    ctrl->Delay = 0;
    ctrl->WrPos = 0;
    /* This will generate request callbacks */
    drv_modem->Notify (MODEM_EVENT_OK, ctrl->Buf);
  }
  else {
ex: if (ch < ' ') {
      /* Flush response buffer on CR/LF */
      ctrl->WrPos = 0;
    }
    return (false);
  }

  /* Check modem online status */
  if (!drv_modem->IsOnline ()) {
    goto ex;
  }
  /* Online transparent mode active */
  ctrl->Cmd   = NULL;
  ctrl->Resp  = NULL;
  ctrl->Delay = 0;
  ctrl->WrPos = 0;
  return (true);
}

/**
  \brief       Run modem device main process.
  \param[in]   com  serial device handle.
  \return      none.
*/
void net_modem_run (NET_COM_CFG *com) {
  if (ctrl->Delay == 0) {
    return;
  }
  /* A timeout started from driver callback */
  if (--ctrl->Delay > 0) {
    return;
  }
  /* Timeout has expired */
  ctrl->WrPos = 0;
  if (ctrl->Retries) {
    ctrl->Retries--;
    /* Restart a delay */
    ctrl->Delay = ctrl->RespTout;
    if (ctrl->Cmd != NULL) {
      /* Re-send a modem command */
      modem_send_cmd (com, ctrl->Cmd);
      net_com_flush_buf (com);
    }
    return;
  }
  /* No retries left, generate timeout event */
  ctrl->Resp = NULL;
  /* This might generate request callbacks */
  drv_modem->Notify (MODEM_EVENT_TIMEOUT, NULL);
}
