/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_efs.c
 * Purpose: Embedded File System Implementation
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "cmsis_compiler.h"

#include "fs_common.h"
#include "fs_efs.h"
#include "fs_evr.h"

/* Local Variables */
static uint32_t  cbuf[CBUFLEN/4];

/* Local Function Prototypes */
static fsStatus efs_AllocBlock   (fsEFS_Handle *fh);
static uint32_t efs_mark_fileMem (fsEFS_Handle *fh);
static void     efs_mark_freeMem (fsEFS_Handle *fh);
static uint32_t efs_get_freeMem  (fsEFS_Volume *vi, uint32_t block);
static uint32_t efs_BlockInvalid (fsEFS_Volume *vi, uint32_t block);

static uint32_t ed_get_next      (fsEFS_Handle *fh);
static uint32_t ed_copy          (fsEFS_Handle *fh_s, fsEFS_Handle *fh_d);

static int32_t  efs_wr           (fsEFS_Handle *fh, const uint8_t *buf, uint32_t len);
static int32_t  efs_rd           (fsEFS_Handle *fh,       uint8_t *buf, uint32_t len);


/**
  Check EFS volume for valid status flags.

  \param[in]  status                    volume status bitmask to check
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
static fsStatus efs_vol_chk (uint32_t status, fsEFS_Volume *vol) {

  if (status & EFS_STATUS_INIT_IO) {
    if ((vol->Status & EFS_STATUS_INIT_IO) == 0) {
      /* Media driver not initialized */
      EvrFsEFS_DriveNotInitialized (vol->DrvLet);
      return (fsUninitializedDrive);
    }
  }
  
  if (status & EFS_STATUS_MOUNT) {
    if ((vol->Status & EFS_STATUS_MOUNT) == 0) {
      /* Volume is not mounted */
      EvrFsEFS_DriveNotMounted (vol->DrvLet);
      return (fsAccessDenied);
    }
  }
  return (fsOK);
}


/**
  Find and return unused EFS file handle.

  \param[in]  vol                       volume description structure
  \return     File handle id or -1 if file handle not available.
*/
__WEAK int32_t efs_handle_get (fsEFS_Volume *vol) {
  fsEFS_Handle *fh;
  int32_t i;

  for (i = 0; i < fs_efs_fh_cnt; i++) {
    fh = &fs_efs_fh[i];

    if (!(fh->flags & EFS_HANDLE_OPEN)) {
      /* Clear File Control Block */
      memset (fh, 0, sizeof (fsEFS_Handle));
      fh->vol = vol;
      return (i);
    }
  }
  /* Handle not available */
  EvrFsEFS_FileHandleUnavailable (vol->DrvLet);
  return (-1);
}


/**
  Reset all file handles on specified EFS volume.

  \param[in]  vol                       volume description structure
*/
static void efs_handles_reset (fsEFS_Volume *vol) {
  fsEFS_Handle *fh;
  uint32_t i;

  /* Reset any opened files for this drive. */
  for (i = 0; i < fs_efs_fh_cnt; i++) {
    fh = &fs_efs_fh[i];

    if (fh->vol == vol) {
      /* Invalidate file handle */
      fh->flags = 0;
    }
  }
}


/**
  Check if file opening is allowed

  This function will check all file handlers available in the pool
  and allow only multiple files opened for read. Only single active
  file handle opened in write mode is allowed per file.

  \param[in]  fh                        EFS file handle
  \return     - true: file is allowed to be opened
              - false: another active file opened in write mode exists
*/
static bool efs_open_allowed (fsEFS_Handle *fh) {
  fsEFS_Handle *ext;
  uint32_t i;

  for (i = 0, ext = &fs_efs_fh[0]; i < fs_efs_fh_cnt; i++, ext++) {
    if ((ext->flags & EFS_HANDLE_OPEN) && (ext->vol == fh->vol) && (ext != fh)) {
      /* Found another file opened on the same volume */

      if (ext->fileID == fh->fileID) {
        /* Found file handle for the same file */

        if ((ext->flags & EFS_HANDLE_WRITE) || (fh->flags & EFS_HANDLE_WRITE)) {
          /* Only multiple file handles opened for read allowed */
          EvrFsEFS_FileInUse (i);
          return (false);
        }
      }
    }
  }
  /* File can be opened, no conflict with other file handlers */
  return (true);
}


/**
  Check if any other file opened for write in given block.

  \param[in]  fh                        file handle
  \param[in]  block                     block number
  \return     - true  = another file opened in the current block
              - false = no files opened in the current block
*/
static uint32_t efs_is_busy (fsEFS_Handle *fh, uint32_t block) {
  fsEFS_Handle *ext;
  uint32_t i;

  for (i = 0, ext = &fs_efs_fh[0]; i < fs_efs_fh_cnt; i++, ext++) {
    if (ext != fh) {
      if ((ext->flags & EFS_HANDLE_WRITE) && (ext->fblock == block)) {
        /* Another File is opened for writing in this Flash Block .*/
        return (true);
      }
    }
  }
  return (false);
}


/**
  Retrieve block start address.

  \param[in]  vol                       volume description structure
  \param[in]  block                     block number
  \return     block address
*/
static uint32_t addr_of_block (fsEFS_Volume *vol, uint32_t block) {
  uint32_t addr;

  addr = vol->Drv->GetSectorAddress (block);

  return (addr);
}


/**
  Retrieve block signature address.

  Signature is written in the last 4 bytes of block (allocation area).

  \param[in]  vol                       volume description structure
  \param[in]  block                     block number
  \return     signature address
*/
static uint32_t addr_of_sign (fsEFS_Volume *vol, uint32_t block) {
  uint32_t addr;

    /* Determine signature address (at the end of the block) */
  addr  = vol->Drv->GetSectorAddress (block);
  addr += vol->Drv->GetSectorSize    (block);
  addr -= 4U;
  
  return (addr);
}


/**
  Erase flash block.

  \param[in]  vol                       volume description structure
  \param[in]  block                     block number
  \return     execution status \ref fsStatus
*/
static fsStatus block_erase (fsEFS_Volume *vol, uint32_t block) {
  fsStatus status;

  status = vol->Drv->SectorErase (block);

  if (status != fsOK) {
    EvrFsEFS_FlashEraseFailed (vol->DrvLet, block, vol->Drv->GetSectorAddress (block));
  }

  return (status);
}


/**
  Write data to flash block.

  Argument addr is always 4-byte aligned.

  \param[in]  vol                       volume description structure
  \param[in]  addr                      block address
  \param[in]  buf                       data buffer
  \param[in]  cnt                       number of bytes to write
  \return     execution status \ref fsStatus
*/
static fsStatus block_write (fsEFS_Volume *vol, uint32_t addr, const void *buf, uint32_t cnt) {
  fsStatus status;

  status = vol->Drv->DataWrite (addr, (void *)(uint32_t)buf, cnt);

  if (status != fsOK) {
    EvrFsEFS_FlashWriteFailed (vol->DrvLet, addr, buf, cnt);
  }

  return (status);
}


/**
  Read data from flash block.

  Arguments addr and buf may be unaligned.

  \param[in]  vol                       volume description structure
  \param[in]  addr                      block address
  \param[in]  buf                       data buffer
  \param[in]  cnt                       number of bytes to read
  \return     execution status \ref fsStatus
*/
static fsStatus block_read (fsEFS_Volume *vol, uint32_t addr, void *buf, uint32_t cnt) {
  fsStatus status;

  status = vol->Drv->DataRead (addr, buf, cnt);

  if (status != fsOK) {
    EvrFsEFS_FlashReadFailed (vol->DrvLet, addr, buf, cnt);
  }

  return (status);
}


/**
  Read block signature (usage information).

  \param[in]  vol                       volume description structure
  \param[in]  block                     block number
  \param[out] sign                      buffer pointer
  \param[out] fa_addr                   address of the first FALLOC after signature (can be NULL)
  \return     execution status \ref fsStatus
*/
static fsStatus sign_read (fsEFS_Volume *vol, uint32_t block, uint32_t *sign, uint32_t *fa_addr) {
  fsStatus status;
  uint32_t addr;

  addr = addr_of_sign (vol, block);

  /* Return FALLOC location */
  if (fa_addr != NULL) {
    *fa_addr = addr - sizeof(FALLOC);
  }

  status = vol->Drv->DataRead (addr, sign, 4U);

  if (status != fsOK) {
    EvrFsEFS_FlashReadFailed (vol->DrvLet, addr, sign, 4U);
  }

  return (status);
}


/**
  Write block signature (usage information).

  \param[in]  vol                       volume description structure
  \param[in]  block                     block number
  \param[out] sign                      signature value
  \return     execution status \ref fsStatus
*/
static fsStatus sign_write (fsEFS_Volume *vol, uint32_t block, uint32_t sign) {
  fsStatus status;
  uint32_t addr;

  addr = addr_of_sign (vol, block);

  status = vol->Drv->DataWrite (addr, &sign, 4U);

  if (status != fsOK) {
    EvrFsEFS_FlashWriteFailed (vol->DrvLet, addr, &sign, 4U);
  }

  return (status);
}


/**
  Read File Allocation Record

  File allocation records are located below signature, on HI addresses
  within block and are growing toward LO addresses.

  \param[in]  vol                       volume description structure
  \param[in]  addr                      address
  \param[out] fa                        file allocation description structure
  \param[out] addr_next                 address of the next FALLOC
  \return     execution status \ref fsStatus
*/
static fsStatus falloc_read (fsEFS_Volume *vol, uint32_t addr, FALLOC *fa, uint32_t *addr_next) {
  fsStatus status;

  if (addr_next != NULL) {
    *addr_next = addr - sizeof(FALLOC);
  }

  status = vol->Drv->DataRead (addr, fa, sizeof(FALLOC));

  if (status != fsOK) {
    EvrFsEFS_FlashReadFailed (vol->DrvLet, addr, fa, sizeof(FALLOC));
  }

  return (status);
}


/**
  Write File Allocation Record

  File allocation records are located below signature, on HI addresses
  within block and are growing toward LO addresses.

  \param[in]  vol                       volume description structure
  \param[in]  block                     block number
  \param[in]  offs                      offset from block start address
  \param[in]  fa                        file allocation description structure
  \return     execution status \ref fsStatus
*/
static fsStatus falloc_write (fsEFS_Volume *vol, uint32_t block, uint32_t offs, FALLOC *fa) {
  fsStatus status;
  uint32_t addr;

  addr  = vol->Drv->GetSectorAddress (block);
  addr += offs;

  status = vol->Drv->DataWrite (addr, fa, sizeof(FALLOC));

  if (status != fsOK) {
    EvrFsEFS_FlashWriteFailed (vol->DrvLet, addr, fa, sizeof(FALLOC));
  }

  return (status);
}


/**
  Search for free/unused file identification number.

  \param[in]  vol                       volume description structure
  \return     File identification number
*/
static uint16_t file_id_alloc (fsEFS_Volume *vol) {
  FALLOC fa;
  uint32_t bl, fid, addr;

  for (fid = (vol->TopID + 1) | 0x8000; fid < 0xFFFF;  ) {
    /* Scan all blocks for a given 'fid' */
    for (bl = 0; bl < vol->SectorCount; bl++) {
      /* Set address of first allocation record */
      addr = addr_of_sign (vol, bl) - 8;

      for (;;) {
        if (falloc_read (vol, addr, &fa, &addr) != fsOK) {
          return (0U);
        }
        /* If this fileID is used, go and check next one. */
        if (fa.fileID == fid) goto next;
        if (fa.end == vol->ErasedValue) break;
      }
    }
    vol->TopID = fid & 0x7FFF;
    return (vol->TopID);
next:
    fid++;
  }
  /* Error */
  return (0U);
}


/**
  Check if given name is EFS valid.

  \param[in]  path                      file path
  \return    - true if given path is EFS valid
             - false if given path is to long or contains invalid characters
*/
static const char *file_name_validate (const char *path) {
  const char *fn;
  uint32_t len;
#ifdef FS_DEBUG
  const char *p = path;
#endif

  if (path) {
    /* Skip slashes at the beginning */
    if ((*path == '\\') || (*path == '/')) { path++; }

    fn = path;

    len = 0;
    while (len < 32) {
      if (*path == '\0') {
        if (len) {
          /* Valid path */
          return (fn);
        }
        break;
      }
      else if ((*path == '\\') || (*path == '/')) { break; }

      len++;
      path++;
    }
  }
  /* Path is not valid */
  EvrFsEFS_FileNameInvalid (p, strlen(p));
  return (NULL);
}


/**
  Store file name into flash memory.

  \param[in]  handle                    file handle
  \param[in]  fname                     file name
  \return     execution status \ref fsStatus
*/
static fsStatus file_name_write (fsEFS_Handle *fh, const char *fname) {
  fsStatus stat;
  FALLOC fa;
  uint32_t i, addr;
  uint8_t  fn[32];

  /* Determine file name length and store it zero terminated */
  for (i = 0U; i < 31U; i++) {
    if (fname[i] == '\0') {
      break;
    }
    fn[i] = fname[i];
  }
  /* Add zero terminator (safety) */
  fn[i++] = '\0';

  /* Align fname length and remember the value */
  addr = (i + 3U) & ~3U;

  /* Clear remaining part of fn buffer */
  while (i < addr) {
    fn[i++] = 0U;
  }
  /* Set the actual length of file name */
  i = addr;

  /* Get flash sector address */
  addr = addr_of_block (fh->vol, fh->fblock);

  stat = block_write (fh->vol, addr + fh->fbot, fn, i);

  if (stat == fsOK) {
    /* Increment bottom address by file name length */
    fh->fbot += i;

    /* Write allocation info, flag 0x8000 is added to FALLOC associated with file name */
    fa.end    = fh->fbot;
    fa.fileID = fh->fileID | 0x8000;
    fa.index  = 0;

    stat = falloc_write (fh->vol, fh->fblock, fh->ftop, &fa);

    if (stat == fsOK) {
      /* Adjust top address to next FALLOC */
      fh->ftop -= sizeof(FALLOC);

      /* Realign bottom address to 4 bytes */
      fh->fbot = (fh->fbot + 3U) & ~3U;
    }
  }

  return (stat);
}


/**
  Low level file create function.

  \param[in]  handle                    file handle
  \param[in]  fname                     file name
  \return     execution status \ref fsStatus
*/
static fsStatus file_create (fsEFS_Handle *fh, const char *fname) {
  fsStatus stat;
  FALLOC fa;
  uint16_t bl;
  uint32_t free, fn_len;

  if (fh->fileID == 0) {
    fh->fileID = file_id_alloc (fh->vol);
    if (fh->fileID == 0) {
      /* All file IDs used, reset TopID */
      fh->vol->TopID = 0;
      fh->fileID = file_id_alloc (fh->vol);
      if (fh->fileID == 0) {
        /* All available fileIDs are used */
        return (fsError);
      }
    }
  }

  /* Determine amount of free space required to store file name */
  fn_len = strlen (fname) + 1U;

  /* Align file name length */
  fn_len = (fn_len + 3U) & ~3U;

  /* Find flash sector with enough free space */
  for (bl = 0; bl < fh->vol->SectorCount; bl++) {
    free = 0U;

    /* Read sector signature */
    stat = sign_read (fh->vol, bl, &fa.end, NULL);

    if (stat != fsOK) {
      return (stat);
    }

    if (fa.end == (fh->vol->ErasedValue ^ BlockUSED)) {
      /* Used sector found, check if any file opened for write is using it */
      if (efs_is_busy (fh, bl) == false) {
        /* Not in use, determine free space */
        free = efs_get_freeMem (fh->vol, bl);

        if (free == 0) {
          /* Out of free space, mark sector as full */
          stat = sign_write (fh->vol, bl, fh->vol->ErasedValue ^ BlockFULL);

          if (stat != fsOK) {
            return (stat);
          }
        }
      }
    }
    else {
      if (fa.end == fh->vol->ErasedValue) {
        /* Empty sector found, mark it as used */
        stat = sign_write (fh->vol, bl, fh->vol->ErasedValue ^ BlockUSED);

        if (stat != fsOK) {
          return (stat);
        }

        /* Determine free space */
        free = efs_get_freeMem (fh->vol, bl);
      }
    }

    if (free >= fn_len) {
      /* Enough free space, break the search */
      break;
    }
  }

  if (bl == fh->vol->SectorCount) {
    /* Out of free space */
    EvrFsEFS_DiskFull (fh->vol->DrvLet);
    return (fsError);
  }

  /* Store sector number */
  fh->fblock = bl;

  /* Determine free space area */
  efs_mark_freeMem (fh);

  stat = file_name_write(fh, fname);

  return (stat);
}


/**
  Low level File Find.

  \param[in]  handle                    file handle
  \param[in]  fname                     file name
  \return     execution status \ref fsStatus
*/
static fsStatus file_find (fsEFS_Handle *fh, const char *fname) {
  fsStatus stat;
  FALLOC fa;
  uint8_t buf[32];
  uint16_t bl;
  uint32_t addr, prev, fn_addr;
  
  /* Search all allocated File Blocks for a given fname */
  for (bl = 0; bl < fh->vol->SectorCount; bl++) {
    /* Read block signature */
    addr = 0U;

    stat = sign_read (fh->vol, bl, &fa.end, &addr);

    if (stat != fsOK) {
      return (stat);
    }
    else {
      if (fa.end != fh->vol->ErasedValue) {
        /* Scan allocation records within current block */
        for (prev = 0; ; prev = fa.end) {
          stat = falloc_read (fh->vol, addr, &fa, &addr);

          if (stat != fsOK) {
            return (stat);
          }
          else {
            if ((fa.fileID > 0x8000) && (fa.fileID < 0xFFFF)) {
              /* Set 4-byte aligned file name address (block.start + fa.end) */
              fn_addr = addr_of_block (fh->vol, bl) + prev;
              fn_addr = (fn_addr + 3U) & ~3U;

              block_read (fh->vol, fn_addr, &buf[0], sizeof (buf));
              buf[31] = 0;

              if (strcmp (fname, (char *)&buf[0]) == 0) {
                /* File name match */
                fh->fileID = fa.fileID & 0x7FFF;
                fh->fblock = bl;

                return (fsOK);
              }
            }
            if (fa.end == fh->vol->ErasedValue) { break; }
          }
        }
      }
    }
  }

  /* File not found */
  return (fsError);
}


/**
  Delete a file.

  \param[in]  handle                    file handle
  \return     execution status \ref fsStatus
*/
static fsStatus file_delete (fsEFS_Handle *fh) {
  fsStatus stat;
  FALLOC fa;
  uint32_t i, bl, addr, invalid, del;

  stat = fsError;
  invalid = ~fh->vol->ErasedValue;
  bl = fh->fblock;

  /* Search for file chunks identified with 'fileID' */
  for (i = 0; i < fh->vol->SectorCount; i++) {
    /* Read block signature */
    addr = 0U;

    stat = sign_read (fh->vol, bl, &fa.end, &addr);

    if (stat != fsOK) {
      break;
    }

    if (fa.end != fh->vol->ErasedValue) {
      /* Non empty sector found, set delete flag */
      del = 1U;

      if (fa.end == (fh->vol->ErasedValue ^ BlockTEMP)) {
        /* This is temp sector (defrag in progress) */
        if ((fh->flags & EFS_HANDLE_DEFRAG) == 0U) {
          /* File not owned by defrag, no delete inside temp sectors */
          del = 0U;
        }
      }
      else {
        /* This is used or full sector */
        if ((fh->flags & EFS_HANDLE_DEFRAG) != 0U) {
          /* File owned by defrag, no delete outside of temp sectors */
          del = 0U;
        }
      }

      if (del != 0U) {
        /* Invalidate all file chunks identified with fileID */
        do {
          stat = falloc_read (fh->vol, addr, &fa, &addr);
          
          if (stat == fsOK) {
            if ((fa.fileID & 0x7FFF) == fh->fileID) {
              /* Clear the fileID & index values */
              stat = block_write (fh->vol, addr + 12, &invalid, 4);
            }
          }

          if (stat != fsOK) {
            break;
          }
        } while (fa.end != fh->vol->ErasedValue);

        if (stat == fsOK) {
          /* Check if current block is invalidated */
          if (efs_BlockInvalid (fh->vol, bl) == true) {
            /* All file fragments are invalid */
            stat = block_erase (fh->vol, bl);
          }
        }

        if (stat != fsOK) {
          break;
        }
      }
    }
    bl++;
    if (bl == fh->vol->SectorCount) {
      bl = 0U;
    }
  }

  /* Re-set top used fileID */
  fh->vol->TopID = fh->fileID - 1U;

  /* Return execution status */
  return (stat);
}


/**
  Low level file rename function.
  
  - Mark old filename chunk as invalid by invalidating the fileID value
  - Create new filename chunk containing the new name

  \param[in]  handle                    file handle
  \param[in]  fname                     new name
  \return     execution status \ref fsStatus
*/
static fsStatus file_rename (fsEFS_Handle *fh, const char *fname) {
  fsStatus stat;
  FALLOC fa;
  uint32_t addr;

  stat = fsOK;

  /* Set address to the first allocation info */
  addr = addr_of_sign (fh->vol, fh->fblock) - 8;

  do {
    /* Read allocation info */
    stat = falloc_read (fh->vol, addr, &fa, &addr);
    if (stat != fsOK) {
      break;
    }

    if (fa.fileID == (fh->fileID | 0x8000)) {
      /* Found the file chunk containing fileID */
      fa.end = ~fh->vol->ErasedValue;

      /* Clear the fileID value */
      stat = block_write (fh->vol, addr + 12, &fa.end, 4);
      break;
    }
  } while (fa.end != fh->vol->ErasedValue);
  
  if (stat == fsOK) {
    /* Create new name for an existing file */
    stat = file_create (fh, fname);
  }

  /* Return execution status */
  return (stat);
}


/**
  Low level file close function.

  \param[in]  handle                    file handle
  \return     execution status \ref fsStatus
*/
static fsStatus file_close (fsEFS_Handle *fh) {
  FALLOC fa;
  fsStatus stat;

  stat = fsOK;

  if ((fh->flags & EFS_HANDLE_WALLOC) != 0) {
    /* Write File Allocation Information */
    fa.end    = fh->fbot;
    fa.fileID = fh->fileID;
    fa.index  = fh->fidx;

    stat = falloc_write (fh->vol, fh->fblock, fh->ftop, &fa);

    if (stat == fsOK) {
      /* Adjust top address to next FALLOC */
      fh->ftop -= sizeof(FALLOC);

      /* Realign bottom address to 4 bytes */
      fh->fbot = (fh->fbot + 3U) & ~3U;

      if (fh->fbot >= (fh->ftop - sizeof(FALLOC))) {
        /* Block is full */
        if ((fh->flags & EFS_HANDLE_DEFRAG) == 0U) {
          /* Update signature */
          stat = sign_write (fh->vol, fh->fblock, fh->vol->ErasedValue ^ BlockFULL);
        }
      }
    }
  }

  return (stat);
}


/**
  Determine size of a file with a given id.

  \param[in]  vol
  \param[in]  id
  \return     execution status \ref fsStatus
*/
static uint32_t file_size_get (fsEFS_Volume *vol, uint32_t block, uint32_t id) {
  FALLOC fa;
  uint32_t addr, prev;
  uint32_t i, size;

  size = 0;

  /* Search for data blocks identified with 'fileID'. */
  for (i = 0; i < vol->SectorCount; i++) {
    /* Read block signature */
    addr = 0U;

    if (sign_read (vol, block, &fa.end, &addr) != fsOK) {
      return (0U);
    }

    if (fa.end != vol->ErasedValue) {
      /* Scan allocation records within current block */
      for (prev = 0; ; prev = fa.end) {
        if (falloc_read (vol, addr, &fa, &addr) != fsOK) {
          return (0U);
        }

        if (fa.fileID == id) {
          /* All file blocks are 4-byte aligned. */
          size += (fa.end - ((prev + 3U) & ~3U));
        }

        if (fa.end == vol->ErasedValue) {
          /* Free space - end of file */
          break;
        }
      }
    }
    /* Go to next block */
    block++;

    if (block == vol->SectorCount) {
      block = 0;
    }
  }

  return (size);
}


/**
  Defragmenter file copy function.

  \param[in]  fh_s                    source file handle
  \param[in]  fh_d                    destination file handle
  \return     - true  = success
              - false = file copy failed
*/
static uint32_t ed_copy (fsEFS_Handle *fh_s, fsEFS_Handle *fh_d) {
  FALLOC fa;
  uint32_t addr, n, nr, sz;

  /* Initialize destination file handle */
  fh_d->fileID = fh_s->fileID;
  fh_d->fsize  = 0U;
  fh_d->fpos   = 0U;
  fh_d->fidx   = 0U;

  if (fh_d->fbot >= (fh_d->ftop - sizeof(FALLOC))) {
    /* Block is full */
    if (efs_AllocBlock(fh_d) != fsOK) {
      /* Out of memory */
      return (false);
    }
  }

  /* Set file name length */
  n = fh_s->ftop - fh_s->fbot;

  /* Set source block start address */
  addr = addr_of_block (fh_s->vol, fh_s->fblock);

  /* Read file name */
  if (block_read (fh_s->vol, addr + fh_s->fbot, &cbuf, n) != fsOK) {
    return (false);
  }

  /* Set destination block start address */
  addr = addr_of_block (fh_d->vol, fh_d->fblock);

  /* Write file name */
  if (block_write (fh_d->vol, addr + fh_d->fbot, &cbuf, n) != fsOK) {
    return (false);
  }

  fh_d->fbot += n;

  fa.end    = fh_d->fbot;
  fa.fileID = fh_d->fileID | 0x8000;
  fa.index  = 0;

  if (falloc_write (fh_d->vol, fh_d->fblock, fh_d->ftop, &fa) != fsOK) {
    return false;
  }

  /* Adjust top address to next FALLOC */
  fh_d->ftop -= sizeof(FALLOC);

  /* Realign bottom address to 4 bytes */
  fh_d->fbot = (fh_d->fbot + 3U) & ~3U;

  /* Copy source file content to destination */
  fh_s->ftop  = 0;
  fh_s->fbot  = 0;
  fh_s->fidx  = 0;
  fh_s->fsize = file_size_get (fh_s->vol, fh_s->fblock, fh_s->fileID);

  sz = fh_s->fsize;

  while (sz) {
    nr = sz;

    if (nr > CBUFLEN) {
      nr = CBUFLEN;
    }

    if (efs_rd (fh_s, (uint8_t *)cbuf, nr) != 0) {
      return (false);
    }

    if (efs_wr (fh_d, (uint8_t *)cbuf, nr) != 0) {
      return (false);
    }
    if (nr < CBUFLEN) {
      break;
    }

    sz -= nr;
  }

  file_close (fh_d);

  return (true);
}


/**
  Defragmenter get next file function.

  \param[in]  fh                        file handle
  \return     - true  = Found next file
              - false = Next file not found
*/
static uint32_t ed_get_next (fsEFS_Handle *fh) {
  FALLOC fa;
  uint16_t bl;
  uint32_t addr, prev;
  uint32_t fid, nid;

  /* Search for file with next higher fileID. */
  fid = 0x8000 | fh->fileID;
  nid = 0xFFFF;

  for (bl = 0; bl < fh->vol->SectorCount; bl++) {
    /* Read block signature */
    addr = 0U;

    if (sign_read (fh->vol, bl, &fa.end, &addr) != fsOK) {
      return (false);
    }

    if ((fa.end != fh->vol->ErasedValue) && (fa.end != (fh->vol->ErasedValue ^ BlockTEMP))) {
      /* Block is used and is not temporary */
      for (prev = 0; ; prev = fa.end) {
        if (falloc_read (fh->vol, addr, &fa, &addr) != fsOK) {
          return (false);
        }

        if (fa.end == fh->vol->ErasedValue) { break; }

        if ((fa.fileID > fid) && (fa.fileID < nid)) {
          /* Store next higher fileID */
          nid = fa.fileID;

          /* Set handle pointers to file name chunk */
          fh->fblock = bl;
          fh->fbot   = (prev + 3U) & ~3U;
          fh->ftop   = fa.end;

          if (nid == fid+1) {
            goto x;
          }
        }
      }
    }
  }
  if (nid == 0xFFFF) {
    /* Next file not found. */
    return (false);
  }

x:fh->fileID = nid & 0x7FFF;
  fh->fidx   = 0;
  fh->fpos   = 0;

  return (true);
}


/**
  Allocate next available Block.

  \param[in]  fh                        file handle
  \return     - true  = block allocated
              - false = failed to allocate the block
*/
static fsStatus efs_AllocBlock (fsEFS_Handle *fh) {
  fsStatus stat;
  uint16_t bl;
  uint32_t i, addr, sign;

  bl = fh->fblock;

  /* Search for file blocks identified with 'fileID' */
  for (i = 0; i < fh->vol->SectorCount; i++) {
    /* Read block signature */
    addr = 0U;

    stat = sign_read (fh->vol, bl, &sign, &addr);

    if (stat != fsOK) {
      return (stat);
    }

    if (sign == fh->vol->ErasedValue) {
      /* Allocate empty block */
      if ((fh->flags & EFS_HANDLE_DEFRAG) == 0U) {
        /* Application owned files reside in regular blocks */
        stat = sign_write (fh->vol, bl, fh->vol->ErasedValue ^ BlockUSED);
      }
      else {
        /* Defragmenter owned files reside in temp blocks */
        stat = sign_write (fh->vol, bl, fh->vol->ErasedValue ^ BlockTEMP);
      }

      fh->fblock = bl;
      fh->ftop = addr - addr_of_block (fh->vol, bl);
      fh->fbot = 0;

      return (stat);
    }

    if ((sign ^ BlockUSED) == fh->vol->ErasedValue) {
      /* Used block found */
      if ((fh->flags & EFS_HANDLE_DEFRAG) == 0U) {
        /* Only application is allowed to allocate used block (defrag allocates empty blocks) */
        if (efs_is_busy (fh, bl) == false) {
          /* Block not busy, retrieve free space */
          if (efs_get_freeMem (fh->vol, bl) == 0) {
            /* No free space, mark block as full */
            stat = sign_write (fh->vol, bl, fh->vol->ErasedValue ^ BlockFULL);

            if (stat != fsOK) {
              return (stat);
            }
          }
          else {
            fh->fblock = bl;
            efs_mark_freeMem (fh);
            return (fsOK);
          }
        }
      }
    }
    /* Go to next block */
    bl++;
    if (bl == fh->vol->SectorCount) {
      bl = 0;
    }
  }
  /* Block allocation failed, out of free space */
  EvrFsEFS_DiskFull (fh->vol->DrvLet);
  return (fsNoFreeSpace);
}


/**
  Find data chunk associated with the current file state.
  Sets the bottom, top address and increases index of opened file.

  \param[in]  fh                        file handle
  \return     true/false
*/
static uint32_t efs_mark_fileMem (fsEFS_Handle *fh) {
  FALLOC fa;
  uint16_t bl;
  uint32_t i, addr, prev;

  bl = fh->fblock;

  /* Search for file blocks identified with 'fileID' */
  for (i = 0; i < fh->vol->SectorCount; i++) {
    /* Read block signature */
    addr = 0U;

    if (sign_read (fh->vol, bl, &fa.end, &addr) != fsOK) {
      return (false);
    }

    if ((fa.end != fh->vol->ErasedValue) && (fa.end != (fh->vol->ErasedValue ^ BlockTEMP))) {
      /* Used block found */
      for (prev = 0; ; prev = fa.end) {
        if (falloc_read (fh->vol, addr, &fa, &addr) != fsOK) {
          return (false);
        }

        if ((fa.fileID == fh->fileID) && (fa.index == fh->fidx)) {
          /* Set Current File Block parameters */
          fh->fblock = bl;
          fh->fbot   = (prev + 3U) & ~3U;
          fh->ftop   = fa.end;
          fh->fidx++;

          return (true);
        }
        if (fa.end == fh->vol->ErasedValue) break;
      }
    }
    /* Go to next block */
    bl++;
    if (bl == fh->vol->SectorCount) {
      bl = 0;
    }
  }
  return (false);
}


/**
  Sets the top and bottom addresses of block free memory region.

  Top address is hi address within block where FALLOC is placed.
  Bottom address is lo address where file names and data is stored.

  \param[in]  fh                        file handle
*/
static void efs_mark_freeMem (fsEFS_Handle *fh) {
  FALLOC fa;
  uint32_t addr, prev;

  /* Set address to first FALLOC within block */
  addr = addr_of_sign (fh->vol, fh->fblock) - sizeof(FALLOC);

  for (prev = 0; ; prev = fa.end) {
    if (falloc_read (fh->vol, addr, &fa, &addr) != fsOK) {
      /* Force block allocation */
      fh->fbot = fh->ftop;
      return;
    }

    if (fa.end == fh->vol->ErasedValue) { break; }
  }

  /* Align previous end address */
  prev = (prev + 3U) & ~3U;
  
  /* Set address to block relative address */
  addr = addr - addr_of_block (fh->vol, fh->fblock);
  
  /* fbot (low address) points to 4-byte aligned free byte */
  fh->fbot = prev;

  /* ftop (hi address) points to free file allocation record */
  fh->ftop = addr + sizeof(FALLOC);
}


/**
  Determine remaining free space in block.

  \param[in]  vol                       volume description structure
  \param[in]  block                     block number
  \return     Free space in bytes.
*/
static uint32_t efs_get_freeMem (fsEFS_Volume *vol, uint32_t block) {
  FALLOC fa;
  uint32_t size, addr, prev;

  /* Set address to first FALLOC within block */
  addr = addr_of_sign (vol, block) - sizeof(FALLOC);

  for (prev = 0; ; prev = fa.end) {
    if (falloc_read (vol, addr, &fa, &addr) != fsOK) {
      return (0U);
    }

    if (fa.end == vol->ErasedValue) { break; }
  }
  /* Align previous end address */
  prev = (prev + 3U) & ~3U;
  
  /* Set address to block relative address */
  addr = addr - addr_of_block (vol, block);

  if (prev < addr) {
    /* Free memory is between empty FALLOC and the end of previous data chunk */
    size = addr - prev;
  }
  else {
    /* Only empty FALLOC exists, no free space */
    size = 0U;
  }

  return (size);
}


/**
  Check if all stored file data within flash block is invalid.

  \param[in]  vol                       volume description structure
  \param[in]  block                     block number
  \return     true/false
*/
static uint32_t efs_BlockInvalid (fsEFS_Volume *vol, uint32_t block) {
  FALLOC fa;
  uint32_t addr;

  /* Set address to first file allocation record */
  addr = addr_of_sign (vol, block) - 8;

  /* Check if any valid file chunk exists within block */
  for (;;) {
    if (falloc_read (vol, addr, &fa, &addr) != fsOK) {
      return (false);
    }

    if (fa.end == vol->ErasedValue) { break; }
    if (fa.fileID != (uint16_t)(~vol->ErasedValue)) {
      /* Found valid file chunk */
      return (false);
    }
  }
  return (true);
}


/**
  Read data from a file at current file position.

  \param[in]  fh                        pointer to file handle
  \param[out] buf                       data buffer
  \param[in]  len                       number of bytes to read
  \return     execution status
                - positive value representing number of bytes read
                - negative value on error
*/
static int32_t efs_rd (fsEFS_Handle *fh, uint8_t *buf, uint32_t len) {
  uint32_t size, cnt, n, addr;
  int32_t rval;

  rval = 0;

  /* Set current block start address */
  addr = addr_of_block (fh->vol, fh->fblock);

  for (n = 0; n < len; n += cnt) {
    if (fh->fbot == fh->ftop) {
      if (efs_mark_fileMem (fh) == false) {
        /* EOF or file invalid */
        break;
      }
      /* Set current block start address */
      addr = addr_of_block (fh->vol, fh->fblock);
    }

    cnt  = len - n;
    size = fh->ftop - fh->fbot;

    if (size < cnt) {
      cnt = size;
    }

    if (block_read (fh->vol, addr + fh->fbot, &buf[n], cnt) != fsOK) {
      rval = -(int32_t)fsError;
      break;
    }

    /* Increment bottom address */
    fh->fbot += cnt;
  }

  fh->fpos += n;

  if (rval == 0) {
    /* Return number of bytes read */
    rval = (int32_t)n;
  }

  return (rval);
}


/**
  Write data to file at current file position.

  \param[in]  fh                        pointer to file handle
  \param[in]  buf                       data buffer
  \param[in]  len                       number of bytes to write
  \return     execution status
                - number of character written (0 when out of space)
                - negative value on error
*/
static int32_t efs_wr (fsEFS_Handle *fh, const uint8_t *buf, uint32_t len) {
  FALLOC fa;
  uint32_t size, cnt, n, addr, state;
  int32_t rval;

  rval = 0;

  /* Set current block start address */
  addr = addr_of_block (fh->vol, fh->fblock);

  if (fh->fbot == fh->ftop) {
    if (efs_is_busy (fh, fh->fblock) == false) {
      /* Retrieve free space within current block */
      efs_mark_freeMem (fh);
    }
  }

  if (fh->fbot >= (fh->ftop - sizeof(FALLOC))) {
    state = EFS_WR_ALLOC;
  } else {
    state = EFS_WR_DATA;
  }

  n = 0U;

  while (state != EFS_WR_DONE) {
    switch (state) {
      case EFS_WR_ALLOC:
        if (efs_AllocBlock (fh) == fsOK) {
          /* Set current block start address */
          addr = addr_of_block (fh->vol, fh->fblock);
          state = EFS_WR_DATA;
        }
        else {
          /* No free space or read/write error */
          state = EFS_WR_DONE;
        }
        break;

      case EFS_WR_FALLOC:
        fh->flags &= ~EFS_HANDLE_WALLOC;

        fa.end    = fh->fbot;
        fa.fileID = fh->fileID;
        fa.index  = fh->fidx++;

        if (falloc_write (fh->vol, fh->fblock, fh->ftop, &fa) != fsOK) {
          state = EFS_WR_ERROR;
        }
        else {
          /* Adjust top address to next FALLOC */
          fh->ftop -= sizeof(FALLOC);
          
          /* Realign bottom address to 4 bytes */
          fh->fbot = (fh->fbot + 3U) & ~3U;

          if (fh->fbot >= (fh->ftop - sizeof(FALLOC))) {
            /* Block is full */
            if ((fh->flags & EFS_HANDLE_DEFRAG) == 0U) {
              /* Not defragmenter owned block, update signature */
              if (sign_write (fh->vol, fh->fblock, fh->vol->ErasedValue ^ BlockFULL) == fsOK) {
                /* Allocate next block */
                state = EFS_WR_ALLOC;
              }
              else {
                /* Write error */
                state = EFS_WR_ERROR;
              }
            }
            else {
              /* Allocate next block */
              state = EFS_WR_ALLOC;
            }
          }
          else {
            /* Continue writing */
            state = EFS_WR_DATA;
          }

          if (n == len) {
            /* All data written */
            state = EFS_WR_DONE;
          }
        }
        break;

      case EFS_WR_DATA:
        /* Determine amount of data left to write */
        cnt = len - n;

        /* Determine amount of free space (reduced for extra FALLOC) */
        size = (fh->ftop - sizeof(FALLOC)) - fh->fbot;

        if (cnt > size) {
          cnt = size;
        }

        if (block_write (fh->vol, addr + fh->fbot, (uint8_t *)(uint32_t)&buf[n], cnt) != fsOK) {
          state = EFS_WR_ERROR;
        }
        else {
          n        += cnt;
          fh->fbot += cnt;

          if (fh->fbot == (fh->ftop - sizeof(FALLOC))) {
            /* Block is full */
            state = EFS_WR_FALLOC;
          }
          else {
            if (n == len) {
              /* Set flag 'write allocation record' */
              fh->flags |= EFS_HANDLE_WALLOC;

              if (len & 3) {
                /* Unaligned data write, write also allocation record */
                state = EFS_WR_FALLOC;
              } else {
                state = EFS_WR_DONE;
              }
            }
          }
        }
        break;

      case EFS_WR_ERROR:
      default:
        rval = -1;
        state = EFS_WR_DONE;
        break;
    }
  }

  fh->fpos  += n;
  fh->fsize += n;

  if (rval == 0) {
    /* Return number of bytes written */
    rval = (int32_t)n;
  }

  return (rval);
}


/* -------------------------- Exported functions -----------------------------*/


/**
  Initialize EFS volume resources and media driver.

  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_init (fsEFS_Volume *vol) {
  fsStatus status;
  uint32_t mask;

  mask = (~EFS_STATUS_MASK) | EFS_STATUS_INIT_IO;

  /* Check if already initialized */
  if ((vol->Status & mask) == EFS_STATUS_INIT_IO) {
    status = fsOK;
  }
  else {
    /* Initializing drive */
    EvrFsEFS_InitDrive (vol->DrvLet);

    /* Load user configuration */
    fs_config ((const char *)&vol->DrvLet);

    if (vol->Mutex == NULL) {
      /* Mutex was not created */
      status = fsError;
    }
    else {
      vol->Status = 0U;

      /* Initializing driver */
      EvrFsEFS_InitDriver (vol->DrvLet, (uint32_t)vol->Drv);

      status = vol->Drv->Init();

      if (status == fsOK) {
        /* Initialization succeeded */
        vol->Status |= EFS_STATUS_INIT_IO;
      }
      else {
        /* Failed to initialize the driver */
        EvrFsEFS_InitDriverFailed (vol->DrvLet, (uint32_t)vol->Drv);
      }
    }
  }

  return (status);
}


/**
  Uninitialize EFS volume and media driver.

  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_uninit (fsEFS_Volume *vol) {
  fsStatus status;

  status = fsOK;

  /* Uninitializing drive */
  EvrFsEFS_UninitDrive (vol->DrvLet);

  if (vol->Mutex != NULL) {
    if (fs_mutex_delete (vol->Mutex) != 0U) {
      status = fsError;
    }
  }

  if (status == fsOK) {
    if (vol->Drv != NULL) {
      EvrFsEFS_UninitDriver (vol->DrvLet, (uint32_t)vol->Drv);

      /* Uninitialize the media driver */
      status = vol->Drv->UnInit();
    }

    /* Clear volume structure */
    memset (vol, 0, sizeof(fsEFS_Volume));
  }

  return (status);
}


/**
  Mount EFS volume.

  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_mount (fsEFS_Volume *vol) {
  uint32_t i;
  uint32_t val;
  fsStatus stat;

  /* Mounting drive */
  EvrFsEFS_MountDrive (vol->DrvLet);

  stat = efs_vol_chk (EFS_STATUS_INIT_IO, vol);
  if (stat != fsOK) {
    return (stat);
  }

  if ((vol->Status & EFS_STATUS_INIT_IO) == 0U) {
    /* Initialize the media driver */
    stat = vol->Drv->Init();
    if (stat != fsOK) {
      return (stat);
    }

    /* Media driver is initialized */
    vol->Status |= EFS_STATUS_INIT_IO;
  }

  /* Retreiving flash info */
  EvrFsEFS_FlashGetInfo (vol->DrvLet);

  if (vol->Drv->DeviceCtrl ((fsDevCtrlCode)fsDevCtrlCodeBlockCount, &vol->SectorCount) != fsOK) {
    /* Could not retrieve the memory sector count */
    stat = fsError;
  }
  else if (vol->Drv->DeviceCtrl ((fsDevCtrlCode)fsDevCtrlCodeErasedValue, &vol->ErasedValue) != fsOK) {
    /* Could not retrieve the memory content erased value */
    stat = fsError;
  }
  else if (vol->Drv->DeviceCtrl ((fsDevCtrlCode)fsDevCtrlCodeProgramUnit, &val) != fsOK) {
    /* Could not retrieve the memory program unit */
    stat = fsError;
  }
  else {
    if (val > EFS_PROG_UNIT) {
      /* EFS needs to write 4 byte chunks of data */
      EvrFsEFS_FlashProgUnitTooBig (vol->DrvLet, val, EFS_PROG_UNIT);
      stat = fsError;
    }
    else {
      /* Determine the total size of device flash memory */
      vol->Size = 0U;
      
      for (i = 0U; i < vol->SectorCount; i++) {
        vol->Size += vol->Drv->GetSectorSize(i);
      }

      EvrFsEFS_FlashCapacity (vol->DrvLet, vol->Size);
    }
  }

  if (stat == fsOK) {
    /* Reset current top file ID */
    vol->TopID = 0U;

    vol->Status |= EFS_STATUS_MOUNT;

    /* Drive mounted */
    EvrFsEFS_MountDriveSuccess (vol->DrvLet);
  }

  return (stat);
}


/**
  Unmount EFS drive.

  Checks for opened file handlers and closes them. When this function returns
  volume is unmounted and cannot be accessed anymore.

  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_unmount (fsEFS_Volume *vol) {
  fsEFS_Handle *fh;
  int32_t i;

  /* Unmounting drive */
  EvrFsEFS_UnmountDrive (vol->DrvLet);

  for (i = 0, fh = &fs_efs_fh[0]; i < fs_efs_fh_cnt; i++, fh++) {
    if (fh->flags & EFS_HANDLE_OPEN) {
      if (fh->vol == vol) {
        /* File belongs to this volume */
        if (efs_close (i) != fsOK) {
          /* Failed to close the file */
        }
        fh->flags = 0;
      }
    }
  }
  vol->Status &= ~EFS_STATUS_MOUNT;

  /* Drive unmounted */
  EvrFsEFS_UnmountDriveSuccess (vol->DrvLet);
  return (fsOK);
}


/**
  Format EFS drive.

  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_format (fsEFS_Volume *vol) {
  fsStatus status;

  /* Formatting drive */
  EvrFsEFS_FormatDrive (vol->DrvLet);

  status = efs_vol_chk (EFS_STATUS_MOUNT, vol);
  if (status != fsOK) {
    return (status);
  }

  /* Reset all opened file handles on this drive */
  efs_handles_reset (vol);

  /* Erase chip */
  EvrFsEFS_FlashEraseChip (vol->DrvLet);

  status = vol->Drv->DeviceCtrl (fsDevCtrlCodeFormat, NULL);

  if (status == fsOK) {
    /* Reset file ID tracker */
    vol->TopID = 0;

    /* Formatting completed  */
    EvrFsEFS_FormatDriveSuccess (vol->DrvLet);
  }
  else {
    EvrFsEFS_FlashEraseChipFailed (vol->DrvLet);
  }

  return (status);
}


/**
  Open a file on the Embedded File System.

  \param[in]  handle                    file handle
  \param[in]  fn                        file name
  \param[in]  openmode                  file open mode flags
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_open (int32_t handle, const char *fn, int32_t openmode) {
  fsEFS_Handle *fh;
  fsStatus status;

  EvrFsEFS_FileOpen (handle, fn, openmode);
  EvrFsEFS_FileName (fn, strlen(fn));

  if ((handle < 0) || (handle >= fs_efs_fh_cnt)) {
    /* Invalid parameter: handle number out of range */
    EvrFsEFS_FileHandleInvalid (handle);
    return (fsInvalidParameter);
  }

  fh = &fs_efs_fh[handle];
  fn = file_name_validate (fn);

  if (fn == NULL) {
    /* Invalid parameter: path not specified */
    return (fsInvalidPath);
  }

  status = efs_vol_chk (EFS_STATUS_MOUNT, fh->vol);
  if (status != fsOK) {
    return (status);
  }

  /* Set the open mode flags */
  if (openmode & FS_FOPEN_RDWR) {
    /* Read/Write mode is unsupported */
    EvrFsEFS_OpenModeUnsupported (handle, openmode);
    return (fsError);
  }
  else if (openmode & FS_FOPEN_WR) {
    /* File opened for write only */
    fh->flags = EFS_HANDLE_WRITE;
  }
  else {
    /* File opened for read only */
    fh->flags = EFS_HANDLE_READ;
  }

  if (openmode & FS_FOPEN_APPEND) {
    /* File opened for append */
    fh->flags |= EFS_HANDLE_APPEND;
  }


  if (file_find (fh, fn) == fsOK) {
    /* File has been found, check if already opened */
    if (efs_open_allowed (fh) == false) {
      /* Only multiple opened files for read are allowed */
      return (fsAccessDenied);
    }

    fh->fsize = file_size_get (fh->vol, fh->fblock, fh->fileID);
    /* File size is now kept in handle */
    fh->flags |= EFS_HANDLE_FSIZE;

    if (fh->flags & EFS_HANDLE_APPEND) {
      /* Append mode, check if other file writes to the same block */
      if (efs_is_busy (fh, fh->fblock) == true) {
        /* Allocate new block on first write */
        fh->fbot = fh->ftop;
      }
      return (fsOK);
    }

    if (fh->flags & EFS_HANDLE_READ) {
      /* Open also 0-size file for reading. */
      fh->ftop = fh->fbot;
      return (fsOK);
    }

    /* Write mode, delete existing file first. */
    if (file_delete (fh) != fsOK) {
      /* File delete failed */
      return (fsError);
    }
  }
  else {
    /* File not found */
    if (fh->flags & EFS_HANDLE_READ) {
      /* Cannot open for read if file does not exists */
      return (fsFileNotFound);
    }
  }

  /* Create a file for write or append mode. */
  fh->fileID = 0;
  return (file_create (fh, fn));
}


/**
  Close a file on the Embedded File System.

  \param[in]  handle                    file handle
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_close (int32_t handle) {
  fsEFS_Handle *fh;
  fsStatus stat;

  /* Closing file */
  EvrFsEFS_FileClose (handle);

  if ((handle < 0) || (handle >= fs_efs_fh_cnt)) {
    /* Invalid parameters */
    EvrFsEFS_FileHandleInvalid (handle);
    return (fsInvalidParameter);
  }

  fh = &fs_efs_fh[handle];

  if ((fh->flags & EFS_HANDLE_OPEN) == 0) {
    /* File not opened */
    EvrFsEFS_FileNotOpened (handle, fh->flags);
    stat = fsError;
  }
  else if ((fh->flags & EFS_HANDLE_READ) != 0) {
    /* File opened for read, just invalidate file handle */
    stat = fsOK;
  }
  else {
    stat = efs_vol_chk (EFS_STATUS_MOUNT, fh->vol);

    if (stat == fsOK) {
      /* Flush content and close the file */
      stat = file_close (fh);
    }
  }

  /* Invalidate file handle */
  fh->flags = 0;

  return (stat);
}


/**
  Read data from file asociated with the specified handle.

  \param[in]  handle                    file handle number
  \param[out] buf                       data buffer
  \param[in]  len                       number of bytes to read
  \return     Number of bytes not read or error if negative.
*/
__WEAK int32_t efs_read (int32_t handle, uint8_t *buf, uint32_t len) {
  fsEFS_Handle *fh;
  fsStatus status;
  int32_t rval;

  EvrFsEFS_FileRead (handle, buf, len);

  if ((handle < 0) || (handle >= fs_efs_fh_cnt)) {
    /* Invalid parameter: handle number out of range */
    EvrFsEFS_FileHandleInvalid (handle);
    rval = -(int32_t)fsInvalidParameter;
  }
  else if (buf == NULL) {
    /* Invalid parameter: buffer not specified */
    rval = -(int32_t)fsInvalidParameter;
  }
  else {
    fh = &fs_efs_fh[handle];

    if ((fh->flags & EFS_HANDLE_OPEN) == 0) {
      /* Error, file not opened. */
      EvrFsEFS_FileNotOpened (handle, fh->flags);
      rval = -(int32_t)fsError;
    }
    else if ((fh->flags & EFS_HANDLE_MODES) == EFS_HANDLE_WRITE) {
      /* Only opened for write */
      EvrFsEFS_FileOpenWriteMode (handle);
      rval = -(int32_t)fsAccessDenied;
    }
    else {
      status = efs_vol_chk (EFS_STATUS_MOUNT, fh->vol);
      if (status != fsOK) {
        rval = -(int32_t)status;
      }
      else {
        /* Read data from a file */
        rval = efs_rd (fh, buf, len);
      }
    }
  }

  return (rval);
}


/**
  Write data to file asociated with the specified handle.

  \param[in]  handle                    file handle number
  \param[in]  buf                       data buffer
  \param[in]  len                       number of bytes to write
  \return     execution status
                - 0 = ok
                - negative value on error
                - positive value on partial success (number of characters not written)
*/
__WEAK int32_t efs_write (int32_t handle, const uint8_t *buf, uint32_t len) {
  fsEFS_Handle *fh;
  fsStatus status;
  int32_t rval;

  EvrFsEFS_FileWrite (handle, buf, len);

  if ((handle < 0) || (handle >= fs_efs_fh_cnt)) {
    /* Invalid parameter: handle number out of range */
    EvrFsEFS_FileHandleInvalid (handle);
    rval = -(int32_t)fsInvalidParameter;
  }
  else if (buf == NULL) {
    /* Invalid parameter: buffer not specified */
    rval = -(int32_t)fsInvalidParameter;
  }
  else {
    fh = &fs_efs_fh[handle];

    if ((fh->flags & EFS_HANDLE_OPEN) == 0) {
      /* File not opened */
      EvrFsEFS_FileNotOpened (handle, fh->flags);

      rval = -(int32_t)fsError;
    }
    else if ((fh->flags & EFS_HANDLE_MODES) == EFS_HANDLE_READ) {
      /* Only opened for read */
      EvrFsEFS_FileOpenReadMode (handle);

      rval = -(int32_t)fsAccessDenied;
    }
    else {
      status = efs_vol_chk (EFS_STATUS_MOUNT, fh->vol);

      if (status != fsOK) {
        rval = -(int32_t)status;
      }
      else if (len == 0U) {
        /* Nothing to do, state unchanged */
        rval = 0;
      }
      else {
        /* Write data into a file */
        rval = efs_wr (fh, buf, len);
      }
    }
  }

  return (rval);
}


/**
  Flush buffer to the media.

  This function will write allocation info record to the media.

  \param[in] handle                     file handle
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_flush (int32_t handle) {
  fsEFS_Handle *fh;
  FALLOC fa;
  fsStatus stat;

  EvrFsEFS_FileFlush (handle);

  fh = &fs_efs_fh[handle];

  if ((fh->flags & EFS_HANDLE_OPEN) == 0) {
    /* Error, file not opened. */
    EvrFsEFS_FileNotOpened (handle, fh->flags);
    return (fsError);
  }
  if ((fh->flags & EFS_HANDLE_WRITE) == 0) {
    /* File not opened for write */
    EvrFsEFS_OpenModeInvalid (handle, fh->flags);
    return (fsAccessDenied);
  }

  stat = efs_vol_chk (EFS_STATUS_MOUNT, fh->vol);
  if (stat != fsOK) {
    return (stat);
  }

  if (fh->flags & EFS_HANDLE_WALLOC) {
    /* Unaligned data end, write alloc record. */
    fa.end    = fh->fbot;
    fa.fileID = fh->fileID;
    fa.index  = fh->fidx++;

    stat = falloc_write (fh->vol, fh->fblock, fh->ftop, &fa);

    if (stat == fsOK) {
      /* Adjust top address to next FALLOC */
      fh->ftop -= sizeof (FALLOC);

      /* Realign bottom address to 4 bytes */
      fh->fbot = (fh->fbot + 3U) & ~3U;

      /* Clear "write FALLOC" flag */
      fh->flags &= ~EFS_HANDLE_WALLOC;
    }
  }
  return (fsOK);
}


/**
  Determine and set the file length

  \param[in] handle                     file handle
  \return    Current length of the file or negative error indicator.
*/
__WEAK int32_t efs_flen (int32_t handle) {
  fsEFS_Handle *fh;
  fsStatus stat;

  fh = &fs_efs_fh[handle];

  if (!(fh->flags & EFS_HANDLE_OPEN)) {
    /* Error, file not opened */
    EvrFsEFS_FileNotOpened (handle, fh->flags);
    return (-(int32_t)fsError);
  }

  stat = efs_vol_chk (EFS_STATUS_MOUNT, fh->vol);
  if (stat != fsOK) {
    return (-(int32_t)stat);
  }

  if ((fh->flags & EFS_HANDLE_FSIZE) == 0) {
    fh->fsize = file_size_get (fh->vol, fh->fblock, fh->fileID);
    /* File size is now keept in handle */
    fh->flags |= EFS_HANDLE_FSIZE;
  }

  return ((int32_t)fh->fsize);
}


/**
  Set File Position pointer.

  \param[in]  handle                    file handle
  \param[in]  offset                    the number of bytes to move
  \param[in]  whence                    file position location
  \return     current file position from the beginning of the file,
              or negative \ref fsStatus return code on failure
*/
__WEAK int32_t efs_seek (int32_t handle, int32_t offset, int32_t whence) {
  fsEFS_Handle *fh;
  FALLOC fa;
  uint16_t bl, fidx;
  uint32_t addr, prev;
  uint32_t i, pos, fpos;
  fsStatus stat;

  /* Setting file position */
  EvrFsEFS_FileSeek (handle, offset, whence);

  fh = &fs_efs_fh[handle];

  if ((fh->flags & EFS_HANDLE_OPEN) == 0) {
    /* Error, file not opened */
    EvrFsEFS_FileNotOpened (handle, fh->flags);
    return -(int32_t)(fsError);
  }

  stat = efs_vol_chk (EFS_STATUS_MOUNT, fh->vol);
  if (stat != fsOK) {
    return (int32_t)(stat);
  }

  if (whence == FS_FSEEK_SET) {
    pos = 0U;
  }
  else if (whence == FS_FSEEK_CUR) {
    pos = fh->fpos;
  }
  else if (whence == FS_FSEEK_END) {
    pos = fh->fsize;
  }
  else {
    /* Invalid parameter: invalid seek operation */
    return -(int32_t)(fsInvalidParameter);
  }

  /* Set new position (from the start of the file) */
  pos = (uint32_t)((int32_t)pos + offset);

  if (pos == fh->fpos) {
    /* We are already here */
    return (fsOK);
  }

  if (pos > fh->fsize) {
    /* EFS does not support fseek beyond file size */
    EvrFsEFS_FileSeekEOF (handle, fh->fsize, pos);
    return -(int32_t)(fsError);
  }

  /* pos <= fh->fize */
  fidx = 0;
  fpos = 0;
  bl   = fh->fblock;

next:
  /* Scan blocks for files with current fileID */
  for (i = 0; i < fh->vol->SectorCount; i++) {
    /* Read block signature */
    addr = 0U;

    if (sign_read (fh->vol, bl, &fa.end, &addr) != fsOK) {
      return -(int32_t)(fsError);
    }

    if (fa.end != fh->vol->ErasedValue) {
      /* Read all allocation info */
      for (prev = 0; ; prev = fa.end) {
        stat = falloc_read (fh->vol, addr, &fa, &addr);

        if (stat != fsOK) {
          return -(int32_t)(stat);
        }

        if ((fa.fileID == fh->fileID) && (fa.index == fidx)) {
          /* All file blocks are 4-byte aligned. */
          prev = (prev + 3U) & ~3U;
          fidx++;

          if (fpos + (fa.end - prev) < pos) {
            /* Go and try next File Block */
            fpos += (fa.end - prev);
            goto next;
          }

          /* OK, the right File Block Index is found */
          fh->fblock = bl;
          fh->fidx   = fidx;
          /* File opened for read. */
          fh->fbot   = prev + (pos - fpos);
          fh->ftop   = fa.end;

          /* Store new file position */
          fh->fpos   = pos;
          return ((int32_t)fh->fpos);
        }

        if (fa.end == fh->vol->ErasedValue) {
          /* Free space area */
          break;
        }
      }
    }
    /* Go to next block */
    bl++;

    if (bl == fh->vol->SectorCount) {
      bl = 0;
    }
  }
  return -(int32_t)(fsError);
}


/**
  Delete a file.

  \param[in]  handle                    file handle
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_delete (const char *path, fsEFS_Volume *vol) {
  fsEFS_Handle fh;
  fsStatus stat;

  /* Deleting file */
  EvrFsEFS_FileDelete (vol->DrvLet, path);
  EvrFsEFS_FileName  (path, strlen(path));

  path = file_name_validate (path);
  if (path == NULL) {
    return (fsInvalidPath);
  }

  stat = efs_vol_chk (EFS_STATUS_MOUNT, vol);
  if (stat != fsOK) {
    return (stat);
  }

  fh.vol   = vol;
  fh.flags = 0U;

  if (file_find (&fh, path) != fsOK) {
    /* File does not exist */
    return (fsFileNotFound);
  }
  return (file_delete (&fh));
}


/**
  Searches for the file with next file ID.

  \param[in]  handle                    file handle
  \param[in]  info                      file information structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_ffind (fsFileInfo *info, fsEFS_Volume *vol) {
  FALLOC fa;
  uint32_t block, addr;
  uint32_t nid, id, id_block = 0U;
  uint32_t prev, fn_addr = 0U;
  fsStatus stat;

  if (info == NULL) {
    /* Invalid parameter */
    EvrFsEFS_InvalidParameter (vol->DrvLet);
    return (fsInvalidParameter);
  }

  stat = efs_vol_chk (EFS_STATUS_MOUNT, vol);
  if (stat != fsOK) {
    return (stat);
  }

  /* Initialize next and current id */
  nid = 0x8000 | (info->fileID + 1);
  id  = 0xFFFF;

  /* Scan through all blocks */
  for (block = 0; block < vol->SectorCount; block++) {
    /* Read block signature */
    addr = 0U;

    stat = sign_read (vol, block, &fa.end, &addr);
    if (stat != fsOK) {
      return (stat);
    }

    /* Read all file allocation info */
    prev = 0;
    while (fa.end != vol->ErasedValue) {
      stat = falloc_read (vol, addr, &fa, &addr);

      if (stat != fsOK) {
        return (stat);
      }

      if (fa.end != vol->ErasedValue) {
        if ((fa.fileID >= nid) && (fa.fileID < id)) {
          /* Store the ID of the current valid file */
          id = fa.fileID;
          /* Store the block where we found the file */
          id_block = block;
          /* Store the file name address */
          fn_addr = addr_of_block(vol, block) + prev;

          if (id == nid) {
            break;
          }
        }
        prev = fa.end;
      }
    }

    if (id == nid) { break; }
  }
  
  if (id == 0xFFFF) {
    /* File not found */
    EvrFsEFS_FileNotFound (vol->DrvLet);
    return (fsFileNotFound);
  }

  /* Filename is stored 4-byte aligned. */
  fn_addr = (fn_addr + 3U) & ~3U;
  /* Copy name to buffer. */
  block_read (vol, fn_addr, &info->name, 32);

  info->fileID = id & 0x7FFF;

  /* Timestamps are not supported in EFS */
  info->time.hr  = 12;
  info->time.min = 0;
  info->time.sec = 0;
  info->time.day = 1;
  info->time.mon = 1;
  info->time.year= 1980;
  
  info->size   = file_size_get (vol, id_block, info->fileID);
  info->attrib = 0;

  return (fsOK);
}


/**
  Rename a file

  \param[in]  fn                        old file name
  \param[in]  newname                   new file name
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_rename (const char *fn, const char *newname, fsEFS_Volume *vol) {
  fsEFS_Handle fh;
  fsStatus stat;

  /* Renaming file fn to newname */
  EvrFsEFS_FileRename (vol->DrvLet, fn, newname);
  EvrFsEFS_FileName  (fn, strlen(fn));
  EvrFsEFS_FileName  (newname, strlen(newname));

  stat = efs_vol_chk (EFS_STATUS_MOUNT, vol);
  if (stat != fsOK) {
    return (stat);
  }

  fn = file_name_validate (fn);
  if (fn == NULL) {
    return (fsInvalidPath);
  }

  newname = file_name_validate (newname);
  if (newname == NULL) {
    return (fsInvalidPath);
  }

  fh.vol = vol;

  if (file_find (&fh, fn) != fsOK) {
    /* File to be renamed was not found */
    EvrFsEFS_FileNotFound (vol->DrvLet);
    return (fsError);
  }

  if (file_find (&fh, newname) == fsOK) {
    /* File with given new name already exists */
    EvrFsEFS_FileAlreadyExists (vol->DrvLet);
    return (fsAlreadyExists);
  }

  return (file_rename (&fh, newname));
}


/**
  Calculate a free space for EFS drive.

  \param[in]  vol                       volume description structure
  \return     Free space in bytes.
*/
__WEAK int64_t efs_free (fsEFS_Volume *vol) {
  uint32_t block;
  fsStatus status;
  int64_t free;

  /* Retrieving free space on drive %s */
  EvrFsEFS_FreeSpaceRetrieve (vol->DrvLet);

  status = efs_vol_chk (EFS_STATUS_MOUNT, vol);

  if (status != fsOK) {
    free = (int64_t)(-status);
  }
  else {
    free = 0;

    for (block = 0; block < vol->SectorCount; block++) {
      /* Add free space from current block */
      free += efs_get_freeMem (vol, block);
    }
    EvrFsEFS_FreeSpaceAmount (vol->DrvLet, (uint32_t)free);
  }

  return (free);
}


/**
  Analyse the Volume and check for file defragmentation.

  \param[in]  vol                       volume description structure
  \return     fragmentation factor or execution status
              value >= 0 or <= 255: fragmentation factor
              value < 0: error occurred, -value is execution status as defined with fsStatus
*/
__WEAK int32_t efs_analyse (fsEFS_Volume *vol) {
  fsStatus stat;
  FALLOC fa;
  uint32_t total, invalid;
  uint32_t i, addr, prev;

  /* Analyzing drive %s defragmentation */
  EvrFsEFS_DriveAnalyze (vol->DrvLet);

  if (efs_vol_chk (EFS_STATUS_MOUNT, vol) != fsOK) {
    return (0);
  }

  /* Initialize total size of invalidated file chunks */
  invalid = 0;

  /* Determine the total size of invalidated blocks */
  for (i = 0; i < vol->SectorCount; i++) {
    /* Read block signature */
    addr = 0U;

    stat = sign_read (vol, i, &fa.end, &addr);

    if (stat == fsOK) {
      if (fa.end != vol->ErasedValue) {
        for (prev = 0; ; prev = fa.end) {
          stat = falloc_read (vol, addr, &fa, &addr);

          if (stat != fsOK) {
            break;
          }

          if (fa.fileID == (uint16_t)(~vol->ErasedValue)) {
            /* Add the size of invalidated file chunk */
            invalid += (fa.end - prev);
          }
          if (fa.end == vol->ErasedValue) { break; }
        }
      }
    }

    if (stat != fsOK) {
      return (-(int32_t)stat);
    }
  }
  /* Reduce total memory size for size of signatures and scale the value */
  total = (vol->Size - (vol->SectorCount * 4)) >> 8;

  /* Return factor range 0..255 */
  EvrFsEFS_DriveAnalyzeSuccess (vol->DrvLet, invalid/total);
  return ((int32_t)(invalid/total));
}


/**
  Check integrity of the Embedded File System

  All blocks are checked for: - valid file IDs
                              - ascending file allocation records
                              - overlapping file allocation records

  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_check (fsEFS_Volume *vol) {
  FALLOC fa;
  uint32_t bl, addr, prev, sa;
  fsStatus stat;

  /* Checking EFS integrity */
  EvrFsEFS_DriveCheck (vol->DrvLet);

  stat = efs_vol_chk (EFS_STATUS_MOUNT, vol);
  if (stat != fsOK) {
    return (stat);
  }

  /* Scan blocks */
  for (bl = 0; bl < vol->SectorCount; bl++) {
    /* Read block signature */
    addr = 0U;

    stat = sign_read (vol, bl, &fa.end, &addr);
    if (stat != fsOK) {
      return (stat);
    }

    if (fa.end != vol->ErasedValue) {
      /* Set current block start address */
      sa = addr_of_block (vol, bl);

      for (prev = 0; ; prev = fa.end) {
        stat = falloc_read (vol, addr, &fa, &addr);

        if (stat == fsOK) {
          if (fa.end != vol->ErasedValue) {

            if ((prev + sizeof (FALLOC)) >= addr) {
              /* Error, used area overlap allocation area */
              EvrFsEFS_DataAreaOverlap (vol->DrvLet);
              stat = fsError;
            }
            else if (fa.fileID == 0x8000) {
              /* Error, not existing file ID. */
              EvrFsEFS_FileIdInvalid (vol->DrvLet);
              stat = fsError;
            }
            else if (fa.end < prev) {
              /* Error, not ascending allocation pointers. */
              EvrFsEFS_AllocationOrderInvalid (vol->DrvLet);
              stat = fsError;
            }
            else {
                if ((sa + fa.end) > addr) {
                /* Error, overlapping file allocation record. */
                EvrFsEFS_AllocationAreaOverlap (vol->DrvLet);
                stat = fsError;
              }
            }
          }
          else {
            /* End of allocation records */
            break;
          }
        }

        if (stat != fsOK) {
          return (stat);
        }
      }
    }
  }
  /* Integrity check passed */
  EvrFsEFS_DriveCheckSuccess (vol->DrvLet);
  return (fsOK);
}


/**
  Re-sort EFS files so that used Flash pages are reduced.

  Drive is examined for invalidated blocks. Blocks which do not have any valid
  file allocation information are erased. Fragmented files are consolidated.
  The result of file consolidation is faster access to allocation information and
  more erased blocks, which increases overall performance.

  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_defrag (fsEFS_Volume *vol) {
  int32_t handle;
  fsEFS_Handle *fh;
  fsEFS_Handle  fh_d;
  uint32_t i, bl, sign;
  fsStatus stat;

  /* Defragment drive */
  EvrFsEFS_DriveDefrag (vol->DrvLet);

  stat = efs_vol_chk (EFS_STATUS_MOUNT, vol);
  if (stat != fsOK) {
    return (stat);
  }

  /* Check for active file handles */
  for (i = 0; i < fs_efs_fh_cnt; i++) {
    fh = &fs_efs_fh[i];

    if (fh->vol == vol) {
      /* Same volume */
      if (fh->flags & EFS_HANDLE_OPEN) {
        /* Error, a file is opened */
        EvrFsEFS_FileHandleActive (vol->DrvLet, i);
        return (fsAccessDenied);
      }
    }
  }

  handle = efs_handle_get(vol);

  if (handle < 0) {
    /* Failed to get free file handle */
    return (fsError);
  }

  fh = &fs_efs_fh[handle];

  fh->flags = EFS_HANDLE_READ | EFS_HANDLE_WRITE;

  /* Erase invalidated blocks */
  for (bl = 0; bl < vol->SectorCount; bl++) {
    /* Read block signature */
    stat = sign_read (vol, bl, &sign, NULL);

    if (stat == fsOK) {
      if (sign != vol->ErasedValue) {
        /* Check if block is invalidated */
        if (efs_BlockInvalid (vol, bl) == true) {
          stat = block_erase (vol, bl);
        }
      }
    }

    if (stat != fsOK) {
      return (stat);
    }
  }

  /* Initialize destination file handle */
  memset (&fh_d, 0, sizeof(fh_d));

  fh_d.vol    = fh->vol;
  fh_d.flags  = EFS_HANDLE_WRITE | EFS_HANDLE_DEFRAG;

  /* Allocate first free sector */
  fh_d.fblock = 0U;

  if (efs_AllocBlock (&fh_d) != fsOK) {
    /* Out of space */
    return (fsError);
  }

  /* Set initial source fileID */
  fh->fileID = 0;

  while (ed_get_next (fh) == true) {
    if (ed_copy (fh, &fh_d) == false) {
      /* Defragmentation failed, delete last temp file. */
      file_delete (&fh_d);

      break;
    }
    /* Delete original file */
    file_delete (fh);
  }
  /* Invalidate file handle */
  fh->flags = 0U;

  /* Erase invalidated blocks */
  for (bl = 0; bl < vol->SectorCount; bl++) {
    /* Read block signature */
    stat = sign_read (vol, bl, &sign, NULL);

    if (stat == fsOK) {
      if (sign != vol->ErasedValue) {
        /* Check if block is invalidated */
        if (efs_BlockInvalid (vol, bl) == true) {
          stat = block_erase (vol, bl);
        }
      }
    }

    if (stat != fsOK) {
      return (stat);
    }
  }

  /* Convert temporary into regular blocks */
  for (bl = 0; bl < vol->SectorCount; bl++) {
    /* Read block signature */
    stat = sign_read (vol, bl, &sign, NULL);

    if (stat == fsOK) {
      if (sign != vol->ErasedValue) {
        /* Check if block is temporary */
        if ((sign ^ BlockTEMP) == vol->ErasedValue) {
          /* Temporary block found, mark it as used */
          sign = vol->ErasedValue ^ BlockUSED;

          if (efs_get_freeMem (vol, bl) == 0) {
            /* Block is full */
            sign = vol->ErasedValue ^ BlockFULL;
          }
          stat = sign_write (vol, bl, sign);
        }
      }
    }

    if (stat != fsOK) {
      break;
    }
  }

  return (stat);
}


/**
  Read drive information.

  \param[out] info                      drive information structure.
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus efs_info (fsDriveInfo *info, fsEFS_Volume *vol) {
  fsStatus status;

  if (info == NULL) {
    status = fsInvalidParameter;
  }
  else {
    status = efs_vol_chk (EFS_STATUS_MOUNT, vol);

    if (status == fsOK) {
      info->fs_type  = fsTypeEFS;
      info->capacity = vol->Size;
    }
  }

  return (status);
}
