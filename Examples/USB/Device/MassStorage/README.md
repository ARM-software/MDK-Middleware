# USB Device Mass Storage Example

This MDK-Middleware example project implements a mass storage USB Device using Mass Storage device Class.

For detailed description see [USB Device MSC Example section in MDK-Middleware documentation](https://arm-software.github.io/MDK-Middleware/latest/USB/usbd_example_msc.html).

## Key usage aspects

For successful build and operation the project needs to be extended with a board layer that implements required [connections](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/#connections) to the target hardware interfaces. For details see example documentation referenced above.

The implementation uses on-chip RAM as a storage device. The USB Host can access this memory drive using standard file access methods.
