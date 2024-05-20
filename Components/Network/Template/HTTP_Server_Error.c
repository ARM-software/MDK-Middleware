/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_Error.c
 * Purpose: HTTP Server Friendly Error Message Definitions
 * Rev.:    V7.6.0
 *----------------------------------------------------------------------------*/
//! [code_HTTP_Server_Error]
typedef struct net_http_error {
  const char *header;                   ///< Common page header
  const char *footer;                   ///< Common page footer
  const char *e401;                     ///< 401 Unauthorized
  const char *e403;                     ///< 403 Forbidden
  const char *e404;                     ///< 404 Not Found
  const char *e501;                     ///< 501 Not Implemented
} const NET_HTTP_ERROR;
 
// Keep HTTP Error page size small
extern NET_HTTP_ERROR net_http_error;
       NET_HTTP_ERROR net_http_error = {
  // HTTP Error page header
  "<head><title>Server Error</title></head>"
  "<body>",
 
  // HTTP Error page footer
  "<hr><br>"
  "<i>Keil Embedded WEB Server V2.30, 2024<br>"
  "<a href=https://www.keil.com>www.keil.com</a>"
  " - Embedded Development Tools</i>"
  "</body>",
 
  // HTTP Error 401 - Unauthorized access
  "<h2>Error 401 - Unauthorized Access</h2>"
  "You are not authorized to access this server.",
 
  // HTTP Error 403 - Forbidden
  "<h2>Error 403 - Forbidden</h2>"
  "You don't have permission to access this resource.",
 
  // HTTP Error 404 - Not Found
  "<h2>Error 404 - Not Found</h2>"
  "The requested URL was not found on this server.",
 
  // HTTP Error 501 - Not Implemented
  "<h2>Error 501 - Not Implemented</h2>"
  "The requested Method is not supported."
};
//! [code_HTTP_Server_Error]
