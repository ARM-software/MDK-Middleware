# Create a File System Application {#create_app}

The steps to create a microcontroller application that uses the File System are:

-# Select \ref RTE_Component_Selection "RTE Components" along with other components that are required for your application.
-# Configure the \ref fs_configuration "File System".
-# Configure the \ref hw_configuration "hardware".
-# Implement the \ref User_Code_Implementation "User Code".
-# \ref fs_debugging "Debug" you application using the built-in mechanisms of the File System Component.
  
## RTE Component Selection {#RTE_Component_Selection}

Only a few steps are necessary to complete the RTE Component selection:

-# From the **File System** Component:
  - Select **File System:CORE** that provides the basic functionality required for data storage and access.
  - Select your desired **File System:Drive**. \ref fs_drives gives more details.
-# From the **CMSIS Driver** Component select an appropriate driver suitable for your **drive**. \ref fs_drives gives more details.
-# From the **CMSIS-Compiler** Component select **CORE** and **File Interface:MDK-MW File System** to provide interface for the Standard C Library functions.
-# From the **Device** Component:
  - Additional device specific drivers may be required according to the validation output.
-# From the CMSIS Component:
  - Select the **CMSIS:CORE** to provide the core interface to the processor.
  - Select a suitable **CMSIS:RTOS** that is a required for the File System Component.

![RTE Component for File System](fs_rte_comp_selection.png)

## Using Drives {#fs_drives}

The section \ref drive shows the relationship between drive - memory device - and driver/interface.

### NOR Flash Drive {#nor_usage}

For using a NOR Flash drive, a \urlout{Flash-API} driver is required, which encapsulates either the Memory Bus interface or SPI (in which case an \urlout{SPI-API} driver is required in addition):

- Set **File System:Drive:NOR** to at least '1' to use a NOR Flash drive (independently if it is using a memory bus or SPI interface).
- From the **CMSIS-Driver** Component, select an appropriate **Flash (API)** device. If your NOR Flash uses an SPI interface, select **SPI (API)** additionally.

If your NOR Flash device is not listed, use one of the examples as a reference to implement a driver for your specific device:

- AM29x800BB/[M29W640FB](https://www.farnell.com/datasheets/40281.pdf)
(Flash with 16-bit memory bus interface)
- [S29GL064Nx2](https://www.infineon.com/dgdl/Infineon-S29GL064N_S29GL032N_64_Mbit_32_Mbit_3_V_Page_Mode_MirrorBit_Flash-DataSheet-v03_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ed556fd548b)
(Flash with 32-bit memory bus interface)
- [AT45DB642D](https://datasheet.octopart.com/AT45DB642D-CNU-Atmel-datasheet-9652374.pdf)
(Flash with SPI interface using an SPI driver)

![Flash (API) Drivers shipped with the Middleware Component](fs_flash_drivers.png)

Configure the driver-hardware relationship using the `FS_Config_NOR_n.h` file:

![NOR Flash Drive Configuration File](fs_config_nor_0_h.png)

\note General \ref system_configuration requirements apply.

### RAM Disk Drive {#ram_usage}

To enable a RAM disk drive, select **File System:Drive:RAM**.

Set the drive characteristics in the `FS_Config_RAM.`h file:

![RAM Disk Configuration File](fs_config_ram_0_h.png)

\note General \ref system_configuration requirements apply.

### Memory Card Drive {#mc_usage}

Memory cards can be connected to a microcontroller device either using an SD/MMC bus mode (MCI) or an SPI bus mode:

- Set **File System:Drive:Memory Card** to at least '1'.
- From the **CMSIS-Driver** Component, select **MCI (API)** or **SPI (API)**, depending on the actual connection of
  the card drive to the microcontroller.

Set the drive characteristics in the `FS_Config_MC_n.h` file:

![Memory Card Drive Configuration File](fs_config_mc_0_h.png)

> **Note**
>
> - If your microcontroller device does not have a **CMSIS-Driver** for the **MCI (API)** or **SPI (API)**, you can try
>   to develop your own one based on the \urlout{CMSIS-Driver-Ref} documentation.
> - The page \ref mc_control_layer lists all supported memory card types and gives further details on how to use these cards.
> - General \ref system_configuration requirements apply.

### USB Flash Drive {#usb_usage}

- Set **File System:Drive:USB** to at least '1'.
- From the **USB** Component, please select the **USB:CORE**, set **USB:Host** to '1', and select **USB:Host:MSC**.
- From the **CMSIS-Driver** component, select the appropriate USB driver from **USB Host (API)**.

Set the drive characteristics in the `FS_Config_USB_n.h` file:

![USB Host Mass Storage Class Drive Configuration File](fs_config_usb_0_h.png)

> **Note**
>
> - If your microcontroller device does not have a **CMSIS-Driver** for the **USB Host (API)**, you can try to develop
>   your own one based on the
>   \urlout{CMSIS-Driver-Ref} documentation.
> - General \ref system_configuration requirements apply.

### NAND Flash Drive {#nand_usage}

- Set **File System:Drive:NAND** to at least '1'.
- From the **CMSIS-Driver** component, select the appropriate NAND driver from **NAND (API)**.

Set the drive characteristics in the `FS_Config_NAND_n.h` file:

![NAND Flash Drive Configuration File](fs_config_nand_0_h.png)

> **Note**
>
> - If your microcontroller device does not have a **CMSIS-Driver** for the **NAND (API)**, you can try to develop your
>   own one based on the \urlout{CMSIS-Driver-Ref} documentation.
> - The page \ref nand_flash_TL describes additional features specific to these devices.
> - Using a **NAND (API)** driver, you can use any NAND Flash device with an 8-/16-bit memory bus interface for data storage.
> - NAND Flashes with SPI interface (Serial NAND) are currently *not supported* by the File System Component.
> - General \ref system_configuration requirements apply.

## File System Configuration {#fs_configuration}

The File System configuration file `FS_Config.h` contains settings for the amount of files that can be open at the same time:

![File System Configuration File](fs_config_h.png)

Maximum number of simultaneously opened files can be set separately for FAT File System and for Embedded File System (EFS).

## Hardware Configuration {#hw_configuration}

As the file system is not bound to a special type of hardware, you need to configure the necessary drivers according to the
requirements of your target device. This configuration is done in the `RTE_Device.h` configuration file:

![Hardware Configuration Example using RTE_Device.h](rte_device_h.png)

\note Consult the device's user manual or hardware reference guide for more details.

## System Resource Configuration {#system_configuration}

For proper operation, the File System Component requires some system configuration settings. The requirements are:

- Minimum **heap size of 512 + 96 Bytes for each opened file**. If you want to be able to have three files open
  at the same time, you need to set a heap size of at least 3 * (512+96) Bytes = 1824 Bytes. This can be configured in the device's
  `startup_device.s` file (`Heap_Size`) or `linker script`.
- As the File System Component is not creating any additional threads, you need to add thread stack size to the calling
  thread. This calling thread can be the main thread, a thread with a default stack size or a thread with a user provided
  stack size. Changing the size for these threads can be done in the
  \urlout{RTX-Conf-CM5} file.
- Each opened file is protected with a mutex in order to ensure thread-safe operation. When working with file functions
  from the standard C library, you need to ensure enough mutex objects available for the file stream operations in the
  standard library system. File System component's \ref resource_requirements section explains how to determine the
  right setting in detail. Adjusting the number of mutex objects available for standard library system can be done in the \urlout{RTX-Conf-CM5} file.

For more information, check the File System component's \ref resource_requirements section.

## User Code Implementation{#User_Code_Implementation}

All available functions are documented in the [Reference](./modules.html) section of the File System
Component. These routines can be adapted to the needs of the microcontroller application, in case more functionality is
needed.

## Debugging {#fs_debugging}

File System Component can be easily configured to generate debug events and provide dynamic visibility to the component operation.

File System supports event annotations for the \urlout{Event-Recorder-About} and make it very easy to analyze the internal operation
of the File System Component during application debug. The \ref evr_gr describes the events implemented in the File System Component.

### Event Recorder {#fsEvr}

\urlout{Event-Recorder-About} is a powerful tool that provides visibility to the dynamic execution of the program.

The File System Component generates \ref evr_gr "a broad set of Debug Events" for the Event Recorder and implements required infrastructure to interface with it.

To use the Event Recorder it is required to create an image with event generation support. The necessary steps are:

  1. \urlout{Event-Recorder-Enable}: in the RTE management dialog enable the software component **CMSIS-View:Event Recorder**.
  2. Ensure that Event Recorder is initialized preferably by \urlout{RTX5-Event-Recorder-Config} if CMSIS-RTOS2 RTX v5 is used,
     or alternatively by calling the function \urlout{Event-Recorder-Initialize-Func} in the application code.
  3. \ref fsEvrConfig "Event Recorder Configuration": if necessary, adjust default Event Recorder configuration.
  4. In `FS_Debug.h` enable File System event generation and configure event filters.
  5. Build the application code, download it to the target hardware and start debug session.

Now, when the File System generates event information, it can be viewed in the \urlout{uv4-Event-Recorder}.

#### Configuration {#fsEvrConfig}

This section describes the configuration settings for the Event Recorder.

**File System Event Generation Configuration**

Selecting the **File System:CORE** will add the file `FS_Debug.h` to your project. Use this file to set the event
generation configuration for File System core and drivers separately.

![FS_Debug.h file for event generation configuration](fs_debug_h.png)

The following settings are available for event generation configuration of each module:

- **Off** means no events will be generated by the module
- **Errors** means only error events will be generated by the module
- **Errors + API** means error and API call events will be generated by the module
- **All** means all available events will be generated by the module. Besides error and API call events, this contains operation and detailed events.

#### Event IDs

The file system component uses the following event IDs:

| Component   | Event ID |
|-------------|----------|
| FsCore      | 0x80     |
| FsFAT       | 0x81     |
| FsEFS       | 0x82     |
| FsIOC       | 0x83     |
| FsNFTL      | 0x84     |
| FsNAND      | 0x85     |
| FsMcMCI     | 0x86     |
| FsMcSPI     | 0x87     |
