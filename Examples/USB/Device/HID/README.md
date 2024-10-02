# USB Device HID Example

This MDK-Middleware example project implements a USB Device that exchanges data with a USB Host using Human Interface Device (HID) class.

For details refer to [USB Device HID Example section in MDK-Middleware documentation](https://arm-software.github.io/MDK-Middleware/latest/USB/usbd_example_hid.html).


## Key usage aspects

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections) to the target hardware interfaces. For details see example documentation referenced above.

A graphical "HID Client" program is available for the USB Host Computer (Windows only) to interface with the LEDs and buttons on the target USB Device.
