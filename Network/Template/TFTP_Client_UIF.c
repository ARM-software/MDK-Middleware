/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    TFTP_Client_UIF.c
 * Purpose: TFTP Client User Interface
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_TFTP_Client_UIF]
#include "rl_net.h"
 
// \brief Notify the user application when TFTP client operation ends.
// \param[in]     event         TFTP client notification event.
// \return        none.
void netTFTPc_Notify (netTFTPc_Event event) {
  switch (event) {
    case netTFTPc_EventSuccess:
      // File operation successful
      break;

    case netTFTPc_EventTimeout:
      // Timeout on file operation
      break;

    case netTFTPc_EventAccessDenied:
      // File access not allowed
      break;

    case netTFTPc_EventFileNotFound:
      // File not found
      break;

    case netTFTPc_EventDiskFull:
      // Disk full
      break;

    case netTFTPc_EventLocalFileError:
      // Local file write error
      break;

    case netTFTPc_EventError:
      // Generic TFTP client error
      break;
  }
}
//! [code_TFTP_Client_UIF]
