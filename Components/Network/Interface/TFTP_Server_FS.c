/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    TFTP_Server_FS.c
 * Purpose: TFTP Server File System Interface
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "rl_net.h"

// Open a file for reading or writing on TFTP server.
void *netTFTPs_fopen (const char *fname, const char *mode) {
  return (fopen (fname, mode));
}

// Close a file previously open on TFTP server.
void netTFTPs_fclose (void *file) {
  fclose (file);
}

// Read block of data from a file on TFTP server.
uint32_t netTFTPs_fread (void *file, uint8_t *buf, uint32_t len) {
  return (fread (buf, 1, len, file));
}

// Write block of data to a file on TFTP server.
uint32_t netTFTPs_fwrite (void *file, const uint8_t *buf, uint32_t len) {
  return (fwrite (buf, 1, len, file));
}
