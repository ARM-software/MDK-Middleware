/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FTP_Server_Multiuser.c
 * Purpose: FTP Server Multi-user Interface
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_FTP_Server_Multiuser]
#include <string.h>
#include "rl_net.h"
 
// \brief Check if an user account exists in the user database.
// \param[in]     username      pointer to username.
// \return      status information:
//              - User identification number or
//              - 0 if the user is not existing.
uint8_t netFTPs_CheckUsername (const char *username) {
  /* Example
  if (strcmp (username, "guest") == 0) {
    // Username is correct
    return (1);
  }
  */
  return (0);
}
 
// \brief Check user account password in the user database.
// \param[in]     user_id       user identification number.
// \param[in]     password      pointer to password.
// \return
//              - true        = password accepted.
//              - false       = invalid password.
bool netFTPs_CheckPassword (uint8_t user_id, const char *password) {
  /* Example
  if (user_id == 1) {
    if (strcmp (password, "guest") == 0) {
      // Password is correct
      return (true);
    }
  }
  */
  return (false);
}
 
// \brief Check if remote user is allowed to access a file on FTP server.
// \param[in]     user_id       user identification number.
// \param[in]     fname         full path of a file to access.
// \param[in]     access        access mode as defined with Network Access definitions.
// \return
//              - true        = File access is allowed.
//              - false       = File access is denied.
bool netFTPs_FileAccess (uint8_t user_id, const char *fname, uint32_t access) {
  /* Example
  if (user_id == 1) {
    if ((strcmp (fname, "/log/test.txt") == 0) && (access == NET_ACCESS_FILE_WRITE)) {
      // User "guest" is not allowed to modify or delete "test.txt" in "log" folder
      return (false);
    }
  }
  */
  return (true);
}
//! [code_FTP_Server_Multiuser]
