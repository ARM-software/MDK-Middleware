/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2026 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Telnet_Server_UIF.c
 * Purpose: Telnet Server User Interface
 * Rev.:    V7.1.0
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "cmsis_vio.h"                  // ::CMSIS Driver:VIO

#include "rl_net.h"                     // Keil::Network&MDK:CORE

// ANSI ESC Sequence for clear screen
#define ANSI_CLS        "\033[2J"

// External references
extern bool LEDrun;
extern const char *net_tcp_ntoa (netTCP_State state);

// Output buffer descriptor
struct buffer {
  char   *data;
  int32_t size;
  int32_t len;
};

// Command definition structure
typedef struct scmd {
  const char *string;
  void (*func)(const char *par, struct buffer *b, uint32_t *state);
} const SCMD;

// Command functions
static void cmd_led    (const char *par, struct buffer *b, uint32_t *state);
static void cmd_list   (const char *par, struct buffer *b, uint32_t *state);
static void cmd_tcpstat(const char *par, struct buffer *b, uint32_t *state);
static void cmd_rinfo  (const char *par, struct buffer *b, uint32_t *state);
static void cmd_passw  (const char *par, struct buffer *b, uint32_t *state);
static void cmd_help   (const char *par, struct buffer *b, uint32_t *state);
static void cmd_bye    (const char *par, struct buffer *b, uint32_t *state);

// Local functions
static int32_t bprintf (struct buffer *b, const char *fmt, ...);

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
  struct buffer b = {
    .data = buf,
    .size = buf_len,
    .len  = 0
  };

  switch (msg) {
    case netTELNETs_MessageWelcome:
      // Initial welcome message
      bprintf (&b, intro);
      break;
    case netTELNETs_MessagePrompt:
      // Prompt message
      bprintf (&b, "\r\n"
                   "Cmd> ");
      break;
    case netTELNETs_MessageLogin:
      // Login message, if authentication is enabled
      bprintf (&b, "\r\n"
                   "Please login...");
      break;
    case netTELNETs_MessageUsername:
      // Username request login message
      bprintf (&b, "\r\n"
                   "Username: ");
      break;
    case netTELNETs_MessagePassword:
      // Password request login message
      bprintf (&b, "\r\n"
                   "Password: ");
      break;
    case netTELNETs_MessageLoginFailed:
      // Incorrect login error message
      bprintf (&b, "\r\n"
                   "Login incorrect");
      break;
    case netTELNETs_MessageLoginTimeout:
      // Login timeout error message
      bprintf (&b, "\r\n"
                   "Login timeout\r\n");
      break;
    case netTELNETs_MessageUnsolicited:
      // Unsolicited message (ie. from basic interpreter)
      break;
  }
  return ((uint32_t)b.len);
}

// Process a command and generate response
uint32_t netTELNETs_ProcessCommand (const char *cmd, char *buf, uint32_t buf_len, uint32_t *pvar) {
  const SCMD *p_cmd;
  const char *cp;
  struct buffer b = {
    .data = buf,
    .size = buf_len,
    .len  = 0
  };

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
    p_cmd->func (cp, &b, pvar);
    return ((uint32_t)b.len);
  }

  bprintf (&b, "\r\nCommand error: %s", cmd);
  return ((uint32_t)b.len);
}

// LED command implementation
static void cmd_led (const char *par, struct buffer *b, uint32_t *state) {
  uint32_t val;

  (void)state;

  if (sscanf (par, "%x", &val) > 0) {
    vioSetSignal(0xFFU, val);
    if (LEDrun == true) {
      bprintf (b, "\r\n Running Lights OFF");
      LEDrun = false;
    }
  }
  else if (LEDrun == false) {
    bprintf (b, "\r\n Running Lights ON");
    LEDrun = true;
  }
}

// LIST command implementation
static void cmd_list (const char *par, struct buffer *b, uint32_t *state) {
  uint32_t val;

  if (*state == 0) {
    // First call to this function
    bprintf (b, ANSI_CLS);
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
      b->len |= (1u << 31);
    }
  }
  else {
    // Subsequent call to this function
    int32_t max = *state >> 16;
    int32_t ln  = *state & 0xFFFF;
    bprintf (b, "This is line # %d in syslog.\r\n", ++ln);
    if (ln < max) {
      *state = *state + 1;
      // Set request for another callback
      b->len |= (1u << 31);
    }
  }
}

// TCPSTAT command implementation
static void cmd_tcpstat (const char *par, struct buffer *b, uint32_t *state) {
  netTCP_State tcp_state;
  NET_ADDR peer;
  char ip_ascii[40];

  (void)par;

  if (*state == 0) {
    // First call to this function
    bprintf (b, ANSI_CLS);
    bprintf (b, tcpstat_header);
    *state = *state + 1;
    // Request a 2nd call
    b->len |= (1u << 31);
  }
  else {
    // Subsequent call to this function
    int32_t socket = (int32_t)*state;
    tcp_state = netTCP_GetState (socket);
    if (tcp_state == netTCP_StateINVALID) {
      // Invalid socket, we are done
      bprintf (b, "\r\n\r\n"
                  "Press any key to end!");
      // Reset state for next callback after 2 seconds
      netTELNETs_RepeatCommand (20);
      *state = 0;
      b->len |= (1u << 31);
    }
    else {
      // Column: Socket, State
      bprintf (b, "\r\n  %-6d%-13s", socket, net_tcp_ntoa(tcp_state));
      // Column: Port
      if (tcp_state >= netTCP_StateLISTEN) {
        bprintf (b, "%-6d", netTCP_GetLocalPort (socket));
      }
      // Column: Timer
      if (tcp_state > netTCP_StateLISTEN) {
        bprintf (b, "%-7d", netTCP_GetTimer (socket));
      }      
      // Column: Address, Port
      if (tcp_state > netTCP_StateLISTEN) {
        netTCP_GetPeer (socket, &peer, sizeof(peer));
        netIP_ntoa (peer.addr_type, peer.addr, ip_ascii, sizeof(ip_ascii));
        bprintf (b, "%-30s%-5d", ip_ascii, peer.port);
      }
      *state = *state + 1;
      b->len |= (1u << 31);
    }
  }
}

// RINFO command implementation
static void cmd_rinfo (const char *par, struct buffer *b, uint32_t *state) {
  NET_ADDR client;
  char ip_ascii[40];

  (void)par;
  (void)state;

  if (netTELNETs_GetClient (&client, sizeof(client)) == netOK) {
    // Convert client IP address to ASCII string
    netIP_ntoa (client.addr_type, client.addr, ip_ascii, sizeof(ip_ascii));
    bprintf (b, "\r\n IP address: %s", ip_ascii);
    bprintf (b, "\r\n TCP port  : %d", client.port);
  }
  else {
    bprintf (b, "\r\n Error!");
  }
}

// PASSW command implementation
static void cmd_passw  (const char *par, struct buffer *b, uint32_t *state) {

  (void)state;

  if (!netTELNETs_LoginActive()) {
    bprintf (b, "\r\n Authentication not enabled!");
    return;
  }
  switch (*par) {
    case '0':
      // Print password
      bprintf (b, "\r\n System Password: \"%s\"", netTELNETs_GetPassword());
      break;
    case '1':
      // Change password
      if ((strlen (par) > 2) && (netTELNETs_SetPassword (&par[2]) == netOK)) {
        bprintf (b, "\r\n OK, New Password: \"%s\"", netTELNETs_GetPassword ());
      }
      else {
        bprintf (b, "\r\n Failed to change password!");
      }
      break;
    case '2':
      // Clear password
      netTELNETs_SetPassword ("");
      bprintf (b, "\r\n OK, Password cleared");
      break;
    default:
      // Error
      bprintf (b, "\r\n Command Error");
      break;
  }
}

// HELP command implementation
static void cmd_help (const char *par, struct buffer *b, uint32_t *state) {

  (void)par;
  (void)state;

  bprintf (b, help1);
  if (netTELNETs_LoginActive()) {
    bprintf (b, help2);
  }
  bprintf (b, help3);
}

// BYE command implementation
static void cmd_bye (const char *par, struct buffer *b, uint32_t *state) {

  (void)par;
  (void)state;

  bprintf (b, "\r\n Disconnected");
  // Bit-30 is a disconnect flag
  b->len |= (1 << 30);
}

// Print formatted data to a buffer
static int32_t bprintf (struct buffer *b, const char *fmt, ...) {
  va_list args;
  int32_t n;
  
  if (b->len >= b->size) {
    return (-1);
  }
  va_start(args, fmt);
  n = vsnprintf (b->data + b->len, b->size - b->len, fmt, args);
  va_end(args);

  if (n < 0) {
    return (-1);
  }
  if (n >= b->size - b->len) {
    b->len = b->size - 1;  // truncated
    return (0);
  }
  b->len += n;
  return (0);
}
