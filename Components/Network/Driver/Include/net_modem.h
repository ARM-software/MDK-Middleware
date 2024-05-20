/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_modem.h
 * Purpose: Network Modem Driver API
 *----------------------------------------------------------------------------*/

#ifndef __NET_MODEM_H
#define __NET_MODEM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
\brief Modem Driver Status of executed operation.
*/
typedef enum _MODEM_STATUS {
  MODEM_OK        = 0,                  ///< Operation succeeded
  MODEM_ERROR     = 1                   ///< Unspecified error
} MODEM_STATUS;

/**
\brief Modem Driver Callback Events.
*/
typedef enum _MODEM_EVENT {
  MODEM_EVENT_OK,                       ///< Operation succeeded
  MODEM_EVENT_TIMEOUT,                  ///< Operation timeout
  MODEM_EVENT_RESPONSE,                 ///< Modem response returned
  MODEM_EVENT_ERROR                     ///< Unspecified error
} MODEM_EVENT;

// Function documentation
/**
  \fn          MODEM_STATUS Modem_Initialize (Modem_Request_t cb_request, const char *init_string)
  \brief       Initialize Modem Device.
  \param[in]   cb_request   Pointer to \ref Modem_Request.
  \param[in]   init_string  Pointer to modem initialization string.
  \return      execution status \ref MODEM_STATUS.
*/
/**
  \fn          MODEM_STATUS Modem_Uninitialize (void)
  \brief       De-initialize Modem Device.
  \return      execution status \ref MODEM_STATUS.
*/
/**
  \fn          MODEM_STATUS Modem_Listen (void)
  \brief       Start Modem Device listening mode.
  \return      execution status \ref MODEM_STATUS.
*/
/**
  \fn          MODEM_STATUS Modem_Dial (const char *dial_num)
  \brief       Start dialing target number.
  \param[in]   dial_num     Target number, a null-terminated string.
  \return      execution status \ref MODEM_STATUS.
*/
/**
  \fn          MODEM_STATUS Modem_Hangup (void)
  \brief       Hangup the line.
  \return      execution status \ref MODEM_STATUS.
*/
/**
  \fn          bool Modem_IsOnline (void)
  \brief       Check if modem is online.
  \return
               - \b true Modem is Online.
               - \b false Modem is Offline.
*/
/**
  \fn          void Modem_Notify (MODEM_EVENT event, const char *response)
  \brief       Notify the Modem Driver of completion event.
  \param[in]   event        Notification event \ref MODEM_EVENT.
  \param[in]   response     Actual modem response, a null-terminated string.
*/

/**
  \fn          void Modem_Request (const char *command, const char *response, uint32_t timeout, uint32_t retries)
  \brief       Request Callback function for the driver.
  \param[in]   command      Command to send to modem.
  \param[in]   response     Expected response from modem, or "#" for any response.
  \param[in]   timeout      Response timeout in milliseconds.
  \param[in]   retries      Number of retries.
  \note        Any response "#" disables internal processing and delivers modem responses in \ref Modem_Notify.
*/

typedef void (*Modem_Request_t) (const char *command, const char *response, uint32_t timeout, uint32_t retries);

/**
\brief  Access structure of the Modem Driver.
*/
typedef struct _DRIVER_MODEM {
  MODEM_STATUS (*Initialize)    (Modem_Request_t cb_request,
                                 const char *init_string);       ///< Pointer to \ref Modem_Initialize : Initialize Modem Device.
  MODEM_STATUS (*Uninitialize)  (void);                          ///< Pointer to \ref Modem_Uninitialize : De-initialize Modem Device.
  MODEM_STATUS (*Listen)        (void);                          ///< Pointer to \ref Modem_Listen : Start Modem Device listening mode.
  MODEM_STATUS (*Dial)          (const char *dial_num);          ///< Pointer to \ref Modem_Dial : Start dialing target number.
  MODEM_STATUS (*Hangup)        (void);                          ///< Pointer to \ref Modem_Hangup : Hangup the line.
  bool         (*IsOnline)      (void);                          ///< Pointer to \ref Modem_IsOnline : Check if modem is online.
  void         (*Notify)        (MODEM_EVENT event,
                                 const char *response);          ///< Pointer to \ref Modem_Notify : Notify the driver of completion event.
} const DRIVER_MODEM;

#endif /* __NET_MODEM_H */
