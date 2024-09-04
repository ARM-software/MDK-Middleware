HID Example
===========

This example demonstrates usage of the Human Interface Device (HID) class running on an USB Device.

[Refer to the MDK-Middleware documentation in the section USB Device Examples for further details.](https://arm-software.github.io/MDK-Middleware/latest/USB/USB_Device.html#dev_hid_tutorial)

Usage
-----

For running on target hardware an compatible board layer is required. For details refer to [CMSIS-Toolbox > Reference Applications](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md).

Using this board layer, the HID interface connects typically to buttons and LEDs using the [CMSIS-Driver VIO interface](https://arm-software.github.io/CMSIS_6/latest/Driver/group__vio__interface__gr.html).

The Keil μVision MDK installation contains a HID Client demo program for Windows. This demo program starts with `<Keil-root>\ARM\Utilities\HID_Client\Release\HIDClient.exe`.
