# USB Host {#USB_Host}

This chapter describes the software structure of the USB Host Component and explains its use for creating an USB Host application.

The USB Host Component simplifies software development of microcontroller systems that can communicate with USB Devices.

**Attributes of the USB Host Component:**

- Complies with the USB 2.0 specification.
- Support for \ref HID "HID", \ref MSC "MSC", \ref CDC "CDC", and \ref CustomClass "Custom" USB Device Classes to be connected to the USB Host.
- Support for \ref USB_Control_Transfers "control", \ref USB_Interrupt_Transfers "interrupt" and \ref USB_Bulk_Transfers "bulk" transfer types.
- Explains how to \subpage usbh_create_app "Create a new application" from scratch.
- Provides \subpage usbh_examples implementing different USB Host use cases.

## RTE Components {#usbh_rte_components}

The following picture shows the relationships of the RTE Components with the microcontroller's USB Host peripheral (USB
Controller). RTE Components provide configuration files and user code templates. 
 - **Configuration files** configure the RTE Components, hardware interfaces, memory resources and USB Host parameters.
 - **User code templates** provide the skeleton for implementing support for different USB Device classes.

![USB Host Structure](usb_host_blocks_config_files.png)
