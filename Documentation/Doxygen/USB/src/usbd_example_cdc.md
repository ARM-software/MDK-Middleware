# USB Device Virtual COM Port {#usbd_example_cdc}

This example implements a USB Device that exchanges data with a USB Host using \ref CDC as virtual communication port.

The USB bridge between a **Virtual COM Port** on the USB Host Computer and a UART port on the evaluation board.

The following picture shows an exemplary connection of the development board and the USB Host Computer.

![USB Device Virtual COM port example hardware setup](cdc_dev_example_setup.png)

## Project Organization

The USB Device Virtual COM Port project is available as part of the \ref usbd_examples "USB Device Reference examples".

<h2>Application Source Files</h2>

Following files implement application-specific logic in the example:

 - `VirtualCOM.c`: contains the application main thread which initializes the USB Device Component.
 - `USBD_User_CDC_ACM_UART_0.c`: adapted code template that implements USB to UART bridge functionality. Refer to \ref usbd_cdcFunctions_acm for details.

<h2>Software Components Configuration Files</h2>

Configuration files for the software components used in the project are available in the `./RTE/` directory and can be modified by users to adjust the operation of related components. Section \ref usbd_rte_components gives an overview about the components and their dependencies.

Following configuration files are provided with this example:

 - For the USB component, in the `./RTE/USB/` folder:
   - `USBD_Config_CDC_0.h`: \ref usbd_cdcFunctions_acm_conf "USB Device CDC (ACM) configuration".
   - `USBD_Config_0.h`: \ref usbd_coreFunctions_conf "USB Device Core configuration".
   - `USB_Debug.h`: \ref usbd_debugging "USB Device Debug configuration".
 - For the CMSIS components, in the `./RTE/CMSIS/` folder:
   - `RTX_Config.h` and `RTX_Config.c`: [CMSIS-RTX Configuration files](https://arm-software.github.io/CMSIS-RTX/latest/config_rtx5.html) for the RTOS Kernel.

When a board layer is added to the project, corresponding configuration files for the board and device components will become available in the local `./Board/` directory.

<h2>Board Layer</h2>

In order to build the project it shall be extended with a compatible board layer that provides following interfaces as [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections):
 - `CMSIS_USB_Device`: CMSIS-Driver for USB Device interface.
 - `CMSIS_VIO`: CMSIS-Driver for Virtual I/O interface.
 - `CMSIS_USART`: CMSIS-Driver for USART interface.

## Build the Project

[Working with MDK-Middleware Examples](../General/working_with_examples.html) explains the workflow for accessing, configuring and building an MDK-Middleware example project for your target hardware.

 -# Open the **USB_Device** MDK-Middleware reference example.
 -# Make sure the compatible board layer is configured.
 -# Select **VirtualCOM** as an active project for the build process.
 -# Build the project and observe that no errors are reported.

## Run the Example

**Setup**

Board-specific hardware setup such as jumpers, USB ports, power supply, etc. is documented in the board layer description (`README.md`) of your selected target.

 -# Load the executable image to the target development board.
 -# Connect the UART on the development board to your PC (you might need an USB to serial RS232 adapter). Use an USB cable to connect your development board to the Host PC and power up your board.
 -# The device should appear as Port - **USB Serial Device**.

**Example execution**

The USB Device Virtual COM example can be tested using a terminal emulation program. Please download an appropriate program for this purpose (such as **PuTTY** for example). Open the two COM ports "COMx" and "COMy". Any data from "COMx" will be echoed on "COMy" and visa versa:

![VCOM terminals](vcom_terminals.png)
