/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FTP_Server_Event.c
 * Purpose: FTP Server Notification events
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_FTP_Server_Event]
#include "rl_net.h"
 
// \brief Notify the user application about events in FTP server service.
// \param[in]     event         FTP Server notification event.
// \return        none.
void netFTPs_Notify (netFTPs_Event event) {

  switch (event) {
    case netFTPs_EventLogin:
      // User logged in, session is busy
      break;

    case netFTPs_EventLogout:
      // User logged out, session is idle
      break;

    case netFTPs_EventLoginFailed:
      // User login failed (invalid credentials)
      break;

    case netFTPs_EventDownload:
      // File download ended
      break;

    case netFTPs_EventUpload:
      // File upload ended
      break;

    case netFTPs_EventDelete:
      // File deleted
      break;

    case netFTPs_EventRename:
      // File or directory renamed
      break;

    case netFTPs_EventMakeDirectory:
      // Directory created
      break;

    case netFTPs_EventRemoveDirectory:
      // Directory removed
      break;

    case netFTPs_EventOperationDenied:
      // Requested file operation denied
      break;

    case netFTPs_EventLocalFileError:
      // Local file operation error
      break;

    case netFTPs_EventFileError:
      // Generic file operation error

    case netFTPs_EventError:
      // Generic FTP server error
      break;
  }
}
//! [code_FTP_Server_Event]
