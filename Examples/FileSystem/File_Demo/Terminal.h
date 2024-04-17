/*------------------------------------------------------------------------------
 * MDK Middleware
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Terminal.h
 * Purpose: File manipulation example terminal definitions
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>

enum {BACKSPACE = 0x08,
      LF        = 0x0A,
      CR        = 0x0D,
      CNTLQ     = 0x11,
      CNTLS     = 0x13,
      ESC       = 0x1B,
      DEL       = 0x7F };

/* External functions */
extern bool getline (char *buf, int32_t bufsz);
