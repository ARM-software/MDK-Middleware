/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_mapi.c
 * Purpose: File Maintenance API Functions
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "cmsis_compiler.h"

#include "fs_common.h"
#include "fs_sys.h"
#include "fs_fat.h"
#include "fs_efs.h"
#include "fs_evr.h"

/* Extern prototypes for fdelete function */
extern fsStatus _fdelete (const char *path, const char *options);
extern fsStatus _fdelete_legacy (const char *path);


/* Local Variables */
static uint8_t cfg_done = 0U;

/* Empty event recorder config function */
__WEAK void fs_config_evr (void) {
  return;
}

/**
  Configure File System component.

  This function is only executed when function finit is called for the first time.
  It initializes all file handles, configures elink cache and sets the default
  drive as the current drive.
*/
static void fs_component_init (void) {
  FS_DEV *dev;
  uint32_t i;

  if (cfg_done == 0U) {
    /* Configure Event Recorder */
    fs_config_evr();

    /* Init number of RTOS ticks in milisecond */
    fs_ms_rtos_tick = fs_get_rtos_tick_freq() / 1000;

    if (fs_ms_rtos_tick == 0U) {
      /* Prevent 0 second delay when tick is less than ms */
      fs_ms_rtos_tick = 1;
    }

    /* Init FAT file handles */
    for (i = 0; i < fs_fat_fh_cnt; i++) {
      fs_fat_fh[i].flags = 0;
    }
    /* Init EFS file handles */
    for (i = 0; i < fs_efs_fh_cnt; i++) {
      fs_efs_fh[i].flags = 0;
    }

    /* Set current drive */
    fs_set_cdrive (0);
    for (dev = &fs_DevPool[0], i = 0; i < fs_ndrv; i++, dev++) {
      if (dev->attr & FS_CUR) {
        /* Initial current drive was already defined */
        fs_set_cdrive ((int32_t)i);
        break;
      }
    }
    cfg_done = 1U;
  }
}

/**
  Retrieve the FileSystem component version.
  \return BCD encoded version: 0xMMmmbbbb (MM:Major, mm:minor, bbbb:build)
*/
uint32_t fversion (void) {
  uint32_t ver = fs_lib_version;

  EvrFsCore_fversion (ver);

  return (ver);
}


/**
  Initialize File System and drive related driver.

  The function \b finit initializes the File System and must be called
  before invoking any other File System function.
  The parameter \a drive is specifying the \ref drive to be initialized.
  The \ref def_sys_drive "Default System Drive" is used if an empty string is provided.

  \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsInvalidParameter = Input parameter invalid.
                - fsInvalidDrive     = Nonexistent drive letter specified.
                - fsDriverError      = Failed to initialize the driver.
                - fsError            = System resource create failed.
*/
fsStatus finit (const char *drive) {
  FS_DEV  *dev;
  int32_t  id;
  fsStatus status;
  uint32_t drv;

  /* Initial file system component configuration */
  fs_component_init ();

  EvrFsCore_finit (drive);

  id = fs_drive_id (drive, NULL);
  if (id < 0) {
    status = (fsStatus)-id;
  }
  else {
    dev = &fs_DevPool[id];

    /* Get drive letter */
    drv = get_u32 ((const uint8_t *)&fs_DevPool[id].id);

    if (dev->attr & FS_FAT) {
      /* Set drive letter */
      ((fsFAT_Volume *)dev->dcb)->DrvLet = drv;

      /* Initialize drive */
      status = fat_init((fsFAT_Volume *)dev->dcb);
    }
    else {
      /* Set drive letter */
      ((fsEFS_Volume *)dev->dcb)->DrvLet = drv;

      /* Initialize drive */
      status = efs_init((fsEFS_Volume *)dev->dcb);
    }
  }

  return (status);
}


/**
  Uninitialize File System.

  \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsInvalidParameter = Input parameter invalid.
                - fsInvalidDrive     = Nonexistent drive letter specified.
                - fsError            = System resource delete failed.
*/
fsStatus funinit (const char *drive) {
  FS_DEV  *dev;
  int32_t  id;
  fsStatus status;

  EvrFsCore_funinit (drive);

  id = fs_drive_id (drive, NULL);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    status = (fsStatus)-id;
  }
  else {
    status = fsError;

    dev = &fs_DevPool[id];

    if (dev->attr & FS_FAT) {
      /* Uninitialize FAT drive */
      status = fat_uninit ((fsFAT_Volume *)dev->dcb);
    }
    else {
      /* Uninitialize EFS drive */
      status = efs_uninit ((fsEFS_Volume *)dev->dcb);
    }
  }

  return (status);
}


/**
  \brief Mount drive.
  \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsUninitializedDrive = Media driver not initialized.
                - fsInvalidParameter = Input parameter invalid.
                - fsInvalidDrive     = Nonexistent drive letter specified.
                - fsDriverError      = Media driver operation failed.
                - fsMediaError       = Failed to initialize the media.
                - fsNoMedia          = Media device is not insterted.
                - fsNoFileSystem     = No filesystem on the volume.
*/
fsStatus fmount (const char *drive) {
  FS_DEV  *dev;
  int32_t  id;
  fsStatus retv;

  START_LOCK (fsStatus);

  EvrFsCore_fmount (drive);

  id = fs_drive_id (drive, NULL);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* Mount FAT volume */
    retv = fat_mount ((fsFAT_Volume *)dev->dcb);
  }
  else {
    /* Lock EFS volume */
    VOLUME_LOCK ((fsEFS_Volume *)dev->dcb);

    /* Mount EFS volume */
    retv = efs_mount ((fsEFS_Volume *)dev->dcb);
  }
  RETURN (retv);
  END_LOCK;
}


/**
  \brief Unmount drive.
  \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsInvalidParameter = Input parameter invalid.
                - fsInvalidDrive     = Nonexistent drive letter specified.
*/
fsStatus funmount (const char *drive) {
  FS_DEV  *dev;
  int32_t  id;
  fsStatus retv;

  START_LOCK (fsStatus);

  EvrFsCore_funmount (drive);

  id = fs_drive_id (drive, NULL);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* Unmount FAT volume */
    retv = fat_unmount((fsFAT_Volume *)dev->dcb);
  }
  else {
    /* Lock EFS volume */
    VOLUME_LOCK ((fsEFS_Volume *)dev->dcb);

    /* Unmount EFS volume */
    retv = efs_unmount((fsEFS_Volume *)dev->dcb);
  }
  RETURN (retv);
  END_LOCK;
}


/**
  \brief Find a file or directory matching search pattern.
  \param[in]  pattern                  string specifying the pattern.
                                       - May include drive prefix and the following wildcards:
                                       - "*" or "*.*" searches for all files in the directory.
                                       - "abc*"       searches for files that begin with abc.
                                       - "*.htm"      searches for files that end with .htm.
                                       - "abc*.text"  searches for files that begin with abc and that end with .text.
  \param[out] info                     structure storing information about matching files.
  \return     execution status \ref fsStatus
*/
fsStatus ffind (const char *pattern, fsFileInfo *info) {
  FS_DEV *dev;
  fsStatus retv;
  int32_t id;
  const char *p;
  int32_t  len1;
  uint32_t len2;

  START_LOCK (fsStatus);

  EvrFsCore_ffind (pattern, info);

  id = fs_drive_id (pattern, &pattern);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  len2 = 0U;
  p    = pattern;

  /* Strip out path from search pattern */
  while (pattern[len2] != '\0') {
    if ((pattern[len2] == '\\') || (pattern[len2] == '/')) {
      p = &pattern[len2+1];
    }
    len2++;
  }

  if (*p == '\0') {
    /* Invalid input (only directory path specified) */
    RETURN (fsInvalidParameter);
  }

  len1 = fs_strpos (p, '*');

  if (len1 >= 0) {
    /* Wildcard found */
    if (strcmp (&p[len1+1],".*") == 0) {
      /* Search using *.* */
      len2 = 0U;
    }
    else {
      len2  = strlen (p);
      len2 -= (uint32_t)(len1 + 1);
    }
  }

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);
  }
  else {
    /* Lock EFS volume */
    VOLUME_LOCK ((fsEFS_Volume *)dev->dcb);
  }

  for (;;) {
    if (dev->attr & FS_FAT) {
      /* Find FAT file */
      retv = fat_ffind (pattern, info, (fsFAT_Volume *)dev->dcb);
    }
    else {
      /* Find EFS file */
      retv = efs_ffind (info, (fsEFS_Volume *)dev->dcb);
    }

    if (retv != fsOK) {
      RETURN (retv);
    }

    if (len1 >= 0) {
      /* Wildcard search, do partial compare */
      if (fs_strmatch (&p[0], &info->name[0], (uint32_t)len1, len2) == 0) {
        /* Partial match */
        RETURN (fsOK);
      }
    }
    else {
      /* No wildcard, must exactly match (case insensitive) */
      if (fs_strncasecmp (&info->name[0], &p[0], strlen(info->name)) == 0) {
        /* Exact match */
        RETURN (fsOK);
      }
    }
  }

  END_LOCK;
}


/**
  \brief Delete one or more files.
  \details The function \b fdelete deletes one or more files. If path
           argument points to the existing file it is removed. If path argument
           points to the directory, all files within that directory are removed.
           
           The \a path argument is specifying the file or directory path and it can
           contail a drive prefix. If the drive prefix is omitted, the Current System
           Drive is used.
           
           The \a options argument can be NULL when options are not used or a pointer
           to a string specifying following options:
           | Option | Description |
           |   /S   | Remove all files within specified directory and all subdirectories. |

  \param[in]  path                     a string specifying the file to be deleted.
  \return     execution status \ref fsStatus
*/
fsStatus fdelete (const char *path, const char *options) {
  FS_DEV *dev;
  int32_t id;

  START_LOCK (fsStatus);

  EvrFsCore_fdelete (path, options);

  id = fs_drive_id (path, &path);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* Delete a file on FAT volume */
    RETURN (fat_delete (path, options, (fsFAT_Volume *)dev->dcb));
  }
  else {
    /* Lock EFS volume */
    VOLUME_LOCK ((fsEFS_Volume *)dev->dcb);

    /* Deleta a file on EFS volume */
    RETURN (efs_delete (path, (fsEFS_Volume *)dev->dcb));
  }

  END_LOCK;
}


/**
  \brief Rename a file or directory with given path name to a new name.
  \param[in]  path                     string specifying the file or directory path.
  \param[in]  newname                  string specifying new file or directory name.
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsInvalidPath      = Invalid path specified.
                - fsInvalidDrive     = Nonexistent drive letter specified.
                - fsAccessDenied     = File is in use.
                - fsTooManyOpenFiles = File cannot be opened due to too many opened files.
                - fsError            = Old file cannot be found or new file already exists.
*/
fsStatus frename (const char *path, const char *newname) {
  FS_DEV *dev;
  int32_t id;

  START_LOCK (fsStatus);

  EvrFsCore_frename (path, newname);

  id = fs_drive_id (path, &path);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    RETURN (fat_rename (path, newname, (fsFAT_Volume *)dev->dcb));
  }
  else {
    /* Lock EFS volume */
    VOLUME_LOCK ((fsEFS_Volume *)dev->dcb);

    RETURN (efs_rename (path, newname, (fsEFS_Volume *)dev->dcb));
  }

  END_LOCK;
}


/**
  \brief Analyse volume and check for file fragmentation.
  \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
  \return     fragmentation factor or execution status
              - value >= 0 or <= 255: fragmentation factor
              - value < 0: error occurred, -value is execution status as defined with \ref fsStatus
  \note       This function supports EFS drives only.
*/
int32_t fanalyse (const char *drive) {
  int32_t id;
  FS_DEV *dev;

  START_LOCK (int32_t);

  EvrFsCore_fanalyse (drive);

  id = fs_drive_id (drive, NULL);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN (id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Not yet supported for a FAT drive. */
    RETURN (-(int32_t)fsUnsupported);
  }
  else {
    /* Lock EFS volume */
    VOLUME_LOCK ((fsEFS_Volume *)dev->dcb);

    /* Analyse Embedded Flash drive. */
    RETURN (efs_analyse ((fsEFS_Volume *)dev->dcb));
  }

  END_LOCK;
}


/**
  \brief Analyse volume and check for allocation errors.
  \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful, no allocation errors
                - fsInvalidDrive     = Nonexistent drive letter specified.
                - fsUnsupported      = FAT drive was specified.
                - fsError            = Check failed due to allocation errors.
  \note       This function supports EFS drives only.
*/
fsStatus fcheck (const char *drive) {
  int32_t id;
  FS_DEV *dev;

  START_LOCK (fsStatus);

  EvrFsCore_fcheck (drive);

  id = fs_drive_id (drive, NULL);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Not yet supported for a FAT drive. */
    RETURN (fsUnsupported);
  }
  else {
    /* Lock EFS volume */
    VOLUME_LOCK ((fsEFS_Volume *)dev->dcb);

    /* Check file integrity on Embedded Flash drive. */
    RETURN (efs_check ((fsEFS_Volume *)dev->dcb));
  }

  END_LOCK;
}


/**
  Format drive.

  The function \b fformat formats an EFS or FAT storage media for using a file
  system. The function closes all open files on the specified \a drive. Existing
  file handles become invalid.
  The parameter \a drive is specifying the \ref drive "drive". In addition, you
  may also use "" (\ref def_sys_drive "Default System Drive").

  The parameter \a param is giving additional options:
  |Option   |Description      |
  |---------|-----------------|
  |/L label | Specifies volume label for a FAT drive. If /L option is not                |
  |         | specified or if label is not given, then volume label will not be written. |
  |/FAT32   | Formats the media for using the FAT32 file system. |
  |/W       | Clears all data. Overwrites sectors with the default value of 0xFF.|
  |/LL      | Clears all data. Performs low-level formatting first. |
  |/LLEB    | Clears all data. Performs low-level formatting and erases bad blocks first.|

  \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
  \param[in]  options                  string specifying formatting options.
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsInvalidDrive     = Nonexistent drive letter specified.
                - fsNoFileHandle     = File cannot be opened due to to many opened files.
                - fsError            = Formatting failed.
*/
fsStatus fformat (const char *drive, const char *options) {
  int32_t  id;
  FS_DEV  *dev;
  fsStatus status;

  START_LOCK (fsStatus);

  EvrFsCore_fformat (drive, options);

  id = fs_drive_id (drive, NULL);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* Format the FAT drive. */
    status = fat_format ((fsFAT_Volume *)dev->dcb, options);
  }
  else {
    /* Lock EFS volume */
    VOLUME_LOCK ((fsEFS_Volume *)dev->dcb);

    /* Format the EFS drive. */
    status = efs_format ((fsEFS_Volume *)dev->dcb);
  }
  RETURN (status);
  END_LOCK;
}


/**
  \brief Find free space on drive.
  \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
  \return     free space or execution status
                - value >= 0: free space on drive in bytes.
                - value < 0:  error occurred, -value is execution status as defined with \ref fsStatus
*/
int64_t ffree (const char *drive) {
  FS_DEV   *dev;
  int32_t   id;
  int64_t   free;

  START_LOCK (int64_t);

  EvrFsCore_ffree (drive);

  id = fs_drive_id (drive, NULL);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN (id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* Determine free space on FAT drive */
    free = fat_free ((fsFAT_Volume *)dev->dcb);
  }
  else {
    /* Lock EFS volume */
    VOLUME_LOCK ((fsEFS_Volume *)dev->dcb);

    /* Determine free space on EFS drive */
    free = efs_free ((fsEFS_Volume *)dev->dcb);
  }
  RETURN (free);

  END_LOCK;
}


/**
  \brief Defragment Embedded Flash drive.
  \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsUnsupported      = FAT drive was specified.
                - fsInvalidDrive     = Nonexistent drive letter specified.
                - fsAccessDenied     = Not all files are closed on specified drive.
                - fsNoFreeSpace      = Not enough space to complete the defragmentation.
  \note       This function supports EFS drives only.
*/
fsStatus fdefrag (const char *drive) {
  FS_DEV  *dev;
  int32_t  id;

  START_LOCK (fsStatus);

  EvrFsCore_fdefrag (drive);

  id = fs_drive_id (drive, NULL);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Not supported on FAT drive. */
    RETURN (fsUnsupported);
  }
  else {
    /* Lock EFS volume */
    VOLUME_LOCK ((fsEFS_Volume *)dev->dcb);

    /* Embedded Flash drive. */
    RETURN (efs_defrag ((fsEFS_Volume *)dev->dcb));
  }

  END_LOCK;
}


/**
  Get attributes from the parameter string:
   +  Sets an attribute
   -  Clears an attribute
   R  Read-only file attribute
   A  Archive file attribute
   S  System file attribute
   H  Hidden file attribute

  \param[in] par  Parameter string
  \return Attribute mask (clear[31:16]/set[15:0])
*/

static uint32_t get_attrib (const char *par) {
  uint32_t attr, cmd, msk;

  attr = 0;
  cmd  = 0;

  while (*par) {
    if (*par == '-' || *par == '+') {
      msk = 0;

      switch (*(par+1) & ~0x20) {
        case 'R': msk = FS_FAT_ATTR_READ_ONLY; break;
        case 'A': msk = FS_FAT_ATTR_ARCHIVE;   break;
        case 'S': msk = FS_FAT_ATTR_SYSTEM;    break;
        case 'H': msk = FS_FAT_ATTR_HIDDEN;    break;
      }

      if (msk) {
        if (cmd & msk) {
          /* We already processed current attribute, this input is invalid */
          return (0);
        }
        cmd |= msk;

        /* Create set/clear attribute mask */
        if (*par == '-') {
          msk = msk << 16;
        }
        attr |= msk;
      }
    }
    par++;
  }
  return (attr);
}


/**
  \brief Change file attributes.
  \param[in]  path                      string specifying file or directory path.
  \param[in]  attr                      string specifying file or directory attributes to be modified.
                                        The following characters are allowed within par string:
                                        - + Sets an attribute.
                                        - - Clears an attribute.
                                        - R Read-only file attribute.
                                        - A Archive file attribute.
                                        - S System file attribute.
                                        - H Hidden file attribute.
  \return     execution status \ref fsStatus
  \note       This function supports FAT drives only.
*/
fsStatus fattrib (const char *path, const char *attr) {
  int32_t  id;
  uint32_t attrib;
  FS_DEV  *dev;

  START_LOCK (fsStatus);

  EvrFsCore_fattrib (path, attr);

  id = fs_drive_id (path, &path);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  /* Get attributes from parameter string */
  attrib = get_attrib (attr);
  if (attrib == 0) {
    /* Invalid input parameter string */
    RETURN (fsInvalidParameter);
  }

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* Change FAT FS dir/file parameters */
    RETURN (fat_attrib ((fsFAT_Volume *)dev->dcb, path, attrib));
  }
  else {
    /* Not supported for EFS */
    RETURN (fsUnsupported);
  }

  END_LOCK;
}


/**
  \brief Read volume label and serial number.
  \param[in]  drive                     a string specifying the \ref drive "memory or storage device".
  \param[out] label                     buffer for storing label as a null terminated string.
                                        When volume has no label an empty string is returned.
                                        This parameter can be NULL meaning label will not be read.
  \param[out] serial                    pointer to variable where serial number will be stored.
                                        This parameter can be NULL meaning serial number will not be read.
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsInvalidDrive     = Nonexistent drive letter specified.
                - fsAccessDenied     = Trying to read label and serial from EFS drive.
                - fsDriverError      = Read/write error.
  \note       This function supports FAT drives only.
*/
fsStatus fvol (const char *drive, char *label, uint32_t *serial) {
  int32_t id;
  FS_DEV *dev;

  START_LOCK (fsStatus);

  EvrFsCore_fvol (drive, label, serial);

  id = fs_drive_id (drive, NULL);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    RETURN (fat_vol (label, serial, (fsFAT_Volume *)dev->dcb));
  }
  else {
    /* File system is EFS */
    RETURN (fsUnsupported);
  }
  END_LOCK;
}


/**
  \brief Check if media present on removable drive.
  \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsNoMedia          = Media not present.
                - fsAccessDenied     = Tried to access non-removable drive.
  \note       This function supports FAT drives only.
*/
fsStatus fmedia (const char *drive) {
  FS_DEV  *dev;
  int32_t  id;

  START_LOCK (fsStatus);

  EvrFsCore_fmedia (drive);

  id = fs_drive_id (drive, NULL);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* File system is FAT */
    RETURN (fat_media ((fsFAT_Volume *)dev->dcb));
  }
  else {
    /* File system is EFS */
    RETURN (fsUnsupported);
  }

  END_LOCK;
}


/**
  \brief Read drive information.
  \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
  \param[out] info                     drive information structure.
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsInvalidParameter = Input parameters invalid.
                - fsInvalidDrive     = Nonexistent drive letter specified.
*/
fsStatus finfo (const char *drive, fsDriveInfo *info) {
  FS_DEV *dev;
  int32_t id;

  START_LOCK (fsStatus);

  EvrFsCore_finfo (drive, info);

  id = fs_drive_id (drive, NULL);
  if (id < 0) {
    /* Nonexistent drive or invalid drive */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* File system is FAT */
    RETURN (fat_info (info, (fsFAT_Volume *)dev->dcb));
  }
  else {
    /* Lock EFS volume */
    VOLUME_LOCK ((fsEFS_Volume *)dev->dcb);

    /* File system is EFS */
    RETURN (efs_info (info, (fsEFS_Volume *)dev->dcb));
  }

  END_LOCK;
}


/**
  \brief Change working directory.

  \details Causes the directory named by the path argument to become the current working
           directory, which is the starting point for path searches for pathnames not
           beginning with '/' or '\'.

           The argument \a path is specifying the directory path and it can contain a
           \ref drive "drive prefix". If the drive prefix is omitted, the
           \ref cur_sys_drive "Current System Drive" is used.

  \param[in]  path                     string specifying directory path.
  \return     execution status \ref fsStatus
*/
fsStatus fchdir (const char *path) {
  FS_DEV *dev;
  int32_t id;

  START_LOCK (fsStatus);

  EvrFsCore_fchdir (path);

  id = fs_drive_id (path, &path);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* File system is FAT */
    RETURN (fat_chdir (path, (fsFAT_Volume *)dev->dcb));
  }
  else {
    /* File system is EFS */
    RETURN (fsUnsupported);
  }
  END_LOCK;
}


/**
  \brief Change current drive.
  
  \details The argument \a drive specifies the \ref drive to be selected as the
           \ref cur_sys_drive "Current System Drive". A NULL pointer is not
           allowed and will be rejected.

  \param[in]  drive                     a string specifying the \ref drive "memory or storage device".
  \return     execution status \ref fsStatus
*/
fsStatus fchdrive (const char *drive) {
  int32_t id;

  EvrFsCore_fchdrive (drive);

  if (drive[0] == '\0') {
    /* Empty string is not allowed */
    return (fsInvalidParameter);
  }
  else {
    id = fs_drive_id (drive, NULL);
    if (id < 0) {
      /* Nonexistent drive or invalid input */
      return ((fsStatus)(-id));
    }
  }
  return (fs_set_cdrive (id));
}


/**
  \brief Print working directory.

  \details Used to output the absolute path of the current working directory in the
           array pointed to by \a buf. The \a len argument is the size in bytes of
           the \a path array and must be at least the size of target file system
           maximum path length.
           If \a len argument is less than maximum path length, function returns with \ref
           fsInvalidParameter and content of \a path remains unchanged.

  \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
  \param[out] buf                      pointer to string buffer
  \param[in]  len                      path buffer size
  \return     execution status \ref fsStatus
*/
fsStatus fpwd (const char *drive, char *buf, uint32_t len) {
  FS_DEV *dev;
  int32_t id;

  START_LOCK (fsStatus);

  EvrFsCore_fpwd (drive, buf, len);

  id = fs_drive_id (drive, NULL);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* File system is FAT */
    RETURN (fat_pwd (buf, len, (fsFAT_Volume *)dev->dcb));
  }
  else {
    /* File system is EFS */
    RETURN (fsUnsupported);
  }
  END_LOCK;
}


/**
  \brief Create a directory.

  \details Used to make a new directory or any intermediate directories in the path.
           The argument \a path is specifying the directory path and it can contain
           a \ref drive "drive prefix". If the drive prefix is omitted, the
           \ref cur_sys_drive "Current System Drive" is used.

  \param[in]  path                      string specifying directory path.
  \return     execution status \ref fsStatus
*/
fsStatus fmkdir (const char *path) {
  int32_t id;
  FS_DEV *dev;

  START_LOCK (fsStatus);

  EvrFsCore_fmkdir (path);

  id = fs_drive_id (path, &path);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* File system is FAT */
    RETURN (fat_mkdir(path, (fsFAT_Volume *)dev->dcb));
  }
  else {
    /* File system is EFS */
    RETURN (fsUnsupported);
  }
  END_LOCK;
}

/**
  \brief Remove a directory.

  \details The function \b frmdir removes a directory if it is empty. Argument
           options is used to specify function options.
           The argument \a path is specifying the directory path. If the drive
           prefix is omitted, the \ref cur_sys_drive "Current Drive" is used.

  \param[in]  path                     string specifying directory path.
  \param[in]  options                  string specifying function options.
  \return     execution status \ref fsStatus
*/
fsStatus frmdir (const char *path, const char *options) {
  int32_t id;
  FS_DEV *dev;

  START_LOCK (fsStatus);

  EvrFsCore_frmdir (path, options);

  id = fs_drive_id (path, &path);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* File system is FAT */
    RETURN (fat_rmdir(path, options, (fsFAT_Volume *)dev->dcb));
  }
  else {
    /* File system is EFS */
    RETURN (fsUnsupported);
  }
  END_LOCK;
}

/**
  \brief Set file timestamp.

  \details The function \b ftime_set modifies the create, last access and last write timestamps.
           The argument \a path is specifying the file or directory path. If the drive
           prefix is omitted, the \ref cur_sys_drive "Current Drive" is used.
           The argument \a create is specifying the file create time and can be NULL.
           The argument \a access is specifying the file last access time and can be NULL.
           The argument \a write is specifying the file last write time and can be NULL.
           The timestamp of the corresponding argument is not modified if that argument equals to NULL.

  \note This function supports FAT drives only.
  \note FAT last access timestamp only stores date and therefore the time members (hour, min, sec)
        in the fsTime structure are ignored.

  \param[in]  path                     string specifying the file or directory path.
  \param[in]  create                   \ref fsTime structure specifying the file creation time
  \param[in]  access                   \ref fsTime structure specifying the file last access time
  \param[in]  write                    \ref fsTime structure specifying the file last write time
  \return     execution status \ref fsStatus
*/
fsStatus ftime_set (const char *path, fsTime *create, fsTime *access, fsTime *write) {
  int32_t id;
  FS_DEV *dev;

  START_LOCK (fsStatus);

  EvrFsCore_ftime_set (path, create, access, write);

  id = fs_drive_id (path, &path);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* File system is FAT */
    RETURN (fat_time_set(path, create, access, write, (fsFAT_Volume *)dev->dcb));
  }
  else {
    /* File system is EFS */
    RETURN (fsUnsupported);
  }
  END_LOCK;
}

/**
  \brief Get file timestamp.

  \details The function \b ftime_get retrieves the create, last access and last write timestamps.
           The argument \a path is specifying the file or directory path. If the drive
           prefix is omitted, the \ref cur_sys_drive "Current Drive" is used.
           The argument \a create is specifying the \ref fsTime structure address where file create time
           will be stored and can be NULL.
           The argument \a access is specifying the \ref fsTime structure address where file last access time
           will be stored and can be NULL.
           The argument \a write is specifying the \ref fsTime structure address where file last write time
           will be stored and can be NULL.
           The timestamp of the corresponding argument is not be retrieved if that argument equals to NULL.

  \note This function supports FAT drives only.
  \note FAT last access timestamp only stores date and therefore the time members (hour, min, sec)
        in the fsTime structure will be set to 0 (zero).

  \param[in]  path                     string specifying the file or directory path.
  \param[out] create                   \ref fsTime structure where file creation time will be stored
  \param[out] access                   \ref fsTime structure where file last access time will be stored
  \param[out] write                    \ref fsTime structure where file last write time will be stored
  \return     execution status \ref fsStatus
*/
fsStatus ftime_get (const char *path, fsTime *create, fsTime *access, fsTime *write) {
  int32_t id;
  FS_DEV *dev;

  START_LOCK (fsStatus);

  EvrFsCore_ftime_get (path, create, access, write);

  id = fs_drive_id (path, &path);
  if (id < 0) {
    /* Nonexistent drive or invalid input */
    RETURN ((fsStatus)-id);
  }
  dev = &fs_DevPool[id];

  if (dev->attr & FS_FAT) {
    /* Lock FAT volume */
    VOLUME_LOCK ((fsFAT_Volume *)dev->dcb);

    /* File system is FAT */
    RETURN (fat_time_get(path, create, access, write, (fsFAT_Volume *)dev->dcb));
  }
  else {
    /* File system is EFS */
    RETURN (fsUnsupported);
  }
  END_LOCK;
}
