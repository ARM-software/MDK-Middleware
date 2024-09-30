# USB Device Virtual COM Port {#dev_cdc_tutorial}

This example implements a USB Device that exchanges data with a USB Host using \ref CDC as virtual communication port.

The USB bridge between a **Virtual COM Port** on the USB Host Computer and a UART port on the evaluation board.

The following picture shows an exemplary connection of the development board and the USB Host Computer.

![USB device virtual COM port example hardware setup](cdc_dev_example_setup.png)

## Project Organization {#prj_org_dev_cdc}

The USB Device Virtual COM port project is available as part of the \ref usbd_ref_example "USB Device Reference example".

<h2>Application Source Files</h2>

Following files implement application-specific logic in the example:

 - `VirtualCOM.c`: contains the main C function for initializing the board hardware and the USB Device Component. It also implements the data internal data exchange between the USB and the UART port.
 - `USBD_User_CDC_ACM_UART_0.c`: adapted code template that implements necessary file access functions. Refer to \ref usbd_cdcFunctions_acm for details.

<h2>Software Components Configuration Files</h2>

Configuration files for the software components used in the project are available in the `/RTE/` directory and can be modified by users to adjust the operation of related components. Section \ref usbd_rte_components gives an overview about the components and their dependencies.

Following configuration files are provided with this example:

 - For the USB component, in the `/RTE/USB/` folder:
   - `USBD_Config_CDC_0.h`: \ref usbd_cdcFunctions_acm_conf "USB Device CDC (ACM) configuration".
   - `USBD_Config_0.h`: \ref usbd_coreFunctions_conf "USB Device Core configuration".
   - `USB_Debug.h`: \ref usbDevEvrConfig "USB Device Debug configuration".
 - For the CMSIS components, in the `/RTE/CMSIS/` folder:
   - `RTX_Config.h` and `RTX_Config.h`: [CMSIS-RTX Configuration files](https://arm-software.github.io/CMSIS-RTX/latest/config_rtx5.html) for the RTOS Kernel.

When a board layer is added to the project, corresponding configuration files for the board and device components will become available in the local `/RTE/` directory as well.

<h2>Board Layer</h2>

In order to build the USB Device HID project it shall be extended with a compatible board layer that provides following interfaces as [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections):
 - `CMSIS_USB_Device`: CMSIS-Driver for USB Device interface
 - `CMSIS_VIO`: CMSIS-Driver for virtual I/O interface
 - `CMSIS_USART`: CMSIS-Driver for USART interface

## Build the Project {#prj_build_dev_cdc}

[Working with MDK-Middleware Examples](../General/working_with_examples.html) explains the workflow for accessing, configuring and building an MDK-Middleware example project for your target hardware.

 -# Open the **USB_Device** MDK-Middleware reference example.
 -# Make sure the compatible board layer is configured.
 -# Select **VirtualCOM** as an active project for the build process.
 -# Build the project and observe that no errors are reported.


## Run the Example {#prj_run_dev_cdc}

**Setup**

Board-specific setup such as jumpers, USB ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target. It also lists supported IOs such as LEDs and Buttons.

 -# Load the firmware image to your development board.
 -# Connect the UART on the development board to your PC (you might need an USB to serial RS232 adapter). Use an USB cable to connect your development board to the Host PC and power up your board.
 -# The **Welcome to the Found New Hardware Wizard** appears. Installation of the driver is described in detail in the `Abstract.txt` file.

**Example execution**

The USB Device Virtual COM example can be tested using a terminal emulation program. Please download an appropriate program for this purpose (such as **PuTTY** for example). Open the two COM ports "COMx" and "COMy". Any data from "COMx" will be echoed on "COMy" and visa versa:

![VCOM terminals](vcom_terminals.png)

**About Host PC driver for Microsoft Windows**

The example folder contains two files relevant for driver installation on the Microsoft Windows:

 - Driver setup information file (xxx-vcom.inf) which is used to create a digitally signed driver catalog file (xxx-vcom.cat)
 - Digitally signed driver catalog file (xxx-vcom.cat)

The driver files are provided as an example, the driver setup information file should be adapted and digitally signed driver catalog file
should be created from adapted driver setup information file.

Driver setup information file should be adapted in the following way:

 - **c251** in Vendor ID **VID_c251** entries should be changed to the vendor ID number assigned to your company by the USB organization
  (c251 Vendor ID is reserved for Keil Software and should not be used)
 - **xxxx** in Product ID **PID_xxxx** entries should be changed to the product ID as assigned by your company
 - in **[DeviceList.xxx]** sections, entries not relevant for the device, should be removed or added as described below:
   - if device is pure CDC class device (**not composite**) then all entries ending with **&MI_xx should be removed**
   - if device is a **composite device** with one or more CDC class instances then entries **not ending** with **&MI_xx should be removed** and entries **ending** with **&MI_xx should exist** for each CDC class instance (example driver contains entries ending with &MI_00 and &MI_02 which are used for composite device containing
     two CDC class instances and each instance uses 2 interfaces where MI_00 describes first CDC instance and
     MI_02 entry describes second CDC instance)
 - **[Strings]** section should be changed as desired

\note
 - Vendor ID and Product ID are configured in the `USBD_Config_0.h` configuration file of the embedded application.
 - For producing digitally signed driver catalog file please refer to Microsoft Windows documentation.