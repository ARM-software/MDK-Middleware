/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    File_Demo.c
 * Purpose: File manipulation example program
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cmsis_os2.h"
#include "rl_fs.h"

/* Use current drive if drive is not specified */
#ifndef FILE_DEMO_DRIVE
  #define FILE_DEMO_DRIVE ""
#endif

/* Some ASCII control characters */
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1200U)
const osThreadAttr_t app_main_attr = {
  .stack_size = APP_MAIN_STK_SZ
};

/* Command definitions structure. */
typedef struct {
  void (*func)(void);
  const char *cmd;
  const char *args;
  const char *info;
} CMD_t;

/* Command function prototypes */
static void cmd_mount   (void);
static void cmd_unmount (void);
static void cmd_format  (void);
static void cmd_write   (void);
static void cmd_read    (void);
static void cmd_delete  (void);
static void cmd_rename  (void);
static void cmd_mkdir   (void);
static void cmd_rmdir   (void);
static void cmd_find    (void);
static void cmd_pwd     (void);
static void cmd_chdir   (void);
static void cmd_chdrive (void);
static void cmd_help    (void);

static const CMD_t cmd_list[] = {
  { cmd_mount,   "MOUNT",   "drive",          "Mount the drive."},
  { cmd_unmount, "UNMOUNT", "drive",          "Unmount the drive."},
  { cmd_format,  "FORMAT",  "drive [options]","Format the drive."},
  { cmd_write,   "WRITE",   "file [n]",       "Write n lines to a file."},
  { cmd_read,    "READ",    "file [n]",       "Read n lines from a file."},
  { cmd_delete,  "DELETE",  "file [options]", "Delete a file."},
  { cmd_rename,  "RENAME",  "file",           "Rename a file."},
  { cmd_mkdir,   "MKDIR",   "path",           "Create a directory."},
  { cmd_rmdir,   "RMDIR",   "path [options]", "Remove a directory."},
  { cmd_find,    "FIND",    "pattern",        "Find a file or directory matching search pattern."},
  { cmd_pwd,     "PWD",     "drive",          "Print working directory."},
  { cmd_chdir,   "CHDIR",   "path",           "Change working directory."},
  { cmd_chdrive, "CHDRIVE", "drive",          "Change current drive."},
  { cmd_help,    "HELP",    "",               "Display help." }
};

#define CMD_LIST_SIZE   (sizeof(cmd_list) / sizeof(cmd_list[0]))

const char *fs_status[] = {
  "fsOK",
  "fsError",
  "fsUnsupported",
  "fsAccessDenied",
  "fsInvalidParameter",
  "fsInvalidDrive",
  "fsInvalidPath",
  "fsUninitializedDrive",
  "fsDriverError",
  "fsMediaError",
  "fsNoMedia",
  "fsNoFileSystem",
  "fsNoFreeSpace",
  "fsFileNotFound",
  "fsDirNotEmpty",
  "fsTooManyOpenFiles",
  "fsAlreadyExists",
  "fsNotDirectory"
};

/* Line and path buffers */
static char cmd_line[300];
static char pwd_path[260];

/**
  \brief Get a command line input from the stdin.
  \param[in]  buf       Pointer to buffer to store input command
  \param[in]  buf_size  The size of buffer
  \return     number of characters read
*/
static uint32_t fs_terminal (char *buf, int32_t buf_size)  {
  int32_t cnt = 0;
  char ch;

  while (cnt < (buf_size - 2)) {
    /* Read character from stdin (blocking) */
    ch = (char)getchar();

    if (ch == ASCII_BS) {
      /* Backspace: remove previous character from the line buffer */
      if (cnt != 0) {
        /* Decrement number of characters */
        cnt--;

        /* Echo backspace */
        putchar(ASCII_BS);
        putchar(' ');
        putchar(ASCII_BS);
        fflush(stdout);
      }
    }
    else if ((ch == ASCII_CR) || (ch == ASCII_LF)) {
      /* Carriage return or new line: End of line */
      break;
    }
    else if ((ch >= ' ') && (ch <= '~')) {
      /* Allowed characters: echo and store character */
      putchar(buf[cnt] = ch);
      fflush(stdout);

      /* Increment number of characters */
      cnt++;
    }
    else {
      /* Ignored characters */
    }
  }

  /* Add NUL terminator */
  buf[cnt] = '\0';

  return (cnt);
}

/**
  \brief Mount a drive.
  \details
  Command 'mount' mounts the drive specified with the 'drive' argument.
  Argument 'drive' is mandatory and specifies the drive to unmount.
  The specified drive is first initialized and after successful initialization
  also mounted.
*/
static void cmd_mount (void) {
  fsStatus status;
  char *drive;

  /* Extract function argument */
  drive = strtok(NULL, " ");

  status = finit(drive);

  if (status != fsOK) {
    printf("\nDrive initialization failed!");
  }
  else {
    status = fmount (drive);
  }

  if (status != fsOK) {
    printf ("\nCommand failed (fsStatus = %s).", fs_status[status]);
  } else {
    printf ("\nDrive mounted.");
  }
}

/**
  \brief Unmount a drive.
  \details
  Command 'unmount' unmounts the drive specified with the 'drive' argument.
  Argument 'drive' is mandatory and specifies the drive to unmount.
  The specified drive is first unmounted and after successful unmount
  also uninitialized.
*/
static void cmd_unmount (void) {
  fsStatus status;
  char *drive;

  /* Extract function argument */
  drive = strtok(NULL, " ");

  status = funmount (drive);

  if (status != fsOK) {
    printf("\nDrive unmount failed!");
  }
  else {
    status = funinit(drive);
  }

  if (status != fsOK) {
    printf ("\nCommand failed (fsStatus = %s).", fs_status[status]);
  } else {
    printf ("\nDrive unmounted.");
  }
}

/**
  \brief Format a drive.
  \details
  Command 'format' formats the drive specified with the 'drive' argument.
  Argument 'drive' is mandatory and specifies the drive to format.
  Argument 'options' is optional and specifies format options such as
  FAT drive label.
*/
static void cmd_format (void) {
  fsStatus status;
  char *drive;
  char *options;

  /* Extract function arguments */
  drive   = strtok(NULL, " ");
  options = strtok(NULL, " ");

  status = fformat (drive, options);

  if (status != fsOK) {
    printf ("\nCommand failed (fsStatus = %s).", fs_status[status]);
  } else {
    printf ("\nDrive formatted.");
  }
}

/**
  \brief Write a file.
  \details
  Command 'write' writes 'n' lines to a file specified with the 'path' argument.
  Argument 'path' is mandatory and specifies a file to write.
  Argument 'n' is optional and specifies the number of lines to write to a file.
  If argument 'n' is not specified, 1000 lines are written to a file.
  The file specified with 'path' is opened in write mode and written line by line
  until 'n' lines are written.
*/
static void cmd_write (void) {
  FILE *f;
  char *file;
  char *n;
  uint32_t i, cnt;

  /* Extract function arguments */
  file = strtok(NULL, " ");
  n    = strtok(NULL, " ");

  if (n != NULL) {
    /* Convert number of lines from string to integer */
    cnt = (uint32_t)atoi(n);
  } else {
    /* Default: write 1000 lines */
    cnt = 1000U;
  }

  f = fopen(file, "w");

  if (f == NULL) {
    printf("\nCan not open file!");
  }
  else {
    for (i = 0; i < cnt; i++)  {
      fprintf(f, "This is line # %d in file %s\n", i, file);

      /* Display dot after every 1000th line during the write progress */
      if ((i % 1000) == 0) {
        printf(".");
        fflush(stdout);
      }
    }

    fclose(f);
    printf("\nFile closed.");
  }
}

/**
  \brief Read a file.
  \details
  Command 'read' reads 'n' lines from a file specified with the 'path' argument.
  Argument 'path' is mandatory and specifies a file to be read.
  Argument 'n' is optional and specifies the number of lines to read from a file.
  If argument 'n' is not specified command reads until the end of file.
  The file specified with 'path' is opened in read mode and each line is read out
  character by character and output to the console. The file is closed after 'n'
  lines has been read or if end of file is reached.
*/
static void cmd_read (void) {
  FILE *f;
  char *path;
  char *n;
  uint32_t n_cnt, n_lim;
  int ch;

  /* Extract function arguments */
  path = strtok(NULL, " ");
  n    = strtok(NULL, " ");

  if (n != NULL) {
    /* Convert number of lines from string to integer */
    n_lim = (uint32_t)atoi(n);
  } else {
    /* Default: read until EOF */
    n_lim = 0U;
  }

  f = fopen(path, "r");

  if (f == NULL) {
    printf("\nCan not open file!");
  }
  else {
    n_cnt = 0U;
    /* Start printing to a new line */
    putchar ('\n');

    while ((ch = fgetc(f)) != EOF) {
      /* Write character to the console */
      putchar(ch);

      if (ch == '\n') {
        /* Increment number of read lines */
        n_cnt++;

        if ((n_cnt - n_lim) == 0) {
          /* All of the requested lines were read out */
          break;
        }
      }
    }

    fclose(f);
    printf("\nFile closed.");
  }
}

/**
  \brief Delete one or more files.
  \details
  Command 'delete' calls function fdelete with the 'path' and 'options' arguments.
  Argument 'path' is mandatory and specifies an existing file or directory.
  Argument 'options' is optional and may specify option '/S' to remove all
  files within the specified directory including the subdirectories.
*/
static void cmd_delete (void) {
  fsStatus status;
  char *path;
  char *options;

  /* Extract function arguments */
  path    = strtok(NULL, " ");
  options = strtok(NULL, " ");

  status = fdelete (path, options);

  if (status != fsOK) {
    printf ("\nCommand failed (fsStatus = %s).", fs_status[status]);
  }
}

/**
  \brief Rename a file or directory.
  \details
  Command 'rename' calls function frename with the 'path' and 'newname' arguments which must
  be specified.
  Argument 'path' specifies file or directory to be renamed.
  Argument 'newname' specifies the new name of the file or directory specified with 'path'.
*/
static void cmd_rename (void) {
  fsStatus status;
  char *path;
  char *newname;

  /* Extract function arguments */
  path    = strtok(NULL, " ");
  newname = strtok(NULL, " ");

  status = frename (path, newname);

  if (status != fsOK) {
    printf ("\nCommand failed (fsStatus = %s).", fs_status[status]);
  }
}

/**
  \brief Create a directory.
  \details
  Command 'mkdir' calls function fmkdir with the 'path' argument which must
  be specified.
  Argument 'path' may specify directory and its subdirectories.
*/
static void cmd_mkdir (void) {
  fsStatus status;
  char *path;

  /* Extract function arguments */
  path = strtok(NULL, " ");

  status = fmkdir (path);

  if (status != fsOK) {
    printf ("\nCommand failed (fsStatus = %s).", fs_status[status]);
  }
}

/**
  \brief Remove a directory.
  \details
  Command 'rmdir' calls function frmdir with the 'path' and 'options' arguments.
  Argument 'path' is mandatory and must be specified and may specify directory
  and subdirectories.
  Argument 'options' is optional and may specify option '/S' to remove all
  directories and files within the specified directory including the directory
  itself.
*/
static void cmd_rmdir (void) {
  fsStatus status;
  char *path;
  char *options;

  /* Extract function arguments */
  path    = strtok(NULL, " ");
  options = strtok(NULL, " ");

  status = frmdir (path, options);

  if (status != fsOK) {
    printf ("\nCommand failed (fsStatus = %s).", fs_status[status]);
  }
}

/**
  \brief Find a file or directory matching search pattern.
  \details
  Command 'find' calls function ffind with the 'pattern' argument which must
  be specified.
  Argument 'pattern' can specify file or directory path or use the wildcard
  character to find the files and directories that match the specified pattern.
  After successful execution the information about file or directory that
  matches the pattern is returned into the 'info' structure and partially
  printed to the console.
*/
static void cmd_find (void) {
  fsStatus status;
  char *pattern;
  fsFileInfo info;
  uint32_t cnt = 0U;

  /* Extract function argument */
  pattern = strtok(NULL, " ");

  /* Member fileID must be set to 0 */
  info.fileID = 0;

  do {
    status = ffind(pattern, &info);
    if (status == fsOK) {

      if (cnt == 0U) {
        /* Print search output header */
        printf("\n%-5s %-12s %-17s %s", "type", "size", "date", "name");
        printf("\n%-5s %-12s %-17s %s", "----", "----", "----", "----");
      }

      /* Print type of entry and its size */
      if (info.attrib & FS_FAT_ATTR_DIRECTORY) {
        printf("\n%-5s %-12s ", "DIR", " ");
      }
      else {
        printf("\n%-5s %-12u ", "FILE", info.size);
      }
      /* Print date and time */
      printf("%02d.%02d.%04d  %02d:%02d ", info.time.day,
                                           info.time.mon,
                                           info.time.year,
                                           info.time.hr,
                                           info.time.min);
      /* Print file or directory name */
      printf("%s", info.name);

      /* Increment number of entries found */
      cnt++;
    }
  } while (status == fsOK);

  if (status == fsFileNotFound) {
    if (info.fileID == 0) {
      printf("\nNo files...");
    }
  }
  else {
    printf ("\nCommand failed (fsStatus = %s).", fs_status[status]);
  }
}

/**
  \brief Print working directory.
  \details
  Command 'pwd' calls function fpwd with the 'drive' argument which must be
  specified.
  Argument 'drive' can either specify a drive or be an empty string ("") to
  specify the current drive.
  After successful execution the current working directory path is written
  into variable 'pwd_path' and printed to the console.
*/
static void cmd_pwd (void) {
  fsStatus status;
  char *drive;

  /* Extract function argument */
  drive = strtok(NULL, " ");

  status = fpwd (drive, pwd_path, sizeof(pwd_path));

  if (status != fsOK) {
    printf ("\nCommand failed (fsStatus = %s).", fs_status[status]);
  } else {
    printf ("\n%s", pwd_path);
  }
}

/**
  \brief Change working directory.
  \details
  Command 'chdir' calls function fchdir with the 'path' argument which must
  be specified.
  Argument 'path' can specify directory and its subdirectories.
  After successful execution the directory specified by 'path' becomes the
  current working directory.
*/
static void cmd_chdir (void) {
  fsStatus status;
  char *path;

  /* Extract function argument */
  path = strtok(NULL, " ");

  status = fchdir (path);

  if (status != fsOK) {
    printf ("\nCommand failed (fsStatus = %s).", fs_status[status]);
  }
}

/**
  \brief Change current drive.
  \details
  Command 'chdrive' calls function fchdrive with the 'drive' argument which must
  be specified.
  Argument 'drive' can either specify a drive or be an empty string ("") to
  specify the current drive.
  After successful execution the drive specified by 'drive' becomes the current drive.
*/
static void cmd_chdrive (void) {
  fsStatus status;
  char *drive;

  /* Extract function argument */
  drive = strtok(NULL, " ");

  status = fchdrive (drive);

  if (status != fsOK) {
    printf ("\nCommand failed (fsStatus = %s).", fs_status[status]);
  }
}

/**
  \brief Display the list of available commands.
*/
static void cmd_help (void) {
  uint32_t i;

  printf("\nThe following commands are defined:");
  printf("\n%-8s %-16s %s", "cmd", "args", "info");
  printf("\n%-8s %-16s %s", "---", "----", "----");

  /* List all commands together with arguments and info */
  for (i = 0; i < CMD_LIST_SIZE; i++) {
    printf("\n%-8s %-16s %s", cmd_list[i].cmd, cmd_list[i].args, cmd_list[i].info);
  }
}

/**
  \brief Print out the command prompt.
*/
static void print_prompt (void) {
  fsStatus status;

  status = fpwd ("", pwd_path, sizeof(pwd_path));

  if (status == fsOK) {
    printf("\n\n%s $ ", pwd_path);
  }
  else {
    printf ("\n\n$ ");
  }
  fflush(stdout);
}

/**
  \brief Print out the FileSystem component version.
*/
static void print_version (void) {
  uint32_t fs_ver;

  fs_ver = fversion();

  printf ("\nMDK-Middleware FileSystem V%d.%d.%d\n", \
          ((fs_ver >> 28) & 0xF)*  10U + ((fs_ver >> 24) & 0xF), \
          ((fs_ver >> 20) & 0xF)*  10U + ((fs_ver >> 16) & 0xF), \
          ((fs_ver >> 12) & 0xF)*1000U +                         \
          ((fs_ver >>  8) & 0xF)* 100U +                         \
          ((fs_ver >>  4) & 0xF)*  10U + (fs_ver & 0xF));
}

/**
  \brief Initialize and mount current drive
  \details
  This function initializes, mounts and formats (if unformatted) the drive
  specified by the FILE_DEMO_DRIVE define. By default, this is the current
  drive.
*/
static void init_filesystem (void) {
  fsStatus stat;
  char *drive;
  char ch;

  print_version();

  printf("\nInitializing and mounting current drive...");

  /* Set the drive to initialize and mount */
  drive = FILE_DEMO_DRIVE;

  stat = finit(drive);

  if (stat != fsOK) {
    printf("\nError: initialization failed (fsStatus = %s).", fs_status[stat]);
  }
  else {
    stat = fmount(drive);

    if (stat == fsNoFileSystem) {
      /* Format the drive */
      printf ("\nDrive not formatted! Proceed with Format [Y/N]");

      ch = (char) getchar();

      if (ch == 'y' || ch == 'Y') {
        /* Format the drive */
        stat = fformat (drive, NULL);

        if (stat == fsOK) {
          printf ("\nDrive formatted!");
        }
        else {
          printf ("\nError: format failed (fsStatus = %s).", fs_status[stat]);
        }
      }
    }
    else {
      if (stat != fsOK) {
        printf ("\nError: mount failed (fsStatus = %s).", fs_status[stat]);
      }
    }

    if (stat == fsOK) {
      /* Drive mounted */
      printf("\nDrive ready!");

      if (drive[0] != '\0') {
        /* Make the specified drive the current drive */
        fchdrive(drive);
      }
    }
  }
}

/**
  \brief File Demo application main thread
  \details
  This thread initializes the first FileSystem drive and enters the main loop
  which is executing a simple CLI using a small list of predefined commands that
  call FileSystem API functions. Type 'help' in the terminal to display the list
  of supported commands.

  \param[in]  arg      thread argument (unused)
*/
__NO_RETURN void app_main_thread (void *argument) {
  char *cmd;
  uint32_t i;

  (void)argument;

  init_filesystem();

  while (1) {
    /* Display command prompt */
    print_prompt();

    /* Get command line input from the stdin */
    if (fs_terminal(cmd_line, sizeof (cmd_line)) > 0U) {
      /* Extract command name from the input */
      cmd = strtok(cmd_line, " ");

      /* Check the list if command exists */
      for (i = 0; i < CMD_LIST_SIZE; i++) {
        /* Compare command strings case-insensitively */
        if (strncasecmp(cmd, cmd_list[i].cmd, strlen(cmd)) == 0) {
          /* Got valid command, execute it */
          cmd_list[i].func();
          break;
        }
      }
      if (i == CMD_LIST_SIZE) {
        /* Command not found */
        printf("\nCommand error\n");
      }
    }
  }
}

/**
  \brief Application main function
  \details
  This function initializes CMSIS-RTOS2 kernel, creates application main thread
  and starts the kernel.
*/
int app_main (void) {
  osKernelInitialize();
  osThreadNew(app_main_thread, NULL, &app_main_attr);
  osKernelStart();
  return 0;
}
