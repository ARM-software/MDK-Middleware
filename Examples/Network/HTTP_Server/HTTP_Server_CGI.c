/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_CGI.c
 * Purpose: HTTP Server CGI Module
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

extern bool LEDrun;
extern int32_t analog_in (uint32_t ch);
extern const char *net_tcp_ntoa (netTCP_State state);

// Local variables.
static uint8_t led_port;
static char lcd_text[2][20+1] = { "LCD line 1", "LCD line 2" };

// Script interpreter functions
static int32_t cgi_network (const char *env, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cgi_leds    (const char *env, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cgi_tcp     (const char *env, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cgi_system  (const char *env, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cgi_language(const char *env, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cgi_lcd     (const char *env, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cgi_ad      (const char *env, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cgx_ad      (const char *env, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cgx_button  (const char *env, char *buf, uint32_t buf_len, uint32_t *state);

// Process query string received by GET request.
void netCGI_ProcessQuery (const char *qstr) {
  uint8_t ip_addr[NET_ADDR_IP6_LEN];
  char var[48];

  do {
    // Loop through all the parameters
    qstr = netCGI_GetEnvVar (qstr, var, sizeof (var));

    if (var[0] == 0) continue;

    if      (strncmp (var, "ip=", 3) == 0) {
      // IPv4 address
      if (netIP_aton (&var[3], NET_ADDR_IP4, ip_addr)) {
        netIF_SetOption (NET_IF_CLASS_ETH, netIF_OptionIP4_Address,
                                           ip_addr, NET_ADDR_IP4_LEN);
      }
    }
    else if (strncmp (var, "ip6=", 4) == 0) {
      // IPv6 address
      if (netIP_aton (&var[4], NET_ADDR_IP6, ip_addr)) {
        netIF_SetOption (NET_IF_CLASS_ETH, netIF_OptionIP6_StaticAddress,
                                           ip_addr, NET_ADDR_IP6_LEN);
      }
    }
    else if (strncmp (var, "msk=", 4) == 0) {
      // Network mask
      if (netIP_aton (&var[4], NET_ADDR_IP4, ip_addr)) {
        netIF_SetOption (NET_IF_CLASS_ETH, netIF_OptionIP4_SubnetMask,
                                           ip_addr, NET_ADDR_IP4_LEN);
      }
    }
    else if (strncmp (var, "gw=", 3) == 0) {
      // Default Gateway
      if (netIP_aton (&var[3], NET_ADDR_IP4, ip_addr)) {
        netIF_SetOption (NET_IF_CLASS_ETH, netIF_OptionIP4_DefaultGateway,
                                           ip_addr, NET_ADDR_IP4_LEN);
      }
    }
    else if (strncmp (var, "gw6=", 4) == 0) {
      // Default Gateway v6
      if (netIP_aton (&var[4], NET_ADDR_IP6, ip_addr)) {
        netIF_SetOption (NET_IF_CLASS_ETH, netIF_OptionIP6_DefaultGateway,
                                           ip_addr, NET_ADDR_IP6_LEN);
      }
    }
    else if (strncmp (var, "pdns=", 5) == 0) {
      // Primary DNS server
      if (netIP_aton (&var[5], NET_ADDR_IP4, ip_addr)) {
        netIF_SetOption (NET_IF_CLASS_ETH, netIF_OptionIP4_PrimaryDNS,
                                           ip_addr, NET_ADDR_IP4_LEN);
      }
    }
    else if (strncmp (var, "pdns6=", 6) == 0) {
      // Primary DNS server v6
      if (netIP_aton (&var[6], NET_ADDR_IP6, ip_addr)) {
        netIF_SetOption (NET_IF_CLASS_ETH, netIF_OptionIP6_PrimaryDNS,
                                           ip_addr, NET_ADDR_IP6_LEN);
      }
    }
    else if (strncmp (var, "sdns=", 5) == 0) {
      // Secondary DNS server
      if (netIP_aton (&var[5], NET_ADDR_IP4, ip_addr)) {
        netIF_SetOption (NET_IF_CLASS_ETH, netIF_OptionIP4_SecondaryDNS,
                                           ip_addr, NET_ADDR_IP4_LEN);
      }
    }
    else if (strncmp (var, "sdns6=", 6) == 0) {
      // Secondary DNS server v6
      if (netIP_aton (&var[6], NET_ADDR_IP6, ip_addr)) {
        netIF_SetOption (NET_IF_CLASS_ETH, netIF_OptionIP6_SecondaryDNS,
                                           ip_addr, NET_ADDR_IP6_LEN);
      }
    }
  } while (qstr);
}

// Process data received by POST request.
// Type code: - 0 = www-url-encoded form data.
//            - 1 = filename for file upload (null-terminated string).
//            - 2 = file upload raw data.
//            - 3 = end of file upload (file close requested).
//            - 4 = any XML encoded POST data (single or last stream).
//            - 5 = the same as 4, but with more XML data to follow.
void netCGI_ProcessData (uint8_t code, const char *data, uint32_t len) {
  char var[40],passw[12];

  if (code != 0) {
    // Ignore all other codes
    return;
  }

  led_port = 0;
  LEDrun   = true;
  if (len == 0) {
    // No data or all items (radio, checkbox) are off
    vioSetSignal(0xFFU, led_port);
    return;
  }
  passw[0] = 1;
  do {
    // Loop through all the parameters
    data = netCGI_GetEnvVar (data, var, sizeof (var));

    if (var[0] == 0) continue;

    if (strcmp (var, "led0=on") == 0) {
      led_port |= 0x01;
    }
    else if (strcmp (var, "led1=on") == 0) {
      led_port |= 0x02;
    }
    else if (strcmp (var, "led2=on") == 0) {
      led_port |= 0x04;
    }
    else if (strcmp (var, "led3=on") == 0) {
      led_port |= 0x08;
    }
    else if (strcmp (var, "led4=on") == 0) {
      led_port |= 0x10;
    }
    else if (strcmp (var, "led5=on") == 0) {
      led_port |= 0x20;
    }
    else if (strcmp (var, "led6=on") == 0) {
      led_port |= 0x40;
    }
    else if (strcmp (var, "led7=on") == 0) {
      led_port |= 0x80;
    }
    else if (strcmp (var, "ctrl=Browser") == 0) {
      LEDrun = false;
    }
    else if ((strncmp (var, "pw0=", 4) == 0) ||
             (strncmp (var, "pw2=", 4) == 0)) {
      // Change password, retyped password
      if (netHTTPs_LoginActive()) {
        if (passw[0] == 1) {
          strcpy (passw, var+4);
        }
        else if (strcmp (passw, var+4) == 0) {
          // Both strings are equal, change the password
          netHTTPs_SetPassword (passw);
        }
      }
    }
    else if (strncmp (var, "lcd1=", 5) == 0) {
      // LCD Module line 1 text
      strcpy (lcd_text[0], var+5);
      printf("LCD1: %s\n", lcd_text[0]);
    }
    else if (strncmp (var, "lcd2=", 5) == 0) {
      // LCD Module line 2 text
      strcpy (lcd_text[1], var+5);
      printf("LCD2: %s\n", lcd_text[1]);
    }
  } while (data);
  vioSetSignal(0xFFU, led_port);
}

// Generate dynamic web data from a script line.
uint32_t netCGI_Script (const char *env, char *buf, uint32_t buf_len, uint32_t *pcgi) {
  int32_t (*fn)(const char *env, char *buf, uint32_t buf_len, uint32_t *state);

  switch (env[0]) {
    case 'a': fn = cgi_network;  break;
    case 'b': fn = cgi_leds;     break;
    case 'c': fn = cgi_tcp;      break;
    case 'd': fn = cgi_system;   break;
    case 'e': fn = cgi_language; break;
    case 'f': fn = cgi_lcd;      break;
    case 'g': fn = cgi_ad;       break;
    case 'x': fn = cgx_ad;       break;
    case 'y': fn = cgx_button;   break;
    default:  return (0);
  }
  return ((uint32_t)fn (&env[1], buf, buf_len, pcgi));
}

// CGI-script implementation: "network.cgi"
static int32_t cgi_network (const char *env, char *buf, uint32_t buf_len, uint32_t *state) {
  uint8_t ip_addr[NET_ADDR_IP6_LEN];
  netIF_Option option;
  int16_t addr_type;
  char ip_ascii[40];
  int32_t len = 0;

  (void)buf_len;
  (void)state;

  switch (env[0]) {
    case 'l':
      // Link-local address
      option    = netIF_OptionIP6_LinkLocalAddress;
      addr_type = NET_ADDR_IP6;
      break;

    case 'i':
      // IPv4 address
      option    = netIF_OptionIP4_Address;
      addr_type = NET_ADDR_IP4;
      break;

    case 'j':
      // IPv6 address
      option    = netIF_OptionIP6_StaticAddress;
      addr_type = NET_ADDR_IP6;
      break;

    case 'm':
      // Network mask
      option    = netIF_OptionIP4_SubnetMask;
      addr_type = NET_ADDR_IP4;
      break;

    case 'g':
      // Default gateway
      option    = netIF_OptionIP4_DefaultGateway;
      addr_type = NET_ADDR_IP4;
      break;

    case 'h':
      // Default gateway v6
      option    = netIF_OptionIP6_DefaultGateway;
      addr_type = NET_ADDR_IP6;
      break;

    case 'p':
      // Primary DNS server
      option    = netIF_OptionIP4_PrimaryDNS;
      addr_type = NET_ADDR_IP4;
      break;

    case 'q':
      // Primary DNS server v6
      option    = netIF_OptionIP6_PrimaryDNS;
      addr_type = NET_ADDR_IP6;
      break;

    case 's':
      // Secondary DNS server
      option    = netIF_OptionIP4_SecondaryDNS;
      addr_type = NET_ADDR_IP4;
      break;

    case 't':
      // Secondary DNS server v6
      option    = netIF_OptionIP6_SecondaryDNS;
      addr_type = NET_ADDR_IP6;
      break;

    default:
      return (len);
  }

  ip_ascii[0] = 0;
  if (netIF_GetOption (NET_IF_CLASS_ETH, option, ip_addr, sizeof(ip_addr)) == netOK) {
    netIP_ntoa (addr_type, ip_addr, ip_ascii, sizeof(ip_ascii));
  }
  len = sprintf (buf, &env[2], ip_ascii);
  return (len);
}

// CGI-script implementation: "leds.cgi"
static int32_t cgi_leds (const char *env, char *buf, uint32_t buf_len, uint32_t *state) {
  int32_t mask, len = 0;

  (void)buf_len;
  (void)state;

  switch (env[0]) {
    case 'c':
      // Select Control
      len = sprintf (buf, &env[2], LEDrun ?     ""     : "selected",
                                   LEDrun ? "selected" :    ""     );
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      // LED CheckBoxes
      mask = 1 << (env[0] - '0');
      len  = sprintf (buf, &env[2], (led_port & mask) ? "checked" : "");
      break;
  }
  return (len);
}

// CGI-script implementation: "tcp.cgi"
static int32_t cgi_tcp (const char *env, char *buf, uint32_t buf_len, uint32_t *state) {
  netTCP_State tcp_state;
  int32_t socket = (int32_t)(*state + 1);
  int32_t len = 0;

  (void)env;
  (void)buf_len;

  tcp_state = netTCP_GetState (socket);
  if (tcp_state != netTCP_StateINVALID) {
    len  = sprintf (buf, "<tr align=\"center\">");

    // Column: Socket, State
    len += sprintf (buf+len, "<td>%d</td><td>%s</td>",socket,net_tcp_ntoa(tcp_state));

    // Column: Port
    if (tcp_state >= netTCP_StateLISTEN) {
      len += sprintf (buf+len, "<td>%d</td>", netTCP_GetLocalPort(socket));
    }
    else {
      len += sprintf (buf+len, "<td>-</td>");
    }

    // Column: Timer
    if (tcp_state > netTCP_StateLISTEN) {
      len += sprintf (buf+len, "<td>%d</td>", netTCP_GetTimer(socket));
    }
    else {
      len += sprintf (buf+len, "<td>-</td>");
    }

    // Column: Address, Port
    if (tcp_state > netTCP_StateLISTEN) {
      NET_ADDR peer;
      char ip_ascii[40];
      netTCP_GetPeer (socket, &peer, sizeof(peer));
      netIP_ntoa (peer.addr_type, peer.addr, ip_ascii, sizeof (ip_ascii));
      len += sprintf (buf+len, "<td>%s</td><td>%d</td>", ip_ascii, peer.port);
    }
    else {
      len += sprintf (buf+len, "<td>-</td><td>-</td>");
    }
    len += sprintf (buf+len, "</tr>");

    *state = *state + 1;
    len |= (1u << 31);
  }
  return (len);
}

// CGI-script implementation: "system.cgi"
static int32_t cgi_system (const char *env, char *buf, uint32_t buf_len, uint32_t *state) {
  int32_t len = 0;

  (void)buf_len;
  (void)state;

  switch (env[0]) {
    case '1':
      len = sprintf (buf, &env[2], netHTTPs_LoginActive() ? "Enabled" : "Disabled");
      break;
    case '2':
      len = sprintf (buf, &env[2], netHTTPs_GetPassword());
      break;
  }
  return (len);
}

// CGI-script implementation: "language.cgi"
static int32_t cgi_language (const char *env, char *buf, uint32_t buf_len, uint32_t *state) {
  const char *lang;
  int32_t len = 0;

  (void)buf_len;
  (void)state;

  lang = netHTTPs_GetLanguage();
  if      (strncmp (lang, "en", 2) == 0) {
    lang = "English";
  }
  else if (strncmp (lang, "de", 2) == 0) {
    lang = "German";
  }
  else if (strncmp (lang, "fr", 2) == 0) {
    lang = "French";
  }
  else if (strncmp (lang, "sl", 2) == 0) {
    lang = "Slovene";
  }
  else {
    lang = "Unknown";
  }
  len = sprintf (buf, &env[0], lang, netHTTPs_GetLanguage());
  return (len);
}

// CGI-script implementation: "lcd.cgi"
static int32_t cgi_lcd (const char *env, char *buf, uint32_t buf_len, uint32_t *state) {
  int32_t len = 0;

  (void)buf_len;
  (void)state;

  switch (env[0]) {
    case '1':
      len = sprintf (buf, &env[2], lcd_text[0]);
      break;
    case '2':
      len = sprintf (buf, &env[2], lcd_text[1]);
      break;
  }
  return (len);
}

// CGI-script implementation: "ad.cgi"
static int32_t cgi_ad (const char *env, char *buf, uint32_t buf_len, uint32_t *state) {
  static int32_t val;
  int32_t len = 0;

  (void)buf_len;
  (void)state;

  switch (env[0]) {
    case '1':
      val = analog_in (0);
      len = sprintf (buf, &env[2], val);
      break;
    case '2': {
      float v = (float)val * 3.3f / 1024;
      len = sprintf (buf, &env[2], (double)v);
    } break;
    case '3':
      val = (val * 100) / 1024;
      len = sprintf (buf, &env[2], val);
      break;
  }
  return (len);
}

// AJAX-script implementation: "ad.cgx"
static int32_t cgx_ad (const char *env, char *buf, uint32_t buf_len, uint32_t *state) {
  int32_t val, len = 0;

  (void)buf_len;
  (void)state;

  val = analog_in (0);
  len = sprintf (buf, &env[1], val);
  return (len);
}

// AJAX-script implementation: "button.cgx"
static int32_t cgx_button (const char *env, char *buf, uint32_t buf_len, uint32_t *state) {
  uint32_t mask;
  int32_t  len = 0;

  (void)buf_len;
  (void)state;

  switch (env[0]) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      // Button states: ON=true, OFF=false
      mask = 1 << (env[0] - '0');
      len  = sprintf (buf, &env[2], (vioGetSignal(0xFFU) & mask) ? "true" : "false");
      break;
  }
  return (len);
}
