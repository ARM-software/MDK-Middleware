# USB Device HID {#usbd_example_hid}

This example implements a USB Device that exchanges data with a USB Host using \ref HID.

A graphical **HID Client** program is available for the USB Host Computer (Windows only) to interface with the LEDs and buttons on the target USB Device.

The following picture shows an exemplary connection of the development board and the USB Host Computer.

![USB Device HID example hardware setup](hid_example_setup.png)

## Project Organization

The USB Device HID project is available as part of the \ref usbd_examples "USB Device Reference examples".

<h2>Application Source Files</h2>

Following files implement application-specific logic in the example:

 - `HID.c`: contains the application main thread which initializes the USB Device Component. It also sends the current input status (typically buttons state) via \ref USBD_HID_GetReportTrigger to the USB Host.
 - `USBD_User_HID_0.c`: adapted code template that implements necessary functions for I/O communication. Refer to \ref usbd_hidFunctions for details.

<h2>Software Components Configuration Files</h2>

Configuration files for the software components used in the project are available in the `./RTE/` directory and can be modified by users to adjust the operation of related components. Section \ref usbd_rte_components gives an overview about the components and their dependencies.

Following configuration files are provided with this example:

 - For the USB component, in the `./RTE/USB/` folder:
   - `USBD_Config_HID_0.h`: \ref usbd_hidFunctions_conf "USB Device HID Class configuration".
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
 -# Select **HID** as an active project for the build process.
 -# Build the project and observe that no errors are reported.

## Run the Example

**Setup**

Board-specific hardware setup such as jumpers, USB ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target.

 -# Load the executable image to the target development board.
 -# Connect a USB Device port on the development board with the USB port on a PC.
 -# The device should appear as Human Interface Device - **USB Input Device**.

**Example execution**

The USB Device HID example can be tested against a \ref hid_client_app "HID Client utility" that is available for Windows PCs only. The program runs stand-alone without installation.

Steps to check the USB communication using the client utility:

 -# Start the `HIDClient.exe` utility.
 -# Select the **Keil USB Device 0** to establish the communication channel.
 -# Press buttons on the target hardware and observe the checkmarks appear in corresponding check boxes in the HID Client utility.
 -# Use the LEDs check boxes in the HID Client and observe LED status changes on the board.

![Testing the connection with the HID Client app](hid_client_test.png)
