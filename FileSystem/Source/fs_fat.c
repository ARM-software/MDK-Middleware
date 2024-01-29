/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_fat.c
 * Purpose: FAT File System Implementation
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "cmsis_compiler.h"

#include "fs_common.h"
#include "fs_fat.h"
#include "fs_fat_elink.h"
#include "fs_evr.h"

/*
  FAT File System Limitations:

  No file has a file size > 0xFFFFFFFF bytes (FAT12, FAT16, FAT32)
  (ie. max file size is limited to 0xFFFFFFFF (4,294,967,295) bytes)

  A directory must not be larger than 65,536 * 32 (2,097,152) bytes
  (i.e. max files within folder is 65,536)

  A root directory on FAT12 and FAT16 can hold max 512 files/folders
  (no such limitation for FAT32)

  FAT12 maximum clusters: 4,084
  FAT16 minimum clusters: 4,085
  FAT16 maximum clusters: 65,524
  FAT32 minimum clusters: 65,525

  FAT32 max. volume size: 2TiB (4G of sectors * 512B)
*/

/* Formatting table */
static const DEVPAR IniDevCfg[] = {
/* NumHeads NumCyl NumSect FatType  SecClus  SecClus32 BootSector */
  {     2,    512,   16,   FS_FAT12,    16,       0,      65 },   /*    8 MB */
  {     2,   1024,   16,   FS_FAT12,    16,       0,      65 },   /*   16 MB */
  {     2,   1024,   32,   FS_FAT12,    32,       0,      65 },   /*   32 MB */
  {     4,   1024,   32,   FS_FAT12,    32,       0,      65 },   /*   64 MB */
  {     8,   1024,   32,   FS_FAT16,    16,       0,      65 },   /*  128 MB */
  {    16,   1024,   32,   FS_FAT16,    16,       4,     129 },   /*  256 MB */
  {    32,   1024,   32,   FS_FAT16,    32,       8,     129 },   /*  512 MB */
  {    64,   1024,   32,   FS_FAT16,    32,      16,     257 },   /*    1 GB */
  {   256,    512,   32,   FS_FAT16,    64,      32,     257 },   /*    2 GB */
  {   256,   1024,   32,   FS_FAT32,    64,      64,     257 },   /*    4 GB */
  {   256,   1024,   63,   FS_FAT32,    64,      64,     257 },   /*    8 GB */
  {   255,   1024,   63,   FS_FAT32,    64,      64,     257 },   /*   16 GB */
  {   255,   1024,   63,   FS_FAT32,    64,      64,     257 }};  /*>= 32 GB */
/* Parameters in this table are optimized for SD/MMC memory cards.            */
/* Optimal file system is FAT12/FAT16 with Cluster sizes 8k, 16k, 32k.        */
/* Cluster 2 shall be block aligned (32K aligned)                             */

#ifdef FS_FAT_LFN
static const uint8_t ChIndex[13] = { 1, 3, 5, 7, 9, 14, 16, 18, 20, 22, 24, 28, 30 };
#endif


/**
  Check FAT volume for valid status flags and media presence.

  Status flag bitmask must be specified in order to check if flag is set.
  For removable drives, logic is inverted, when FAT_STATUS_REMOVABLE
  bitmask is specified, flag is NOT checked!

  \param[in]  status                    volume status bitmask to check
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
static fsStatus fat_vol_chk (uint32_t status, fsFAT_Volume *vol) {
  uint32_t flags;

  if (status & FAT_STATUS_INIT_IO) {
    if (!(vol->Status & FAT_STATUS_INIT_IO)) {
      /* Media driver not initialized */
      EvrFsFAT_DriverNotInitialized (vol->DrvLet);
      return (fsUninitializedDrive);
    }
  }
  if ((status & FAT_STATUS_REMOVABLE) == 0) {
    if (vol->Status & FAT_STATUS_REMOVABLE) {
      /* Update Media status for removable drives. */
      flags = 0;
      vol->Drv->DeviceCtrl (fsDevCtrlCodeCheckMedia, &flags);

      if (flags & FS_MEDIA_NOCHKMEDIA) {
        /* Media presence and write protection cannot be detected */
        vol->Status &= ~FAT_STATUS_REMOVABLE;
      }
      else {
        if (!(flags & FS_MEDIA_INSERTED)) {
          /* Media is not inserted */
          vol->Status &= ~(FAT_STATUS_INIT_MEDIA | FAT_STATUS_READY | FAT_STATUS_MOUNT);
          EvrFsFAT_MediaInsertStatError (vol->DrvLet);
          return (fsNoMedia);
        }
        if (flags & FS_MEDIA_PROTECTED) {
          /* Write protection active */
          vol->Status &= ~FAT_STATUS_WRITE;
        } else {
          /* Write protection inactive */
          vol->Status |=  FAT_STATUS_WRITE;
        }
      }
    }
  }
  if (status & FAT_STATUS_INIT_MEDIA) {
    if (!(vol->Status & FAT_STATUS_INIT_MEDIA)) {
      /* Media not initialized */
      EvrFsFAT_MediaInitStatError (vol->DrvLet);
      return (fsMediaError);
    }
  }
  if (status & FAT_STATUS_READY) {
    if (!(vol->Status & FAT_STATUS_READY)) {
      /* Not ready */
      EvrFsFAT_VolumeReadyStatError (vol->DrvLet);
      return (fsAccessDenied);
    }
  }
  if (status & FAT_STATUS_MOUNT) {
    if (!(vol->Status & FAT_STATUS_MOUNT)) {
      /* Not mounted */
      EvrFsFAT_VolumeNotMounted (vol->DrvLet);
      return (fsAccessDenied);
    }
  }
  if (status & FAT_STATUS_WRITE) {
    if (!(vol->Status & FAT_STATUS_WRITE)) {
      /* Write protection is active */
      EvrFsFAT_VolumeWriteStatError (vol->DrvLet);
      return (fsAccessDenied);
    }
  }

  return (fsOK);
}


/**
  Find and return pointer to unused FAT file handle

  \param[in]  vol                       volume description structure

  \return     Pointer to file handle or NULL if all file handles used
*/
__WEAK int32_t fat_handle_get (fsFAT_Volume *vol) {
  fsFAT_Handle *fh;
  int32_t i;

  for (i = 0; i < fs_fat_fh_cnt; i++) {
    fh = &fs_fat_fh[i];

    if (!(fh->flags & FAT_HANDLE_OPEN)) {
      /* Clear File Control Block */
      memset (fh, 0, sizeof (fsFAT_Handle));
      fh->vol = vol;
      return (i);
    }
  }
  /* Handle not available */
  EvrFsFAT_FileHandleUnavailable (vol->DrvLet);

  return (-1);
}


/**
  Reset all file handles on the FAT volume

  \param[in]  vol                       volume description structure
*/
static void fat_handles_reset (fsFAT_Volume *vol) {
  fsFAT_Handle *fh;
  uint32_t i;

  /* Resetting FAT file handles */
  EvrFsFAT_ResetHandles (vol->DrvLet);

  /* Reset any opened files for this drive. */
  for (i = 0; i < fs_fat_fh_cnt; i++) {
    fh = &fs_fat_fh[i];

    if (fh->vol == vol) {
      /* Invalidate file handle */
      fh->flags = 0;
    }
  }
}


/**
  Check if file is in use by another handle

  This function will check all file handles available in the pool and compare
  handle flags with input flags. If any of input flags is set, function returns
  true.

  \param[in]  pos                       short entry position
  \param[in]  vol                       volume description structure

  \return     true if found opened file with specified flags set, false otherwise
*/
static bool file_locked (ENTRY_POS *pos, uint32_t flags, fsFAT_Volume *vol) {
  fsFAT_Handle *ext;
  uint32_t i;

  for (i = 0, ext = &fs_fat_fh[0]; i < fs_fat_fh_cnt; i++, ext++) {
    if ((ext->flags & FAT_HANDLE_OPEN) && (ext->vol == vol)) {
      /* Found another opened file on the same drive */
      if ((ext->short_ent_clus == pos->Clus) &&
          (ext->short_ent_offs == pos->Offs)) {
        /* Check if specified flags are set */
        if (ext->flags & flags) {
          EvrFsFAT_FileIsInUse (vol->DrvLet);
          return (true);
        }
      }
    }
  }
  return (false);
}


/**
  Find a string p within string s using case insensitive compare.

  String p in s must be separated using space, slash, carriage return,
  new line or null character.

  \param[in]  s                         string pointer
  \param[in]  p                         string pointer

  \return     a pointer to the located string, or a null pointer if the string
              is not found.
*/
static const char *find_param (const char *s, const char *p) {
  uint32_t i, m;

  while (s && *s) {
    m = 0;
    if (*p == *s) {
      m  = 1;
      for (s++, i = 1; m && p[i]; i++) {
        if (((p[i] ^ *s) & ~0x20)) {
          m = 0;
        } else {
          s++;
        }
      }
      if (m) {
        if (*s == ' ' || *s == '/' || *s == '\0' || *s == '\r' || *s == '\n') {
          return (s - i);
        }
      }
    } else {
      s++;
    }
  }
  return (NULL);
}


/**
  Converts current system time to file entry time stamp format.

  \param[in]  time                      pointer to FS_TIME structure containing current time
  \return     Packed time value
*/
static uint16_t format_time (fsTime *time) {
  uint16_t retv, v;

  /* Seconds: 0-29 (2-second count) */
  v = (time->sec >> 1) & 0x1F;
  if (v > 29) { v = 0; }
  retv = v;

  /* Minutes: 0-59 */
  v = time->min & 0x3F;
  if (v > 59) { v = 0; }
  retv |= (v << 5);

  /* Hours: 0-23 */
  v = time->hr & 0x1F;
  if (v > 23) { v = 23; }
  retv |= (v << 11);

  return (retv);
}


/**
  Converts current system date to file entry time stamp format.

  \param[in]  time                      Pointer to FS_TIME structure containing current date
  \return     Packed time value
*/
static uint16_t format_date (fsTime *time) {
  uint16_t retv, v;

  /* Days: 1-31 */
  v = time->day & 0x1F;
  if (v == 0) { v = 1; }
  retv = v;

  /* Month: 1-12 */
  v = time->mon & 0x0F;
  if (v > 12 || v == 0) { v = 1; }
  retv |= (v << 5);

  /* Year: 0-127 */
  v  = time->year - 1980;
  v &= 0x7F;
  retv |= (v << 9);

  return (retv);
}

/**
  Extract date information from FAT entry format.

  Note that only members day, month and year are populated in the fsTime structure.

  \param[in]  date                      date information in FAT entry format
  \param[out] td                        fsTime structure where date will be stored
*/
static void extract_date (uint16_t date, fsTime *td) {
  /* Days: 1-31 */
  td->day  = (date & 0x1F);

  /* Month: 1-12 */
  td->mon  = (date >> 5) & 0x0F;

  /* Year: 0-127 */
  td->year = ((date >> 9) & 0x7F) + 1980;
}

/**
  Extract time information from FAT entry format.

  Note that only members hour, minute and second are populated in the fsTime structure.

  \param[in]  time                      time information in FAT entry format
  \param[out] td                        fsTime structure where time will be stored
*/
static void extract_time (uint16_t time, fsTime *td) {
    /* Hours: 0-23 */
  td->hr  = (time >> 11);

  /* Minutes: 0-59 */
  td->min = (time >>  5) & 0x3F;

  /* Seconds: 0-29 (2-second count) */
  td->sec = (uint8_t)((time & 0x1F) << 1);
}

/**
  Extract cluster number from FAT entry format.

  \parma[in] sfne                       short directory entry

  \return    Cluster number
*/
static uint32_t extract_clus (FILEREC *sfne) {
  uint32_t clus;

  clus  = (uint32_t)(get_u16((uint8_t *)&sfne->FirstClusHI) << 16);
  clus |= (uint32_t)(get_u16((uint8_t *)&sfne->FirstClusLO));

  return (clus);
}

/**
  Extract name from the short entry

  \param[out] dst                       file name buffer
  \param[in]  src                       short name as stored in the SFN entry

  \return     Length of the extracted name (including dot if extension exists)
*/
static uint32_t sfn_extract (char *dst, const uint8_t *src) {
  uint32_t i, j;

  /* Eliminate trailing space characters from a name */
  for (j = 8; j; j--) {
    if (src[j-1] != ' ') { break; }
  }
  /* Copy name and add dot with extension */
  for (i = 0; i < j; i++) {
    dst[i] = src[i];
  }

  if (src[8] != ' ') {
    /* Extension exists - add dot to the name */
    dst[j++] = '.';

    /* Copy extension */
    for (i = 8; i < 11; i++) {
      if (src[i] == ' ') { break; }
      dst[j++] = src[i];
    }
  }
  dst[j] = '\0';
  return (j);
}


/**
  Embed name into the short entry

  \param[out] dst                       pointer to FileName field in the SFN entry
  \param[in]  src                       name to embed into SFN entry
*/
static void sfn_embed (char *dst, const char *src) {
  char nn[11];
  char ch;
  uint32_t i;

  /* Fill entry name with spaces */
  for (i = 0; i < 11; i++) {
    nn[i] = ' ';
  }

  for (i = 0; *src && *src != '.'; i++) {
    nn[i] = *src++;
  }
  if (*src == '.') {
    src++;
    /* Copy extension */
    for (i = 8; *src; i++) {
      nn[i] = *src++;
    }
  }

  /* Copy entry name to destination */
  for (i = 0; i < 11; i++) {
    ch = nn[i];
    if (ch >= 'a' && ch <= 'z') {
      /* Convert character to uppercase */
      ch &= ~0x20;
    }
    dst[i] = ch;
  }
}


/**
  Calculate CRC32 from a given fn its length and given seed
  Lowercase characters in fn are converted to uppercase

  \param[in]  fn                        pointer to name
  \param[in]  fn_len                    name length
  \param[in]  crc32                     seed for CRC32 calcualtion

  \return     Calculated CRC32
*/
static uint32_t fat_crc32 (const char *fn, uint32_t fn_len, uint32_t crc32) {
  static const uint32_t crc32_table[256] = {
    0x00000000,0x04C11DB7,0x09823B6E,0x0D4326D9,0x130476DC,0x17C56B6B,0x1A864DB2,0x1E475005,
    0x2608EDB8,0x22C9F00F,0x2F8AD6D6,0x2B4BCB61,0x350C9B64,0x31CD86D3,0x3C8EA00A,0x384FBDBD,
    0x4C11DB70,0x48D0C6C7,0x4593E01E,0x4152FDA9,0x5F15ADAC,0x5BD4B01B,0x569796C2,0x52568B75,
    0x6A1936C8,0x6ED82B7F,0x639B0DA6,0x675A1011,0x791D4014,0x7DDC5DA3,0x709F7B7A,0x745E66CD,
    0x9823B6E0,0x9CE2AB57,0x91A18D8E,0x95609039,0x8B27C03C,0x8FE6DD8B,0x82A5FB52,0x8664E6E5,
    0xBE2B5B58,0xBAEA46EF,0xB7A96036,0xB3687D81,0xAD2F2D84,0xA9EE3033,0xA4AD16EA,0xA06C0B5D,
    0xD4326D90,0xD0F37027,0xDDB056FE,0xD9714B49,0xC7361B4C,0xC3F706FB,0xCEB42022,0xCA753D95,
    0xF23A8028,0xF6FB9D9F,0xFBB8BB46,0xFF79A6F1,0xE13EF6F4,0xE5FFEB43,0xE8BCCD9A,0xEC7DD02D,
    0x34867077,0x30476DC0,0x3D044B19,0x39C556AE,0x278206AB,0x23431B1C,0x2E003DC5,0x2AC12072,
    0x128E9DCF,0x164F8078,0x1B0CA6A1,0x1FCDBB16,0x018AEB13,0x054BF6A4,0x0808D07D,0x0CC9CDCA,
    0x7897AB07,0x7C56B6B0,0x71159069,0x75D48DDE,0x6B93DDDB,0x6F52C06C,0x6211E6B5,0x66D0FB02,
    0x5E9F46BF,0x5A5E5B08,0x571D7DD1,0x53DC6066,0x4D9B3063,0x495A2DD4,0x44190B0D,0x40D816BA,
    0xACA5C697,0xA864DB20,0xA527FDF9,0xA1E6E04E,0xBFA1B04B,0xBB60ADFC,0xB6238B25,0xB2E29692,
    0x8AAD2B2F,0x8E6C3698,0x832F1041,0x87EE0DF6,0x99A95DF3,0x9D684044,0x902B669D,0x94EA7B2A,
    0xE0B41DE7,0xE4750050,0xE9362689,0xEDF73B3E,0xF3B06B3B,0xF771768C,0xFA325055,0xFEF34DE2,
    0xC6BCF05F,0xC27DEDE8,0xCF3ECB31,0xCBFFD686,0xD5B88683,0xD1799B34,0xDC3ABDED,0xD8FBA05A,
    0x690CE0EE,0x6DCDFD59,0x608EDB80,0x644FC637,0x7A089632,0x7EC98B85,0x738AAD5C,0x774BB0EB,
    0x4F040D56,0x4BC510E1,0x46863638,0x42472B8F,0x5C007B8A,0x58C1663D,0x558240E4,0x51435D53,
    0x251D3B9E,0x21DC2629,0x2C9F00F0,0x285E1D47,0x36194D42,0x32D850F5,0x3F9B762C,0x3B5A6B9B,
    0x0315D626,0x07D4CB91,0x0A97ED48,0x0E56F0FF,0x1011A0FA,0x14D0BD4D,0x19939B94,0x1D528623,
    0xF12F560E,0xF5EE4BB9,0xF8AD6D60,0xFC6C70D7,0xE22B20D2,0xE6EA3D65,0xEBA91BBC,0xEF68060B,
    0xD727BBB6,0xD3E6A601,0xDEA580D8,0xDA649D6F,0xC423CD6A,0xC0E2D0DD,0xCDA1F604,0xC960EBB3,
    0xBD3E8D7E,0xB9FF90C9,0xB4BCB610,0xB07DABA7,0xAE3AFBA2,0xAAFBE615,0xA7B8C0CC,0xA379DD7B,
    0x9B3660C6,0x9FF77D71,0x92B45BA8,0x9675461F,0x8832161A,0x8CF30BAD,0x81B02D74,0x857130C3,
    0x5D8A9099,0x594B8D2E,0x5408ABF7,0x50C9B640,0x4E8EE645,0x4A4FFBF2,0x470CDD2B,0x43CDC09C,
    0x7B827D21,0x7F436096,0x7200464F,0x76C15BF8,0x68860BFD,0x6C47164A,0x61043093,0x65C52D24,
    0x119B4BE9,0x155A565E,0x18197087,0x1CD86D30,0x029F3D35,0x065E2082,0x0B1D065B,0x0FDC1BEC,
    0x3793A651,0x3352BBE6,0x3E119D3F,0x3AD08088,0x2497D08D,0x2056CD3A,0x2D15EBE3,0x29D4F654,
    0xC5A92679,0xC1683BCE,0xCC2B1D17,0xC8EA00A0,0xD6AD50A5,0xD26C4D12,0xDF2F6BCB,0xDBEE767C,
    0xE3A1CBC1,0xE760D676,0xEA23F0AF,0xEEE2ED18,0xF0A5BD1D,0xF464A0AA,0xF9278673,0xFDE69BC4,
    0x89B8FD09,0x8D79E0BE,0x803AC667,0x84FBDBD0,0x9ABC8BD5,0x9E7D9662,0x933EB0BB,0x97FFAD0C,
    0xAFB010B1,0xAB710D06,0xA6322BDF,0xA2F33668,0xBCB4666D,0xB8757BDA,0xB5365D03,0xB1F740B4
  };
  char ch;
  uint32_t i = fn_len;

  while (i--) {
    ch = fn[i];

    if (ch >= 'a' && ch <= 'z') {
      /* Convert to uppercase */
      ch &= ~0x20;
    }
    /* Get CRC32 from the lookup table */
    crc32 = (crc32 << 8) ^ crc32_table[(crc32 >> 24) ^ ch];
  }
  return (crc32);
}


/**
  Calculate hash number from given name of specified length
  Lowercase characters are converted into uppercase

  \param[in]  fn                        pointer to name
  \param[in]  fn_len                    name length

  \return     Calculated hash
*/
static uint32_t name_hash (const char *fn, uint32_t fn_len) {
  uint32_t  hash;

  hash = fat_crc32 (fn, fn_len, 0xFFFFFFFF);
  return (hash);
}


/**
  Calculate hash number from the name stored in a short entry
  First parameter is FILEREC * therefore all characters are uppercase

  \param[in]  frec
  \param[in]  hash

  \return     Calculated hash
*/
static uint32_t short_ent_hash (FILEREC *frec, uint32_t hash) {
  uint32_t len;
  char  fn[13];

  len = sfn_extract (fn, frec->FileName);

  hash = fat_crc32 (fn, len, 0xFFFFFFFF);
  return (hash);
}


#ifdef FS_FAT_LFN
/**
  Calculate hash number from the long filename
  \param[in]  frec                      LFN entry component
  \param[in]  hash                      hash calculation seed

  \return     Calculated hash
*/
static uint32_t long_ent_hash (LFN_FILEREC *frec, uint32_t hash) {
  uint32_t i;
  char ch;
  char fn[13];
  char *lfn;

  lfn = (char *)frec;
  i = 0;

  while (i < 13) {
    ch = lfn[ChIndex[i]];
    if (ch == 0) { break; }
    fn[i++] = ch;
  }

  hash = fat_crc32 (fn, i, hash);
  return (hash);
}
#endif


#ifdef FS_FAT_LFN
/**
  Copy LFN name to an entry

  \param[in]  lfnr                      long entry pointer
  \param[in]  fn                        name buffer
  \param[in]  len                       number of characters to copy
*/
static void lfn_name_copy (LFN_FILEREC *lfnr, const char *fn, uint32_t len) {
  char *ln;
  uint8_t val;
  uint32_t i;

  ln = (char *)lfnr;

  for (i = 0; i < len; i++) {
    ln[ChIndex[i]]   = fn[i];
    ln[ChIndex[i]+1] = 0;
  }
  for (val = 0; i < 13; val = 0xFF, i++) {
    ln[ChIndex[i]]   = val;
    ln[ChIndex[i]+1] = val;
  }
}
#endif


#ifdef FS_FAT_LFN
/**
  Compare len of fn with chars in long entry name (case insensitive).

  \param[in]  lfr                       LFN entry component
  \param[in]  fn                        file name to compare with

  \return
*/
static bool lfn_name_cmp (LFN_FILEREC *lfnr, const char *fn, uint32_t len) {
  const char *ln;
  uint32_t i;

  ln = (const char *)lfnr;

  for (i = 0; i < len; i++) {
    if ((ln[ChIndex[i]] ^ fn[i]) & ~0x20) {
      /* Characters different. */
      return (false);
    }
  }
  if (len < 13) {
    if (ln[ChIndex[i]] != '\0') {
      return (false);
    }
  }
  return (true);
}
#endif


#ifdef FS_FAT_LFN
/**
  Determine number of name characters in long entry

  Number of characters can be 13 if long name length is multiple of 13
  or less if name is NUL terminated and padded with 0xFFFF characters.

  \param[in]  lfn                       long entry name buffer

  \return     Number of characters in long name entry
*/
static uint32_t lfn_char_cnt (LFN_FILEREC *lfnr) {
  uint8_t *ln;
  uint8_t  ch;
  uint32_t i;

  ln = (uint8_t *)lfnr;

  for (i = 0; i < 13; i++) {
    ch = ln[ChIndex[i]];

    if ((ch == '\0') || (ch == 0xFF)) {
      break;
    }
  }
  return (i);
} 
#endif

#ifdef FS_FAT_LFN
/**
  Extract name characters from long entry

  \param[out] dst                       destination buffer
  \param[in]  lfnr                      long entry
  \param[in]  cnt                       number of characters to extract
*/
static void lfn_extract (char *dst, LFN_FILEREC *lfnr, uint32_t cnt) {
  const char *ln;
  uint32_t    i;

  ln = (const char *)lfnr;

  for (i = 0; i < cnt; i++) {
    dst[i] = ln[ChIndex[i]];
  }
}
#endif


#ifdef FS_FAT_LFN
/**
  Calculate short entry name checksum which is placed in every long entry.
*/
static uint8_t sn_chksum (char *fn) {
  uint32_t i;
  uint8_t  sum;

  sum = 0;
  for (i = 0; i < 11; i++) {
    sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + fn[i];
  }
  return (sum);
}
#endif


/**
  Validate label character

  \param[in]  ch                        ASCII character
  \return     ch if character is valid FAT label character, zero otherwise
*/
static char val_char_lab (char ch) {

  if ((ch >= '0' && ch <= '9') ||
      (ch >= 'A' && ch <= 'Z') ||
      (ch == '_') || (ch == ' ')) {
    return (ch);
  }
  if (ch >= 'a' && ch <= 'z') {
    /* Convert to uppercase. */
    return (ch & ~0x20);
  }
  /* This is not a valid disk label character. */
  return (0);
}


/**
  Check if input string specifies a valid FAT label

  \param[in]  label                     character string
  \return     Label length or -1 if label invalid
*/
static int32_t validate_label (const char *label) {
  uint32_t i;
  int32_t len;
  char ch;

  i   = 0;
  len = 0;
  do {
    ch = label[i++];

    if ((ch == '/') || (ch == '\0')) {
      /* Terminating character detected */
      break;
    }

    if (i < 12) {
      if (val_char_lab (ch) != 0) {
        len++;
      }
      else {
        len = -1;
      }
    }
    else {
      /* Check for trailing spaces */
      if (ch != ' ') {
        len = -1;
      }
    }
  }
  while (len != -1);

  /* Return label length or -1 if label invalid */
  return (len);
}


/**
  Validate if input character is valid FAT character

  \param[in]  ch                        input character

  \return     - 0: character is invalid
              - 1: character is valid SFN character
              - 2: character is valid LFN character
*/
static uint32_t char_validate (char ch) {
  uint32_t val = 0;

  /* Check if this is valid FAT character */
  if ((ch >= ' ' && ch <= '!') ||
      (ch >= '#' && ch <= ')') ||
      (ch == '-')              ||
      (ch == '.')              ||
      (ch >= '0' && ch <= '9') ||
      (ch == '@')              ||
      (ch >= 'A' && ch <= 'Z') ||
      (ch >= '^' && ch <= '{') ||
      (ch == '}')              ||
      (ch == '~')               ) {

    val = 1;
  }
#ifdef FS_FAT_LFN
  if ((ch == '+')              ||
      (ch == ',')              ||
      (ch == ';')              ||
      (ch == '=')              ||
      (ch == '[')              ||
      (ch == ']')              ||
      (ch >  127)               ) {

    val = 2;
  }
#endif
  return (val);
}


/**
  Validate input name for characters used

  \param[in]  name                      character array containing file or directory name
  \param[in]  len                       name length (without the <NUL> character)
  \return validation status: 0 if specified name is invalid, >0 otherwise
*/
static uint32_t name_validate (const char *name, uint32_t len) {
  uint32_t i, r;
  char ch;

  r = 0;
  for (i = 0; i < len; i++) {
    ch = name[i];

    r = char_validate(ch);

    if (r == 0) {
      /* Invalid character found */
      EvrFsFAT_PathInvalidChar (ch);
      break;
    }
  }

  return (r);
}


/**
  Validate input path for length and characters used.

  Microsoft specifies MAX_PATH as: drive letter, colon, backslash, name components
  separated by backslashes, and a terminating null character.

  The parameter path can start with the backslash following by name components and
  therefore we determine the max path len by excluding drive letter and a colon.

  \param[in]  path                      character string containing path information

  \return     execution status: 0 = input path is FAT valid
                                1 = input path is invalid
*/
static uint32_t path_validate (const char *path) {
  char     ch;
  uint32_t i, n, len, max_len, err;

  if (path[0] == '\0') {
    /* Path is empty string */
    err = 1U;
  }
  else {
    /* Check path */
    max_len = FAT_MAX_PATH - 3;

    i   = 0U;
    len = 0U;
    err = 0U;

    do {
      ch = path[len];

      if ((ch == '\\') || (ch == '/') || (ch == '\0')) {
        n = len - i;

        if (n != 0) {
          if (name_validate (&path[i], n) == 0U) {
            /* Name contains invalid character */
            err = 1U;
          }
        }
        i = len + 1U;
      }

      len++;

      if (len > max_len) {
        /* Path is too long */
        EvrFsFAT_PathIsTooLong (max_len);
        err = 1U;
      }

      if (err != 0U) {
        break;
      }
    }
    while (ch != '\0');
  }

  return (err);
}


#ifdef FS_FAT_LFN
/**
  Insert numeric tail into input name string.

  Numeric tail if inserted before the extension and aligned so that it fits
  into the 8.3 format.

  \param[in,out] fn                     name string
  \param[in]     num                    number to append to the name

  \return     true if numeric tail generated, false if number to high
*/
static bool name_nt_gen (char *fn, uint32_t num) {
  char     ext[3];
  uint32_t i, j;
  uint32_t p;
  uint32_t val;
  uint32_t digits;

  if (num > 999999) {
    /* Numeric tail ranges from ~1 to ~999999 */
    return false;
  }

  /* Determine how many digits are needed */
  if      (num > 99999) { digits = 6; }
  else if (num >  9999) { digits = 5; }
  else if (num >   999) { digits = 4; }
  else if (num >    99) { digits = 3; }
  else if (num >     9) { digits = 2; }
  else                  { digits = 1; }

  /* Get current period position */
  p = 0;
  for (i = 0; fn[i]; i++) {
    if (fn[i] == '.') {
      /* Extension found */
      p = i;
      /* Copy extension into buffer */
      for (j = 0, i++; fn[i]; j++, i++) {
        ext[j] = fn[i];
      }
      i = p;
      break;
    }
  }

  /* Determine tilde position */
  while ((i + digits) >= 8) { i--; }

  /* Append numeric tail */
  fn[i++] = '~';

  switch (digits) {
    case 6:
      val            = num / 100000;
      num           -= val * 100000;
      fn[i++]  = (char)('0' + val);
      __FALLTHROUGH;
    case 5:
      val            = num /  10000;
      num           -= val *  10000;
      fn[i++]  = (char)('0' + val);
      __FALLTHROUGH;
    case 4:
      val            = num /   1000;
      num           -= val *   1000;
      fn[i++]  = (char)('0' + val);
      __FALLTHROUGH;
    case 3:
      val            = num /    100;
      num           -= val *    100;
      fn[i++]  = (char)('0' + val);
      __FALLTHROUGH;
    case 2:
      val            = num /     10;
      num           -= val *     10;
      fn[i++]  = (char)('0' + val);
      __FALLTHROUGH;
    case 1:
      fn[i++]  = (char)('0' + num);
      break;
  }

  /* Append extension */
  if (p) {
    fn[i++] = '.';
    j = 0;
    while (j < 3) {
      fn[i++] = ext[j++];
    }
  }
  /* Name string NUL termination */
  fn[i] = '\0';

  return (true);
}
#endif


#ifdef FS_FAT_LFN
/**
  Generate basis name from the input name

  Basis name buffer (bn) must be at least 13 characters long. Generated name
  will be NULL terminated.

  \param[in]  fn                        input name
  \param[in]  fn_len                    input name length
  \param[out] bn                        basis name buffer

  \return     true if lossy conversion, false otherwise
*/
static bool name_basis_gen (const char *fn, uint32_t fn_len, char *bn) {
  bool     lossy; /* Lossy conversion flag */
  char     ch;
  uint32_t len;
  uint32_t i;
  uint32_t p;

  /* Find last period in the input name */
  p = 0;
  i = fn_len;
  while (!p && i--) {
    if (fn[i] == '.') {
      p = i;
    }
  }

  i = 0;
  /* Strip leading spaces */
  while (fn[i] == ' ' && i < fn_len) { i++; }
  /* Strip leading periods */
  while (fn[i] == '.' && i < fn_len) { i++; }

  len   = 0;
  lossy = false;

  while (i < fn_len && len < 8) {
    ch = fn[i];

    if (ch == '.') {
      if (i == p) {
        /* Last period in the input name */
        break;
      }
    }
    else {
      /* Skip all embedded spaces */
      if (ch != ' ') {
        if (len < 8) {
          if (char_validate (ch) != 1) {
            *bn = '_';
            lossy = true;
          }
          else {
            *bn = ch;
          }
          bn++;
          len++;
        }
      }
    }
    i++;
  }

  if (p) {
    /* Append extension */
    len = 0;
    while (p < fn_len && len++ < 4) {
      *bn++ = fn[p++];
    }
  }
  *bn = '\0';
  return (lossy);
}
#endif


/**
  Analyze input name

  Specified flags are set if name contains:
   - SFN: valid SFN characters, is in 8.3 format and is only upper/lower case.
   - LFN: valid LFN characters which are not allowed in SFN, if name is not
          in 8.3 format or is mixedcase.
   - WILDCARD: wildcard character * (asterisk) or ? (question mark)
   - DIR: directory separator '\\' or '/'
   - LAST: null terminated character at the end

  \param[in]  fn                        name buffer
  \param[in]  fn_len                    name length

  \return     - =0: input name invalid
              - >0: analyze result bit mask
*/
static uint8_t name_analyse (const char *fn, uint32_t fn_len) {
#ifdef FS_FAT_LFN
  char     pch; /* Previous character in a name */
#endif
  char     ch;  /* Current character in a name */
  uint8_t  res; /* Analysis result -> output flags */
  uint8_t  dot; /* Number of periods in a name */
  uint32_t ext; /* Extension length */
  uint32_t len;

  res = FAT_NAME_SFN |
        #ifdef FS_FAT_LFN
        FAT_NAME_LFN |
        #endif
        0;
  dot = 0;
  ext = 0;
#ifdef FS_FAT_LFN
  pch = 0;
#endif

  for (len = 1; len <= fn_len; len++) {
    ch = fn[len-1];

    if (ch == '*' || ch == '?') {
      res |= FAT_NAME_WILDCARD;
    }

    /* Check if current character valid */
    #ifdef FS_FAT_LFN
    if (char_validate (ch) == 2) {
      /* Invalid SFN character */
      res &= ~FAT_NAME_SFN;
    }
    #endif

    if (dot) {
      ext++;
      if (ext > 3) {
        /* Extension is longer than 3 characters, not SFN name */
        #ifdef FS_FAT_LFN
        res &= ~FAT_NAME_SFN;
        #else
        return (0);
        #endif
      }
    }

    if (ch == '.') {
      dot++;

      if (fn_len == 1) {
        res |= FAT_NAME_DOT;
      }
      else if ((fn_len == 2) && (dot == 2)) {
        res |= FAT_NAME_DOTDOT;
      }
      else {
        if (dot > 1) {
          /* Dot exists in name more than once: only in LFN */
          #ifdef FS_FAT_LFN
            res &= ~FAT_NAME_SFN;
          #else
            /* Multiple dots are not allowed in SFN */
            return (0);
          #endif
        }
      }
    }
    #ifdef FS_FAT_LFN
    else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
      if (pch) {
        if ((pch ^ ch) & 0x20) {
          /* Mixed case characters found in a name */
          res &= ~FAT_NAME_SFN;
        }
      }
      pch = ch;
    }
    #endif

    if (len > 8) {
      if (!dot) {
        #ifdef FS_FAT_LFN
        res &= ~FAT_NAME_SFN;
        #else
        /* Name without extension must consist of 8 characters for SFN */
        return (0);
        #endif
      }
    }
  }
  return (res);
}


/**
  Strip a name out of specified path.

  \param[in]  fn                        path pointer
  \param[out] len                       pointer to length variable
  \param[out] flags                     pointer to name flags variable

  \return     Pointer to first name character
*/
static const char *name_strip (const char *fn, uint8_t *len, uint8_t *flags) {
  const char *ns, *nn;
  uint8_t sz;

  *flags = 0;

  if ((*fn == '\\') || (*fn == '/')) {
    fn++;
  }
  sz = 0;
  ns = fn;

  while (*fn && (*fn != '\\') && (*fn != '/')) {
    fn++;
    sz++;
  }
  *len = sz;

  nn = ns + sz;

  if (*nn == '\\' || *nn == '/') {
    *flags |= FAT_NAME_DIR;
  }
  if (*nn == '\0' || *(nn + 1) == '\0') {
    *flags |= FAT_NAME_LAST;
  }
  return (ns);
}


/**
  Read a 512 byte sector from the media.
*/
static uint32_t read_sector (fsFAT_Volume *vol, uint32_t sect) {

  if (sect == vol->ca.sect) {
    /* Required sector already in buffer. */
    return (true);
  }

  if (sect >= vol->ca.csect && sect < (vol->ca.csect + vol->ca.nwr)) {
    /* This sector is in write cache buffer. */
    memcpy (vol->ca.buf, vol->ca.cbuf + (sect - vol->ca.csect) * 512, 512);
    vol->ca.sect = sect;
    return (true);
  }

  /* Sector not in cache, read it from Device. */
  if (vol->Drv->ReadSect (sect, vol->ca.buf, 1) == true) {
    vol->ca.sect = sect;
    return (true);
  }
  vol->ca.sect = INVAL_SECT;

  /* Sector read failed */
  EvrFsFAT_SectorReadFailed (vol->DrvLet, sect, 1);
  return (false);
}


/**
  Write a 512 byte sector to the media.
*/
static uint32_t write_sector (fsFAT_Volume *vol, uint32_t sect, uint32_t act) {
  uint32_t rtv;

  if (vol->fsj && act == ACT_USEJOUR) {
    /* Write sector with journal */
    rtv = fsj_write (vol->fsj, sect, 0, vol->ca.buf);
  }
  else {
    /* Write sector */
    rtv = vol->Drv->WriteSect (sect, vol->ca.buf, 1);
  }

  if (rtv) {
    vol->ca.sect = sect;
    return (true);
  }

  /* Sector write failed */
  EvrFsFAT_SectorWriteFailed (vol->DrvLet, sect, 1);
  return (false);
}


/**
  Read a 512 byte sector through cache.
*/
static uint32_t read_cache (fsFAT_Volume *vol, uint32_t sect, uint32_t cnt) {

  if ((vol->CaSize == 0) || (vol->ca.nwr > 0)) {
    /* File Caching switched off or write caching active. */
    return (read_sector (vol, sect));
  }

  if (vol->ca.nrd > 0) {
    if ((vol->ca.csect <= sect) && sect < (vol->ca.csect + vol->ca.nrd)) {
      /* Requested sector is already cached. */
      memcpy (vol->ca.buf, vol->ca.cbuf + (sect - vol->ca.csect) * 512, 512);
      vol->ca.sect = sect;
      return (true);
    }
  }

  if (cnt > vol->CaSize) {
    cnt = vol->CaSize;
  }

  /* Sector not in cache, read it from the Memory Card. */
  if (vol->Drv->ReadSect (sect, vol->ca.buf, cnt) == true) {
    vol->ca.sect  = sect;
    /* First sector is used, the rest is cached. */
    vol->ca.csect = sect + 1;
    vol->ca.nrd   = (uint8_t)cnt - 1;
    return (true);
  }
  vol->ca.sect = INVAL_SECT;
  vol->ca.nrd  = 0;

  /* Sector read failed */
  EvrFsFAT_SectorReadFailed (vol->DrvLet, sect, cnt);
  return (false);
}


/**
  Write a 512 byte sector through cache.
*/
static uint32_t write_cache (fsFAT_Volume *vol, uint32_t sect) {

  if (vol->CaSize == 0) {
    /* File Caching switched off. */
    if (sect) {
      if (write_sector (vol, sect, ACT_NONE) == false) {
        return (false);
      }
    }
    return (true);
  }
  if (sect == 0) {
    /* Flush cache request. */
    if (vol->ca.nwr) {
      if (vol->Drv->WriteSect (vol->ca.csect, vol->ca.cbuf, vol->ca.nwr) == false) {
        /* Sector write failed */
        EvrFsFAT_SectorWriteFailed (vol->DrvLet, vol->ca.csect, vol->ca.nwr);
        return (false);
      }
      vol->ca.nwr = 0;
    }
    return (true);
  }
  vol->ca.sect = sect;
  if (vol->ca.nwr > 0) {
    if (sect == (vol->ca.csect + vol->ca.nwr) && vol->ca.nwr < vol->CaSize) {
      /* Next sector is continuous, still space in cache. */
      memcpy (vol->ca.cbuf + (vol->ca.nwr * 512), vol->ca.buf, 512);
      vol->ca.nwr++;
      return (true);
    }
    else if (sect >= (vol->ca.csect ) && sect < (vol->ca.csect + vol->ca.nwr)) {
      /* Sector is already in the cache */
      memcpy (vol->ca.cbuf + ((sect - vol->ca.csect) * 512), vol->ca.buf, 512);
      return (true);
    }
    /* Not continuous sector or buffer full, flush the cache. */
    if (vol->Drv->WriteSect (vol->ca.csect, vol->ca.cbuf, vol->ca.nwr) == false) {
      /* Sector write failed */
      EvrFsFAT_SectorWriteFailed (vol->DrvLet, vol->ca.csect, vol->ca.nwr);
      return (false);
    }
    vol->ca.nwr = 0;
  }
  /* Write Data cache is empty. */
  memcpy (vol->ca.cbuf, vol->ca.buf, 512);
  vol->ca.csect = sect;
  vol->ca.nwr   = 1;
  vol->ca.nrd   = 0;
  return (true);
}


/**
  Handle FAT Cache. Read/write a 512 byte FAT sector.
*/
static uint32_t cache_fat (fsFAT_Volume *vol, uint32_t sect) {
  uint32_t ofs;

  if (sect == vol->fat.sect) {
    /* Required sector already in buffer. */
    return (true);
  }
  if (vol->fat.dirty == true) {
    /* Current FAT sector has been changed, write it first. */

    if (vol->fsj && vol->Status & FAT_STATUS_JOURACT) {
      /* Write FAT with journal */
      ofs = (vol->cfg.NumOfFat == 2) ? (vol->fat.sect + vol->cfg.FatSize) : (0);
      if (fsj_write (vol->fsj, vol->fat.sect, ofs, vol->fat.buf) == false) {
        return (false);
      }
    }
    else {
      /* Write FAT without journal */
      if (vol->Drv->WriteSect (vol->fat.sect, vol->fat.buf, 1) == false) {
        /* Sector write failed */
        EvrFsFAT_SectorWriteFailed (vol->DrvLet, vol->fat.sect, 1);
        return (false);
      }

      if (vol->cfg.NumOfFat == 2) {
        /* Write copy of FAT */
        if (vol->Drv->WriteSect (vol->fat.sect+vol->cfg.FatSize, vol->fat.buf, 1) == false) {
          /* Sector write failed */
          EvrFsFAT_SectorWriteFailed (vol->DrvLet, vol->fat.sect+vol->cfg.FatSize, 1);
          return (false);
        }
      }
    }

    vol->fat.dirty = false;
  }
  if (sect == 0) {
    /* Only write if sector updated. */
    return (true);
  }

  /* Set sector offset to select FAT */
  ofs = vol->fat.cfat * vol->cfg.FatSize;

  if (vol->Drv->ReadSect (sect+ofs, vol->fat.buf, 1) == true) {
    vol->fat.sect = sect;
    return (true);
  }
  /* Sector read failed */
  EvrFsFAT_SectorReadFailed (vol->DrvLet, sect+ofs, 1);

  vol->fat.sect = INVAL_SECT;
  return (false);
}


/**
  Calculate absolute FAT Sector address from cluster address.
*/
static uint32_t get_fat_sect (FATINFO *fvi, uint32_t clus) {
  uint32_t fats;

  switch (fvi->FatType) {
    case FS_FAT12:
      /* FAT Cluster width 12 bits. */
      fats = (clus * 3) / 1024;
      break;

    case FS_FAT16:
      /* FAT Cluster width 16 bits. */
      fats = clus / 256;
      break;

    case FS_FAT32:
      /* FAT Cluster width 32 bits. */
      fats = clus / 128;
      break;

    default:
      /* Not supported. */
      return (0);
  }
  if (fats >= fvi->FatSize) {
    return (0);
  }
  fats += (fvi->BootSector + fvi->RsvdSecCnt);
  return (fats);
}


/**
  Retrieve root cluster number

  \param[in]  vol                       volume description structure
*/
static uint32_t get_root_clus (fsFAT_Volume *vol) {
  if (vol->cfg.FatType == FS_FAT32) {
    /* Root directory cluster on FAT32 */
    return (vol->cfg.FAT32_RootClus);
  }
  else {
    /* Root directory cluster on FAT12 or FAT16 */
    return (0);
  }
}


/**
  Calculate absolute sector address from the cluster number
*/
static uint32_t clus_to_sect (FATINFO *fvi, uint32_t clus) {
  uint32_t sect;

  /* Root directory first sector for FAT12 or FAT16 */
  sect = fvi->BootSector + fvi->RootDirAddr;
  if (clus == 0) {
    if (fvi->FatType != FS_FAT32) {
      /* Root cluster for FAT12 or FAT16 */
      return sect;
    }
  }

  if (fvi->FatType != FS_FAT32) {
    /* First sector of data region for FAT12 or FAT16 */
    sect += fvi->RootSecCnt;
  }
  /* First sector of given cluster in data region */
  sect += (clus - 2) * fvi->SecPerClus;
  return (sect);
}


/**
  Return an End Of Chain Marker.
*/
static uint32_t get_EOC (uint32_t fat_type) {
  switch (fat_type) {
    case FS_FAT12: return (0x00000FFF);
    case FS_FAT16: return (0x0000FFFF);
    case FS_FAT32: return (0x0FFFFFFF);

    default: return (0);
  }
}


/**
  Return "Cluster Reserved" Marker.
*/
static uint32_t get_RSVD (uint32_t fat_type) {
  switch (fat_type) {
    case FS_FAT12: return (0x00000FF7);
    case FS_FAT16: return (0x0000FFF7);
    case FS_FAT32: return (0x0FFFFFF7);

    default: return (0);
  }
}


/**
  Read FAT entry value for given cluster

  \param[in]  clus                      cluster number
  \param[out] val                       FAT entry value
  \param[in]  vol                       volume description structure
*/
static uint32_t alloc_table_read (uint32_t clus, uint32_t *val, fsFAT_Volume *vol) {
  uint32_t sect, offs, link;

  /* Read a part of FAT table to buffer. */
  sect = get_fat_sect (&vol->cfg, clus);
  if (cache_fat (vol, sect) == false) {
    return (false);
  }

  switch (vol->cfg.FatType) {
    case FS_FAT12:
      offs  = ((clus * 3) / 2) & 0x1FF;
      if (offs < 511) {
        link = get_u16 (&vol->fat.buf[offs]);
      }
      else {
        /* This cluster spans on two sectors in the FAT. */
        link = vol->fat.buf[511];
        if (cache_fat (vol, sect+1) == false) {
          return (false);
        }
        link |= (uint32_t)(vol->fat.buf[0] << 8);
      }
      if (clus & 0x001) {
        link >>= 4;
      }
      else {
        link &= 0xFFF;
      }
      break;

    case FS_FAT16:
      offs  = (clus & 0xFF) << 1;
      link = get_u16 (&vol->fat.buf[offs]);
      break;

    case FS_FAT32:
      offs  = (clus & 0x7F) << 2;
      link = get_u32 (&vol->fat.buf[offs]);
      break;

    default:
      return (false);
  }
  *val = link;
  return (true);
}


/**
  Write FAT entry value for given cluster
*/
static uint32_t alloc_table_write (uint32_t clus, uint32_t val, fsFAT_Volume *vol) {
  uint32_t sect, link, offs;
  uint16_t temp;

  sect = get_fat_sect (&vol->cfg, clus);
  if (cache_fat (vol, sect) == false) {
    return (false);
  }

  link = val;

  switch (vol->cfg.FatType) {
    case FS_FAT12:
      link &= 0x0FFF;
      offs  = ((clus * 3) / 2) & 0x1FF;
      if (offs < 511) {
        temp = get_u16 (&vol->fat.buf[offs]);
        if (clus & 0x001) {
          temp = (uint8_t)((temp & 0x000F) | (link << 4));
        }
        else {
          temp = (uint8_t)((temp & 0xF000) | link);
        }
        set_u16 (&vol->fat.buf[offs], temp);
      }
      else {
        /* This cluster spans on two sectors in the FAT. */
        if (clus & 0x001) {
          vol->fat.buf[511] = (uint8_t)((vol->fat.buf[511] & 0x0F) | (link << 4));
        }
        else {
          vol->fat.buf[511] = (uint8_t)link;
        }
        vol->fat.dirty = true;

        if (cache_fat (vol, sect + 1) == false) {
          return (false);
        }

        if (clus & 0x001) {
          vol->fat.buf[0] = (uint8_t)(link >> 4);
        }
        else {
          vol->fat.buf[0] = (uint8_t)((vol->fat.buf[0] & 0xF0) | (link >> 8));
        }
      }
      break;

    case FS_FAT16:
      offs  = (clus & 0xFF) << 1;
      set_u16 (&vol->fat.buf[offs], (uint16_t)link);
      break;

    case FS_FAT32:
      offs  = (clus & 0x7F) << 2;
      set_u32 (&vol->fat.buf[offs], link);
      break;

    default:
      return (false);
  }
  vol->fat.dirty = true;
  return (true);
}


/**
  Scan FAT and count number of free clusters.

  \param[out] count                     number of free clusters
  \param[in]  vol                       volume description structure
*/
static uint32_t count_free_clus (uint32_t *count, fsFAT_Volume *vol) {
  uint32_t clus, link, cnt;

  EvrFsFAT_CountFreeClus(vol->DrvLet);

  cnt = 0;
  for (clus = 2; clus < (vol->cfg.DataClusCnt + 2); clus++) {
    if (alloc_table_read (clus, &link, vol) == false) {
      /* Read sector issue */
      return (false);
    }
    if (link == 0) {
      cnt++;
    }
  }
  *count = cnt;
  return (true);
}


/**
  Scan FAT-32 allocation table and count number of free clusters.

  This routine is meant to be used only during mount to quickly
  determine number of free clusters required by FS Info structure.

  \param[out] count                     number of free clusters
  \param[in]  vol                       volume description structure
*/
static uint32_t count_free_clus32 (uint32_t *count, fsFAT_Volume *vol) {
  uint32_t sect, csect;
  uint32_t offs, clus, link, cnt;

  EvrFsFAT_CountFreeClus(vol->DrvLet);

  cnt   = 0;
  csect = 0;
  clus  = 2;
  offs  = vol->cfg.BootSector + vol->cfg.RsvdSecCnt;

  do {
    /* Determine table sector */
    sect = clus / 128;
    
    if ((offs + sect) > csect) {
      /* Block read requests till we cross sector border */
      csect = offs + sect;

      if (read_cache (vol, csect, vol->cfg.FatSize - sect) == false) {
        return (false);
      }
    }

    /* Get linked cluster number */
    link = get_u32(&vol->ca.buf[(clus & 0x7F) << 2]);

    if (link == 0) {
      cnt++;
    }
    clus++;
  }
  while (clus < (vol->cfg.DataClusCnt + 2));

  *count = cnt;
  return (true);
}


/**
  Allocate cluster
*/
static uint32_t alloc_clus (uint32_t *clus, fsFAT_Volume *vol) {
  uint32_t n, link;

  link = 0xFFFFFFFF;
  /* Find free cluster */
  for (n = vol->free_clus; n < (vol->cfg.DataClusCnt + 2); n++) {
    if (alloc_table_read (n, &link, vol) == false) {
      /* Read sector issue */
      return (false);
    }
    if (link == 0) {
      break;
    }
  }

  if (link != 0) {
    /* No free clusters - disk full */
    EvrFsFAT_DiskFull(vol->DrvLet);
    return (false);
  }
  if (alloc_table_write (n, get_EOC(vol->cfg.FatType), vol) == false) {
    return (false);
  }
  vol->free_clus = n + 1;

  /* Cluster allocated */
  if (vol->cfg.FatType == FS_FAT32) {
    vol->free_clus_cnt--;
    vol->Status |= FAT_STATUS_FSINFO;
  }
  *clus = n;
  return (true);
}


/**
  Link two clusters together
*/
static uint32_t link_clus (uint32_t clus, uint32_t next_clus, fsFAT_Volume *vol) {
  return alloc_table_write (clus, next_clus, vol);
}

/**
  Unlink cluster chain.
*/
static uint32_t unlink_clus (uint32_t clus, fsFAT_Volume *vol) {
  uint32_t n, link, eoc;

  if (clus < 2) {
    /* Empty file */
    return (true);
  }
  eoc = get_EOC (vol->cfg.FatType);

  n = clus;
  while (n < (vol->cfg.DataClusCnt + 2)) {
    if (alloc_table_read (n, &link, vol) == false) {
      return (false);
    }
    if (alloc_table_write (n, 0, vol) == false) {
      return (false);
    }
    if (vol->cfg.FatType == FS_FAT32) {
      /* Update free cluster count for FAT32 */
      vol->free_clus_cnt++;
      vol->Status |= FAT_STATUS_FSINFO;
    }
    if (link == eoc) {
      break;
    } else {
      /* Check if link corrupted */
      if (link < 2) {
        return (false);
      }
    }
    n = link;

    if (n < clus) {
      /* Keep track of the lowest free cluster */
      clus = n;
    }
  }
  if (clus < vol->free_clus) {
    vol->free_clus = clus;
  }
  /* Cluster chain unlinked */
  return (true);
}


/**
  Traverse cluster chain.
*/
static uint32_t follow_clus (uint32_t *clus, uint32_t *link, uint32_t cnt, fsFAT_Volume *vol) {
  uint32_t eoc, n, val;

  eoc = get_EOC (vol->cfg.FatType);
  n   = *clus;

  do {
    if (alloc_table_read (n, &val, vol) == false) {
      return (false);
    }
    if (val == eoc) {
      break;
    }
    if (cnt) {
      n = val;
    }
  }
  while (cnt--);
  *clus = n;
  *link = val;
  return (true);
}


/**
  Clear current cluster.

  \param[in]  clus                      cluster number
  \param[in]  vol                       volume description structure

  \return     - true: Ok
              - false: sector write failed
*/
static uint32_t clear_clus (uint32_t clus, fsFAT_Volume *vol) {
  uint32_t i;
  uint32_t sect = clus_to_sect(&vol->cfg, clus);

  /* Use cache for faster write. */
  memset(vol->ca.buf, 0, 512);
  for (i = 0; i < vol->cfg.SecPerClus; i++) {
    if (write_cache(vol, sect + i) == false) {
      return (false);
    }
  }
  /* Flush cache buffer */
  if (write_cache (vol, 0) == false) {
    return (false);
  }
  return (true);
}


/**
  Compare current directory entry position with elink entry and determine how
  many entries we can skip (since they are cached).

  \param[in]  pos                       entry position structure pointer
  \param[in]  el                        entry link structure pointer

  \return     -  0: entry position and entry link do not match
              - >0: number of directory entries covered by name cache
*/
static uint8_t entry_pos_comp (ENTRY_POS *pos, ELINK *el) {
  if (el) {
    if (pos->Clus == el->Info.EntryClus) {
      if (pos->Offs == el->Info.EntryOffs) {
        /* Entry position match */
        return ((uint8_t)el->Info.EntryCount);
      }
    }
  }
  return (0);
}


/**
  Increment directory entry position

  \param[in]  pos                       initial entry position
  \param[in]  cnt                       increment count
  \param[in]  vol                       volume description structure

  \return     - 0: Ok
              - 1: Failed, no more entries, end of current directory
              - 2: Failed, no more entries
*/
static uint32_t entry_pos_inc (ENTRY_POS *pos, uint32_t cnt, fsFAT_Volume *vol) {
  uint32_t clus, eoc, offs;
  
  /* Advance offset within cluster */
  offs  = pos->Offs & (vol->cfg.EntsPerClus - 1);
  offs += cnt;

  pos->Offs += cnt;

  if (vol->cfg.FatType != FS_FAT32) {
    /* Check for root entry limit on FAT12/FAT16 */
    if (pos->Clus == 0) {
      if (pos->Offs >= 512) {
        /* No more entries */
        return (2);
      }
      return (0);
    }
  }

  eoc = get_EOC (vol->cfg.FatType);

  /* Check if step to next cluster is needed. */
  while (offs >= vol->cfg.EntsPerClus) {
    offs -= vol->cfg.EntsPerClus;

    /* Preserve last used cluster */
    clus = pos->Clus;

    if (alloc_table_read (clus, &clus, vol) == false) {
      /* Sector (FAT) read error */
      return (2);
    }

    if (clus == eoc) {
      /* No more entries, end of current directory. */
      return (1);
    }
    /* Set next cluster */
    pos->Clus = clus;
  }
  return (0);
}


/**
  Read directory entry at given position and return it's pointer

  \param[in]  pos                       directory entry position
  \param[out] frec                      file record structure pointer
  \param[in]  vol                       volume description structure

  \return     - 0: Directory entry read and pointer to it is set
              - 1: Sector read error
*/
static uint32_t entry_read (ENTRY_POS *pos, FILEREC **frec, fsFAT_Volume *vol) {
  uint32_t sect, offs;

  if ((pos->Clus == 0) && (vol->cfg.FatType != FS_FAT32)) {
    offs = pos->Offs;
  }
  else {
    offs = pos->Offs & (vol->cfg.EntsPerClus - 1);
  }

  sect = clus_to_sect (&vol->cfg, pos->Clus);

  if (read_sector (vol, sect + (offs >> 4)) == false) {
    /* Sector (directory entries) read error */
    return (1);
  }
  *frec = (FILEREC *)(uint32_t)vol->ca.buf + (offs & 0x0F);
  return (0);
}


/**
  Return directory entry type

  This function will check the filename and attributes field in order to
  determine directory entry type.

  \param[in]  de                        directory entry

  \return     Directory entry type \ref ENTRY_TYPE
*/
static uint32_t entry_type (FILEREC *de) {
  if (de->FileName[0] == 0x00) {
    /* Free entry, last one in the directory */
    return (ENTRY_TYPE_LAST_IN_DIR);
  }
  else if (de->FileName[0] == 0xE5) {
    /* Free directory entry (erased) */
    return (ENTRY_TYPE_FREE);
  }
  else if ((de->Attr & ATTR_LONG_NAME_MASK) == ATTR_LONG_NAME) {
    /* Long file name entry */
    return (ENTRY_TYPE_LFN);
  }
  else {
    if ((de->Attr & (FS_FAT_ATTR_DIRECTORY | FS_FAT_ATTR_VOLUME_ID)) == FS_FAT_ATTR_VOLUME_ID) {
    /* Volume label */
    return (ENTRY_TYPE_LABEL);
    }
    /* Short file name entry (may be file or directory */
    return (ENTRY_TYPE_SFN);
  }
}

/**
  Flush entry that was previously read with entry_read to the media

  \param[in]  pos                       directory entry position
  \param[in]  vol                       volume description structure

  \return \ref fsStatus
*/
static fsStatus entry_flush (ENTRY_POS *pos, fsFAT_Volume *vol) {
  uint32_t sect, offs;

  if ((pos->Clus == 0) && (vol->cfg.FatType != FS_FAT32)) {
    offs = pos->Offs;
  }
  else {
    offs = pos->Offs & (vol->cfg.EntsPerClus - 1);
  }

  sect = clus_to_sect (&vol->cfg, pos->Clus);

  if (write_sector (vol, sect + (offs >> 4), ACT_USEJOUR) == false) {
    /* Sector write error */
    return (fsDriverError);
  }
  /* Entry flushed to media */
  return (fsOK);
}


/**
  Allocate requested number of contiguous directory entries
  Scan for unused or enough erased entries in given directory

  \param[in]  start_clus                first cluster of the directory we are searching in
  \param[in]  cnt                       number of entries to allocate
  \param[out] rpos                      record position
  \param[in]  vol                       volume description structure

  \return     - 0: Ok, entries allocated
              - 1: Failed, cannot allocate (no space, FAT12/16 root limit, driver error)
*/
static uint32_t frec_allocate (uint32_t start_clus, uint32_t cnt, ENTRY_POS *pos, fsFAT_Volume *vol) {
  ELINK    *el;
  FILEREC  *frec;
  ENTRY_POS cur_pos;
  uint32_t  typ;
  uint32_t  err;
  uint32_t  clus;
  uint8_t   ents;
  uint8_t   inc;

  elink_cmd (ELINK_CMD_DIR_REWIND, vol->ncache);

  /* Set initial entry position */
  cur_pos.Clus = start_clus;
  cur_pos.Offs = 0;

  /* Search through name entries. */
  ents = 0;
  do {
    el  = elink_cmd (ELINK_CMD_GET_LAST, vol->ncache);
    inc = entry_pos_comp (&cur_pos, el);

    if (inc == 0) {
      inc = 1;
      /* Read current directory entry */
      if (entry_read (&cur_pos, &frec, vol)) {
        return (1);
      }

      typ = entry_type (frec);

      if ((typ == ENTRY_TYPE_FREE) || (typ == ENTRY_TYPE_LAST_IN_DIR)) {
        /* Free entry or last entry in directory */
        if (ents == 0) {
          /* We found first unused entry. */
          pos->Clus = cur_pos.Clus;
          pos->Offs = cur_pos.Offs;
        }

        ents++;

        if (ents == cnt) {
          /* Requested number of contiguous free entries found */
          return (0);
        }
      }
      else {
        /* Valid used entry found, reset counter */
        ents = 0;
      }
    }
    else {
      /* Cached entry found, reset counter */
      ents = 0;

      /* Increment cached entry position */
      elink_cmd (ELINK_CMD_POS_INC, vol->ncache);
    }

    err = entry_pos_inc (&cur_pos, inc, vol);

    if (err != 0) {
      if (err == 1) {
        /* EOC - no more entries, allocate new cluster */
        clus = cur_pos.Clus;
        if (alloc_clus (&cur_pos.Clus, vol) == false) {
          return (1);
        }
        if (link_clus (clus, cur_pos.Clus, vol) == false) {
          return (1);
        }
        if (clear_clus (cur_pos.Clus, vol) == false) {
          return (1);
        }

        /* Clear error flag */
        err = 0;
      }
    }
  }
  while (err == 0);

  /* Not able to allocate enough entries for given name */
  EvrFsFAT_DirEntryAllocFailed (vol->DrvLet, start_clus);

  return (1);
}


/**
  Delete file record link from the name cache

  \param[in]  rpos                      entry position
  \param[in]  vol                       volume description structure

  \return \ref fsStatus
*/
static fsStatus frec_delete_elink (FREC_POS *rpos, fsFAT_Volume *vol) {
  ELINK *el;

  el = elink_cmd (ELINK_CMD_GET_LAST, vol->ncache);

  if (el) {
    if (entry_pos_comp (&rpos->pos, el)) {
      elink_delete (el, vol->ncache);

      EvrFsFAT_NameCacheEntryDelete (vol->DrvLet, el->Info.EntryClus, el->Info.EntryOffs, el->Info.EntryCount);
    }
  }
  return (fsOK);
}

/**
  Delete file record at specified position

  \param[in]  rpos                      entry position
  \param[in]  vol                       volume description structure

  \return \ref fsStatus
*/
static fsStatus frec_delete (FREC_POS *rpos, fsFAT_Volume *vol) {
  FILEREC *frec;
  ENTRY_POS pos;
  uint32_t cnt;
  uint32_t err;
  bool     done;

  pos  = rpos->pos;
  cnt  = rpos->cnt;
  done = false;

  while (!done) {
    err = entry_read (&pos, &frec, vol);

    if (err) {
      /* Read/Write error */
      return (fsError);
    }

    /* Mark entry as free */
    frec->FileName[0] = 0xE5;

    cnt--;

    if (cnt == 0) {
      done = true;
    }

    if (done || (pos.Offs & 0x0F) == 0x0F) {
      if (entry_flush (&pos, vol) != fsOK) {
        /* Read/Write error */
        return (fsError);
      }
    }

    /* Increment file record position to the next entry */
    if (entry_pos_inc (&pos, 1, vol)) {
      return (fsError);
    }
  }
  return (fsOK);
}


/**
  Find and load first valid file record begining with the current position

  \param[in]  pinfo                     path info structure
  \param[out] fn                        file name buffer
  \param[in]  len                       file name buffer length
  \param[in]  vol                       volume description structure

  \return     execution status \ref fsStatus
                - fsOK           = file record found and loaded
                - fsFileNotFound = no more files within directory
                - fsError        = io operation failed
*/
static fsStatus frec_next (PATH_INFO *pinfo, char *fn, uint32_t len, fsFAT_Volume *vol) {
#ifdef FS_FAT_LFN
  uint8_t chksum;
  uint8_t valid;
  bool    ord;
  ENTRY_POS ord_pos;
  LFN_FILEREC *lfne;
#endif
  uint32_t i;
  ELINK   *el;
  uint32_t hash;
  bool     found;
  uint8_t  ents;
  char     sn[13];

  /* Initialize search */
  hash  = 0;
  found = false;
  ents  = 0;
#ifdef FS_FAT_LFN
  valid   = 0;
  chksum  = 0;
  ord     = false;
  ord_pos.Clus = 0U;
  ord_pos.Offs = 0U;
#endif

  /* Go through directory entries */
  do {
    if (entry_read (&pinfo->frec.pos, &pinfo->sfne, vol)) {
      return (fsError);
    }

    switch (entry_type (pinfo->sfne)) {
      case ENTRY_TYPE_LAST_IN_DIR:
        /* Free entry, last one in directory */
        return (fsFileNotFound);

      #ifdef FS_FAT_LFN
      case ENTRY_TYPE_LFN:
        /* Process LFN entry */
        lfne = (LFN_FILEREC *)pinfo->sfne;

        if (lfne->Ordinal & ORD_LONG_NAME_LAST) {
          /* Ordinal sub-component of the LFN */
          ents   = lfne->Ordinal & ~ORD_LONG_NAME_LAST;
          chksum = lfne->Checksum;
          hash   = 0xFFFFFFFF;
          ord    = true;

          ord_pos = pinfo->frec.pos;
          
          if (fn) {
            i = (ents - 1) * 13 + lfn_char_cnt(lfne);

            if ((i + 1) > len) {
              /* File name buffer is too small */
              EvrFsFAT_PathBufferToSmall(vol->DrvLet);
              return (fsInvalidParameter);
            }
            fn[i] = '\0';
          }
        }

        if (chksum == lfne->Checksum) {
          valid++;

          if (fn) {
            /* Long name checking in progress. */
            i = ((lfne->Ordinal & 0x1F) - 1) * 13;

            /* Copy name characters into name array */
            lfn_extract (&fn[i], lfne, lfn_char_cnt(lfne));
          }

          if (vol->ncache) {
            /* Name cache enabled, calculate long name hash */
            hash = long_ent_hash (lfne, hash);
          }
        }
        else {
          /* Entry checksum verification failed */
          valid = 0;
        }
        break;
      #endif

      case ENTRY_TYPE_SFN:
        /* Process SFN entry */
        #ifdef FS_FAT_LFN
        if (ord) {
          ord = 0;
          if (ents == valid) {
            if (chksum == sn_chksum ((char *)pinfo->sfne->FileName)) {
              /* Valid long name file record found */
              found = true;
              /* Copy ordinal position and entry count */
              pinfo->frec.pos = ord_pos;
              pinfo->frec.cnt = 1 + ents;
            }
          }
        }
        #endif

        ents += 1;

        if (ents == 1) {
          /* Independent short filename entry */
          found = true;

          if (fn) {
            /* Copy name into input buffer */
            i = sfn_extract (sn, pinfo->sfne->FileName);
            
            if ((i + 1) >= len) {
              /* File name buffer is too small */
              EvrFsFAT_PathBufferToSmall(vol->DrvLet);
              return (fsInvalidParameter);
            }
            sfn_extract (fn, pinfo->sfne->FileName);
          }

          /* Save position */
          pinfo->frec.cnt = 1;
          /* Reset long hash */
          hash = 0;
        }

        el = elink_cmd (ELINK_CMD_ALLOC, vol->ncache);

        if (el) {
          el->Info.LongNameH  = hash;
          el->Info.ShortNameH = short_ent_hash (pinfo->sfne, el->Info.ShortNameH);
          el->Info.DirClus    = pinfo->dir_clus;
          el->Info.EntryCount = pinfo->frec.cnt;
          el->Info.EntryClus  = pinfo->frec.pos.Clus;
          el->Info.EntryOffs  = pinfo->frec.pos.Offs;

          elink_insert (el, vol->ncache);

          EvrFsFAT_NameCacheEntryInsert (vol->DrvLet, el->Info.EntryClus, el->Info.EntryOffs, el->Info.EntryCount);
        }

        if (found) {
          return (fsOK);
        }
        else {
          /* Found invalid entry */
          found = false;
        }
        break;

      case ENTRY_TYPE_LABEL:
      case ENTRY_TYPE_FREE:
        /* Skip to next directory entry */
        break;
    }
  }
  while (entry_pos_inc (&pinfo->frec.pos, 1, vol) == 0);
  /* Unexpected end of directory */
  return (fsError);
}


/**
  Find file record with specified name in the FAT name cache

  \param[in]  pinfo                     path information structure
  \param[in]  vol                       volume description structure

  \return \ref fsStatus
*/
static fsStatus frec_find_elink (PATH_INFO *pinfo, fsFAT_Volume *vol) {
  ENTRY_POS pos;
  uint32_t  hash;
  ELINK    *el;

  if (vol->ncache) {
    /* Calculate input name hash */
    hash = name_hash (pinfo->fn, pinfo->fn_len);

    /* Find the hash in cache */
    if (elink_scan (hash, &el, vol->ncache) == 0) {
      EvrFsFAT_NameCacheEntryFound (vol->DrvLet, el->Info.EntryClus, el->Info.EntryOffs, el->Info.EntryCount);

      /* File record found in cache */
      EvrFsFAT_NameCacheHit (vol->DrvLet, pinfo->fn, pinfo->fn_len);

      pinfo->frec.cnt      = (uint8_t)el->Info.EntryCount;
      pinfo->frec.pos.Clus = el->Info.EntryClus;
      pinfo->frec.pos.Offs = (uint8_t)el->Info.EntryOffs;

      pos.Clus = pinfo->frec.pos.Clus;
      pos.Offs = pinfo->frec.pos.Offs;

      if (pinfo->frec.cnt > 1) {
        /* Reposition to short entry */
        entry_pos_inc (&pos, pinfo->frec.cnt-1, vol);
      }
      /* Read short entry */
      if (entry_read (&pos, &pinfo->sfne, vol)) {
        return (fsError);
      }
      return (fsOK);
    }
    EvrFsFAT_NameCacheMiss (vol->DrvLet, pinfo->fn, pinfo->fn_len);
  }
  return (fsFileNotFound);
}


/**
  Find entry with specified name

  \param[in,out] pinfo                  path info structure
  \param[in]     vol                    volume description structure

  \return \ref fsStatus
*/
static fsStatus frec_find (PATH_INFO *pinfo, fsFAT_Volume *vol) {
#ifdef FS_FAT_LFN
  bool     ord;
  uint8_t  offs;
  uint8_t  chksum;
  uint8_t  cnt;         /* Number of characters to compare */
  uint8_t  vlen;        /* Validated name length */
  LFN_FILEREC *lfne;    /* LFN entry structure */
#endif
  FILEREC *sfne;        /* SFN entry structure */
  ENTRY_POS pos;        /* Current entry position */
  bool     insert;
  bool     match;
  char     sn[13];
  uint8_t  inc;         /* Entry position increment */
  uint8_t  ents;
  ELINK   *el;
  uint32_t hash;
  uint32_t err;

  /* Init scan */
  pos.Clus = pinfo->dir_clus;
  pos.Offs = 0;

  hash   = 0;
  match  = false;
  ents   = 0;
#ifdef FS_FAT_LFN
  vlen   = 0;
  chksum = 0;
  ord    = false;
#endif

  el = elink_cmd (ELINK_CMD_DIR_REWIND, vol->ncache);

  /* Search through entries */
  do {
    insert = false;

    inc = entry_pos_comp (&pos, el);
    if (inc == 0) {
      /* Read entry from the media */
      if (entry_read (&pos, &sfne, vol)) {
        return (fsError);
      }

      switch (entry_type (sfne)) {
        case ENTRY_TYPE_LAST_IN_DIR:
          /* Free entry, last one in directory (0x00) */
          return (fsFileNotFound);

        #ifdef FS_FAT_LFN
        case ENTRY_TYPE_LFN:
          /* Process LFN entry component */
          lfne = (LFN_FILEREC *)sfne;

          if (lfne->Ordinal & ORD_LONG_NAME_LAST) {
            /* Ordinal sub-component of the LFN */
            vlen = 0;
            ents   = lfne->Ordinal & ~ORD_LONG_NAME_LAST;
            chksum = lfne->Checksum;
            hash   = 0xFFFFFFFF;
            match  = false;
            ord    = true;

            pinfo->frec.pos = pos;

            /* Check if name length matches */
            if (pinfo->fn_len != ((ents - 1) * 13 + lfn_char_cnt(lfne))) {
              /* Name length different, clear checksum and continue processing */
              chksum = 0U;
            }
          }

          if (vol->ncache) {
            /* Name cache enabled, calculate long name hash */
            hash = long_ent_hash (lfne, hash);
          }

          if (chksum == lfne->Checksum) {
            /* Long name checking in progress. */
            offs = ((lfne->Ordinal & 0x1F) - 1) * 13;
            cnt  = pinfo->fn_len - offs;
            if (cnt > 13) {
              cnt = 13;
            }

            /* Case insensitive name compare with long entry name */
            if (lfn_name_cmp (lfne, &pinfo->fn[offs], cnt)) {
              vlen += cnt;
            }
          }
          else {
            /* Entry checksum verification failed */
            vlen = 0;
          }
          break;
        #endif

        case ENTRY_TYPE_SFN:
          /* Process SFN entry */
          #ifdef FS_FAT_LFN
          if (ord) {
            ord = 0;
            if (vlen == pinfo->fn_len) {
              if (chksum == sn_chksum ((char *)sfne->FileName)) {
                match = true;
              }
            }
          }
          vlen = 0;
          #endif

          ents += 1;
          /* Set number of entries for current file record */
          pinfo->frec.cnt = ents;

          if (ents == 1) {
            /* This is independent short filename entry */
            pinfo->frec.pos = pos;
            /* Reset match flag */
            match = false;
            /* Reset long hash */
            hash  = 0;
          }

          if (sfn_extract (sn, sfne->FileName) == pinfo->fn_len) {
            /* Short entry name length matches input length */
            if (fs_strncasecmp (sn, pinfo->fn, pinfo->fn_len) == 0) {
              /* Equal names */
              match = true;
            }
          }

          el = elink_cmd (ELINK_CMD_ALLOC, vol->ncache);

          if (el) {
            el->Info.LongNameH   = hash;
            el->Info.ShortNameH  = short_ent_hash (sfne, el->Info.ShortNameH);
            el->Info.DirClus     = pinfo->dir_clus;
            el->Info.EntryCount  = pinfo->frec.cnt;

            if (pinfo->frec.cnt > 1) {
              el->Info.EntryClus = pinfo->frec.pos.Clus;
              el->Info.EntryOffs = pinfo->frec.pos.Offs;
            }
            else {
              el->Info.EntryClus = pos.Clus;
              el->Info.EntryOffs = pos.Offs;
            }
            /* Add entry to the name cache */
            elink_insert (el, vol->ncache);

            EvrFsFAT_NameCacheEntryInsert (vol->DrvLet, el->Info.EntryClus, el->Info.EntryOffs, el->Info.EntryCount);

            insert = true;
          }

          if (match) {
            /* Found entry with the name we are searching for */
            pinfo->sfne = sfne;
            return (fsOK);
          }

          /* Entry with a different name */
          ents = 0;
          break;

        case ENTRY_TYPE_LABEL:
        case ENTRY_TYPE_FREE:
          break;
      }
    }

    if (inc || insert) {
      /* Increment cached entry link */
      el = elink_cmd (ELINK_CMD_POS_INC, vol->ncache);
    }
    if (inc == 0) {
      inc = 1;
    }
    err = entry_pos_inc (&pos, inc, vol);
  }
  while (err == 0);
  if (err == 1) {
    /* End of directory, name not found */
    return (fsFileNotFound);
  }
  /* End of directory, directory full or IO error */
  return (fsError);
}


/**
  Check if directory is empty (contains only "." and ".." entries)

  Directory entry scanning begins at third entry since first two entries are
  "dot" and "dot dot" entries.

  \param[in]  dir_clus                  first cluster of the directory
  \param[in]  vol                       volume description structure

  \return     - true:  Directory is empty
              - false: Directory not empty
*/
static uint32_t is_dir_empty (uint32_t dir_clus, fsFAT_Volume *vol) {
  ENTRY_POS pos;
  FILEREC  *frec;

  if (dir_clus == get_root_clus (vol)) {
    /* Root directory, return not empty */
    return (false);
  }

  pos.Clus = dir_clus;
  pos.Offs = 2;

  /* Search until unallocated entry found. */
  do {
    if (entry_read (&pos, &frec, vol)) {
      return (false);
    }

    switch (entry_type (frec)) {
      case ENTRY_TYPE_LAST_IN_DIR:
        /* There are no allocated entries after this one. */
        return (true);

      case ENTRY_TYPE_FREE:
        /* Free entry */
        break;

      case ENTRY_TYPE_LABEL:
      case ENTRY_TYPE_LFN:
      case ENTRY_TYPE_SFN:
        /* Valid directory entry found -> directory is not empty */
        return (false);
    }
  }
  while (entry_pos_inc (&pos, 1, vol) == 0);

  return (false);
}


#ifdef FS_FAT_LFN
/**
  Create long name component of the directory entry

  \param[in]  pinfo                     path information
  \param[in]  sfn_chksum                short name checksum
  \param[in]  vol                       volume description structure

  \return \ref fsStatus
*/
static fsStatus create_lfn_entry (PATH_INFO *pinfo, uint8_t sfn_chksum, fsFAT_Volume *vol) {
  LFN_FILEREC *frec;
  bool     ord;
  uint8_t  seq;
  uint32_t offs;
  uint32_t cnt;
  fsStatus stat;

  seq  = (pinfo->fn_len + 12) / 13;
  ord  = true;
  offs = 0;

  do {
    if (entry_read (&pinfo->frec.pos, (FILEREC **)&frec, vol)) {
      return (fsError);
    }

    /* Long name's long entry creation. */
    frec->Ordinal     = seq;
    frec->Ordinal    |= (ord) ? (ORD_LONG_NAME_LAST) : (0);
    frec->Checksum    = sfn_chksum;
    frec->FirstClusLO = 0;
    frec->Attr        = ATTR_LONG_NAME;
    frec->Type        = 0;

    /* Block update of the ordinal field */
    ord = false;

    seq--;

    /* Set name offset and number of characters to copy */
    offs = seq * 13;
    cnt  = pinfo->fn_len - offs;
    if (cnt > 13) {
      cnt = 13;
    }
    /* Copy name characters into long entry component */
    lfn_name_copy (frec, &pinfo->fn[offs], cnt);

    if ((pinfo->frec.pos.Offs & 0x0F) == 0x0F) {
      /* Sector full, flush entries */
      stat = entry_flush (&pinfo->frec.pos, vol);
      if (stat != fsOK) {
        return (stat);
      }
    }

    /* Increment file record position to the next entry */
    if (entry_pos_inc (&pinfo->frec.pos, 1, vol)) {
      return (fsError);
    }
  }
  while (seq);

  /* Long file name entry created */
  return (fsOK);
}
#endif


/**
  Create short file name directory entry

  \param[in]  fn                        short file name in entry format
  \param[in]  pos                       directory entry position
  \param[in]  first_clus                first cluster number
  \param[in]  attr                      directory entry attributes
  \param[in]  vol                       volume description structure

  \return \ref fsStatus
*/
static fsStatus create_sfn_entry (const char *fn, ENTRY_POS *pos, uint32_t first_clus, uint8_t attr, fsFAT_Volume *vol) {
  FILEREC *frec;
  fsTime   time;
  uint32_t i;

  if (entry_read (pos, &frec, vol)) {
    return (fsError);
  }

  /* Copy name into short entry */
  for (i = 0; i < 11; i++) {
    frec->FileName[i] = fn[i];
  }

  frec->Attr         = attr;
  frec->NTRsvd       = 0;
  frec->CrtTimeTenth = 0;
  fs_get_time (&time);
  set_u16 ((uint8_t *)&frec->CrtTime, format_time(&time));
  set_u16 ((uint8_t *)&frec->CrtDate, format_date(&time));
  frec->LastAccDate  = frec->CrtDate;
  frec->WriteTime    = frec->CrtTime;
  frec->WriteDate    = frec->CrtDate;
  set_u16 ((uint8_t *)&frec->FirstClusHI, (uint16_t)(first_clus >> 16));
  set_u16 ((uint8_t *)&frec->FirstClusLO, (uint16_t)(first_clus      ));
  frec->FileSize     = 0;

  if (entry_flush (pos, vol) != fsOK) {
    return (fsError);
  }
  return (fsOK);
}


/**
  Create "." and ".." directory entries.
*/
static fsStatus create_dot_entries (uint32_t dir_clus, uint32_t first_clus, fsFAT_Volume *vol) {
  FILEREC  *frec;
  ENTRY_POS pos;
  fsTime    time;
  uint16_t  ent_time, ent_date;

  pos.Clus = first_clus;
  pos.Offs = 0;

  if (entry_read (&pos, &frec, vol)) {
    return (fsError);
  }

  /* Get current system time */
  fs_get_time (&time);
  ent_time = format_time (&time);
  ent_date = format_date (&time);

  /* Create "." entry. */
  memcpy (&frec->FileName[0], ".          ", 11);
  frec->Attr = FS_FAT_ATTR_DIRECTORY;
  set_u16 ((uint8_t *)&frec->CrtTime,     ent_time);
  set_u16 ((uint8_t *)&frec->CrtDate,     ent_date);
  set_u16 ((uint8_t *)&frec->LastAccDate, ent_date);
  set_u16 ((uint8_t *)&frec->WriteTime,   ent_time);
  set_u16 ((uint8_t *)&frec->WriteDate,   ent_date);

  /* Duplicate above setting in the ".." entry. */
  memcpy (frec + 1, frec, sizeof (FILEREC));

  set_u16 ((uint8_t *)&frec->FirstClusHI, (uint16_t)(first_clus >> 16));
  set_u16 ((uint8_t *)&frec->FirstClusLO, (uint16_t)(first_clus      ));

  /* Create ".." entry, modify "." entry differences. */
  frec++;
  frec->FileName[1] = '.';

  /* For Root folder FirstClusHI and FirstClusLO are 0. */
  if (dir_clus == vol->cfg.FAT32_RootClus) {
    dir_clus = 0;
  }

  set_u16 ((uint8_t *)&frec->FirstClusHI, (uint16_t)(dir_clus >> 16));
  set_u16 ((uint8_t *)&frec->FirstClusLO, (uint16_t)(dir_clus      ));

  return (entry_flush (&pos, vol));
}


/**
  Construct and write a Master Boot Record
*/
static uint32_t mbr_write (fsFAT_Volume *vol, uint32_t iSz) {
  uint16_t val;

  EvrFsFAT_WriteMBR (vol->DrvLet, 0);

  memset (vol->ca.buf, 0, 512);

  /* Boot Descriptor: Non Bootable Device. */
  vol->ca.buf[446] = 0;

  /* Beginning of Partition - Head */
  vol->ca.buf[447] = 1;

  /* First Partition Cylinder/Sector */
  set_u16 (&vol->ca.buf[448], 1);

  /* File System Descriptor. */
  switch (vol->cfg.FatType) {
    case FS_FAT12:
      /* Type 12-bit FAT */
      vol->ca.buf[450] = 0x01;
      break;
    case FS_FAT16:
      if (iSz < 3) {
        /* Type 16-bit FAT, Partition < 32 MB */
        vol->ca.buf[450] = 0x04;
      }
      else {
        /* Type 16-bit FAT, Partition > 32MB */
        vol->ca.buf[450] = 0x06;
      }
      break;
    case FS_FAT32:
        /* Type 32-bit FAT, Partition < 2047GB */
      vol->ca.buf[450] = 0x0b;
      break;
  }

  /* End of Partition - Head */
  vol->ca.buf[451] = (uint8_t)(IniDevCfg[iSz].NumHeads - 1);

  /* End of Partition Cylinder/Sector. */
  val  = IniDevCfg[iSz].NumSect;
  val |= ((IniDevCfg[iSz].NumCyl - 1) << 6);
  set_u16 (&vol->ca.buf[452], val);

  /* First Sector Position Relative to Beginning of Device. */
  set_u32 (&vol->ca.buf[454], vol->cfg.BootSector);

  /* Number of Sectors in Partition */
  set_u32 (&vol->ca.buf[458], vol->cfg.DskSize);

  /* Executable Marker */
  set_u16 (&vol->ca.buf[510], 0xAA55);
  return (write_sector (vol, 0, ACT_NONE));
}


/**
  Read Master Boot Record info
*/
static uint32_t mbr_read (fsFAT_Volume *vol) {

  EvrFsFAT_ReadMBR (vol->DrvLet, 0);

  if (read_sector (vol, 0) == false) {
    return (false);
  }

  /* Check boot signature */
  if (get_u16 (&vol->ca.buf[510]) != 0xAA55) {
    /* Invalid Master Boot Record. */
    EvrFsFAT_InvalidMBR(vol->DrvLet);
    return (false);
  }

  if ((vol->ca.buf[0] == 0xE9) || (vol->ca.buf[0] == 0xEB && vol->ca.buf[2] == 0x90)) {
    /* This is a Boot Sector, Partition Table (MBR) does not exist */
    vol->cfg.BootSector = 0;
    EvrFsFAT_NonexistentMBR(vol->DrvLet);
  }
  else {
    /* Flash Cards have only one Partition. */
    vol->cfg.BootSector = get_u32 (&vol->ca.buf[454]);
  }
  return (true);
}


/**
  Construct and write a Boot Sector.
*/
static uint32_t bs_write (fsFAT_Volume *vol) {
  uint32_t sernum = 0x12345678;

  memset (vol->ca.buf, 0, 512);

  /* Boot Code: E9 00 90 */
  vol->ca.buf[0] = 0xE9;
  vol->ca.buf[1] = 0x00;
  vol->ca.buf[2] = 0x90;

  /* OEM name */
  memcpy (&vol->ca.buf[3], "MSWIN4.1", 8);

  /* Bytes per Sector */
  set_u16 (&vol->ca.buf[11], 512);

  /* Sectors per Cluster */
  vol->ca.buf[13] = vol->cfg.SecPerClus;

  /* Reserved Sectors */
  vol->ca.buf[14] = (uint8_t)vol->cfg.RsvdSecCnt;

  /* Number of FAT Tables */
  vol->ca.buf[16] = 2;

  /* Root Entry Count */
  if (vol->cfg.FatType != FS_FAT32) {
    /* Must be 0 for FAT32. */
    set_u16 (&vol->ca.buf[17], 512);
  }

  /* Total Sector Count */
  if (vol->cfg.DskSize < 0x10000) {
    set_u16 (&vol->ca.buf[19], (uint16_t)vol->cfg.DskSize);
  }
  else {
    set_u32 (&vol->ca.buf[32], vol->cfg.DskSize);
  }

  /* Media Type, must be the same as FAT byte 0 */
  vol->ca.buf[21] = 0xF8;

  /* Sectors per Track (required by Linux dosfsck) */
  vol->ca.buf[24] = 63;

  /* Number of Heads (required by Linux dosfsck) */
  vol->ca.buf[26] = 255;

  /* Number of Hidden Sectors */
  set_u32 (&vol->ca.buf[28], vol->cfg.BootSector);

  if (vol->cfg.FatType != FS_FAT32) {
    /* FAT Size */
    set_u16 (&vol->ca.buf[22], (uint16_t)vol->cfg.FatSize);

    /* Physical Disk Number */
    vol->ca.buf[36] = 0x80;

    /* Boot Sig */
    vol->ca.buf[38] = 0x29;

    /* Volume ID */
    set_u32 (&vol->ca.buf[39], sernum);

    /* Volume Label */
    /* NOTE: This field is not changed by commands "format" or "volume" on
       Windows OS - it is always set to NO NAME */
    memcpy (&vol->ca.buf[43], "NO NAME    ", 11);

    /* File System Type. */
    if (vol->cfg.FatType == FS_FAT12) {
      memcpy (&vol->ca.buf[54], "FAT12   ", 8);
    }
    else {
      memcpy (&vol->ca.buf[54], "FAT16   ", 8);
    }
  }
  else {
    /* FAT32 Structure different from offset 36. */

    /* FAT Size */
    set_u32 (&vol->ca.buf[36], vol->cfg.FatSize);

    /* Root Cluster Number. */
    set_u32 (&vol->ca.buf[44], 2);

    /* FSInfo */
    set_u16 (&vol->ca.buf[48], 1);

    /* Backup Boot Sector */
    set_u16 (&vol->ca.buf[50], 6);

    /* Physical Disk Number */
    vol->ca.buf[64] = 0x80;

    /* Boot Sig */
    vol->ca.buf[66] = 0x29;

    /* Volume ID */
    set_u32 (&vol->ca.buf[67], sernum);

    /* Volume Label */
    /* NOTE: This field is not changed by commands "format" or "volume" on
       Windows OS - it is always set to NO NAME */
    memcpy (&vol->ca.buf[71], "NO NAME    ", 11);

    /* File System Type. */
    memcpy (&vol->ca.buf[82], "FAT32   ", 8);
  }

  /* Executable Marker */
  set_u16 (&vol->ca.buf[510], 0xAA55);

  /* Writing boot sector */
  EvrFsFAT_WriteBootSector (vol->DrvLet, vol->cfg.BootSector);

  if (write_sector (vol, vol->cfg.BootSector, ACT_NONE) == false) {
    return (false);
  }

  /* Backup Boot Sector on FAT32. */
  if (vol->cfg.FatType == FS_FAT32) {
    if (write_sector (vol, vol->cfg.BootSector + 6, ACT_NONE) == false) {
      return (false);
    }
  }
  return (true);
}


/**
  Read Volume Boot Sector info
*/
static uint32_t bs_read (fsFAT_Volume *vol) {

  /* Reading Boot Sector (sector %d) */
  EvrFsFAT_ReadBootSector (vol->DrvLet, vol->cfg.BootSector);

  if (read_sector (vol, vol->cfg.BootSector) == false) {
    return (false);
  }

  /* Check Boot sector signature */
  if (get_u16 (&vol->ca.buf[510]) != 0xAA55) {
    /* Invalid Boot Sector */
    EvrFsFAT_InvalidBootSector (vol->DrvLet);
    return (false);
  }
  /* Set Volume Parameter Info. */
  vol->cfg.SecPerClus = vol->ca.buf[13];
  vol->cfg.BytesPerSec= get_u16 (&vol->ca.buf[11]);
  vol->cfg.RsvdSecCnt = get_u16 (&vol->ca.buf[14]);
  vol->cfg.NumOfFat   = vol->ca.buf[16];
  vol->cfg.RootEntCnt = get_u16 (&vol->ca.buf[17]);
  vol->cfg.DskSize    = get_u16 (&vol->ca.buf[19]);
  vol->cfg.FatSize    = get_u16 (&vol->ca.buf[22]);

  if ((vol->cfg.DskSize == 0) && (vol->cfg.FatSize == 0)) {
    /* Read FAT32 specific data */
    vol->cfg.FAT32_ExtFlags  = get_u16 (&vol->ca.buf[40]);
    vol->cfg.FAT32_RootClus  = get_u32 (&vol->ca.buf[44]);
    vol->cfg.FAT32_FSInfo    = get_u16 (&vol->ca.buf[48]);
    vol->cfg.FAT32_BkBootSec = get_u16 (&vol->ca.buf[50]);
  }

  if (vol->cfg.DskSize == 0) {
    /* If num of sectors > 0xFFFF, this field is nonzero. */
    vol->cfg.DskSize = get_u32 (&vol->ca.buf[32]);
  }

  if (vol->cfg.FatSize == 0) {
    /* Fat size is in this field for FAT32. */
    vol->cfg.FatSize = get_u32 (&vol->ca.buf[36]);
  }

  return (true);
}


/**
  Construct and write a FSInfo record (FAT32)
*/
static uint32_t fsinfo_write (fsFAT_Volume *vol) {
  uint32_t sect;

  memset (vol->ca.buf, 0, 512);

  /* Lead Signature */
  set_u32 (&vol->ca.buf[0], 0x41615252);

  /* Struct Signature */
  set_u32 (&vol->ca.buf[484], 0x61417272);

  /* Cluster Free Count */
  set_u32 (&vol->ca.buf[488], vol->free_clus_cnt);

  /* Next Free Cluster */
  set_u32 (&vol->ca.buf[492], vol->free_clus);

  /* Trail Signature */
  set_u16 (&vol->ca.buf[510], 0xAA55);

  sect = vol->cfg.BootSector + vol->cfg.FAT32_FSInfo;

  /* Writing FS Info (sector %d) */
  EvrFsFAT_WriteFSInfo (vol->DrvLet, sect);

  return (write_cache (vol, sect));
}


/**
  Read FSInfo Record (FAT32)
*/
static fsStatus fsinfo_read (fsFAT_Volume *vol) {
  fsStatus status;
  uint32_t v;
  uint32_t sect = vol->cfg.BootSector + vol->cfg.FAT32_FSInfo;

  /* Reading FS Info */
  EvrFsFAT_ReadFSInfo (vol->DrvLet, sect);

  if (read_sector (vol, sect) == false) {
    status = fsError;
  }
  else if (get_u32 (&vol->ca.buf[508]) != 0xAA550000) {
    /* Trailing signature invalid */
    EvrFsFAT_InvalidFSInfo (vol->DrvLet);
    status = fsError;
  }
  else if (get_u32 (&vol->ca.buf[0]) != 0x41615252) {
    /* Leading signature invalid */
    EvrFsFAT_InvalidFSInfo (vol->DrvLet);
    status = fsError;
  }
  else if (get_u32 (&vol->ca.buf[484]) != 0x61417272) {
    /* FSInfo structure signature invalid */
    EvrFsFAT_InvalidFSInfo (vol->DrvLet);
    status = fsError;
  }
  else {
    /* Read Free Cluster Count. */
    v = get_u32 (&vol->ca.buf[488]);

    if (v > vol->cfg.DataClusCnt) {
      /* Number of free clusters invalid */
      EvrFsFAT_InvalidFreeClusFSInfo (vol->DrvLet);
      status = fsError;
    }
    else {
      vol->free_clus_cnt = v;

      /* Read Next Free Cluster. */
      v = get_u32 (&vol->ca.buf[492]);

      if ((v < 2) || (v > vol->cfg.DataClusCnt)) {
        /* Next free cluster number invalid */
        EvrFsFAT_InvalidNextClusFSInfo (vol->DrvLet);
        status = fsError;
      }
      else {
        vol->free_clus = v;

        /* FSInfo is ok */
        status = fsOK;
      }
    }
  }

  return (status);
}


/**
  Write a drive label into the root directory.
*/
static uint32_t label_write (fsFAT_Volume *vol, const char *label, uint32_t len, uint32_t sect) {
  FILEREC *frec;
  fsTime   time;
  uint32_t i;

  if (len <= 11) {
    /* Buffer ca.buf is already cleared. */
    frec = (FILEREC *)(uint32_t)&vol->ca.buf[0];

    for (i = 0; i < len; i++) {
      frec->FileName[i] = label[i];
    }
    for (; i < 11; i++) {
      frec->FileName[i] = ' ';
    }
    frec->Attr = FS_FAT_ATTR_VOLUME_ID;
    /* Get current system time */
    fs_get_time (&time);
    /* Update access time stamp */
    set_u16 ((uint8_t *)&frec->WriteTime, format_time(&time));
    set_u16 ((uint8_t *)&frec->WriteDate, format_date(&time));

    if (write_cache (vol, sect)) {
      /* Leave the buffer cleared on exit. */
      memset (&vol->ca.buf[0], 0, 32);
      return (true);
    }
  }
  return (false);
}


/**
  Read volume label through search in root directory

  Buffer where label will be written as a null terminated string. When volume
  has no label an empty string is returned.

  \param[out] buf                       label buffer
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
static fsStatus label_read (fsFAT_Volume *vol, char *buf) {
  ENTRY_POS pos;
  FILEREC *frec;
  uint32_t typ;
  uint32_t i, j, err;

  /* Empty string equals to "no label" */
  *buf = '\0';

  /* Go in root and find label entry */
  pos.Clus = get_root_clus (vol);
  pos.Offs = 0;

  do {
    err = entry_read (&pos, &frec, vol);
    if (err != 0) {
      return (fsDriverError);
    }

    typ = entry_type (frec);

    if (typ == ENTRY_TYPE_LAST_IN_DIR) {
      /* There are no allocated entries after this one. */
      break;
    }

    if (typ == ENTRY_TYPE_LABEL) {
      /* Find the end of label */
      for (j = 11; j; j--) {
        if (frec->FileName[j-1] != ' ') {
          break;
        }
      }
      /* Copy label into the buffer */
      for (i = 0; i < j; i++) {
        buf[i] = frec->FileName[i];
      }
      buf[i] = '\0';
      /* Label was found and written into buffer */
      break;
    }
  }
  while (entry_pos_inc (&pos, 1, vol));

  #ifdef FS_DEBUG
  if (strlen(buf) != 0) {
    EvrFsFAT_LabelString (buf, strlen(buf));
  } else {
    EvrFsFAT_LabelNotSet (vol->DrvLet);
  }
  #endif

  return (fsOK);
}


/**
  Read FAT volume serial number

  \param[out] serial                    pointer to 32-bit variable
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
static fsStatus serial_read (fsFAT_Volume *vol, uint32_t *serial) {
  uint32_t offs;

  if (read_sector (vol, vol->cfg.BootSector) == true) {
    if (vol->cfg.FatType != FS_FAT32) {
      offs = 39;
    }
    else {
      offs = 67;
    }
    *serial = get_u32 (&vol->ca.buf[offs]);
    return (fsOK);
  }
  return (fsDriverError);
}


/**
  Clear the whole disk (clear all sectors)

  \todo Make this function faster
*/
static uint32_t wipe_disk (fsFAT_Volume *vol, uint32_t dsize) {
  uint32_t i,csize;

  /* Invalidate the cache. */
  vol->ca.nwr = 0;
  vol->ca.nrd = 0;

  /* Use the cache buffer. */
  csize = vol->CaSize;
  if (csize == 0) {
    csize = 1;
  }
  memset (vol->ca.buf, 0xFF, csize * 512);

  for (i = 0; i < dsize; i += csize) {
    if (vol->Drv->WriteSect (i, vol->ca.buf, csize) == false) {
      return (false);
    }
  }
  return (true);
}


/**
  Check if Volume Info is sane.

  \param[in]  vol                       volume information
  \return execution status fsStatus
*/
static fsStatus fat_validate (FATINFO *vol) {
  fsStatus status;

  if (vol->BytesPerSec != 512) {
    /* We only support 512 byte sectors. */
    status = fsError;
  }
  else if (vol->RsvdSecCnt == 0) {
    /* There should be at least 1 reserved sector. */
    status = fsError;
  }
  else if (vol->FatSize > vol->DskSize) {
    /* Size mismatch */
    status = fsError;
  }
  else if (vol->NumOfFat > 2) {
    /* Only 1 or 2 FAT tables supported */
    status = fsError;
  }
  else {
    /* Check for valid number of sectors per cluster */
    switch (vol->SecPerClus) {
      case 1:
      case 2:
      case 4:
      case 8:
      case 16:
      case 32:
      case 64:
      case 128: status = fsOK;    break;
      default:  status = fsError; break;
    }
  }

  return (status);
}


/* -------------------------- FAT Journal ------------------------------------*/

/**
  Switch FAT - flush cache and select new FAT table
*/
static uint32_t switch_fat (fsFAT_Volume *vol, uint8_t fat_num) {

  if (vol->fat.dirty == true) {
    /* Flush cache */
    if (cache_fat (vol, 0) == false) {
      return (false);
    }
  }
  /* Invalidate cache */
  vol->fat.sect = INVAL_SECT;
  /* Set new FAT */
  vol->fat.cfat = fat_num;
  return (true);
}


/**
  Return number of clusters reserved for the journal.
*/
static uint32_t j_clus_cnt (uint32_t num_rsvd_sect, uint32_t sec_per_clus) {
  /* Sector count to cluster count conversion */
  return (num_rsvd_sect + (sec_per_clus-1)) / sec_per_clus;
}


/**
  Return first cluster of the journal if cluster with signature is given.
*/
static uint32_t j_get_first (fsFAT_Volume *vol, uint32_t sign_clus) {
  return sign_clus - j_clus_cnt(vol->RsvdS, vol->cfg.SecPerClus) + 1;
}


/**
  Return low boundary cluster of the journal region.
*/
static uint32_t j_low_boundary (fsFAT_Volume *vol) {
  return (vol->cfg.DataClusCnt + 2) - (vol->cfg.DataClusCnt + 2) / 100;
}


/**
  Check if we have found a journal signature cluster.
*/
static uint32_t j_is_signed (fsFAT_Volume *vol, uint32_t clus) {
  uint32_t sect, s1, s2;

  /* Set last sector in cluster */
  sect = clus_to_sect (&vol->cfg, clus) + vol->cfg.SecPerClus - 1;

  if (read_sector (vol, sect) == false) {
    return (false);
  }

  /* Check if this is fat journal signature and return true if it is */
  s1 = get_u32 (&vol->ca.buf[0]);
  s2 = get_u32 (&vol->ca.buf[508]);
  if (s1 == SIGN_FATJ && s2 == SIGN_FATJ) {
    /* We have valid signature */
    return true;
  }
  /* Signature nonexisting or invalid */
  return (false);
}


/**
  Write signature in the last sector of cluster.
*/
static uint32_t j_wr_sign (fsFAT_Volume *vol, uint32_t clus) {
  uint32_t sect;

  /* Set last sector in cluster */
  sect = clus_to_sect (&vol->cfg, clus) + vol->cfg.SecPerClus - 1;

  set_u32 (&vol->ca.buf[0],   SIGN_FATJ);
  set_u32 (&vol->ca.buf[508], SIGN_FATJ);

  if (write_sector (vol, sect, ACT_NONE) == false) {
    return (false);
  }

  /* Signature written */
  return (true);
}


/**
  Mark clusters in range as reserved.
*/
static uint32_t j_mark_rsvd (fsFAT_Volume *vol, uint32_t first_clus, uint32_t cnt) {
  uint32_t clus, max, rsvd_val;

  max = first_clus + cnt;
  rsvd_val = get_RSVD (vol->cfg.FatType);

  /* Mark cluster range as reserved */
  for (clus = first_clus; clus < max; clus++) {
    if (alloc_table_write (clus, rsvd_val, vol) == false) {
      return (false);
    }
  }

  /* Flush cache */
  if (cache_fat (vol, 0) == false) {
    return (false);
  }
  /* Clusters are reserved */
  return (true);
}


/**
  Find clusters suitable for journal in predefined range
*/
static uint32_t j_find_reg (fsFAT_Volume *vol, uint32_t *first_clus, uint32_t *num_of) {
  uint32_t first, clus, cnt, low, val;

  cnt = 0U;
  /* Set lowest cluster = Last cluster - 1% of all clusters */
  low = j_low_boundary (vol);

  /* Go from the last cluster downward */
  for (clus = vol->cfg.DataClusCnt + 1; clus > low; clus--) {
    if (alloc_table_read (clus, &val, vol) == false) {
      return (false);
    }

    if (val == 0U) {
      /* Current cluster is free */
      first = clus;
      cnt++;

      if (cnt == j_clus_cnt(vol->RsvdS, vol->cfg.SecPerClus)) {
        /* We have enough contiguous clusters */
        *first_clus = first;
        *num_of     = cnt;

        return (true);
      }
    }
    else {
      /* Current cluster is reserved or bad */
      cnt = 0U;
    }
  }

  /* Region can not be used for journal */
  *first_clus = 0;
  *num_of     = 0;
  return (false);
}


/**
  Check if FAT clusters in given range are reserved
*/
static uint32_t j_chk_rsvd (fsFAT_Volume *vol, uint32_t first_clus, uint32_t cnt) {
  uint32_t clus, max, rsvd_val, fen_val;

  rsvd_val = get_RSVD (vol->cfg.FatType);
  max      = first_clus + cnt;

  for (clus = first_clus; clus < max; clus++) {
    /* Read FAT entry for current cluster */
    if (alloc_table_read (clus, &fen_val, vol) == false) {
      return (false);
    }

    if (fen_val != rsvd_val) {
      /* Cluster is not reserved */
      return (false);
    }
  }
  /* All clusters are reserved */
  return (true);
}


/**
  Assign free space for the journal
*/
static uint32_t j_rep_space (fsFAT_Volume *vol, uint32_t first_clus) {
  uint32_t first_sect = clus_to_sect (&vol->cfg, first_clus);
  uint32_t cnt        = vol->cfg.SecPerClus * j_clus_cnt(vol->RsvdS, vol->cfg.SecPerClus) - 1;

  if (fsj_set_space (vol->fsj, first_sect, cnt) == false) {
    /* Journal requests more space */
    return (false);
  }
  /* Journal can now be used */
  return (true);
}


/**
  Find reserved clusters in FAT
*/
static uint32_t j_find_rsvd (fsFAT_Volume *vol, uint32_t *first_clus) {
  uint32_t low, clus, fen_val, first;

  /* Invalidate first cluster of the journal space */
  *first_clus = 0;

  /* Determine cluster range == 1% of total clusters */
  low = j_low_boundary (vol);

  for (clus = vol->cfg.DataClusCnt + 1; clus > low; clus--) {
    /* Read FAT entry for the current cluster */
    if (alloc_table_read (clus, &fen_val, vol) == false) {
      return (false);
    }

    if (fen_val == get_RSVD (vol->cfg.FatType)) {
      /* This is reserved cluster */
      if (j_is_signed (vol, clus) == true) {
        /* This cluster has journal signature in it */
        first = j_get_first (vol, clus);
        if (j_chk_rsvd (vol, first, j_clus_cnt(vol->RsvdS, vol->cfg.SecPerClus)) == true) {
          *first_clus = first;
          /* First cluster of the journal found */
          return (true);
        }
      }
    }
  }
  /* Reserved clusters not found */
  return (false);
}


/**
  Find journal on FAT level, search for reserved clusters.
*/
static uint32_t j_init_fats (fsFAT_Volume *vol, uint32_t *first_clus) {
  uint32_t first, cnt;

  /* Select FAT 1 */
  if (switch_fat (vol, FAT_1) == false) {
    return (false);
  }

  *first_clus = 0;
  if (j_find_rsvd (vol, &first) == true) {
    
    /* First cluster of the journal space found in FAT 1 */
    if (switch_fat (vol, FAT_2) == false) {
      return (false);
    }
    cnt = j_clus_cnt (vol->RsvdS, vol->cfg.SecPerClus);

    if (j_chk_rsvd (vol, first, cnt) == false) {
      /* Same clusters are not marked as reserved in FAT 2 - mark them */
      if (j_mark_rsvd (vol, first, cnt) == false) {
        return (false);
      }
    }
    *first_clus = first;
  }
  else {
    /* Select FAT 2 */
    if (switch_fat (vol, FAT_2) == false) {
      return (false);
    }

    if (j_find_rsvd (vol, &first) == false) {
      /* Reserved clusters not found in FAT 2 - prepare for journal */
      *first_clus = 0;
    }
    else {
      /* First cluster of the journal space found in FAT 2 */
      if (switch_fat (vol, FAT_1) == false) {
        return (false);
      }
      cnt = j_clus_cnt (vol->RsvdS, vol->cfg.SecPerClus);

      if (j_chk_rsvd (vol, first, cnt) == false) {
        /* Same clusters are not marked as reserved in FAT 2 - mark them */
        if (j_mark_rsvd (vol, first, cnt) == false) {
          return (false);
        }
      }
      *first_clus = first;
    }
  }

  /* Select FAT 1 */
  if (switch_fat (vol, FAT_1) == false) {
    return (false);
  }
  return (true);
}


/**
  Init FAT file system journal.
*/
__WEAK uint32_t fat_jour_init (fsFAT_Volume *vol) {
  uint32_t first_clus;

  if (vol->fsj != NULL) {
    /* Initializing FAT journal */
    EvrFsFAT_InitJournal (vol->DrvLet);

    /* Reset "journal active" flag */
    vol->Status &= ~FAT_STATUS_JOURACT;

    /* We need two FATs to work */
    if (vol->cfg.NumOfFat < 2) {
      EvrFsFAT_InitJournalFATError (vol->DrvLet);
      return (false);
    }

    /* Init both FAT tables */
    first_clus = 0;
    if (j_init_fats (vol, &first_clus) == false) {
      return (false);
    }

    if (first_clus == 0) {
      /* Clusters for journal must be prepared */
      if (fat_jour_prep (vol) == false) {
        return (false);
      }
    }
    else {
      /* Report how many space is reserved for journal */
      if (j_rep_space (vol, first_clus) == false) {
        return (false);
      }
    }

    /* Init low level journaling system */
    if (fsj_init (vol->fsj, vol->Drv) == false) {
      /* Journal inconsistent? Driver error? */
      return (false);
    }

    /* Set "journal active" flag */
    vol->Status |= FAT_STATUS_JOURACT;

    EvrFsFAT_InitJournalSuccess (vol->DrvLet);
  }

  return (true);
}


/**
  Prepare FAT file system for journaling.
*/
__WEAK uint32_t fat_jour_prep (fsFAT_Volume *vol) {
  uint32_t first_clus, cnt;

  if (vol->fsj != NULL) {
    /* Reset "journal active" flag */
    vol->Status   &= ~FAT_STATUS_JOURACT;

    /* Invalidate FAT cache */
    vol->fat.sect  = INVAL_SECT;
    vol->fat.dirty = false;

    first_clus    = 0;
    cnt           = 0;

    /* Find contiguous region suitable for journal */
    if (j_find_reg (vol, &first_clus, &cnt) == false) {
      if (first_clus == 0 && cnt == 0) {
        /* It is not possible to write journal */
        return (false);
      }
      /* Clusters in range are used - use defrag or delete some files */
      return (false);
    }

    /* Reserve clusters in returned region */
    if (j_mark_rsvd (vol, first_clus, cnt) == false) {
      return (false);
    }

    /* Write signature */
    if (j_wr_sign (vol, first_clus + cnt - 1) == false) {
      return (false);
    }

    /* Report number of sectors */
    if (j_rep_space (vol, first_clus) == false) {
      return (false);
    }
  }
  return (true);
}


/**
  Initialize path information

  Input path is checked for absolute or relative so that pinfo members can
  be initialized accordingly. Processing of:
    - absolute path will start from root directory.
    - relative path will start from current directory.

  \param[in]  path                      input path
  \param[in]  pinfo                     path information structure
  \param[in]  vol                       volume description structure
*/
static void path_init (const char *path, PATH_INFO *pinfo, fsFAT_Volume *vol) {

  if (*path == '\\' || *path == '/') {
    /* Absolute path - start from root */
    pinfo->dir_clus = get_root_clus (vol);

    /* Init name cache */
    elink_dir (pinfo->dir_clus, ELINK_DIR_ROOT, vol->ncache);
  }
  else {
    /* Relative path - start from the current directory */
    pinfo->dir_clus = vol->cdir_clus;

    /* Init name cache */
    elink_dir (pinfo->dir_clus, ELINK_DIR_CD, vol->ncache);
  }
  pinfo->fn = path;
}


/**
  Process path and open requested folder or file

  \param[in]  pinfo                     path information structure
  \param[in]  vol                       volume description structure

  \return \ref fsStatus
*/
static fsStatus path_open (PATH_INFO *pinfo, fsFAT_Volume *vol) {
  fsStatus stat;

  /* Processing path */
  EvrFsFAT_PathProcessing (vol->DrvLet, pinfo->fn);

  pinfo->fn_len = 0;

  do {
    /* Set name */
    pinfo->fn += pinfo->fn_len;
    pinfo->fn_flags = 0;

    /* Strip name to find */
    pinfo->fn = name_strip (pinfo->fn, &pinfo->fn_len, &pinfo->fn_flags);

    if (pinfo->fn_len == 0) {
      return (fsInvalidPath);
    }

    /* Analyze input name */
    pinfo->fn_flags |= name_analyse (pinfo->fn, pinfo->fn_len);
    if (pinfo->fn_flags & FAT_NAME_WILDCARD) {
      return (fsOK);
    }
    else {
      if ((pinfo->fn_flags & (FAT_NAME_SFN | FAT_NAME_LFN)) == 0) {
        /* Invalid name */
        return (fsInvalidPath);
      }
    }

    if (!(pinfo->fn_flags & FAT_NAME_DOT)) {
      /* Scan current directory name */
      stat = frec_find_elink (pinfo, vol);
      if (stat == fsFileNotFound) {
        stat = frec_find (pinfo, vol);
      }

      if (stat != fsOK) {
        /* Nonexistent name or r/w error */
        return (stat);
      }

      if ((pinfo->fn_flags & (FAT_NAME_DIR | FAT_NAME_LAST)) == FAT_NAME_DIR) {
        if (!(pinfo->sfne->Attr & FS_FAT_ATTR_DIRECTORY)) {
          /* This is name of a file not directory */
          return (fsInvalidPath);
        }

        /* Step into the directory */
        pinfo->dir_clus = extract_clus (pinfo->sfne);

        if (pinfo->fn_flags & FAT_NAME_DOTDOT) {
          /* Decrement name cache path depth */
          elink_dir (pinfo->dir_clus, ELINK_DIR_DEPTH_DEC, vol->ncache);
        }
        else {
          /* Increment name cache path depth */
          elink_dir (pinfo->dir_clus, ELINK_DIR_DEPTH_INC, vol->ncache);
        }
      }
    }
  }
  while (!(pinfo->fn_flags & FAT_NAME_LAST));
  /* Requested name was found */
  return (fsOK);
}


/**
  Process path and create requested folder or file

  \param[in]  pinfo                     path information structure
  \param[in]  mkdir                     true when path is a directory
  \param[in]  vol                       volume description structure

  \return \ref fsStatus
*/
static fsStatus path_create (PATH_INFO *pinfo, bool mkdir, fsFAT_Volume *vol) {
  char      sn[13];
  uint32_t  err;
  uint32_t  first_clus;
  uint8_t   attrib;
  ELINK    *el;
  #ifdef FS_FAT_LFN
  fsStatus  stat;
  uint32_t  num;
  PATH_INFO pinfo_nt;
  #endif

  pinfo->fn_len = 0;

  do {
    pinfo->fn += pinfo->fn_len;
    pinfo->fn_flags = 0;

    /* Strip name to create */
    pinfo->fn = name_strip (pinfo->fn, &pinfo->fn_len, &pinfo->fn_flags);

    if (pinfo->fn_len == 0) {
      return (fsInvalidPath);
    }

    /* Analyze input name */
    pinfo->fn_flags |= name_analyse (pinfo->fn, pinfo->fn_len);

    if (pinfo->fn_flags & (FAT_NAME_DOT | FAT_NAME_DOTDOT)) {
      /* Invalid name */
      return (fsInvalidPath);
    }
    if ((pinfo->fn_flags & (FAT_NAME_SFN | FAT_NAME_LFN)) == 0) {
      /* Invalid name */
      return (fsInvalidPath);
    }

    if (mkdir) {
      pinfo->fn_flags |= FAT_NAME_DIR;
    }

    pinfo->frec.cnt = 1;

    #ifdef FS_FAT_LFN
    if (!(pinfo->fn_flags & FAT_NAME_SFN)) {
      /* Set number of entries to store long name */
      pinfo->frec.cnt += (pinfo->fn_len + 12) / 13;

      /* Generate basis name */
      name_basis_gen (pinfo->fn, pinfo->fn_len, sn);

      /* Copy path info */
      pinfo_nt = *pinfo;
      /* Update path name */
      pinfo_nt.fn = sn;

      /* Generate numeric tail name */
      num = 1;
      do {
        if (name_nt_gen (sn, num++) == false) {
          return (fsError);
        }
        pinfo_nt.fn_len = (uint8_t)strlen (sn);
        stat = frec_find_elink (&pinfo_nt, vol);
        if (stat == fsFileNotFound) {
          stat = frec_find (&pinfo_nt, vol);
        }
      }
      while (stat == fsOK);

      if (stat != fsFileNotFound) {
        return (stat);
      }
    }
    #endif

    /* Allocate name entries */
    err = frec_allocate (pinfo->dir_clus, pinfo->frec.cnt, &pinfo->frec.pos, vol);
    if (err) {
      /* Cannot allocate */
      return (fsError);
    }
    el = elink_cmd (ELINK_CMD_ALLOC, vol->ncache);

    if (pinfo->fn_flags & FAT_NAME_DIR) {
      /* We are creating folder, allocate its first cluster */
      if (alloc_clus (&first_clus, vol) == false) {
        return (fsError);
      }
      if (clear_clus (first_clus, vol) == false) {
        return (fsError);
      }
      /* Create "." and ".." entries */
      if (create_dot_entries (pinfo->dir_clus, first_clus, vol) != fsOK) {
        return (fsError);
      }
      attrib = FS_FAT_ATTR_DIRECTORY;
    }
    else {
      /* Create file */
      first_clus = 0;
      attrib     = FS_FAT_ATTR_ARCHIVE;
    }

    if (pinfo->frec.cnt == 1) {
      memcpy (sn, pinfo->fn, pinfo->fn_len);
      sn[pinfo->fn_len] = '\0';
    }

    /* Convert name to 8.3 short entry format */
    sfn_embed (sn, sn);

    #ifdef FS_FAT_LFN
    if (pinfo->frec.cnt > 1) {
      if (el) {
        /* Fill LFN part of entry link */
        el->Info.LongNameH = name_hash (pinfo->fn, pinfo->fn_len);
        el->Info.EntryClus = pinfo->frec.pos.Clus;
        el->Info.EntryOffs = pinfo->frec.pos.Offs;
      }
      /* Create entry long name component */
      create_lfn_entry (pinfo, sn_chksum(sn), vol);
    }
    #endif

    /* Create short entry */
    if (create_sfn_entry (sn, &pinfo->frec.pos, first_clus, attrib, vol) != fsOK) {
      return (fsError);
    }

    if (el) {
      el->Info.ShortNameH  = short_ent_hash ((FILEREC *)(uint32_t)sn, el->Info.ShortNameH);
      el->Info.DirClus     = pinfo->dir_clus;
      el->Info.EntryCount  = pinfo->frec.cnt;

      if (pinfo->frec.cnt == 1) {
        el->Info.EntryClus = pinfo->frec.pos.Clus;
        el->Info.EntryOffs = pinfo->frec.pos.Offs;
      }

      elink_insert (el, vol->ncache);

      EvrFsFAT_NameCacheEntryInsert (vol->DrvLet, el->Info.EntryClus, el->Info.EntryOffs, el->Info.EntryCount);
    }

    if (attrib == FS_FAT_ATTR_DIRECTORY) {
      /* Step into the directory */
      pinfo->dir_clus = first_clus;

      /* Increment name cache path depth */
      elink_dir (pinfo->dir_clus, ELINK_DIR_DEPTH_INC, vol->ncache);
    }
  }
  while (!(pinfo->fn_flags & FAT_NAME_LAST));

  /* Flush allocation table */
  if (cache_fat (vol, 0) == false) {
    return (fsError);
  }

  /* Path created */
  return (fsOK);
}


/* -------------------------- Exported functions -----------------------------*/

/**
  Initialize FAT volume resources and media driver.

  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_init (fsFAT_Volume *vol) {
  fsStatus status;
  uint32_t mask;

  mask = (~FAT_STATUS_MASK) | FAT_STATUS_INIT_IO;

  /* Check if already initialized */
  if ((vol->Status & mask) == FAT_STATUS_INIT_IO) {
    status = fsOK;
  }
  else {
    /* Initializing drive */
    EvrFsFAT_InitDrive (vol->DrvLet);

    /* Load user configuration */
    fs_config ((const char *)&vol->DrvLet);

    if (vol->Mutex == NULL) {
      /* Mutex was not created */
      status = fsError;
    }
    else if (vol->Drv == NULL) {
      /* Configuration error, no driver registered */
      EvrFsFAT_InitDriverCfgError (vol->DrvLet);
      status = fsError;
    }
    else {
      vol->Status = 0U;

      /* Initialize cache buffers */
      vol->fat.buf = (uint8_t *)&vol->CaBuf[0];
      vol->ca.buf  = (uint8_t *)&vol->CaBuf[128];
      vol->ca.cbuf = (uint8_t *)&vol->CaBuf[256];

      if (vol->Drv->Init (DM_IO) == false) {
        /* Failed to initialize the driver */
        EvrFsFAT_InitDriverError (vol->DrvLet);
        status = fsDriverError;
      }
      else {
        /* Initialization succeeded */
        vol->Status |= FAT_STATUS_INIT_IO;
        status = fsOK;

        EvrFsFAT_InitDriveSuccess (vol->DrvLet);
      }
    }
  }

  return (status);
}


/**
  Uninitialize FAT volume and media driver.

  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_uninit (fsFAT_Volume *vol) {
  fsStatus status;

  status = fsOK;

  /* Uninitializing drive */
  EvrFsFAT_UninitDrive (vol->DrvLet);

  if (vol->Mutex != NULL) {
    if (fs_mutex_delete (vol->Mutex) != 0U) {
      status = fsError;
    }
  }

  if (status == fsOK) {
    /* Uninitialize low level driver */
    if (vol->Drv != NULL) {
      vol->Drv->UnInit (DM_IO);
    }

    /* Clear volume structure */
    memset (vol, 0, sizeof(fsFAT_Volume));
  }

  return (status);
}

/**
  Mount FAT volume.

  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_mount (fsFAT_Volume *vol) {
  fsStatus status;
  uint32_t root_scnt;

  /* Mounting drive */
  EvrFsFAT_MountDrive (vol->DrvLet);

  /* Assume writable, removable media */
  vol->Status |= FAT_STATUS_WRITE | FAT_STATUS_REMOVABLE;
  
  status = fat_vol_chk (FAT_STATUS_INIT_IO, vol);
  if (status != fsOK) {
    /* Cannot mount */
    return (status);
  }

  /* Initialize the media. */
  if (vol->Drv->Init (DM_MEDIA) == false) {
    /* Media init failed */
    EvrFsFAT_InitMediaError (vol->DrvLet);
    return (fsMediaError);
  }
  vol->Status |= FAT_STATUS_INIT_MEDIA | FAT_STATUS_READY;

  /* Init name caching for FAT drives */
  elink_init (vol->ncache);

  /* Clear Volume info record. */
  memset (&vol->cfg, 0, sizeof (vol->cfg));

  /* Initialize FAT Cache and Data Cache */
  vol->fat.sect  = INVAL_SECT;
  vol->fat.dirty = false;
  vol->fat.cfat  = FAT_1;

  vol->ca.sect = INVAL_SECT;
  vol->ca.nwr  = 0;
  vol->ca.nrd  = 0;

  /* First 2 clusters are always reserved. */
  vol->free_clus = 2;

  /* Read Master Boot Record */
  if (mbr_read (vol) == false) {
    /* Invalid MBR? */
    return (fsNoFileSystem);
  }

  /* Read Boot Sector */
  if (bs_read (vol) == false) {
    /* Invalid BS? */
    return (fsNoFileSystem);
  }

  /* Check if FAT valid */
  if (fat_validate (&vol->cfg) != fsOK) {
    /* Invalid volume parameters */
    EvrFsFAT_InvalidFAT (vol->DrvLet);
    return (fsNoFileSystem);
  }

  /* Calculate Root Sector Count. */
  root_scnt = (vol->cfg.RootEntCnt * 32 + vol->cfg.BytesPerSec - 1) / vol->cfg.BytesPerSec;

  /* Count Data Sectors/Clusters */
  vol->cfg.DataSecCnt  = vol->cfg.DskSize - (vol->cfg.RsvdSecCnt +
                        vol->cfg.NumOfFat * vol->cfg.FatSize + root_scnt);
  vol->cfg.DataClusCnt = vol->cfg.DataSecCnt / vol->cfg.SecPerClus;
  vol->cfg.RootDirAddr = vol->cfg.RsvdSecCnt + vol->cfg.NumOfFat * vol->cfg.FatSize;
  vol->cfg.RootSecCnt  = (uint16_t)root_scnt;
  vol->cfg.ClusSize    = vol->cfg.SecPerClus * vol->cfg.BytesPerSec;
  vol->cfg.EntsPerClus = (uint16_t)(vol->cfg.ClusSize / 32);

  /* Determine Fat Type. */
  if (vol->cfg.DataClusCnt < 4085) {
    vol->cfg.FatType = FS_FAT12;
  }
  else if (vol->cfg.DataClusCnt < 65525) {
    vol->cfg.FatType = FS_FAT16;
  }
  else {
    vol->cfg.FatType = FS_FAT32;
    /* Read File System info sector. */
    if (fsinfo_read (vol) != fsOK) {
      if (count_free_clus32 (&vol->free_clus_cnt, vol) == false) {
        /* Read/Write error */
        return (fsDriverError);
      }

      if (vol->Status & FAT_STATUS_WRITE) {
        /* Write corrected FSInfo */
        if (fsinfo_write (vol) == false) {
          /* Read/Write error */
          return (fsDriverError);
        }
      }
    }
  }
  vol->cdir_clus = get_root_clus (vol);

  vol->Status |= FAT_STATUS_MOUNT;

  if (vol->Status & FAT_STATUS_WRITE) {
    /* Init FAT Journal */
    if (fat_jour_init(vol) == false) {
      /* Journal init failed */
      vol->Status |= FAT_STATUS_JOURERR;
    }
  }
  EvrFsFAT_MountDriveSuccess (vol->DrvLet);

  return (fsOK);
}


/**
  Unmount FAT volume.

  Checks for opened file handlers and closes them. When this function returns
  volume status is unmounted and media cannot be accessed anymore until fmount
  is called again.

  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_unmount (fsFAT_Volume *vol) {
  fsFAT_Handle *fh;
  int32_t i;

  /* Unmounting drive */
  EvrFsFAT_UnmountDrive (vol->DrvLet);

  for (i = 0, fh = &fs_fat_fh[0]; i < fs_fat_fh_cnt; i++, fh++) {
    if (fh->flags & FAT_HANDLE_OPEN) {
      if (fh->vol == vol) {
        /* File belongs to this volume */
        if (fat_close (i) != fsOK) {
          /* Failed to close the file */
        }
        fh->flags = 0;
      }
    }
  }
  /* Uninitialize media */
  vol->Drv->UnInit (DM_MEDIA);
  /* Update volume status */
  vol->Status &= ~(FAT_STATUS_MOUNT | FAT_STATUS_INIT_MEDIA);

  /* Drive is unmounted */
  EvrFsFAT_UnmountDriveSuccess (vol->DrvLet);

  return (fsOK);
}


/**
  Format a volume for FAT12, FAT16 or FAT32

  \param[in]  vol                       volume description structure
  \param[in]  opt                       formatting options as a NULL terminated string
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_format (fsFAT_Volume *vol, const char *opt) {
  uint32_t datSect, volSz, iSz, i, sec, mbr;
  uint8_t secClus;
  int32_t len;
  fsMediaInfo info;
  fsStatus    stat;
  const char *ptr;
  uint64_t calc;
  bool     skip;

  /* Formatting drive */
  EvrFsFAT_FormatDrive (vol->DrvLet);
  #ifdef FS_DEBUG
  if (strlen(opt) != 0) {
    EvrFsFAT_OptionsString (opt, strlen(opt));
  }
  #endif

  /* Check volume status */
  stat = fat_vol_chk (FAT_STATUS_INIT_MEDIA | FAT_STATUS_READY | FAT_STATUS_WRITE, vol);
  if (stat != fsOK) {
    return (stat);
  }

  /* Reset all active file handles on this drive */
  fat_handles_reset (vol);

  /* Check for parameter: /LLEB */
  if ((ptr = find_param (opt, "/LLEB")) != NULL) {
    EvrFsFAT_FormatOptionDetected (vol->DrvLet, 4);
    EvrFsFAT_FormatLowLevel (vol->DrvLet);

    /* Low level format the device. Parameter is passed to a device driver */
    if (vol->Drv->DeviceCtrl (fsDevCtrlCodeFormat, (char *)(uint32_t)ptr) != fsOK) {
      return (fsDriverError);
    }
  }
  else {
    /* Check for parameter: /LL */
    if (find_param (opt, "/LL") != NULL) {
      EvrFsFAT_FormatOptionDetected (vol->DrvLet, 3);
      EvrFsFAT_FormatLowLevel (vol->DrvLet);

      /* Low level format the device. Parameter is passed to a device driver */
      if (vol->Drv->DeviceCtrl (fsDevCtrlCodeFormat, NULL) != fsOK) {
        return (fsDriverError);
      }
    }
  }

  /* Check for parameter: /L */
  ptr = find_param (opt, "/L");
  len = 0;

  if (ptr != NULL) {
    /* Detected label specifier */
    EvrFsFAT_FormatOptionDetected (vol->DrvLet, 0);

    if (ptr[2] == ' ') {
      /* Validate label and get its length */
      len = validate_label (&ptr[3]);

      if (len == -1) {
        /* Invalid label specified */
        EvrFsFAT_LabelInvalid (vol->DrvLet);
        return (fsInvalidParameter);
      }
    }
  }

  /* Read media configuration info */
  if (vol->Drv->ReadInfo (&info) == false) {
    return (fsMediaError);
  }

  /* Check Block count */
  if (info.block_cnt == 0) {
    EvrFsFAT_FormatLowLevel (vol->DrvLet);
    /* Try also low level format. */
    if (vol->Drv->DeviceCtrl (fsDevCtrlCodeFormat, NULL) != fsOK) {
      return (fsDriverError);
    }

    /* Read again new block count */
    if (vol->Drv->ReadInfo (&info) == false) {
      return (fsMediaError);
    }
  }

  EvrFsFAT_FormatMediaCapacity (vol->DrvLet, info.block_cnt);

  /* Check Block count and Read/Write Block sizes. */
  if ((info.block_cnt < 36) || (info.read_blen != info.write_blen)) {
    /* Minimum block count to setup FAT12 is 36:                        */
    /* Boot sector + Reserved sector + FAT 1 + FAT 2 + 32 Root Sectors  */
    EvrFsFAT_FormatNoSpace (vol->DrvLet);
    return (fsNoFreeSpace);
  }

  volSz = info.block_cnt >> 11;
  for (iSz = 0, i = 8; iSz < 13; i <<= 1, iSz++) {
    if (volSz < i) break;
  }
  if (iSz == 13) {
    /* All sizes above 32GB. */
    iSz = 12;
  }

  /* Invalidate FAT cache */
  vol->fat.sect = INVAL_SECT;

  /* Check for parameter: /W */
  if (find_param (opt, "/W")) {
    EvrFsFAT_FormatOptionDetected (vol->DrvLet, 2);

    /* Clear the whole disk. */
    if (wipe_disk (vol, info.block_cnt) == false) {
      return (fsDriverError);
    }
  }

  /* Dont write MBR if this is RAM drive */
  if ((vol->DrvLet & 0xFF) == 'R') { mbr = 0; }
  else                             { mbr = 1; }

  if (mbr) {
    /* Format with partition table (MBR) */
    vol->cfg.BootSector = IniDevCfg[iSz].BootSector;
    secClus             = IniDevCfg[iSz].SecClus;
  }
  else {
    /* Don't write MBR */
    vol->cfg.BootSector = 0;

    /* Use small clusters when appropriate */
    if (info.block_cnt <= 4142) {
      secClus = 1;
    }
    else if (info.block_cnt <= 8229) {
      secClus = 2;
    }
    else {
      secClus = IniDevCfg[iSz].SecClus;
    }
  }

  /* Set Volume Parameter Info. */
  vol->cfg.FatType    = IniDevCfg[iSz].FatType;
  vol->cfg.DskSize    = info.block_cnt - vol->cfg.BootSector;
  vol->cfg.RsvdSecCnt = 1;
  vol->cfg.BytesPerSec= 512;
  vol->cfg.NumOfFat   = 2;

  /* Check for parameter: /FAT32 */
  if (find_param (opt, "/FAT32")) {
    EvrFsFAT_FormatOptionDetected (vol->DrvLet, 1);
    /* Force FAT32 */
    vol->cfg.FatType  = FS_FAT32;
  }

  /* Check if FAT32 detected or forced */
  if (vol->cfg.FatType == FS_FAT32) {
    vol->cfg.RsvdSecCnt     = 8;
    vol->cfg.FAT32_FSInfo   = 1;
    vol->cfg.FAT32_RootClus = 2;
    vol->cfg.FAT32_BkBootSec= 6;

    secClus  = IniDevCfg[iSz].SecClus32;
    if (secClus == 0) {
      /* Not possible to use FAT32, size is too small. */
      EvrFsFAT_FormatNoSpaceFAT32 (vol->DrvLet);
      return (fsNoFreeSpace);
    }
  }

  vol->cfg.SecPerClus  = secClus;
  vol->cfg.ClusSize    = secClus * 512;
  vol->cfg.EntsPerClus = (uint16_t)(vol->cfg.ClusSize / 32);

  datSect = vol->cfg.DskSize - vol->cfg.RsvdSecCnt;
  /* Calculate Data Space and FAT Table Size. */
  switch (vol->cfg.FatType) {
    case FS_FAT12:
      datSect -= 32;
      vol->cfg.RootEntCnt  = 512;
      vol->cfg.RootSecCnt  = 32;
      vol->cfg.DataClusCnt = (datSect * 512 - 1022) / (secClus * 512 + 3);
      vol->cfg.FatSize     = (uint32_t)((vol->cfg.DataClusCnt * 3 + 1022) / 1024);
      break;
    case FS_FAT16:
      datSect -= 32;
      vol->cfg.RootEntCnt  = 512;
      vol->cfg.RootSecCnt  = 32;
      vol->cfg.DataClusCnt = (datSect * 128 - 255) / (secClus * 128 + 1);
      vol->cfg.FatSize     = (uint32_t)((vol->cfg.DataClusCnt + 255) / 256);
      break;
    case FS_FAT32:
      vol->cfg.RootSecCnt  = 0;
      calc                = ((uint64_t)(datSect) * 64 - 127) / (secClus * 64 + 1);
      vol->cfg.DataClusCnt = (uint32_t)(calc);
      vol->cfg.FatSize     = (uint32_t)((vol->cfg.DataClusCnt + 127) / 128);
      break;
  }

  /* Output formatting properties */
  EvrFsFAT_FormatProperties (vol->DrvLet, vol->cfg.FatType, vol->cfg.ClusSize, vol->cfg.DataClusCnt);

  /* Count Data Sectors/Clusters */
  vol->cfg.DataSecCnt  = vol->cfg.DskSize - (vol->cfg.RsvdSecCnt + vol->cfg.RootSecCnt +
                                             vol->cfg.NumOfFat * vol->cfg.FatSize);
  vol->cfg.RootDirAddr = vol->cfg.RsvdSecCnt + vol->cfg.NumOfFat * vol->cfg.FatSize;

  if (mbr) {
    /* 2nd Cluster should be 32K aligned for optimal Card performance. */
    sec = vol->cfg.RootDirAddr + vol->cfg.RootSecCnt;
    vol->cfg.BootSector = ((vol->cfg.BootSector + sec) & (uint32_t)~0x3F) - sec;

    /* Write MBR, create Partition Table. */
    if (mbr_write (vol, iSz) == false) {
      return (fsDriverError);
    }
  }

  memset (vol->ca.buf, 0, 512);

  /* Clear hidden sectors */
  EvrFsFAT_ClearHiddenSectors (vol->DrvLet, 1, vol->cfg.BootSector);

  for (i = 1; i < vol->cfg.BootSector; i++) {
    if (write_cache (vol, i) == false) {
      return (fsDriverError);
    }
  }

  /* Generate and write Boot Sector */
  if (bs_write (vol) == false) {
    return (fsDriverError);
  }

  /* First 2 clusters are always reserved. */
  vol->free_clus = 2;

  if (vol->cfg.FatType == FS_FAT32) {
    vol->free_clus_cnt = vol->cfg.DataClusCnt - 1;

    /* Generate FSInfo on FAT32. */
    if (fsinfo_write (vol) == false) {
      return (fsDriverError);
    }
  }

  /* Clear data buffer */
  memset (vol->ca.buf, 0, 512);

  /* Set sector number to boot sector */
  sec = vol->cfg.BootSector;

  /* Clear reserved sectors */
  EvrFsFAT_ClearReservedSectors (vol->DrvLet, 1, vol->cfg.RsvdSecCnt);

  for (i = 1; i < vol->cfg.RsvdSecCnt; i++) {
    skip = false;

    if (vol->cfg.FatType == FS_FAT32) {
      if ((i == vol->cfg.FAT32_FSInfo) || (i == vol->cfg.FAT32_BkBootSec)) {
        /* Skip FSInfo structure and backup boot sector */
        skip = true;
      }
    }

    if (!skip) {
      if (write_cache (vol, sec + i) == false) {
        return (fsDriverError);
      }
    }
  }

  /* Set start of FAT 1 */
  sec = vol->cfg.BootSector + vol->cfg.RsvdSecCnt;

  /* Data buffer is cleared, only mark clusters 0 and 1 as reserved */
  switch (vol->cfg.FatType) {
    case FS_FAT12:
      set_u32 (&vol->ca.buf[0], 0xFFFFF8);
      break;
    case FS_FAT16:
      set_u32 (&vol->ca.buf[0], 0xFFFFFFF8);
      break;
    case FS_FAT32:
      /* Write also Root Dir Cluster to FAT table. */
      set_u32 (&vol->ca.buf[0], 0x0FFFFFF8);
      set_u32 (&vol->ca.buf[4], 0xFFFFFFFF);
      set_u32 (&vol->ca.buf[8], 0x0FFFFFFF);
      break;
  }
  
  /* Creating File Allocation Table (FAT) */
  EvrFsFAT_CreateFAT (vol->DrvLet, sec, vol->cfg.FatSize);

  /* Write first sector of FAT 1 */
  if (write_cache (vol, sec) == false) {
    return (fsDriverError);
  }
  /* Write first sector of FAT 2 */
  if (write_cache (vol, sec + vol->cfg.FatSize) == false) {
    return (fsDriverError);
  }

  /* Clear table for remaining FAT sectors */
  memset (vol->ca.buf, 0, 12);

  /* Clear remaining sectors of FAT 1 */
  for (i = 1; i < vol->cfg.FatSize; i++) {
    if (write_cache (vol, sec + i) == false) {
      return (fsDriverError);
    }
  }

  /* Set start of FAT 2 */
  sec += vol->cfg.FatSize;

  /* Clear remaining sectors of FAT 2 */
  for (i = 1; i < vol->cfg.FatSize; i++) {
    if (write_cache (vol, sec + i) == false) {
      return (fsDriverError);
    }
  }

  /* Set start of root/data area */
  sec += vol->cfg.FatSize;
  
  /* Set number of sectors to clear in data area / root directory */
  if (vol->cfg.FatType == FS_FAT32) {
    /* Clear first data cluster (FAT32) */
    datSect = vol->cfg.SecPerClus;
  }
  else {
    /* Clear root directory (FAT12, FAT16) */
    datSect = vol->cfg.RootSecCnt;
  }

  if ((ptr != NULL) && (len > 0)) {
    /* Write label */
    if (label_write (vol, &ptr[3], (uint32_t)len, sec) == false) {
      return (fsDriverError);
    }
    sec++;
    datSect--;
  }

  /* Clear sectors in the data area / root directory */
  EvrFsFAT_ClearRootSectors (vol->DrvLet, sec, datSect);

  for (i = 0; i < datSect; i++) {
    if (write_cache (vol, sec + i) == false) {
      return (fsDriverError);
    }
  }

  /* Flush the cache when done. */
  if (write_cache (vol, 0) == false) {
    return (fsDriverError);
  }

  /* Reserve space for journal */
  if (fat_jour_prep (vol) == false) {
    return (fsError);
  }

  /* Init journal */
  if (fat_jour_init (vol) == false) {
    return (fsError);
  }
  /* Flush FAT Name Cache */
  elink_cmd (ELINK_CMD_FLUSH, vol->ncache);

  /* Current directory is root */
  vol->cdir_clus = get_root_clus (vol);

  /* Status Media mounted. */
  vol->Status |= FAT_STATUS_MOUNT;

  /* Drive is formatted */
  EvrFsFAT_FormatDriveSuccess (vol->DrvLet);

  return (fsOK);
}


/**
  Open a file on the FAT file system

  \param[out] handle                    FAT file handle
  \param[in]  path                      file path
  \param[in]  openmode                  file open mode flags
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_open (int32_t handle, const char *path, int32_t openmode) {
  fsFAT_Handle *fh;
  PATH_INFO pinfo;
  fsStatus  status;
  fsTime    time;
  uint16_t  mode;

  EvrFsFAT_FileOpen (handle, path, openmode);
  EvrFsFAT_PathName (path, strlen(path));

  if ((handle < 0) || (handle >= fs_fat_fh_cnt)) {
    /* Invalid parameter: handle number out of range */
    EvrFsFAT_FileHandleInvalid (handle);
    return (fsInvalidParameter);
  }

  fh = &fs_fat_fh[handle];

  if (path == NULL) {
    /* Invalid parameter: path not specified */
    EvrFsFAT_InvalidParameter (fh->vol->DrvLet);
    return (fsInvalidParameter);
  }
  if (path_validate (path)) {
    return (fsInvalidPath);
  }

  status = fat_vol_chk (FAT_STATUS_INIT_IO    |
                        FAT_STATUS_INIT_MEDIA |
                        FAT_STATUS_READY      |
                        FAT_STATUS_MOUNT      , fh->vol);

  if (status != fsOK) {
    return (status);
  }

  if (openmode & FS_FOPEN_WR) {
    if (!(fh->vol->Status & FAT_STATUS_WRITE)) {
      /* Write protection is active */
      EvrFsFAT_VolumeWriteStatError (fh->vol->DrvLet);
      return (fsAccessDenied);
    }
  }

  /* Set the open mode flags */
  if (openmode & FS_FOPEN_RDWR) {
    /* File opened for read/write */
    mode = FAT_HANDLE_READ | FAT_HANDLE_WRITE | FAT_HANDLE_PLUS;
  }
  else if (openmode & FS_FOPEN_WR) {
    /* File opened for write only */
    mode = FAT_HANDLE_WRITE;
  }
  else {
    /* File opened for read only */
    mode = FAT_HANDLE_READ;
  }

  if (openmode & FS_FOPEN_APPEND) {
    /* File opened for append */
    mode |= FAT_HANDLE_APPEND;
  }

  path_init (path, &pinfo, fh->vol);

  status = path_open (&pinfo, fh->vol);

  if (status == fsOK) {
    if (pinfo.sfne->Attr & FS_FAT_ATTR_DIRECTORY) {
      /* This is name of a directory not file */
      EvrFsFAT_PathIsDirNotFile (fh->vol->DrvLet);
      return (fsInvalidPath);
    }

    /* Reposition frec to short entry */
    if (entry_pos_inc (&pinfo.frec.pos, pinfo.frec.cnt-1, fh->vol) != 0) {
      return (fsError);
    }
    if (file_locked (&pinfo.frec.pos, FAT_HANDLE_WRITE, fh->vol)) {
      /* Found another handle using current file and is opened for write */
      return (fsAccessDenied);
    }

    if ((openmode & FS_FOPEN_TRUNCATE) == FS_FOPEN_TRUNCATE) {
      /* Opening existing file for write - truncate the file */
      if (pinfo.sfne->Attr & FS_FAT_ATTR_READ_ONLY) {
        /* Read only attribute is set */
        EvrFsFAT_FileIsReadOnly (fh->vol->DrvLet);
        return (fsAccessDenied);
      }
      /* Remember first cluster for FAT update */
      fh->first_clus = extract_clus (pinfo.sfne);

      /* Get current system time */
      fs_get_time (&time);

      /* Update CrtTime, CrtDate, LastAccDate, WriteTime and WriteDate */
      set_u16 ((uint8_t *)&pinfo.sfne->CrtTime, format_time(&time));
      set_u16 ((uint8_t *)&pinfo.sfne->CrtDate, format_date(&time));
      pinfo.sfne->LastAccDate = pinfo.sfne->CrtDate;
      pinfo.sfne->WriteTime   = pinfo.sfne->CrtTime;
      pinfo.sfne->WriteDate   = pinfo.sfne->CrtDate;

      /* Update FirstCluster and Size */
      pinfo.sfne->FirstClusHI = 0;
      pinfo.sfne->FirstClusLO = 0;
      pinfo.sfne->FileSize    = 0;

      if (entry_flush (&pinfo.frec.pos, fh->vol) != fsOK) {
        return (fsError);
      }
      /* Update FAT */
      if (unlink_clus (fh->first_clus, fh->vol) == false) {
        return (fsError);
      }
      if (fh->vol->cfg.FatType == FS_FAT32) {
        /* Update FSInfo for FAT32 */
        if (fsinfo_write (fh->vol) == false) {
          return (fsError);
        }
      }
    }
  }
  else if (status == fsFileNotFound) {
    /* File does not exist */
    EvrFsFAT_FileIsNonExistent (fh->vol->DrvLet);

    if ((openmode & FS_FOPEN_CREATE) == FS_FOPEN_CREATE) {
      /* Create path */
      status = path_create (&pinfo, false, fh->vol);
    }
  }

  if (status == fsOK) {
    /* Read short entry */
    if (entry_read (&pinfo.frec.pos, &pinfo.sfne, fh->vol)) {
      return (fsError);
    }
    /* Save entry data */
    fh->fsize = get_u32 ((uint8_t *)&pinfo.sfne->FileSize);
    fh->fcsz  = fh->fsize;
    fh->fpos  = 0;

    fh->short_ent_clus = pinfo.frec.pos.Clus;
    fh->short_ent_offs = pinfo.frec.pos.Offs;

    fh->first_clus = extract_clus (pinfo.sfne);

    fh->current_clus = fh->first_clus;
    fh->current_sect = 0;
  }

  if (status == fsOK) {
    /* Set handle open modes */
    fh->flags = mode;
  }
  return (status);
}


/**
  Close a file on the FAT file system

  Flushes file content to media, updates FAT and directory entry.

  \param[in]  handle                    FAT file handle
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_close (int32_t handle) {
  fsFAT_Handle *fh;
  ENTRY_POS pos;
  FILEREC  *frec;
  fsStatus  stat;
  fsTime    time;
  uint16_t  ent_time, ent_date;
  bool      frec_update;

  EvrFsFAT_FileClose(handle);

  if ((handle < 0) || (handle >= fs_fat_fh_cnt)) {
    /* Invalid parameter: handle number out of range */
    EvrFsFAT_FileHandleInvalid (handle);
    return (fsInvalidParameter);
  }

  fh = &fs_fat_fh[handle];

  if ((fh->flags & FAT_HANDLE_ERROR) || !(fh->flags & FAT_HANDLE_OPEN)) {
    /* Handle error or file not opened */
    EvrFsFAT_FileHandleError (handle, fh->flags);
    return (fsError);
  }

  if (fh->flags & (FAT_HANDLE_DATA_RD | FAT_HANDLE_DATA_WR)) {
    /* Check volume status */
    stat = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT | FAT_STATUS_WRITE, fh->vol);
    if (stat != fsOK) {
      fh->flags |= FAT_HANDLE_ERROR;
      goto exit;
    }

    /* Check if short entry needs update */
    frec_update = false;

    pos.Clus = fh->short_ent_clus;
    pos.Offs = fh->short_ent_offs;

    if (entry_read (&pos, &frec, fh->vol)) {
      stat = fsError;
      goto exit;
    }
    /* Get current system time and convert it into entry format */
    fs_get_time (&time);

    ent_time = format_time(&time);
    ent_date = format_date(&time);

    if (get_u16((uint8_t *)&frec->LastAccDate) != ent_date) {
      /* Update last access date */
      set_u16 ((uint8_t *)&frec->LastAccDate, ent_date);
      frec_update = true;
    }

    if (fh->flags & FAT_HANDLE_DATA_WR) {
      frec_update = true;
      /* File written, flush content and close it */
      set_u16 ((uint8_t *)&frec->FirstClusHI, (uint16_t)(fh->first_clus >> 16));
      set_u16 ((uint8_t *)&frec->FirstClusLO, (uint16_t)(fh->first_clus      ));
      set_u32 ((uint8_t *)&frec->FileSize, fh->fcsz);

      /* Update write time and date */
      set_u16 ((uint8_t *)&frec->WriteTime, ent_time);
      set_u16 ((uint8_t *)&frec->WriteDate, ent_date);

      /* Update attributes */
      frec->Attr |= FS_FAT_ATTR_ARCHIVE;
    }

    if (frec_update) {
      /* Write updated short entry */
      if (entry_flush (&pos, fh->vol) != fsOK) {
        stat = fsError;
        goto exit;
      }
    }

    if (fh->flags & FAT_HANDLE_DATA_WR) {
      /* Update FSInfo (FAT32) */
      if (fh->vol->Status & FAT_STATUS_FSINFO) {
        fh->vol->Status &= ~FAT_STATUS_FSINFO;
        if (fsinfo_write (fh->vol) == false) {
          stat = fsError;
          goto exit;
        }
      }
      /* Flush cached sectors */
      if (write_cache (fh->vol, 0) == false) {
        stat = fsError;
        goto exit;
      }

      /* Allocation table might be dirty */
      if (cache_fat (fh->vol, 0) == false) {
        stat = fsError;
        goto exit;
      }
    }
  }
  else {
    stat = fsOK;
  }

exit:
  /* Invalidate file handle */
  fh->flags = 0;

  return stat;
}


/**
  Read data from file at current file position.

  \param[in]  handle                    file handle
  \param[out] buf                       data buffer
  \param[in]  len                       number of bytes to read
  \return     Number of bytes read or error indication if negative.
*/
__WEAK int32_t fat_read (int32_t handle, uint8_t *buf, uint32_t len) {
  fsFAT_Handle *fh;
  fsStatus status;
  uint32_t sect, pos, nr, rlen;

  EvrFsFAT_FileRead (handle, buf, len);

  if ((handle < 0) || (handle >= fs_fat_fh_cnt)) {
    /* Invalid parameter: handle number out of range */
    EvrFsFAT_FileHandleInvalid (handle);
    return (-(int32_t)fsInvalidParameter);
  }

  fh = &fs_fat_fh[handle];

  if (buf == NULL) {
    /* Invalid parameter: buffer not specified */
    EvrFsFAT_InvalidParameter (fh->vol->DrvLet);
    return (-(int32_t)fsInvalidParameter);
  }

  if (fh->flags & FAT_HANDLE_ERROR) {
    /* Handle error */
    return (-(int32_t)fsError);
  }

  if ((fh->flags & FAT_HANDLE_OPEN) == 0) {
    /* File not opened */
    return (-(int32_t)fsAccessDenied);
  }

  if ((fh->flags & FAT_HANDLE_MODES) == FAT_HANDLE_WRITE) {
    /* Only opened for write */
    return (-(int32_t)fsAccessDenied);
  }

  status = fat_vol_chk (FAT_STATUS_REMOVABLE | FAT_STATUS_READY | FAT_STATUS_MOUNT, fh->vol);
  if (status != fsOK) {
    fh->flags |= FAT_HANDLE_ERROR;
    return (-(int32_t)status);
  }

  if ((fh->fcsz - fh->fpos) < len) {
    /* Check for End Of File. */
    len = fh->fcsz - fh->fpos;
    if (len == 0) {
      /* End of File. */
      return (0);
    }
  }
  fh->flags |= FAT_HANDLE_DATA_RD;

  pos = fh->fpos & 0x1FF;
  for (nr = 0; nr < len; nr += rlen) {
    if (fh->current_sect == fh->vol->cfg.SecPerClus) {
      /* All sectors from current cluster are read, load next cluster */
      if (alloc_table_read (fh->current_clus, &fh->current_clus, fh->vol) == false) {
        /* Read error */
        return (-(int32_t)fsDriverError);
      }
      fh->current_sect = 0;
    }

    sect = clus_to_sect (&fh->vol->cfg, fh->current_clus) + fh->current_sect;
    /* Try to cache current cluster. */
    if (read_cache (fh->vol, sect, fh->vol->cfg.SecPerClus - fh->current_sect) == false) {
      /* Read error */
      return (-(int32_t)fsDriverError);
    }

    rlen = len - nr;
    if ((rlen + pos) > 512) {
      rlen = 512 - pos;
    }

    memcpy (&buf[nr], &fh->vol->ca.buf[pos], rlen);
    pos = (pos + rlen) & 0x1FF;
    if (pos == 0) {
      /* Current sector complete, get next one. */
      fh->current_sect++;
    }
  }
  fh->fpos += nr;

  /* Return number of characters read. */
  return ((int32_t)nr);
}


/**
  Write data to file at current file position.

  \param[in]  handle                    file handle
  \param[in]  buf                       data buffer
  \param[in]  len                       number of bytes to write
  \return     - a positive number representing the number of characters written
              - a negative number indicating an error
*/
__WEAK int32_t fat_write (int32_t handle, const uint8_t *buf, uint32_t len) {
  fsFAT_Handle *fh;
  fsStatus status;
  uint32_t sect,pos,cnt,wlen,clus,sz;

  EvrFsFAT_FileWrite (handle, buf, len);

  if ((handle < 0) || (handle >= fs_fat_fh_cnt)) {
    /* Invalid parameter: handle number out of range */
    EvrFsFAT_FileHandleInvalid (handle);
    return (-(int32_t)fsInvalidParameter);
  }

  fh = &fs_fat_fh[handle];

  if (buf == NULL) {
    /* Invalid parameter: buffer not specified */
    EvrFsFAT_InvalidParameter (fh->vol->DrvLet);
    return (-(int32_t)fsInvalidParameter);
  }

  if ((fh->flags & FAT_HANDLE_ERROR) || !(fh->flags & FAT_HANDLE_OPEN)) {
    /* Handle error or file not opened */
    EvrFsFAT_FileHandleError (handle, fh->flags);
    return (-(int32_t)fsError);
  }

  if ((fh->flags & FAT_HANDLE_MODES) == FAT_HANDLE_READ) {
    /* Only opened for read */
    EvrFsFAT_FileModeRead (handle);
    return (-(int32_t)fsAccessDenied);
  }

  status = fat_vol_chk (FAT_STATUS_REMOVABLE | FAT_STATUS_READY | FAT_STATUS_MOUNT, fh->vol);
  if (status != fsOK) {
    fh->flags |= FAT_HANDLE_ERROR;
    return (-(int32_t)status);
  }

  if ((fh->vol->Status & FAT_STATUS_WRITE) == 0) {
    /* Write protection is active */
    EvrFsFAT_VolumeWriteStatError (fh->vol->DrvLet);
    return (-(int32_t)fsAccessDenied);
  }
  fh->flags |= FAT_HANDLE_DATA_WR;

  if (fh->flags & (FAT_HANDLE_PLUS | FAT_HANDLE_SEEK)) {
    fh->flags &= ~FAT_HANDLE_SEEK;
    sz = 512;
  }
  else {
    sz = 0;
  }

  if (fh->first_clus == 0) {
    /* First data cluster not allocated yet */
    if (alloc_clus (&fh->first_clus, fh->vol) == false) {
      /* Out of space */
      return (-(int32_t)fsNoFreeSpace);
    }
    fh->current_clus = fh->first_clus;
    fh->current_sect = 0;
  }

  cnt = 0;

  while (cnt < len) {
    if (fh->fpos == 0xFFFFFFFE) {
      /* Maximum file size for FAT is 4GB                     */
      /* C library retarget works correctly until size 4GB-2B */
      break;
    }

    if (fh->current_sect == fh->vol->cfg.SecPerClus) {
      /* This cluster is filled, get next one. */
      if (fh->fpos < fh->fcsz) {
        /* Cluster is already allocated */
        if (alloc_table_read (fh->current_clus, &fh->current_clus, fh->vol) == false) {
          /* Read error */
          return (-(int32_t)fsDriverError);
        }
      }
      else {
        clus = fh->current_clus;
        /* Allocate a free cluster and link it to chain */
        if (alloc_clus (&fh->current_clus, fh->vol) == false) {
          /* Out of free space, return number of bytes written */
          break;
        }
        if (link_clus (clus, fh->current_clus, fh->vol) == false) {
          /* Write error */
          return (-(int32_t)fsDriverError);
        }
      }
      fh->current_sect = 0;
    }
    sect = clus_to_sect (&fh->vol->cfg, fh->current_clus) + fh->current_sect;

    /* Calculate write-length for the sector. */
    pos  = fh->fpos & 0x1FF;
    wlen = len - cnt;

    if ((pos + wlen) > 512) {
      wlen = 512 - pos;
    }

    if (fh->fpos > (0xFFFFFFFE - wlen)) {
      wlen = 0xFFFFFFFE - fh->fpos;
    }

    if ((pos != 0U) || (wlen < sz)) {
      /* File position not 512-byte aligned. */
      if (read_sector (fh->vol, sect) == false) {
        /* Read error */
        return (-(int32_t)fsDriverError);
      }
    }

    /* Write data to current sector */
    memcpy (&fh->vol->ca.buf[pos], &buf[cnt], wlen);

    if (write_cache (fh->vol, sect) == false) {
      /* Write error */
      return (-(int32_t)fsDriverError);
    }

    cnt      += wlen;
    fh->fpos += wlen;

    if ((fh->fpos & 0x1FF) == 0U) {
      /* Current sector is full, use next one. */
      fh->current_sect++;
    }
  }

  /* Adjust file size */
  if (fh->fpos > fh->fcsz) {
    fh->fcsz = fh->fpos;
  }

  return ((int32_t)cnt);
}


/**
  Flush file buffers.

  \param[in] handle                     file handle
  \return    -0: Operation succeeded
             -1: Operation failed
*/
__WEAK fsStatus fat_flush (int32_t handle) {
  fsFAT_Handle *fh;

  /* Flushing file buffer */
  EvrFsFAT_FileFlush (handle);

  fh = &fs_fat_fh[handle];

  if (write_cache (fh->vol, 0) == false) {
    /* Write error */
    return (fsDriverError);
  }
  if (cache_fat (fh->vol, 0) == false) {
    /* Write error */
    return (fsDriverError);
  }
  /* Update FSInfo (FAT32) */
  if (fh->vol->Status & FAT_STATUS_FSINFO) {
    fh->vol->Status &= ~FAT_STATUS_FSINFO;

    if (fsinfo_write (fh->vol) == false) {
      /* Write error */
      return (fsDriverError);
    }
  }
  return (fsOK);
}


/**
  Get the current length of a file

  \param[in] handle                     file handle
  \return    Current length of the file or negative error indicator.
*/
__WEAK int32_t fat_flen (int32_t handle) {
  fsFAT_Handle *fh;
  int32_t rval;

  rval = -(int32_t)fsError;

  fh = &fs_fat_fh[handle];

  if (fh->flags & FAT_HANDLE_OPEN) {
    if ((fh->fcsz & 0x80000000) == 0) {
      rval = (int32_t)fh->fcsz;
    }
  }

  return (rval);
}


/**
  Set File Position pointer.

  \param[in]  handle                    file handle
  \param[in]  offset                    the number of bytes to move
  \param[in]  whence                    file position location
  \return     current file position from the beginning of the file,
              or negative \ref fsStatus return code on failure
*/
__WEAK int64_t fat_seek (int32_t handle, int64_t offset, int32_t whence) {
  fsFAT_Handle *fh;
  fsStatus      stat;
  uint32_t i, cnt, clus, link;
  uint32_t offs, wlen, len;
  uint8_t sect;
  uint32_t pos;

  /* Setting file position */
  EvrFsFAT_FileSeek (handle, offset, whence);

  if ((handle < 0) || (handle >= fs_fat_fh_cnt)) {
    /* Invalid parameter: handle number out of range */
    EvrFsFAT_FileHandleInvalid (handle);
    return -(int64_t)(fsInvalidParameter);
  }

  fh = &fs_fat_fh[handle];

  if ((fh->flags & FAT_HANDLE_ERROR) || !(fh->flags & FAT_HANDLE_OPEN)) {
    /* Handle error or file not opened */
    EvrFsFAT_FileHandleError (handle, fh->flags);
    return -(int64_t)(fsError);
  }

  stat = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT, fh->vol);
  if (stat != fsOK) {
    fh->flags |= FAT_HANDLE_ERROR;
    return -(int64_t)(stat);
  }

  if (whence == FS_FSEEK_SET) {
    pos = 0U;
  }
  else if (whence == FS_FSEEK_CUR) {
    pos = fh->fpos;
  }
  else if (whence == FS_FSEEK_END) {
    pos = fh->fcsz;
  }
  else {
    /* Invalid parameter: invalid seek operation */
    return -(int64_t)(fsInvalidParameter);
  }

  /* Set new position (from the start of the file) */
  pos = (uint32_t)((int32_t)pos + offset);

  if (pos == fh->fpos) {
    /* We are already here */
    return (fsOK);
  }

  if (pos > fh->fcsz) {
    /* Increase file size */
    EvrFsFAT_FileSeekIncrease (handle, fh->fcsz, pos);

    if (fh->first_clus == 0) {
      /* First data cluster not yet allocated */
      if (alloc_clus (&fh->first_clus, fh->vol) == false) {
        return -(int64_t)(fsDriverError);
      }
      if (clear_clus (fh->first_clus, fh->vol) == false) {
        return -(int64_t)(fsDriverError);
      }
      clus = fh->first_clus;
    }
    else {
      if (follow_clus (&fh->current_clus, &link, get_EOC(fh->vol->cfg.FatType), fh->vol) == false) {
        /* R/W error */
        return -(int64_t)(fsDriverError);
      }
      clus = fh->current_clus;
    }

    sect = (fh->fcsz / 512) % fh->vol->cfg.SecPerClus;
    offs = fh->fcsz & 0x1FF;
    len  = pos - fh->fcsz;

    for (cnt = 0; cnt < len; cnt += wlen) {
      i = clus_to_sect (&fh->vol->cfg, clus) + sect;

      wlen = len - cnt;
      if ((wlen + offs) > 512) {
        wlen = 512 - offs;
      }

      if (offs) {
        /* File position not 512-byte aligned. */
        if (read_sector (fh->vol, i) == false) {
          return -(int64_t)(fsError);
        }
      }

      memset (&fh->vol->ca.buf[offs], 0, wlen);
      if (write_cache (fh->vol, i) == false) {
        return -(int64_t)(fsError);
      }

      offs = (offs + wlen) & 0x1FF;
      if (offs == 0) {
        /* Current sector is full, use next one. */
        sect++;

        if (sect == fh->vol->cfg.SecPerClus) {
          /* Allocate a free cluster and link it to chain */
          if (alloc_clus (&link, fh->vol) == false) {
            return -(int64_t)(fsError);
          }
          if (link_clus (clus, link, fh->vol) == false) {
            return -(int64_t)(fsError);
          }
          clus = link;
          sect = 0;
        }
      }
    }
    fh->fcsz += len;
  }
  else /* if (pos <= fh->fcsz) */ {
    fh->flags |= FAT_HANDLE_SEEK;

    if (pos < fh->fpos) {
      /* Scan the cluster chain from the first cluster */
      clus = fh->first_clus;
      cnt  = pos / fh->vol->cfg.ClusSize;
    }
    else /* if (pos > fh->fpos) */ {
      /* Scan the cluster chain from current cluster */
      clus = fh->current_clus;
      i    = fh->fpos & ~(fh->vol->cfg.ClusSize-1);
      cnt  = (pos - i) / fh->vol->cfg.ClusSize;

      if (fh->current_sect == fh->vol->cfg.SecPerClus) {
        /* Current sector is out of bound, load next cluster */
        if (alloc_table_read (clus, &clus, fh->vol) == false) {
          return -(int64_t)(fsError);
        }
      }
    }

    link = clus;

    for (i = cnt; i; i--) {
      link = clus;
      if (alloc_table_read (link, &clus, fh->vol) == false) {
        return -(int64_t)(fsError);
      }
    }

    if (clus == get_EOC(fh->vol->cfg.FatType)) {
      sect = fh->vol->cfg.SecPerClus;
      clus = link;
    }
    else {
      sect = (pos / 512) % fh->vol->cfg.SecPerClus;
    }
  }
  /* Set new position */
  fh->current_clus = clus;
  fh->current_sect = sect;
  fh->fpos = pos;

  return (fh->fpos);
}


/**
  Delete a file or directory from requested directory.

  \param[in]  path                      file name
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
  \note       This function is included to support legacy code.
*/
__WEAK fsStatus fat_delete_l (const char *path, fsFAT_Volume *vol) {
  PATH_INFO pinfo;
  fsStatus  status;
  uint32_t  first_clus;

  /* Deleting file */
  EvrFsFAT_FileDelete (vol->DrvLet, path);
  EvrFsFAT_PathName (path, strlen(path));

  if (path == NULL) {
    /* Invalid parameter: path not specified */
    EvrFsFAT_InvalidParameter (vol->DrvLet);
    return (fsInvalidParameter);
  }
  if (path_validate (path)) {
    return (fsInvalidPath);
  }

  status = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT | FAT_STATUS_WRITE, vol);
  if (status != fsOK) {
    return (status);
  }

  path_init (path, &pinfo, vol);

  status = path_open (&pinfo, vol);

  if (status == fsOK) {
    first_clus = extract_clus (pinfo.sfne);

    if (pinfo.sfne->Attr & FS_FAT_ATTR_DIRECTORY) {
      /* Check if directory empty */
      if (is_dir_empty (first_clus, vol) == false) {
        /* Directory is not empty, do not delete it. */
        return (fsError);
      }
    }
    /* Delete entry link from cache */
    frec_delete_elink (&pinfo.frec, vol);
    /* Delete directory entries */
    status = frec_delete (&pinfo.frec, vol);

    if (status == fsOK) {
      /* Unlink FAT cluster chain */
      if (unlink_clus (first_clus, vol) == false) {
        return (fsError);
      }

      /* Write FSInfo for FAT32. */
      if (vol->cfg.FatType == FS_FAT32) {
        if (fsinfo_write (vol) == false) {
          return (fsError);
        }
        /* Flush cache */
        if (write_cache (vol, 0) == false) {
          return (fsError);
        }
      }
      /* Flush allocation table */
      if (cache_fat (vol, 0) == false) {
        return (fsError);
      }

      /* File deleted */
      EvrFsFAT_FileDeleteSuccess (vol->DrvLet, path);
    }
  }
  return (status);
}


/**
  Delete a file from requested directory.

  \param[in]  path                      file name
  \param[in]  options                   function options
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_delete (const char *path, const char *options, fsFAT_Volume *vol) {
  PATH_INFO pinfo;
  fsStatus  status;
  uint32_t  clus, edir;
  uint8_t   state, subd, opt, skip;
  FREC_POS  frec;

  /* Deleting file */
  EvrFsFAT_FileDelete (vol->DrvLet, path);
  EvrFsFAT_PathName (path, strlen(path));
  #ifdef FS_DEBUG
  if (strlen(options) != 0) {
    EvrFsFAT_OptionsString (options, strlen(options));
  }
  #endif

  if (path == NULL) {
    /* Invalid parameter: path not specified */
    EvrFsFAT_InvalidParameter (vol->DrvLet);
    return (fsInvalidParameter);
  }
  if (path_validate (path)) {
    return (fsInvalidPath);
  }

  status = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT | FAT_STATUS_WRITE, vol);
  if (status != fsOK) {
    return (status);
  }

  /* Check function options */
  opt = 0;
  if (find_param (options, "/S")) {
    opt |= FAT_OPTION_S;
  }

  path_init (path, &pinfo, vol);

  status = path_open (&pinfo, vol);
  if (status != fsOK) {
    return status;
  }

  if (pinfo.sfne->Attr & FS_FAT_ATTR_DIRECTORY) {
    /* Specified path is a directory */
    state = 0;
  }
  else {
    /* Specified path is a file */
    state = 3;
  }
  subd = 0;
  skip = 0;
  edir = 0;

  while (state < 5) {
    switch (state) {
      case 0:   /* Step into the directory */
        clus = extract_clus (pinfo.sfne);

        if (skip) {
          if (clus == edir) {
            skip = 0;
          }
          state = 4; /* -> Load next directory entry */
        }
        else {
          pinfo.dir_clus = clus;

          pinfo.frec.pos.Clus = clus;
          pinfo.frec.pos.Offs = 2;

          elink_dir (pinfo.dir_clus, ELINK_DIR_DEPTH_INC, vol->ncache);

          subd++;
          state = 2; /* -> Load directory entry */
        }
        break;

      case 1:   /* Step out of dir */
        subd--;

        pinfo.frec.pos.Clus = pinfo.dir_clus;
        pinfo.frec.pos.Offs = 1;
        status = frec_next (&pinfo, NULL, 0, vol);
        if (status != fsOK) {
          return (status);
        }
        clus = extract_clus (pinfo.sfne);

        pinfo.dir_clus = clus;

        pinfo.frec.pos.Clus = clus;
        pinfo.frec.pos.Offs = 2;

        elink_dir (pinfo.dir_clus, ELINK_DIR_DEPTH_DEC, vol->ncache);

        state = 2; /* -> Load directory entry */
        break;

      case 2:   /* Load directory entry */
        status = frec_next (&pinfo, NULL, 0, vol);

        if (status == fsFileNotFound) {
          /* Directory is empty */
          edir  = pinfo.dir_clus;
          state = 6; /* -> Exit */

          if (opt & FAT_OPTION_S) {
            if (subd > 1) {
              skip  = 1;
              state = 1; /* -> Step out of dir */
            }
          }
        }
        else {
          if (status != fsOK) {
            return (status);
          }

          if (pinfo.sfne->Attr & FS_FAT_ATTR_DIRECTORY) {
            if (opt & FAT_OPTION_S) {
              state = 0; /* -> Step into dir */
            }
            else {
              state = 4; /* -> Set next entry */
            }
          }
          else {
            state = 3; /* -> Delete entry */
          }
        }
        break;

      case 3: /* Delete entry */
        /* Check if file open */
        frec = pinfo.frec;

        /* Reposition frec to short entry */
        if (entry_pos_inc (&frec.pos, frec.cnt-1, vol) != 0) {
          return (fsError);
        }
        if (file_locked (&frec.pos, FAT_HANDLE_OPEN, vol)) {
          /* This file is already opened */
          return (fsAccessDenied);
        }

        clus = extract_clus (pinfo.sfne);

        /* Unlink FAT cluster chain */
        if (unlink_clus (clus, vol) == false) {
          return (fsError);
        }

        /* Delete entry link from cache */
        frec_delete_elink (&pinfo.frec, vol);
        /* Delete directory entries */
        status = frec_delete (&pinfo.frec, vol);

        if (status != fsOK) {
          return (status);
        }

        if (subd) {
          state = 4; /* -> Set next entry */
        }
        else {
          state = 5;
        }
        break;

      case 4: /* Set next entry */
        if (entry_pos_inc (&pinfo.frec.pos, pinfo.frec.cnt, vol)) {
          return (fsError);
        }
        state = 2;
        break;

      default: return (fsError);
    }
  }

  if (vol->cfg.FatType == FS_FAT32) {
    /* Write FSInfo structure */
    if (fsinfo_write (vol) == false) {
      return (fsError);
    }
    /* Flush cache */
    if (write_cache (vol, 0) == false) {
      return (fsError);
    }
  }
  /* Flush allocation table */
  if (cache_fat (vol, 0) == false) {
    return (fsError);
  }

  /* File deleted */
  EvrFsFAT_FileDeleteSuccess (vol->DrvLet, path);

  return (fsOK);
}


/**
  Find a file or directory in requested directory.

  \param[in]  fn                        path name
  \param[out] info                      file information structure
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_ffind (const char *fn, fsFileInfo *info, fsFAT_Volume *vol) {
  PATH_INFO pinfo;
  fsStatus  status;

  if ((fn == NULL) || (info == NULL)) {
    /* Invalid parameters */
    EvrFsFAT_InvalidParameter (vol->DrvLet);
    return (fsInvalidParameter);
  }
  if (*fn == '\0') {
    return (fsInvalidPath);
  }

  status = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT, vol);
  if (status != fsOK) {
    return (status);
  }

  path_init (fn, &pinfo, vol);

  status = path_open (&pinfo, vol);

  if (status == fsOK) {
    if (pinfo.fn_flags & FAT_NAME_WILDCARD) {
      /* Reposition entry offset */
      pinfo.frec.pos.Clus = pinfo.dir_clus;
      pinfo.frec.pos.Offs = 0;

      if (entry_pos_inc (&pinfo.frec.pos, info->fileID, vol)) {
        return (fsFileNotFound);
      }

      /* Find next valid name */
      status = frec_next (&pinfo, info->name, 256, vol);
      if (status != fsOK) {
        return (status);
      }
    }
    else {
      /* Explicit search without wildcard */
      if (info->fileID != 0) {
        /* We found this file already in the previous iteration */
        return (fsError);
      }
      /* Copy name from path information */
      memcpy (info->name, pinfo.fn, pinfo.fn_len);
      /* Terminate name string */
      info->name[pinfo.fn_len] = '\0';
    }

    /* Fill up info structure */
    info->size   = get_u32 ((uint8_t *)&pinfo.sfne->FileSize);
    info->fileID = (uint16_t)(pinfo.frec.pos.Offs + pinfo.frec.cnt);
    info->attrib = pinfo.sfne->Attr;

    extract_date (pinfo.sfne->WriteDate, &info->time);
    extract_time (pinfo.sfne->WriteTime, &info->time);
  }
  return (status);
}


/**
  Rename a file or directory

  \param[in]  path                      path to the file or directory to rename
  \param[in]  newname                   new name
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_rename (const char *path, const char *newname, fsFAT_Volume *vol) {
  PATH_INFO pinfo;
  FREC_POS  ent;
  FILEREC   old_entry;
  fsTime    time;
  fsStatus  status;
  uint32_t  hash;
  ELINK    *el;

  /* Renaming file */
  EvrFsFAT_FileRename (vol->DrvLet, path, newname);
  EvrFsFAT_PathName (path, strlen(path));
  EvrFsFAT_PathName (newname, strlen(newname));

  if ((path == NULL) || (newname == NULL)) {
    /* Invalid parameters */
    EvrFsFAT_InvalidParameter (vol->DrvLet);
    return (fsInvalidParameter);
  }
  if (path_validate (path)) {
    return (fsInvalidPath);
  }
  if (name_validate (newname, strlen(newname)) == 0U) {
    return (fsInvalidPath);
  }

  status = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT | FAT_STATUS_WRITE, vol);
  if (status != fsOK) {
    return (status);
  }

  path_init (path, &pinfo, vol);

  status = path_open (&pinfo, vol);

  if (status == fsOK) {
    if (vol->ncache) {
      /* Get current name hash so we can find it later on */
      hash = name_hash (pinfo.fn, pinfo.fn_len);
    }
    else { hash = 0; }
    /* Old path found, save short entry */
    memcpy (&old_entry, pinfo.sfne, sizeof (FILEREC));

    /* Save frec position */
    ent = pinfo.frec;

    /* Reposition frec to short entry */
    if (entry_pos_inc (&pinfo.frec.pos, pinfo.frec.cnt-1, vol) != 0) {
      return (fsError);
    }
    if (file_locked (&pinfo.frec.pos, FAT_HANDLE_OPEN, vol)) {
      /* This file is already opened */
      return (fsAccessDenied);
    }

    /* Check if new name already exists */
    pinfo.fn = newname;

    status = path_open (&pinfo, vol);

    if (status == fsFileNotFound) {
      /* Create new path */
      pinfo.fn_flags = 0;
      status = path_create (&pinfo, false, vol);

      if (status == fsOK) {
        /* When path is created, frec.pos is already at SN entry!!! */
        if (entry_read (&pinfo.frec.pos, &pinfo.sfne, vol)) {
          return (fsError);
        }
        /* Copy some data from the old entry */
        pinfo.sfne->Attr         = old_entry.Attr | FS_FAT_ATTR_ARCHIVE;
        pinfo.sfne->NTRsvd       = 0;
        pinfo.sfne->CrtTimeTenth = 0;
        pinfo.sfne->CrtTime      = old_entry.CrtTime;
        pinfo.sfne->CrtDate      = old_entry.CrtDate;
        fs_get_time (&time);
        set_u16 ((uint8_t *)&pinfo.sfne->LastAccDate, format_date(&time));
        pinfo.sfne->FirstClusHI  = old_entry.FirstClusHI;
        pinfo.sfne->WriteTime    = old_entry.WriteTime;
        pinfo.sfne->WriteDate    = old_entry.WriteDate;
        pinfo.sfne->FirstClusLO  = old_entry.FirstClusLO;
        pinfo.sfne->FileSize     = old_entry.FileSize;

        if (entry_flush (&pinfo.frec.pos, vol)) {
          return (fsError);
        }
        /* Delete old file record from the name cache */
        if (elink_scan (hash, &el, vol->ncache) == 0) {
          frec_delete_elink (&ent, vol);
        }
        /* Delete old file record */
        status = frec_delete (&ent, vol);
      }
    }
    else if (status == fsOK) {
      if ((pinfo.frec.pos.Offs != ent.pos.Offs) || (pinfo.frec.pos.Clus != ent.pos.Clus)) {
        /* Cannot rename, name exists */
        status = fsAlreadyExists;
      }
    }
  }
  /* else if (status == fsFileNotFound) { Original path not found }*/

#ifdef FS_DEBUG
  if (status == fsOK) {
    EvrFsFAT_FileRenameSuccess (vol->DrvLet, path, newname);
  }
#endif

  return (status);
}


/**
  Create a directory

  \param[in]  path                      directory path
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_mkdir (const char *path, fsFAT_Volume *vol) {
  PATH_INFO pinfo;
  fsStatus status;

  /* Creating directory */
  EvrFsFAT_DirCreate (vol->DrvLet, path);
  EvrFsFAT_PathName (path, strlen(path));

  if (path == NULL) {
    /* Invalid parameters */
    EvrFsFAT_InvalidParameter (vol->DrvLet);
    return (fsInvalidParameter);
  }
  if (path_validate (path)) {
    return (fsInvalidPath);
  }

  status = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT | FAT_STATUS_WRITE, vol);
  if (status != fsOK) {
    return (status);
  }

  path_init (path, &pinfo, vol);

  status = path_open (&pinfo, vol);

  if (status == fsFileNotFound) {
    /* Create directory */
    status = path_create (&pinfo, true, vol);

    if (status == fsOK) {
      /* Flush allocation table */
      if (cache_fat (vol, 0) == false) {
        return (fsError);
      }
    }
  }
  else {
    if (status == fsOK) {
      /* Path already exists */
      return (fsAlreadyExists);
    }
  }
#ifdef FS_DEBUG
  if (status == fsOK) {
    EvrFsFAT_DirCreateSuccess (vol->DrvLet, path);
  }
#endif
  return (status);
}


/**
  Remove a directory

  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_rmdir (const char *path, const char *options, fsFAT_Volume *vol) {
  PATH_INFO pinfo;
  FREC_POS  fpos;
  fsStatus  status;
  uint32_t  clus, dclus;
  uint8_t   state, subd, opt;

  /* Removing directory */
  EvrFsFAT_DirRemove (vol->DrvLet, path, options);
  EvrFsFAT_PathName (path, strlen(path));
  #ifdef FS_DEBUG
  if (strlen(options) != 0) {
    EvrFsFAT_OptionsString (options, strlen(options));
  }
  #endif

  if (path == NULL) {
    /* Invalid parameters */
    EvrFsFAT_InvalidParameter (vol->DrvLet);
    return (fsInvalidParameter);
  }
  if (path_validate (path)) {
    return (fsInvalidPath);
  }

  status = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT | FAT_STATUS_WRITE, vol);
  if (status != fsOK) {
    return (status);
  }

  opt = 0;
  if (find_param (options, "/S")) {
    opt |= FAT_OPTION_S;
  }

  path_init (path, &pinfo, vol);

  status = path_open (&pinfo, vol);
  if (status != fsOK) {
    return status;
  }

  if (!(pinfo.sfne->Attr & FS_FAT_ATTR_DIRECTORY)) {
    /* Path is not a directory */
    return (fsNotDirectory);
  }
  /* Store starting point directory info */
  dclus = pinfo.dir_clus;
  fpos  = pinfo.frec;

  state = 0;
  subd  = 0;

  while (state < 5) {
    switch (state) {
      case 0:   /* Step into the directory */
        clus = extract_clus (pinfo.sfne);

        /* Set new directory cluster */
        pinfo.dir_clus = clus;

        /* Set position to first entry after dot entries */
        pinfo.frec.pos.Clus = clus;
        pinfo.frec.pos.Offs = 2;

        elink_dir (pinfo.dir_clus, ELINK_DIR_DEPTH_INC, vol->ncache);

        subd++;
        state = 2; /* -> Load next entry */
        break;

      case 1:   /* Step out of dir */
        subd--;

        if (subd) {
          /* Determine parent directory cluster */
          pinfo.frec.pos.Clus = pinfo.dir_clus;
          pinfo.frec.pos.Offs = 1;
          status = frec_next (&pinfo, NULL, 0, vol);
          if (status != fsOK) {
            return (status);
          }
          clus = extract_clus (pinfo.sfne);

          /* Set parent directory cluster */
          pinfo.dir_clus = clus;

          pinfo.frec.pos.Clus = clus;
          pinfo.frec.pos.Offs = 2;
        }
        else {
          /* Set directory cluster of the starting point */
          pinfo.dir_clus = dclus;
          pinfo.frec = fpos;
        }
        elink_dir (pinfo.dir_clus, ELINK_DIR_DEPTH_DEC, vol->ncache);

        status = frec_next (&pinfo, NULL, 0, vol);

        if (status != fsOK) {
          return (status);
        }
        state = 3; /* -> Delete entry */
        break;

      case 2:   /* Load next entry */
        status = frec_next (&pinfo, NULL, 0, vol);

        if (status == fsFileNotFound) {
          /* Directory is empty */
          state = 1; /* -> Step out of dir */
        }
        else {
          if (status != fsOK) {
            return (status);
          }
          if (opt & FAT_OPTION_S) {
            if (pinfo.sfne->Attr & FS_FAT_ATTR_DIRECTORY) {
              state = 0; /* -> Step into dir */
            }
            else {
              state = 3; /* -> Delete entry */
            }
          }
          else {
            return (fsDirNotEmpty);
          }
        }
        break;

      case 3: /* Delete entry */
        clus = extract_clus (pinfo.sfne);

        /* Unlink FAT cluster chain */
        if (unlink_clus (clus, vol) == false) {
          return (fsError);
        }

        /* Delete entry link from cache */
        frec_delete_elink (&pinfo.frec, vol);
        /* Delete directory entries */
        status = frec_delete (&pinfo.frec, vol);

        if (status != fsOK) {
          return (status);
        }

        if (subd) {
          state = 4; /* -> Set next entry */
        }
        else {
          state = 5; /* -> Exit */
        }
        break;

      case 4: /* Set next entry */
        if (entry_pos_inc (&pinfo.frec.pos, pinfo.frec.cnt, vol)) {
          return (fsError);
        }
        state = 2; /* -> Load next entry */
        break;

      default: return (fsError);
    }
  }

  /* Write FSInfo for FAT32. */
  if (vol->cfg.FatType == FS_FAT32) {
    if (fsinfo_write (vol) == false) {
      return (fsError);
    }
    /* Flush cache */
    if (write_cache (vol, 0) == false) {
      return (fsError);
    }
  }
  /* Flush allocation table */
  if (cache_fat (vol, 0) == false) {
    return (fsError);
  }

  /* Directory removed */
  EvrFsFAT_DirRemoveSuccess (vol->DrvLet, path, options);

  return (fsOK);
}


/**
  \brief Check if media present on removable FAT drive.
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
                - fsOK               = Operation successful.
                - fsNoMedia          = Media not present.
                - fsAccessDenied     = Tried to access non-removable drive.
*/
__WEAK fsStatus fat_media (fsFAT_Volume *vol) {
  fsStatus status;

  /* Assume removable media */
  vol->Status |= FAT_STATUS_REMOVABLE;

  status = fat_vol_chk (FAT_STATUS_INIT_IO, vol);
  
  if (status == fsOK) {
    /* Check if removable status was cleared */
    if ((vol->Status & FAT_STATUS_REMOVABLE) == 0U) {
      status = fsUnsupported;
    }
  }

  return (status);
}


/**
  Read drive information.

  \param[out] info                      drive information structure.
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_info (fsDriveInfo *info, fsFAT_Volume *vol) {
  fsStatus status;

  if (info == NULL) {
    /* Invalid parameter */
    EvrFsFAT_InvalidParameter (vol->DrvLet);
    status = fsInvalidParameter;
  }
  else {
    status = fat_vol_chk (FAT_STATUS_MOUNT, vol);

    if (status == fsOK) {
      switch (vol->cfg.FatType) {
        case FS_RAW:   info->fs_type = fsTypeNone;  break;
        case FS_FAT12: info->fs_type = fsTypeFAT12; break;
        case FS_FAT16: info->fs_type = fsTypeFAT16; break;
        case FS_FAT32: info->fs_type = fsTypeFAT32; break;

        default:
          status = fsError;
          break;
      }
      info->capacity = (uint64_t)vol->cfg.DskSize * 512;
    }
  }

  return (status);
}


/**
  Change working directory.

  Causes the directory named by the path argument to become the current working
  directory - that is, the starting point for path searches for pathnames not
  beginning with '/'.
  Path can be specified using drive letter and directory path. If
  drive letter is omitted, current drive is used.

  \param[in]  path                      string specifying directory path.
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_chdir (const char *path, fsFAT_Volume *vol) {
  PATH_INFO pinfo;
  fsStatus  status;

  /* Setting current directory */
  EvrFsFAT_ChDir (vol->DrvLet, path);
  EvrFsFAT_PathName (path, strlen(path));

  if (path == NULL) {
    /* Invalid parameters */
    EvrFsFAT_InvalidParameter (vol->DrvLet);
    return (fsInvalidParameter);
  }

  if (path_validate (path)) {
    return (fsInvalidPath);
  }

  status = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT, vol);
  if (status != fsOK) {
    return (status);
  }

  path_init (path, &pinfo, vol);

  if ((pinfo.fn[0] == '\\' || pinfo.fn[0] == '/') && (pinfo.fn[1] == '\0')) {
    /* Root will be the current directory */
    vol->cdir_clus = get_root_clus (vol);

    status = fsOK;
  }
  else if ((pinfo.fn[0] == '.') && (pinfo.fn[1] == '\0')) {
    /* Current directory specified, nothing to do */
    status = fsOK;
  }
  else {
    status = path_open (&pinfo, vol);

    if (status == fsOK) {
      if (!(pinfo.sfne->Attr & FS_FAT_ATTR_DIRECTORY)) {
        /* Not a directory */
        return (fsNotDirectory);
      }
      vol->cdir_clus = extract_clus (pinfo.sfne);

      if (vol->cdir_clus == 0) {
      /* Current directory is root */
        vol->cdir_clus = get_root_clus (vol);
      }

      if (pinfo.fn_flags & FAT_NAME_DOTDOT) {
        /* Decrement name cache path depth */
        elink_dir (vol->cdir_clus, ELINK_DIR_DEPTH_DEC, vol->ncache);
      }
      else {
        /* Increment name cache path depth */
        elink_dir (vol->cdir_clus, ELINK_DIR_DEPTH_INC, vol->ncache);
      }
    }
  }

  if (status == fsOK) {
    if (vol->ncache) {
      pinfo.dir_clus      = vol->cdir_clus;
      pinfo.frec.pos.Clus = vol->cdir_clus;
      pinfo.frec.pos.Offs = 0;

      if (frec_next (&pinfo, NULL, 0, vol) == fsOK) {
        elink_cmd (ELINK_CMD_CHDIR, vol->ncache);
      }
    }
    EvrFsFAT_ChDirSuccess (vol->DrvLet, path);
  }

  return (status);
}


/**
  Print working directory.

  Used to output the absolute path of the current working directory in the
  array pointed to by \a path. The \a len argument is the size in bytes of
  the \a path array and must be at least the size of target file system
  maximum path length.
  If \a len argument is less than maximum path length, function returns with \ref
  fsInvalidParameter and content of \a path remains unchanged.

  \param[out] path                      pointer to string buffer
  \param[in]  len                       path buffer size
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_pwd (char *path, uint32_t len, fsFAT_Volume *vol) {
  PATH_INFO   pinfo;
  uint32_t    clus, root, parent, child;
  fsStatus    status;
  uint32_t    i, offs, sz;

  EvrFsFAT_Pwd (vol->DrvLet, path, len);

  if ((path == NULL) || (len < FAT_MAX_PATH)) {
    /* Invalid parameters */
    EvrFsFAT_InvalidParameter (vol->DrvLet);
    return (fsInvalidParameter);
  }

  status = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT, vol);
  if (status != fsOK) {
    return (status);
  }

  path[0] = '\0';

  offs  = len - 1;
  root  = get_root_clus(vol);
  child = vol->cdir_clus;

  while (child != root) {
    /* Read "dotdot" entry to get parent directory cluster */
    pinfo.frec.pos.Clus = child;
    pinfo.frec.pos.Offs = 1;

    if (entry_read (&pinfo.frec.pos, &pinfo.sfne, vol)) {
      return (fsDriverError);
    }
    parent = extract_clus (pinfo.sfne);

    /* Search for child directory entry */
    if (parent == 0) {
      parent = root;
      pinfo.frec.pos.Offs = 0;
    }
    else {
      pinfo.frec.pos.Offs = 2;
    }
    pinfo.frec.pos.Clus = parent;

    /* Decrement name cache path depth */
    elink_dir (parent, ELINK_DIR_DEPTH_DEC, vol->ncache);

    do {
      status = frec_next (&pinfo, &path[1], offs - 1, vol);
      if (status != fsOK) {
        return (status);
      }
      pinfo.frec.pos.Offs += pinfo.frec.cnt;

      clus = extract_clus (pinfo.sfne);
    }
    while (clus != child);

    sz = strlen(&path[1]);

    if ((offs - sz) < 2) {
      /* Input buffer to small */
      return (fsInvalidParameter);
    }
    /* Copy name to the end of buffer */
    path[offs--] = '\\';

    while (sz) {
      path[offs--] = path[sz--];
    }

    child = parent;
  }

  if (offs < 4) {
    /* Input buffer to small */
    return (fsInvalidParameter);
  }

  /* Copy drive information */
  set_u32 ((uint8_t *)&path[0], vol->DrvLet);

  i = 2;

  path[i++] = ':';
  path[i++] = '\\';

  /* Copy path information */
  offs++;
  while (offs < len) {
    path[i++] = path[offs++];
  }

  path[i] = '\0';

  EvrFsFAT_PwdSuccess (vol->DrvLet, path, len);
  EvrFsFAT_PathName (path, len);

  return (fsOK);
}


/**
  Change file attributes

  \param[in]  vol                       volume description structure
  \param[in]  path                      path name
  \param[in]  attrib                    FAT attribute mask
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_attrib (fsFAT_Volume *vol, const char *path, uint32_t attrib) {
  PATH_INFO pinfo;
  fsStatus  status;

  /* Setting file attributes */
  EvrFsFAT_AttribSet (vol->DrvLet, path, attrib);
  EvrFsFAT_PathName (path, strlen(path));

  if ((path == NULL) || (attrib == 0U)) {
    /* Invalid parameters */
    EvrFsFAT_InvalidParameter (vol->DrvLet);
    return (fsInvalidParameter);
  }
  if (path_validate (path)) {
    return (fsInvalidPath);
  }

  status = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT | FAT_STATUS_WRITE, vol);
  if (status != fsOK) {
    return (status);
  }

  path_init (path, &pinfo, vol);

  status = path_open (&pinfo, vol);

  if (status == fsOK) {
    /* Reposition frec to short entry */
    if (entry_pos_inc (&pinfo.frec.pos, pinfo.frec.cnt-1, vol) != 0) {
      return (fsError);
    }

    /* Check if file is opened already */
    if (file_locked (&pinfo.frec.pos, FAT_HANDLE_OPEN, vol)) {
      return (fsAccessDenied);
    }

    /* Set attributes */
    pinfo.sfne->Attr |= attrib;

    /* Clear attributes */
    pinfo.sfne->Attr &= ~(attrib >> 16);

    /* Write updated short entry */
    status = entry_flush (&pinfo.frec.pos, vol);
  }

#ifdef FS_DEBUG
  if (status == fsOK) {
    EvrFsFAT_AttribSetSuccess (vol->DrvLet, path, pinfo.sfne->Attr);
  }
#endif
  return (status);
}


/**
  Calculate a free space on a FAT Drive.

  \param[in]  vol                       volume description structure
  \return     Free space in bytes.
*/
__WEAK int64_t fat_free (fsFAT_Volume *vol) {
  fsStatus status;
  int64_t free;

  /* Retrieving free space */
  EvrFsFAT_GetFreeSpace (vol->DrvLet);

  status = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT, vol);

  if (status == fsOK) {
    if (vol->cfg.FatType != FS_FAT32) {
      /* Count free clusters only for FAT12 and FAT16 */
      if (count_free_clus (&vol->free_clus_cnt, vol) == false) {
        status = fsDriverError;
      }
    }
  }

  if (status == fsOK) {
    /* Return free data space in bytes. */
    EvrFsFAT_FreeAmount (vol->DrvLet, vol->free_clus_cnt, vol->cfg.ClusSize);
    free = ((int64_t)(vol->free_clus_cnt) * vol->cfg.ClusSize);
  }
  else {
    free = (int64_t)(-status);
  }

  return (free);
}


/**
  Read FAT volume label and serial number

  \param[out] label                     pointer to label buffer
  \param[out] serial                    pointer to 32-bit variable
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_vol (char *label, uint32_t *serial, fsFAT_Volume *vol) {
  fsStatus status;

  status = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT, vol);

  if ((status == fsOK) && (label != NULL)) {
    status = label_read (vol, label);
  }

  if ((status == fsOK) && (serial != NULL)) {
    status = serial_read (vol, serial);
  }

  return (status);
}


/**
  \brief Set file timestamp.

  \param[in]  path                      string specifying the file or directory path.
  \param[in]  create                    structure specifying the file creation time
  \param[in]  access                    structure specifying the file last access time
  \param[in]  write                     structure specifying the file last write time
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_time_set (const char *path, fsTime *create, fsTime *access, fsTime *write, fsFAT_Volume *vol) {
  PATH_INFO pinfo;
  fsStatus  status;

  EvrFsFAT_TimeSet  (vol->DrvLet, path);
  EvrFsFAT_TimeData (create, access, write);

  if (path == NULL) {
    /* Invalid parameter */
    EvrFsFAT_InvalidParameter (vol->DrvLet);
    return (fsInvalidParameter);
  }
  if (path_validate (path)) {
    return (fsInvalidPath);
  }

  status = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT | FAT_STATUS_WRITE, vol);
  if (status != fsOK) {
    return (status);
  }

  path_init (path, &pinfo, vol);

  status = path_open (&pinfo, vol);

  if (status == fsOK) {
    /* Reposition frec to short entry */
    if (entry_pos_inc (&pinfo.frec.pos, pinfo.frec.cnt-1, vol) != 0) {
      return (fsError);
    }
    /* Check if file is opened already */
    if (file_locked (&pinfo.frec.pos, FAT_HANDLE_OPEN, vol)) {
      return (fsAccessDenied);
    }

    if (create != NULL) {
      /* Set CrtTime and CrtDate */
      set_u16 ((uint8_t *)&pinfo.sfne->CrtTime, format_time(create));
      set_u16 ((uint8_t *)&pinfo.sfne->CrtDate, format_date(create));
    }

    if (access != NULL) {
      /* Set LastAccDate */
      set_u16 ((uint8_t *)&pinfo.sfne->LastAccDate, format_date(access));
    }

    if (write != NULL) {
      /* Set WriteTime and WriteDate */
      set_u16 ((uint8_t *)&pinfo.sfne->WriteTime, format_time(write));
      set_u16 ((uint8_t *)&pinfo.sfne->WriteDate, format_date(write));
    }

    if (entry_flush (&pinfo.frec.pos, vol)) {
      status = fsError;
    }
  }

  return (status);
}

/**
  \brief Get file timestamp.

  \param[in]  path                      string specifying the file or directory path.
  \param[out] create                    structure where file creation time will be stored
  \param[out] access                    structure where file last access time will be stored
  \param[out] write                     structure where file last write time will be stored
  \param[in]  vol                       volume description structure
  \return     execution status \ref fsStatus
*/
__WEAK fsStatus fat_time_get (const char *path, fsTime *create, fsTime *access, fsTime *write, fsFAT_Volume *vol) {
  PATH_INFO pinfo;
  fsStatus  status;

  EvrFsFAT_TimeGet (vol->DrvLet, path);

  if (path == NULL) {
    /* Invalid parameter */
    EvrFsFAT_InvalidParameter (vol->DrvLet);
    return (fsInvalidParameter);
  }
  if (path_validate (path)) {
    return (fsInvalidPath);
  }

  status = fat_vol_chk (FAT_STATUS_READY | FAT_STATUS_MOUNT, vol);
  if (status != fsOK) {
    return (status);
  }

  path_init (path, &pinfo, vol);

  status = path_open (&pinfo, vol);

  if (status == fsOK) {
    if (create != NULL) {
      /* Get create timestamp from CrtTime and CrtDate */
      extract_time (pinfo.sfne->CrtTime, create);
      extract_date (pinfo.sfne->CrtDate, create);
    }

    if (access != NULL) {
      /* Get last access timestamp from LastAccDate */
      extract_date (pinfo.sfne->LastAccDate, access);
      /* Set time member to zero */
      access->hr  = 0U;
      access->min = 0U;
      access->sec = 0U;
    }

    if (write != NULL) {
      /* Get last write timestamp from WriteTime and WriteDate */
      extract_time (pinfo.sfne->WriteTime, write);
      extract_date (pinfo.sfne->WriteDate, write);
    }

    EvrFsFAT_TimeData (create, access, write);
  }

  return (status);
}
