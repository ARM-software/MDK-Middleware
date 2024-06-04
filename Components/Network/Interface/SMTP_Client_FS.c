/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    SMTP_Client_FS.c
 * Purpose: SMTP Client File System Interface
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "rl_net.h"

// Open a file for reading in SMTP client.
void *netSMTPc_fopen (const char *fname) {
  return (fopen (fname, "r"));
}

// Close a file previously open in SMTP client.
void netSMTPc_fclose (void *file) {
  fclose (file);
}

// Read block of data from a file in SMTP client.
uint32_t netSMTPc_fread (void *file, uint8_t *buf, uint32_t len) {
  return (fread (buf, 1, len, file));
}
