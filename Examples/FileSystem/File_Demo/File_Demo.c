/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    File_Demo.c
 * Purpose: File manipulation example program
 *----------------------------------------------------------------------------*/

#include <stdio.h>                      // Standard I/O .h-file
#include <ctype.h>                      // Character functions
#include <string.h>                     // String and memory functions

#include "main.h"

#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

#include "rl_fs.h"                      // Keil::File System&MDK:CORE

/* Some ASCII control characters */
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_CNTLQ     0x11
#define ASCII_CNTLS     0x13
#define ASCII_ESC       0x1B
#define ASCII_DEL       0x7F

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1200U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

/* Command definitions structure. */
typedef struct scmd {
  char val[8];
  void (*func)(char *par);
} SCMD;

/* Command function prototypes */
static void cmd_capture (char *par);
static void cmd_type    (char *par);
static void cmd_rename  (char *par);
static void cmd_copy    (char *par);
static void cmd_delete  (char *par);
static void cmd_dir     (char *par);
static void cmd_format  (char *par);
static void cmd_help    (char *par);
static void cmd_fill    (char *par);

static const char intro[] =
  "\n\n\n\n\n\n\n\n"
  "+-----------------------------------------------------------------------+\n"
  "|                 FileSystem File Manipulation example                  |\n";
static const char help[] =
  "+ command ------------------+ function ---------------------------------+\n"
  "| CAP \"fname\" [/A]          | captures serial data to a file            |\n"
  "|                           |  [/A option appends data to a file]       |\n"
  "| FILL \"fname\" [nnnn]       | create a file filled with text            |\n"
  "|                           |  [nnnn - number of lines, default=1000]   |\n"
  "| TYPE \"fname\"              | displays the content of a text file       |\n"
  "| REN \"fname1\" \"fname2\"     | renames a file 'fname1' to 'fname2'       |\n"
  "| COPY \"fin\" [\"fin2\"] \"fout\"| copies a file 'fin' to 'fout' file        |\n"
  "|                           |  ['fin2' option merges 'fin' and 'fin2']  |\n"
  "| DEL \"fname\"               | deletes a file                            |\n"
  "| DIR [\"mask\"]              | displays a list of files in the directory |\n"
  "| FORMAT \"drive\" [options]  | formats the drive                         |\n"
  "|                           | [/FAT32 option selects FAT32 file system] |\n"
  "| HELP  or  ?               | displays this help                        |\n"
  "+---------------------------+-------------------------------------------+\n";

static const SCMD cmd[] = {
  { "CAP",    cmd_capture },
  { "TYPE",   cmd_type    },
  { "REN",    cmd_rename  },
  { "COPY",   cmd_copy    },
  { "DEL",    cmd_delete  },
  { "DIR",    cmd_dir     },
  { "FORMAT", cmd_format  },
  { "HELP",   cmd_help    },
  { "FILL",   cmd_fill    },
  { "?",      cmd_help    }
};

#define CMD_COUNT   (sizeof (cmd) / sizeof (cmd[0]))

/* Line buffer */
static char line_buf[160];

static uint32_t fs_getline (char *line, int32_t n)  {
  int32_t cnt = 0;
  char ch;

  do {
    ch = (char)getchar();

    if ((ch == ASCII_BS) || (ch == ASCII_DEL)) {
      /* Remove previous character from the line buffer */
      if (cnt == 0) {
        /* Line empty, nothing to do */
        break;
      }
      /* Decrement number of characters and line pointer */
      cnt--;
      line--;
      /* Echo backspace */
      putchar(ASCII_BS);
      putchar(' ');
      putchar(ASCII_BS);
      fflush(stdout);
    }
    else if (ch == ASCII_ESC) {
      /* ESC - stop editing line */
      cnt = 0;
      break;
    }
    else {
      if ((ch != ASCII_CNTLQ) && (ch != ASCII_CNTLS)) {
        if (ch == ASCII_CR) {
          /* CR - stop editing line */
          *line = ch;
          /* Increment line pointer and number of characters */
          line++;
          cnt++;
          /* Add LF */
          ch = ASCII_LF;
        }
        /* Echo and store character */
        putchar(*line = ch);
        fflush(stdout);
        /* Increment line pointer and number of characters */
        line++;
        cnt++;
      }
    }
  } while ((cnt < (n - 2)) && (ch != ASCII_LF));
  /* Add NUL terminator */
  *line = 0;

  return (cnt);
}
/*-----------------------------------------------------------------------------
 *        Extract drive specification from the input string
 *----------------------------------------------------------------------------*/
static char *get_drive (char *src, char *dst, uint32_t dst_sz) {
  uint32_t i, n;

  i = 0;
  n = 0;
  while (!n && src && src[i] && (i < dst_sz)) {
    dst[i] = src[i];

    if (dst[i] == ':') {
      n = i + 1;
    }
    i++;
  }
  if (n == dst_sz) {
    n = 0;
  }
  dst[n] = '\0';

  return (src + n);
}

/*-----------------------------------------------------------------------------
 *        Process input string for long or short name entry
 *----------------------------------------------------------------------------*/
static char *get_entry (char *cp, char **pNext) {
  char *sp, lfn = 0, sep_ch = ' ';

  if (cp == NULL) {                           /* skip NULL pointers           */
    *pNext = cp;
    return (cp);
  }

  for ( ; *cp == ' ' || *cp == '\"'; cp++) {  /* skip blanks and starting  "  */
    if (*cp == '\"') { sep_ch = '\"'; lfn = 1; }
    *cp = 0;
  }
 
  for (sp = cp; *sp != ASCII_CR && *sp != ASCII_LF && *sp != 0; sp++) {
    if ( lfn && *sp == '\"') break;
    if (!lfn && *sp == ' ' ) break;
  }

  for ( ; *sp == sep_ch || *sp == ASCII_CR || *sp == ASCII_LF; sp++) {
    *sp = 0;
    if ( lfn && *sp == sep_ch) { sp ++; break; }
  }

  *pNext = (*sp) ? sp : NULL;                 /* next entry                   */
  return (cp);
}

/*-----------------------------------------------------------------------------
 *        Print size in dotted format
 *----------------------------------------------------------------------------*/
static void dot_format (uint64_t val, char *sp) {

  if (val >= (uint64_t)1e12) {
    sp += sprintf(sp,"%d.",(uint32_t)(val/(uint64_t)1e12));
    val %= (uint64_t)1e12;
    sp += sprintf(sp,"%03d.",(uint32_t)(val/(uint64_t)1e9));
    val %= (uint64_t)1e9;
    sp += sprintf(sp,"%03d.",(uint32_t)(val/(uint64_t)1e6));
    val %= (uint64_t)1e6;
    sprintf(sp,"%03d.%03d",(uint32_t)(val/1000),(uint32_t)(val%1000));
    return;
  }
  if (val >= (uint64_t)1e9) {
    sp += sprintf(sp,"%d.",(uint32_t)(val/(uint64_t)1e9));
    val %= (uint64_t)1e9;
    sp += sprintf(sp,"%03d.",(uint32_t)(val/(uint64_t)1e6));
    val %= (uint64_t)1e6;
    sprintf(sp,"%03d.%03d",(uint32_t)(val/1000),(uint32_t)(val%1000));
    return;
  }
  if (val >= (uint64_t)1e6) {
    sp += sprintf(sp,"%d.",(uint32_t)(val/(uint64_t)1e6));
    val %= (uint64_t)1e6;
    sprintf(sp,"%03d.%03d",(uint32_t)(val/1000),(uint32_t)(val%1000));
    return;
  }
  if (val >= 1000) {
    sprintf(sp,"%d.%03d",(uint32_t)(val/1000),(uint32_t)(val%1000));
    return;
  }
  sprintf(sp,"%d",(uint32_t)(val));
}

/*-----------------------------------------------------------------------------
 *        Capture serial data to file
 *----------------------------------------------------------------------------*/
static void cmd_capture (char *par) {
  char *fname, *next;
  bool  append;
  uint32_t cnt;
  FILE *f;

  fname = get_entry(par, &next);
  if (fname == NULL) {
    printf("\nFilename missing.\n");
    return;
  }
  append = false;
  if (next) {
    par = get_entry(next, &next);
    if ((strcmp(par, "/A") == 0) ||(strcmp(par, "/a") == 0)) {
      append = true;
    }
    else {
      printf("\nCommand error.\n");
      return;
    }
  }
  printf((append) ? "\nAppend data to file %s" :
                    "\nCapture data to file %s", fname);
  printf("\nPress ESC to stop.\n");
  f = fopen(fname,append ? "a" : "w");  /* open a file for writing            */
  if (f == NULL) {
    printf("\nCan not open file!\n");   /* error when trying to open file     */
    return;
  }
  do {
    cnt = fs_getline(line_buf, sizeof (line_buf));
    fputs(line_buf, f);
  } while (cnt > 0U);
  fclose(f);                            /* close the output file              */
  printf("\nFile closed.\n");
}

/*-----------------------------------------------------------------------------
 *        Create a file and fill it with some text
 *----------------------------------------------------------------------------*/
static void cmd_fill (char *par) {
  char *fname, *next;
  FILE *f;
  int i,cnt = 1000;

  fname = get_entry(par, &next);
  if (fname == NULL) {
    printf("\nFilename missing.\n");
    return;
  }
  if (next) {
    par = get_entry(next, &next);
    if (sscanf(par,"%d", &cnt) == 0) {
      printf("\nCommand error.\n");
      return;
    }
  }

  f = fopen(fname, "w");                /* open a file for writing            */
  if (f == NULL) {
    printf("\nCan not open file!\n");   /* error when trying to open file     */
    return;
  } 
  for (i = 0; i < cnt; i++)  {
    fprintf(f, "This is line # %d in file %s\n", i, fname);
    if (!(i & 0x3FF)) printf("."); fflush(stdout);
  }
  fclose(f);                            /* close the output file              */
  printf("\nFile closed.\n");
}

/*-----------------------------------------------------------------------------
 *        Read file and dump it to serial window
 *----------------------------------------------------------------------------*/
static void cmd_type (char *par) {
  char *fname,*next;
  FILE *f;
  int ch;

  fname = get_entry(par, &next);
  if (fname == NULL) {
    printf("\nFilename missing.\n");
    return;
  }
  printf("\nRead data from file %s\n",fname);
  f = fopen(fname,"r");                 /* open the file for reading          */
  if (f == NULL) {
    printf("\nFile not found!\n");
    return;
  }
 
  while ((ch = fgetc(f)) != EOF) {      /* read the characters from the file  */
    putchar(ch);                        /* and write them on the screen       */
  }
  fclose(f);                            /* close the input file when done     */
  printf("\nFile closed.\n");
}

/*-----------------------------------------------------------------------------
 *        Rename a File
 *----------------------------------------------------------------------------*/
static void cmd_rename (char *par) {
  char *fname,*fnew,*next,dir;

  fname = get_entry(par, &next);
  if (fname == NULL) {
    printf("\nFilename missing.\n");
    return;
  }
  fnew = get_entry(next, &next);
  if (fnew == NULL) {
    printf("\nNew Filename missing.\n");
    return;
  }
  if (strcmp(fname,fnew) == 0) {
    printf("\nNew name is the same.\n");
    return;
  }

  dir = 0;
  if (*(fname + strlen(fname) - 1) == '\\') {
    dir = 1;
  }

  if (frename(fname, fnew) == fsOK) {
    if (dir) {
      printf("\nDirectory %s renamed to %s\n",fname,fnew);
    }
    else {
      printf("\nFile %s renamed to %s\n",fname,fnew);
    }
  }
  else {
    if (dir) {
      printf("\nDirectory rename error.\n");
    }
    else {
      printf("\nFile rename error.\n");
    }
  }
}

/*-----------------------------------------------------------------------------
 *        Copy a File
 *----------------------------------------------------------------------------*/
static void cmd_copy (char *par) {
  char *fname,*fnew,*fmer,*next;
  FILE *fin,*fout;
  uint32_t cnt,total;
  static char buf[512];
  bool merge;

  fname = get_entry(par, &next);
  if (fname == NULL) {
    printf("\nFilename missing.\n");
    return;
  }
  fmer = get_entry(next, &next);
  if (fmer == NULL) {
    printf("\nNew Filename missing.\n");
    return;
  }
  fnew = get_entry(next, &next);
  if (fnew != NULL) {
    merge = true;
  }
  else {
    merge = false;
    fnew = fmer;
  }
  if ((strcmp(fname,fnew) == 0) || (merge && strcmp(fmer,fnew) == 0)) {
    printf("\nNew name is the same.\n");
    return;
  }

  fin = fopen(fname,"r");               /* open the file for reading          */
  if (fin == NULL) {
    printf("\nFile %s not found!\n",fname);
    return;
  }

  if (merge == false) {
    printf("\nCopy file %s to %s\n",fname,fnew);
  }
  else {
    printf("\nCopy file %s, %s to %s\n",fname,fmer,fnew);
  }
  fout = fopen(fnew,"w");               /* open the file for writing          */
  if (fout == NULL) {
    printf("\nFailed to open %s for writing!\n",fnew);
    fclose(fin);
    return;
  }

  total = 0;
  while ((cnt = fread(&buf, 1, 512, fin)) != 0) {
    fwrite(&buf, 1, cnt, fout);
    total += cnt;
  }
  fclose(fin);                          /* close input file when done         */

  if (merge == true) {
    fin = fopen(fmer,"r");              /* open the file for reading          */
    if (fin == NULL) {
      printf("\nFile %s not found!\n",fmer);
    }
    else {
      while ((cnt = fread(&buf, 1, 512, fin)) != 0) {
        fwrite(&buf, 1, cnt, fout);
        total += cnt;
      }
      fclose(fin);
    }
  }
  fclose(fout);
  dot_format(total, &buf[0]);
  printf("\n%s bytes copied.\n", &buf[0]);
}

/*-----------------------------------------------------------------------------
 *        Delete a File
 *----------------------------------------------------------------------------*/
static void cmd_delete (char *par) {
  char *fname, *next;
  fsStatus status;

  fname = get_entry(par, &next);
  if (fname == NULL) {
    printf("\nFilename missing.\n");
    return;
  }
  status = fdelete(fname, next);

  if (status == fsOK) {
    printf("\nDelete successful.\n");
  }
  else if (status == fsFileNotFound) {
    printf("\nFile %s not found.\n",fname);
  }
  else {
    printf("\nDelete failed.\n");
  }
}

/*-----------------------------------------------------------------------------
 *        Print a Directory
 *----------------------------------------------------------------------------*/
static void cmd_dir (char *par) {
  int64_t  free;
  uint64_t fsize;
  uint32_t files,dirs,i;
  char temp[32],*mask,*next,ch;
  fsFileInfo info;

  mask = get_entry(par, &next);
  if (mask == NULL) {
    mask = "*.*";
  } else if ((mask[1] == ':') && (mask[2] == 0)) {
    mask[2] = '*'; 
    mask[3] = '.'; 
    mask[4] = '*'; 
    mask[5] = 0; 
  } else if ((mask[2] == ':') && (mask[3] == 0)) {
    mask[3] = '*'; 
    mask[4] = '.'; 
    mask[5] = '*'; 
    mask[6] = 0; 
  }

  printf("\nFile System Directory...");
  files = 0;
  dirs  = 0;
  fsize = 0;
  info.fileID  = 0;
  while (ffind(mask,&info) == fsOK) {
    if (info.attrib & FS_FAT_ATTR_DIRECTORY) {
      i = 0;
      while (strlen((const char *)info.name+i) > 41) {
        ch = info.name[i+41];
        info.name[i+41] = 0;
        printf("\n%-41s", &info.name[i]);
        info.name[i+41] = ch;
        i += 41;
      }
      printf("\n%-41s    <DIR>       ", &info.name[i]);
      printf("  %02d.%02d.%04d  %02d:%02d",
              info.time.day, info.time.mon, info.time.year,
              info.time.hr, info.time.min);
      dirs++;
    }
    else {
      dot_format(info.size, &temp[0]);
      i = 0;
      while (strlen((const char *)info.name+i) > 41) {
        ch = info.name[i+41];
        info.name[i+41] = 0;
        printf("\n%-41s", &info.name[i]);
        info.name[i+41] = ch;
        i += 41;
      }
      printf("\n%-41s %14s ", &info.name[i], temp);
      printf("  %02d.%02d.%04d  %02d:%02d",
              info.time.day, info.time.mon, info.time.year,
              info.time.hr, info.time.min);
      fsize += info.size;
      files++;
    }
  }
  if (info.fileID == 0) {
    printf("\nNo files...");
  }
  else {
    dot_format(fsize, &temp[0]);
    printf("\n              %9d File(s)    %21s bytes", files, temp);
  }
  free = ffree(mask);
  if (free >= 0) {
    dot_format((uint64_t)free, &temp[0]);
    if (dirs) {
      printf("\n              %9d Dir(s)     %21s bytes free.\n", dirs, temp);
    }
    else {
      printf("\n%56s bytes free.\n",temp);
    }
  }
}

/*-----------------------------------------------------------------------------
 *        Format Device
 *----------------------------------------------------------------------------*/
static void cmd_format (char *par) {
  char  label[12];
  char  drive[4];
  int   retv;

  par = get_drive(par, drive, 4);

  printf("\nProceed with Format [Y/N]\n");
  retv = getchar();
  if (retv == 'y' || retv == 'Y') {
    /* Format the drive */
    if (fformat(drive, par) == fsOK) {
      printf("Format completed.\n");
      if (fvol(drive, label, NULL) == 0) {
        if (label[0] != '\0') {
          printf("Volume label is \"%s\"\n", label);
        }
      }
    }
    else {
      printf("Formatting failed.\n");
    }
  }
  else {
    printf("Formatting canceled.\n");
  }
}

/*-----------------------------------------------------------------------------
 *        Display Command Syntax help
 *----------------------------------------------------------------------------*/
static void cmd_help (char *par) {
  (void)par;

  printf(help);
}

/*-----------------------------------------------------------------------------
 *        Initialize a Flash Memory Card
 *----------------------------------------------------------------------------*/
static void init_filesystem (void) {
  fsStatus stat;

  printf("Initializing and mounting drive M0...\n\n");

  stat = finit("M0:");
  if (stat == fsOK) {
    stat = fmount("M0:");
    if (stat == fsOK) {
      printf("Drive M0 ready!\n");
      /* Set it as current drive */
      fchdrive("M0:");
    }
    else if (stat == fsNoFileSystem) {
      /* Format the drive */
      printf("Drive M0 not formatted!\n");
      cmd_format("M0:");
    }
    else {
      printf("Drive M0 mount failed with error code %d\n", stat);
    }
  }
  else {
    printf("Drive M0 initialization failed!\n");
  }

  printf("\nDone!\n");
}

/*-----------------------------------------------------------------------------
 *        Application main thread 
 *----------------------------------------------------------------------------*/
__NO_RETURN void app_main_thread (void *argument) {
  char *sp,*cp,*next;
  uint32_t i;

  (void)argument;

  /* Display example intro */
  printf(intro);
  printf(help);

  init_filesystem();

  while (1) {
    /* Display command prompt */
    printf("\nCmd> ");
    fflush(stdout);

    /* Get input from the stdin */
    if (fs_getline(line_buf, sizeof (line_buf)) > 0U) {
      /* Read the line content */
      sp = get_entry(&line_buf[0], &next);

      if (*sp != 0) {
        /* Convert command to upper case */
        for (cp = sp; *cp && *cp != ' '; cp++) {
          *cp = (char)toupper(*cp);
        }

        /* Check if command exists */
        for (i = 0; i < CMD_COUNT; i++) {
          if (strcmp(sp, (const char *)&cmd[i].val) == 0) {
            /* Execute command */
            cmd[i].func(next);
            break;
          }
        }
        if (i == CMD_COUNT) {
          /* Command not found */
          printf("\nCommand error\n");
        }
      }
    }
  }
}

/*-----------------------------------------------------------------------------
 *        Application main function
 *----------------------------------------------------------------------------*/
int app_main (void) {
  osKernelInitialize();
  osThreadNew(app_main_thread, NULL, &app_main_attr);
  osKernelStart();
  return 0;
}
