/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_Multiuser.c
 * Purpose: HTTP Server Multi-user Interface
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_HTTP_Server_Multiuser]
#include <string.h>
#include "rl_net.h"
 
// \brief Check if an user account exist in the user database.
// \param[in]     username      pointer to username.
// \param[in]     password      pointer to password.
// \return      status information:
//              - User identification number or
//              - 0 if the user is not existing.
uint8_t netHTTPs_CheckAccount (const char *username, const char *password) {
  /* Example
  if ((strcmp (username, "guest") == 0) && (strcmp (password, "guest") == 0)) {
    // Accept guest account
    return (1);
  }
  */
  return (0);
}
 
// \brief Check if remote user is allowed to access a file on HTTP server.
// \param[in]     user_id       user identification number.
// \param[in]     fname         name of a file to access.
// \return
//              - true        = File access is allowed.
//              - false       = File access is denied.
bool netHTTPs_FileAccess (uint8_t user_id, const char *fname) {
  /* Example
  if (user_id == 1) {
    if (strcmp (fname, "system.cgi") == 0) {
      // User "guest" is not allowed to see "system.cgi"
      return (false);
    }
  }
  */
  return (true);
}
//! [code_HTTP_Server_Multiuser]
