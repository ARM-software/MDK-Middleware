# USB Host Keyboard Example

This MDK-Middleware example project implements a USB Host that receives data from a USB Device (Keyboard) using Human Interface Device (HID) class.

For detailed description see [USB Host Keyboard Example section in MDK-Middleware documentation](https://arm-software.github.io/MDK-Middleware/latest/USB/usbh_example_hid.html).

## Key usage aspects

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections) to the target hardware interfaces. For details see example documentation referenced above.

After the USB HID keyboard is connected to the development board, the pressed characters are forwarded to the standard output.
