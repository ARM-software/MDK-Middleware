# USB Host Mass Storage {#usbh_example_msc}

This examples implements a USB Host that accesses a USB Mass Storage Device (USB memory stick) using \ref MSC.

This example will create or overwrite a `Test.txt` file on the connected USB Stick with the content `USB Host Mass Storage!`.

The following picture shows an exemplary connection of the development board and a USB memory stick.

![USB Host Mass Storage example hardware setup](usbh_msc_setup.png)

## Project Organization

The USB Host Mass Storage project is available as part of the \ref usbh_examples "USB Host Reference examples".

<h2>Application Source Files</h2>

Following files implement application-specific logic in the example:

 - `MassStorage.c`: contains the application main thread which initializes the flash storage device and the USB Host Component. Also, it contains that code that will write a file with a message onto the attached USB stick.
 - `USBH_MSC.c` and `USBH_MSC.h`: adapted code template for the application specific functionality of the USB Host MSC class. It implements the access to a USB storage device and allows file I/O via the File System component.

<h2>Software Components Configuration Files</h2>

Configuration files for the software components used in the project are available in the `./RTE/` directory and can be modified by users to adjust the operation of related components. Section \ref usbh_rte_components gives an overview about the components and their dependencies.

Following configuration files are provided with this example:

 - For the USB component, in the `./RTE/USB/` folder:
   - `USBD_Config_MSC_0.h`: \ref usbh_mscFunctions_conf "USB Host MSC configuration".
   - `USBH_Config_0.h`: \ref usbh_coreFunctions_conf "USB Host Core configuration".
   - `USB_Debug.h`: \ref usbh_debugging "USB Host Debug configuration".
 - For the File System component, in the `./RTE/File_System/` folder:
   - `FS_Config.h`, `FS_Config_MC_0.h`, `FS_Debug.h`: [File System](../FileSystem/index.html) configuration files
 - For the CMSIS components, in the `./RTE/CMSIS/` folder:
   - `RTX_Config.h` and `RTX_Config.c`: [CMSIS-RTX Configuration files](https://arm-software.github.io/CMSIS-RTX/latest/config_rtx5.html) for the RTOS Kernel.

When a board layer is added to the project, corresponding configuration files for the board and device components will become available in the local `./Board/` directory.

<h2>Board Layer</h2>

In order to build the project it shall be extended with a compatible board layer that provides following interfaces as [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections):
 - `CMSIS_USB_Host`: CMSIS-Driver for USB Host interface.

## Build the Project

[Working with MDK-Middleware Examples](../General/working_with_examples.html) explains the workflow for accessing, configuring and building an MDK-Middleware example project for your target hardware.

 -# Open the **USB_Host** MDK-Middleware reference example.
 -# Make sure the compatible board layer is configured.
 -# Select **MassStorage** as an active project for the build process.
 -# Build the project and observe that no errors are reported.

## Run the Example

Board-specific hardware setup such as jumpers, USB ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target.

 -# Load the executable image to the target development board.
 -# Insert a USB memory stick to the USB Host port on the development board.
 -# Press reset button to ensure the firmware is executed.
 -# Detach the USB memory stick and connected it to a PC to check the availability of the `Test.txt` file.
