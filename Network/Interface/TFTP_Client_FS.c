/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    TFTP_Client_FS.c
 * Purpose: TFTP Client File System Interface
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_TFTP_Client_FS]
#include <stdio.h>
#include "rl_net.h"

// Open local file for reading or writing in TFTP client.
__weak void *netTFTPc_fopen (const char *fname, const char *mode) {
  return (fopen (fname, mode));
}

// Close local file previously open in TFTP client.
__weak void netTFTPc_fclose (void *file) {
  fclose (file);
}

// Read block of data from local file in TFTP client.
__weak uint32_t netTFTPc_fread (void *file, uint8_t *buf, uint32_t len) {
  return (fread (buf, 1, len, file));
}

// Write block of data to local file in TFTP client.
__weak uint32_t netTFTPc_fwrite (void *file, const uint8_t *buf, uint32_t len) {
  return (fwrite (buf, 1, len, file));
}
//! [code_TFTP_Client_FS]
