/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2026 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_CGI.c
 * Purpose: HTTP Server CGI Module
 * Rev.:    V7.1.0
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "rl_net.h"                     // Keil::Network&MDK:CORE
#include "rl_fs.h"                      // Keil::File System&MDK:CORE

// Output buffer descriptor
struct buffer {
  char   *data;
  int32_t size;
  int32_t len;
};

// Local variables
static char label[12] = "SD_CARD";

// Script interpreter functions
static void cgi_dir    (const char *env, struct buffer *b, uint32_t *state);
static void cgi_format (const char *env, struct buffer *b, uint32_t *state);

// Local functions
static void fmt_size (struct buffer *b, uint64_t size);
static int32_t bprintf (struct buffer *b, const char *fmt, ...);


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
            snprintf (&label[0], sizeof(label), "%s", &var[6]);
          }
          else if (strcmp(var, "format=yes") == 0) {
            do_format = true;
          }
        } while (data);
        // Format SD card on request
        if (do_format) {
          snprintf (&var[0], sizeof(var), "/L %s", label);
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
  void (*fn)(const char *env, struct buffer *b, uint32_t *state);
  struct buffer b = {
    .data = buf,
    .size = buf_len,
    .len  = 0
  };

  switch (env[0]) {
    case 'd': fn = cgi_dir;      break;
    case 'f': fn = cgi_format;   break;
    default:  return (0);
  }
  fn (&env[1], &b, pcgi);
  return ((uint32_t)b.len);
}

// CGI-script implementation: "dir.cgi"
static void cgi_dir (const char *env, struct buffer *b, uint32_t *state) {
  static fsFileInfo info;

  (void)env;

  if (*state == 0) {
    // Initialize environment on first call
    info.fileID = 0;
    if (!(finit("M0:") == fsOK) ||
        !(fmount("M0:") == fsOK)) {
      // No card or failed to initialize
      return;
    }
  }
  // Repeat for all files, ignore folders
  *state = *state + 1;
  if (ffind("*.*", &info) == fsOK) {
    bprintf (b, "<tr align=center><td>%d.</td>"
                "<td align=left><a href=\"/%s\">%s</a></td>",
                *state, info.name, info.name);
    bprintf (b, "<td align=right>");
    fmt_size(b, info.size);
    bprintf (b, "</td>"
                "<td>%02d.%02d.%04d - %02d:%02d</td>"
                "</tr>\r\n",
                info.time.day, info.time.mon, info.time.year,
                info.time.hr, info.time.min);
    // Bit-31 is a repeat flag
    b->len |= (1u << 31);
  }
}

// CGI-script implementation: "format.cgi"
static void cgi_format (const char *env, struct buffer *b, uint32_t *state) {

  (void)state;

  switch (env[0]) {
    case '1':
      // Format label
      bprintf (b, &env[2], label);
      break;
  }
}

// Format file size with dots as thousands separators.
static void fmt_size (struct buffer *b, uint64_t size) {
  uint16_t group[4];
  uint32_t i;

  for (i = 0; i < 4; i++) {
    group[i] = (uint16_t)(size % 1000);
    size /= 1000;
  }
  
  while (i > 1 && group[i-1] == 0) i--;
  bprintf (b, "%u", group[i-1]);
  while (i-- > 1) {
    bprintf (b, ".%03u", group[i-1]);
  }
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
