/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_CGI.c
 * Purpose: HTTP Server CGI Module
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "rl_net.h"                     // Keil::Network&MDK:CORE
#include "rl_fs.h"                      // Keil::File System&MDK:CORE

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6210000)
  #pragma  clang diagnostic ignored "-Wformat-nonliteral"
  #pragma  clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

// Local variables
static char label[12] = "SD_CARD";

// Script interpreter functions
static int32_t cgi_dir    (const char *env, char *buf, uint32_t buf_len, uint32_t *state);
static int32_t cgi_format (const char *env, char *buf, uint32_t buf_len, uint32_t *state);

// Local functions
static void dot_format (uint64_t val, char *sp);


// Process query string received by GET request.
void netCGI_ProcessQuery (const char *qstr) {
  // Method not used in this example
  (void)qstr;
}

// Process data received by POST request.
// Type code: - 0 = www-url-encoded form data.
//            - 1 = filename for file upload (null-terminated string).
//            - 2 = file upload raw data.
//            - 3 = end of file upload (file close requested).
//            - 4 = any XML encoded POST data (single or last stream).
//            - 5 = the same as 4, but with more XML data to follow.
void netCGI_ProcessData (uint8_t code, const char *data, uint32_t len) {
  static FILE *file = NULL;
  char var[40];

  switch (code) {
    case 0:
      // Url encoded form data received
      if (len > 0) {
        bool do_format = false;
        do {
          // Loop through all the parameters
          data = netCGI_GetEnvVar(data, var, sizeof (var));
          if (strncmp(var, "label=", 6) == 0) {
            strcpy(&label[0], &var[6]);
          }
          else if (strcmp(var, "format=yes") == 0) {
            do_format = true;
          }
        } while (data);
        // Format SD card on request
        if (do_format) {
          sprintf(&var[0], "/L %s", label);
          if (finit("M0:") == fsOK) {
            fmount("M0:");
            fformat("M0:", var);
          }
        }
      }
      break;

    case 1:
      // Filename for file upload received
      if (len > 0) {
        const char *p;
        // Skip path information
        for (p = data; *p; p++) {
          if (*p == '\\' || *p == '/') {
            data = p + 1;
          }
        }
        // Initialize and mount SD card
        if ((finit("M0:") == fsOK) &&
            (fmount("M0:") == fsOK)) {
          file = fopen(data, "w");
        }
      }
      break;

    case 2:
      // File content data received
      if (file != NULL) {
        fwrite(data, 1, len, file);
      }
      break;

    case 3:
      // File upload finished
      if (file != NULL) {
        fclose(file);
      }
      break;

    default:
      // Ignore all other codes
      break;
  }
}

// Generate dynamic web data from a script line.
uint32_t netCGI_Script (const char *env, char *buf, uint32_t buf_len, uint32_t *pcgi) {
  int32_t (*fn)(const char *env, char *buf, uint32_t buf_len, uint32_t *state);

  switch (env[0]) {
    case 'd': fn = cgi_dir;      break;
    case 'f': fn = cgi_format;   break;
    default:  return (0);
  }
  return ((uint32_t)fn (&env[1], buf, buf_len, pcgi));
}

// CGI-script implementation: "dir.cgi"
static int32_t cgi_dir (const char *env, char *buf, uint32_t buf_len, uint32_t *state) {
  static fsFileInfo info;
  char temp[16];
  int32_t len = 0;

  (void)env;
  (void)buf_len;

  if (*state == 0) {
    // Initialize environment on first call
    info.fileID = 0;
    if (!(finit("M0:") == fsOK) ||
        !(fmount("M0:") == fsOK)) {
      // No card or failed to initialize
      return (len);
    }
  }
  // Repeat for all files, ignore folders
  *state = *state + 1;
  if (ffind("*.*", &info) == fsOK) {
    len += sprintf(buf+len, "<tr align=center><td>%d.</td>"
                            "<td align=left><a href=\"/%s\">%s</a></td>",
                            *state, info.name, info.name);
    dot_format (info.size, temp);
    len += sprintf(buf+len, "<td align=right>%s</td>"
                            "<td>%02d.%02d.%04d - %02d:%02d</td>"
                            "</tr>\r\n",
                            temp,
                            info.time.day, info.time.mon, info.time.year,
                            info.time.hr, info.time.min);
    // Bit-31 is a repeat flag
    len |= (1u << 31);
  }
  return (len);
}

// CGI-script implementation: "format.cgi"
static int32_t cgi_format (const char *env, char *buf, uint32_t buf_len, uint32_t *state) {
  int32_t len = 0;

  (void)buf_len;
  (void)state;

  switch (env[0]) {
    case '1':
      // Format label
      len += (uint32_t)sprintf(buf, &env[2], label);
      break;
  }
  return (len);
}

// Print size in dotted decimal format.
static void dot_format (uint64_t val, char *sp) {
  uint16_t group[4];
  uint32_t i;

  for (i = 0; i < 4; val /= 1000, i++) {
    group[i] = (uint16_t)(val % 1000);
  }

  if (group[3] > 0) {
    sprintf(sp,"%d.%03d.%03d.%03d",group[3],group[2],group[1],group[0]);
  }
  else if (group[2] > 0) {
    sprintf(sp,"%d.%03d.%03d",group[2],group[1],group[0]);
  }
  else if (group[1]) {
    sprintf(sp,"%d.%03d",group[1],group[0]);
  }
  else {
    sprintf(sp,"%d",group[0]);
  }
}
