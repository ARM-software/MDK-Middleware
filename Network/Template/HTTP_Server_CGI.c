/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_CGI.c
 * Purpose: HTTP Server CGI Module
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_HTTP_Server_CGI]
#include <stdio.h>
#include <string.h>
#include "rl_net.h"
 
// \brief Process query string received by GET request.
// \param[in]     qstr          pointer to the query string.
// \return        none.
void netCGI_ProcessQuery (const char *qstr) {
  char var[40];
 
  do {
    // Loop through all the parameters
    qstr = netCGI_GetEnvVar (qstr, var, sizeof (var));
    // Check return string, 'qstr' now points to the next parameter
    if (var[0] != 0) {
      // First character is non-null, string exists
 
      /* Example of a parameter "ip=192.168.0.100"
      if (strncmp (var, "ip=", 3) == 0) {
        uint8_t ip_addr[NET_ADDR_IP4_LEN];
        // Read parameter IP address submitted by the browser
        netIP_aton (&var[3], NET_ADDR_IP4, ip_addr);
         ...
      }
      */
    }
  } while (qstr);
}
 
// \brief Process data received by POST request.
// \param[in]     code          callback context:
//                              - 0 = www-url-encoded form data,
//                              - 1 = filename for file upload (null-terminated string),
//                              - 2 = file upload raw data,
//                              - 3 = end of file upload (file close requested),
//                              - 4 = any other type of POST data (single or last stream),
//                              - 5 = the same as 4, but with more data to follow.
// \param[in]     data          pointer to POST data.
// \param[in]     len           length of POST data.
// \return        none.
void netCGI_ProcessData (uint8_t code, const char *data, uint32_t len) {
  char var[40];
 
  switch (code) {
    case 0:
      // Url encoded form data received
      do {
        // Parse all parameters
        data = netCGI_GetEnvVar (data, var, sizeof (var));
        if (var[0] != 0) {
          // First character is non-null, string exists
          /* Example
          if (strcmp (var, "led0=on") == 0) {
            // ... Switch LED 0 on
          }
          */
        }
      } while (data);
      break;
 
    case 1:
      // Filename for file upload received
      /* Example
      if (data[0] != 0) {
        // Open a file for writing
        file = fopen (var, "w");
      }
      */
      break;
 
    case 2:
      // File content data received
      /* Example
      if (file != NULL) {
        // Write data to a file
        fwrite (data, 1, len, file);
      }
      */
      break;
 
    case 3:
      // File upload finished
      /* Example
      if (file != NULL) {
        // Close a file
        fclose (file);
      }
      */
      break;
 
    case 4:
      // Other content type data, last packet
      /* Example
      if (strcmp (netHTTPs_GetContentType(), "text/xml; charset=utf-8") == 0) {
        // Content type xml, utf-8 encoding
         ...
      }
      */
      break;
 
    case 5:
      // Other content type data, more to follow
      // ... Process data
      break;
 
    default:
      // Ignore all other codes
      break;
  }
}
 
// \brief Generate dynamic web data based on a CGI script.
// \param[in]     env           environment string.
// \param[out]    buf           output data buffer.
// \param[in]     buf_len       size of output buffer (from 536 to 1440 bytes).
// \param[in,out] pcgi          pointer to a session's local buffer of 4 bytes.
//                              - 1st call = cleared to 0,
//                              - 2nd call = not altered by the system,
//                              - 3rd call = not altered by the system, etc.
// \return        number of bytes written to output buffer.
//                - return len | (1U<<31) = repeat flag, the system calls this function
//                                          again for the same script line.
//                - return len | (1U<<30) = force transmit flag, the system transmits
//                                          current packet immediately.
uint32_t netCGI_Script (const char *env, char *buf, uint32_t buf_len, uint32_t *pcgi) {
  uint32_t len = 0;
 
  // Analyze a 'c' script line starting position 2
  /* Example script
  // c a i <td><input type=text name=ip value="%s" size=18 maxlength=18></td>
 
  char ip_ascii[40];
 
  switch (env[0]) {
    case 'a' :
      switch (env[2]) {
        case 'i':
          // Write the local IP address
          netIP_ntoa (NET_ADDR_IP4, IpAddr, ip_ascii, sizeof(ip_ascii));
          len = sprintf (buf, &env[4], ip_ascii);
          break;
          ...
      }
      break;
  }
  */
  return (len);
}
 
// \brief Override default Content-Type for CGX script files.
// \return        pointer to user defined Content-Type.
const char *netCGX_ContentType (void) {
  /* Example
  return ("text/xml; charset=utf-8");
  */
  return (NULL);
}
 
// \brief Override default character encoding in html documents.
// \return        pointer to user defined character set type.
const char *netCGI_Charset (void) {
  /* Example
  return ("utf-8");
  */
  return (NULL);
}
//! [code_HTTP_Server_CGI]
