# Overview {#mainpage}

The **USB Component** enables you to create **USB Device** and **USB Host** applications and is part of \url{MW-General}.
The USB Protocol is handled by the USB Component, so that developers can focus on their application needs.

![USB Component Structure](usbmiddleware.svg)

The **USB Component** is structured as follows:

- \ref USB_Device implements an USB Device peripheral that can be connected to an USB Host.
- \ref USB_Host is used to communicate to other USB Device peripherals over the USB bus.
- The following USB Classes are supported:
  - \ref HID "Human Interface Device (HID)"
  - \ref MSC "Mass Storage Class (MSC)"
  - \ref CDC "Communication Device Class (CDC)"
  - \ref ADC "Audio Device Class (ADC)" (USB Device only)
  - \ref CustomClass "Custom Class" (for implementing new or unsupported USB Classes)
  - **Composite USB Devices** supporting multiple device classes can be implemented.
- \urlout{USB-API} for USB Device and USB Host provide the interface to the microcontroller peripherals.

## Documentation Structure {#doc_structure}

This user's guide contains the following chapters:

- \ref rev_hist lists the changes made in each version of the USB Component.
- \ref USB_Device explains how to implement a **USB Device** with a microcontroller.
- \ref USB_Host explains how to implement a **USB Host** with a microcontroller.
- \ref USB_Concepts contains general information about the USB.
- \ref USB_Classes provides details about the supported USB Device Classes.
- \ref function_overview lists the complete API of the USB Component.
- \ref resource_requirements shows the configuration of system resources for USB Component in detail.
- \ref usb_migration contains information how to migrate projects to latest version of USB Component (v8).
- [**API Reference**](./modules.html) shows the directory and file organization and contains the detailed API description.

The [USB-IF Document Library](https://www.usb.org/documents) contains the complete
[USB 2.0 Specification](https://www.usb.org/document-library/usb-20-specification) and additional
information that is referenced in this user's guide.
