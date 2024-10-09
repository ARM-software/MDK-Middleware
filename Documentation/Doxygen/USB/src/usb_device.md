# USB Device {#USB_Device}

This chapter describes the software structure of the USB Device Component and its use for creating applications.

The USB Device Component simplifies the software development of microcontroller systems that interface to an USB Host.

**Attributes of the USB Device Component:**

- Supports \ref USB_Transfer_Rates "Low-Speed", \ref USB_Transfer_Rates "Full-Speed" and \ref USB_Transfer_Rates "High-Speed".
- Supports standard \ref USB_Classes "USB Classes" with multiple device class instances.
- Supports composite devices. Combine USB Device Classes to create a composite device.
- Supports multiple USB Devices on a single microcontroller with more than one USB Device controller.
- Provides \urlout{uv4_ca_sourcefiles} for implementing the USB Device functionality.
- Provides an user-friendly configuration file for each device class to generate \ref USB_Descriptors.
- Flexibly assigns \ref USB_Endpoints "USB Endpoints" to the microcontroller USB Device peripheral.
- Explains how to \subpage usbd_create_app "Create a new application" from scratch.
- Provides \subpage usbd_examples implementing different USB Device use cases.
- Provides \subpage usbd_pc_utilities for testing USB Device functionality.

## RTE Components {#usbd_rte_components}

The picture shows the relationship between the software components and the microcontroller's USB Device peripheral (USB Controller).

RTE Components provide configuration files and user code templates.
 - **Configuration files** configure the RTE Components, hardware interfaces, memory resources and USB Device driver parameters. They can have an impact on multiple RTE Components.
 - **User code templates** provide the skeleton for implementing the USB Device functionality.

The gray area around the RTE Components USB Device 1 and Driver_USBD1, as well as USB Controller 1 means that these components are optional and can only be used if a microcontroller device has multiple USB controllers present. If this is the case, an USB Device Class can be connected to any of the USB Device Instances.

![USB Device Structure](usb_device_blocks_config_files.png)

USB Device peripherals can have one or more of the following USB Device Classes:

- \ref ADC "Audio Device Class (ADC)" is used to exchange streaming audio data between the USB Host and the USB Device.
- \ref CDC "Communication Device Class (CDC)" provides virtual communication port functionality to the USB Host.
- \ref HID "Human Interface Device (HID)" is typically used to implement a keyboard, joystick, or mouse. The
  HID Class can be also used for low bandwidth data exchange.
- \ref MSC "Mass Storage Class (MSC)" is used to connect various storage devices to an USB Host. Mass Storage Class media can be an SD card, internal or external Flash memory, or RAM.
- \ref CustomClass "Custom Class" is used to implement either a standard or a vendor specific USB Device Class.

Generic information about USB Device Classes can be found on the USB-IF's [Approved Class Specification Documents](https://www.usb.org/documents?search=&category%5B%5D=49&type%5B%5D=55&items_per_page=50)
page.

Multiple RTE Component instances can interface with more than one USB Controller or can implement multiple
USB Device Classes. RTE Component instances are numbered. The number is appended to the RTE Component name, related configuration files, and user code templates. Each RTE Component has a separate configuration file. For example, for HID 0 and HID 1
the configuration files have the name `USB_Config_HID_0.h` and `USB_Config_HID_1.h`.

\note The default configuration settings are pre-configured for one instance of an USB Device or USB Device Class in a non-composite device peripheral. For other combinations the settings *need* to be edited to ensure proper operation.
