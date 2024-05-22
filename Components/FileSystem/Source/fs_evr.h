/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2016-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_evr.h
 * Purpose: Definition of debug events for FileSystem
 *----------------------------------------------------------------------------*/

#ifndef FS_EVR_H__
#define FS_EVR_H__

#include "fs_core.h"

/* Fs component number - available range: [0x80-0x9F] */
#define EvtFsCore_No   (0x80 | 0)       /* FsCore component number */
#define EvtFsFAT_No    (0x80 | 1)       /* FsFAT component number */
#define EvtFsEFS_No    (0x80 | 2)       /* FsEFS component number */
#define EvtFsIOC_No    (0x80 | 3)       /* FsIOC component number */
#define EvtFsNFTL_No   (0x80 | 4)       /* FsNFTL component number */
#define EvtFsNAND_No   (0x80 | 5)       /* FsNAND component number */
#define EvtFsMcMCI_No  (0x80 | 6)       /* FsMcMCI component number */
#define EvtFsMcSPI_No  (0x80 | 7)       /* FsMcSPI component number */

#ifdef FS_DEBUG
/* Fs component IDs */
#define EvtFsCoreId(Level,  Msg_No)     EventID(Level, EvtFsCore_No,  Msg_No)
#define EvtFsFATId(Level,   Msg_No)     EventID(Level, EvtFsFAT_No,   Msg_No)
#define EvtFsEFSId(Level,   Msg_No)     EventID(Level, EvtFsEFS_No,   Msg_No)
#define EvtFsIOCId(Level,   Msg_No)     EventID(Level, EvtFsIOC_No,   Msg_No)
#define EvtFsNFTLId(Level,  Msg_No)     EventID(Level, EvtFsNFTL_No,  Msg_No)
#define EvtFsNANDId(Level,  Msg_No)     EventID(Level, EvtFsNAND_No,  Msg_No)
#define EvtFsMcMCIId(Level, Msg_No)     EventID(Level, EvtFsMcMCI_No, Msg_No)
#define EvtFsMcSPIId(Level, Msg_No)     EventID(Level, EvtFsMcSPI_No, Msg_No)

/* Event id list for "FsCore" */
#define EvtFsCore_sys_open              EvtFsCoreId(EventLevelAPI,     0)
#define EvtFsCore_sys_close             EvtFsCoreId(EventLevelAPI,     1)
#define EvtFsCore_sys_write             EvtFsCoreId(EventLevelAPI,     2)
#define EvtFsCore_sys_read              EvtFsCoreId(EventLevelAPI,     3)
#define EvtFsCore_sys_seek              EvtFsCoreId(EventLevelAPI,     4)
#define EvtFsCore_sys_flen              EvtFsCoreId(EventLevelAPI,     5)
#define EvtFsCore_sys_handle_assign     EvtFsCoreId(EventLevelOp,      6)
#define EvtFsCore_finit                 EvtFsCoreId(EventLevelAPI,     7)
#define EvtFsCore_funinit               EvtFsCoreId(EventLevelAPI,     8)
#define EvtFsCore_fmount                EvtFsCoreId(EventLevelAPI,     9)
#define EvtFsCore_funmount              EvtFsCoreId(EventLevelAPI,    10)
//#define EvtFsCore_                      EvtFsCoreId(EventLevel,       11)
#define EvtFsCore_fdelete               EvtFsCoreId(EventLevelAPI,    12)
#define EvtFsCore_ffind                 EvtFsCoreId(EventLevelAPI,    13)
#define EvtFsCore_frename               EvtFsCoreId(EventLevelAPI,    14)
#define EvtFsCore_fattrib               EvtFsCoreId(EventLevelAPI,    15)
#define EvtFsCore_fpwd                  EvtFsCoreId(EventLevelAPI,    16)
#define EvtFsCore_fchdir                EvtFsCoreId(EventLevelAPI,    17)
#define EvtFsCore_fmkdir                EvtFsCoreId(EventLevelAPI,    18)
#define EvtFsCore_frmdir                EvtFsCoreId(EventLevelAPI,    19)
#define EvtFsCore_fchdrive              EvtFsCoreId(EventLevelAPI,    20)
#define EvtFsCore_ffree                 EvtFsCoreId(EventLevelAPI,    21)
#define EvtFsCore_fformat               EvtFsCoreId(EventLevelAPI,    22)
#define EvtFsCore_fanalyse              EvtFsCoreId(EventLevelAPI,    23)
#define EvtFsCore_fcheck                EvtFsCoreId(EventLevelAPI,    24)
#define EvtFsCore_fdefrag               EvtFsCoreId(EventLevelAPI,    25)
#define EvtFsCore_fmedia                EvtFsCoreId(EventLevelAPI,    26)
#define EvtFsCore_finfo                 EvtFsCoreId(EventLevelAPI,    27)
#define EvtFsCore_fvol                  EvtFsCoreId(EventLevelAPI,    28)
#define EvtFsCore_InvalidDrive          EvtFsCoreId(EventLevelError,  29)
#define EvtFsCore_CurrentDriveSelect    EvtFsCoreId(EventLevelOp,     30)
#define EvtFsCore_ftime_set             EvtFsCoreId(EventLevelAPI,    31)
#define EvtFsCore_ftime_get             EvtFsCoreId(EventLevelAPI,    32)
#define EvtFsCore_fversion              EvtFsCoreId(EventLevelAPI,    33)
#define EvtFsCore_fs_fopen              EvtFsCoreId(EventLevelAPI,    34)
#define EvtFsCore_fs_fclose             EvtFsCoreId(EventLevelAPI,    35)
#define EvtFsCore_fs_fwrite             EvtFsCoreId(EventLevelAPI,    36)
#define EvtFsCore_fs_fread              EvtFsCoreId(EventLevelAPI,    37)
#define EvtFsCore_fs_fflush             EvtFsCoreId(EventLevelAPI,    38)
#define EvtFsCore_fs_fseek              EvtFsCoreId(EventLevelAPI,    39)
#define EvtFsCore_fs_fsize              EvtFsCoreId(EventLevelAPI,    40)

/* Event id list for "FsFAT" */
#define EvtFsFAT_InitDrive              EvtFsFATId(EventLevelOp,       0)
#define EvtFsFAT_InitDriveSuccess       EvtFsFATId(EventLevelOp,       1)
#define EvtFsFAT_InitDriverCfgError     EvtFsFATId(EventLevelError,    2)
#define EvtFsFAT_InitDriverError        EvtFsFATId(EventLevelError,    3)
#define EvtFsFAT_UninitDrive            EvtFsFATId(EventLevelOp,       4)
#define EvtFsFAT_MountDrive             EvtFsFATId(EventLevelOp,       5)
#define EvtFsFAT_InitMediaError         EvtFsFATId(EventLevelError,    6)
#define EvtFsFAT_MediaInsertStatError   EvtFsFATId(EventLevelError,    7)
#define EvtFsFAT_MediaInitStatError     EvtFsFATId(EventLevelError,    8)
#define EvtFsFAT_VolumeReadyStatError   EvtFsFATId(EventLevelError,    9)
#define EvtFsFAT_VolumeWriteStatError   EvtFsFATId(EventLevelError,   10)
#define EvtFsFAT_VolumeNotMounted       EvtFsFATId(EventLevelError,   11)
#define EvtFsFAT_ReadMBR                EvtFsFATId(EventLevelOp,      12)
#define EvtFsFAT_InvalidMBR             EvtFsFATId(EventLevelError,   13)
#define EvtFsFAT_NonexistentMBR         EvtFsFATId(EventLevelOp,      14)
#define EvtFsFAT_ReadBootSector         EvtFsFATId(EventLevelOp,      15)
#define EvtFsFAT_InvalidBootSector      EvtFsFATId(EventLevelError,   16)
#define EvtFsFAT_InvalidFAT             EvtFsFATId(EventLevelError,   17)
#define EvtFsFAT_ReadFSInfo             EvtFsFATId(EventLevelOp,      18)
#define EvtFsFAT_InvalidFSInfo          EvtFsFATId(EventLevelError,   19)
#define EvtFsFAT_InvalidFreeClusFSInfo  EvtFsFATId(EventLevelError,   20)
#define EvtFsFAT_InvalidNextClusFSInfo  EvtFsFATId(EventLevelError,   21)
#define EvtFsFAT_WriteFSInfo            EvtFsFATId(EventLevelOp,      22)
#define EvtFsFAT_CountFreeClus          EvtFsFATId(EventLevelOp,      23)
#define EvtFsFAT_MountDriveSuccess      EvtFsFATId(EventLevelOp,      24)
#define EvtFsFAT_UnmountDrive           EvtFsFATId(EventLevelOp,      25)
#define EvtFsFAT_UnmountDriveSuccess    EvtFsFATId(EventLevelOp,      26)
#define EvtFsFAT_FormatDrive            EvtFsFATId(EventLevelOp,      27)
#define EvtFsFAT_FormatDriveSuccess     EvtFsFATId(EventLevelOp,      28)
#define EvtFsFAT_ResetHandles           EvtFsFATId(EventLevelOp,      29)
#define EvtFsFAT_FormatOptionDetected   EvtFsFATId(EventLevelOp,      30)
#define EvtFsFAT_FormatMediaCapacity    EvtFsFATId(EventLevelOp,      31)
#define EvtFsFAT_FormatLowLevel         EvtFsFATId(EventLevelOp,      32)
#define EvtFsFAT_FormatNoSpace          EvtFsFATId(EventLevelError,   33)
#define EvtFsFAT_FormatNoSpaceFAT32     EvtFsFATId(EventLevelError,   34)
#define EvtFsFAT_WriteMBR               EvtFsFATId(EventLevelOp,      35)
#define EvtFsFAT_WriteBootSector        EvtFsFATId(EventLevelOp,      36)
#define EvtFsFAT_ClearHiddenSectors     EvtFsFATId(EventLevelOp,      37)
#define EvtFsFAT_ClearReservedSectors   EvtFsFATId(EventLevelOp,      38)
#define EvtFsFAT_ClearRootSectors       EvtFsFATId(EventLevelOp,      39)
#define EvtFsFAT_CreateFAT              EvtFsFATId(EventLevelOp,      40)
#define EvtFsFAT_FormatProperties       EvtFsFATId(EventLevelOp,      41)
#define EvtFsFAT_FileOpen               EvtFsFATId(EventLevelOp,      42)
#define EvtFsFAT_PathInvalidChar        EvtFsFATId(EventLevelError,   43)
#define EvtFsFAT_PathIsTooLong          EvtFsFATId(EventLevelError,   44)
#define EvtFsFAT_PathIsDirNotFile       EvtFsFATId(EventLevelError,   45)
#define EvtFsFAT_FileIsInUse            EvtFsFATId(EventLevelError,   46)
#define EvtFsFAT_FileIsReadOnly         EvtFsFATId(EventLevelError,   47)
#define EvtFsFAT_FileIsNonExistent      EvtFsFATId(EventLevelError,   48)
#define EvtFsFAT_PathProcessing         EvtFsFATId(EventLevelOp,      49)
#define EvtFsFAT_FileHandleUnavailable  EvtFsFATId(EventLevelError,   50)
#define EvtFsFAT_FileHandleInvalid      EvtFsFATId(EventLevelError,   51)
#define EvtFsFAT_LabelInvalid           EvtFsFATId(EventLevelError,   52)
#define EvtFsFAT_LabelNotSet            EvtFsFATId(EventLevelError,   53)
#define EvtFsFAT_SectorReadFailed       EvtFsFATId(EventLevelError,   54)
#define EvtFsFAT_SectorWriteFailed      EvtFsFATId(EventLevelError,   55)
#define EvtFsFAT_DiskFull               EvtFsFATId(EventLevelError,   56)
#define EvtFsFAT_DirEntryAllocFailed    EvtFsFATId(EventLevelError,   57)
#define EvtFsFAT_PathBufferToSmall      EvtFsFATId(EventLevelError,   58)
#define EvtFsFAT_InitJournal            EvtFsFATId(EventLevelOp,      59)
#define EvtFsFAT_InitJournalSuccess     EvtFsFATId(EventLevelOp,      60)
#define EvtFsFAT_InitJournalFATError    EvtFsFATId(EventLevelOp,      61)
#define EvtFsFAT_FileClose              EvtFsFATId(EventLevelOp,      62)
#define EvtFsFAT_FileHandleError        EvtFsFATId(EventLevelError,   63)
#define EvtFsFAT_FileRead               EvtFsFATId(EventLevelOp,      64)
#define EvtFsFAT_FileWrite              EvtFsFATId(EventLevelOp,      65)
#define EvtFsFAT_FileFlush              EvtFsFATId(EventLevelOp,      66)
#define EvtFsFAT_FileSeek               EvtFsFATId(EventLevelOp,      67)
#define EvtFsFAT_FileSeekIncrease       EvtFsFATId(EventLevelOp,      68)
#define EvtFsFAT_FileDelete             EvtFsFATId(EventLevelOp,      69)
#define EvtFsFAT_FileDeleteSuccess      EvtFsFATId(EventLevelOp,      70)
#define EvtFsFAT_FileRename             EvtFsFATId(EventLevelOp,      71)
#define EvtFsFAT_FileRenameSuccess      EvtFsFATId(EventLevelOp,      72)
#define EvtFsFAT_DirCreate              EvtFsFATId(EventLevelOp,      73)
#define EvtFsFAT_DirCreateSuccess       EvtFsFATId(EventLevelOp,      74)
#define EvtFsFAT_DirRemove              EvtFsFATId(EventLevelOp,      75)
#define EvtFsFAT_DirRemoveSuccess       EvtFsFATId(EventLevelOp,      76)
#define EvtFsFAT_ChDir                  EvtFsFATId(EventLevelOp,      77)
#define EvtFsFAT_ChDirSuccess           EvtFsFATId(EventLevelOp,      78)
#define EvtFsFAT_Pwd                    EvtFsFATId(EventLevelOp,      79)
#define EvtFsFAT_PwdSuccess             EvtFsFATId(EventLevelOp,      80)
#define EvtFsFAT_AttribSet              EvtFsFATId(EventLevelOp,      81)
#define EvtFsFAT_AttribSetSuccess       EvtFsFATId(EventLevelOp,      82)
#define EvtFsFAT_GetFreeSpace           EvtFsFATId(EventLevelOp,      83)
#define EvtFsFAT_FreeAmount             EvtFsFATId(EventLevelOp,      84)
#define EvtFsFAT_FileModeRead           EvtFsFATId(EventLevelError,   85)
#define EvtFsFAT_FileFind               EvtFsFATId(EventLevelOp,      86)
#define EvtFsFAT_InvalidParameter       EvtFsFATId(EventLevelError,   87)
#define EvtFsFAT_DriverNotInitialized   EvtFsFATId(EventLevelError,   88)
#define EvtFsFAT_PathName               EvtFsFATId(EventLevelDetail,  89)
#define EvtFsFAT_OptionsString          EvtFsFATId(EventLevelDetail,  90)
#define EvtFsFAT_LabelString            EvtFsFATId(EventLevelDetail,  91)
#define EvtFsFAT_NameCacheHit           EvtFsFATId(EventLevelOp,      92)
#define EvtFsFAT_NameCacheMiss          EvtFsFATId(EventLevelOp,      93)
#define EvtFsFAT_NameCacheEntryFound    EvtFsFATId(EventLevelDetail,  94)
#define EvtFsFAT_NameCacheEntryInsert   EvtFsFATId(EventLevelDetail,  95)
#define EvtFsFAT_NameCacheEntryDelete   EvtFsFATId(EventLevelDetail,  96)
#define EvtFsFAT_TimeSet                EvtFsFATId(EventLevelOp,      97)
#define EvtFsFAT_TimeGet                EvtFsFATId(EventLevelOp,      98)
#define EvtFsFAT_TimeData               EvtFsFATId(EventLevelDetail,  99)

/* Event id list for "FsEFS" */
#define EvtFsEFS_InitDrive              EvtFsEFSId(EventLevelOp,       0)
#define EvtFsEFS_InitDriver             EvtFsEFSId(EventLevelOp,       1)
#define EvtFsEFS_InitDriverFailed       EvtFsEFSId(EventLevelError,    2)
#define EvtFsEFS_UninitDrive            EvtFsEFSId(EventLevelOp,       3)
#define EvtFsEFS_UninitDriver           EvtFsEFSId(EventLevelOp,       4)
#define EvtFsEFS_MountDrive             EvtFsEFSId(EventLevelOp,       5)
#define EvtFsEFS_MountDriveSuccess      EvtFsEFSId(EventLevelOp,       6)
#define EvtFsEFS_FlashGetInfo           EvtFsEFSId(EventLevelOp,       7)
#define EvtFsEFS_FlashProgUnitTooBig    EvtFsEFSId(EventLevelError,    8)
#define EvtFsEFS_FlashCapacity          EvtFsEFSId(EventLevelOp,       9)
#define EvtFsEFS_UnmountDrive           EvtFsEFSId(EventLevelOp,      10)
#define EvtFsEFS_UnmountDriveSuccess    EvtFsEFSId(EventLevelOp,      11)
#define EvtFsEFS_FormatDrive            EvtFsEFSId(EventLevelOp,      12)
#define EvtFsEFS_FormatDriveSuccess     EvtFsEFSId(EventLevelOp,      13)
#define EvtFsEFS_FlashEraseChip         EvtFsEFSId(EventLevelOp,      14)
#define EvtFsEFS_FlashEraseChipFailed   EvtFsEFSId(EventLevelError,   15)
#define EvtFsEFS_FlashEraseChipTimeout  EvtFsEFSId(EventLevelError,   16)
#define EvtFsEFS_FlashEraseSectors      EvtFsEFSId(EventLevelOp,      17)
#define EvtFsEFS_FileOpen               EvtFsEFSId(EventLevelOp,      18)
#define EvtFsEFS_FileNameInvalid        EvtFsEFSId(EventLevelError,   19)
#define EvtFsEFS_OpenModeUnsupported    EvtFsEFSId(EventLevelError,   20)
#define EvtFsEFS_FileClose              EvtFsEFSId(EventLevelOp,      21)
#define EvtFsEFS_FileNotOpened          EvtFsEFSId(EventLevelError,   22)
#define EvtFsEFS_FileRead               EvtFsEFSId(EventLevelOp,      23)
#define EvtFsEFS_FileOpenWriteMode      EvtFsEFSId(EventLevelError,   24)
#define EvtFsEFS_FileWrite              EvtFsEFSId(EventLevelOp,      25)
#define EvtFsEFS_FileOpenReadMode       EvtFsEFSId(EventLevelError,   26)
#define EvtFsEFS_FileFlush              EvtFsEFSId(EventLevelOp,      27)
#define EvtFsEFS_OpenModeInvalid        EvtFsEFSId(EventLevelError,   28)
#define EvtFsEFS_FileSeek               EvtFsEFSId(EventLevelOp,      29)
#define EvtFsEFS_FileSeekEOF            EvtFsEFSId(EventLevelError,   30)
#define EvtFsEFS_FileDelete             EvtFsEFSId(EventLevelOp,      31)
#define EvtFsEFS_FileRename             EvtFsEFSId(EventLevelOp,      32)
#define EvtFsEFS_FileNotFound           EvtFsEFSId(EventLevelError,   33)
#define EvtFsEFS_FileAlreadyExists      EvtFsEFSId(EventLevelError,   34)
#define EvtFsEFS_FreeSpaceRetrieve      EvtFsEFSId(EventLevelOp,      35)
#define EvtFsEFS_FreeSpaceAmount        EvtFsEFSId(EventLevelOp,      36)
#define EvtFsEFS_DriveAnalyze           EvtFsEFSId(EventLevelOp,      37)
#define EvtFsEFS_DriveAnalyzeSuccess    EvtFsEFSId(EventLevelOp,      38)
#define EvtFsEFS_DriveCheck             EvtFsEFSId(EventLevelOp,      39)
#define EvtFsEFS_DriveCheckSuccess      EvtFsEFSId(EventLevelOp,      40)
#define EvtFsEFS_DataAreaOverlap        EvtFsEFSId(EventLevelError,   41)
#define EvtFsEFS_FileIdInvalid          EvtFsEFSId(EventLevelError,   42)
#define EvtFsEFS_AllocationOrderInvalid EvtFsEFSId(EventLevelError,   43)
#define EvtFsEFS_AllocationAreaOverlap  EvtFsEFSId(EventLevelError,   44)
#define EvtFsEFS_DriveDefrag            EvtFsEFSId(EventLevelOp,      45)
#define EvtFsEFS_FileHandleActive       EvtFsEFSId(EventLevelError,   46)
#define EvtFsEFS_FileHandleUnavailable  EvtFsEFSId(EventLevelError,   47)
#define EvtFsEFS_FileHandleInvalid      EvtFsEFSId(EventLevelError,   48)
#define EvtFsEFS_FileInUse              EvtFsEFSId(EventLevelError,   49)
#define EvtFsEFS_FlashWriteFailed       EvtFsEFSId(EventLevelError,   50)
#define EvtFsEFS_FlashReadFailed        EvtFsEFSId(EventLevelError,   51)
#define EvtFsEFS_FlashReadTimeout       EvtFsEFSId(EventLevelError,   52)
#define EvtFsEFS_FlashEraseFailed       EvtFsEFSId(EventLevelError,   53)
#define EvtFsEFS_DiskFull               EvtFsEFSId(EventLevelError,   54)
#define EvtFsEFS_InvalidParameter       EvtFsEFSId(EventLevelError,   55)
#define EvtFsEFS_DriveNotMounted        EvtFsEFSId(EventLevelError,   56)
#define EvtFsEFS_DriveNotInitialized    EvtFsEFSId(EventLevelError,   57)
#define EvtFsEFS_FileName               EvtFsEFSId(EventLevelDetail,  58)
#define EvtFsEFS_FileAllocWrite         EvtFsEFSId(EventLevelDetail,  59)
#define EvtFsEFS_FileAllocRead          EvtFsEFSId(EventLevelDetail,  60)

/* Event id list for "FsIOC" */
#define EvtFsIOC_GetId                  EvtFsIOCId(EventLevelAPI,      0)
#define EvtFsIOC_GetIdSuccess           EvtFsIOCId(EventLevelOp,       1)
#define EvtFsIOC_GetIdError             EvtFsIOCId(EventLevelError,    2)
#define EvtFsIOC_Lock                   EvtFsIOCId(EventLevelAPI,      3)
#define EvtFsIOC_LockSuccess            EvtFsIOCId(EventLevelOp,       4)
#define EvtFsIOC_Unlock                 EvtFsIOCId(EventLevelAPI,      5)
#define EvtFsIOC_UnlockSuccess          EvtFsIOCId(EventLevelOp,       6)
#define EvtFsIOC_GetCache               EvtFsIOCId(EventLevelAPI,      7)
#define EvtFsIOC_GetCacheSuccess        EvtFsIOCId(EventLevelOp,       8)
#define EvtFsIOC_ReadSector             EvtFsIOCId(EventLevelAPI,      9)
#define EvtFsIOC_ReadSectorSuccess      EvtFsIOCId(EventLevelOp,      10)
#define EvtFsIOC_ReadSectorError        EvtFsIOCId(EventLevelError,   11)
#define EvtFsIOC_WriteSector            EvtFsIOCId(EventLevelAPI,     12)
#define EvtFsIOC_WriteSectorSuccess     EvtFsIOCId(EventLevelOp,      13)
#define EvtFsIOC_WriteSectorError       EvtFsIOCId(EventLevelError,   14)
#define EvtFsIOC_ReadInfo               EvtFsIOCId(EventLevelAPI,     15)
#define EvtFsIOC_ReadInfoSuccess        EvtFsIOCId(EventLevelOp,      16)
#define EvtFsIOC_ReadInfoError          EvtFsIOCId(EventLevelError,   17)
#define EvtFsIOC_DeviceControl          EvtFsIOCId(EventLevelAPI,     18)
#define EvtFsIOC_DeviceControlSuccess   EvtFsIOCId(EventLevelOp,      19)
#define EvtFsIOC_DriveIdInvalid         EvtFsIOCId(EventLevelError,   20)
#define EvtFsIOC_DriveNotFAT            EvtFsIOCId(EventLevelError,   21)
#define EvtFsIOC_NotAllowed             EvtFsIOCId(EventLevelError,   22)

/* Event id list for "FsNFTL" */
#define EvtFsNFTL_Init                  EvtFsNFTLId(EventLevelOp,      0)
#define EvtFsNFTL_InitMediaFailed       EvtFsNFTLId(EventLevelError,   1)
#define EvtFsNFTL_Mount                 EvtFsNFTLId(EventLevelOp,      2)
#define EvtFsNFTL_MountSuccess          EvtFsNFTLId(EventLevelOp,      3)
#define EvtFsNFTL_DeviceNotInitialized  EvtFsNFTLId(EventLevelError,   4)
#define EvtFsNFTL_ResetDevice           EvtFsNFTLId(EventLevelOp,      5)
#define EvtFsNFTL_ResetDeviceFailed     EvtFsNFTLId(EventLevelError,   6)
#define EvtFsNFTL_ResetDeviceTimeout    EvtFsNFTLId(EventLevelError,   7)
#define EvtFsNFTL_ReadBootBlock         EvtFsNFTLId(EventLevelOp,      8)
#define EvtFsNFTL_ReadBootBlockFailed   EvtFsNFTLId(EventLevelError,   9)
#define EvtFsNFTL_BootSignatureValid    EvtFsNFTLId(EventLevelOp,     10)
#define EvtFsNFTL_BootSignatureInvalid  EvtFsNFTLId(EventLevelOp,     11)
#define EvtFsNFTL_DataBlockCapacity     EvtFsNFTLId(EventLevelOp,     12)
#define EvtFsNFTL_ReadTable             EvtFsNFTLId(EventLevelOp,     13)
#define EvtFsNFTL_ReadTableFailed       EvtFsNFTLId(EventLevelError,  14)
#define EvtFsNFTL_Uninit                EvtFsNFTLId(EventLevelOp,     15)
#define EvtFsNFTL_UninitDriver          EvtFsNFTLId(EventLevelOp,     16)
#define EvtFsNFTL_UninitDriverFailed    EvtFsNFTLId(EventLevelError,  17)
#define EvtFsNFTL_ReadInfo              EvtFsNFTLId(EventLevelOp,     18)
#define EvtFsNFTL_ReadInfoSuccess       EvtFsNFTLId(EventLevelOp,     19)
#define EvtFsNFTL_DevCtrl               EvtFsNFTLId(EventLevelOp,     20)
#define EvtFsNFTL_DevCtrlUnsupported    EvtFsNFTLId(EventLevelError,  21)
#define EvtFsNFTL_Format                EvtFsNFTLId(EventLevelOp,     22)
#define EvtFsNFTL_FormatSuccess         EvtFsNFTLId(EventLevelOp,     23)
#define EvtFsNFTL_FormatLLEB            EvtFsNFTLId(EventLevelOp,     24)
#define EvtFsNFTL_BBMPositionSet        EvtFsNFTLId(EventLevelOp,     25)
#define EvtFsNFTL_BadBlockDetected      EvtFsNFTLId(EventLevelOp,     26)
#define EvtFsNFTL_BadBlockMark          EvtFsNFTLId(EventLevelOp,     27)
#define EvtFsNFTL_WriteSector           EvtFsNFTLId(EventLevelOp,     28)
#define EvtFsNFTL_WriteSectorSuccess    EvtFsNFTLId(EventLevelOp,     29)
#define EvtFsNFTL_ReadSector            EvtFsNFTLId(EventLevelOp,     30)
#define EvtFsNFTL_ReadSectorSuccess     EvtFsNFTLId(EventLevelOp,     31)
#define EvtFsNFTL_LsnToLbn              EvtFsNFTLId(EventLevelOp,     32)
#define EvtFsNFTL_LbnToPbn              EvtFsNFTLId(EventLevelOp,     33)
#define EvtFsNFTL_LbnOutOfRange         EvtFsNFTLId(EventLevelError,  34)
#define EvtFsNFTL_PbnOutOfRange         EvtFsNFTLId(EventLevelError,  35)
#define EvtFsNFTL_TableLookup           EvtFsNFTLId(EventLevelOp,     36)
#define EvtFsNFTL_TableEntryNotFound    EvtFsNFTLId(EventLevelOp,     37)
#define EvtFsNFTL_AllocateBlock         EvtFsNFTLId(EventLevelOp,     38)
#define EvtFsNFTL_LsnFind               EvtFsNFTLId(EventLevelOp,     39)
#define EvtFsNFTL_LsnFound              EvtFsNFTLId(EventLevelOp,     40)
#define EvtFsNFTL_LsnNotFound           EvtFsNFTLId(EventLevelOp,     41)
#define EvtFsNFTL_GarbageCollection     EvtFsNFTLId(EventLevelOp,     42)
#define EvtFsNFTL_SetBadBlockMarker     EvtFsNFTLId(EventLevelOp,     43)
#define EvtFsNFTL_MoveData              EvtFsNFTLId(EventLevelOp,     44)
#define EvtFsNFTL_RelocateBlock         EvtFsNFTLId(EventLevelOp,     45)
#define EvtFsNFTL_RelocateBlockFailed   EvtFsNFTLId(EventLevelError,  46)
#define EvtFsNFTL_SkipBadBlock          EvtFsNFTLId(EventLevelOp,     47)
#define EvtFsNFTL_PageReadFailed        EvtFsNFTLId(EventLevelError,  48)
#define EvtFsNFTL_PageProgramStatusErr  EvtFsNFTLId(EventLevelOp,     49)
#define EvtFsNFTL_PageProgramFailed     EvtFsNFTLId(EventLevelError,  50)
#define EvtFsNFTL_PageProgramTimeout    EvtFsNFTLId(EventLevelError,  51)
#define EvtFsNFTL_BlockEraseStatusErr   EvtFsNFTLId(EventLevelOp,     52)
#define EvtFsNFTL_BlockEraseFailed      EvtFsNFTLId(EventLevelError,  53)
#define EvtFsNFTL_BlockEraseTimeout     EvtFsNFTLId(EventLevelError,  54)
#define EvtFsNFTL_BitErrorCorrected     EvtFsNFTLId(EventLevelOp,     55)
#define EvtFsNFTL_ParameterInvalid      EvtFsNFTLId(EventLevelError,  56)
#define EvtFsNFTL_TableUpdate           EvtFsNFTLId(EventLevelOp,     57)
#define EvtFsNFTL_AllocatedBlock        EvtFsNFTLId(EventLevelOp,     58)
#define EvtFsNFTL_LoadSector            EvtFsNFTLId(EventLevelDetail, 59)
#define EvtFsNFTL_LoadTableLayout       EvtFsNFTLId(EventLevelDetail, 60)
#define EvtFsNFTL_LoadDataLayout        EvtFsNFTLId(EventLevelDetail, 61)
#define EvtFsNFTL_SetupPageLayout       EvtFsNFTLId(EventLevelDetail, 62)
#define EvtFsNFTL_SetupSpareLayout      EvtFsNFTLId(EventLevelDetail, 63)
#define EvtFsNFTL_PageWrite             EvtFsNFTLId(EventLevelDetail, 64)
#define EvtFsNFTL_PageRead              EvtFsNFTLId(EventLevelDetail, 65)
#define EvtFsNFTL_BlockErase            EvtFsNFTLId(EventLevelDetail, 66)
#define EvtFsNFTL_StatusRead            EvtFsNFTLId(EventLevelDetail, 67)
#define EvtFsNFTL_CacheWrite            EvtFsNFTLId(EventLevelDetail, 68)
#define EvtFsNFTL_CacheRead             EvtFsNFTLId(EventLevelDetail, 69)

#if defined(FS_NAND_FLASH_0) || defined(FS_NAND_FLASH_1)
/* Event id list for "FsNAND" */
#define EvtFsNAND_Init                  EvtFsNANDId(EventLevelOp,      0)
#define EvtFsNAND_InitDriver            EvtFsNANDId(EventLevelOp,      1)
#define EvtFsNAND_InitDriverError       EvtFsNANDId(EventLevelError,   2)
#define EvtFsNAND_DriverPowerCtrlError  EvtFsNANDId(EventLevelError,   3)
#define EvtFsNAND_DriverBusModeSelect   EvtFsNANDId(EventLevelOp,      4)
#define EvtFsNAND_DriverBusWidthSelect  EvtFsNANDId(EventLevelOp,      5)
#define EvtFsNAND_DriverBusWidthError   EvtFsNANDId(EventLevelError,   6)
#define EvtFsNAND_DriverDevPowerSelect  EvtFsNANDId(EventLevelOp,      7)
#define EvtFsNAND_DriverEccInquire      EvtFsNANDId(EventLevelOp,      8)
#define EvtFsNAND_DriverEccSelect       EvtFsNANDId(EventLevelOp,      9)
#define EvtFsNAND_DriverEccInquireError EvtFsNANDId(EventLevelOp,     10)
#define EvtFsNAND_DriverTimeoutError    EvtFsNANDId(EventLevelError,  11)
#define EvtFsNAND_DeviceTimeoutError    EvtFsNANDId(EventLevelError,  12)
#define EvtFsNAND_Uninit                EvtFsNANDId(EventLevelOp,     13)
#define EvtFsNAND_StatusRead            EvtFsNANDId(EventLevelDetail, 14)
#define EvtFsNAND_BlockErase            EvtFsNANDId(EventLevelOp,     15)
#define EvtFsNAND_PageWrite             EvtFsNANDId(EventLevelOp,     16)
#define EvtFsNAND_PageRead              EvtFsNANDId(EventLevelOp,     17)
#define EvtFsNAND_ParamPageRead         EvtFsNANDId(EventLevelOp,     18)
#define EvtFsNAND_IdRead                EvtFsNANDId(EventLevelOp,     19)
#define EvtFsNAND_DeviceReset           EvtFsNANDId(EventLevelOp,     20)
#endif /* defined(FS_NAND_FLASH_0) || defined(FS_NAND_FLASH_1) */

#if defined(FS_MEMORY_CARD_0) || defined(FS_MEMORY_CARD_1)
/* Event id list for "FsMcMCI" */
#define EvtFsMcMCI_InitDriver           EvtFsMcMCIId(EventLevelOp,     0)
#define EvtFsMcMCI_InitDriverError      EvtFsMcMCIId(EventLevelError,  1)
#define EvtFsMcMCI_InitDriverPowerError EvtFsMcMCIId(EventLevelError,  2)
#define EvtFsMcMCI_InitMedia            EvtFsMcMCIId(EventLevelOp,     3)
#define EvtFsMcMCI_MediaNotInitialized  EvtFsMcMCIId(EventLevelOp,     4)
#define EvtFsMcMCI_MediaCdActive        EvtFsMcMCIId(EventLevelOp,     5)
#define EvtFsMcMCI_MediaCdInactive      EvtFsMcMCIId(EventLevelError,  6)
#define EvtFsMcMCI_MediaWpActive        EvtFsMcMCIId(EventLevelOp,     7)
#define EvtFsMcMCI_MediaWpInactive      EvtFsMcMCIId(EventLevelOp,     8)
#define EvtFsMcMCI_MediaReset           EvtFsMcMCIId(EventLevelOp,     9)
#define EvtFsMcMCI_MediaDetectionSD     EvtFsMcMCIId(EventLevelOp,    10)
#define EvtFsMcMCI_MediaSD_V2           EvtFsMcMCIId(EventLevelOp,    11)
#define EvtFsMcMCI_MediaReady           EvtFsMcMCIId(EventLevelOp,    12)
#define EvtFsMcMCI_MediaNoResponse      EvtFsMcMCIId(EventLevelOp,    13)
#define EvtFsMcMCI_MediaDetectionMMC    EvtFsMcMCIId(EventLevelOp,    14)
#define EvtFsMcMCI_MediaDetectionError  EvtFsMcMCIId(EventLevelError, 15)
#define EvtFsMcMCI_CidReadError         EvtFsMcMCIId(EventLevelError, 16)
#define EvtFsMcMCI_RcaReadError         EvtFsMcMCIId(EventLevelError, 17)
#define EvtFsMcMCI_RcaWriteError        EvtFsMcMCIId(EventLevelError, 18)
#define EvtFsMcMCI_CsdReadError         EvtFsMcMCIId(EventLevelError, 19)
#define EvtFsMcMCI_TranStateError       EvtFsMcMCIId(EventLevelError, 20)
#define EvtFsMcMCI_BlenWriteError       EvtFsMcMCIId(EventLevelError, 21)
#define EvtFsMcMCI_ExtCsdReadError      EvtFsMcMCIId(EventLevelError, 22)
#define EvtFsMcMCI_HighSpeedSwitchError EvtFsMcMCIId(EventLevelError, 23)
#define EvtFsMcMCI_DataBusSwitchError   EvtFsMcMCIId(EventLevelError, 24)
#define EvtFsMcMCI_StbyStateError       EvtFsMcMCIId(EventLevelError, 25)
#define EvtFsMcMCI_InitSuccess          EvtFsMcMCIId(EventLevelOp,    26)
#define EvtFsMcMCI_UninitDriver         EvtFsMcMCIId(EventLevelOp,    27)
#define EvtFsMcMCI_UninitMedia          EvtFsMcMCIId(EventLevelOp,    28)
#define EvtFsMcMCI_SectorRead           EvtFsMcMCIId(EventLevelOp,    29)
#define EvtFsMcMCI_SectorReadError      EvtFsMcMCIId(EventLevelError, 30)
#define EvtFsMcMCI_SectorWrite          EvtFsMcMCIId(EventLevelOp,    31)
#define EvtFsMcMCI_SectorWriteError     EvtFsMcMCIId(EventLevelError, 32)
#define EvtFsMcMCI_TransferError        EvtFsMcMCIId(EventLevelError, 33)
#define EvtFsMcMCI_TransferRetry        EvtFsMcMCIId(EventLevelOp,    34)
#define EvtFsMcMCI_ReadInfo             EvtFsMcMCIId(EventLevelOp,    35)
#define EvtFsMcMCI_ReadInfoSuccess      EvtFsMcMCIId(EventLevelOp,    36)
#define EvtFsMcMCI_DevCtrl              EvtFsMcMCIId(EventLevelOp,    37)
#define EvtFsMcMCI_DevCtrlUnsupported   EvtFsMcMCIId(EventLevelOp,    38)
#define EvtFsMcMCI_LockUnlockExec       EvtFsMcMCIId(EventLevelOp,    39)
#define EvtFsMcMCI_LockUnlockExecError  EvtFsMcMCIId(EventLevelError, 40)
#define EvtFsMcMCI_LockActive           EvtFsMcMCIId(EventLevelOp,    41)
#define EvtFsMcMCI_LockDeactivated      EvtFsMcMCIId(EventLevelOp,    42)
#define EvtFsMcMCI_DeviceStateUnknown   EvtFsMcMCIId(EventLevelError, 43)
#define EvtFsMcMCI_DeviceStateInvalid   EvtFsMcMCIId(EventLevelError, 44)
#define EvtFsMcMCI_DeviceStateTimeout   EvtFsMcMCIId(EventLevelError, 45)
#define EvtFsMcMCI_TransferAbort        EvtFsMcMCIId(EventLevelOp,    46)
#define EvtFsMcMCI_SleepAwakeControl    EvtFsMcMCIId(EventLevelOp,    47)
#define EvtFsMcMCI_SleepActive          EvtFsMcMCIId(EventLevelOp,    48)
#define EvtFsMcMCI_AwakeActive          EvtFsMcMCIId(EventLevelOp,    49)
#define EvtFsMcMCI_CardPowerControl     EvtFsMcMCIId(EventLevelOp,    50)
#define EvtFsMcMCI_SendCommandError     EvtFsMcMCIId(EventLevelError, 51)
#define EvtFsMcMCI_ReadXferSetupError   EvtFsMcMCIId(EventLevelError, 52)
#define EvtFsMcMCI_WriteXferSetupError  EvtFsMcMCIId(EventLevelError, 53)
#define EvtFsMcMCI_ParameterInvalid     EvtFsMcMCIId(EventLevelError, 54)
#define EvtFsMcMCI_MediaPasswordEnabled EvtFsMcMCIId(EventLevelError, 55)

/* Event id list for "FsMcSPI" */
#define EvtFsMcSPI_InitDriver           EvtFsMcSPIId(EventLevelOp,     0)
#define EvtFsMcSPI_InitDriverSuccess    EvtFsMcSPIId(EventLevelOp,     1)
#define EvtFsMcSPI_InitDriverError      EvtFsMcSPIId(EventLevelError,  2)
#define EvtFsMcSPI_InitDriverPowerError EvtFsMcSPIId(EventLevelError,  3)
#define EvtFsMcSPI_InitMedia            EvtFsMcSPIId(EventLevelOp,     4)
#define EvtFsMcSPI_InitMediaSuccess     EvtFsMcSPIId(EventLevelOp,     5)
#define EvtFsMcSPI_MediaNotInitialized  EvtFsMcSPIId(EventLevelError,  6)
#define EvtFsMcSPI_MediaCdActive        EvtFsMcSPIId(EventLevelOp,     7)
#define EvtFsMcSPI_MediaCdInactive      EvtFsMcSPIId(EventLevelError,  8)
#define EvtFsMcSPI_MediaWpActive        EvtFsMcSPIId(EventLevelOp,     9)
#define EvtFsMcSPI_MediaWpInactive      EvtFsMcSPIId(EventLevelOp,    10)
#define EvtFsMcSPI_MediaReset           EvtFsMcSPIId(EventLevelOp,    11)
#define EvtFsMcSPI_MediaResetError      EvtFsMcSPIId(EventLevelError, 12)
#define EvtFsMcSPI_MediaDetectionSD     EvtFsMcSPIId(EventLevelOp,    13)
#define EvtFsMcSPI_MediaSD_V2           EvtFsMcSPIId(EventLevelOp,    14)
#define EvtFsMcSPI_MediaReady           EvtFsMcSPIId(EventLevelOp,    15)
#define EvtFsMcSPI_MediaNoResponse      EvtFsMcSPIId(EventLevelOp,    16)
#define EvtFsMcSPI_MediaDetectionMMC    EvtFsMcSPIId(EventLevelOp,    17)
#define EvtFsMcSPI_MediaDetectionError  EvtFsMcSPIId(EventLevelError, 18)
#define EvtFsMcSPI_CidReadError         EvtFsMcSPIId(EventLevelError, 19)
#define EvtFsMcSPI_CsdReadError         EvtFsMcSPIId(EventLevelError, 20)
#define EvtFsMcSPI_BlenWriteError       EvtFsMcSPIId(EventLevelError, 21)
#define EvtFsMcSPI_UninitDriver         EvtFsMcSPIId(EventLevelOp,    22)
#define EvtFsMcSPI_UninitMedia          EvtFsMcSPIId(EventLevelOp,    23)
#define EvtFsMcSPI_SectorRead           EvtFsMcSPIId(EventLevelOp,    24)
#define EvtFsMcSPI_SectorReadError      EvtFsMcSPIId(EventLevelError, 25)
#define EvtFsMcSPI_SectorWrite          EvtFsMcSPIId(EventLevelOp,    26)
#define EvtFsMcSPI_SectorWriteError     EvtFsMcSPIId(EventLevelError, 27)
#define EvtFsMcSPI_ReadInfo             EvtFsMcSPIId(EventLevelOp,    28)
#define EvtFsMcSPI_ReadInfoSuccess      EvtFsMcSPIId(EventLevelOp,    29)
#define EvtFsMcSPI_DevCtrl              EvtFsMcSPIId(EventLevelOp,    30)
#define EvtFsMcSPI_DevCtrlUnsupported   EvtFsMcSPIId(EventLevelError, 31)
#define EvtFsMcSPI_LockUnlockExec       EvtFsMcSPIId(EventLevelOp,    32)
#define EvtFsMcSPI_LockUnlockExecError  EvtFsMcSPIId(EventLevelError, 33)
#define EvtFsMcSPI_LockActive           EvtFsMcSPIId(EventLevelOp,    34)
#define EvtFsMcSPI_LockDeactivated      EvtFsMcSPIId(EventLevelOp,    35)
#define EvtFsMcSPI_BusSpeedControl      EvtFsMcSPIId(EventLevelOp,    36)
#define EvtFsMcSPI_SlaveSelCtrlMissing  EvtFsMcSPIId(EventLevelOp,    37)
#define EvtFsMcSPI_ReadCdMissing        EvtFsMcSPIId(EventLevelOp,    38)
#define EvtFsMcSPI_ReadWpMissing        EvtFsMcSPIId(EventLevelOp,    39)
#define EvtFsMcSPI_CrcDisableError      EvtFsMcSPIId(EventLevelError, 40)
#define EvtFsMcSPI_DriverSendError      EvtFsMcSPIId(EventLevelError, 41)
#define EvtFsMcSPI_DriverReceiveError   EvtFsMcSPIId(EventLevelError, 42)
#define EvtFsMcSPI_CmdSendError         EvtFsMcSPIId(EventLevelError, 43)
#define EvtFsMcSPI_CmdResponseError     EvtFsMcSPIId(EventLevelError, 44)
#define EvtFsMcSPI_ACmdResponseError    EvtFsMcSPIId(EventLevelError, 45)
#define EvtFsMcSPI_ParameterInvalid     EvtFsMcSPIId(EventLevelError, 46)
#define EvtFsMcSPI_MediaPasswordEnabled EvtFsMcSPIId(EventLevelError, 47)
#endif /* defined(FS_MEMORY_CARD_0) || defined(FS_MEMORY_CARD_1) */
#endif


/**
  \brief  Event on file open (API)
  \param[in]  name     file name pointer
  \param[in]  openmode ANSI open mode bitmap
 */
#ifdef EvtFsCore_sys_open
  __STATIC_INLINE void EvrFsCore_sys_open (const char *name, int openmode) {
    EventRecord2 (EvtFsCore_sys_open, (uint32_t)name, (uint32_t)openmode);
  }
#else
  #define EvrFsCore_sys_open(name, openmode)
#endif

/**
  \brief  Event on file close (API)
  \param[in]  fh        file handle
 */
#ifdef EvtFsCore_sys_close
  __STATIC_INLINE void EvrFsCore_sys_close (int fh) {
    EventRecord2 (EvtFsCore_sys_close, (uint32_t)fh, 0);
  }
#else
  #define EvrFsCore_sys_close(fh)
#endif

/**
  \brief  Event on write to a file (API)
  \param[in]  fh        file handle
  \param[in]  buf       data buffer
  \param[in]  len       data length
 */
#ifdef EvtFsCore_sys_write
  __STATIC_INLINE void EvrFsCore_sys_write (int fh, const unsigned char *buf, unsigned int len) {
    EventRecord4 (EvtFsCore_sys_write, (uint32_t)fh, (uint32_t)buf, len, 0);
  }
#else
  #define EvrFsCore_sys_write(fh, buf, len)
#endif

/**
  \brief  Event on read from a file (API)
  \param[in]  fh        file handle
  \param[in]  buf       data buffer
  \param[in]  len       data length
 */
#ifdef EvtFsCore_sys_read
  __STATIC_INLINE void EvrFsCore_sys_read (int fh, unsigned char *buf, unsigned int len) {
    EventRecord4 (EvtFsCore_sys_read, (uint32_t)fh, (uint32_t)buf, len, 0);
  }
#else
  #define EvrFsCore_sys_read(fh, buf, len)
#endif

/**
  \brief  Event on file position move (API)
  \param[in]  fh        file handle
  \param[in]  pos       file position from the file start
 */
#ifdef EvtFsCore_sys_seek
  __STATIC_INLINE void EvrFsCore_sys_seek (int fh, long pos) {
    EventRecord2 (EvtFsCore_sys_seek, (uint32_t)fh, (uint32_t)pos);
  }
#else
  #define EvrFsCore_sys_seek(fh, pos)
#endif

/**
  \brief  Event on current file length request (API)
  \param[in]  fh        file handle
 */
#ifdef EvtFsCore_sys_flen
  __STATIC_INLINE void EvrFsCore_sys_flen (int fh) {
    EventRecord2 (EvtFsCore_sys_flen, (uint32_t)fh, 0);
  }
#else
  #define EvrFsCore_sys_flen(fh)
#endif

/**
  \brief  Event on file handle assignment (Op)
  \param[in]  name      file name pointer
  \param[in]  fh        file handle
 */
#ifdef EvtFsCore_sys_handle_assign
  __STATIC_INLINE void EvrFsCore_sys_handle_assign (const char *name, int fh) {
    EventRecord2 (EvtFsCore_sys_handle_assign, (uint32_t)name, (uint32_t)fh);
  }
#else
  #define EvrFsCore_sys_handle_assign(name, fh)
#endif

/**
  \brief  Event on drive initialization (API)
  \param[in]  drive     specified drive
 */
#ifdef EvtFsCore_finit
  __STATIC_INLINE void EvrFsCore_finit (const char *drive) {
    EventRecord2 (EvtFsCore_finit, (uint32_t)drive, 0);
  }
#else
  #define EvrFsCore_finit(drive)
#endif

/**
  \brief  Event on drive uninitialization (API)
  \param[in]  drive     specified drive
 */
#ifdef EvtFsCore_funinit
  __STATIC_INLINE void EvrFsCore_funinit (const char *drive) {
    EventRecord2 (EvtFsCore_funinit, (uint32_t)drive, 0);
  }
#else
  #define EvrFsCore_funinit(drive)
#endif

/**
  \brief  Event on drive mount (API)
  \param[in]  drive     specified drive
 */
#ifdef EvtFsCore_fmount
  __STATIC_INLINE void EvrFsCore_fmount (const char *drive) {
    EventRecord2 (EvtFsCore_fmount, (uint32_t)drive, 0);
  }
#else
  #define EvrFsCore_fmount(drive)
#endif

/**
  \brief  Event on drive unmount (API)
  \param[in]  drive     specified drive
 */
#ifdef EvtFsCore_funmount
  __STATIC_INLINE void EvrFsCore_funmount (const char *drive) {
    EventRecord2 (EvtFsCore_funmount, (uint32_t)drive, 0);
  }
#else
  #define EvrFsCore_funmount(drive)
#endif

/**
  \brief  Event on file delete (API)
  \param[in]  path      a string specifying the file or directory.
  \param[in]  options   a string specifying function options.
 */
#ifdef EvtFsCore_fdelete
  __STATIC_INLINE void EvrFsCore_fdelete (const char *path, const char *options) {
    EventRecord2 (EvtFsCore_fdelete, (uint32_t)path, (uint32_t)options);
  }
#else
  #define EvrFsCore_fdelete(path, options)
#endif

/**
  \brief  Event on file or directory search (API)
  \param[in]  pattern   a string specifying the pattern.
  \param[in]  info      structure storing information about matching files.
 */
#ifdef EvtFsCore_ffind
  __STATIC_INLINE void EvrFsCore_ffind (const char *pattern, fsFileInfo *info) {
    EventRecord2 (EvtFsCore_ffind, (uint32_t)pattern, (uint32_t)info);
  }
#else
  #define EvrFsCore_ffind(pattern, info)
#endif

/**
  \brief  Event on file or directory rename (API)
  \param[in]  path      a string specifying the file or directory.
  \param[in]  newname   a string specifying new file or directory name.
 */
#ifdef EvtFsCore_frename
  __STATIC_INLINE void EvrFsCore_frename (const char *path, const char *newname) {
    EventRecord2 (EvtFsCore_frename, (uint32_t)path, (uint32_t)newname);
  }
#else
  #define EvrFsCore_frename(path, newname)
#endif

/**
  \brief  Event on file or directory attribute change (API)
  \param[in]  path      a string specifying the file or directory.
  \param[in]  attr      string specifying file or directory attributes to be modified.
 */
#ifdef EvtFsCore_fattrib
  __STATIC_INLINE void EvrFsCore_fattrib (const char *path, const char *attr) {
    EventRecord2 (EvtFsCore_fattrib, (uint32_t)path, (uint32_t)attr);
  }
#else
  #define EvrFsCore_fattrib(path, attr)
#endif

/**
  \brief  Event on working directory retrieval (API)
  \param[in]  drive     a string specifying the file or directory.
  \param[in]  buf       pointer to string buffer.
  \param[in]  len       string buffer length.
 */
#ifdef EvtFsCore_fpwd
  __STATIC_INLINE void EvrFsCore_fpwd (const char *drive, char *buf, uint32_t len) {
    EventRecord4 (EvtFsCore_fpwd, (uint32_t)drive, (uint32_t)buf, len, 0);
  }
#else
  #define EvrFsCore_fpwd(drive, buf, len)
#endif

/**
  \brief  Event on working directory change (API)
  \param[in]  path      a string specifying the directory path.
 */
#ifdef EvtFsCore_fchdir
  __STATIC_INLINE void EvrFsCore_fchdir (const char *path) {
    EventRecord2 (EvtFsCore_fchdir, (uint32_t)path, 0);
  }
#else
  #define EvrFsCore_fchdir(path)
#endif

/**
  \brief  Event on directory create (API)
  \param[in]  path      a string specifying directory path.
 */
#ifdef EvtFsCore_fmkdir
  __STATIC_INLINE void EvrFsCore_fmkdir (const char *path) {
    EventRecord2 (EvtFsCore_fmkdir, (uint32_t)path, 0);
  }
#else
  #define EvrFsCore_fmkdir(path)
#endif

/**
  \brief  Event on directory remove (API)
  \param[in]  path      a string specifying directory path.
  \param[in]  options   a string specifying function options.
 */
#ifdef EvtFsCore_frmdir
  __STATIC_INLINE void EvrFsCore_frmdir (const char *path, const char *options) {
    EventRecord2 (EvtFsCore_frmdir, (uint32_t)path, (uint32_t)options);
  }
#else
  #define EvrFsCore_frmdir(path, options)
#endif

/**
  \brief  Event on current drive change (API)
  \param[in]  drive     a string specifying the drive.
 */
#ifdef EvtFsCore_fchdrive
  __STATIC_INLINE void EvrFsCore_fchdrive (const char *drive) {
    EventRecord2 (EvtFsCore_fchdrive, (uint32_t)drive, 0);
  }
#else
  #define EvrFsCore_fchdrive(drive)
#endif

/**
  \brief  Event on free space retrieval (API)
  \param[in]  drive     a string specifying the drive.
 */
#ifdef EvtFsCore_ffree
  __STATIC_INLINE void EvrFsCore_ffree (const char *drive) {
    EventRecord2 (EvtFsCore_ffree, (uint32_t)drive, 0);
  }
#else
  #define EvrFsCore_ffree(drive)
#endif

/**
  \brief  Event on drive format (API)
  \param[in]  drive     a string specifying the drive.
  \param[in]  options   string specifying formatting options.
 */
#ifdef EvtFsCore_fformat
  __STATIC_INLINE void EvrFsCore_fformat (const char *drive, const char *options) {
    EventRecord2 (EvtFsCore_fformat, (uint32_t)drive, (uint32_t)options);
  }
#else
  #define EvrFsCore_fformat(drive, options)
#endif

/**
  \brief  Event on drive analyse (API)
  \param[in]  drive     a string specifying the drive.
 */
#ifdef EvtFsCore_fanalyse
  __STATIC_INLINE void EvrFsCore_fanalyse (const char *drive) {
    EventRecord2 (EvtFsCore_fanalyse, (uint32_t)drive, 0);
  }
#else
  #define EvrFsCore_fanalyse(drive)
#endif

/**
  \brief  Event on drive check (API)
  \param[in]  drive     a string specifying the drive.
 */
#ifdef EvtFsCore_fcheck
  __STATIC_INLINE void EvrFsCore_fcheck (const char *drive) {
    EventRecord2 (EvtFsCore_fcheck, (uint32_t)drive, 0);
  }
#else
  #define EvrFsCore_fcheck(drive)
#endif

/**
  \brief  Event on drive defrag (API)
  \param[in]  drive     a string specifying the drive.
 */
#ifdef EvtFsCore_fdefrag
  __STATIC_INLINE void EvrFsCore_fdefrag (const char *drive) {
    EventRecord2 (EvtFsCore_fdefrag, (uint32_t)drive, 0);
  }
#else
  #define EvrFsCore_fdefrag(drive)
#endif

/**
  \brief  Event on media presence check (API)
  \param[in]  drive     a string specifying the drive.
 */
#ifdef EvtFsCore_fmedia
  __STATIC_INLINE void EvrFsCore_fmedia (const char *drive) {
    EventRecord2 (EvtFsCore_fmedia, (uint32_t)drive, 0);
  }
#else
  #define EvrFsCore_fmedia(drive)
#endif

/**
  \brief  Event on drive information read (API)
  \param[in]  drive     a string specifying the drive.
  \param[in]  info      drive information structure.
 */
#ifdef EvtFsCore_finfo
  __STATIC_INLINE void EvrFsCore_finfo (const char *drive, fsDriveInfo *info) {
    EventRecord2 (EvtFsCore_finfo, (uint32_t)drive, (uint32_t)info);
  }
#else
  #define EvrFsCore_finfo(drive, info)
#endif

/**
  \brief  Event on drive label and serial number read (API)
  \param[in]  drive     a string specifying the drive.
  \param[in]  label     buffer for storing label as a null terminated string.
  \param[in]  serial    pointer to variable where serial number will be stored.
 */
#ifdef EvtFsCore_fvol
  __STATIC_INLINE void EvrFsCore_fvol (const char *drive, char *label, uint32_t *serial) {
    EventRecord4 (EvtFsCore_fvol, (uint32_t)drive, (uint32_t)label, (uint32_t)serial, 0);
  }
#else
  #define EvrFsCore_fvol(drive, label, serial)
#endif

/**
  \brief  Event on invalid or disabled drive specifier (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsCore_InvalidDrive
  __STATIC_INLINE void EvrFsCore_InvalidDrive (uint32_t drive) {
    EventRecord2 (EvtFsCore_InvalidDrive, drive, 0);
  }
#else
  #define EvrFsCore_InvalidDrive(drive)
#endif

/**
  \brief  Event on current drive selection (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsCore_CurrentDriveSelect
  __STATIC_INLINE void EvrFsCore_CurrentDriveSelect (uint32_t drive) {
    EventRecord2 (EvtFsCore_CurrentDriveSelect, drive, 0);
  }
#else
  #define EvrFsCore_CurrentDriveSelect(drive)
#endif

/**
  \brief  Event on file or directory timestamp set (API)
  \param[in]  path      string specifying the file or directory path.
  \param[in]  create    \ref fsTime structure specifying the file creation time
  \param[in]  access    \ref fsTime structure specifying the file last access time
  \param[in]  write     \ref fsTime structure specifying the file last write time
 */
#ifdef EvtFsCore_ftime_set
  __STATIC_INLINE void EvrFsCore_ftime_set (const char *path, fsTime *create, fsTime *access, fsTime *write) {
    EventRecord4 (EvtFsCore_ftime_set, (uint32_t)path, (uint32_t)create, (uint32_t)access, (uint32_t)write);
  }
#else
  #define EvrFsCore_ftime_set(path, create, access, write)
#endif

/**
  \brief  Event on file or directory timestamp get (API)
  \param[in]  path      string specifying the file or directory path.
  \param[out] create    \ref fsTime structure where file creation time will be stored
  \param[out] access    \ref fsTime structure where file last access time will be stored
  \param[out] write     \ref fsTime structure where file last write time will be stored
 */
#ifdef EvtFsCore_ftime_get
  __STATIC_INLINE void EvrFsCore_ftime_get (const char *path, fsTime *create, fsTime *access, fsTime *write) {
    EventRecord4 (EvtFsCore_ftime_get, (uint32_t)path, (uint32_t)create, (uint32_t)access, (uint32_t)write);
  }
#else
  #define EvrFsCore_ftime_get(path, create, access, write)
#endif

/**
  \brief  Event on FileSystem component version retrieve (API)
  \param[in]  version   BCD encoded version: 0xMMmmbbbb (MM:major, mm:minor, bbbb:build)
 */
#ifdef EvtFsCore_fversion
  __STATIC_INLINE void EvrFsCore_fversion (uint32_t version) {
    uint32_t MMmm, bbbb;

    MMmm   = (((version >> 28) & 0xF)*10) + ((version >> 24) & 0xF);
    MMmm <<= 16;
    MMmm  |= (((version >> 20) & 0xF)*10) + ((version >> 16) & 0xF);

    bbbb   = ((version >> 12) & 0xF)*1000 + \
             ((version >>  8) & 0xF)*100  + \
             ((version >>  4) & 0xF)*10   + \
              (version & 0xF);

    EventRecord2 (EvtFsCore_fversion, MMmm, bbbb);
  }
#else
  #define EvrFsCore_fversion(version)
#endif

/**
  \brief  Event on file open (API)
  \param[in]  path     file name pointer
  \param[in]  mode     ANSI open mode bitmap
 */
#ifdef EvtFsCore_fs_fopen
  __STATIC_INLINE void EvrFsCore_fs_fopen (const char *path, int mode) {
    EventRecord2 (EvtFsCore_fs_fopen, (uint32_t)path, (uint32_t)mode);
  }
#else
  #define EvrFsCore_fs_fopen(path, mode)
#endif

/**
  \brief  Event on file close (API)
  \param[in]  handle    file handle
 */
#ifdef EvtFsCore_fs_fclose
  __STATIC_INLINE void EvrFsCore_fs_fclose (int handle) {
    EventRecord2 (EvtFsCore_fs_fclose, (uint32_t)handle, 0);
  }
#else
  #define EvrFsCore_fs_fclose(handle)
#endif

/**
  \brief  Event on write to a file (API)
  \param[in]  handle    file handle
  \param[in]  buf       data buffer
  \param[in]  cnt       number of bytes to write
 */
#ifdef EvtFsCore_fs_fwrite
  __STATIC_INLINE void EvrFsCore_fs_fwrite (int32_t handle, const void *buf, uint32_t cnt) {
    EventRecord4 (EvtFsCore_fs_fwrite, (uint32_t)handle, (uint32_t)buf, cnt, 0);
  }
#else
  #define EvrFsCore_fs_fwrite(handle, buf, cnt)
#endif

/**
  \brief  Event on read from a file (API)
  \param[in]  handle    file handle
  \param[in]  buf       data buffer
  \param[in]  cnt       number of bytes to read
 */
#ifdef EvtFsCore_fs_fread
  __STATIC_INLINE void EvrFsCore_fs_fread (int32_t handle, void *buf, uint32_t cnt) {
    EventRecord4 (EvtFsCore_fs_fread, (uint32_t)handle, (uint32_t)buf, cnt, 0);
  }
#else
  #define EvrFsCore_fs_fread(handle, buf, cnt)
#endif

/**
  \brief  Event on file buffer flush (API)
  \param[in]  handle    file handle
 */
#ifdef EvtFsCore_fs_fflush
  __STATIC_INLINE void EvrFsCore_fs_fflush (int32_t handle) {
    EventRecord2 (EvtFsCore_fs_fflush, (uint32_t)handle, 0U);
  }
#else
  #define EvrFsCore_fs_fflush(handle)
#endif

/**
  \brief  Event on file position move (API)
  \param[in]  handle    file handle
  \param[in]  offset    the number of bytes to move
  \param[in]  whence    file position location
 */
#ifdef EvtFsCore_fs_fseek
  __STATIC_INLINE void EvrFsCore_fs_fseek (int32_t handle, int64_t offset, int32_t whence) {
    EventRecord4 (EvtFsCore_fs_fseek, (uint32_t)handle, (uint32_t)offset, (uint32_t)(offset>>32), (uint32_t)whence);
  }
#else
  #define EvrFsCore_fs_fseek(handle, offset, whence)
#endif

/**
  \brief  Event on current file length request (API)
  \param[in]  handle    file handle
 */
#ifdef EvtFsCore_fs_fsize
  __STATIC_INLINE void EvrFsCore_fs_fsize (int handle) {
    EventRecord2 (EvtFsCore_fs_fsize, (uint32_t)handle, 0);
  }
#else
  #define EvrFsCore_fs_fsize(handle)
#endif


/**
  \brief  Event on FAT drive initialization (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InitDrive
  __STATIC_INLINE void EvrFsFAT_InitDrive(uint32_t drive) {
    EventRecord2(EvtFsFAT_InitDrive, drive, 0);
  }
#else
  #define EvrFsFAT_InitDrive(drive)
#endif

/**
  \brief  Event on successful FAT drive initialization (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InitDriveSuccess
  __STATIC_INLINE void EvrFsFAT_InitDriveSuccess(uint32_t drive) {
    EventRecord2(EvtFsFAT_InitDriveSuccess, drive, 0);
  }
#else
  #define EvrFsFAT_InitDriveSuccess(drive)
#endif

/**
  \brief  Event on FAT driver configuration error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InitDriverCfgError
  __STATIC_INLINE void EvrFsFAT_InitDriverCfgError(uint32_t drive) {
    EventRecord2(EvtFsFAT_InitDriverCfgError, drive, 0);
  }
#else
  #define EvrFsFAT_InitDriverCfgError(drive)
#endif

/**
  \brief  Event on FAT driver initialization error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InitDriverError
  __STATIC_INLINE void EvrFsFAT_InitDriverError(uint32_t drive) {
    EventRecord2(EvtFsFAT_InitDriverError, drive, 0);
  }
#else
  #define EvrFsFAT_InitDriverError(drive)
#endif

/**
  \brief  Event on FAT drive uninitialization (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_UninitDrive
  __STATIC_INLINE void EvrFsFAT_UninitDrive(uint32_t drive) {
    EventRecord2(EvtFsFAT_UninitDrive, drive, 0);
  }
#else
  #define EvrFsFAT_UninitDrive(drive)
#endif

/**
  \brief  Event on FAT drive mount operation (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_MountDrive
  __STATIC_INLINE void EvrFsFAT_MountDrive(uint32_t drive) {
    EventRecord2(EvtFsFAT_MountDrive, drive, 0);
  }
#else
  #define EvrFsFAT_MountDrive(drive)
#endif

/**
  \brief  Event on FAT media initialization error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InitMediaError
  __STATIC_INLINE void EvrFsFAT_InitMediaError(uint32_t drive) {
    EventRecord2(EvtFsFAT_InitMediaError, drive, 0);
  }
#else
  #define EvrFsFAT_InitMediaError(drive)
#endif

/**
  \brief  Event on FAT media not present status error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_MediaInsertStatError
  __STATIC_INLINE void EvrFsFAT_MediaInsertStatError(uint32_t drive) {
    EventRecord2(EvtFsFAT_MediaInsertStatError, drive, 0);
  }
#else
  #define EvrFsFAT_MediaInsertStatError(drive)
#endif

/**
  \brief  Event on FAT media init status error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_MediaInitStatError
  __STATIC_INLINE void EvrFsFAT_MediaInitStatError(uint32_t drive) {
    EventRecord2(EvtFsFAT_MediaInitStatError, drive, 0);
  }
#else
  #define EvrFsFAT_MediaInitStatError(drive)
#endif

/**
  \brief  Event on FAT volume ready status error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_VolumeReadyStatError
  __STATIC_INLINE void EvrFsFAT_VolumeReadyStatError(uint32_t drive) {
    EventRecord2(EvtFsFAT_VolumeReadyStatError, drive, 0);
  }
#else
  #define EvrFsFAT_VolumeReadyStatError(drive)
#endif

/**
  \brief  Event on FAT volume write protection active status error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_VolumeWriteStatError
  __STATIC_INLINE void EvrFsFAT_VolumeWriteStatError(uint32_t drive) {
    EventRecord2(EvtFsFAT_VolumeWriteStatError, drive, 0);
  }
#else
  #define EvrFsFAT_VolumeWriteStatError(drive)
#endif

/**
  \brief  Event on FAT when using unmounted volume (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_VolumeNotMounted
  __STATIC_INLINE void EvrFsFAT_VolumeNotMounted (uint32_t drive) {
    EventRecord2(EvtFsFAT_VolumeNotMounted, drive, 0);
  }
#else
  #define EvrFsFAT_VolumeNotMounted(drive)
#endif

/**
  \brief  Event on Master Boot Record read operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sector    Master Boot Record sector number
 */
#ifdef EvtFsFAT_ReadMBR
  __STATIC_INLINE void EvrFsFAT_ReadMBR (uint32_t drive, uint32_t sector) {
    EventRecord2(EvtFsFAT_ReadMBR, drive, sector);
  }
#else
  #define EvrFsFAT_ReadMBR(drive, sector)
#endif

/**
  \brief  Event on invalid Master Boot Record (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InvalidMBR
  __STATIC_INLINE void EvrFsFAT_InvalidMBR (uint32_t drive) {
    EventRecord2(EvtFsFAT_InvalidMBR, drive, 0);
  }
#else
  #define EvrFsFAT_InvalidMBR(drive)
#endif

/**
  \brief  Event on nonexistent Master Boot Record (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_NonexistentMBR
  __STATIC_INLINE void EvrFsFAT_NonexistentMBR (uint32_t drive) {
    EventRecord2(EvtFsFAT_NonexistentMBR, drive, 0);
  }
#else
  #define EvrFsFAT_NonexistentMBR(drive)
#endif

/**
  \brief  Event on Boot Sector read operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sector    Boot Sector sector number
 */
#ifdef EvtFsFAT_ReadBootSector
  __STATIC_INLINE void EvrFsFAT_ReadBootSector (uint32_t drive, uint32_t sector) {
    EventRecord2(EvtFsFAT_ReadBootSector, drive, sector);
  }
#else
  #define EvrFsFAT_ReadBootSector(drive, sector)
#endif

/**
  \brief  Event on invalid Boot Sector (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InvalidBootSector
  __STATIC_INLINE void EvrFsFAT_InvalidBootSector (uint32_t drive) {
    EventRecord2(EvtFsFAT_InvalidBootSector, drive, 0);
  }
#else
  #define EvrFsFAT_InvalidBootSector(drive)
#endif

/**
  \brief  Event on invalid FAT volume description (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InvalidFAT
  __STATIC_INLINE void EvrFsFAT_InvalidFAT (uint32_t drive) {
    EventRecord2(EvtFsFAT_InvalidFAT, drive, 0);
  }
#else
  #define EvrFsFAT_InvalidFAT(drive)
#endif

/**
  \brief  Event on FAT FSInfo read operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sector    FS Info sector number
 */
#ifdef EvtFsFAT_ReadFSInfo
  __STATIC_INLINE void EvrFsFAT_ReadFSInfo (uint32_t drive, uint32_t sector) {
    EventRecord2(EvtFsFAT_ReadFSInfo, drive, sector);
  }
#else
  #define EvrFsFAT_ReadFSInfo(drive, sector)
#endif

/**
  \brief  Event on invalid FAT FSInfo signature (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InvalidFSInfo
  __STATIC_INLINE void EvrFsFAT_InvalidFSInfo (uint32_t drive) {
    EventRecord2(EvtFsFAT_InvalidFSInfo, drive, 0);
  }
#else
  #define EvrFsFAT_InvalidFSInfo(drive)
#endif

/**
  \brief  Event on invalid number of free clusters in FAT FSInfo signature (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InvalidFreeClusFSInfo
  __STATIC_INLINE void EvrFsFAT_InvalidFreeClusFSInfo (uint32_t drive) {
    EventRecord2(EvtFsFAT_InvalidFreeClusFSInfo, drive, 0);
  }
#else
  #define EvrFsFAT_InvalidFreeClusFSInfo(drive)
#endif

/**
  \brief  Event on invalid next free cluster in FAT FSInfo signature (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InvalidNextClusFSInfo
  __STATIC_INLINE void EvrFsFAT_InvalidNextClusFSInfo (uint32_t drive) {
    EventRecord2(EvtFsFAT_InvalidNextClusFSInfo, drive, 0);
  }
#else
  #define EvrFsFAT_InvalidNextClusFSInfo(drive)
#endif

/**
  \brief  Event on FAT FSInfo write operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sector    FS Info sector number
 */
#ifdef EvtFsFAT_WriteFSInfo
  __STATIC_INLINE void EvrFsFAT_WriteFSInfo (uint32_t drive, uint32_t sector) {
    EventRecord2(EvtFsFAT_WriteFSInfo, drive, sector);
  }
#else
  #define EvrFsFAT_WriteFSInfo(drive, sector)
#endif

/**
  \brief  Event on count number of free clusters operation (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_CountFreeClus
  __STATIC_INLINE void EvrFsFAT_CountFreeClus (uint32_t drive) {
    EventRecord2(EvtFsFAT_CountFreeClus, drive, 0);
  }
#else
  #define EvrFsFAT_CountFreeClus(drive)
#endif

/**
  \brief  Event on successful mount of a FAT drive (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_MountDriveSuccess
  __STATIC_INLINE void EvrFsFAT_MountDriveSuccess(uint32_t drive) {
    EventRecord2(EvtFsFAT_MountDriveSuccess, drive, 0);
  }
#else
  #define EvrFsFAT_MountDriveSuccess(drive)
#endif

/**
  \brief  Event on FAT drive unmount operation (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_UnmountDrive
  __STATIC_INLINE void EvrFsFAT_UnmountDrive(uint32_t drive) {
    EventRecord2(EvtFsFAT_UnmountDrive, drive, 0);
  }
#else
  #define EvrFsFAT_UnmountDrive(drive)
#endif

/**
  \brief  Event on successful unmount of a FAT drive (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_UnmountDriveSuccess
  __STATIC_INLINE void EvrFsFAT_UnmountDriveSuccess(uint32_t drive) {
    EventRecord2(EvtFsFAT_UnmountDriveSuccess, drive, 0);
  }
#else
  #define EvrFsFAT_UnmountDriveSuccess(drive)
#endif

/**
  \brief  Event on FAT drive format operation (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_FormatDrive
  __STATIC_INLINE void EvrFsFAT_FormatDrive(uint32_t drive) {
    EventRecord2(EvtFsFAT_FormatDrive, drive, 0);
  }
#else
  #define EvrFsFAT_FormatDrive(drive)
#endif

/**
  \brief  Event on successful formating of a FAT drive (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_FormatDriveSuccess
  __STATIC_INLINE void EvrFsFAT_FormatDriveSuccess(uint32_t drive) {
    EventRecord2(EvtFsFAT_FormatDriveSuccess, drive, 0);
  }
#else
  #define EvrFsFAT_FormatDriveSuccess(drive)
#endif

/**
  \brief  Event on reset of all FAT file handles (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_ResetHandles
  __STATIC_INLINE void EvrFsFAT_ResetHandles (uint32_t drive) {
    EventRecord2(EvtFsFAT_ResetHandles, drive, 0);
  }
#else
  #define EvrFsFAT_ResetHandles(drive)
#endif

/**
  \brief  Event on FAT drive format option detection (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  option    option identifier
 */
#ifdef EvtFsFAT_FormatOptionDetected
  __STATIC_INLINE void EvrFsFAT_FormatOptionDetected (uint32_t drive, uint32_t option) {
    EventRecord2(EvtFsFAT_FormatOptionDetected, drive, option);
  }
#else
  #define EvrFsFAT_FormatOptionDetected(drive, option)
#endif

/**
  \brief  Event on FAT media capacity read (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sectors   number of available sectors
 */
#ifdef EvtFsFAT_FormatMediaCapacity
  __STATIC_INLINE void EvrFsFAT_FormatMediaCapacity (uint32_t drive, uint32_t sectors) {
    EventRecord2(EvtFsFAT_FormatMediaCapacity, drive, sectors);
  }
#else
  #define EvrFsFAT_FormatMediaCapacity(drive, sectors)
#endif

/**
  \brief  Event on FAT media low level format operation (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_FormatLowLevel
  __STATIC_INLINE void EvrFsFAT_FormatLowLevel (uint32_t drive) {
    EventRecord2(EvtFsFAT_FormatLowLevel, drive, 0);
  }
#else
  #define EvrFsFAT_FormatLowLevel(drive)
#endif

/**
  \brief  Event on not enough free space for FAT12/16 format (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_FormatNoSpace
  __STATIC_INLINE void EvrFsFAT_FormatNoSpace (uint32_t drive) {
    EventRecord2(EvtFsFAT_FormatNoSpace, drive, 0);
  }
#else
  #define EvrFsFAT_FormatNoSpace(drive)
#endif

/**
  \brief  Event on not enough free space for FAT32 format (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_FormatNoSpaceFAT32
  __STATIC_INLINE void EvrFsFAT_FormatNoSpaceFAT32 (uint32_t drive) {
    EventRecord2(EvtFsFAT_FormatNoSpaceFAT32, drive, 0);
  }
#else
  #define EvrFsFAT_FormatNoSpaceFAT32(drive)
#endif

/**
  \brief  Event on Master Boot Record write operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sector    master boot record sector number
 */
#ifdef EvtFsFAT_WriteMBR
  __STATIC_INLINE void EvrFsFAT_WriteMBR (uint32_t drive, uint32_t sector) {
    EventRecord2(EvtFsFAT_WriteMBR, drive, sector);
  }
#else
  #define EvrFsFAT_WriteMBR(drive, sector)
#endif

/**
  \brief  Event on Boot Sector write operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sector    boot sector number
 */
#ifdef EvtFsFAT_WriteBootSector
  __STATIC_INLINE void EvrFsFAT_WriteBootSector (uint32_t drive, uint32_t sector) {
    EventRecord2(EvtFsFAT_WriteBootSector, drive, sector);
  }
#else
  #define EvrFsFAT_WriteBootSector(drive, sector)
#endif

/**
  \brief  Event on FAT hidden sectors clear operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sector    sector number
  \param[in]  count     number of sectors
 */
#ifdef EvtFsFAT_ClearHiddenSectors
  __STATIC_INLINE void EvrFsFAT_ClearHiddenSectors (uint32_t drive, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsFAT_ClearHiddenSectors, drive, sector, count, 0);
  }
#else
  #define EvrFsFAT_ClearHiddenSectors(drive, sector, count)
#endif

/**
  \brief  Event on FAT reserved sectors clear operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sector    sector number
  \param[in]  count     number of sectors
 */
#ifdef EvtFsFAT_ClearReservedSectors
  __STATIC_INLINE void EvrFsFAT_ClearReservedSectors (uint32_t drive, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsFAT_ClearReservedSectors, drive, sector, count, 0);
  }
#else
  #define EvrFsFAT_ClearReservedSectors(drive, sector, count)
#endif

/**
  \brief  Event on FAT root sectors clear operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sector    sector number
  \param[in]  count     number of sectors
 */
#ifdef EvtFsFAT_ClearRootSectors
  __STATIC_INLINE void EvrFsFAT_ClearRootSectors (uint32_t drive, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsFAT_ClearRootSectors, drive, sector, count, 0);
  }
#else
  #define EvrFsFAT_ClearRootSectors(drive, sector, count)
#endif

/**
  \brief  Event on FAT File Allocation Table create operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sector    sector number
  \param[in]  count     number of sectors
 */
#ifdef EvtFsFAT_CreateFAT
  __STATIC_INLINE void EvrFsFAT_CreateFAT (uint32_t drive, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsFAT_CreateFAT, drive, sector, count, 0);
  }
#else
  #define EvrFsFAT_CreateFAT(drive, sector, count)
#endif

/**
  \brief  Event on setting FAT format properties (Op)
  \param[in]  drive         4 byte encoded drive letter
  \param[in]  type          FAT file system type
  \param[in]  clus_size     cluster size
  \param[in]  data_clus     number of data clusters
 */
#ifdef EvtFsFAT_FormatProperties
  __STATIC_INLINE void EvrFsFAT_FormatProperties (uint32_t drive, uint32_t type, uint32_t clus_size, uint32_t data_clus) {
    EventRecord4 (EvtFsFAT_FormatProperties, drive, type, clus_size, data_clus);
  }
#else
  #define EvrFsFAT_FormatProperties(drive, type, clus_size, data_clus)
#endif

/**
  \brief  Event on FAT file open (Op)
  \param[in]  h         FAT file handle index
  \param[in]  path      file path buffer memory location
  \param[in]  openmode  open mode
 */
#ifdef EvtFsFAT_FileOpen
  __STATIC_INLINE void EvrFsFAT_FileOpen (int32_t h, const char *path, int32_t openmode) {
    EventRecord4 (EvtFsFAT_FileOpen, (uint32_t)h, (uint32_t)path, (uint32_t)openmode, 0);
  }
#else
  #define EvrFsFAT_FileOpen(h, path, openmode)
#endif

/**
  \brief  Event on invalid FAT path character (Error)
  \param[in]  ch        invalid character found
 */
#ifdef EvtFsFAT_PathInvalidChar
  __STATIC_INLINE void EvrFsFAT_PathInvalidChar (const char ch) {
    EventRecord2 (EvtFsFAT_PathInvalidChar, ch, 0);
  }
#else
  #define EvrFsFAT_PathInvalidChar(ch)
#endif

/**
  \brief  Event on too long FAT path (Error)
  \param[in]  max_len   maximum path length
 */
#ifdef EvtFsFAT_PathIsTooLong
  __STATIC_INLINE void EvrFsFAT_PathIsTooLong (uint32_t max_len) {
    EventRecord2 (EvtFsFAT_PathIsTooLong, max_len, 0);
  }
#else
  #define EvrFsFAT_PathIsTooLong(max_len)
#endif

/**
  \brief  Event on FAT directory found instead of file (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_PathIsDirNotFile
  __STATIC_INLINE void EvrFsFAT_PathIsDirNotFile (uint32_t drive) {
    EventRecord2 (EvtFsFAT_PathIsDirNotFile, drive, 0);
  }
#else
  #define EvrFsFAT_PathIsDirNotFile(drive)
#endif

/**
  \brief  Event on specified FAT file already in use (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_FileIsInUse
  __STATIC_INLINE void EvrFsFAT_FileIsInUse (uint32_t drive) {
    EventRecord2 (EvtFsFAT_FileIsInUse, drive, 0);
  }
#else
  #define EvrFsFAT_FileIsInUse(drive)
#endif

/**
  \brief  Event on encounter of read only FAT file (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_FileIsReadOnly
  __STATIC_INLINE void EvrFsFAT_FileIsReadOnly (uint32_t drive) {
    EventRecord2 (EvtFsFAT_FileIsReadOnly, drive, 0);
  }
#else
  #define EvrFsFAT_FileIsReadOnly(drive)
#endif

/**
  \brief  Event on specification of nonexistent FAT file (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_FileIsNonExistent
  __STATIC_INLINE void EvrFsFAT_FileIsNonExistent (uint32_t drive) {
    EventRecord2 (EvtFsFAT_FileIsNonExistent, drive, 0);
  }
#else
  #define EvrFsFAT_FileIsNonExistent(drive)
#endif

/**
  \brief  Event on FAT path processing (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      current path address
 */
#ifdef EvtFsFAT_PathProcessing
  __STATIC_INLINE void EvrFsFAT_PathProcessing (uint32_t drive, const char *path) {
    EventRecord2 (EvtFsFAT_PathProcessing, drive, (uint32_t)path);
  }
#else
  #define EvrFsFAT_PathProcessing(drive, path)
#endif

/**
  \brief  Event on FAT file handle not available (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_FileHandleUnavailable
  __STATIC_INLINE void EvrFsFAT_FileHandleUnavailable (uint32_t drive) {
    EventRecord2 (EvtFsFAT_FileHandleUnavailable, drive, 0);
  }
#else
  #define EvrFsFAT_FileHandleUnavailable(drive)
#endif

/**
  \brief  Event on invalid FAT file handle number (Error)
  \param[in]  h         FAT file handle index
 */
#ifdef EvtFsFAT_FileHandleInvalid
  __STATIC_INLINE void EvrFsFAT_FileHandleInvalid (int32_t h) {
    EventRecord2 (EvtFsFAT_FileHandleInvalid, (uint32_t)h, 0);
  }
#else
  #define EvrFsFAT_FileHandleInvalid(h)
#endif

/**
  \brief  Event on invalid character found in FAT label (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_LabelInvalid
  __STATIC_INLINE void EvrFsFAT_LabelInvalid (uint32_t drive) {
    EventRecord2 (EvtFsFAT_LabelInvalid, drive, 0);
  }
#else
  #define EvrFsFAT_LabelInvalid(drive)
#endif

/**
  \brief  Event on nonexisting FAT volume label (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_LabelNotSet
  __STATIC_INLINE void EvrFsFAT_LabelNotSet (uint32_t drive) {
    EventRecord2 (EvtFsFAT_LabelNotSet, drive, 0);
  }
#else
  #define EvrFsFAT_LabelNotSet(drive)
#endif

/**
  \brief  Event on sector read error (Error)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sector    sector number
  \param[in]  count     number of sectors to read
 */
#ifdef EvtFsFAT_SectorReadFailed
  __STATIC_INLINE void EvrFsFAT_SectorReadFailed (uint32_t drive, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsFAT_SectorReadFailed, drive, sector, count, 0);
  }
#else
  #define EvrFsFAT_SectorReadFailed(drive, sector, count)
#endif

/**
  \brief  Event on sector write error (Error)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sector    sector number
  \param[in]  count     number of sectors to write
 */
#ifdef EvtFsFAT_SectorWriteFailed
  __STATIC_INLINE void EvrFsFAT_SectorWriteFailed (uint32_t drive, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsFAT_SectorWriteFailed, drive, sector, count, 0);
  }
#else
  #define EvrFsFAT_SectorWriteFailed(drive, sector, count)
#endif

/**
  \brief  Event on out of free space error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_DiskFull
  __STATIC_INLINE void EvrFsFAT_DiskFull (uint32_t drive) {
    EventRecord2 (EvtFsFAT_DiskFull, drive, 0);
  }
#else
  #define EvrFsFAT_DiskFull(drive)
#endif

/**
  \brief  Event on directory entry allocation error (Error)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  dir_clus  first cluster of directory
 */
#ifdef EvtFsFAT_DirEntryAllocFailed
  __STATIC_INLINE void EvrFsFAT_DirEntryAllocFailed (uint32_t drive, uint32_t dir_clus) {
    EventRecord2 (EvtFsFAT_DirEntryAllocFailed, drive, dir_clus);
  }
#else
  #define EvrFsFAT_DirEntryAllocFailed(drive, dir_clus)
#endif

/**
  \brief  Event on directory/file path buffer size error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_PathBufferToSmall
  __STATIC_INLINE void EvrFsFAT_PathBufferToSmall (uint32_t drive) {
    EventRecord2 (EvtFsFAT_PathBufferToSmall, drive, 0);
  }
#else
  #define EvrFsFAT_PathBufferToSmall(drive)
#endif

/**
  \brief  Event on FAT journal initialization start (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InitJournal
  __STATIC_INLINE void EvrFsFAT_InitJournal (uint32_t drive) {
    EventRecord2 (EvtFsFAT_InitJournal, drive, 0);
  }
#else
  #define EvrFsFAT_InitJournal(drive)
#endif

/**
  \brief  Event on successful FAT journal initialization (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InitJournalSuccess
  __STATIC_INLINE void EvrFsFAT_InitJournalSuccess (uint32_t drive) {
    EventRecord2 (EvtFsFAT_InitJournalSuccess, drive, 0);
  }
#else
  #define EvrFsFAT_InitJournalSuccess(drive)
#endif

/**
  \brief  Event on invalid FAT formatting to complete journal initialization (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InitJournalFATError
  __STATIC_INLINE void EvrFsFAT_InitJournalFATError (uint32_t drive) {
    EventRecord2 (EvtFsFAT_InitJournalFATError, drive, 0);
  }
#else
  #define EvrFsFAT_InitJournalFATError(drive)
#endif

/**
  \brief  Event on FAT file close (Op)
  \param[in]  h         FAT file handle index
 */
#ifdef EvtFsFAT_FileClose
  __STATIC_INLINE void EvrFsFAT_FileClose (int32_t h) {
    EventRecord2 (EvtFsFAT_FileClose, (uint32_t)h, 0);
  }
#else
  #define EvrFsFAT_FileClose(h)
#endif

/**
  \brief  Event on FAT file handle error (Error)
  \param[in]  h         FAT file handle index
  \param[in]  flags     file handle flags
 */
#ifdef EvtFsFAT_FileHandleError
  __STATIC_INLINE void EvrFsFAT_FileHandleError (int32_t h, uint32_t flags) {
    EventRecord2 (EvtFsFAT_FileHandleError, (uint32_t)h, flags);
  }
#else
  #define EvrFsFAT_FileHandleError(h, flags)
#endif

/**
  \brief  Event on FAT file read (Op)
  \param[in]  h         FAT file handle index
  \param[in]  buf       data buffer
  \param[in]  len       number of bytes to read
 */
#ifdef EvtFsFAT_FileRead
  __STATIC_INLINE void EvrFsFAT_FileRead (int32_t h, uint8_t *buf, uint32_t len) {
    EventRecord4 (EvtFsFAT_FileRead, (uint32_t)h, (uint32_t)buf, len, 0);
  }
#else
  #define EvrFsFAT_FileRead(h, buf, len)
#endif

/**
  \brief  Event on FAT file write (Op)
  \param[in]  h         FAT file handle index
  \param[in]  buf       data buffer
  \param[in]  len       number of bytes to write
 */
#ifdef EvtFsFAT_FileWrite
  __STATIC_INLINE void EvrFsFAT_FileWrite (int32_t h, const uint8_t *buf, uint32_t len) {
    EventRecord4 (EvtFsFAT_FileWrite, (uint32_t)h, (uint32_t)buf, len, 0);
  }
#else
  #define EvrFsFAT_FileWrite(h, buf, len)
#endif

/**
  \brief  Event on FAT file flush (Op)
  \param[in]  h         FAT file handle index
 */
#ifdef EvtFsFAT_FileFlush
  __STATIC_INLINE void EvrFsFAT_FileFlush (int32_t h) {
    EventRecord2 (EvtFsFAT_FileFlush, (uint32_t)h, 0);
  }
#else
  #define EvrFsFAT_FileFlush(h)
#endif

/**
  \brief  Event on FAT file seek (Op)
  \param[in]  h         FAT file handle index
  \param[in]  offset    the number of bytes to move
  \param[in]  whence    file position location
 */
#ifdef EvtFsFAT_FileSeek
  __STATIC_INLINE void EvrFsFAT_FileSeek (int32_t h, int64_t offset, int32_t whence) {
    EventRecord4 (EvtFsFAT_FileSeek, (uint32_t)h, (uint32_t)offset, (uint32_t)(offset>>32), (uint32_t)whence);
  }
#else
  #define EvrFsFAT_FileSeek(h, offset, whence)
#endif

/**
  \brief  Event on FAT file increase using \ref fseek (Op)
  \param[in]  h         FAT file handle index
  \param[in]  csize     current file size
  \param[in]  nsize     new file size
 */
#ifdef EvtFsFAT_FileSeekIncrease
  __STATIC_INLINE void EvrFsFAT_FileSeekIncrease (int32_t h, uint32_t csize, uint32_t nsize) {
    EventRecord4 (EvtFsFAT_FileSeekIncrease, (uint32_t)h, csize, nsize, 0);
  }
#else
  #define EvrFsFAT_FileSeekIncrease(h, csize, nsize)
#endif

/**
  \brief  Event on FAT file delete (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      file path buffer address
 */
#ifdef EvtFsFAT_FileDelete
  __STATIC_INLINE void EvrFsFAT_FileDelete (uint32_t drive, const char *path) {
    EventRecord2 (EvtFsFAT_FileDelete, drive, (uint32_t)path);
  }
#else
  #define EvrFsFAT_FileDelete(drive, path)
#endif

/**
  \brief  Event on successful FAT file delete operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      file path buffer address
 */
#ifdef EvtFsFAT_FileDeleteSuccess
  __STATIC_INLINE void EvrFsFAT_FileDeleteSuccess (uint32_t drive, const char *path) {
    EventRecord2 (EvtFsFAT_FileDeleteSuccess, drive, (uint32_t)path);
  }
#else
  #define EvrFsFAT_FileDeleteSuccess(drive, path)
#endif

/**
  \brief  Event on FAT file rename (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      file path buffer address
  \param[in]  newname   new file name buffer address
 */
#ifdef EvtFsFAT_FileRename
  __STATIC_INLINE void EvrFsFAT_FileRename (uint32_t drive, const char *path, const char *newname) {
    EventRecord4 (EvtFsFAT_FileRename, drive, (uint32_t)path, (uint32_t)newname, 0);
  }
#else
  #define EvrFsFAT_FileRename(drive, path, newname)
#endif

/**
  \brief  Event on successful FAT file rename operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      file path buffer address
  \param[in]  newname   new file name buffer address
 */
#ifdef EvtFsFAT_FileRenameSuccess
  __STATIC_INLINE void EvrFsFAT_FileRenameSuccess (uint32_t drive, const char *path, const char *newname) {
    EventRecord4 (EvtFsFAT_FileRenameSuccess, drive, (uint32_t)path, (uint32_t)newname, 0);
  }
#else
  #define EvrFsFAT_FileRenameSuccess(drive, path, newname)
#endif

/**
  \brief  Event on FAT directory create (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      directory path buffer address
 */
#ifdef EvtFsFAT_DirCreate
  __STATIC_INLINE void EvrFsFAT_DirCreate (uint32_t drive, const char *path) {
    EventRecord2 (EvtFsFAT_DirCreate, drive, (uint32_t)path);
  }
#else
  #define EvrFsFAT_DirCreate(drive, path)
#endif

/**
  \brief  Event on successful FAT directory create operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      file path buffer address
 */
#ifdef EvtFsFAT_DirCreateSuccess
  __STATIC_INLINE void EvrFsFAT_DirCreateSuccess (uint32_t drive, const char *path) {
    EventRecord2 (EvtFsFAT_DirCreateSuccess, drive, (uint32_t)path);
  }
#else
  #define EvrFsFAT_DirCreateSuccess(drive, path)
#endif

/**
  \brief  Event on FAT directory remove (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      directory path buffer address
  \param[in]  options   remove options buffer address
 */
#ifdef EvtFsFAT_DirRemove
  __STATIC_INLINE void EvrFsFAT_DirRemove (uint32_t drive, const char *path, const char *options) {
    EventRecord4 (EvtFsFAT_DirRemove, drive, (uint32_t)path, (uint32_t)options, 0);
  }
#else
  #define EvrFsFAT_DirRemove(drive, path, options)
#endif

/**
  \brief  Event on successful FAT directory remove operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      file path buffer address
  \param[in]  options   remove options buffer address
 */
#ifdef EvtFsFAT_DirRemoveSuccess
  __STATIC_INLINE void EvrFsFAT_DirRemoveSuccess (uint32_t drive, const char *path, const char *options) {
    EventRecord4 (EvtFsFAT_DirRemoveSuccess, drive, (uint32_t)path, (uint32_t)options, 0);
  }
#else
  #define EvrFsFAT_DirRemoveSuccess(drive, path, options)
#endif

/**
  \brief  Event on FAT current directory change (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      directory path buffer address
 */
#ifdef EvtFsFAT_ChDir
  __STATIC_INLINE void EvrFsFAT_ChDir (uint32_t drive, const char *path) {
    EventRecord2 (EvtFsFAT_ChDir, drive, (uint32_t)path);
  }
#else
  #define EvrFsFAT_ChDir(drive, path)
#endif

/**
  \brief  Event on successful FAT directory remove operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      file path buffer address
 */
#ifdef EvtFsFAT_ChDirSuccess
  __STATIC_INLINE void EvrFsFAT_ChDirSuccess (uint32_t drive, const char *path) {
    EventRecord2 (EvtFsFAT_ChDirSuccess, drive, (uint32_t)path);
  }
#else
  #define EvrFsFAT_ChDirSuccess(drive, path)
#endif

/**
  \brief  Event on FAT current directory print (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      directory path buffer address
  \param[in]  len       buffer length
 */
#ifdef EvtFsFAT_Pwd
  __STATIC_INLINE void EvrFsFAT_Pwd (uint32_t drive, const char *path, uint32_t len) {
    EventRecord4 (EvtFsFAT_Pwd, drive, (uint32_t)path, len, 0);
  }
#else
  #define EvrFsFAT_Pwd(drive, path, len)
#endif

/**
  \brief  Event on successful FAT current directory print (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      directory path buffer address
  \param[in]  len       buffer length
 */
#ifdef EvtFsFAT_PwdSuccess
  __STATIC_INLINE void EvrFsFAT_PwdSuccess (uint32_t drive, const char *path, uint32_t len) {
    EventRecord4 (EvtFsFAT_PwdSuccess, drive, (uint32_t)path, len, 0);
  }
#else
  #define EvrFsFAT_PwdSuccess(drive, path, len)
#endif

/**
  \brief  Event on FAT attribute setting (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      path buffer address
  \param[in]  attrib    attribute bitmask
 */
#ifdef EvtFsFAT_AttribSet
  __STATIC_INLINE void EvrFsFAT_AttribSet (uint32_t drive, const char *path, uint32_t attrib) {
    EventRecord4 (EvtFsFAT_AttribSet, drive, (uint32_t)path, attrib, 0);
  }
#else
  #define EvrFsFAT_AttribSet(drive, path, len)
#endif

/**
  \brief  Event on successful FAT attribute setting (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      path buffer address
  \param[in]  attrib    attribute bitmask
 */
#ifdef EvtFsFAT_AttribSetSuccess
  __STATIC_INLINE void EvrFsFAT_AttribSetSuccess (uint32_t drive, const char *path, uint32_t attrib) {
    EventRecord4 (EvtFsFAT_AttribSetSuccess, drive, (uint32_t)path, attrib, 0);
  }
#else
  #define EvrFsFAT_AttribSetSuccess(drive, path, len)
#endif

/**
  \brief  Event on FAT free space retrieve request (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_GetFreeSpace
  __STATIC_INLINE void EvrFsFAT_GetFreeSpace (uint32_t drive) {
    EventRecord2 (EvtFsFAT_GetFreeSpace, drive, 0);
  }
#else
  #define EvrFsFAT_GetFreeSpace(drive)
#endif

/**
  \brief  Event on FAT free space retrieval (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  clus      number of free clusters
  \param[in]  size      cluster size
 */
#ifdef EvtFsFAT_FreeAmount
  __STATIC_INLINE void EvrFsFAT_FreeAmount (uint32_t drive, uint32_t clus, uint32_t size) {
    EventRecord4 (EvtFsFAT_FreeAmount, drive, clus, size, drive);
  }
#else
  #define EvrFsFAT_FreeAmount(clus, size, drive)
#endif

/**
  \brief  Event on write to FAT file opened for read (Error)
  \param[in]  h         FAT file handle index
 */
#ifdef EvtFsFAT_FileModeRead
  __STATIC_INLINE void EvrFsFAT_FileModeRead (int32_t h) {
    EventRecord2 (EvtFsFAT_FileModeRead, (uint32_t)h, 0);
  }
#else
  #define EvrFsFAT_FileModeRead(h)
#endif

/**
  \brief  Event on FAT file find operation (Op)
  \param[in]  h         FAT file handle index
 */
#ifdef EvtFsFAT_FileFind
  __STATIC_INLINE void EvrFsFAT_FileFind (int32_t h) {
    EventRecord2 (EvtFsFAT_FileFind, (uint32_t)h, 0);
  }
#else
  #define EvrFsFAT_FileFind(h)
#endif

/**
  \brief  Event on FAT invalid function parameter error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_InvalidParameter
  __STATIC_INLINE void EvrFsFAT_InvalidParameter (uint32_t drive) {
    EventRecord2 (EvtFsFAT_InvalidParameter, drive, 0);
  }
#else
  #define EvrFsFAT_InvalidParameter(drive)
#endif

/**
  \brief  Event on EFS when media driver is not initialized (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsFAT_DriverNotInitialized
  __STATIC_INLINE void EvrFsFAT_DriverNotInitialized (uint32_t drive) {
    EventRecord2 (EvtFsFAT_DriverNotInitialized, drive, 0);
  }
#else
  #define EvrFsFAT_DriverNotInitialized(drive)
#endif

/**
  \brief  Event on FAT path specification (Detail)
  \param[in]  buf       path string buffer
  \param[in]  len       path string length
 */
#ifdef EvtFsFAT_PathName
  __STATIC_INLINE void EvrFsFAT_PathName (const char *buf, uint32_t len) {
    EventRecordData (EvtFsFAT_PathName, (const uint8_t *)buf, len);
  }
#else
  #define EvrFsFAT_PathName(buf, len)
#endif

/**
  \brief  Event on FAT options specification (Detail)
  \param[in]  buf       options string buffer
  \param[in]  len       options string length
 */
#ifdef EvtFsFAT_OptionsString
  __STATIC_INLINE void EvrFsFAT_OptionsString (const char *buf, uint32_t len) {
    EventRecordData (EvtFsFAT_OptionsString, (const uint8_t *)buf, len);
  }
#else
  #define EvrFsFAT_OptionsString(buf, len)
#endif

/**
  \brief  Event on FAT volume label string output (Detail)
  \param[in]  buf       label string buffer
  \param[in]  len       label string length
 */
#ifdef EvtFsFAT_LabelString
  __STATIC_INLINE void EvrFsFAT_LabelString (const char *buf, uint32_t len) {
    EventRecordData (EvtFsFAT_LabelString, (const uint8_t *)buf, len);
  }
#else
  #define EvrFsFAT_LabelString(buf, len)
#endif

/**
  \brief  Event on FAT name cache hit (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  name      name buffer pointer
  \param[in]  len       name length
 */
#ifdef EvtFsFAT_NameCacheHit
  __STATIC_INLINE void EvrFsFAT_NameCacheHit (uint32_t drive, const char *name, uint32_t len) {
    EventRecord4 (EvtFsFAT_NameCacheHit, drive, (uint32_t)name, len, 0);
  }
#else
  #define EvrFsFAT_NameCacheHit(drive, name, len)
#endif

/**
  \brief  Event on FAT name cache miss (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  name      name buffer pointer
  \param[in]  len       name length
 */
#ifdef EvtFsFAT_NameCacheMiss
  __STATIC_INLINE void EvrFsFAT_NameCacheMiss (uint32_t drive, const char *name, uint32_t len) {
    EventRecord4 (EvtFsFAT_NameCacheMiss, drive, (uint32_t)name, len, 0);
  }
#else
  #define EvrFsFAT_NameCacheMiss(drive, name, len)
#endif

/**
  \brief  Event on found file name entry in FAT name cache (Detail)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  clus      name entry cluster number
  \param[in]  offs      name entry offset
  \param[in]  cnt       number of name entries
 */
#ifdef EvtFsFAT_NameCacheEntryFound
  __STATIC_INLINE void EvrFsFAT_NameCacheEntryFound (uint32_t drive, uint32_t clus, uint32_t offs, uint32_t cnt) {
    EventRecord4 (EvtFsFAT_NameCacheEntryFound, drive, clus, offs, cnt);
  }
#else
  #define EvrFsFAT_NameCacheEntryFound(drive, clus, offs, cnt)
#endif

/**
  \brief  Event on FAT name cache entry insert (Detail)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  clus      name entry cluster number
  \param[in]  offs      name entry offset
  \param[in]  cnt       number of name entries
 */
#ifdef EvtFsFAT_NameCacheEntryInsert
  __STATIC_INLINE void EvrFsFAT_NameCacheEntryInsert (uint32_t drive, uint32_t clus, uint32_t offs, uint32_t cnt) {
    EventRecord4 (EvtFsFAT_NameCacheEntryInsert, drive, clus, offs, cnt);
  }
#else
  #define EvrFsFAT_NameCacheEntryInsert(drive, clus, offs, cnt)
#endif

/**
  \brief  Event on FAT name cache entry delete (Detail)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  clus      name entry cluster number
  \param[in]  offs      name entry offset
  \param[in]  cnt       number of name entries
 */
#ifdef EvtFsFAT_NameCacheEntryDelete
  __STATIC_INLINE void EvrFsFAT_NameCacheEntryDelete (uint32_t drive, uint32_t clus, uint32_t offs, uint32_t cnt) {
    EventRecord4 (EvtFsFAT_NameCacheEntryDelete, drive, clus, offs, cnt);
  }
#else
  #define EvrFsFAT_NameCacheEntryDelete(drive, clus, offs, cnt)
#endif

/**
  \brief  Event on FAT timestamp set (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      file path buffer address
 */
#ifdef EvtFsFAT_TimeSet
  __STATIC_INLINE void EvrFsFAT_TimeSet (uint32_t drive, const char *path) {
    EventRecord2 (EvtFsFAT_TimeSet, drive, (uint32_t)path);
  }
#else
  #define EvrFsFAT_TimeSet(drive, path)
#endif

/**
  \brief  Event on FAT timestamp get (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      file path buffer address
 */
#ifdef EvtFsFAT_TimeGet
  __STATIC_INLINE void EvrFsFAT_TimeGet (uint32_t drive, const char *path) {
    EventRecord2 (EvtFsFAT_TimeGet, drive, (uint32_t)path);
  }
#else
  #define EvrFsFAT_TimeGet(drive, path)
#endif

/**
  \brief  Event on FAT timestamp data set/get (Detail)
  \param[in]  create    pointer to fsTime structure containing create timestamp data
  \param[in]  access    pointer to fsTime structure containing last access timestamp data
  \param[in]  write     pointer to fsTime structure containing last write timestamp data
 */
#ifdef EvtFsFAT_TimeData
  __STATIC_INLINE void EvrFsFAT_TimeData (fsTime *create, fsTime *access, fsTime *write) {
    int32_t v1, v2, v3;
    fsTime  *t;

    if (create != NULL) {
      t  = create;
      v1 = 0;
      v2 = (t->sec  << 16) | (t->min << 8) | (t->hr);
      v3 = (t->year << 16) | (t->mon << 8) | (t->day);
      EventRecord4 (EvtFsFAT_TimeData, (uint32_t)v1, (uint32_t)v2, (uint32_t)v3, 0);
    }

    if (access != NULL) {
      t  = access;
      v1 = 1;
      v2 = (t->sec  << 16) | (t->min << 8) | (t->hr);
      v3 = (t->year << 16) | (t->mon << 8) | (t->day);
      EventRecord4 (EvtFsFAT_TimeData, (uint32_t)v1, (uint32_t)v2, (uint32_t)v3, 0);
    }

    if (write != NULL) {
      t  = write;
      v1 = 2;
      v2 = (t->sec  << 16) | (t->min << 8) | (t->hr);
      v3 = (t->year << 16) | (t->mon << 8) | (t->day);
      EventRecord4 (EvtFsFAT_TimeData, (uint32_t)v1, (uint32_t)v2, (uint32_t)v3, 0);
    }
  }
#else
  #define EvrFsFAT_TimeData(create, access, write)
#endif

/**
  \brief  Event on EFS drive initialization (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_InitDrive
  __STATIC_INLINE void EvrFsEFS_InitDrive (uint32_t drive) {
    EventRecord2 (EvtFsEFS_InitDrive, drive, 0);
  }
#else
  #define EvrFsEFS_InitDrive(drive)
#endif

/**
  \brief  Event on EFS driver initialization (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  driver    CMSIS Flash driver control block address
 */
#ifdef EvtFsEFS_InitDriver
  __STATIC_INLINE void EvrFsEFS_InitDriver (uint32_t drive, uint32_t driver) {
    EventRecord2 (EvtFsEFS_InitDriver, drive, driver);
  }
#else
  #define EvrFsEFS_InitDriver(drive, driver)
#endif

/**
  \brief  Event on EFS driver initialization error (Error)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  driver    CMSIS Flash driver control block address
 */
#ifdef EvtFsEFS_InitDriverFailed
  __STATIC_INLINE void EvrFsEFS_InitDriverFailed (uint32_t drive, uint32_t driver) {
    EventRecord2 (EvtFsEFS_InitDriverFailed, drive, driver);
  }
#else
  #define EvrFsEFS_InitDriverFailed(drive, driver)
#endif

/**
  \brief  Event on EFS drive uninitialization (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_UninitDrive
  __STATIC_INLINE void EvrFsEFS_UninitDrive (uint32_t drive) {
    EventRecord2 (EvtFsEFS_UninitDrive, drive, 0);
  }
#else
  #define EvrFsEFS_UninitDrive(drive)
#endif

/**
  \brief  Event on EFS driver uninitialization (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  driver    CMSIS Flash driver control block address
 */
#ifdef EvtFsEFS_UninitDriver
  __STATIC_INLINE void EvrFsEFS_UninitDriver (uint32_t drive, uint32_t driver) {
    EventRecord2 (EvtFsEFS_UninitDrive, drive, driver);
  }
#else
  #define EvrFsEFS_UninitDriver(drive, driver)
#endif

/**
  \brief  Event on EFS drive mount (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_MountDrive
  __STATIC_INLINE void EvrFsEFS_MountDrive (uint32_t drive) {
    EventRecord2 (EvtFsEFS_MountDrive, drive, 0);
  }
#else
  #define EvrFsEFS_MountDrive(drive)
#endif

/**
  \brief  Event on successful EFS drive mount (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_MountDriveSuccess
  __STATIC_INLINE void EvrFsEFS_MountDriveSuccess (uint32_t drive) {
    EventRecord2 (EvtFsEFS_MountDriveSuccess, drive, 0);
  }
#else
  #define EvrFsEFS_MountDriveSuccess(drive)
#endif

/**
  \brief  Event on Flash device info retrieve operation (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_FlashGetInfo
  __STATIC_INLINE void EvrFsEFS_FlashGetInfo (uint32_t drive) {
    EventRecord2 (EvtFsEFS_FlashGetInfo, drive, 0);
  }
#else
  #define EvrFsEFS_FlashGetInfo(drive)
#endif

/**
  \brief  Event on Flash program unit too big (Error)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  current   current program unit in bytes
  \param[in]  required  required program unit in bytes
 */
#ifdef EvtFsEFS_FlashProgUnitTooBig
  __STATIC_INLINE void EvrFsEFS_FlashProgUnitTooBig (uint32_t drive, uint32_t current, uint32_t required) {
    EventRecord4 (EvtFsEFS_FlashProgUnitTooBig, drive, current, required, 0);
  }
#else
  #define EvrFsEFS_FlashProgUnitTooBig(drive, current, required)
#endif

/**
  \brief  Event on retrieved capacity of the Flash device (Error)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  capacity  flash device capacity in bytes
 */
#ifdef EvtFsEFS_FlashCapacity
  __STATIC_INLINE void EvrFsEFS_FlashCapacity (uint32_t drive, uint32_t capacity) {
    EventRecord2 (EvtFsEFS_FlashCapacity, drive, capacity);
  }
#else
  #define EvrFsEFS_FlashCapacity(drive, capacity)
#endif

/**
  \brief  Event on EFS drive unmount (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_UnmountDrive
  __STATIC_INLINE void EvrFsEFS_UnmountDrive (uint32_t drive) {
    EventRecord2 (EvtFsEFS_UnmountDrive, drive, 0);
  }
#else
  #define EvrFsEFS_UnmountDrive(drive)
#endif

/**
  \brief  Event on successful EFS drive unmount operation (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_UnmountDriveSuccess
  __STATIC_INLINE void EvrFsEFS_UnmountDriveSuccess (uint32_t drive) {
    EventRecord2 (EvtFsEFS_UnmountDriveSuccess, drive, 0);
  }
#else
  #define EvrFsEFS_UnmountDriveSuccess(drive)
#endif

/**
  \brief  Event on EFS drive format (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_FormatDrive
  __STATIC_INLINE void EvrFsEFS_FormatDrive (uint32_t drive) {
    EventRecord2 (EvtFsEFS_FormatDrive, drive, 0);
  }
#else
  #define EvrFsEFS_FormatDrive(drive)
#endif

/**
  \brief  Event on successful EFS drive format operation (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_FormatDriveSuccess
  __STATIC_INLINE void EvrFsEFS_FormatDriveSuccess (uint32_t drive) {
    EventRecord2 (EvtFsEFS_FormatDriveSuccess, drive, 0);
  }
#else
  #define EvrFsEFS_FormatDriveSuccess(drive)
#endif

/**
  \brief  Event on Flash chip erase operation (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_FlashEraseChip
  __STATIC_INLINE void EvrFsEFS_FlashEraseChip (uint32_t drive) {
    EventRecord2 (EvtFsEFS_FlashEraseChip, drive, 0);
  }
#else
  #define EvrFsEFS_FlashEraseChip(drive)
#endif

/**
  \brief  Event on Flash chip erase operation error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_FlashEraseChipFailed
  __STATIC_INLINE void EvrFsEFS_FlashEraseChipFailed (uint32_t drive) {
    EventRecord2 (EvtFsEFS_FlashEraseChipFailed, drive, 0);
  }
#else
  #define EvrFsEFS_FlashEraseChipFailed(drive)
#endif

/**
  \brief  Event on Flash chip erase operation timeout (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_FlashEraseChipTimeout
  __STATIC_INLINE void EvrFsEFS_FlashEraseChipTimeout (uint32_t drive) {
    EventRecord2 (EvtFsEFS_FlashEraseChipTimeout, drive, 0);
  }
#else
  #define EvrFsEFS_FlashEraseChipTimeout(drive)
#endif

/**
  \brief  Event on Flash sectors erase operation (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  sector    first sector to erase
  \param[in]  cnt       number of sectors to erase
 */
#ifdef EvtFsEFS_FlashEraseSectors
  __STATIC_INLINE void EvrFsEFS_FlashEraseSectors (uint32_t drive, uint32_t sector, uint32_t cnt) {
    EventRecord4 (EvtFsEFS_FlashEraseSectors, drive, sector, cnt, 0);
  }
#else
  #define EvrFsEFS_FlashEraseSectors(drive, sector, cnt)
#endif

/**
  \brief  Event on EFS file open (Op)
  \param[in]  h         EFS file handle index
  \param[in]  path      file path buffer memory location
  \param[in]  openmode  open mode
 */
#ifdef EvtFsEFS_FileOpen
  __STATIC_INLINE void EvrFsEFS_FileOpen (int32_t h, const char *path, int32_t openmode) {
    EventRecord4 (EvtFsEFS_FileOpen, (uint32_t)h, (uint32_t)path, (uint32_t)openmode, 0);
  }
#else
  #define EvrFsEFS_FileOpen(h, path, openmode)
#endif

/**
  \brief  Event on invalid EFS filename specification (Error)
  \param[in]  name      file name buffer
  \param[in]  len       file name length
 */
#ifdef EvtFsEFS_FileNameInvalid
  __STATIC_INLINE void EvrFsEFS_FileNameInvalid (const char *name, uint32_t len) {
    EventRecordData (EvtFsEFS_FileNameInvalid, (const uint8_t *)name, len);
  }
#else
  #define EvrFsEFS_FileNameInvalid(name, len)
#endif

/**
  \brief  Event on unsupported EFS file open mode specification (Error)
  \param[in]  h         EFS file handle index
  \param[in]  openmode  open mode
 */
#ifdef EvtFsEFS_OpenModeUnsupported
  __STATIC_INLINE void EvrFsEFS_OpenModeUnsupported (int32_t h, int32_t openmode) {
    EventRecord2 (EvtFsEFS_OpenModeUnsupported, (uint32_t)h, (uint32_t)openmode);
  }
#else
  #define EvrFsEFS_OpenModeUnsupported(h, openmode)
#endif

/**
  \brief  Event on EFS file close (Op)
  \param[in]  h         EFS file handle index
 */
#ifdef EvtFsEFS_FileClose
  __STATIC_INLINE void EvrFsEFS_FileClose (int32_t h) {
    EventRecord2 (EvtFsEFS_FileClose, (uint32_t)h, 0);
  }
#else
  #define EvrFsEFS_FileClose(h)
#endif

/**
  \brief  Event on EFS file close (Op)
  \param[in]  h         EFS file handle index
  \param[in]  flags     file handle flags
 */
#ifdef EvtFsEFS_FileNotOpened
  __STATIC_INLINE void EvrFsEFS_FileNotOpened (int32_t h, uint32_t flags) {
    EventRecord2 (EvtFsEFS_FileNotOpened, (uint32_t)h, flags);
  }
#else
  #define EvrFsEFS_FileNotOpened(h, flags)
#endif

/**
  \brief  Event on EFS file read (Op)
  \param[in]  h         EFS file handle index
  \param[in]  buf       data buffer
  \param[in]  len       number of bytes to read
 */
#ifdef EvtFsEFS_FileRead
  __STATIC_INLINE void EvrFsEFS_FileRead (int32_t h, uint8_t *buf, uint32_t len) {
    EventRecord4 (EvtFsEFS_FileRead, (uint32_t)h, (uint32_t)buf, len, 0);
  }
#else
  #define EvrFsEFS_FileRead(h, buf, len)
#endif

/**
  \brief  Event on EFS when reading from write only file (Error)
  \param[in]  h         EFS file handle index
 */
#ifdef EvtFsEFS_FileOpenWriteMode
  __STATIC_INLINE void EvrFsEFS_FileOpenWriteMode (int32_t h) {
    EventRecord2 (EvtFsEFS_FileOpenWriteMode, (uint32_t)h, 0);
  }
#else
  #define EvrFsEFS_FileOpenWriteMode(h)
#endif

/**
  \brief  Event on EFS file write (Op)
  \param[in]  h         EFS file handle index
  \param[in]  buf       data buffer
  \param[in]  len       number of bytes to write
 */
#ifdef EvtFsEFS_FileWrite
  __STATIC_INLINE void EvrFsEFS_FileWrite (int32_t h, const uint8_t *buf, uint32_t len) {
    EventRecord4 (EvtFsEFS_FileWrite, (uint32_t)h, (uint32_t)buf, len, 0);
  }
#else
  #define EvrFsEFS_FileWrite(h, buf, len)
#endif

/**
  \brief  Event on EFS when writing to read only file (Error)
  \param[in]  h         EFS file handle index
 */
#ifdef EvtFsEFS_FileOpenReadMode
  __STATIC_INLINE void EvrFsEFS_FileOpenReadMode (int32_t h) {
    EventRecord2 (EvtFsEFS_FileOpenReadMode, (uint32_t)h, 0);
  }
#else
  #define EvrFsEFS_FileOpenReadMode(h)
#endif

/**
  \brief  Event on EFS file flush (Op)
  \param[in]  h         EFS file handle index
 */
#ifdef EvtFsEFS_FileFlush
  __STATIC_INLINE void EvrFsEFS_FileFlush (int32_t h) {
    EventRecord2 (EvtFsEFS_FileFlush, (uint32_t)h, 0);
  }
#else
  #define EvrFsEFS_FileFlush(h)
#endif

/**
  \brief  Event on invalid EFS file handle open mode (Error)
  \param[in]  h         EFS file handle index
  \param[in]  flags     file handle flags
 */
#ifdef EvtFsEFS_OpenModeInvalid
  __STATIC_INLINE void EvrFsEFS_OpenModeInvalid (int32_t h, uint32_t flags) {
    EventRecord2 (EvtFsEFS_OpenModeInvalid, (uint32_t)h, flags);
  }
#else
  #define EvrFsEFS_OpenModeInvalid(h, flags)
#endif

/**
  \brief  Event on EFS file seek (Op)
  \param[in]  h         EFS file handle index
  \param[in]  offset    the number of bytes to move
  \param[in]  whence    file position location
 */
#ifdef EvtFsEFS_FileSeek
  __STATIC_INLINE void EvrFsEFS_FileSeek (int32_t h, int32_t offset, int32_t whence) {
    EventRecord4 (EvtFsEFS_FileSeek, (uint32_t)h, (uint32_t)offset, (uint32_t)whence, 0U);
  }
#else
  #define EvrFsEFS_FileSeek(h, offset, whence)
#endif

/**
  \brief  Event on EFS \ref fseek "file seek" beyond EOF (Error)
  \param[in]  h         EFS file handle index
  \param[in]  size      file size
  \param[in]  pos       file position
 */
#ifdef EvtFsEFS_FileSeekEOF
  __STATIC_INLINE void EvrFsEFS_FileSeekEOF (int32_t h, uint32_t size, uint32_t pos) {
    EventRecord4 (EvtFsEFS_FileSeekEOF, (uint32_t)h, size, pos, 0);
  }
#else
  #define EvrFsEFS_FileSeekEOF(h, size, pos)
#endif

/**
  \brief  Event on EFS file delete (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      file path buffer address
 */
#ifdef EvtFsEFS_FileDelete
  __STATIC_INLINE void EvrFsEFS_FileDelete (uint32_t drive, const char *path) {
    EventRecord2 (EvtFsEFS_FileDelete, drive, (uint32_t)path);
  }
#else
  #define EvrFsEFS_FileDelete(drive, path)
#endif

/**
  \brief  Event on EFS file rename (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  path      file path buffer address
  \param[in]  newname   new file name buffer address
 */
#ifdef EvtFsEFS_FileRename
  __STATIC_INLINE void EvrFsEFS_FileRename (uint32_t drive, const char *path, const char *newname) {
    EventRecord4 (EvtFsEFS_FileRename, drive, (uint32_t)path, (uint32_t)newname, 0);
  }
#else
  #define EvrFsEFS_FileRename(drive, path, newname)
#endif

/**
  \brief  Event on EFS file not found (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_FileNotFound
  __STATIC_INLINE void EvrFsEFS_FileNotFound (uint32_t drive) {
    EventRecord2 (EvtFsEFS_FileNotFound, drive, 0);
  }
#else
  #define EvrFsEFS_FileNotFound(drive)
#endif

/**
  \brief  Event on EFS file already exists (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_FileAlreadyExists
  __STATIC_INLINE void EvrFsEFS_FileAlreadyExists (uint32_t drive) {
    EventRecord2 (EvtFsEFS_FileAlreadyExists, drive, 0);
  }
#else
  #define EvrFsEFS_FileAlreadyExists(drive)
#endif

/**
  \brief  Event on EFS free space retrieve request (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_FreeSpaceRetrieve
  __STATIC_INLINE void EvrFsEFS_FreeSpaceRetrieve (uint32_t drive) {
    EventRecord2 (EvtFsEFS_FreeSpaceRetrieve, drive, 0);
  }
#else
  #define EvrFsEFS_FreeSpaceRetrieve(drive)
#endif

/**
  \brief  Event on EFS free space retrieval  (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  free      free space amount
 */
#ifdef EvtFsEFS_FreeSpaceAmount
  __STATIC_INLINE void EvrFsEFS_FreeSpaceAmount (uint32_t drive, uint32_t free) {
    EventRecord2 (EvtFsEFS_FreeSpaceAmount, drive, free);
  }
#else
  #define EvrFsEFS_FreeSpaceAmount(drive, free)
#endif

/**
  \brief  Event on EFS drive analyze (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_DriveAnalyze
  __STATIC_INLINE void EvrFsEFS_DriveAnalyze (uint32_t drive) {
    EventRecord2 (EvtFsEFS_DriveAnalyze, drive, 0);
  }
#else
  #define EvrFsEFS_DriveAnalyze(drive)
#endif

/**
  \brief  Event on successful EFS drive analyze (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  factor    defragmentation factor
 */
#ifdef EvtFsEFS_DriveAnalyzeSuccess
  __STATIC_INLINE void EvrFsEFS_DriveAnalyzeSuccess (uint32_t drive, uint32_t factor) {
    EventRecord2 (EvtFsEFS_DriveAnalyzeSuccess, drive, factor);
  }
#else
  #define EvrFsEFS_DriveAnalyzeSuccess(drive, factor)
#endif

/**
  \brief  Event on EFS drive check (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_DriveCheck
  __STATIC_INLINE void EvrFsEFS_DriveCheck (uint32_t drive) {
    EventRecord2 (EvtFsEFS_DriveCheck, drive, 0);
  }
#else
  #define EvrFsEFS_DriveCheck(drive)
#endif

/**
  \brief  Event on EFS drive check completed without errors (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_DriveCheckSuccess
  __STATIC_INLINE void EvrFsEFS_DriveCheckSuccess (uint32_t drive) {
    EventRecord2 (EvtFsEFS_DriveCheckSuccess, drive, 0);
  }
#else
  #define EvrFsEFS_DriveCheckSuccess(drive)
#endif

/**
  \brief  Event on EFS user and allocation area overlap detection (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_DataAreaOverlap
  __STATIC_INLINE void EvrFsEFS_DataAreaOverlap (uint32_t drive) {
    EventRecord2 (EvtFsEFS_DataAreaOverlap, drive, 0);
  }
#else
  #define EvrFsEFS_DataAreaOverlap(drive)
#endif

/**
  \brief  Event on EFS invalid file ID detection (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_FileIdInvalid
  __STATIC_INLINE void EvrFsEFS_FileIdInvalid (uint32_t drive) {
    EventRecord2 (EvtFsEFS_FileIdInvalid, drive, 0);
  }
#else
  #define EvrFsEFS_FileIdInvalid(drive)
#endif

/**
  \brief  Event on EFS allocation order error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_AllocationOrderInvalid
  __STATIC_INLINE void EvrFsEFS_AllocationOrderInvalid (uint32_t drive) {
    EventRecord2 (EvtFsEFS_AllocationOrderInvalid, drive, 0);
  }
#else
  #define EvrFsEFS_AllocationOrderInvalid(drive)
#endif

/**
  \brief  Event on EFS allocation area overlap error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_AllocationAreaOverlap
  __STATIC_INLINE void EvrFsEFS_AllocationAreaOverlap (uint32_t drive) {
    EventRecord2 (EvtFsEFS_AllocationAreaOverlap, drive, 0);
  }
#else
  #define EvrFsEFS_AllocationAreaOverlap(drive)
#endif

/**
  \brief  Event on EFS drive defrag (Op)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_DriveDefrag
  __STATIC_INLINE void EvrFsEFS_DriveDefrag (uint32_t drive) {
    EventRecord2 (EvtFsEFS_DriveDefrag, drive, 0);
  }
#else
  #define EvrFsEFS_DriveDefrag(drive)
#endif

/**
  \brief  Event  (Error)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  h         EFS file handle index
 */
#ifdef EvtFsEFS_FileHandleActive
  __STATIC_INLINE void EvrFsEFS_FileHandleActive (uint32_t drive, uint32_t h) {
    EventRecord2 (EvtFsEFS_FileHandleActive, drive, h);
  }
#else
  #define EvrFsEFS_FileHandleActive(drive, h)
#endif

/**
  \brief  Event on too many open files (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_FileHandleUnavailable
  __STATIC_INLINE void EvrFsEFS_FileHandleUnavailable (uint32_t drive) {
    EventRecord2 (EvtFsEFS_FileHandleUnavailable, drive, 0);
  }
#else
  #define EvrFsEFS_FileHandleUnavailable(drive)
#endif

/**
  \brief  Event on invalid handle (Error)
  \param[in]  h         EFS file handle index
 */
#ifdef EvtFsEFS_FileHandleInvalid
  __STATIC_INLINE void EvrFsEFS_FileHandleInvalid (int32_t h) {
    EventRecord2 (EvtFsEFS_FileHandleInvalid, (uint32_t)h, 0);
  }
#else
  #define EvrFsEFS_FileHandleInvalid(h)
#endif

/**
  \brief  Event on EFS file busy (Error)
  \param[in]  h         EFS file handle index
 */
#ifdef EvtFsEFS_FileInUse
  __STATIC_INLINE void EvrFsEFS_FileInUse (uint32_t h) {
    EventRecord2 (EvtFsEFS_FileInUse, h, 0);
  }
#else
  #define EvrFsEFS_FileInUse(h)
#endif

/**
  \brief  Event on out of free space on EFS drive (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_DiskFull
  __STATIC_INLINE void EvrFsEFS_DiskFull (uint32_t drive) {
    EventRecord2 (EvtFsEFS_DiskFull, drive, 0);
  }
#else
  #define EvrFsEFS_DiskFull(drive)
#endif

/**
  \brief  Event on Flash driver write data error (Error)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  addr      block address
  \param[in]  buf       data buffer address
  \param[in]  cnt       number of bytes to write
 */
#ifdef EvtFsEFS_FlashWriteFailed
  __STATIC_INLINE void EvrFsEFS_FlashWriteFailed (uint32_t drive, uint32_t addr, const void *buf, uint32_t cnt) {
    EventRecord4 (EvtFsEFS_FlashWriteFailed, drive, addr, (uint32_t)buf, cnt);
  }
#else
  #define EvrFsEFS_FlashWriteFailed(drive, addr, buf, cnt)
#endif

/**
  \brief  Event on Flash driver read data error (Error)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  addr      block address
  \param[in]  buf       data buffer address
  \param[in]  cnt       number of bytes to read
 */
#ifdef EvtFsEFS_FlashReadFailed
  __STATIC_INLINE void EvrFsEFS_FlashReadFailed (uint32_t drive, uint32_t addr, void *buf, uint32_t cnt) {
    EventRecord4 (EvtFsEFS_FlashReadFailed, drive, addr, (uint32_t)buf, cnt);
  }
#else
  #define EvrFsEFS_FlashReadFailed(drive, addr, buf, cnt)
#endif

/**
  \brief  Event on Flash driver read data timeout (Error)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  addr      block address
  \param[in]  buf       data buffer address
  \param[in]  cnt       number of bytes to read
 */
#ifdef EvtFsEFS_FlashReadTimeout
  __STATIC_INLINE void EvrFsEFS_FlashReadTimeout (uint32_t drive, uint32_t addr, void *buf, uint32_t cnt) {
    EventRecord4 (EvtFsEFS_FlashReadTimeout, drive, addr, (uint32_t)buf, cnt);
  }
#else
  #define EvrFsEFS_FlashReadTimeout(drive, addr, buf, cnt)
#endif

/**
  \brief  Event on Flash driver read data error (Error)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  block     block number
  \param[in]  addr      block address
 */
#ifdef EvtFsEFS_FlashEraseFailed
  __STATIC_INLINE void EvrFsEFS_FlashEraseFailed (uint32_t drive, uint32_t block, uint32_t addr) {
    EventRecord4 (EvtFsEFS_FlashEraseFailed, drive, block, addr, 0);
  }
#else
  #define EvrFsEFS_FlashEraseFailed(drive, block, addr)
#endif

/**
  \brief  Event on EFS invalid function parameter error (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_InvalidParameter
  __STATIC_INLINE void EvrFsEFS_InvalidParameter (uint32_t drive) {
    EventRecord2 (EvtFsEFS_InvalidParameter, drive, 0);
  }
#else
  #define EvrFsEFS_InvalidParameter(drive)
#endif

/**
  \brief  Event on EFS when using unmounted drive (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_DriveNotMounted
  __STATIC_INLINE void EvrFsEFS_DriveNotMounted (uint32_t drive) {
    EventRecord2 (EvtFsEFS_DriveNotMounted, drive, 0);
  }
#else
  #define EvrFsEFS_DriveNotMounted(drive)
#endif

/**
  \brief  Event on EFS when media driver is not initialized (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsEFS_DriveNotInitialized
  __STATIC_INLINE void EvrFsEFS_DriveNotInitialized (uint32_t drive) {
    EventRecord2 (EvtFsEFS_DriveNotInitialized, drive, 0);
  }
#else
  #define EvrFsEFS_DriveNotInitialized(drive)
#endif

/**
  \brief  Event on EFS file name specification (Detail)
  \param[in]  buf       filename buffer
  \param[in]  len       filename length
 */
#ifdef EvtFsEFS_FileName
  __STATIC_INLINE void EvrFsEFS_FileName (const char *buf, uint32_t len) {
    EventRecordData (EvtFsEFS_FileName, (const uint8_t *)buf, len);
  }
#else
  #define EvrFsEFS_FileName(buf, len)
#endif

/**
  \brief  Event on EFS file allocation info write (Detail)
  \param[in]  addr      memory address
  \param[in]  end       end address of the file fragment
  \param[in]  fileID    file identification number
  \param[in]  index     file fragment ordering number
 */
#ifdef EvtFsEFS_FileAllocWrite
  __STATIC_INLINE void EvrFsEFS_FileAllocWrite (uint32_t addr, uint32_t end, uint32_t fileID, uint32_t index) {
    EventRecord4 (EvtFsEFS_FileAllocWrite, addr, end, fileID, index);
  }
#else
  #define EvrFsEFS_FileAllocWrite(addr, end, fileID, index)
#endif

/**
  \brief  Event on EFS file allocation info read (Detail)
  \param[in]  addr      memory address
  \param[in]  end       end address of the file fragment
  \param[in]  fileID    file identification number
  \param[in]  index     file fragment ordering number
 */
#ifdef EvtFsEFS_FileAllocRead
  __STATIC_INLINE void EvrFsEFS_FileAllocRead (uint32_t addr, uint32_t end, uint32_t fileID, uint32_t index) {
    EventRecord4 (EvtFsEFS_FileAllocRead, addr, end, fileID, index);
  }
#else
  #define EvrFsEFS_FileAllocRead(addr, end, fileID, index)
#endif


/**
  \brief  Event on call of \ref fs_ioc_get_id function (API)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsIOC_GetId
  __STATIC_INLINE void EvrFsIOC_GetId (uint32_t drive) {
    EventRecord2 (EvtFsIOC_GetId, drive, 0);
  }
#else
  #define EvrFsIOC_GetId(drive)
#endif

/**
  \brief  Event on retrieval of drive ID (Op)
  \param[in]  drive     4 byte encoded drive letter
  \param[in]  drv_id    drive ID
 */
#ifdef EvtFsIOC_GetIdSuccess
  __STATIC_INLINE void EvrFsIOC_GetIdSuccess (uint32_t drive, int32_t drv_id) {
    EventRecord2 (EvtFsIOC_GetIdSuccess, drive, (uint32_t)drv_id);
  }
#else
  #define EvrFsIOC_GetIdSuccess(drive, drv_id)
#endif

/**
  \brief  Event on invalid or nonexisting drive specifier (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsIOC_GetIdError
  __STATIC_INLINE void EvrFsIOC_GetIdError (uint32_t drive) {
    EventRecord2 (EvtFsIOC_GetIdError, drive, 0);
  }
#else
  #define EvrFsIOC_GetIdError(drive)
#endif

/**
  \brief  Event on call of \ref fs_ioc_lock function (API)
  \param[in]  drv_id    drive ID
 */
#ifdef EvtFsIOC_Lock
  __STATIC_INLINE void EvrFsIOC_Lock (int32_t drv_id) {
    EventRecord2 (EvtFsIOC_Lock, (uint32_t)drv_id, 0);
  }
#else
  #define EvrFsIOC_Lock(drv_id)
#endif

/**
  \brief  Event on successful drive lock operation (Op)
  \param[in]  drv_id    drive ID
 */
#ifdef EvtFsIOC_LockSuccess
  __STATIC_INLINE void EvrFsIOC_LockSuccess (int32_t drv_id) {
    EventRecord2 (EvtFsIOC_LockSuccess, (uint32_t)drv_id, 0);
  }
#else
  #define EvrFsIOC_LockSuccess(drv_id)
#endif

/**
  \brief  Event on call of \ref fs_ioc_unlock function (API)
  \param[in]  drv_id    drive ID
 */
#ifdef EvtFsIOC_Unlock
  __STATIC_INLINE void EvrFsIOC_Unlock (int32_t drv_id) {
    EventRecord2 (EvtFsIOC_Unlock, (uint32_t)drv_id, 0);
  }
#else
  #define EvrFsIOC_Unlock(drv_id)
#endif

/**
  \brief  Event on successful drive unlock operation (Op)
  \param[in]  drv_id    drive ID
 */
#ifdef EvtFsIOC_UnlockSuccess
  __STATIC_INLINE void EvrFsIOC_UnlockSuccess (int32_t drv_id) {
    EventRecord2 (EvtFsIOC_UnlockSuccess, (uint32_t)drv_id, 0);
  }
#else
  #define EvrFsIOC_UnlockSuccess(drv_id)
#endif

/**
  \brief  Event on call of \ref fs_ioc_get_cache function (API)
  \param[in]  drv_id     drive ID
  \param[in]  cache_info cache information structure address
 */
#ifdef EvtFsIOC_GetCache
  __STATIC_INLINE void EvrFsIOC_GetCache (int32_t drv_id, uint32_t cache_info) {
    EventRecord2 (EvtFsIOC_GetCache, (uint32_t)drv_id, cache_info);
  }
#else
  #define EvrFsIOC_GetCache(drv_id, cache_info)
#endif

/**
  \brief  Event on successful cache buffer information retrieve operation (Op)
  \param[in]  drv_id     drive ID
  \param[in]  buf        cache buffer address
  \param[in]  size       cache buffer size
 */
#ifdef EvtFsIOC_GetCacheSuccess
  __STATIC_INLINE void EvrFsIOC_GetCacheSuccess (int32_t drv_id, uint32_t buf, uint32_t size) {
    EventRecord4 (EvtFsIOC_GetCacheSuccess, (uint32_t)drv_id, buf, size, 0);
  }
#else
  #define EvrFsIOC_GetCacheSuccess(drv_id, buf, size)
#endif

/**
  \brief  Event on call of \ref fs_ioc_read_sector function (API)
  \param[in]  drv_id    drive ID
  \param[in]  sect      sector number
  \param[in]  buf       data buffer address
  \param[in]  cnt       number of sectors
 */
#ifdef EvtFsIOC_ReadSector
  __STATIC_INLINE void EvrFsIOC_ReadSector (int32_t drv_id, uint32_t sect, uint32_t buf, uint32_t cnt) {
    EventRecord4 (EvtFsIOC_ReadSector, (uint32_t)drv_id, sect, buf, cnt);
  }
#else
  #define EvrFsIOC_ReadSector(drv_id, sect, buf, cnt)
#endif

/**
  \brief  Event on successful read sector operation (Op)
  \param[in]  drv_id    drive ID
  \param[in]  sect      sector number
  \param[in]  buf       data buffer address
  \param[in]  cnt       number of sectors
 */
#ifdef EvtFsIOC_ReadSectorSuccess
  __STATIC_INLINE void EvrFsIOC_ReadSectorSuccess (int32_t drv_id, uint32_t sect, uint32_t buf, uint32_t cnt) {
    EventRecord4 (EvtFsIOC_ReadSectorSuccess, (uint32_t)drv_id, sect, buf, cnt);
  }
#else
  #define EvrFsIOC_ReadSectorSuccess(drv_id, sect, buf, cnt)
#endif

/**
  \brief  Event on read sector operation error (Error)
  \param[in]  drv_id    drive ID
  \param[in]  sect      sector number
  \param[in]  buf       data buffer address
  \param[in]  cnt       number of sectors
 */
#ifdef EvtFsIOC_ReadSectorError
  __STATIC_INLINE void EvrFsIOC_ReadSectorError (int32_t drv_id, uint32_t sect, uint32_t buf, uint32_t cnt) {
    EventRecord4 (EvtFsIOC_ReadSectorError, (uint32_t)drv_id, sect, buf, cnt);
  }
#else
  #define EvrFsIOC_ReadSectorError(drv_id, sect, buf, cnt)
#endif

/**
  \brief  Event on call of \ref fs_ioc_write_sector function (API)
  \param[in]  drv_id    drive ID
  \param[in]  sect      sector number
  \param[in]  buf       data buffer address
  \param[in]  cnt       number of sectors
 */
#ifdef EvtFsIOC_WriteSector
  __STATIC_INLINE void EvrFsIOC_WriteSector (int32_t drv_id, uint32_t sect, uint32_t buf, uint32_t cnt) {
    EventRecord4 (EvtFsIOC_WriteSector, (uint32_t)drv_id, sect, buf, cnt);
  }
#else
  #define EvrFsIOC_WriteSector(drv_id, sect, buf, cnt)
#endif

/**
  \brief  Event on successful read sector operation (Op)
  \param[in]  drv_id    drive ID
  \param[in]  sect      sector number
  \param[in]  buf       data buffer address
  \param[in]  cnt       number of sectors
 */
#ifdef EvtFsIOC_WriteSectorSuccess
  __STATIC_INLINE void EvrFsIOC_WriteSectorSuccess (int32_t drv_id, uint32_t sect, uint32_t buf, uint32_t cnt) {
    EventRecord4 (EvtFsIOC_WriteSectorSuccess, (uint32_t)drv_id, sect, buf, cnt);
  }
#else
  #define EvrFsIOC_WriteSectorSuccess(drv_id, sect, buf, cnt)
#endif

/**
  \brief  Event on write sector operation error (Error)
  \param[in]  drv_id    drive ID
  \param[in]  sect      sector number
  \param[in]  buf       data buffer address
  \param[in]  cnt       number of sectors
 */
#ifdef EvtFsIOC_WriteSectorError
  __STATIC_INLINE void EvrFsIOC_WriteSectorError (int32_t drv_id, uint32_t sect, uint32_t buf, uint32_t cnt) {
    EventRecord4 (EvtFsIOC_WriteSectorError, (uint32_t)drv_id, sect, buf, cnt);
  }
#else
  #define EvrFsIOC_WriteSectorError(drv_id, sect, buf, cnt)
#endif

/**
  \brief  Event on call of \ref fs_ioc_read_info function (API)
  \param[in]  drv_id     drive ID
  \param[in]  info       media information structure address
 */
#ifdef EvtFsIOC_ReadInfo
  __STATIC_INLINE void EvrFsIOC_ReadInfo (int32_t drv_id, uint32_t info) {
    EventRecord2 (EvtFsIOC_ReadInfo, (uint32_t)drv_id, info);
  }
#else
  #define EvrFsIOC_ReadInfo(drv_id, info)
#endif

/**
  \brief  Event on successful read info operation (Op)
  \param[in]  drv_id      drive ID
  \param[in]  block_count media capacity in 512 bytes blocks
 */
#ifdef EvtFsIOC_ReadInfoSuccess
  __STATIC_INLINE void EvrFsIOC_ReadInfoSuccess (int32_t drv_id, uint32_t block_count) {
    EventRecord2 (EvtFsIOC_ReadInfoSuccess, (uint32_t)drv_id, block_count);
  }
#else
  #define EvrFsIOC_ReadInfoSuccess(drv_id, block_count)
#endif

/**
  \brief  Event on read info operation error (Error)
  \param[in]  drv_id     drive ID
 */
#ifdef EvtFsIOC_ReadInfoError
  __STATIC_INLINE void EvrFsIOC_ReadInfoError (int32_t drv_id) {
    EventRecord2 (EvtFsIOC_ReadInfoError, (uint32_t)drv_id, 0);
  }
#else
  #define EvrFsIOC_ReadInfoError(drv_id)
#endif

/**
  \brief  Event on call of \ref fs_ioc_read_info function (API)
  \param[in]  drv_id     drive ID
  \param[in]  code       device control code \ref fsDevCtrlCode
  \param[in]  p          generic pointer address
 */
#ifdef EvtFsIOC_DeviceControl
  __STATIC_INLINE void EvrFsIOC_DeviceControl (int32_t drv_id, uint32_t code, uint32_t p) {
    EventRecord4 (EvtFsIOC_DeviceControl, (uint32_t)drv_id, code, p, 0);
  }
#else
  #define EvrFsIOC_DeviceControl(drv_id, code, p)
#endif

/**
  \brief  Event on call of \ref fs_ioc_read_info function (API)
  \param[in]  drv_id     drive ID
  \param[in]  code       device control code \ref fsDevCtrlCode
  \param[in]  p          generic pointer address
 */
#ifdef EvtFsIOC_DeviceControlSuccess
  __STATIC_INLINE void EvrFsIOC_DeviceControlSuccess (int32_t drv_id, uint32_t code, uint32_t p) {
    EventRecord4 (EvtFsIOC_DeviceControlSuccess, (uint32_t)drv_id, code, p, 0);
  }
#else
  #define EvrFsIOC_DeviceControlSuccess(drv_id, code, p)
#endif

/**
  \brief  Event on invalid drive ID specifier (Error)
  \param[in]  drv_id     drive ID
 */
#ifdef EvtFsIOC_DriveIdInvalid
  __STATIC_INLINE void EvrFsIOC_DriveIdInvalid (int32_t drv_id) {
    EventRecord2 (EvtFsIOC_DriveIdInvalid, (uint32_t)drv_id, 0);
  }
#else
  #define EvrFsIOC_DriveIdInvalid(drv_id)
#endif

/**
  \brief  Event on non-FAT drive specifier (Error)
  \param[in]  drv_id     drive ID
 */
#ifdef EvtFsIOC_DriveNotFAT
  __STATIC_INLINE void EvrFsIOC_DriveNotFAT (int32_t drv_id) {
    EventRecord2 (EvtFsIOC_DriveNotFAT, (uint32_t)drv_id, 0);
  }
#else
  #define EvrFsIOC_DriveNotFAT(drv_id)
#endif

/**
  \brief  Event on specifying drive without IOC support (Error)
  \param[in]  drive     4 byte encoded drive letter
 */
#ifdef EvtFsIOC_NotAllowed
  __STATIC_INLINE void EvrFsIOC_NotAllowed (uint32_t drive) {
    EventRecord2 (EvtFsIOC_NotAllowed, drive, 0);
  }
#else
  #define EvrFsIOC_NotAllowed(drive)
#endif

/**
  \brief  Event on NFTL initialization (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_Init
  __STATIC_INLINE void EvrFsNFTL_Init (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_Init, instance, 0);
  }
#else
  #define EvrFsNFTL_Init(instance)
#endif

/**
  \brief  Event on NFTL memory media initialization error (Error)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_InitMediaFailed
  __STATIC_INLINE void EvrFsNFTL_InitMediaFailed (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_InitMediaFailed, instance, 0);
  }
#else
  #define EvrFsNFTL_InitMediaFailed(instance)
#endif

/**
  \brief  Event on NFTL mount (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_Mount
  __STATIC_INLINE void EvrFsNFTL_Mount (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_Mount, instance, 0);
  }
#else
  #define EvrFsNFTL_Mount(instance)
#endif

/**
  \brief  Event on successful NFTL mount (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_MountSuccess
  __STATIC_INLINE void EvrFsNFTL_MountSuccess (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_MountSuccess, instance, 0);
  }
#else
  #define EvrFsNFTL_MountSuccess(instance)
#endif

/**
  \brief  Event (Error)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_DeviceNotInitialized
  __STATIC_INLINE void EvrFsNFTL_DeviceNotInitialized (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_DeviceNotInitialized, instance, 0);
  }
#else
  #define EvrFsNFTL_DeviceNotInitialized(instance)
#endif

/**
  \brief  Event on NAND device reset (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_ResetDevice
  __STATIC_INLINE void EvrFsNFTL_ResetDevice (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_ResetDevice, instance, 0);
  }
#else
  #define EvrFsNFTL_ResetDevice(instance)
#endif

/**
  \brief  Event on NAND device reset error (Error)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_ResetDeviceFailed
  __STATIC_INLINE void EvrFsNFTL_ResetDeviceFailed (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_ResetDeviceFailed, instance, 0);
  }
#else
  #define EvrFsNFTL_ResetDeviceFailed(instance)
#endif

/**
  \brief  Event on NAND device reset timeout (Error)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_ResetDeviceTimeout
  __STATIC_INLINE void EvrFsNFTL_ResetDeviceTimeout (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_ResetDeviceTimeout, instance, 0);
  }
#else
  #define EvrFsNFTL_ResetDeviceTimeout(instance)
#endif

/**
  \brief  Event on NFTL boot block read (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_ReadBootBlock
  __STATIC_INLINE void EvrFsNFTL_ReadBootBlock (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_ReadBootBlock, instance, 0);
  }
#else
  #define EvrFsNFTL_ReadBootBlock(instance)
#endif

/**
  \brief  Event on NFTL boot block error (Error)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_ReadBootBlockFailed
  __STATIC_INLINE void EvrFsNFTL_ReadBootBlockFailed (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_ReadBootBlockFailed, instance, 0);
  }
#else
  #define EvrFsNFTL_ReadBootBlockFailed(instance)
#endif

/**
  \brief  Event on NFTL valid boot signatures (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_BootSignatureValid
  __STATIC_INLINE void EvrFsNFTL_BootSignatureValid (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_BootSignatureValid, instance, 0);
  }
#else
  #define EvrFsNFTL_BootSignatureValid(instance)
#endif

/**
  \brief  Event on NFTL invalid boot signatures (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_BootSignatureInvalid
  __STATIC_INLINE void EvrFsNFTL_BootSignatureInvalid (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_BootSignatureInvalid, instance, 0);
  }
#else
  #define EvrFsNFTL_BootSignatureInvalid(instance)
#endif

/**
  \brief  Event on NFTL invalid boot signatures (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  capacity  number of available data blocks
 */
#ifdef EvtFsNFTL_DataBlockCapacity
  __STATIC_INLINE void EvrFsNFTL_DataBlockCapacity (uint32_t instance, uint32_t capacity) {
    EventRecord2 (EvtFsNFTL_DataBlockCapacity, instance, capacity);
  }
#else
  #define EvrFsNFTL_DataBlockCapacity(instance, capacity)
#endif

/**
  \brief  Event on NFTL invalid boot signatures (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_ReadTable
  __STATIC_INLINE void EvrFsNFTL_ReadTable (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_ReadTable, instance, 0);
  }
#else
  #define EvrFsNFTL_ReadTable(instance)
#endif

/**
  \brief  Event on NFTL  (Error)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_ReadTableFailed
  __STATIC_INLINE void EvrFsNFTL_ReadTableFailed (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_ReadTableFailed, instance, 0);
  }
#else
  #define EvrFsNFTL_ReadTableFailed(instance)
#endif

/**
  \brief  Event on NFTL device uninitialization (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_Uninit
  __STATIC_INLINE void EvrFsNFTL_Uninit (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_Uninit, instance, 0);
  }
#else
  #define EvrFsNFTL_Uninit(instance)
#endif

/**
  \brief  Event on NFTL driver uninitialization (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_UninitDriver
  __STATIC_INLINE void EvrFsNFTL_UninitDriver (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_UninitDriver, instance, 0);
  }
#else
  #define EvrFsNFTL_UninitDriver(instance)
#endif

/**
  \brief  Event on NFTL driver uninitialization error (Error)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_UninitDriverFailed
  __STATIC_INLINE void EvrFsNFTL_UninitDriverFailed (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_UninitDriverFailed, instance, 0);
  }
#else
  #define EvrFsNFTL_UninitDriverFailed(instance)
#endif

/**
  \brief  Event on NFTL driver uninitialization (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  info      \ref fsMediaInfo structure address
 */
#ifdef EvtFsNFTL_ReadInfo
  __STATIC_INLINE void EvrFsNFTL_ReadInfo (uint32_t instance, uint32_t info) {
    EventRecord2 (EvtFsNFTL_ReadInfo, instance, info);
  }
#else
  #define EvrFsNFTL_ReadInfo(instance, info)
#endif

/**
  \brief  Event on succesful media capacity retrieval (Op)
  \param[in]  instance   NFTL instance number
  \param[in]  sect_count capacity in number of sectors
 */
#ifdef EvtFsNFTL_ReadInfoSuccess
  __STATIC_INLINE void EvrFsNFTL_ReadInfoSuccess (uint32_t instance, uint32_t sect_count) {
    EventRecord2 (EvtFsNFTL_ReadInfoSuccess, instance, sect_count);
  }
#else
  #define EvrFsNFTL_ReadInfoSuccess(instance, sect_count)
#endif

/**
  \brief  Event on NFTL device control operation (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  code      device control command code
  \param[in]  p         command code argument
 */
#ifdef EvtFsNFTL_DevCtrl
  __STATIC_INLINE void EvrFsNFTL_DevCtrl (uint32_t instance, uint32_t code, void *p) {
    EventRecord4 (EvtFsNFTL_DevCtrl, instance, code, (uint32_t)p, 0);
  }
#else
  #define EvrFsNFTL_DevCtrl(instance, code, p)
#endif

/**
  \brief  Event on NFTL device control code unsupported error (Error)
  \param[in]  instance  NFTL instance number
  \param[in]  code      device control command code
 */
#ifdef EvtFsNFTL_DevCtrlUnsupported
  __STATIC_INLINE void EvrFsNFTL_DevCtrlUnsupported (uint32_t instance, uint32_t code) {
    EventRecord2 (EvtFsNFTL_DevCtrlUnsupported, instance, code);
  }
#else
  #define EvrFsNFTL_DevCtrlUnsupported(instance, code)
#endif

/**
  \brief  Event on NFTL format operation (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_Format
  __STATIC_INLINE void EvrFsNFTL_Format (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_Format, instance, 0);
  }
#else
  #define EvrFsNFTL_Format(instance)
#endif

/**
  \brief  Event on successful NFTL format operation (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_FormatSuccess
  __STATIC_INLINE void EvrFsNFTL_FormatSuccess (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_FormatSuccess, instance, 0);
  }
#else
  #define EvrFsNFTL_FormatSuccess(instance)
#endif

/**
  \brief  Event on NFTL format operation with option LLEB (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_FormatLLEB
  __STATIC_INLINE void EvrFsNFTL_FormatLLEB (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_FormatLLEB, instance, 0);
  }
#else
  #define EvrFsNFTL_FormatLLEB(instance)
#endif

/**
  \brief  Event on NFTL bad block position setting (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  pos       bad block position
 */
#ifdef EvtFsNFTL_BBMPositionSet
  __STATIC_INLINE void EvrFsNFTL_BBMPositionSet (uint32_t instance, uint32_t pos) {
    EventRecord2 (EvtFsNFTL_BBMPositionSet, instance, pos);
  }
#else
  #define EvrFsNFTL_BBMPositionSet(instance, pos)
#endif

/**
  \brief  Event on NFTL bad block detection (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  block     block number
 */
#ifdef EvtFsNFTL_BadBlockDetected
  __STATIC_INLINE void EvrFsNFTL_BadBlockDetected (uint32_t instance, uint32_t block) {
    EventRecord2 (EvtFsNFTL_BadBlockDetected, instance, block);
  }
#else
  #define EvrFsNFTL_BadBlockDetected(instance, block)
#endif

/**
  \brief  Event on NFTL bad block mark operation (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  block     block number
 */
#ifdef EvtFsNFTL_BadBlockMark
  __STATIC_INLINE void EvrFsNFTL_BadBlockMark (uint32_t instance, uint32_t block) {
    EventRecord2 (EvtFsNFTL_BadBlockMark, instance, block);
  }
#else
  #define EvrFsNFTL_BadBlockMark(instance, block)
#endif

/**
  \brief  Event on NFTL write sector operation (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  sector    sector number
  \param[in]  buf       data buffer
  \param[in]  cnt       number of sectors to write
 */
#ifdef EvtFsNFTL_WriteSector
  __STATIC_INLINE void EvrFsNFTL_WriteSector (uint32_t instance, uint32_t sector, const uint8_t *buf, uint32_t cnt) {
    EventRecord4 (EvtFsNFTL_WriteSector, instance, sector, (uint32_t)buf, cnt);
  }
#else
  #define EvrFsNFTL_WriteSector(instance, sector, buf, cnt)
#endif

/**
  \brief  Event on successful NFTL write sector operation (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_WriteSectorSuccess
  __STATIC_INLINE void EvrFsNFTL_WriteSectorSuccess (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_WriteSectorSuccess, instance, 0);
  }
#else
  #define EvrFsNFTL_WriteSectorSuccess(instance)
#endif

/**
  \brief  Event on NFTL read sector operation (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  sector    sector number
  \param[in]  buf       data buffer
  \param[in]  cnt       number of sectors to read
 */
#ifdef EvtFsNFTL_ReadSector
  __STATIC_INLINE void EvrFsNFTL_ReadSector (uint32_t instance, uint32_t sector, uint8_t *buf, uint32_t cnt) {
    EventRecord4 (EvtFsNFTL_ReadSector, instance, sector, (uint32_t)buf, cnt);
  }
#else
  #define EvrFsNFTL_ReadSector(instance, sector, buf, cnt)
#endif

/**
  \brief  Event on successful NFTL read sector operation (Op)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_ReadSectorSuccess
  __STATIC_INLINE void EvrFsNFTL_ReadSectorSuccess (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_ReadSectorSuccess, instance, 0);
  }
#else
  #define EvrFsNFTL_ReadSectorSuccess(instance)
#endif

/**
  \brief  Event on NFTL resolving logical sector to logical block (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  lsn       logical sector number
  \param[in]  lbn       logical block number
 */
#ifdef EvtFsNFTL_LsnToLbn
  __STATIC_INLINE void EvrFsNFTL_LsnToLbn (uint32_t instance, uint32_t lsn, uint32_t lbn) {
    EventRecord4 (EvtFsNFTL_LsnToLbn, instance, lsn, lbn, 0);
  }
#else
  #define EvrFsNFTL_LsnToLbn(instance, lsn, lbn)
#endif

/**
  \brief  Event on NFTL resolving logical block to physical block (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  lbn       logical block number
  \param[in]  pbn_0     physical block number
  \param[in]  pbn_1     physical block number
 */
#ifdef EvtFsNFTL_LbnToPbn
  __STATIC_INLINE void EvrFsNFTL_LbnToPbn (uint32_t instance, uint32_t lbn, uint32_t pbn_0, uint32_t pbn_1) {
    EventRecord4 (EvtFsNFTL_LbnToPbn, instance, lbn, pbn_0, pbn_1);
  }
#else
  #define EvrFsNFTL_LbnToPbn(instance, lbn, pbn_0, pbn_1)
#endif

/**
  \brief  Event when logical block number is out of range (Error)
  \param[in]  instance  NFTL instance number
  \param[in]  lbn       logical block number
  \param[in]  max       maximum logical block number
 */
#ifdef EvtFsNFTL_LbnOutOfRange
  __STATIC_INLINE void EvrFsNFTL_LbnOutOfRange (uint32_t instance, uint32_t lbn, uint32_t max) {
    EventRecord4 (EvtFsNFTL_LbnOutOfRange, instance, lbn, max, 0);
  }
#else
  #define EvrFsNFTL_LbnOutOfRange(instance, lbn, max)
#endif

/**
  \brief  Event when physical block number is out of range (Error)
  \param[in]  instance  NFTL instance number
  \param[in]  pbn       physical block number
  \param[in]  max       maximum logical block number
 */
#ifdef EvtFsNFTL_PbnOutOfRange
  __STATIC_INLINE void EvrFsNFTL_PbnOutOfRange (uint32_t instance, uint32_t pbn, uint32_t max) {
    EventRecord4 (EvtFsNFTL_PbnOutOfRange, instance, pbn, max, 0);
  }
#else
  #define EvrFsNFTL_PbnOutOfRange(instance, pbn, max)
#endif

/**
  \brief  Event on NFTL translation table lookup (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  lbn       logical block number
 */
#ifdef EvtFsNFTL_TableLookup
  __STATIC_INLINE void EvrFsNFTL_TableLookup (uint32_t instance, uint32_t lbn) {
    EventRecord2 (EvtFsNFTL_TableLookup, instance, lbn);
  }
#else
  #define EvrFsNFTL_TableLookup(instance, lbn)
#endif

/**
  \brief  Event on nonexisting translation table entry (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  lbn       logical block number
 */
#ifdef EvtFsNFTL_TableEntryNotFound
  __STATIC_INLINE void EvrFsNFTL_TableEntryNotFound (uint32_t instance, uint32_t lbn) {
    EventRecord2 (EvtFsNFTL_TableEntryNotFound, instance, lbn);
  }
#else
  #define EvrFsNFTL_TableEntryNotFound(instance, lbn)
#endif

/**
  \brief  Event on NAND block allocation operation (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  area      0:translation table block, 1:user data block
 */
#ifdef EvtFsNFTL_AllocateBlock
  __STATIC_INLINE void EvrFsNFTL_AllocateBlock (uint32_t instance, uint32_t area) {
    EventRecord2 (EvtFsNFTL_AllocateBlock, instance, area);
  }
#else
  #define EvrFsNFTL_AllocateBlock(instance, area)
#endif

/**
  \brief  Event on search of logical sector within NAND block (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  lsn       logical sector number
  \param[in]  pbn       physical block number
 */
#ifdef EvtFsNFTL_LsnFind
  __STATIC_INLINE void EvrFsNFTL_LsnFind (uint32_t instance, uint32_t lsn, uint32_t pbn) {
    EventRecord4 (EvtFsNFTL_LsnFind, instance, lsn, pbn, 0);
  }
#else
  #define EvrFsNFTL_LsnFind(instance, lsn, pbn)
#endif

/**
  \brief  Event when logical sector is found within NAND block (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  lsn       logical sector number
  \param[in]  pbn       physical block number
  \param[in]  pg        page number
 */
#ifdef EvtFsNFTL_LsnFound
  __STATIC_INLINE void EvrFsNFTL_LsnFound (uint32_t instance, uint32_t lsn, uint32_t pbn, uint32_t pg) {
    EventRecord4 (EvtFsNFTL_LsnFound, instance, lsn, pbn, pg);
  }
#else
  #define EvrFsNFTL_LsnFound(instance, lsn, pbn, pg)
#endif

/**
  \brief  Event when logical sector is not found within NAND block (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  lsn       logical sector number
  \param[in]  pbn       physical block number
 */
#ifdef EvtFsNFTL_LsnNotFound
  __STATIC_INLINE void EvrFsNFTL_LsnNotFound (uint32_t instance, uint32_t lsn, uint32_t pbn) {
    EventRecord4 (EvtFsNFTL_LsnNotFound, instance, lsn, pbn, 0);
  }
#else
  #define EvrFsNFTL_LsnNotFound(instance, lsn, pbn)
#endif

/**
  \brief  Event on NFTL garbage collection operation (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  src_0     physical block number (source 0)
  \param[in]  src_1     physical block number (source 1)
  \param[in]  dst       physical block number (destination)
 */
#ifdef EvtFsNFTL_GarbageCollection
  __STATIC_INLINE void EvrFsNFTL_GarbageCollection (uint32_t instance, uint32_t src_0, uint32_t src_1, uint32_t dst) {
    EventRecord4 (EvtFsNFTL_GarbageCollection, instance, src_0, src_1, dst);
  }
#else
  #define EvrFsNFTL_GarbageCollection(instance, src_0, src_1, dst)
#endif

/**
  \brief  Event on setting NAND block as bad (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  pbn       physical block number
 */
#ifdef EvtFsNFTL_SetBadBlockMarker
  __STATIC_INLINE void EvrFsNFTL_SetBadBlockMarker (uint32_t instance, uint32_t pbn) {
    EventRecord2 (EvtFsNFTL_SetBadBlockMarker, instance, pbn);
  }
#else
  #define EvrFsNFTL_SetBadBlockMarker(instance, pbn)
#endif

/**
  \brief  Event on block content move (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  src       physical block number
  \param[in]  dst       physical block number
 */
#ifdef EvtFsNFTL_MoveData
  __STATIC_INLINE void EvrFsNFTL_MoveData (uint32_t instance, uint32_t src, uint32_t dst) {
    EventRecord4 (EvtFsNFTL_MoveData, instance, src, dst, 0);
  }
#else
  #define EvrFsNFTL_MoveData(instance, src, dst)
#endif

/**
  \brief  Event on attempt to change block physical location (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  pbn       physical block number
 */
#ifdef EvtFsNFTL_RelocateBlock
  __STATIC_INLINE void EvrFsNFTL_RelocateBlock (uint32_t instance, uint32_t pbn) {
    EventRecord2 (EvtFsNFTL_RelocateBlock, instance, pbn);
  }
#else
  #define EvrFsNFTL_RelocateBlock(instance, pbn)
#endif

/**
  \brief  Event on failed attempt to change block physical location (Error)
  \param[in]  instance  NFTL instance number
  \param[in]  pbn       physical block number
 */
#ifdef EvtFsNFTL_RelocateBlockFailed
  __STATIC_INLINE void EvrFsNFTL_RelocateBlockFailed (uint32_t instance, uint32_t pbn) {
    EventRecord2 (EvtFsNFTL_RelocateBlockFailed, instance, pbn);
  }
#else
  #define EvrFsNFTL_RelocateBlockFailed(instance, pbn)
#endif

/**
  \brief  Event on skipped bad block (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  pbn       physical block number
 */
#ifdef EvtFsNFTL_SkipBadBlock
  __STATIC_INLINE void EvrFsNFTL_SkipBadBlock (uint32_t instance, uint32_t pbn) {
    EventRecord2 (EvtFsNFTL_SkipBadBlock, instance, pbn);
  }
#else
  #define EvrFsNFTL_SkipBadBlock(instance, pbn)
#endif

/**
  \brief  Event on NAND page read error (Error)
  \param[in]  instance  NFTL instance number
  \param[in]  row       page address
 */
#ifdef EvtFsNFTL_PageReadFailed
  __STATIC_INLINE void EvrFsNFTL_PageReadFailed (uint32_t instance, uint32_t row) {
    EventRecord2 (EvtFsNFTL_PageReadFailed, instance, row);
  }
#else
  #define EvrFsNFTL_PageReadFailed(instance, row)
#endif

/**
  \brief  Event on invalid page program status value (Error)
  \param[in]  instance  NFTL instance number
  \param[in]  row       page address
 */
#ifdef EvtFsNFTL_PageProgramStatusErr
  __STATIC_INLINE void EvrFsNFTL_PageProgramStatusErr (uint32_t instance, uint32_t row) {
    EventRecord2 (EvtFsNFTL_PageProgramStatusErr, instance, row);
  }
#else
  #define EvrFsNFTL_PageProgramStatusErr(instance, row)
#endif

/**
  \brief  Event on NAND page program error (Error)
  \param[in]  instance  NFTL instance number
  \param[in]  row       page address
 */
#ifdef EvtFsNFTL_PageProgramFailed
  __STATIC_INLINE void EvrFsNFTL_PageProgramFailed (uint32_t instance, uint32_t row) {
    EventRecord2 (EvtFsNFTL_PageProgramFailed, instance, row);
  }
#else
  #define EvrFsNFTL_PageProgramFailed(instance, row)
#endif

/**
  \brief  Event on NAND page program timeout (Error)
  \param[in]  instance  NFTL instance number
  \param[in]  row       page address
 */
#ifdef EvtFsNFTL_PageProgramTimeout
  __STATIC_INLINE void EvrFsNFTL_PageProgramTimeout (uint32_t instance, uint32_t row) {
    EventRecord2 (EvtFsNFTL_PageProgramTimeout, instance, row);
  }
#else
  #define EvrFsNFTL_PageProgramTimeout(instance, row)
#endif

/**
  \brief  Event on invalid block erase status value (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  row       block address
 */
#ifdef EvtFsNFTL_BlockEraseStatusErr
  __STATIC_INLINE void EvrFsNFTL_BlockEraseStatusErr (uint32_t instance, uint32_t row) {
    EventRecord2 (EvtFsNFTL_BlockEraseStatusErr, instance, row);
  }
#else
  #define EvrFsNFTL_BlockEraseStatusErr(instance, row)
#endif

/**
  \brief  Event on NAND block erase error (Error)
  \param[in]  instance  NFTL instance number
  \param[in]  row       block address
 */
#ifdef EvtFsNFTL_BlockEraseFailed
  __STATIC_INLINE void EvrFsNFTL_BlockEraseFailed (uint32_t instance, uint32_t row) {
    EventRecord2 (EvtFsNFTL_BlockEraseFailed, instance, row);
  }
#else
  #define EvrFsNFTL_BlockEraseFailed(instance, row)
#endif

/**
  \brief  Event on NAND block erase timeout (Error)
  \param[in]  instance  NFTL instance number
  \param[in]  row       block address
 */
#ifdef EvtFsNFTL_BlockEraseTimeout
  __STATIC_INLINE void EvrFsNFTL_BlockEraseTimeout (uint32_t instance, uint32_t row) {
    EventRecord2 (EvtFsNFTL_BlockEraseTimeout, instance, row);
  }
#else
  #define EvrFsNFTL_BlockEraseTimeout(instance, row)
#endif

/**
  \brief  Event on ECC bit error correction (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  row       block address
 */
#ifdef EvtFsNFTL_BitErrorCorrected
  __STATIC_INLINE void EvrFsNFTL_BitErrorCorrected (uint32_t instance, uint32_t row) {
    EventRecord2 (EvtFsNFTL_BitErrorCorrected, instance, row);
  }
#else
  #define EvrFsNFTL_BitErrorCorrected(instance, row)
#endif

/**
  \brief  Event on invalid function parameter(s) (Error)
  \param[in]  instance  NFTL instance number
 */
#ifdef EvtFsNFTL_ParameterInvalid
  __STATIC_INLINE void EvrFsNFTL_ParameterInvalid (uint32_t instance) {
    EventRecord2 (EvtFsNFTL_ParameterInvalid, instance, 0);
  }
#else
  #define EvrFsNFTL_ParameterInvalid(instance)
#endif

/**
  \brief  Event on translation table update (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  lbn       logical block number
  \param[in]  pbn_0     physical block number
  \param[in]  pbn_1     physical block number
 */
#ifdef EvtFsNFTL_TableUpdate
  __STATIC_INLINE void EvrFsNFTL_TableUpdate (uint32_t instance, uint32_t lbn, uint32_t pbn_0, uint32_t pbn_1) {
    EventRecord4 (EvtFsNFTL_TableUpdate, instance, lbn, pbn_0, pbn_1);
  }
#else
  #define EvrFsNFTL_TableUpdate(instance, lbn, pbn_0, pbn_1)
#endif

/**
  \brief  Event on successful block allocation (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  pbn       physical block number
 */
#ifdef EvtFsNFTL_AllocatedBlock
  __STATIC_INLINE void EvrFsNFTL_AllocatedBlock (uint32_t instance, uint32_t pbn) {
    EventRecord2 (EvtFsNFTL_AllocatedBlock, instance, pbn);
  }
#else
  #define EvrFsNFTL_AllocatedBlock(instance, pbn)
#endif

/**
  \brief  Event on loading sector data from/to NAND page buffer (Detail)
  \param[in]  instance  NFTL instance number
  \param[in]  lsn       logical sector number
  \param[in]  offs      sector data offset (position within page)
  \param[in]  spare     sector spare offset (position within page)
 */
#ifdef EvtFsNFTL_LoadSector
  __STATIC_INLINE void EvrFsNFTL_LoadSector (uint32_t instance, uint32_t lsn, uint32_t offs, uint32_t spare) {
    EventRecord4 (EvtFsNFTL_LoadSector, instance, lsn, offs, spare);
  }
#else
  #define EvrFsNFTL_LoadSector(instance, lsn, offs, spare)
#endif

/**
  \brief  Event on translation table layout configuration read (Detail)
  \param[in]  instance  NFTL instance number
  \param[in]  pbn_0     physical block number
  \param[in]  pbn_1     physical block number
 */
#ifdef EvtFsNFTL_LoadTableLayout
  __STATIC_INLINE void EvrFsNFTL_LoadTableLayout (uint32_t instance, uint32_t pbn_0, uint32_t pbn_1) {
    EventRecord4 (EvtFsNFTL_LoadTableLayout, instance, pbn_0, pbn_1, 0);
  }
#else
  #define EvrFsNFTL_LoadTableLayout(instance, pbn_0, pbn_1)
#endif

/**
  \brief  Event on file data layout configuration read (Detail)
  \param[in]  instance  NFTL instance number
  \param[in]  pbn_0     physical block number
  \param[in]  pbn_1     physical block number
 */
#ifdef EvtFsNFTL_LoadDataLayout
  __STATIC_INLINE void EvrFsNFTL_LoadDataLayout (uint32_t instance, uint32_t pbn_0, uint32_t pbn_1) {
    EventRecord4 (EvtFsNFTL_LoadDataLayout, instance, pbn_0, pbn_1, 0);
  }
#else
  #define EvrFsNFTL_LoadDataLayout(instance, pbn_0, pbn_1)
#endif

/**
  \brief  Event on page layout setup (Detail)
  \param[in]  instance    NFTL instance number
  \param[in]  sector_inc  increment till next sector
  \param[in]  spare_ofs   first spare area offset
  \param[in]  spare_inc   increment till next spare
 */
#ifdef EvtFsNFTL_SetupPageLayout
  __STATIC_INLINE void EvrFsNFTL_SetupPageLayout (uint32_t instance, uint32_t sector_inc, uint32_t spare_ofs, uint32_t spare_inc) {
    EventRecord4 (EvtFsNFTL_SetupPageLayout, instance, sector_inc, spare_ofs, spare_inc);
  }
#else
  #define EvrFsNFTL_SetupPageLayout(instance, sector_inc, spare_ofs, spare_inc)
#endif

/**
  \brief  Event on spare area layout setup (Detail)
  \param[in]  instance  NFTL instance number
  \param[in]  ofs_lsn   LSN position offset
  \param[in]  ofs_dcm   DCM position offset
  \param[in]  ofs_bbm   BBM position offset
  \param[in]  ofs_ecc   ECC position offset
 */
#ifdef EvtFsNFTL_SetupSpareLayout
  __STATIC_INLINE void EvrFsNFTL_SetupSpareLayout (uint32_t instance, uint32_t ofs_lsn, uint32_t ofs_dcm, uint32_t ofs_bbm, uint32_t ofs_ecc) {
    EventRecord4 (EvtFsNFTL_SetupSpareLayout, instance, (ofs_dcm << 16) | ofs_lsn, ofs_bbm, ofs_ecc);
  }
#else
  #define EvrFsNFTL_SetupSpareLayout(instance, ofs_lsn, ofs_dcm, ofs_bbm, ofs_ecc)
#endif

/**
  \brief  Event on page write (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  pbn       physical block number
  \param[in]  pg        page number
 */
#ifdef EvtFsNFTL_PageWrite
  __STATIC_INLINE void EvrFsNFTL_PageWrite (uint32_t instance, uint32_t pbn, uint32_t pg) {
    EventRecord4 (EvtFsNFTL_PageWrite, instance, pbn, pg, 0);
  }
#else
  #define EvrFsNFTL_PageWrite(instance, pbn, pg)
#endif

/**
  \brief  Event on page read (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  pbn       physical block number
  \param[in]  pg        page number
 */
#ifdef EvtFsNFTL_PageRead
  __STATIC_INLINE void EvrFsNFTL_PageRead (uint32_t instance, uint32_t pbn, uint32_t pg) {
    EventRecord4 (EvtFsNFTL_PageRead, instance, pbn, pg, 0);
  }
#else
  #define EvrFsNFTL_PageRead(instance, pbn, pg)
#endif

/**
  \brief  Event on block erase (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  pbn       physical block number
 */
#ifdef EvtFsNFTL_BlockErase
  __STATIC_INLINE void EvrFsNFTL_BlockErase (uint32_t instance, uint32_t pbn) {
    EventRecord2 (EvtFsNFTL_BlockErase, instance, pbn);
  }
#else
  #define EvrFsNFTL_BlockErase(instance, pbn)
#endif

/**
  \brief  Event on status read (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  status    NAND status byte
 */
#ifdef EvtFsNFTL_StatusRead
  __STATIC_INLINE void EvrFsNFTL_StatusRead (uint32_t instance, uint32_t status) {
    EventRecord2 (EvtFsNFTL_StatusRead, instance, status);
  }
#else
  #define EvrFsNFTL_StatusRead(instance, status)
#endif

/**
  \brief  Event on page cache write (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  pbn       physical block number
  \param[in]  pg        page number
 */
#ifdef EvtFsNFTL_CacheWrite
  __STATIC_INLINE void EvrFsNFTL_CacheWrite (uint32_t instance, uint32_t pbn, uint32_t pg) {
    EventRecord4 (EvtFsNFTL_CacheWrite, instance, pbn, pg, 0);
  }
#else
  #define EvrFsNFTL_CacheWrite(instance, pbn, pg)
#endif

/**
  \brief  Event on page cache read (Op)
  \param[in]  instance  NFTL instance number
  \param[in]  pbn       physical block number
  \param[in]  pg        page number
  \param[in]  col       page column number
 */
#ifdef EvtFsNFTL_CacheRead
  __STATIC_INLINE void EvrFsNFTL_CacheRead (uint32_t instance, uint32_t pbn, uint32_t pg, uint32_t col) {
    EventRecord4 (EvtFsNFTL_CacheRead, instance, pbn, pg, col);
  }
#else
  #define EvrFsNFTL_CacheRead(instance, pbn, pg, col)
#endif

/**
  \brief  Event on NAND media layer initialization (Op)
  \param[in]  instance  NAND media layer instance
 */
#ifdef EvtFsNAND_Init
  __STATIC_INLINE void EvrFsNAND_Init (uint32_t instance) {
    EventRecord2 (EvtFsNAND_Init, instance, 0);
  }
#else
  #define EvrFsNAND_Init(instance)
#endif

/**
  \brief  Event on NAND driver initialization (Op)
  \param[in]  instance  NAND media layer instance
  \param[in]  driver    CMSIS NAND driver control block address
 */
#ifdef EvtFsNAND_InitDriver
  __STATIC_INLINE void EvrFsNAND_InitDriver (uint32_t instance, ARM_DRIVER_NAND *driver) {
    EventRecord2 (EvtFsNAND_InitDriver, instance, (uint32_t)driver);
  }
#else
  #define EvrFsNAND_InitDriver(instance, driver)
#endif

/**
  \brief  Event on NAND driver initialization error (Error)
  \param[in]  instance  NAND media layer instance
  \param[in]  driver    CMSIS NAND driver control block address
 */
#ifdef EvtFsNAND_InitDriverError
  __STATIC_INLINE void EvrFsNAND_InitDriverError (uint32_t instance, ARM_DRIVER_NAND *driver) {
    EventRecord2 (EvtFsNAND_InitDriverError, instance, (uint32_t)driver);
  }
#else
  #define EvrFsNAND_InitDriverError(instance, driver)
#endif

/**
  \brief  Event on NAND driver initialization error (Error)
  \param[in]  instance  NAND media layer instance
  \param[in]  driver    CMSIS NAND driver control block address
  \param[in]  state     CMSIS driver power state
 */
#ifdef EvtFsNAND_DriverPowerCtrlError
  __STATIC_INLINE void EvrFsNAND_DriverPowerCtrlError (uint32_t instance, ARM_DRIVER_NAND *driver, uint32_t state) {
    EventRecord4 (EvtFsNAND_DriverPowerCtrlError, instance, (uint32_t)driver, state, 0);
  }
#else
  #define EvrFsNAND_DriverPowerCtrlError(instance, driver, state)
#endif

/**
  \brief  Event on NAND driver initialization error (Error)
  \param[in]  instance  NAND media layer instance
  \param[in]  driver    CMSIS NAND driver control block address
  \param[in]  mode      NAND bus mode
 */
#ifdef EvtFsNAND_DriverBusModeSelect
  __STATIC_INLINE void EvrFsNAND_DriverBusModeSelect (uint32_t instance, ARM_DRIVER_NAND *driver, uint32_t mode) {
    EventRecord4 (EvtFsNAND_DriverBusModeSelect, instance, (uint32_t)driver, mode, 0);
  }
#else
  #define EvrFsNAND_DriverBusModeSelect(instance, driver, mode)
#endif

/**
  \brief  Event on NAND driver initialization error (Error)
  \param[in]  instance  NAND media layer instance
  \param[in]  driver    CMSIS NAND driver control block address
  \param[in]  width     NAND bus width
 */
#ifdef EvtFsNAND_DriverBusWidthSelect
  __STATIC_INLINE void EvrFsNAND_DriverBusWidthSelect (uint32_t instance, ARM_DRIVER_NAND *driver, uint32_t width) {
    EventRecord4 (EvtFsNAND_DriverBusWidthSelect, instance, (uint32_t)driver, width, 0);
  }
#else
  #define EvrFsNAND_DriverBusWidthSelect(instance, driver, width)
#endif

/**
  \brief  Event on NAND driver initialization error (Error)
  \param[in]  instance  NAND media layer instance
  \param[in]  driver    CMSIS NAND driver control block address
  \param[in]  width     NAND bus width
 */
#ifdef EvtFsNAND_DriverBusWidthError
  __STATIC_INLINE void EvrFsNAND_DriverBusWidthError (uint32_t instance, ARM_DRIVER_NAND *driver, uint32_t width) {
    EventRecord4 (EvtFsNAND_DriverBusWidthError, instance, (uint32_t)driver, width, 0);
  }
#else
  #define EvrFsNAND_DriverBusWidthError(instance, driver, width)
#endif

/**
  \brief  Event on NAND device voltage change (Op)
  \param[in]  instance  NAND media layer instance
  \param[in]  driver    CMSIS NAND driver control block address
  \param[in]  volt      NAND device voltage
 */
#ifdef EvtFsNAND_DriverDevPowerSelect
  __STATIC_INLINE void EvrFsNAND_DriverDevPowerSelect (uint32_t instance, ARM_DRIVER_NAND *driver, uint32_t volt) {
    EventRecord4 (EvtFsNAND_DriverDevPowerSelect, instance, (uint32_t)driver, volt, 0);
  }
#else
  #define EvrFsNAND_DriverDevPowerSelect(instance, driver, volt)
#endif

/**
  \brief  Event on NAND driver ECC inquiry (Op)
  \param[in]  instance  NAND media layer instance
  \param[in]  driver    CMSIS NAND driver control block address
 */
#ifdef EvtFsNAND_DriverEccInquire
  __STATIC_INLINE void EvrFsNAND_DriverEccInquire (uint32_t instance, ARM_DRIVER_NAND *driver) {
    EventRecord2 (EvtFsNAND_DriverEccInquire, instance, (uint32_t)driver);
  }
#else
  #define EvrFsNAND_DriverEccInquire(instance, driver)
#endif

/**
  \brief  Event on NAND driver ECC selection (Op)
  \param[in]  instance  NAND media layer instance
  \param[in]  driver    CMSIS NAND driver control block address
  \param[in]  index     ECC selection index
 */
#ifdef EvtFsNAND_DriverEccSelect
  __STATIC_INLINE void EvrFsNAND_DriverEccSelect (uint32_t instance, ARM_DRIVER_NAND *driver, int32_t index) {
    EventRecord4 (EvtFsNAND_DriverEccSelect, instance, (uint32_t)driver, (uint32_t)index, 0);
  }
#else
  #define EvrFsNAND_DriverEccSelect(instance, driver, index)
#endif

/**
  \brief  Event on NAND driver ECC inquiry error (Op)
  \param[in]  instance  NAND media layer instance
  \param[in]  driver    CMSIS NAND driver control block address
 */
#ifdef EvtFsNAND_DriverEccInquireError
  __STATIC_INLINE void EvrFsNAND_DriverEccInquireError (uint32_t instance, ARM_DRIVER_NAND *driver) {
    EventRecord2 (EvtFsNAND_DriverEccInquireError, instance, (uint32_t)driver);
  }
#else
  #define EvrFsNAND_DriverEccInquireError(instance, driver)
#endif

/**
  \brief  Event on NAND driver timeout error (Error)
  \param[in]  instance  NAND media layer instance
  \param[in]  driver    CMSIS NAND driver control block address
 */
#ifdef EvtFsNAND_DriverTimeoutError
  __STATIC_INLINE void EvrFsNAND_DriverTimeoutError (uint32_t instance, ARM_DRIVER_NAND *driver) {
    EventRecord2 (EvtFsNAND_DriverTimeoutError, instance, (uint32_t)driver);
  }
#else
  #define EvrFsNAND_DriverTimeoutError(instance, driver)
#endif

/**
  \brief  Event on NAND device timeout error (Error)
  \param[in]  instance  NAND media layer instance
  \param[in]  driver    CMSIS NAND driver control block address
  \param[in]  device    NAND device number
 */
#ifdef EvtFsNAND_DeviceTimeoutError
  __STATIC_INLINE void EvrFsNAND_DeviceTimeoutError (uint32_t instance, ARM_DRIVER_NAND *driver, uint32_t device) {
    EventRecord4 (EvtFsNAND_DeviceTimeoutError, instance, (uint32_t)driver, device, 0);
  }
#else
  #define EvrFsNAND_DeviceTimeoutError(instance, driver, device)
#endif

/**
  \brief  Event on NAND media layer uninitialization (Op)
  \param[in]  instance  NAND media layer instance
 */
#ifdef EvtFsNAND_Uninit
  __STATIC_INLINE void EvrFsNAND_Uninit (uint32_t instance) {
    EventRecord2 (EvtFsNAND_Uninit, instance, 0);
  }
#else
  #define EvrFsNAND_Uninit(instance)
#endif

/**
  \brief  Event on NAND device status retrieval (Detail)
  \param[in]  instance  NAND media layer instance
  \param[in]  status    device status
 */
#ifdef EvtFsNAND_StatusRead
  __STATIC_INLINE void EvrFsNAND_StatusRead (uint32_t instance, uint32_t status) {
    EventRecord2 (EvtFsNAND_StatusRead, instance, status);
  }
#else
  #define EvrFsNAND_StatusRead(instance, status)
#endif

/**
  \brief  Event on NAND erase block operation (Op)
  \param[in]  instance  NAND media layer instance
  \param[in]  row       block (row) address
 */
#ifdef EvtFsNAND_BlockErase
  __STATIC_INLINE void EvrFsNAND_BlockErase (uint32_t instance, uint32_t row) {
    EventRecord2 (EvtFsNAND_BlockErase, instance, row);
  }
#else
  #define EvrFsNAND_BlockErase(instance, row)
#endif

/**
  \brief  Event on NAND page write operation (Op)
  \param[in]  instance  NAND media layer instance
  \param[in]  row       page (row) address
  \param[in]  col       byte (column) address
  \param[in]  len       number of bytes
 */
#ifdef EvtFsNAND_PageWrite
  __STATIC_INLINE void EvrFsNAND_PageWrite (uint32_t instance, uint32_t row, uint32_t col, uint32_t len) {
    EventRecord4 (EvtFsNAND_PageWrite, instance, row, col, len);
  }
#else
  #define EvrFsNAND_PageWrite(instance, row, col, len)
#endif

/**
  \brief  Event on NAND page read operation (Op)
  \param[in]  instance  NAND media layer instance
  \param[in]  row       page (row) address
  \param[in]  col       byte (column) address
  \param[in]  len       number of bytes
 */
#ifdef EvtFsNAND_PageRead
  __STATIC_INLINE void EvrFsNAND_PageRead (uint32_t instance, uint32_t row, uint32_t col, uint32_t len) {
    EventRecord4 (EvtFsNAND_PageRead, instance, row, col, len);
  }
#else
  #define EvrFsNAND_PageRead(instance, row, col, len)
#endif

/**
  \brief  Event on NAND parameter page read operation (Op)
  \param[in]  instance  NAND media layer instance
  \param[in]  col       byte (column) address
  \param[in]  len       number of bytes
 */
#ifdef EvtFsNAND_ParamPageRead
  __STATIC_INLINE void EvrFsNAND_ParamPageRead (uint32_t instance, uint32_t col, uint32_t len) {
    EventRecord4 (EvtFsNAND_ParamPageRead, instance, col, len, 0);
  }
#else
  #define EvrFsNAND_ParamPageRead(instance, col, len)
#endif

/**
  \brief  Event on NAND device ID read operation (Op)
  \param[in]  instance  NAND media layer instance
  \param[in]  addr      ID address
  \param[in]  len       number of bytes
 */
#ifdef EvtFsNAND_IdRead
  __STATIC_INLINE void EvrFsNAND_IdRead (uint32_t instance, uint32_t addr, uint32_t len) {
    EventRecord4 (EvtFsNAND_IdRead, instance, addr, len, 0);
  }
#else
  #define EvrFsNAND_IdRead(instance, addr, len)
#endif

/**
  \brief  Event on NAND device reset operation (Op)
  \param[in]  instance  NAND media layer instance
 */
#ifdef EvtFsNAND_DeviceReset
  __STATIC_INLINE void EvrFsNAND_DeviceReset (uint32_t instance) {
    EventRecord2 (EvtFsNAND_DeviceReset, instance, 0);
  }
#else
  #define EvrFsNAND_DeviceReset(instance)
#endif

/**
  \brief  Event on memory card driver initialization (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  driver    CMSIS MCI driver control block address
 */
#ifdef EvtFsMcMCI_InitDriver
  __STATIC_INLINE void EvrFsMcMCI_InitDriver (uint32_t instance, uint32_t driver) {
    EventRecord2 (EvtFsMcMCI_InitDriver, instance, driver);
  }
#else
  #define EvrFsMcMCI_InitDriver(instance, driver)
#endif

/**
  \brief  Event on memory card driver initialization error (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  driver    CMSIS MCI driver control block address
 */
#ifdef EvtFsMcMCI_InitDriverError
  __STATIC_INLINE void EvrFsMcMCI_InitDriverError (uint32_t instance, uint32_t driver) {
    EventRecord2 (EvtFsMcMCI_InitDriverError, instance, driver);
  }
#else
  #define EvrFsMcMCI_InitDriverError(instance, driver)
#endif

/**
  \brief  Event on memory card driver initialization error (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  driver    CMSIS MCI driver control block address
  \param[in]  state     CMSIS driver power state
 */
#ifdef EvtFsMcMCI_InitDriverPowerError
  __STATIC_INLINE void EvrFsMcMCI_InitDriverPowerError (uint32_t instance, uint32_t driver, uint32_t state) {
    EventRecord4 (EvtFsMcMCI_InitDriverPowerError, instance, driver, state, 0);
  }
#else
  #define EvrFsMcMCI_InitDriverPowerError(instance, driver, state)
#endif

/**
  \brief  Event on memory card device initialization (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_InitMedia
  __STATIC_INLINE void EvrFsMcMCI_InitMedia (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_InitMedia, instance, 0);
  }
#else
  #define EvrFsMcMCI_InitMedia(instance)
#endif

/**
  \brief  Event on using uninitialized memory card device (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_MediaNotInitialized
  __STATIC_INLINE void EvrFsMcMCI_MediaNotInitialized (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_MediaNotInitialized, instance, 0);
  }
#else
  #define EvrFsMcMCI_MediaNotInitialized(instance)
#endif

/**
  \brief  Event on media detection using card detect switch (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_MediaCdActive
  __STATIC_INLINE void EvrFsMcMCI_MediaCdActive (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_MediaCdActive, instance, 0);
  }
#else
  #define EvrFsMcMCI_MediaCdActive(instance)
#endif

/**
  \brief  Event on card detect switch check and media is not present (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_MediaCdInactive
  __STATIC_INLINE void EvrFsMcMCI_MediaCdInactive (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_MediaCdInactive, instance, 0);
  }
#else
  #define EvrFsMcMCI_MediaCdInactive(instance)
#endif

/**
  \brief  Event on active write protection switch (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_MediaWpActive
  __STATIC_INLINE void EvrFsMcMCI_MediaWpActive (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_MediaWpActive, instance, 0);
  }
#else
  #define EvrFsMcMCI_MediaWpActive(instance)
#endif

/**
  \brief  Event on inactive write protection switch (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_MediaWpInactive
  __STATIC_INLINE void EvrFsMcMCI_MediaWpInactive (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_MediaWpInactive, instance, 0);
  }
#else
  #define EvrFsMcMCI_MediaWpInactive(instance)
#endif

/**
  \brief  Event on memory card device reset (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_MediaReset
  __STATIC_INLINE void EvrFsMcMCI_MediaReset (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_MediaReset, instance, 0);
  }
#else
  #define EvrFsMcMCI_MediaReset(instance)
#endif

/**
  \brief  Event on start of SD memory card detection operation (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_MediaDetectionSD
  __STATIC_INLINE void EvrFsMcMCI_MediaDetectionSD (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_MediaDetectionSD, instance, 0);
  }
#else
  #define EvrFsMcMCI_MediaDetectionSD(instance)
#endif

/**
  \brief  Event on received response from SD memory card V2 (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_MediaSD_V2
  __STATIC_INLINE void EvrFsMcMCI_MediaSD_V2 (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_MediaSD_V2, instance, 0);
  }
#else
  #define EvrFsMcMCI_MediaSD_V2(instance)
#endif

/**
  \brief  Event on received response from SD memory card V2 (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  ocr       OCR register content
 */
#ifdef EvtFsMcMCI_MediaReady
  __STATIC_INLINE void EvrFsMcMCI_MediaReady (uint32_t instance, uint32_t ocr) {
    EventRecord2 (EvtFsMcMCI_MediaReady, instance, ocr);
  }
#else
  #define EvrFsMcMCI_MediaReady(instance, ocr)
#endif

/**
  \brief  Event on nonresponsive media device (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_MediaNoResponse
  __STATIC_INLINE void EvrFsMcMCI_MediaNoResponse (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_MediaNoResponse, instance, 0);
  }
#else
  #define EvrFsMcMCI_MediaNoResponse(instance)
#endif

/**
  \brief  Event on start of MMC/eMMC device detection procedure (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_MediaDetectionMMC
  __STATIC_INLINE void EvrFsMcMCI_MediaDetectionMMC (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_MediaDetectionMMC, instance, 0);
  }
#else
  #define EvrFsMcMCI_MediaDetectionMMC(instance)
#endif

/**
  \brief  Event on media device detection error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_MediaDetectionError
  __STATIC_INLINE void EvrFsMcMCI_MediaDetectionError (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_MediaDetectionError, instance, 0);
  }
#else
  #define EvrFsMcMCI_MediaDetectionError(instance)
#endif

/**
  \brief  Event on CID register read error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_CidReadError
  __STATIC_INLINE void EvrFsMcMCI_CidReadError (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_CidReadError, instance, 0);
  }
#else
  #define EvrFsMcMCI_CidReadError(instance)
#endif

/**
  \brief  Event on RCA (Relative Card Address) register read error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_RcaReadError
  __STATIC_INLINE void EvrFsMcMCI_RcaReadError (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_RcaReadError, instance, 0);
  }
#else
  #define EvrFsMcMCI_RcaReadError(instance)
#endif

/**
  \brief  Event on RCA (Relative Card Address) register write error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_RcaWriteError
  __STATIC_INLINE void EvrFsMcMCI_RcaWriteError (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_RcaWriteError, instance, 0);
  }
#else
  #define EvrFsMcMCI_RcaWriteError(instance)
#endif

/**
  \brief  Event on CSD register read error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_CsdReadError
  __STATIC_INLINE void EvrFsMcMCI_CsdReadError (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_CsdReadError, instance, 0);
  }
#else
  #define EvrFsMcMCI_CsdReadError(instance)
#endif

/**
  \brief  Event on failed switch to TRAN state (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_TranStateError
  __STATIC_INLINE void EvrFsMcMCI_TranStateError (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_TranStateError, instance, 0);
  }
#else
  #define EvrFsMcMCI_TranStateError(instance)
#endif

/**
  \brief  Event on failed block length setting (Error)
  \param[in]  instance  memory card control layer instance
  \param[in]  blen      block length
 */
#ifdef EvtFsMcMCI_BlenWriteError
  __STATIC_INLINE void EvrFsMcMCI_BlenWriteError (uint32_t instance, uint32_t blen) {
    EventRecord2 (EvtFsMcMCI_BlenWriteError, instance, blen);
  }
#else
  #define EvrFsMcMCI_BlenWriteError(instance, blen)
#endif

/**
  \brief  Event on EXT_CSD register read error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_ExtCsdReadError
  __STATIC_INLINE void EvrFsMcMCI_ExtCsdReadError (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_ExtCsdReadError, instance, 0);
  }
#else
  #define EvrFsMcMCI_ExtCsdReadError(instance)
#endif

/**
  \brief  Event on high speed mode switch error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_HighSpeedSwitchError
  __STATIC_INLINE void EvrFsMcMCI_HighSpeedSwitchError (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_HighSpeedSwitchError, instance, 0);
  }
#else
  #define EvrFsMcMCI_HighSpeedSwitchError(instance)
#endif

/**
  \brief  Event on data bus width switch error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_DataBusSwitchError
  __STATIC_INLINE void EvrFsMcMCI_DataBusSwitchError (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_DataBusSwitchError, instance, 0);
  }
#else
  #define EvrFsMcMCI_DataBusSwitchError(instance)
#endif

/**
  \brief  Event on STBY state switch error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_StbyStateError
  __STATIC_INLINE void EvrFsMcMCI_StbyStateError (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_StbyStateError, instance, 0);
  }
#else
  #define EvrFsMcMCI_StbyStateError(instance)
#endif

/**
  \brief  Event on successful memory card device initialization (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_InitSuccess
  __STATIC_INLINE void EvrFsMcMCI_InitSuccess (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_InitSuccess, instance, 0);
  }
#else
  #define EvrFsMcMCI_InitSuccess(instance)
#endif

/**
  \brief  Event on MCI driver uninitialization (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  driver    CMSIS MCI driver control block address
 */
#ifdef EvtFsMcMCI_UninitDriver
  __STATIC_INLINE void EvrFsMcMCI_UninitDriver (uint32_t instance, uint32_t driver) {
    EventRecord2 (EvtFsMcMCI_UninitDriver, instance, driver);
  }
#else
  #define EvrFsMcMCI_UninitDriver(instance, driver)
#endif

/**
  \brief  Event on memory card device uninitialization (Op)
  \param[in]  instance  memory card control layer instance
 
 */
#ifdef EvtFsMcMCI_UninitMedia
  __STATIC_INLINE void EvrFsMcMCI_UninitMedia (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_UninitMedia, instance, 0);
  }
#else
  #define EvrFsMcMCI_UninitMedia(instance)
#endif

/**
  \brief  Event on sector read operation (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  sector    sector number
  \param[in]  count     number of sectors to read
 */
#ifdef EvtFsMcMCI_SectorRead
  __STATIC_INLINE void EvrFsMcMCI_SectorRead (uint32_t instance, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsMcMCI_SectorRead, instance, sector, count, 0);
  }
#else
  #define EvrFsMcMCI_SectorRead(instance, sector, count)
#endif

/**
  \brief  Event on sector read operation error (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  sector    sector number
  \param[in]  count     number of sectors to read
 */
#ifdef EvtFsMcMCI_SectorReadError
  __STATIC_INLINE void EvrFsMcMCI_SectorReadError (uint32_t instance, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsMcMCI_SectorReadError, instance, sector, count, 0);
  }
#else
  #define EvrFsMcMCI_SectorReadError(instance, sector, count)
#endif

/**
  \brief  Event on sector write operation (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  sector    sector number
  \param[in]  count     number of sectors to write
 */
#ifdef EvtFsMcMCI_SectorWrite
  __STATIC_INLINE void EvrFsMcMCI_SectorWrite (uint32_t instance, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsMcMCI_SectorWrite, instance, sector, count, 0);
  }
#else
  #define EvrFsMcMCI_SectorWrite(instance, sector, count)
#endif

/**
  \brief  Event on sector write operation error (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  sector    sector number
  \param[in]  count     number of sectors to write
 */
#ifdef EvtFsMcMCI_SectorWriteError
  __STATIC_INLINE void EvrFsMcMCI_SectorWriteError (uint32_t instance, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsMcMCI_SectorWriteError, instance, sector, count, 0);
  }
#else
  #define EvrFsMcMCI_SectorWriteError(instance, sector, count)
#endif

/**
  \brief  Event on transfer error (Error)
  \param[in]  instance  memory card control layer instance
  \param[in]  events    \ref evr_fsmcmci
 */
#ifdef EvtFsMcMCI_TransferError
  __STATIC_INLINE void EvrFsMcMCI_TransferError (uint32_t instance, uint32_t events) {
    EventRecord2 (EvtFsMcMCI_TransferError, instance, events);
  }
#else
  #define EvrFsMcMCI_TransferError(instance, events)
#endif

/**
  \brief  Event on transfer setup error (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  retry     retry number
 */
#ifdef EvtFsMcMCI_TransferRetry
  __STATIC_INLINE void EvrFsMcMCI_TransferRetry (uint32_t instance, uint32_t retry) {
    EventRecord2 (EvtFsMcMCI_TransferRetry, instance, retry);
  }
#else
  #define EvrFsMcMCI_TransferRetry(instance, retry)
#endif

/**
  \brief  Event on media capacity retrieval operation (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_ReadInfo
  __STATIC_INLINE void EvrFsMcMCI_ReadInfo (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_ReadInfo, instance, 0);
  }
#else
  #define EvrFsMcMCI_ReadInfo(instance)
#endif

/**
  \brief  Event on media capacity retrieval operation (Op)
  \param[in]  instance   memory card control layer instance
  \param[in]  sect_count capacity in number of sectors
 */
#ifdef EvtFsMcMCI_ReadInfoSuccess
  __STATIC_INLINE void EvrFsMcMCI_ReadInfoSuccess (uint32_t instance, uint32_t sect_count) {
    EventRecord2 (EvtFsMcMCI_ReadInfoSuccess, instance, sect_count);
  }
#else
  #define EvrFsMcMCI_ReadInfoSuccess(instance, sect_count)
#endif

/**
  \brief  Event on memory card device control operation (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  code      device control command code
  \param[in]  p         command code argument address
 */
#ifdef EvtFsMcMCI_DevCtrl
  __STATIC_INLINE void EvrFsMcMCI_DevCtrl (uint32_t instance, uint32_t code, uint32_t p) {
    EventRecord4 (EvtFsMcMCI_DevCtrl, instance, code, p, 0);
  }
#else
  #define EvrFsMcMCI_DevCtrl(instance, code, p)
#endif

/**
  \brief  Event on memory card device control code unsupported error (Error)
  \param[in]  instance  memory card control layer instance
  \param[in]  code      device control command code
 */
#ifdef EvtFsMcMCI_DevCtrlUnsupported
  __STATIC_INLINE void EvrFsMcMCI_DevCtrlUnsupported (uint32_t instance, uint32_t code) {
    EventRecord2 (EvtFsMcMCI_DevCtrlUnsupported, instance, code);
  }
#else
  #define EvrFsMcMCI_DevCtrlUnsupported(instance, code)
#endif

/**
  \brief  Event on Lock/Unlock command execution (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_LockUnlockExec
  __STATIC_INLINE void EvrFsMcMCI_LockUnlockExec (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_LockUnlockExec, instance, 0);
  }
#else
  #define EvrFsMcMCI_LockUnlockExec(instance)
#endif

/**
  \brief  Event on Lock/Unlock command execution error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_LockUnlockExecError
  __STATIC_INLINE void EvrFsMcMCI_LockUnlockExecError (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_LockUnlockExecError, instance, 0);
  }
#else
  #define EvrFsMcMCI_LockUnlockExecError(instance)
#endif

/**
  \brief  Event on active password protection status (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_LockActive
  __STATIC_INLINE void EvrFsMcMCI_LockActive (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_LockActive, instance, 0);
  }
#else
  #define EvrFsMcMCI_LockActive(instance)
#endif

/**
  \brief  Event on deactivated password protection status (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_LockDeactivated
  __STATIC_INLINE void EvrFsMcMCI_LockDeactivated (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_LockDeactivated, instance, 0);
  }
#else
  #define EvrFsMcMCI_LockDeactivated(instance)
#endif

/**
  \brief  Event on discovery of unknown device state (Error)
  \param[in]  instance  memory card control layer instance
  \param[in]  state     memory card state
 */
#ifdef EvtFsMcMCI_DeviceStateUnknown
  __STATIC_INLINE void EvrFsMcMCI_DeviceStateUnknown (uint32_t instance, uint32_t state) {
    EventRecord2 (EvtFsMcMCI_DeviceStateUnknown, instance, state);
  }
#else
  #define EvrFsMcMCI_DeviceStateUnknown(instance, state)
#endif

/**
  \brief  Event on discovery of invalid device state (Error)
  \param[in]  instance  memory card control layer instance
  \param[in]  state     memory card state
 */
#ifdef EvtFsMcMCI_DeviceStateInvalid
  __STATIC_INLINE void EvrFsMcMCI_DeviceStateInvalid (uint32_t instance, uint32_t state) {
    EventRecord2 (EvtFsMcMCI_DeviceStateInvalid, instance, state);
  }
#else
  #define EvrFsMcMCI_DeviceStateInvalid(instance, state)
#endif

/**
  \brief  Event on discovery of invalid device state (Error)
  \param[in]  instance  memory card control layer instance
  \param[in]  cstate    current device state
  \param[in]  nstate    new device state
 */
#ifdef EvtFsMcMCI_DeviceStateTimeout
  __STATIC_INLINE void EvrFsMcMCI_DeviceStateTimeout (uint32_t instance, uint32_t cstate, uint32_t nstate) {
    EventRecord4 (EvtFsMcMCI_DeviceStateTimeout, instance, cstate, nstate, 0);
  }
#else
  #define EvrFsMcMCI_DeviceStateTimeout(instance, cstate, nstate)
#endif

/**
  \brief  Event on data transfer abort operation (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_TransferAbort
  __STATIC_INLINE void EvrFsMcMCI_TransferAbort (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_TransferAbort, instance, 0);
  }
#else
  #define EvrFsMcMCI_TransferAbort(instance)
#endif

/**
  \brief  Event on SLEEP/AWAKE control operation (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_SleepAwakeControl
  __STATIC_INLINE void EvrFsMcMCI_SleepAwakeControl (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_SleepAwakeControl, instance, 0);
  }
#else
  #define EvrFsMcMCI_SleepAwakeControl(instance)
#endif

/**
  \brief  Event on SLEEP mode active status (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_SleepActive
  __STATIC_INLINE void EvrFsMcMCI_SleepActive (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_SleepActive, instance, 0);
  }
#else
  #define EvrFsMcMCI_SleepActive(instance)
#endif

/**
  \brief  Event on AWAKE mode active status (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_AwakeActive
  __STATIC_INLINE void EvrFsMcMCI_AwakeActive (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_AwakeActive, instance, 0);
  }
#else
  #define EvrFsMcMCI_AwakeActive(instance)
#endif

/**
  \brief  Event on memory card power control operation (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  voltage   memory card power supply voltage
 */
#ifdef EvtFsMcMCI_CardPowerControl
  __STATIC_INLINE void EvrFsMcMCI_CardPowerControl (uint32_t instance, uint32_t voltage) {
    EventRecord2 (EvtFsMcMCI_CardPowerControl, instance, voltage);
  }
#else
  #define EvrFsMcMCI_CardPowerControl(instance, voltage)
#endif

/**
  \brief  Event on AWAKE mode active status (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  cmd       command index
  \param[in]  arg       command argument
 */
#ifdef EvtFsMcMCI_SendCommandError
  __STATIC_INLINE void EvrFsMcMCI_SendCommandError (uint32_t instance, uint32_t cmd, uint32_t arg) {
    EventRecord4 (EvtFsMcMCI_SendCommandError, instance, cmd, arg, 0);
  }
#else
  #define EvrFsMcMCI_SendCommandError(instance, cmd, arg)
#endif

/**
  \brief  Event on TransferSetup error for block read operation (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  buf       data buffer
  \param[in]  cnt       number of blocks to read
  \param[in]  size      block size
 */
#ifdef EvtFsMcMCI_ReadXferSetupError
  __STATIC_INLINE void EvrFsMcMCI_ReadXferSetupError (uint32_t instance, uint8_t *buf, uint32_t cnt, uint32_t size) {
    EventRecord4 (EvtFsMcMCI_ReadXferSetupError, instance, (uint32_t)buf, cnt, size);
  }
#else
  #define EvrFsMcMCI_ReadXferSetupError(instance, buf, cnt, size)
#endif

/**
  \brief  Event on TransferSetup error for write operation (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  buf       data buffer
  \param[in]  cnt       number of blocks to write
  \param[in]  size      block size
 */
#ifdef EvtFsMcMCI_WriteXferSetupError
  __STATIC_INLINE void EvrFsMcMCI_WriteXferSetupError (uint32_t instance, const uint8_t *buf, uint32_t cnt, uint32_t size) {
    EventRecord4 (EvtFsMcMCI_WriteXferSetupError, instance, (uint32_t)buf, cnt, size);
  }
#else
  #define EvrFsMcMCI_WriteXferSetupError(instance, buf, cnt, size)
#endif

/**
  \brief  Event on invalid function parameter(s) (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_ParameterInvalid
  __STATIC_INLINE void EvrFsMcMCI_ParameterInvalid (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_ParameterInvalid, instance, 0);
  }
#else
  #define EvrFsMcMCI_ParameterInvalid(instance)
#endif

/**
  \brief  Event on detection of enabled media password protection (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcMCI_MediaPasswordEnabled
  __STATIC_INLINE void EvrFsMcMCI_MediaPasswordEnabled (uint32_t instance) {
    EventRecord2 (EvtFsMcMCI_MediaPasswordEnabled, instance, 0);
  }
#else
  #define EvrFsMcMCI_MediaPasswordEnabled(instance)
#endif

/**
  \brief  Event on memory card driver initialization (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  driver    CMSIS SPI driver control block address
 */
#ifdef EvtFsMcSPI_InitDriver
  __STATIC_INLINE void EvrFsMcSPI_InitDriver (uint32_t instance, uint32_t driver) {
    EventRecord2 (EvtFsMcSPI_InitDriver, instance, driver);
  }
#else
  #define EvrFsMcSPI_InitDriver(instance, driver)
#endif

/**
  \brief  Event on successful memory card driver initialization (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  driver    CMSIS SPI driver control block address
 */
#ifdef EvtFsMcSPI_InitDriverSuccess
  __STATIC_INLINE void EvrFsMcSPI_InitDriverSuccess (uint32_t instance, uint32_t driver) {
    EventRecord2 (EvtFsMcSPI_InitDriverSuccess, instance, driver);
  }
#else
  #define EvrFsMcSPI_InitDriverSuccess(instance, driver)
#endif

/**
  \brief  Event on memory card driver initialization error (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  driver    CMSIS SPI driver control block address
 */
#ifdef EvtFsMcSPI_InitDriverError
  __STATIC_INLINE void EvrFsMcSPI_InitDriverError (uint32_t instance, uint32_t driver) {
    EventRecord2 (EvtFsMcSPI_InitDriverError, instance, driver);
  }
#else
  #define EvrFsMcSPI_InitDriverError(instance, driver)
#endif

/**
  \brief  Event on memory card driver power control error (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  driver    CMSIS SPI driver control block address
  \param[in]  state     CMSIS driver power state
 */
#ifdef EvtFsMcSPI_InitDriverPowerError
  __STATIC_INLINE void EvrFsMcSPI_InitDriverPowerError (uint32_t instance, uint32_t driver, uint32_t state) {
    EventRecord4 (EvtFsMcSPI_InitDriverPowerError, instance, driver, state, 0);
  }
#else
  #define EvrFsMcSPI_InitDriverPowerError(instance, driver, state)
#endif

/**
  \brief  Event on memory card device initialization (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_InitMedia
  __STATIC_INLINE void EvrFsMcSPI_InitMedia (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_InitMedia, instance, 0);
  }
#else
  #define EvrFsMcSPI_InitMedia(instance)
#endif

/**
  \brief  Event on successful memory card device initialization (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_InitMediaSuccess
  __STATIC_INLINE void EvrFsMcSPI_InitMediaSuccess (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_InitMediaSuccess, instance, 0);
  }
#else
  #define EvrFsMcSPI_InitMediaSuccess(instance)
#endif

/**
  \brief  Event on using uninitialized memory card device (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_MediaNotInitialized
  __STATIC_INLINE void EvrFsMcSPI_MediaNotInitialized (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_MediaNotInitialized, instance, 0);
  }
#else
  #define EvrFsMcSPI_MediaNotInitialized(instance)
#endif

/**
  \brief  Event on media detection using card detect switch (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_MediaCdActive
  __STATIC_INLINE void EvrFsMcSPI_MediaCdActive (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_MediaCdActive, instance, 0);
  }
#else
  #define EvrFsMcSPI_MediaCdActive(instance)
#endif

/**
  \brief  Event on card detect switch check and media is not present (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_MediaCdInactive
  __STATIC_INLINE void EvrFsMcSPI_MediaCdInactive (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_MediaCdInactive, instance, 0);
  }
#else
  #define EvrFsMcSPI_MediaCdInactive(instance)
#endif

/**
  \brief  Event on active write protection switch (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_MediaWpActive
  __STATIC_INLINE void EvrFsMcSPI_MediaWpActive (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_MediaWpActive, instance, 0);
  }
#else
  #define EvrFsMcSPI_MediaWpActive(instance)
#endif

/**
  \brief  Event on inactive write protection switch (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_MediaWpInactive
  __STATIC_INLINE void EvrFsMcSPI_MediaWpInactive (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_MediaWpInactive, instance, 0);
  }
#else
  #define EvrFsMcSPI_MediaWpInactive(instance)
#endif

/**
  \brief  Event on start of memory card device reset operation (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_MediaReset
  __STATIC_INLINE void EvrFsMcSPI_MediaReset (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_MediaReset, instance, 0);
  }
#else
  #define EvrFsMcSPI_MediaReset(instance)
#endif

/**
  \brief  Event on memory card device reset error (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_MediaResetError
  __STATIC_INLINE void EvrFsMcSPI_MediaResetError (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_MediaResetError, instance, 0);
  }
#else
  #define EvrFsMcSPI_MediaResetError(instance)
#endif

/**
  \brief  Event on start of SD memory card detection operation (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_MediaDetectionSD
  __STATIC_INLINE void EvrFsMcSPI_MediaDetectionSD (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_MediaDetectionSD, instance, 0);
  }
#else
  #define EvrFsMcSPI_MediaDetectionSD(instance)
#endif

/**
  \brief  Event on received response from SD memory card V2 (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_MediaSD_V2
  __STATIC_INLINE void EvrFsMcSPI_MediaSD_V2 (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_MediaSD_V2, instance, 0);
  }
#else
  #define EvrFsMcSPI_MediaSD_V2(instance)
#endif

/**
  \brief  Event on received response from SD memory card V2 (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  ocr       OCR register content
 */
#ifdef EvtFsMcSPI_MediaReady
  __STATIC_INLINE void EvrFsMcSPI_MediaReady (uint32_t instance, uint32_t ocr) {
    EventRecord2 (EvtFsMcSPI_MediaReady, instance, ocr);
  }
#else
  #define EvrFsMcSPI_MediaReady(instance, ocr)
#endif

/**
  \brief  Event on nonresponsive media device (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_MediaNoResponse
  __STATIC_INLINE void EvrFsMcSPI_MediaNoResponse (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_MediaNoResponse, instance, 0);
  }
#else
  #define EvrFsMcSPI_MediaNoResponse(instance)
#endif

/**
  \brief  Event on start of MMC/eMMC device detection procedure (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_MediaDetectionMMC
  __STATIC_INLINE void EvrFsMcSPI_MediaDetectionMMC (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_MediaDetectionMMC, instance, 0);
  }
#else
  #define EvrFsMcSPI_MediaDetectionMMC(instance)
#endif

/**
  \brief  Event on media device detection error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_MediaDetectionError
  __STATIC_INLINE void EvrFsMcSPI_MediaDetectionError (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_MediaDetectionError, instance, 0);
  }
#else
  #define EvrFsMcSPI_MediaDetectionError(instance)
#endif

/**
  \brief  Event on CID register read error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_CidReadError
  __STATIC_INLINE void EvrFsMcSPI_CidReadError (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_CidReadError, instance, 0);
  }
#else
  #define EvrFsMcSPI_CidReadError(instance)
#endif

/**
  \brief  Event on CSD register read error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_CsdReadError
  __STATIC_INLINE void EvrFsMcSPI_CsdReadError (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_CsdReadError, instance, 0);
  }
#else
  #define EvrFsMcSPI_CsdReadError(instance)
#endif

/**
  \brief  Event on failed block length setting (Error)
  \param[in]  instance  memory card control layer instance
  \param[in]  blen      block length
 */
#ifdef EvtFsMcSPI_BlenWriteError
  __STATIC_INLINE void EvrFsMcSPI_BlenWriteError (uint32_t instance, uint32_t blen) {
    EventRecord2 (EvtFsMcSPI_BlenWriteError, instance, blen);
  }
#else
  #define EvrFsMcSPI_BlenWriteError(instance, blen)
#endif

/**
  \brief  Event on SPI driver uninitialization (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  driver    CMSIS MCI driver control block address
 */
#ifdef EvtFsMcSPI_UninitDriver
  __STATIC_INLINE void EvrFsMcSPI_UninitDriver (uint32_t instance, uint32_t driver) {
    EventRecord2 (EvtFsMcSPI_UninitDriver, instance, driver);
  }
#else
  #define EvrFsMcSPI_UninitDriver(instance, driver)
#endif

/**
  \brief  Event on memory card device uninitialization (Op)
  \param[in]  instance  memory card control layer instance
 
 */
#ifdef EvtFsMcSPI_UninitMedia
  __STATIC_INLINE void EvrFsMcSPI_UninitMedia (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_UninitMedia, instance, 0);
  }
#else
  #define EvrFsMcSPI_UninitMedia(instance)
#endif

/**
  \brief  Event on sector read operation (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  sector    sector number
  \param[in]  count     number of sectors to read
 */
#ifdef EvtFsMcSPI_SectorRead
  __STATIC_INLINE void EvrFsMcSPI_SectorRead (uint32_t instance, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsMcSPI_SectorRead, instance, sector, count, 0);
  }
#else
  #define EvrFsMcSPI_SectorRead(instance, sector, count)
#endif

/**
  \brief  Event on sector read operation error (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  sector    sector number
  \param[in]  count     number of sectors to read
 */
#ifdef EvtFsMcSPI_SectorReadError
  __STATIC_INLINE void EvrFsMcSPI_SectorReadError (uint32_t instance, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsMcSPI_SectorReadError, instance, sector, count, 0);
  }
#else
  #define EvrFsMcSPI_SectorReadError(instance, sector, count)
#endif

/**
  \brief  Event on sector write operation (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  sector    sector number
  \param[in]  count     number of sectors to write
 */
#ifdef EvtFsMcSPI_SectorWrite
  __STATIC_INLINE void EvrFsMcSPI_SectorWrite (uint32_t instance, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsMcSPI_SectorWrite, instance, sector, count, 0);
  }
#else
  #define EvrFsMcSPI_SectorWrite(instance, sector, count)
#endif

/**
  \brief  Event on sector write operation error (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  sector    sector number
  \param[in]  count     number of sectors to write
 */
#ifdef EvtFsMcSPI_SectorWriteError
  __STATIC_INLINE void EvrFsMcSPI_SectorWriteError (uint32_t instance, uint32_t sector, uint32_t count) {
    EventRecord4 (EvtFsMcSPI_SectorWriteError, instance, sector, count, 0);
  }
#else
  #define EvrFsMcSPI_SectorWriteError(instance, sector, count)
#endif

/**
  \brief  Event on media capacity retrieval operation (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_ReadInfo
  __STATIC_INLINE void EvrFsMcSPI_ReadInfo (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_ReadInfo, instance, 0);
  }
#else
  #define EvrFsMcSPI_ReadInfo(instance)
#endif

/**
  \brief  Event on media capacity retrieval operation (Op)
  \param[in]  instance   memory card control layer instance
  \param[in]  sect_count capacity in number of sectors
 */
#ifdef EvtFsMcSPI_ReadInfoSuccess
  __STATIC_INLINE void EvrFsMcSPI_ReadInfoSuccess (uint32_t instance, uint32_t sect_count) {
    EventRecord2 (EvtFsMcSPI_ReadInfoSuccess, instance, sect_count);
  }
#else
  #define EvrFsMcSPI_ReadInfoSuccess(instance, sect_count)
#endif

/**
  \brief  Event on memory card device control operation (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  code      device control command code
  \param[in]  p         command code argument address
 */
#ifdef EvtFsMcSPI_DevCtrl
  __STATIC_INLINE void EvrFsMcSPI_DevCtrl (uint32_t instance, uint32_t code, uint32_t p) {
    EventRecord4 (EvtFsMcSPI_DevCtrl, instance, code, p, 0);
  }
#else
  #define EvrFsMcSPI_DevCtrl(instance, code, p)
#endif

/**
  \brief  Event on memory card device control code unsupported error (Error)
  \param[in]  instance  memory card control layer instance
  \param[in]  code      device control command code
 */
#ifdef EvtFsMcSPI_DevCtrlUnsupported
  __STATIC_INLINE void EvrFsMcSPI_DevCtrlUnsupported (uint32_t instance, uint32_t code) {
    EventRecord2 (EvtFsMcSPI_DevCtrlUnsupported, instance, code);
  }
#else
  #define EvrFsMcSPI_DevCtrlUnsupported(instance, code)
#endif

/**
  \brief  Event on Lock/Unlock command execution (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_LockUnlockExec
  __STATIC_INLINE void EvrFsMcSPI_LockUnlockExec (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_LockUnlockExec, instance, 0);
  }
#else
  #define EvrFsMcSPI_LockUnlockExec(instance)
#endif

/**
  \brief  Event on Lock/Unlock command execution error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_LockUnlockExecError
  __STATIC_INLINE void EvrFsMcSPI_LockUnlockExecError (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_LockUnlockExecError, instance, 0);
  }
#else
  #define EvrFsMcSPI_LockUnlockExecError(instance)
#endif

/**
  \brief  Event on active password protection status (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_LockActive
  __STATIC_INLINE void EvrFsMcSPI_LockActive (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_LockActive, instance, 0);
  }
#else
  #define EvrFsMcSPI_LockActive(instance)
#endif

/**
  \brief  Event on deactivated password protection status (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_LockDeactivated
  __STATIC_INLINE void EvrFsMcSPI_LockDeactivated (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_LockDeactivated, instance, 0);
  }
#else
  #define EvrFsMcSPI_LockDeactivated(instance)
#endif

/**
  \brief  Event on SPI bus speed control operation (Op)
  \param[in]  instance  memory card control layer instance
  \param[in]  speed     bus speed in Hz
 */
#ifdef EvtFsMcSPI_BusSpeedControl
  __STATIC_INLINE void EvrFsMcSPI_BusSpeedControl (uint32_t instance, uint32_t speed) {
    EventRecord2 (EvtFsMcSPI_BusSpeedControl, instance, speed);
  }
#else
  #define EvrFsMcSPI_BusSpeedControl(instance, speed)
#endif

/**
  \brief  Event on missing \ref fs_mc_spi_control_ss function (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_SlaveSelCtrlMissing
  __STATIC_INLINE void EvrFsMcSPI_SlaveSelCtrlMissing (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_SlaveSelCtrlMissing, instance, 0);
  }
#else
  #define EvrFsMcSPI_SlaveSelCtrlMissing(instance)
#endif

/**
  \brief  Event on missing \ref fs_mc_read_wp function (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_ReadWpMissing
  __STATIC_INLINE void EvrFsMcSPI_ReadWpMissing (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_ReadWpMissing, instance, 0);
  }
#else
  #define EvrFsMcSPI_ReadWpMissing(instance)
#endif

/**
  \brief  Event on missing \ref fs_mc_read_cd function (Op)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_ReadCdMissing
  __STATIC_INLINE void EvrFsMcSPI_ReadCdMissing (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_ReadCdMissing, instance, 0);
  }
#else
  #define EvrFsMcSPI_ReadCdMissing(instance)
#endif

/**
  \brief  Event on CRC check disable error (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_CrcDisableError
  __STATIC_INLINE void EvrFsMcSPI_CrcDisableError (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_CrcDisableError, instance, 0);
  }
#else
  #define EvrFsMcSPI_CrcDisableError(instance)
#endif

/**
  \brief  Event on SPI driver Send function error (Error)
  \param[in]  instance  memory card control layer instance
  \param[in]  event     SignalEvent function event flags
 */
#ifdef EvtFsMcSPI_DriverSendError
  __STATIC_INLINE void EvrFsMcSPI_DriverSendError (uint32_t instance, uint32_t event) {
    EventRecord2 (EvtFsMcSPI_DriverSendError, instance, event);
  }
#else
  #define EvrFsMcSPI_DriverSendError(instance, event)
#endif

/**
  \brief  Event on SPI driver Receive function error (Error)
  \param[in]  instance  memory card control layer instance
  \param[in]  event     SignalEvent function event flags
 */
#ifdef EvtFsMcSPI_DriverReceiveError
  __STATIC_INLINE void EvrFsMcSPI_DriverReceiveError (uint32_t instance, uint32_t event) {
    EventRecord2 (EvtFsMcSPI_DriverReceiveError, instance, event);
  }
#else
  #define EvrFsMcSPI_DriverReceiveError(instance, event)
#endif

/**
  \brief  Event on command send error (Error)
  \param[in]  instance  memory card control layer instance
  \param[in]  cmd       command index
  \param[in]  arg       command argument
 */
#ifdef EvtFsMcSPI_CmdSendError
  __STATIC_INLINE void EvrFsMcSPI_CmdSendError (uint32_t instance, uint32_t cmd, uint32_t arg) {
    EventRecord4 (EvtFsMcSPI_CmdSendError, instance, cmd, arg, 0);
  }
#else
  #define EvrFsMcSPI_CmdSendError(instance, cmd, arg)
#endif

/**
  \brief  Event on missing or invalid command response (Error)
  \param[in]  instance  memory card control layer instance
  \param[in]  cmd       command index
 */
#ifdef EvtFsMcSPI_CmdResponseError
  __STATIC_INLINE void EvrFsMcSPI_CmdResponseError (uint32_t instance, uint32_t cmd) {
    EventRecord2 (EvtFsMcSPI_CmdResponseError, instance, cmd);
  }
#else
  #define EvrFsMcSPI_CmdResponseError(instance, cmd)
#endif

/**
  \brief  Event on missing or invalid application command response (Error)
  \param[in]  instance  memory card control layer instance
  \param[in]  cmd       command index
 */
#ifdef EvtFsMcSPI_ACmdResponseError
  __STATIC_INLINE void EvrFsMcSPI_ACmdResponseError (uint32_t instance, uint32_t cmd) {
    EventRecord2 (EvtFsMcSPI_ACmdResponseError, instance, cmd);
  }
#else
  #define EvrFsMcSPI_ACmdResponseError(instance, cmd)
#endif

/**
  \brief  Event on invalid function parameter(s) (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_ParameterInvalid
  __STATIC_INLINE void EvrFsMcSPI_ParameterInvalid (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_ParameterInvalid, instance, 0);
  }
#else
  #define EvrFsMcSPI_ParameterInvalid(instance)
#endif

/**
  \brief  Event on detection of enabled media password protection (Error)
  \param[in]  instance  memory card control layer instance
 */
#ifdef EvtFsMcSPI_MediaPasswordEnabled
  __STATIC_INLINE void EvrFsMcSPI_MediaPasswordEnabled (uint32_t instance) {
    EventRecord2 (EvtFsMcSPI_MediaPasswordEnabled, instance, 0);
  }
#else
  #define EvrFsMcSPI_MediaPasswordEnabled(instance)
#endif

#endif /* FS_EVR_H__ */
