/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Telnet_Server_UIF.c
 * Purpose: Telnet Server User Interface
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "cmsis_vio.h"                  // ::CMSIS Driver:VIO

#include "rl_net.h"                     // Keil::Network&MDK:CORE

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6210000)
  #pragma  clang diagnostic ignored "-Wformat-nonliteral"
  #pragma  clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

// ANSI ESC Sequence for clear screen
#define ANSI_CLS        "\033[2J"

// External references
extern bool LEDrun;
extern const char *net_tcp_ntoa (netTCP_State state);

// Command definition structure
typedef struct scmd {
  const char *string;
  int32_t (*func)(const char *par, char *buf, uint32_t buf_len, uint32_t *state);
} const SCMD;

// Command function prototypes
static int32_t cmd_led    (const char *par, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cmd_list   (const char *par, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cmd_tcpstat(const char *par, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cmd_rinfo  (const char *par, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cmd_passw  (const char *par, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cmd_help   (const char *par, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cmd_bye    (const char *par, char *buf, uint32_t buf_len, uint32_t *state);

// Command function table
static const SCMD cmd_table[] = {
  { "LED",    cmd_led     },
  { "LIST",   cmd_list    },
  { "TCPSTAT",cmd_tcpstat },
  { "RINFO",  cmd_rinfo   },
  { "PASSW",  cmd_passw   },
  { "HELP",   cmd_help    },
  { "?",      cmd_help    },
  { "BYE",    cmd_bye     },
  { NULL,     NULL        }
};

// Local constants
static const char intro[] = 
  "\r\n"
  "+-------------------------------------------------------------------+\r\n"
  "| Telnet Server: Command Line Interface (CLI) example               |\r\n"
  "+-------------------------------------------------------------------+\r\n";

static const char help1[] =
  "\r\n"
  "+ command --------------+ function ---------------------------------+\r\n"
  "| LED [xx]              | writes hexval xx to LED port or           |\r\n"
  "|                       |   (no parameter re-enables running lights)|\r\n"
  "| LIST n                | prints n log lines                        |\r\n"
  "| TCPSTAT               | prints TCP socket status                  |\r\n"
  "| RINFO                 | prints client info (IP address and port)  |\r\n";

static const char help2[] =
  "| PASSW n [new_password]| handle system password, n=action          |\r\n"
  "|                       |   (0=print, 1=change, 2=clear)            |\r\n";

static const char help3[] =
  "| HELP or ?             | displays this help                        |\r\n"
  "| <BS>                  | deletes character left                    |\r\n"
  "| <UP> or <DOWN>        | recalls command History                   |\r\n"
  "| BYE or <ESC> or ^C    | disconnects from server                   |\r\n"
  "+-----------------------+-------------------------------------------+\r\n";

static const char tcpstat_header[] =
  "+-----------------------------------------------------------------------------+\r\n"
  "| Sock  State        Port  Timer  Remote Address                Port          |\r\n"
  "+-----------------------------------------------------------------------------+\r\n";

// Request message for Telnet server session
uint32_t netTELNETs_ProcessMessage (netTELNETs_Message msg, char *buf, uint32_t buf_len) {
  int32_t len = 0;

  (void)buf_len;

  switch (msg) {
    case netTELNETs_MessageWelcome:
      // Initial welcome message
      len = sprintf(buf, intro);
      break;
    case netTELNETs_MessagePrompt:
      // Prompt message
      len = sprintf(buf, "\r\n"
                         "Cmd> ");
      break;
    case netTELNETs_MessageLogin:
      // Login message, if authentication is enabled
      len = sprintf(buf, "\r\n"
                         "Please login...");
      break;
    case netTELNETs_MessageUsername:
      // Username request login message
      len = sprintf(buf, "\r\n"
                         "Username: ");
      break;
    case netTELNETs_MessagePassword:
      // Password request login message
      len = sprintf(buf, "\r\n"
                         "Password: ");
      break;
    case netTELNETs_MessageLoginFailed:
      // Incorrect login error message
      len = sprintf(buf, "\r\n"
                         "Login incorrect");
      break;
    case netTELNETs_MessageLoginTimeout:
      // Login timeout error message
      len = sprintf(buf, "\r\n"
                         "Login timeout\r\n");
      break;
    case netTELNETs_MessageUnsolicited:
      // Unsolicited message (ie. from basic interpreter)
      break;
  }
  return ((uint32_t)len);
}

// Process a command and generate response
uint32_t netTELNETs_ProcessCommand (const char *cmd, char *buf, uint32_t buf_len, uint32_t *pvar) {
  const SCMD *p_cmd;
  const char *cp;
  int32_t len;

  // Command line parser
  for (p_cmd = cmd_table; p_cmd->string != NULL; p_cmd++) {
    if (!netTELNETs_CheckCommand (cmd, p_cmd->string)) {
      continue;
    }
    // Locate parameter position in cmd buffer
    for (cp = cmd; *cp; cp++) {
      if (*cp == ' ') {
        cp++;
        break;
      }
    }
    return ((uint32_t)p_cmd->func (cp, buf, buf_len, pvar));
  }

  len = sprintf (buf, "\r\nCommand error: %s", cmd);
  return ((uint32_t)len);
}

// LED command implementation
static int32_t cmd_led (const char *par, char *buf, uint32_t buf_len, uint32_t *state) {
  uint32_t val;
  int32_t  len = 0;

  (void)buf_len;
  (void)state;

  if (sscanf (par, "%x", &val) > 0) {
    vioSetSignal(0xFFU, val);
    if (LEDrun == true) {
      len = sprintf (buf,"\r\n Running Lights OFF");
      LEDrun = false;
    }
  }
  else if (LEDrun == false) {
    len = sprintf (buf,"\r\n Running Lights ON");
    LEDrun = true;
  }
  return (len);
}

// LIST command implementation
static int32_t cmd_list (const char *par, char *buf, uint32_t buf_len, uint32_t *state) {
  uint32_t val;
  int32_t  len = 0;

  (void)buf_len;

  if (*state == 0) {
    // First call to this function
    len = sprintf (buf, ANSI_CLS);
    // Read parameter n
    if (sscanf (par, "%u", &val) > 0) {
      if (val > 65535) val = 65535;
    }
    else {
      val = 100;
    }
    *state = val << 16;
    if (*state != 0) {
      // Bit-31 is a repeat flag
      len |= (1u << 31);
    }
  }
  else {
    // Subsequent call to this function
    int32_t max = *state >> 16;
    int32_t ln  = *state & 0xFFFF;
    len = sprintf (buf, "This is line # %d in syslog.\r\n", ++ln);
    if (ln < max) {
      *state = *state + 1;
      // Set request for another callback
      len |= (1u << 31);
    }
  }
  return (len);
}

// TCPSTAT command implementation
static int32_t cmd_tcpstat (const char *par, char *buf, uint32_t buf_len, uint32_t *state) {
  netTCP_State tcp_state;
  NET_ADDR peer;
  char ip_ascii[40];
  int32_t len = 0;

  (void)par;
  (void)buf_len;

  if (*state == 0) {
    // First call to this function
    len  = sprintf (buf, ANSI_CLS);
    len += sprintf (buf+len, tcpstat_header);
    *state = *state + 1;
    // Request a 2nd call
    len |= (1u << 31);
  }
  else {
    // Subsequent call to this function
    int32_t socket = (int32_t)*state;
    tcp_state = netTCP_GetState (socket);
    if (tcp_state == netTCP_StateINVALID) {
      // Invalid socket, we are done
      len = sprintf (buf, "\r\n\r\n"
                          "Press any key to end!");
      // Reset state for next callback after 2 seconds
      netTELNETs_RepeatCommand (20);
      *state = 0;
      len |= (1u << 31);
    }
    else {
      // Column: Socket, State
      len = sprintf (buf, "\r\n  %-6d%-13s", socket, net_tcp_ntoa(tcp_state));
      // Column: Port
      if (tcp_state >= netTCP_StateLISTEN) {
        len += sprintf (buf+len, "%-6d", netTCP_GetLocalPort (socket));
      }
      // Column: Timer
      if (tcp_state > netTCP_StateLISTEN) {
        len += sprintf (buf+len, "%-7d", netTCP_GetTimer (socket));
      }      
      // Column: Address, Port
      if (tcp_state > netTCP_StateLISTEN) {
        netTCP_GetPeer (socket, &peer, sizeof(peer));
        netIP_ntoa (peer.addr_type, peer.addr, ip_ascii, sizeof(ip_ascii));
        len += sprintf (buf+len, "%-30s%-5d", ip_ascii, peer.port);
      }
      *state = *state + 1;
      len |= (1u << 31);
    }
  }
  return (len);
}

// RINFO command implementation
static int32_t cmd_rinfo (const char *par, char *buf, uint32_t buf_len, uint32_t *state) {
  NET_ADDR client;
  char ip_ascii[40];
  int32_t len = 0;

  (void)par;
  (void)buf_len;
  (void)state;

  if (netTELNETs_GetClient (&client, sizeof(client)) == netOK) {
    // Convert client IP address to ASCII string
    netIP_ntoa (client.addr_type, client.addr, ip_ascii, sizeof(ip_ascii));
    len  = sprintf (buf    ,"\r\n IP address: %s", ip_ascii);
    len += sprintf (buf+len,"\r\n TCP port  : %d", client.port);
  }
  else {
    len  = sprintf (buf, "\r\n Error!");
  }
  return (len);
}

// PASSW command implementation
static int32_t cmd_passw  (const char *par, char *buf, uint32_t buf_len, uint32_t *state) {
  int32_t len = 0;

  (void)buf_len;
  (void)state;

  if (!netTELNETs_LoginActive()) {
    len = sprintf (buf, "\r\n Authentication not enabled!");
    return (len);
  }
  switch (*par) {
    case '0':
      // Print password
      len = sprintf (buf, "\r\n System Password: \"%s\"", netTELNETs_GetPassword());
      break;
    case '1':
      // Change password
      if ((strlen (par) > 2) && (netTELNETs_SetPassword (&par[2]) == netOK)) {
        len = sprintf (buf, "\r\n OK, New Password: \"%s\"", netTELNETs_GetPassword ());
      }
      else {
        len = sprintf (buf, "\r\n Failed to change password!");
      }
      break;
    case '2':
      // Clear password
      netTELNETs_SetPassword ("");
      len = sprintf (buf, "\r\n OK, Password cleared");
      break;
    default:
      // Error
      len = sprintf (buf, "\r\n Command Error");
      break;
  }
  return (len);
}

// HELP command implementation
static int32_t cmd_help (const char *par, char *buf, uint32_t buf_len, uint32_t *state) {
  int32_t len = 0;

  (void)par;
  (void)buf_len;
  (void)state;

  len = sprintf (buf, help1);
  if (netTELNETs_LoginActive()) {
    len += sprintf (buf+len, help2);
  }
  len += sprintf (buf+len, help3);
  return (len);
}

// BYE command implementation
static int32_t cmd_bye (const char *par, char *buf, uint32_t buf_len, uint32_t *state) {
  int32_t len = 0;

  (void)par;
  (void)buf_len;
  (void)state;

  len = sprintf (buf, "\r\n Disconnected");
  // Bit-30 is a disconnect flag
  return (len | (1 << 30));
}
