# USB Host Keyboard {#usbh_example_hid}

This example implements a USB Host that receives data from a USB Device (Keyboard) using \ref HID.

The Standard I/O on the evaluation board (typically debugger Virtual COM) is used to show the keyboard inputs.

The following picture shows an exemplary connection of the development board, USB Keyboard and development computer.

![USB Host Keyboard example hardware setup](usbh_hid_setup.png)

## Project Organization

The USB Host Keyboard project is available as part of the \ref usbh_examples "USB Host Reference examples".

<h2>Application Source Files</h2>

Following files implement application-specific logic in the example:

 - `Keyboard.c`: contains the application main thread which initializes the USB Host Component. It also reads the input from the attached keyboard and outputs it via the Standard I/O.

<h2>Software Components Configuration Files</h2>

Configuration files for the software components used in the project are available in the `./RTE/` directory and can be modified by users to adjust the operation of related components. Section \ref usbh_rte_components gives an overview about the components and their dependencies.

Following configuration files are provided with this example:

 - For the USB component, in the `./RTE/USB/` folder:
   - `USBH_Config_HID.h`: \ref usbh_hidFunctions_conf "USB Host HID Class configuration".
   - `USBH_Config_0.h`: \ref usbh_coreFunctions_conf "USB Host Core configuration".
   - `USB_Debug.h`: \ref usbh_debugging "USB Host Debug configuration".
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
 -# Select **Keyboard** as an active project for the build process.
 -# Build the project and observe that no errors are reported.

## Run the Example

Board-specific hardware setup such as jumpers, USB ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target.

 -# Load the executable image to the target development board.
 -# Connect a USB keyboard to the USB Host port on the development board.
 -# When the program runs on the board you should be able to see something similar on the Standard I/O:
  ```
  USB Host HID Keyboard example
  Keyboard connected!
  <text entered via USB Keyboard>
  ```
