/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2021 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Config_HTTP_Server.h
 * Purpose: Network Configuration for HTTP Server
 * Rev.:    V7.2.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>HTTP Server
#define HTTP_SERVER_ENABLE          1

//   <o>Number of Sessions <1-10>
//   <i>Number of simultaneously active HTTP Sessions.
//   <i>Default: 6
#define HTTP_SERVER_NUM_SESSIONS    6

//   <o>Port Number <1-65535>
//   <i>Listening port number
//   <i>Default: 0 (autoselects: 80/HTTP, 443/HTTPS)
#define HTTP_SERVER_PORT_NUM        0

//   <s.50>Server-Id header
//   <i>Optional header, which overrides the default server-id header.
//   <i>Default: ""
#define HTTP_SERVER_ID              ""

//   <e>Enable User Authentication
//   <i>When enabled, requires authentication of the user through
//   <i>the credentials to access any web page on the server.
#define HTTP_SERVER_AUTH_ENABLE     1

//     <o>Authentication Scheme <0=>Basic <1=>Digest
//     <i>Defines Access Authentication type
//     <i>Default: Basic
#define HTTP_SERVER_AUTH_TYPE       0

//     <s.20>Authentication Realm
//     <i>Defines the protection space on server
//     <i>Default: "Embedded WEB Server"
#define HTTP_SERVER_AUTH_REALM      "Embedded WEB Server"

//     <e>Built-in Administrator Account
//     <i>Enable the built-in Administrator account on the server
//     <i>Default: Enabled
#define HTTP_SERVER_AUTH_ADMIN      1

//       <s.15>Administrator Username
//       <i>Default: "admin"
#define HTTP_SERVER_AUTH_USER       "admin"

//       <s.32>Administrator Secret
//       <i>Plain text password or Digested value HA1
//       <i>HA1 = MD5(username:realm:password)
//       <i>Default: ""
#define HTTP_SERVER_AUTH_PASS       ""
//     </e>
//   </e>

//   <e>Enable Root Folder
//   <i>When enabled, the system prepends the root folder to the file name
//   <i>when opening web resource files on external local drive.
//   <i>Default: Disabled
#define HTTP_SERVER_ROOT_ENABLE     0

//     <s.80>Web Root Folder
//     <i>Absolute path to the Web root folder on external drive
//     <i>*** Use unix style '/' folder separators ***
//     <i>Default: "/Web"
#define HTTP_SERVER_ROOT_FOLDER     "/Web"
//   </e>

// </h>

// Number of nonce cache entries
#define HTTP_SERVER_NONCE_TAB_SIZE   8

// Nonce cache timeout in seconds
#define HTTP_SERVER_NONCE_CACHE_TOUT 60

//------------- <<< end of configuration section >>> ---------------------------
