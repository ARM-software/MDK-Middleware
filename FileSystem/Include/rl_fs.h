/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    rl_fs.h
 * Purpose: File System API
 *----------------------------------------------------------------------------*/

#ifndef __RL_FS_H
#define __RL_FS_H

#include <stdint.h>
#include <stdbool.h>

// ==== Enumeration, structures, defines ====

/// FAT File Attribute bit masks.
#define FS_FAT_ATTR_READ_ONLY    0x01   ///< Read-only file attribute
#define FS_FAT_ATTR_HIDDEN       0x02   ///< Hidden file attribute
#define FS_FAT_ATTR_SYSTEM       0x04   ///< System file attribute
#define FS_FAT_ATTR_VOLUME_ID    0x08   ///< Volume ID attribute
#define FS_FAT_ATTR_DIRECTORY    0x10   ///< Directory file attribute
#define FS_FAT_ATTR_ARCHIVE      0x20   ///< Archive file attribute

/// IOC control code fsDevCtrlCodeCheckMedia return bit masks.
#define FS_MEDIA_INSERTED        0x01   ///< Media Detected
#define FS_MEDIA_PROTECTED       0x02   ///< Media Write Protected
#define FS_MEDIA_INITIALIZED     0x04   ///< Media Initialized
#define FS_MEDIA_NOCHKMEDIA      0x80   ///< Check Media Not implemented

/// IOC control code fsDevCtrlCodeControlMedia argument values.
#define FS_CONTROL_MEDIA_INIT    0x00   ///< Initialize media device
#define FS_CONTROL_EMMC_SLEEP    0x01   ///< Switch eMMC device to Sleep State
#define FS_CONTROL_EMMC_AWAKE    0x02   ///< Wake-up eMMC device from Sleep State

/// IOC control code fsDevCtrlCodeLockUnlock argument flags bit masks.
#define FS_DEVICE_SET_PASSWORD   0x01   ///< Set password
#define FS_DEVICE_CLR_PASSWORD   0x02   ///< Clear password
#define FS_DEVICE_LOCK_UNLOCK    0x04   ///< Lock the device
#define FS_DEVICE_ERASE          0x08   ///< Force erase operation

#ifdef __cplusplus
extern "C"  {
#endif

/// File System return codes.
typedef enum {
  fsOK = 0,                             ///< Operation succeeded
  fsError,                              ///< Unspecified error
  fsUnsupported,                        ///< Operation not supported
  fsAccessDenied,                       ///< Resource access denied
  fsInvalidParameter,                   ///< Invalid parameter specified
  fsInvalidDrive,                       ///< Nonexistent drive
  fsInvalidPath,                        ///< Invalid path specified
  fsUninitializedDrive,                 ///< Drive is uninitialized
  fsDriverError,                        ///< Read/write error
  fsMediaError,                         ///< Media error
  fsNoMedia,                            ///< No media, or not initialized
  fsNoFileSystem,                       ///< File system is not formatted
  fsNoFreeSpace,                        ///< No free space available
  fsFileNotFound,                       ///< Requested file not found
  fsDirNotEmpty,                        ///< The directory is not empty
  fsTooManyOpenFiles,                   ///< Too many open files
  fsAlreadyExists,                      ///< File or directory already exists
  fsNotDirectory                        ///< Path is not a directory
} fsStatus;

/// File System Type.
typedef enum _fsType {
  fsTypeNone = 0,                       ///< No file system (volume unformatted)
  fsTypeUnknown,                        ///< File system type is unknown
  fsTypeFAT12,                          ///< File system type is FAT12
  fsTypeFAT16,                          ///< File system type is FAT16
  fsTypeFAT32,                          ///< File system type is FAT32
  fsTypeEFS                             ///< File system type is EFS
} fsType;

/// Driver DeviceCtrl parameter code.
typedef enum _fsDevCtrlCode {
  fsDevCtrlCodeCheckMedia = 0,          ///< Check Media Status
  fsDevCtrlCodeControlMedia,            ///< Control Media Device
  fsDevCtrlCodeFormat,                  ///< Format Media at low level
  fsDevCtrlCodeSerial,                  ///< Return device serial number
  fsDevCtrlCodeGetCID,                  ///< Read Memory Card CID Register
  fsDevCtrlCodeLockUnlock,              ///< Manage device password protection
  fsDevCtrlCodeHealthStatus             ///< Access device health status (S.M.A.R.T)
} fsDevCtrlCode;

/// Media information.
typedef struct _fsMediaInfo {
  uint32_t  block_cnt;                  ///< Total number of blocks on the volume
  uint16_t  read_blen;                  ///< Read block length
  uint16_t  write_blen;                 ///< Write block length
} fsMediaInfo;

/// IO Control Cache information.
typedef struct _fsIOC_Cache {
  uint8_t *buffer;                      ///< Cache Buffer (4 byte aligned)
  uint32_t size;                        ///< Cache Buffer size in bytes
} fsIOC_Cache;

/// Memory card CID register.
typedef struct _fsCID_Register {
  uint32_t   PSN;                       ///< Product serial number
  uint16_t   OID;                       ///< OEM/Application ID
  uint8_t    MID;                       ///< Manufacturer ID
  uint8_t    PRV;                       ///< Product revision
  uint16_t   MDT;                       ///< Manufacturing date
  uint8_t    PNM[6];                    ///< Product name
} fsCID_Register;

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wpadded"
#endif

/// Media lock/unlock information structure.
typedef struct {
  uint8_t const *password;              ///< Password data buffer
  uint8_t        length;                ///< Password length in bytes
  uint8_t        flags;                 ///< Operation definition bitmask
} fsLockUnlock;

/// Health status access structure.
typedef struct {
  uint32_t arg;                         ///< Health status control argument
  uint8_t *buf;                         ///< Data buffer
  uint32_t buf_sz;                      ///< Data buffer size in bytes
} fsHealthStatus;

/// File System Time structure.
typedef struct _fsTime {
  uint8_t  hr;                          ///< Hours    [0..23]
  uint8_t  min;                         ///< Minutes  [0..59]
  uint8_t  sec;                         ///< Seconds  [0..59]
  uint8_t  day;                         ///< Day      [1..31]
  uint8_t  mon;                         ///< Month    [1..12]
  uint16_t year;                        ///< Year     [1980..2107]
} fsTime;

/// File information data.
typedef struct _fsFileInfo {
  char     name[256];                   ///< Filename path
  uint32_t size;                        ///< File size in bytes
  uint16_t fileID;                      ///< System Identification
  uint8_t  attrib;                      ///< Attributes
  fsTime   time;                        ///< Create/Modify Time
} fsFileInfo;

/// Drive information.
typedef struct _fsDriveInfo {
  fsType   fs_type;                     ///< Drives file system type
  uint64_t capacity;                    ///< Drives capacity in bytes
} fsDriveInfo;

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#endif

//  ==== System Routines ====

/// \brief Initialize File System and drive related driver.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsInvalidParameter = Input parameter invalid.
///               - fsInvalidDrive     = Nonexistent drive letter specified.
///               - fsDriverError      = Failed to initialize the driver.
///               - fsError            = System resource create failed.
extern fsStatus finit (const char *drive);

/// \brief Uninitialize File System.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsInvalidParameter = Input parameter invalid.
///               - fsInvalidDrive     = Nonexistent drive letter specified.
///               - fsError            = System resource delete failed.
extern fsStatus funinit (const char *drive);

/// \brief Mount drive.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsUninitializedDrive = Media driver not initialized.
///               - fsInvalidParameter = Input parameter invalid.
///               - fsInvalidDrive     = Nonexistent drive letter specified.
///               - fsDriverError      = Media driver operation failed.
///               - fsMediaError       = Failed to initialize the media.
///               - fsNoMedia          = Media device is not insterted.
///               - fsNoFileSystem     = No filesystem on the volume.
extern fsStatus fmount (const char *drive);

/// \brief Unmount drive.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsInvalidParameter = Input parameter invalid.
///               - fsInvalidDrive     = Nonexistent drive letter specified.
extern fsStatus funmount (const char *drive);

/// \brief Retrieve the File System component version.
/// \return     BCD encoded version: 0xMMmmbbbb (MM:major, mm:minor, bbbb:build)
extern uint32_t fversion (void);

//  ==== File Maintenance Routines ====

/// \brief Delete one or multiple files.
/// \param[in]  path                     a string specifying the file or directory.
/// \param[in]  options                  a string specifying function options.
/// \return     execution status \ref fsStatus
#ifdef MW_FILESYSTEM_FDELETE_LEGACY
extern fsStatus fdelete (const char *path);
#else
extern fsStatus fdelete (const char *path, const char *options);
#endif


/// \brief Find a file or directory matching search pattern.
/// \param[in]  pattern                  string specifying the pattern.
///                                      - May include drive prefix and the following wildcards:
///                                      - "*" or "*.*" searches for all files in the directory.
///                                      - "abc*"       searches for files that begin with abc.
///                                      - "*.htm"      searches for files that end with .htm.
///                                      - "abc*.text"  searches for files that begin with abc and that end with .text.
/// \param[out] info                     structure storing information about matching files.
/// \return     execution status \ref fsStatus
extern fsStatus ffind (const char *pattern, fsFileInfo *info);

/// \brief Rename a file or directory with given path name to a new name.
/// \param[in]  path                     string specifying the file or directory path.
/// \param[in]  newname                  string specifying new file or directory name.
/// \return     execution status \ref fsStatus
extern fsStatus frename (const char *path, const char *newname);

/// \brief Change file attributes.
/// \param[in]  path                     string specifying file or directory path.
/// \param[in]  attr                     string specifying file or directory attributes to be modified.
///                                      The following characters are allowed within attr string:
///                                      - + Sets an attribute.
///                                      - - Clears an attribute.
///                                      - R Read-only file attribute.
///                                      - A Archive file attribute.
///                                      - S System file attribute.
///                                      - H Hidden file attribute.
/// \return     execution status \ref fsStatus
/// \note       This function supports FAT drives only.
extern fsStatus fattrib (const char *path, const char *attr);


//  ==== Directory Maintenance Routines ====

/// \brief Print working directory.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \param[out] buf                      pointer to string buffer.
/// \param[in]  len                      string buffer length.
/// \return     execution status \ref fsStatus
extern fsStatus fpwd (const char *drive, char *buf, uint32_t len);

/// \brief Change working directory.
/// \param[in]  path                     string specifying the directory path.
/// \return     execution status \ref fsStatus
extern fsStatus fchdir (const char *path);

/// \brief Create a directory.
/// \param[in]  path                     string specifying directory path.
/// \return     execution status \ref fsStatus
extern fsStatus fmkdir (const char *path);

/// \brief Remove a directory.
/// \param[in]  path                     string specifying directory path.
/// \param[in]  options                  string specifying function options.
/// \return     execution status \ref fsStatus
extern fsStatus frmdir (const char *path, const char *options);


//  ==== Utility Routines ====

/// \brief Change current drive.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \return     execution status \ref fsStatus
extern fsStatus fchdrive (const char *drive);

/// \brief Find free space on drive.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \return     free space or execution status
///             - value >= 0: free space on drive in bytes.
///             - value < 0:  error occurred, -value is execution status as defined with \ref fsStatus
extern int64_t ffree (const char *drive);

/// \brief Format drive.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \param[in]  options                  string specifying formatting options.
/// \return     execution status \ref fsStatus
extern fsStatus fformat (const char *drive, const char *options);

/// \brief Analyse volume and check for file fragmentation.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \return     fragmentation factor or execution status
///             - value >= 0 or <= 255: fragmentation factor
///             - value < 0: error occurred, -value is execution status as defined with \ref fsStatus
/// \note       This function supports EFS drives only.
extern int32_t fanalyse (const char *drive);

/// \brief Analyse volume and check for allocation errors.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful, no allocation errors.
///               - fsInvalidDrive     = Nonexistent drive letter specified.
///               - fsUnsupported      = FAT drive was specified.
///               - fsError            = Check failed due to allocation errors.
/// \note       This function supports EFS drives only.
extern fsStatus fcheck (const char *drive);

/// \brief Defragment Embedded Flash drive.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsInvalidDrive     = Nonexistent drive letter specified.
///               - fsAccessDenied     = Not all files are closed on specified drive or unsupported drive.
///               - fsNoFreeSpace      = Not enough space to complete the defragmentation.
/// \note       This function supports EFS drives only.
extern fsStatus fdefrag (const char *drive);

/// \brief Check if media present on removable drive.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful, media is present.
///               - fsNoMedia          = Media not present.
///               - fsUnsupported      = Media presence cannot be determined.
///               - fsAccessDenied     = Tried to access non-removable drive.
/// \note       This function supports FAT drives only.
extern fsStatus fmedia (const char *drive);

/// \brief Read drive information.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \param[out] info                     drive information structure.
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsInvalidParameter = Input parameters invalid.
///               - fsInvalidDrive     = Nonexistent drive letter specified.
///               - fsAccessDenied     = Specified drive is not mounted.
extern fsStatus finfo (const char *drive, fsDriveInfo *info);

/// \brief Read volume label and serial number.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \param[out] label                    buffer for storing label as a null terminated string.
///                                      When volume has no label an empty string is returned.
///                                      This parameter can be NULL meaning label will not be read.
/// \param[out] serial                   pointer to variable where serial number will be stored.
///                                      This parameter can be NULL meaning serial number will not be read.
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsInvalidDrive     = Nonexistent drive letter specified.
///               - fsAccessDenied     = Trying to read label and serial from EFS drive.
///               - fsDriverError      = Read/write error.
/// \note       This function supports FAT drives only.
extern fsStatus fvol (const char *drive, char *label, uint32_t *serial);


//  ==== File Time Support Routines ====

/// \brief Set file or directory timestamp.
/// \param[in]  path                     string specifying the file or directory path.
/// \param[in]  create                   \ref fsTime structure specifying the create time.
/// \param[in]  access                   \ref fsTime structure specifying the last access time.
/// \param[in]  write                    \ref fsTime structure specifying the last write time.
/// \return     execution status \ref fsStatus
/// \note       This function supports FAT drives only.
extern fsStatus ftime_set (const char *path, fsTime *create, fsTime *access, fsTime *write);

/// \brief Get file or directory timestamp.
/// \param[in]  path                     string specifying the file or directory path.
/// \param[out] create                   \ref fsTime structure for storing the create time.
/// \param[out] access                   \ref fsTime structure for storing the last access time.
/// \param[out] write                    \ref fsTime structure for storing the last write time.
/// \return     execution status \ref fsStatus
/// \note       This function supports FAT drives only.
extern fsStatus ftime_get (const char *path, fsTime *create, fsTime *access, fsTime *write);

/// \brief Callback function used to provide the current date and time to the File System.
/// \param[out] time                     Pointer to the \ref fsTime structure.
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsError            = Failed to get the current time
/// \note       This function supports FAT drives only.
extern fsStatus fs_get_time (fsTime *time);


//  ==== Media Support Routines ====

/// \brief   Callback function used to read Card Detect (CD) pin state when
///          Memory Card is used in SPI mode.
/// \param[in]  drive_num                Memory Card Drive number
/// \return     1:card detected, 0:card not detected, or error
extern int32_t fs_mc_read_cd (uint32_t drive_num);

/// \brief   Callback function used to read Write Protect (WP) pin state when
///          Memory Card is used in SPI mode.
/// \param[in]  drive_num                Memory Card Drive number
/// \return     1:write protected, 0:not write protected, or error
extern int32_t fs_mc_read_wp (uint32_t drive_num);

/// \brief   Callback function used to control Slave Select signal when
///          Memory Card is used in SPI mode.
/// \param[in]  drive_num                Memory Card Drive number
/// \param[in]  ss                       Slave select signal state
/// \return     execution status
///               - slave select signal state: 1:active, 0:inactive
///               - -1: slave select signal control is not implemented
extern int32_t fs_mc_spi_control_ss (uint32_t drive_num, uint32_t ss);

/// \brief Callback function used to retrieve password management information
///        in order to manage locked memory card device during initialization.
/// \param[in]  drive_num                Memory Card Drive number
/// \return     pointer to fsLockUnlock structure or NULL when structure not available
extern fsLockUnlock *fs_mc_lock_unlock (uint32_t drive_num);


//  ==== I/O Control Interface Routines ====

/// \brief Check if valid drive is specified and return its ID.
/// \param[in]  drive                    a string specifying the \ref drive "memory or storage device".
/// \return     drive ID or execution status
///               - value >= 0: drive ID as an integer when specified drive exists and allows IOC access
///               - value < 0: error occurred, -value is execution status as defined with \ref fsStatus
extern int32_t fs_ioc_get_id (const char *drive);

/// \brief Lock drive and block media access to the upper layer.
/// \param[in]  drv_id                   Drive ID obtained by \ref fs_ioc_get_id.
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsInvalidDrive     = Nonexistent drive ID specified.
///               - fsAccessDenied     = IOC access not allowed for specified drive.
extern fsStatus fs_ioc_lock (int32_t drv_id);

/// \brief Unlock drive and allow media access to the upper layer.
/// \param[in]  drv_id                   Drive ID obtained by \ref fs_ioc_get_id.
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsInvalidDrive     = Nonexistent drive ID specified.
///               - fsAccessDenied     = IOC access not allowed for specified drive.
extern fsStatus fs_ioc_unlock (int32_t drv_id);

/// \brief Return IOC cache buffer information.
/// \param[in]  drv_id                   Drive ID obtained by \ref fs_ioc_get_id.
/// \param[out] cache_info               IOC Cache information.
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsInvalidParameter = Input parameters are not valid.
///               - fsInvalidDrive     = Nonexistent drive ID specified.
///               - fsAccessDenied     = IOC access not allowed for specified drive.
extern fsStatus fs_ioc_get_cache (int32_t drv_id, fsIOC_Cache *cache_info);

/// \brief Read sector from media.
/// \param[in]  drv_id                   Drive ID obtained by \ref fs_ioc_get_id.
/// \param[in]  sect                     Sector number.
/// \param[out] buf                      Data buffer.
/// \param[in]  cnt                      Count of sectors.
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsInvalidDrive     = Nonexistent drive ID specified.
///               - fsNoMedia          = Media not present.
///               - fsAccessDenied     = IOC access not allowed for specified drive.
///               - fsError            = Read sector failed.
extern fsStatus fs_ioc_read_sector (int32_t drv_id, uint32_t sect, uint8_t *buf, uint32_t cnt);

/// \brief Write sector to media.
/// \param[in]  drv_id                   Drive ID obtained by \ref fs_ioc_get_id.
/// \param[in]  sect                     Sector number.
/// \param[out] buf                      Data buffer.
/// \param[in]  cnt                      Count of sectors.
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsInvalidDrive     = Nonexistent drive ID specified.
///               - fsNoMedia          = Media not present.
///               - fsAccessDenied     = IOC access not allowed for specified drive.
///               - fsError            = Read sector failed.
extern fsStatus fs_ioc_write_sector (int32_t drv_id, uint32_t sect, const uint8_t *buf, uint32_t cnt);

/// \brief Read media configuration info.
/// \param[in]  drv_id                   Drive ID obtained by \ref fs_ioc_get_id.
/// \param[out] info                     Media information structure.
/// \return     execution status \ref fsStatus
///               - fsOK               = Operation successful.
///               - fsInvalidDrive     = Nonexistent drive ID specified.
///               - fsNoMedia          = Media not present.
///               - fsAccessDenied     = IOC access not allowed for specified drive.
///               - fsError            = Media information read failed.
extern fsStatus fs_ioc_read_info (int32_t drv_id, fsMediaInfo *info);

/// \brief IOC device control access; control code is sent directly to device driver.
/// \param[in]  drv_id                   Drive ID obtained by \ref fs_ioc_get_id.
/// \param[in]  code                     Device control code.
/// \param[in,out] p                     Generic pointer.
/// \return     execution status \ref fsStatus
extern fsStatus fs_ioc_device_ctrl (int32_t drv_id, fsDevCtrlCode code, void *p);

#ifdef __cplusplus
}
#endif

#endif /* __RL_FS_H */
