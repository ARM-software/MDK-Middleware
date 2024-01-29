/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FTP_Client_UIF.c
 * Purpose: FTP Client User Interface
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_FTP_Client_UIF]
#include <stdio.h>
#include "rl_net.h"
 
// \brief Request parameters for FTP client session.
// \param[in]     request       request code.
// \param[out]    buf           output buffer to write the data to.
// \param[in]     buf_len       length of the output buffer in bytes.
// \return        number of bytes written to output buffer.
uint32_t netFTPc_Process (netFTPc_Request request, char *buf, uint32_t buf_len) {
  uint32_t len = 0;
//int32_t i;
 
  switch (request) {
    case netFTPc_RequestUsername:
      // Username to login to FTP server
      /* Example
      len = sprintf (buf, "anonymous");
      */
      break;
 
    case netFTPc_RequestPassword:
      // Password to login to FTP server
      /* Example
      len = sprintf (buf, "test@keil.com");
      */
      break;
 
    case netFTPc_RequestDirectory:
      // Working directory path on server
      /* Example
      len = sprintf (buf, "/Logs");
      */
      break;
 
    case netFTPc_RequestName:
      // Filename for PUT, GET, APPEND, DELETE and RENAME commands
      // Directory name for MKDIR and RMDIR commands
      /* Example
      len = sprintf (buf, "test.log");
      */
      break;
 
    case netFTPc_RequestNewName:
      // New name for a RENAME command
      /* Example
      len = sprintf (buf, "renamed.log");
      */
      break;
 
    case netFTPc_RequestListMask:
      // File filter/mask for LIST command (wildcards allowed)
      /* Example
      len = sprintf (buf, "");
      */
      break;
 
    case netFTPc_RequestList:
      // Received data if LIST command is given
      /* Example
      for (i = 0; i < len; i++) {
        putchar (buf[i]);
      }
      */
      break;
 
    case netFTPc_RequestLocalFilename:
      // Local filename
      /* Example
      len = sprintf (buf, "test_log.txt");
      */
      break;
  }
  return (len);
}
 
// \brief Notify the user application when FTP client operation ends.
// \param[in]     event         FTP client notification event.
// \return        none.
void netFTPc_Notify (netFTPc_Event event) {
  switch (event) {
    case netFTPc_EventSuccess:
      // File operation successful
      break;
 
    case netFTPc_EventTimeout:
      // Timeout on file operation
      break;
 
    case netFTPc_EventLoginFailed:
      // Login error, username/password invalid
      break;
 
    case netFTPc_EventAccessDenied:
      // File access not allowed
      break;
 
    case netFTPc_EventFileNotFound:
      // File not found
      break;
 
    case netFTPc_EventInvalidDirectory:
      // Working directory path not found
      break;
 
    case netFTPc_EventLocalFileError:
      // Local file open/write error
      break;
 
    case netFTPc_EventError:
      // Generic FTP client error
      break;
  }
}
//! [code_FTP_Client_UIF]
