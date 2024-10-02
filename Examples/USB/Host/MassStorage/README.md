# USB Host Mass Storage Example

This MDK-Middleware example project implemens a USB Host device that accesses a USB Mass Storage Device (USB memory stick) using Mass Storage Class.

For detailed description see [USB Host Mass Storage Example section in MDK-Middleware documentation](https://arm-software.github.io/MDK-Middleware/latest/USB/usbh_example_msc.html).


## Key usage aspects

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/ReferenceApplications.md#connections) to the target hardware interfaces. For details see example documentation referenced above.

This program creates (or overwrites) a `Test.txt` file on the connected USB Stick with the content:

```
USB Host Mass Storage!"
```
