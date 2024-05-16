/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_time.c 
 * Purpose: File System Time Support Functions
 *----------------------------------------------------------------------------*/

#include "rl_fs.h"
#include "cmsis_compiler.h"

/**
  Callback function used to provide the current date and time to the File System.
 
  \param[out] time                     Pointer to the \ref fsTime structure.
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsError            = Failed to get the current time
  \note         This function is only supported on a FAT drive.
*/
__WEAK fsStatus fs_get_time (fsTime *time) {

  /* Modify here, add a system call to read RTC. */

  time->hr   = 12;                      /* Hours:   0 - 23      */
  time->min  = 0;                       /* Minutes: 0 - 59      */
  time->sec  = 0;                       /* Seconds: 0 - 59      */
  
  time->day  = 1;                       /* Day:     1 - 31      */
  time->mon  = 1;                       /* Month:   1 - 12      */
  time->year = 2016;                    /* Year:    1980 - 2107 */
  
  return (fsOK);
}
