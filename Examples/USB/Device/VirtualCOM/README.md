# USB Device Virtual COM Port Example

This MDK-Middleware example project implements a USB Device that exchanges data with a USB Host using Communication Device Class as a virtual communication port.

For detailed description see [USB Device Virtual COM Example section in MDK-Middleware documentation](https://arm-software.github.io/MDK-Middleware/latest/USB/usbd_example_cdc.html).

## Key usage aspects

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/#connections) to the target hardware interfaces. For details see example documentation referenced above.

The device enumerates on the USB Host as virtual communication port and can be accessed via a Terminal application.
