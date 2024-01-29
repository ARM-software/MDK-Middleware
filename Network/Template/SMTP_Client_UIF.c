/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    SMTP_Client_UIF.c
 * Purpose: SMTP Client User Interface
 * Rev.:    V7.1.0
 *----------------------------------------------------------------------------*/
//! [code_SMTP_Client_UIF]
#include <stdio.h>
#include "rl_net.h"
 
// \brief Request parameters for SMTP client session. 
// \param[in]     request       request code.
// \param[out]    buf           output buffer to write the data to.
// \param[in]     buf_len       length of the output buffer in bytes.
// \param[in,out] pvar          pointer to a session's local buffer of 4 bytes.
//                              - 1st call = cleared to 0,
//                              - 2nd call = not altered by the system,
//                              - 3rd call = not altered by the system, etc.
// \return        number of bytes written to output buffer.
//                - return len | (1u<<31) = repeat flag, the system calls this function
//                                          again when request is netSMTPc_RequestBody.
uint32_t netSMTPc_Process (netSMTPc_Request request, char *buf, uint32_t buf_len, uint32_t *pvar) {
  uint32_t len = 0;
 
  switch (request) {
    case netSMTPc_RequestUsername:
      // Username to login to SMTP server
      /* Example
      len = sprintf (buf, "guest");
      */
      break;
 
    case netSMTPc_RequestPassword:
      // Password to login to SMTP server
      /* Example
      len = sprintf (buf, "guest");
      */
      break;
 
    case netSMTPc_RequestSender:
      // Email address of the sender
      /* Example
      len = sprintf (buf, "me@domain.com");
      */
      break;
 
    case netSMTPc_RequestRecipient:
      // Email address of the recipient
      /* Example
      len = sprintf (buf, "you@domain.com");
      */
      break;
 
    case netSMTPc_RequestSubject:
      // Subject of email
      /* Example
      len = sprintf (buf, "Hello");
      */
      break;
 
    case netSMTPc_RequestBody:
      // Email body in plain ascii format
      /* Example
      len = sprintf (buf, "Hello, how are you?\r\n");
      */
      break;
  }
  return (len);
}
 
// \brief Notify the user application when SMTP client operation ends.
// \param[in]     event         SMTP client notification event.
// \return        none.
void netSMTPc_Notify (netSMTPc_Event event) {
  switch (event) {
    case netSMTPc_EventSuccess:
      // Email successfully sent
      break;
 
    case netSMTPc_EventTimeout:
      // Timeout sending email
      break;
 
    case netSMTPc_EventAuthenticationFailed:
      // User authentication failed
      break;

    case netSMTPc_EventError:
      // Error when sending email
      break;
  }
}
 
// \brief Accept or deny authentication requested by SMTP server.
// \param[in]     addr          structure containing IP address and port of SMTP server.
// \return
//              - true        = Authentication is accepted.
//              - false       = Authentication is denied.
bool netSMTPc_AcceptAuthentication (const NET_ADDR *addr) {
  /* Example
  if (addr->addr_type == NET_ADDR_IP4) {
    // IPv4 server
    if (addr->addr[0] == 192  &&
        addr->addr[1] == 168  &&
        addr->addr[2] == 0    &&
        addr->addr[3] == 1) {
      // Deny authentication from local server at 192.168.0.1
      return (false);
    }
  }
  else {
    // IPv6 server
    const uint8_t ip6_addr[NET_ADDR_IP6_LEN] = { 
                     0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x1c, 0x30, 0x6c, 0xff, 0xfe, 0xa2, 0x45, 0x5e };
    if (memcmp (addr->addr, ip6_addr, NET_ADDR_IP6_LEN) == 0) {
      // Deny authentication from local server at [fe80::1c30:6cff:fea2:455e]
      return (false);
    }
  }
  */
  return (true);
}
//! [code_SMTP_Client_UIF]
