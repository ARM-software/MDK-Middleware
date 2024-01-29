/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Telnet_Server_UIF.c
 * Purpose: Telnet Server User Interface
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_Telnet_Server_UIF]
#include <stdio.h>
#include "rl_net.h"
 
// \brief Request a message for a Telnet server session.
// \param[in]     msg           code of requested message.
// \param[out]    buf           output buffer to write the message to.
// \param[in]     buf_len       length of the output buffer in bytes.
// \return        number of bytes written to output buffer.
uint32_t netTELNETs_ProcessMessage (netTELNETs_Message msg, char *buf, uint32_t buf_len) {
  uint32_t len = 0;
 
  switch (msg) {
    case netTELNETs_MessageWelcome:
      // Initial welcome message
      /* Example
      len = sprintf (buf, "\r\n"
                          "Embedded Telnet Server\r\n");
      */
      break;
 
    case netTELNETs_MessagePrompt:
      // Prompt message
      /* Example
      len = sprintf (buf, "\r\n"
                          "Cmd> ");
      */
      break;
 
    case netTELNETs_MessageLogin:
      // Login message, if authentication is enabled
      /* Example
      len = sprintf (buf, "\r\n"
                          "Please login...");
      */
      break;
 
    case netTELNETs_MessageUsername:
      // Username request login message
      /* Example
      len = sprintf (buf, "\r\n"
                          "Username: ");
      */
      break;
 
    case netTELNETs_MessagePassword:
      // Password request login message
      /* Example
      len = sprintf (buf, "\r\n"
                          "Password: ");
      */
      break;
 
    case netTELNETs_MessageLoginFailed:
      // Incorrect login error message
      /* Example
      len = sprintf (buf, "\r\n"
                          "Login incorrect");
      */
      break;
 
    case netTELNETs_MessageLoginTimeout:
      // Login timeout error message
      /* Example
      len = sprintf (buf, "\r\n"
                          "Login timeout\r\n");
      */
      break;
 
    case netTELNETs_MessageUnsolicited:
      // Unsolicited message (ie. from basic interpreter)
      /* Example
      len = sprintf (buf, "\r\n"
                          "Unsolicited message\r\n");
      */
      break;
  }
  return (len);
}
 
// \brief Process and execute a command requested by the Telnet client.
// \param[in]     cmd           pointer to command string from Telnet client.
// \param[out]    buf           output buffer to write the return message to.
// \param[in]     buf_len       length of the output buffer in bytes.
// \param[in,out] pvar          pointer to a session's local buffer of 4 bytes.
//                              - 1st call = cleared to 0,
//                              - 2nd call = not altered by the system,
//                              - 3rd call = not altered by the system, etc.
// \return        number of bytes written to output buffer.
//                - return len | (1u<<31) = repeat flag, the system calls this function
//                                          again for the same command.
//                - return len | (1u<<30) = disconnect flag, the system disconnects
//                                          the Telnet client.
uint32_t netTELNETs_ProcessCommand (const char *cmd, char *buf, uint32_t buf_len, uint32_t *pvar) {
  uint32_t len = 0;
 
  /* Example
  extern int32_t counter;
 
  if (netTELNETs_CheckCommand (cmd, "COUNT") == true) {
    // Print counter value
    len = sprintf (buf, "\r\n"
                        " Counter = %d", counter);
    return (len);
  }
  // ...
  if (netTELNETs_CheckCommand (cmd, "BYE") == true) {
    // Generate reply and disconnect
    len = sprintf (buf, "\r\n"
                        "Disconnecting\r\n");
    return (len | (1u<<30));
  }
  */
  return (len);
}
//! [code_Telnet_Server_UIF]
