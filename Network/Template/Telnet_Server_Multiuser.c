/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Telnet_Server_Multiuser.c
 * Purpose: Telnet Server Multi-user Interface
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_Telnet_Server_Multiuser]
#include <string.h>
#include "rl_net.h"
 
// \brief Check if an user account exist in the user database.
// \param[in]     username      pointer to username.
// \return      status information:
//              - User identification number or
//              - 0 if the user is not existing.
uint8_t netTELNETs_CheckUsername (const char *username) {
  /* Example
  if (strcmp (username, "guest") == 0) {
    // Username is correct, return user_id
    return (1);
  }
  */
  return (0);
}
 
// Check user account password in the user database.
// \param[in]     user_id       user identification number.
// \param[in]     password      pointer to password.
// \return
//              - true        = password accepted.
//              - false       = invalid password.
bool netTELNETs_CheckPassword (uint8_t user_id, const char *password) {
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
//! [code_Telnet_Server_Multiuser]
