# USB Device Mass Storage {#usbd_example_msc}

This example implements a mass storage USB Device using \ref MSC.

The implementation uses on-chip RAM as a storage media. The USB Host can access this memory drive using standard file access methods.

The following picture shows an exemplary connection of the development board and the USB Host Computer.

![USB Device Mass Storage example hardware setup](msc_dev_example_setup.png)

## Project Organization

The USB Device Mass Storage project is available as part of the \ref usbd_examples "USB Device Reference examples".

<h2>Application Source Files</h2>

Following files implement application-specific logic in the example:

 - `MassStorage.c`: contains the application main thread which initializes the USB Device Component.
 - `MemoryDiskImage.c`: a FAT12 disk image with small README.TXT file that gets copied to the RAM and is used as storage media.
 - `USBD_User_MSC_0.c`: adapted code template that implements necessary file access functions. Refer to \ref usbd_mscFunctions for details about these template functions.

<h2>Software Components Configuration Files</h2>

Configuration files for the software components used in the project are available in the `./RTE/` directory and can be modified by users to adjust the operation of related components. Section \ref usbd_rte_components gives an overview about the components and their dependencies.

Following configuration files are provided with this example:

 - For the USB component, in the `./RTE/USB/` folder:
   - `USBD_Config_MSC_0.h`: \ref usbd_mscFunctions_conf "USB Device MSC configuration".
   - `USBD_Config_0.h`: \ref usbd_coreFunctions_conf "USB Device Core configuration".
   - `USB_Debug.h`: \ref usbd_debugging "USB Device Debug configuration".
 - For the CMSIS components, in the `./RTE/CMSIS/` folder:
   - `RTX_Config.h` and `RTX_Config.c`: [CMSIS-RTX Configuration files](https://arm-software.github.io/CMSIS-RTX/latest/config_rtx5.html) for the RTOS Kernel.

When a board layer is added to the project, corresponding configuration files for the board and device components will become available in the local `./Board/` directory.

<h2>Board Layer</h2>

In order to build the project it shall be extended with a compatible board layer that provides following interfaces as [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections):
 - `CMSIS_USB_Device`: CMSIS-Driver for USB Device interface.
 - `CMSIS_VIO`: CMSIS-Driver for Virtual I/O interface.

## Build the Project

[Working with MDK-Middleware Examples](../General/working_with_examples.html) explains the workflow for accessing, configuring and building an MDK-Middleware example project for your target hardware.

 -# Open the **USB_Device** MDK-Middleware reference example.
 -# Make sure the compatible board layer is configured.
 -# Select **MassStorage** as an active project for the build process.
 -# Build the project and observe that no errors are reported.

## Run the Example

**Setup**

Board-specific hardware setup such as jumpers, USB ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target.

 -# Load the executable image to the target development board.
 -# Connect a USB Device port on the development board with the USB port on a PC.
 -# Wait for USB disk **USB_Memory** to appear.

**Example execution**

After a successful disk mount on the host PC you can observe **USB_Memory** drive to become accessible from the PC operating system. For example on a Windows PC you should see it appear in the File Explorer.

On this drive you can find a single file `README.TXT` with the following content:

```
This is a USB Memory Device Demonstration.
https://arm-software.github.io/MDK-Middleware/latest/USB/usbd_example_msc.html
```
