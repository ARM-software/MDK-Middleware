# Theory of Operation {#operation}

The File System Component provides functions to create, save, read, and modify files.
The Retarget Interface of the C/C++ compiler run-time library allows the usage of
ANSI C standard file I/O functions. The File System Component is designed for Cortex-M
processor-based devices and requires an CMSIS-RTOS compliant real-time operating system
for thread-safe operation.

The File System Component consists of two different file systems, placed on top of different layers, depending on the media type:

- \ref fat_fs (using FAT12, FAT16, or FAT32) is used for \ref mc_usage "memory card" devices,
  \ref usb_usage "USB Memory sticks", \ref nand_usage "NAND Flash" devices, and \ref ram_usage "RAM disks". FAT is a standard
  file system used on many computers.
- \ref emb_fs is optimized for \ref nor_usage "NOR Flash" memory devices.
- \ref journaling_fat is an extension to the \ref fat_fs that helps to keep the file system consistent in an embedded
  system.
- The \ref mc_control_layer is used to control SDxC/MMC/eMMC memory devices.
- The \ref nand_flash_TL provides means of wear leveling, bad block management, and error correction on
  \ref nand_usage "NAND Flash" devices.

## Using the File System {#fs_usage}

The File System Component integrates with the Standard C/C++ Library and requires a CMSIS-RTOS2 compliant RTOS.
It cannot be used with the Standard C/C++ Library that does not provide the (retarget) hooks for I/O file handling.

The following picture shows the File System's structure from a developer's perspective.

![File System Component Structure](fs_structure.png)

- **System, File Management**: functions that manage the File System and provide operation to format a drive and manage files and directories.
- **Standard File I/O**: functions to perform input and output operations on files, such as read, write, and seek.
- **ARM Standard Run-Time Library**: is the ARM Compiler standard C library with functions to manage file.
- **FS_Config.c**: configuration file for general characteristics of the file system.
- **FS_Config_xxx_n.h**: configuration file for the characteristics of each drive or media (MC, NAND, NOR, RAM, or USB memory sticks).
- **File System Core**: handles low-level input and output file operations (some are re-targeted to use the ARM Standard
  Run-Time Library). Depending on configuration settings, it uses the appropriate file system (FAT or EFS) and implements
  the NAND Flash Translation Layer.
- **IOC - FAT Media API**: are I/O Control Interface Routines for the FAT file system to access physical sectors.
- **FAT, EFS Media**: the FAT file system supports Memory Cards (MC), NAND Flash, and USB memory sticks, and RAM disks. The Embedded File System (EFS) supports NOR Flash devices.
- **Drivers** The File System Core accesses the drives via CMSIS-Drivers that are typically part of the Device Family Pack. Every drive uses dedicated driver.

The configuration steps and files that are needed to create applications for the different devices are explained here:

- \ref nor_usage
- \ref ram_usage
- \ref mc_usage
- \ref usb_usage
- \ref nand_usage

## Drives, Memory Devices and Drivers {#drive}

The File System Component supports various memory and storage devices that are assigned to a **drive**.

![File System Component: Media types and Drives](fs_types.png)

All available drives are defined with the \ref RTE_Component_Selection. A drive is passed as string to
\ref system_routines and may be part of a filename. The system allows you to configure a current drive that is used when no
drive is specified as part of the filename.

The file system type is automatically chosen depending on the drive type (which
correlates to a certain memory device). Some drives may use different interfaces that will be covered by appropriate
CMSIS-Drivers. The following table shows the relationship between drive - memory device - and driver/interface:

|Drive        |File System       |Drive Type                          | CMSIS-Driver/Interface|
|:------------|:----------------:|:-----------------------------------|-----------------------|
|"F:" or "F0:"|\ref emb_fs "EFS" |NOR Flash drive 0                   | \urlout{Flash-API} (Memory Bus or \urlout{SPI-API}) |
|"F1:"        |\ref emb_fs "EFS" |NOR Flash drive 1                   | \urlout{Flash-API} (Memory Bus or \urlout{SPI-API}) |
|"R:" or "R0:"|\ref fat_fs "FAT" |RAM drive                           | N/A |
|"M:" or "M0:"|\ref fat_fs "FAT" |Memory Card drive 0                 | \urlout{MCI-API} or \urlout{SPI-API} |
|"M1:"        |\ref fat_fs "FAT" |Memory Card drive 1                 | \urlout{MCI-API} or \urlout{SPI-API} |
|"U:" or "U0:"|\ref fat_fs "FAT" |USB Memory Stick drive 0            | \urlout{USB-API}                     |
|"U1:"        |\ref fat_fs "FAT" |USB Memory Stick drive 1            | \urlout{USB-API}                     |
|"N:" or "N0:"|\ref fat_fs "FAT" |NAND Flash drive 0                  | \urlout{NAND-API} (Memory Bus)       |
|"N1:"        |\ref fat_fs "FAT" |NAND Flash drive 1                  | \urlout{NAND-API} (Memory Bus)       |

> **Note**
>
> - NOR Flash is typically connected over Memory Bus or SPI. For using a NOR Flash drive, a Flash driver is required which
>   encapsulates either the Memory Bus interface or SPI (in which case an SPI driver required in addition).
> - The Middleware Component contains example drivers for Flash devices that can be used to implement other device drivers if
>   required. The examples are:
>   AM29x800BB/[M29W640FB](https://www.farnell.com/datasheets/40281.pdf)
>   (Flash with 16-bit memory bus interface),
>   [S29GL064Nx2](https://www.infineon.com/dgdl/Infineon-S29GL064N_S29GL032N_64_Mbit_32_Mbit_3_V_Page_Mode_MirrorBit_Flash-DataSheet-v03_00-EN.pdf) (Flash with 32-bit
>   memory bus interface), and
>   [AT45DB642D](https://datasheet.octopart.com/AT45DB642D-CNU-Atmel-datasheet-9652374.pdf) (Flash with SPI interface using an SPI
>   driver).
> - Other drives (memory card, USB memory device, NAND Flash) require appropriate CMSIS-Driver for the microcontroller
>   device. If no suitable CMSIS-Driver is available for your selected device, you may create the required Flash interface
>   driver. Refer to the \urlout{CMSIS-Driver-Ref} for details.
> - Currently, using NAND Flash devices with SPI interface (Serial NAND) *is not supported*.

### Current Drive {#cur_sys_drive}

The **Current Drive** defines the drive used when the filename itself does not specify the drive
or when functions that expect a drive are called with an empty ("") string.

If there is only one drive enabled in the system then it automatically becomes the "Current Drive". Alternatively,
when there are multiple drives enabled, the one with the drive letter that is first in alphabetical order is
set as the "Current Drive".
The \ref cur_sys_drive "Current Drive" can be changed during run-time using the function \ref fchdrive.

> **Note**
>
> - Define `FS_INITIAL_CDRIVE` can be used to define initial current drive at build time.
> - Set `FS_INITIAL_CDRIVE` to one of the following values: 0(F0), 1(F1), 2(M0), 3(M1), 4(N0), 5(N1), 6(R0), 9(R1), 7(U0), 8(U1)

## FAT File System {#fat_fs}

The FAT File System was initially used on PC operating systems such as MS-DOS and early versions of Microsoft Windows. Still,
it is widely used on **USB memory sticks** or **memory cards** for file storage. It is simple, robust and offers
good performance especially in embedded applications.

FAT is an acronym for **File Allocation Table**. This table provides the index of the files in the system and is statically
allocated at the time of formatting the drive. It contains an entry for each cluster (a data storage area).
The FAT's entries contain either the number of the next cluster in the file, or a marker indicating EOF (end of file), unused
disk space, or other areas of the drive that are specially reserved. The drive's root directory contains the number of the
first cluster of each file in the directory; the operating system traverses the FAT table, looking up the cluster
number of each successive part of the file, until the end of the file is reached. Likewise, sub-directories are implemented
as special files containing the directory entries of their respective files.

Since the first implementation, the maximum number of clusters of a drive has increased dramatically, and so the number of
bits used to identify each cluster has grown. The versions of the FAT format are named after the number of table element
bits: **FAT12**, **FAT16**, and **FAT32**. Each of these variants is is supported by the File System Component.

### Layout {#fat_layout}

The actual FAT file system is made up of four different sections:

|Section              |Description                                                                                        |
|:--------------------|:--------------------------------------------------------------------------------------------------|
|Boot sector          |Contains machine startup code                                                                      |
|File Allocation Table|Contains the data region map; multiple entries might exist for redundancy purposes                 |
|Root Directory       |Stores information about the files and directories located in the root directory (FAT12/FAT16 only)|
|File Data Region     |Actual stored file data                                                                            |

### Volume name {#fat_volume_label}

Volume label for FAT12, FAT16 or FAT32 volume must be maximum 11 characters long and cannot contain
the * ? / \ | , ; : + = < > [ ] \" characters.

### Path name handling {#fat_path_handling}

Path name format used to specify either a drive, directory or file:

- **[drive:][directory separator][path]**

Depending on the usage case, each specifier can be optional:

- drive specifier must consist of drive letter followed by optional drive number and a semicolon.
- directory separator can be slash (/) or backslash (\\)
- path can consist of multiple directory names and a file name

#### Absolute and relative path {#fat_abs_rel_path}

The table below explains how to specify either absolute or relative path.

|Path name       | Type     | Description                                          |
|:---------------|:---------|:-----------------------------------------------------|
|\               | Absolute | Root directory                                       |
|\\ ..           | Absolute | Root directory                                       |
|.               | Relative | Current directory                                    |
|..              | Relative | Parent directory                                     |
|M:\\file.txt    | Absolute | A file in root directory of the drive M0             |
|M:file.txt      | Relative | A file in current directory of the drive M0          |
|\\file.txt      | Absolute | A file in the root directory  of the current drive   |
|file.txt        | Relative | A file in the current directory of the current drive |
|folder1\\ ..    | Relative | Current directory                                    |

### Long and Short File Name handling {#lfn_sfn_handling}

File System Component is provided with the long (LFN) and short (SFN or 8.3) filename support.
Since there is no compulsory algorithm for creating the 8.3 name from an LFN, File System
Component uses convention described below.

- The name in uppercase or lowercase, which is 8.3 valid, is stored in uppercase in a single SFN entry
  - Example: "TEXTFILE.TXT" is stored as "TEXTFILE.TXT" (SFN)
  - Example: "textfile.txt" is stored as "TEXTFILE.TXT" (SFN)

- A mixed-case name, which is 8.3 valid, is stored as mixed-case in LFN entry and SFN entry is created,
  which is concluded with tilde and a numeric value.
  - Example: "TextFile.txt" is stored as "TextFile.txt" (LFN) and "TEXTFI~1.TXT" (SFN)

- Name which is not 8.3 valid is stored in LFN entry and SFN entry. Name written in SFN entry
  is in uppercase, stripped of invalid 8.3 characters, which are replaced with underscore "_" and
  concluded with tilde and a numeric value.
  - Example: "Tex+File.txt" is stored as "Tex+File.txt" (LFN) and "TEX_FI~1.TXT (SFN)"

### Time Stamping Support {#fat_time_stamping}

The File System Component fully supports time stamping for files and directories:

- Create time
- Create date
- Last access date
- Write time
- Write date

To be able to use the time information provided by an on-chip RTC, the user application needs to implement the function
\ref fs_get_time(). The \ref fsTime structure must be filled with the data returned from the RTC driver. If the
\ref fs_get_time() function is not implemented in the user application (which is the default), predefined time and date
stamps are used.

### Journaling for the FAT File System {#journaling_fat}

The File System Component supports a proprietary
[journaling](https://en.wikipedia.org/wiki/Journaling_file_system) mechanism for FAT
which makes file I/O functions robust against system crashes or power failures. When FAT Journaling is enabled, the file
allocation table and directory entries are protected.

**FAT Journaling** keeps track of the changes in the file system using a **journal** before committing them to the
main file system. In the event of a system failure, file system does not become corrupted. Only content of files that
are opened in writing mode may be lost. Other files are not affected.

Journaling can be enabled individually for each drive that is using the FAT File System.
For information on how to enable Journaling refer to:

- \ref mc_usage
- \ref usb_usage
- \ref nand_usage

#### System Initialization and Drive Formatting {#sys_init_drv_format}

There requirements for creating a **journal** are:

- the drive must be formatted with two FAT tables (two consecutive tables in the FAT section of the drive)
- the drive must have at least 64 sectors (= 32kB) of contiguous free space at the end of a partition (in the last 1% space).

If journaling is enabled, mounting a drive triggers the File System to search for the journal.

When journal is found the journaling system is initialized and the journal content is checked for any
incomplete data transactions that may be the result of a system crash. When incomplete data transactions
are found the file system restores the drive to the last known valid state.

When no journal is found, the journal is automatically created.
The required space (32kB) for journal is reserved by marking the adequate amount of clusters
in the last 1% of the drive partition. This reduces the reported amount of free space on the drive accordingly.

No special drive formatting is required. When a drive is formatted on a different computer
(running Window or Linux) free space is typical available at the end of a partition.
Creation of a journal may fail on removable media when above requirements are not meet.
This may happen if the drive is fragmented or there is not enough space. In this case the
drive should be defragmented or some files should be deleted.

When using the \ref fformat function of the File System Component, the journal is automatically created on drives with
enabled option "Use FAT Journal".

#### System failure behavior {#sys_fail_beh}

When File System Component experiences a power loss or crashes before a file opened for writing is
successfully closed the content of that file is lost. This results in lost data clusters which
can be restored using a file system repair utility tool such as chkdsk on Windows or fsck on Linux.

### FAT System Design Limitations {#fat_sys_design_limitations}

By design, FAT carries a few limitations:

- Maximum file size is limited to 4,294,967,295 bytes
- Maximum files within folder is 65,536 (i.e. a directory must not be larger than 2,097,152 bytes).
- A root directory on FAT12 and FAT16 can hold max 512 files/folders (there is no such limitation for FAT32)
- FAT type depends on maximum and minimum number of clusters:
  - FAT12 maximum clusters: 4,084
  - FAT16 minimum clusters: 4,085
  - FAT16 maximum clusters: 65,524
  - FAT32 minimum clusters: 65,525
- Maximum volume size depends on FAT type:
  - FAT12: 256MB (for 64kB clusters)
  - FAT16: 4GB (for 64kB clusters)
  - FAT32: 2TB (4G of 512B sectors)

### FAT Implementation Limitations {#fat_imp_limitations}

The actual FAT implementation of the File System Component has the following limitations:

- Multiple partitions are not supported and will not be detected.
- Multiple active file handles per file are allowed only for files opened in read mode.
- Maximum path length is 260 bytes for FAT with LFN support and 80 bytes for FAT without LFN support.
- Seeking within files is limited to the area of two gigabytes due to the \ref fseek and \ref ftell limitation set by ANSI C Standard library.
- UTF-16 and UTF-8 character encoding is not supported.

## Memory Card Control Layer {#mc_control_layer}

The **Memory Card (MC) Control Layer** is used when connecting to external memory cards or eMMC devices. It supports data
transfer via MCI or SPI, but the data rates are much better using MCI.

### List of Supported Devices {#mci_list_of_devices}

In general, the following devices are supported by the File System Component:

|Memory Card | Specification|
|------------|--------------|
|[Standard Capacity SD Memory Card (SDSC)](https://en.wikipedia.org/wiki/Secure_Digital#SD)  | Up to and including 2 GB                  |
|[High Capacity SD Memory Card (SDHC)](https://en.wikipedia.org/wiki/Secure_Digital#SDHC)    | More than 2GB and up to and including 32GB|
|[Extended Capacity SD Memory Card (SDXC)](https://en.wikipedia.org/wiki/Secure_Digital#SDXC)| More than 32GB and up to and including 2TB|
|[MultiMediaCard (MMC)](https://en.wikipedia.org/wiki/MultiMediaCard)                        | Up to specification version 4.2           |
|[Embedded MultiMediaCard (eMMC)](https://en.wikipedia.org/wiki/MultiMediaCard#eMMC)         | From specification 4.3                    |

### List of Supported MCI Driver Capabilities {#mci_list_of_mci_capabilities}

MC Control Layer uses \urlout{MCI-API} when memory device is connected via MCI. Depending on the \urlout{ARM-MCI-Capabilities},
File System Component can take advantage of certain features. The following table briefly describes how enabled capabilities affect
the functionality of the MC Control Layer:

|Capability  |Functionality|
|:----------:|-----------|
|cd_state    |Enables device presence detection by pooling the Card Detect (CD) switch state |
|cd_event    |Enables device presence detection using MCI driver events |
|wp_state    |Enables write protection detection by pooling Write Protect (WP) switch state |
|vdd         |Power consumption can be controlled by turning VDD (VCC) power supply on or off |
|vccq        |Power consumption can be controlled by turning VCCQ power supply on or off |
|data_width_4|Data transfer rate can be increased by using 4-bit data bus |
|data_width_8|Data transfer rate can be increased by using 8-bit data bus |
|high_speed  |Data transfer rate can be increased by using MCI bus frequencies up to 52MHz |
|rst_n       |eMMC device can be reset from software using RST_n pin |

### Device Specific Features {#mci_device_specifics}

#### Power Supply Voltage Control

With appropriately designed hardware and support from the corresponding MCI driver, MC Control Layer can control memory
device power supply voltage:

- VDD (VCC) and VCCQ power supply is turned **on** when device is \ref fmount "mounted"
- VDD (VCC) and VCCQ power supply is turned **off** when device is \ref funmount "unmounted",
- VCC power supply is turned **on/off** when eMMC device is **switched to/from Sleep state**

#### Data Bus Width

Depending on the memory device type, appropriate data bus width will be selected:

- 1-bit data bus width is selected by default, since it must be supported by all devices and MCI drivers.
- 4-bit or 8-bit data bus width selection depends on both, device and MCI driver capabilities. In order to switch either
  to 4 or 8-bit data bus, both, device and MCI driver must support it.

#### High Speed Mode

The MC Control Layer is able to automatically switch to
[high speed mode](https://www.sdcard.org/developers/sd-standard-overview/bus-speed-default-speed-high-speed-uhs-sd-express/) for SD cards
supporting SD specification version 1.10 or higher (3.3V signaling, bus clock frequency up to 50MHz) and MMC/eMMC devices
supporting MMCA specification version 4.2 or higher (3.3V signaling, bus clock frequency up to 52MHz). In order to
do so, the high speed mode needs to be supported by the corresponding \urlout{MCI-API}.

\note Improper PCB design can cause reliability problems when in high speed mode.

#### Password protection

The password protection feature enables the host to lock a card while providing a password, which later will be used for
unlocking the card. The password is stored to device internal non-volatile registers and can consist of up to 16 bytes,
where each byte can be arbitrary 8-bit value.

Password can be managed by using the function \ref fs_ioc_device_ctrl, with control code \ref fsDevCtrlCodeLockUnlock and
structure \ref fsLockUnlock as control code argument. When executing management operation, members of the \ref fsLockUnlock
structure must be set as follows:

**Setting the password:**

- assign password buffer to member `.password`
- define password length by setting member `.length`
- define set password operation by setting member `.flags` to \ref FS_DEVICE_SET_PASSWORD

When operation executes successfully, new password is stored into device registers.

**Reset the password:**

- assign password buffer to member `.password`
- define password length by setting member `.length`
- define clear password operation by setting member `.flags` to \ref FS_DEVICE_CLR_PASSWORD

When operation executes successfully, password is cleared from device registers.

**Lock the card:**

- assign password buffer to member `.password`
- define password length by setting member `.length`
- define lock operation by setting member `.flags` to \ref FS_DEVICE_LOCK_UNLOCK

When operation executes successfully, data access protection is activated. All read/write accesses are rejected.

> **Note**
> It is possible to set password and lock the card in a single step:
>
> - define set password and lock the card operation by setting member `.flags` to (\ref FS_DEVICE_SET_PASSWORD | \ref FS_DEVICE_LOCK_UNLOCK)

**Unlock the card:**

- assign password buffer to member `.password`
- define password length by setting member `.length`
- define unlock operation by setting member `.flags` to 0 (zero)

When operation executes successfully, data access protection is deactivated. All read/write accesses are allowed.

**Forcing erase:**

- members `.password` and `.length` are ignored
- define force erase operation by setting member `.flags` to \ref FS_DEVICE_ERASE

When operation executes successfully, all the card data content is erased along with the current password.

> **Note**
>
> - SD and MMCA specification also allows to replace the password in a single step. This is not supported by the FileSystem library,
>   since it is known that various devices have issues with the interpretation of this operation. Therefore, when changing the password,
>   old one must be first cleared and then new password can be set.
> - Is is also recommended to unlock the device before resetting the password.

To successfully \ref fmount "mount" locked memory card and gain read/write data access, password management information
must be provided from the user application:

- \ref fs_mc_lock_unlock function is used to return pointer to \ref fsLockUnlock structure containing password information

User application should only implement callback function \ref fs_mc_lock_unlock when support for memory card password management
is required. This function will be called only when mounting a memory card with active password protection.

#### Health Status (S.M.A.R.T)

Many industrial grade eMMC and SD memory devices support health status monitoring system known as S.M.A.R.T. (Self-Monitoring,
Analysis and Reporting Technology). Health Status monitor data is used to anticipate and detect imminent memory failures.

eMMC and SD memory devices use common command (CMD56) to access vendor specific information about memory health status. In order
to retrieve relevant information, function \ref fs_ioc_device_ctrl can be used together with control code \ref fsDevCtrlCodeHealthStatus
and structure \ref fsHealthStatus as control code argument.

Health status access command behavior is vendor specific but in general the following rules apply:

- Variable \ref fsHealthStatus.arg is used to control data direction and to provide vendor specific control info:
  - Bit [0] controls direction: read mode requires setting this bit to 1, write mode requires setting this bit to 0
  - Bits [1-31] are vendor specific and shall be set according to device datasheet
- Variables \ref fsHealthStatus.buf and \ref fsHealthStatus.buf_sz specify data buffer where health status information will be
  stored. Data buffer address shall be 4-byte aligned and buffer size shall be equal to or greater than 512 bytes.

When function \ref fs_ioc_device_ctrl is called, general command (CMD56) is executed and variable \ref fsHealthStatus.arg is used
as command argument. The transfered data gets copied into or from the buffer specified by \ref fsHealthStatus.buf.

\note Health Status is usually not available on non-industrial memory devices.

### Power Saving Features {#mci_power_save}

File System Component tends to reduce overall power consumption as much as possible, therefore the MC Control Layer
will automatically:

- **switch** the memory device between **Transfer** state and **Standby** state. This way the memory device spends
  **most of the time in Standby state** without noticeable effect on the data transfer performance.
- **stop bus clock** when there is **no communication** on the bus

eMMC devices (MMCA specification version 4.3 and higher) support CMD5 (**Sleep** state). To switch between Sleep state and Standby state,
use the function \ref fs_ioc_device_ctrl. While in the Sleep state, VCC power supply may be switched off to further
minimize the power consumption.

### SPI Mode Support {#mc_spi_mode_support}

SD memory cards (up to SD specification version 4.10) and MMC devices (up to MMCA specification version 4.2) also support
Serial Peripheral Interface (SPI) bus mode as alternative way of communication. When SPI mode is used, MC Control Layer
will use \urlout{SPI-API} to use the underlying hardware.

The advantage of using the memory card in SPI mode is in simplified hardware requirements, but at the cost of performance loss and
limited command set. Since SPI consists of only SCLK, MOSI, MISO and nSS communication lines, File System Component provides following
callback routines for interfacing Card Detect (CD) and Write Protect (WP) switches:

- \ref fs_mc_read_cd is used to read the state of the **Card Detect** pin
- \ref fs_mc_read_wp is used to read the state of the **Write Protect** pin

Callback functions \ref fs_mc_read_cd and \ref fs_mc_read_wp are optional. If they are not implemented by user application, File System
Component will assume that memory device in SPI mode is always present and without write protection.

Since software controlled Slave Select line is not always available an alternative way is provided in such case:

- \ref fs_mc_spi_control_ss is used to control the **Slave Select** signal

User application should only implement callback function \ref fs_mc_spi_control_ss when SPI driver does not provide software
controlled Slave Select line.

\note
Do not implement \ref fs_mc_spi_control_ss when using "SPI Master to Multi-Slave Driver Wrapper". In this case Slave Select line
is controlled by Multi-Slave driver function
[SPI_Control_SlaveSelect](https://arm-software.github.io/CMSIS-Driver/latest/driver_SPI.html).

#### SPI bus frequency setting {#mc_spi_bus_freq_setting}

Reliable communication at high frequencies is a common problem when using SPI bus, since maximum bus frequency can be limited
by PCB design. To overcome this problem, FileSystem component implements simple bus frequency adaptation algorithm in order to
determine the maximum bus frequency at which it is still possible to communicate with the memory card.

FileSystem component will always attempt to set the highest bus frequency supported by the memory card (usually 25MHz) but if
after that the memory card is not accessible anymore I will lower the bus frequency. The adaptation is performed only during
memory card initialization procedure.

\cond This_should_be_done_later
### mci_best_practices Best Practices
- Drive cache size can have huge impact on memory device read/write performance. Depending on the internal structure, optimal
  cache size varies between devices. Usually more is better, but not necessarily the best.
- eMMC devices can be put to \ref mci_power_save "Sleep" state, which reduces current consumption from a range of ~100mA to
  ~100uA.

### Performance Measurements {#mci_performance}
   - Currently we don't yet have appropriate performance test, but we need to provide this at some point.
       It should be similar to old performance table (\refrlarm{fs_technical_data}, but with updated measurements.
\endcond

## NAND Flash Translation Layer {#nand_flash_TL}

**Features**

- NAND types: SLC, MLC, TLC.
- Up to 65536 blocks.
- Up to 256 pages in block.
- \ref wear_leveling.
- \ref bad_block_management.
- \ref slc_ecc in software (Hamming) or on-chip (EZ NAND).
- Power fail safe.

**NAND Flash Overview**

NAND Flash memory is used in USB drives, secure digital (SD) cards, memory sticks, CompactFlash cards and multimedia cards
(MMC). It is a non-volatile memory with 60 percent smaller memory cells comparing to NOR memory cells thus offers high
data density at a lower cost. It is organized into a series of blocks, which are the smallest erasable entities in a NAND
Flash device. The smallest entity that can be programmed is a byte. Due to a limited number of times NAND Flash blocks can
be reliably programmed and erased (nominally, each NAND block will survive 100000 program/erase cycles) it is critical to
implement a **NAND Flash Translation Layer (NFTL)** to maximize the lifespan of the device. This layer comprises of wear
leveling algorithms, bad block management and error control algorithms and thus maximizes the lifespan of a device by
redirecting new writes to different physical blocks. It only applies to NAND Flash devices directly attached to the
microcontroller.

\note External storage devices such as memory cards or USB sticks carry an integrated NAND Flash controller that is
taking care of the NAND Flash in the device.

**NAND Flash Architecture**

A NAND Flash array is organized in blocks with multiple pages per block. Each page consist of user data area and spare data
area. The spare data area is physically the same as user data area and is typically used for error correction information
(ECC), wear leveling and other software system functions. The user data area is available to the user for data storage.
The size of the user data area depends on the page size and is typically of one or multiple sector size.

### Wear Leveling {#wear_leveling}

Due to its architecture, data can be written to an address on a NAND flash memory only a finite number of times. NAND flash
memory wears out if data is written too often to the same address, so wear leveling is used to help prolong the life of the
NAND flash device. It  ensures that data erasures and writes are distributed evenly across the NAND Flash storage medium,
so that NAND memory blocks don't fail prematurely due to a high number of erase cycles.

Dynamic wear leveling addresses the issue of repeated writes to the same logical blocks by redirecting new writes to
different physical blocks. Dynamic wear leveling works on data blocks that are being written to dynamically.

Static wear leveling addresses the issue of static data that is kept unchanged for a long time in content and location.
Some static data can stay in the same data block until the NAND flash memory is broken. To evenly distribute write and erase
cycles across NAND memory blocks, an algorithm is implemented to move the static data from one location to the other in the
Flash.

The File System Component implements wear leveling for all kinds of NAND Flash devices (SLC, MLC, TLC).

### Bad Block Management {#bad_block_management}

It is normal for a NAND flash memory to contain bad or invalid blocks of memory. Invalid blocks are blocks that contain one
or more invalid bits so that the reliability can no longer be guaranteed. Blocks can become invalid during erasing and
programming at run-time or during the manufacturing process. Bad blocks generated during manufacturing process are marked
bad by the manufacturer. Bad blocks detected at run-time are marked as bad by monitoring the status register in the NAND
flash chip during an erase and program operation. A block that is marked as bad is skipped and no longer used.

The File System Component implements bad block management for all kinds of NAND Flash devices (SLC, MLC, TLC):

- Skip factory marked bad blocks
- Mark block as bad as soon as an erase operation fails on the block
- Replace/mode data from blocks marked as bad to good blocks

### Error Correction Codes (ECC) {#slc_ecc}

Error detection and correction codes are used in flash memory to protect data from corruption. All types of error correction
codes add redundant information to the data being stored to the flash memory. The redundant information is calculated before
the data is written, added to the spare area of the page and then stored in flash memory. When the data is read, the
redundant information is recalculated and compared to those stored in the flash.

Error correction codes (ECC) used in the NAND flash memory are block codes. This means that the redundant data bits are
calculated for a fixed size block of used data. NTFL is implementing a
[Hamming](https://en.wikipedia.org/wiki/Hamming_code) ECC algorithm which is able
to correct 1-bit and detect 2-bit errors for a fixed size of one sector or 512 bytes for SLC NAND Flashes. The redundant
information is calculated in a way that a balance of correction power and efficiency is achieved.

\warning If you are using a NAND Flash device that requires higher than 1-bit correctability per 512 byte codeword, you
need to **disable the Software ECC** in the `FS_Config_NAND_n.h` file, otherwise the File System will not function correctly.
Hardware driver or on-chip ECC calculation must be provided in such case.

On-Chip ECC is supported on EZ NAND compliant devices. Various ECC layouts are supported by using flexible ECC codeword
configuration.

![ECC Configuration (Contiguous Layout)](fs_nand_ecc_config_0.png)

ECC algorithms typically operate on smaller sections of the NAND physical page size. This section can be described as
ECC virtual page, with its main and spare part. Typically main part consists of 512 bytes while spare part can be of different
sizes (16, 28, 32, etc. bytes).

Amount of protected data is defined by codeword size, which is typically 512 bytes in main area and of various sizes in spare.
Spare codeword may also not be aligned to start of spare area, but is located in spare with some offset. Spare codewords may
also follow each other in arbitrary order, therefore codeword gap defines the gap between each codeword.
It is similar with generated ECC data - its size, offset from start of spare and gap between each other may be arbitrary.

## Embedded File System (EFS) {#emb_fs}

The **Embedded File System (EFS)** is a proprietary file system used on NOR flash devices. Basic features are:

- \ref efs_mem_org of the flash device is optimized for maximum performance.
- \ref efs_alloc_info is reduced to a minimum, allowing small data overhead.
- \ref efs_file_content are stored in fragments of variable size which provide optimal file access times.

### Memory Organization {#efs_mem_org}

A NOR flash device memory array is physically divided into **sectors** or **blocks**. The File System Component designates them
as **blocks**. Typically, a block's size is 64 kB which is also the smallest erasable unit. Blocks can be further divided,
down to memory cells. The memory cell size depends on the device architecture and is 8- (byte), 16- (half word) or 32-bit
wide (word). The memory cell architecture also defines smallest programmable unit, which must be maximum 32-bit for use with
the Embedded File System.

Embedded File System organizes each block into three regions:

- \ref efs_alloc_info "Allocation Information", located on top of the block, grows in descending order and contains file
  allocation records.
- **Free Space**
- \ref efs_file_content "File Names & Content", located on bottom of the block, grows in ascending order and contains file
  names and data.

![Flash Memory Organization](fs_memoryorg.png)

### Allocation Information {#efs_alloc_info}

Allocation information region is located on top of a block and describes the block's content. It consists of block signature
and file allocation information records, which are written in descending order. Each file has at least one record associated
with it. Multiple records belong to files with content and to fragmented files. A file is fragmented when it is modified or
its content size exceeds a single block size and must be stored across several blocks. Several small files are stored into a
single block.

![Flash Block Allocation](fs_allocinfo.png)

#### Block Signature

Each block contains a signature, consisting of 4 bytes and determines if block is:

- **empty** i.e. erased
- **used** but more data can be written into it
- **used** temporarily during defragmentation
- **full** and cannot be written anymore (only erased)

#### Allocation Information Record

The file allocation information record consists of 8 bytes and has the following components:

- **end** is the end address of the file fragment.
- **fileID** is the file identification number and is associated with the file name.
- **index** is the file fragment ordering number, which starts at 0 for each file.

\code
struct falloc {
  uint32_t end;
  uint16_t fileID;
  uint16_t index;
};
\endcode

The file allocation information is written when:

- The file is opened for writing.
- The file is closed.
- The file is flushed and file fragment is not yet defined by the allocation information record.
- The block is full and there is no more free space.

### File Names & Content {#efs_file_content}

The file names & content region is located at the bottom of a block and is fully defined through the file allocation
information records. It consists of file names and file content, which can both be fragmented. The first file fragment
always starts at the beginning of a block (at offset 0) and is written in ascending order.

#### File Names

In the Embedded File System, a file name consists of maximum 31 characters. Directories are not supported, therefore any file
name which contains a directory separator character, such as slash (/) or backslash(\\), is rejected as invalid. Other
characters are allowed.

#### File Content

Since file fragments are of variable size, **create big file fragments** in order to reduce the total number of file
fragments and make the best use of a block. Writing or appending small amounts of data to a file is not optimal, since such
an approach creates a large number of allocation information records. They consume free space and the required processing
time results in a slow file access time.

When the file content is modified, the old file content is invalidated and a new file fragment is allocated. a block is
erased when all the data stored within has been invalidated.

### Limitations {#efs_limits}

The following restrictions are applicable to the EFS:

- Maximum file name length is limited to 31 characters.
- Minimum block size should be 512 bytes or more.
- Directories or folders are not supported.
- Multiple active file handles per file are allowed only for files opened in read mode.
- Seeking (\ref fseek) within files works only for files opened in read mode.
- File update modes (r+, w+, a+) (\ref fopen) are not supported.
- Timestamp information is not supported for a file.
- Drive partitions are not supported.
- The EFS is not compatible with the FAT file system and cannot be used with a USB mass storage device.
