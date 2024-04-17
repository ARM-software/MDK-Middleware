/*------------------------------------------------------------------------------
 * MDK Middleware
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Getline.c
 * Purpose: Line Edited Character Input
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdbool.h>
#include "Terminal.h"

/*------------------------------------------------------------------------------
 *      Line Editor
 *----------------------------------------------------------------------------*/
bool getline (char *line, int32_t n)  {
  int32_t cnt = 0;
  char c;

  do {
    c = (char)getchar();
    switch (c) {
      case CNTLQ:                           /* ignore Control S/Q             */
      case CNTLS:
        break;
      case BACKSPACE:
      case DEL:
        if (cnt == 0) {
          break;
        }
        cnt--;                              /* decrement count                */
        line--;                             /* and line pointer               */
        putchar(BACKSPACE);                 /* echo backspace                 */
        putchar(' ');
        putchar(BACKSPACE);
        fflush(stdout);
        break;
      case ESC:
        *line = 0;                          /* ESC - stop editing line        */
        return false;
      case CR:                              /* CR - done, stop editing line   */
        *line = c;
        line++;                             /* increment line pointer         */
        cnt++;                              /* and count                      */
        c = LF;
      default:
        putchar(*line = c);                 /* echo and store character       */
        fflush(stdout);
        line++;                             /* increment line pointer         */
        cnt++;                              /* and count                      */
        break;
    }
  } while (cnt < n - 2  &&  c != LF);       /* check limit and CR             */

  *line = 0;                                /* mark end of string             */
  
  return true;
}
