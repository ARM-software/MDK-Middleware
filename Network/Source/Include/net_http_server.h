/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_http_server.h
 * Purpose: Web Server Definitions
 *----------------------------------------------------------------------------*/

#ifndef __NET_HTTP_SERVER_H
#define __NET_HTTP_SERVER_H

#include "rl_net_lib.h"

#define HTTP_SERVER_PORT    80          // HTTP Server standard port
#define HTTP_CGI_MAXLINE    120         // HTTP CGI Script max. size of one line
#define HTTPS_SERVER_PORT   443         // HTTPS Server standard port
#define HTTPS_HEADER_LEN    5           // Size of SSL header in HTTPS

#define NET_BUFFER_HLEN     8           // The size of NET_BUFFER header
#define COOKIE_HEADER_LEN   ((NET_BUFFER_HLEN - NET_HEADER_LEN) | 0x40000000)

/* HTTP Server States */
#define HTTP_STATE_IDLE     0           // Session is free and unused
#define HTTP_STATE_ERROR    1           // Error, session not initialized
#define HTTP_STATE_RESERVED 2           // Reserved, session not activated
#define HTTP_STATE_ACTIVE   3           // Active, waiting for GET or POST
#define HTTP_STATE_FILE     4           // Sending file
#define HTTP_STATE_CGI      5           // Processiong CGI script file
#define HTTP_STATE_TEXT     6           // Sending CGI generated data
#define HTTP_STATE_UNAUTH   7           // Unauthorized access to Web Server
#define HTTP_STATE_POST     8           // Multipacket POST
#define HTTP_STATE_UPLOAD   9           // File upload with POST
#define HTTP_STATE_NOTIMP   10          // Request method not implemented

/* HTTP Server File Types */
#define HTTP_TYPE_HTML      0           // html text
#define HTTP_TYPE_CGI       1           // cgi script
#define HTTP_TYPE_CSS       2           // css style sheet
#define HTTP_TYPE_JS        3           // java script
#define HTTP_TYPE_XML       4           // xml text
#define HTTP_TYPE_XSLT      5           // xslt style sheet
#define HTTP_TYPE_GIF       6           // gif image
#define HTTP_TYPE_JPEG      7           // jpeg image
#define HTTP_TYPE_BMP       8           // bmp image
#define HTTP_TYPE_PNG       9           // png image
#define HTTP_TYPE_ICO       10          // icon image
#define HTTP_TYPE_BIN       11          // binary stream
#define HTTP_TYPE_CGX       12          // cgi-xml script
#define HTTP_TYPE_XAP       13          // xap Silverlight application
#define HTTP_TYPE_XAML      14          // extended xml
#define HTTP_TYPE_PHP       15          // php script
#define HTTP_TYPE_TXT       16          // plain text
#define HTTP_TYPE_SVG       17          // svg image
#define HTTP_TYPE_SVGZ      18          // svg image compressed (gzip)
#define HTTP_TYPE_JZ        19          // java script compressed (gzip)
#define HTTP_TYPE_JAR       20          // java script archive
#define HTTP_TYPE_ZIP       21          // zip compressed archive
#define HTTP_TYPE_DLL       22          // executable or system file
#define HTTP_TYPE_PDF       23          // pdf document
#define HTTP_TYPE_USER      24          // user type document

/* HTTP Server Error Codes */
#define HTTP_ERR_301        0           // 301 Moved Permanently
#define HTTP_ERR_304        1           // 304 Not Modified
#define HTTP_ERR_401        2           // 401 Unauthorized
#define HTTP_ERR_403        3           // 403 Forbidden
#define HTTP_ERR_404        4           // 404 Not Found
#define HTTP_ERR_501        5           // 501 Not Implemented

/* HTTP Server Error Classes */
#define HTTP_ERR_3xx        HTTP_ERR_301// 3xx Redirection
#define HTTP_ERR_4xx        HTTP_ERR_401// 4xx Client Error
#define HTTP_ERR_5xx        HTTP_ERR_501// 5xx Server Error

/* HTTP Server Script Command Tokens */
#define CGI_CMD_DOT         0x00        // '.' command (end of script)
#define CGI_CMD_T           0x01        // 't' command script token
#define CGI_CMD_C           0x02        // 'c' command script token
#define CGI_CMD_I           0x03        // 'i' command script token
#define CGI_CMD_END         (CGI_CMD_I+1)

/* HTTP Server Internal Flags */
#define HTTP_FLAG_CGI       0x0001      // Processing CGI Script file
#define HTTP_FLAG_HEADER    0x0002      // HTTP Header has been sent
#define HTTP_FLAG_NOFILE    0x0004      // Requested file not found
#define HTTP_FLAG_NOACCESS  0x0008      // Access to requested file not allowed
#define HTTP_FLAG_CGIREP    0x0010      // Repeat again CGI Script line
#define HTTP_FLAG_CACHED    0x0020      // File Cached on Browser
#define HTTP_FLAG_NEXT      0x0040      // Next CGI line already fetched
#define HTTP_FLAG_DFILE     0x0080      // External Data File opened
#define HTTP_FLAG_SFILE     0x0100      // External Script File opened
#define HTTP_FLAG_UFILE     0x0200      // Upload File opened
#define HTTP_FLAG_MULTI     0x0400      // Multipart POST form data
#define HTTP_FLAG_XPOST     0x0800      // Other POST type, no buffering (XML,..)
#define HTTP_FLAG_STALE     0x1000      // Nonce number is stale (Digest auth.)

/* HTTP Request Methods */
#define HTTP_METHOD_GET     1           // GET method
#define HTTP_METHOD_POST    2           // POST method
#define HTTP_METHOD_HEAD    3           // HEAD method
#define HTTP_METHOD_PUT     4           // PUT method
#define HTTP_METHOD_DELETE  5           // DELETE method
#define HTTP_METHOD_CONNECT 6           // CONNECT method
#define HTTP_METHOD_OPTIONS 7           // OPTIONS method
#define HTTP_METHOD_TRACE   8           // TRACE method

/* HTTP File type */
typedef struct net_http_ftype {
  char    ext[5];                       // Finename extension
  uint8_t id;                           // Internal server type id
} NET_HTTP_FTYPE;

/* HTTP CGI info */
typedef struct net_cgi_info {
  uint8_t     session;                  // CGI session index
  uint16_t    len;                      // Data len for the callback return call
  const char *end;                      // End of data pointer (buf+len)
} NET_CGI_INFO;

/* HTTP parser tag info */
typedef struct {
  const char *sp;                       // Pointer to a string
  int32_t     len;                      // Length of a string
} NET_TAG_INFO;

/* HTTP auth parser info */
typedef struct net_http_tag {
  NET_TAG_INFO username;                // username="<username>"
  NET_TAG_INFO realm;                   // realm="<realm>"
  NET_TAG_INFO nonce;                   // nonce="<server-nonce>"
  NET_TAG_INFO uri;                     // uri="<requested-uri>"
  NET_TAG_INFO qop;                     // qop=qop-value
  NET_TAG_INFO nc;                      // nc=nc-value
  NET_TAG_INFO cnonce;                  // cnonce="<client-nonce>"
  NET_TAG_INFO response;                // response="<request-digest>"
  NET_TAG_INFO opaque;                  // opaque="<opaque>"
} NET_HTTP_TAG;

#define __HTTP_TAG(tag)     (NET_HTTP_TAG *)((void *)(tag))

/* HTTP core control info */
typedef struct net_http_ctrl {
  uint16_t port_num;                    // Service port number
  bool     en_auth;                     // Enable authentication 
  bool     run_service;                 // Run service
  const char *root;                     // Root folder path
  const char *user;                     // Username for admin account
  const char *passw;                    // Password for admin account
  uint8_t  user_len;                    // Length of username
  uint8_t  passw_len;                   // Length of password
  bool     hdr_access;                  // Enable user access to request headers
  bool     en_cookie;                   // Enable support for cookies
} NET_HTTP_CTRL;

/* Variables */
extern NET_HTTP_CTRL net_http_control;
#define http       (&net_http_control)
extern NET_HTTP_CFG  net_http_config;
#define httpc      (&net_http_config)

extern const NET_HTTP_ERROR net_http_error;

/* Functions */
extern void net_http_server_init (void);
extern void net_http_server_uninit (void);
extern void net_http_server_run (void);

/* ROM Image */
extern const uint32_t imageLastModified;
extern uint32_t       imageFileInfo (const char *name, const uint8_t **data);

#endif /* __NET_HTTP_SERVER_H */
