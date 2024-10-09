# Revision History {#rev_hist}

<table class="cmtable" summary="Revision History">
    <tr>
      <th>Version</th>
      <th>Description</th>
    </tr>
    <tr>
      <td>V8.0.0</td>
      <td>
        - removed CORE component variants
        - changed source config files to headers
        - removed support for legacy Arm Compiler 5
        - removed support for CMSIS-RTOS version 1
        - removed support for legacy fdelete function
        - fixed allocation table write access (regression) when using FAT12
        - fixed FAT volume label character checking
        - corrected ffind error code for FAT on repeated search without wildcard
      </td>
    </tr>
    <tr>
      <td>V6.16.6</td>
      <td>
        - using Event Recorder component from CMSIS-View pack (instead of deprecated ARM_Compiler pack)
        - using File Interface API from CMSIS-Compiler pack (instead of deprecated ARM_Compiler pack)
        - added File Interface implementation (CMSIS-Compiler:File Interface:MDK-MW File System)
        - updated FS_Config.c configuration file (clean-up)
        - fixed buffer alignment passed to MCI driver when using fsDevCtrlCodeLockUnlock control code
        - fixed FAT journaling interference with Android OS
      </td>
    </tr>
    <tr>
      <td>V6.15.3</td>
      <td>
        - enforced push-pull CMD bus mode for SD cards in native bus mode
        - prevent CMSIS-Driver Flash ProgramData call with cnt equal to zero
        - removed unnecessary value display from component view
      </td>
    </tr>
    <tr>
      <td>V6.15.0</td>
      <td>
        - libraries built with Arm Compiler 6
      </td>
    </tr>
    <tr>
      <td>V6.14.4</td>
      <td>
        - enhance memory card error handling after failure to switch to high speed mode
        - fixed regression in EFS when Flash driver ProgramData returns with zero
      </td>
    </tr>
    <tr>
      <td>V6.14.1</td>
      <td>
        - added health status (S.M.A.R.T.) access support for industrial grade eMMC and SD devices (see \ref mci_device_specifics)
        - enhanced error handling and return status propagation in EFS
        - corrected bug in \ref ftime_set which could cause long file name entry overwrite
        - corrected bug in FAT name cache which could cause a cache miss situation when switching directories using \ref fchdir
      </td>
    </tr>
    <tr>
      <td>V6.13.8</td>
      <td>
        - corrected bug in \ref fwrite for EFS which could cause a write error in multithreaded environment
        - corrected bug in \ref fdelete for EFS which could cause that file could not be deleted
      </td>
    </tr>
    <tr>
      <td>V6.13.6</td>
      <td>
        - corrected bug in FAT name cache which could cause a cache miss situation on file entry scan
        - corrected bug in FAT name cache which could cause a long file name entry overwrite
        - corrected bug in \ref fchdir for FAT which prevented chdir into single character folder
        - corrected bug in \ref fwrite for EFS which could cause invalid file allocation order
        - corrected bug in \ref fseek for EFS which caused invalid setting of the file position pointer
        - minor optimizations and fixes in \ref fdefrag (EFS only)
      </td>
    </tr>
    <tr>
      <td>V6.13.0</td>
      <td>
        - added \ref ftime_set and \ref ftime_get functions used to manage the file or directory timestamps
        - added status codes \ref fsAlreadyExists and \ref fsNotDirectory that allow more control when using \ref fmkdir, \ref frmdir and \ref fchdir 
        - added function \ref fversion used to retrieve the version of the FileSystem component
        - enhanced error reporting for FileSystem functions used by the \ref stdio_routines (use errno to retrieve last error code)
        - corrected bug in \ref frmdir when using option /S and name cache which could cause a file to become inaccessible after rename operation
      </td>
    </tr>
    <tr>
      <td>V6.12.0</td>
      <td>
        - added second instance of the RAM drive (drive R1)
        - corrected bug in FAT file name compare for names with equal root and 13 characters in length (LFN)
        - corrected bug in FAT cluster handling when deleting file which could cause false out of space error
        - corrected bug in fmedia which could cause non-initialized pointer access when using uninitialized drive
      </td>
    </tr>
    <tr>
      <td>V6.11.0</td>
      <td>
        - added debug configuration for debugging with Event Recorder
      </td>
    </tr>
    <tr>
      <td>V6.10.1</td>
      <td>
        - minor change in Flash driver timeout handling (EFS)
      </td>
    </tr>
    <tr>
      <td>V6.10.0</td>
      <td>
        - added support for NAND on-chip ECC (EZ NAND)
        - SPI slave select pin reconfigured as unused in CMSIS SPI driver when using \ref fs_mc_spi_control_ss callback function
        - limited minimum path buffer length in \ref fpwd to target file system maximum path length
        - enhanced debug messaging for Event Recorder
        - corrected bug in EFS defragmenter which could cause write access to undefined area
        - corrected bug in NFTL which could cause file system inconsistency due to the ECC error
        - corrected bug in FAT file name compare for names containing multiple of 13 characters (LFN)
        - corrected compilation errors when using CMSIS RTOS compatibility module
        - corrected FAT file handle display in debug view
      </td>
    </tr>
    <tr>
      <td>V6.9.8</td>
      <td>
        - added \ref mc_spi_bus_freq_setting "bus frequency adaptation algorithm" for memory cards using SPI bus
        - corrected bug in EFS triggered by multiple file append or small file create operation.
        - corrected error reporting in \ref ferror when removable FAT drive was removed during read operation
        - corrected file access bug in FAT which could be triggered when an empty file was created within a subfolder
        - enhancements in debug messaging for Event Recorder
      </td>
    </tr>
    <tr>
      <td>V6.9.4</td>
      <td>
        - enhanced \ref finit and \ref funinit robustness
        - corrected \ref finfo parameter checking and behavior when drive is unmounted.
        - corrected newname parameter specification checking for FAT drives in \ref frename function
        - corrected FAT label specification checking in \ref fformat function
        - corrected \ref ffind returning invalid results when using *xxx pattern
        - corrected bug in FAT name cache which could be triggered when drive was empty
      </td>
    </tr>
    <tr>
      <td>V6.9.0</td>
      <td>
        - added debug events
        - enhanced eMMC initialization process robustness
        - corrected \ref ffree execution status return value
        - corrected FAT file write on attempt to write beyond 4GB
        - corrected mutex release issue in \ref ffind which could resulted in wait for mutex lockup
        - corrected FAT second timestamping in [32-59] seconds range
        - corrected bug in FAT name cache which could be triggered at file delete
      </td>
    </tr>
    <tr>
      <td>V6.8.0</td>
      <td>
        - added debug capabilities
        - added uniform flash sector support on Embedded File System
        - corrected setting of archive bit when modifying file in FAT file system
        - enhanced FAT-32 mount speed for a fresh formatted removable media devices
        - enhanced compatibility with ARM Compiler 6
      </td>
    </tr>
    <tr>
      <td>V6.7.0</td>
      <td>
        - enhanced \ref fmedia functionality when using memory cards without card detection switch
        - corrected bug in \ref fchdir function when using ".." string to change current directory to root
        - corrected bug in Embedded File System when reading from unaligned flash addresses
        - enabled \ref ioc_interface_routines on RAM drive
        - corrected issue in \ref fdefrag function which caused invalid data write
        - corrected initialization process for certain 2GB eMMC devices
      </td>
    </tr>
    <tr>
      <td>V6.6.0</td>
      <td>
        - added support for memory card lock/unlock command (CMD42), please see \ref fs_ioc_device_ctrl for control codes and
          \ref mci_device_specifics "Device Specific Features" for description of password management operations.
        - enhanced handling of Memory Cards when in SPI mode
        - corrected initialization process for SD cards in SPI mode
        - corrected end of file condition check in \ref fread for files sized near 4GB
        - corrected initialization issues in \ref fdefrag function
        - corrected write protect status handling
        - corrected fsDevCtrlCodeControlMedia handling on NAND drives
      </td>
    </tr>
    <tr>
      <td>V6.5.0</td>
      <td>
        - corrected issues on RAM drive, when storage memory is below 8MB
        - corrected MCI bus clock handling: bus clock is disabled when bus communication is idle
        - corrected reserved area clearing when formatting FAT file system
        - enhanced error handling for MCI memory devices
        - increased Memory Card Transfer Timeouts
      </td>
    </tr>
    <tr>
      <td>V6.4.0</td>
      <td>
        - corrected fpwd memory overwrite bug
        - corrected fmount issue when switching between I/O Control Interface and Standard I/O Routines
        - corrected funinit procedure to power off the peripheral driver before uninitializing it
      </td>
    </tr>
    <tr>
      <td>V6.3.0</td>
      <td>
        - retarget system updated:
          - "ARM Compiler" pack is now required to handle File System retargeting
          - "Compiler I/O: File (File System)" software component must be enabled
        - fixed reliability issues in multithreaded environment
        - added support for NAND Sequence operation
        - corrected fflush() for FAT drives
        - added support for eMMC Sleep State (CMD5), please see \ref fs_ioc_device_ctrl for control codes
        - added \ref fsUnsupported return code, which designates unsupported operations and features
        - added \ref fs_mc_spi_control_ss callback function
        - reduced number of write accesses when closing a file on FAT file system
        - added support for auto switch of SD card to high speed mode using CMD6
        - added \ref fsDevCtrlCodeControlMedia device control code
        - corrected drive prefix handling in fchdir, fmkdir and frmdir functions
        - corrected fpwd bug for FAT32 which caused fpwd return status fsFileNotFound
        - corrected fchdir status code when current directory set to root directory
        - corrected fwrite and fclose behavior on Embedded File System when disk full
        - corrected \ref fdelete and \ref frename to return with \ref fsAccessDenied if specified file is not closed
      </td>
    </tr>
    <tr>
      <td>V6.2.0</td>
      <td>
        - added support for MMCA System Specification V4.1 and higher (eMMC)
        - current drive and current directory support added, see \ref fchdrive, \ref fchdir and \ref fpwd functions
        - added \ref fmkdir and \ref frmdir functions
        - \ref fdelete function API and behavior changed to common concept:
          - function is now only able to delete files (does not delete directories, use \ref frmdir instead)
          - additional parameter "options" was added
          - define MW_FILESYSTEM_FDELETE_LEGACY in the project to use the old function API and behavior
        - configuration files changed:
          - "FAT Name Cache Size" option renamed to "Filename Cache Size" and moved from core configuration file to FAT drive configuration files
          - "Initial Current Drive" option added to core configuration file
          - "Default Drive" option removed from drive configuration files
        - corrected FAT mount problem on large removable devices (>8GB) with 2k allocation unit size
        - write performance improved for SD cards in native mode
        - corrected fs_lock/fs_unlock locking mechanism
        - enhanced drive initialization and mount procedures
        - enhanced error reporting and added more restrictive parameter checking
      </td>
    </tr>
    <tr>
      <td>V6.1.0</td>
      <td> 
        - corrected multithread file write issue on Embedded File System which caused corruption of the file system
        - corrected \ref fseek function when seeking beyond file size (FAT File System)
        - corrected \ref frename function issue when using FAT name cache: after rename it was still possible
          to open the file using its old name.
        - corrected timestamp handling when file size is unchanged after write access (FAT File System)
      </td>
    </tr>
    <tr>
      <td>V6.0.0</td>
      <td> 
        - adapted to CMSIS-Driver V2 interface
        - added \ref fs_mc_read_cd and \ref fs_mc_read_wp callback functions
        - deprecated support for OneNAND
      </td>
    </tr>
    <tr>
      <td>V5.1.4</td>
      <td>
        - FAT filename processing bug fixes
      </td>
    </tr>
    <tr>
      <td>V5.1</td>
      <td>
        - added \ref fmount, \ref funmount, \ref fmedia and \ref finfo functions
      </td>
    </tr>
    <tr>
      <td>V5.0</td>
      <td>
        - Initial release for MDK-ARM Version 5.
      </td>
    </tr>
 </table>
