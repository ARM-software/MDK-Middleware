USB Host Keyboard Example
===========

This MDK-Middleware example project implements a USB Host device that receives data from a USB Device (Keyboard) using Human Interface Device (HID) Class.

For detailed description see [USB Host Keyboard Example section in MDK-Middleware documenation](https://arm-software.github.io/MDK-Middleware/latest/USB/host_hid_tutorial.html).

Key usage aspects
-----

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections) to the target hardware intefaces. For details see example documentation referenced above.

After the USB HID keyboard is connected to the development board, the pressed characters are forwarded to the standard output.
