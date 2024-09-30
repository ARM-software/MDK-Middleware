# USB Device HID {#dev_hid_tutorial}

This example implements a USB Device that exchanges data with a USB Host using \ref HID.

A graphical "HID Client" program is available for the USB Host Computer (Windows only) to interface with the LEDs and buttons on the target USB Device. The following picture shows an exemplary connection of a development board and the USB Host Computer.

![USB device HID example hardware setup](hid_example_setup.png)

## Project Organization {#prj_org_dev_hid}

The USB Device HID project is available as part of the \ref usbd_ref_example "USB Device Reference example".

<h2>Application Source Files</h2>

Following files implement application-specific logic in the example:

 - `HID.c`: contains the main C function that initializes the board hardware and the USB Device Component. It also sends the current input status (typical buttons) via \ref USBD_HID_GetReportTrigger to the USB Host.
 - `USBD_User_HID_0.c`: adapted code template that implements necessary functions for I/O communication. Refer to \ref usbd_hidFunctions for details.

<h2>Software Components Configuration Files</h2>

Configuration files for the software components used in the project are available in the `/RTE/` directory and can be modified by users to adjust the operation of related components. Section \ref usbd_rte_components gives an overview about the components and their dependencies.

Following configuration files are provided with this example:

 - For the USB component, in the `/RTE/USB/` folder:
   - `USBD_Config_HID_0.h`: \ref usbd_hidFunctions_conf "USB Device HID Class configuration".
   - `USBD_Config_0.h`: \ref usbd_coreFunctions_conf "USB Device Core configuration".
   - `USB_Debug.h`: \ref usbDevEvrConfig "USB Device Debug configuration".
 - For the CMSIS components, in the `/RTE/CMSIS/` folder:
   - `RTX_Config.h` and `RTX_Config.h`: [CMSIS-RTX Configuration files](https://arm-software.github.io/CMSIS-RTX/latest/config_rtx5.html) for the RTOS Kernel.

When a board layer is added to the project, corresponding configuration files for the board and device components will become available in the local `/RTE/` directory as well.

<h2>Board Layer</h2>

In order to build the USB Device HID project it shall be extended with a compatible board layer that provides following interfaces as [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections):
 - `CMSIS_USB_Device`: CMSIS-Driver for USB Device interface
 - `CMSIS_VIO`: CMSIS-Driver for virtual I/O interface

## Build the Project {#prj_build_dev_hid}

[Working with MDK-Middleware Examples](../General/working_with_examples.html) explains the workflow for accessing, configuring and building an MDK-Middleware example project for your target hardware.

 -# Open the **USB_Device** MDK-Middleware reference example.
 -# Make sure the compatible board layer is configured.
 -# Select **HID** as an active project for the build process.
 -# Build the project and observe that no errors are reported.

## Run the Example {#prj_run_dev_hid}

**Setup**

Board-specific setup such as jumpers, USB ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target. It also lists supported IOs such as LEDs and Buttons.

 -# Load the firmware image to the target development board.
 -# Connect a USB Device port on the development board with the USB port on a PC.
 -# Wait for the automatic driver installation on the PC to complete.

**Example execution**

The USB Device HID example can be tested against a \ref hid_client_app "HID Client utility" that is available for Windows PCs only. The program runs stand-alone without installation.

Steps to check the USB communication using the client utility:

 -# Run `HIDClient.exe`.
 -# Select the **Keil USB Device** to establish the communication channel.
 -# Press buttons on the target hardware and observe the checkmarks appear in corresponding check boxes in the HID Client utility.
 -# Use the LEDs check boxes in the HID Client and observe LED status changes on the board.

![Testing the connection with the HID Client app](hid_client_test.png)

## HID Client {#hid_client_app}

The HID Client utility is graphical application for Windows PC that can be used for testing the \ref dev_hid_tutorial implementation. It is available as part of [Keil MDK uVision](https://developer.arm.com/documentation/101407/latest/About-uVision/Installation) and is located in `<install_dir>\ARM\Utilities\HID_Client\Release\` folder, where `<install_dir>` refers to the Keil MDK uVision installation directory. The program runs stand-alone without installation.

### HID Client Source Code {#client_app_cpp}

The source code of the HID Client application is available in `install_dir\ARM\Utilities\HID_Client`. Visual Studio 2005 and 2010
based projects are available (`HIDClient.vproj`).

**Header Files**

- `HID.h` includes the function declarations of the functions that are defined in `HID.cpp`.
- `HIDClient.h` is the main header file for the application. It includes other project specific headers (including Resource.h)
  and declares the `CHIDClientApp` application class.
- `HIDClientDlg.h` defines the behavior of the application's main dialog.

**Resource Files**

- `HIDClient.ico` is an icon file, which is used as the application's icon. This icon is included by the main resource file
  `HIDClient.rc`.
- `HIDClient.rc2` contains resources that are not edited by Microsoft Visual C++. You should place all resources not editable
  by the resource editor in this file.

**Source Files**

- `HID.cpp` contains the necessary functions that are used in this example application to communicate with an USB HID device.
  All available functions in Windows for HID interaction are explained here:
  <a href="https://learn.microsoft.com/en-us/windows-hardware/drivers/hid/introduction-to-hid-concepts" target=_blank>Introduction to HID Concepts</a>.
  The function:
  - `HID_Init` initializes the HID class to be used with the USB Host.
  - `HID_UnInit` de-initializes the HID class from the USB Host.
  - `HID_FindDevices` scans the USB Bus and lists all available HID devices for connection to the application. This
    information is obtained from the \ref USB_Device_Descriptor that is generated by the USB Component using the configuration files.
  - `HID_GetName` evaluates the \ref prod_string "Product String" of the device to be shown in the drop down box
    of the application.
  - `HID_GetInputReportSize` extracts the value of `USBD_HIDn_IN_REPORT_MAX_SZ` as specified in the
    \ref usbd_hidFunctions_conf "USBD_Config_HID_n.h" file.
  - `HID_GetOutputReportSize` extracts the value of `USBD_HIDn_OUT_REPORT_MAX_SZ` as specified in the
    \ref usbd_hidFunctions_conf "USBD_Config_HID_n.h" file.
  - `HID_GetFeatureReportSize` extracts the value of `USBD_HIDn_FEAT_REPORT_MAX_SZ` as specified in the
    \ref usbd_hidFunctions_conf "USBD_Config_HID_n.h" file.
  - `HID_Open` opens the device.
  - `HID_GetSelectedDevice` returns the selected device.
  - `HID_Close` closes the device.
  - `HID_Read` initiates a \ref USBD_HIDn_GetReport within the device to send data to the USB Host.
  - `HID_Write` triggers a \ref USBD_HIDn_SetReport within the device to read data sent from the USB Host.
  - `HID_GetFeature` and `HID_SetFeature` work on the USB HID Device's feature report (which is optional).
- `HIDClient.cpp` is the main application source file that contains the application class `CHIDClientApp`.
- `HIDClient.rc` is a listing of all of the Microsoft Windows resources that the program uses (located in the res subdirectory).
- `HIDClientDlg.cpp` implements the code of the client's dialog and calls the functions specified in `HID.cpp`. This is
  the actual place where the interaction between the USB Host and the USB Device is defined.
