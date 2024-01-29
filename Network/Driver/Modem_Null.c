/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Modem_Null.c
 * Purpose: Modem Driver for Direct Cable connection (NULL modem)
 * Rev.:    V6.2.0
 *----------------------------------------------------------------------------*/

/* History:
 *  Version 6.2.0
 *    Updated for ARM compiler 6
 *  Version 6.1.0
 *    Added access to modem response string
 *  Version 6.0
 *    Initial release
 */

#include "net_modem.h"

#ifdef __clang__
  #pragma clang diagnostic ignored "-Wpadded"
#endif

/* Modem driver states */
enum Modem_State {
  Modem_Idle,
  Modem_Error,
  Modem_Ready,
  Modem_Listen,
  Modem_Dial,
  Modem_Online,
  Modem_Hangup
};

/* Modem driver local variables */
static struct {
  Modem_Request_t   callback;
  enum Modem_State  state;
  bool              dial_mode;
} drv;


/**
  \fn          MODEM_STATUS Modem_Initialize (Modem_Request_t cb_request, const char *init_string))
  \brief       Initialize Modem Device.
  \param[in]   cb_request   Pointer to \ref Modem_Request
  \param[in]   init_string  Pointer to modem initialization string
  \return      execution status \ref MODEM_STATUS
*/
static MODEM_STATUS Initialize (Modem_Request_t cb_request, const char *init_string) {
  (void)init_string;

  if (cb_request == NULL) {
    return MODEM_ERROR;
  }
  drv.callback = cb_request;
  drv.state    = Modem_Idle;
  return MODEM_OK;
}


/**
  \fn          MODEM_STATUS Modem_Uninitialize (void)
  \brief       De-initialize Modem Device.
  \return      execution status \ref MODEM_STATUS
*/
static MODEM_STATUS Uninitialize (void) {

  drv.state = Modem_Idle;
  return MODEM_OK;
}


/**
  \fn          MODEM_STATUS Modem_Listen (void)
  \brief       Start Modem Device listening mode.
  \return      execution status \ref MODEM_STATUS
*/
static MODEM_STATUS Listen (void) {

  if (drv.state > Modem_Ready) {
    return MODEM_ERROR;
  }
  drv.dial_mode = false;
  drv.state     = Modem_Listen;

  drv.callback (NULL, "CLIENT", 0, 0);
  return MODEM_OK;
}


/**
  \fn          MODEM_STATUS Modem_Dial (const char *dial_num)
  \brief       Start dialing target number.
  \param[in]   dial_num     Target number, a null-terminated string
  \return      execution status \ref MODEM_STATUS
*/
static MODEM_STATUS Dial (const char *dial_num) {
  (void)dial_num;

  if (drv.state > Modem_Ready) {
    return MODEM_ERROR;
  }
  drv.dial_mode = true;
  drv.state     = Modem_Dial;

  drv.callback ("CLIENT\r", "CLIENTSERVER", 3000, 2);
  return MODEM_OK;
}


/**
  \fn          MODEM_STATUS Modem_Hangup (void)
  \brief       Hangup the line.
  \return      execution status \ref MODEM_STATUS
*/
static MODEM_STATUS Hangup (void) {

  if (drv.state <= Modem_Ready) {
    return MODEM_OK;
  }
  drv.state = Modem_Hangup;

  drv.callback (NULL, NULL, 500, 0);
  return MODEM_OK;
}


/**
  \fn          bool Modem_IsOnline (void)
  \brief       Check if modem is online.
  \return      - \b true Modem is Online
               - \b false Modem is Offline
*/
static bool IsOnline (void) {
  if (drv.state == Modem_Online) {
    return true;
  }
  return false;
}


/**
  \fn          void Modem_Notify (MODEM_EVENT event, const char *response)
  \brief       Notify the Modem Driver of completion event.
  \param[in]   event        notification event \ref MODEM_EVENT
  \param[in]   response     response string received from the modem
  \return      None.
*/
static void Notify (MODEM_EVENT event, const char *response) {
  (void)response;

  switch (drv.state) {
    case Modem_Dial:
      if (event != MODEM_EVENT_OK) {
        /* Modem reset failed */
        drv.state = Modem_Error;
        break;
      }
      drv.state = Modem_Online;
      break;

    case Modem_Listen:
      if (event != MODEM_EVENT_OK) {
        /* Modem reset failed */
        drv.state = Modem_Error;
        break;
      }
      drv.callback ("CLIENTSERVER\r", "", 0, 0);
      drv.state = Modem_Online;
      break;

    case Modem_Hangup:
      if (drv.dial_mode) {
        drv.state = Modem_Ready;
        break;
      }
      /* Wait for 'CLIENT' request again */
      drv.state = Modem_Listen;
      drv.callback (NULL, "CLIENT", 0, 0);
      break;

    case Modem_Idle:
    case Modem_Error:
    case Modem_Ready:
    case Modem_Online:
      break;
  }
}

/* Modem Driver Control Block */
extern
DRIVER_MODEM Driver_MODEM;
DRIVER_MODEM Driver_MODEM = {
  Initialize,
  Uninitialize,
  Listen,
  Dial,
  Hangup,
  IsOnline,
  Notify
};
