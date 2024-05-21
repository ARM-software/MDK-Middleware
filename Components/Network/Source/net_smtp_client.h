/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_smtp_client.h
 * Purpose: Mail Transfer Client Definitions
 *----------------------------------------------------------------------------*/

#ifndef NET_SMTP_CLIENT_H_
#define NET_SMTP_CLIENT_H_

#include "net_lib.h"

#define SMTP_SERVER_PORT    25          // SMTP server standard port
#define SMTP_MAX_RCPT       20          // Maximum number of recipients
#define SMTPS_SERVER_PORT   465         // SMTPS server standard port
#define SMTPS_HEADER_LEN    5           // Size of SSL header in SMTPS

/* SMTP States */
#define SMTP_STATE_IDLE     0           // SMTP client is idle
#define SMTP_STATE_CONNECT  1           // Connect to SMTP server requested
#define SMTP_STATE_TLSINIT  2           // Initialize TLS context for SMTPS
#define SMTP_STATE_READY    3           // Connected, check if server ready
#define SMTP_STATE_EHLO     4           // Send 'EHLO' command try ESMTP
#define SMTP_STATE_STARTTLS 5           // Send 'STARTTLS' command for SMTPS
#define SMTP_STATE_HELO     6           // Send 'HELO' command for SMTP
#define SMTP_STATE_AUTH     7           // ESMTP user authentication
#define SMTP_STATE_USER     8           // Send username and password
#define SMTP_STATE_MAILFROM 9           // Send 'MAIL FROM' command
#define SMTP_STATE_RCPTTO   10          // Send 'RCPT TO' command
#define SMTP_STATE_DATA     11          // Send 'DATA' command
#define SMTP_STATE_MSGBODY  12          // Send message body
#define SMTP_STATE_MSGEND   13          // Send message end sequence
#define SMTP_STATE_QUIT     14          // Send 'QUIT' command
#define SMTP_STATE_STOP     15          // Stop SMTP client
#define SMTP_STATE_TWAIT    16          // Wait for socket to be closed

/* SMTP Flags */
#define SMTP_FLAG_WACK      0x0001      // Waiting for data acknowledge
#define SMTP_FLAG_RESP      0x0002      // Waiting for SMTP response
#define SMTP_FLAG_PASSW     0x0004      // Send a password to server
#define SMTP_FLAG_RCPTEND   0x0008      // List of recipients completed
#define SMTP_FLAG_HEADER    0x0010      // Multipart header done
#define SMTP_FLAG_DATAEND   0x0020      // Mail end, nomore data to send
#define SMTP_FLAG_FIRST     0x0040      // First call to the function
#define SMTP_FLAG_STARTTLS  0x0080      // 'STARTTLS' advertisement received
#define SMTP_FLAG_USETLS    0x0100      // Use TLS security mode
#define SMTP_FLAG_ERROR     0x0200      // Client error, abort operation
#define SMTP_FLAG_NOFILE    0x0400      // Mail attachment not existing
#define SMTP_FLAG_SUSPEND   0x0800      // SendMail thread suspended

/* SMTP Authentication Modes */
#define SMTP_AUTH_NONE      0           // Authentication not required
#define SMTP_AUTH_PLAIN     1           // PLAIN authentication type
#define SMTP_AUTH_LOGIN     2           // LOGIN authentication type
#define SMTP_AUTH_DIGEST_MD5 3          // DIGEST-MD5 authentication type
#define SMTP_AUTH_CRAM_MD5  4           // CRAM-MD5 authentication type
#define SMTP_AUTH_NTLM      5           // NTLM authentication type
#define SMTP_AUTH_UNKNOWN   6           // Unknown authentication type

/* SMTP server reply codes */
#define SMTP_RESP_READY     0           // Server is ready
#define SMTP_RESP_QUITOK    1           // Service closing transmission channel
#define SMTP_RESP_AUTHOK    2           // User authentication successful
#define SMTP_RESP_OK        3           // Requested mail action ok, ready
#define SMTP_RESP_CONTINUE  4           // Continue request
#define SMTP_RESP_DATAOK    5           // Start mail input; end with <CRLF>.<CRLF>
#define SMTP_RESP_UNKNOWN   6           // Unrecognised response

/* SMTP user interface callbacks */
typedef struct net_smtp_uif {
  uint32_t (*process)(netSMTPc_Request,char*,uint32_t,uint32_t*);
  bool     (*accept) (const NET_ADDR*);
  void     (*notify) (netSMTPc_Event);
} NET_SMTP_UIF;

/* SMTP address parser info */
typedef struct net_smtp_tag {
  const char *Base;                     // Base address of email address
  uint8_t  NameOffs;                    // Friendly name offset
  uint8_t  NameLen;                     // Length of friendly name
  uint8_t  AddrOffs;                    // Email address offset
  uint8_t  AddrLen;                     // Length of email address
} NET_SMTP_TAG;

/* SMTP core control info */
typedef struct net_smtp_ctrl {
  uint8_t  State;                       // Session state
  uint8_t  Socket;                      // Assigned TCP Socket
  uint16_t Flags;                       // State flags
  __ADDR   Server;                      // Server IP address and port
  const char *SrvName;                  // Hostname of the server (client only)
  uint8_t  Timer;                       // Timeout Timer value
  uint8_t  AuthMode;                    // SMTP Authentication mode
  uint8_t  TlsId;                       // TLS context identifier
  bool     Secure;                      // Secure TLS mode active
  netSMTPc_Event cb_event;              // Return value for callback
  uint8_t  NumRcpt;                     // Number of recipient email addresses
  uint8_t  CramLen;                     // Length of CRAM-MD5 challenge message
  char    *CramBuf;                     // CRAM-MD5 challenge message value
  uint32_t SVar;                        // Session private variable
  const NET_SMTP_UIF *uif;              // User interface callback functions
  uint32_t (*f_compose)(char*,uint32_t);// Compose mail callback function
} NET_SMTP_CTRL;

/* SMTP client session info */
typedef struct net_smtp_info {
  NETOS_ID Thread;                      // Calling thread identifier
  uint8_t  NumFiles;                    // Number of files to attach
  bool     Encode8bit;                  // Message needs 8-bit encoding
  bool     Attach;                      // Attachment file opened
  const NET_SMTP_MAIL *mail;            // Email content descriptor
  const NET_SMTP_MTA  *mta;             // Server address and user credentials
  const char *From;                     // Mail From address
  const char *Rcpt[SMTP_MAX_RCPT];      // Mail Recipient addresses
  uint8_t  Hash[16];                    // Random hash for boundary calculation
} NET_SMTP_INFO;

/* Variables */
extern NET_SMTP_CTRL  net_smtp_control;
#define smtp        (&net_smtp_control)
extern NET_SMTP_CFG   net_smtp_config;
#define smtpc       (&net_smtp_config)

/* Functions */
extern void net_smtp_client_init (void);
extern void net_smtp_client_uninit (void);
extern void net_smtp_client_run (void);
extern void net_smtp_client_start (uint16_t use_tls);

#endif /* NET_SMTP_CLIENT_H_ */
