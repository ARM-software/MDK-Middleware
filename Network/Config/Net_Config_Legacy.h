/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Config_Legacy.h
 * Purpose: Network Configuration for Legacy API
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>Network Legacy Configuration

//   <q>Ethernet Notifications
//   <i>Enable or disable Ethernet legacy notifications:
//   <i> - eth_link_notify    (called from netETH_Notify)
//   <i> - dhcp_client_notify (called from netDHCP_Notify)
#define ETH_LEGACY              1

//   <q>FTP Server Callbacks
//   <i>Enable or disable FTP Server legacy callbacks:
//   <i> - ftp_accept_client (called from netFTPs_AcceptClient)
//   <i> - ftp_file_access   (called from netFTPs_FileAccess)
#define FTP_SERVER_LEGACY       1

//   <q>TFTP Server Callback
//   <i>Enable or disable TFTP Server legacy callback:
//   <i> - tftp_accept_client (called from netTFTPs_AcceptClient)
#define TFTP_SERVER_LEGACY      1

//   <q>TFTP Client Callback
//   <i>Enable or disable TFTP Client legacy callback:
//   <i> - tftp_client_notify (called from netTFTPc_Notify)
#define TFTP_CLIENT_LEGACY      1

//   <q>Telnet Server Callback
//   <i>Enable or disable Telnet Server legacy callback:
//   <i> - telnet_accept_client (called from netTELNETs_AcceptClient)
#define TELNET_SERVER_LEGACY    1

//   <q>HTTP Server Callback
//   <i>Enable or disable HTTP Server legacy callback:
//   <i> - http_accept_client (called from netHTTPs_AcceptClient)
#define HTTP_SERVER_LEGACY      1

//   <q>SMTP Client Callback
//   <i>Enable or disable SMTP Client legacy callback:
//   <i> - smtp_client_accept_authentication (called from netSMTPc_AcceptAuthentication)
#define SMTP_CLIENT_LEGACY      1

// </h>

//------------- <<< end of configuration section >>> ---------------------------
